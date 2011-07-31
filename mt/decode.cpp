#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <net/ethernet.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "utils.h"
#include "pkt.h"
#include "decode.h"

int decode(void *buf, size_t len, int fd)
{
    size_t leftlen = 0, cnt = 0, bodylen = 0;
    uint8_t *u8p = NULL;

    //Read Packet
    bodylen = len;
    cnt++;

    /// 不是IP的话直接跳过
    if (get_ether_type(buf) != ETHER_TYPE_IP) {
        //printf("It is Not IP\n");
        return -1;
    } 

    /// 跳过Ethernet
    u8p = (uint8_t*)buf + sizeof(struct ether_header);
    leftlen += sizeof(struct ether_header);
	if (leftlen > bodylen) return -1;

    if (get_ip_protocol(u8p) != IP_PROT_UDP) {
        // 不是UDP直接跳过
        //printf("It's not UDP\n");
        return -1;
    } 

    /// 跳过IP和UDP
    u8p = u8p + sizeof(struct udphdr) + sizeof(struct iphdr);
    leftlen += sizeof(struct udphdr) + sizeof(struct iphdr);
	if (leftlen > bodylen) return -1;

    /// 找到NS_UNIDATA数据包
    if (get_ns_type(u8p, leftlen) != NS_UNIDATA) {
        // 不是NS_UNIDATA数据包直接跳过
        //printf("Not NS-UNIDATA: %x\n", get_ns_type(u8p, leftlen));
        return -1;
    } 

    /// 跳过NS
    u8p = u8p + sizeof(struct ns);
    leftlen += sizeof(struct ns);
	if (leftlen > bodylen) return -1;

    uint8_t sdu_type = get_sdu_type(u8p, leftlen);
    if (sdu_type != DL_UNIDATA && sdu_type != UL_UNIDATA) {
        // 不是DL/UL格式，直接跳过
        //printf("Not DL/UL: %x\n", sdu_type);
        return -1;
    }

    /// 不需要跳过SDU，直接处理ul/dl unidata
    if (sdu_type == DL_UNIDATA) {
        //struct dl_unidata *dlp = (struct dl_unidata *)u8p;

        // 跳过dl_unidata固定头
        u8p += sizeof(struct dl_unidata);
        leftlen += sizeof(struct dl_unidata);
		if (leftlen > bodylen) return -1;

        // 循环处理可变头
        uint8_t tmptype = 0;
        uint8_t tmplen = 0;
        while (*u8p != LLC_PDU) {
            tmptype = *u8p++; // 在这里将需要的字段提取出来
            tmplen = *u8p & 0x0f;
            // 跳过tmplen长度
            u8p = u8p + tmplen + 1;
            leftlen += tmplen + 1;
			if (leftlen > bodylen) return -1;
        }
    } else {
        //struct ul_unidata * ulp = (struct ul_unidata *)u8p;

        /// 跳过固定UL_UNIDATA
        u8p = u8p + sizeof(struct ul_unidata);
        leftlen += sizeof(struct ul_unidata);
		if (leftlen > bodylen) return -1;

        uint8_t tmptype = 0;
        uint8_t tmplen = 0;
        while (*u8p != LLC_PDU) {
            tmptype = *u8p++;
            tmplen = *u8p & 0x0f;
            // 跳过tmplen长度
            u8p = u8p + tmplen + 1;
            leftlen = leftlen + tmplen + 1;
			if (leftlen > bodylen) return -1;
        }
    }

    /// 读取LLC头
    //struct llc *pllc = (struct llc*)u8p;

    /// 跳过LLC头
    u8p = u8p + sizeof(struct llc);
    leftlen = leftlen + sizeof(struct llc);
	if (leftlen > bodylen) return -1;

    if (get_llc_sapi(u8p) != LLGMM) {
        //printf("Not LLGMM\n");
        return -1;
    }
    /// 跳过LLC SAPI 和 Unconfirmed UI
    u8p += 3;
    leftlen += 3;
	if (leftlen > bodylen) return -1;

    struct gmm *gmmp = (struct gmm*)u8p;
    log_msg(LOG_LEVEL_INFO, "-------[%d] len:%u", cnt, bodylen);
    switch (gmmp->type) {
        case DETACH_ACCEPT:
            log_msg(LOG_LEVEL_INFO, "It's DETACH ACCEPT");
            break;
        case ACTIVE_PDP_REQUEST:
            log_msg(LOG_LEVEL_INFO, "It's PDP context request");
            break;
            /// TODO Add more type And type handler
        default:
            log_msg(LOG_LEVEL_INFO, "type is: 0x%x", gmmp->type);
			write(fd, buf, bodylen);
            break;
    }

	return 0;
}	

size_t print_packet_hdr(uint8_t *pbuf)
{
	packet_hdr_t *p = (packet_hdr_t*)pbuf;
	printf("ts_sec: %u\n", p->ts_sec);
	printf("ts_usec: %u\n", p->ts_usec);
	printf("incl_len: %u\n", p->incl_len);
	printf("orig_len: %u\n", p->orig_len);

	return p->orig_len;
}

int init_static_worker(struct static_worker *sworker)
{
	int fd[2];
	pid_t pid;

	if (socketpair(AF_UNIX, SOCK_STREAM, 0,  fd) < 0) {
		log_msg(LOG_LEVEL_ERR, strerror(errno));
	}
	log_msg(LOG_LEVEL_INFO, "init_worker: worker init, fd[0]=%d, fd[1]=%d.", fd[0], fd[1]);

	pid = fork();
	if (pid == 0) {
		// 开启工作进程
		do_work(fd[1]);
		exit(0);
	} else if (pid < 0) {
		log_msg(LOG_LEVEL_ERR, strerror(errno));
	} else {
		sworker->fd = fd[0];
		sworker->pid = (int)pid;
	}
	return 0;
}

/// 使用hashmap提高效率
/// 编译的时候使用-std=c++0x参数
#include <unordered_map>
#include <string>

using namespace std;

int do_work(int fd)
{
	char buf[65536];
	int ret = 0;
	// 记录用户流量信息的hashmap
	unordered_map<string , uint64_t> stat_map;

	log_msg(LOG_LEVEL_INFO, "static worker startup");

	while (1) {
		if ((ret = read(fd, buf, sizeof(buf))) > 0) {
			log_msg(LOG_LEVEL_INFO, "read msg len: %d", ret);
			/// TODO 提取出IP PORT TLLI，拼成KEY
		} 
	}

	return 0;
}

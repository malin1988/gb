#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <net/ethernet.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include "utils.h"
#include "pkt.h"

int decode(void *buf, size_t len)
{
    size_t ret = 0, leftlen = 0, cnt = 0, bodylen = 0;
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
        struct dl_unidata *dlp = (struct dl_unidata *)u8p;
        //printf("dl type: %x\n", dlp->type);
        //printf("dl tlli: %x %x %x %x\n", dlp->ctlli[0], dlp->ctlli[1], dlp->ctlli[2], dlp->ctlli[3]);
        // printf("dl liftime[0]: %x\n", dlp->lifetime[0]);
        //printf("dl mrac[0]: %x\n", dlp->mrac[0]);
        //printf("dl drx[0]: %x\n", dlp->drx[0]);
        //printf("dl ismi[0]: %x\n", dlp->imsi[0]);
        //printf("sizeof dl: %u\n", sizeof(struct dl_unidata));

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
        struct ul_unidata * ulp = (struct ul_unidata *)u8p;
        //printf("ul type: %x\n", ulp->type);
        //printf("ul tlli: %x %x %x %x\n", ulp->tlli[0], ulp->tlli[1], ulp->tlli[2], ulp->tlli[3]);

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
    struct llc *pllc = (struct llc*)u8p;
    //printf("size of llc: %u\n", sizeof(struct llc));
    //printf("llc id: %x\n", pllc->id);
    //printf("llc len: %d\n", pllc->len);

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

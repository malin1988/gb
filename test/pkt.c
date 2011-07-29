#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include "pkt.h"

FILE *open_file(const char *name)
{
	FILE *fp;
	fp = fopen(name, "r");
	if (!fp) {
		perror("fopen");
		exit(-1);
	}
    return fp;
}

size_t read_pcap_hdr(FILE *fp, void *buf)
{
	size_t ret;
	ret = fread(buf, sizeof(pcap_hdr_t), 1, fp);
	if (ret != 1) {
		fprintf(stderr, "len error\n");
		exit(-1);
	}
    return ret;
}

void print_pcap_hdr(void *buf)
{
	pcap_hdr_t *p = (pcap_hdr_t*)buf;
	printf("magic_number: %x\n", p->magic_number);
	printf("version_major: %d\n", p->version_major);
	printf("version_minor: %d\n", p->version_minor);
	printf("thiszone: %d\n", p->thiszone);
	printf("sigfigs: %d\n", p->sigfigs);
	printf("snaplen: %d\n", p->snaplen);
	printf("network: %d\n", p->network);
}

/// 返回其后数据包的长度
size_t read_pkt_hdr(FILE *fp, void *buf)
{
	size_t ret;
	ret = fread(buf, sizeof(packet_hdr_t), 1, fp);
	if (ret < 0) {
		fprintf(stderr, "len error\n");
		exit(-1);
	} else if (ret == 0) {
        return 0;
    }

    packet_hdr_t *p = (packet_hdr_t *)buf;
    return p->orig_len;
}

size_t read_pkt_body(FILE *fp, void *buf, size_t len)
{
	size_t ret;
	ret = fread(buf, sizeof(uint8_t), len, fp);
	if (ret != len) {
		fprintf(stderr, "len error\n");
		exit(-1);
	}
    return ret;
}

uint16_t get_ether_type(void *buf)
{
    struct ether_header *p = (struct ether_header *)buf;

    return ntohs(p->ether_type);
}

uint8_t get_ip_protocol(void *ipbuf)
{
    struct iphdr *p = (struct iphdr*)ipbuf;

    return p->protocol;
}

uint8_t get_ns_type(void *nsbuf, size_t len)
{
    if (len <= 0)
         return 0xFF;
    return *(uint8_t *)nsbuf;
}


uint8_t get_sdu_type(void *sdubuf, size_t len)
{
    if (len <= 0)
        return 0xFF;
    return *(uint8_t *)sdubuf;
}

/// 处理DL/UL_UNIDATA

/// 处理LLC PDU
uint8_t get_llc_sapi(void *llcbuf)
{
    return *(uint8_t*)llcbuf & 0x0f;
}

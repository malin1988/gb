#ifndef __PKT_H__
#define __PKT_H__ 1

typedef struct pcap_hdr {
	uint32_t magic_number;
	uint16_t version_major;
	uint16_t version_minor;
	int32_t thiszone;
	uint32_t sigfigs;
	uint32_t snaplen;
	uint32_t network;
} pcap_hdr_t;

typedef struct packet_hdr {
	uint32_t ts_sec;
	uint32_t ts_usec;
	uint32_t incl_len;
	uint32_t orig_len;
} packet_hdr_t;

FILE *open_file(const char *name);
size_t read_pcap_hdr(FILE *fp, void *buf);
void print_pcap_hdr(void *buf);
/// 返回其后数据包的长度
size_t read_pkt_hdr(FILE *fp, void *buf);
size_t read_pkt_body(FILE *fp, void *buf, size_t len);

///
#define ETHER_TYPE_IP 0x0800
uint16_t get_ether_type(void *buf);

#define IP_PROT_UDP 17
uint8_t get_ip_protocol(void *ipbuf);

#define NS_UNIDATA 0x00
struct ns {
    uint8_t type;
    uint8_t spare;
    uint16_t bvci;
};
uint8_t get_ns_type(void *nsbuf, size_t len);

#define DL_UNIDATA 0x00
#define UL_UNIDATA 0x01

struct sdu {
    uint8_t type;
};
uint8_t get_sdu_type(void *sdubuf, size_t len);

/// DL/UL UNIDATA
struct dl_unidata {
    uint8_t type;
};

struct ul_unidata {
    uint8_t type;
    uint8_t tlli[4];
    uint8_t qos[3];
    uint8_t cid[10];
    uint8_t alo[2]; // alignment octets
};

struct llc {
    uint8_t id;
    uint8_t zero;
    uint8_t len;
};

/// 获取llc地址的SAPI
#define LLGMM 0x01
uint8_t get_llc_sapi(void *llcbuf);

#define ACTIVE_PDP_CONTEXT_REQ 0x41
struct gmm {
    uint8_t pd;
    uint8_t type;
};
#endif

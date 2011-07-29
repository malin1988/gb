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

#define LLC_PDU 0x0e // LLC PDU开始标志
/// DL/UL UNIDATA
/// UL_UNIDATA固定头
/// 可变头动态处理
struct ul_unidata {
    uint8_t type;
    uint8_t tlli[4];
    uint8_t qos[3];
    uint8_t cid[10];
};

/// DL_UNIDATA固定头
/// 可变头动态处理
struct dl_unidata {
    uint8_t type;
    uint8_t ctlli[4]; // current tlli
    uint8_t qos[3];
    uint8_t lifetime[4];
};

struct llc {
    uint8_t id;
    uint8_t zero;
    uint8_t len;
};

/// 获取llc地址的SAPI
#define LLGMM 0x01
uint8_t get_llc_sapi(void *llcbuf);

#define ATTACH_REQUEST         0x01
#define ATTACH_ACCEPT          0x02
#define ATTACH_COMPLETE        0x03
#define ATTACH_REJECT          0x04

#define DETACH_REQUEST         0x05
#define DETACH_ACCEPT          0x06

#define ROUTE_UPDATE_REQUEST   0x08
#define ROUTE_UPDATE_ACCEPT    0x09
#define ROUTE_UPDATE_COMPLETE  0x0a
#define ROUTE_UPDATE_REJECT    0x0b

#define ACTIVE_PDP_REQUEST     0x41
#define ACTIVE_PDP_ACCEPT      0x42
#define ACTIVE_PDP_REJECT      0x43

#define DEACTIVE_PDP_REQUEST   0x46
#define DEACTIVE_PDP_ACCEPT    0x47

struct gmm {
    uint8_t pd;
    uint8_t type;
};
#endif

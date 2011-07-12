#ifndef __GLOBAL_H__
#define __GLOBAL_H__

/**
* TurboCap抓下来的原始数据包，重新封装
*/
struct raw_packet {
	unsigned long len;
	void *pkt;
};

typedef struct raw_packet raw_packet_t;
#endif // __GLOBAL

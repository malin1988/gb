#ifndef __DECODE__H_
#define __DECODE__H_
#include <stdint.h>

// 主进程向子进程传递的数据包格式
struct stat_pkt {
	uint16_t cmd; // 命令格式：控制,数据
	uint16_t len;  // 数据包长度
	uint8_t pkt[2000]; 	 // 数据
};

#define STAT_CMD_CNTL 0x01 // 命令包
#define STAT_CMD_DATA 0x02 // 数据包

int decode (void *buf, size_t len, int fd);

struct static_worker {
	int pid;
	int fd;
};


int init_static_worker(struct static_worker *sworker);

int do_work(int fd);
#endif

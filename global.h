#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <unistd.h>

/**
* TurboCap抓下来的原始数据包，重新封装
*/
struct raw_packet {
	unsigned long len;
	void *pkt;
};

typedef struct raw_packet raw_packet_t;

/**
* 工作进程结构
*/
struct worker {
	pid_t pid;
	int fd;
};
typedef struct worker worker_t;

/**
* 工作队列
*/

#define WORKER_CNT 4 // 工作进程数
struct worker_queue {
	worker_t worker[WORKER_CNT];
	int turn;
};

typedef struct worker_queue worker_queue_t;

#endif // __GLOBAL

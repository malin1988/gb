#ifndef __DECODE__H_
#define __DECODE__H_

int decode (void *buf, size_t len, int fd);

struct static_worker {
	int pid;
	int fd;
};


int init_static_worker(struct static_worker *sworker);

int do_work(int fd);
#endif

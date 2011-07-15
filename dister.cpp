#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "dister.h"
#include "utils.h"
#include "tc.h"
#include "global.h"
#include "worker.h"

// 工作队列
extern worker_queue_t workers;

/**
* 抓包分发函数入口
*/
void dister()
{
	// 初始化工作队列
	init_worker();

	// 开始抓包
	tc_run();
}

void init_worker()
{
	int i, fd[2];
	pid_t pid;

	for (i = 0; i < WORKER_CNT; i++) {
		// 初始化sockfd
		if (socketpair(AF_UNIX, 0, SOCK_STREAM, fd) < 0) {
			log_msg(LOG_LEVEL_ERR, strerror(errno));
		}

		pid = fork();
		if (pid == 0) {
			close(fd[0]);
			// 开启工作进程
			do_work(fd[1]);
		} else if (pid < 0) {
			log_msg(LOG_LEVEL_ERR, strerror(errno));
		} else {
			close(fd[1]);
			workers.worker[i].fd = fd[0];
			workers.worker[i].pid = pid;
		}
	}
}


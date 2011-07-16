#include <stdlib.h>
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
worker_queue_t workers;

/**
* 初始化工作队列
*/

static void init_worker(worker_queue_t * workers);

/**
* 抓包分发函数入口
*/
void dister()
{
	// 初始化工作队列
	init_worker(&workers);

	// 开始抓包
	tc_start(mydispatcher);
}

static void init_worker(worker_queue_t * workers)
{
	int i, fd[2];
	pid_t pid;

	for (i = 0; i < WORKER_CNT; i++) {
		// 初始化sockfd
		if (socketpair(AF_UNIX, SOCK_STREAM, 0,  fd) < 0) {
			log_msg(LOG_LEVEL_ERR, strerror(errno));
		}
		log_msg(LOG_LEVEL_INFO, "init_worker: worker[%d] init, fd[0]=%d, fd[1]=%d.", i, fd[0], fd[1]);

		pid = fork();
		if (pid == 0) {
			// 开启工作进程
			do_work(fd[1]);
			exit(0);
		} else if (pid < 0) {
			log_msg(LOG_LEVEL_ERR, strerror(errno));
		} else {
			workers->worker[i].fd = fd[0];
			workers->worker[i].pid = pid;
		}
	}
}

void mydispatcher(raw_packet_t *rpkt)
{
	//需保证工作队列是有效的
	int turn = (workers.turn == WORKER_CNT) ? 0 : workers.turn;
	int fd = workers.worker[turn].fd;
	int len = sizeof(rpkt->len) + rpkt->len;

	log_msg(LOG_LEVEL_INFO, "dispatcher:send pkt turn=%d, len=%lu", turn, len);
	// FIXME 不能保证写成功
	write(fd, rpkt, len);
	// 更新下一个工作进程
	workers.turn = ++turn;

	return;
}


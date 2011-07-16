#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include "tc.h"
#include "dister.h"
#include "utils.h"

int main()
{
	pid_t pid;

	log_msg(LOG_LEVEL_INFO, "main thread start,pid=%d", getpid());
	pid = fork();
	if (pid == 0) {
		/* 抓包分发进程 */
		dister();
		return 0;
	} else if (pid > 0) {
		/* 主监控进程 */
		//monitor();
		wait(NULL);
	} else {
		log_msg(LOG_LEVEL_ERR, strerror(errno));
	}

	return 0;
}

#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "tc.h"
#include "dister.h"
#include "utils.h"

int main()
{
	pid_t pid;

	pid = fork();
	if (pid == 0) {
		/* 抓包分发进程 */
		dister();
	} else if (pid > 0) {
		/* 主监控进程 */
		//monitor();
	} else {
		log_msg(LOG_LEVEL_ERR, strerror(errno));
	}

	return 0;
}

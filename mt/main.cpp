#include <unistd.h>
#include "tc.h"
#include "utils.h"
#include "decode.h"

int main()
{
	int daemonize = 0;
	struct static_worker worker;

	// 启动守护进程
	if (daemonize) {
		daemon(0, 0);
	}

	log_msg(LOG_LEVEL_INFO, "main process start");

	init_static_worker(&worker);
	log_msg(LOG_LEVEL_INFO, "init static worker done");

    tc_start(decode, worker.fd);

	return 0;
}

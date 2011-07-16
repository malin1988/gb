#include "global.h"
#include "worker.h"
#include "utils.h"

void do_work(int fd)
{
	log_msg(LOG_LEVEL_INFO, "do work called");
	while (1) {
		unsigned long headlen;
		if (read(fd, &headlen, sizeof(headlen)) != sizeof(headlen)) {
			log_msg(LOG_LEVEL_INFO, "Read pkt head ERROR", headlen);
		} else {
			log_msg(LOG_LEVEL_INFO, "Read pkt head: %lu", headlen);
		}

		char pktbuf[102400];
		if (read(fd, pktbuf, headlen) != headlen) {
			log_msg(LOG_LEVEL_INFO, "Read pkt body ERROR");
		} else {
			log_msg(LOG_LEVEL_INFO, "Read pkt body OK, len: %lu");
		}
	}
}

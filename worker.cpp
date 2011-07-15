#include "global.h"
#include "worker.h"

void do_work(int fd)
{
	unsigned long headlen;
	if (read(fd, &headlen, sizeof(headlen)) != sizeof(headlen)) {
		log_msg(LOG_LEVEL_INFO, "Read pkt head ERROR\n", headlen);
	} else {
		log_msg(LOG_LEVEL_INFO, "Read pkt head: %lu\n", headlen);
	}

	char pktbuf[102400];
	if (read(fd, pktbuf, headlen) != headlen) {
		log_msg(LOG_LEVEL_INFO, "Read pkt body ERROR\n");
	} else {
		log_msg(LOG_LEVEL_INFO, "Read pkt body OK, len: %lu\n");
	}
}

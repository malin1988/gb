#ifndef __TC_H__
#define __TC_H__

#include "TcApi.h"

typedef int (*dispatcher)(void *buf, size_t len, int fd);

void tc_start(dispatcher dispfunc, int fd);


static void ProcessPacketsBuffer(TC_PACKETS_BUFFER hPacketBuffer, dispatcher dispfunc, int fd);

#endif

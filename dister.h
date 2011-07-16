#ifndef __DISTER__H__
#define __DISTER__H__

#include "global.h"

/**
* 分发进程入口
*/
void dister();

void dispatcher(raw_packet_t *rpkt);
#endif

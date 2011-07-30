#ifndef __TC_H__
#define __TC_H__

#include "TcApi.h"

typedef int (*dispatcher)(void *buf, size_t len);

void tc_start(dispatcher dispfunc);


static void ProcessPacketsBuffer(TC_PACKETS_BUFFER hPacketBuffer, dispatcher dispfunc);

static void PrintPacket(PVOID pData, PTC_PACKET_HEADER pHeader);

#endif

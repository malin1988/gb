#ifndef __TC_H__
#define __TC_H__

#include "TcApi.h"
#include "global.h"

typedef void (*dispatcher)(raw_packet_t *rpkt);

void tc_start(dispatcher dispfunc);


static void ProcessPacketsBuffer(TC_PACKETS_BUFFER hPacketBuffer, dispatcher dispfunc);

static void PrintPacket(PVOID pData, PTC_PACKET_HEADER pHeader);

#endif

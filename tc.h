#ifndef __TC_H__
#define __TC_H__

#include "TcApi.h"
#include "global.h"

void tc_start();

void dispatcher(raw_packet_t *rpkt);

static void ProcessPacketsBuffer(TC_PACKETS_BUFFER hPacketBuffer);

static void PrintPacket(PVOID pData, PTC_PACKET_HEADER pHeader);

#endif

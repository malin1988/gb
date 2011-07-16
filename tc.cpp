#include <stdlib.h>
#include <stdio.h>
#include "tc.h"
#include "utils.h"

void tc_start(dispatcher dispfunc)
{
	TC_INSTANCE hInstance;
	TC_PACKETS_BUFFER hPacketsBuffer;
	TC_STATUS result;
	PTC_PORT portList;
	TC_PORT	port;
	ULONG	numPorts;	

	result = TcQueryPortList(&portList, &numPorts);

	if (result != TC_SUCCESS)
	{
		log_msg(LOG_LEVEL_ERR, "Error, cannot open port, error %s (%08x)\n", TcStatusGetString(result), result);
	}

	if (numPorts == 0) {
		log_msg(LOG_LEVEL_ERR, "No ports available");
	}

	/* 取第一个可用的端口 */
	port = portList[0];

	result = TcInstanceOpenByPort(port, &hInstance);
	if (result != TC_SUCCESS) {
		log_msg(LOG_LEVEL_ERR, "Can't open port");
	}

	TcFreePortList(portList);

	result = TcInstanceSetFeature(hInstance, TC_INST_FT_RX_STATUS, 1);
	if (result != TC_SUCCESS)
	{
		(VOID)TcInstanceClose(hInstance);
		log_msg(LOG_LEVEL_ERR, "Can't enable reception: %s (%08x)", TcStatusGetString(result), result);
	}

	/* 循环抓包 */
	do {
		result = TcInstanceReceivePackets(hInstance, &hPacketsBuffer);
		if (result != TC_SUCCESS) 
			break;

		if (hPacketsBuffer == NULL) 
			continue;

		ProcessPacketsBuffer(hPacketsBuffer, dispfunc);
		TcPacketsBufferDestroy(hPacketsBuffer);
	}while(TRUE);

	if (result != TC_SUCCESS)
	{
		log_msg(LOG_LEVEL_ERR, "Error in the reception process: %s (%08x)\n", TcStatusGetString(result), result);
	}

	TcInstanceClose(hInstance);
}

static void ProcessPacketsBuffer(TC_PACKETS_BUFFER hPacketsBuffer, dispatcher dispfunc)
{
    PVOID pData;
    TC_PACKET_HEADER header;
    TC_STATUS status;
	raw_packet_t rpkt;

    do {
		status = TcPacketsBufferQueryNextPacket(hPacketsBuffer,
				&header,
				&pData);

		if (status != TC_SUCCESS) break;

		// 封装成自己的格式
		rpkt.len = header.Length;
		rpkt.pkt = pData;

		/* FIXME 临时测试用*/
		// PrintPacket(pData, &header);
		/* 调用分发器 */
		dispfunc(&rpkt);
    }while(TRUE);
}

static void PrintPacket(PVOID pData, PTC_PACKET_HEADER pHeader)
{
	printf("Received packet at %I64u, size = %u\n", pHeader->Timestamp, pHeader->Length);
}


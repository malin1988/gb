#include <stdlib.h>
#include <stdio.h>
#include "tc.h"
#include "utils.h"

void tc_start(dispatcher dispfunc, int fd)
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
	port = portList[1];

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

		ProcessPacketsBuffer(hPacketsBuffer, dispfunc, fd);
		TcPacketsBufferDestroy(hPacketsBuffer);
	}while(TRUE);

	if (result != TC_SUCCESS)
	{
		log_msg(LOG_LEVEL_ERR, "Error in the reception process: %s (%08x)", TcStatusGetString(result), result);
	}

	TcInstanceClose(hInstance);
}

static void ProcessPacketsBuffer(TC_PACKETS_BUFFER hPacketsBuffer, dispatcher dispfunc, int fd)
{
    PVOID pData;
    TC_PACKET_HEADER header;
    TC_STATUS status;

    do {
		status = TcPacketsBufferQueryNextPacket(hPacketsBuffer,
				&header,
				&pData);

		if (status != TC_SUCCESS) break;
		/* 调用分发器 */
		dispfunc(pData, header.Length, fd);
    }while(TRUE);
}

#include "NetworkLayer.h"

#define PACKETSIZE 97						//Defines the maximum packet length

struct FrameStruct WTX_TEMP[PACKETSIZE];	//Temporary array for WifiTransmitTask
uint16_t WTX_COUNTER = 0;					//Point the next available place in the 'WTX_TEMP'

/*
*	Name: WifiTransmitTask
*	Description: This task responsible for the continious frame transmit 
*				 via WiFi from 'xWifiTxQueue'. Interprets the frame ID's
*				 and execute the proper operation. (ex. send, userdata store)  
*/
void WifiTransmitTask(void *pvParameters)
{
	while(1)
	{
		if (xQueueReceive(xWifiTxQueue, &WTX_TEMP[WTX_COUNTER], portMAX_DELAY) == pdTRUE)
		{
			switch(WTX_TEMP[WTX_COUNTER].Id){
				case FTYPE_USERDATA:
					if((WTX_COUNTER+1) < PACKETSIZE)
						WTX_COUNTER++;
				break;
				case FTYPE_UDPSEND:
					NWLwifiSendUDP(&WTX_TEMP[WTX_COUNTER]);
				break;
			}
			//SendFrame(&WTX_TEMP[WTX_COUNTER]);
		}
	}
}

/*
*	Name: UartTransmitTask
*	Description: This task responsible for the continious frame transmit 
*				 via UART from 'xUartTxQueue'. 
*/
void UartTransmitTask(void *pvParameters)
{
	struct FrameStruct Temp;
	while(1)
	{
		if(xQueueReceive(xUartTxQueue, &Temp, portMAX_DELAY) == pdTRUE){
			SendFrame(&Temp);
		//vTaskDelay(1000 / portTICK_RATE_MS);
		}
	}
}

/*
*	Name: RoutingAlgorithm
*	Description: This task responsible for routes each frame  
*				 to the proper queue.
*/
void RoutingAlgorithm(void *pvParameters)
{
	struct FrameStruct RTemp;
	while(1)
	{
		if(xQueueReceive(xUartRxQueue, &RTemp, portMAX_DELAY) == pdTRUE){  		//Checks that are there any available frame
																				//for processing
			switch(RTemp.Id){
				case FTYPE_USERDATA:
				case FTYPE_UDPSEND:
					xQueueSend(xWifiTxQueue, &RTemp, portMAX_DELAY);
				break;
			}
			//xQueueSend(xUartTxQueue, &RTemp, portMAX_DELAY);
		}
	}
}

/*
*	Name: NetworkLayerInit
*	Description: The function creates the task which are process the frames 
*				 
*/
void ICACHE_FLASH_ATTR
NetworkLayerInit(){

	xUartRxQueue = xQueueCreate(UART_RX_Q_SIZE, sizeof(struct FrameStruct));
	xUartTxQueue = xQueueCreate(UART_TX_Q_SIZE, sizeof(struct FrameStruct));
	xWifiTxQueue = xQueueCreate(WIFI_TX_Q_SIZE, sizeof(struct FrameStruct));

	espconn_init();

	xTaskCreate(WifiTransmitTask, "WifiTransmitTask", 256, NULL, 2, NULL); 
	xTaskCreate(UartTransmitTask, "UartTransmitTask", 256, NULL, 2, NULL); 
	xTaskCreate(RoutingAlgorithm, "RoutingAlgorithm", 256, NULL, 2, NULL); 
}

/*
*	Name: NWLwifiSendUDP
*	Description: Sends the 'WTX_TEMP' array items through WiFi with UDP protocol.
*				 The paramater frame contains the IP address and the port number.
*				 
*/
void ICACHE_FLASH_ATTR
NWLwifiSendUDP(struct FrameStruct *frame){
	uint32_t port = 0;
	port = frame->data[7];
	port = port << 8;
	port = port | frame->data[6];
	port = port << 8;
	port = port | frame->data[5];
	port = port << 8;
	port = port | frame->data[4];
	/*
	char buffer[26];
	sprintf(buffer,"IP %d.%d.%d.%d PORT %d", frame->data[0], frame->data[1], frame->data[2], frame->data[3], port); //system_get_free_heap_size
	sendDebug(buffer, 26);*/

	sendResponse.type = ESPCONN_UDP;
	sendResponse.state = ESPCONN_NONE;
	sendResponse.proto.udp = &udp;
	IP4_ADDR((ip_addr_t *)sendResponse.proto.udp->remote_ip, frame->data[0], frame->data[1], frame->data[2], frame->data[3]);
	sendResponse.proto.udp->remote_port = port; // Remote port

	sint8 err;
	err = espconn_create(&sendResponse);
	err = espconn_send(&sendResponse, (char*)&WTX_TEMP[0], FRAME_SIZE_BYTE*WTX_COUNTER);
	espconn_delete(&sendResponse);
	
	WTX_COUNTER = 0;
	return;
}


/*
*	Name: SendFrame
*	Description: Send a single frame through uart.
*				 
*/
void ICACHE_FLASH_ATTR
SendFrame(struct FrameStruct *ToSend){
	char temp[FRAME_SIZE_BYTE];
	temp[0] = ToSend->Id;
	temp[1] = ToSend->FrameNumber;
	temp[14] = ToSend->crc;

	int i;
	for (i = 2; i < FRAME_SIZE_BYTE-1; ++i)
		temp[i] = ToSend->data[i-2];

	//sendDatagram(temp, 15);
	UartSendFrame(temp);
}


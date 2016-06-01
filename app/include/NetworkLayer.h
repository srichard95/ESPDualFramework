#ifndef INCLUDE_NETWORKLAYER_H_
#define INCLUDE_NETWORKLAYER_H_

#include "esp_common.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include "DataLinkLayer.h"
#include "espconn.h"

struct FrameStruct {
	char Id;
	char FrameNumber;
	char data[12];
	char crc;
};

#define UART_RX_Q_SIZE 100
xQueueHandle xUartRxQueue;

#define UART_TX_Q_SIZE 10
xQueueHandle xUartTxQueue;

#define WIFI_RX_Q_SIZE 10
xQueueHandle xWifiRxQueue;

#define WIFI_TX_Q_SIZE 97
xQueueHandle xWifiTxQueue;

#define FTYPE_USERDATA 0x00
#define FTYPE_UDPSEND 0x20

struct espconn sendResponse;
esp_udp udp;

void ICACHE_FLASH_ATTR NetworkLayerInit();
void ICACHE_FLASH_ATTR SendFrame();
void ICACHE_FLASH_ATTR NWLwifiSendUDP(struct FrameStruct *frame);

#endif
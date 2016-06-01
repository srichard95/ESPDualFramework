#ifndef INCLUDE_DATALINKLAYER_H_
#define INCLUDE_DATALINKLAYER_H_

#include "esp_common.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include "networklayer.h"

#define FRAME_SIZE_BYTE	15		//Size of each frame
#define DEBUG_ON	0			//DEBUG function on/off
#define SYNC_TIMEOUT_THRS 100

void ICACHE_FLASH_ATTR uart_isr_handle(void);
void ICACHE_FLASH_ATTR uart_send_frame(char *frame);
bool ICACHE_FLASH_ATTR IsSyncFrame(char *frame);
void ICACHE_FLASH_ATTR Sync_procedure(void);
void ICACHE_FLASH_ATTR SendSyncFrame(void);
void ICACHE_FLASH_ATTR ResetBuffer(void);
void ICACHE_FLASH_ATTR PutFrameInQueue();

xSemaphoreHandle xSemaphore;

#endif

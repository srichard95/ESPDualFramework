#ifndef INCLUDE_DATALINKLAYER_H_
#define INCLUDE_DATALINKLAYER_H_

#include "esp_common.h"
#include "freertos/FreeRTOS.h"
#include "freertos/FreeRTOSConfig.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include "networklayer.h"

#define FRAME_SIZE_BYTE	15		//Size of each frame
#define SYNC_TIMEOUT_THRS 100	//Sync timeout value

void UartISRHandle(void *pvParameters);
void ICACHE_FLASH_ATTR UartSendFrame(char *frame);
bool ICACHE_FLASH_ATTR IsSyncFrame(char *frame);
void ICACHE_FLASH_ATTR SyncProcedure(void);
void ICACHE_FLASH_ATTR SendSyncFrame(void);
void ICACHE_FLASH_ATTR PutFrameInQueue();
void ICACHE_FLASH_ATTR DLLinit();

#endif

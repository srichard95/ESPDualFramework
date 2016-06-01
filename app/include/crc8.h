#include "esp_common.h"
#include "freertos/FreeRTOS.h"


uint8_t ICACHE_FLASH_ATTR CreateCRC(uint8_t *s);
uint8_t ICACHE_FLASH_ATTR CheckCRC(uint8_t *s);
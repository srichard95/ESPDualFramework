/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2015 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "esp_common.h"
#include "driver/uart.h"
#include "NetworkLayer.h"
#include "espconn.h"
#include "user_config.h"
#include "DataLinkLayer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/FreeRTOSConfig.h"
#include "freertos/semphr.h"

/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/

/******************************************************************************
DEBUG SECTION! THIS CAN BE SWITCHED BY 'DEBUG_ON' CONSTANT.
*******************************************************************************/
#if DEBUG_ON
void recvCB(void *arg, char *pData, unsigned short len) {
	struct espconn *pEspConn = (struct espconn *)arg;
	os_printf("Received data!! - length = %d\n", len);
	int i=0;
	for (i=0; i<len; i++) {
		os_printf("%c", pData[i]);
	}

	
	if(pData[0] == 'c')
	{
		char ToSend[] = { 0x78, 0x1C, 0x60, 0x7D, 0xDC, 0x69, 0x3B, 0x56, 0xDB, 0xB5, 0xF1, 0xD7, 0xC4, 0xA5, 0x1D };
		struct FrameStruct Temp = {0x78, 0x1C, 0x60, 0x7D, 0xDC, 0x69, 0x3B, 0x56, 0xDB, 0xB5, 0xF1, 0xD7, 0xC4, 0xA5, 0x1D};
		int i;
		for(i=0; i<97;i++)
			xQueueSend(xWifiTxQueue, &Temp, portMAX_DELAY);
		UartSendFrame(ToSend);
	}
	if(pData[0] == 'w')
	{
		char ToSend[] = { 0x50, 0x4D, 0xF4, 0xEB, 0xF8, 0xB1, 0x6F, 0x75, 0x43, 0x82, 0xAB, 0x23, 0x42, 0xE9 };
		UartSendFrame(ToSend);
	}
	if(pData[0] == 's')
	{
		char ToSend[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
		UartSendFrame(ToSend);
	}

	os_printf("\n");
} // End of recvCB


struct espconn conn1;
esp_udp udp1;


struct espconn sendResponseDebug;
esp_udp udpDebug;

void setupUDP() {
	sint8 err;
	conn1.type = ESPCONN_UDP;
	conn1.state = ESPCONN_NONE;
	udp1.local_port = 4001;
	conn1.proto.udp = &udp1;
	err = espconn_create(&conn1);
	err = espconn_regist_recvcb(&conn1, recvCB);




	sendResponseDebug.type = ESPCONN_UDP;
	sendResponseDebug.state = ESPCONN_NONE;
	sendResponseDebug.proto.udp = &udpDebug;
	IP4_ADDR((ip_addr_t *)sendResponseDebug.proto.udp->remote_ip, 192, 168, 4, 255);
	sendResponseDebug.proto.udp->remote_port = 5000; // Remote port


} // End of setupUDP

void sendDebug(char *datagram, uint16 size) {
	sint8 err;
	err = espconn_create(&sendResponseDebug);
	err = espconn_send(&sendResponseDebug, datagram, size);
	espconn_delete(&sendResponseDebug);
}
#endif
/******************************************************************************
DEBUG SECTION ENDS!
*******************************************************************************/

void user_init(void)
{
    wifi_set_opmode(SOFTAP_MODE);

    struct softap_config *config = (struct softap_config *)zalloc(sizeof(struct softap_config));
	wifi_softap_get_config(config); // Get soft-AP config first.

 	sprintf(config->ssid, DEMO_AP_SSID);
 	sprintf(config->password, DEMO_AP_PASSWORD); 

	config->authmode = AUTH_WPA_WPA2_PSK;
	config->ssid_len = 0; // or its actual SSID length
	config->max_connection = 4;
	wifi_softap_set_config(config); // Set ESP8266 soft-AP config
	free(config); 

	system_update_cpu_freq(SYS_CPU_160MHZ);

	UartDriverInit();
	
	DLLinit();

	NetworkLayerInit();

	//DEBUG SECTION INITIALIZE
	#if DEBUG_ON
	setupUDP();
	#endif
	//************************
}


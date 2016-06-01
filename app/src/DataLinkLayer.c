#include "DataLinkLayer.h"

bool sync = false;

char InBuffer[FRAME_SIZE_BYTE];
int BuffPointer = 0;
/*
*	Name: uart_isr_handle
*	Description: Handle the interrupt of the uart.
*				 Frame processing is happening here.
*/
void ICACHE_FLASH_ATTR
uart_isr_handle(void)
{

	//int rxbytes = uart_rx_available();

	//while(rxbytes > 0)
	//{
		InBuffer[BuffPointer] = uart_getchar();
		BuffPointer++;


		if(BuffPointer == FRAME_SIZE_BYTE)
		{
			BuffPointer = 0;
			if(CheckCRC(InBuffer) == 0)
			{
				//sendDatagram(InBuffer, FRAME_SIZE_BYTE);
				PutFrameInQueue();
				//uart_send_frame(InBuffer);
			}
			else
			{
				/*sendDatagram("CRC FAIL!", 9);
				sendDatagram(InBuffer, FRAME_SIZE_BYTE);*/
				Sync_procedure();
				//break;
			}
		}

	//rxbytes = uart_rx_available();
	//}
	return;
}

void ICACHE_FLASH_ATTR
Sync_procedure(void)
{
  int SyncTimeout = 0;
  int FFs = 0;
  char c;

  SendSyncFrame();
  //sendDatagram("SYNC 1!", 7);

  while(FFs != FRAME_SIZE_BYTE)
  {
    SyncTimeout++;
    c = uart_getchar_ms(1);
    if(c == 0xFF)
    {
      FFs++;
      c = 0x00;
      //sendDatagram("SYNC 2!", 7);
    }
    if(SyncTimeout >= SYNC_TIMEOUT_THRS){
      SendSyncFrame();
      SyncTimeout = 0;
      uart_rx_flush();
      //sendDatagram("SYNC 3!", 7);
    }
  }
  return;
}

void ICACHE_FLASH_ATTR
SendSyncFrame(void)
{
	char sync_frame[FRAME_SIZE_BYTE];
	int i;
	for (i = 0; i < FRAME_SIZE_BYTE; ++i)
		sync_frame[i] = 0xFF;
	uart_send_frame(sync_frame);
	return;
}
void ICACHE_FLASH_ATTR
ResetBuffer(void)
{
	int i;
	for (i = 0; i < FRAME_SIZE_BYTE; ++i)
		InBuffer[i] = 0x00;
}



bool ICACHE_FLASH_ATTR
IsSyncFrame(char *frame)
{
	int i;
	bool result = true;
	for (i = 0; i < FRAME_SIZE_BYTE; ++i)
		if(frame[i] != 0xFF)
			result = false;
	return result;
}



void ICACHE_FLASH_ATTR
uart_send_frame(char *frame){
	int i;
	for (i = 0; i < FRAME_SIZE_BYTE; ++i)
		uart_tx_one_char(UART0, frame[i]);
}

void ICACHE_FLASH_ATTR
PutFrameInQueue(){
	struct FrameStruct Temp;
	Temp.Id = InBuffer[0];
	Temp.FrameNumber = InBuffer[1];
	Temp.crc = InBuffer[14];
	int i;
	for (i = 2; i < FRAME_SIZE_BYTE-1; ++i)
		Temp.data[i-2] = InBuffer[i];

	xQueueSend( xUartRxQueue, &Temp, 10000 );
	return;
}


/*
void ICACHE_FLASH_ATTR
uart_init()
{
	UART_ConfigTypeDef uart_config; 
	uart_config.baud_rate = BIT_RATE_921600;
	uart_config.data_bits = UART_WordLength_8b;
	uart_config.parity = USART_Parity_None;
	uart_config.stop_bits = USART_StopBits_1;
	uart_config.flow_ctrl = USART_HardwareFlowControl_None;
	uart_config.UART_RxFlowThresh = 120;
	uart_config.UART_InverseMask = UART_None_Inverse;
	UART_ParamConfig(UART0, &uart_config);


	if(DEBUG_ON)
		UART_SetPrintPort(UART0);
	else
		UART_SetPrintPort(UART1);

	xSemaphore = NULL;
	vSemaphoreCreateBinary( xSemaphore );
	//uart_rx_init();
}*/



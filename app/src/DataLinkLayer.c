#include "DataLinkLayer.h"

char InBuffer[FRAME_SIZE_BYTE];		//InBuffer contains the frame which read from the FIFO
int BuffPointer = 0;				//Point the next place inside the InBuffer

/*
*	Name: UartISRHandle
*	Description: Handle the interrupt of the uart.
*				 Frame processing is happening here.
*/
void UartISRHandle(void *pvParameters)
{
	while(1){

		InBuffer[BuffPointer] = uart_getchar();
		BuffPointer++;

		if(BuffPointer == FRAME_SIZE_BYTE)
		{
			BuffPointer = 0;
			if(CheckCRC(InBuffer) == 0)
				PutFrameInQueue();
			else
				SyncProcedure();
		}
	}
}

/*
*	Name: SyncProcedure
*	Description: Sync algorithm works here
*
*/
void ICACHE_FLASH_ATTR
SyncProcedure(void)
{
  int SyncTimeout = 0;
  int FFs = 0;								//Counts the 'FFs' which come on uart
  char c;									//Contains the next actual byte which read from the uart

  SendSyncFrame();

  while(FFs != FRAME_SIZE_BYTE)				//Until we can't catch 'FRAME_SIZE_BYTE' pieces byte from uart
  {
    SyncTimeout++;							//Counts the timeout
    c = uart_getchar_ms(1);					//Gets the next byte from uart and store it in 'c'
    if(c == 0xFF)
    {
      FFs++;								//Increase the 'FFs' counter
      c = 0x00;
    }
    if(SyncTimeout >= SYNC_TIMEOUT_THRS){	//If we hadn't got 'FRAME_SIZE_BYTE' pieces 'FF' byte for a while we resend
      SendSyncFrame();						//the sync frame
      SyncTimeout = 0;
      uart_rx_flush();
    }
  }
  return;
}

/*
*	Name: SendSyncFrame
*	Description: Sends a single sync frame via
*				 uart.
*/
void ICACHE_FLASH_ATTR
SendSyncFrame(void)
{
	char sync_frame[FRAME_SIZE_BYTE];
	int i;
	for (i = 0; i < FRAME_SIZE_BYTE; ++i)
		sync_frame[i] = 0xFF;
	UartSendFrame(sync_frame);
	return;
}

/*
*	Name: UartSendFrame
*	Description: Sends a single frame through uart.
*				 
*/
void ICACHE_FLASH_ATTR
UartSendFrame(char *frame){
	int i;
	for (i = 0; i < FRAME_SIZE_BYTE; ++i)
		uart_tx_one_char(UART0, frame[i]);
}

/*
*	Name: PutFrameInQueue
*	Description: Puts a frame from 'InBuffer' into the 'xUartRxQueue'.
*				 The NetworkLayer read the frames from the xUartRxQueue.
*/
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
*	Name: DLLinit
*	Description: Initialize the DataLinkLayer.
*				 Starts the UartISRHandle task.
*/
void ICACHE_FLASH_ATTR
DLLinit(){
	xTaskCreate(UartISRHandle, "UartISRHandle", 256, NULL, tskIDLE_PRIORITY+2, NULL);
	return;
}



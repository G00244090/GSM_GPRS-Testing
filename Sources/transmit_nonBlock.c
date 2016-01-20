/*
 * transmit_nonBlock.c
 *
 *  Created on: 16 Jan 2016
 *      Author: Aonghus
 **********************************************************************************************/
#include "fsl_device_registers.h"
#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_uart_driver.h"

char ch;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
extern void UART_DRV_IRQHandler(uint32_t instance);
/*******************************************************************************
 * Code
 ******************************************************************************/

/* Implementation of UART0 handler named in startup code. */
void UART1_IRQHandler(void)
{
	UART_DRV_IRQHandler(1);
}

int main(void)
{
	uart_state_t uartState; // user provides memory for the driver state structure
	uart_user_config_t uartConfig;
	long int x;
	char AT[] = "AT\n\r";

    //Initialise the FRDM-KL26Z Board
	hardware_init();
	configure_uart_pins(0);	//instance 0 is UART1???
	OSA_Init();

	uartConfig.baudRate = 9600;
	uartConfig.bitCountPerChar = kUart8BitsPerChar;
	uartConfig.parityMode = kUartParityDisabled;
	uartConfig.stopBitCount = kUartOneStopBit;

	UART_DRV_Init(1, &uartState,&uartConfig);
	//Print message to serial terminal
	PRINTF("First Embedded Systems Lab_Aonghus\r\n");
	PRINTF("Type a character and it will be echoed back\r\n\n");
	uint32_t txBytesRemaining, rxBytesRemaining;
	printf("Size of AT is %d\n",sizeof(AT));

	UART_DRV_SendDataBlocking(1, AT, sizeof(AT)+8,300);
	if(UART_DRV_GetTransmitStatus(1, &txBytesRemaining) == kStatus_UART_Success )
		printf("Success\n");
	while(UART_DRV_GetTransmitStatus(1, &txBytesRemaining) == kStatus_UART_TxBusy)
	{
		PRINTF("Busy\n");
	}


    while(1) {
    	//UART_DRV_SendDataBlocking(1, AT, sizeof(AT), 1000);
    		//PRINTF("SUCCESS\n\r");

    	for(x=0;x<5000000;x++);
    }
    /* Never leave main */
    return 0;
}
////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////

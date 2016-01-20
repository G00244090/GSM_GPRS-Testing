/*
 * Uart1_recieve_test.c
 *
 *  Created on: 18 Jan 2016
 *      Author: Aonghus
 */

#include "fsl_device_registers.h"
#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_uart_driver.h"

char ch;
int wait_time;
int result = 5;
#define SUCCESS 0
#define FAIL -1
#define ERROR -2
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
extern void UART_DRV_IRQHandler(uint32_t instance);
/*******************************************************************************
 * Code
 ******************************************************************************/

/* Implementation of UART1 handler named in startup code. */
void UART1_IRQHandler(void){
	UART_DRV_IRQHandler(1);
}

int send_command(int instance, char * command_ptr, int byteCountBuff, int wait_time,char * response ){
	PRINTF("\r\nInside send command\r\n");
	int i =0;
	UART_DRV_SendDataBlocking(instance, command_ptr, byteCountBuff, wait_time);
	 while(UART_DRV_ReceiveDataBlocking (instance, response, sizeof(response),16000u) == kStatus_UART_Success ){
		 PRINTF("Recieved:\r\n");
		 for(i=0;i<sizeof(response);i++)
		 PRINTF("%c",response[i]);
	 }
	 if(strstr(response - sizeof(response), "Goodnight") ){
		PRINTF("\nRecived Goodbye in UART\n\r");
		return 0;
	}

}
int main(void)
{
	uart_state_t uartState; // user provides memory for the driver state structure
	uart_user_config_t uartConfig;
	long int x;

	char AT[] = "\nTA";
	char response[20];
	enum STATES {INIT, CHECK_PIN};
	enum STATES CurrentState = INIT;
	uint32_t byteCountBuff = 0;
    //Initialise the FRDM-KL26Z Board
	hardware_init();
	configure_uart_pins(0);	//instance 0 is UART1???
	// Call OSA_Init to setup LP Timer for timeout
	OSA_Init();


	uartConfig.baudRate = 9600;
	uartConfig.bitCountPerChar = kUart8BitsPerChar;
	uartConfig.parityMode = kUartParityDisabled;
	uartConfig.stopBitCount = kUartOneStopBit;

	UART_DRV_Init(1, &uartState,&uartConfig);

	PRINTF("Full test of send/recieve on UART1\r\n");

	int i = 0;
	byteCountBuff = sizeof(AT);
	wait_time = 16000u;
    while(1) {
    	//UART_DRV_SendDataBlocking(1, AT, byteCountBuff, 16000u);
		//    	 while(UART_DRV_ReceiveDataBlocking ( 1, rxBuff, sizeof(response),16000u) == kStatus_UART_Success )
		//    	 {
		//    		 for(i=0;i<sizeof(response);i++)
		//    		 PRINTF("%c",response[i]);
		//    	 }
		//    	 for(x=0;x<10000000;x++);
    	 
    	 switch(CurrentState)
    	 		{
    	 		case INIT:	//Check connection to MODEM by sending AT. Expected response is OK
    	 			result = send_command(1,AT,byteCountBuff,wait_time,response);
    	 			//send_command(int instance, char * command_ptr, int byteCountBuff, int wait_time) 
    	 			//send_command(AT, response, sizeof(response), 2000);
    	 			if(result == SUCCESS || result == ERROR)
    	 			{
    	 				//printf_response(response);
    	 			}
    	 			if(result == SUCCESS)	//"OK" was returned by MODEM
    	 				CurrentState = CHECK_PIN;
    	 			break;
    	 		case CHECK_PIN:	//Check connection to MODEM by sending AT. Expected response is OK
					PRINTF("CHECK_PIN");
					break;
    	 		default:
    	 			break;
    	 		}//end switch-case
    }
    /* Never leave main */
    return 0;
}
////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////




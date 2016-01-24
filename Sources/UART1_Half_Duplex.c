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
	int i =0, x;
	UART_DRV_SendDataBlocking(instance, command_ptr, byteCountBuff, wait_time);
	printf("Sent\r\n");
	 while(UART_DRV_ReceiveDataBlocking (instance, response, sizeof(response),200000u) == kStatus_UART_Success ){
		 PRINTF("Recieved:\r\n");
		 for(i=0;i<=sizeof(response);i++)
		 {
//			 if(response[i] == '\r')
//				 PRINTF("HI\n");
//		 while(response[i] != '\0')
//		 {
			 PRINTF("%c",response[i]);
//			 if(response[i] == '\r')
//						 PRINTF("\\r");
//			 if(response[i] == '\n')
//									 PRINTF("\\n");
			 //i++;
		 }

		 //PRINTF("%c",response[i]);
		 //}
//		 while(*response != '\0')
//		 	{
//		 		PUTCHAR(*response);
//		 		if(*response++ == '\r')
//		 			PUTCHAR('\n');
//		 	}
	 }
//	 if(strcmp(response[0],"ER") ){
//		PRINTF("\nRecived OK in UART\n\r");
//		return 0;
//	}
//	 else
//	 {
//		 return -2;
//	 }

}
int main(void)
{
	uart_state_t uartState; // user provides memory for the driver state structure
	uart_user_config_t uartConfig;
	long int x;


	char AT[] = "AT+GMI\r";
	char PIN_CHECK[] = "AT+CPIN?\r";//Setting up a char variable PIN_CHECK for the a long string
	//char  PIN_ENTER[] = "AT+CPIN=\"1234\"\r\n";//Setting up a char variable ENTER_PIN for the a long string
	//char  CREG[] = "AT+CREG?\r\n";//Setting up a char variable CREG for the a long string
	uint8_t response[100];
	enum STATES {INIT, CHECK_PIN,ENTER_PIN};
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

	int i = 0,y;
	for(y=0;y<sizeof(response);y++)
		response[i]=0;

	byteCountBuff = sizeof(AT);
	wait_time = 16000u;
    while(1) {
    	switch(CurrentState)
    	 		{
    	 		case INIT:	//Check connection to MODEM by sending AT. Expected response is OK
    	 			result = send_command(1,AT,byteCountBuff,wait_time,response);
    	 			if(result == SUCCESS)	//"OK" was returned by MODEM
    	 				CurrentState = CHECK_PIN;
    	 			break;
    	 		case CHECK_PIN:	//Check connection to MODEM by sending AT. Expected response is OK
					PRINTF("CHECK_PIN\r");
//					byteCountBuff = sizeof(PIN_CHECK);
//					result = send_command(1,PIN_CHECK,byteCountBuff,wait_time,response);
//					if(result == SUCCESS)	//"OK" was returned by MODEM
//						CurrentState = ENTER_PIN;
//					else if(result == ERROR)
//						CurrentState = ENTER_PIN;
					break;
    	 		case ENTER_PIN:	//Check connection to MODEM by sending AT. Expected response is OK
					PRINTF("ENTER_PIN\r");
//					result = send_command(1,AT,byteCountBuff,wait_time,response);
//					//send_command(int instance, char * command_ptr, int byteCountBuff, int wait_time)
//					//send_command(AT, response, sizeof(response), 2000);
//					if(result == SUCCESS || result == ERROR)
//					{
//						//printf_response(response);
//					}
//					if(result == SUCCESS)	//"OK" was returned by MODEM
//						CurrentState = CHECK_PIN;
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




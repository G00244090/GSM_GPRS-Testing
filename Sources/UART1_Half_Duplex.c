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
uart_status_t returnValue;
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

int send_command(int instance, char * command_ptr, int byteCountBuff, int wait_time,char * response,int recieve_size){
	PRINTF("\r\nInside send command\r\n");
	int i =0, x;
	for(x=0;x<200;x++)
		response[x]=0;
	UART_DRV_SendDataBlocking(instance, command_ptr, byteCountBuff, wait_time);
	printf("Sent\r\n");
	printf("Size of response1 %d\r\n",sizeof(response));
	returnValue = UART_DRV_ReceiveDataBlocking (instance,response,recieve_size,wait_time);//Buffer size in bytes to accept all of the me
	printf("Size of response2 %d\r\n",sizeof(response));
	if(returnValue == kStatus_UART_Success){
		PRINTF("Fn Recieved:\r\n");
		while(response[i] != '\0')
		{
			 PRINTF("%c",response[i]);
			 i++;
		}
		PRINTF("\r\n");

		if(strstr(*response,"OK") || strstr(*response,">") || strstr(*response,"+CMSG:")  == 0){
			printf("Response matches\r\n");
			return 0;
		}

		 //return -2;
	}
}
int main(void)
{
	uart_state_t uartState; // user provides memory for the driver state structure
	uart_user_config_t uartConfig;
	long int x;
	int recieve_size=0;
	char AT[] = "AT\r";
	char CMGF[] = "AT+CMGF=1\r";
	char CMGS[] = "AT+CMGS=\"+353877763894\"\r";
	char MESSAGE[] = "HELLO AONGHUS KL26Z \x1A";

	char response[200];
	enum STATES {INIT,TXT_MODE,SEND_SMS,SEND_MSG};
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
	for(y=0;y<=sizeof(response);y++)
		response[i]='c';

	byteCountBuff = sizeof(AT);
	wait_time = 16000u;
    while(1){
    	switch(CurrentState){
			case INIT:	//Check connection to MODEM by sending AT. Expected response is OK
				recieve_size =4;
				byteCountBuff = sizeof(AT);
				result = send_command(1,AT,byteCountBuff,wait_time,response,recieve_size);
				if(result == SUCCESS){
					printf("returned INIT\r\n");//"OK" was returned by MODEM
					CurrentState = TXT_MODE;
				}
				break;
			case TXT_MODE:	//Check connection to MODEM by sending AT. Expected response is OK
				recieve_size =4;
				byteCountBuff = sizeof(CMGF);
				result = send_command(1,CMGF,byteCountBuff,wait_time,response,recieve_size);
				if(result == SUCCESS){
					printf("returned TXT_MODE\r\n");//"OK" was returned by MODEM
					CurrentState = SEND_SMS;
				}
				break;
			case SEND_SMS:	//Check connection to MODEM by sending AT. Expected response is OK
				recieve_size = 2;
				byteCountBuff = sizeof(CMGS);
				result = send_command(1,CMGS,byteCountBuff,wait_time,response,recieve_size);
				if(result == SUCCESS){
					printf("returned SEND_SMS\r\n");//"OK" was returned by MODEM
					CurrentState = SEND_MSG;
				}
				break;
			case SEND_MSG:	//Check connection to MODEM by sending AT. Expected response is OK
				recieve_size =8;
				byteCountBuff = sizeof(MESSAGE);
				result = send_command(1,MESSAGE,byteCountBuff,wait_time,response,recieve_size);
				if(result == SUCCESS){
					printf("returned SEND_MSG\r\n");//"OK" was returned by MODEM
					//CurrentState = CHECK_PIN;
				}
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




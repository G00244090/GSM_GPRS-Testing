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
long int x;
int recieve_size=0;
uint32_t byteCountBuff = 0;
int wait_time;
int result = 5;
#define SUCCESS 0
#define FAIL -1
#define ERROR -2
uart_status_t returnValue;
void GSM_init();
void update();
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
	PRINTF("Sent\r\n");
	PRINTF("Size of response %d\r\n",sizeof(response));
	returnValue = UART_DRV_ReceiveDataBlocking (instance,response,recieve_size,wait_time);//Buffer size in bytes to accept all of the me
	PRINTF("Size of response2 %d\r\n",sizeof(response));
	if(returnValue == kStatus_UART_Success){
		PRINTF("Fn Recieved:\r\n");
		while(response[i] != '\0')
		{
			 PRINTF("%c",response[i]);
			 i++;
		}
		PRINTF("\r\n");

		if(strstr(*response,"OK") || strstr(*response,">") || strstr(*response,"+CMSG:")  == 0){
			PRINTF("Response matches\r\n");
			return 0;
		}

		 //return -2;
	}
}
void GSM_init(){
	int i = 0,y;

	char AT[] = "AT\r";
	char CIPSHUT[] = "AT+CIPSHUT\r";
	char CIPSMUX[] = "AT+CIPMUX=0\r";
	char CGATT[] = "AT+CGATT=1\r";
	char CSTT[] = "AT+CSTT=\"www\","",""\r";
	char CIICR[] = "AT+CIICR\r";
	char CIFSR[] = "AT+CIFSR\r";
	char CIPSTART[] = "AT+CIPSTART=\"TCP\",\"86.46.63.162\",\"27\"\r";


	char response[200];
	for(y=0;y<=sizeof(response);y++)
			response[i]='c';
	byteCountBuff = sizeof(AT);
	wait_time = 16000u;
	enum STATES {INIT,SHUT,MUX,ATT,STT,ICR,FSR,CONNECT};
	enum STATES CurrentState = INIT;
	switch(CurrentState){
				case INIT:	//Check connection to MODEM by sending AT. Expected response is OK
					recieve_size = 4;
					byteCountBuff = sizeof(AT);
					result = send_command(1,AT,byteCountBuff,wait_time,response,recieve_size);
					if(result == SUCCESS){
						//PRINTF("returned ATtt\r\n");//"OK" was returned by MODEM
						CurrentState = SHUT;
					}
					else
						break;
				case SHUT:	//Check connection to MODEM by sending AT. Expected response is OK
					recieve_size =10;
					byteCountBuff = sizeof(CIPSHUT);
					result = send_command(1,CIPSHUT,byteCountBuff,wait_time,response,recieve_size);
					if(result == SUCCESS){
						PRINTF("returned SHUT\r\n");//"OK" was returned by MODEM
						CurrentState = MUX;
					}
					else
						break;
				case MUX:	//Check connection to MODEM by sending AT. Expected response is OK
					recieve_size = 6;
					byteCountBuff = sizeof(CIPSMUX);
					result = send_command(1,CIPSMUX,byteCountBuff,wait_time,response,recieve_size);
					if(result == SUCCESS){
						PRINTF("returned MUX\r\n");//"OK" was returned by MODEM
						CurrentState = ATT;
					}
					else
						break;
				case ATT:	//Check connection to MODEM by sending AT. Expected response is OK
					recieve_size =6;
					byteCountBuff = sizeof(CGATT);
					result = send_command(1,CGATT,byteCountBuff,wait_time,response,recieve_size);
					if(result == SUCCESS){
						PRINTF("returned ATT\r\n");//"OK" was returned by MODEM
						CurrentState = STT;
					}
					else
						break;
				case STT:	//Check connection to MODEM by sending AT. Expected response is OK
					recieve_size =6;
					byteCountBuff = sizeof(CSTT);
					result = send_command(1,CSTT,byteCountBuff,wait_time,response,recieve_size);
					if(result == SUCCESS){
						PRINTF("returned STT\r\n");//"OK" was returned by MODEM
						CurrentState = ICR;
					}
					else
						break;
				case ICR:	//Check connection to MODEM by sending AT. Expected response is OK
					recieve_size =6;
					byteCountBuff = sizeof(CIICR);
					result = send_command(1,CIICR,byteCountBuff,wait_time,response,recieve_size);
					if(result == SUCCESS){
						PRINTF("returned ICR\r\n");//"OK" was returned by MODEM
						CurrentState = FSR;
					}
					else
						break;
				case FSR:	//Check connection to MODEM by sending AT. Expected response is OK
					recieve_size =18;
					byteCountBuff = sizeof(CIFSR);
					result = send_command(1,CIFSR,byteCountBuff,wait_time,response,recieve_size);
					if(result == SUCCESS){
						PRINTF("returned FSR\r\n");//"OK" was returned by MODEM
						CurrentState = CONNECT;
					}
					else
						break;
				case CONNECT:
					recieve_size =14;
						byteCountBuff = sizeof(CIPSTART);
						result = send_command(1,CIPSTART,byteCountBuff,wait_time,response,recieve_size);
						if(result == SUCCESS){
							PRINTF("SERVER CONNECTED\r\n");//"OK" was returned by MODEM

						}
				default:
					break;
			}//end switch-case

}
void update(){
	int i = 0,y;

	char SEND[] = "AT+CIPSEND\r";
	char MESSAGE[] = "HELLO AONGHUS KL26Z \x1A";


	char response[200];
	for(y=0;y<=sizeof(response);y++)
			response[i]='c';
	byteCountBuff = sizeof(SEND);
	wait_time = 16000u;
	enum STATES {SENDING,MSG};
	enum STATES CurrentState = SENDING;
	switch(CurrentState){
				case SENDING:	//Check connection to MODEM by sending AT. Expected response is OK
					recieve_size = 4;
					byteCountBuff = sizeof(SEND);
					result = send_command(1,SEND,byteCountBuff,wait_time,response,recieve_size);
					if(result == SUCCESS){
						//PRINTF("returned ATtt\r\n");//"OK" was returned by MODEM
						CurrentState = MSG;
					}
					else
						break;
				case MSG:	//Check connection to MODEM by sending AT. Expected response is OK
					recieve_size =7;
					byteCountBuff = sizeof(MESSAGE);
					result = send_command(1,MESSAGE,byteCountBuff,wait_time,response,recieve_size);
					if(result == SUCCESS){
						PRINTF("returned MSG\r\n");//"OK" was returned by MODEM
						//CurrentState = MUX;
					}
					else
						break;
				default:
					break;
			}//end switch-case

}
int main(void)
{
	uart_state_t uartState; // user provides memory for the driver state structure
	uart_user_config_t uartConfig;
	int y =0,i=0;
	char AT[] = "AT\r";
	char send[] = "AT+CIPSEND\r";
	char server_response[200];
		for(y=0;y<=sizeof(server_response);y++)
			server_response[i]='c';
    //Initialise the FRDM-KL26Z Board
	hardware_init();
	configure_uart_pins(0);	//instance 0 is UART1???
	OSA_Init();

	uartConfig.baudRate = 9600;
	uartConfig.bitCountPerChar = kUart8BitsPerChar;
	uartConfig.parityMode = kUartParityDisabled;
	uartConfig.stopBitCount = kUartOneStopBit;

	UART_DRV_Init(1, &uartState,&uartConfig);
	PRINTF("Full test of send/recieve on UART1\r\n");
	GSM_init();

    while(1){
    	if(i<3){
    		update();
    		i++;
    	}
    		//returnValue = UART_DRV_ReceiveData(0,server_response,recieve_size);//Buffer size in bytes to accept all of the me

//    		while (kStatus_UART_RxBusy == UART_DRV_GetReceiveStatus(0, NULL)){}
//
//    		if(returnValue == kStatus_UART_Success){
//    			PRINTF("server_response Recieved:\r\n");
//    			while(server_response[i] != '\0')
//    			{
//    				 PRINTF("%c",server_response[i]);
//    				 i++;
//    			}
//    			PRINTF("\r\n");
//
//    			if(strstr(server_response,"OK") || strstr(server_response,">") || strstr(server_response,"+CMSG:")  == 0){
//    				PRINTF("Response matches\r\n");
//    			}
//    		}

    }
    /* Never leave main */
    return 0;
}
////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////




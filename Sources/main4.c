/*
 * main3.c
 *
 *  Created on: 30 Dec 2015
 *  Author: Aonghus
 *  Additions: Added buffers.
 */

#include "board.h"
#include "fsl_clock_manager.h"
#include "fsl_debug_console.h"


#define RDRF_MASK 0x20	//Receive Data Register Full Flag Mask
#define RIE_MASK 0x20	//Receive Interrupt Enable Mask
#define TDRE_MASK 0x80u
#define BufferSize 20
//response codes for AT command function
#define SUCCESS 0
#define FAIL -1
#define ERROR -2
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
char char_received();
void enable_UART0_receive_interrupt();
char c;
char buffer[BufferSize];
char * wr_ptr;
char * rd_ptr;
volatile unsigned long tick_count;
//Function Prototypes
void enable_UART5_RX_interrupt();
void buffer_init();
char buffer_get();
void buffer_put(char data);
int send_command(char * command_ptr, char * response_ptr, int size, int wait_time);
int send_sms(char * number, char * message);
void printf_response(char * ptr);
/*******************************************************************************
 * Code
 ******************************************************************************/

/*
 * UART0 Interrupt Handler
 * Echos received character
 */
void UART0_IRQHandler(void)
{
	PRINTF("Recived: %c %d %i %x\r\n",c);
    if(UART0_S1 & RDRF_MASK)
    {
    	UART0_D=c;
    	buffer_put(c);
    }
}
void buffer_put(char data)
{
	*wr_ptr = data;
	if(wr_ptr == &buffer[BufferSize-1])
		wr_ptr = buffer;
	else
		wr_ptr++;

	if(wr_ptr == rd_ptr)
	{
		if(++rd_ptr == &buffer[BufferSize-1])
			rd_ptr = buffer;
	}
}
char buffer_get()
{
	char ch;
	if(rd_ptr != wr_ptr)
	{
		ch = *rd_ptr;
		if(rd_ptr == &buffer[BufferSize-1])
			rd_ptr = buffer;
		else
			rd_ptr++;
		return ch;
	}
	else
	{
		//printf("\nBuffer Empty\r\n");
		return 0;
	}
}


char char_received()
{
	if(UART0_S1 & RDRF_MASK)
	{
		return 1;
	}
	else
		{
		return 0;
		}
}


void enable_UART0_receive_interrupt()
{
	//Configure NVIC
	NVIC_ClearPendingIRQ(12);
	NVIC_EnableIRQ(12);
	UART0_C2 |= RIE_MASK;	//set RIE to enable receive interrupt
}
void buffer_init()
{
	char x;
	rd_ptr = buffer;
	wr_ptr = buffer;
	for(x=0; x<BufferSize; x++)
		buffer[x]=0;
}
void printf_response(char * ptr)
{
	while(*ptr != '\0')
	{
		PUTCHAR(*ptr);
		if(*ptr++ == '\r')
			PUTCHAR('\n');
	}
}
int send_command(char * command_ptr, char * response_ptr, int size, int wait_time)
{
	volatile int time = 0;
	char CR_count=0;
	int x, termination_char_received = 0, command_received=0;
	char c;
	char buff[20];
	char rx_count = 0;
	char *temp_ptr = response_ptr;

	for(x=0;x<size;x++)	//clear response buffer
		*temp_ptr++ = 0;
	for(x=0;x<20;x++)
		buff[x] = 0;
	//send command to serial port
	while(*command_ptr != '\0')
	{
		PUTCHAR(*command_ptr++);
	}
	//PRINTF("\r\nWaiting\r\n");
	//wait for a command response or a timeout
	time = tick_count;
	x=0;
	while( (!command_received) && (tick_count - time < wait_time) )
	{
		//PRINTF("\r\n inside response while %i",&wait_time);
		c = buffer_get();
		if(c)	//buffer not empty
		{
			//PRINTF("%c",c);
			*response_ptr++ = c;
			rx_count++;
			if(!termination_char_received)
			{
				if(strstr(response_ptr - rx_count, "OK") || strstr(response_ptr - rx_count, "ERROR:")
						|| strstr(response_ptr - rx_count, "READY") || strstr(response_ptr - rx_count, "SIM PIN")
						|| strstr(response_ptr - rx_count, "+CREG") )
				{
					PRINTF("\nOK or ERROR received\n");
					termination_char_received = 1;
				}
			}
			else if(c == '\r')
			{
				command_received = 1;
				//printf("\nResponse Received:  \n");
			}
		}
	}
PRINTF("Got out of while !!!!!");
	if(command_received)
	{
		PRINTF("Inside command recived\n");
		if(strstr(response_ptr - rx_count, "OK") || strstr(response_ptr - rx_count, "READY:"))
			return SUCCESS;
		else if(strstr(response_ptr - rx_count, "ERROR:"))
			return ERROR;
	}
	else
	{
		PRINTF("\nTimeout on serial response\n");
		return FAIL;
	}

}

int send_sms(char * number, char * message)
{
	char sms_send_command[40] = "AT+CMGS=";
	char * ptr;
	volatile int time = 0;
	char response_received = 0;
	char c;
	char response[20];
	int result = 0;
	char char_recived = 0;

	strcat(sms_send_command, number);
	strcat(sms_send_command, "\r");
	ptr = sms_send_command;

	while(*ptr != '\0')
	{
		PUTCHAR(*ptr);
		*ptr++;
	}
	time = tick_count;
	while( (char_recived == 0)  &&   ( tick_count - time ) < 5 )
	{
		c = buffer_get();
		if(c)
		{
			char_recived = 1;
		}
	}

	if((tick_count - time) >=5)	//buffer not empty
	{
		//printf("\nTime out\n");
		return FAIL;
	}
	else
	{
		if(c == '>')
		{
			char_recived = 1;
			memset(sms_send_command,'\0',sizeof(sms_send_command));
			strcat(sms_send_command, message);
			strcat(sms_send_command, "\x1A");
			ptr = sms_send_command;
			result = send_command(sms_send_command, response, sizeof(response), 20);
			if(result == SUCCESS || result == ERROR)
			{
				return result;
			}
		}
		else
		{
			printf("\n Major Error rebooting . . .\n");
			return ERROR;
		}
	}

}
void PIT_Configure_interrupt_mode(float time_delay_secs)
{
unsigned long reload_value = (unsigned long)(12000000 * time_delay_secs);
//Configure NVIC
NVIC_ClearPendingIRQ(22);
NVIC_EnableIRQ(22);
//Enable PIT peripheral clock
SIM_SCGC6 |= SIM_SCGC6_PIT_MASK;
//Configure PIT
PIT_MCR = 0x01ul; //PIT timer clock enabled
PIT_TCTRL0 = 0; //timer disabled and interrupt turned off
//tick_count++;
PIT_TFLG0 = 0x01ul; //Clear interrupt flag
PIT_LDVAL0 = reload_value; //load time value
PIT_TCTRL0 |= 0x03ul; //enable timer and interrupt
}
void PIT_IRQHandler()
{
	PIT_TFLG0 = 0x01ul;	//Clear interrupt flag
	PIT_TCTRL0 |= 0x03ul;	//enable timer and interrupt
	tick_count++;
}

int main()
{
	char * AT = "AT\r\n";//Setting up a char variable AT for the a long string
	char response[20]; //A character buffer called response that can hold 20 characters
	int result = 0;	//int result to check the value of the response sent back
	int transmit_send = 0;
	volatile int CurrentTick;	//Volitile interger to hold the current tick count of the current time
	char * ptr;	//Character
	char * stat;
	enum STATES {INIT};
	enum STATES CurrentState = INIT;
	buffer_init();
	hardware_init();
	enable_UART0_receive_interrupt();
	PIT_Configure_interrupt_mode(1);
	while(1)
	{
		switch(CurrentState)
		{
		case INIT:	//Check connection to MODEM by sending AT. Expected response is OK
			result = send_command(AT, response, sizeof(response), 2000);
			if(result == SUCCESS || result == ERROR)
			{
				printf_response(response);
			}
			if(result == SUCCESS)	//"OK" was returned by MODEM
				PRINTF("It Worked");
			else	//incorrect response or timeout. Delay and try again
			{
				CurrentTick = tick_count;
				while((tick_count - CurrentTick) < 5)
				{}
			}
			break;

		default:
			break;
		}//end switch-case
	}
}






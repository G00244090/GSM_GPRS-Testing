/*
 * MyUart1.c
 *
 *  Created on: 17 Jan 2016
 *      Author: Aonghus
 */




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
#include "fsl_device_registers.h"
#include "fsl_uart_driver.h"


#define RDRF_MASK 0x20	//Receive Data Register Full Flag Mask
#define RIE_MASK 0x20	//Receive Interrupt Enable Mask
#define TDRE_MASK 0x80u
#define BufferSize 80
//response codes for AT command function
#define SUCCESS 0
#define FAIL -1
#define ERROR -2
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
char char_received();
void enable_UART1_receive_interrupt();
char c;
char buffer[BufferSize];
char * wr_ptr;
char * rd_ptr;
volatile unsigned long tick_count;
//Function Prototypes
void buffer_init();
char buffer_get();
void buffer_put(char data);
int send_command(char * command_ptr, char * response_ptr, int size, int wait_time);
int send_sms(char * number, char * message);
/*******************************************************************************
 * Code
 ******************************************************************************/

/*
 * UART0 Interrupt Handler
 * Echos received character
 */
void UART1_IRQHandler(void)
{

	//PRINTF("Echo:");
    if(UART1_S1 & RDRF_MASK)
    {
    	    	//PRINTF("%c",c);
    	c = UART1_D;

    	PRINTF("Recived: %c %d %i\r\n",&c);

    	//PUTCHAR(c);
    	buffer_put(c);
    }
    //PRINTF("\r\n");
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
	if(UART1_S1 & RDRF_MASK)
	{
		return 1;
	}
	else
		{
		return 0;
		}
}

void buffer_init()
{
	char x;
	rd_ptr = buffer;
	wr_ptr = buffer;
	for(x=0; x<BufferSize; x++)
		buffer[x]=0;
}

int send_command(char * command_ptr, char * response_ptr, int size, int wait_time)
{
	PRINTF("\r\nInside send command\r\n");
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
	PRINTF("\r\nBuffers cleared, sending command \r\n");
	//send command to serial port
	while(*command_ptr != '\0')
	{
		//PUTCHAR(*command_ptr++);
		while((UART1_S1 & TDRE_MASK) == 0)//wait until tx buffer is empty
		{}
		UART1_D = *command_ptr++;

	}
	PRINTF("\r\nWaiting for a response command \r\n");
	//wait for a command response or a timeout
	time = tick_count;
	x=0;
	while( (!command_received) && (tick_count - time < wait_time) )
	{
		//PRINTF("\r\n inside response while %i",&wait_time);
		c = buffer_get();
		if(c)	//buffer not empty
		{
			*response_ptr++ = c;
			rx_count++;
			if(!termination_char_received)
			{
				if(strstr(response_ptr - rx_count, "Te") )
				{

					PRINTF("\nBloody Te again\n");
				}
				if(strstr(response_ptr - rx_count, "OK") || strstr(response_ptr - rx_count, "ERROR:")
						|| strstr(response_ptr - rx_count, "READY") || strstr(response_ptr - rx_count, "SIM PIN")
						|| strstr(response_ptr - rx_count, "+CREG") || strstr(response_ptr - rx_count, "Te") )
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

/*---------------------------------------------------------------------------
 * Function to send an SMS text message
 *
 * The function is passed 2 strings - the phone number and the text message
 * Text message must be terminated with Control Z (0x1A)
 * e.g. send_sms("\"+353876477260\"","\"this is a test\"");
 *
 */

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
	//Next step is needed because of a device error
	//See e2682 in errata for mask 0M33Z
	//Without this subsequent interrupts will not be generated
	PIT_TCTRL0 |= 0x03ul;	//enable timer and interrupt
	tick_count++;
	//PRINTF("\r\nISR TICK COUNT %D",&tick_count);
}

int main()
{

	char * AT = "\r\nAT\r\n";//Setting up a char variable AT for the a long string
	char * PIN_CHECK = "\r\nAT+CPIN?\r\n";//Setting up a char variable PIN_CHECK for the a long string
	char * ENTER_PIN = "\r\nAT+CPIN=\"1234\"\r\n";//Setting up a char variable ENTER_PIN for the a long string
	char * CREG = "\r\nAT+CREG?\r\n";//Setting up a char variable CREG for the a long string
	char response[20]; //A character buffer called response that can hold 20 characters
	int result = 0;	//int result to check the value of the response sent back
	int transmit_send = 0;
	volatile int CurrentTick;	//Volitile interger to hold the current tick count of the current time
	char * ptr;	//Character
	char * stat;
	enum STATES {INIT, CHECK_PIN, SEND_PIN, CHECK_NETWORK_REG, SEND_SMS, CONNECTED};
	enum STATES CurrentState = INIT;
	uart_state_t uartState; // user provides memory for the driver state structure
	uart_user_config_t uartConfig;
	buffer_init();
	hardware_init();
	//UART0_config();
	PIT_Configure_interrupt_mode(1);
	configure_uart_pins(0);	//instance 0 is UART1???
	// Call OSA_Init to setup LP Timer for timeout
	OSA_Init();


	uartConfig.baudRate = 9600;
	uartConfig.bitCountPerChar = kUart8BitsPerChar;
	uartConfig.parityMode = kUartParityDisabled;
	uartConfig.stopBitCount = kUartOneStopBit;

	UART_DRV_Init(1, &uartState,&uartConfig);
	//PRINTF("UART0 Test Code\n\r");
	//PRINTF("Any entered character will be echoed\r\n\n");
	while(1)
	{
		switch(CurrentState)
		{
		case INIT:	//Check connection to MODEM by sending AT. Expected response is OK
			printf("Testing Modem Connection\n");
			result = send_command(AT, response, sizeof(response), 2000);
			if(result == SUCCESS || result == ERROR)
			{
				//printf_response(response);
			}
			if(result == SUCCESS)	//"OK" was returned by MODEM
				CurrentState = CHECK_PIN;
			else	//incorrect response or timeout. Delay and try again
			{
				CurrentTick = tick_count;
				while((tick_count - CurrentTick) < 5)
				{}
			}
			break;

		case CHECK_PIN:	//Check if SIM card is ready
			result = send_command(PIN_CHECK, response, sizeof(response), 10);
			if(result == SUCCESS || result == ERROR)
			{
				//printf_response(response);
			}
			if(result == SUCCESS)	//"OK" returned, check response string for "READY" or "SIM_PIN"
			{
				if(strstr(response, "READY"))
				{
					CurrentState = CHECK_NETWORK_REG;
				}
				else if(strstr(response, "SIM PIN"))
				{
					CurrentState = SEND_PIN;
				}
			}
			else
				CurrentState = INIT;
			break;

		case SEND_PIN:	//Send PIN code. "OK" response if PIN is correct
			result = send_command(ENTER_PIN, response, sizeof(response),10);
			if(result == SUCCESS || result == ERROR)
			{
				//printf_response(response);
			}
			if(result == SUCCESS)	//"OK" returned, check response string for "READY" or "SIM_PIN"
			{
				CurrentState = CHECK_NETWORK_REG;
			}
			else
				CurrentState = INIT;
			break;


		case CHECK_NETWORK_REG:	//check if registered on mobile network
			result = send_command(CREG, response, sizeof(response), 20);
			if(strstr(response, "+CREG"))
			{
				stat = (char *)strstr(response,":");
				stat += 4;
				switch(*stat)
				{
				case '0':
					CurrentState = INIT;
					break;
				case '1':
					CurrentState = SEND_SMS;
					break;
				case '2':
					CurrentTick = tick_count;
					while((tick_count - CurrentTick) < 5)
					{}
					CurrentState = CHECK_NETWORK_REG;
					break;
				case '3':
					CurrentState = INIT;
					break;
				case '4':
					CurrentState = CONNECTED;
					break;
				case '5':
					CurrentState = SEND_SMS;
					break;

				}
			}


		case SEND_SMS:	//Send a text message
			transmit_send = send_sms("\"0877763894\"","\"Testing 123		\"");
			if(transmit_send == SUCCESS)
			{
				CurrentState = CONNECTED;
			}
			else if(transmit_send == FAIL)
			{
				printf("A transmission fail has been detected or you have timed out\r\n");
				CurrentState = SEND_SMS;
			}
			else if(transmit_send == ERROR)
			{
				printf("A transmission ERROR has been detected,rebooting\r\n");
				CurrentState = INIT;
			}
			break;

		case CONNECTED:
			printf("\nInside Connected \r\n");
			while(1)	//dummy loop
			{}
			break;

		default:
			break;
		}//end switch-case
	}
}






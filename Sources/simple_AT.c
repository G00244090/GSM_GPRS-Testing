#include "board.h"
#include "fsl_clock_manager.h"
#include "fsl_debug_console.h"


#define RDRF_MASK 0x20	//Receive Data Register Full Flag Mask
#define RIE_MASK 0x20	//Receive Interrupt Enable Mask
#define TDRE_MASK 0x80u

void send(char * command_ptr);


void send(char * command_ptr)
{
	//PRINTF("\r\nInside send command\r\n");
	while(*command_ptr != '\0')
	{
		PUTCHAR(*command_ptr++);
		while((UART0_S1 & TDRE_MASK) == 0) //**** SHOULD BE ZERO wait until tx buffer is empty
		{}
		UART0_D = *command_ptr++;
	}
}
int main()
{
	char * AT = "\r\nAT\r\n";//Setting up a char variable AT for the a long string
	hardware_init();
	send(AT);
	while(1)
	{}
}






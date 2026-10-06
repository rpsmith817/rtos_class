//sets the psp, sets the ASP bit, and tests several interrupts for memory dump ability.
//targets the tm4c123gh6pm for the ek-tm4c123ghx tiva board
//relies on professor Losh's clock and uart code, taken from an embedded I project.
//  also on gpio control code from iot. Probably from some other assignments too.
//author: Ryan Smith 9/22/2026


//pin targets:
//UART0 -> PA0+PA1
//BTNS -> PC4, PC5, PC6, PC7, PD6, PA3, PA4,
//LEDS -> PE4, PE3, PE2, PE1, from bottom up.


//includes

#include "tm4c123gh6pm.h"	//main header for target hw
#include "cti.h"			//common terminal interface
#include "clock.h"			//losh file that sets clock to 40mhz
#include "cti.h"            //terminal interface/shell
#include "rtos_common.h"    //rtos common functions
#include "debug.h"          //leds and other helpers
#include "asp_bit.h"        //some assembly functions like setASPBit(), also contains psp
#include "memory.h"         //mpu and other memory functions
#include "gpio.h"           //gpio control code from losh from iot, for buttons and leds.

#include "interruptor.h"    //debug/test code incl faultloop function.


//all hardware inits
void init_hw(void)
{
	initSystemClockTo40Mhz();   //setup system clock to 40MHz

	initUart0();    //set up the USB UART0 on PA0 + PA1

//setup for buttons etc, code courtesy of losh from iot.
	//enable required ports
	enablePort(PORTA);
	enablePort(PORTC);
	enablePort(PORTD);
	enablePort(PORTE);
	enablePort(PORTF);

	//setup buttons
	selectPinDigitalInput(PORTC,4);
    enablePinPullup(PORTC,4);
	selectPinDigitalInput(PORTC,5);
    enablePinPullup(PORTC,5);
	selectPinDigitalInput(PORTC,6);
    enablePinPullup(PORTC,6);
	selectPinDigitalInput(PORTC,7);
    enablePinPullup(PORTC,7);
	selectPinDigitalInput(PORTD,6);
    enablePinPullup(PORTD,6);
	selectPinDigitalInput(PORTA,3);
    enablePinPullup(PORTA,3);
	selectPinDigitalInput(PORTA,4);
    enablePinPullup(PORTA,4);

    //setup LEDs
    selectPinPushPullOutput(PORTE,4);
    selectPinPushPullOutput(PORTE,3);
    selectPinPushPullOutput(PORTE,2);
    selectPinPushPullOutput(PORTE,1);
    selectPinPushPullOutput(PORTF,1);
    selectPinPushPullOutput(PORTF,2);
    selectPinPushPullOutput(PORTF,3);

    return;
}

//loop for the PSP area.
void ecscape()
{
    faultloop();//perform tests of interrupts

    while(1);   //we live here now.
}

//enable fault handlers where needed
void setup_handlers()
{
    NVIC_SYS_HND_CTRL_R |= (NVIC_SYS_HND_CTRL_USAGE | NVIC_SYS_HND_CTRL_BUS/* | NVIC_SYS_HND_CTRL_MEM*/);
}

//main
int main(void)
{
	init_hw();	        //start up all hardware
	init_mem();         //setup the memory map
	setup_handlers();   //enable fault handling where required.
	setpsp((void*)HEAP_AT);       //set the psp bit to whatever it should be.
	setASPBit();        //set the ASP bit so that we get into program stack

	ecscape();          //go live in PSP land.

//DEBUG:
//    helpMe();       //debug command to list out cti functions.

//    shell();    //cycle shell forever.

}


void yield(void){
	//empty per assignment
}

//sets the psp, sets the ASP bit, and tests several interrupts for memory dump ability.
//targets the tm4c123gh6pm for the ek-tm4c123ghx tiva board
//relies on professor Losh's clock and uart code, taken from an embedded I project.
//  also on gpio control code from iot. Probably from some other assignments too.
//author: Ryan Smith 9/22/2026


//pin targets:
//UART0 -> PA0+PA1
//BTNS -> PC4, PC5, PC6, PC7, PD6, PD7, PF4
//LEDS -> PF1, PE3, PE2, PE1


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

//global define for the psp, but global defines are dirty and wrong. except this one.
uint32_t psp= 0x20008000;

//all hardware inits
void init_hw(void)
{
	initSystemClockTo40Mhz();
	initUart0();

//setup for buttons etc, code courtesy of losh from iot.
	//enable required ports
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
	selectPinDigitalInput(PORTD,7);
    enablePinPullup(PORTD,7);
	selectPinDigitalInput(PORTF,4);
    enablePinPullup(PORTF,4);

    //setup LEDs
    selectPinPushPullOutput(PORTF,1);
    selectPinPushPullOutput(PORTE,3);
    selectPinPushPullOutput(PORTE,2);
    selectPinPushPullOutput(PORTE,1);

}

//loop for the PSP area.
ecscape()
{
    faultloop();    //perform tests of interrupts

    while(1);   //we live here now.
    return;     //we will never return.
}

//main
int main(void)
{
	init_hw();	    //start up all hardware
	setpsp(psp);    //set the psp bit to whatever it should be.
	setASPBit();    //set the ASP bit so that we get into program stack

	ecscape();      //go live in PSP land.

//DEBUG:
//    helpMe();       //debug command to list out cti functions.

//    shell();    //cycle shell forever.

}


void yield(void){
	//empty per assignment
}

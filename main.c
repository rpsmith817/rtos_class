//sets the psp, sets the ASP bit, and tests several interrupts for memory dump ability.
//targets the tm4c123gh6pm for the ek-tm4c123ghx tiva board
//relies on professor Losh's clock and uart code, taken from an embedded I project.
//author: Ryan Smith 9/22/2026


//includes

#include "tm4c123gh6pm.h"	//main header for target hw
#include "cti.h"			//common terminal interface
#include "clock.h"			//losh file that sets clock to 40mhz
#include "cti.h"            //terminal interface/shell
#include "rtos_common.h"    //rtos common functions
#include "debug.h"          //leds and other helpers
#include "asp_bit.h"        //some assembly functions like setASPBit(), also contains psp


//global define for the psp.
uint32_t psp= 0x20008000;

//all hardware inits
void init_hw(void)
{
	initSystemClockTo40Mhz();
	initUart0();
	init_LEDs();
}

//main
int main(void)
{
	init_hw();	    //start up all hardware
	setpsp(psp);    //set the psp bit to whatever it should be.
	setASPBit();    //set the ASP bit so that we get into program stack

//DEBUG:
//	helpMe();       //debug command to list out cti functions.

	shell();	//cycle shell forever.

	return 0;	//never gonna get here
}


void yield(void){
	//empty per assignment
}

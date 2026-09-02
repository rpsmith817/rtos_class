//small shell by uart
//targets the tm4c123gh6pm for the ek-tm4c123ghx tiva board
//relies on professor Losh's clock code.
//author: Ryan Smith



//----includes

#include "tm4c123gh6pm.h"	//main header for target hw
#include "cti.h"			//common terminal interface
#include "clock.h"			//losh file that sets clock to 40mhz
#include "cti.h"            //terminal interface/shell
#include "rtos_common.h"    //rtos common functions
#include "debug.h"          //leds and other helpers


//all hardware inits
void init_hw(void)
{
	initSystemClockTo40Mhz();
	initUart0();
	init_LEDs();
}

//------main
int main(void)
{
	init_hw();	//start up all hardware

	shell();	//cycle shell forever.

	return 0;	//never gonna get here
}


void yield(void){
	//empty per assignment
}

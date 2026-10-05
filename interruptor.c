//defines functions that cause faults on purpose, for fun!
//author ryan smith 9/22/2026


#include <stdint.h>
#include <stdbool.h>
#include "interruptor.h"    //prototypes and some macros
#include "tm4c123gh6pm.h"   //main header for target hw
#include "gpio.h"           //buttons
#include "interrupts.h"     //makehardfault association.
#include "wait.h"           //waitMicrosecond

uint8_t makehardfault=0;


//hardfault -> we will cause a usagefault in usagefault by setting a global variable to true
void cause_hardfault(void)
{
    makehardfault=1;        //set flag that sets the usagefault handler to make another fault

    cause_usagefault();       //then make a usage fault.

}

//mpufault -> gotta set a protected area first.
void cause_mpufault(void)
{


    return;
}

//busfault
void cause_busfault(void)
{
    //one method of bus fault is to access a closed port
//    disablePort(PORTB);
    getPinValue(PORTB,1);
}

//usagefault
void cause_usagefault(void)
{
    uint8_t val =0;
    uint8_t i=0;
    val=5;
    i=1;
    while(1)
    {
        val/=--i;
    }
}

//pendsv not fault
void cause_pendsv(void)
{
    //this should be a simple register set so that service is pending. I wonder what will live here.
    NVIC_INT_CTRL_R |= NVIC_INT_CTRL_PEND_SV;

}


//cause faults by checking for button presses.
void faultloop(void)
{
    volatile uint8_t buttons=0;


    //setup for the fault creation
    NVIC_CFG_CTRL_R |= (1<<4);  //the fifth bit in the --ctl register is the div0 bit, set high and we trap on divide by 0. Losh said to set this bit and then cause a div0 fault.

    while(1)
    {
        //check if a button is pressed.
        //BTNS -> PC4, PC5, PC6, PC7, PD6, PA3, PA4
        buttons = (!getPinValue(PORTC,4) <<6) + (!getPinValue(PORTC,5) <<5) + (!getPinValue(PORTC,6) <<4) + (!getPinValue(PORTC,7) <<3) + (!getPinValue(PORTD,6) <<2) + (!getPinValue(PORTA,3) <<1) + (!getPinValue(PORTA,4) <<0);
        if(buttons)
        {
            waitMicrosecond(150000); //wait a little while to debounce the button.

            switch(buttons)
            {
            case (1<<6):    //hardfault
                cause_hardfault();
                break;
            case (1<<5):    //mpufault
                cause_mpufault();
                break;
            case (1<<4):    //busfault
                cause_busfault();
                break;
            case (1<<2):    //usagefault
                cause_usagefault();
                break;
            case (1<<1):    //pendsv
                cause_pendsv();
                break;
            case (1<<0):    //tbd
                BB_LEDB =1;                 //turn on the TIVA blue LED
                break;
            case (1<<3):    //tbd
                BB_LEDG =1;                 //turn on the TIVA green LED
                break;
            }

            buttons=0;
        }
    }
}

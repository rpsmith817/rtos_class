/*
 * Things to assist debugging, but right now just the one function.
 * Ryan Smith
 *
 *
 */

#include <stdint.h>
#include <stdbool.h>
#include "clock.h"          //clock init
#include "wait.h"           //waitMicrosecond()
#include "tm4c123gh6pm.h"   //hardware register and mask macros


//function to take 32bit val and parse out to ascii version of hex string.
char* toAsciiHex(char* buff, uint32_t Val){
    int i;  //we leave this signed due to comparison to 0 compiler warning and occasional weird overflow/wraparound error.
    uint8_t Hexbits=0;
    for(i=7;i>=0;i--){
        Hexbits = Val % 16;
        if(Hexbits>9)
            buff[i] = Hexbits+55;
        else
            buff[i] = Hexbits+48;
        Val /= 16;
    }

    buff[8] = '\0';

    return buff;
}


//***systick stuff ***//

//initialize the systick. Like it says on the box. You will still need to create a systemTicker variable in main.c
void init_sysTick(){
    NVIC_ST_CTRL_R =0;      //shut it down to change settings/clear settings
    NVIC_ST_RELOAD_R = 39999;  //COUNTING FROM 0 TO 39999, LIKE FROM 00:00 TO 12:59
    NVIC_ST_CURRENT_R = 0;
    NVIC_ST_CTRL_R = 7; //use sysclk, enable interrupt, and enable systick clk.
}
uint32_t systemTicker, iamatime, iamalsoatime, iamaninterval=0;




//give the current time in ms
uint32_t currentTime(){
    return systemTicker;
}

//return the difference between two times
uint32_t deltaTime(uint32_t t0, uint32_t t1){
    return t0-t1;
}

//systick ISR, which continuously counts milliseconds.
void SysTick_Handler(){
    systemTicker++;
    iamatime=systemTicker;
    iamaninterval = deltaTime(iamatime,iamalsoatime);
    if(iamaninterval< 500)
        redLED(1);
    if(iamaninterval > 500)
        redLED(0);
    if(iamaninterval > 1000)
        iamalsoatime=iamatime;
}

//**LED Control Stuff**//

//bitbanding addresses for the color of LED they are named.
#define RED_BB  (*((volatile uint32_t *)(0x42000000 + ((0x400253FC - 0x40000000) *32) + (1 *4))))//PF1
#define BLUE_BB (*((volatile uint32_t *)(0x42000000 + ((0x400253FC - 0x40000000) *32) + (2 *4))))//PF2
#define GREEN_BB (*((volatile uint32_t *)(0x42000000 + ((0x400253FC - 0x40000000) *32) + (3 *4))))//PF3

#define REDMASK 0x02        //the pin here is PF1.  PF definition is (sw1, g, b, r, sw2), so our value should be 0 0010 in binary or the value it is rn in hex
#define BLUEMASK 0x04       //pin here is PF2. value should be 0 0100 in binary or the value it is rn in hex
#define GREENMASK 0x08      //pin here is pf3. that is 0 1000 in binary, or the value it is rn in hex.

//to use the hardware we have to tell the device what we want. See page 656 of the tm4 datasheet for detailed steps.
//mainly this is for LED setup
void init_LEDs()
{
    //init gpio clock for portf
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5; //R5 is the reserved name for portf per datasheet.
    _delay_cycles(3);                        //datasheet says you need 3 clock cycles prior to gpio being able to r/w

    //GPIODIR is next.
    GPIO_PORTF_DIR_R |= 0x0E; //sets LED pins to outputs.

    //next would be configure GPIOAFSELto program each bit as a GPIO or alternate. It should default GPIO but we will revisit if it blows up.
    //then we would set the drive strength. By default it is 2mA, we should be OK here because blinky worked.

    //now we enable pins we want.
    GPIO_PORTF_DEN_R |= REDMASK|BLUEMASK|GREENMASK;   //to 1 something we or //to 0 something we and not
    //there are more optional steps we are skipping.
}

//uses bitbanding to turn on/off led
void blueLED(bool state){
    BLUE_BB = state;
}

//uses bitbanding to turn on/off led
void redLED(bool state){
    RED_BB = state;
}

//uses bitbanding to turn on/off led
void greenLED(bool state){
    GREEN_BB=state;
}

//combined debug init
void init_debug(){
    init_sysTick();
    init_LEDs();
}

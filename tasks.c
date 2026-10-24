// Tasks
// J Losh

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target uC:       TM4C123GH6PM
// System Clock:    40 MHz

//-----------------------------------------------------------------------------
// Device includes, defines, and assembler directives
//-----------------------------------------------------------------------------

#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "gpio.h"
#include "wait.h"
#include "kernel.h"
#include "tasks.h"
#include "mm.h"

// REQUIRED: Change ports as needed below for your LEDs
#define BLUE_LED   PORTF,2 // on-board blue LED
#define RED_LED    PORTF,7 // off-board red LED
#define ORANGE_LED PORTF,7 // off-board orange LED
#define YELLOW_LED PORTF,7 // off-board yellow LED
#define GREEN_LED  PORTF,7 // off-board green LED

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

// Initialize Hardware
// REQUIRED: Add initialization for blue, orange, red, green, and yellow LEDs
//           Add initialization for 6 pushbuttons
void initHw(void)
{
    // Setup LEDs and pushbuttons

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

    // Power-up flash
    setPinValue(GREEN_LED, 1);
    waitMicrosecond(250000);
    setPinValue(GREEN_LED, 0);
    waitMicrosecond(250000);
}

// REQUIRED: add code to return a value from 0-127 indicating which of 7 PBs are pressed
uint8_t readPbs(void)
{
    return ((!getPinValue(PORTC,4) <<6) + (!getPinValue(PORTC,5) <<5) + (!getPinValue(PORTC,6) <<4) + (!getPinValue(PORTC,7) <<3) + (!getPinValue(PORTD,6) <<2) + (!getPinValue(PORTA,3) <<1) + (!getPinValue(PORTA,4) <<0));
}

// one task must be ready at all times or the scheduler will fail
// the idle task is implemented for this purpose                    //when you go to idle for the first time, pop 16 words off stack, call the first task.
void idle(void)
{
    while(true)
    {
        setPinValue(ORANGE_LED, 1);
        waitMicrosecond(1000);
        setPinValue(ORANGE_LED, 0);
        yield();
    }
}

void flash4Hz(void)
{
    while(true)
    {
        setPinValue(GREEN_LED, !getPinValue(GREEN_LED));
        sleep(125);
    }
}

void oneshot(void)
{
    while(true)
    {
        wait(flashReq);
        setPinValue(YELLOW_LED, 1);
        sleep(1000);
        setPinValue(YELLOW_LED, 0);
    }
}

void partOfLengthyFn(void)
{
    // represent some lengthy operation
    waitMicrosecond(990);
    // give another process a chance to run
    yield();
}

void lengthyFn(void)
{
    uint16_t i;
    uint8_t *mem;
    mem = malloc_from_heap(5000 * sizeof(uint8_t));
    while(true)
    {
        lock(resource);
        for (i = 0; i < 5000; i++)
        {
            partOfLengthyFn();
            mem[i] = i % 256;
        }
        setPinValue(RED_LED, !getPinValue(RED_LED));
        unlock(resource);
    }
}

void readKeys(void)
{
    uint8_t buttons;
    while(true)
    {
        wait(keyReleased);
        buttons = 0;
        while (buttons == 0)
        {
            buttons = readPbs();
            yield();
        }
        post(keyPressed);
        if ((buttons & 1) != 0)
        {
            setPinValue(YELLOW_LED, !getPinValue(YELLOW_LED));
            setPinValue(RED_LED, 1);
        }
        if ((buttons & 2) != 0)
        {
            post(flashReq);
            setPinValue(RED_LED, 0);
        }
        if ((buttons & 4) != 0)
        {
            restartThread(flash4Hz);
        }
        if ((buttons & 8) != 0)
        {
            killThread(flash4Hz);
        }
        if ((buttons & 16) != 0)
        {
            setThreadPriority(lengthyFn, 4);
        }
        yield();
    }
}

void debounce(void)
{
    uint8_t count;
    while(true)
    {
        wait(keyPressed);
        count = 10;
        while (count != 0)
        {
            sleep(10);
            if (readPbs() == 0)
                count--;
            else
                count = 10;
        }
        post(keyReleased);
    }
}

void uncooperative(void)
{
    while(true)
    {
        while (readPbs() == 64)
        {
        }
        yield();
    }
}

void errant(void)
{
    uint32_t* p = (uint32_t*)0x20000000;
    while(true)
    {
        while (readPbs() == 32)
        {
            *p = 0;
        }
        yield();
    }
}

void important(void)
{
    while(true)
    {
        lock(resource);
        setPinValue(BLUE_LED, 1);
        sleep(1000);
        setPinValue(BLUE_LED, 0);
        unlock(resource);
    }
}

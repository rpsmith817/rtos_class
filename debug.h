/*
 * debug helper header.
 * Ryan Smith
 *
 */
#ifndef DEBUG_H_
#define DEBUG_H_

#include <stdint.h>
#include <stdbool.h>

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

//function to take 32bit val and parse out to ascii version of hex string.
//the given string's index size must be 9 or greater.
char* toAsciiHex(char* buff, uint32_t Val);

//init sysTick and LED controls for debugging purposes.
void init_debug();

//init systick hw
void init_sysTick();

//systick ISR, which continuously counts milliseconds.
void SysTick_Handler();

//give the current time in ms
uint32_t currentTime();

//return the difference between two times
uint32_t deltaTime(uint32_t t0, uint32_t t1);

//init GPIO hw, mostly for LEDs. Port F.
void init_LEDs();

//uses bitbanding to turn on/off led
void blueLED(bool state);

//uses bitbanding to turn on/off led
void redLED(bool state);

//uses bitbanding to turn on/off led
void greenLED(bool state);

#endif

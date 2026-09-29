/*
 * Things to assist debugging, but right now just the one function.
 * Ryan Smith
 *
 *
 */

#include <stdint.h>
#include "debug.h"
#include "tm4c123gh6pm.h"   //hardware register and mask macros


//function to take 32bit val and parse out to ascii version of hex string.
void toAsciiHex(char* buff, uint32_t Val){
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

    return;
}


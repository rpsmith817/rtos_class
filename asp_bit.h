//define assembly functions and the psp + setter func for the psp

#ifndef ASP_BIT_H
#define ASP_BIT_H

#include <stdint.h>


extern uint32_t psp;

//set the psp.
void setpsp(uint32_t * p);

//functions defines in asp_bit.s
extern void setASPBit(void);            //set the ASP bit, don't call unless you have already set the psp
extern void getpsp(uint32_t *p);        //return the psp address
extern void getmsp(uint32_t *p);        //return mps address


#endif

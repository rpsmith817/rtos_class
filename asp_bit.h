//define assembly functions and the psp + setter func for the psp

#ifndef ASP_BIT_H
#define ASP_BIT_H

#include <stdint.h>


extern uint32_t psp;

//set the psp.
void setpsp(uint32_t * p);

//functions defines in asp_bit.s
extern void setASPBit(void);
extern uint32_t getR0(void);
extern uint32_t getR1(void);
extern uint32_t getR2(void);
extern uint32_t getR3(void);
extern uint32_t getR7(void);


#endif

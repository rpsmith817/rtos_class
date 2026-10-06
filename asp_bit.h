//define assembly functions and the psp + setter func for the psp

#ifndef ASP_BIT_H
#define ASP_BIT_H

#include <stdint.h>


//functions defines in asp_bit.s
extern void setASPBit(void);            //set the ASP bit, don't call unless you have already set the psp
extern void setTmpl(void);              //go to unprivileged mode now pls (per Losh board)
extern void setpsp(uint32_t *p);        //set the PSP
extern void getpsp(uint32_t *p);        //return the psp address
extern void getmsp(uint32_t *p);        //return mps address
extern void memBarrier(void);           //dsb/isb per p.127+p.134

#endif

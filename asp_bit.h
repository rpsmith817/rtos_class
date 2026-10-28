//define assembly functions and the psp + setter func for the psp

#ifndef ASP_BIT_H
#define ASP_BIT_H

#include <stdint.h>


//functions defines in asp_bit.s
extern void setASPBit(void);            //set the ASP bit, don't call unless you have already set the psp
extern void setTmpl(void);              //go to unprivileged mode now pls (per Losh board)
extern void setpsp(uint32_t *p);        //set the PSP
extern uint32_t* getpsp(void);          //return the psp address as pointer
extern uint32_t* getmsp(void);          //return mps address as pointer
extern void memBarrier(void);           //dsb/isb per p.127+p.134
extern void asm_pop(void);              //pop into R0, but don't return R0 so just pop
extern uint32_t* asm_pull(void);        //pull r4:r11 into pointer.


//svc handlers want an imm, so we will call each number as a straight call with imm rather than work out how to get the arg into there or read the arg later in svc handler
extern void asm_svc_1(void);            //call svc with number 1
extern void asm_svc_2(void);            //call svc with number 2
extern void asm_svc_3(void);            //call svc with number 3
extern void asm_svc_4(void);            //call svc with number 4
extern void asm_svc_5(void);            //call svc with number 5
extern void asm_svc_6(void);            //call svc with number 6
extern void asm_svc_7(void);            //call svc with number 7
extern void asm_svc_8(void);            //call svc with number 8

#endif

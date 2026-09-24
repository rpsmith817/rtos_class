//header for interrupt handlers
//author ryan smith

#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#define FAULT_FLAG_MASK 0x030FBFBB
#define DERR_IERR_FLAG_MASK 0x3

#include "tm4c123gh6pm.h"

typedef struct _memdump{
    uint32_t * ptr_psp;
    uint32_t* ptr_msp;
    uint32_t pid;
    uint32_t fault_flags;
} memdump;


//get registers and put them into a struct for function use. spit out the values that are commonly spat out while we are at it.
memdump dumpMem(void);

//hardfault
void HardFaultHandler(void);

//mpu
void MPUFaultHandler(void);

//bus fault
void BusFaultHandler(void);

//usage fault
void UsageFaultHandler(void);

//pendsv
void PendSVHandler(void);


#endif

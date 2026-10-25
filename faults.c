// Shell functions
// J Losh

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target uC:       TM4C123GH6PM
// System Clock:    40 MHz

//-----------------------------------------------------------------------------
// Device includes, defines, and assembler directives and structs and maybe more, I'll never tell.
//-----------------------------------------------------------------------------

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "faults.h"
#include "asp_bit.h"
#include "debug.h"          //toasciihex
#include "gpio.h"           //leds
#include "wait.h"           //waitMicroseconds
#include "kernel.h"
#include "cti.h"

extern uint32_t current_pid;

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

// REQUIRED: code this function
// REQUIRED: add hooks to exception handlers in startup ccs file
void initFaultHandlers(void)
{
    NVIC_SYS_HND_CTRL_R |= (NVIC_SYS_HND_CTRL_USAGE | NVIC_SYS_HND_CTRL_BUS | NVIC_SYS_HND_CTRL_MEM);

    //setup for the fault creation
//    NVIC_CFG_CTRL_R |= (1<<4);  //the fifth bit in the --ctl register is the div0 bit, set high and we trap on divide by 0. Losh said to set this bit and then cause a div0 fault.
}

// REQUIRED: If these were written in assembly
//           add a faults.s file


// REQUIRED: code this function
void mpuFaultIsr(void)
{
/*  If an MPU exception is invoked, display “MPU fault in thread PID”, where PID will be a variable provided
by the OS. Or now, just use a variable named pid. Also, provide the value of the PSP, MSP, and mfault
flags (in hex). Also, print the offending instruction and data addresses. Display the process stack dump
(xPSR, PC, LR, R0-3, R12. Clear the MPU fault pending bit and trigger a pendsv ISR call.
*/
    char str[BUFFER_STR_SIZE];

    memdump *m={0};

    putsUart0("MPU fault in thread ");
    intToAlpha(current_pid,(char*)str);
    putsUart0((char*)str);
    putsUart0("\r\n");

    dumpmem((char*)str,m);    //dump the memory.

    m->fault_flags = NVIC_FAULT_ADDR_R & MFAULT_MASK;

    putsUart0("Fault Flags: ");
    toAsciiHex((char*)str,(m->fault_flags));
    putsUart0((char*)str);
    putsUart0("\r\n");

    //clear the mpu fault pending flag bitwise
    NVIC_SYS_HND_CTRL_R &= ~(1<<13);


    //set pendsv flag
    NVIC_INT_CTRL_R |= NVIC_INT_CTRL_PEND_SV;
}

//dump memory, armv7m ref manual p.B1-532, arm dui 0553a p.2-27
void dumpmem(char* str, memdump* m)
{
    getpsp(m->ptr_psp);
    getmsp(m->ptr_msp);
    m->fault_flags = NVIC_FAULT_STAT_R & FAULT_FLAG_MASK; //load the fault status register into the dump

    putsUart0("PSP: ");
    toAsciiHex((char*)str,(uint32_t)m->ptr_psp); // cast psp ptr as an int then convert to ascii hex.
    putsUart0((char*)str);                     //then print it out.
    putsUart0("\r\n");

    putsUart0("MSP: ");
    toAsciiHex((char*)str,(uint32_t)m->ptr_msp);
    putsUart0((char*)str);
    putsUart0("\r\n");

    putsUart0("XPSR: ");
    toAsciiHex((char*)str,m->ptr_psp[7]); //convert xpsr to hex.
    putsUart0((char*)str);
    putsUart0("\r\n");

    putsUart0("PC: ");
    toAsciiHex((char*)str,m->ptr_psp[6]); //convert PC to hex.
    putsUart0((char*)str);
    putsUart0("\r\n");

    putsUart0("LR: ");
    toAsciiHex((char*)str,m->ptr_psp[5]); //convert LR to hex.
    putsUart0((char*)str);
    putsUart0("\r\n");

    putsUart0("R0: ");
    toAsciiHex((char*)str,m->ptr_psp[0]); //convert R0 to hex.
    putsUart0((char*)str);
    putsUart0("\r\n");

    putsUart0("R1: ");
    toAsciiHex((char*)str,m->ptr_psp[1]); //convert R1 to hex.
    putsUart0((char*)str);
    putsUart0("\r\n");

    putsUart0("R2: ");
    toAsciiHex((char*)str,m->ptr_psp[2]); //convert R2 to hex.
    putsUart0((char*)str);
    putsUart0("\r\n");

    putsUart0("R3: ");
    toAsciiHex((char*)str,m->ptr_psp[3]); //convert R3 to hex.
    putsUart0((char*)str);
    putsUart0("\r\n");

    putsUart0("R12: ");
    toAsciiHex((char*)str,m->ptr_psp[4]); //convert R12 to hex.
    putsUart0((char*)str);
    putsUart0("\r\n");

}

// REQUIRED: code this function
void hardFaultIsr(void)
{
/*   If a hard fault exception is invoked, display “Hard fault in thread PID”, where PID will be a variable
    provided by the OS. Or now, just use a variable named pid. Also, provide the value of the PSP, MSP,
    and all fault flags (in hex). Also, print the offending instruction. Display the process stack dump (xPSR,
    PC, LR, R0-3, R12.*/

    //setup useful vars
    memdump *m={0};
    char str[BUFFER_STR_SIZE];


    //print out alerts
    putsUart0("Hard fault in thread");
    intToAlpha(current_pid,(char*)str);
    putsUart0((char*)str);
    putsUart0("\r\n");

    dumpmem((char*)str, m);   //dump the memory

    putsUart0("Fault Flags: ");
    toAsciiHex((char*)str,(m->fault_flags));
    putsUart0((char*)str);
    putsUart0("\r\n");

    while(1){}
}

// REQUIRED: code this function
void busFaultIsr(void)
{
/*  If a bus fault exception is invoked, display “Bus fault in thread PID”, where PID will be a variable provided
by the OS. Or now, just use a variable named current_pid.
*/
    char str[BUFFER_STR_SIZE];

    putsUart0("Bus fault in thread ");
    intToAlpha(current_pid,(char*)str);
    putsUart0((char*)str);
    putsUart0("\r\n");

    while(1){}
}

// REQUIRED: code this function
void usageFaultIsr(void)
{
/*  If a usage fault exception is invoked, display “Usage fault in thread PID”, where PID will be a variable
provided by the OS. Or now, just use a variable named current_pid.
*/
    char str[BUFFER_STR_SIZE];


    putsUart0("Usage fault in thread ");
    intToAlpha(current_pid,(char*)str);
    putsUart0((char*)str);
    putsUart0("\r\n");

//    if(makehardfault)
//    {
//        str[0]=1;
//        str[1]=2;
//        while(1)
//        {
//            str[0]-=1;
//            str[1] /= str[0];
//        }
//    }

    while(1){}
}


//holds interrupts handlers for the rtos
//author ryan smith

#include "interrupts.h"
#include "asp_bit.h"
#include "rtos_common.h"
#include "debug.h"
#include "cti.h"

//dump memory
memdump dumpmem(char* str, memdump* m)
{
    getpsp(m->ptr_psp);
    getmsp(m->ptr_msp);
    m->fault_flags = *(volatile uint32_t)NVIC_FAULT_ADDR_R & FAULT_FLAG_MASK; //load the fault status register into the dump

    putsUart0("PSP: ");
    toAsciiHex(*str,(uint32_t)ptr_psp); // cast psp ptr as an int then convert to ascii hex.
    putsUart0(*str);                     //then print it out.
    putsUart0("\r\n");

    putsUart0("MSP: ");
    toAsciiHex(*str,(uint32_t)ptr_msp);
    putsUart0(*str);
    putsUart0("\r\n");

    putsUart0("R3: ");
    toAsciiHex(*str,m->ptr_psp[3]); //convert R3 to hex.
    putsUart0(*str);
    putsUart0("\r\n");

    putsUart0("R3: ");
    toAsciiHex(*str,m->ptr_psp[3]); //convert R3 to hex.
    putsUart0(*str);
    putsUart0("\r\n");

    putsUart0("R3: ");
    toAsciiHex(*str,m->ptr_psp[3]); //convert R3 to hex.
    putsUart0(*str);
    putsUart0("\r\n");

    putsUart0("R0: ");
    toAsciiHex(*str,m->ptr_psp[0]); //convert R0 to hex.
    putsUart0(*str);
    putsUart0("\r\n");

    putsUart0("R1: ");
    toAsciiHex(*str,m->ptr_psp[1]); //convert R1 to hex.
    putsUart0(*str);
    putsUart0("\r\n");

    putsUart0("R2: ");
    toAsciiHex(*str,m->ptr_psp[2]); //convert R2 to hex.
    putsUart0(*str);
    putsUart0("\r\n");

    putsUart0("R3: ");
    toAsciiHex(*str,m->ptr_psp[3]); //convert R3 to hex.
    putsUart0(*str);
    putsUart0("\r\n");

}


//hardfault
void HardFaultHandler(void)
{
/*   If a hard fault exception is invoked, display “Hard fault in thread PID”, where PID will be a variable
    provided by the OS. Or now, just use a variable named pid. Also, provide the value of the PSP, MSP,
    and all fault flags (in hex). Also, print the offending instruction. Display the process stack dump (xPSR,
    PC, LR, R0-3, R12.*/

    //setup useful vars
    memdump *m;
    char str[11];

//DEBUG:
    m->pid = 123;

    //print out alerts
    putsUart0("Hard fault in thread");
    intToAlpha(pid,*str);
    putsUart0(*str);
    putsUart0("\r\n");

    dumpmem(*str, m);   //dump the memory

    putsUart0("Fault Flags: ");
    toAsciiHex(*str,(m->fault_flags));
    putsUart0(*str);
    putsUart0("\r\n");

    while(1){}
}

//mpu
void MPUFaultHandler(void)
{
/*  If an MPU exception is invoked, display “MPU fault in thread PID”, where PID will be a variable provided
by the OS. Or now, just use a variable named pid. Also, provide the value of the PSP, MSP, and mfault
flags (in hex). Also, print the offending instruction and data addresses. Display the process stack dump
(xPSR, PC, LR, R0-3, R12. Clear the MPU fault pending bit and trigger a pendsv ISR call.
*/
    char str[11];

    memdump *m;
    m->pid = 456;

    putsUart0("MPU fault in thread ");
    intToAlpha(pid,*str);
    putsUart0(*str);
    putsUart0("\r\n");

    dumpmem(*str,m);    //dump the memory.

    m->fault_flags = NVIC_FAULT_ADDR_R & MFAULT_MASK;

    putsUart0("Fault Flags: ");
    toAsciiHex(*str,(m->fault_flags));
    putsUart0(*str);
    putsUart0("\r\n");


    while(1){}
}

//bus fault
void BusFaultHandler(void)
{
/*  If a bus fault exception is invoked, display “Bus fault in thread PID”, where PID will be a variable provided
by the OS. Or now, just use a variable named pid.
*/
    putsUart0("Bus fault in thread ");
    intToAlpha(pid,*str);
    putsUart0(*str);
    putsUart0("\r\n");


    while(1){}
}

//usage fault
void UsageFaultHandler(void)
{
/*  If a usage fault exception is invoked, display “Usage fault in thread PID”, where PID will be a variable
provided by the OS. Or now, just use a variable named pid.
*/
    putsUart0("Usage fault in thread ");
    intToAlpha(pid,*str);
    putsUart0(*str);
    putsUart0("\r\n");


    while(1){}
}

//pendsv
void PendSVHandler(void)
{
/*  If a pendsv exception is invoked, display “Pendsv in thread PID”. If the MPU DERR or IERR bits are set,
clear them and display the message “memory protection called this handler”.

*/
    uint32_t val;

    putsUart0("Pendsv in thread ");
    putsUart0(intToAlpha(pid));
    putsUart0("\r\n");

    val = NVIC_FAULT_ADDR_R & DERR_IERR_FLAG_MASK;

    if(val)
    {
        putsUart0("memory protection called this handler\r\n");
    }

    while(1){}
}



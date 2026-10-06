//holds interrupts handlers for the rtos
//author ryan smith

#include <stdint.h>
#include "interrupts.h"
#include "asp_bit.h"

#include "debug.h"          //toasciihex
#include "cti.h"            //terminal
#include "gpio.h"           //leds
#include "wait.h"           //waitMicroseconds

uint32_t pid = 0x12341234;  //0d305402420


//dump memory
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


//hardfault
void HardFaultHandler(void)
{
/*   If a hard fault exception is invoked, display “Hard fault in thread PID”, where PID will be a variable
    provided by the OS. Or now, just use a variable named pid. Also, provide the value of the PSP, MSP,
    and all fault flags (in hex). Also, print the offending instruction. Display the process stack dump (xPSR,
    PC, LR, R0-3, R12.*/

    //setup useful vars
    memdump *m={0};
    char str[BUFFER_STR_SIZE];

//DEBUG:
    pid = 123;

    BB_LED1 =1;                 //turn on the yellow LED


    //print out alerts
    putsUart0("Hard fault in thread");
    intToAlpha(pid,(char*)str);
    putsUart0((char*)str);
    putsUart0("\r\n");

    dumpmem((char*)str, m);   //dump the memory

    putsUart0("Fault Flags: ");
    toAsciiHex((char*)str,(m->fault_flags));
    putsUart0((char*)str);
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
    char str[BUFFER_STR_SIZE];

    memdump *m={0};
    pid = 456;

    BB_LED2 =1;                 //turn on the orange LED

    putsUart0("MPU fault in thread ");
    intToAlpha(pid,(char*)str);
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

//bus fault
void BusFaultHandler(void)
{
/*  If a bus fault exception is invoked, display “Bus fault in thread PID”, where PID will be a variable provided
by the OS. Or now, just use a variable named pid.
*/
    char str[BUFFER_STR_SIZE];

    BB_LED3=1;

    putsUart0("Bus fault in thread ");
    intToAlpha(pid,(char*)str);
    putsUart0((char*)str);
    putsUart0("\r\n");


    while(1){}
}

//usage fault
void UsageFaultHandler(void)
{
/*  If a usage fault exception is invoked, display “Usage fault in thread PID”, where PID will be a variable
provided by the OS. Or now, just use a variable named pid.
*/
    char str[BUFFER_STR_SIZE];

    BB_LED4 =1;                 //turn on the green LED

    putsUart0("Usage fault in thread ");
    intToAlpha(pid,(char*)str);
    putsUart0((char*)str);
    putsUart0("\r\n");

    if(makehardfault)
    {
        str[0]=1;
        str[1]=2;
        while(1)
        {
            str[0]-=1;
            str[1] /= str[0];
        }
    }

    while(1){}
}

//pendsv
void PendSVHandler(void)
{
/*  If a pendsv exception is invoked, display “Pendsv in thread PID”. If the MPU DERR or IERR bits are set,
clear them and display the message “memory protection called this handler”.

*/
    uint32_t val;
    char str[BUFFER_STR_SIZE];

    BB_LEDR =1;                 //turn on the TIVA red LED


    putsUart0("Pendsv in thread ");
    intToAlpha(pid,(char*)str);
    putsUart0((char*)str);
    putsUart0("\r\n");

    val = NVIC_FAULT_ADDR_R & DERR_IERR_FLAG_MASK;

    if(val)
    {
        putsUart0("memory protection called this handler\r\n");
    }

    while(1){}

    NVIC_INT_CTRL_R |= NVIC_INT_CTRL_UNPEND_SV;     //clear the pend so we don't return.
    return;
}



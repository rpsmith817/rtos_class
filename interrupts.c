//holds interrupts handlers for the rtos
//author ryan smith

#include "interrupts.h"


//hardfault
void HardFaultHandler(void)
{
/*   If a hard fault exception is invoked, display “Hard fault in thread PID”, where PID will be a variable
    provided by the OS. Or now, just use a variable named pid. Also, provide the value of the PSP, MSP,
    and all fault flags (in hex). Also, print the offending instruction. Display the process stack dump (xPSR,
    PC, LR, R0-3, R12.*/




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
    while(1){}
}

//bus fault
void BusFaultHandler(void)
{
/*  If a bus fault exception is invoked, display “Bus fault in thread PID”, where PID will be a variable provided
by the OS. Or now, just use a variable named pid.
*/
    while(1){}
}

//usage fault
void UsageFaultHandler(void)
{
/*  If a usage fault exception is invoked, display “Usage fault in thread PID”, where PID will be a variable
provided by the OS. Or now, just use a variable named pid.
*/

    while(1){}
}

//pendsv
void PendSVHandler(void)
{
/*  If a pendsv exception is invoked, display “Pendsv in thread PID”. If the MPU DERR or IERR bits are set,
clear them and display the message “memory protection called this handler”.

*/
    while(1){}
}



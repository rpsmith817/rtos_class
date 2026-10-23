// Shell functions
// J Losh

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target uC:       TM4C123GH6PM
// System Clock:    40 MHz

//-----------------------------------------------------------------------------
// Device includes, defines, and assembler directives
//-----------------------------------------------------------------------------

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "faults.h"

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

// REQUIRED: code this function
// REQUIRED: add hooks to exception handlers in startup ccs file
void initFaultHandlers(void);

// REQUIRED: If these were written in assembly
//           add a faults.s file

// REQUIRED: code this function
void mpuFaultIsr(void)
{
}

// REQUIRED: code this function
void hardFaultIsr(void)
{
}

// REQUIRED: code this function
void busFaultIsr(void)
{
}

// REQUIRED: code this function
void usageFaultIsr(void)
{
}


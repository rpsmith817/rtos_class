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
#include "shell.h"
#include "cti.h"

// REQUIRED: Add header files here for your strings functions, ...

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

// shell loop //
void shell(void)
{
    USER_DATA data;
    for(;;){
        if(kbhit())                         //check if hardware event for keyboard
        {
            getsUart0(&data);       //grab the data
            parseFields(&data);     //parse the fields
            doCommands(&data);    //check if we have a matching command
        }
        else
        {
            yield();
        }
    }
}

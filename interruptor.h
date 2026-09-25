//interrupts things
//author Ryan Smith 9/22/2026

#include "interruptor.h"


//hardfault
void cause_hardfault(void);


//mpufault
void cause_mpufault(void);


//busfault
void cause_busfault(void);


//usagefault
void cause_usagefault(void);


//pendsv not fault
void cause_pendsv(void);

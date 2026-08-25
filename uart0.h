//setup hardware for uart0 users.
//author:ryan smith

#ifndef UART0_H
#define UART0_H

#include "tm4c123gh6pm.h"


//-- prototypes --//
void initUart0();
void putcUart0(char c);
void putsUart0(char* str);
char getcUart0(void);


#endif
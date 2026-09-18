//header for interrupt handlers
//author ryan smith

#ifndef INTERRUPTS_H
#define INTERRUPTS_H

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

//common RTOS functions etc
// author: ryan smith
// date: 8/24/2026

#include "rtos_common.h"
#include "tm4c123gh6pm.h"

//reboot command. see tm4 datasheet p164-165 for details, also REF:https://github.com/yuvadm/tiva-c/blob/master/driverlib/sysctl.c for simultaneous
void rebootie(void)
{
    //write the key and the reset to the apint register at the same time.
    NVIC_APINT_R = NVIC_APINT_VECTKEY | NVIC_APINT_SYSRESETREQ;

    //if that doesn't reset it then we should sit around in a fault state. maybe a watchdog will get us.
    while(1)
    {
    }
}

//calls svc which will eventually call rebootie from kernel instead of task.
void rebooter(uint8_t a, uint8_t b)
{
    asm_svc_7();
}

//empty thing
void yield(void);

//display process (thread) status.
void ps(uint8_t a, uint8_t b)
{
    asm_svc_7();
}

//display inter-process (thread) status.
void ipcs(uint8_t a, uint8_t b)
{
    asm_svc_7();
}

//kills process (thread) associated with matching pid.
void kill(uint32_t pid, uint8_t b)
{
    asm_svc_7();
}

//kills process by name
void pkill(char* proc_name) //no need for additional args as this calls other functions that go to svc.
{
    uint32_t pid;
    pid = pidof(proc_name,8);
    kill(pid,3);
}

//turns preemption on or off
void preempt(bool on, uint8_t b)
{
    asm_svc_7();
}

//selected priority inheritance on or off
void pi(bool on, uint8_t b)
{
    asm_svc_7();
}

//select priority or round-robin scheduling.
void sched(bool prio_on, uint8_t b)
{
    asm_svc_7();
}

//returns the pid from the process name
uint32_t pidof(const char name[], uint8_t b, uint32_t c)
{
    asm_svc_7();
    return c;
}

//runs named process in the background
void bg_runner(char* name, uint8_t b)
{
    asm_svc_7();
}


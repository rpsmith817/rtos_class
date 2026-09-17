//common RTOS functions etc
// author: ryan smith
// date: 8/24/2026

#include "rtos_common.h"
#include "debug.h"
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

//empty thing
void yield(void);

//display process (thread) status.
void ps(void)
{
    putsUart0("PS Called\r\n");
}

//display inter-process (thread) status.
void ipcs(void)
{
    putsUart0("IPCS Called\r\n");
}

//kills process (thread) associated with matching pid.
void kill(uint32_t pid)
{
    char buf[MAX_CHARS];
    intToAlpha(pid, (char*)&buf);
    putsUart0((char*)&buf);
    putsUart0(" killed");
}

//kills process by name
void pkill(char* proc_name)
{
    uint32_t pid;
    pid = pidof(proc_name);
    kill(pid);
}

//turns preemption on or off
void preempt(bool on)
{
    if(on)
    {
        putsUart0("preempt on\r\n");
    }
    else
    {
        putsUart0("preempt off\r\n");
    }
}

//selected priority inheritance on or off
void pi(bool on)
{
    if(on)
    {
        putsUart0("pi on\r\n");
        return;
    }
    else 
    {
        putsUart0("pi off\r\n");
        return;
    }
}

//select priority or round-robin scheduling.
void sched(bool prio_on)
{
    if(prio_on)
    {
        putsUart0("sched prio\r\n");
    }
    else
    {
        putsUart0("sched rr\r\n");
    }
}

//returns the pid from the process name
uint32_t pidof(const char name[])
{
    putsUart0((char*)name);
    putsUart0(" launched\r\n");
    return 123456789;
}

//runs named process in the background
void bg_runner(char* name)
{
    redLED(1);
}

/*
    required, from assignment doc:
    reboot          - reboots microcontroller
    ps              - display process (thread) status. for now calls ps() which outputs "PS Called"
    ipcs            - Displays the inter-process (thread) status. for now calls ipcs() which outputs "IPCS Called"
    kill pid        - Kills the process (thread) associated with the matching pid. For now, calls kill(uint32_t pid) which displays the text "[pid] killed"
    pkill proc_name - Kills the thread based on the process name
    pi on|off       - turns priority inheritance on or off. for now calls pi(bool on) that displays "pi on" or "pi off"
    preempt on|off  - turns preemption on or off. for now it calls preempt(bool on) that displays "preempt on" or "preempt off"
    sched prio|rr   - Selected priority or round-robin shceduling. for now calls sched(bool prio_on) that displays "sched prio" or "sched rr"
    pidof proc_name - displays the pid of the process (thread) for now calls pidof(const char name[]) that displays "proc_name launched"
    proc_name &     - runs the selected program in the background. for now turns on the red LED.
*/

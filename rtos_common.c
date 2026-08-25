//common RTOS functions etc
// author: ryan smith
// date: 8/24/2026

#include "rtos_common.h"
//reboot command. see tm4 datasheet p164-165 for details
void rebootie(void)
{
    //step 1 write NVIC_APINT_VECTKEY(0x05FA) to NVIC_APINT_R
    NVIC_APINT_R |= NVIC_APINT_VECTKEY;
    //step2 set high sysresetreq.
    NVIC_APINT_R |= NVIC_APINT_SYSRESETREQ; 
}

void yield(void);

//display process (thread) status.
void ps(void);

//display inter-process (thread) status.
void ipcs(void);

//kills process (thread) associated with matching pid.
void kill(uint32_t pid);

//turns priority inheritance on or off
void pi(bool on);

//turns preemption on or off
void preempt(bool on);

//selected priority inheritance on or off
void pi(bool on);

//select priority or round-robin scheduling.
void sched(bool prio_on)

//returns the pid from the process name
uint32_t pidof(char* name);

//runs named process in the background
void bg_runner(char* name);

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
    pidof proc_name - displays the pid of the process (thread) for no calls pidof(const char name[]) that displays "proc_name launched"
    proc_name &     - runs the selected program in the background. for now turns on the red LED.
*/
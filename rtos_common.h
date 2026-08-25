//common things that multiple files will need for RTOS
//author: ryan smith

#ifndef RTOS_COMMON_H
#define RTOS_COMMON_H


#include stdbool.h
#include stdint.h


void yield(void);

//for reboot.
void rebootie(void);

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



#endif
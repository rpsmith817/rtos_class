//common things that multiple files will need for RTOS
//author: ryan smith

#ifndef RTOS_COMMON_H
#define RTOS_COMMON_H


#include <stdbool.h>
#include <stdint.h>

#include "cti.h"

extern uint32_t current_pid;

void yield(void);

//for reboot.
void rebootie(void);
void reboot(uint8_t a, uint8_t b);

//display process (thread) status.
void ps(void, uint8_t b);

//display inter-process (thread) status.
void ipcs(void, uint8_t b);

//kills process (thread) associated with matching pid.
void kill(uint32_t pid, uint8_t b);

//kills process by name
void pkill(char* proc_name);

//turns preemption on or off
void preempt(bool on, uint8_t b);

//selected priority inheritance on or off
void pi(bool on, uint8_t b);

//select priority or round-robin scheduling.
void sched(bool prio_on);

//returns the pid from the process name
uint32_t pidof(const char name[]);

//runs named process in the background
void bg_runner(char* name);


#endif

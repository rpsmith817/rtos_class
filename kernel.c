// Kernel functions
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
#include "mm.h"
#include "kernel.h"
#include "faults.h"
#include "uart0.h"
#include "cti.h"

//-----------------------------------------------------------------------------
// RTOS Defines and Kernel Variables
//-----------------------------------------------------------------------------

// mutex
typedef struct _mutex
{
    bool lock;
    uint8_t queueSize;
    uint8_t processQueue[MAX_MUTEX_QUEUE_SIZE];
    uint8_t lockedBy;
} mutex;
mutex mutexes[MAX_MUTEXES];

// semaphore
typedef struct _semaphore
{
    uint8_t count;
    uint8_t queueSize;
    uint8_t processQueue[MAX_SEMAPHORE_QUEUE_SIZE];
} semaphore;
semaphore semaphores[MAX_SEMAPHORES];

// task states
#define ST_INVALID           0 // no task
#define ST_READY             1 // can start/resume at any time
#define ST_DELAYED           3 // awaiting timeout
#define ST_BLOCKED_SEM       4 // blocked by semaphore
#define ST_BLOCKED_MUT       5 // blocked by mutex
#define ST_KILLED            6 // task has been killed (all memory freed)

// task
uint8_t taskCurrent = 0;          // index of last dispatched task
uint8_t taskCount = 0;            // total number of valid tasks
uint32_t current_pid;             // the current pid

// control
bool priorityScheduler = true;    // priority (true) or round-robin (false)
bool priorityInheritance = false; // priority inheritance for mutexes
bool preemption = true;           // preemption (true) or cooperative (false)

// tcb
#define NUM_PRIORITIES   8
struct _tcb
{
    uint8_t state;                 // see STATE_ values above
    void *pid;                     // used to uniquely identify thread (add of task fn)
    uint32_t stackSize;            // stack size requested with thread created
    void *sp;                      // current stack pointer
    uint8_t priority;              // 0=highest
    uint8_t currentPriority;       // 0=highest (needed for pi)
    uint32_t ticks;                // ticks until sleep complete
    uint64_t srd;                  // MPU subregion disable bits
    char name[16];                 // name of task used in ps command
    uint8_t mutex;                 // index of the mutex in use or blocking the thread
    uint8_t semaphore;             // index of the semaphore that is blocking the thread
} tcb[MAX_TASKS];

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

bool initMutex(uint8_t mutex)
{
    bool ok = (mutex < MAX_MUTEXES);
    if (ok)
    {
        mutexes[mutex].lock = false;
        mutexes[mutex].lockedBy = 0;
    }
    return ok;
}

bool initSemaphore(uint8_t semaphore, uint8_t count)
{
    bool ok = (semaphore < MAX_SEMAPHORES);
    {
        semaphores[semaphore].count = count;
    }
    return ok;
}

// REQUIRED: initialize systick for 1ms system timer
void initRtos(void)
{
    uint8_t i;
    // no tasks running
    taskCount = 0;
    // clear out tcb records
    for (i = 0; i < MAX_TASKS; i++)
    {
        tcb[i].state = ST_INVALID;
        tcb[i].pid = 0;
    }
}

// REQUIRED: Implement prioritization to NUM_PRIORITIES
uint8_t rtosScheduler(void)
{
    bool ok;
    static uint8_t task = 0xFF;
    ok = false;
    while (!ok)
    {
        task++;
        if (task >= MAX_TASKS)
            task = 0;
        ok = (tcb[task].state == ST_READY);
    }
    return task;
}

// REQUIRED: modify this function to start the operating system
// by calling scheduler, set srd bits, setting PSP, ASP bit, call fn with fn add in R0
// fn set TMPL bit, and PC <= fn
void startRtos(void)
{
    uint8_t i=0;
    uint8_t task = rtosScheduler();     //call scheduler
    applySramAccessMask(tcb[task].srd); //set srdbits
    setpsp(tcb[task].sp);               //set psp
    setAspBit();                        //set ASP bit
    setTmpl();                          //set TMPL
    for(i=0;i<16;i++)
    {
    __asm("POP\n");                     //pop the top 16 values, as they are mostly garbage.
    }
    _fn fn=tcb[task].pid;               //set pointer to task
    *fn();                              //run task.
}

// REQUIRED:
// add task if room in task list
// store the thread name
// allocate stack space and store top of stack in sp
// pre-load the stack so make it appear that the task has run before (including xPSR)   -> say thumb here.
// we pre-load so that the pendsvISR doesn't need to check if a thread is new and change behavior, it can pop and push as though the thread has always existed.
// set the srd bits based on the memory allocation
bool createThread(_fn fn, const char name[], uint8_t priority, uint32_t stackBytes)
{
    bool ok = false;
    uint8_t i = 0;
    (uint32_t)* ptr;    //for register handling, a 32bit pointer. it should provide enough offset each time when we index up to push onto the stack
    bool found = false;
    if (taskCount < MAX_TASKS)
    {
        // make sure fn not already in list (prevent reentrancy)
        while (!found && (i < MAX_TASKS))
        {
            found = (tcb[i++].pid ==  fn);
        }
        if (!found)
        {
            // find first available tcb record
            i = 0;
            while (tcb[i].state != ST_INVALID) {i++;}
            tcb[i].sp = malloc_from_heap(stackBytes);   //try to malloc the amount requested
            if(tcb[i].sp==NULL)                         //if we don't have space
            {
                return 0;                               //then gtfo
            }
            tcb[i].state = ST_READY;                    //otherwise set the state to ready
            tcb[i].stackSize = stackBytes;              //set size
            tcb[i].pid = fn;                            //set pid
            tcb[i].priority = priority;                 //set priority
            tcb[i].srd = setSramAccessWindow(tcb[i].sp,stackBytes); //set the srd and also return the bits that were used. PendSV will handle setting the accesswindow at task runtime, so we don't especially need to clear this yet

            //now push 16 values so that pendsv has something to handle/thinks things have already run before.

            //most values can be garbage, but we need to say thumb, which might mean that xpsr should say thumb.
            //also PC maybe says thumb too.
            //we need to know what order things pop in to know what order to push in. lucky that we already know that from fault handling.
            //ARM DUI 0553A p.2-27 shows from stack top R0,R1,R2,R3,R12,LR,PC,XPSR.
            //pendsv will upon call store those via h/w, then save R11-R4 by s/w, then save the PsP into tcb, then save the SRD into tcb, then s/w restore the SRD, then restore the PSP, then restore by s/w R4-R11 (backwards), then on starting the function h/w will restore the remaining registers in the reverse order from above.
            //So: our push order must R0,R1,R2,R3,R12,LR,PC,XPSR,R11,R10,R9,R8,R7,R6,R5,R4
            //R0 is a function arg, our tasks will probably want 0s
//            __asm("PUSH 0\n");  //R0
//            __asm("PUSH 12345\nPUSH 12345\nPUSH 12345\nPUSH 12345\n");//R1:3,R12 should just be junk
//            __asm("PUSH \".pendSvIsr\"\n"); //LR is the Return Link, which should maybe be pendsv?
//            __asm("PUSH 2\n");              //PC is the program counter, it might need to also say thumb, but I don't quite know how.
//            __asm("PUSH 0x01000000\n"); //xPSR, one of the spots we need to say thumb. tm4 datasheet p.81 shows the register def, we want the 24th bit to be 1, I don't think we care about hte rest.
            //this has to go to a different stack than the kernel stack, so we need to provide some offset info and push there. the placement still stands, so we may as well do them in order too.
            ptr=(uint32_t *)tcb[i].sp;      //setting our stack pointer to our stack pointer. cast to same type.
            ptr[0]=(uint32_t)0;             //R0
            ptr[1]=(uint32_t)12345;         //R1
            ptr[2]=(uint32_t)12345;         //R2
            ptr[3]=(uint32_t)12345;         //R3
            ptr[4]=(uint32_t)12345;         //R12
            ptr[5]=(void*)(yield);          //LR, this should call a pointer to the yield function, in case it is ever used it'll just audit whether it should be running or not.
            ptr[6]=(uint32_t)0x1;           //PC, apparently saying 1 in the LSB means it is thumb? idk,
            ptr[7]=(uint32_t)0x01000000;    //xPSR, as explained above this says thumb.
            for(i=8;i<16;i++)
            {
                ptr[i]=(uint32_t)12345;     //the rest of the registers R11:4
            }

            // increment task count
            taskCount++;
            ok = true;
        }
    }
    return ok;
}

// REQUIRED: modify this function to kill a thread
// REQUIRED: free memory, remove any pending semaphore waiting,
//           unlock any mutexes, mark state as killed
void killThread(_fn fn)
{

}

// REQUIRED: modify this function to restart a thread, including creating a stack
void restartThread(_fn fn)
{

}

// REQUIRED: modify this function to set a thread priority
void setThreadPriority(_fn fn, uint8_t priority)
{

}

// REQUIRED: modify this function to yield execution back to scheduler using pendsv
void yield(void)
{
    //call pendsv through svcall
//    __asm("SVC 1\n");

}

// REQUIRED: modify this function to support 1ms system timer
// execution yielded back to scheduler until time elapses using pendsv
void sleep(uint32_t tick)
{
    //svcall
//    __asm("SVC 2\n");
}

// REQUIRED: modify this function to wait a semaphore using pendsv
void wait(int8_t semaphore)
{

}

// REQUIRED: modify this function to signal a semaphore is available using pendsv
void post(int8_t semaphore)
{

}

// REQUIRED: modify this function to lock a mutex using pendsv
void lock(int8_t mutex)
{

}

// REQUIRED: modify this function to unlock a mutex using pendsv
void unlock(int8_t mutex)
{

}

// REQUIRED: modify this function to add support for the system timer
// REQUIRED: in preemptive code, add code to request task switch
void systickIsr(void)
{
    uint8_t i=0;
//handle ticks
    while(i!=MAX_TASKS)
    {
        if(tcb[i].ticks>0)
        {
            tcb[i].ticks--;
            if(tcb[i].ticks==0)
            {
                tcb[i].state=ST_READY;
            }
        }
    }
//request task switch if preemptive

}

// REQUIRED: in coop and preemptive, modify this function to add support for task switching
void pendSvIsr(void)
{

    uint32_t val;
    char str[BUFFER_STR_SIZE];



    putsUart0("Pendsv in thread ");
    intToAlpha(current_pid,(char*)str);
    putsUart0((char*)str);
    putsUart0("\r\n");

    val = NVIC_FAULT_ADDR_R & DERR_IERR_FLAG_MASK;

    if(val)
    {
        putsUart0("memory protection called this handler\r\n");
    }

    NVIC_INT_CTRL_R |= NVIC_INT_CTRL_UNPEND_SV;     //clear the pend so we don't return.




    //store task data, load data, start new task




}

// REQUIRED: modify this function to add support for the service call
// REQUIRED: in preemptive code, add code to handle synchronization primitives
void svCallIsr(void)
{



//paired functions courtesy of https://support.arm.com/documentation/107706/0100/System-exceptions/Supervisor-Call---SVC
//these consist of an assembly wrapper and main function. The assembly wrapper helps us decide if we are in the psp or msp when we call it.
//  __asm(
//    ".global SVC_Handler_Main\n"
//    "TST lr, #4\n"
//    "ITE EQ\n"
//    "MRSEQ r0, MSP\n"
//    "MRSNE r0, PSP\n"
//    "B SVC_Handler_Main\n"
//  ) ;
//}
//void SVC_Handler_Main(unsigned int *svc_args)
//{
//    //only yield right now, call pendsv
//    unsigned int svc_number;
//
//    /*
//    * Stack contains:
//    * r0, r1, r2, r3, r12, r14, the return address and xPSR
//    * First argument (r0) is svc_args[0]
//    */
//    svc_number = ( ( char * )svc_args[ 6 ] )[ -2 ] ;
//    switch( svc_number )
//    {
//        case 1: //yield
//            NVIC_INT_CTRL_R |= NVIC_INT_CTRL_PEND_SV;   //sets the pendsvcall, but because the priority is lower we go after this call ends.
//            break;
//        case 2: //sleep
//            //state = delay
//            //ticks = R0???whatever arg that sleep(uint32_t tick) had add that to the systick tasklist tick count.
//            //pendsv
//            NVIC_INT_CTRL_R |= NVIC_INT_CTRL_PEND_SV;   //pendsv as above
//
//        default:
//            break;
//    }
}

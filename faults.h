// Faults functions
// J Losh

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target uC:       TM4C123GH6PM
// System Clock:    40 MHz

#ifndef FAULTS_H_
#define FAULTS_H_


//-----------------------------------------------------------------------------
// Macros and struct
//-----------------------------------------------------------------------------


#define FAULT_FLAG_MASK 0x030FBFBB
#define DERR_IERR_FLAG_MASK 0x3
#define MFAULT_MASK 0xBB
#define BUFFER_STR_SIZE 11

typedef struct _memdump{
    uint32_t * ptr_psp;
    uint32_t* ptr_msp;
    uint32_t pid;
    uint32_t fault_flags;
} memdump;


//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

void dumpmem(char* str, memdump* m); //get registers and put them into a struct for function use. spit out the values that are commonly spat out while we are at it.

void initFaultHandlers(void);
void mpuFaultIsr(void);
void hardFaultIsr(void);
void busFaultIsr(void);
void usageFaultIsr(void);

#endif

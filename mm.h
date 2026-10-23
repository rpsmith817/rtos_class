// Memory manager functions
// J Losh

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target uC:       TM4C123GH6PM
// System Clock:    40 MHz

#ifndef MM_H_
#define MM_H_

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

void * malloc_from_heap(uint32_t size_in_bytes);
void free_to_heap(void *address_from_malloc);
void initMemoryManager(void);
void initMpu(void);

#endif

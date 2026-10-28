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
// Macross
//-----------------------------------------------------------------------------


#define HEAP_AT 0x20001000      //the kernel reserve 4k memory = 4096, though this is superseded for now by the request for the program stack to start at 0x20008000.
#define TASKMAX 16              //max number of tasks that we can expect. i dont remember why this was made!
#define PAGES 28                //max number of pages the heap may have. in our case 32-4 = 28, as 4 is reserved for kernel.
#define PAGE_SIZE 1024          //default size from assignment.


//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

void * malloc_from_heap(uint32_t size_in_bytes);
void free_to_heap(void *address_from_malloc);
void initMemoryManager(void);
void initMpu(void);


void mpuEnablePls(void);
void setBasicAccess(void);

//return a pointer given a mapped page index.
void* point_to_mem(uint8_t idx);

//set all pages within a block of memory to free.
void clearblock(uint8_t idx);

//counts all the free pages within a block and sets the size of that block.
void freeBlockCount(void);

//return the index of a free block matching the size.
uint8_t check_free(uint8_t cnt);

//frees from heap while not stepping on cstd functions' toes
void free_to_heap(void *p);

//creates a full-access MPU aperture for flash with RWX access for both privileged and unprivileged access.
void allowFlashAccess(void);

//creates a full-access MPU aperture to peripherals and peripheral bitbanded addresses with RW access for privileged and unprivileged access.
void allowPeripheralAccess(void);

//creates multiple MPUs regions to cover the 32KiB SRAM (each MPU region covers 8KiB or 4 KiB, with 8 subregions of 1KiB or 512B each with RW access for privileged mode and no access for unprivileged mode.
void setupSramAccess(void);

//return values of bits that allow no access ig? more like return no sram access mask.
uint64_t createNoSramAccess(void);

//applies SRD bits to MPU regions, only called in privileged mode.
void applySramAccessMask(uint64_t srdBitMask);

//adds access to the requested SRAM address range
void addSramAccessWindow(uint64_t * srdBitMask, uint32_t *baseAdd, uint32_t size_in_bytes);

//dunno. This is mentioned in the doc, called in addSramAccessWindow with the values (uint32_t*)0x20000000 and 32768
uint64_t setSramAccessWindow(uint32_t *ijustworkhere, uint32_t metoo);




#endif

// Memory manager functions
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
#include "mm.h"
#include "tm4c123gh6pm.h"   //device macros
#include "rtos_common.h"    //pid
#include "asp_bit.h"        //memBarrier

#define TEXSCB_FL (0x2<<16)     //tex, s, c, and b settings for flash
#define TEXSCB_ISRAM (0x6<<16)  //"" for internal SRAM
#define TEXSCB_ESRAM (0x9<<16)  //"" for external SRAM
#define TEXSCB_PER (0x5<<16)    //"" for peripherals.

//defines a page
typedef struct _page
{
    uint32_t the_pid;   //who owns it?
    uint8_t size;       //how many pages in this block?
    uint8_t first_idx;  //what idx is the first page in the map?
    void* start;        //where is the block's first memory address?
}page;

//defines the map
typedef struct _memmap
{
    //we'll define a block as a set of pages, each page will contain the metadata needed.
    uint8_t free;
    page pages[PAGES];      //array of structs the same as the number of pages in the map.
}memmap;

memmap the_map;    //instantiate the map as a global

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

// REQUIRED: modify cmd file or add #pragma DATA_SECTION to allocate memory for stack

// REQUIRED: add code to initialize the memory manager
void initMemoryManager(void)
{
    uint8_t i;
    for(i =0; i<PAGES; i++)
    {
        the_map.pages[i].size = PAGES;
        the_map.pages[i].the_pid = 0;
        the_map.pages[i].first_idx = 0;
        the_map.pages[i].start = NULL;
    }
    the_map.free=PAGES; //the total count of free pages is the max at init.
}

// REQUIRED: add your malloc code here and update the SRD bits for the current thread
void * malloc_from_heap(uint32_t size_in_bytes)
{
    if((size_in_bytes < 1)|| (size_in_bytes/PAGE_SIZE > PAGES)) return NULL;  //shortcut if we don't need to do anything.

    uint8_t cnt;
    uint8_t idx;
    uint8_t val;
    void* p = NULL;

    //find out how many pages we need to use for a given carve-out.
    cnt = size_in_bytes/PAGE_SIZE;           //about how many 1024s do we have?
    if(size_in_bytes % PAGE_SIZE)            //if we are over an exact amount
    {
        cnt++;                        //then round up to nearest page size
    }

    //if the requested size is larger than the pages available then return a null.
    if(cnt > the_map.free) return p;

    //to continue we check if there is a contiguous space free and grab its index
    idx = check_free(cnt);
    if(idx+cnt-1 < PAGES)                         //if we found a matching index
    {
        for(val=0;val<cnt;val++)            //then iterate through to reserve the pages.
        {
            the_map.pages[idx+val].size = cnt;
            the_map.pages[idx+val].first_idx = idx;
            the_map.pages[idx+val].the_pid = pid;
            the_map.pages[idx+val].start = point_to_mem(idx);
        }
        p = the_map.pages[idx].start;           //save the pointer to the matching address.
        the_map.free -= cnt;                    //and indicate the reduction in free space.
    }

    freeBlockCount();   //count up all the free pages and set their block sizes for later use.

    return p;           //return a pointer to either NULL or the register for memory.
}

//gives the starting point to a real memory address of a mapped memory address
void* point_to_mem(uint8_t idx)
{
    if(idx > PAGES-1) return NULL;                                      //if we are out of bounds, then ignore the request.
    else                                                                //and otherwise return the address start
    {
        return (void*)(HEAP_AT + PAGE_SIZE * the_map.pages[idx].first_idx); //which is where heap starts plus the size of page times the number)
    }
}

//return an index to the first page of a free block which has enough space to accommodate a given page count.
uint8_t check_free(uint8_t cnt)
{
    uint8_t i=0;

    while(i<PAGES)    //go through each page if needed
    {
        if((the_map.pages[i].size >= cnt) && (the_map.pages[i].the_pid == 0))    //check if we have enough space to reserve for the task and the block is free.
        {
            return i;                       //return the page index if so
        }
        i++;//iterate
    }

    return PAGES;                 //otherwise we fell out with no match, so return bad val as max, which is an invalid index to use and should always be guarded against.
}

//frees from heap while not stepping on cstd functions' toes
void free_to_heap(void *p)
{
    uint8_t i=0;
    while((i<PAGES) && (the_map.pages[i].start != p)){i++;} //loop through til we find the page, but bound our checks to the correct range
    if(i<PAGES)                                             //if less than pages we found a match
    {
        if((the_map.pages[i].the_pid == 0) || ((the_map.pages[i].the_pid != pid) && (pid != 0))){ return;}   //if we are already free then leave. if we don't have permission or aren't the kernel, then leave.
        the_map.free += the_map.pages[i].size;      //otherwise add some freeness
        clearblock(i);                              //clear the block
        freeBlockCount();                           //and count up all the free pages and set their block sizes for later use.
    }
    else
    {
        //otherwise we fell through. Later we might do something here. like call a fault. not being able to free something sounds bad.
    }

    return; //either way we are done.
}

// REQUIRED: add your custom MPU functions here (eg to return the srd bits)

// REQUIRED: initialize MPU here
void initMpu(void)
{
    // REQUIRED: call your MPU functions here
}


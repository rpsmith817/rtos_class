//simple memory manager for rtos
//author Ryan Smith 9/22/2026


#include "memory.h"
#include "rtos_common.h"    //pid

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

//initializes the map.
void init_mem(void)
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

//mallocs to the heap while not stepping on cstd functions' toes
//The memory allocated will be rounded up to the nearest multiple of 1024B if the allocation requested is <= 1024B and
//rounded up to the nearest multiple of 1024B otherwise. The memory address of all allocated memory chunks will be
//aligned to a multiple of 1024.
void * malloc_from_heap (int size_in_bytes)
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

    while(i<PAGES-1)    //go through each page if needed
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
    while((i<PAGES) && (the_map.pages[i].start != p)){i++;} //loop through til we find the page
    if(i<PAGES)                                             //if less than pages we found a match
    {
        the_map.free += the_map.pages[i].size;
        clearblock(i);  //clear the block
        freeBlockCount();   //count up all the free pages and set their block sizes for later use.
    }
    else
    {
        //otherwise we fell through. Later we might do something here. like call a fault. not being able to free something sounds bad.
    }

    return; //either way we are done.
}


//set all pages in a block to free.
void clearblock(uint8_t idx)
{
    uint8_t i;
    uint8_t firstofall= the_map.pages[idx].first_idx;   //find the first index
    uint8_t presize = the_map.pages[idx].size;          //find the size of the block.
    for(i=firstofall;  ((i < (presize+firstofall)) && (i < PAGES));  i++)    //then clear for size number of pages to free the associated block.
    {
        the_map.pages[i].the_pid = 0;
    }
}


//count the number of entries in each block that are free, set the size in each.
void freeBlockCount(void)
{
    uint8_t i=0;
    uint8_t j=0;
    uint8_t cnt=0;
    while(i<PAGES+1)  //go through each page
    {
        if((i<PAGES) && the_map.pages[i].the_pid == 0)   //if the spot we are at is a free spot.
        {
            cnt++; //count it
        }
        else    //otherwise we are no longer in a free block.
        {
            if(cnt!=0) //if we have accumulated some count then that must be spent.
            {
/*consider single page0 -> i=1, cnt=1, first_idx = 1-1=0
        or pages 1-4 -> hits on i=5, cnt=4, first_idx = 5-4 = 1.
        or pages 26-27 -> hits on i=28, cnt=2, first_idx = 27-2=26.
        so this should fix indexing
*/
                for(j=0; j<cnt; j++)    //reset the important values here.
                {
                    the_map.pages[i-j-1].first_idx = i-cnt;
                    the_map.pages[i-j-1].start = (void*)(HEAP_AT + PAGE_SIZE * (i-cnt));
                    the_map.pages[i-j-1].size = cnt;
                }
                cnt=0;
            }
        }
        i++;
    }
}

//creates a full-access MPU aperture for flash with RWX access for both privileged and unprivileged access.
void allowFlashAccess(void)
{

}

//creates a full-access MPU aperture to peripherals and peripheral bitbanded addresses with RW access for privileged and unprivileged access.
void allowPeripheralAccess(void)
{

}

//creates multiple MPUs regions to cover the 32KiB SRAM (each MPU region covers 8KiB or 4 KiB, with 8 subregions of 1KiB or 512B each with RW access for privileged mode and no access for unprivileged mode.
void setupSramAccess(void)
{

}

//return values of bits that allow no access ig? more like return no sram access mask.
uint64_t createNoSramAccess(void)
{
    return 0;
}

//applies SRD bits to MPU regions, only called in privileged mode.
void applySramAccessMask(uint64_t srdBitMask)
{

}

//adds access to the requested SRAM address range
void addSramAccessWindow(uint64_t * srdBitMask, uint32_t *baseAdd, uint32_t size_in_bytes)
{

}

//dunno. This is mentioned in the doc, called with the values (uint32_t*)0x20000000 and 32768
void setSramAccessWindow(uint32_t *ijustworkhere, uint32_t metoo)
{

}



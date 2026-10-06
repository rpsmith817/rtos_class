//simple memory manager for rtos
//author Ryan Smith 9/22/2026

#include "tm4c123gh6pm.h"   //device macros
#include "memory.h"
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


/* 
OK lets assume that we are going to have 0x20001000 as our PSP delimiter. This gives the OS 0x20000000 - 0x20000FFF, and defines 28 pages of 1024 for the tasks to use.
We need a region for SRAM, with some specific permission levels for each of the 32 slices in there by srd. Probably the same for the bitband region of SRAM, with each of the 32 slices matching in both bitband and non-bitband regions.
We also need a region for flash, and peripherals.

The MPU supports 8 memory regions.

NVIC_MPU_NUMBER_R is the region access register, which bits we need are [2:0].
NVIC_MPU_BASE_R is the base of the region, which bits we care about are [31:5] for the region base address, [4] for validity setting (to indicate we need to update the region using the base address, [2:0] for the Region Number.
  ->If we write the base, region, and validity, then we don't need to write the region access number first.
NVIC_MPU_ATTR_R is the MPU Region and size register(and more), which bits we desire fervently to edit are [28] XN - which allows/disallows execution in a given region, [26:24] AP - which sets the access privilege for a region, [21:16] which are the TEX, S, C, and B bits, which together form the bulk of permissions settings (see p128 of tm4 manual, cause its a doozy), [15:8] SRD - which give more granular control to regions, [5:1] SIZE which defines the size of a region, and [0] EN - which enables a region.

So first we need to define broad rules. We will go with a restrictive base region, with subregions granting specific access.

p.130 in the tm4 manual indicates these settings
flash       ->  TEXSCB = 000010; normal memory, non-sharable, write-through
Int.SRAM    ->  TEXSCB = 000110; normal memory, sharable, write-through
ext SRAM    ->  TEXSCB = 000111; normal memory, sharable, write-back write-allocate (and also we don't care about this setting)
peripherals ->  TEXSCB = 000101; device memory, sharable.

Per Losh board we shoud consider using 4 SRAM regions with 8 subregions for the granularity required.

so with 8 regions we have:
region 0 - base region - default setting covering everything
region 1 - flash memory - program data.
region 2 - peripheral - timers, gpios, etc.
region 3:6 - SRAM regions (subdivided by SRD)
region 7 - would have been external SRAM, but the TM4 doesnt have this, and apparently this was copy-pasted generic guidance for an M4. so we will just leave it.

Memory regions should be
 *  0x0000.0000 thru all                                                    |r0
 *  0x0000.0000 -> flash start                                              |r1
 *  0x0003.FFFF -> flash end                                                |r1
 *  0x2000.0000 -> SRAM start, MSP start+OS lives here.                     |r3:6
 *  0x2000.0FFF -> OS end                                                   |r3:6
 *  0x2000.1000 -> PSP start + tasks live here.                             |r3:6
 *      >>>That gives us 32768-4096 = 28672, /=1024 gives us our 28 pages.
 *          Each will want specific permissions, which is where our srd comes in.
 *  0x2000.7FFF -> SRAM end, and consequently PSP ends.                     |r3:6
 *  0x2200.0000 -> SRAM Bitband start                                       |r3:6?
 *  0x220F.FFFF -> SRAM Bitband end                                         |r3:6?
 *  0x4000.0000 -> Peripherals start                                        |r2
 *  0x5FFF.FFFF -> Peripherals end.                                         |r2
 *  0x6000.0000 -> External RAM start                                       |r0 -> as per above explanation we won't be defining this unless some later doc says to do so, as we dont have ext. SRAM.
 *  0x9FFF.FFFF -> External RAM end                                         |r0

 */

//set the mpu to be used. we only care about enabling, not the background rule. We set our own in region0, so we don't really need a -1, and this simplifies things for us.
void mpuEnablePls(void)
{
    NVIC_MPU_CTRL_R |= 0x1;
}

//set the basic access rule, which will be that no permission is granted except in privileged mode
void setBasicAccess(void)
{
    //set base register, region, and validity bit to ensure we write this. region0
    NVIC_MPU_BASE_R = 0x00000000 |  0x0 | (0x1 << 4) ;
    //set attributes for no rwx except for privileged access.texscb = xxxxxx, relevant regions will be set to the default recommended texscb, and otherwise ignored. so no setting here, which I guess means 000000
    NVIC_MPU_ATTR_R = 0x10000000 | (1 << 24)| (0x1F << 1) | 1;

}

//creates a full-access MPU aperture for flash with RWX access for both privileged and unprivileged access.
void allowFlashAccess(void)
{
    //set base register, region, and validity bit to ensure we write it. region1
    NVIC_MPU_BASE_R = 0x00000000 | (0x1 << 4) | 0x1;
    //set attributes for rwx all, texscb=000010, SIZE -> size=0x00040000 -> 2^18, should be 2^(SIZE-1), so size=17
    NVIC_MPU_ATTR_R = 0x00000000 | (3 << 24) | (0x2<<16) | (17<<1) | 1;
}

//creates a full-access MPU aperture to peripherals and peripheral bitbanded addresses with RW access for privileged and unprivileged access.
void allowPeripheralAccess(void)
{
    //set base register, region, and validity bit. region2
    NVIC_MPU_BASE_R = 0x40000000 | (0x1 << 4) | 0x2;
    //set attributes for rw all, xn TEXSCB = 000101, SIZE-> size = 0x20000000->2^29, should be 2^(SIZE-1), so SIZE=28
    NVIC_MPU_ATTR_R = 0x10000000 | (3 << 24) | (5<<16) | (28<<1) | 1;
}

//creates multiple MPUs regions to cover the 32KiB SRAM (each MPU region covers 8KiB or 4 KiB, with 8 subregions of 1KiB or 512B each with RW access for privileged mode and no access for unprivileged mode.
void setupSramAccess(void)
{
    //regions3:6, need to scale up so add size to each base (0x2000)
    //region3
    NVIC_MPU_BASE_R = 0x20000000 | (0x1<<4)|0x3;
    //texscb=000110 full size is 32kib, we need 4 regions at that size so we need 32kib/4=8kib = 0x2000 -> 2^13, so SIZE=12 for each. Subregions are enabled for each, removing access to unprivileged tasks.
    NVIC_MPU_ATTR_R = 0x10000000 | (3<<24) | (6<<16) | (12<<1) | 1 | (0xFF << 8);
    //then the rest follow the same pattern.
    //region4
    NVIC_MPU_BASE_R = 0x20002000 | (0x1<<4)|0x4;
    NVIC_MPU_ATTR_R = 0x10000000 | (3<<24) | (6<<16) | (12<<1) | 1 | (0xFF << 8);
    //region5
    NVIC_MPU_BASE_R = 0x20004000 | (0x1<<4)|0x5;
    NVIC_MPU_ATTR_R = 0x10000000 | (3<<24) | (6<<16) | (12<<1) | 1 | (0xFF << 8);
    //region6
    NVIC_MPU_BASE_R = 0x20006000 | (0x1<<4)|0x6;
    NVIC_MPU_ATTR_R = 0x10000000 | (3<<24) | (6<<16) | (12<<1) | 1 | (0xFF << 8);

}

//return values of bits that allow no access ig? more like return no sram access mask.
uint64_t createNoSramAccess(void)
{
    return 0xffffffffffffffff;  //our most restrictive setting is all bits enabled.
}

//applies SRD bits to MPU regions, only called in privileged mode.
//this makes a few assumptions regarding that bitmask. We assume that it covers all mpu regions, and that the lower bit fields correspond to the lower region numbers, at 1byte ea field
void applySramAccessMask(uint64_t srdBitMask)
{
    uint8_t i;  //an iterator0
    for(i=0;i<4;i++)
    {
        NVIC_MPU_NUMBER_R = (0x3+i);    //select each SRAM region in turn. assign, not or, so we don't accumulate region numbers.
        //rewrite the attributes with this region's byte of the mask in the srd field [15:8].
        NVIC_MPU_ATTR_R = 0x10000000 | (3<<24) | (6<<16) | (12<<1) | 1 | ((uint32_t)((srdBitMask >> (8*i)) & 0xFF) << 8);
    }

    //an instruction barrier is called for in cases where an interrupt might occur per the manual p.127, form is at p.134.
    memBarrier();
}

//adds access to the requested SRAM address range
void addSramAccessWindow(uint64_t *srdBitMask, uint32_t *baseAdd, uint32_t size_in_bytes)
{
    uint32_t start = (uint32_t)baseAdd;     //work with the addr as a number instead of constantly casting
    uint8_t first;                          //first subregion index in window
    uint8_t last;                           //one past the last subregion index in the window
    uint8_t i;                              //an iteratorr

    //given the size in bytes from a base address, check first if the window is within sram bounds and nonzero.
    if((srdBitMask == NULL) || (size_in_bytes == 0)){ return;}
    if((start < 0x20000000) || (size_in_bytes > (32*PAGE_SIZE) || ((start - 0x20000000) + size_in_bytes > (32*PAGE_SIZE)))){ return;}

    //then check that it lines up on subregion boundaries.
    if(((start - 0x20000000) % PAGE_SIZE) || (size_in_bytes % PAGE_SIZE)) {return;}

    //then figure out what bits to clear. subregion n lives at SRAM_AT + n*1024, and is bit n of the mask, and we love that about it.
    first = (start - 0x20000000) / PAGE_SIZE;
    last  = first + (size_in_bytes / PAGE_SIZE);
    for(i=first; i<last; i++)
    {
        *srdBitMask &= ~((uint64_t)1 << i);  //clearing the bit enables the subregion, which grants the region's AP=011 access.
    }

    //an instruction barrier is called for in cases where an interrupt might occur, per the manual p.127, form is at p.134.
    memBarrier();
}

//dunno. This is mentioned in the doc, called with the values (uint32_t*)0x20000000 and 32768.
//we will make this a function that calls the add window function and apply window function for a given base address and size of access window.
void setSramAccessWindow(uint32_t *baseAdd, uint32_t size_in_bytes)
{
    uint64_t srdBitMask = createNoSramAccess();               //pull least access at first
    addSramAccessWindow(&srdBitMask,baseAdd,size_in_bytes); //clear out portions we want to give access
    applySramAccessMask(srdBitMask);                        //apply our changes
}

//simple memory manager for rtos
//author Ryan Smith 9/22/2026

#ifndef MEMORY_H
#define MEMORY_H




//mallocs to the heap while not stepping on cstd functions' toes
//The memory allocated will be rounded up to the nearest multiple of 1024B if the allocation requested is <= 1024B and
//  rounded up to the nearest multiple of 1024B otherwise. The memory address of all allocated memory chunks will be
//  aligned to a multiple of 1024.
void * malloc_from_heap (int size_in_bytes);


//frees from heap while not stepping on cstd functions' toes
void free_to_heap(void *p);


//creates a full-access MPU aperture for flash with RWX access for both privileged and unprivileged access.
void allowFlashAccess(void);

//creates a full-access MPU aperture to peripherals and peripheral bitbanded addresses with RW access for privileged and unprivileged access.
void allowPeripheralAccess(void);

//creates multiple MPUs regions to cover the 32KiB SRAM (each MPU region covers 8KiB or 4 KiB, with 8 subregions of 1KiB or 512B each with RW access for privileged mode and no access for unprivileged mode.
void setupSramAccess(void);




#endif

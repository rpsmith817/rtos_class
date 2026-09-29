/*
 * debug helper header.
 * Ryan Smith
 *
 */
#ifndef DEBUG_H_
#define DEBUG_H_

#include <stdint.h>
#include <stdbool.h>

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

//function to take 32bit val and parse out to ascii version of hex string.
//the given string's index size must be 9 or greater.
void toAsciiHex(char* buff, uint32_t Val);


#endif

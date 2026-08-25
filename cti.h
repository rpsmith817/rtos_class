/*
 * cti.h
 *
 *  Created on: Mar 24, 2025
 *      Author: ryans
 */

#ifndef CTI_H_
#define CTI_H_

#include <stdint.h>
#include <stdbool.h>

//size defs for fields.
#define MAX_CHARS 80
#define MAX_FIELDS 5

//stores user data
typedef struct _USER_DATA{
    char buffer[MAX_CHARS+1];               //raw input until parsed, then it contains fields and NULLs.
    uint8_t fieldCount;                     //These values are always index number + 1 style for better truthiness. AKA if there is no count then fieldCount = 0 = FALSE, else fieldCount = TRUE.
    uint8_t fieldPosition[MAX_FIELDS];      //Start index of data of interest.
    char fieldType[MAX_FIELDS];             //data is either 'a' or 'n' for alpha and numeric.
} USER_DATA;


//my lil strlen
uint8_t strlength(const char str[]);

// Initialize UART0 on Port A
void initUart0();

// Blocking function that writes a serial character when the UART buffer is not full
void putcUart0(char c);

// Blocking function that writes a string when the UART buffer is not full
void putsUart0(char* str);

//get a character from uart.
char getcUart0();

//returns true if there is anything in the fifo
bool kbhit(void);

//This is a function to receive characters from the user interface, processing special characters such as backspace and writing the resultant string into the buffer.
void getsUart0(USER_DATA *data);

//This is a function that takes the buffer string from the getsUart0() function and processes the string in-place and returns information about the parsed fields in fieldCount, fieldPosition, and fieldType.
void parseFields(USER_DATA *data);

//Returns the value of a field requested if the field number is in range or NULL otherwise.
char* getFieldString(USER_DATA *data, uint8_t fieldNumber);

//Returns the integer value of the field if the field number is in range and the field type is numeric or 0 otherwise.
int32_t getFieldInteger(USER_DATA *data, uint8_t fieldNumber);

// This function returns true if the command matches the first field and the number of arguments (excluding the command field) is greater than or equal to the requested number of minimum arguments.
bool isCommand(USER_DATA *data, const char strCommand[], uint8_t minArguments);


#endif /* CTI_H_ */
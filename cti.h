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
typedef struct _USER_DATA {
    char buffer[MAX_CHARS+1];               //raw input until parsed, then it contains fields and NULLs.
    uint8_t fieldCount;                     //These values are always index number + 1 style for better truthiness. AKA if there is no count then fieldCount = 0 = FALSE, else fieldCount = TRUE.
    uint8_t fieldPosition[MAX_FIELDS];      //Start index of data of interest.
    char fieldType[MAX_FIELDS];             //data is either 'a' or 'n' for alpha and numeric.
} USER_DATA;

//command type for members of command list.
typedef struct _Cti_Cmd_t {
    const char *cmd;    //the command string
    callback_t cmdFunc;      //pointer to function command represents.  
    uint8_t minArguments;    //number of arguments required to run a command.
} Cti_Cmd_t;

//lists commands and their structure
void helpMe();

//typedef of callbacks
typedef void(*callback)(USER_DATA *data) callback_t;

//protypes of callbacks
void cmd_helpMe(USER_DATA *data);
void cmd_rebootie(USER_DATA *data);
void cmd_ps(USER_DATA *data);
void cmd_ipcs(USER_DATA *data);
void cmd_kill(USER_DATA *data);
void cmd_pkill(USER_DATA *data);
void cmd_pi(USER_DATA *data);
void cmd_preempt(USER_DATA *data);
void cmd_sched(USER_DATA *data);
void cmd_pidof(USER_DATA *data);
void cmd_bg_runner(USER_DATA *data);

//my lil strlenm;    /
uint8_t strlength(char *str);

//my lil string compare
bool stringComp(char* str1, char* str2);

//lightweight (imprecise) power function
uint16_t myPow(float x, uint16_t n);

// Initialize UART0 on Port A
void initUart0();

// Blocking function that writes a serial character when the UART buffer is not full
void putcUart0(char c);

// Blocking function that writes a string when the UART buffer is not full
void putsUart0(char *str);

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
uint32_t getFieldInteger(USER_DATA *data, uint8_t fieldNumber);

// This function returns true if the command matches the first field and the number of arguments (excluding the command field) is greater than or equal to the requested number of minimum arguments.
bool isCommand(USER_DATA *data, const char strCommand[], uint8_t minArguments);

//convert from an integer into an ascii output.
void intToAlpha(uint32_t in);

//compare the input to the list of commands, then run the command if we match
void doCommands(USER_DATA *data);

//shell task/thread/loop
void shell(void);

//a list of commands and the functions that they will call. We could organize by most commonly used, but I don't know what will be used...
static const Cti_Cmd_t cti_cmd_list[] = {
    {"help",        cmd_helpMe,     0}, //get list of commands
    {"reboot",      cmd_rebootie,   0}, //reboot the device
    {"ps",          cmd_ps,         1}, //get process status (likely needs a pid, or might give all running processes)
    {"ipcs",        cmd_ipcs,       0}, //call interprocess thread, whatever that means for this system
    {"kill",        cmd_kill,       1}, //kill [pid] -> terminate process by id
    {"pkill",       cmd_pkill,      1}, //kill [pname] -> terminate process by name
    {"pi",          cmd_pi,         1}, //pi on|off -> toggles priority inheritance
    {"preempt",     cmd_preempt,    1}, // on|off ->toggles preemption
    {"sched",       cmd_sched,      1}, // prio|rr -> toggles priority or round-robin scheduling
    {"pidof",       cmd_pidof,      1}, //displays pid be pname
    {"proc_name",   cmd_bg_runner,  1}, //runs named program in background. we may need to make an entry for each function name?
    {0,0,0}   //the NULL entry to let us know we are done. dunno if we need it but it is nice to have.
};

#endif /* CTI_H_ */
/*
 * common terminal interface  made for inputting and acting on commands and values.
 *
 * REF:	https://leetcode.com/problems/powx-n/solutions/6567501/easy-c-100-o-n/ for pow
 *      https://stackoverflow.com/questions/11395821/how-to-determine-the-length-of-a-string-without-using-strlen -> strlen() w/o strlen()
 *      uart0.c written by Jason Losh
 */


#include <stdint.h>
#include <stdbool.h>

#include "tm4c123gh6pm.h"   //hardware register and mask macros

#include "cti.h"            //shell
#include "debug.h"          //toAsciiHex()
#include "clock.h"          //clock init
#include "wait.h"           //waitMicrosecond()

//size defs for fields.
#define MAX_CHARS 80
#define MAX_FIELDS 5

// PortA masks
#define UART_TX_MASK 2
#define UART_RX_MASK 1

//command type for members of command list.
typedef struct {
    const char *cmd;    //the command string
    _callback cmdFunc;  //pointer to function command represents.  
    uint8_t special;    //flag indicating special handling of command
    uint8_t build;      //flag indicating command must be build instead of standard call
} Cti_Cmd_t;

//a list of commands and the functions that they will call.
static const Cti_Cmd_t cti_cmd_list[] ={
    {"reboot",      rebootie,      0, 0},
    {"ps",          ps,            0, 0},
    {"ipcs",        ipcs,          0, 0},
    {"kill",        cb_kill,       0, 0},
    {"pkill",       cb_pkill,      0, 0},
    {"pi",          cb_pi,         0, 0},
    {"preempt",     cb_preempt,    0, 0},
    {"sched",       cb_sched,      0, 0},
    {"pidof",       cb_pidof,      0, 0},
    {"proc_name",   cb_bg_runner,  0, 0},
    {NULL,NULL,NULL,NULL}
}

//stores entry data
typedef struct _USER_DATA{
    char buffer[MAX_CHARS+1];               //raw input until parsed, then it contains fields and NULLs.
    uint8_t fieldCount;                     //current number of fields.
    uint8_t fieldPosition[MAX_FIELDS];      //Start index of data of interest.
    char fieldType[MAX_FIELDS];             //data is either 'a' or 'n' for alpha and numeric.
} USER_DATA;


//my lil strlen
uint8_t strlength(char* str){		//having clearer bounds would prevent this from whiling away endlessly.	Set a limit maybe?
    uint8_t len=0;
    while(str[len] && len < MAX_CHARS){len++;}     //while the position is not a string terminator add to our len counter.
    return len;                 //then return the count
}

//compare strings, return true if they match and false if they don't
bool stringComp(char* str1, char* str2){
    uint8_t i = 0;
    while(str1[i]){                 //while there is data in the str to read, read that data.
        if(str1[i] != str2[i]){     //if there is a character mismatch, then we return false.
            return 0;
        }
        i++;
    }
    if(str2[i]!='\0')               //if we still have some data in the other string, then we didn't actually match.
        return 0;
    else
        return 1;                   //We did it, they match!

}

//lightweight (imprecise) power function
uint16_t myPow(float x, uint16_t n) {
    if (n == 0)
        return 1.0;
    int N = n;
    //    //handle negative exponents. this portion won't be effective with an unsigned int, so it is commented out. why have extra steps?
    //    if (N < 0) {
    //        x = 1 / x;
    //        N = -N;
    //    }
    float ans = 1.0;    //we don't have half precision in C, so we gotta use the full 32.
    while (N > 0) {
        if (N % 2 == 1)
            ans *= x;
        x *= x;
        N /= 2;
    }
    return ans;
}

// Initialize UART0 on Port A
void initUart0(){
    // Enable clocks
    SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R0;
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0;
    _delay_cycles(3);

    // Configure UART0 pins
    GPIO_PORTA_DR2R_R |= UART_TX_MASK;                  // set drive strength to 2mA (not needed since default configuration -- for clarity)
    GPIO_PORTA_DEN_R |= UART_TX_MASK | UART_RX_MASK;    // enable digital on UART0 pins
    GPIO_PORTA_AFSEL_R |= UART_TX_MASK | UART_RX_MASK;  // use peripheral to drive PA0, PA1
    GPIO_PORTA_PCTL_R &= ~(GPIO_PCTL_PA1_M | GPIO_PCTL_PA0_M); // clear bits 0-7
    GPIO_PORTA_PCTL_R |= GPIO_PCTL_PA1_U0TX | GPIO_PCTL_PA0_U0RX;
    // select UART0 to drive pins PA0 and PA1: default, added for clarity

    // Configure UART0 to 115200 baud, 8N1 format
    UART0_CTL_R = 0;                                    // turn-off UART0 to allow safe programming
    UART0_CC_R = UART_CC_CS_SYSCLK;                     // use system clock (40 MHz)
    UART0_IBRD_R = 21;                                  // r = 40 MHz / (Nx115.2kHz), set floor(r)=21, where N=16
    UART0_FBRD_R = 45;                                  // round(fract(r)*64)=45
    UART0_LCRH_R = UART_LCRH_WLEN_8 | UART_LCRH_FEN;    // configure for 8N1 w/ 16-level FIFO
    UART0_CTL_R = UART_CTL_TXE | UART_CTL_RXE | UART_CTL_UARTEN;
    // enable TX, RX, and module
}

// Blocking function that writes a serial character when the UART buffer is not full
void putcUart0(char c){
    while (UART0_FR_R & UART_FR_TXFF);               // wait if uart0 tx fifo full
    if(c=='\0')
        return;
    UART0_DR_R = c;                                  // write character to fifo
}

// Blocking function that writes a string when the UART buffer is not full
void putsUart0(char* str){
    uint8_t i = 0;
    while (str[i] != '\0')
        putcUart0(str[i++]);
}

//get a character from uart.
char getcUart0(){
    while (UART0_FR_R & UART_FR_RXFE)   // wait if uart0 tx fifo empty and also blink a light to show we are waiting for some input.
    {
        yield();                        //yield per assignment
    }
    return UART0_DR_R & 0xFF;           // pull char from FIFO
}

bool kbhit()
{
    return ~(UART0_FR_R & UART_FR_RXFE);            //if the FIFO is not empty then return true
}

//This is a function to receive characters from the user interface, processing special characters such as backspace and writing the resultant string into the buffer.
//If the count of characters in the buffer is > 0, process backspace characters (ASCII code 8 or 127) by decrementing the count of received characters, effectively erasing the character from the buffer.
//If the character received is a line feed (ASCII code 10) or carriage return (ASCII code 13), add a null terminator to the end of the buffer and return.
//Ignore any other characters that are unprintable (ASCII code < 32 “space”)
//For the printable characters, add each character received to the buffer, increment the character count, and return from the function if the count of characters in the buffer is equal to MAX_CHARS. You may want to make the interface case insensitive. If this behavior is desired, convert upper-case to lower-case or vice-versa to make string comparisons easier.
void getsUart0(USER_DATA *data){
    volatile uint8_t i = 0; //an iterator
    while(i != MAX_CHARS)
    {
        //go ahead and set the char, if we need to we will discard it after checking.
        data->buffer[i] = getcUart0();
        putcUart0(data->buffer[i]); //print it out for now.

        //if newline then set to null and return.
        if(data->buffer[i] == 10 || data->buffer[i] == 13)
        {
            data->buffer[i] = '\0';
            return;
        }

        //deal with backspaces
        else if((data->buffer[i] == 8 || data->buffer[i] == 127 || data->buffer[i] > 127) && (i>0))
        {
            i--;
        }

        //if we are 32 or more we should maybe be ok.
        else if(data->buffer[i]>=32 && data->buffer[i] < 127 || data->buffer[i] == '&')
        {
            i++;
        }
    }
    data->buffer[i+1] = '\0';
    return;
}

//This is a function that takes the buffer string from the getsUart0() function and processes the string in-place and returns information about the parsed fields in fieldCount, fieldPosition, and fieldType.
//Decide on 3 sets of characters – alpha, numeric, and delimiter. Alpha is a-z and A-Z, numeric is 0-9 and optionally hyphen and period (or comma in some localizations), and everything else is a delimiter.
// Assume that the previous character type is a delimiter when starting to search the buffer.
// Go through the buffer from left to right, looking for the start of a field (a transition from a delimiter to a alpha or numeric character). For each field (at the transition), record the type of field (alpha or numeric – you can use ‘a’ or ‘n’ if you wish) in the type array, and the offset of the field within the buffer of the field in the position array, and increment the field count. Make the previous character stored equal to the new character and keep moving through the buffer string until the end is found. If the field count equals MAX_FIELDS, return from the function.
//Before returning, convert all delimiters in the string to NULL characters to aid the getter functions to follow.
void parseFields(USER_DATA *data){
    uint8_t i; //iterator
    //initialize values
    i=0;
    data->fieldCount = 0;   //if count is zero and we base everything on the count, then we don't have to worry about clearing everything, only count.

    //the big loop, if there is data in the buffer, handle that data, unless we reach the MAX_FIELDS
    while(data->buffer[i] && data->fieldCount < MAX_FIELDS){
        //numeric check
        if(data->buffer[i] > 47 && data->buffer[i] < 58){
            data->fieldType[data->fieldCount] = 'n';
            data->fieldPosition[data->fieldCount] = i;
            data->fieldCount++;
            while(data->buffer[i] > 47 && data->buffer[i] < 58){i++;}  //iterate up til the read isn't a number, because then we will have left the field.
        }
        //alpha check
        if((data->buffer[i] > 64 && data->buffer[i] < 91)||(data->buffer[i] > 96 && data->buffer[i] < 123)){
            data->fieldType[data->fieldCount] = 'a';
            data->fieldPosition[data->fieldCount] = i;
            data->fieldCount++;
            while((data->buffer[i] > 65 && data->buffer[i] < 91)||(data->buffer[i] > 96 && data->buffer[i] < 123)){i++;}    //iterate til we aren't a letter.
        }
        //special character check
        if((data->buffer[i] == '&'))
        {
            data->fieldPosition[data->fieldCount] = i;
            data->fieldType[data-fieldCount] = 's';
            data->fieldCount++;
        }
        //we assume a delimiter if the other tests prove false.
        data->buffer[i] = '\0';
        i++;
    }
    return;
}

//Returns the value of a field requested if the field number is in range or NULL otherwise.
char* getFieldString(USER_DATA *data, uint8_t fieldNumber){
    //check if we even have a value there.
    if(fieldNumber > data->fieldCount)
        return '\0';

    //urgently do the needful.
    char* field;
    if(data->buffer[data->fieldPosition[fieldNumber]])              //while the buffer char at position of fieldPosition[fieldNumber] plus our iterator is not NULL
    {
        field = &data->buffer[data->fieldPosition[fieldNumber]];      //write the character to our return string.
    }
    return field;
}

//Returns the integer value of the field if the field number is in range and the field type is numeric or 0 otherwise.
uint32_t getFieldInteger(USER_DATA *data, uint8_t fieldNumber){
    uint32_t val=0;
    //check if we even have a value there, and if it is a number. If not then return 0
    if(fieldNumber > data->fieldCount || data->fieldType[fieldNumber] != 'n')
    {
        return 0;
    }

    char* field = getFieldString(data,fieldNumber); //get the full number as a c-string
    uint8_t len = strlength(field);                 //get the length of the field for exponentiation
    uint8_t i=0;
    uint32_t tens=1;

    //set initial tens value
    for(i;i<len;i++)
    {
        tens*=10;
    }

    //reset iterator
    i=0;

    //loop through each idx
    while(len!=0)
    {
        val += ((field[i++]-'0') * tens);       //add to value the numeric value of field times the tens place that it is in.
        tens/=10;                               //divide by tens to reduce tens place
        len--;                                    //iterate down
    }
    return val;
}

// This function returns true if the command matches the first field and the number of arguments (excluding the command field) is greater than or equal to the requested number of minimum arguments.
bool isCommand(USER_DATA *data, const char strCommand[], uint8_t minArguments){

    uint8_t i=0;    //iterator.
    char* stringy = data->buffer;

    //if the minimum argument number is more than our fields give
    if(minArguments > (data->fieldCount)-1)
        return 0;

    //return our lil boolean strcmplite.
    return stringComp((char*)strCommand,&data->buffer);

}

//convert from an integer into an ascii output.
char* toAlpha(uint32_t in)
{
    char out[MAX_CHARS];    //the output string
    char buf[max_chars];
    uint32_t val=in;
    uint8_t i=0;              //an iterator

    //get length
    while(val != 0)
    {
        val /= 10;
        i++;
    }
    //set back to input value
    val=in;
    //loop through to get the value into the output buffer
    for(i; i> 0; i--)
    {
        buf[i] = val%10;
    }


}

// shell loop //
void shell(void)
{
    USER_DATA shelly;
    shelly.fieldCount=0;
    shelly.fieldType={};
    shelly.fieldPosition={};
    shelly.buffer[0]='\0';

    for(;;){
        if(kbhit())		                        //check if hardware event for keyboard and also our length is OK.
        {
            getsUart0(&data);       //grab the data
            parseFields(&data);     //parse the fields
            

        }
    }
}
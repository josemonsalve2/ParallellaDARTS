#include "e_darts_print.h"

void e_darts_print(const char * message){
    // if sendPrintInstruction is not 0, it means the host 
    // has not finished printing
    while (__printBuffer.sendPrintInstruction != 0);
    
    int charCount = 0;
    while (message[charCount] != 0x0)
    {
        __printBuffer.printprintingBufferHead[charCount] = message[charCount];
        charCount++;
    }
    __printBuffer.sendPrintInstruction = 1;
}
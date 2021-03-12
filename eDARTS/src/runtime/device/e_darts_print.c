#include "e_darts_print.h"

darts_mutex_t __printingMutex __attribute__ ((section("printingMutex"))) = DARTS_MUTEX_NULL; // Located in a particular location in core 0,0 in linker script
printBuffer_t __printBuffer __attribute__ ((section(".printBuffer"))) = {0, &__printingMutex, ""};

/* debugging float issue -- function wrapper
void e_darts_print(const char * message, ...) {
    va_list args_list;
    va_start(args_list, message);
    e_darts_print_in(message, args_list);
}
*/

void e_darts_print(const char * message, ...){

    darts_mutex_lock(__printBuffer.mutex);
    // if sendPrintInstruction is not 0, it means the host
    // has not finished printing
    while (__printBuffer.sendPrintInstruction != 0);

    va_list args_list;
    int charCount = 0;
    int argCount = 0;
    char argTypes[MAX_NUM_ARGUMENTS]; // up to 20 arguments more

    // We copy the message to the buffer up to 200 characters
    while (message[charCount] != 0x0 && charCount < MAX_NUM_CHARACTERS)
    {
    	// Counting the number of arguments we should have
    	if (message[charCount] == '%' && argCount < MAX_NUM_ARGUMENTS)
    	{
    		argTypes[argCount] = message[charCount + 1];
    		argCount++;
    	}
    	__printBuffer.printingBufferHead[charCount] = message[charCount];
    	charCount++;
    }
    // If we reach 200 characters, we want to truncate the message
    if(charCount == MAX_NUM_CHARACTERS) {
    	__printBuffer.printingBufferHead[charCount - 1] = 0x0;
    } else {
    	__printBuffer.printingBufferHead[charCount] = 0x0;
    }

    // Now, let's add all the arguments to the arguments buffer
    va_start(args_list, message);
    char * argStart = (char*) args_list;
    charCount = 0;
    for (; argCount >= 0 && charCount < ARGUMENTS_SIZE; argCount--) {
    	// TODO Fix %s. It seems like the characters are not sent in the argument args_list
    	if (argTypes[argCount] == 's' || argTypes[argCount] == 'S') {
    		//__printBuffer.arguments[charCount++] = 0;
    		//while (argStart[charCount] != 0x0 && charCount < ARGUMENTS_SIZE) {
    		//    __printBuffer.arguments[charCount] = argStart[charCount];
    		//    charCount++;
    		//}
    	}
    	else {
    		int size = TYPE_SELECTOR(argTypes[argCount]);
    		while(size > 0) {
    			__printBuffer.arguments[charCount] = argStart[charCount];
    			charCount++;
    			size --;
    		}
    	}
    }
    va_end(args_list);

    int i = 0;
    // Wait for the whole buffer to reach memory...
    __printBuffer.sendPrintInstruction = 1;
    darts_mutex_unlock(__printBuffer.mutex);

}

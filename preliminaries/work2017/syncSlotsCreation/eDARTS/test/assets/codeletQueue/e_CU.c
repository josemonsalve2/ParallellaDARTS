//First implementation of a codeletsQueue for the parallella

#include <stdio.h>
#include <stdlib.h>
#include "e-lib.h"
#include "common.h"
#include "codeletsQueue.h"

typedef void (*codelet)();

int main(void)
{
    unsigned *done,*workFlag,*value, *codQuePtr;
    unsigned codelet_to_exec; 
    codeletsQueue_t codeletQueue;

    unsigned cuAddress = (unsigned) e_get_global_address( 0 , 1 , 0x0000 );
    
    codelet toExecute;

    done = (unsigned *) 0x2000;
    workFlag = (unsigned *) 0x2004;
    codQuePtr = (unsigned *) 0x2008; 
    *codQuePtr = (unsigned) &codeletQueue;
    value = (unsigned *) 0x2228;

    //Init the queue
    initCodeletsQueue(&codeletQueue, 300, cuAddress + (unsigned *) 0x200C);

    *done = 0;
    *value = 0;
    
    // This happens forever until the runtime is stopped
    while(*done == 0)
    {
	      //      //flag to check if there is work to do.
            if ( popCodeletQueue(&codeletQueue,0,1, &codelet_to_exec) == 0 ) // Assuming core(0,0)
            {
                  
               toExecute = (codelet) codelet_to_exec;
               toExecute();
            }
    }
    //*value = codelet_to_exec;//(unsigned) &codeletQueue;
    return 0;
}


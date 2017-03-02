//Firts attempt to an scheduling unit in Parallella
//
//

#include <stdio.h>
#include <stdlib.h>
#include "e-lib.h"
#include "common.h"


typedef void (*codelet)();

int main(void)
{
    unsigned *done,*workFlag,*value, *codeletQueue;
    codelet toExecute;

    done = (unsigned *) 0x2000;
    workFlag = (unsigned *) 0x2004;
    codeletQueue = (unsigned *) 0x2008;
    value = (unsigned *) 0x2028;

    *done = 0;
    *workFlag = 0;
    *value = 0;
    
    // This happens forever until the runtime is stopped
    while(*done==0)
    {
            //flag to check if there is work to do.
            if (*workFlag==1)
            {
               toExecute = (codelet) *codeletQueue;
               toExecute();
                *workFlag = 0;
            }
    }
    return 0;
}


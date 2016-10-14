//Firts attempt to a compute unit in Parallella
//
//

#include <stdio.h>
#include <stdlib.h>
#include "e-lib.h"
#include "common.h"


/* Internal Memory Banks for eCores
 *  Bank 0  ->  0x00000000+
 *  Bank 1  ->  0x00002000+         // Data
 *  Bank 2  ->  0x00004000+
 *  Bank 3  ->  0x00006000+
 * Reserved Internal Memory
 *  Reserved   ->  0x00008000+
 * Memory-Mapped Registers
 *  Registers   ->  0x000F0000 to 0x00100000
 */

typedef void (*codelet)();

int main(void)
{
    unsigned *done,*workFlag,*value, *codeletQueue;
    codelet toExecute;

    // CU's local memory
    done = (unsigned *) (DATA_OFFSET + 0x0000);
    workFlag = (unsigned *) (DATA_OFFSET + 0x0004);
    codeletQueue = (unsigned *) (DATA_OFFSET + 0x0008);
    value = (unsigned *) (DATA_OFFSET + 0x0028);

    *done = 0;
    *workFlag = 0;
    *value = 0;
    
    // This happens forever until the runtime is stopped
    while(*done==0)
    {
            /* Would there a need for workFlag if codeletQueue had a designated NULL value (maybe 0)?
             *  For instance if(*codeletQueue!=0) then execute next codelet.
             */

            //flag to check if there is work to do.
            if (*workFlag==1)
            {
              toExecute = (codelet) *codeletQueue;
              toExecute();
              *workFlag = 0;        // what if there are more codelets in the queue?
            }
    }
    return 0;
}


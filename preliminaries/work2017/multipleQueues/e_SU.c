#include <stdio.h>
#include <stdlib.h>
#include "e-lib.h"
#include "common.h"
#include "codeletsQueue.h"


void sum()
{
    unsigned *value;
    value = (unsigned *) 0x2228;

    (*value) ++;
}

int main(void)
{
    int i=0;
    unsigned *value;


    //for local memory
    value = (unsigned *) 0x2228;

   
    //for global memory of the CUs
    unsigned *cuCodelet, *cuDone;
    codeletsQueue_t * codeletQueue;
    unsigned cuAddress, suAddress, cuCodeletQueueAddr;
    
    cuAddress = (unsigned) e_get_global_address( 0 , 1 , 0x0000 );
    suAddress = (unsigned)  e_get_global_address( SU_ROW , SU_COL , 0x0000 );
    cuDone =    (unsigned *)  (cuAddress + (unsigned) CU_DONE_ADDR);

    // I need to change this to a barrier
    while(*cuDone != 0);
    
    cuCodeletQueueAddr = ((unsigned)(cuAddress + CU_CODQUEUE_ADDR));
    codeletQueue =(codeletsQueue_t *) (cuCodeletQueueAddr);

    for (i = 0 ; i < 1000000; i++)
    {
        while ( pushCodeletQueue(codeletQueue,SU_ROW, SU_COL, (unsigned) ( *suAddress + ((unsigned) &sum))) == 1);
    }

    while( queueEmpty(codeletQueue) );
    *cuDone=1;

    return 0;
}


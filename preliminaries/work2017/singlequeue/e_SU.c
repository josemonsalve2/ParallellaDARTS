//Firts attempt to an scheduling unit in Parallella
//
//

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
    unsigned *queue;
    unsigned *value;


    //for local memory
    queue = (unsigned *) 0x2000;
    value = (unsigned *) 0x2228;

   
    //for global memory of the CUs
    unsigned *cuCodeletQueueAddr, *cuCodelet, *cuDone, *suAddress;
    codeletsQueue_t * codeletQueue;
    unsigned cuAddress;
    suAddress = (unsigned *) 0x2004 ;
    cuAddress = (unsigned) e_get_global_address( 0 , 1 , 0x0000 );
    *suAddress = (unsigned)  e_get_global_address( 0 , 0 , 0x0000 );
    cuDone =    (unsigned *)  (cuAddress + (unsigned) 0x2000);

    while(*cuDone != 0); 
    cuCodeletQueueAddr = ((unsigned *)(cuAddress + (unsigned) 0x2008));
    codeletQueue =(codeletsQueue_t *) (cuAddress + *cuCodeletQueueAddr);

    for (i = 0 ; i < 1000000; i++)
    {
        while ( pushCodeletQueue(codeletQueue,0,0,(unsigned) ( *suAddress + ((unsigned) &sum))) == 1);
    }

    while(codeletQueue->curNumElements!=0);
    *cuDone=1;
    
    
    

    return 0;
}


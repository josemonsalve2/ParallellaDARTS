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

typedef struct SU_states_s
{
    unsigned signal[NUM_CU];
} SU_states_t;

int main(void)
{
    SU_states_t * states;
    states = (SU_states_t *) SU_STATES_BASE;
    
    int i=0;
   
    //for global memory of the CUs
    unsigned *cuDone, *startSignal;
    codeletsQueue_t * codeletQueue;
    unsigned cuAddress, suAddress, cuCodeletQueueAddr;
    
    cuAddress = (unsigned) e_get_global_address( 0 , 1 , 0x0000 );
    suAddress = (unsigned)  e_get_global_address( SU_ROW , SU_COL , 0x0000 );
    cuDone =    (unsigned *)  (cuAddress + (unsigned) CU_DONE_ADDR);
    
    cuCodeletQueueAddr = ((unsigned)(cuAddress + CU_CODQUEUE_ADDR));
    codeletQueue =(codeletsQueue_t *) (cuCodeletQueueAddr);
    
    //wait for host
    startSignal = (unsigned *)  (RT_START_SIGNAL);
    while(*startSignal != 0);

    //Wait for CUs to be ready
    for ( i = 0; i < NUM_CU; ++i)
    {
        while (states->signal[i] != 0);
    }
    
    for (i = 0 ; i < 10; i++)
    {
        while ( pushCodeletQueue(codeletQueue,0, 1, (unsigned) ( suAddress + ((unsigned) &sum))) == 1);
    }

    while( queueEmpty(codeletQueue) != 0);
    *cuDone=1;

    return 0;
}


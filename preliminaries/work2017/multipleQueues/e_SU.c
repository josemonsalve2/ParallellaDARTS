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
    unsigned startSignal;
    unsigned done;
    unsigned suBaseAddress;
    unsigned signal[NUM_CU];
    unsigned cuBaseAddress[NUM_CU];
} SU_states_t;

void init_SU_states(SU_states_t ** states)
{
    int i;
    *states = (SU_states_t *) SU_STATES_BASE;
    (*states)->done = 0;
    (*states)->suBaseAddress = (unsigned) e_get_global_address( SU_ROW , SU_COL , 0x0000 );
    for ( i = 0; i < NUM_CU; ++i )
    {
        (*states)->cuBaseAddress[i] = (unsigned) e_get_global_address( (i+1) / 4 , (i+1) % 4  , 0x0000 );
    }
}

void set_CU_DONE(SU_states_t * states, unsigned int cu_num)
{
    unsigned *cuDone;
    cuDone = (unsigned *) (states->cuBaseAddress[cu_num] + (unsigned) CU_DONE_ADDR);
    *cuDone=1;
}

int main(void)
{
    SU_states_t *states;
    
    init_SU_states(&states);
    
    int i,j;
   
    //for global memory of the CUs
    codeletsQueue_t * codeletQueue;
    unsigned cuCodeletQueueAddr;
    
    //Wait for host
    while(states->startSignal != 0);

    //Wait for CUs to be ready
    for (i = 0; i < NUM_CU; ++i)
    {
        while (states->signal[i] != 0);
    }
    
    for (i = 0; i < NUM_CU; ++i)
    {
        cuCodeletQueueAddr = ((unsigned)(states->cuBaseAddress[i] + CU_CODQUEUE_ADDR));
        codeletQueue = (codeletsQueue_t *) (cuCodeletQueueAddr);
        for ( j = 0; j < 100; ++j )
        {
            
            while ( pushCodeletQueue(codeletQueue, (unsigned) ( states->suBaseAddress + ((unsigned) &sum))) == 1);
        }
    }

    //set CUs done
    for (i = 0; i < NUM_CU; ++i)
    {
        cuCodeletQueueAddr = ((unsigned)(states->cuBaseAddress[i] + CU_CODQUEUE_ADDR));
        codeletQueue = (codeletsQueue_t *) (cuCodeletQueueAddr);
        while( queueEmpty(codeletQueue) != 0);
    }

    //set CUs done
    for (i = 0; i < NUM_CU; ++i)
    {
        set_CU_DONE(states, i);
    }
    states->done = 1;
    return 0;
}


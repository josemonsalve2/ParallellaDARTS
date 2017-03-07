#include <stdio.h>
#include <stdlib.h>
#include "e-lib.h"
#include "common.h"
#include "codeletsQueue.h"

// Trying to keep this with the same order
// Otherwhise you will need to change common.h
typedef struct PACKED CU_states_s
{
    unsigned startSignal;
    unsigned done;
    codeletsQueue_t codeletQueue;
} CU_states_t;

void init_CU_states(CU_states_t** states)
{
    *states = (CU_states_t *) CU_STATES_BASE_ADDR;
    
    // e_group_config and e_emem_config are structures defined
    // in e-lib that contain core values.
    initCodeletsQueue(&((*states)->codeletQueue), 100, (unsigned *)(CU_MY_GLOBAL_BASE_ADDR + CU_QUEUEHEAD_ADDR));

}

int main(void)
{
    //Runtime variables
    CU_states_t* states;
    init_CU_states(&states);
    
    unsigned *value;
    unsigned popedCodelet; 

    codelet toExecute;
     
    value = (unsigned *) 0x2228;
    
    unsigned suAddress;
    suAddress = (unsigned)  e_get_global_address( SU_ROW , SU_COL , 0x0000 );
    
    //Init the queue
    states->done = 0;
    *value = 0;
    
    // RT Barrier
    while(states->startSignal != 0);
    
    //signal SU
    unsigned * su_signal;
    su_signal = (unsigned *) (suAddress + SU_CU_SIGNALS + MY_CU_NUM*sizeof(unsigned));
    *su_signal = 0;

    // This happens forever until the runtime is stopped
    while( states->done == 0 || queueEmpty(&states->codeletQueue)  != 0)
    {
            //flag to check if there is work to do.
            if ( popCodeletQueue(&(states->codeletQueue), &popedCodelet) == 0 )
            {
               toExecute = (codelet) popedCodelet;
               toExecute();
            }
    }
    return 0;
}


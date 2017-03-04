#include <stdio.h>
#include <stdlib.h>
#include "e-lib.h"
#include "common.h"
#include "codeletsQueue.h"

// Trying to keep this with the same order
// Otherwhise you will need to change common.h
typedef struct CU_states_s
{
    unsigned done;
    codeletsQueue_t codeletQueue;
} CU_states_t;

void init_CU_states(CU_states_t* states)
{
    states = (unsigned *) CU_STATES_BASE_ADDR; // I am starting here because there is some reserve memory before and bank 2
    
    // e_group_config and e_emem_config are structures defined
    // in e-lib that contain core values.
    initCodeletsQueue(&(states->codeletQueue), 100, (unsigned *)(CU_BASE_ADDR + CU_QUEUEHEAD_ADDR));

}

int main(void)
{
    //Runtime variables
    CU_states_t* states;
    init_CU_states(states);
    
    unsigned *value;
    unsigned codelet_to_exec; 

    codelet toExecute;
     
    value = (unsigned *) 0x2228;

    //Init the queue
    states->done = 0;
    *value = 0;
    
    // This happens forever until the runtime is stopped
    while( states->done == 0 )
    {
	      //      //flag to check if there is work to do.
            if ( popCodeletQueue(&(states->codeletQueue),CU_COL,CU_ROW, &codelet_to_exec) == 0 ) // Assuming core(0,0)
            
                  
               toExecute = (codelet) codelet_to_exec;
               toExecute();
            }
    }
    return 0;
}


#include <stdio.h>
#include <stdlib.h>
#include "e-lib.h"
#include "e_darts_barrier.h"
#include "e_darts_print.h"

#define SEQUENTIALIZED_RESULT_LOCATION 0x2220
#define UNSAFE_RESULT_LOCATION 0x2224
#define INIT_SIGNAL 0x2004
#define FINAL_SIGNAL 0x2008
#define BARRIER_LOCATION 0x200C

int main(void)
{
    // for loops
    int i, j;

    unsigned base0_0Address = (unsigned) e_get_global_address( 0 , 0 , 0x0000 );
    unsigned *initSignal = (unsigned *) (base0_0Address + INIT_SIGNAL);
    unsigned *finalSignal = (unsigned *) (base0_0Address + FINAL_SIGNAL);
    unsigned *seqResultLocation = (unsigned *) (base0_0Address + SEQUENTIALIZED_RESULT_LOCATION);
    unsigned *unsafeResultLocation = (unsigned *) (base0_0Address + UNSAFE_RESULT_LOCATION);
    darts_barrier_t * myBarrier =(darts_barrier_t *) ( base0_0Address + BARRIER_LOCATION);

    // initialization of the barrier since this part it is not threadsafe.
    // We still have to rely on initSignal
    if (e_group_config.core_row == 0 && e_group_config.core_col == 0) {
    	*seqResultLocation = 0;
    	*unsafeResultLocation = 0;
    	darts_barrier_init(myBarrier,16);
        *initSignal = 1;
    	e_darts_print("\n === HERE IS WITH BARRIERS: ===\n");
    } else {
        while (*initSignal != 1);
    }
    //stress test
    for (i = 0; i < 1000; i++)
    {
        for (j = 0; j < e_group_config.core_row * 4 + e_group_config.core_col; j++);
        darts_barrier(myBarrier);
        for (j = 0; j < e_group_config.core_row * 4 + e_group_config.core_col; j++);
        darts_barrier(myBarrier);
    }

    int NUM_LOOPS = 1;
    int threadNumber = e_group_config.core_row + e_group_config.core_col*4;
    for (i = 0; i < 16 * NUM_LOOPS; i ++) {
    	darts_barrier(myBarrier);
        if (e_group_config.core_row == 0 && e_group_config.core_col == 0) {
        	e_darts_print("Before -- ");
        }
    	// we barrier everyone
    	darts_barrier(myBarrier);
    	if (e_group_config.core_row == 1 && e_group_config.core_col == 1) {
    	        	e_darts_print("Middle -- ");
    	        }
    	// we barrier again
    	darts_barrier(myBarrier);
    	if (e_group_config.core_row == 2 && e_group_config.core_col == 2) {
			e_darts_print("after \n");
		}

    }

    if (e_group_config.core_row == 0 && e_group_config.core_col == 0) {
    	e_darts_print("\n === HERE IS WITH NO BARRIERS: ===\n");
    }
	darts_barrier(myBarrier);


    for (i = 0; i < 16 * NUM_LOOPS; i ++) {
    	// only one thread increments
        if (e_group_config.core_row == 0 && e_group_config.core_col == 0) {
        	e_darts_print("Before -- ");
        }
    	// we barrier everyone
    	if (e_group_config.core_row == 1 && e_group_config.core_col == 1) {
    	        	e_darts_print("Middle -- ");
    	        }
    	// we barrier again
    	if (e_group_config.core_row == 2 && e_group_config.core_col == 2) {
			e_darts_print("after \n");
		}
    }

    // Final signaling
	darts_barrier(myBarrier);
	if (e_group_config.core_row == 0 && e_group_config.core_col == 0) {
		*finalSignal = 1;
	}

    return 0;
}

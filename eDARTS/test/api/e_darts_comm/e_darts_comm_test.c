#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "e-lib.h"
#include "codelet.h"
#include "codeletsQueue.h"
#include "threadedProcedure.h"
#include "tpClosuresQueue.h"
#include "e_darts_rt.h"
#include "e_darts_mailbox.h"
#include "e_DARTS.h"

extern codelet_t _dartsFinalCodelet;
extern commSpace_t _dartsCommSpace;

void fake_SU() {
   int tmp = e_darts_receive_signal(); 
   while (tmp != 1) {
       //e_darts_print("delaying\n");
       e_darts_print_SU_queue();
       e_darts_print_NM_queue();
       int i=0;
       while(i<100000000) i++;
       tmp = e_darts_receive_signal();
   }
   tmp = 2;
   e_darts_send_signal(&tmp);
}

int main(void)
{
    //e_ctimer_set(E_CTIMER_0, E_CTIMER_MAX); //sets timer zero to max value since its going to count downwards
    //e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK); //start timer zero to record clock cycles (decrements on event)
    unsigned base0_0Address = (unsigned) e_get_global_address(0, 0, 0x0000);

    e_darts_cam_t CAM = {SU, CU, CU ,CU,\
	                     CU, CU, CU, CU,\
                         CU, CU, CU, CU,\
			             CU, CU, CU, CU};
    e_darts_rt(CAM, CU_ROUND_ROBIN, SU_ROUND_ROBIN);
    if (e_group_config.core_row == 0 && e_group_config.core_col == 0) {
        e_darts_print("initializing comm queue\n");
        e_darts_comm_init();
        e_darts_print("NMtoSU @ %x\n", &(_dartsCommSpace.NMtoSU));
        fake_SU();
    }
    //e_darts_run();
    unsigned clock_cycles = E_CTIMER_MAX - e_ctimer_stop(E_CTIMER_0);
    int row = e_group_config.core_row;
    int col = e_group_config.core_col;
    //e_darts_print("core (%d,%d) reports %d cycles\n", row, col, clock_cycles);
    return 0;
}

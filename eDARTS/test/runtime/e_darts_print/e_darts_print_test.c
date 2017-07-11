#include <stdio.h>
#include <stdlib.h>
#include "e_darts_print.h"
#include "e-lib.h"

#define FINAL_BARRIER 0x2008

int main(void)
{
    unsigned * finalBarrier = (unsigned *) FINAL_BARRIER;
    e_darts_print("Hello world ... %d %f %d \n",e_group_config.core_row, 333.33,e_group_config.core_col);
    int i = 0 ;
    while (i++ < 1000);
    *finalBarrier = 1;
    return 0;
}

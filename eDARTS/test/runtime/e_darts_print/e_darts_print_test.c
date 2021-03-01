#include <stdio.h>
#include <stdlib.h>
#include "e_darts_print.h"
#include "e-lib.h"

#define FINAL_BARRIER 0x2008

void vectorTesting();

int main(void)
{
    unsigned * finalBarrier = (unsigned *) FINAL_BARRIER;
    e_darts_print("Hello world ... %d %f %d \n",e_group_config.core_row, 333.33,e_group_config.core_col);
    vectorTesting();
    int i = 0 ;
    while (i++ < 1000);
    *finalBarrier = 1;
    return 0;
}

void vectorTesting()
{
    float vector[8];
    for (int i=0; i<8; i++) {
        vector[i] = 1.0;
    }
    e_darts_print("[ %f %f %f %f %f %f %f %f ]\n", vector[0], vector[1], vector[2], vector[3], vector[4], vector[5], vector[6], vector[7]);
    e_darts_print("vector addresses: [ %x %x %x %x %x %x %x %x ]\n", &(vector[0]), &(vector[1]), &(vector[2]), &(vector[3]), &(vector[4]), &(vector[5]), &(vector[6]), &(vector[7]));
}

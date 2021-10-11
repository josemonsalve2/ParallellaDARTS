#include <stdio.h>
#include <stdlib.h>
#include "e_darts_print.h"
#include "e-lib.h"
#include "e_darts_mutex.h"

#define FINAL_BARRIER 0x2008

void vectorTesting();

int main(void)
{
    unsigned * finalBarrier = (unsigned *) FINAL_BARRIER;
    float single = 241.3333;
//    e_darts_print("Hello world ... %d %f %d \n",e_group_config.core_row, 333.33,e_group_config.core_col);
    //e_darts_print("Hello world ... %d %f %d \n",e_group_config.core_row, (float)single,e_group_config.core_col);
    float test = 333.33;
    float zero = 0.0;
    e_darts_print("Hello world ... %f %d %i %d %u %G\n", 120.758, e_group_config.core_row, e_group_config.core_col, -2, 2147483642, (double)333.33);
    //e_darts_print("Hello world ...  %e %e %d %d %d %d\n", 333.33, 5.5, 2147483642, e_group_config.core_row, e_group_config.core_col, -2);
    //e_darts_print("Hello world ... %d %d %f \n",e_group_config.core_row, e_group_config.core_col, test, zero);
    int i = 0;
    while (i++ < 10000);
    vectorTesting();
    i = 0 ;
    while (i++ < 1000);
    *finalBarrier = 1;
    return 0;
}

void vectorTesting()
{
    double vector[8];
    for (int i=0; i<8; i++) {
        vector[i] = i * 33.3333 + 12.10023;
    }
    e_darts_print(" %f %f %f %f %f %f %f %f \n", vector[0], vector[1], vector[2], vector[3], vector[4], vector[5], vector[6], vector[7]);
  //  e_darts_print("vector addresses: [ %x %x %x %x %x %x %x %x ]\n", &(vector[0]), &(vector[1]), &(vector[2]), &(vector[3]), &(vector[4]), &(vector[5]), &(vector[6]), &(vector[7]));
}

#include <stdio.h>
#include <stdlib.h>
#include "e_darts_print.h"

#define FINAL_BARRIER 0x2008

int main(void)
{
    unsigned * finalBarrier = (unsigned *) FINAL_BARRIER;
    e_darts_print("aaaa1...%d %f %d  ***\n",2, 333.33,3);
    e_darts_print("bbbb1\n");
    e_darts_print("cccc3...%d  %d ***\n",255, -3);
    e_darts_print("dddd4...%d  %d  ***\n",255, 444);
    e_darts_print("wwww5...%d  %d  ***\n",255, 444);
    int i = 0 ;
    while (i++ < 10000);
    *finalBarrier = 1;
    return 0;
} 
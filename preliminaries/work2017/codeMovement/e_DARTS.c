#include <stdio.h>
#include <stdlib.h>
#include "e-lib.h"
#include "common.h"
#include "codeletsQueue.h"
#include "e_DARTS.h"


/*
 * This is a testing function with no dependencies.
 *
 */
void sum()
{
    unsigned *value;
    value = (unsigned *) 0x2228;
    *value = 999;
}

int main(void)
{
    // Identify SU
    if (e_group_config.core_row == 0 && e_group_config.core_col == 0) 
    {
        _SU_rt();
    }
    else
    {
        _CU_rt();
    }
    return 0;
}

//Firts attempt to an scheduling unit in Parallella
//
//

#include <stdio.h>
#include <stdlib.h>
#include "e-lib.h"
#include "common.h"


void sum()
{
    unsigned *value;
    value = (unsigned *) 0x2028;

    (*value) ++;
}

int main(void)
{
    int i=0;
    unsigned *queue;
    unsigned *value;


    //for local memory
    queue = (unsigned *) 0x2000;
    value = (unsigned *) 0x2028;

    //for global memory of the CUs
    unsigned *cuWorkFlag, *cuCodelet, *cuDone, *suAddress;
    unsigned cuAddress ;
    suAddress = (unsigned *) 0x2004 ;
    cuAddress = (unsigned) e_get_global_address( 0 , 1 , 0x0000 );
    *suAddress = (unsigned)  e_get_global_address( 0 , 0 , 0x0000 );
    cuWorkFlag = (unsigned *) (cuAddress + (unsigned) 0x2004);
    cuCodelet = (unsigned *)  (cuAddress + (unsigned) 0x2008);
    cuDone =    (unsigned *)  (cuAddress + (unsigned) 0x2000);
     

    for (i = 0 ; i < 10; i++)
    {
        while (*cuWorkFlag != 0);
        *cuCodelet = (unsigned) ( *suAddress + ((unsigned) &sum));
        *cuWorkFlag=1;
    }
    
    *cuDone=1;
    
    
    

    return 0;
}


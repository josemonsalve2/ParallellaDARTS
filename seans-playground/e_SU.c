//Firts attempt to a scheduling unit in Parallella
//
//

#include <stdio.h>
#include <stdlib.h>
#include "e-lib.h"
#include "common.h"


/* Internal Memory Banks for eCores
 *  Bank 0  ->  0x00000000+
 *  Bank 1  ->  0x00002000+     // data
 *  Bank 2  ->  0x00004000+
 *  Bank 3  ->  0x00006000+
 * Reserved Internal Memory
 *  Rerserved   ->  0x00008000+
 * Memory-Mapped Registers
 *  Registers   ->  0x000F0000 to 0x00100000
 */



/* This is a local function to SU.
 *  SU loads this function into CU's local memory,
 *  then CU executes it within its own local memory.
 *      Therefore, CU reads and increments its own data variable 'value'
 */
void sum()
{
    unsigned *value;
    value = (unsigned *) (DATA_OFFSET + 0x0028); // share memory or local memory address?
    (*value) ++; // increment value at the address
}


int main(void)
{
    int i=0;
//    unsigned *queue;        // queue of codelets to execute
//    unsigned *value;


    //for local memory
//    queue = (unsigned *) 0x2000;
//    value = (unsigned *) 0x2028;

    //for global memory of the CU's variables and SU's global address
    unsigned *cuWorkFlag, *cuCodelet, *cuDone;
    unsigned cuAddress, suAddress;

    //get SU's 32-bit absolute base global address from its local base address
    suAddress = (unsigned)  e_get_global_address( 0 , 0 , 0x0000 );
    //get CU's 32-bit absolute base global address from its local base address
    cuAddress = (unsigned) e_get_global_address( 0 , 1 , 0x0000 );

    cuDone =    (unsigned *)  (cuAddress + (unsigned) (DATA_OFFSET + 0x0000));  //see e_CU.c
    cuWorkFlag = (unsigned *) (cuAddress + (unsigned) (DATA_OFFSET + 0x0004));  //see e_CU.c
    cuCodelet = (unsigned *)  (cuAddress + (unsigned) (DATA_OFFSET + 0x0008));  //see e_CU.c
     
 
    //Run 25000 codelets on CU back-to-back, each codelet increments a counter
    for (i = 0 ; i < 25000; i++)
    {
        while (*cuWorkFlag != 0); // Wait for CU to finish work
        *cuCodelet = (unsigned) ( suAddress + ((unsigned) &sum));   //set CU's codelet queue
        *cuWorkFlag=1;                                   //let CU know there is work to be done
    }
    
    *cuDone=1;      //CU has done all its work 
    

    return 0;
}


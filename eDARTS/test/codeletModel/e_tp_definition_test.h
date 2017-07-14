#include "threadedProcedure.h"
#include "tpClosure.h"
/**
 * Threaded Procedure defined in this file
 *  /---------------------------\
 *  | MY_TEST(a,b)              |
 *  | DRAM                      |
 *  |   int num_a;              |
 *  |   int num_b;              |
 *  |                           |
 *  | INTERNAL                  |
 *  |   int num_c;              |
 *  |   int num_d;              |
 *  |                           |
 *  | SYNC_SLOT                 |
 *  |   0 PRINT()               |
 *  |   1 ADD_DRAM_TO_C()       |
 *  |   2 ADD_DRAM_TO_D()       |
 *  |   3 MULT_C_AND_D_TO_C()   |
 *  |                           |
 *  | DEP_GRAPH                 |
 *  |                           |
 *  |     1               2     |
 *  |      \             /      |
 *  |       \---- 3 ----/       |
 *  |             |             |
 *  |             0             |
 *  \---------------------------/
 *
 */
// DEFINING THE MEMORY REGIONS

DEFINE_TP_MEM_REGIONS(my_test,
                      // DRAM
                      int a;
                      int b;
                      ,// INTERNAL
                      int c;
                      int d;
                      ,// DIST
                      )

//DEFINE_THREADED_PROCEDURE(tpname,num sync slots, initialization code, arguments
DEFINE_THREADED_PROCEDURE(my_test,4, {
                      this->memDRAM->a = a;
                      this->memDRAM->b = b;
                      }
                      , int a, int b);

DEFINE_TP_CLOSURE(my_test,int,int);



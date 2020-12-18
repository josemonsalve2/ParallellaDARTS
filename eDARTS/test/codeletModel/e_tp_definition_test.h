#include "threadedProcedure.h"
#include "tpClosure.h"
#include "e_darts_print.h"
#include "e_darts_global.h"
#include "codelet.h"
#include "e_darts_barrier.h"
#include "syncSlot.h"

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



// SOME VALUES SINCE THERE IS NO RUNTIME YET
#define TP_CLOSURE_LOCATION 0x6060
#define TP_LOCATION 0x6060
#define LOCAL_MEM_LOCATION 0x7000
#define EXTERNAL_MEM_LOCATION 0x8f000000

// DECLARING CODELETS

// DEFINING THE CODELETS
// codelets must be defined first so they can be added on the constructor
void print_codelet();
void add_dram_to_c();
void add_dram_to_d();
void mult_c_and_d_to_c();
//

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

//DEFINE_THREADED_PROCEDURE(tpname,num sync slots, initialization code, arguments of constructor)
// there are four elements created. 1) this, a pointer to the TP,
//                                  2) memDRAM, a pointer to the DRAM section,
//                                  3) memLocal a pointer to the local section
//                                  4) memDist a pointer to the distributed section
DEFINE_THREADED_PROCEDURE(my_test,4, {
                      e_darts_print("Initializing  ... user constructor \n");
                      memDRAM->a = a;
                      memDRAM->b = b;
                      ASSIGN_SYNC_SLOT_CODELET(*this,0,print_codelet,1,1,1);
                      ASSIGN_SYNC_SLOT_CODELET(*this,1,add_dram_to_c,0,0,1);
                      ASSIGN_SYNC_SLOT_CODELET(*this,2,add_dram_to_d,0,0,1);
                      ASSIGN_SYNC_SLOT_CODELET(*this,3,mult_c_and_d_to_c,2,2,1);
                      }
                      , int a, int b);

//DEFINE_TP_SYNCSLOTS_NAMES(my_test,print_sl,add_dram_sl,add_dram2_sl,mult_sl)

DEFINE_TP_CLOSURE(my_test,int,int);


// DEFINING THE CODELETS
// codelets must be defined first so they can be added on the constructor
void print_codelet(){
    e_darts_print(" PRINT_CODELET EXECUTED \n");
    // The runtime must provide a way to obtain this context
    // for now we know the TP location
    _tp_metadata_t * actualTP = (_tp_metadata_t *) TP_LOCATION;
    my_test_memDRAM_t * memDRAM = (my_test_memDRAM_t * ) actualTP->memDRAM;
    my_test_memLocal_t * memLocal = (my_test_memLocal_t * ) actualTP->memLocal;
    e_darts_print("DRAM VALUES = \n a=%d, b=%d \nLOCAL VALUES = \n c=%d, d=%d \n",memDRAM->a,memDRAM->b,memLocal->c,memLocal->d);

}

void add_dram_to_c()
{
    e_darts_print(" ADD_DRAM_TO_C EXECUTED \n");
    // The runtime must provide a way to obtain this context
    // for now we know the TP location
    // The runtime must provide a way to obtain this context
    // for now we know the TP location
    _tp_metadata_t * actualTP = (_tp_metadata_t *) TP_LOCATION;
    my_test_memDRAM_t * memDRAM = (my_test_memDRAM_t * ) actualTP->memDRAM;
    my_test_memLocal_t * memLocal = (my_test_memLocal_t * ) actualTP->memLocal;
    // Adding
    memLocal->c = memDRAM->a + memDRAM->b;

    // Signaling the multiplier
    syncSlot_t * multSyncSlot = GET_SYNC_SLOT(*actualTP, 3);
    syncSlotDecDep(multSyncSlot);
}
void add_dram_to_d()
{
    e_darts_print(" ADD_DRAM_TO_D EXECUTED \n");
    // The runtime must provide a way to obtain this context
    // for now we know the TP location
    _tp_metadata_t * actualTP = (_tp_metadata_t *) TP_LOCATION;
    my_test_memDRAM_t * memDRAM = (my_test_memDRAM_t * ) actualTP->memDRAM;
    my_test_memLocal_t * memLocal = (my_test_memLocal_t * ) actualTP->memLocal;
    // Adding
    memLocal->d = memDRAM->a + memDRAM->b;

    // Signaling the multiplier
    syncSlot_t * multSyncSlot = GET_SYNC_SLOT(*actualTP, 3);
    syncSlotDecDep(multSyncSlot);
}
void mult_c_and_d_to_c()
{
    e_darts_print(" MULT_C_AND_D_TO_C EXECUTED \n");
    // The runtime must provide a way to obtain this context
    // for now we know the TP location
    _tp_metadata_t * actualTP = (_tp_metadata_t *) TP_LOCATION;
    my_test_memDRAM_t * memDRAM = (my_test_memDRAM_t * ) actualTP->memDRAM;
    my_test_memLocal_t * memLocal = (my_test_memLocal_t * ) actualTP->memLocal;
    // Multiplying
    memLocal->c = memLocal->c + memLocal->d;

    // Signaling the printer
    syncSlot_t * printSyncSlot = GET_SYNC_SLOT(*actualTP, 0);
    syncSlotDecDep(printSyncSlot);

}
//

/**
 * @file threadedProcedure.h
 * @author Jose M Monsalve
 * @date 26 May 2017
 * @brief Definition of the synchronization slots
 * @todo Add copyright
 *
 * Codelets are the unito of computation of the codelet model.
 * A codelet represents a task and it is a non-preemtible segment
 * of code.
 * A codelet can be waiting for dependencies, enabled, ready or fired.
 * Enabled codelets have all their dependencies satisfied, but have not been
 * allocated to a CU nor the code prefetched. ready codelets are allocated
 * but have not been scheduled for execution, while fired codelets are codelets
 * that are executing or have been already executed
 * A codelet has an ID that can be used as reference within the computation
 * A codelet always belongs to a syncronization slot, which is where
 * dependencies are kept. A syncronization slot might fire multiple
 * codelets.
 * A codelet can use the syncslot to obtian the TP frame
 */

#ifndef THREADED_PROCEDURE_H
#define THREADED_PROCEDURE_H

#include "e_darts_mutex.h"
#include "tpClosure.h"
#include "syncSlot.h"

/**
 *
 * Threaded procedures are structs that contain the data for the
 * codelet execution in adition to frame information to obtain such data
 * It has some initial metadata that is stored in a known location
 * within the SU. This structure represents that metadata.
 * Additionally there are three pointers to location in memory that are
 * dependent on the user defined TP. The idea is that each pointer points to
 * different memory regions (i.e. internal, external, distributed)
 * The SU is in charge of allocating these memory regions and assign
 * its location to these pointers. After The metadata, as many sync slots
 * as the user specified are defined. However, these syncslots might be
 * located somewhere else in memory. for example in other CUs.
 *
 * TP memory looks like this
 * |-----------------------------|
 * |  __tp_metadata_t METADATA   |
 * -------------------------------
 * |     syncSlots *pointer      |
 * |     syncSlots *pointer      |
 * |     syncSlots *pointer      |
 * |     syncSlots *pointer      |
 * |                             |
 * | ...As many as syncSlots...  |
 * -------------------------------
 *
 * In addition, a user defined TP will not be created until scheduled
 * my the TP scheduler. The user can push TP creation request by invoking
 * TPs. Invoking to a TP implies pushing a recipe of how to create a new
 * TP (what are the arguments, and what is the initialization function)
 * This recipe is known as the tpClosure. Actual creation of the TP happens
 * when a tpClosure is popped. Creating a TP has four steps
 *      1. Metadata is created and allocated within the SU
 *      2. Memory regions are created and assigned to the pointer
 *      3. SyncSlots are assigned to either the SU or other CUs
 *      3. User initialization code is called.
 *
 * A TP has a fixed number of syncSlots that is assigned in definition
 * of the TP. However, within the initialization code the user must
 * specify what are the codelets associated to which particular SyncSlot
 * knowing the corresponding syncSlot number. the macro DEFINE_TP_SYNCSLOTS_NAMES
 * is used to make the programmer's life easier wrt remembering this
 * association. However, it is important to remember the order in which it is assigned
 * during initialization code
 *
 *
 */

/**
 * @brief Threaded Procedure Metadata
 *
 * Basic metadata all TP must have
 *
 */
typedef struct __attribute__ ((__packed__)) _tp_metadata_s {
    unsigned _TPid;
    unsigned numSyncSlots;
    unsigned refCount;
    unsigned sizeDRAM;
    unsigned sizeLocal;
    unsigned sizeDist;
    darts_mutex_t lock;
    void *memDRAM;
    void *memLocal;
    void *memDist;
} _tp_metadata_t;

/**
 *  @brief Constructor for tp metadata
 *
 *  This function also allows to have the code in the SU at all
 *  time.
 *
 *  @TODO place this function within the SU local memory
 */

_tp_metadata_t _genericMetadataCtro(unsigned _TPid,
                    unsigned numSyncSlots, unsigned sizeDRAM,
                    unsigned sizeLocal, unsigned sizeDist);

/* --------------- HELPER MACROS --------------- */
/**
 * @brief This macro is to get a pointer to a syncSlot that is within the
 * TP memory region as explained above. All SyncSlots are placed together.
 * the user is responsible to assign which syncSlot corresponds to what codelet
 *
 */
#define GET_SYNC_SLOT(_Tp, numSyncSlot) \
        ( (syncSlot_t *) (((unsigned)&(_Tp)) + sizeof(_tp_metadata_t) + sizeof(syncSlot_t)*numSyncSlot)) \
/**
 * @brief Asign a codelet to a syncronization slot
 *
 * This function finds the corresponding syncronization slot give the
 * syncSlotNum within the __Tp. __Tp is the actual element since it is dereferenced here.
 * provide a codeletFunction, number of initial dependencies (numDep), a number of reset
 * dependencies (resetDep), and a number of codelets created whenever this syncSlot is ready.
 *
 */
//note: added tpFrame assignment, and full syncSlot address w/ core ID - SU location is hardcoded currently
#define ASSIGN_SYNC_SLOT_CODELET(_Tp, syncSlotNum, codeletFunction, numDep, resetDep, numExec) \
    {\
        syncSlot_t* theSyncSlot = GET_SYNC_SLOT(_Tp,syncSlotNum);\
        codelet_t newCodeletTemplate;\
        theSyncSlot->tpFrame = (_tp_metadata_t *) DARTS_APPEND_COREID(0x808,this);\
        syncSlot_t* syncSlot_full_address = (syncSlot_t*) DARTS_APPEND_COREID(0x808,theSyncSlot);\
        initCodelet(&newCodeletTemplate,0 ,syncSlot_full_address, codeletFunction);\
        initSyncSlot(theSyncSlot, syncSlotNum, resetDep, numDep, newCodeletTemplate, numExec);\
    }\


/* --------------- TP DEFINITIONS --------------- */
//Memory regions
#define DEFINE_TP_MEM_REGIONS(TPname, memRegionDRAM, memRegionLocal, memRegionDist)\
    typedef struct __attribute__((__packed__)) {\
        memRegionDRAM\
    } TPname##_memDRAM_t;\
    typedef struct __attribute__((__packed__)) {\
        memRegionLocal;\
    } TPname##_memLocal_t;\
    typedef struct __attribute__((__packed__)) {\
        memRegionDist;\
    } TPname##_memDist_t;\

//Syncslots name
#define DEFINE_TP_SYNCSLOTS_NAMES(TPname,...)\

// Threaded procedure
#define DEFINE_THREADED_PROCEDURE(...) DEFINE_THREADED_PROCEDURE_ARGS (__VA_ARGS__)
#define DEFINE_THREADED_PROCEDURE_ARGS(TPname, _numSyncSlots, initializationCode, ...)\
    typedef struct __attribute__((__packed__)) TPname##_threadedProcedure_s {\
        _tp_metadata_t metadata;\
    }TPname##_threadedProcedure_t;\
    \
    typedef TPname##_threadedProcedure_t TPname;\
    \
    _tp_metadata_t _##TPname##_metadataCtor(unsigned _TPid) {\
        return _genericMetadataCtro(_TPid, _numSyncSlots, \
                sizeof(TPname##_memDRAM_t), \
                sizeof(TPname##_memLocal_t),\
                sizeof(TPname##_memDist_t));\
    }\
    \
    void _##TPname##_userInitCtor(_tp_metadata_t * _tp, ##__VA_ARGS__) {\
        TPname##_threadedProcedure_t * this = (TPname##_threadedProcedure_t *) _tp;\
        TPname##_memDRAM_t *memDRAM = (TPname##_memDRAM_t *) _tp->memDRAM;\
        TPname##_memLocal_t *memLocal = (TPname##_memLocal_t *) _tp->memLocal;\
        TPname##_memDist_t *memDist = (TPname##_memDist_t *) _tp->memDist;\
        \
        initializationCode;\
        \
    }\
    \

//TP closure
#define DEFINE_TP_CLOSURE(...) \
        DEFINE_TP_CLOSURE_ARGS(__VA_ARGS__)\

#endif /* THREADED_PROCEDURE_H */

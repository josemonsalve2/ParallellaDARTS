/**
 * @file tpClosuresQueue.h
 * @author Jose M Monsalve
 * @date 15 July 2017
 * @brief File defining the concurrent tpClosuresQueue_t struct.
 * @todo Add copyright
 *
 * tpClosuresQueue_t is an structure that implements a concurrent queue
 * that contains the tpClosures. This queue uses a mutex to allow
 * multiple cores to push and pop from the queue. This asset is intended
 * to be part of the implementation of a TP scheduler that exist within the
 * Scheduling Units (SUs)
 */

#ifndef TP_CLOSURE_QUEUE_H
#define TP_CLOSURE_QUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tpClosure.h"
#include "e_darts_mutex.h"

// Return codes
#define TPC_QUEUE_SUCCESS_OP 0
#define TPC_QUEUE_INVALID_QUEUE_ADDRESS 1
#define TPC_QUEUE_NOT_ENOUGH_SPACE 2
#define TPC_QUEUE_EMPTY_QUEUE 3
#define TPC_QUEUE_NOT_OWNER 4

/**
 * @brief tpClosure queue struct
 *
 * Elements of this queue are not equidistant from each other.
 * The number of elements that the queue can fit also depends on the
 * type of tpClosures that are stored, since closures have different
 * number and size of arguments, it is not possible to know how many
 * tpClosures will fit. Size is given in bytes.
 *
 * tpClosures are recipes to build threadedProcedures. This recepies
 * contain a pointer to the tpConstructor defined by the user. This queue
 * allows the SU to read new work provided by any other core.
 *
 * It is important to know that this queue does not support communication
 * with the host given that mutex only work for internal memory.
 *
 * The owner is the only one that can peak and delete the element that is
 * at the head of the queue. The reason of this mechanism is because the
 * SU that wants to obtain an element from the queue have no clue how many
 * arguments and how much space they take, so it will not know how much space
 * to reserve when sending a pointer. If returned by a pop function, we will need
 * to know the type of the original tpClosure in order to properly return the
 * arguments of the tpClosure. To avoid this we let the owner read the head of
 * the queue and use it, whenever the owner is done with this element, it is his
 * responsibility to delete this element before releasing ownership of the queue.
 *
 * Ownership guarantees that the top element in the queue is not going to be deleted
 * or modified by somebody else. However, it does not prevent others from pushing
 * elements to the queue.
 *
 *
 * Memory organization of a tpClosuresQueue
 *
 * /--------------------------------\
 * | int owner;                     |
 * | void * headAddress;            |
 * | void * tailAddress;            |
 * | unsigned size;                 |
 * | unsigned padding;              |
 * | darts_mutex_t lockMutex;       |
 * |--------------------------------|
 * |         QUEUE RESERVED         |<-- initPosition = tpClosureQueueAddress + 4*4
 * |             SPACE              |
 * |                                |
 * |********************************|<-- headAddress
 * |********************************|
 * |********************************|
 * |*********QUEUE CONTENT**********|
 * |********************************|
 * |********************************|
 * |********************************|
 * |********************************|
 * |********************************|<-- tailAddress
 * |                                |
 * |         QUEUE RESERVED         |
 * |             SPACE              |
 * |                                |<-- initPosition + size;
 * \--------------------------------/
 *
 */

typedef struct __attribute__((__packed__)) tpClosuresQueue_s {
    void * headAddress;
    void * tailAddress;
    unsigned padding;
    unsigned size; // in bytes
    darts_mutex_t lockMutex; //
    unsigned queueOwner; //coreId of current owner
} tpClosuresQueue_t;

/**
 * @brief Initialization function for a tpClosureQueue
 *
 * @warning @p newHeadAddress must be global if you want to be able to access the
 * queue from other cores
 *
 * @p queue location pointer. Use full address pointer (with coreId)
 * @p newSize The reserved space size
 *
 * @return SUCCESS_OP if successfully initialized. Or INVALID_QUEUE_ADDRESS if address does not contain coreId
 *
 */
unsigned initTpClosuresQueue(tpClosuresQueue_t * queue, unsigned int newSize);

/**
 * @brief push a tpClosure to the queue
 *
 * push a new TP to the queue. It uses the sizeOfArguments within the tpClosure to copy
 * the arguments to the queue. It updates queue accordingly. It will only push into the queue
 * if the available space is enough. Otherwise it will return 2. It is guaranteed that the content will
 * be stored in a consecutive memory space. If a new tpClosure does not fit between the tail and the end
 * point of the reserved space, this region will be stored as padding and it won't be used. The new tpClosure
 * will be added to the initPosition. Padding will be used again whenever the head reaches the endPoint of the
 * memory space.
 *
 * @p queue queue location. Use full address with coreID, otherwise it will return 1
 * @p newTpClosure. Pointer to the tpClosure to insert. It must be a reference.
 * @return 0 element was inserted normally, 1 queue address did not contain coreID, 2 not enough space to insert
 * the new element
 */
unsigned pushTpClosureQueue (tpClosuresQueue_t * queue, genericTpClosure_t * newTpClosure);

/**
 * @brief Owning tpClosureQueue. Only the owner can pop elements.
 *
 * In order to read an element from the queue safely, it is necessary to own the queue.
 * Ownership guarantees that nobody will remove or take the top element in the queue.
 * The caller must be in charge of using pop or popAndDisown to remove this
 * element from the queue after using it
 *
 * @p queue This is the tpClosure queue to own
 */
unsigned ownTpClosureQueue (tpClosuresQueue_t * queue);

/**
 * @brief disowning tpClosureQueue.
 *
 * In order to read an element from the queue safely, it is necessary to own the queue.
 * Ownership guarantees that nobody will remove or take the top element in the queue.
 * The caller must be in charge of using pop or popAndDisown to remove this
 * element from the queue after using it. This function just removes ownership of the queue
 *
 * @p queue This is the tpClosure queue to own
 */
unsigned disownTpClosureQueue (tpClosuresQueue_t * queue);

/**
 * @brief peak top element of tpClosureQueue.
 *
 * This function assigns a pointer to the top element of the queue if the caller
 * is the owner of the queue. Otherwise the pointer will be assigne to 0, and
 * NOT_OWNER will be returned
 *
 * @p queue Queue from where to obtain the top element pointer
 * @p topElement Pointer that will be modified with the address of the top element
 */
unsigned peakTopElement(tpClosuresQueue_t * queue, genericTpClosure_t ** topElementPointer);

/**
 * @brief pop top element from the tpClosureQueue
 *
 * This function pops the top element of the queue. However this element is not returned, it
 * is just removed from the queue You must be the owner in order to pop an element from the queue
 *
 * @p queue The queue from where to pop the top element
 *
 * @return SUCCESS_OP if element was popped correctly. NOT_OWNER if the caller is not the owner of
 * this queue
 */
unsigned popTopElementQueue (tpClosuresQueue_t * queue);

/**
 * @brief pop top element from the tpClosureQueue and disown the queue
 *
 * This function pops the top element of the queue. However this element is not returned, it
 * is just removed from the queue You must be the owner in order to pop an element from the queue.
 * After popping the element the ownership of the queue is removed as well.
 *
 * @p queue The queue from where to pop the top element
 *
 * @return SUCCESS_OP if element was popped correctly. NOT_OWNER if the caller is not the owner of
 * this queue
 */
unsigned popTopElementAndDisownQueue (tpClosuresQueue_t * queue);

/**
 * @brief is queue empty?
 *
 * check if the queue is empty
 *
 * \p queue codelet queue to check
 *
 * @return 1 if queue is empty 0 if queue is not empty
 */

unsigned isTpClosureQueueEmpty(tpClosuresQueue_t * queue);

#endif /* TP_CLOSURE_QUEUE_H */

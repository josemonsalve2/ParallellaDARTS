/**
 * @file e_darts_mutex.h
 * @author Jose M Monsalve
 * @date 2 June 2017
 * @brief own implementation of mutex based on the e_SDK mutex implementation
 * @todo Add copyright
 *
 * Mutex are required in several parts of the runtime. 
 * It is required to always have the full address of 
 * the mutex in order to lock it or unlock it from anywhere 
 * in the chip
 *
 * This file borrows heavily from the original eSDK 
 *
 */

typedef int  darts_mutex_t;
#define DARTS_MUTEX_NULL (0)

/**
 * @brief Locking a mutex given its location
 *
 * @param
 *
 */

void darts_mutex_lock( e_mutex_t *mutex, unsigned baseAddr);

void darts_mutex_unlock( e_mutex_t *mutex, unsigned baseAddr);

/**
 * @file e_darts_global.h
 * @author Jose M Monsalve
 * @date 7 August 2017
 * @brief CU definitions
 * @todo Add copyright
 *
 * Definitions that should persist between different files that are not
 * part of a particular part of the runtime, but of all of it
 *
 */

#ifndef E_DARTS_GLOBAL_H
#define E_DARTS_GLOBAL_H

// Flag that keeps the runtime going
extern unsigned darts_rt_alive;

#define DARTS_GETCOREID(coreIdVar) __asm__ __volatile__ ("movfs %0, coreid" : "=r" (coreIdVar))
#define DARTS_APPEND_COREID(coreID, address) ((((unsigned)coreID) << 20) + (((unsigned)address) & 0x000FFFFF))

#endif /* E_DARTS_GLOBAL_H */

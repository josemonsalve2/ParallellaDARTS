/**
 * @file e_darts_cam.h
 * @author Jose M Monsalve
 * @date 8 July 2017
 * @brief Codelet Abstract Machine
 * @todo Add copyright
 * @todo add printing of variables
 *
 * Codelet abstract machine definitions. The codelet abstract machine
 * is defined as an array of chars where each char represents a single core.
 * Since the epiphany is a mesh we assign cores in a row major scheme.
 * e.g. [core(0,0) core(0,1) core(0,2) core(0,3) core(1,0) core(1,1) ...]
 *
 * Each entry in the array can have one of three values.
 *  - 0                 -> SU
 *  - FF                -> Not used
 *  - Any other value   -> CU, where the value represents the corresponding SU
 *
 */

#ifndef E_DARTS_CAM_H
#define E_DARTS_CAM_H

#include "darts_rt_params.h"

#define DARTS_CAM_SU 0
#define DARTS_CAM_CU(SUVAL) SUVAL
#define DARTS_CAM_NOT_USED 255

typedef enum core_type {
    SU = 0,
    CU = 1,
    NA = 2
} coreType;

//typedef char e_darts_cam_t[_DARTS_NUM_CORES];
typedef coreType e_darts_cam_t[_DARTS_NUM_CORES]; //this should be row major directly correlating to the core

#endif

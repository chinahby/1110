#ifndef _DRIFTCORRECTION_H_
#define _DRIFTCORRECTION_H_
/* =============================================================================
                                driftcorrection.h

DESCRIPTION
  Header file for driftcorrection.c.  See description of driftcorrection.c

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Portions copyrighted by QUALCOMM Incorporated;
Copyright 2007 QUALCOMM Incorporated, All Rights Reserved
============================================================================= */

/* =============================================================================

                               EDIT HISTORY FOR FILE

$Header: //source/qcom/qct/multimedia/qtv/player/avsync/main/latest/src/driftcorrection.h#8 $
$DateTime: 2008/05/14 16:54:15 $
$Change: 659855 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/5/06    ateng   Initial version
============================================================================= */

/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "comdef.h"

#define NO_FILTER              0
#define AVG_FILTER             1
#define IIR_FILTER             2
#define FIR_FILTER             3
#define BASIC_FILTER_0         4
#define BASIC_FILTER_1         5
#define MOVING_AVG_FILTER      6

#define PCR_FILTER_ID          0
#define DEC_CLK_FILTER_ID      1
#define ARM_CLK_FILTER_ID      2
#define UNDEF_FILTER_ID        3
#define NUM_FILTERS            3

#define COLD_RESET             0
#define WARM_RESET             1

typedef void *dc_handle_type;

typedef int64 iDWORD;
typedef uint64 uDWORD;
#define NUM_UDW_BITS 64
#define DRIFTING_RATE_UNIT_SCALE 1000000.0f

int32 driftingCorrectionInit(dc_handle_type *dcHdl,
                             uint8 driftFilterType[],
                             uint32 ui32NumCoeffs,
                             uint16 ui16DCDRStep,
                             uint16 ui16DCARStep,
                             uint16 ui16MaxNumCorSamples);

int32 driftingCorrectionFilterCoeffChange(dc_handle_type *dcHdl,
                                          uint32 ui32NumCoeffs,
                                          int32 *i32DriftingCoeff,
                                          int32 *i32FilteredDriftingCoeff);

int32 DCSetCorrectionThreshold(dc_handle_type *dcHdl, uint32 threshold);

int32 DCSetAvgFilterZeroPadding(dc_handle_type dcHdl,
                                 uint32 sRZeroPadding,
                                 uint32 rDZeroPadding);

int32 DCSetFilterTimeConstant(dc_handle_type dcHdl,
                              int filterID,
                              uint32 timeConstant);

int32 DCSetMovingAvgFilterMemory(dc_handle_type dcHdl,
                                 int filterID,
                                 uint32 measureInterval,
                                 uint32 numOfSamples);

iDWORD sRDriftingInput(dc_handle_type dcHdl,
                       uDWORD udwTCR,
                       uDWORD udwPCR);

iDWORD rDDriftingInput(dc_handle_type dcHdl,
                       uDWORD udwTCR,
                       uDWORD udwACR,
                       uDWORD _udwACR);

iDWORD aRDriftingInput(dc_handle_type dcHdl,
                       uDWORD udwTCR,
                       uDWORD udwACR,
                       uDWORD _udwACR);

int32 driftingCorrection(dc_handle_type dcHdl, uDWORD udwTCR, int filterID);
int32 DCFilterReset(dc_handle_type dcHdl, int filterID, int resetOption);
iDWORD driftingValuePeek(dc_handle_type dcHdl);
int32 driftingCorrectionExit(dc_handle_type dcHdl);
#endif


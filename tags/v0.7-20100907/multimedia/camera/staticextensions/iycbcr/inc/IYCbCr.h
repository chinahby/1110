#ifndef  IYCBCR_H
#define  IYCBCR_H
/*=================================================================================
FILE:          IYCbCr.h

SERVICES:      IYCbCr interface

DESCRIPTION:   The IYCbCr provides a means for accessing raw YCbCr bitmap data.

PUBLIC CLASSES:   Not Applicable

INITIALIAZTION AND SEQUENCEING REQUIREMENTS: Not Applicable

        Copyright © 2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=================================================================================*/
/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/multimedia/camera/staticextensions/iycbcr/main/latest/inc/IYCbCr.h#1 $
 $DateTime: 2008/05/09 16:46:58 $
 $Author: karenp $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------
 02/04/05  JN     Cleaned up version after code review with QIS.
============================================================================*/

#include "AEE.h"
#include "AEEBitmap.h"

//*********************************************************************************
//
// IYCbCr Interface
//
//*********************************************************************************

#define AEEIID_YCBCR 0x0102c158

// do not overlap with IDIB_COLORSCHEME
#define IYCBCR_COLORSCHEME_NONE   0xcb00   // no fixed mapping for pixel values
#define IYCBCR_COLORSCHEME_420LP  0xcb01
#define IYCBCR_COLORSCHEME_422LP  0xcb02
#define IYCRCB_COLORSCHEME_420LP  0xcb03
#define IYCRCB_COLORSCHEME_422LP  0xcb04

typedef struct IYCbCr IYCbCr;

struct IYCbCr {
   const AEEVTBL(IBitmap) *pvt;
   void *            pLuma;          // luma data
   void *            pChroma;        // chroma data
   uint32            cx;             // number of pixels in width
   uint32            cy;             // number of pixels in height
   int32             nYPitch;        // offset from one row to the next (in bytes) in Y plane
   int32             nCPitch;        // offset from one row to the next (in bytes) in C plane
   uint32            uScheme;        // IYCBCR_COLORSCHEME_...
   uint32            reserved[4];    // initialize to 0 when constructing; ignore when using
};

// This is just for type safe casting of an IYCbCr* to an IBitmap*.
static __inline IBitmap *IYCBCR_TO_IBITMAP(const IYCbCr *pIYCbCr)
{
   return (IBitmap*)(void*)pIYCbCr;
}

// IYCBCR macros
#define IYCBCR_AddRef(p)               AEEGETPVTBL(IYCBCR_TO_IBITMAP(p),IBitmap)->AddRef(IYCBCR_TO_IBITMAP(p))
#define IYCBCR_Release(p)              AEEGETPVTBL(IYCBCR_TO_IBITMAP(p),IBitmap)->Release(IYCBCR_TO_IBITMAP(p))
#define IYCBCR_QueryInterface(p,i,p2)  AEEGETPVTBL(IYCBCR_TO_IBITMAP(p),IBitmap)->QueryInterface(IYCBCR_TO_IBITMAP(p),(i),(p2))

#endif /* IYCBCR_H */


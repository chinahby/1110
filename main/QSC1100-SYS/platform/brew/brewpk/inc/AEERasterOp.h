#ifndef AEERASTEROP_H
#define AEERASTEROP_H
/*=============================================================================

FILE:          AEERasterOp.h

SERVICES:      AEERasterOp type

DESCRIPTION:

PUBLIC CLASSES:   Not Applicable

INITIALIAZTION AND SEQUENCEING REQUIREMENTS: Not Applicable

Copyright © 2001-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/

#include "AEEStdDef.h"

// Bitmap raster operation type
enum {
   AEE_RO_OR,
   AEE_RO_XOR,
   AEE_RO_COPY,
   AEE_RO_NOT,
   AEE_RO_OLDMASK,    // same as TRANSPARENT in v1.0; not supported in IBitmap
   AEE_RO_MERGENOT,
   AEE_RO_ANDNOT,
   AEE_RO_TRANSPARENT,
   AEE_RO_AND,
   AEE_RO_BLEND,
   AEE_RO_TOTAL
};

#ifdef _WIN32
typedef unsigned AEERasterOp;
#else
typedef int8 AEERasterOp;
#endif


#define AEE_RO_MASK           AEE_RO_TRANSPARENT
#define AEE_RO_MASKNOT        AEE_RO_ANDNOT

#define AEE_RO_OLDTRANSPARENT AEE_RO_OLDMASK   // soon to be deleted

/*
===============================================================================
DATA STRUCTURES DOCUMENTATION
===============================================================================

AEERasterOp

Description:
This ENUM specifies the raster operation for bit-block transfers of bitmaps,
and drawing images with the functions in the IImage Interface.

Definition:
typedef enum
{
   AEE_RO_OR,
   AEE_RO_XOR,
   AEE_RO_COPY,
   AEE_RO_NOT,
   AEE_RO_OLDMASK,    // same as TRANSPARENT in v1.0; not supported in IBitmap
   AEE_RO_MERGENOT,
   AEE_RO_ANDNOT,
   AEE_RO_TRANSPARENT,
   AEE_RO_AND,
   AEE_RO_BLEND,
   AEE_RO_TOTAL
} AEERasterOp;

#define AEE_RO_MASK           AEE_RO_TRANSPARENT
#define AEE_RO_MASKNOT        AEE_RO_ANDNOT
#define AEE_RO_OLDTRANSPARENT AEE_RO_OLDMASK

Members:
    AEE_RO_MASK is deprecated in BREW 2.1; use AEE_RO_OLDTRANSPARENT instead.
    AEE_RO_MASKNOT is deprecated in BREW 2.1; use AEE_RO_ANDNOT instead.
    AEE_RO_OR  :  SRC .OR. DST*.
    AEE_RO_XOR  :  SRC .XOR. DST*.
    AEE_RO_COPY  :  DST = SRC*.
    AEE_RO_NOT  :  DST = (!SRC)*.
    AEE_RO_OLDTRANSPARENT  :  Same as AEE_RO_TRANSPARENT. In monochrome mode it 
                              is equivalent to DST .AND. SRC*.
    AEE_RO_MERGENOT  :  DST .OR. (!SRC).
    AEE_RO_AND :  DST .AND. SRC.
    AEE_RO_ANDNOT :  DST .AND. (!SRC).
    AEE_RO_TRANSPARENT  :  The SRC* pixels with a certain color are transparent
                           meaning that the corresponding DST* pixels are seen through:
    For a monochrome device, the color is RGB_MASK_MONO, which is white.
    For a grayscale device, the color is RGB_MASK_GREY, which is white
    For a color device, the color is RGB_MASK_COLOR, which is magenta.
    AEE_RO_BLEND  :  This raster operation is used for alpha blending.  This
                     signals that the alpha channel in the source should be
                     used in coping the source color to the destination.
    AEE_RO_TOTAL  :  The total number of raster operations
    * Where SRC is the source bitmap buffer, and DST is the destination bitmap buffer.

Comments:
   None

See Also:
   None

===============================================================================
*/
#endif //AEERASTEROP_H


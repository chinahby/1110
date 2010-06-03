#ifndef  YCBCR_PRIV_H
#define  YCBCR_PRIV_H
/*=================================================================================
FILE:          YCbCr_priv.h

SERVICES:      IYCbCr interface

DESCRIPTION:   Private header file to expose YCbCr_New.

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

 $Header: //source/qcom/qct/multimedia/camera/staticextensions/iycbcr/main/latest/inc/YCbCr_priv.h#1 $
 $DateTime: 2008/05/09 16:46:58 $
 $Author: karenp $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------
 02/04/05  JN     Cleaned up version after code review with QIS.
============================================================================*/

int YCbCr_New(uint32 uScheme, uint32 cx, uint32 cy, IYCbCr **ppiYCbCr);

typedef enum {
   YCBCR_ROTATION_NONE,
   YCBCR_ROTATION_90CW,
   YCBCR_ROTATION_180,
   YCBCR_ROTATION_90CCW
} YCbCrRotationType;

typedef enum {
   YCBCR_SCALING_NONE,
   YCBCR_SCALING_0_25X,
   YCBCR_SCALING_0_5X,
   YCBCR_SCALING_0_75X,
   YCBCR_SCALING_2X,
   YCBCR_SCALING_ASCALE
} YCbCrScalingType;

typedef struct {
   IYCbCr   bmp;  // we share the vtable with bmp (IYCbCr)
   uint32   uRef;
   YCbCrRotationType  rotate;
   YCbCrScalingType   scale;
   AEEPoint ascale;       // Arbitrary scale width/height passed along
} YCbCr;

#endif /* YCBCR_PRIV_H */


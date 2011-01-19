#ifndef CAMIF_H
#define CAMIF_H
/* =========================================================================
                 CAMIF Interface

   DESCRIPTION
     This file contains the definitions needed for the camera interface
     device driver (LDD).

   Design Note:

   Copyright (c) 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/multimedia/camera/6k/sensorcommon/main/latest/inc/camif.h#1 $ $DateTime: 2008/05/09 16:34:47 $ $Author: karenp $

 when      who    what, where, why
-------------------------------------------------------------------------------
 08/28/06  jch    Fix camif skip pattern to skip up to 15 frames.
 05/18/06  dle    6260, SC2X and LCU does not have MDDI.

=============================================================================*/

typedef enum
{
  CAMIF_BAYER_G_B,
  CAMIF_BAYER_B_G,
  CAMIF_BAYER_G_R,
  CAMIF_BAYER_R_G,
  CAMIF_YCbCr_Y_Cb_Y_Cr,
  CAMIF_YCbCr_Y_Cr_Y_Cb,
  CAMIF_YCbCr_Cb_Y_Cr_Y,
  CAMIF_YCbCr_Cr_Y_Cb_Y,
  CAMIF_YCbCr_4_2_2_linepacked,    /* Only valid for Offline Snapshot */
  CAMIF_YCbCr_4_2_0_linepacked,    /* Only valid for Offline Snapshot */
  CAMIF_NumberOf_InputFormatType   /* Used for count purposes only */
} CAMIF_InputFormatType;

typedef enum
{
  CAMIF_APS = 0,
  CAMIF_EFS = 1,
  CAMIF_ELS = 2
} CAMIF_SyncModeType;

typedef enum
{
  CAMIF_High = 0,
  CAMIF_Low
} CAMIF_ActiveType;

typedef enum
{
  /* CAMIF_ALL_FRAMES0 and CAMIF_ALL_FRAMES1 are synonomous.  Both */
  /* collect all frames. */
  CAMIF_ALL_FRAMES0,
  CAMIF_ALL_FRAMES1,
  CAMIF_EVERY_OTHER_FRAME,
  CAMIF_EVERY_THIRD_FRAME,
  CAMIF_EVERY_FOURTH_FRAME,
  CAMIF_EVERY_FIFTH_FRAME,
  CAMIF_EVERY_SIXTH_FRAME,
  CAMIF_EVERY_SEVENTH_FRAME,
  CAMIF_EVERY_EIGHTH_FRAME,
  CAMIF_EVERY_NINTH_FRAME,
  CAMIF_EVERY_TENTH_FRAME,
  CAMIF_EVERY_ELEVENTH_FRAME,
  CAMIF_EVERY_TWELFTH_FRAME,
  CAMIF_EVERY_THIRTEENTH_FRAME,
  CAMIF_EVERY_FOURTEENTH_FRAME,
  CAMIF_MAX_FRAME_SKIP
} CAMIF_FrameSkipType;

typedef enum
{
  CAMIF_Every_16_Cycles,
  CAMIF_Every_12_Cycles
} CAMIF_PixelSkipWrapType;

typedef __packed struct
{
  unsigned int       reserved          :  1;
  CAMIF_ActiveType   VSyncEdge         :  1;
  CAMIF_ActiveType   HSyncEdge         :  1;
  CAMIF_SyncModeType SyncMode          :  2;
  unsigned int       VFE_SubsampleEn   :  1;
#ifdef CAMERA_NO_MDDI
  unsigned int       reserved1         :  1;
#else /* CAMERA_NO_MDDI */
  unsigned int       DMA_SubsampleEn   :  1;
#endif /* CAMERA_NO_MDDI */
  unsigned int       AHB_SubsampleEn   :  1;
  unsigned int       VFE_OutputEn      :  1;
#ifdef CAMERA_NO_MDDI
  unsigned int       reserved2         :  1;
#else /* CAMERA_NO_MDDI */
  unsigned int       DMA_OutputEn      :  1;
#endif /* CAMERA_NO_MDDI */
  unsigned int       AHB_OutputEn      :  1;
  unsigned int       INTR_SubsampleEn  :  1;
#ifdef CAMERA_NO_MDDI
  unsigned int       DataSampleEdge    :  1; /* 1: sample on rising edge */
  unsigned int       HSYNCSampleEdge   :  1; /* 1: sample on rising edge */
  unsigned int       VSYNCSampleEdge   :  1; /* 1: sample on rising edge */
  unsigned int       reserved3         : 16;
#else /* CAMERA_NO_MDDI */
  unsigned int       reserved1         : 19;
#endif /* CAMERA_NO_MDDI */
  unsigned int       MISR_En           :  1;
} CAMIF_ConfigType;

typedef __packed struct
{
  unsigned int EFS_EOL : 8;
  unsigned int EFS_SOL : 8;
  unsigned int EFS_EOF : 8;
  unsigned int EFS_SOF : 8;
} CAMIF_EFS_ConfigType;

typedef __packed struct
{
  unsigned int       pixelsPerLine : 14;
  unsigned int       linesPerFrame : 14;
  unsigned int       reserved      :  4;
} CAMIF_FrameConfigType;

typedef __packed struct
{
  unsigned int lastPixel  : 14;
  unsigned int firstPixel : 14;
  unsigned int reserved   :  4;
} CAMIF_WindowWidthConfigType;

typedef __packed struct
{
  unsigned int lastLine  : 14;
  unsigned int firstLine : 14;
  unsigned int reserved  :  4;
} CAMIF_WindowHeightConfigType;

typedef __packed struct
{
  unsigned int pixelSkip : 16;
  unsigned int lineSkip  : 16;
} CAMIF_Subsample1ConfigType;

typedef __packed struct
{
  CAMIF_FrameSkipType     frameSkip      : 4;
  CAMIF_PixelSkipWrapType PixelSkipWrap  : 1;
  unsigned int          reserved       :27;
} CAMIF_Subsample2ConfigType;

typedef __packed struct
{
  unsigned int epoch_line : 14;
  unsigned int reserved   : 18;
} CAMIF_EpochIntrType;

#endif /* CAMIF_H */

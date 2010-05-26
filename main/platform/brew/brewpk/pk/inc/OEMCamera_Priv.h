#ifndef OEMCAMERA_PRIV_H
#define OEMCAMERA_PRIV_H
/*============================================================================
FILE:	OEMCamera_Priv.h

SERVICES:  BREW Camera Services

DESCRIPTION:
   This file defines items specific to QCamera/ICamera
          
PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

        Copyright © 1999-2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================*/

#ifndef AEECAMERA_H
#error Aeecamera.h must be included before oemcamera_priv.h
#endif //#ifndef aeecamera.h

#define CAM_PARM_INCALL_TYPE        (CAM_PARM_CTL_BASE + 20)// [Get] Whether in call and VOCODER type if in call

//In call type
#define CAM_INCALL_BASE                   0                 // Base used by ICamera
#define CAM_INCALL_USER_BASE              0x1000            // Base used for extension

#define CAM_NOT_INCALL                    (CAM_INCALL_BASE)     // No incall
#define CAM_INCALL_AMR                    (CAM_INCALL_BASE+1)   // Incall use AMR vocoder
#define CAM_INCALL_HR                     (CAM_INCALL_BASE+2)   // Incall use half rate vocoder
#define CAM_INCALL_FR                     (CAM_INCALL_BASE+3)   // Incall use full rate vocoder
#define CAM_INCALL_EFR                    (CAM_INCALL_BASE+4)   // Incall use extended full rate vocoder


#define CAM_CMD_OVERLAY_IMAGE       (CAM_CMD_USER_BASE + 1)

typedef struct AEECameraOverlayImage
{
   uint16  nWidth;
   uint16  nHeight;
   uint16  nColorType;
   uint32  nTransparentColor;
   uint16  nPaletteSize;
   byte   *pPalette;
   byte   *pImage;
   byte    cBPP;
} AEECameraOverlayImage;

int ICamera_SaveAppPicture(ICamera * pme, uint16 nWidth, uint16 nHeight,
                                  void *pBuff);

int ICamera_GetInCallParm(ICamera * pme, int32 * pP1);


#endif // #ifndef OEMCAMERA_PRIV_H

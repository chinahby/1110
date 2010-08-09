#ifndef  AEEBITMAP_H
#define  AEEBITMAP_H
/*=================================================================================
FILE:          AEEBitmap.h

SERVICES:      AEE Bitmap interfaces

DESCRIPTION:   This file provides definitions for the bitmap interfaces made
available by the AEE to the application developers  This is a standard header
file that must be included by all applications using IBitmap modules of AEE.

PUBLIC CLASSES:   Not Applicable

INITIALIAZTION AND SEQUENCEING REQUIREMENTS: Not Applicable

Copyright © 1999-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=================================================================================*/

/*---------------------------------------------------------------------------------
      Include Files
---------------------------------------------------------------------------------*/

#include "AEE.h"
#include "../../inc/AEEIBitmap.h"
#include "../../inc/AEEIBitmapDev.h"
#include "../../inc/AEEIDIB.h"

#define AEEIID_BITMAP AEEIID_IBitmap
#define AEEIID_BITMAPDEV AEEIID_IBitmapDev

// Access macros for IBitmap
#define  IBITMAP_AddRef                   IBitmap_AddRef
#define  IBITMAP_Release                  IBitmap_Release
#define  IBITMAP_QueryInterface           IBitmap_QueryInterface
#define  IBITMAP_RGBToNative              IBitmap_RGBToNative
#define  IBITMAP_NativeToRGB              IBitmap_NativeToRGB
#define  IBITMAP_DrawPixel                IBitmap_DrawPixel
#define  IBITMAP_GetPixel                 IBitmap_GetPixel
#define  IBITMAP_SetPixels                IBitmap_SetPixels
#define  IBITMAP_DrawHScanline            IBitmap_DrawHScanline
#define  IBITMAP_FillRect                 IBitmap_FillRect
#define  IBITMAP_BltIn                    IBitmap_BltIn
#define  IBITMAP_BltOut                   IBitmap_BltOut
#define  IBITMAP_GetInfo                  IBitmap_GetInfo
#define  IBITMAP_CreateCompatibleBitmap   IBitmap_CreateCompatibleBitmap
#define  IBITMAP_SetTransparencyColor     IBitmap_SetTransparencyColor
#define  IBITMAP_GetTransparencyColor     IBitmap_GetTransparencyColor
#define  IBITMAP_Invalidate(p,r)          IBitmap_FillRect((p),(r),0,AEE_RO_TRANSPARENT)

// Access macros for IBitmapDev
#define  IBITMAPDEV_AddRef                IBitmapDev_AddRef
#define  IBITMAPDEV_Release               IBitmapDev_Release
#define  IBITMAPDEV_QueryInterface        IBitmapDev_QueryInterface
#define  IBITMAPDEV_Update                IBitmapDev_Update
#define  IBITMAPDEV_IsEnabled             IBitmapDev_IsEnabled
#define  IBITMAPDEV_NotifyEnable          IBitmapDev_NotifyEnable

// This is just for type safe casting of an IDIB* to an IBitmap*.
static __inline IBitmap *IDIB_TO_IBITMAP(const IDIB *pIDIB)
{
   return (IBitmap*)(void*)pIDIB;
}

//*********************************************************************************
//
// IDIB Interface
//
//*********************************************************************************

// AEEDIB is included for backward compatibility; this will be phased out.
typedef struct _aee_dib {
   IQueryInterface * pPaletteMap;
   byte    *         pBmp;
   uint32  *         pRGB;
   NativeColor       transparentColor;
   uint16            cx;
   uint16            cy;
   int16             nPitch;
   uint16            cntRGB;
   uint8             colorDepth;
   uint8             reserved[7];
} AEEDIB;

#define AEEDIB_FlushPalette(pdib)   ((pdib)->pPaletteMap && (IQI_Release((pdib)->pPaletteMap), (pdib)->pPaletteMap = 0))

#endif /* AEEBITMAP_H */


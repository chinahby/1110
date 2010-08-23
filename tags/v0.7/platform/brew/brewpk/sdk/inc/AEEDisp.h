#ifndef AEEDISP_H
#define AEEDISP_H
/*===========================================================================

FILE:      AEEDisp.h
                 
 
SERVICES:  AEE Display Interface services for application developers of mobile devices

 
DESCRIPTION: This file provides defintions for the IDISPLAY interfaces made available by
the AEE to application developers. This is a standard header file that must be included by
all applications using the Display services of the AEE


PUBLIC CLASSES:  Not Applicable


INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable


Copyright © 2000-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===============================================================================
    INCLUDES AND VARIABLE DEFINITIONS
===============================================================================*/

/*-------------------------------------------------------------------
            Defines
-------------------------------------------------------------------*/

/*-------------------------------------------------------------------
      Include Files
-------------------------------------------------------------------*/

#include "AEE.h"
#include "AEEBitmap.h"
#include "AEEFont.h"

/*-------------------------------------------------------------------
      Class IDs
-------------------------------------------------------------------*/

// AEECLSID_DISPLAY is defined in AEEClassIDs.h.
//      AEECLSID_DISPLAY      0xXXXXXXXX  // default display
#define AEECLSID_DISPLAY1     0x010127d4  // display 1
#define AEECLSID_DISPLAY2     0x010127d5  // display 2
#define AEECLSID_DISPLAY3     0x010127d6  // display 3
#define AEECLSID_DISPLAY4     0x010127d7  // display 4

#define AEECLSID_DISPLAY_NULL 0x01013b73  // IDisplay with NULL destination and device bitmap


/*-------------------------------------------------------------------
      Type Declarations
-------------------------------------------------------------------*/

// Text Type
typedef struct _text {
   AECHAR       *pszText;
   word        *pwLength;
} AEEText;

typedef struct _IPagerDisplay IPagerDisplay;

// Generic Bitmap type. TBD: Delete it here and use it from AEE.h
typedef void* AEEBmp;

#include "../../inc/AEEIDisplay.h"
#include "../../inc/AEERect.h"

/* added missing symbols */
#define IDISPLAY_AddRef IDisplay_AddRef
#define IDISPLAY_Backlight IDisplay_Backlight
#define IDISPLAY_ClearScreen IDisplay_ClearScreen
#define IDISPLAY_CreateDIBitmapEx IDisplay_CreateDIBitmapEx
#define IDISPLAY_DrawFrame IDisplay_DrawFrame
#define IDISPLAY_EraseRect IDisplay_EraseRect
#define IDISPLAY_FrameButton IDisplay_FrameButton
#define IDISPLAY_FrameRect IDisplay_FrameRect
#define IDISPLAY_FrameSolidRect IDisplay_FrameSolidRect
#define IDISPLAY_GetClipRect IDisplay_GetClipRect
#define IDISPLAY_GetDestination IDisplay_GetDestination
#define IDISPLAY_GetDeviceBitmap IDisplay_GetDeviceBitmap
#define IDISPLAY_GetFontMetrics IDisplay_GetFontMetrics
#define IDISPLAY_GetSymbol IDisplay_GetSymbol
#define IDISPLAY_InvertRect IDisplay_InvertRect
#define IDISPLAY_IsEnabled IDisplay_IsEnabled
#define IDISPLAY_MakeDefault IDisplay_MakeDefault
#define IDISPLAY_MeasureText IDisplay_MeasureText
#define IDISPLAY_MeasureTextEx IDisplay_MeasureTextEx
#define IDISPLAY_NotifyEnable IDisplay_NotifyEnable
#define IDISPLAY_Release IDisplay_Release
#define IDISPLAY_SetAnnunciators IDisplay_SetAnnunciators
#define IDISPLAY_SetColor IDisplay_SetColor
#define IDISPLAY_SetDestination IDisplay_SetDestination
#define IDISPLAY_SetFont IDisplay_SetFont
#define IDISPLAY_BitBlt IDisplay_BitBlt
#define IDISPLAY_CreateDIBitmap IDisplay_CreateDIBitmap
#define IDISPLAY_DrawRect IDisplay_DrawRect
#define IDISPLAY_DrawText IDisplay_DrawText
#define IDISPLAY_FillRect IDisplay_FillRect
#define IDISPLAY_SetPrefs IDisplay_SetPrefs
#define IDISPLAY_Update IDisplay_Update
#define IDISPLAY_UpdateEx IDisplay_UpdateEx
#define IDISPLAY_Clone IDisplay_Clone
#define IDISPLAY_SetClipRect IDisplay_SetClipRect

#define IDISPLAY_OEMDrawText(p,f,psz,nl,x,y,prcb,flags)  IDISPLAY_DrawText(p,f,(const AECHAR *)psz,nl,x,y,prcb,((flags)|IDF_TEXT_FORMAT_OEM));

#define IDISPLAY_EraseRgn(p,x,y,cx,cy) \
  {AEERect qrc;SETAEERECT(&qrc,(x),(y),(cx),(cy));IDisplay_EraseRect((p),&qrc);}

#define IDISPLAY_DrawHLine(p,x,y,len) \
  {AEERect rc;SETAEERECT(&rc,(x),(y),(len),1); IDisplay_FillRect((p),&rc, RGB_BLACK);}

#define IDISPLAY_DrawVLine(p,x,y,len) \
  {AEERect rc;SETAEERECT(&rc,(x),(y),1,(len)); IDisplay_FillRect((p),&rc, RGB_BLACK);}

#endif    // AEEDISP_H

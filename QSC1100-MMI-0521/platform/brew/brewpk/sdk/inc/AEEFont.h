#ifndef  AEEFONT_H
#define  AEEFONT_H
/*=================================================================================
FILE:          AEEFont.h

SERVICES:      Text drawing and measuring

DESCRIPTION:   This file describes in interface for drawing and measuring text
    that can be supplied by the system or by applications.

PUBLIC CLASSES:   Not Applicable

INITIALIAZTION AND SEQUENCEING REQUIREMENTS: Not Applicable

Copyright © 1999-2005,2007-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=================================================================================*/

/*---------------------------------------------------------------------------------
      Include Files
---------------------------------------------------------------------------------*/

#include "AEEBitmap.h"

/*---------------------------------------------------------------------------------
      Class IDs
---------------------------------------------------------------------------------*/
// AEECLSID_FONT is in AEEClsIds.h.

// System fonts
#define AEECLSID_FONTSYSNORMAL      0x01012786
#define AEECLSID_FONTSYSLARGE       0x01012787
#define AEECLSID_FONTSYSBOLD        0x01012788
#define AEECLSID_FONTSYSITALIC      0x0101402c
#define AEECLSID_FONTSYSBOLDITALIC  0x0101402d
#define AEECLSID_FONTSYSLARGEITALIC 0x0101402e

#include "../../inc/AEEIFont.h"

/* added missing symbols */
#define IFONT_AddRef IFont_AddRef
#define IFONT_DrawText IFont_DrawText
#define IFONT_GetInfo IFont_GetInfo
#define IFONT_MeasureText IFont_MeasureText
#define IFONT_QueryInterface IFont_QueryInterface
#define IFONT_Release IFont_Release


#endif /* AEEFONT_H */

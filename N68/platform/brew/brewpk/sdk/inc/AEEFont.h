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
#define AEECLAID_FONTSMALL          0x0101402f

#ifdef CUST_EDITION
//Huge
#define AEECLSID_FONTSYSHUGE				0x0107772c		
#define AEECLSID_FONTSYSHUGEBOLD			0x0107772d	 
#define AEECLSID_FONTSYSHUGEITALIC			0x0107772f  
#define AEECLSID_FONTSYSHUGEBOLDITALIC		0x0107772e 

//Large	
#define AEECLSID_FONTSYSLARGEBOLD	 		0x0107777c
#define AEECLSID_FONTSYSLARGEBOLDITALIC		0x0107777d

//Middle
#define AEECLSID_FONTSYSMIDDLE				0x01077730			
#define AEECLSID_FONTSYSMIDDLEBOLD			0x01077731		 
#define AEECLSID_FONTSYSMIDDLEITALIC		0x01077733	 
#define AEECLSID_FONTSYSMIDDLEBOLDITALIC	0x01077732 

//Small
#define AEECLSID_FONTSYSSMALL       		0x01012789
#define AEECLSID_FONTSYSSMALLBOLD   		0x0101278b 
#define AEECLSID_FONTSYSSMALLITALIC 		0x0101278c 
#define AEECLSID_FONTSYSSMALLBOLDITALIC 	0x0101278d

//Mini
#define AEECLSID_FONTSYSMINI				0x0107a196
#define AEECLSID_FONTSYSMINIBOLD			0x0107a199		 
#define AEECLSID_FONTSYSMINIITALIC			0x0107a19a	 
#define AEECLSID_FONTSYSMINIBOLDITALIC		0x0107a19b 

//Tiny
#define AEECLSID_FONTSYSTINY				0x01075c09
#define AEECLSID_FONTSYSTINYBOLD			0x01077734		 
#define AEECLSID_FONTSYSTINYITALIC			0x01077735	 
#define AEECLSID_FONTSYSTINYBOLDITALIC		0x01077736 

//Other
#define AEECLSID_FONTSYSBIGNUMBER   		0x0101278a
#define AEECLSID_FONTSYSBIGNUMBERBOLD		0x0108636d
#endif

#include "../../inc/AEEIFont.h"

/* added missing symbols */
#define IFONT_AddRef IFont_AddRef
#define IFONT_DrawText IFont_DrawText
#define IFONT_GetInfo IFont_GetInfo
#define IFONT_MeasureText IFont_MeasureText
#define IFONT_QueryInterface IFont_QueryInterface
#define IFONT_Release IFont_Release


#endif /* AEEFONT_H */

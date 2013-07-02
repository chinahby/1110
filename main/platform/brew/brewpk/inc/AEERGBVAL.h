#ifndef AEERGBVAL_H
#define AEERGBVAL_H
/*=============================================================================

FILE:          AEERGBVAL.h

SERVICES:      RGBVAL type

DESCRIPTION:

PUBLIC CLASSES:   Not Applicable

INITIALIAZTION AND SEQUENCEING REQUIREMENTS: Not Applicable

Copyright © 2001-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/

#include "AEEStdDef.h"

typedef uint32 RGBVAL;

#define MAKE_RGB(r, g, b)     (RGBVAL) ( ((uint32)(r) << 8) + ((uint32)(g) << 16) + ((uint32)(b) << 24) )
#define MAKE_RGBA(r, g, b, a) (RGBVAL) ( ((uint32)(r) << 8) + ((uint32)(g) << 16) + ((uint32)(b) << 24) + ((uint32)(a)))

#define RGB_NONE              (0xffffffffL)
#define RGB_WHITE             (MAKE_RGB(0xff, 0xff, 0xff))
#define RGB_BLACK             (MAKE_RGB(0,0,0))
#ifdef CUST_EDITION	
#define RGB_YELLOW_EX         (MAKE_RGB(0xff, 0x00, 0x00))//wlh 20090427 add
#define RGB_RED               (MAKE_RGB(255,0,0))
#define RGB_WINTE_BACKDROP    (MAKE_RGB(56,98,107))           //add by ydc 090522
#define RGB_WIRIE_SCRLLBAR    (MAKE_RGB(133,181,190))           //add by ydc 090522
#define RGB_GRAY_BACK         (MAKE_RGB(181,181,181))           //add by ydc 090522 
#endif /*CUST_EDITION*/
//
// AEE_RO_TRANSPARENT Mask Values: These describe the default transparent
// colors for the display bitmap and bitmaps compatible with it.  For
// monochrome and greyscale displays, the transparent color is
// RGBToNative(RGB_WHITE), and for color displays the default transparent
// color is RGBToNative(RGB_MASK_COLOR).
//

#define RGB_MASK_MONO      (RGB_WHITE)
#define RGB_MASK_GREY      (RGB_WHITE)
#define RGB_MASK_COLOR     MAKE_RGB(255, 0, 255)      // Magenta

/*
===============================================================================
DATA STRUCTURES DOCUMENTATION
===============================================================================

RGBVAL

Description:
The RGB value for a color is defined using this data type. The 8-bit values
for red, green, blue are stored in 32 bits as following. The reserved bits are 
filled with zeros.
 
Definition:
typedef uint32 RGBVAL

Members:
None

Comments:
The user can create their own colors using the MAKE_RGB macro with their values
for red, green and blue to get the corresponding RGBVAL .

See Also:
None

===============================================================================
MACROS DOCUMENTATION
===============================================================================

Function: MAKE_RGB()

Description:
  This macro is used to compose an RGBVAL from red, green, and blue components.

Prototype:
  #define MAKE_RGB(r, g, b)     (RGBVAL) ( ((uint32)(r) << 8) + ((uint32)(g) << 16) + ((uint32)(b) << 24) )

Parameters:
  r          [in]: Red component
  g          [in]: Green component
  b          [in]: Blue component

Return Value:
  Returns an RGBVAL.

Comments:
  None.

Version:
   Introduced BREW Client 1.1

See Also:
  RGBVAL

===============================================================================
*/
#endif //AEERGBVAL_H


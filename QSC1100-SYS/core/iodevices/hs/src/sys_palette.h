#ifndef SYS_PALETTE_H
#define SYS_PALETTE_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   sys_palette.h

General Description
 
  Copyright (c) 2008 QUALCOMM Incorporated. 
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*============================================================================

                       EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.

 $Header: //depot/asic/msmshared/services/hs/sys_palette.h#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/20/08   sg      Decouple LCD driver for multiprocessor targets
02/05/08   dk      Fixed compilation errors for 7k targets.
01/23/08   dk      Added the structure and macro definitions from OEMCommondisp.h as a part of BREW cleanup activity.

============================================================================ */

 
#if !defined(FEATURE_MULTIPROCESSOR) || (defined(FEATURE_MULTIPROCESSOR) && defined(IMAGE_APPS_PROC))
#include "disp_options.h"
#include "AEEBitmap.h"
#include "AEEDisp.h"

/*
 * This file contains a macro to lookup the index into the system
 * palette that corresponds most closely to the given RGB triplet.
 */
 
extern const unsigned long int system_palette[];
extern uint16 gPalette8To16Tbl[256];
#ifdef DISP_DEVICE_16BPP
extern uint8  roundTo5[256];
extern uint8  roundTo6[256];
#elif defined DISP_DEVICE_18BPP
extern uint8  roundTo6[256];
#elif defined DISP_DEVICE_8BPP
extern uint8  roundTo2[256];
extern uint8  roundTo3[256];
#endif /* DISP_DEVICE_8BPP */

/*
* The following structure is taken from OEMCommondisp.h.BREW team has decided to 
* phaseoff that header file and this is being used by Chartoimg.c of hs section.
* Perforce path : //depot/asic/msmshared/apps/brew_3.1.5/pk/inc/oemcommondisp.h#3
*/
/* Font type defines the font used in displaying text . */
typedef struct
{
   long ascent;                 /* The font ascent (units above the base line)
                                   ** Usually the height of the letter 'A' or 'H' */
   long descent;                /* The font descent (units below the base line)
                                   ** Usually the overhang of the letter 'g' */
   unsigned long (*get_char_width)(unsigned short character); /* Function returns the width
                                                                 of the specified character */
   long (*get_char_offset_leading)(unsigned short character); /* Function returns the leading
                                                                 horizontal offset in pixels
                                                                 of the character. */
   long (*get_char_offset_trailing)(unsigned short character);/* Function returns the trailing
                                                                 horizontal offset in pixels
                                                                 of the character. */
   const unsigned char *(*get_char_data)(unsigned short character, unsigned long *image_width);
   /* Function returns the block of data used to represent the character.
   ** The pointer returned must be byte aligned, and the data must be
   ** formatted as follows:
   ** One bit representing a single pixel of the character from left to right
   ** for the width of the character, followed by (image_width - char width),
   ** bits of padding from the top row of the image through the bottom row of the image
   ** Zero (0) represents a background color bit
   ** One (1) represents a foreground color bit
   ** This data block must contain at least (get_char_width x get_char_height) bits
   ** Used for drawing purposes.
   */
  /* This is the ative color table */
  RGBVAL             clrs[MAX_ACTIVE_COLOR + 1];
} OEMFontInfo;

/*
* The following structure is taken from OEMCommondisp.h.BREW team has decided to 
* phaseoff that header file and this is being used by Chartoimg.c of hs section.
* Perforce path : //depot/asic/msmshared/apps/brew_3.1.5/pk/inc/oemcommondisp.h#3
*/

/* Definition of a 32-bit word containing a 16-bit filler and the
   RGB bits in the least significant 16 bits, as RRRRRGGGGGGBBBBB
   or a 24-bit filler and the RGB in the least significant 8 bits
   as BBGGGRRR.
   This format is as seen in the register.
*/
#ifdef DISP_DEVICE_16BPP
// Macro to translate given 8-bit values for R, G, and B to a R5G6B5 16-bit value
#define RGB_TO_NATIVE(r, g, b)         \
  ( (roundTo5[r] << 11) |  \
    (roundTo6[g] <<  5) |  \
    (roundTo5[b]      )         )

// Macro to translate given a 16-bit RGB (5,6,5) to 32-bit RGB
#define NATIVE_TO_RGB(clr16)                 \
  ( ( (clr16 & 0xF800)       ) | \
    ( (clr16 & 0x07E0) << 13 ) | \
    ( (clr16 & 0x001F) << 27 )   )

#elif defined DISP_DEVICE_18BPP
// Macro to translate given 8-bit values for R, G, and B to a R6G6B6 18-bit value
#define RGB_TO_NATIVE(r, g, b)         \
  ( (roundTo6[r] << 12) |  \
    (roundTo6[g] <<  6) |  \
    (roundTo6[b]      )         )

// Macro to translate given a 18-bit RGB (6,6,6) to 32-bit RGB
#define NATIVE_TO_RGB(clr18)                 \
  ( ( (clr18 & 0x0003F000) >>  2 ) | \
    ( (clr18 & 0x00000FC0) << 12 ) | \
    ( (clr18 & 0x0000003F) << 26 )   )

#elif defined DISP_DEVICE_8BPP
// Macro to translate given 8-bit values for R, G, and B to a B2G3R3 8-bit value
#define RGB_TO_NATIVE(r, g, b)        \
  ( (roundTo3[r]     ) |  \
    (roundTo3[g] << 3) |  \
    (roundTo2[b] << 6)         )

// Macro to translate given a 8-bit RGB (B2,G3,R3) to 32-bit RGB
#define NATIVE_TO_RGB(clr8)               \
  ( ( (clr8 & 0x07) << 13 ) | \
    ( (clr8 & 0x38) << 18 ) | \
    ( (clr8 & 0xC0) << 24 )   )

#endif /* DISP_DEVICE_16BPP */

#ifdef DISP_DEVICE_16BPP
    typedef word OEMPixel;
#define OEM_PIXEL_SIZE 2
#elif defined DISP_DEVICE_18BPP
    typedef dword OEMPixel;
#define OEM_PIXEL_SIZE 4
#elif defined DISP_DEVICE_8BPP
    typedef byte OEMPixel;
#define OEM_PIXEL_SIZE 1
#endif /* DISP_DEVICE_16BPP */


#endif /*#if !defined(FEATURE_MULTIPROCESSOR) || 
         (defined(FEATURE_MULTIPROCESSOR) && defined(IMAGE_APPS_PROC))*/
#endif /* SYS_PALETTE_H */

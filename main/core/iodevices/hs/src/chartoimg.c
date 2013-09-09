/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      Handset Bitmap Interface

GENERAL DESCRIPTION
   This module contains functions and data blocks pertaining to the image
   handset interface to support a character based UI. This module mainly
   converts character operations into image operations. This maybe only
   temporaray until a full REX graphics subsystem will be developed.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None

  Copyright (c) 2001-2008 QUALCOMM Incorporated. 
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*============================================================================

                       EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.

 $Header: //depot/asic/msmshared/services/hs/chartoimg.c#55 $DateTime: $Author: rtilak $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/07/08   rt      Fix featurization to remove compiler warning
06/20/08   sg      Decouple LCD driver for multiprocessor targets
06/10/08   sg      Fixed Klocwork/lint warnings
02/06/08   dk      Adding disp2d.h as a part of HALMDP activity.
02/05/08   dk      Featurised MDDI error display for fixing compilation error 
02/05/08   dk      Fixed compilation error in 7600 
01/23/08   dk      Featurised the MDDI dependency 
01/23/08   dk      Removed inclusion of OEMCommondisp.h with BREW cleanup
04/11/07   ah      Added support to display error messages using MDDI
02/26/07   jas     Featurized references to 3GUI entities.
09/19/06   cf      Add support for BREW 4.0
03/07/06   ty      Clear MDP error mode after error message display
01/22/06   ptm     Removed hardcoded values from lcd_debug_message.
10/05/05   trg     Updated fatal error api call to mdp.
08/10/05   ~SN     Added the infamous blue screen :-(
05/26/05   ty      Minimized Brew dependencies in error display routine
11/17/04   ap      Use correct case for include of oemcommondisp.
09/25/03   tml     Remove UI dependencies
09/24/03   kar     Replaced call to AEE_Active with AEE_GetShell
03/25/03   kar     Changes to use IDIB. Fixed screen flickering
03/14/03   kar     Clear screen during update
03/11/03   sun     HS_AN_MSG should not be updated if FEATURE_WMS_APP is defined
01/22/03   kar     Displayed D icon for digital service.
12/03/02   lad     Removed call to dload_entry() from lcd_message().  This
                   is a diagnostic function, not an LCD driver function.
10/17/02   dle     Changed for new GRP interface.
09/15/02   kar     Featurized call to AEE_Active under F_UI_CORE
09/11/02   kar     Annunciator bug fixes
09/05/02   dle     In chartoimg_init, if pDisplay is not available, then
                   don't do anything else but to get a bitmap up for
                   display. Doing other things may cause additional failure
                   because BREW is not initialized, and prevent the error
                   message from being displayed.
07/19/02   kar     Removed reference to OEMAnnunciator_priv.h
07/18/02   kar     Don't clear annunciator area while clearing display
07/17/02   dle     Modified for grp prototype change.
06/27/02    lz     Changed UIApp.h to CoreApp.h
06/27/02    kc     Used IANNUCIATOR_xx functions in
                   chartoimg_update_annuciators().
06/06/02   dle     Moved lcd_message() here. lcd_message() calls dload_entry
                   to get into download mode. Added lcd_debug_message().
06/03/02   dle     chartoimg_init() as needed. In the function, if BREW
                   has not been initialized, go to the OEM to get a bitmap.
05/16/02   lz      Added necessary functions to draw text using MSM default
                   font.
04/10/02   dle     When in BREW state, do not update screen. Upon exiting
                   from BREW state, clear screen and redraw the rectangle.
03/28/02   dle     Implemented DrawRect locally. Also moved pDevBmp to where
                   it is used and released after use.

 *
 *    Rev 1.3   25 Sep 2001 17:33:54   stam
 * In function chartoimg_update_screen, the function draw_text is to be
 * invoked only if line of text changed. Changes made by Nurit Y.
 *
 *    Rev 1.2   14 Aug 2001 11:36:08   karthick
 * Changes for new GS API
 *
 *    Rev 1.1   02 Jul 2001 13:33:16   karthick
 * Removed LED support
 *
 *    Rev 1.0   05 Apr 2001 16:58:12   karthick
 * Initial revision.

============================================================================ */

/*============================================================================
                       INCLUDE FILES FOR MODULE
============================================================================*/
#include "target.h"
#include "customer.h"
#include "comdef.h"    /* Contains REX basic types definitions */
#if defined(FEATURE_BITMAP)  && !defined(FEATURE_UI_CORE_REMOVED)
#if !defined(FEATURE_MULTIPROCESSOR) || (defined(FEATURE_MULTIPROCESSOR) && defined(IMAGE_APPS_PROC))
#include "chartoimg.h" /* Chartoimg type definitions */
#ifdef FEATURE_GRP
#error code not present
#endif /* FEATURE_GRP */

#include "err.h"       /* Error reporting */

#include "hs.h"        /* Annunciator definitions */
#ifdef INC_DISP_H
#include INC_DISP_H
#else
#include "disp.h"      /* For display upate       */
#endif

#ifdef FEATURE_MDP
#include "mdp_sys_api.h"   /* Add mdp prototypes */
#endif

#ifdef FEATURE_HS_MDDI_ENABLE_ERROR_DISPLAY
#include "mddihost.h"
#endif

#ifdef FEATURE_EARLY_ERROR_RESET
#include "boot_hw_reset.h"
#endif

#include "sys_palette.h"
#include "font_msm_system.rff"

#include "AEE_OEM.h"
#include "AEEShell.h"
#include "AEEDisp.h"
#include "AEEBitmap.h"
#include "AEEFont.h"
#include "AEEStdLib.h"

#ifndef USES_MMI
#ifndef FEATURE_UIONE_HDK
#ifdef FEATURE_UI_CORE
#include "CoreApp.h"
#endif /* FEATURE_UI_CORE */
#endif /* FEATURE_UIONE_HDK */
#endif

#ifdef FEATURE_ANNUNCIATOR
#include "AEEAnnunciator.h"
#endif /* FEATURE ANNUNCIATOR */
#include "BREWVersion.h"

#if !MIN_BREW_VERSION(4,0)
#ifdef DISP_DEVICE_18BPP
extern int OEMBitmap18_New(uint16 w, uint16 h, byte *pBuf, IBitmap **pBmp );
#else /* DISP_DEVICE_18BPP */
extern int OEMBitmap16_New(uint16 w, uint16 h, byte *pBuf, IBitmap **pBmp );
#endif /* DISP_DEVICE_18BPP */
extern int OEM_InitDispInfo(void);
#endif


/*============================================================================
              CONSTANTS, MACROS AND STRUCTURE TYPES FOR MODULE
============================================================================*/

/* The character based will not take the entire display area, the following
   are constants for that display area
 */

/* The character-based display offset from the entirre display */
#define CHAR_DISPLAY_OFFSET_X       0
#define CHAR_DISPLAY_OFFSET_Y       40

#define CHAR_DISPLAY_WIDTH          176
#define CHAR_DISPLAY_HEIGHT         60

/* The start of the text screen */
#define TEXT_OFFSET_X               40
#define TEXT_OFFSET_Y               10

/* The text portion of the screen dimensions */
#define TEXT_WIDTH                  96
#define TEXT_HEIGHT                 48

/* Number of characters to fit in each line, and the maximum
   number of lines
 */
#define CHARS_PER_LINE              12
#define MAX_NUM_LINES               4
#define LINE_HEIGHT                 12

typedef struct
{
  word annunciator;
  byte rssi;
  byte batt;
} AnnunciatorData;

/*===========================================================================
              GLOBAL VARIABLES
============================================================================*/


/*===========================================================================
              LOCAL VARIABLES
============================================================================*/
char keep_str[MAX_NUM_LINES*CHARS_PER_LINE];

LOCAL boolean   chartoimg_initialized = FALSE;
LOCAL IDisplay *pDisplay = NULL;
LOCAL IDIB     *pDDB = NULL;
LOCAL NativeColor clrFill, clrFrame;
LOCAL IShell   *pShell = NULL;
#ifdef FEATURE_ANNUNCIATOR
LOCAL IAnnunciator *pAnn = NULL;
LOCAL AEECallback cb;
LOCAL AnnunciatorData annunciatorData;
#endif /* FEATURE_ANNUNCIATOR */


/*===========================================================================
              LOCAL FUNCTIONS PROTOTYPES
============================================================================*/

#ifdef FEATURE_ANNUNCIATOR
/*===========================================================================

   Function:  UpdateAnnunciators()

==============================================================================*/
static void UpdateAnnunciators(AnnunciatorData *pAnnunData)
{
#ifndef USES_MMI
   /* First see the system Digital or Analog */
  if ( ( (pAnnunData->annunciator) & HS_AN_CDMA) != 0 )
  {
    /* Draw the Digital icon */
    IANNUNCIATOR_SetField (pAnn, ANNUN_FIELD_MODE, ANNUN_STATE_DIGITAL);
  }
  else
  {
    IANNUNCIATOR_SetField (pAnn, ANNUN_FIELD_MODE, ANNUN_STATE_OFF);
  }

  /* Check roaming */
  if ( ( (pAnnunData->annunciator) & HS_AN_ROAM) != 0)
  {
    IANNUNCIATOR_SetField (pAnn, ANNUN_FIELD_ROAM, ANNUN_STATE_ON);
  }
  else
  {
    IANNUNCIATOR_SetField (pAnn, ANNUN_FIELD_ROAM, ANNUN_STATE_OFF);
  }

  /* Check the Msg/envelope icon */
#if !defined(FEATURE_WMS_APP)
  if ( ( (pAnnunData->annunciator) & HS_AN_MSG) != 0 )
  {
    IANNUNCIATOR_SetField (pAnn, ANNUN_FIELD_MSG, ANNUN_STATE_ON);
  }
  else
  {
    IANNUNCIATOR_SetField (pAnn, ANNUN_FIELD_MSG, ANNUN_STATE_OFF);
  }
#endif

  /* Check RSSI level */
  MSG_FATAL("UpdateAnnunciators",0,0,0);
  switch( pAnnunData->rssi )
  {
    case 5:
      /* Value 5 is actually RSSI with 4 bars - go figure :) */
      IANNUNCIATOR_SetField (pAnn, ANNUN_FIELD_RSSI, ANNUN_STATE_RSSI_4);      
#ifdef FEATURE_VERSION_EC99
      IANNUNCIATOR_SetField (pAnn, ANNUN_FIELD_3G_RSSI, ANNUN_STATE_RSSI_4); 
#endif
      break;
    case 4:
      /* Value 4 is RSSI with 3 bars */
      IANNUNCIATOR_SetField (pAnn, ANNUN_FIELD_RSSI, ANNUN_STATE_RSSI_3);    
#ifdef FEATURE_VERSION_EC99
      IANNUNCIATOR_SetField (pAnn, ANNUN_FIELD_3G_RSSI, ANNUN_STATE_RSSI_3); 
#endif      
      break;
    case 3:
      /* Value 3 is RSSI with 2 bars */
      IANNUNCIATOR_SetField (pAnn, ANNUN_FIELD_RSSI, ANNUN_STATE_RSSI_2); 
#ifdef FEATURE_VERSION_EC99
      IANNUNCIATOR_SetField (pAnn, ANNUN_FIELD_3G_RSSI, ANNUN_STATE_RSSI_2); 
#endif      
      break;
    case 2:
       /* Value 2 means RSSI with 1 bar */
      IANNUNCIATOR_SetField (pAnn, ANNUN_FIELD_RSSI, ANNUN_STATE_RSSI_1);
#ifdef FEATURE_VERSION_EC99
      IANNUNCIATOR_SetField (pAnn, ANNUN_FIELD_3G_RSSI, ANNUN_STATE_RSSI_1); 
#endif       
      break;
    case 1:
      /* Value 1 means just RSSI icon with no bars */
      IANNUNCIATOR_SetField (pAnn, ANNUN_FIELD_RSSI, ANNUN_STATE_RSSI_0);    
#ifdef FEATURE_VERSION_EC99
      IANNUNCIATOR_SetField (pAnn, ANNUN_FIELD_3G_RSSI, ANNUN_STATE_RSSI_0); 
#endif      
      break;
    case 0:
      IANNUNCIATOR_SetField (pAnn, ANNUN_FIELD_RSSI, ANNUN_STATE_OFF);
#ifdef FEATURE_VERSION_EC99
      IANNUNCIATOR_SetField (pAnn, ANNUN_FIELD_3G_RSSI, ANNUN_STATE_OFF); 
#endif      
      break;   
    default:
      MSG_ERROR( "Illegal RSSI Value (%d)", pAnnunData->rssi, 0, 0 );
      break;
  }

  /* Check the phone icone (inuse or no service)
   * In-Use -or- No Service (phone ICON)
   */
  if ( ( (pAnnunData->annunciator) & HS_AN_INUSE) != 0 )
  {
    /* A calling phone icon */
    IANNUNCIATOR_SetField (pAnn, ANNUN_FIELD_INUSE, ANNUN_STATE_IN_USE);
  }
  else if ( ( (pAnnunData->annunciator) & HS_AN_NOSVC) != 0 )
  {
    /* A no-service phone icon */
    IANNUNCIATOR_SetField (pAnn, ANNUN_FIELD_INUSE, ANNUN_STATE_NO_SVC);
    /* Make sure that the digital/roam icons are off */
    IANNUNCIATOR_SetField (pAnn, ANNUN_FIELD_ROAM, ANNUN_STATE_OFF);
    IANNUNCIATOR_SetField (pAnn, ANNUN_FIELD_MODE, ANNUN_STATE_OFF);
  }
  else
  {
    /* Basically this is a no phone icon */
    IANNUNCIATOR_SetField (pAnn, ANNUN_FIELD_INUSE, ANNUN_STATE_OFF);
  }

  /* Check Battery level */
  /* Battery level
   */

  MSG_ERROR("***zzg pAnnunData->batt=%d***", pAnnunData->batt, 0, 0 );
  
  switch ( pAnnunData->batt )
  {
  #if /*defined(FEATURE_VERSION_LM126C)||*/defined(FEATURE_5_LEVEL_BATTERY)//xxzhen
  	 case 6:
      /* This value means 4-bars battery */
      IANNUNCIATOR_SetField (pAnn, ANNUN_FIELD_BATT, ANNUN_STATE_BATT_FULL);
      break;
 	 case 5:
      /* This value means 4-bars battery */
      IANNUNCIATOR_SetField (pAnn, ANNUN_FIELD_BATT, ANNUN_STATE_BATT_4);
      break;
  #else    
    case 5:
      /* This value means 4-bars battery */
      IANNUNCIATOR_SetField (pAnn, ANNUN_FIELD_BATT, ANNUN_STATE_BATT_FULL);
      break;    
  #endif
    case 4:
      /* This means 3 bars */
      IANNUNCIATOR_SetField (pAnn, ANNUN_FIELD_BATT, ANNUN_STATE_BATT_3);
      break;
    case 3:
      /* This means 2 bars */
      IANNUNCIATOR_SetField (pAnn, ANNUN_FIELD_BATT, ANNUN_STATE_BATT_2);
      break;
    case 2:
      /* This means 1 bar */
      IANNUNCIATOR_SetField (pAnn, ANNUN_FIELD_BATT, ANNUN_STATE_BATT_1);
      break;
    case 1:
      /* This means empty battery */
      IANNUNCIATOR_SetField (pAnn, ANNUN_FIELD_BATT, ANNUN_STATE_BATT_LOW);
      break;
    case 0:
      /* no battery-icon */
      IANNUNCIATOR_SetField (pAnn, ANNUN_FIELD_BATT, ANNUN_STATE_OFF);
      break;
    default:
      MSG_ERROR( "Illegal BATT Value (%d)", pAnnunData->batt, 0, 0 );
      break;
  }
  
  #endif
  return;
} /* End UpdateAnnunciators */
#endif /* FEATURE_ANNUNCIATOR */



#if !MIN_BREW_VERSION(4,0)

/*===========================================================================

   Function:  DrawRect()

==============================================================================*/
static void DrawRect( IBitmap *po,
                     const AEERect *pRect,
                     NativeColor color,
                     AEERasterOp rop )
{
  int xMin, xMax, yMin ,yMax, i;

  yMin = pRect->y;
  yMax = pRect->y + pRect->dy - 1;
  xMin = pRect->x;
  xMax = xMin + pRect->dx - 1;

  IBITMAP_DrawHScanline(po, yMin, xMin, xMax, color, rop);
  IBITMAP_DrawHScanline(po, yMax, xMin, xMax, color, rop);

  for (i = yMin; i <= yMax; i++)
  {
    IBITMAP_DrawPixel(po, xMin, i, color, rop);
    IBITMAP_DrawPixel(po, xMax, i, color, rop);
  }
}

#endif

/*===========================================================================

   Function:  render_char_data()

  ** Renders character data (where each bit corresponds to a pixel to set
  ** or not) to a standard 8 bit-per-pixel destination buffer.

============================================================================*/
static void render_char_data
(
uint8        *buf,
int           buf_width,
int           x,
int           y,
int           width,
int           height,
const uint8  *char_data,
int           char_width,
int           char_start_x,
int           char_start_y,
NativeColor   clrText,
NativeColor   clrBackground
)
{
  uint32 skip_bits;
  uint8 bit_mask;
  int i;
  OEMPixel *lbuf = (OEMPixel *)buf;

  /* Adjust lbuf to point to where rendering will begin */
  lbuf += (y * buf_width) + x;

  /* Skip this many bits at start of char_data */
  skip_bits = (char_start_y * char_width) + char_start_x;

  /* Initialize char_data and bit_mask to skip right number of bits */
  char_data += (skip_bits >> 3);
  bit_mask = 0x80 >> (skip_bits & 0x7);

  /* Loop through rows */
  while (height--)
  {
    /* Loop through displayable width of char_data */
    for (i = 0; i < width; i++)
    {
      /* Render pixel if corresponding bit in char_data is set */
      if (*char_data & bit_mask)
      {
        lbuf[i] = clrText;
      }
      else
      {
        lbuf[i] = clrBackground;
      }

      if ((bit_mask >>= 1) == 0)
      {
        bit_mask = 0x80;
        char_data++;
      }
    }

    /* Skip any non-displayable portion between rows of char_data */
    for (; i < char_width; i++)
    {
      if ((bit_mask >>= 1) == 0)
      {
        bit_mask = 0x80;
        char_data++;
      }
    }

    /* Go to next row of destination buffer */
    lbuf += buf_width;
  }
} /* End render_char_data */

/*===========================================================================

   Function:  render_unicode_string()

** Renders a string of unicode character to a standard 8 bit-per-pixel
** destination buffer.  This means the corresponding pixels are set in
** the destination.  The background pixels are always left as-is.
** (So, to draw text with a background, you must fill the appropriate
** rectangle with the background color before calling this function.)
**
==============================================================================*/
static void render_unicode_string
(
uint8        *buf,
int           buf_width,
int           x,
int           y,
const uint16 *unicode_string,
int           max_len,
OEMFontInfo  *font,
NativeColor   clrText,
NativeColor   clrBackground,
int           clip_x,
int           clip_y,
int           clip_width,
int           clip_height
)
{
  const uint8 *char_data;
  int char_width;
  int char_height;
  int char_offset_leading;
  int char_offset_trailing;
  int char_start_x;
  int char_start_y;
  int char_stop_x;
  int char_stop_y;
  int i;

  /* Make sure parameters are valid */
  if ( (buf == NULL) ||
       (buf_width < 0) ||
       (unicode_string == NULL) ||
       (clip_x < 0) ||
       (clip_y < 0) ||
       (clip_width < 0) ||
       (clip_height < 0) ||
       ((clip_x + clip_width) > buf_width) )
  {
    return;
  }

  /* Get height of the unicode characters (same for all chars) */
  char_height = font->ascent + font->descent;

  /* Drop out now if nothing potentially in clipping rectangle */
  if ( ((y + char_height) <= clip_y) ||
       (y >= (clip_y + clip_height)) ||
       (x >= (clip_x + clip_width)) )
  {
    return;
  }

  /* Calculate which row of char data to start at (same for all chars) */
  if (y < clip_y)
  {
    char_start_y = clip_y - y;
  }
  else
  {
    char_start_y = 0;
  }

  /* Calculate which row of char data to stop at (same for all chars) */
  if ((y + char_height) > (clip_y + clip_height))
  {
    char_stop_y = clip_y + clip_height - y;
  }
  else
  {
    char_stop_y = char_height;
  }

  /* Loop through each character in the unicode string */
  for (i = 0; (i < max_len) && (unicode_string[i] != 0); i++)
  {
    /* Get the character data*/
    char_data = font->get_char_data(unicode_string[i], (uint32 *)&char_width);
    char_offset_leading = font->get_char_offset_leading(unicode_string[i]);
    char_offset_trailing = font->get_char_offset_trailing(unicode_string[i]);

    /* Advance x for leading offset of character */
    x += char_offset_leading;

    /* Break out of loop if past right edge of clipping rectangle */
    if (x >= (clip_x + clip_width))
    {
      break;
    }

    /* Calculate which column of char data to start at */
    if (x < clip_x)
    {
      char_start_x = clip_x - x;
    }
    else
    {
      char_start_x = 0;
    }

    /* Calculate which column of the char data to stop at */
    if ((x + char_width) > (clip_x + clip_width))
    {
      char_stop_x = clip_x + clip_width - x;
    }
    else
    {
      char_stop_x = char_width;
    }

    /* Render character only if reached left edge of clipping rectangle */
    if ((x + char_width) > clip_x)
    {
      render_char_data(
                      buf,
                      buf_width,
                      (x + char_start_x),
                      (y + char_start_y),
                      (char_stop_x - char_start_x),
                      (char_stop_y - char_start_y),
                      char_data,
                      char_width,
                      char_start_x,
                      char_start_y,
                      clrText,
                      clrBackground
                      );
    }

    /* Advance x for next character */
    x += char_width + char_offset_trailing;
  }
} /* End render_unicode_string */

/*===========================================================================

   Function:  DrawText()

==============================================================================*/
static int DrawText(IBitmap *pDst,
                    int x,
                    int y,
                    const AECHAR *pcText,
                    int nChars,
                    NativeColor foreground, //8 bit palette index
                    NativeColor background, //8 bit palette index
                    const AEERect *prcClip)
{
  OEMFontInfo *font = &font_msm_system;
  int32 clip_x, clip_y, clip_width, clip_height;
  NativeColor clrBackground, clrText;
  IDIB *dstDib;

  // Assumption - only ONE line of text will be written - this function
  // will not wrap the text in the clipping rect.
  if (pcText == NULL)
  {
    return(EFAILED);
  }

  if (pDst == NULL)
  {
    return EFAILED;
  }
  else
  {
      IBITMAP_QueryInterface(pDst, AEECLSID_DIB, (void **) &dstDib);
   }

  // Set up clipping
  if (prcClip)
  {
    clip_x = prcClip->x;
    clip_y = prcClip->y;
    clip_width = prcClip->dx;
    clip_height = prcClip->dy;

    /* Check for clip_x */
    if (clip_x < 0)
    {
      clip_width += clip_x; /* Adjust width */
      clip_x = 0;           /* then adjust left */
    }
    else if (clip_x >= dstDib->cx)
    {
      IDIB_Release(dstDib);
      return EBADPARM;
    }

    /* Check for clip_width */
    if (clip_width <= 0) return EBADPARM;
    if ((clip_x + clip_width) > dstDib->cx)
    {
      clip_width = dstDib->cx - clip_x;
    }

    /* Check for clip_y */
    if (clip_y < 0)
    {
      clip_height += clip_y; /* Adjust height */
      clip_y = 0;            /* then adjust top */
    }
    else if (clip_y >= pDDB->cy)
    {
      IDIB_Release(dstDib);
      return EBADPARM;
    }

    /* Check for clip_height */
    if (clip_height <= 0) return EBADPARM;
    if ((clip_y + clip_height) > dstDib->cy)
    {
      clip_height = pDDB->cy - clip_y;
    }
  }
  else
  { /* the clip area is the whole sceen */
    clip_x = 0;
    clip_y = 0;
    clip_width = pDDB->cx;
    clip_height = pDDB->cy;
  }

  // Normal text colors
  clrBackground = background;
  clrText = foreground;

#ifdef GRP_ENABLED
#error code not present
#endif /* GRP_ENABLED */

  // Render the text
  render_unicode_string (
                        pDDB->pBmp,
                        pDDB->nPitch/OEM_PIXEL_SIZE,
                        x,
                        y,
                        pcText,
                        nChars,
                        font,
                        clrText,
                        clrBackground,
                        clip_x,
                        clip_y,
                        clip_width,
                        clip_height
                        );
  IDIB_Release(dstDib);

  return(SUCCESS);
} /* End DrawText */


/*---------------------------------------------------------------------------
 *            External Functions
 *--------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION       CHARTOIMG_INIT

DESCRIPTION
               This function initialises and clears the entire display screen.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void chartoimg_init(void)
{
  AEERect  rect;
  IBitmap  *pDevBmp = NULL;
  IBitmap  *pBmp = NULL;

  if (chartoimg_initialized) return;

  /* Since pBmp could have been allocated due to error condition
   * below, we have to make sure there is no memory leak */
  if (pDDB)
    IDIB_Release(pDDB);
  pDDB = NULL;

  pShell = AEE_GetShell();

  if (pShell)
  {
    ISHELL_CreateInstance(pShell, AEECLSID_DISPLAY, (void **)&pDisplay);
  }

  if (pDisplay)
  {
    // Get the device bitmap to create a DDB
    IDISPLAY_GetDeviceBitmap(pDisplay, &pDevBmp);

    /* Create a new DDB */
    IBITMAP_CreateCompatibleBitmap(pDevBmp, &pBmp, CHAR_DISPLAY_WIDTH,
                               CHAR_DISPLAY_HEIGHT);
    IBITMAP_Release(pDevBmp);

    SETAEERECT(&rect, 0, 0, CHAR_DISPLAY_WIDTH, CHAR_DISPLAY_HEIGHT);
    clrFill = IBITMAP_RGBToNative(pBmp, MAKE_RGB(0xff, 0xff, 0xff)); // White
    clrFrame = IBITMAP_RGBToNative(pBmp, MAKE_RGB(0, 0, 0)); // Black
    IBITMAP_FillRect(pBmp, &rect, clrFill, AEE_RO_COPY);
    
#ifndef FEATURE_UIONE_HDK
#ifndef USES_MMI
#ifdef FEATURE_UI_CORE
    if (CoreApp_IsSKUI())
#endif /* FEATURE_UI_CORE */
#endif
    DrawRect(pBmp, &rect, clrFrame, AEE_RO_COPY);
#endif /* FEATURE_UIONE_HDK */

    IBITMAP_QueryInterface(pBmp, AEECLSID_DIB, (void **) &pDDB);
    IBITMAP_Release(pBmp);

#ifndef FEATURE_UIONE_HDK
#ifndef USES_MMI
#ifdef FEATURE_UI_CORE
    if (CoreApp_IsSKUI())
#endif /* FEATURE_UI_CORE */
#endif
    disp_update(pDDB->pBmp,
                CHAR_DISPLAY_WIDTH,
                0,
                0,
                CHAR_DISPLAY_HEIGHT,
                CHAR_DISPLAY_WIDTH,
                CHAR_DISPLAY_OFFSET_Y,
                CHAR_DISPLAY_OFFSET_X);
#endif /* FEATURE_UIONE_HDK */

#ifdef FEATURE_ANNUNCIATOR
    if(pShell != NULL)
    {
        if (ISHELL_CreateInstance (pShell, AEECLSID_ANNUNCIATOR, (void **) &pAnn) != SUCCESS) {
            MSG_ERROR ("Annunciator interface could not be created", 0, 0, 0);
            return;
        }
    }
#endif /* FEATURE_ANNUNCIATOR */
    chartoimg_initialized = TRUE;
  }
  else
  {
    /* If we cannot get a display object, BREW must have not been
    ** initialized. We got here because of error that happened before
    ** BREW was initialized.
    ** OEM_InitDispInfo must be called for OEM functions to function.
    ** We need to get a bitmap for display.
    ** DO NOT pretend chartoimg is initialized.
    */
#if !MIN_BREW_VERSION(4,0)
    OEM_InitDispInfo();
#ifdef DISP_DEVICE_18BPP
    OEMBitmap18_New(CHAR_DISPLAY_WIDTH, CHAR_DISPLAY_HEIGHT, 0, &pBmp);
#else /* DISP_DEVICE_18BPP */
    OEMBitmap16_New(CHAR_DISPLAY_WIDTH, CHAR_DISPLAY_HEIGHT, 0, &pBmp);
#endif /* DISP_DEVICE_18BPP */
    SETAEERECT(&rect, 0, 0, CHAR_DISPLAY_WIDTH, CHAR_DISPLAY_HEIGHT);
    clrFill = IBITMAP_RGBToNative(pBmp, MAKE_RGB(0xff, 0xff, 0xff)); // White
    clrFrame = IBITMAP_RGBToNative(pBmp, MAKE_RGB(0, 0, 0)); // Black
    IBITMAP_FillRect(pBmp, &rect, clrFill, AEE_RO_COPY);
    DrawRect(pBmp, &rect, clrFrame, AEE_RO_COPY);
    IBITMAP_QueryInterface(pBmp, AEECLSID_DIB, (void **) &pDDB);
    IBITMAP_Release(pBmp);
    disp_update(pDDB->pBmp,
                CHAR_DISPLAY_WIDTH,
                0,
                0,
                CHAR_DISPLAY_HEIGHT,
                CHAR_DISPLAY_WIDTH,
                CHAR_DISPLAY_OFFSET_Y,
                CHAR_DISPLAY_OFFSET_X);
#endif
  }
} /* End chartoimg_init */


/*===========================================================================

FUNCTION       CHARTTOIMG_UPDATE_SCREEN

DESCRIPTION
               This function updates the screen portion of the display
               with the input character buffer. It basically draws
               the text on the screen portion.
DEPENDENCIES
  Assumes chartoimg_init has already been called to initialize the text
  image.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void chartoimg_update_screen(
  /* A pointer to a buffer containing the 4 lines of characters */
  byte *screen_ptr
)
{
  AECHAR   wstr[CHARS_PER_LINE + 1];
  AEERect  clipRect, fillRect;
  IBitmap  *pBmp = NULL;
  int i;

  if (!chartoimg_initialized)
    chartoimg_init();

  SETAEERECT(&clipRect, TEXT_OFFSET_X, TEXT_OFFSET_Y,
             TEXT_WIDTH, TEXT_HEIGHT);

  for (i = 0; i < MAX_NUM_LINES; i++)
  {
    /* Draw only if changed */
    if ( memcmp (screen_ptr+i*CHARS_PER_LINE, keep_str+i*CHARS_PER_LINE,
                 CHARS_PER_LINE) )
    {
    /* Convert to AECHAR */
      STR_TO_WSTR((char *) (screen_ptr+i*CHARS_PER_LINE), wstr, sizeof(wstr));
      SETAEERECT(&fillRect, TEXT_OFFSET_X, TEXT_OFFSET_Y + i * LINE_HEIGHT,
                 TEXT_WIDTH, LINE_HEIGHT);
      pBmp = IDIB_TO_IBITMAP(pDDB);
      IBITMAP_FillRect(pBmp, &fillRect, clrFill, AEE_RO_COPY);
      /* Draw text using MSM default font */
      DrawText(IDIB_TO_IBITMAP(pDDB), TEXT_OFFSET_X, (TEXT_OFFSET_Y + i*LINE_HEIGHT), wstr,
               CHARS_PER_LINE, clrFrame, clrFill, &clipRect);

      memcpy (keep_str+i*CHARS_PER_LINE, screen_ptr+i*CHARS_PER_LINE,
          CHARS_PER_LINE);
    }
  }

  /* Update the display */
  disp_update(pDDB->pBmp,
              CHAR_DISPLAY_WIDTH,
              TEXT_OFFSET_Y,
              TEXT_OFFSET_X,
              TEXT_HEIGHT,
              TEXT_WIDTH,
              CHAR_DISPLAY_OFFSET_Y + TEXT_OFFSET_Y,
              CHAR_DISPLAY_OFFSET_X + TEXT_OFFSET_X);
  return;
} /* End chartoimg_update_screen */


/*===========================================================================

FUNCTION       CHARTOIMG_UPDATE_ANNUNCIATORS

DESCRIPTION
  Assumes chartoimg_init() has already been called to initialize the
  images.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void chartoimg_update_annunciators(
  /* A pointer to the annunciators to set */
  word *annunciator_ptr,
  /* A pointer to the RSSI level to set */
  byte *rssi_ptr,
  /* A pointer to the battery level to set */
  byte *batt_ptr
)
{
#ifdef FEATURE_ANNUNCIATOR
  annunciatorData.annunciator = *annunciator_ptr;  /* Annunciator other than
                                                      battery and RSSI */

  annunciatorData.rssi = *rssi_ptr;                /* RSSI annunciator */
  annunciatorData.batt = *batt_ptr;                /* Battery annunciator */

#ifdef FEATURE_UI_CORE
  if (AEE_GetShell() == NULL) return;
#endif

  if (!chartoimg_initialized || !pAnn) return;

  /* Register a callback function to do the actual update of annunciators */
  CALLBACK_Init(&cb, (PFNNOTIFY) UpdateAnnunciators, &annunciatorData);
  ISHELL_Resume(pShell, &cb);

  return;
#endif /* FEATURE_ANNUNCIATOR */
} /* End chartoimg_update_annunciators */


#ifdef FEATURE_ERROR_MESSAGE_STANDALONE
/*===========================================================================

FUNCTION       CHARTTOIMG_ERROR_MESSAGE

DESCRIPTION
               This function displays an error message on the screen

DEPENDENCIES
  

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/* allocate enough memory for one line of display for 18BPP */
static uint8 chartoimg_imageBuf[LINE_HEIGHT*TEXT_WIDTH*4];

void chartoimg_error_message(
  /* A pointer to a buffer containing the 4 lines of characters */
  byte *msg
)
{
  uint16          wstr[CHARS_PER_LINE + 1]; /* buffer to convert the message 
                                               to unicode */
  int             i, j; /* counter variables */
  /* size of image buffer is fixed */
  const uint16    imgWidth = TEXT_WIDTH, imgHeight = LINE_HEIGHT;
  uint16          dispX, dispY;
#ifdef FEATURE_HS_MDDI_ENABLE_ERROR_DISPLAY  
  uint16          disp_width, disp_height;
#endif
  disp_info_type  dispInfo;
  NativeColor     clrBackground, clrText;
  OEMFontInfo    *font = &font_msm_system;

#ifndef FEATURE_HS_MDDI_ENABLE_ERROR_DISPLAY
/* determine parameters of LCD: pixel size, eventually to reduce
   * compile time definitions */
  dispInfo = disp_get_info();

  /* Making of the infamous BLUE SCREEN */
  clrBackground = 0x234;
  clrText = 0xffffffff;

  #ifdef FEATURE_MDP
  /* Flag mdp for error mode updating */
  mdp_set_fatal_err();
  #endif /* FEATURE_MDP */

  /* initialize the image buffer */
  memset((void*)chartoimg_imageBuf, 0xff, sizeof(chartoimg_imageBuf));

  disp_clear_screen_area(40, dispInfo.disp_height, 0, dispInfo.disp_width);

  /* center the image on the screen */
  dispX = (dispInfo.disp_width - imgWidth) / 2;
  dispY = 40 + TEXT_OFFSET_Y;
#else 
  /* Making of the infamous BLUE SCREEN */
  clrBackground = 0x234;
  clrText = 0xffffffff;

  /* initialize the image buffer */
  memset((void*)chartoimg_imageBuf, 0xff, sizeof(chartoimg_imageBuf));

#ifdef FEATURE_EARLY_ERROR_RESET
  if (boot_early_error()) 
  {
    disp_width = 240;
    disp_height = 320;
  }
  else
#endif
  {
    (void)mddi_lcd_disp_init();
    dispInfo = mddi_lcd_disp_get_info(1);
    disp_width = dispInfo.disp_width;
    disp_height = dispInfo.disp_height;
  }

  /* clear the screen */
  mddi_queue_splash_screen(NULL, TRUE, disp_width, 0, 0, disp_height, disp_width, 0, 0);


  /* center the image on the screen */
  dispX = (disp_width - imgWidth) / 2;
  dispY = 40 + TEXT_OFFSET_Y;
#endif /* #ifndef FEATURE_HS_MDDI_ENABLE_ERROR_DISPLAY */
  
  /* draw the text line by line */
  for (i = 0; i < MAX_NUM_LINES; i++)
  {
    /* Convert to unicode */
    for (j = 0; j < CHARS_PER_LINE; j++)
    {
      wstr[j] = *(msg + i*CHARS_PER_LINE + j);
    }
    /* Render the text */
    render_unicode_string (
                          chartoimg_imageBuf,/* image buffer to render into */
                          imgWidth,       /* pixels per row in image buffer */
                          0,              /* start of column in buffer to render */
                          0,              /* start of row in buffer to render */
                          wstr,           /* unicode string */
                          CHARS_PER_LINE, /* max chars per line */
                          font,           /* font to use */
                          clrText,        /* color of text */
                          clrBackground,  /* color of background */
                          0,              /* start of clipping column */
                          0,              /* start of clipping row */
                          imgWidth,       /* width of clipping area */
                          imgHeight       /* height of clipping area */
                          );
#ifndef FEATURE_HS_MDDI_ENABLE_ERROR_DISPLAY
    disp_update(chartoimg_imageBuf,           /* image buffer to display to screen */
                imgWidth,          /* src image width*/
                0,                  /* src starting row */
                0,                  /* src starting column */
                imgHeight,          /* num of rows to update */
                imgWidth,           /* num of columns to update */
                dispY,              /* dst start row */
                dispX               /* dst start column */
                );
    dispY += imgHeight;
#else
    mddi_set_msg_screen(TRUE);

    mddi_queue_splash_screen (chartoimg_imageBuf,    /* image buffer to display to screen */ 
                              FALSE,                 /* clear display screen */
                imgWidth,          /* src image width*/
                0,                  /* src starting row */
                0,                  /* src starting column */
                imgHeight,          /* num of rows to update */
                imgWidth,           /* num of columns to update */
                dispY,              /* dst start row */
                dispX               /* dst start column */
                );
    dispY += imgHeight;
#endif /* #ifndef FEATURE_HS_MDDI_ENABLE_ERROR_DISPLAY */
  }

#ifndef FEATURE_HS_MDDI_ENABLE_ERROR_DISPLAY
#ifdef FEATURE_MDP
  /* Clear Flag mdp for error mode updating to prevent performance impact*/
  mdp_clear_fatal_err();
#endif /* FEATURE_MDP */
#endif /* #ifndef FEATURE_HS_MDDI_ENABLE_ERROR_DISPLAY */
}
#else /* FEATURE_ERROR_MESSAGE_STANDALONE */

/*===========================================================================

FUNCTION       CHARTTOIMG_ERROR_MESSAGE

DESCRIPTION
               This function displays an error message on the screen

DEPENDENCIES
  Assumes chartoimg_init has already been called to initialize the text
  image.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void chartoimg_error_message(
  /* A pointer to a buffer containing the 4 lines of characters */
  byte *msg
)
{
  AECHAR   wstr[CHARS_PER_LINE + 1];
  AEERect  clipRect, fillRect;
  IBitmap  *pBmp = NULL;
  int i;

  #ifdef FEATURE_MDP
  /* Flag mdp for error mode updating */
  mdp_set_fatal_err();
  #endif /* FEATURE_MDP */

  if (!chartoimg_initialized)
    chartoimg_init();

  disp_clear_screen_area(40, 220, 0, 176);

  /* Fill and draw text rectangle */
  SETAEERECT(&fillRect, 0, 0, CHAR_DISPLAY_WIDTH, CHAR_DISPLAY_HEIGHT);
  pBmp = IDIB_TO_IBITMAP(pDDB);
  IBITMAP_FillRect(pBmp, &fillRect, clrFill, AEE_RO_COPY);
  DrawRect(IDIB_TO_IBITMAP(pDDB), &fillRect, clrFrame, AEE_RO_COPY);

  SETAEERECT(&clipRect, TEXT_OFFSET_X, TEXT_OFFSET_Y,
             TEXT_WIDTH, TEXT_HEIGHT);

  for (i = 0; i < MAX_NUM_LINES; i++)
  {
    /* Convert to AECHAR */
    STR_TO_WSTR((char *) (msg+i*CHARS_PER_LINE), wstr, sizeof(wstr));
    /* Draw text using MSM default font */
    DrawText(IDIB_TO_IBITMAP(pDDB), TEXT_OFFSET_X, (TEXT_OFFSET_Y + i*LINE_HEIGHT), wstr,
             CHARS_PER_LINE, clrFrame, clrFill, &clipRect);
  }

  /* Update the display */
  disp_update(pDDB->pBmp,
              CHAR_DISPLAY_WIDTH,
              0,
              0,
              CHAR_DISPLAY_HEIGHT,
              CHAR_DISPLAY_WIDTH,
              CHAR_DISPLAY_OFFSET_Y,
              CHAR_DISPLAY_OFFSET_X);
  
  #ifdef FEATURE_MDP
  /* Clear Flag mdp for error mode updating to prevent performance impact*/
  mdp_clear_fatal_err();
  #endif /* FEATURE_MDP */

  return;

} /* End chartoimg_error_message */
#endif /* FEATURE_ERROR_MESSAGE_STANDALONE */

/*========================================================================

FUNCTION      LCD_DEBUG_MESSAGE

DESCRIPTION
              Write the specified message to the LCD.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

========================================================================*/
#if (HS_SCRN_HIGH != MAX_NUM_LINES) || (HS_SCRN_WIDE != CHARS_PER_LINE)
#error "debug message size conflict between chartoimg.c and err.c"
#endif

void lcd_debug_message (char* msg)
{
  static char text[CHARS_PER_LINE * MAX_NUM_LINES];
  int len;
#ifdef CUST_EDITION
  // Open Backlight
  disp_set_backlight(LCD_BACKLIGHT_LVL_2);
#endif
  /* Make sure we copy 4 lines of text */
  memset (text, ' ', sizeof(text));
  len = strlen(msg);
  if( len > CHARS_PER_LINE * MAX_NUM_LINES )
  {
    len = CHARS_PER_LINE * MAX_NUM_LINES;
  }
  memcpy (text, msg, len);
  chartoimg_error_message((byte *) text);
}

/*========================================================================

FUNCTION      LCD_MESSAGE

DESCRIPTION
              Display the specified message on the LCD, then put the
              phone into download mode.

              This function is only called after a fatal error.  We
              wish to inform the user of the error, then put the
              phone in download mode so that new (and hopefully better)
              code can be downloaded to the phone.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

========================================================================*/
void lcd_message (char* msg)
{
  lcd_debug_message(msg);
}

#endif /*#if !defined(FEATURE_MULTIPROCESSOR) || 
         (defined(FEATURE_MULTIPROCESSOR) && defined(IMAGE_APPS_PROC))*/
#endif /* defined(FEATURE_BITMAP) && !defined(FEATURE_UI_CORE_REMOVED) */

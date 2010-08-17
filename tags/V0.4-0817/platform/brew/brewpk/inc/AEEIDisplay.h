#ifndef AEEIDISPLAY_H
#define AEEIDISPLAY_H
/*=============================================================================

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
=============================================================================*/

#include "AEEIBase.h"
#include "AEECallback.h"
#include "AEEIBitmap.h"
#include "AEEIDIB.h"
#include "AEEIFont.h"

/*-------------------------------------------------------------------
      Type Declarations
-------------------------------------------------------------------*/

// IDisplay flags
// Rect flags -- IDF_INVERTRECT overrides the other QRECT flags 
#define IDF_RECT_NONE           0x00000000
#define IDF_RECT_FRAME          0x00000001
#define IDF_RECT_FILL           0x00000002
#define IDF_RECT_INVERT         0x00000004
#define IDF_RECT_MASK           0x0000000F      // mask to isolate QRECT flags

// Horizontal alignment --  (mutually exclusive within this group) 
#define IDF_ALIGN_NONE          0x00000000
#define IDF_ALIGN_LEFT          0x00000010
#define IDF_ALIGN_CENTER        0x00000020
#define IDF_ALIGN_RIGHT         0x00000040
#define IDF_ALIGN_FILL          0x00000080
#define IDF_ALIGNHORZ_MASK      0x000000F0      // mask to isolate ALIGN(horizontal) flags

// Vertical alignment -- (mutually exclusive within this group) 
#define IDF_ALIGN_TOP           0x00000100
#define IDF_ALIGN_MIDDLE        0x00000200
#define IDF_ALIGN_BOTTOM        0x00000400
#define IDF_ALIGN_SPREAD        0x00000800
#define IDF_ALIGNVERT_MASK      0x00000F00      // mask to isolate ALIGN(vertical) flags
#define IDF_ALIGN_MASK          (IDF_ALIGNHORZ_MASK|IDF_ALIGNVERT_MASK)

// Text Format Flags 
#define IDF_TEXT_UNDERLINE          0x00002000      // Underline text
#define IDF_TEXT_INVERTED           0x00004000      // Inverted text (highlights)
#define IDF_TEXT_TRANSPARENT        0x00008000      // Background is preserved...
#define IDF_TEXT_FORMAT_OEM         0x00010000      // Text is OEM_RAW_TEXT format (byte *)
#define IDF_TEXT_DASHED_UNDERLINE   0x00020000      // Dashed underline text


// Logical font type
enum {
   AEE_FONT_NORMAL=0x8000,
   AEE_FONT_BOLD,
   AEE_FONT_LARGE,
   AEE_FONT_ITALIC,
   AEE_FONT_BOLD_ITALIC,
   AEE_FONT_LARGE_ITALIC,
   AEE_FONT_USER_1,
   AEE_FONT_USER_2,
   AEE_FONT_TOTAL
};

#ifdef _WIN32
typedef unsigned AEEFont;
#else
typedef uint16 AEEFont;
#endif

#define AEE_NUM_FONTS   (AEE_FONT_TOTAL - AEE_FONT_NORMAL)


#define AEESymbol  uint16


// Special symbols - Resolve using GetSymbol
#define  AEE_SYM_ELLIPSES     0
#define  AEE_SYM_AM           0x1
#define  AEE_SYM_PM           0x2
#define  AEE_SYM_KEY_LEFT     0x3
#define  AEE_SYM_KEY_RIGHT    0x4
#define  AEE_SYM_KEY_CLR      0x5
#define  AEE_SYM_KEY_1        0x6
#define  AEE_SYM_KEY_2        0x7
#define  AEE_SYM_KEY_3        0x8
#define  AEE_SYM_KEY_4        0x9
#define  AEE_SYM_KEY_5        0xa
#define  AEE_SYM_KEY_6        0xb
#define  AEE_SYM_KEY_7        0xc
#define  AEE_SYM_KEY_8        0xd
#define  AEE_SYM_KEY_9        0xe
#define  AEE_SYM_KEY_0        0xf
#define  AEE_SYM_KEY_STAR     0x10
#define  AEE_SYM_KEY_POUND    0x11
#define  AEE_SYM_KEY_SEND     0x12
#define  AEE_SYM_KEY_END      0x13
#define  AEE_SYM_KEY_SELECT   0x14

#define  AEE_SYM_USER         0xff

// COLOR Definitions

// The following ENUM specifies both active drawing and system color types.
// 
// Active drawing colors (CLR_XXXX) are used in the DrawText routine.  
// System colors (SYSCLR_XXX) are not used automatically by the
// DrawText, FillRect routines.  They are provided so that apps/tools can 
// leverage what the OEM specifies as the default colors for certain 
// visible objects.
//
// NOTE: Only the CLR_XXX constants can be changed via SetColor.  The system
// colors can be retrieved via SetColor(pd, SYSCLR_XXX, RGB_NONE).  Thus, to 
// set the current active text color to the system color for SYSCLR_BUTTON_TEXT, 
// the following calls would be made.
//
// clr = IDisplay_SetColor(pd, CLR_TEXT, SYSCLR_BUTTON_TEXT);
// (Do any display processing...)
// IDisplay_SetColor(pd, CLR_TEXT, clr);  // Restore the color...
//

enum {
   CLR_USER_TEXT=1,         // Active Text Color
   CLR_USER_BACKGROUND,     // Active Background color
   CLR_USER_LINE,           // Active Line color (frames, etc.)
   CLR_SYS_TITLE,           // Title Background
   CLR_SYS_TITLE_TEXT,      // Title Text
   CLR_SYS_ITEM,            // ITEM (softkey, menu, button) Background
   CLR_SYS_ITEM_TEXT,       // ITEM Text
   CLR_SYS_ITEM_SEL,        // ITEM Selected Background
   CLR_SYS_ITEM_SEL_TEXT,   // ITEM Selected Text
   CLR_SYS_WIN,             // Standard Window Background
   CLR_SYS_FRAME_HI,        // Frame Highlight color (usually white)
   CLR_SYS_FRAME_LO,        // Frame solid color (usually black)
   CLR_SYS_LT_SHADOW,       // Shadow color (usually light grey)
   CLR_SYS_DK_SHADOW,       // Shadow color (usually dark grey)
   CLR_SYS_SCROLLBAR,       // Scroll Bars - Background
   CLR_SYS_SCROLLBAR_FILL,  // Scroll Bars - Filled area
   CLR_SYS_LAST
};

#ifdef _WIN32
typedef unsigned AEEClrItem;
#else
typedef int8 AEEClrItem;
#endif

#define CLR_USER_FRAME     (CLR_USER_LINE)

#define MAX_ACTIVE_COLOR   (CLR_USER_LINE)
#define MAX_SYSTEM_COLOR   (CLR_SYS_LAST - 1)

// Annunciators

#if !defined(ANNUN_MSG)
#define ANNUN_PTC_SRVC_AVAIL      0x0008
#define ANNUN_PTC_SRVC_UNAVAIL    0x0010
#define ANNUN_PTC_SESSION_ACTIVE  0x0020
#define ANNUN_NET_SECURE    0x0040     //SSL. Used by Browsers when they go to a secure page
#define ANNUN_GSM1X         0x0080
#define ANNUN_MMS           0x0100
#define ANNUN_MSG           0x0200
#define ANNUN_NET_MSG       0x0400
#define ANNUN_ALARMCLOCK    0x0800
#define ANNUN_NET_LOCK      0x1000
#define ANNUN_STOPWATCH     0x2000
#define ANNUN_COUNTDOWN     0x4000
#define ANNUN_SILENCEALL    0x8000
#endif

//
// Frame Styles - Used in many standard objects.  
//

enum {
   AEE_FT_NONE,            // No Frame
   AEE_FT_EMPTY,           // 1 Pixel Offset - No Frame Drawn
   AEE_FT_3D_EMPTY,        // 2 Pixel Offset - No Frame Drawn
   AEE_FT_RAISED,          // 3D 2 Pixel Raised Frame
   AEE_FT_LOWERED,         // 3D 2 Pixel Lowered Frame
   AEE_FT_BOX,             // 1 Pixel Box 
   AEE_FT_INDENT,          // 3D 1 Pixel Lowered Frame
   AEE_FT_TAB_BOTTOM_SEL,  // Tab - 1 Pixel with tab orientation down
   AEE_FT_TAB_BOTTOM,      // Tab - 1 Pixel unselected tab (nothing)
   AEE_FT_TAB_TOP_SEL,     // Tab - 1 Pixel with tab orientation up
   AEE_FT_TAB_TOP          // Tab - 1 Pixel unselected tab (nothing)
};

#ifdef _WIN32
typedef unsigned AEEFrameType;
#else
typedef int8 AEEFrameType;
#endif

#define AEE_FRAME_SIZE           1     // Single pixel frame sizes
#define AEE_3D_FRAME_SIZE        2     // 3D frame sizes

//*****************************************************************************
//
// IDisplay Definition
//
//*****************************************************************************
#define AEEIID_IDisplay 0x01036d36

#define INHERIT_IDisplay(iname) \
   INHERIT_IBase(iname); \
   int      (*GetFontMetrics)(iname * po,AEEFont nFont,int * pnAscent,int * pnDescent); \
   int      (*MeasureTextEx)(iname * po, AEEFont nFont, const AECHAR * pcText,int nChars,int nMaxWidth, int * pnFits); \
   int      (*DrawText)(iname * po,AEEFont nFont, const AECHAR * pcText,int nChars,int x,int y,const AEERect * prcBackground,uint32 dwFlags); \
   void     (*DrawRect)(iname * po,const AEERect * pRect,RGBVAL clrFrame, RGBVAL clrFill, uint32 dwFlags); \
   void     (*BitBlt)(iname * po,int xDest,int yDest,int cxDest,int cyDest,const void * pbmSource,int xSrc,int ySrc,AEERasterOp dwRopCode); \
   void     (*Update)(iname * po, boolean bDefer); \
   void     (*SetAnnunciators)(iname * pd, uint16 wVal, uint16 wMask); \
   void     (*Backlight)(iname * po,boolean bOn); \
   RGBVAL   (*SetColor)(iname * po, AEEClrItem clr, RGBVAL rgb); \
   AECHAR   (*GetSymbol)(iname * po,AEESymbol sym, AEEFont nFont); \
   int      (*DrawFrame)(iname * po, AEERect * prc,AEEFrameType ft, RGBVAL rgbFill); \
   int      (*CreateDIBitmap)(iname *po, IDIB **ppIDIB, uint8 colorDepth, uint16 w, uint16 h); \
   int      (*SetDestination)(iname *po, IBitmap *pDst); \
   IBitmap *(*GetDestination)(iname *po); \
   int      (*GetDeviceBitmap)(iname *po, IBitmap **ppIBitmap); \
   IFont *  (*SetFont)(iname *po, AEEFont nFont, IFont *piFont); \
   void     (*SetClipRect)(iname *po, const AEERect * pRect); \
   void     (*GetClipRect)(iname *po, AEERect * pRect); \
   int      (*Clone)(iname *po, IDisplay **ppIDisplayNew); \
   void     (*MakeDefault)(iname *po); \
   boolean  (*IsEnabled)(iname *po); \
   int      (*NotifyEnable)(iname *po, AEECallback *pcb); \
   int      (*CreateDIBitmapEx)(iname *po, IDIB **ppIDIB, int nDepth, int nHeight, int nWidth, int nPaletteSize, int cbExtra); \
   int      (*SetPrefs)(iname *po, const char *pchSettings, int cSettings)

AEEINTERFACE_DEFINE(IDisplay);

// Access macros for IDisplay

static __inline uint32 IDisplay_AddRef(IDisplay *p)
{
   return AEEGETPVTBL(p, IDisplay)->AddRef(p);
}
static __inline uint32 IDisplay_Release(IDisplay *p)
{
   return AEEGETPVTBL(p, IDisplay)->Release(p);
}
static __inline int IDisplay_GetFontMetrics(IDisplay *p, AEEFont f, int *a, int *d)
{
   return AEEGETPVTBL(p, IDisplay)->GetFontMetrics((p),(f),(a),(d));
}
static __inline int IDisplay_MeasureTextEx(IDisplay *p, AEEFont f, const AECHAR *psz, int cb, int nmw, int *fits)
{
   return AEEGETPVTBL(p, IDisplay)->MeasureTextEx((p),(f),(psz),(cb),(nmw),(fits));
}
static __inline int IDisplay_MeasureText(IDisplay *p, AEEFont f, const AECHAR *psz)
{
   return AEEGETPVTBL(p, IDisplay)->MeasureTextEx((p),(f),(psz),-1,-1,NULL);
}
static __inline int IDisplay_DrawText(IDisplay *p, AEEFont f, const AECHAR *psz, int nl, int x, int y, const AEERect *prcb, uint32 flags)
{
   return AEEGETPVTBL(p, IDisplay)->DrawText((p),(f),(psz),(nl),(x),(y),(prcb),(flags));
}
static __inline void IDisplay_DrawRect(IDisplay *p, const AEERect *pr, RGBVAL cf, RGBVAL cfill, uint32 flags)
{
   AEEGETPVTBL(p, IDisplay)->DrawRect((p),(pr),(cf),(cfill),(flags));
}
static __inline void IDisplay_BitBlt(IDisplay *p, int xd, int yd, int w, int h, const void *ps, int xs, int ys, AEERasterOp rop)
{
   AEEGETPVTBL(p, IDisplay)->BitBlt((p),(xd),(yd),(w),(h),(ps),(xs),(ys),(rop));
}
static __inline void IDisplay_UpdateEx(IDisplay *p, boolean bDefer)
{
   AEEGETPVTBL(p, IDisplay)->Update((p), (bDefer));
}
static __inline void IDisplay_SetAnnunciators(IDisplay *p, uint16 wv, uint16 wm)
{
   AEEGETPVTBL(p, IDisplay)->SetAnnunciators((p),(wv),(wm));
}
static __inline void IDisplay_Backlight(IDisplay *p, boolean b)
{
   AEEGETPVTBL(p, IDisplay)->Backlight((p),(b));
}
static __inline RGBVAL IDisplay_SetColor(IDisplay *p, AEEClrItem clr, RGBVAL rgb)
{
   return AEEGETPVTBL(p, IDisplay)->SetColor((p),(clr),(rgb));
}
static __inline void IDisplay_Update(IDisplay *p)
{
   AEEGETPVTBL(p, IDisplay)->Update((p), (TRUE));
}
static __inline AECHAR IDisplay_GetSymbol(IDisplay *p, AEESymbol sym, AEEFont fnt)
{
   return AEEGETPVTBL(p, IDisplay)->GetSymbol((p),(sym),(fnt));
}
static __inline int IDisplay_DrawFrame(IDisplay *p, AEERect *prc, AEEFrameType bt, RGBVAL f)
{
   return AEEGETPVTBL(p, IDisplay)->DrawFrame((p),(prc),(bt),(f));
}
static __inline int IDisplay_CreateDIBitmap(IDisplay *p, IDIB **b, uint8 c, uint16 w, uint16 h)
{
   return AEEGETPVTBL(p, IDisplay)->CreateDIBitmap((p),(b),(c),(w),(h));
}
static __inline int IDisplay_CreateDIBitmapEx(IDisplay *p, IDIB **b, int c, int h, int w, int ps, int e)
{
   return AEEGETPVTBL(p, IDisplay)->CreateDIBitmapEx((p),(b),(c),(h),(w),(ps),(e));
}
static __inline int IDisplay_SetDestination(IDisplay *p, IBitmap *b)
{
   return AEEGETPVTBL(p, IDisplay)->SetDestination((p),(b));
}
static __inline IBitmap * IDisplay_GetDestination(IDisplay *p)
{
   return AEEGETPVTBL(p, IDisplay)->GetDestination(p);
}
static __inline int IDisplay_GetDeviceBitmap(IDisplay *p, IBitmap **b)
{
   return AEEGETPVTBL(p, IDisplay)->GetDeviceBitmap((p),(b));
}
static __inline IFont * IDisplay_SetFont(IDisplay *p, AEEFont f, IFont *pif)
{
   return AEEGETPVTBL(p, IDisplay)->SetFont((p),(f),(pif));
}
static __inline void IDisplay_SetClipRect(IDisplay *p, const AEERect *prc)
{
   AEEGETPVTBL(p, IDisplay)->SetClipRect((p),(prc));
}
static __inline void IDisplay_GetClipRect(IDisplay *p, AEERect *prc)
{
   AEEGETPVTBL(p, IDisplay)->GetClipRect((p),(prc));
}
static __inline int IDisplay_Clone(IDisplay *p, IDisplay **ppnew)
{
   return AEEGETPVTBL(p, IDisplay)->Clone((p),(ppnew));
}
static __inline void IDisplay_MakeDefault(IDisplay *p)
{
   AEEGETPVTBL(p, IDisplay)->MakeDefault((p));
}
static __inline boolean IDisplay_IsEnabled(IDisplay *p)
{
   return AEEGETPVTBL(p, IDisplay)->IsEnabled((p));
}
static __inline int IDisplay_NotifyEnable(IDisplay *p, AEECallback *c)
{
   return AEEGETPVTBL(p, IDisplay)->NotifyEnable((p),(c));
}
static __inline int IDisplay_SetPrefs(IDisplay *p, const char *s, int l)
{
   return AEEGETPVTBL(p, IDisplay)->SetPrefs((p),(s),(l));
}


static __inline int IDisplay_FrameButton(IDisplay *p, AEERect *prc, AEEFrameType b, RGBVAL f)
{
   return AEEGETPVTBL(p, IDisplay)->DrawFrame(p,prc,(AEEFrameType)(b ? AEE_FT_LOWERED : AEE_FT_RAISED), f);
}

/////////////////////////////////////////////
// Extra IDISPLAY function macros

static __inline void IDisplay_FillRect(IDisplay *p, const AEERect *pr, RGBVAL clrFill)
{
   AEEGETPVTBL(p, IDisplay)->DrawRect((p),(pr),RGB_NONE,(clrFill),IDF_RECT_FILL);
}

static __inline void IDisplay_ClearScreen(IDisplay *p)
{
   AEEGETPVTBL(p, IDisplay)->DrawRect((p),(NULL),RGB_NONE,(RGB_NONE),IDF_RECT_FILL);
}

static __inline void IDisplay_FrameRect(IDisplay *p, const AEERect *pr)
{
   AEEGETPVTBL(p, IDisplay)->DrawRect((p),(pr),RGB_NONE,RGB_NONE,IDF_RECT_FRAME);
}

static __inline void IDisplay_FrameSolidRect(IDisplay *p, const AEERect *pr)
{
   AEEGETPVTBL(p, IDisplay)->DrawRect((p),(pr),RGB_NONE,RGB_NONE,IDF_RECT_FRAME|IDF_RECT_FILL);
}

static __inline void IDisplay_InvertRect(IDisplay *p, const AEERect *pr)
{
   AEEGETPVTBL(p, IDisplay)->DrawRect((p),(pr),RGB_NONE,RGB_NONE,IDF_RECT_INVERT);
}

static __inline void IDisplay_EraseRect(IDisplay *p, const AEERect *pr)
{
   AEEGETPVTBL(p, IDisplay)->DrawRect((p),(pr),RGB_NONE,(RGB_NONE),IDF_RECT_FILL);
}

/*
===============================================================================
DATA STRUCTURES DOCUMENTATION
===============================================================================

AEEClrItem

Description:
    This ENUM specifies color types for active drawing and system colors. Starting
with BREW 2.1, there is no distinction between user items and system items. 

Definition:
typedef enum 
{ 
   CLR_USER_TEXT=1, 
   CLR_USER_BACKGROUND, 
   CLR_USER_LINE, 
   CLR_SYS_TITLE,
   CLR_SYS_TITLE_TEXT,
   CLR_SYS_ITEM,
   CLR_SYS_ITEM_TEXT,
   CLR_SYS_ITEM_SEL, 
   CLR_SYS_ITEM_SEL_TEXT,
   CLR_SYS_WIN,
   CLR_SYS_FRAME_HI,
   CLR_SYS_FRAME_LO,
   CLR_SYS_LT_SHADOW,
   CLR_SYS_DK_SHADOW,
   CLR_SYS_SCROLLBAR,
   CLR_SYS_SCROLLBAR_FILL,
   CLR_SYS_LAST
} AEEClrItem;

Members:
 
CLR_USER_TEXT : Active Text Color
CLR_USER_BACKGROUND : Active Background color
CLR_USER_LINE : Active Line color (frames, and other items)
CLR_SYS_TITLE : Title Background
CLR_SYS_TITLE_TEXT : Title Text
CLR_SYS_ITEM : ITEM (SoftKey, menu, button) Background
CLR_SYS_ITEM_TEXT : ITEM Text
CLR_SYS_ITEM_SEL : ITEM Selected Background
CLR_SYS_ITEM_SEL_TEXT : ITEM Selected Text
CLR_SYS_WIN : Standard Window Background
CLR_SYS_FRAME_HI : Frame Highlight color (usually white)
CLR_SYS_FRAME_LO : Frame solid color (usually black)
CLR_SYS_LT_SHADOW : Shadow color (usually light grey)
CLR_SYS_DK_SHADOW : Shadow color (usually dark grey)
CLR_SYS_SCROLLBAR : Scroll Bars: Background
CLR_SYS_SCROLLBAR_FILL : Scroll Bars: Filled area


Comments:
Each instance of IDisplay maintains a table that associates these color items
with actual colors.  These associations may be obtained or changed with
IDisplay_SetColor().

See Also:
IDisplay_SetColor() 

===============================================================================

AEEFont

Description:
This ENUM specifies the logical font type used in IDisplay text drawing operations.

Definition:
enum {
   AEE_FONT_NORMAL=0x8000,
   AEE_FONT_BOLD,
   AEE_FONT_LARGE,
   AEE_FONT_ITALIC,
   AEE_FONT_BOLD_ITALIC,
   AEE_FONT_LARGE_ITALIC,
   AEE_FONT_USER_1,
   AEE_FONT_USER_2,
   AEE_FONT_TOTAL
};

Members:
 
These are fonts that may be implemented on a device:

AEE_FONT_NORMAL
AEE_FONT_BOLD
AEE_FONT_LARGE
AEE_FONT_ITALIC
AEE_FONT_BOLD_ITALIC
AEE_FONT_LARGE_ITALIC

These are not currently defined to correspond to particular fonts, but may
be used to map to user-defined fonts:

AEE_FONT_USER_1
AEE_FONT_USER_2

AEE_FONT_TOTAL marks the end of the enum.  Its value may change in future
versions of this header file, if new fonts are added.

Comments:
Not all devices will support every font in this list.

See Also:
None 

===============================================================================
 
AEE_FT_

 
AEE_FT_XXX is the format for AEEFrameType.

===============================================================================

AEEFrameType

Description:
This ENUM specifies the various frame types supported by the IDisplay_DrawFrame()
function. Frame types are also used when specifying menu item styles with IMENUCTL_SetStyle
function.

Definition:
typedef enum
{
   AEE_FT_NONE, 
   AEE_FT_EMPTY,
   AEE_FT_3D_EMPTY,
   AEE_FT_RAISED,
   AEE_FT_LOWERED, 
   AEE_FT_BOX,
   AEE_FT_INDENT,
   AEE_FT_TAB_BOTTOM_SEL,
   AEE_FT_TAB_BOTTOM,
   AEE_FT_TAB_TOP_SEL,
   AEE_FT_TAB_TOP
} AEEFrameType;

Members:
 
AEE_FT_NONE : No Frame
AEE_FT_EMPTY : 1 Pixel Offset: No Frame Drawn
AEE_FT_3D_EMPTY : 2 Pixel Offset: No Frame Drawn
AEE_FT_RAISED : 3D 2 Pixel Raised Frame
AEE_FT_LOWERED : 3D 2 Pixel Lowered Frame
AEE_FT_BOX : 1 Pixel Box
AEE_FT_INDENT : 3D 1 Pixel Lowered Frame
AEE_FT_TAB_BOTTOM_SEL : 3D Bottom Tab (selected)
AEE_FT_TAB_BOTTOM : 3D Bottom Tab
AEE_FT_TAB_TOP_SEL : 3D Top Tab (selected)
AEE_FT_TAB_TOP : 3D Top Tab

Comments:
   None

See Also:
   AEEItemStyle 

===============================================================================
 
AEE_SYM_

 
AEE_SYM_XXX is the format for AEESymbol.

===============================================================================

AEESymbol

Description:
This is a uint16 that specifies the special symbol whose corresponding AECHAR 
value is returned by IDisplay_GetSymbol() . Some mobile devices have AECHAR 
values defined for these symbols, and an IDisplay_DrawText() call with the 
AECHAR string corresponding to the symbol draws that symbol on the display.

Definition:
#define AEESymbol uint16

The following symbols are pre-defined
   AEE_SYM_ELLIPSES,
   AEE_SYM_AM,
   AEE_SYM_PM,
   AEE_SYM_KEY_LEFT,
   AEE_SYM_KEY_RIGHT,
   AEE_SYM_KEY_CLR,
   AEE_SYM_KEY_1,
   AEE_SYM_KEY_2,
   AEE_SYM_KEY_3,
   AEE_SYM_KEY_4,
   AEE_SYM_KEY_5,
   AEE_SYM_KEY_6,
   AEE_SYM_KEY_7,
   AEE_SYM_KEY_8,
   AEE_SYM_KEY_9,
   AEE_SYM_KEY_0,
   AEE_SYM_KEY_STAR,
   AEE_SYM_KEY_POUND,
   AEE_SYM_KEY_SEND,
   AEE_SYM_KEY_END,
   AEE_SYM_KEY_SELECT
   AEE_SYM_USER

Members:
 
AEE_SYM_ELLIPSES : Ellipses symbol
AEE_SYM_AM : Symbol indicating AM for 12H time format
AEE_SYM_PM : Symbol indicating PM for 12H time format
AEE_SYM_KEY_LEFT : Left Arrow Key
AEE_SYM_KEY_RIGHT : Right Arrow Key
AEE_SYM_KEY_CLR : Clear Key
AEE_SYM_KEY_1 : The "1" Key
AEE_SYM_KEY_2 : The "2" Key
AEE_SYM_KEY_3 : The "3" Key
AEE_SYM_KEY_4 : The "4" Key
AEE_SYM_KEY_5 : The "5" Key
AEE_SYM_KEY_6 : The "6" Key
AEE_SYM_KEY_7 : The "7" Key
AEE_SYM_KEY_8 : The "8" Key
AEE_SYM_KEY_9 : The "9" Key
AEE_SYM_KEY_0 : The "0" Key
AEE_SYM_KEY_STAR : The " * " Key
AEE_SYM_KEY_POUND : The "#" Key
AEE_SYM_KEY_SEND : The "Send", or "Call" Key
AEE_SYM_KEY_END : The "End" Key
AEE_SYM_KEY_SELECT : The Key to select, or "Enter" Key

AEE_SYM_USER:     For OEMs to defines their own symbols

Comments:
None

See Also:
None

===============================================================================
 
IDF_TEXT_

 
IDF_TEXT_XXX is the format for IDISPLAY Flags for Text Formats. 

===============================================================================
 
IDF_ALIGN_

 
IDF_ALIGN_XXX is the format for IDISPLAY Flags for alignment.

===============================================================================
 
IDF_RECT_

 
IDF_RECT_XXX is the format for IDISPLAY Flags.

===============================================================================

IDISPLAY Flags

Description:
These constants are used to build the flags (dwFlags parameter) in the IDisplay_DrawRect()
and IDisplay_DrawText() functions.

Definition:
Flags for the Rectangle
NOTE: IDF_RECT_INVERT overrides the other IDF_RECT flags.
 
IDF_RECT_NONE:       No Rectangle, used in IDisplay_DrawText() 
IDF_RECT_FRAME:      Draw rectangular frame. 
IDF_RECT_FILL:       Draw filled rectangle.
IDF_RECT_INVERT:     Invert the text and background colors.
IDF_RECT_MASK:       Mask to isolate IDF_RECT flags.

Flags for Horizontal alignment of text
NOTE: These flags are mutually exclusive: 
 
IDF_ALIGN_NONE:      No alignment specified. 
IDF_ALIGN_LEFT:      Left justified.
IDF_ALIGN_CENTER:    Centered. 
IDF_ALIGN_RIGHT:     Right justified.
IDF_ALIGN_FILL:      Fill the rectangle width (specified with IDisplay_DrawText() ) with the text.
IDF_ALIGNHORZ_MASK:  Mask to isolate ALIGN(horizontal) flags 

Flags for Vertical alignment of text
NOTE: These flags are mutually exclusive

IDF_ALIGN_TOP:       Align text with top of rectangle specified.
IDF_ALIGN_MIDDLE:    Place text in the middle of the specified rectangle.
IDF_ALIGN_BOTTOM:    Align text with the bottom of the rectangle specified.
IDF_ALIGN_SPREAD:    Spread the text over the height of the rectangle.
IDF_ALIGNVERT_MASK:  Mask to isolate ALIGN(vertical) flags.

IDF_ALIGN_MASK       Mask to isolate all text alignment flags

Flags for Text Formats 
 
IDF_TEXT_UNDERLINE:        Underline text
IDF_TEXT_INVERTED:         Inverted text (highlights)
IDF_TEXT_TRANSPARENT:      Background is preserved
IDF_TEXT_FORMAT_OEM:       Text is OEM_RAW_TEXT format (byte * )
IDF_TEXT_DASHED_UNDERLINE: Underline text using dashes

Members:
None

Comments:
None

See Also:
None

===============================================================================
*/

/*
===============================================================================
INTERFACES DOCUMENTATION
===============================================================================

IDisplay Interface

Description: 
   This interface provides display services. It provides functions
   for getting display metrics, drawing rectangles, drawing bitmaps, and so forth.
   An IDisplay is usually obtained with a class ID of AEECLSID_DISPLAY.
   This returns the current default IDisplay.  BREW maintains a reference to the
   default IDisplay, which it releases when the app exits.

   Clones of the default IDisplay may be obtained with AEECLSID_DISPLAYCLONE.
   This makes a new IDisplay, but copies all the state from the default IDisplay.

   To obtain a new instance of an IDisplay for a particular display, use
   AEECLSID_DISPLAYn, where n is a digit in the range of 1-4.  1 corresponds
   to the primary display.  This is available in BREW 2.1 and later.

   Any IDisplay can be made the default by calling IDisplay_MakeDefault().
   This is available in BREW 2.1 and later.
===H2>
   Display Access Priority
===/H2>
 
   Display access is granted differently for the primary display and any secondary
   displays.  For the primary display, access is granted to the currently active BREW
   app.  Secondary display access is granted first to the currently active app, and
   if that app is not trying to use the display, BREW goes through the
   background app list, starting with the most recently backgrounded app, and ending
   with the app that has been running in the background the longest.  An app that is
   trying use a display is defined as an app that has a reference to device bitmap
   for that display.  The OEM layer may preempt access to any display at any time.

   Access to a display may be disabled when an app obtains an IDisplay interface.
   IDisplay_IsEnabled() is provided to allow an app to determine whether is has
   access to the display.  An app may also register for notification of the display
   becoming enabled or disabled with IDisplay_NotifyEnable().  These function are
   provided in BREW 2.1 and later.

The following header file is required:

AEEDisp.h

===============================================================================

Function: IDisplay_AddRef()

This function is inherited from IBase_AddRef().

See Also:
   IDisplay_Release()

===============================================================================

Function: IDisplay_Release()

This function is inherited from IBase_Release().

See Also:
   IDisplay_AddRef()

===============================================================================

Function: IDisplay_GetFontMetrics()

Description:
		This function retrieves information about the specified font. It retrieves
      information about the ascent and descent values for the specified font.
      Note: ascent + descent = total character height.

Prototype:
	  int IDisplay_GetFontMetrics(IDisplay * po,
                                 AEEFont Font,
                                 int * pnAscent,
                                 int * pnDescent)

Parameters:
   po: [in]: Pointer to the IDisplay interface object whose font metrics is to be retrieved.
   Font: [in]: specifies the font type for which the Ascent and Descent information is 
   to be retrieved.
   pnAscent: [out]: This must be either NULL or a pointer to an int. On function return,
   the int specified (if any) will contain the Ascent value for the specified font.
   pnDescent: [out]: This must be either NULL or a pointer to an int. On function return,
   the int specified (if any) will contain the Descent value for the specified font.

Return Value:
   If successful, returns the character height for the specified font. This is the 
   sum of the Ascent and Descent values for the specified font. 

   If failed, returns EFAILED.

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also:
   IDisplay_DrawText(), 
   IDisplay_MeasureText(), 
   IDisplay_MeasureTextEx(),
   IDisplay_SetFont()

===============================================================================

Function: IDisplay_MeasureText()

Description:
  This function measures the width of a given text string if drawn using the 
  specified font.  


Prototype:
   int IDisplay_MeasureText(IDisplay * po,
                            AEEFont Font, 
                            const AECHAR * pcText)

Parameters:
   po :[in]: Pointer to the IDisplay interface object to be used for measuring the
   width of the given text.

   Font: [in]: Specifies the font that needs to be used for measuring the 
   width of the given text. This function measures the width of a given text 
   string if drawn using this font.

   pcText: [in]: Pointer to a zero-terminated string to be measured.

Return Value:
   If successful, returns the number of pixels required to draw the characters 
   in the string pcText. 
   
   If unsuccessful, returns 0.

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also:
   IDisplay_DrawText(), 
   IDisplay_GetFontMetrics(), 
   IDisplay_MeasureTextEx(),
   IDisplay_SetFont()

===============================================================================

Function: IDisplay_MeasureTextEx()

Description:
  This function measures the width of a given text string if drawn using the 
  specified font.  The return value is the actual pixel width of the string.


Prototype:
   int IDisplay_MeasureTextEx(IDisplay * po,
                              AEEFont Font, 
                              const AECHAR * pcText,
                              int nChars,
                              int nMaxWidth, 
                              int * pnFits)

Parameters:
   po :[in]: Pointer to the IDisplay interface object to be used for measuring the
   width of the given text.  

   Font: [in]: Specifies the font that needs to be used for measuring the 
   width of the given text. This function measures the width of a given text 
   string if drawn using this font.

   pcText: [in]: Pointer to characters to be measured.

   nChars: [in]: Specifies the number of characters in "pcText". If this is -1,
   the length will be computed by treating pcText[] as a zero-terminated string.
   
   nMaxWidth: [in]: Specifies the maximum available pixel width that can be
   used for drawing the text. If "nMaxWidth" is set to -1, then the nChars part of text 
   string is measured, and "*pnFits" returned will always be the entire text length.
   If "nMaxWidth" >= 0, then it represents the maximum available pixel width for
   the text to be rendered within; this function will compute the maximum
   number of characters which actually fit within this constraint and return
   this number of characters in "*pnFits" and the actual pixel width of these
   characters as the return value.

   pnFits:[out]: This must be either NULL or a pointer to an int. On function return,
   the int specified (if any) will contain the number of characters that fit the given
   pixel width specified by "nMaxWidth". If "nMaxWidth" is -1, the integer will
   contain the length of the entire string "pcText".


Return Value:
   If successful, returns the number of pixels required to draw the characters 
   in the string pcText. The number of characters that can be drawn in this 
   width is contained in "*pnFits".  
   
   If unsuccessful, returns 0.

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also:
   IDisplay_DrawText(), 
   IDisplay_GetFontMetrics(), 
   IDisplay_MeasureText(),
   IDisplay_SetFont()

===============================================================================

Function: IDisplay_DrawText()

Description:
  This function draws the given text at the given location, using the given
  font and bounds the text to the clipping rectangle.
  After calling this function, IDisplay_Update() must be called to update the screen.
  If there is a sequence of drawing operations being performed, it is sufficient
  to IDisplay_Update() once after all the drawing is done.

Prototype:
   int   IDisplay_DrawText(IDisplay * po,
                           AEEFont Font, 
                           const AECHAR * pcText,
                           int nChars,
                           int x,
                           int y,
                           const AEERect * prcBackground,
                           uint32 dwFlags)

Parameters:
   po :[in]: Pointer to the IDisplay interface object to be used for drawing the text.

   Font: [in]: Specifies the font that needs to be used for drawing the text.

   pcText: [in]: Contains the String that needs to be drawn.

   nChars: [in]: Specifies the number of characters in pcText. If this is -1,
   the length will be automatically computed by this function.
   
   x [in]: Specifies the left edge of the rectangle into which text will be drawn.
   This parameter is ignored when a horizontal alignment flag is set.

   y [in]: Specifies the top edge of the rectangle into which text will be drawn.
   This parameter is ignored when a vertical alignment flag is set.

   prcBackground [in]: Specifies the coordinates of the clipping rectangle. If this
   is NULL, the boundries of the destination bitmap are taken as the clipping rectangle.
   No text is drawn outside this clipping rectangle.
   If any IDF_RECT_XXXX flag is specified, this rect is also used as filling, and so forth.
   NOTE: This is distinct from the clipping rectangle set with IDisplay_SetClipRect.
   The text is clipped to the intersection of these two clipping rectangles.
   IDF_ALIGN_XXXX flags align the text relative to the rectangle specified with
   prcBackground only.

   dwFlags [in]: Specifies the flags that can be used for drawing the text. This 
   can be a logical OR of one of the items selected from each of the entries a,b,c below:
~
      a. One of the horizontal allignment flags (IDF_ALIGN_LEFT, IDF_ALIGN_CENTER,IDF_ALIGN_RIGHT)~
      b. One of the vertical allignment flags (IDF_ALIGN_TOP, IDF_ALIGN_MIDDLE,IDF_ALIGN_BOTTOM)~
      c. One of the text format flags (IDF_TEXT_UNDERLINE, IDF_TEXT_INVERTED, IDF_TEXT_DASHED_UNDERLINE)~
      d. One of the rect format flags (IDF_RECT_FRAME,IDF_RECT_FILL,IDF_RECT_INVERT) --- these flags
      works on the prcBackground rectangle, using the CLR_USER_BACKGROUND as the fill color
      and CLR_USER_FRAME as the frame color.
*

   If no alignment flags are specified, the position of the text is determined by parameter x and y.

   The text will block everything behind it by default.  
   To avoid this effect,use IDF_TEXT_TRANSPARENT flag.

   Currently in SDK IDF_ALIGN_SPREAD and IDF_ALIGN_FILL are not supported.

   Example: The combination IDF_ALIGN_CENTER | IDF_ALIGN_TOP | IDF_TEXT_UNDERLINE draws
   underlined text at the top (vertical) center (horizontal) of the clipping rectangle.

Return Value:
   If successful, returns SUCCESS.

   If failed, returns error code.

Comments:  
   Coordinates x==0 and y==0 refer to the top-left corner of the display.


Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also:
   IDisplay_GetFontMetrics(), 
   IDisplay_MeasureText(), 
   IDisplay_MeasureTextEx(),
   IDisplay_SetFont()

===============================================================================

Function: IDisplay_DrawRect()

Description:
  This function draws the given rectangle using the specified color and flags.
  After calling this function, IDisplay_Update() must be called to update the screen.
  If there is a sequence of drawing operations being performed, it is sufficient
  to IDisplay_Update() once after all the drawing is done.


Prototype:
   void  IDisplay_DrawRect(IDisplay * po,
                           const AEERect * pRect,
                           RGBVAL clrFrame, 
                           RGBVAL clrFill, 
                           uint32 dwFlags)

Parameters:
   po :[in]: Pointer to the IDisplay interface object to be used for drawing the rectangle.

   pRect: [in]: Pointer to a AEERect structure that defines the coordinates of the 
   rectangle to be drawn. These coordinates are in terms of destination bitmap coordinates
   with the left, top of the destination being 0,0.

   clrFrame: [in]: This specifies the color to be used for drawing the frame (outer
   borders) of the rectangle. This parameter is used only if dwFlags contains
   the flag IDF_RECT_FRAME.

   clrFill: [in]: This specifies the color to be used for filling the rectangle. 
   This parameter is used only if dwFlags contains  the flag IDF_RECT_FILL.

   dwFlags: [in]: Specifies the flags to be used for drawing the rectangle. This
   can be a logical OR of one or more of the following flags:~
~
      IDF_RECT_FRAME : Draw the outer borders of the rectangle only~
      IDF_RECT_FILL  : Fill the rectangle with clrFill color        ~
      IDF_RECT_INVERT: Invert the contents of the specified rectangle. When this flag 
						is set, clrFrame and clrFill parameters won't matter.~
*


Return Value:
   None

Comments:  
   If pRect is NULL and dwFlags contains IDF_RECT_FILL, this function clears the entire
   destination bitmap (or current clip rectangle if set) using clrFill. If pRect is NULL
   without IDF_RECT_FILL flag, this function treats pRect as an empty rectangle.

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also:
   IDisplay_DrawFrame()
   IDisplay_EraseRect()
   IDISPLAY_EraseRgn()
   IDisplay_FillRect()
   IDisplay_FrameButton()
   IDisplay_FrameRect()
   IDisplay_FrameSolidRect()
   IDisplay_InvertRect()

===============================================================================

Function: IDisplay_BitBlt()

Description:
  This function performs a bit-block transfer of the data corresponding to a 
  rectangle of pixels from the specified source bitmap into the given area in the
  destination bitmap.
  After calling this function, IDisplay_Update() must be called to update the 
  screen. If there is a sequence of drawing operations being performed, it is 
  sufficient to IDisplay_Update() once after all the drawing is done.


Prototype:
   void     IDisplay_BitBlt(IDisplay * po,
                            int xDest,
                            int yDest,
                            int cxDest,
                            int cyDest,
                            const void * pbmSource,
                            int xSrc,
                            int ySrc,
                            AEERasterOp dwRopCode)

Parameters:
   po :[in]: Pointer to the IDisplay interface object into which the bit-block
   transfer needs to be done.

   xDest: [in]: Specifies the x-coordinates of the upper left corner of the
   destination rectangular area.

   yDest: [in]: Specifies the y-coordinates of the upper left corner of the
   destination rectangular area.

   cxDest: [in]: Specifies the width of the destination rectangle. If this is
   greater than the width of the source bitmap (pmSource),
   this parameter is taken to be equal to the width of the source bitmap.

   cyDest: [in]: Specifies the height of the destination rectangle. If this is
   greater than the height of the source bitmap (pmSource),
   this parameter is taken to be equal to the height of the source bitmap.

   pbmSource: [in]: IBitmap pointer for the source bitmap.

   xSrc: [in]: Specifies the x-coordinate of the upper left corner of the
   source bitmap from where the bit-block transfer must begin.

   ySrc: [in]: Specifies the y-coordinate of the upper left corner of the
   source bitmap from where the bit-block transfer must begin.

   dwRopCode: [in]: Specifies the Raster operation that must be used
   while doing the bit-block transfer.


Return Value:
   None

Comments:  
   Prior to BREW 1.2, the pbmSource parameter pointed to an OEM-defined
   structure returned from CONVERTBMP().  Starting with BREW 1.2, this
   parameter takes a pointer to an IBitmap interface, and CONVERTBMP()
   returns an IBitmap interface pointer.

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also:
   IBITMAP_BltIn()

===============================================================================

Function: IDisplay_UpdateEx()

Description:
  This function updates the screen if the destination bitmap is the device
  bitmap. If the bDefer flag is set to false, the screen is refreshed (updated)
  immediately. If the bDefer flag is set to true, the update message is
  posted onto the queue of the UI task, thereby allowing all the drawings to
  be done before updating the screen. For example, the following pseudocode segment:
~
         1.DrawText;
         2.BitBlt;
         3.UpdateEx;
*
  will ensure that the screen is updated once after all the drawings are done.


Prototype:
   void     IDisplay_UpdateEx(IDisplay * po, boolean bDefer)

Parameters:
   po :[in]: Pointer to the IDisplay interface object that needs to be updated.

   bDefer: [in]: Boolean flag to indicate whether the screen needs to be
   updated immediately or deferred.

Return Value:
   None

Comments:  
   Prior to BREW 2.1, this function always updated the display from the device
   bitmap.  Starting in BREW 2.1, this function does nothing if the destination
   bitmap for this IDisplay is not the device bitmap.

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also:
   IDisplay_GetDestination(), 
   IDisplay_GetDeviceBitmap(), 
   IDisplay_Update()

===============================================================================

Function: IDisplay_Update()

Description:
  This function updates the screen if the destination bitmap is the device
  bitmap. The update message is posted onto the queue of the UI task, thereby
  allowing all the drawings to be done before updating the screen.


Prototype:
   void     IDisplay_Update(IDisplay * po)

Parameters:
   po :[in]: Pointer to the IDisplay interface object that needs to be updated.

Return Value:
   None

Comments:  
   Prior to BREW 2.1, this function always updated the display from the device
   bitmap.  Starting in BREW 2.1, this function does nothing if the destination
   bitmap for this IDisplay is not the device bitmap.

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also:
   IDisplay_GetDestination(), 
   IDisplay_GetDeviceBitmap(), 
   IDisplay_UpdateEx()

===============================================================================

Function: IDisplay_SetAnnunciators()

Description: 
   This function turns the specified annunciators on (or off). The support and 
behavior of this function is totally dependent on the specific handset. 

The complete list of annunciators is: ~
~
  
   ANNUN_MMS           To be used by MMS Applications~
   ANNUN_MSG           Voice mail ~
   ANNUN_NET_MSG       Net Message~
   ANNUN_ALARMCLOCK    Alarm Clock~
   ANNUN_NET_LOCK      Phone Locked~
   ANNUN_STOPWATCH     Stop Watch~
   ANNUN_COUNTDOWN     Count Down clock~
   ANNUN_SILENCEALL    Ringer Off~
*

   A handset may support few or all of the above listed Annunciators.

Prototype:
   void     IDisplay_SetAnnunciators(IDisplay * po, uint16 wVal, uint16 wMask)

Parameters:
   po :[in]: Pointer to the IDisplay interface object to be used for enabling/disabling
   annunciators.
   wVal [in]: Specifies that set of annunciators that this function is trying to
   enable or disable. This is a logical OR of one or more of the annunciator 
   flags defined above.
   wMask [in]: For each annunciator flag contained in wVal, the corresponding bit 
   here specifies if that annunciator needs to be turned on or off.

   For example: to turn the ANNUN_MSG and ANUN_NET_MSG annunciators ON and to 
   turn the ANNUN_COUNTDOWN annunciator off, use:
===pre>
      uint16 wVal = 0;
      uint16 wMask = 0;
      wVal |= ANNUN_MSG | ANNUN_NET_MSG | ANNUN_COUNTDOWN;
      wMask |= ANNUN_MSG | ANNUN_NET_MSG;
      IDisplay_SetAnnunciators(po,wVal,wMask);
===/pre>

Return Value:
   None

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also:
   None

===============================================================================

Function: IDisplay_Backlight()

Description: 
   This function turns the display backlight(s) of the phone on or off depending 
   on the parameter bOn. When this function is used to turn the backlight on, the 
   OEM of the handset (that is, handset manufacturer) decides how long it remains 
   on before it is turned off again. Similarly, when this function is used to turn 
   off the backlight, the OEM of the handset decides how long it remains off before 
   it is turned on again.


Prototype:
   void  IDisplay_Backlight(IDisplay * po,boolean bOn)

Parameters:
   po :[in]: Pointer to the IDisplay interface object to be used for controlling
   the backlight of the phone.
   bOn: [in]: If true, the display backlights are turned on. If false, the display
   backlights are turned off.

Return Value:
   None

Comments:  
   This function affects all of the backlights associated with the device display.

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also:
   None

===============================================================================

Function: IDisplay_SetColor()

Description: 
   This function sets and retrieves the color of the specified color item.
   The color of an item can either be set to a specific RGB value or it can
   be set to the color of another item in the AEEClrItem enumeration.

   For example, to set the color of the item CLR_USER_TEXT to be the same
   as the color of the item CLR_SYS_TITLE_TEXT, use:
===pre>
   IDisplay_SetColor(po,CLR_USER_TEXT,CLR_SYS_TITLE_TEXT)
===/pre>

   Similarly, to set it to white, use:
===pre>
   IDIS===/pre>PLAY_SetColor(po,CLR_USER_TEXT,RGB_WHITE)
===/pre>

   To set it to any specific RGB value, use:
===pre>
   IDisplay_SetColor(po,CLR_USER_TEXT,MAKE_RGB(0x40,0x30,0x50))
===/pre>

Prototype:
   RGBVAL   IDisplay_SetColor(IDisplay * po, AEEClrItem item, RGBVAL rgb)

Parameters:
   po :[in]: Pointer to the IDisplay interface object to be used for changing the color
   of an user item.
   item: [in]: Specifies the color item whose color needs to be changed or retrieved.
   rgb: [in]: Specifies the new color to be associated with the item mentioned
   in "item" parameter. This can be either be an index of an item in the AEEClrItem
   enumeration or it can be a RGB value.  A value of RGB_NONE may be used in order
   to retrieve the color without changing it.

Return Value:
   If successful, returns the previous color associated with "item".

   If failed, returns RGB_NONE.

Comments:  
   Prior to BREW 2.1, only user color items were valid for the item parameter.
   Starting with BREW 2.1, all color items are valid.

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also:
   None

===============================================================================

Function: IDisplay_GetSymbol()

Description: 
   This function returns the AECHAR value corresponding to the specified symbol
   value.

Prototype:
   AECHAR   IDisplay_GetSymbol(IDisplay * po, AEESymbol sym, AEEFont fnt);

Parameters:
   po :[in]: Pointer to the IDisplay interface object to be used for changing the color
   of an user item.
   sym: [in]: Requested symbol
   fnt: [in]: Requested font

Return Value:
   The AECHAR associated with the specified symbol.

Comments:  
   If the symbol does not exist on the device a space character will be returned
   (AECHAR)' '.

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also:
   None

===============================================================================

Function: IDisplay_DrawFrame()

Description: 
   This function draws complex frames based upon the color resolution of the 
   system.  It allows single and 3D frames to be drawn.

   Passing a valid clrFill to the routine will fill the inside of the specified
   rectangle with the specified color.  

   The specified rectangle is adjusted by the size of the resulting operation.  This
   allows the routine to be called and have it automatically adjust the rectangle
   so that subsequent operations (such as text drawing) are offset by the proper
   amount.

Prototype:
   int IDisplay_DrawFrame(IDisplay * po, AEERect * prc, AEEFrameType ft, RGBVAL clrFill)

Parameters:
   po :[in]: Pointer to the IDisplay interface object
   prc :[in/out]: Pointer to the source rectangle. If this is NULL, this function just computes
	the adjustment value on each side based on the frame type and returns that value.
   ft :[in]: Frame type
   clrFill:[in]: Fill type for the inside of the frame

Return Value:
   The input rectangle (prc) is adjusted by this call.  The return value is the adjustment
	value on each side.

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also:
   IDisplay_DrawRect()
   IDisplay_EraseRect()
   IDISPLAY_EraseRgn()
   IDisplay_FillRect()
   IDisplay_FrameButton()
   IDisplay_FrameRect()
   IDisplay_FrameSolidRect()
   IDisplay_InvertRect()

===============================================================================

Function: IDisplay_FrameButton()

Description: 
   This function draws a 3D framed button based upon the color resolution of
   the system. The function will draw a lowered button when bPressed 
   is TRUE, and a raised button when the bPressed is FALSE.

   This function draws complex frames based upon the color resolution of the 
   system.  It allows single and 3D frames to be drawn.

   Passing a valid clrFill to the routine will fill the inside of the specified
   rectangle with the specified color.  

   The specified rectangle is adjusted by the size of the resulting operation.  This
   allows the routine to be called and have it automatically adjust the rectangle
   so that subsequent operations (such as text drawing) are offset by the proper
   amount.

Prototype:
   void IDisplay_FrameButton(IDisplay * po, AEERect * prc, boolean bPressed, RGBVAL clrFill)

Parameters:
   po :[in]: Pointer to the IDisplay interface object
   prc :[in/out]: Pointer to the source rectangle
   bPressed :[in]: Button pressed/raised indicator
   clrFill:[in]: Fill type for the inside of the frame

Return Value:
   The input rectangle (prc) is adjusted by this call.

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also:
   IDisplay_DrawFrame()
   IDisplay_DrawRect()
   IDisplay_EraseRect()
   IDISPLAY_EraseRgn()
   IDisplay_FillRect()
   IDisplay_FrameRect()
   IDisplay_FrameSolidRect()
   IDisplay_InvertRect()

===============================================================================

Function: IDisplay_FillRect()

Description: 
   This function draws a filled rectangle with a specified color.

Prototype:
   void IDisplay_FillRect(IDisplay *po,AEERect *pRect,RGBVAL clrFill) 

Parameters:
   po [in]: Pointer to IDISPLAY interface object to be used to fill the rectangle.
   pRect: [in]: A valid pointer to a rectangle that needs to be filled with the 
   specified color.
   clrFill: [in]: Specifies the color to be used to fill the rectangle.

Return Value:
   None

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also:
   IDisplay_DrawFrame()
   IDisplay_DrawRect()
   IDisplay_EraseRect()
   IDISPLAY_EraseRgn()
   IDisplay_FrameButton()
   IDisplay_FrameRect()
   IDisplay_FrameSolidRect()
   IDisplay_InvertRect()

===============================================================================

Function: IDisplay_ClearScreen()

Description: 
   This function clears the the current clipping rectangle, or the 
   entire destination bitmap if no clipping rectangle set.

Prototype:
   void IDisplay_ClearScreen(IDisplay *po);

Parameters:
   po [in]: Pointer to the IDisplay interface object.

Return Value:
   None

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also:
   None

===============================================================================

Function: IDisplay_FrameRect()

Description: 
   This function draws the borders of a rectangle. The color used for
   drawing the borders is the current color assigned to the item
   CLR_USER_FRAME.

Prototype:
   void IDisplay_FrameRect(IDisplay* po, AEERect *pRect)

Parameters:
   po [in]: Pointer to IDISPLAY interface object to be used to draw the rectangle.
   pRect: [in]: A valid pointer to a rectangle whose borders need to be drawn.


Return Value:
   None

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also:
   IDisplay_DrawFrame()
   IDisplay_DrawRect()
   IDisplay_EraseRect()
   IDISPLAY_EraseRgn()
   IDisplay_FillRect()
   IDisplay_FrameButton()
   IDisplay_FrameSolidRect()
   IDisplay_InvertRect()

===============================================================================

Function: IDisplay_FrameSolidRect()

Description: 
   This function draws the borders of a rectangle and fills it with a color. 
   The color used for drawing the borders is the current color assigned to the 
   item CLR_USER_FRAME. The color used for filling the rectangle is the current 
   color assigned to the item CLR_USER_BACKGROUND

Prototype:
   void IDisplay_FrameSolidRect(IDisplay* po, AEERect* pRect)

Parameters:
   po [in]: Pointer to IDisplay interface object to be used to draw the rectangle.
   pRect: [in]: A valid pointer to a rectangle that needs to be drawn and filled.

Return Value:
   None

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also:
   IDisplay_DrawFrame()
   IDisplay_DrawRect()
   IDisplay_EraseRect()
   IDISPLAY_EraseRgn()
   IDisplay_FillRect()
   IDisplay_FrameButton()
   IDisplay_FrameRect()
   IDisplay_InvertRect()

===============================================================================

Function: IDisplay_InvertRect()

Description: 
   This function inverts the colors in the given rectangle.

Prototype:
   void IDisplay_InvertRect(IDisplay* po, AEERect *pRect)

Parameters:
   po [in]: Pointer to IDisplay interface object to be used to invert the rectangle.
   pRect: [in]: A valid pointer to a rectangle to be inverted.

Return Value:
   None

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also:
   IDisplay_DrawFrame()
   IDisplay_DrawRect()
   IDisplay_EraseRect()
   IDISPLAY_EraseRgn()
   IDisplay_FillRect()
   IDisplay_FrameButton()
   IDisplay_FrameRect()
   IDisplay_FrameSolidRect()

===============================================================================

Function: IDisplay_EraseRect()

Description: 
   This function fills the given rectangle with the default background color
   (that is, the color associated with the item CLR_USER_BACKGROUND).

Prototype:
   void IDisplay_EraseRect(IDisplay* po, AEERect* pRect)

Parameters:
   po [in]: Pointer to IDisplay interface object to be used to erase the rectangle.
   pRect: [in]: A valid pointer to a rectangle to erase.

Return Value:
   None

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also:
   IDisplay_DrawFrame()
   IDisplay_DrawRect()
   IDISPLAY_EraseRgn()
   IDisplay_FillRect()
   IDisplay_FrameButton()
   IDisplay_FrameRect()
   IDisplay_FrameSolidRect()
   IDisplay_InvertRect()

===============================================================================

Function: IDISPLAY_EraseRgn()

Description: 
   This function fills the region enclosed by the given coordinates 
   with the default background color  (that is, the color associated for the item 
   CLR_USER_BACKGROUND).

Prototype:
   void IDISPLAY_EraseRgn(IDisplay* po, int16 x, int16 y, uint16 cx, uint16 cy)

Parameters:
   po [in]: Pointer to IDisplay interface object to be used to fill the region.
   x  [in]: X coordinate of the top,left corner of the region.
   y  [in]: Y coordinate of the top, left corner of the region.
   cx [in]: Width of the region.
   cy [in]: Height of the region.

Return Value:
   None

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also:
   IDisplay_DrawFrame()
   IDisplay_DrawRect()
   IDisplay_EraseRect()
   IDisplay_FillRect()
   IDisplay_FrameButton()
   IDisplay_FrameRect()
   IDisplay_FrameSolidRect()
   IDisplay_InvertRect()

===============================================================================

Function: IDISPLAY_DrawHLine()

Description: 
   This function draws a horizontal line of the given length, starting from the 
   given point. The line shall be Black in color.

Prototype:
   void IDISPLAY_DrawHLine(IDisplay* po,int16 x,int16 y,int16 len)

Parameters:
   po [in]: Pointer to IDisplay interface object to be used to draw the horizontal line.
   x  [in]: X coordinate of the starting point of the line.
   y  [in]: Y coordinate of the starting point of the line.
   len [in]: Length of the line.

Return Value:
   None

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also:
   IDISPLAY_DrawVLine()

===============================================================================

Function: IDISPLAY_DrawVLine()

Description: 
   This function draws a vertical line of the given length, starting from the 
   given point. The line shall be Black in color.

Prototype:
   void IDISPLAY_DrawVLine(IDisplay* po,int16 x,int16 y,int16 len)

Parameters:
   po [in]: Pointer to IDisplay interface object to be used to draw the vertical line.
   x  [in]: X coordinate of the starting point of the line.
   y  [in]: Y coordinate of the starting point of the line.
   len[in]: Length of the line.

Return Value:
   None

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.0

See Also:
   IDISPLAY_DrawHLine()

===============================================================================

Function: IDisplay_CreateDIBitmap()

Description: 

   This function creates a new device-independent bitmap (DIB).  The IDIB
   interface is an IBitmap interface with public data members.  An IDIB
   pointer may be cast to an IBitmap pointer and used anywhere an IBitmap
   pointer may be used.  For type safe casting, use the IDIB_TO_IBITMAP
   inline function.

   The specified width, height, and bit depth are used to allocate memory to
   hold the pixel array with 32-bit alignment of rows.  The IDIB members
   cx, cy, nPitch, and pBmp are initialized accordingly, assuming a top-down
   (positive-pitch) layout.  The bitmap's palette size, color scheme, and
   transparency color are all initialized to zero.

   The caller can manipulate IDIB data members directly to perform
   further initialization work. The pRGB and pBmp members of the IDIB can
   be set to point into memory outside of that allocated for the new
   bitmap; in that case, the caller must take care to ensure that the
   referenced memory remains valid as long at the bitmap exists with
   pointers to it.  When creating a DIB to refer to external bitmap data,
   the DIB can be created with a height of zero to avoid allocating memory
   internal to the bitmap.  It is recommended that the width not be set to
   zero so BREW can initialize nPitch.

   DIBs created in this manner exist primarily to blit data to or from
   device-dependent bitmaps.  They do not implement most drawing operations or
   color conversion functions, so the usual IDisplay or IGraphics functions
   for drawing text or graphics into DIBs will not work.  Specifically, the
   following IBitmap member functions always return EUNSUPPORTED:
~
   IBITMAP_RGBToNative()~
   IBITMAP_NativeToRGB()~
   IBITMAP_DrawPixel()~
   IBITMAP_GetPixel()~
   IBITMAP_SetPixels()~
   IBITMAP_DrawHScanline()~
   IBITMAP_FillRect()~
   IBITMAP_BltOut()~
*
  Furthermore, BltIn() simply defers to
   the destination bitmap's BltOut.

   In future versions, DIBs may suport these drawing operations.

Prototype:
   int IDisplay_CreateDIBitmap(IDisplay *po, IDIB **ppIDIB, uint8 colorDepth, uint16 w, uint16 h)

Parameters:
   po [in]: Pointer to IDisplay interface object.
   ppIDIB [out]: Pointer to interface for new DIB.
   colorDepth [in]: Number of bits used per pixel
   w  [in]: Width in pixels of the new bitmap.
   h  [in]: Height in pixels of the new bitmap.

Return Value:
   SUCCESS on success

   ENOMEMORY if there was not enough memory to allocate
   the bitmap buffer

   EUNSUPPORTED if the bitmap type if not supported

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 1.2

See Also:
   IDisplay_CreateDIBitmapEx()
   IDIB
   IDIB_TO_IBITMAP()

===============================================================================

Function: IDisplay_CreateDIBitmapEx()

Description: 
   This function works just like IDisplay_CreateDIBitmap(), except that it
   also allocates space for a palette and for extra data.  nPaletteSize may be
   0, in which case no memory is allocated for the palette, but (*ppIDIB)->pRGB
   will not be NULL so it can be used to find the extra data.  If cbExtra is not
   0, this extra memory will located at (*ppIDIB)->pRGB + (*ppIDIB)->cntRGB.

Prototype:
   int IDisplay_CreateDIBitmapEx(IDisplay *po, IDIB **ppIDIB, int nDepth, int nHeight, int nWidth, int nPaletteSize, int cbExtra)

Parameters:
   po           [in]: Pointer to IDisplay interface object.
   ppIDIB      [out]: Pointer to interface for new DIB.
   nDepth       [in]: Number of bits used per pixel.
   nHeight      [in]: Height in pixels of the new bitmap.
   nWidth       [in]: Width in pixels of the new bitmap.
   nPaletteSize [in]: Number of palette entries.
   cbExtra      [in]: Number of extra bytes to allocate.

Return Value:
   SUCCESS on success

   ENOMEMORY if there was not enough memory to allocate the bitmap buffer.

   EUNSUPPORTED if the bitmap type if not supported.

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 3.1

See Also:
   IDisplay_CreateDIBitmap(), 
   IDIB, 
   IDIB_TO_IBITMAP

===============================================================================

Function: IDisplay_SetDestination()

Description: 
   This function sets the destination bitmap to which IDisplay functions
   will render.

Prototype:
   int IDisplay_SetDestination(IDisplay *po, IBitmap *pDst)

Parameters:
   po   [in]: Pointer to IDISPLAY interface object.
   pDst [in]: IBitmap interface pointer that will be used for rendering,
              or NULL to set to device (screen) bitmap.

Return Value:
   SUCCESS or
   
   EFAILED.

Comments:  
   When IDisplay is instantiated, the default bitmap is the device (screen) bitmap.
   Use this function will NULL for pDst to restore to default.
   IBITMAP_AddRef will be called on the new destination and IBITMAP_Release
   will be called on the old destination.

Side Effects: 
   None

Version:
   Introduced BREW Client 1.2

See Also:
   IDisplay_GetDestination()

===============================================================================

Function:   IDisplay_GetDestination()

Description:
   This function gets the destination bitmap to which IDisplay functions
   will render.  The reference count of the bitmap will be incremented.

Prototype:
   IBitmap* IDisplay_GetDestination(IDisplay *po);

Parameters:
   po [in]: Pointer to IDisplay interface object.

Return Value:
   Pointer to the current destination bitmap.

Comments:
   If this function ever returns NULL, it is an error.

Side Effects:
   None

Version:
   Introduced BREW Client 1.2

See Also:
   IDisplay_SetDestination(), 
   IDisplay_GetDeviceBitmap()

===============================================================================

Function: IDisplay_GetDeviceBitmap()

Description: 

   This function retrieves the device (screen) bitmap.  This is the bitmap
   whose contents will be copied to the screen when IDisplay_Update() is called.

Prototype:
   int IDisplay_GetDeviceBitmap(IDisplay *po, IBitmap **ppIBitmap);

Parameters:
   po [in]: Pointer to IDisplay interface object.
   ppIBitmap [out]: Pointer to interface for device bitmap.

Return Value:
   SUCCESS on success, 

   Otherwise error code.

Comments:  
   This function will internally add a reference to the IBitmap pointer returned
   as the second parameter, if one is returned. This pointer must be released
   by the app developer using IBITMAP_Release() to free it.

   Each app has its own instance of the device bitmap, though the pixel buffer
   is shared between apps.

   Aside from the pixel data, care should be taken not to modify any data members
   (as might be possible using IDIB).

   The exception to this rule is the device bitmap's transparency color.  To perform
   transparent blits *from* a device bitmap, the application should set the
   transparency color immediately prior to performing the blit.  No
   assumptions should be made of the transparency color before setting
   it, or after external functions external to the application have been
   called (other than blitting functions).
   
   Note that compatible bitmaps inherit the transparency color of the bitmap
   from which they are created.  When the device bitmap's transparency color is
   unknown, the initial transparency color of any offscreen bitmap created
   from it will be unknown.

Side Effects: 
   None

Version:
   Introduced BREW Client 1.2

See Also:
   IDisplay_GetDestination(), 
   IDisplay_Update(), 
   IDisplay_UpdateEx()

===============================================================================

Function: IDisplay_SetFont()

Description: 
   This function sets the IFont to be used to draw the specified AEEFont.  The 
   current IFont in use is returned.

Prototype:
   IFont  *IDisplay_SetFont(IDisplay * po, AEEFont nFont, IFont *piNewFont)

Parameters:
   po :[in]: Pointer to the IDisplay interface object 
   nFont: the font designator
   piNewFont: an IFont pointer

Return Value:
   If successful, returns the old IFont associated with nFont.

   If failed, returns NULL

Comments:  
   Applications should take care to either Release the returned IFont, or, 
   more correctly, to save off the old IFont for restoration when the app
   exits.

   The passed-in IFont piNewFont is not AddRef'd, nor is the returned IFont, 
   as application and IDisplay are merely swapping pointers with already
   positive refcounts.  For this reason, applications mustn't call Release() 
   on piNewFont after passing it to SetFont().

Side Effects: 
   None

Version:
   Introduced BREW Client 1.2
 
See Also:
   IDisplay_DrawText(), 
   IDisplay_MeasureText(), 
   IDisplay_MeasureTextEx(),
   IDisplay_GetFontMetrics()

===============================================================================

Function: IDisplay_SetClipRect()

Description: 
   This function sets the clipping rectangle for the IDisplay instance. The
   clipping rectangle is the valid draw region of the currently selected 
   display bitmap. All drawing performed outside of the clipping rectangle
   will not be displayed.
      
Prototype:
   void IDisplay_SetClipRect(IDisplay * po, const AEERect *prc)

Parameters:
   po :[in]: Pointer to the IDisplay interface object 
   prc: [in]: the clipping rectangle to set

Return Value:
   None
   
Comments:
   Sets the IDisplay clipping rectangle to the intersection of rectangle
   pointed to by 'prc' and the destination bitmap. If 'prc' does not
   intersect the destination bitmap, the resultant clipping rectangle
   is a Null rectangle (0,0,0,0) and subsequent IDisplay operations will
   not affect the destination bitmap. If prc->dx or prc->dy are zero or
   negative, the result is also a Null rectangle.
   
   To reset the clipping rectangle to the default (destination bitmap
   bounds), pass the value NULL (0) for the 'prc' argument. 

   This function sets the clipping rectangle for the named IDisplay instance 
   only.

Side Effects: 
   Affects all further drawing functions through the IDisplay instance 
   by clipping the output to the passed rectangle.

Version:
   Introduced BREW Client 2.0
 
See Also:
   IDisplay_GetClipRect()

===============================================================================

Function: IDisplay_GetClipRect()

Description: 
   This function returns the current clipping rectangle set by 
   IDisplay_SetClipRect(), or the default clipping rect (desination bitmap
   bounds). The clipping rectangle is the valid draw region 
   of the currently selected display bitmap. All drawing performed outside 
   of the clipping rectangle will not be displayed.
      
Prototype:
   void IDisplay_GetClipRect(IDisplay * po, AEERect *prc)

Parameters:
   po :[in]: Pointer to the IDisplay interface object 
   prc: [out]: pointer to AEERect to receive the current clip rect.

Return Value:
   None
   
Comments:  
   This function returns the clipping rectangle for the named IDisplay 
   instance only.

Side Effects: 
   None

Version:
   Introduced BREW Client 2.0
 
See Also:
   IDisplay_SetClipRect()

===============================================================================

Function: IDisplay_Clone()

Description: 
   This function creates a new IDisplay instance and copies the current 
   settings from the primary (source) IDisplay instance to the new one.
      
Prototype:
   int IDisplay_Clone(IDisplay * po, IDisplay ** ppIDisplayNew);

Parameters:
   po :[in]: Pointer to the primary (source) IDisplay interface object 
   ppIDisplayNew:[out]: Pointer to the interface pointer which will be
                        filled with the address of the new instance.

Return Value:
   SUCCESS     IDisplay instance was cloned sucessfully.

   ENOMEMORY   Operation failed due to out of memory condition.
   
Comments:  
   This function has no affect the primary IDisplay object.

Side Effects: 
   None

Version:
   Introduced BREW Client 2.0
 
See Also:
   None

===============================================================================

Function: IDisplay_MakeDefault()

Description:
   This function makes this IDisplay the default.  (The default IDisplay is
   the one that is returned for AEECLSID_DISPLAY.)  This causes BREW to release
   its reference on the old default IDisplay and AddRef this IDisplay.

Prototype:
   void IDisplay_MakeDefault(IDisplay *po);

Parameters:
   po :[in]: Pointer to IDisplay interface object 

Return Value:
   None.

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 2.1
 
See Also:
   None

===============================================================================

Function: IDisplay_IsEnabled()

Description:
   This function is used to query the enabled state of the display
   corresponding to the destination bitmap.

Prototype:
   boolean IDisplay_IsEnabled(IDisplay *po);

Parameters:
   po       [in]: Pointer to IDisplay interface.

Return Value:
   TRUE if this display is enabled or the destination bitmap is not a device bitmap.

   FALSE otherwise.

Comments:
   While the display is disabled, no drawing operations are allowed.  BREW's
   drawing functions will have no effect.  If an application has direct access
   to the device bitmap's pixel buffer through IDIB, it is important that the app
   refrain from modifying the pixel buffer while the display is disabled.

Side Effects: 
   None

Version:
   Introduced BREW Client 2.1

See Also:
   IDisplay_NotifyEnable()

===============================================================================

Function: IDisplay_NotifyEnable()

Description:
   This function registers for a notification of a change to the display's
   enabled state.  The callback will be triggered synchronously when the state
   of the display changes either from enabled to disabled or disabled to enabled.

Prototype:
   int IDisplay_NotifyEnable(IDisplay *po, AEECallback *pcb);

Parameters:
   po       [in]: Pointer to IDisplay interface.
   pcb      [in]: Pointer to callback structure.

Return Value:
   SUCCESS if function executed correctly.

   EBADPARM if pcb is NULL.

   Other error codes may be returned depending on the implementation.

Comments:
   It is desirable to reregister for this notification in the callback function,
   so as not to miss any state changes.

   This callback will also be triggered when the device bitmap is destroyed (the
   last reference is released).

   Displays become enabled and disabled differently for the primary display and
   all other displays.  For the primary display, this happens synchronously with
   a suspend or resume event, so the notification will happen immediately before
   EVT_APP_RESUME and immediately after EVT_APP_SUSPEND.  For all other displays,
   the state change happens asynchronously.  For example, when a suspended app
   that is using the primary and secondary displays becomes top visable, the
   following steps occur in order:

   1) Primary display becomes enabled.
   2) Enable notification sent for primary display.
   3) EVT_APP_RESUME sent.
   4) Secondary display becomes enabled.
   5) Enable notification sent for secondary display.

   Steps 1, 2, and 3 happen together, and steps 4 and 5 happen together.

Side Effects: 
   None

Version:
   Introduced BREW Client 2.1

See Also:
   IDisplay_IsEnabled()

===============================================================================

Function: IDisplay_SetPrefs()

Description:
   This functions changes the display settings for the display associated with
   this IDisplay's destination bitmap.  If the destination bitmap is not a
   device bitmap, this function does nothing.

   pchSettings is a comma-separated list of name-value pairs, in the form
   <name>:<value>.  There should be no white space in the string.  Currently,
   the following names are defined:
===pre>
      name              meaning
      ____              _______
      w                 screen width
      h                 screen height
      c                 color scheme
      a                 annunicators (1 for yes, 0 for no)
      r                 screen rotation (degrees clockwise: 0, 90, 180, or 270)
===/pre>

   The values all of the currently defined names may be any one of the
   following:
===pre>
      value             meaning
      _____             _______
      <decimal integer> use the value specified
      <empty string>    use the default value
      *                 use the maximum meaningful value for this parameter
===/pre>

   A name that is not present is equivalent to a name with no value.  For
   example the following two display settings strings are equivalent:
===pre>
      w:100,h:,a:1
      w:100,a:1
===/pre>

   For c, * means that the highest possible color depth should be used.
   Specifying a color scheme signifies a request for a DIB-compatible display
   with that DIB color scheme.

   The display may not be able to accommodate all values of all parameters, so
   it is important that the app not assume that it received what it asked for.

Prototype:
   int IDisplay_SetPrefs(IDisplay *po, const char *pchSettings, int cSettings)

Parameters:
   po          [in]: Pointer to IDisplay interface object.
   pchSettings [in]: Display settings string, need not be null-terminated
   cSettings   [in]: Number of characters in pcDispSettings

Return Value:
   This function will return SUCCESS upon successful execution.  Otherwise,
   error codes include:
   EUNSUPPORTED if the destination is not a device bitmap or the display does not
   support display settings.
   EPRIVLEVEL if the application does not possess AEEPRIVID_DISPSETTINGS.

Comments:  
   Requires the AEEPRIVID_DISPSETTINGS privilege.  This requirement was added
   in BREW 3.1.4.

   When using the "r" parameter in combination with "h" and/or "w", an
   unexpected screen size may result.  This is due to the maximum width
   and height of the screen being calculated based on the initial screen
   orientation.  To avoid this, specify the "r" parameter by itself, and then
   set "w" and "h" in a separate step.

Side Effects: 
   None

Version:
   Introduced BREW Client 3.1

See Also:
   None

===============================================================================
*/

#endif    // AEEIDISPLAY_H

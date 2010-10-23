/*
  ========================================================================

  FILE:  AEEWProperties.h
  
  SERVICES:  

  GENERAL DESCRIPTION: Widget style property constants

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __AEEWPROPERTIES_H__
#define __AEEWPROPERTIES_H__

#include "AEEWidget.h"
#include "AEEResFile.h"
#include "AEEXYContainer.h"
#include "AEEImage.h"

/////////////////////////////////////////////////////////////////////////////
// ScrollText
typedef struct {
   uint16	   nScrollAmount; // step size (pixels)
   uint16      nScrollDelay;  // scroll speed (msec)
   uint16      nStartDelay;	// start delay (msec)
   uint16      nEndDelay;		// end delay (msec)
   uint16      nRepeat;       // number of times to repeat (zero means infinite)
} ScrollText;

/////////////////////////////////////////////////////////////////////////////
// Widget structs for named variable argument lists and resfile related functions
#define INHERIT_WProps\
   IWidget *piWidget;\
   va_list *args

typedef struct {
   INHERIT_WProps;
} WPropDesc;

typedef struct {
   INHERIT_WProps;   
   IResFile *piResFile;
} WResPropDesc;

/////////////////////////////////////////////////////////////////////////////
// Color macros with alpha-channel components

typedef uint32 RGBAVAL;    // similar to RGBVAL, except the alpha channel is significant

#ifndef MAKE_RGBA
#define MAKE_RGBA(r,g,b,a)    (RGBVAL)(((uint32)(r) << 8) + ((uint32)(g) << 16) + ((uint32)(b) << 24) + ((uint32)(a)))
#endif

#define RGBA_NONE              (0L)
#define RGBA_WHITE             MAKE_RGBA(255,255,255,255)
#define RGBA_BLACK             MAKE_RGBA(0,0,0,255)

// extract the alpha channel component from an RGBAVAL
#define RGBA_GETALPHA(rgba)    (uint8)((rgba) & 0xFF)

/////////////////////////////////////////////////////////////////////////////
// Redifining AEE RGB macros which have no opacity by default

#ifdef MAKE_RGB
#undef MAKE_RGB
#endif

#ifdef RGB_WHITE
#undef RGB_WHITE
#endif

#ifdef RGB_BLACK
#undef RGB_BLACK
#endif

#ifdef RGB_NONE
#undef RGB_NONE
#endif 

#define MAKE_RGB(r,g,b)       MAKE_RGBA(r,g,b,255)
#define RGB_WHITE RGBA_WHITE
#define RGB_BLACK RGBA_BLACK
#define RGB_NONE  RGBA_NONE

/////////////////////////////////////////////////////////////////////////////
/* 
   PROP_BORDERWIDTH and PROP_BORDERCOLOR are shortcuts for 
   setting width and color for both active and inactive borders 
   to the same value. If used For Get operations, the returned 
   value will be the Active width or color. 

   For example:
   
   IWIDGET_SetProperty with PROP_BORDERWIDTH sets both
   PROP_ACTIVE_BORDERWIDTH and PROP_INACTIVE_BORDERWIDTH to
   the same value.
      
   IWIDGET_GetProperty with PROP_BORDERWIDTH gets the
   PROP_ACTIVE_BORDERWIDTH property.

   Note: Inactive border widths are constrained by the widgets
   to be no larger than the active border width.
   
*/
#define PROP_BORDERWIDTH               0x100 // Set (only) both active and inactive widths to same value
#define PROP_ACTIVE_BORDERWIDTH        0x101 // Get/Set border width when widget is active
#define PROP_INACTIVE_BORDERWIDTH      0x102 // Get/Set border width when widget is inactive
#define PROP_BORDERRADIUS              0x103 // Get/Set the radius of rounded border corners

#define PROP_BORDERCOLOR               0x110 // Set (only) all border colors to same value
#define PROP_ACTIVE_BORDERCOLOR        0x111 // Get/Set border color when widget is active
#define PROP_INACTIVE_BORDERCOLOR      0x112 // Get/Set border color when widget is inactive
#define PROP_SACTIVE_BORDERCOLOR       0x113 // Get/Set border color when widget is selected/active
#define PROP_SINACTIVE_BORDERCOLOR     0x114 // Get/Set border color when widget is selected/inactive
#define PROP_SELECTED_BORDERCOLOR      0x115 // Set (only) border color for selected (active&inactive)

#define PROP_BORDERSTYLE               0x119 // Get/Set border style (BORDERSTYLE_xxx)

#define PROP_PADDING                   0x120 // Set (only) all pads to same value
#define PROP_LEFT_PAD                  0x121 // Get/Set left padding just inside the border
#define PROP_RIGHT_PAD                 0x122 // Get/Set right ...
#define PROP_TOP_PAD                   0x123 // Get/Set top ...
#define PROP_BOTTOM_PAD                0x124 // Get/Set bottom ...

// background colors
#define PROP_BGCOLOR                   0x130 // Set (only) all bgcolors to same value
#define PROP_ACTIVE_BGCOLOR            0x131 // Get/Set bgcolor when widget is active
#define PROP_INACTIVE_BGCOLOR          0x132 // Get/Set bgcolor when widget is inactive 
#define PROP_SACTIVE_BGCOLOR           0x133 // Get/Set bgcolor when widget is selected/active  
#define PROP_SINACTIVE_BGCOLOR         0x134 // Get/Set bgcolor when widget is selected/inactive   
#define PROP_SELECTED_BGCOLOR          0x135 // Set (only) bgcolor for selected (active&inactive)
#define PROP_TEXT_SACTIVE_BGCOLOR      0x136 // Get/Set bgcolor for active text selection (used in TextWidget)
#define PROP_TEXT_SINACTIVE_BGCOLOR    0x137 // Get/Set bgcolor for inactive text selection (used in TextWidget)
#define PROP_TEXT_SELECTED_BGCOLOR     0x138 // Get/Set bgcolor for active&inactive text selection (used in TextWidget)

// foreground colors
#define PROP_FGCOLOR                   0x140 // Set (only) all fgcolors to same value
#define PROP_ACTIVE_FGCOLOR            0x141 // Get/Set fgcolor when widget is active           
#define PROP_INACTIVE_FGCOLOR          0x142 // Get/Set fgcolor when widget is inactive         
#define PROP_SACTIVE_FGCOLOR           0x143 // Get/Set fgcolor when widget is selected/active  
#define PROP_SINACTIVE_FGCOLOR         0x144 // Get/Set fgcolor when widget is selected/inactive
#define PROP_SELECTED_FGCOLOR          0x145 // Set (only) fgcolor for selected (active&inactive)

#define PROP_TRANSP_COLOR              0x150 // Get/Set BitmapWidget: color representing transparent area for bitblts
#define PROP_SELECTED                  0x151 // Get/Set 'selected' property for list item widgets
#define PROP_FONT                      0x152 // Get/Set font for widget to use
#define PROP_FLAGS                     0x153 // Get/Set widget specific flags
#define PROP_OFFSETX                   0x154 // Get/Set viewport offset in x direction
#define PROP_OFFSETY                   0x155 // Get/Set viewport offset in y direction
#define PROP_INCREMENT                 0x156 // Get/Set viewport jump increment in pixels for all directions


#define PROP_TRANSPARENCY              0x157 // Get/Set transparency of entire widget
#define PROP_BGTRANSPARENCY            0x158 // Get/Set transparency of widget background
#define PROP_BORDERTRANSPARENCY        0x159 // Get/Set transparency of widget border
#define PROP_ROTATE                    0x160 // @@@ not implemented
#define PROP_VIEWMODEL                 0x161 // Get/Set the widget's view model
#define PROP_CARETPOS                  0x162 // Get the widget's caret position (relative to widget, loword is x, hiword is y)
#define PROP_INDEXER                   0x163 // Set the widget's indexer function (List,Grid,Pick widgets)
#define PROP_SELECTINDEX               0x164 // Set select (Cause an item at index to be selected)
#define PROP_FOCUSINDEX                0x165 // Get/Set list/pick/grid focused item index
#define PROP_TOPINDEX                  0x166 // Get/Set the first (top) visible item in the (List/Grid/PickWidget)
#define PROP_ITEMHEIGHT                0x167 // Get/Set item height for ListWidget, GridWidget
#define PROP_ITEMWIDTH                 0x168 // Get/Set item width for PickWidget, GridWidget
#define PROP_ROWS                      0x169 // Get number of rows (ListWidget/GridWidget) 
#define PROP_COLS                      0x170 // Get number of cols (PickWidget/GridWidget)
#define PROP_HINT_ROWS                 0x171 // Get/Set row "hint" used by GetPreferredExtent
#define PROP_HINT_COLS                 0x172 // Get/Set column "hint" used by GetPreferredExtent
#define PROP_HINT_WIDTH                0x173 // Get/Set width "hint" used by GetPreferredExtent
#define PROP_ITEMRECT                  0x174 // Get rectangle of item widget in a List/Pick/Grid
#define PROP_VARITEMSIZE               0x175 // Get default item size / Set(override) item size for variable-sized items
#define PROP_SELITEMSIZE               0x176 // Get/Set size (width or height) of selected item

#define PROP_SCROLLHANDLEWIDTH         0x180 // Get/Set width of scrollbar handle
#define PROP_SCROLLPAD                 0x181 // Get/Set space between scrollbar/indicator and child widget
#define PROP_SCROLLSTYLE               0x182 // Get/Set scroll style
#define PROP_SCROLLREQ                 0x183 // Set valueless property to fire a scroll event (used at init time)
#define PROP_ACTIVE_SCROLLCOLOR        0x184 // Get/Set active color of scrollbar/indicator
#define PROP_INACTIVE_SCROLLCOLOR      0x185 // Get/Set inactive color of scrollbar/indicator
#define PROP_SCROLLCOLOR               0x186 // Set (only) active & inactive color to same value
#define PROP_ACTIVE_SCROLLHANDLECOLOR  0x187 // Get/Set active color of scrollbar handle
#define PROP_INACTIVE_SCROLLHANDLECOLOR 0x188 // Get/Set inactive color of scrollbar handle
#define PROP_SCROLLHANDLECOLOR         0x189 // Set (only) active & inactive handle color to same value
#define PROP_SCROLLFILLCOLOR           0x190 // Get/Set fill (inside, non-handle) color of scroll bar

#define PROP_SCROLLBARRECT             0x191 // Get scrollbar rectangle including border and padding
#define PROP_VSCROLLBARRECT            0x192 // Get vertical scrollbar rectangle (when SCROLL_BOTH style)
#define PROP_HSCROLLBARRECT            0x193 // Get horizontal scrollbar rectangle (when SCROLL_BOTH style) 
                                             
#define PROP_SCROLLHANDLERECT          0x194 // Get rectangle of scroll handle
#define PROP_VSCROLLHANDLERECT         0x195 // Get scroll handle rectangle of vertical scrollbar (when SCROLL_BOTH style)
#define PROP_HSCROLLHANDLERECT         0x196 // Get scroll handle rectangle of horizontal scrollbar (when SCROLL_BOTH style) 
                                             
#define PROP_SCROLLFILLRECT            0x197 // Get scrollbar interior fill rectangle
#define PROP_VSCROLLFILLRECT           0x198 // Get vertical scrollbar interior fill rectangle (when SCROLL_BOTH style)
#define PROP_HSCROLLFILLRECT           0x199 // Get horizontal scrollbar interior fill rectangle (when SCROLL_BOTH style)

#define PROP_SOFTKEY1                  0x200 // Get/Set softkey label widget 
#define PROP_SOFTKEY2                  0x201 // Get/Set softkey label widget 

#define PROP_SHADOWTRANSPARENCY        0x202 // Get/Set shadow transparency value 0=opaque, 255=transparent
#define PROP_SHADOWOFFSET              0x203 // Set (only) shadow offset x and y to same value (negative value == shadow on top,left)
#define PROP_SHADOWOFFSETX             0x204 // Get/Set shadow offset in x direction (negative value == shadow on left) 
#define PROP_SHADOWOFFSETY             0x205 // Get/Set shadow offset in y direction (negative value == shadow on top)
#define PROP_SHADOWCOLOR               0x206 // Get/Set shadow color 

#define PROP_LAYOUTSTYLE               0x207 // Get/Set layout style for containers that support it

#define PROP_GRADIENT_STYLE            0x210 // Get/Set gradient fill style

#define PROP_CURSOR_SHOW               0x211 // Set: make cursor visible at a specific position
#define PROP_CURSOR_DISMISS            0x212 // Set: hide the cursor widget

#define PROP_IMAGESTATIC_IMAGEWIDGET   0x213 // Get/Set image widget in ImageStaticWidget
#define PROP_IMAGESTATIC_STATICWIDGET  0x214 // Get/Set static widget in ImageStaticWidget

#define PROP_IMAGE_PARM                0x215 // Set (only) IIMAGE_SetParm pass through for ImageWidget
#define PROP_ANIMATE                   0x216 // Get/Set Retrieve anim state or start/stop animations/marquee scrolling. dwParam(0=stop 1=start)
#define PROP_IMAGE_ANIMATE      PROP_ANIMATE // Set (only) Start/stop animation (ImageWidget) dwParam(0=stop 1=start)

#define PROP_SLIDER_MAXPOS             0x217 // Get/Set slider maximum position
#define PROP_SLIDER_HANDLEBORDERCOLOR  0x218 // Get/Set slider handle border (outline) color

#define PROP_LIMITTEXT                 0x219 // Get/Set max entry chars allowed
#define PROP_INPUTMODE                 0x220 // Get/Set current text input mode
#define PROP_STARTLINE                 0x221 // Get start of line (from position)
#define PROP_PREVLINE                  0x222 // Get start of previous line (from pos)
#define PROP_NEXTLINE                  0x223 // Get start of next line (from pos)
#define PROP_TEXTCONTROLLER            0x224 // Get/Set the widget's Controller
#define PROP_TEXTCONTROLLERMODEKEY     0x230 // Get/Set the text controller's input mode key

#define PROP_TITLEWIDGET               0x225 // Get the static widget from the TitleWidget
#define PROP_SCROLLBARENDPAD           0x226 // @@@
#define PROP_PASSWORDMASKCHAR          0x227 // Set the AECHAR to use for password masking
#define PROP_IMAGESTRIP                0x228 // Set the imagestrip used by the widget (for Check/Radio/Scrollbar)
#define PROP_IMAGESTRIP_H              PROP_IMAGESTRIP   // scroll widgets have both vertical and horizontal imagestrips
#define PROP_IMAGESTRIP_V              0x229 // Set the vertical imagestrip used by scroll widgets

#define PROP_GRADIENT                  0x238  // Set (only) background gradient end color (all)
#define PROP_SELECTED_GRADIENT         0x239 // Set (only) background gradient end color (selected, all)
#define PROP_ACTIVE_GRADIENT           0x240 // Get/Set background gradient end color (active)
#define PROP_INACTIVE_GRADIENT         0x241 // Get/Set background gradient end color (inactive) 
#define PROP_SACTIVE_GRADIENT          0x242 // Get/Set background gradient end color (selected,active)  
#define PROP_SINACTIVE_GRADIENT        0x243 // Get/Set background gradient end color (selected,inactive)   

#define PROP_SELECTED_SHADOWOFFSET     0x250 // Set (only) shadow offset (selected, all)
#define PROP_SELECTED_SHADOWCOLOR      0x251 // Set (only) shadow (selected, all)
#define PROP_SELECTED_SHADOWOFFSETX    0x252 // Set (only) shadow x offset (selected, all)
#define PROP_SELECTED_SHADOWOFFSETY    0x253 // Set (only) shadow x offset (selected, all)

#define PROP_CARETWIDGET               0x300 // Get the caret widget from the TextWidget
#define PROP_CARETPULSE                0x301 // Set: make caret pulse
#define PROP_PASSWORDMASKDELAY         0x302 // Get/Set: delay from typing char to conversion to mask char
#define PROP_LINEGAP                   0x303 // Get/Set spacing between text lines
#define PROP_SCROLLTEXT                0x304

#define PROP_TABSPACING_VERT           0x310 // Get/Set vertical tab spacing
#define PROP_TABSPACING_HORZ           0x311 // Get/Set horizontal tab spacing
#define PROP_ACTIVE_TABWIDTH           0x312 // Get/Set width of active tabs (default of 0 means size to fit content)
#define PROP_INACTIVE_TABWIDTH         0x313 // Get/Set width of inactive tabs (default of 0 means size to fit content) 
#define PROP_TABHEIGHT                 0x314 // Get/Set height of tabs

#define PROP_FONT_CLASS                0x315 // Set (only) the font based on classid
#define PROP_FONT_OUTLINEWIDTH         0x316 // Get/Set the width of the outline applied to the font
#define PROP_FONT_OUTLINECOLOR         0x317 // Get/Set the color of the font's outline

#define PROP_CLIENTRECT                0x320 // Get (only) the client rect (extent rect minus padding)

#define PROP_LOCALE                    0x330 // Set the ILocal used by the date/time widget
#define PROP_FORMATSTRING              0x331 // Set the format string for the date/time widget
#define PROP_DELIMITER                 0x332 // Set the delimiter string for the date/time widget
#define PROP_FIELDPAD                  0x333 // Set the padding around text in date/time fields
#define PROP_ARROWPAD                  0x334 // Set the padding between selection and up/down arrows

#define PROP_APPLYWPROPS               0x340 // Lookup (and apply) widget props

#define PROP_VISIBLE                   0x341 // Visibility of child widgets in 

#define PROP_TILED_BDT                 0x342 // Image Widget tiled border distance top
#define PROP_TILED_BDB                 0x343 // Image Widget tiled border distance bottom
#define PROP_TILED_BDL                 0x344 // Image Widget tiled border distance left
#define PROP_TILED_BDR                 0x345 // Image Widget tiled border distance right

#define PROP_IMAGE_FRAME               0x346 // frame of the image to draw

#define PROP_ANIMATE_FLAGS             0x347 // Flags specific to animation

#define PROP_DEFAULTTEXTALIGNMENT      0x348 // Default text alignment

// Background image props (apply to all widgets that support border props)
#define PROP_BGIMAGE                   0x350 // Get/Set background image (IImage*)
#define PROP_BGIMAGE_FLAGS             0x351 // Get/Set background image flags
#define PROP_BGIMAGE_PARM              0x352 // Set BG image ImageParm (direct to IImage)
#define PROP_BGIMAGE_FRAME             0x353 // Set BG image current frame
#define PROP_BGIMAGE_ANIMATE           0x354 // Set BG image animation state (on/off)
#define PROP_BGIMAGE_TILED_BORDER      0x355 // Set BG image tiled border distances (l,t,r,b)
#define PROP_BGIMAGE_ANIMATE_FLAGS     0x356 // Get/Set background image's animation flags

// BG image flags
#define BGIF_TILED                     1  // tiled image mode
#define BGIF_HASSELECTIMAGE            2  // image is composed of two tiles of equal size, the 
                                          // first displayed in the unselected state, and the 
                                          // other displayed in the  selected state. (Set by 
                                          // PROP_SELECTED)
#define BGIF_IMAGEUNDER                4  // when set, the BG image is drawn underneath the 
                                          // background fill color (by default, it's drawn on top)


// Grid Container properties
#define PROP_ROWPROPS                  0x400 // Get/Set Grid Container row properties.  See RowColProperties.
#define PROP_COLPROPS                  0x401 // Get/Set Grid Container column properties.  See RowColProperties.

#define PROP_RAISE                     0x410 // Set the raise/unraised status of this widget 
#define PROP_RAISECONTAINER            0x411 // Set the raisecontainer status of this widget

#define PROP_FONTBIDIUTILCLASSID       0x412 // Get the class id of the preferred IFontBidiUtil implementation.

#define PROP_TEXTRECTS                 0x413 // Get the display rects for a range of text characters.

#define PROP_IMAGE_NFRAMES             0x414 // Get (only) the number of frames of the displayed image

#define PROP_SPECIAL                   0x1000 // for widget-specific properties

#define PROP_OEM_SPECIAL               0x8000 // reserved for OEM use

// update values
#define UPDATE_DISABLE                 1
#define UPDATE_ENABLE                  0

// gradient styles
#define GRADIENT_STYLE_VERT            0     // gradient fill vertically (top to bottom)
#define GRADIENT_STYLE_HORZ            1     // gradient fill horizontally (left to right)
#define GRADIENT_STYLE_CENTERVERT      2     // gradient fill from top & bottom vertically to center
#define GRADIENT_STYLE_CENTERHORZ      3     // gradient fill from left & right to center 
#define GRADIENT_STYLE_NONE            4     // gradient fill none

// border styles
#define BORDERSTYLE_NORMAL          0
#define BORDERSTYLE_BEVELED         1
#define BORDERSTYLE_ROUNDED         2
#define BORDERSTYLE_MAXSTYLE        2

// scroll style, also used as scroll type value in ScrollEvent (except SCROLL_BOTH)
#define SCROLL_VERT     0     // vertical scroll bar/indicator
#define SCROLL_HORZ     1     // horizontal scroll bar/indicator
#define SCROLL_BOTH     2     // both vertical and horizontal scroll bar/indicators visible

#define LAYOUT_NONE     0     // no layout
#define LAYOUT_VERT     1     // vertical layout
#define LAYOUT_HORZ     2     // horizontal layout style
#define LAYOUT_BOTH     3     // layout both dimensions

// animation flags
#define AF_ENABLE_EVT_STEP    (1<<0)      // enables the step event to be sent to the view model

// indexer function for List/Grid/Pick/Tab Widget
typedef void (*PFNINDEXER)(void *pCtx, int nIndex, boolean bSelected);

// used for passing the indexer and user pointer with IWIDGET_SetProperty
typedef struct IndexerDesc {
   PFNINDEXER  pfnIndexer;
   void *      pCtx;
} IndexerDesc;

typedef struct RaiseDesc {
   IXYContainer * pixyTarget;    // xy container we are raising TO
   IWidget *      piw;           // the widget we are raising
} RaiseDesc;


typedef struct {
   int parm;   // Parameter of image to set. See IPARM_* in AEEImage.h
   int arg1;   // argument 1 for parameter set
   int arg2;   // argument 2 for parameter set
} ImageParm;


typedef struct {
   uint16   left;
   uint16   top;
   uint16   right;
   uint16   bottom;
} BGImageTiledBorder;


typedef struct {
   IWidget *piw;
   boolean bVisible;
} WidgetVis;

static __inline int IWIDGET_GetBorderWidth(IWidget *po, int *pnBorderWidth) {
   return IWIDGET_GetPropInt(po, PROP_BORDERWIDTH, pnBorderWidth);
}

static __inline int IWIDGET_SetBorderWidth(IWidget *po, int nBorderWidth) {
   return IWIDGET_SetProperty(po, PROP_BORDERWIDTH, nBorderWidth);
}

static __inline int IWIDGET_GetActiveBorderWidth(IWidget *po, int *pnBorderWidth) {
   return IWIDGET_GetPropInt(po, PROP_ACTIVE_BORDERWIDTH, pnBorderWidth);
}
static __inline int IWIDGET_SetActiveBorderWidth(IWidget *po, int nBorderWidth) {
   return IWIDGET_SetProperty(po, PROP_ACTIVE_BORDERWIDTH, nBorderWidth);
}                                         

static __inline int IWIDGET_GetInactiveBorderWidth(IWidget *po, int *pnBorderWidth) {
   return IWIDGET_GetPropInt(po, PROP_INACTIVE_BORDERWIDTH, pnBorderWidth);
}
static __inline int IWIDGET_SetInactiveBorderWidth(IWidget *po, int nBorderWidth) {
   return IWIDGET_SetProperty(po, PROP_INACTIVE_BORDERWIDTH, nBorderWidth);
}

static __inline int IWIDGET_GetBorderRadius(IWidget *po, int *pnBorderRadius) {
   return IWIDGET_GetPropInt(po, PROP_BORDERRADIUS, pnBorderRadius);
}

static __inline int IWIDGET_SetBorderRadius(IWidget *po, int nBorderRadius) {
   return IWIDGET_SetProperty(po, PROP_BORDERRADIUS, nBorderRadius);
}

static __inline int IWIDGET_GetBorderStyle(IWidget *po, int *pnStyle) {
   return IWIDGET_GetPropInt(po, PROP_BORDERSTYLE, pnStyle);
}
static __inline int IWIDGET_SetBorderStyle(IWidget *po, int nStyle) {
   return IWIDGET_SetProperty(po, PROP_BORDERSTYLE, nStyle);
}

static __inline int IWIDGET_SetPadding(IWidget *po, int nPadding) {
   return IWIDGET_SetProperty(po, PROP_PADDING, nPadding);
}

static __inline int IWIDGET_GetLeftPadding(IWidget *po, int *pnPadding) {
   return IWIDGET_GetPropInt(po, PROP_LEFT_PAD, pnPadding);
}
static __inline int IWIDGET_SetLeftPadding(IWidget *po, int nPadding) {
   return IWIDGET_SetProperty(po, PROP_LEFT_PAD, nPadding);
}

static __inline int IWIDGET_GetRightPadding(IWidget *po, int *pnPadding) {
   return IWIDGET_GetPropInt(po, PROP_RIGHT_PAD, pnPadding);
}
static __inline int IWIDGET_SetRightPadding(IWidget *po, int nPadding) {
   return IWIDGET_SetProperty(po, PROP_RIGHT_PAD, nPadding);
}

static __inline int IWIDGET_GetTopPadding(IWidget *po, int *pnPadding) {
   return IWIDGET_GetPropInt(po, PROP_TOP_PAD, pnPadding);
}
static __inline int IWIDGET_SetTopPadding(IWidget *po, int nPadding) {
   return IWIDGET_SetProperty(po, PROP_TOP_PAD, nPadding);
}

static __inline int IWIDGET_GetBottomPadding(IWidget *po, int *pnPadding) {
   return IWIDGET_GetPropInt(po, PROP_BOTTOM_PAD, pnPadding);
}
static __inline int IWIDGET_SetBottomPadding(IWidget *po, int nPadding) {
   return IWIDGET_SetProperty(po, PROP_BOTTOM_PAD, nPadding);
}

// font
static __inline int IWIDGET_GetFont(IWidget *po, IFont **ppiFont) {
   return IWIDGET_GetPropPtr(po, PROP_FONT, (void **)ppiFont);
}
static __inline int IWIDGET_SetFont(IWidget *po, IFont *piFont) {
   return IWIDGET_SetProperty(po, PROP_FONT, (uint32)piFont);
}
static __inline int IWIDGET_SetFontClass(IWidget *po, AEECLSID fontClass) {
   return IWIDGET_SetProperty(po, PROP_FONT_CLASS, (uint32)fontClass);
}

// font outline
static __inline int IWIDGET_GetFontOutlineWidth(IWidget *po, int *dwWidth) {
   return IWIDGET_GetPropPtr(po, PROP_FONT_OUTLINEWIDTH, (void **)dwWidth);
}
static __inline int IWIDGET_SetFontOutlineWidth(IWidget *po, int dwWidth) {
   return IWIDGET_SetProperty(po, PROP_FONT_OUTLINEWIDTH, (uint32)dwWidth);
}

static __inline int IWIDGET_GetFontOutlineColor(IWidget *po, RGBAVAL *rgb) {
   return IWIDGET_GetPropPtr(po, PROP_FONT_OUTLINECOLOR, (void **)rgb);
}
static __inline int IWIDGET_SetFontOutlineColor(IWidget *po, RGBAVAL rgb) {
   return IWIDGET_SetProperty(po, PROP_FONT_OUTLINECOLOR, (uint32)rgb);
}

///////////////////////////////////////////////////////////////////////////////
// GetTextRects
typedef struct {
   uint32   count;  // in: if pRects is not equal to NULL, then count 
                    //     contains the number of AEERect allocated in
                    //     pRects.
                    // out: output contains the number of rects needed
                    //      to locate the range of text,
   uint32   flags;  // in: for future expansion, currently must be zero.
   uint32   start;  // in: starting character index to range of text
   uint32   end;    // in: ending character index to range of text
   AEERect* pRects; // out: array of rects that locate the range of text.
                    //      using the text widget coordinates.
                    //      
} WidgetTextRects;


static __inline int IWIDGET_GetTextRects(IWidget *po, uint32 start, uint32 end, uint32* pCount, AEERect* pRects) {
   int result;
   WidgetTextRects wtr;

   if (NULL == pCount) {
      return EBADPARM;
   }

   wtr.flags  = 0;
   wtr.count  = *pCount;
   wtr.start  = start;
   wtr.end    = end;
   wtr.pRects = pRects;
   
   result = IWIDGET_GetProperty(po, PROP_TEXTRECTS, (uint32*) &wtr);
   if ( SUCCESS == result ) {
      *pCount = wtr.count;
   }

   return result;
}

// border colors (all)
static __inline int IWIDGET_SetBorderColor(IWidget *po, RGBAVAL rgb) {
   return IWIDGET_SetProperty(po, PROP_BORDERCOLOR, rgb);
}

// active border color
static __inline int IWIDGET_GetActiveBorderColor(IWidget *po, RGBAVAL *prgb) {
   return IWIDGET_GetPropRGB(po, PROP_ACTIVE_BORDERCOLOR, prgb);
}
static __inline int IWIDGET_SetActiveBorderColor(IWidget *po, RGBAVAL rgb) {
   return IWIDGET_SetProperty(po, PROP_ACTIVE_BORDERCOLOR, rgb);
}

// selected/active border color
static __inline int IWIDGET_GetSelectedActiveBorderColor(IWidget *po, RGBAVAL *prgb) {
   return IWIDGET_GetPropRGB(po, PROP_SACTIVE_BORDERCOLOR, prgb);
}
static __inline int IWIDGET_SetSelectedActiveBorderColor(IWidget *po, RGBAVAL rgb) {
   return IWIDGET_SetProperty(po, PROP_SACTIVE_BORDERCOLOR, rgb);
}

// inactive border color
static __inline int IWIDGET_GetInactiveBorderColor(IWidget *po, RGBAVAL *prgb) {
   return IWIDGET_GetPropRGB(po, PROP_INACTIVE_BORDERCOLOR, prgb);
}
static __inline int IWIDGET_SetInactiveBorderColor(IWidget *po, RGBAVAL rgb) {
   return IWIDGET_SetProperty(po, PROP_INACTIVE_BORDERCOLOR, rgb);
}

// selected/inactive border color
static __inline int IWIDGET_GetSelectedInactiveBorderColor(IWidget *po, RGBAVAL *prgb) {
   return IWIDGET_GetPropRGB(po, PROP_SINACTIVE_BORDERCOLOR, prgb);
}
static __inline int IWIDGET_SetSelectedInactiveBorderColor(IWidget *po, RGBAVAL rgb) {
   return IWIDGET_SetProperty(po, PROP_SINACTIVE_BORDERCOLOR, rgb);
}

// selected border color (both)
static __inline int IWIDGET_SetSelectedBorderColor(IWidget *po, RGBAVAL rgb) {
   return IWIDGET_SetProperty(po, PROP_SELECTED_BORDERCOLOR, rgb);
}

// foreground color (all)
static __inline int IWIDGET_SetFGColor(IWidget *po, RGBVAL rgb) {
   return IWIDGET_SetProperty(po, PROP_FGCOLOR, rgb);
}

// active foreground color
static __inline int IWIDGET_GetActiveFGColor(IWidget *po, RGBVAL *prgb) {
   return IWIDGET_GetPropRGB(po, PROP_ACTIVE_FGCOLOR, prgb);
}
static __inline int IWIDGET_SetActiveFGColor(IWidget *po, RGBVAL rgb) {
   return IWIDGET_SetProperty(po, PROP_ACTIVE_FGCOLOR, rgb);
}

// selected/active foreground color
static __inline int IWIDGET_GetSelectedActiveFGColor(IWidget *po, RGBVAL *prgb) {
   return IWIDGET_GetPropRGB(po, PROP_SACTIVE_FGCOLOR, prgb);
}
static __inline int IWIDGET_SetSelectedActiveFGColor(IWidget *po, RGBVAL rgb) {
   return IWIDGET_SetProperty(po, PROP_SACTIVE_FGCOLOR, rgb);
}

// inactive foreground color
static __inline int IWIDGET_GetInactiveFGColor(IWidget *po, RGBVAL *prgb) {
   return IWIDGET_GetPropRGB(po, PROP_INACTIVE_FGCOLOR, prgb);
}
static __inline int IWIDGET_SetInactiveFGColor(IWidget *po, RGBVAL rgb) {
   return IWIDGET_SetProperty(po, PROP_INACTIVE_FGCOLOR, rgb);
}

// selected/Inactive foreground color
static __inline int IWIDGET_GetSelectedInactiveFGColor(IWidget *po, RGBVAL *prgb) {
   return IWIDGET_GetPropRGB(po, PROP_SINACTIVE_FGCOLOR, prgb);
}
static __inline int IWIDGET_SetSelectedInactiveFGColor(IWidget *po, RGBVAL rgb) {
   return IWIDGET_SetProperty(po, PROP_SINACTIVE_FGCOLOR, rgb);
}

// selected/both foreground color
static __inline int IWIDGET_SetSelectedFGColor(IWidget *po, RGBVAL rgb) {
   return IWIDGET_SetProperty(po, PROP_SELECTED_FGCOLOR, rgb);
}

// background colors (all)
static __inline int IWIDGET_SetBGColor(IWidget *po, RGBAVAL rgb) {
   return IWIDGET_SetProperty(po, PROP_BGCOLOR, rgb);
}

// active background color
static __inline int IWIDGET_GetActiveBGColor(IWidget *po, RGBAVAL *prgb) {
   return IWIDGET_GetPropRGB(po, PROP_ACTIVE_BGCOLOR, prgb);
}
static __inline int IWIDGET_SetActiveBGColor(IWidget *po, RGBAVAL rgb) {
   return IWIDGET_SetProperty(po, PROP_ACTIVE_BGCOLOR, rgb);
}

// selected/active background color
static __inline int IWIDGET_GetSelectedActiveBGColor(IWidget *po, RGBAVAL *prgb) {
   return IWIDGET_GetPropRGB(po, PROP_SACTIVE_BGCOLOR, prgb);
}
static __inline int IWIDGET_SetSelectedActiveBGColor(IWidget *po, RGBAVAL rgb) {
   return IWIDGET_SetProperty(po, PROP_SACTIVE_BGCOLOR, rgb);
}

// inactive background color
static __inline int IWIDGET_GetInactiveBGColor(IWidget *po, RGBAVAL *prgb) {
   return IWIDGET_GetPropRGB(po, PROP_INACTIVE_BGCOLOR, prgb);
}
static __inline int IWIDGET_SetInactiveBGColor(IWidget *po, RGBAVAL rgb) {
   return IWIDGET_SetProperty(po, PROP_INACTIVE_BGCOLOR, rgb);
}

// selected/active background color
static __inline int IWIDGET_GetSelectedInactiveBGColor(IWidget *po, RGBAVAL *prgb) {
   return IWIDGET_GetPropRGB(po, PROP_SINACTIVE_BGCOLOR, prgb);
}
static __inline int IWIDGET_SetSelectedInactiveBGColor(IWidget *po, RGBAVAL rgb) {
   return IWIDGET_SetProperty(po, PROP_SINACTIVE_BGCOLOR, rgb);
}

// selected/both foreground color
static __inline int IWIDGET_SetSelectedBGColor(IWidget *po, RGBAVAL rgb) {
   return IWIDGET_SetProperty(po, PROP_SELECTED_BGCOLOR, rgb);
}

static __inline int IWIDGET_GetBGTransparency(IWidget *po, int *pnTrans) {
   return IWIDGET_GetPropInt(po, PROP_BGTRANSPARENCY, pnTrans);
}
static __inline int IWIDGET_SetBGTransparency(IWidget *po, int nTrans) {
   return IWIDGET_SetProperty(po, PROP_BGTRANSPARENCY, nTrans);
}

static __inline int IWIDGET_GetBorderTransparency(IWidget *po, int *pnTrans) {
   return IWIDGET_GetPropInt(po, PROP_BORDERTRANSPARENCY, pnTrans);
}
static __inline int IWIDGET_SetBorderTransparency(IWidget *po, int nTrans) {
   return IWIDGET_SetProperty(po, PROP_BORDERTRANSPARENCY, nTrans);
}

static __inline int IWIDGET_GetTransparency(IWidget *po, int *pnTrans) {
   return IWIDGET_GetPropInt(po, PROP_TRANSPARENCY, pnTrans);
}
static __inline int IWIDGET_SetTransparency(IWidget *po, int nTrans) {
   return IWIDGET_SetProperty(po, PROP_TRANSPARENCY, nTrans);
}

#define IWIDGET_SetOpacity(p,v) IWIDGET_SetTransparency(p, 255 - (v))


static __inline int IWIDGET_GetViewModel(IWidget *po, IModel **ppo) {
   return IWIDGET_GetPropPtr(po, PROP_VIEWMODEL, (void **)ppo);
}
static __inline int IWIDGET_SetViewModel(IWidget *po, IModel *ppo) {
   return IWIDGET_SetProperty(po, PROP_VIEWMODEL, (uint32)ppo);
}

static __inline int IWIDGET_GetFocusIndex(IWidget *po, int *pnIndex) {
   return IWIDGET_GetPropInt(po, PROP_FOCUSINDEX, pnIndex);
}
static __inline int IWIDGET_SetFocusIndex(IWidget *po, int index) {
   return IWIDGET_SetProperty(po, PROP_FOCUSINDEX, (uint32)index);
}

static __inline int IWIDGET_GetTopIndex(IWidget *po, int *pnIndex) {
   return IWIDGET_GetPropInt(po, PROP_TOPINDEX, pnIndex);
}
static __inline int IWIDGET_SetTopIndex(IWidget *po, int index) {
   return IWIDGET_SetProperty(po, PROP_TOPINDEX, (uint32)index);
}

static __inline int IWIDGET_SelectIndex(IWidget *po, int index) {
   return IWIDGET_SetProperty(po, PROP_SELECTINDEX, (uint32)index);
}

static __inline int IWIDGET_GetItemWidth(IWidget *po, int *pnDelta) {
   return IWIDGET_GetPropInt(po, PROP_ITEMWIDTH, pnDelta);
}
static __inline int IWIDGET_SetItemWidth(IWidget *po, int v) {
   return IWIDGET_SetProperty(po, PROP_ITEMWIDTH, (uint32)v);
}

static __inline int IWIDGET_GetItemHeight(IWidget *po, int *pnDelta) {
   return IWIDGET_GetPropInt(po, PROP_ITEMHEIGHT, pnDelta);
}
static __inline int IWIDGET_SetItemHeight(IWidget *po, int v) {
   return IWIDGET_SetProperty(po, PROP_ITEMHEIGHT, (uint32)v);
}

static __inline int IWIDGET_GetFlags(IWidget *po, uint32 *pdwFlags) {
   return IWIDGET_GetProperty(po, PROP_FLAGS, pdwFlags);
}
static __inline int IWIDGET_SetFlags(IWidget *po, uint32 dwFlags) {
   return IWIDGET_SetProperty(po, PROP_FLAGS, dwFlags);
}
static __inline int IWIDGET_AddFlags(IWidget *po, uint32 dwFlagsToAdd) {
   uint32 dwFlags;
   IWIDGET_GetFlags(po, &dwFlags);
   dwFlags |= dwFlagsToAdd;
   return IWIDGET_SetFlags(po, dwFlags);
}
static __inline int IWIDGET_RemoveFlags(IWidget *po, uint32 dwFlagsToRemove) {
   uint32 dwFlags;
   IWIDGET_GetFlags(po, &dwFlags);
   dwFlags &= ~dwFlagsToRemove;
   return IWIDGET_SetFlags(po, dwFlags);
}

static __inline int IWIDGET_GetTranspColor(IWidget *po, RGBVAL *prgbTransp) {
   return IWIDGET_GetPropRGB(po, PROP_TRANSP_COLOR, prgbTransp);
}
static __inline int IWIDGET_SetTranspColor(IWidget *po, RGBVAL rgbTransp) {
   return IWIDGET_SetProperty(po, PROP_TRANSP_COLOR, (uint32)rgbTransp);
}

static __inline int IWIDGET_GetSelected(IWidget *po, boolean *pbSelected) {
   return IWIDGET_GetPropBool(po, PROP_SELECTED, pbSelected);
}
static __inline int IWIDGET_SetSelected(IWidget *po, boolean bSelected) {
   return IWIDGET_SetProperty(po, PROP_SELECTED, (uint32)bSelected);
}

static __inline int IWIDGET_GetScrollHandleWidth(IWidget *po, int *pnWidth) {
   return IWIDGET_GetPropInt(po, PROP_SCROLLHANDLEWIDTH, pnWidth);
}
static __inline int IWIDGET_SetScrollHandleWidth(IWidget *po, int nWidth) {
   return IWIDGET_SetProperty(po, PROP_SCROLLHANDLEWIDTH, (uint32)nWidth);
}

static __inline int IWIDGET_GetScrollPad(IWidget *po, int *pnPad) {
   return IWIDGET_GetPropInt(po, PROP_SCROLLPAD, pnPad);
}
static __inline int IWIDGET_SetScrollPad(IWidget *po, int nPad) {
   return IWIDGET_SetProperty(po, PROP_SCROLLPAD, (uint32)nPad);
}

static __inline int IWIDGET_GetScrollEndPad(IWidget *po, int *pnPad) {
   return IWIDGET_GetPropInt(po, PROP_SCROLLBARENDPAD, pnPad);
}
static __inline int IWIDGET_SetScrollEndPad(IWidget *po, int nPad) {
   return IWIDGET_SetProperty(po, PROP_SCROLLBARENDPAD, (uint32)nPad);
}

static __inline int IWIDGET_GetScrollStyle(IWidget *po, int *pnStyle) {
   return IWIDGET_GetPropInt(po, PROP_SCROLLSTYLE, pnStyle);
}
static __inline int IWIDGET_SetScrollStyle(IWidget *po, int nStyle) {
   return IWIDGET_SetProperty(po, PROP_SCROLLSTYLE, (uint32)nStyle);
}

static __inline int IWIDGET_GetActiveScrollColor(IWidget *po, RGBVAL *prgb) {
   return IWIDGET_GetPropRGB(po, PROP_ACTIVE_SCROLLCOLOR, prgb);
}
static __inline int IWIDGET_SetActiveScrollColor(IWidget *po, RGBVAL rgb) {
   return IWIDGET_SetProperty(po, PROP_ACTIVE_SCROLLCOLOR, rgb);
}

static __inline int IWIDGET_GetInactiveScrollColor(IWidget *po, RGBVAL *prgb) {
   return IWIDGET_GetPropRGB(po, PROP_INACTIVE_SCROLLCOLOR, prgb);
}
static __inline int IWIDGET_SetInactiveScrollColor(IWidget *po, RGBVAL rgb) {
   return IWIDGET_SetProperty(po, PROP_INACTIVE_SCROLLCOLOR, rgb);
}

static __inline int IWIDGET_SetScrollColor(IWidget *po, RGBVAL rgb) {
   return IWIDGET_SetProperty(po, PROP_SCROLLCOLOR, (uint32)rgb);
}

static __inline int IWIDGET_GetActiveScrollHandleColor(IWidget *po, RGBVAL *prgb) {
   return IWIDGET_GetPropRGB(po, PROP_ACTIVE_SCROLLHANDLECOLOR, prgb);
}
static __inline int IWIDGET_SetActiveScrollHandleColor(IWidget *po, RGBVAL rgb) {
   return IWIDGET_SetProperty(po, PROP_ACTIVE_SCROLLHANDLECOLOR, rgb);
}

static __inline int IWIDGET_GetInactiveScrollHandleColor(IWidget *po, RGBVAL *prgb) {
   return IWIDGET_GetPropRGB(po, PROP_INACTIVE_SCROLLHANDLECOLOR, prgb);
}
static __inline int IWIDGET_SetInactiveScrollHandleColor(IWidget *po, RGBVAL rgb) {
   return IWIDGET_SetProperty(po, PROP_INACTIVE_SCROLLHANDLECOLOR, rgb);
}

static __inline int IWIDGET_SetScrollHandleColor(IWidget *po, RGBVAL rgb) {
   return IWIDGET_SetProperty(po, PROP_SCROLLHANDLECOLOR, (uint32)rgb);
}

static __inline int IWIDGET_GetScrollFillColor(IWidget *po, RGBVAL *prgb) {
   return IWIDGET_GetPropRGB(po, PROP_SCROLLFILLCOLOR, prgb);
}
static __inline int IWIDGET_SetScrollFillColor(IWidget *po, RGBVAL rgb) {
   return IWIDGET_SetProperty(po, PROP_SCROLLFILLCOLOR, rgb);
}

static __inline int IWIDGET_GetSoftkey(IWidget *po, uint16 skid, IWidget **ppo) {
   return IWIDGET_GetProperty(po, skid, (uint32 *)ppo);
}
static __inline int IWIDGET_SetSoftkey(IWidget *po, uint16 skid, IWidget *piw) {
   return IWIDGET_SetProperty(po, skid, (uint32)piw);
}

static __inline int IWIDGET_SetShadowTransparency(IWidget *po, int nTransp) {
   return IWIDGET_SetProperty(po, PROP_SHADOWTRANSPARENCY, (uint32)nTransp);
}
static __inline int IWIDGET_SetShadowOffset(IWidget *po, int nOffset) {
   return IWIDGET_SetProperty(po, PROP_SHADOWOFFSET, (uint32)nOffset);
}
static __inline int IWIDGET_SetShadowColor(IWidget *po, RGBAVAL rgb) {
   return IWIDGET_SetProperty(po, PROP_SHADOWCOLOR, (uint32)rgb);
}
static __inline int IWIDGET_SetShadowOffsetX(IWidget *po, int nOffset) {
   return IWIDGET_SetProperty(po, PROP_SHADOWOFFSETX, (uint32)nOffset);
}
static __inline int IWIDGET_SetShadowOffsetY(IWidget *po, int nOffset) {
   return IWIDGET_SetProperty(po, PROP_SHADOWOFFSETY, (uint32)nOffset);
}

static __inline int IWIDGET_SetSelectedShadowColor(IWidget *po, RGBAVAL rgb) {
   return IWIDGET_SetProperty(po, PROP_SELECTED_SHADOWCOLOR, (uint32)rgb);
}
static __inline int IWIDGET_SetSelectedShadowOffset(IWidget *po, int nOffset) {
   return IWIDGET_SetProperty(po, PROP_SELECTED_SHADOWOFFSET, (uint32)nOffset);
}
static __inline int IWIDGET_SetSelectedShadowOffsetX(IWidget *po, int nOffset) {
   return IWIDGET_SetProperty(po, PROP_SELECTED_SHADOWOFFSETX, (uint32)nOffset);
}
static __inline int IWIDGET_SetSelectedShadowOffsetY(IWidget *po, int nOffset) {
   return IWIDGET_SetProperty(po, PROP_SELECTED_SHADOWOFFSETY, (uint32)nOffset);
}

static __inline int IWIDGET_GetRows(IWidget *po, int *pnValue) {
   return IWIDGET_GetPropInt(po, PROP_ROWS, pnValue);
}
static __inline int IWIDGET_SetRows(IWidget *po, int v) {
   return IWIDGET_SetProperty(po, PROP_ROWS, (uint32)v);
}

static __inline int IWIDGET_GetCols(IWidget *po, int *pnValue) {
   return IWIDGET_GetPropInt(po, PROP_COLS, pnValue);
}
static __inline int IWIDGET_SetCols(IWidget *po, int v) {
   return IWIDGET_SetProperty(po, PROP_COLS, (uint32)v);
}

static __inline int IWIDGET_GetHintRows(IWidget *po, int *pnValue) {
   return IWIDGET_GetPropInt(po, PROP_HINT_ROWS, pnValue);
}
static __inline int IWIDGET_SetHintRows(IWidget *po, int v) {
   return IWIDGET_SetProperty(po, PROP_HINT_ROWS, (uint32)v);
}

static __inline int IWIDGET_GetHintCols(IWidget *po, int *pnValue) {
   return IWIDGET_GetPropInt(po, PROP_HINT_COLS, pnValue);
}
static __inline int IWIDGET_SetHintCols(IWidget *po, int v) {
   return IWIDGET_SetProperty(po, PROP_HINT_COLS, (uint32)v);
}

static __inline int IWIDGET_GetHintWidth(IWidget *po, int *pnWidth) {
   return IWIDGET_GetPropInt(po, PROP_HINT_WIDTH, pnWidth);
}
static __inline int IWIDGET_SetHintWidth(IWidget *po, int nWidth) {
   return IWIDGET_SetProperty(po, PROP_HINT_WIDTH, nWidth);
}

static __inline int IWIDGET_GetOffsetX(IWidget *po, int *pnValue) {
   return IWIDGET_GetPropInt(po, PROP_OFFSETX, pnValue);
}
static __inline int IWIDGET_SetOffsetX(IWidget *po, int v) {
   return IWIDGET_SetProperty(po, PROP_OFFSETX, (uint32)v);
}

static __inline int IWIDGET_GetOffsetY(IWidget *po, int *pnValue) {
   return IWIDGET_GetPropInt(po, PROP_OFFSETY, pnValue);
}
static __inline int IWIDGET_SetOffsetY(IWidget *po, int v) {
   return IWIDGET_SetProperty(po, PROP_OFFSETY, (uint32)v);
}

static __inline int IWIDGET_GetIncrement(IWidget *po, int *pnValue) {
   return IWIDGET_GetPropInt(po, PROP_INCREMENT, pnValue);
}
static __inline int IWIDGET_SetIncrement(IWidget *po, int v) {
   return IWIDGET_SetProperty(po, PROP_INCREMENT, (uint32)v);
}

static __inline int IWIDGET_RequestScrollEvent(IWidget *po) {
   return IWIDGET_SetProperty(po, PROP_SCROLLREQ, (uint32)0);
}

static __inline int IWIDGET_GetCaretPos(IWidget *po, int *pnCaretXPos, int *pnCaretYPos) {
   uint32 dwTemp = 0;
   int nErr = IWIDGET_GetProperty(po, PROP_CARETPOS, &dwTemp);
   if (!nErr) {
      *pnCaretXPos = (dwTemp & 0xFFFF);
      *pnCaretYPos = ((dwTemp >> 16) & 0xFFFF);
   }
   return nErr;
}

static __inline int IWIDGET_GetLayoutStyle(IWidget *po, int *pnStyle) {
   return IWIDGET_GetPropInt(po, PROP_LAYOUTSTYLE, pnStyle);
}
static __inline int IWIDGET_SetLayoutStyle(IWidget *po, int nStyle) {
   return IWIDGET_SetProperty(po, PROP_LAYOUTSTYLE, (uint32)nStyle);
}

static __inline int IWIDGET_SetFocusListener(IWidget *po, ModelListener *pml, PFNLISTENER pfn, void *pv) {
   
   IModel *pim = 0;
   IWIDGET_GetViewModel(po, &pim);
   if (pim) {
      IMODEL_AddListenerEx(pim, pml, pfn, pv);
      IMODEL_Release(pim);
      return SUCCESS;
   }
   return EFAILED;
}


static __inline int IWIDGET_SetIndexer(IWidget *po, PFNINDEXER pfnIndexer, void *pCtx) {
   IndexerDesc idxd;
   idxd.pfnIndexer = pfnIndexer;
   idxd.pCtx       = pCtx;
   return IWIDGET_SetProperty(po, PROP_INDEXER, (uint32)&idxd);
}

static __inline int IWIDGET_SetPasswordMaskChar(IWidget *po, AECHAR wch) {
   return IWIDGET_SetProperty(po, PROP_PASSWORDMASKCHAR, (uint32)wch);
}

static __inline int IWIDGET_SetPropImageStrip(IWidget *po, IBitmap *pib) {
   return IWIDGET_SetProperty(po, PROP_IMAGESTRIP, (uint32)pib);
}

static __inline int IWIDGET_SetPropImageStripH(IWidget *po, IBitmap *pib) {
   return IWIDGET_SetProperty(po, PROP_IMAGESTRIP_H, (uint32)pib);
}

static __inline int IWIDGET_SetPropImageStripV(IWidget *po, IBitmap *pib) {
   return IWIDGET_SetProperty(po, PROP_IMAGESTRIP_V, (uint32)pib);
}

static __inline int IWIDGET_GetTabSpacingH(IWidget *po, int *pnValue) {
   return IWIDGET_GetPropInt(po, PROP_TABSPACING_HORZ, pnValue);
}
static __inline int IWIDGET_SetTabSpacingH(IWidget *po, int v) {
   return IWIDGET_SetProperty(po, PROP_TABSPACING_HORZ, (uint32)v);
}

static __inline int IWIDGET_GetTabSpacingV(IWidget *po, int *pnValue) {
   return IWIDGET_GetPropInt(po, PROP_TABSPACING_VERT, pnValue);
}
static __inline int IWIDGET_SetTabSpacingV(IWidget *po, int v) {
   return IWIDGET_SetProperty(po, PROP_TABSPACING_VERT, (uint32)v);
}

static __inline int IWIDGET_GetActiveTabWidth(IWidget *po, int *pnValue) {
   return IWIDGET_GetPropInt(po, PROP_ACTIVE_TABWIDTH, pnValue);
}
static __inline int IWIDGET_SetActiveTabWidth(IWidget *po, int v) {
   return IWIDGET_SetProperty(po, PROP_ACTIVE_TABWIDTH, (uint32)v);
}

static __inline int IWIDGET_GetInactiveTabWidth(IWidget *po, int *pnValue) {
   return IWIDGET_GetPropInt(po, PROP_INACTIVE_TABWIDTH, pnValue);
}
static __inline int IWIDGET_SetInactiveTabWidth(IWidget *po, int v) {
   return IWIDGET_SetProperty(po, PROP_INACTIVE_TABWIDTH, (uint32)v);
}

static __inline int IWIDGET_GetClientRect(IWidget *po, AEERect *prc) {
   return IWIDGET_GetProperty(po, PROP_CLIENTRECT, (uint32*)prc);
}

static __inline int IWIDGET_SetGradientStyle(IWidget *po, uint16 nStyle) {
   return IWIDGET_SetProperty(po, PROP_GRADIENT_STYLE, (uint32)nStyle);
}

static __inline int IWIDGET_Animate(IWidget *po, boolean bStart) {
   return IWIDGET_SetProperty(po, PROP_ANIMATE, (uint32)bStart);
}

static __inline boolean IWIDGET_IsAnimated(IWidget *po) {
   boolean bAnim = 0;
   return (SUCCESS == IWIDGET_GetPropBool(po, PROP_ANIMATE, &bAnim) && bAnim);
}

static __inline int IWIDGET_GetLineGap(IWidget *po, int *pnLineGap) {
   return IWIDGET_GetPropInt(po, PROP_LINEGAP, pnLineGap);
}

static __inline int IWIDGET_SetLineGap(IWidget *po, int nLineGap) {
   return IWIDGET_SetProperty(po, PROP_LINEGAP, (uint32)nLineGap);
}

static __inline int IWIDGET_SetPropertiesV(IWidget *piWidget, IResFile *piResFile, va_list* args) {      
   WResPropDesc desc;
   desc.piWidget = piWidget;   
   desc.args = (va_list *)args;   
   desc.piResFile = piResFile; 
   return (piWidget) ? IWIDGET_SetProperty(piWidget, PROP_APPLYWPROPS, (uint32)&desc) : EBADPARM;
}

static __inline int IWIDGET_SetProperties(IWidget *piWidget, IResFile *piResFile,  ...) {
   int result;
   va_list args;
   va_start(args, piResFile);
   result = IWIDGET_SetPropertiesV(piWidget, piResFile, &args);
   va_end(args);
   return result;
}

static __inline int IWIDGET_ContainerRaiseWidget(IWidget *po, IWidget *piwRaise, IXYContainer *pixyRaiseContainer) {
   RaiseDesc rd;
   rd.pixyTarget  = pixyRaiseContainer;
   rd.piw         = piwRaise;
   return IWIDGET_SetProperty(po, PROP_RAISE, (uint32)&rd);
}

static __inline int ICONTAINER_RaiseWidget(IContainer *po, IWidget *piwRaise, IXYContainer *pixyRaiseContainer) {
   IWidget *piwc = 0;
   int nErr = ICONTAINER_QueryInterface(po, AEEIID_WIDGET, (void**)&piwc);
   if (!nErr) {
      nErr = IWIDGET_ContainerRaiseWidget(piwc, piwRaise, pixyRaiseContainer);
      IWIDGET_Release(piwc);
   }
   return nErr;
}

static __inline int IWIDGET_Raise(IWidget *po, IXYContainer *pixyRaiseTo) {
   IContainer *picParent = 0;
   int nErr = EFAILED;

   IWIDGET_GetParent(po, &picParent);
   if (picParent) {
      nErr = ICONTAINER_RaiseWidget(picParent, po, pixyRaiseTo);
      ICONTAINER_Release(picParent);
   }
   return nErr;
}

static __inline IXYContainer* IWIDGET_ContainerGetRaiseTo(IWidget *po, IWidget *piwRaised) {
   RaiseDesc rd;
   rd.pixyTarget  = NULL;
   rd.piw         = piwRaised;
   IWIDGET_GetProperty(po, PROP_RAISE, (uint32*)&rd);
   return rd.pixyTarget;
}


static __inline int IWIDGET_SetBGImage(IWidget *po, IImage *pii) {
   return IWIDGET_SetProperty(po, PROP_BGIMAGE, (uint32)pii);
}

static __inline int IWIDGET_GetBGImage(IWidget *po, IImage **ppii) {
   return IWIDGET_GetProperty(po, PROP_BGIMAGE, (uint32*)ppii);
}

static __inline int IWIDGET_SetBGImageFlags(IWidget *po, uint32 dwFlags) {
   return IWIDGET_SetProperty(po, PROP_BGIMAGE_FLAGS, dwFlags);
}

static __inline int IWIDGET_GetBGImageFlags(IWidget *po, uint32 *pdwFlags) {
   return IWIDGET_GetProperty(po, PROP_BGIMAGE_FLAGS, pdwFlags);
}

static __inline int IWIDGET_SetBGImageFrame(IWidget *po, int nFrame) {
   return IWIDGET_SetProperty(po, PROP_BGIMAGE_FRAME, (uint32)nFrame);
}

static __inline int IWIDGET_SetBGImageAnimate(IWidget *po, boolean bStart) {
   return IWIDGET_SetProperty(po, PROP_BGIMAGE_ANIMATE, (uint32)bStart);
}

static __inline int IWIDGET_SetBGImageParm(IWidget *po, int parm, int arg1, int arg2) {
   ImageParm ip;
   ip.parm = parm;
   ip.arg1 = arg1;
   ip.arg2 = arg2;
   return IWIDGET_SetProperty(po, PROP_BGIMAGE_PARM, (uint32)&parm);
}

static __inline int IWIDGET_SetBGImageOffset(IWidget *po, int xOffset, int yOffset) {
   return IWIDGET_SetBGImageParm(po, IPARM_OFFSET, xOffset, yOffset);
}


#ifndef IPARM_SCALE
#define IPARM_SCALE     12   // p1 = scaled width, p2 = scaled height
#endif

static __inline int IWIDGET_SetBGImageScale(IWidget *po, uint32 dx, uint32 dy) {
   return IWIDGET_SetBGImageParm(po, IPARM_SCALE, (int)dx, (int)dy);
}

static __inline int IWIDGET_SetBGImageSize(IWidget *po, int dx, int dy) {
   return IWIDGET_SetBGImageParm(po, IPARM_SIZE, dx, dy);
}

static __inline int IWIDGET_SetBGImageNFrames(IWidget *po, int nFrames) {
   return IWIDGET_SetBGImageParm(po, IPARM_NFRAMES, nFrames, 0);
}

static __inline int IWIDGET_SetBGImageRate(IWidget *po, uint32 dwMsecs) {
   return IWIDGET_SetBGImageParm(po, IPARM_RATE, (int)dwMsecs, 0);
}

static __inline int IWIDGET_SetBGImageROP(IWidget *po, uint32 rop) {
   return IWIDGET_SetBGImageParm(po, IPARM_ROP, (int)rop, 0);
}

static __inline int IWIDGET_SetBGImageTiledBorder(IWidget *po, uint16 left, uint16 top, 
                                                  uint16 right, uint16 bottom) {
   BGImageTiledBorder tb;
   tb.left    = left;
   tb.top     = top;
   tb.right   = right;
   tb.bottom  = bottom;
   return IWIDGET_SetProperty(po, PROP_BGIMAGE_TILED_BORDER, (uint32)&tb);
}

// get my parent's widget interface
static __inline int IWIDGET_GetParentWidget(IWidget *po, IWidget **ppiw) {
   IContainer *pic = 0;
   int nErr = EFAILED;

   *ppiw = 0;

   IWIDGET_GetParent(po, &pic);
   if (pic) {
      nErr = ICONTAINER_QueryInterface(pic, AEEIID_WIDGET, (void**)ppiw);
      ICONTAINER_Release(pic);
   }
   return nErr;
}

// ask a container (through its IWidget i/f) to set visibility state of its child widget
static __inline int IWIDGET_SetChildVisibility(IWidget *po, IWidget *piwChild, boolean bVisible) {
   WidgetVis wv;
   wv.piw = piwChild;
   wv.bVisible = bVisible;
   return IWIDGET_SetProperty(po, PROP_VISIBLE, (uint32)&wv);      
}


// ask a container (through its IWidget i/f) to get visibility state of its child widget 
static __inline int IWIDGET_GetChildVisibility(IWidget *po, IWidget *piwChild, boolean *pbVisible) {
   int nErr;
   WidgetVis wv;
   wv.piw = piwChild;
   wv.bVisible = 0;
   nErr = IWIDGET_GetProperty(po, PROP_VISIBLE, (uint32*)&wv);      
   *pbVisible = wv.bVisible;
   return nErr;
}

// am I visible?
static __inline boolean IWIDGET_IsVisible(IWidget *po) {
   boolean bVis = 0;
   IWidget *piwParent = 0;
   if (SUCCESS == IWIDGET_GetParentWidget(po, &piwParent)) {
       IWIDGET_GetChildVisibility(piwParent, po, &bVis);
       IWIDGET_Release(piwParent);
   }
   return bVis;
}

// set my visibility
static __inline void IWIDGET_SetVisible(IWidget *po, boolean bVisible) {
   IWidget *piwParent = 0;
   if (SUCCESS == IWIDGET_GetParentWidget(po, &piwParent)) {
       IWIDGET_SetChildVisibility(piwParent, po, bVisible);
       IWIDGET_Release(piwParent);
   }
}


#endif /* __AEEWPROPERTIES_H__ */

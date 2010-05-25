/*
  ========================================================================

  FILE:                 AEETextLayout.h
  
  SERVICES:             ITextLayout

  GENERAL DESCRIPTION:  I define an object that lays out and draws text
                        within a specified rectangle.

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#ifndef __AEE_LAYOUTTEXT_H__
#define __AEE_LAYOUTTEXT_H__

#include "AEE.h"
#include "AEEFontMapModel.h"
#include "AEEWProperties.h"

// TextLayout Interface ID
#define AEEIID_TEXTLAYOUT    	0x0103bd09

// ITextLayout Layout Flags
#define TEXTLAYOUT_FLAG_MULTILINE      0x10000000           // this is a multiline text layout
#define TEXTLAYOUT_FLAG_BREAKONWORD    0x20000000           // break lines on word boundaries
#define TEXTLAYOUT_FLAG_ELLIPSIFY      0x40000000           // truncate the text with an ellipsis
#define TEXTLAYOUT_FLAG_TRIMLINE       0x80000000           // trim whitespace from end of lines

// ITextLayout Interface
#define INHERIT_ITextLayout(iname) \
   INHERIT_IQueryInterface          (iname); \
   boolean  (*HandleEvent)          (iname *po, AEEEvent evt, uint16 wParam, uint32 dwParam); \
   int      (*GetPreferredExtent)   (iname *po, IModel *piModel, uint32 dwLayoutFlags, WExtent *pwPreferredExtent); \
   int      (*Layout)               (iname *po, IModel *piModel, WExtent *pWExtent, uint32 dwLayoutFlags); \
   int      (*Draw)                 (iname *po, ICanvas *piCanvas, int dwX, int dwY); \
   int      (*GetLayoutSize)        (iname *po, WExtent *pwe, int *pdwLineCount); \
   int      (*GetLineFromIndex)     (iname *po, int dwIndex, int *pdwLine); \
   int      (*GetLineStartIndex)    (iname *po, int dwLine,  int *pdwIndex); \
   int      (*GetCaretInfo)         (iname *po, int dwIndex, AEERect *prcCaret); \
   int      (*GetCaretAfterMove)    (iname *po, int dwCaret, int dwMovement, int *pdwMovedCaret); \
   int      (*ComputeScrollOffset)  (iname *po, int dwTextWidth, int dwDisplayWidth, int dwScrollOffset)

AEEINTERFACE_DEFINE(ITextLayout);

// AddRef
static __inline uint32 ITEXTLAYOUT_AddRef(ITextLayout *po) {
   return AEEGETPVTBL(po, ITextLayout)->AddRef(po);
}

// Release
static __inline uint32 ITEXTLAYOUT_Release(ITextLayout *po) {
   return AEEGETPVTBL(po, ITextLayout)->Release(po);
}

// QI
static __inline int ITEXTLAYOUT_QueryInterface(ITextLayout *po, AEECLSID cls, void **ppo) {
   return AEEGETPVTBL(po, ITextLayout)->QueryInterface(po, cls, ppo);
}

// HandleEvent for properties
static __inline boolean ITEXTLAYOUT_HandleEvent(ITextLayout *po, AEEEvent evt, uint16 wParam, uint32 dwParam) {
   return AEEGETPVTBL(po, ITextLayout)->HandleEvent(po, evt, wParam, dwParam);
}

// get the preferred extent of the text layout, using PROP_HINTWIDTH or PROP_HINTROWS.
static __inline int ITEXTLAYOUT_GetPreferredExtent(ITextLayout *po, IModel *piModel, uint32 dwLayoutFlags, WExtent *pwPreferredExtent) {
   return AEEGETPVTBL(po, ITextLayout)->GetPreferredExtent(po, piModel, dwLayoutFlags, pwPreferredExtent);
}

// layout text stored within a model.
static __inline int ITEXTLAYOUT_Layout(ITextLayout *po, IModel *piModel, WExtent *pWExtent, uint32 dwLayoutFlags) {
   return AEEGETPVTBL(po, ITextLayout)->Layout(po, piModel, pWExtent, dwLayoutFlags);
}

// draw a layout to screen, clipping according to the canvas's clip rect
static __inline int ITEXTLAYOUT_Draw(ITextLayout *po, ICanvas *piCanvas, int dwX, int dwY) {
   return AEEGETPVTBL(po, ITextLayout)->Draw(po, piCanvas, dwX, dwY);
}

// UI Layout related routines.  To be called after a TextLayout has been composed with ITEXTLAYOUT_Layout()

// return the actual size of the text
static __inline int ITEXTLAYOUT_GetLayoutSize(ITextLayout *po, WExtent *pwe, int *pdwLineCount) {
   return AEEGETPVTBL(po, ITextLayout)->GetLayoutSize(po, pwe, pdwLineCount);
}

// given an character index, return which line it's on
static __inline int ITEXTLAYOUT_GetLineFromIndex(ITextLayout *po, int dwIndex, int *pdwLine) {
   return AEEGETPVTBL(po, ITextLayout)->GetLineFromIndex(po, dwIndex, pdwLine);
}

// given a line index, return which character the line starts with
static __inline int ITEXTLAYOUT_GetLineStartIndex(ITextLayout *po, int dwLine,  int *pdwIndex) {
   return AEEGETPVTBL(po, ITextLayout)->GetLineStartIndex(po, dwLine, pdwIndex);
}

// return size information about the caret given a particular in-memory character
static __inline int ITEXTLAYOUT_GetCaretInfo(ITextLayout *po, int dwIndex, AEERect *prcCaret) {
   return AEEGETPVTBL(po, ITextLayout)->GetCaretInfo(po, dwIndex, prcCaret);
}

// determine the character index the caret ended up at after a particular movement.  Movement
// types are defined in AEEWProperties.h.
static __inline int ITEXTLAYOUT_GetCaretAfterMove(ITextLayout *po, int dwCaret, int dwMovement, int *pdwMovedCaret) {
   return AEEGETPVTBL(po, ITextLayout)->GetCaretAfterMove(po, dwCaret, dwMovement, pdwMovedCaret);
}

// given the width of the display area and the current scroll offset, return an offset that
// should be added to the x-coordinate used to display the text to scroll it.
static __inline int ITEXTLAYOUT_ComputeScrollOffset(ITextLayout *po, int dwTextWidth, int dwDisplayWidth, int dwScrollOffset) {
   return AEEGETPVTBL(po, ITextLayout)->ComputeScrollOffset(po, dwTextWidth, dwDisplayWidth, dwScrollOffset);
}


// ITextLayout properties

// Get/Set properties
static __inline int ITEXTLAYOUT_GetProperty(ITextLayout *po, uint16 propId, uint32 *pVal) {
   return ((0 != ITEXTLAYOUT_HandleEvent(po, EVT_WDG_GETPROPERTY, propId, (uint32)pVal)) ? SUCCESS : EFAILED);
}

static __inline int ITEXTLAYOUT_SetProperty(ITextLayout *po, uint16 propId, uint32 val) {
   return ((0 != ITEXTLAYOUT_HandleEvent(po, EVT_WDG_SETPROPERTY, propId, val)) ? SUCCESS : EFAILED);
}

// Ellipsis - defaults to 0x2029
static __inline int ITEXTLAYOUT_GetEllipsis(ITextLayout *po, AECHAR *pawEllipsis) {
   return ITEXTLAYOUT_GetProperty(po, PROP_ELLIPSIS, (uint32 *)(void*) pawEllipsis);
}
static __inline int ITEXTLAYOUT_SetEllipsis(ITextLayout *po, AECHAR awEllipsis) {
   return ITEXTLAYOUT_SetProperty(po, PROP_ELLIPSIS, (uint32)(void*)awEllipsis);
}

// FontMapModel - tells TextLayout how to interpret text markup
static __inline int ITEXTLAYOUT_GetFontMapModel(ITextLayout *po, IFontMapModel **ppiFontMapModel) {
   return ITEXTLAYOUT_GetProperty(po, PROP_FONTMAPMODEL, (uint32 *)(void*)ppiFontMapModel);
}
static __inline int ITEXTLAYOUT_SetFontMapModel(ITextLayout *po, IFontMapModel *piFontMapModel) {
   return ITEXTLAYOUT_SetProperty(po, PROP_FONTMAPMODEL, (uint32)piFontMapModel);
}

// Linegap
static __inline int ITEXTLAYOUT_GetLinegap(ITextLayout *po, int *pdwLinegap) {
   return ITEXTLAYOUT_GetProperty(po, PROP_LINEGAP, (uint32 *)pdwLinegap);
}
static __inline int ITEXTLAYOUT_SetLinegap(ITextLayout *po, int dwLinegap) {
   return ITEXTLAYOUT_SetProperty(po, PROP_LINEGAP, (uint32)dwLinegap);
}

// hint width - used to calculate preferred extent
static __inline int ITEXTLAYOUT_GetHintWidth(ITextLayout *po, int *pdwHintWidth) {
   return ITEXTLAYOUT_GetProperty(po, PROP_HINT_WIDTH, (uint32 *)pdwHintWidth);
}
static __inline int ITEXTLAYOUT_SetHintWidth(ITextLayout *po, int dwHintWidth) {
   return ITEXTLAYOUT_SetProperty(po, PROP_HINT_WIDTH, (uint32)dwHintWidth);
}

// hint rows - used to calculate preferred extent
static __inline int ITEXTLAYOUT_GetHintRows(ITextLayout *po, int *pdwHintRows) {
   return ITEXTLAYOUT_GetProperty(po, PROP_HINT_ROWS, (uint32 *)pdwHintRows);
}
static __inline int ITEXTLAYOUT_SetHintRows(ITextLayout *po, int dwHintRows) {
   return ITEXTLAYOUT_SetProperty(po, PROP_HINT_ROWS, (uint32)dwHintRows);
}

// default font 
static __inline int ITEXTLAYOUT_GetFont(ITextLayout *po, IFont **ppIFont) {
   return ITEXTLAYOUT_GetProperty(po, PROP_FONT, CAST(uint32 *,ppIFont));
}
static __inline int ITEXTLAYOUT_SetFont(ITextLayout *po, IFont *pIFont) {
   return ITEXTLAYOUT_SetProperty(po, PROP_FONT, (uint32)pIFont);
}

// default text color
static __inline int ITEXTLAYOUT_GetFGColor(ITextLayout *po, RGBAVAL *prgba) {
   return ITEXTLAYOUT_GetProperty(po, PROP_FGCOLOR, (uint32 *)prgba);
}
static __inline int ITEXTLAYOUT_SetFGColor(ITextLayout *po, RGBAVAL rgba) {
   return ITEXTLAYOUT_SetProperty(po, PROP_FGCOLOR, (uint32)rgba);
}

// background color
static __inline int ITEXTLAYOUT_GetBGColor(ITextLayout *po, RGBAVAL *prgba) {
   return ITEXTLAYOUT_GetProperty(po, PROP_BGCOLOR, (uint32 *)prgba);
}
static __inline int ITEXTLAYOUT_SetBGColor(ITextLayout *po, RGBAVAL rgba) {
   return ITEXTLAYOUT_SetProperty(po, PROP_BGCOLOR, (uint32)rgba);
}

// default selected text color
static __inline int ITEXTLAYOUT_GetSelectedFGColor(ITextLayout *po, RGBAVAL rgba) {
   return ITEXTLAYOUT_GetProperty(po, PROP_SELECTED_FGCOLOR, (uint32*)rgba);
}

static __inline int ITEXTLAYOUT_SetSelectedFGColor(ITextLayout *po, RGBAVAL rgba) {
   return ITEXTLAYOUT_SetProperty(po, PROP_SELECTED_FGCOLOR, (uint32)rgba);
}

static __inline int ITEXTLAYOUT_GetSelectedBGColor(ITextLayout *po, RGBAVAL rgba) {
   return ITEXTLAYOUT_GetProperty(po, PROP_SELECTED_BGCOLOR, (uint32*)rgba);
}
static __inline int ITEXTLAYOUT_SetSelectedBGColor(ITextLayout *po, RGBAVAL rgba) {
   return ITEXTLAYOUT_SetProperty(po, PROP_SELECTED_BGCOLOR, (uint32)rgba);
}

static __inline int ITEXTLAYOUT_GetTextRects(ITextLayout *po, uint32 start, uint32 end, uint32* pCount, AEERect* pRects) {
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
   
   result = ITEXTLAYOUT_GetProperty(po, PROP_TEXTRECTS, (uint32*)(void*)&wtr);
   if ( SUCCESS == result ) {
      *pCount = wtr.count;
   }

   return result;
}


#endif   //__AEE_LAYOUTTEXT_H__

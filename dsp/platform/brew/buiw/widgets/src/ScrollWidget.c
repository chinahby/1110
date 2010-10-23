/*
  ========================================================================

  FILE:  ScrollbarWidget.c
  
  SERVICES:  

  GENERAL DESCRIPTION: ScrollbarWidget implementation

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#include "ScrollWidget.h"
#include "AEEWProperties.h"
#include "wutil.h"
#include "widget_res.h"
#include "AEEDisplayCanvas.h"


#define SBBORDER_WIDTH     1
#define SBWIDTH            (me->nHandleWidth + (2 * SBBORDER_WIDTH))

#define DEFAULT_SCROLLBORDER_COLOR MAKE_RGBA(0, 0, 0, 255);
#define DEFAULT_SCROLLHANDLE_COLOR MAKE_RGBA(192, 192, 192, 255);

static __inline ScrollbarWidget *DECORATOR_TO_SCROLLBARWIDGET(IDecorator *po) {
   return (ScrollbarWidget*)po;
}
static __inline ScrollbarWidget *CONTAINER_TO_SCROLLBARWIDGET(IContainer *po) {
   return (ScrollbarWidget*)po->pMe;
}

static __inline ScrollIndicatorWidget *DECORATOR_TO_SCROLLINDICATORWIDGET(IDecorator *po) {
   return (ScrollIndicatorWidget*)po;
}
static __inline ScrollIndicatorWidget *CONTAINER_TO_SCROLLINDICATORWIDGET(IContainer *po) {
   return (ScrollIndicatorWidget*)po->pMe;
}


#define SCROLLBAR_FROM_DECORATOR ScrollbarWidget *me = DECORATOR_TO_SCROLLBARWIDGET(po)
#define SCROLLBAR_FROM_CONTAINER ScrollbarWidget *me = CONTAINER_TO_SCROLLBARWIDGET(po)

#define SCROLLIND_FROM_DECORATOR ScrollIndicatorWidget *me = DECORATOR_TO_SCROLLINDICATORWIDGET(po)
#define SCROLLIND_FROM_CONTAINER ScrollIndicatorWidget *me = CONTAINER_TO_SCROLLINDICATORWIDGET(po)

// compute scrollbar handle size
//
// The ratio of the scrollbar handle's size to the total
// scrollbar size is the same as the ratio of the
// ScrollEvent's visible member to its total range.  That
// is:
//
//  scrollbar handle (c)        visible (v)
// ----------------------  ==  ------------
//  total scrollbar (d)          range (r)
//
// So:
//
//    c = d * (v / r)
//
static __inline uint16 ScrollbarWidget_ComputeSize(uint32 d, uint32 r, uint32 v)
{
   uint32 c = 0;

   // non-zero range must have been specified
   if (r > 0) {
      c = (d * v) / r;
   }

   return (uint16)c;
}


// compute scrollbar handle position 
//
// Valid handle pixel positions (c) are within the range of 0 
// to the total scrollbar size (d) minus the scrollbar handle
// size.   0 < c < ScrollbarWidget_ComputeSize(d, r, v)
//
// The ScrollEvent's position (p) falls with the same range of 0
// to the total range (r) minus the visible (v).  0 < p < r - v.
// Any position beyond that range is treated as fully scrolled.
//
// To set the scroll handle to its maximum position, p >= r-v.
// 
// For all other positions, the ratio of the pixel position of
// the handle (c) to the total pixel range of the scroll bar handle
// (d - ScrollbarWidget_ComputeSize(d, r, v)) is the same as the 
// ratio of the ScrollEvent's position (p) to the total scroll range 
// minus the visisible (d - v).  That is:
//
// c / (d - ScrollbarWidget_ComputeSize(d, r, v)) == p / (r - v)
//
// So: 
//
// c = (p * (d - ScrollbarWidget_ComputeSize(d, r, v))) / (r - v)
//
// This can be simplified as follows:
//
// c = (p * (d - (d * v/r)) / (r - v)
//
// c = (p * ((dr - dv)/r) / (r - v)
//
// c = (p * d * (r - v) * (1 / r) * 1/(r- v)
//
// Ultimately, the scoll handle's pixel position can be computed
// from the total scroll bar pixel size (d), the ScrollEvent's
// position (p) and the ScrollEvent's total range (r) as:
//
// c = p * d / r
//
static __inline uint16 ScrollbarWidget_ComputePosition(uint32 d, uint32 p, uint32 r, uint32 v) 
{
   uint32 c = 0;

   // non-zero range must have been specified
   if (r > 0) {
      // scrollbar is 100& scrolled if position is greater than or equal to range - visible.
      if (p >= (r - v)) {
         // 100% scrolled, make sure scroll bar is at the bottom for any value
         // of p beyond (r - v).
         c = d - (d * v / r);
      } else {
         // compute scrollbar position.  Do our best to avoid overflow and roundoff errors.  
         // Position, range and visible may be large or small, so let's assume the worst.
         c = p * d;
         c /= r;
      }
   }

   return (uint16)c;
}

/////////////////////////////////////////////////////////////////////////////
//

static void SbArrows_Ctor(SbArrows *me, IShell *piShell)
{
   me->piShell = piShell;
   ISHELL_AddRef(piShell);
}

static void SbArrows_Dtor(SbArrows *me)
{
   ISHELL_Release(me->piShell);
   TiledImage_Dtor(&me->tiVArrows);
   TiledImage_Dtor(&me->tiHArrows);
}

static void SbArrows_DrawVertArrows(SbArrows *me, IDisplay *piDisplay, AEERect *prc, 
                                    RGBVAL rgbUp, RGBVAL rgbDown, boolean bReduce,
                                    int nEndPad)
{
   int x, y;

   // ensure resource image is loaded
   if (!TILEDIMAGE_HASIMAGE(&me->tiVArrows) && 
       me->piShell && 
       0 != TiledImage_LoadResBitmap(&me->tiVArrows, me->piShell, "widgets.mif", IDB_VSBARROWS, 2)) {
      return;
   }

   x = prc->x + ((prc->dx - me->tiVArrows.cxTile) / 2);
   y = prc->y;
   if (rgbUp != RGB_NONE) {
      TiledImage_ReplaceColor(&me->tiVArrows, RGB_BLACK, rgbUp);
      TiledImage_DrawTile(&me->tiVArrows, piDisplay, x, y, 0);
   }

   y = prc->y + prc->dy - me->tiVArrows.cyTile;
   if (rgbDown != RGB_NONE) {
      TiledImage_ReplaceColor(&me->tiVArrows, RGB_BLACK, rgbDown);
      TiledImage_DrawTile(&me->tiVArrows, piDisplay, x, y, 1);
   }

   if (bReduce) {
      INFLATERECT(prc, 0, -(MAX(nEndPad, me->tiVArrows.cyTile+1)));
   }
}

static void SbArrows_DrawHorzArrows(SbArrows *me, IDisplay *piDisplay, AEERect *prc, 
                                    RGBVAL rgbLeft, RGBVAL rgbRight, boolean bReduce,
                                    int nEndPad)
{
   int x, y;

   // ensure resource image is loaded
   if (!TILEDIMAGE_HASIMAGE(&me->tiHArrows) &&
       me->piShell &&
       0 != TiledImage_LoadResBitmap(&me->tiHArrows, me->piShell, "widgets.mif", IDB_HSBARROWS, 2)) {
      return;
   }

   x = prc->x;
   y = prc->y + ((prc->dy - me->tiHArrows.cyTile) / 2);
   if (rgbLeft != RGB_NONE) {
      TiledImage_ReplaceColor(&me->tiHArrows, RGB_BLACK, rgbLeft);
      TiledImage_DrawTile(&me->tiHArrows, piDisplay, x, y, 0);
   }
   
   x = prc->x + prc->dx - me->tiHArrows.cxTile;
   if (rgbRight != RGB_NONE) {
      TiledImage_ReplaceColor(&me->tiHArrows, RGB_BLACK, rgbRight);
      TiledImage_DrawTile(&me->tiHArrows, piDisplay, x, y, 1);
   }

   if (bReduce) {
      INFLATERECT(prc, -(MAX(nEndPad, me->tiHArrows.cxTile+1)), 0);
   }
}




/////////////////////////////////////////////////////////////////////////////
//


static __inline boolean ScrollbarWidget_IsVisible(ScrollbarWidget *me, boolean bHorz)
{
   if (me->dwFlags & SBWF_NOAUTOHIDE) {
      return 1;
   } else {
      int nn = bHorz ? SCROLL_HORZ : SCROLL_VERT;
      return (me->scroll[nn].range > me->scroll[nn].visible);
   }
}



static __inline void ScrollbarWidget_GetChildRect(IDecorator *po, int x, int y, AEERect *prc)
{
   SCROLLBAR_FROM_DECORATOR;

   WExtent e;

   if (me->base.pChild) {
      Decorator_GetExtent(po, &e);
   } else {
      e.width = me->border.rcClient.dx;
      e.height = me->border.rcClient.dy;
   }
   SETAEERECT(prc, x + me->border.rcClient.x, y + me->border.rcClient.y, e.width, e.height);
}


static __inline int ScrollbarWidget_GetScrollbarWidth(ScrollbarWidget *me, boolean bHorz)
{
   int nWidth = 0;
   if (me->dwFlags & SBWF_ARROWS) {
      nWidth = bHorz ? me->arrows.tiHArrows.cyTile : me->arrows.tiVArrows.cxTile;
   }

   nWidth = MAX(SBWIDTH, nWidth);
   if (me->base.pChild) {
      nWidth += me->nPad;
   }

   return nWidth;
}


// Gets horizontal or vertical scrollbar rectangle in widget coordinates
static void ScrollbarWidget_GetScrollbarRect(ScrollbarWidget *me, boolean bHorz, AEERect *prc)
{
   AEERect rc;

   SETAEERECT(prc, 0, 0, 0, 0);

   if (ScrollbarWidget_IsVisible(me, bHorz)) {

      int nSBWidth = ScrollbarWidget_GetScrollbarWidth(me, bHorz);

      ScrollbarWidget_GetChildRect((IDecorator*)me, 0, 0, &rc);

      if (bHorz) {
         int nClient = MAX(me->border.rcClient.dy - nSBWidth + me->nPad, 0);
         SETAEERECT(prc, 
                    me->border.rcClient.x,
                    me->border.rcClient.y + nClient,
                    rc.dx,
                    me->nHandleWidth + (2*SBBORDER_WIDTH));
      } else {
         int nClient = MAX(me->border.rcClient.dx - nSBWidth + me->nPad, 0);
         SETAEERECT(prc, 
                    me->border.rcClient.x + nClient,
                    me->border.rcClient.y,
                    me->nHandleWidth + (2*SBBORDER_WIDTH),
                    rc.dy);
      }
   }
}

static void ScrollbarWidget_GetScrollHandleRects(ScrollbarWidget *me, boolean bHorz, 
                                                 AEERect *prcHandle, AEERect *prcFill)
{
   AEERect rc;
   AEERect rcFill;
   ScrollInfo *psi = &me->scroll[bHorz ? SCROLL_HORZ : SCROLL_VERT];

   ScrollbarWidget_GetScrollbarRect(me, bHorz, &rc);
   INFLATERECT(&rc, -SBBORDER_WIDTH, -SBBORDER_WIDTH);

   if (bHorz) {
      int nRows = 1;
      if (SUCCESS != IWIDGET_GetRows(SCROLLBARWIDGET_TO_IWIDGET(me),&nRows) ||
          nRows == 0){
         nRows = 1;
      }
 
      if(me->dwFlags & SBWF_ARROWS) {
         INFLATERECT(&rc, -(MAX(me->nEndPad, me->arrows.tiHArrows.cxTile+1)), 0);
      } else {
         INFLATERECT(&rc, -me->nEndPad, 0);
      }

      rcFill = rc;
      if (psi->range > psi->visible) {
         int dxMin = MIN(4, rc.dx);
         if((me->dwFlags & SBWF_FOCUSTRACK) && me->numItems > 0) {
            rc.x += (rc.dx * (me->focusIndex/nRows)) / 
                             ((me->numItems+nRows-1)/nRows);
            rc.dx = rc.dx / ((me->numItems+nRows-1)/nRows);
         }
         else {
            rc.x += ScrollbarWidget_ComputePosition(rc.dx, psi->position, psi->range, psi->visible);
            ScrollbarWidget_ComputeSize(rc.dx, psi->range, psi->visible);
         }
         rc.dx = MAX(dxMin, rc.dx);
      } else {
         if((me->dwFlags & SBWF_FOCUSTRACK) && me->numItems > 0) {
            int dxMin = MIN(4, rc.dx);

            rc.x += (rc.dx * (me->focusIndex/nRows)) / 
                             ((me->numItems+nRows-1)/nRows);
            rc.dx = rc.dx / ((me->numItems+nRows-1)/nRows);

            rc.dx = MAX(dxMin, rc.dx);
         } else {
            rc.dx = 0;
         }
      }
   } else {
      int nCols = 1;
      if (SUCCESS != IWIDGET_GetCols(SCROLLBARWIDGET_TO_IWIDGET(me),&nCols) ||
          nCols == 0){
         nCols = 1;
      }
      if(me->dwFlags & SBWF_ARROWS) {
         INFLATERECT(&rc, 0, -(MAX(me->nEndPad, me->arrows.tiVArrows.cyTile+1)));
      } else {
         INFLATERECT(&rc, 0, -me->nEndPad);
      }
      rcFill = rc;
      if (psi->range > psi->visible) {
         int dyMin = MIN(4, rc.dy);
         if((me->dwFlags & SBWF_FOCUSTRACK) && me->numItems > 0) {
            rc.y += (rc.dy * (me->focusIndex/nCols)) / 
                             ((me->numItems+nCols-1)/nCols);
            rc.dy = rc.dy / ((me->numItems+nCols-1)/nCols);
         }
         else {
            rc.y += ScrollbarWidget_ComputePosition(rc.dy, psi->position, psi->range, psi->visible);
            rc.dy = ScrollbarWidget_ComputeSize(rc.dy, psi->range, psi->visible);
         }
         rc.dy = MAX(dyMin, rc.dy);
      } else {
         if((me->dwFlags & SBWF_FOCUSTRACK) && me->numItems > 0) {
            int dyMin = MIN(4, rc.dy);

            rc.y += (rc.dy * (me->focusIndex/nCols)) / 
                             ((me->numItems+nCols-1)/nCols);
            rc.dy = rc.dy / ((me->numItems+nCols-1)/nCols);
 

            rc.dy = MAX(dyMin, rc.dy);
         } else {
            rc.dy = 0;
         }
      }
   }

   if (prcHandle) {
      *prcHandle = rc;
   }

   if (prcFill) {
      *prcFill = rcFill;
   }
}


static __inline void ScrollbarWidget_CalcExtent(IDecorator *po)
{
   SCROLLBAR_FROM_DECORATOR;
   ScrollbarWidget_SetExtent(po, &me->base.base.extent);
}


/////////////////////////////////////////////////////////////////////////////
// IWidget methods


uint32 ScrollbarWidget_Release(IDecorator *po)
{
   SCROLLBAR_FROM_DECORATOR;

   if (me->base.base.nRefs == 1) {
      // specific cleanup
      SbArrows_Dtor(&me->arrows);
      LISTENER_Cancel(&me->scrollListener);
      Border_Dtor(&me->border);
   }

   return Decorator_Release(po);
}

#if 1
void ScrollbarWidget_GetPreferredExtent(IDecorator *po, WExtent *pweOut)
{
   SCROLLBAR_FROM_DECORATOR;
   
   WExtent we;

   if (me->base.pChild) {
      Decorator_GetPreferredExtent(po, &we);

      if (me->nScrollStyle == SCROLL_VERT || me->nScrollStyle == SCROLL_BOTH) {
         if (ScrollbarWidget_IsVisible(me, SCROLL_VERT)) {
            we.width += ScrollbarWidget_GetScrollbarWidth(me, 0);
         }
      }
      if (me->nScrollStyle == SCROLL_HORZ || me->nScrollStyle == SCROLL_BOTH) {
         if (ScrollbarWidget_IsVisible(me, SCROLL_HORZ)) {
            we.height += ScrollbarWidget_GetScrollbarWidth(me, 1);
         }
      }

   } else {

      if (me->nScrollStyle == SCROLL_VERT) {
         we.width = ScrollbarWidget_GetScrollbarWidth(me, 0);
         we.height = 20;
      } else {
         we.width  = 20;
         we.height = ScrollbarWidget_GetScrollbarWidth(me, 0);
      }
   }

   Border_CalcPreferredExtent(&me->border, pweOut, &we);
}
#else

void ScrollbarWidget_GetPreferredExtent(IDecorator *po, WExtent *pweOut)
{
   SCROLLBAR_FROM_DECORATOR;
   
   WExtent we;

   if (me->base.pChild) {
      Decorator_GetPreferredExtent(po, &we);

      if (me->nScrollStyle == SCROLL_VERT || me->nScrollStyle == SCROLL_BOTH) {
         if (ScrollbarWidget_IsVisible(me, SCROLL_VERT)) {
            we.width += ScrollbarWidget_GetScrollbarWidth(me, 0);
         }
      }
      if (me->nScrollStyle == SCROLL_HORZ || me->nScrollStyle == SCROLL_BOTH) {
         if (ScrollbarWidget_IsVisible(me, SCROLL_HORZ)) {
            we.height += ScrollbarWidget_GetScrollbarWidth(me, 1);
         }
      }

      Border_CalcPreferredExtent(&me->border, pweOut, &we);

   } else {

      if (me->nScrollStyle == SCROLL_VERT) {
         we.width  = SBWIDTH;
         we.height = 20;
      } else {
         we.width  = 20;
         we.height = SBWIDTH;
      }

      Border_CalcPreferredExtent(&me->border, pweOut, &we);
   }
}

#endif

void ScrollbarWidget_GetExtent(IDecorator *po, WExtent *pe)
{
   WidgetBase_GetExtent(IDECORATOR_TO_IWIDGET(po), pe);
}


void ScrollbarWidget_SetExtent(IDecorator *po, WExtent *pe)
{
   SCROLLBAR_FROM_DECORATOR;

   WidgetBase_SetExtent((IWidget*)&me->base.base, pe);
   Border_CalcClientRect(&me->border);

   if (me->base.pChild) {

      WExtent we;

      we.width = me->border.rcClient.dx;
      we.height = me->border.rcClient.dy;
   
      if (me->nScrollStyle == SCROLL_VERT || me->nScrollStyle == SCROLL_BOTH) {

         if (ScrollbarWidget_IsVisible(me, SCROLL_VERT)) {
            we.width = MAX(0, we.width-ScrollbarWidget_GetScrollbarWidth(me, 0));
         }
      }
      if (me->nScrollStyle == SCROLL_HORZ || me->nScrollStyle == SCROLL_BOTH) {

         if (ScrollbarWidget_IsVisible(me, SCROLL_HORZ)) {
            we.height = MAX(0, we.height-ScrollbarWidget_GetScrollbarWidth(me, 1));
         }
      }
   
      Decorator_SetExtent(po, &we);
   }
}


void ScrollbarWidget_SetWidget(IDecorator *po, IWidget *piChild)
{
   SCROLLBAR_FROM_DECORATOR;
   WExtent we;

   Decorator_SetWidget(po, piChild);
   
   // set our (decorator) extent from the child's extent
   // so that the child extent will not change
   Decorator_GetExtent(po, &we);

   if (me->nScrollStyle == SCROLL_HORZ || me->nScrollStyle == SCROLL_BOTH) {
      we.height += ScrollbarWidget_GetScrollbarWidth(me, 0);
   }
   if (me->nScrollStyle == SCROLL_VERT || me->nScrollStyle == SCROLL_BOTH) {
      we.width += ScrollbarWidget_GetScrollbarWidth(me, 1);
   }
   Border_CalcPreferredExtent(&me->border, &we, &we);
   ScrollbarWidget_SetExtent(po, &we);

   if (piChild) {
      // connect the child widget's view model to the decorator
      IModel *piModel = 0;
      IWIDGET_GetViewModel(piChild, &piModel); 
      IWIDGET_SetModel(IDECORATOR_TO_IWIDGET(po), piModel);
      RELEASEIF(piModel);
   }

}


int ScrollbarWidget_SetModel(IDecorator *po, IModel *piModel)
{
   SCROLLBAR_FROM_DECORATOR;

   WidgetBase_SetModel((IWidget*)po, piModel);
   IMODEL_AddListener(piModel, &me->scrollListener);

   return SUCCESS;
}


void ScrollbarWidget_Draw(IDecorator *po, ICanvas *piCanvas, int x, int y)
{
   SCROLLBAR_FROM_DECORATOR;
   AEERect rc;
   IDisplay *piDisplay = 0;

   RGBVAL rgbBorder = me->rgbBorder[me->border.bActive];

   Border_Draw(&me->border, piCanvas, x, y);

   if (SUCCESS != ICANVAS_GetDisplay(piCanvas, &piDisplay)) {
      return;
   }

   if (me->nScrollStyle == SCROLL_VERT || me->nScrollStyle == SCROLL_BOTH) {

      // Get scrollbar rect            
      ScrollbarWidget_GetScrollbarRect(me, 0, &rc);

      if (rc.dx && rc.dy) {
         ScrollInfo *psi = &me->scroll[SCROLL_VERT];
   
         // inset by drawing location
         rc.x += x;
         rc.y += y;
   
         if (me->dwFlags & SBWF_ARROWS) {
            SbArrows_DrawVertArrows(&me->arrows, piDisplay, &rc, rgbBorder, rgbBorder, 1, 
                                    me->nEndPad);
         } else {
            INFLATERECT(&rc, 0, -me->nEndPad);
         }

   
         if (rgbBorder != RGBA_NONE) {
            DrawAndInsetBorder(piDisplay, &rc, SBBORDER_WIDTH, rgbBorder);
         }
      
         IDISPLAY_DrawRect(piDisplay, &rc, RGB_NONE, RGBA_STRIPALPHA(me->rgbFill), IDF_RECT_FILL);
   
         if (psi->range > psi->visible) {
            int dyMin = MIN(4, rc.dy);
            int yMax = rc.y + rc.dy;

            if((me->dwFlags & SBWF_FOCUSTRACK) && me->numItems > 0) {
               rc.y += (rc.dy * me->focusIndex) / me->numItems;
               rc.dy = rc.dy / me->numItems;
            }
            else {
               rc.y += ScrollbarWidget_ComputePosition(rc.dy, psi->position, psi->range, psi->visible);
               rc.dy = ScrollbarWidget_ComputeSize(rc.dy, psi->range, psi->visible);
            }

            rc.dy = MAX(dyMin, rc.dy);
            rc.y = MIN(rc.y, yMax - rc.dy);
   
            IDISPLAY_DrawRect(piDisplay, &rc, RGB_NONE, RGBA_STRIPALPHA(me->rgbHandle[me->border.bActive]), IDF_RECT_FILL);
         }
      }
   }

   if (me->nScrollStyle == SCROLL_HORZ || me->nScrollStyle == SCROLL_BOTH) {

      ScrollbarWidget_GetScrollbarRect(me, 1, &rc);

      if (rc.dx && rc.dy) {
         ScrollInfo *psi = &me->scroll[SCROLL_HORZ];
   
         // Get scrollbar rect 
         // inset by drawing location
         rc.x += x;
         rc.y += y;
   
         if (me->dwFlags & SBWF_ARROWS) {
            SbArrows_DrawHorzArrows(&me->arrows, piDisplay, &rc, rgbBorder, rgbBorder, 1,
                                    me->nEndPad);
         } else {
            INFLATERECT(&  rc, -me->nEndPad, 0);
         }
   
         if (rgbBorder != RGBA_NONE) {
            DrawAndInsetBorder(piDisplay, &rc, SBBORDER_WIDTH, rgbBorder);
         }
      
         IDISPLAY_DrawRect(piDisplay, &rc, RGB_NONE, RGBA_STRIPALPHA(me->rgbFill), IDF_RECT_FILL);
   
         if (psi->range > psi->visible) {
            int dxMin = MIN(4, rc.dx);

            if((me->dwFlags & SBWF_FOCUSTRACK) && me->numItems > 0) {
               rc.x += (rc.dx * me->focusIndex) / me->numItems;
               rc.dx = rc.dx / me->numItems;
            }
            else {
               rc.x += ScrollbarWidget_ComputePosition(rc.dx, psi->position, psi->range, psi->visible);
               rc.dx = ScrollbarWidget_ComputeSize(rc.dx, psi->range, psi->visible);
            }
   
            rc.dx = MAX(dxMin, rc.dx);

            IDISPLAY_DrawRect(piDisplay, &rc, RGB_NONE, RGBA_STRIPALPHA(me->rgbHandle[me->border.bActive]), IDF_RECT_FILL);
         }
      }
   }

   RELEASEIF(piDisplay);

   Decorator_Draw(po, piCanvas, me->border.rcClient.x + x, me->border.rcClient.y + y);
}


static void ScrollbarWidget_InvalidateScrollbar(ScrollbarWidget *me, boolean bHorz)
{
   int style = bHorz ? SCROLL_HORZ : SCROLL_VERT;

   if (me->nScrollStyle == style || me->nScrollStyle == SCROLL_BOTH) {
      AEERect rc;
      ScrollbarWidget_GetScrollbarRect(me, bHorz, &rc);
      Decorator_Invalidate((IDecorator*)me, &rc, 0);
   }

}

static void ScrollbarWidget_onScrollEvent(IDecorator *po, ModelEvent *pEvent)
{
   SCROLLBAR_FROM_DECORATOR;
   boolean bVis;

   if (pEvent->evCode == EVT_MDL_SCROLL_CHANGE) {
      ScrollEvent *pScrollEvent = (ScrollEvent *)pEvent;
      int index = pScrollEvent->bVertical ? SCROLL_VERT : SCROLL_HORZ;

      bVis = ScrollbarWidget_IsVisible(me, (boolean)index);
      me->scroll[index].range     = pScrollEvent->range;
      me->scroll[index].visible   = pScrollEvent->visible;
      me->scroll[index].position  = MIN(pScrollEvent->position, pScrollEvent->range);
      ScrollbarWidget_InvalidateScrollbar(me, (boolean)index);

      // scrollbar visibility changed -- change the child extent
      if (bVis != ScrollbarWidget_IsVisible(me, (boolean)index)) {
         ScrollbarWidget_CalcExtent(po);
      }
   }
   else if((pEvent->evCode == EVT_MDL_FOCUS_CHANGE) &&
           (me->dwFlags & SBWF_FOCUSTRACK)) {
      FocusEvent *pFocusEvent = (FocusEvent *)pEvent;

      // grab the new focus index
      me->focusIndex = pFocusEvent->focusIndex;
      me->numItems = pFocusEvent->numItems;
      // invalidate both since the direction is undetermined
      if(me->nScrollStyle == SCROLL_VERT || me->nScrollStyle == SCROLL_BOTH) {
         bVis = ScrollbarWidget_IsVisible(me, (boolean)SCROLL_VERT);
         ScrollbarWidget_InvalidateScrollbar(me, (boolean)SCROLL_VERT);

         // scrollbar visibility changed -- change the child extent
         if (bVis != ScrollbarWidget_IsVisible(me, (boolean)SCROLL_VERT)) {
            ScrollbarWidget_CalcExtent(po);
         }
      }
      if(me->nScrollStyle == SCROLL_HORZ || me->nScrollStyle == SCROLL_BOTH) {
         bVis = ScrollbarWidget_IsVisible(me, (boolean)SCROLL_HORZ);
         ScrollbarWidget_InvalidateScrollbar(me, (boolean)SCROLL_HORZ);

         // scrollbar visibility changed -- change the child extent
         if (bVis != ScrollbarWidget_IsVisible(me, (boolean)SCROLL_HORZ)) {
            ScrollbarWidget_CalcExtent(po);
         }
      }
   }
} 


boolean ScrollbarWidget_HandleEvent(IDecorator *po, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   SCROLLBAR_FROM_DECORATOR;

   AEERect *prcClient = &me->border.rcClient;
   AEERect rcSave = *prcClient;

   // if we have a child, let it handle these focus events first
   if (me->base.pChild && 
       evt == EVT_WDG_HASFOCUS || evt == EVT_WDG_CANTAKEFOCUS) {
      return Decorator_HandleEvent(po, evt, wParam, dwParam);
   }
   
   if (Border_HandleEvent(&me->border, evt, wParam, dwParam)) {
      if (rcSave.dx != prcClient->dx ||
          rcSave.dy != prcClient->dy) {
         ScrollbarWidget_CalcExtent(po);
      }
      return TRUE;
   }

   switch(evt) {
      
      case EVT_WDG_SETPROPERTY:
         switch(wParam) {
         
            case PROP_IMAGESTRIP_H:
               TiledImage_SetBitmap(&me->arrows.tiHArrows, (IBitmap*)dwParam, 2);
               ScrollbarWidget_InvalidateScrollbar(me, 1);
               return TRUE;

            case PROP_IMAGESTRIP_V:
               TiledImage_SetBitmap(&me->arrows.tiVArrows, (IBitmap*)dwParam, 2);
               ScrollbarWidget_InvalidateScrollbar(me, 0);
               return TRUE;

            case PROP_FLAGS:
               me->dwFlags = dwParam;
               return TRUE;

            case PROP_SCROLLPAD:
               me->nPad = (int8)dwParam;
               ScrollbarWidget_CalcExtent(po);
               return TRUE;

            case PROP_SCROLLBARENDPAD:
               me->nEndPad = (int8)dwParam;
               ScrollbarWidget_CalcExtent(po);
               return TRUE;

            case PROP_SCROLLHANDLEWIDTH:
               me->nHandleWidth = (int8)dwParam;
               ScrollbarWidget_CalcExtent(po);
               return TRUE;
      
            case PROP_SCROLLSTYLE:
               me->nScrollStyle = (uint8)dwParam;
               goto sbinval;

            case PROP_SCROLLCOLOR:
               me->rgbBorder[RGBINDEX_INACTIVE] = (int)dwParam;
               // fall thru
            case PROP_ACTIVE_SCROLLCOLOR:
               me->rgbBorder[RGBINDEX_ACTIVE] = (int)dwParam;
               goto sbinval;
      
            case PROP_INACTIVE_SCROLLCOLOR:
               me->rgbBorder[RGBINDEX_INACTIVE] = (int)dwParam;
               goto sbinval;
      
            case PROP_SCROLLFILLCOLOR:
               me->rgbFill = (int)dwParam;
               goto sbinval;

            case PROP_SCROLLHANDLECOLOR:
               me->rgbHandle[RGBINDEX_INACTIVE] = (int)dwParam;
               // fall thru
            case PROP_ACTIVE_SCROLLHANDLECOLOR:
               me->rgbHandle[RGBINDEX_ACTIVE] = (int)dwParam;
               goto sbinval;
      
            case PROP_INACTIVE_SCROLLHANDLECOLOR:
               me->rgbHandle[RGBINDEX_INACTIVE] = (int)dwParam;
sbinval:
               ScrollbarWidget_InvalidateScrollbar(me, 0);
               ScrollbarWidget_InvalidateScrollbar(me, 1);
               return TRUE;

         }
         break;   // EVT_WDG_SETPROPERTY

      case EVT_WDG_GETPROPERTY:
         switch(wParam) {
            
            case PROP_FLAGS:
               *(uint32*)dwParam = me->dwFlags;
               return TRUE;

            case PROP_SCROLLPAD:
               *(int*)dwParam = (int)me->nPad;
               return TRUE;
      
            case PROP_SCROLLBARENDPAD:
               *(int*)dwParam = (int)me->nEndPad;
               return TRUE;

            case PROP_SCROLLHANDLEWIDTH:
               *(int*)dwParam = (int)me->nHandleWidth;
               return TRUE;
      
            case PROP_SCROLLSTYLE:
               *(int*)dwParam = (int)me->nScrollStyle;
               return TRUE;

            case PROP_SCROLLCOLOR:
            case PROP_ACTIVE_SCROLLCOLOR:
               *(RGBVAL *)dwParam = me->rgbBorder[RGBINDEX_ACTIVE];
               return TRUE;
      
            case PROP_INACTIVE_SCROLLCOLOR:
               *(RGBVAL *)dwParam = me->rgbBorder[RGBINDEX_INACTIVE];
               return TRUE;
      
            case PROP_SCROLLHANDLECOLOR:
            case PROP_ACTIVE_SCROLLHANDLECOLOR:
               *(RGBVAL *)dwParam = me->rgbHandle[RGBINDEX_ACTIVE];
               return TRUE;
      
            case PROP_INACTIVE_SCROLLHANDLECOLOR:
               *(RGBVAL *)dwParam = me->rgbHandle[RGBINDEX_INACTIVE];
               return TRUE;

            case PROP_SCROLLFILLCOLOR:
               *(RGBVAL *)dwParam = me->rgbFill;
               return TRUE;

            case PROP_SCROLLHANDLERECT:
               if (me->nScrollStyle == SCROLL_BOTH)
                  return FALSE;
               ScrollbarWidget_GetScrollHandleRects(me, (boolean)(me->nScrollStyle == SCROLL_HORZ),
                                                   (AEERect*)dwParam, 0);
               return TRUE;

            case PROP_VSCROLLHANDLERECT:
            case PROP_HSCROLLHANDLERECT:
               if (me->nScrollStyle != SCROLL_BOTH)
                  return FALSE;
               ScrollbarWidget_GetScrollHandleRects(me, (boolean)(wParam == PROP_HSCROLLHANDLERECT), 
                                                   (AEERect*)dwParam, 0);
               return TRUE;

            case PROP_SCROLLFILLRECT:
               if (me->nScrollStyle == SCROLL_BOTH)
                  return FALSE;
               ScrollbarWidget_GetScrollHandleRects(me, (boolean)(me->nScrollStyle == SCROLL_HORZ),
                                                    0, (AEERect*)dwParam);
               return TRUE;

            case PROP_VSCROLLFILLRECT:
            case PROP_HSCROLLFILLRECT:
               if (me->nScrollStyle != SCROLL_BOTH)
                  return FALSE;
               ScrollbarWidget_GetScrollHandleRects(me, (boolean)(wParam == PROP_HSCROLLFILLRECT), 
                                                    0, (AEERect*)dwParam);
               return TRUE;

            case PROP_SCROLLBARRECT:
               if (me->nScrollStyle == SCROLL_BOTH)
                  return FALSE;
               ScrollbarWidget_GetScrollbarRect(me, (boolean)(me->nScrollStyle == SCROLL_HORZ), 
                                                (AEERect*)dwParam);
               return TRUE;

            case PROP_VSCROLLBARRECT:
            case PROP_HSCROLLBARRECT:
               if (me->nScrollStyle != SCROLL_BOTH)
                  return FALSE;
               ScrollbarWidget_GetScrollbarRect(me, (boolean)(wParam == PROP_HSCROLLBARRECT),
                                                (AEERect*)dwParam);
               return TRUE;

         }
         break;   // EVT_WDG_GETPROPERTY
   }
      
   return Decorator_HandleEvent(po, evt, wParam, dwParam);
}


/////////////////////////////////////////////////////////////////
// Container methods


void ScrollbarWidget_Invalidate(IContainer *po, IWidget *piw, const AEERect *prcIn, uint32 dwFlags)
{
   SCROLLBAR_FROM_CONTAINER;
   Decorator_ClientInvalidate((IDecorator*)me, &me->border.rcClient, prcIn, dwFlags);
}

int ScrollbarWidget_Locate(IContainer *po, IWidget *piw, IContainer **ppRoot, AEERect *prcRel)
{
   SCROLLBAR_FROM_CONTAINER;
   return Decorator_ClientLocate((IDecorator*)me, &me->border.rcClient, piw, ppRoot, prcRel);
}

/////////////////////////////////////////////////////////////////

void ScrollbarWidget_Ctor(ScrollbarWidget *me, AEEVTBL(IDecorator) *pvt, IShell *piShell, 
                          IModule *piModule, PFNHANDLER pfnDefHandler)
{
   Decorator_Ctor(&me->base, pvt, piModule, DEFHANDLER(ScrollbarWidget_HandleEvent));

   pvt->Release            = ScrollbarWidget_Release;
   pvt->GetPreferredExtent = ScrollbarWidget_GetPreferredExtent;
   pvt->GetExtent          = ScrollbarWidget_GetExtent;
   pvt->SetExtent          = ScrollbarWidget_SetExtent;
   pvt->Draw               = ScrollbarWidget_Draw;
   pvt->SetWidget          = ScrollbarWidget_SetWidget;
   pvt->SetModel           = ScrollbarWidget_SetModel;
   
   me->base.vtContainer.Invalidate  = ScrollbarWidget_Invalidate;
   me->base.vtContainer.Locate      = ScrollbarWidget_Locate;
   

   Border_Ctor(&me->border, (PFNINVALIDATE)WidgetBase_Invalidate, me, &me->base.base.extent, FALSE, &WBASE(me)->piViewModel);
   me->border.nWidth[0] =
   me->border.nWidth[1] = 0;
   Border_SetBGColor(&me->border, 0, RGBA_NONE);

   me->rgbBorder[RGBINDEX_ACTIVE]   =
   me->rgbBorder[RGBINDEX_INACTIVE] = DEFAULT_SCROLLBORDER_COLOR; 
   
   me->rgbHandle[RGBINDEX_ACTIVE]   = 
   me->rgbHandle[RGBINDEX_INACTIVE] = DEFAULT_SCROLLHANDLE_COLOR; 

   me->rgbFill = RGB_WHITE;

   me->nHandleWidth = 3;
   me->nPad = 2;
   me->nEndPad = 0;
   me->nScrollStyle = SCROLL_VERT;
   me->focusIndex = 0;
   me->numItems = 0;

   me->piShell = piShell;
   ISHELL_AddRef(piShell);

   SbArrows_Ctor(&me->arrows, piShell);

   LISTENER_Init(&me->scrollListener, ScrollbarWidget_onScrollEvent, 
                 SCROLLBARWIDGET_TO_IDECORATOR(me));

   SETWEXTENT(&me->base.base.extent, 0, 0);

   TiledImage_LoadResBitmap(&me->arrows.tiVArrows, me->piShell, "widgets.mif", IDB_VSBARROWS, 2);
   TiledImage_LoadResBitmap(&me->arrows.tiHArrows, me->piShell, "widgets.mif", IDB_HSBARROWS, 2);
}

int ScrollbarWidget_New(IDecorator **ppo, IShell *piShell, IModule *piModule)
{
   ScrollbarWidget *me = MALLOCREC_VTBL(ScrollbarWidget, IDecorator);
   if (!me)
      return ENOMEMORY;

   ScrollbarWidget_Ctor(me, GETVTBL(me,IDecorator), piShell, piModule, 0);
   *ppo = SCROLLBARWIDGET_TO_IDECORATOR(me);

   return 0;
}


/////////////////////////////////////////////////////////////////
// Utility functions

int IWidget_WrapInScrollbar(IWidget **ppioChild, IShell *piShell, IModule *piModule)
{
   int nErr;
   IDecorator *piDecorator = NULL;
   
   nErr = ScrollbarWidget_New(&piDecorator, piShell, piModule);

   if (nErr == SUCCESS) {
      IDECORATOR_SetWidget(piDecorator, *ppioChild);
      IWIDGET_Release(*ppioChild);
      *ppioChild = IDECORATOR_TO_IWIDGET(piDecorator);
   }

   return nErr;
}


/////////////////////////////////////////////////////////////////
// ScrollIndicatorWidget

#define DYEXTRA   (me->arrows.tiVArrows.cyTile + me->nPad)
#define DXEXTRA   (me->arrows.tiHArrows.cxTile + me->nPad)

static __inline void ScrollIndicatorWidget_GetChildRect(IDecorator *po, int x, int y, AEERect *prc)
{
   SCROLLIND_FROM_DECORATOR;

   WExtent e;

   if (me->base.pChild) {
      Decorator_GetExtent(po, &e);
   } else {
      e.width = me->border.rcClient.dx;
      e.height = me->border.rcClient.dy;
   }

   SETAEERECT(prc, x + me->border.rcClient.x, y + me->border.rcClient.y, e.width, e.height);

   if (me->nScrollStyle == SCROLL_VERT || me->nScrollStyle == SCROLL_BOTH) {
      prc->y += DYEXTRA;
   }
   if (me->nScrollStyle == SCROLL_HORZ || me->nScrollStyle == SCROLL_BOTH) {
      prc->x += DXEXTRA;
   }

}


static void ScrollIndicatorWidget_OnScrollEvent(IDecorator *po, ScrollEvent *pEvent)
{
   if (pEvent->base.evCode == EVT_MDL_SCROLL_CHANGE) {

      SCROLLIND_FROM_DECORATOR;
      
      int index = pEvent->bVertical ? SCROLL_VERT : SCROLL_HORZ;
   
      me->scroll[index].range     = pEvent->range;
      me->scroll[index].visible   = pEvent->visible;
      me->scroll[index].position  = pEvent->position;
      
      if (me->nScrollStyle == SCROLL_BOTH || index == me->nScrollStyle) {
         Decorator_Invalidate(po, &me->border.rcClient, 0);
      }
   }
} 



void ScrollIndicatorWidget_GetPreferredExtent(IDecorator *po, WExtent *pweOut)
{
   SCROLLIND_FROM_DECORATOR;

   WExtent we;

   if (me->base.pChild) {
      Decorator_GetPreferredExtent(po, &we);

      if (me->nScrollStyle == SCROLL_VERT || me->nScrollStyle == SCROLL_BOTH) {
         we.width += (DXEXTRA * 2);
      }
      if (me->nScrollStyle == SCROLL_HORZ || me->nScrollStyle == SCROLL_BOTH) {
         we.height += (DYEXTRA * 2);
      }
   
   } else {

      if (me->nScrollStyle == SCROLL_VERT) {
         we.height = (me->arrows.tiVArrows.cyTile * 2) + me->nPad;
         we.width = me->arrows.tiVArrows.cxTile;

      } else if (me->nScrollStyle == SCROLL_HORZ) {
         we.height = me->arrows.tiHArrows.cyTile;
         we.width = (me->arrows.tiHArrows.cxTile * 2) + me->nPad;

      } else {
         we.height = (me->arrows.tiVArrows.cyTile * 2) + me->nPad;
         we.width = (me->arrows.tiHArrows.cxTile + (me->arrows.tiHArrows.cxTile * 2) + (me->nPad * 2));

      }
   }
   
   Border_CalcPreferredExtent(&me->border, pweOut, &we);
}

void ScrollIndicatorWidget_GetExtent(IDecorator *po, WExtent *pe)
{
   WidgetBase_GetExtent(IDECORATOR_TO_IWIDGET(po), pe);              
}

void ScrollIndicatorWidget_SetExtent(IDecorator *po, WExtent *pe)
{
   SCROLLIND_FROM_DECORATOR;
   WExtent we;

   WidgetBase_SetExtent((IWidget*)&me->base.base, pe);
   Border_CalcClientRect(&me->border);

   we.width = me->border.rcClient.dx;
   we.height = me->border.rcClient.dy;

   if (me->nScrollStyle == SCROLL_VERT || me->nScrollStyle == SCROLL_BOTH) {
      we.height = MAX(0, we.height - (DYEXTRA * 2));
   }
   if (me->nScrollStyle == SCROLL_HORZ || me->nScrollStyle == SCROLL_BOTH) {
      we.width = MAX(0, we.width - (DXEXTRA * 2));
   }

   Decorator_SetExtent(po, &we);
}


void ScrollIndicatorWidget_SetWidget(IDecorator *po, IWidget *piChild)
{
   SCROLLIND_FROM_DECORATOR;
   WExtent we;
   WExtent weBorder;

   Decorator_SetWidget(po, piChild);
   
   // set our (decorator) extent from the child's extent
   // so that the child extent will not change
   Decorator_GetExtent(po, &we);

   if (me->nScrollStyle == SCROLL_VERT ||  me->nScrollStyle == SCROLL_BOTH) {
      we.height += (DYEXTRA * 2);
   }
   if (me->nScrollStyle == SCROLL_HORZ || me->nScrollStyle == SCROLL_BOTH) {
      we.width += (DXEXTRA * 2);
   }

   Border_CalcPreferredExtent(&me->border, &weBorder, &we);
   ScrollIndicatorWidget_SetExtent(po, &weBorder);

   if (piChild) {
      IModel *piModel;
      IWIDGET_GetViewModel(piChild, &piModel);
      IWIDGET_SetModel(IDECORATOR_TO_IWIDGET(po), piModel);
      IMODEL_Release(piModel);
   }

}

int ScrollIndicatorWidget_SetModel(IDecorator *po, IModel *piModel)
{
   SCROLLIND_FROM_DECORATOR;

   WidgetBase_SetModel((IWidget*)po, piModel);
   IMODEL_AddListener(piModel, &me->scrollListener);

   return SUCCESS;
}



static void ScrollIndicatorWidget_DrawArrows(ScrollIndicatorWidget *me, IDisplay *piDisplay, AEERect *prc)
{
   RGBVAL rgbBack, rgbForward;
   ScrollInfo *psi;
   uint16 wSBPos, wSBSize;
   boolean bBScrollable, bFScrollable;

   if (me->nScrollStyle == SCROLL_VERT || me->nScrollStyle == SCROLL_BOTH) {
      
      psi = &me->scroll[SCROLL_VERT];

      // determine size and position of scrollbar on screen
      wSBPos = ScrollbarWidget_ComputePosition(prc->dy, psi->position, psi->range, psi->visible);
      wSBSize = ScrollbarWidget_ComputeSize(prc->dy, psi->range, psi->visible);

      // this widget is scrollable if the scrollbar is not at either end
      bBScrollable = (psi->range > 0 && psi->position > 0);
      bFScrollable = (psi->range > 0 && (wSBPos + wSBSize) < prc->dy);

      // dim arrows pointing in directions we can't scroll
      rgbBack = bBScrollable  ? me->rgbIndicator[RGBINDEX_ACTIVE] 
                            : me->rgbIndicator[RGBINDEX_INACTIVE];

      rgbForward = bFScrollable ? me->rgbIndicator[RGBINDEX_ACTIVE] 
                             : me->rgbIndicator[RGBINDEX_INACTIVE];

      SbArrows_DrawVertArrows(&me->arrows, piDisplay, prc, rgbBack, rgbForward, 0, 0);
   }

   if (me->nScrollStyle == SCROLL_HORZ || me->nScrollStyle == SCROLL_BOTH) {

      psi = &me->scroll[SCROLL_HORZ];
      
      // determine size and position of scrollbar on screen
      wSBPos = ScrollbarWidget_ComputePosition(prc->dx, psi->position, psi->range, psi->visible);
      wSBSize = ScrollbarWidget_ComputeSize(prc->dx, psi->range, psi->visible);

      // this widget is scrollable if the scrollbar is not at either end
      bBScrollable = (psi->range > 0 && psi->position > 0);
      bFScrollable = (psi->range > 0 && (wSBPos + wSBSize) < prc->dx);
 
      // dim arrows pointing in directions we can't scroll
      rgbBack = bBScrollable  ? me->rgbIndicator[RGBINDEX_ACTIVE] 
                              : me->rgbIndicator[RGBINDEX_INACTIVE];

      rgbForward = bFScrollable ? me->rgbIndicator[RGBINDEX_ACTIVE] 
                              : me->rgbIndicator[RGBINDEX_INACTIVE];

      SbArrows_DrawHorzArrows(&me->arrows, piDisplay, prc, rgbBack, rgbForward, 0, 0);
   }
}


void ScrollIndicatorWidget_Draw(IDecorator *po, ICanvas *piCanvas, int x, int y)
{
   SCROLLIND_FROM_DECORATOR;
   IDisplay *piDisplay = 0;
   AEERect rc;

   Border_Draw(&me->border, piCanvas, x, y);

   // Get scroll rect 
   rc = me->border.rcClient;
   // inset by drawing location
   rc.x += x;
   rc.y += y;

   if (SUCCESS == ICANVAS_GetDisplay(piCanvas, &piDisplay)) {
      ScrollIndicatorWidget_DrawArrows(me, piDisplay, &rc);
      IDISPLAY_Release(piDisplay);
   }

   ScrollIndicatorWidget_GetChildRect(po, x, y, &rc);

   Decorator_Draw(po, piCanvas, rc.x, rc.y);
}


boolean ScrollIndicatorWidget_HandleEvent(IDecorator *po, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   SCROLLIND_FROM_DECORATOR;

   AEERect *prcClient = &me->border.rcClient;
   AEERect rcSave = *prcClient;

   // if we have a child, let it handle these focus events first
   if (me->base.pChild && 
       evt == EVT_WDG_HASFOCUS || evt == EVT_WDG_CANTAKEFOCUS) {
      return Decorator_HandleEvent(po, evt, wParam, dwParam);
   }

   if (Border_HandleEvent(&me->border, evt, wParam, dwParam)) {
      if (rcSave.dx != prcClient->dx ||
          rcSave.dy != prcClient->dy) {
         IWIDGET_SetExtent(IDECORATOR_TO_IWIDGET(po), &WBASE(me)->extent);
      }
      return TRUE;
   }

   switch(evt) {
      
      case EVT_WDG_SETPROPERTY:
         switch(wParam) {
            
            case PROP_IMAGESTRIP_H:
               TiledImage_SetBitmap(&me->arrows.tiHArrows, (IBitmap*)dwParam, 2);
               Decorator_Invalidate(po, &me->border.rcClient, 0);
               return TRUE;
            
            case PROP_IMAGESTRIP_V:
               TiledImage_SetBitmap(&me->arrows.tiVArrows, (IBitmap*)dwParam, 2);
               Decorator_Invalidate(po, &me->border.rcClient, 0);
               return TRUE;

            case PROP_SCROLLPAD:
               me->nPad = (int)dwParam;
               ScrollIndicatorWidget_SetExtent(po, &me->base.base.extent);
               return TRUE;
         
            case PROP_SCROLLSTYLE: {
               uint8 style = (uint8)dwParam;
               if (style == SCROLL_HORZ || style == SCROLL_VERT || style == SCROLL_BOTH) {
                  me->nScrollStyle = style;
                  goto vsinval;
               }
            } return FALSE;

            case PROP_SCROLLCOLOR:
               me->rgbIndicator[RGBINDEX_INACTIVE] = (int)dwParam;
               // fall thru
            case PROP_ACTIVE_SCROLLCOLOR:
               me->rgbIndicator[RGBINDEX_ACTIVE] = (int)dwParam;
               goto vsinval;
      
            case PROP_INACTIVE_SCROLLCOLOR:
               me->rgbIndicator[RGBINDEX_INACTIVE] = (int)dwParam;
vsinval:
               Decorator_Invalidate(po, &me->border.rcClient, 0);
               return TRUE;
         }
         break;   // EVT_WDG_SETPROPERTY


      case EVT_WDG_GETPROPERTY:
         switch (wParam) {
            
            case PROP_SCROLLPAD:
               *(uint32*)dwParam = me->nPad;
               return TRUE;
         
            case PROP_SCROLLSTYLE:
               *(uint32*)dwParam = (int)me->nScrollStyle;
               return TRUE;

            case PROP_SCROLLCOLOR:
            case PROP_ACTIVE_SCROLLCOLOR:
               *(uint32*)dwParam = me->rgbIndicator[RGBINDEX_ACTIVE];
               return TRUE;
      
            case PROP_INACTIVE_SCROLLCOLOR:
               *( uint32*)dwParam = me->rgbIndicator[RGBINDEX_INACTIVE];
               return TRUE;
         }
         break;   // EVT_WDG_GETPROPERTY

   }
      
   return Decorator_HandleEvent(po, evt, wParam, dwParam);

}

uint32 ScrollIndicatorWidget_Release(IDecorator *po)
{
   SCROLLIND_FROM_DECORATOR;

   if (me->base.base.nRefs == 1) {
      // specific cleanup
      SbArrows_Dtor(&me->arrows);
      LISTENER_Cancel(&me->scrollListener);
      Border_Dtor(&me->border);
   }

   return Decorator_Release(po);
}


/////////////////////////////////////////////////////////////////
// Container methods


void ScrollIndicatorWidget_Invalidate(IContainer *po, IWidget *piw, const AEERect *prcIn, uint32 dwFlags)
{
   SCROLLIND_FROM_CONTAINER;
   AEERect rcChild;
   ScrollIndicatorWidget_GetChildRect((IDecorator*)me, 0, 0, &rcChild);
   Decorator_ClientInvalidate((IDecorator*)me, &rcChild, prcIn, dwFlags);
}

int ScrollIndicatorWidget_Locate(IContainer *po, IWidget *piw, IContainer **ppRoot, AEERect *prcRel)
{
   SCROLLIND_FROM_CONTAINER;
   return Decorator_ClientLocate((IDecorator*)me, &me->border.rcClient, piw, ppRoot, prcRel);
}

/////////////////////////////////////////////////////////////////


void ScrollIndicatorWidget_Ctor(ScrollIndicatorWidget *me, AEEVTBL(IDecorator) *pvt, 
                                IShell *piShell, IModule *piModule, PFNHANDLER pfnDefHandler)
{
   Decorator_Ctor(&me->base, pvt, piModule, DEFHANDLER(ScrollIndicatorWidget_HandleEvent));
   
   pvt->Release            = ScrollIndicatorWidget_Release;
   pvt->Draw               = ScrollIndicatorWidget_Draw;
   pvt->GetExtent          = ScrollIndicatorWidget_GetExtent;
   pvt->SetExtent          = ScrollIndicatorWidget_SetExtent; 
   pvt->SetWidget          = ScrollIndicatorWidget_SetWidget;
   pvt->GetPreferredExtent = ScrollIndicatorWidget_GetPreferredExtent;
   pvt->SetModel           = ScrollIndicatorWidget_SetModel;

   me->base.vtContainer.Invalidate  = ScrollIndicatorWidget_Invalidate;
   me->base.vtContainer.Locate      = ScrollIndicatorWidget_Locate;

   Border_Ctor(&me->border, (PFNINVALIDATE)WidgetBase_Invalidate, me, &me->base.base.extent, FALSE, &WBASE(me)->piViewModel);
   me->border.nWidth[0] =
   me->border.nWidth[1] = 0;
   Border_SetBorderColor(&me->border, 0, RGBA_NONE);

   me->rgbIndicator[RGBINDEX_ACTIVE]   = RGBA_WHITE;
   me->rgbIndicator[RGBINDEX_INACTIVE] = RGBA_NONE;

   me->nPad = 1;

   me->nScrollStyle = SCROLL_VERT;

   me->piShell = piShell;
   ISHELL_AddRef(piShell);

   SbArrows_Ctor(&me->arrows, piShell);

   LISTENER_Init(&me->scrollListener, ScrollIndicatorWidget_OnScrollEvent, (IWidget*)me);

   TiledImage_LoadResBitmap(&me->arrows.tiVArrows, me->piShell, "widgets.mif", IDB_VSBARROWS, 2);
   TiledImage_LoadResBitmap(&me->arrows.tiHArrows, me->piShell, "widgets.mif", IDB_HSBARROWS, 2);
}


int ScrollIndicatorWidget_New(IDecorator **ppo, IShell *piShell, IModule *piModule)
{
   ScrollIndicatorWidget *me = MALLOCREC_VTBL(ScrollIndicatorWidget, IDecorator);
   if (!me) {
      return ENOMEMORY;
   }

   ScrollIndicatorWidget_Ctor(me, GETVTBL(me,IDecorator), piShell, piModule, 0);
   *ppo = (IDecorator*)me;
   
   return SUCCESS;
}



/*
  ========================================================================

  FILE:  ListWidget.c
  
  SERVICES:  

  GENERAL DESCRIPTION: Implementation of the List, Pick and Grid widgets

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#include "AEEStdLib.h"
#include "AEEComUtil.h"
#include "AEEDisp.h"

#include "ListWidget.h"
#include "AEEWProperties.h"
#include "ModelBase.h"
#include "wutil.h"

#define ME_FROM_CONTAINER ListWidget *me = (ListWidget *)po->pMe
#define ME_FROM_DECORATOR ListWidget *me = (ListWidget *)po

#define RIGHT(prc)    ((prc)->x + (prc)->dx)
#define BOTTOM(prc)   ((prc)->y + (prc)->dy)

#define SETRECTEMPTY(prc)  ((prc)->dx = (prc)->dy = 0)

// FARFing
#define FARF_DRAW       0
#define FARF_OFFSET     0
#define FARF_SCROLL     0
#define FARF_TEST       0
#define FARF_ITEMDRAW   0


#ifndef FARF
#define FARF(x, p)  if (0 != FARF_##x) DBGPRINTF p 
#endif


#define DIVROUNDUP(a,b)    (((a) / (b)) + (((a) % (b)) != 0))

#define INSHORTRANGE(dw)    (((dw) <= 32767) && ((dw) >= -32768))

typedef struct {
   int   x;
   int   y;
   int   dx;
   int   dy;
} BigRect;

static __inline int SafeDiv(int a, int b) {
   return (b ? (a / b) : a);
}

static __inline int NZSafeDiv(int a, int b) {
   int div = SafeDiv(a,b);
   return div ? div : 1;
}


#define HASEXTENT(pex)   (((pex)->width > 0) && ((pex)->height > 0))


static __inline void ListWidget_NotifyFocusChange(ListWidget *me);
static __inline void ListWidget_NotifyFocusWrap(ListWidget *me);
static __inline void ListWidget_InvalidateExtent(IDecorator *po, uint32 dwFlags);
static void ListWidget_NotifyScroll(ListWidget *me);
static __inline void ListWidget_SetItemData(ListWidget *me, int index);


static __inline boolean IsVarHeight(ListWidget *me) {
  return ( (me->dwFlags & LWF_VARIABLEITEMSIZE) // variable-height flag
        && (NULL != me->idx.pfnIndexer)         // we must have an indexer for this to work
        && (me->nOrientation != LWO_GRID) );    // not valid for grids
}


// extract coordinates of an AEERect
#define RCRIGHT(prc)          (((prc)->x) + ((prc)->dx) - 1)
#define RCBOTTOM(prc)         (((prc)->y) + ((prc)->dy) - 1)
#define RCCOORD(prc,l,t,r,b)  (l)=(prc)->x,(t)=(prc)->y,(r)=RCRIGHT(prc),(b)=RCBOTTOM(prc)


static __inline void Decorator_SetExtentEx(IDecorator *po, int width, int height) {
   WExtent we;
   we.width  = width;
   we.height = height;
   Decorator_SetExtent(po, &we);
}


typedef boolean (*PFNINTERNALINDEXER) (ListWidget*, int nIndex, void*);

static void ListWidget_Indexer(ListWidget *me, int nStartIndex, int nCount, 
                               PFNINTERNALINDEXER pfn, void *pvContext);


static int ListWidget_MeasureSpan(ListWidget *me, uint32 indexStart, uint32 count);

static int ListWidget_IndexToLimitRev(ListWidget *me, uint32 indexStart, uint32 limit);

static boolean HandleEventGetPropertyEx(
   IDecorator* po,
   uint32      dwParam);

static boolean HandleEventSetPropertyEx(
   IDecorator* po,
   uint32      dwParam);

/////////////////////////////////////////////////////////////////////////////
//

static __inline int CalcVisible(int nWindow, int nItem)
{
   int val;
   
   if (!nItem) {
      nItem++;
   }
   
   val = DIVROUNDUP(nWindow, nItem);

   if (!val) {
      val++;
   }
   return val;
}

static __inline int VisibleRows(ListWidget *me)
{
   return CalcVisible(me->border.rcClient.dy, me->dyItem);
}

static __inline int VisibleCols(ListWidget *me)
{
   return NZSafeDiv(me->border.rcClient.dx, me->dxItem);
}

static int VisibleItems(ListWidget *me)
{
   int rows = VisibleRows(me);
   int cols = VisibleCols(me);

   return (me->nOrientation == LWO_VERTICAL)    ? rows :
          (me->nOrientation == LWO_HORIZONTAL)  ? cols : rows * cols;
}

static __inline int GetPartial(ListWidget *me)
{
   if (me->nOrientation == LWO_VERTICAL) {
      int dy = me->dyItem ? me->dyItem : 1;
      return me->border.rcClient.dy % dy;
   }

   if (me->nOrientation == LWO_HORIZONTAL) {
      int dx = me->dxItem ? me->dxItem : 1;
      return me->border.rcClient.dx % dx;
   }

   return 0;
}

static boolean ConstrainRect(AEERect *prcBound, AEERect *prc, int *pxOffset, int *pyOffset)
{
   int rcx = prc->x;
   int rcy = prc->y;

   if (RIGHT(prc) > RIGHT(prcBound)) {
      prc->x = RIGHT(prcBound) - prc->dx;
   
   } else if (prc->x < prcBound->x) {
      prc->x = prcBound->x;
   }

   if (BOTTOM(prc) > BOTTOM(prcBound)) {
      prc->y = BOTTOM(prcBound) - prc->dy;
   
   } else if (prc->y < prcBound->y) {
      prc->y = prcBound->y;
   }

   // return the offsets in the x & y directions
   if (pxOffset) {
      *pxOffset = prc->x - rcx;
   }
   if (pyOffset) {
      *pyOffset = prc->y - rcy;
   }

   return (rcx != prc->x) || (rcy != prc->y);
}

static boolean ConstrainBigRect(AEERect *prcBound, BigRect *prc, int *pxOffset, int *pyOffset)
{
   int rcx = prc->x;
   int rcy = prc->y;

   if (RIGHT(prc) > RIGHT(prcBound)) {
      prc->x = RIGHT(prcBound) - prc->dx;
   
   } else if (prc->x < prcBound->x) {
      prc->x = prcBound->x;
   }

   if (BOTTOM(prc) > BOTTOM(prcBound)) {
      prc->y = BOTTOM(prcBound) - prc->dy;
   
   } else if (prc->y < prcBound->y) {
      prc->y = prcBound->y;
   }

   // return the offsets in the x & y directions
   if (pxOffset) {
      *pxOffset = prc->x - rcx;
   }
   if (pyOffset) {
      *pyOffset = prc->y - rcy;
   }

   return (rcx != prc->x) || (rcy != prc->y);
}


static __inline int ListWidget_GetModelSize(ListWidget *me)
{
   return (me->piModel) ? ILISTMODEL_Size(me->piModel) : 0;
}

static int ListWidget_VCalcItemPos(ListWidget *me, int index)
{
   int span, pos;

   if (me->nVarTopIndex < index) {

      /* Case 1: where requested item (index) is below the top item -- measure
      ::   the span from top item to index, then add that to the top position
      */
      span = ListWidget_MeasureSpan(me, me->nVarTopIndex, index - me->nVarTopIndex);
      pos = me->nVarTopPos + span;
   
   } else {

      /* Case 2: measure span from item 0 to index, and 
      ::   use that as the new position
      */
      pos = ListWidget_MeasureSpan(me, 0, index);
   }

   return pos;
}


/* ListWidget_VCalcItemRect
   Calculate an item rectangle for variable list (items may be different sizes)
   
   Item rectangle will be in list coordinates or widget coordinates based on value
   of bWidgetCoord argument:
   
      bWidgetCoord == TRUE  --> prc contains the item rectangle in widget coordinates
      bWidgetCoord == FALSE --> prc contains the item rectangle in list coordinates
*/
static void ListWidget_VCalcItemRect(ListWidget *me, int index, BigRect *prc)
{
   int32 pos    = ListWidget_VCalcItemPos(me, index);
   int itemsize = ListWidget_MeasureSpan(me, index, 1);

   if (me->nOrientation == LWO_VERTICAL) {
      prc->x = me->border.rcClient.x;
      prc->y = pos;
      prc->dx = me->dxItem;
      prc->dy = itemsize;
   } else {
      prc->x = pos;
      prc->y = me->border.rcClient.y;
      prc->dx = itemsize;
      prc->dy = me->dyItem;
   }
}


/* ListWidget_NCalcItemRect
   Calculate an item rectangle for normal list (all items are the same size)
   
   Item rectangle will be in list or widget coordinates based on value
   of bWidgetCoord argument:
   
      bWidgetCoord == TRUE  --> prc contains the item rectangle in client coordinates
      bWidgetCoord == FALSE --> prc contains the item rectangle in list coordinates
*/
static void ListWidget_NCalcItemRect(ListWidget *me, int index, BigRect *prc)
{
   SETAEERECT(prc, 0,0,0,0);

   if (me->nOrientation == LWO_VERTICAL) {

      prc->x  = me->border.rcClient.x;
      prc->y  = index * me->dyItem;
      prc->dx = me->dxItem;
      prc->dy = me->dyItem;
      if (index == me->focusIndex && me->nSelItem > 0) {
         prc->dy = me->nSelItem;
      }

   } else if (me->nOrientation == LWO_HORIZONTAL) {

      prc->x  = index * me->dxItem;
      prc->y  = me->border.rcClient.y;
      prc->dx = me->dxItem;
      prc->dy = me->dyItem;
      if (index == me->focusIndex && me->nSelItem > 0) {
         prc->dx = me->nSelItem;
      }

   } else { // me->nOrientation == LWO_GRID
      
      int dx      = me->border.rcClient.dx;
      int dxItem  = me->dxItem;
      int cols    = NZSafeDiv(dx, dxItem);
      int xpad    = dx - (dxItem * cols);

      int dy      = me->border.rcClient.dy;
      int dyItem  = me->dyItem;
      int rows    = NZSafeDiv(dy, dyItem);
      int ypad    = dy - (dyItem * rows);

      // distribute total padding in space between columns
      dxItem += SafeDiv(xpad, cols-1);
      dyItem += SafeDiv(ypad, rows-1);

      prc->x  = (index % cols) * dxItem;
      prc->y  = (index / cols) * dyItem;
      prc->dx = me->dxItem;
      prc->dy = me->dyItem;
   }
}

static boolean ListWidget_CalcItemRect(ListWidget *me, int index, BigRect *prc)
{
   int modelSize = ListWidget_GetModelSize(me);
   if (0 == modelSize || index < 0 || index >= modelSize) {
      return 0;
   }

   if (IsVarHeight(me)) {
      ListWidget_VCalcItemRect(me, index, prc);
   } else {
      ListWidget_NCalcItemRect(me, index, prc);
   }

   return 1;
}


static __inline void ListWidget_BigItemRectToWidgetCoords(ListWidget *me, BigRect *prc) {
   
   if (me->nOrientation == LWO_VERTICAL || me->nOrientation == LWO_GRID) {
      prc->y += me->yOffset + me->border.rcClient.y;
   }
   if (me->nOrientation == LWO_HORIZONTAL ||  me->nOrientation == LWO_GRID) {
      prc->x += me->xOffset + me->border.rcClient.x;
   }
}


static
boolean ListWidget_ItemRectToWidgetCoords(ListWidget *me, BigRect *prcIn, AEERect *prcOut) {
   
   int   nTempX   = 0;
   int   nTempY   = 0;

   if (me->nOrientation == LWO_VERTICAL) {
      nTempY = prcIn->y + me->yOffset + me->border.rcClient.y;
      if (!INSHORTRANGE(prcIn->x) || !INSHORTRANGE(nTempY)) {
         return 0;
      }

      prcOut->x   = int32toint16(prcIn->x);
      prcOut->y   = int32toint16(nTempY);
      prcOut->dx  = int32toint16(prcIn->dx);
      prcOut->dy  = int32toint16(prcIn->dy);
   }
   if (me->nOrientation == LWO_HORIZONTAL) {
      nTempX = prcIn->x + me->xOffset + me->border.rcClient.x;
      if (!INSHORTRANGE(nTempX) || !INSHORTRANGE(prcIn->y)) {
         return 0;
      }

      prcOut->x   = int32toint16(nTempX);
      prcOut->y   = int32toint16(prcIn->y);
      prcOut->dx  = int32toint16(prcIn->dx);
      prcOut->dy  = int32toint16(prcIn->dy);
   }
   if (me->nOrientation == LWO_GRID) {
      nTempX = prcIn->x + me->xOffset + me->border.rcClient.x;
      nTempY = prcIn->y + me->yOffset + me->border.rcClient.y;
      if (!INSHORTRANGE(nTempX) || !INSHORTRANGE(nTempY)) {
         return 0;
      }

      prcOut->x   = int32toint16(nTempX);
      prcOut->y   = int32toint16(nTempY);
      prcOut->dx  = int32toint16(prcIn->dx);
      prcOut->dy  = int32toint16(prcIn->dy);
   }

   return 1;
}


static boolean ListWidget_IsVisible(ListWidget *me, int nIndex)
{
   BigRect rcItem;
   AEERect rcWidget, rc;
   if (ListWidget_CalcItemRect(me, nIndex, &rcItem) 
         && ListWidget_ItemRectToWidgetCoords(me, &rcItem, &rcWidget)) {
      return IntersectRect(&rc, &me->border.rcClient, &rcWidget);
   }
   return 0;
}

static boolean ListWidget_IsFullyVisible(ListWidget *me, int nIndex)
{
   BigRect rcItem;
   AEERect rcWidget;
   if (ListWidget_CalcItemRect(me, nIndex, &rcItem) 
         && ListWidget_ItemRectToWidgetCoords(me, &rcItem, &rcWidget)) {
      return !ConstrainRect(&me->border.rcClient, &rcWidget, 0, 0);
   }
   return 0;
}


static __inline boolean ListWidget_IsFocusVisible(ListWidget *me) {
   return ListWidget_IsFullyVisible(me, me->focusIndex);
}


static boolean ListWidget_EnsureVisible(ListWidget *me, int index)
{
   int x = 0;
   int y = 0;
   BigRect rcItem;

   if (HASEXTENT(&WBASE(me)->extent) 
         && ListWidget_CalcItemRect(me, index, &rcItem)) {

      boolean bVert = (me->nOrientation == LWO_VERTICAL);
      int32 pos = bVert ? rcItem.y : rcItem.x;

      ListWidget_BigItemRectToWidgetCoords(me, &rcItem);
      ConstrainBigRect(&me->border.rcClient, &rcItem, &x, &y);
   
      FARF(OFFSET, ("xOffset %d -> %d", me->xOffset, me->xOffset + x));
      FARF(OFFSET, ("yOffset %d -> %d", me->yOffset, me->yOffset + y));
   
      if (me->nOrientation == LWO_HORIZONTAL ||  me->nOrientation == LWO_GRID) {
         me->xOffset += x;
      }
      if (me->nOrientation == LWO_VERTICAL ||  me->nOrientation == LWO_GRID) {
         me->yOffset += y;
      }

      if ((x != 0 || y != 0) && IsVarHeight(me)) {
         
         // if positive move, index is above the client rect, so
         // new top index is the visible item
         if (x > 0 || y > 0) {
            me->nVarTopPos   = pos;
            me->nVarTopIndex = index;
         } else {

            int limit = bVert ? me->border.rcClient.dy : me->border.rcClient.dx;
            int nNewTopIndex = ListWidget_IndexToLimitRev(me, index, limit);
            int nSpan = ListWidget_MeasureSpan(me, nNewTopIndex, index-nNewTopIndex);
            me->nVarTopPos    = pos - nSpan;
            me->nVarTopIndex  = nNewTopIndex;
         }
         FARF(OFFSET, ("OFFSET EnsureVisible: TopIndex=%d, pos=%d", me->nVarTopIndex, me->nVarTopPos));
      }

   }

   return ((x != 0) || (y != 0));
}

static boolean ListWidget_MeasureSpanWork(ListWidget *me, int nIndex, int *pnSpan)
{
   *pnSpan += me->nVarItemSize;
   return 1;
}

/* note: count must always be positive
         returns span
*/
static int ListWidget_MeasureSpan(ListWidget *me, uint32 indexStart, uint32 count)
{
   int span = 0;
   ListWidget_Indexer(me, indexStart, count, (PFNINTERNALINDEXER)ListWidget_MeasureSpanWork, &span);
   return span;
}

typedef struct {
   int      span;
   int      index;
} IndexLimit;

static boolean ListWidget_IndexToLimitWork(ListWidget *me, int nIndex, IndexLimit *pil)
{
   pil->span -= me->nVarItemSize;
   pil->index = nIndex;
   return (pil->span > 0);
}

/* returns new index */
static int ListWidget_IndexToLimitRev(ListWidget *me, uint32 indexStart, uint32 limit)
{
   int size = ListWidget_GetModelSize(me);
   
   IndexLimit il;
   il.span = limit;
   il.index = indexStart;
   
   ListWidget_Indexer(me, indexStart, -size, (PFNINTERNALINDEXER)ListWidget_IndexToLimitWork, &il);
   return il.index;
}

static boolean ListWidget_VForceToTop(ListWidget *me, int index)
{
   int size = ListWidget_GetModelSize(me);
   if (size && HASEXTENT(&WBASE(me)->extent)) {
      ListWidget_EnsureVisible(me, size-1);
      ListWidget_EnsureVisible(me, index);
      return TRUE;
   }

   return FALSE;
}

static boolean ListWidget_NForceToTop(ListWidget *me, int index)
{
   int size = ListWidget_GetModelSize(me);
   int vis = VisibleItems(me);

   if (size && HASEXTENT(&WBASE(me)->extent)) {

      BigRect rc, rcMax;
      ListWidget_CalcItemRect(me, index, &rc);

      if (me->nOrientation == LWO_GRID) {
         int cols = VisibleCols(me);
         size = (size + (cols-1)) / cols * cols;
      }
      if ((size-vis) < 0) {
         ListWidget_CalcItemRect(me, 0, &rcMax);
      } else {
         ListWidget_CalcItemRect(me, (size-vis), &rcMax);
      }

      if (me->nOrientation == LWO_VERTICAL || me->nOrientation == LWO_GRID) {
         FARF(OFFSET, ("ForceToTop: yOffset %d -> %d", me->yOffset, -(MIN(rcMax.y, rc.y))));
         me->yOffset = -(MIN(rcMax.y, rc.y));
         return TRUE;
      
      } else if (me->nOrientation == LWO_HORIZONTAL) {
         me->xOffset = -(MIN(rcMax.x, rc.x));
         return TRUE;
      }
   }

   return FALSE;
}

static boolean ListWidget_ForceToTop(ListWidget *me, int index)
{
   return (IsVarHeight(me) ? ListWidget_VForceToTop(me, index) 
                           : ListWidget_NForceToTop(me, index));
}


static void ListWidget_SetTopAndFocusIndex(ListWidget *me, boolean bForceNotify)
{
   if (HASEXTENT(&WBASE(me)->extent)) {

      boolean bScroll = 0;

      if (me->topIndex > -1) {
         bScroll = ListWidget_ForceToTop(me, me->topIndex);
         // if scrolled, we may have to change the focus index
         if (!ListWidget_IsFocusVisible(me)) {
            int focusIndex = me->focusIndex;
            // move the focus to ensure that it's visible
            me->focusIndex = me->topIndex;
            if (focusIndex != me->focusIndex) {
               ListWidget_NotifyFocusChange(me);
            }
         }
         me->topIndex = -1;

      } else if (ListWidget_EnsureVisible(me, me->focusIndex)) {
         bScroll = 1;
      }

      if (bScroll || bForceNotify) {
         ListWidget_NotifyScroll(me);
         ListWidget_InvalidateExtent((IDecorator*)me, 0);
      }
   }
}



static __inline int ListWidget_GetTopIndex(ListWidget *me)
{
   if (me->nOrientation == LWO_VERTICAL) {
      return ABS(me->yOffset) / ((me->dyItem != 0) ? me->dyItem : 1);

   } else if (me->nOrientation == LWO_HORIZONTAL) {
      return ABS(me->xOffset) / ((me->dxItem != 0) ? me->dxItem : 1);

   } else {
      // me->nOrientation == LWO_GRID
      return (ABS(me->yOffset) / ((me->dyItem != 0) ? me->dyItem : 1)) * VisibleCols(me);
   }
}


/////////////////////////////////////////////////////////////////////////////
//

static void ListWidget_Notify(ListWidget *me, uint32 evCode)
{
   if (WBASE(me)->piViewModel) {
      ModelEvent ev;
      ev.evCode   = evCode;
      ev.dwParam  = (uint32)me->focusIndex;
      IMODEL_Notify(WBASE(me)->piViewModel, &ev);
   }
}


static __inline void ListWidget_NotifyFocus(ListWidget *me, uint32 evCode)
{
   if(WBASE(me)->piViewModel) {
      FocusEvent fe;

      fe.base.evCode = evCode;
      fe.base.dwParam = fe.focusIndex = me->focusIndex;
      fe.numItems = ListWidget_GetModelSize(me);

      IMODEL_Notify(WBASE(me)->piViewModel, (ModelEvent*)&fe);
   }
}

static __inline void ListWidget_NotifyFocusWrap(ListWidget *me) {
   ListWidget_NotifyFocus(me, EVT_MDL_FOCUS_WRAP);
}

static __inline void ListWidget_NotifyFocusChange(ListWidget *me) {
   ListWidget_NotifyFocus(me, EVT_MDL_FOCUS_CHANGE);
}


static __inline boolean ListWidget_NotifySelect(ListWidget *me)
{
   // notify selection through the focus model
   if ((me->dwFlags & LWF_NOSELECT) == 0) {
      ListWidget_Notify(me, EVT_MDL_FOCUS_SELECT);
      return TRUE;
   }
   
   return FALSE;
}


static __inline void ScaleScrollEvent(ScrollEvent *pse, int vis, int pos, int range)
{
   if (range & (int)0xFF000000) {
      pos   = (uint32)pos >> 16;
      range = (uint32)range >> 16;
   } else if (range & 0x00FF0000) {
      pos   = (uint32)pos >> 8;
      range = (uint32)range >> 8;
   }

   pse->visible   = int32touint16(vis);
   pse->position  = int32touint16(pos);
   pse->range     = int32touint16(range);
}


static __inline void ListWidget_RecalcRange(ListWidget *me)
{
   if (IsVarHeight(me)) {
      int size = ListWidget_GetModelSize(me);
      me->nVarRange = (size > 0) ? ListWidget_MeasureSpan(me, 0, size) : 0;
   } else {
      me->nVarRange = 0;
   }
}


static __inline int ListWidget_GetRange(ListWidget *me)
{
   if (IsVarHeight(me)) {
      if (!me->nVarRange) {
         ListWidget_RecalcRange(me);
      }
      return me->nVarRange;

   } else {
      int item = (me->nOrientation == LWO_VERTICAL) ? me->dyItem : me->dxItem;
      int size = ListWidget_GetModelSize(me);
      if (me->nSelItem > 0 && size > 0) {
         return (item * (size-1) + me->nSelItem);
      } else {
         return (item * size);
      }
   }
}

static void ListWidget_NotifyScroll(ListWidget *me)
{
   if (WBASE(me)->piViewModel) {

      ScrollEvent se;
      se.base.evCode  = EVT_MDL_SCROLL_CHANGE;
   
      if (me->nOrientation == LWO_VERTICAL) {
         ScaleScrollEvent(&se, me->border.rcClient.dy, ABS(me->yOffset), ListWidget_GetRange(me));
         se.bVertical    = 1;
      
      } else if (me->nOrientation == LWO_HORIZONTAL) {
         ScaleScrollEvent(&se, me->border.rcClient.dx, ABS(me->xOffset), ListWidget_GetRange(me));
         se.bVertical    = 0;
   
      } else {
   
         int size = ListWidget_GetModelSize(me);
         int vis  = VisibleItems(me);
         int cols = VisibleCols(me);
         
         if (!cols) {
            cols++;
         }
         ScaleScrollEvent(&se, DIVROUNDUP(vis, cols) * me->dyItem,
            ABS(me->yOffset), DIVROUNDUP(size, cols) * me->dyItem);
         se.bVertical = 1;
      }
   
      FARF(SCROLL, ("range= %d, visible= %d, position= %d", 
                     se.range,
                     se.visible,
                     se.position));
   
      IMODEL_Notify(WBASE(me)->piViewModel, (ModelEvent*)&se);
   }
}
                                                

static void ListWidget_NotifyView(ListWidget *me)
{
   ListWidget_NotifyFocusChange(me);
   ListWidget_NotifyScroll(me);
}


static __inline
void ListWidget_InvalidateExtent(IDecorator *po, uint32 dwFlags) 
{
   ME_FROM_DECORATOR;
   AEERect rc;
   SETAEERECT(&rc, 0, 0, WBASE(me)->extent.width, WBASE(me)->extent.height);
   Decorator_Invalidate(po, &rc, dwFlags);
}

static 
void ListWidget_ModelChanged(IDecorator *po, ListModelEvent *pEvent)
{
   if (pEvent->base.evCode == EVT_MDL_LIST_CHANGE) {
      
      ME_FROM_DECORATOR;

      int size = pEvent->newSize;
      int sizeDelta = size - pEvent->oldSize;
      boolean bAnimated = 0;

      // turn off animations/marquee scrolling
      if (me->base.pChild && IWIDGET_IsAnimated(me->base.pChild)) {
         bAnimated = 1;
         IWIDGET_Animate(me->base.pChild, 0);
      }

      if (size == 0) {
         me->focusIndex = 0;
         me->topIndex = -1;
         me->xOffset = 0;
         me->yOffset = 0;
         me->nVarRange = 0;
         me->nVarTopPos = 0;
         me->nVarTopIndex = 0;

         me->bDrawing = 1;
         ListWidget_SetItemData(me, -1);
         me->bDrawing = 0;

         if (me->base.pChild && bAnimated) {
            IWIDGET_Animate(me->base.pChild, 1);
         }

         ListWidget_NotifyView(me);
         ListWidget_InvalidateExtent(po, 0);

         return;
      }

      if (pEvent->pos < me->focusIndex && sizeDelta) {
         // There's a change above the focus item. Adjust the focus position
         if (pEvent->pos - sizeDelta > me->focusIndex) {
            // Focused item was deleted
            me->focusIndex = pEvent->pos;
         } else {
            // Adjust focus
            me->focusIndex += sizeDelta;
         }
      }

      // variable height lists need to calculate nVarRange
      if (IsVarHeight(me)) {
         uint32 start, count;
         // were new items only appended to end of list?
         if (pEvent->newSize > pEvent->oldSize && pEvent->pos == pEvent->oldSize) {
            // then measure new items only
            start = pEvent->pos;
            count = pEvent->newSize - pEvent->oldSize;
         } else {    
            // measure entire list
            me->nVarRange = 0;
            start = 0;
            count = pEvent->newSize;
         }
         // add to existing range (could be 0)
         me->nVarRange += ListWidget_MeasureSpan(me, start, count);
      }

      // see if focusIndex is out of range...
      if (size >= 0 && size < pEvent->oldSize) {

         int minYOffset, minXOffset;
         int dyRange = me->dyItem * size;
         int dxRange = me->dxItem * size;

         // if variable height, override a calculated 
         // range with the actual measured range
         if (IsVarHeight(me)) {
            if (me->nOrientation == LWO_VERTICAL) {
               dyRange = me->nVarRange;
            } else {
               dxRange = me->nVarRange;
            }
         }

         minYOffset = MIN(0, me->border.rcClient.dy - dxRange);
         minXOffset = MIN(0, me->border.rcClient.dx - dyRange);

         if (me->focusIndex >= size) {
            me->focusIndex = MAX(0, size-1);
         }

         if (me->yOffset < minYOffset) {
            me->yOffset = minYOffset;
         }

         if (me->xOffset < minXOffset) {
            me->xOffset = minXOffset;
         }
      }

      if (me->base.pChild && bAnimated) {
         IWIDGET_Animate(me->base.pChild, 1);
      }

      ListWidget_NotifyView(me);
      ListWidget_InvalidateExtent(po, 0);
   }
}


// Note that index < 0 means NULL out the value model
static __inline void ListWidget_SetItemData(ListWidget *me, int index) {
   if (me->piValueModel) {
      void *pv = 0;
      if (index >= 0) {
         ILISTMODEL_GetAt(me->piModel, index, (void**)&pv);
      }
      IVALUEMODEL_SetValue(me->piValueModel, pv, -1, NULL);
   }
}


/////////////////////////////////////////////////////////////////////////////
// Internal indexer

static void ListWidget_Indexer(ListWidget *me, int nStartIndex, int nCount, 
                               PFNINTERNALINDEXER pfnWork, void *pvContext)
{
   int index = nStartIndex;
   int inc = 1;
   int size = ListWidget_GetModelSize(me);
   boolean bDrawing = me->bDrawing;

   // can't index over an empty list
   // or without an item widget
   if (!size || !me->base.pChild) {
      return;
   }
   
   if (nCount < 0) {
      inc = -inc;
      nCount = -nCount;
      nCount = MIN(index+1, nCount);
   
   } else {

      nCount = MIN(nCount, size-index+1);
   }

   me->bDrawing = 1;
   while (nCount--) {
      
      ListWidget_SetItemData(me, index);

      // reset the var item size member to default
      me->nVarItemSize = (me->nOrientation == LWO_VERTICAL) ? me->dyItem : me->dxItem;
      
      // client installed indexer callback
      if (me->idx.pfnIndexer) {
         me->idx.pfnIndexer(me->idx.pCtx, index, (boolean)(index == me->focusIndex));
      }

      if (!(me->dwFlags & LWF_NOSELECT))  {
         IWIDGET_SetSelected(me->base.pChild, (boolean)(index == me->focusIndex));
      }

      // call internal 'work' function
      if (0 == pfnWork(me, index, pvContext)) {
         break;
      }
      index += inc;
      // we can tolerate counts that are too large
      // index over only the valid indexes
      if (index < 0 || index >= size) {
         break;
      }
   }

   ListWidget_SetItemData(me, me->focusIndex);
   IWIDGET_SetSelected(me->base.pChild, (boolean)(0 == (me->dwFlags & LWF_NOSELECT)));

   me->bDrawing = bDrawing;
}

/////////////////////////////////////////////////////////////////////////////
// variable-height drawing


static void ListWidget_DrawItem(ListWidget *me, ICanvas *piCanvas, AEERect *prcClip, AEERect *prcItem)
{
   AEERect rcItemClip;
   if (IntersectRect(&rcItemClip, prcClip, prcItem)) {

      boolean bSel = 0;

      ICANVAS_SetClipRect(piCanvas, &rcItemClip);

      // force the item extent
      Decorator_SetExtentEx((IDecorator*)me, prcItem->dx, prcItem->dy);

      // Now draw
      Decorator_Draw((IDecorator*)me, piCanvas, prcItem->x, prcItem->y);

      IWIDGET_GetSelected(me->base.pChild, &bSel);
      
      // save rectangle of selected item
      if (bSel) {
         me->rcSel = *prcItem;
      }
   }
}


typedef struct {
   AEERect *   prcItem;    // item rectangle
   AEERect *   prcClip;    // original canvas clip rect
   ICanvas *   piCanvas;   // canvas to draw on
   int         xMax;       // x-limit, for picks, stop when this is reached
   int         yMax;       // y-limit, for lists, stop when this is reached
   int         xDraw;      // x origin of widget
   int         yDraw;      // y origin of widget
} VDrawContext;


static boolean ListWidget_DrawWork(ListWidget *me, int index, VDrawContext *pdc)
{
   int size = 0;
   boolean bVert = me->nOrientation == LWO_VERTICAL;

   // If variable list, allow the app to modify the appropriate dimension
   // get the item size which the indexer may have altered

   if (IsVarHeight(me)) {
      size = me->nVarItemSize;
   } else if (index == me->focusIndex) {
      size = me->nSelItem;
   }

   if (bVert) {
      pdc->prcItem->dy = size ? size : me->dyItem;
   } else {
      pdc->prcItem->dx = size ? size : me->dxItem;
   }

   ListWidget_DrawItem(me, pdc->piCanvas, pdc->prcClip, pdc->prcItem);
   FARF(DRAW, ("Draw, index=%d, x=%d, y=%d", index, pdc->prcItem->x, pdc->prcItem->y));

   // adjust item rectangle origin by it's extent
   if (bVert) {
      pdc->prcItem->y += pdc->prcItem->dy;
   } else {
      pdc->prcItem->x += pdc->prcItem->dx;
   }

   // check if we have reached the end of our display window
   if (pdc->prcItem->y > pdc->yMax ||
       pdc->prcItem->x > pdc->xMax) {
      return 0;
   }

   return 1;
}

static boolean ListWidget_GridDrawWork(ListWidget *me, int index, VDrawContext *pdc)
{
   AEERect rcWidget;
   BigRect rcItem;

   ListWidget_CalcItemRect(me, index, &rcItem);
   ListWidget_ItemRectToWidgetCoords(me, &rcItem, &rcWidget);
   
   rcWidget.x += pdc->xDraw;
   rcWidget.y += pdc->yDraw;

   ListWidget_DrawItem(me, pdc->piCanvas, pdc->prcClip, &rcWidget);

   // check if we have reached the end of our display window
   if (rcWidget.y > pdc->yMax || 
       rcWidget.x > pdc->xMax) {
      return 0;
   }

   return 1;
}

static boolean IsRectConstrainedBy(AEERect *prcBounds, AEERect *prcInner) {
   AEERect rcInner = *prcInner;
   return (   !ISRECTEMPTY(prcBounds) 
           && !ISRECTEMPTY(prcInner) 
           && 0 == ConstrainRect(prcBounds, &rcInner, NULL, NULL));
}

void ListWidget_Draw(IDecorator *po, ICanvas *piCanvas, int x, int y)
{
   ME_FROM_DECORATOR;

   BigRect rcItem;
   AEERect rcWidget, rcClip;
   int modelSize;
   int nTopIndex;
   boolean bAnimated = 0;
   boolean bDrawItemOnly = me->fInvalChild;

   AEERect rcSel;
   rcSel = me->rcSel;
   rcSel.x += x;
   rcSel.y += y;

   FARF(ITEMDRAW, ("Enter Draw"));

   Border_Draw(&me->border, piCanvas, x, y);
   Border_AdjustDisplayClipRect(&me->border, piCanvas, x, y);

   ICANVAS_GetClipRect(piCanvas, &rcClip);

   me->fInvalChild = 0;

   modelSize = ListWidget_GetModelSize(me);
   if (!modelSize || !me->base.pChild) {
      return;  // bail if nothing in model or no item widget
   }

   // special case for when only the item widget content is invalidated
   if (bDrawItemOnly && IsRectConstrainedBy(&rcSel, &rcClip)) {

      FARF(ITEMDRAW, ("rcClip=%d,%d,%d,%d, rcSel=%d,%d,%d,%d", 
                       rcClip.x, rcClip.y, rcClip.dx, rcClip.dy,
                       rcSel.x, rcSel.y, rcSel.dx, rcSel.dy));

      // just draw the selected item
      me->bDrawing = 1;
      Decorator_Draw((IDecorator*)me, piCanvas, rcSel.x, rcSel.y);
      me->bDrawing = 0;
      return;
   }


   me->bDrawing = TRUE;
   if (IsVarHeight(me)) {
      nTopIndex = me->nVarTopIndex;
   } else {
      nTopIndex = ListWidget_GetTopIndex(me);
   }

   ListWidget_CalcItemRect(me, nTopIndex, &rcItem);
   ListWidget_ItemRectToWidgetCoords(me, &rcItem, &rcWidget);

   rcWidget.x += x;
   rcWidget.y += y;

   SETRECTEMPTY(&me->rcSel);

   // turn off animations/marquee scrolling
   bAnimated = IWIDGET_IsAnimated(me->base.pChild);
   if (bAnimated) {
      IWIDGET_Animate(me->base.pChild, 0);
   }

   // set up the draw context
   {  VDrawContext vdc;
      vdc.prcClip    = &rcClip;
      vdc.prcItem    = &rcWidget;
      vdc.piCanvas   = piCanvas;
      vdc.xMax       = x + me->border.rcClient.x + me->border.rcClient.dx + 1;
      vdc.yMax       = y + me->border.rcClient.y + me->border.rcClient.dy + 1;
      vdc.xDraw      = x;
      vdc.yDraw      = y;
      
      // call work function for every visible item
      ListWidget_Indexer(me, nTopIndex, modelSize - nTopIndex, 
                         (me->nOrientation == LWO_GRID) ? 
                         (PFNINTERNALINDEXER)ListWidget_GridDrawWork :
                         (PFNINTERNALINDEXER)ListWidget_DrawWork,
                         &vdc);
   }

   // restore the extent of the selected item
   if (!ISRECTEMPTY(&me->rcSel)) {
      WExtent we;
      we.width = me->rcSel.dx;
      we.height = me->rcSel.dy;
      Decorator_SetExtent((IDecorator*)me, &we);
      // convert to widget coordinates (from screen coordinates)
      me->rcSel.x -= x;
      me->rcSel.y -= y;
   }

   if (bAnimated) {
      IWIDGET_Animate(me->base.pChild, 1);
   }
   me->bDrawing = 0;

}


void ListWidget_SetExtent(IDecorator *po, WExtent *pe)
{
   ME_FROM_DECORATOR;

   WidgetBase_SetExtent((IWidget*)&me->base.base, pe);
   Border_CalcClientRect(&me->border);
   
   if (me->nOrientation == LWO_VERTICAL) {
      me->dxItem = me->border.rcClient.dx;
   } else if (me->nOrientation == LWO_HORIZONTAL) {
      me->dyItem = me->border.rcClient.dy;
   } else if (me->nOrientation == LWO_GRID) {
      if (me->bKeepLayout) {
         me->dyItem = me->border.rcClient.dy / me->nRows;
         me->dxItem = me->border.rcClient.dx / me->nCols;
      }
   }
    
   ListWidget_SetItemData(me, 0);
   Decorator_SetExtentEx(po, me->dxItem, me->dyItem);
   ListWidget_SetItemData(me, me->focusIndex);

   // when the extent changes, we can end up with a bunch of blank space below
   // the end of the list. For example, if the last widget in the list has the 
   // focus, then when the extent gets bigger, the last list item will be at the top
   // and the rest of area under the last item will be white space.  To prevent 
   // this from happening, check if the topindex is no longer valid, then clear 
   // scroll position and top index to force recalc in SetTopAndFocusIndex()
   {
      int top = ListWidget_GetTopIndex(me);
      int vis = VisibleItems(me);
      int modelSize = ListWidget_GetModelSize(me);

      if (modelSize > 0 && top > (modelSize - vis)) {
         me->topIndex = -1;
         me->yOffset = 0;
      }
   }
   ListWidget_SetTopAndFocusIndex(me, 1);
}

void ListWidget_GetExtent(IDecorator *po, WExtent *pe)
{
   // Don't ask child, we control the extent
   WidgetBase_GetExtent(IDECORATOR_TO_IWIDGET(po), pe);
}

static boolean PickWidget_GetPrefExtentWork(ListWidget *me, int index, WExtent *pwe)
{
   WExtent weChild;
   Decorator_GetPreferredExtent((IDecorator*)&me->base, &weChild);
   pwe->height = MAX(weChild.height, pwe->height);
   pwe->width += me->nVarItemSize;
   return TRUE;
}
static boolean ListWidget_GetPrefExtentWork(ListWidget *me, int index, WExtent *pwe)
{
   WExtent weChild;
   Decorator_GetPreferredExtent((IDecorator*)&me->base, &weChild);
   pwe->width = MAX(weChild.width, pwe->width);
   pwe->height += me->nVarItemSize;
   return TRUE;
}

void ListWidget_GetPreferredExtent(IDecorator *po, WExtent *pweOut)
{
   ME_FROM_DECORATOR;
   
   WExtent we;
   int nModelSize = ListWidget_GetModelSize(me);
   int nRowsCols = (me->nOrientation == LWO_VERTICAL) ? me->nRows : me->nCols;
   int nSize = (nRowsCols > 0) ? nRowsCols : (nModelSize != 0) ? nModelSize : 1;
   int nPrefItemSize = (me->nOrientation == LWO_VERTICAL) ? me->dxPrefItem : me->dyPrefItem;

   we.height = 0;
   we.width = 0;

   // index thru some list items if:
   //   we have model data -and-
   //   we are a variable-sized list -or-
   //   we don't know how wide(list) or high(pick) we're supposed to be
   if (nModelSize && (IsVarHeight(me) || !nPrefItemSize)) {
      int nStart = ((me->topIndex < 0) ? 0 : me->topIndex);
      int nCount = ((nRowsCols > 0) ? nRowsCols : nModelSize);
      ListWidget_Indexer(me, nStart, nCount, 
                         ((me->nOrientation == LWO_VERTICAL) 
                            ? (PFNINTERNALINDEXER)ListWidget_GetPrefExtentWork
                            : (PFNINTERNALINDEXER)PickWidget_GetPrefExtentWork), 
                         &we);
   }

   // do for LIST widgets...
   if (me->nOrientation == LWO_VERTICAL) {
      // force height if not set in above block
      if (0 == we.height) {
         we.height = me->dyItem * nSize;
      }
      // override width if item width was explicitly set
      if (me->dxPrefItem) {
         we.width = me->dxPrefItem;
      }
      // if width is still zero, 
      // then ask the item widget for it's preferred extent
      if (0 == we.width) {
         WExtent weTemp;
         Decorator_GetPreferredExtent(po, &weTemp);
         we.width = weTemp.width;
      }

   // do for PICK widgets...
   } else {
      // force width if not set in above block
      if (0 == we.width) {
         we.width = me->dxItem * nSize;
      }
      // override height if item height was explicitly set
      if (me->dyPrefItem) {
         we.height = me->dyPrefItem;
      }
      // if height is still zero, 
      // then ask the item widget for it's preferred extent
      if (0 == we.height) {
         WExtent weTemp;
         Decorator_GetPreferredExtent(po, &weTemp);
         we.height = weTemp.height;
      }

   }

   Border_CalcPreferredExtent(&me->border, pweOut, &we);   
}

boolean ListWidget_IntersectOpaque(IDecorator *po, AEERect *prcOut, 
                                   const AEERect *prcIn)
{
   ME_FROM_DECORATOR;
   return Border_IntersectOpaque(&me->border, prcOut, prcIn);
}


static boolean ListWidget_Move(ListWidget *me, uint16 wParamKey)
{
   int focusIndex = me->focusIndex;
   int xOffSave = me->xOffset;
   int yOffSave = me->yOffset;

   int size = ListWidget_GetModelSize(me);
   int vis = VisibleItems(me);

   boolean bVert = (me->nOrientation == LWO_VERTICAL);
   boolean bHorz = (me->nOrientation == LWO_HORIZONTAL);

   boolean bInval = 0;

   if (size == 0) {
      return FALSE;
   }

   if (bVert && (wParamKey == AVK_LEFT || wParamKey == AVK_RIGHT)) {
      return FALSE;
   }

   if (bHorz && (wParamKey == AVK_UP || wParamKey == AVK_DOWN)) {
      return FALSE;
   }

   // NOSELECT means we just scroll the list, there's no selection to move
   if (me->dwFlags & LWF_NOSELECT) {
      int inc;
      int yMin, xMin;

      if (GetPartial(me)) {
         size++;
      }

      if (size <= vis) {
         return TRUE;
      }

      inc = bVert ? -me->dyItem : -me->dxItem;

      if (wParamKey == AVK_UP || wParamKey == AVK_LEFT) {
         inc = -inc;
      }

      if (bVert) {
         yMin = -((size - vis) * me->dyItem);
         FARF(OFFSET, ("Move: yOffset %d ->", me->yOffset));
         me->yOffset += inc;
         me->yOffset = CONSTRAIN(me->yOffset, yMin, 0);
         if (me->yOffset == yMin) {
            me->yOffset += GetPartial(me);
         }
         FARF(OFFSET, ("Move: yOffset %d", me->yOffset));
      
      } else {
         xMin = -((size - vis) * me->dxItem);
         me->xOffset += inc;
         me->xOffset = CONSTRAIN(me->xOffset, xMin, 0);
         if (me->xOffset == xMin) {
            me->xOffset += GetPartial(me);
         }
         FARF(OFFSET, ("Move: xOffset %d", me->xOffset));
      }
         

   } else {

      if (wParamKey == AVK_UP || wParamKey == AVK_LEFT) {
   
         if (me->focusIndex == 0 && (me->dwFlags & LWF_WRAP)) {
            me->focusIndex = MAX(size - 1, 0);
            ListWidget_NotifyFocusWrap(me);
         } else {
            me->focusIndex = MAX(me->focusIndex - 1, 0);
         }
   
      } else {
   
         if (me->focusIndex == size - 1 && (me->dwFlags & LWF_WRAP)) {
            me->focusIndex = 0;
            ListWidget_NotifyFocusWrap(me);
         } else {
            me->focusIndex = MIN(me->focusIndex + 1, size - 1);
         }
      }

      ListWidget_EnsureVisible(me, me->focusIndex);
   }


   if (focusIndex != me->focusIndex) {
      ListWidget_NotifyFocusChange(me);
      bInval = 1;
   }

   if (xOffSave != me->xOffset || yOffSave != me->yOffset) {
      ListWidget_NotifyScroll(me);
      bInval = 1;
   }

   if (bInval) {
      ListWidget_InvalidateExtent((IDecorator*)me, 0);
   }

   return TRUE;
}


boolean ListWidget_HandleEvent(IDecorator *po, AEEEvent evt, uint16 wParam, 
                               uint32 dwParam)
{
   ME_FROM_DECORATOR;

   AEERect *prcClient = &me->border.rcClient;
   AEERect rcSave     = *prcClient;

   // Deal with border properties first
   if (Border_HandleEvent(&me->border, evt, wParam, dwParam)) {
      if (rcSave.dx != prcClient->dx ||
          rcSave.dy != prcClient->dy) {
         // client rect changed 
         IWIDGET_SetExtent(IDECORATOR_TO_IWIDGET(po), &WBASE(me)->extent);
      }
      // Deal with focus
      if (evt == EVT_WDG_SETFOCUS) {
         // pass it into our item (child) widget 
         Decorator_HandleEvent(po, evt, wParam, dwParam);
      }
      return TRUE;
   }

   switch(evt) {
   
      case EVT_KEY: {
         switch (wParam) {
            case AVK_LEFT:
            case AVK_UP:
            case AVK_RIGHT:
            case AVK_DOWN:
               if (me->pfnMove && me->pfnMove(me, wParam)) {
                  return TRUE;
               }
               break;
   
            case AVK_SELECT:
               return ListWidget_NotifySelect(me);             
         }
      } break;

      case EVT_WDG_SETPROPERTY: {
         switch (wParam) {
         
            case PROP_INDEXER:
               me->idx = *((IndexerDesc*)dwParam);
               ListWidget_RecalcRange(me);
               return TRUE;

            case PROP_TOPINDEX:
               if ((int)dwParam >= ListWidget_GetModelSize(me)) {
                  return FALSE;
               }
               me->topIndex = dwParam;
               ListWidget_SetTopAndFocusIndex(me, 0);
               ListWidget_InvalidateExtent((IDecorator*)me, 0);
               return TRUE;

            case PROP_FOCUSINDEX:
            case PROP_SELECTINDEX: {
               int focusIndex = me->focusIndex;
               if ((int)dwParam >= ListWidget_GetModelSize(me)) {
                  return FALSE;
               }
               me->focusIndex = dwParam;
               ListWidget_SetTopAndFocusIndex(me, 0);
               if (focusIndex != me->focusIndex) {
                  ListWidget_NotifyFocusChange(me);
               }
               if (wParam == PROP_SELECTINDEX) {
                  ListWidget_NotifySelect(me);
               }
               ListWidget_InvalidateExtent((IDecorator*)me, 0);
               return TRUE;
            }
      
            case PROP_SELITEMSIZE:
               me->nSelItem = (uint8)dwParam;
               return TRUE;
            
            case PROP_ITEMHEIGHT:
               if (me->nOrientation == LWO_VERTICAL || me->nOrientation == LWO_GRID) {
                  me->dyItem = (uint8)dwParam;
                  me->bKeepLayout = FALSE; // on extent change, number of rows may change
                  IWIDGET_SetExtent(IDECORATOR_TO_IWIDGET(po), &WBASE(me)->extent);
               }
               me->dyPrefItem = (uint8)dwParam;
               return TRUE;
            
            case PROP_ITEMWIDTH:
               if (me->nOrientation == LWO_HORIZONTAL || me->nOrientation == LWO_GRID) {
                  me->dxItem = (uint8)dwParam;
                  me->bKeepLayout = FALSE; // on extent change, number of cols may change
                  IWIDGET_SetExtent(IDECORATOR_TO_IWIDGET(po), &WBASE(me)->extent);
               }
               me->dxPrefItem = (uint8)dwParam;
               return TRUE;
            
            case PROP_HINT_ROWS:
               if (me->nOrientation == LWO_GRID || me->nOrientation == LWO_VERTICAL) {
                  me->nRows = (uint8)dwParam;
                  return TRUE;
               }
               break;
      
            case PROP_HINT_COLS:
               if (me->nOrientation == LWO_GRID || me->nOrientation == LWO_HORIZONTAL) {
                  me->nCols = (uint8)dwParam;
                  return TRUE;
               }
               break;

            case PROP_ROWS:
               if ((me->nOrientation == LWO_GRID || me->nOrientation == LWO_VERTICAL) && (dwParam > 0)) {
                  me->dyItem = me->border.rcClient.dy / (uint8)dwParam;
                  me->bKeepLayout = TRUE; // on extent change, resize the items to keep this number of rows
               }
               me->nRows = (uint8)dwParam;
               return TRUE;

            case PROP_COLS:
               if ((me->nOrientation == LWO_GRID || me->nOrientation == LWO_HORIZONTAL) && (dwParam > 0)) {
                  me->dxItem = me->border.rcClient.dx / (uint8)dwParam;
                  me->bKeepLayout = TRUE; // on extent change, resize the items to keep this number of cols
               }
               me->nCols = (uint8)dwParam;
               return TRUE;
      
            case PROP_FLAGS:
               me->dwFlags = dwParam;
               if (me->dwFlags & LWF_NOSELECT) {
                  // force LWF_WRAP off
                  me->dwFlags &= ~LWF_WRAP;
                  // unselect the item
                  if(me->base.pChild) {
                     IWIDGET_SetSelected(me->base.pChild, 0);
                  }
               }
               return TRUE;

            case PROP_VARITEMSIZE:
               if (me->bDrawing && IsVarHeight(me)) {
                  me->nVarItemSize = (int)dwParam;
                  return TRUE;
               }
               return FALSE;

            case PROP_EX:
               if (HandleEventSetPropertyEx(po, dwParam)) {
                  return TRUE;
               }
               break;
         } 
         break;
      }  // EVT_WDG_SETPROPERTY
   
      case EVT_WDG_GETPROPERTY:
         switch (wParam) {
         
            case PROP_VARITEMSIZE:
               if (me->bDrawing && IsVarHeight(me)) {
                  *((int*)dwParam) = me->nVarItemSize;
                  return TRUE;
               }
               return FALSE;

            case PROP_ITEMRECT: {
               PropItemRectDesc *pi = (PropItemRectDesc*)dwParam;
               BigRect rcItem;

               if (ListWidget_IsVisible(me, pi->nIndex) 
                     && ListWidget_CalcItemRect(me, pi->nIndex, &rcItem) 
                     && ListWidget_ItemRectToWidgetCoords(me, &rcItem, pi->prc)) {
                  return TRUE;
               }

               SETAEERECT(pi->prc, 0,0,0,0);
               return FALSE;
            }

            case PROP_TOPINDEX:
               if (IsVarHeight(me)) {
                  *(int*)dwParam = me->nVarTopIndex;
               } else {
                  *(int*)dwParam = (me->topIndex > -1) ? me->topIndex : ListWidget_GetTopIndex(me);
               }
               return TRUE;

            case PROP_SELITEMSIZE:
               *(int*)dwParam = me->nSelItem;
               return TRUE;

            case PROP_FOCUSINDEX:
               *(int *)dwParam = me->focusIndex;
               return TRUE;
            
            case PROP_ITEMHEIGHT:
               *(int *)dwParam = me->dyItem;
               return TRUE;

            case PROP_ITEMWIDTH:
               *(int *)dwParam = me->dxItem;
               return TRUE;
      
            case PROP_HINT_ROWS:
               *(int*)dwParam = me->nRows;
               return TRUE;

            case PROP_HINT_COLS:
               *(int*)dwParam = me->nCols;
               return TRUE;

            case PROP_ROWS:
               *(int*)dwParam = me->border.rcClient.dy / ((me->dyItem != 0) ? me->dyItem : 1);
               return TRUE;
      
            case PROP_COLS:
               *(int*)dwParam = me->border.rcClient.dx / ((me->dxItem != 0) ? me->dxItem : 1);
               return TRUE;
      
            case PROP_FLAGS:
               *(uint32*)dwParam = me->dwFlags;
               return TRUE;
      
            case PROP_VIEWMODEL:
               if (SUCCESS == WidgetBase_GetViewModel(WBASE(me), (IModel**)dwParam)) {
                  CALLBACK_Resume(&me->cbkView,  ListWidget_NotifyView, me, me->piShell);
               }
               return TRUE;

            case PROP_EX:
               if (HandleEventGetPropertyEx(po, dwParam)) {
                  return TRUE;
               }
               break;

            default:
               break;
         } 
         break;
   }
   
   return Decorator_HandleEvent(po, evt, wParam, dwParam);
}

static boolean HandleEventGetPropertyEx(
   IDecorator* po,
   uint32      dwParam)
{
   ME_FROM_DECORATOR;

   WidgetPropEx*  pWidgetPropEx  = (WidgetPropEx*)dwParam;
   boolean        bHandled       = FALSE;

   switch (pWidgetPropEx->nPropId) {

   case PROPEX_ITEMHEIGHT:
      {
         int*  pnItemHeight   = pWidgetPropEx->pUser;
         int   nDataSize      = (int)sizeof(*pnItemHeight);

         if (pWidgetPropEx->nSize >= nDataSize) {
            *pnItemHeight = me->dyItem;
            pWidgetPropEx->nSize = nDataSize;
            bHandled = TRUE;
         }
      }
      break;

   case PROPEX_ITEMWIDTH:
      {
         int*  pnItemWidth = pWidgetPropEx->pUser;
         int   nDataSize   = (int)sizeof(*pnItemWidth);

         if (pWidgetPropEx->nSize >= nDataSize) {
            *pnItemWidth = me->dxItem;
            pWidgetPropEx->nSize = nDataSize;
            bHandled = TRUE;
         }
      }
      break;

   case PROPEX_SELITEMSIZE:
      {
         int*  pnSelItemSize  = pWidgetPropEx->pUser;
         int   nDataSize      = (int)sizeof(*pnSelItemSize);

         if (pWidgetPropEx->nSize >= nDataSize) {
            *pnSelItemSize = me->nSelItem;
            pWidgetPropEx->nSize = nDataSize;
            bHandled = TRUE;
         }
      }
      break;

   default:
      break;
   }

   return bHandled;
}

static boolean HandleEventSetPropertyEx(
   IDecorator* po,
   uint32      dwParam)
{
   ME_FROM_DECORATOR;

   WidgetPropEx*  pWidgetPropEx  = (WidgetPropEx*)dwParam;
   boolean        bHandled       = FALSE;

   switch (pWidgetPropEx->nPropId) {

   case PROPEX_ITEMHEIGHT:
      {
         int*  pnItemHeight   = pWidgetPropEx->pUser;
         int   nDataSize      = (int)sizeof(*pnItemHeight);

         if (pWidgetPropEx->nSize >= nDataSize) {
            if ((me->nOrientation == LWO_VERTICAL) || (me->nOrientation == LWO_GRID)) {
               me->dyItem = *pnItemHeight;
               me->bKeepLayout = FALSE; // on extent change, number of rows may change
               IWIDGET_SetExtent(IDECORATOR_TO_IWIDGET(po), &WBASE(me)->extent);
            }
            me->dyPrefItem = *pnItemHeight;
            bHandled = TRUE;
         }
      }
      break;

   case PROPEX_ITEMWIDTH:
      {
         int*  pnItemWidth = pWidgetPropEx->pUser;
         int   nDataSize   = (int)sizeof(*pnItemWidth);

         if (pWidgetPropEx->nSize >= nDataSize) {
            if ((me->nOrientation == LWO_HORIZONTAL) || (me->nOrientation == LWO_GRID)) {
               me->dxItem = *pnItemWidth;
               me->bKeepLayout = FALSE; // on extent change, number of cols may change
               IWIDGET_SetExtent(IDECORATOR_TO_IWIDGET(po), &WBASE(me)->extent);
            }
            me->dxPrefItem = *pnItemWidth;
            bHandled = TRUE;
         }
      }
      break;

   case PROPEX_SELITEMSIZE:
      {
         int*  pnSelItemSize  = pWidgetPropEx->pUser;
         int   nDataSize      = (int)sizeof(*pnSelItemSize);

         if (pWidgetPropEx->nSize >= nDataSize) {
            me->nSelItem = *pnSelItemSize;
            bHandled = TRUE;
         }
      }
      break;

   default:
      break;
   }

   return bHandled;
}

uint32 ListWidget_Release(IDecorator *po)
{
   ME_FROM_DECORATOR;

   if (WBASE(me)->nRefs == 1) {
      CALLBACK_Cancel(&me->cbkView);
      LISTENER_Cancel(&me->modelListener);
      RELEASEIF(me->piModel);
      RELEASEIF(me->piValueModel);
      RELEASEIF(me->piShell);
      Border_Dtor(&me->border);
   }
   return Decorator_Release(po);
}

int ListWidget_SetModel(IDecorator *po, IModel *piModel)
{
   ME_FROM_DECORATOR;

   int nOldSize = ListWidget_GetModelSize(me);

   RELEASEIF(me->piModel);

   if (SUCCESS != IMODEL_QueryInterface(piModel, AEEIID_LISTMODEL, (void **)&me->piModel)) {
      return ECLASSNOTSUPPORT;
   }

   WidgetBase_SetModel(IDECORATOR_TO_IWIDGET(po), piModel);
   IMODEL_AddListenerEx(piModel, &me->modelListener, (PFNLISTENER)ListWidget_ModelChanged, me);

   // force a list model change
   {
      ListModelEvent ev;
      int nNewSize = ListWidget_GetModelSize(me);
      LISTMODELEVENT_Init(&ev, EVT_MDL_LIST_CHANGE, me->focusIndex, nOldSize, nNewSize);
      ListWidget_ModelChanged(po, &ev);
   }

   ListWidget_InvalidateExtent(po, 0);

   return SUCCESS;
}

int ListWidget_GetModel(IDecorator *po, AEECLSID clsid, IModel **ppo)
{
   ME_FROM_DECORATOR;

   if (me->piModel) {
      return ILISTMODEL_QueryInterface(me->piModel, clsid, (void **)ppo);
   } else {
      *ppo = 0;
      return EFAILED;
   }
}

void ListWidget_SetWidget(IDecorator *po, IWidget *pw)
{
   ME_FROM_DECORATOR;

   // get the value model from the item widget that is passed to use
   RELEASEIF(me->piValueModel);
   
   if (pw) {
      IWIDGET_GetModel(pw, AEEIID_VALUEMODEL, (IModel **)&me->piValueModel);
   }

   Decorator_SetWidget(po, pw);

   IWIDGET_SetExtent(IDECORATOR_TO_IWIDGET(po), &WBASE(me)->extent);
}

void ListWidget_Invalidate(IContainer *po, IWidget *pw, const AEERect *prc, uint32 dwFlags)
{
   ME_FROM_CONTAINER;

   FARF(ITEMDRAW, ("Invalidate: me->bDrawing = %d", me->bDrawing));

   // Ignore invalidates generated from within our Draw loop
   if (!me->bDrawing) {

      // if selection rectangle is not valid
      if (ISRECTEMPTY(&me->rcSel) || (dwFlags & ICIF_EXTENT)) {
         // draw entire widget
         ListWidget_InvalidateExtent((IDecorator*)me, dwFlags);
      
      } else {
         AEERect rcInval = me->rcSel;
         if (prc) {
            rcInval.x = rcInval.x + prc->x;
            rcInval.y = rcInval.y + prc->y;
            rcInval.dx = prc->dx;
            rcInval.dy = prc->dy;
         }
         me->fInvalChild = 1;

         FARF(ITEMDRAW, ("Invalidate: rcInval=%d,%d,%d,%d", 
                         rcInval.x, rcInval.y, rcInval.dx, rcInval.dy));
         Decorator_Invalidate((IDecorator*)me, &rcInval, dwFlags);
      }
   }
}


int ListWidget_Locate(IContainer *po, IWidget *piw, IContainer **ppRoot, 
                      AEERect *prcRel)
{
   // Since we re-use the child, there is no good way to know
   // where the child is relative to this list
   return EFAILED;
}


void ListWidget_Ctor(ListWidget *me, AEEVTBL(IDecorator) *pvt, IListModel *piModel, IShell *piShell, 
                     IModule *piModule, PFNHANDLER pfnDefHandler)
{
   Decorator_Ctor(&me->base, pvt, piModule, DEFHANDLER(ListWidget_HandleEvent)); 
   
   // derived Widget methods
   pvt->Release            = ListWidget_Release;
   pvt->Draw               = ListWidget_Draw;
   pvt->GetExtent          = ListWidget_GetExtent;
   pvt->SetExtent          = ListWidget_SetExtent;
   pvt->GetPreferredExtent = ListWidget_GetPreferredExtent;
   pvt->IntersectOpaque    = ListWidget_IntersectOpaque;
   pvt->SetModel           = ListWidget_SetModel;
   pvt->GetModel           = ListWidget_GetModel;
   pvt->SetWidget          = ListWidget_SetWidget;

   // replacable move handler
   me->pfnMove = ListWidget_Move;

   me->piShell = piShell;
   ISHELL_AddRef(piShell);

   // Container definitions
   WCBASE(me)->vtContainer.Invalidate = ListWidget_Invalidate;
   WCBASE(me)->vtContainer.Locate = ListWidget_Locate;

   me->bDrawing      = FALSE;
   me->focusIndex    = 0;
   me->nOrientation  = LWO_VERTICAL;
   me->nRows         = 0;   // zero means nRows == ILISTMODEL_Size()
   me->nCols         = 1;

   Border_Ctor(&me->border, piShell, (PFNINVALIDATE)WidgetBase_Invalidate, me, &WBASE(me)->extent, TRUE, &WBASE(me)->piViewModel);
   Border_SetBGColor(&me->border, 0, RGBA_WHITE);

   me->dxItem = 10;
   me->dyItem = 10;
   me->nSelItem = 0;

   me->dxPrefItem = 0;
   me->dyPrefItem = 0;

   me->topIndex = -1;

   IWIDGET_SetModel(LISTWIDGET_TO_IWIDGET(me), ILISTMODEL_TO_IMODEL(piModel));
}


typedef void (*PFNCTOR)(ListWidget*, AEEVTBL(IDecorator)*, IListModel*, IShell*, 
                        IModule*, PFNHANDLER);


int ListWidget_NewEx(IDecorator **ppo, IListModel *piListModel, PFNCTOR pfnCtor, 
                     IShell *piShell, IModule *piModule, PFNHANDLER pfnDefHandler)
{
   int nErr = SUCCESS;

   if (!piListModel) {
      nErr = ListModel_New(&piListModel, piModule);
   } else {
      ILISTMODEL_AddRef(piListModel);
   }

   if (!nErr) {
      ListWidget *me = MALLOCREC_VTBL(ListWidget, IDecorator);
      if (me) {
         pfnCtor(me, GETVTBL(me,IDecorator), piListModel, piShell, piModule, pfnDefHandler);
         *ppo = LISTWIDGET_TO_IDECORATOR(me);
      } else {
         nErr = ENOMEMORY;
         *ppo = 0;
      }
   }
   
   RELEASEIF(piListModel);

   return nErr;
}


int ListWidget_New(IDecorator **ppo, IListModel *piListModel, IShell *piShell, IModule *piModule)
{
   return ListWidget_NewEx(ppo, piListModel, ListWidget_Ctor, piShell, piModule, 0);
}

//////////////////////////////////////////////////////////////////////////
// PickWidget

void PickWidget_Ctor(ListWidget *me, AEEVTBL(IDecorator) *pvt, IListModel *piModel, 
                     IShell *piShell, IModule *piModule, PFNHANDLER pfnDefHandler)
{
   ListWidget_Ctor((ListWidget*)me, pvt, piModel, piShell, piModule, pfnDefHandler);

   // override the class id
   me->nOrientation     = LWO_HORIZONTAL;
   me->nRows            = 1;
   me->nCols            = 0;
}


int PickWidget_New(IDecorator **ppo, IListModel *piListModel, IShell *piShell, IModule *piModule)
{
   return ListWidget_NewEx(ppo, piListModel, PickWidget_Ctor, piShell, piModule, 0);
}


//////////////////////////////////////////////////////////////////////////
// GridWidget

static boolean GridWidget_Move(ListWidget *me, uint16 wParamKey)
{
   const int focus = me->focusIndex;
   const int size  = (me->piModel) ? ILISTMODEL_Size(me->piModel) : 0;
   const int maxN  = size-1;
   
   int dir, newIndex, nVisibleCols;

   boolean bColumn;
   boolean bReverse;
   boolean bScroll = 0;
   boolean bInval = 0;
   boolean bColumnWrap;
   boolean bAColumnWrap;

   if (size == 0) {
      return FALSE;
   }

   bColumn = (wParamKey == AVK_DOWN || wParamKey == AVK_UP);
   bReverse = (wParamKey == AVK_LEFT || wParamKey == AVK_UP);

   bColumnWrap = (boolean)(me->dwFlags & LWF_COLUMNWRAP);
   bAColumnWrap = (boolean)(me->dwFlags & LWF_ADVANCECOLUMNWRAP);

   nVisibleCols = VisibleCols(me);

   dir = bReverse ? -1 : 1;

   if (bColumn) {
      dir *= nVisibleCols;
   }

   newIndex = me->focusIndex + dir;

   if ((newIndex >= 0) && (newIndex < maxN+1)) {
      me->focusIndex = newIndex;

   } else if (bColumn) {

      int nMove = 0;
      if (bAColumnWrap) {
         nMove = bReverse ? -1 : 1;
      }

      if (!bReverse) {

         // special case here...
         // if there is a partial row at the bottom
         // move to the last item
         boolean bPartial = (size % nVisibleCols);
         int nLastRow = (size-1) / nVisibleCols;
         int nRow = me->focusIndex / nVisibleCols;

         if (!bAColumnWrap && bPartial && (nRow != nLastRow)) {
            me->focusIndex = size-1;
         } else if (bAColumnWrap || bColumnWrap) {
            me->focusIndex = (newIndex+nMove) % nVisibleCols;
         }

      } else if (bColumnWrap || bAColumnWrap) {

         int nextCol = ((me->focusIndex + nMove) + nVisibleCols) % nVisibleCols;
         me->focusIndex = nextCol + (((size-1) / nVisibleCols) * nVisibleCols);
         if (me->focusIndex >= size) {
            me->focusIndex -= nVisibleCols;
         }
      }

   } else if (me->dwFlags & LWF_WRAP) {
      me->focusIndex = (bReverse ? maxN : 0);
      ListWidget_NotifyFocusWrap(me);
   }


   if (wParamKey == AVK_DOWN && focus == me->focusIndex) {
      // if we are on the last item in the column and there is
      // a partial line below, force a scroll down so that
      // the last line will be visible.
      bScroll = ListWidget_EnsureVisible(me, maxN);
   } else {
      bScroll = ListWidget_EnsureVisible(me, me->focusIndex);
   }


   if (focus != me->focusIndex) {
      ListWidget_NotifyFocusChange(me);
      bInval = 1;
   }

   if (bScroll) {
      ListWidget_NotifyScroll(me);
      bInval = 1;
   }

   if (bInval) {
      ListWidget_InvalidateExtent((IDecorator*)me, 0);
   }

   return TRUE;
}

void GridWidget_GetPreferredExtent(IDecorator *po, WExtent *pweOut)
{
   ME_FROM_DECORATOR;
   
   WExtent we;

   we.width  = me->dxPrefItem * me->nCols;
   we.height = me->dyPrefItem * me->nRows;

   Border_CalcPreferredExtent(&me->border, pweOut, &we);   
}


void GridWidget_Ctor(ListWidget *me, AEEVTBL(IDecorator) *pvt, IListModel *piModel, IShell *piShell, 
                     IModule *piModule, PFNHANDLER pfnDefHandler)
{
   ListWidget_Ctor((ListWidget*)me, pvt, piModel, piShell, piModule, pfnDefHandler);

   // override the class id
   me->nOrientation  = LWO_GRID;
   me->nRows         = 3;
   me->nCols         = 3;
   me->pfnMove       = GridWidget_Move;
   
   // and some interface methods
   pvt->GetPreferredExtent = GridWidget_GetPreferredExtent;
}


int GridWidget_New(IDecorator **ppo, IListModel *piListModel, IShell *piShell, IModule *piModule)
{
   return ListWidget_NewEx(ppo, piListModel, GridWidget_Ctor, piShell, piModule, 0);
}


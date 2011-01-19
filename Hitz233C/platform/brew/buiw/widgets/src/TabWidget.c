/*
  ========================================================================

  FILE:  TabWidget.c
  
  SERVICES:  

  GENERAL DESCRIPTION: Definition for tabbed style decorator widget

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#include "TabWidget.h"
#include "wutil.h"
#include "ModelBase.h"
#include "VectorModel.h"
#include "AEEImageStaticWidget.h"
#include "widget_res.h"
#include "AEEDisplayCanvas.h"
#include "AEECardContainer.h"

/*
   TabWidget is similar to a ListWidget in that it uses a ListModel and an
   ImageStaticWidget for the 'item' drawing. The ListModel must contain
   pointers to ImageStaticInfo structs to define the ImageStatic data. 
   
   TabWidget is also a decorator and exposes the IDecorator interface.
*/


#define TAB_LEDGE       2
#define TABSPACE_CX     1
#define TABSPACE_CY     1
#define TAB_DEFPAD      2

/////////////////////////////////////////////////////////////////////////////
//  macros, types and inlines

static __inline TabWidget *DECORATOR_TO_TABWIDGET(IDecorator *po) {
   return (TabWidget*)po;
}
static __inline TabWidget *CONTAINER_TO_TABWIDGET(IContainer *po) {
   return (TabWidget*)po->pMe;
}

#define TAB_FROM_DECORATOR TabWidget *me = DECORATOR_TO_TABWIDGET(po)
#define TAB_FROM_CONTAINER TabWidget *me = CONTAINER_TO_TABWIDGET(po) 


// callback for TabWidget_Indexer()
// return TRUE to continue, FALSE to stop
typedef boolean (*PFNINTERNALINDEXER) (TabWidget *me, int nIndex, void *pv);


#define MYEXTENT(p)     WBASE(p)->extent
#define MYMODEL(p)      (IListModel*)WBASE(p)->piModel
#define MYRCCLIENT(p)   (p)->base.border.rcClient

static __inline void GetRCClient(TabWidget *me, int xOffset, int yOffset, AEERect *prc) {
   SETAEERECT(prc, MYRCCLIENT(me).x + xOffset, MYRCCLIENT(me).y + yOffset, 
              MYRCCLIENT(me).dx, MYRCCLIENT(me).dy);
}

static __inline void GetRCWidget(TabWidget *me, int xOffset, int yOffset, AEERect *prc) {
   SETAEERECT(prc, xOffset, yOffset, MYEXTENT(me).width, MYEXTENT(me).height);
}

#define RCRIGHT(rc)    (rc.x + rc.dx)
#define RCBOTTOM(rc)   (rc.y + rc.dy)

#define OFFSETRECT(prc, nx, ny)  (prc)->x+=(nx),(prc)->y+=(ny)


static __inline void TabWidget_SetViewModel(TabWidget *me, IModel *piModel);
static void TabWidget_SetFocusPos(TabWidget *me, int index);


/////////////////////////////////////////////////////////////////////////////
// local methods


static int IWIDGET_SetFontFromClass(IWidget *piWidget, uint16 propid, IShell *piShell, AEECLSID clsid)
{
   int result;
   IFont *piFont = 0;

   result = ISHELL_CreateFont(piShell, clsid, &piFont);

   if (result == 0)
      IWIDGET_SetProperty(piWidget, propid, (uint32)piFont);

   RELEASEIF(piFont);
   return result;
}


static void TabBorder_CalcPreferredExtent(Border *me, int cyTabHeight, WExtent *pweOut, WExtent *pweClientIn)
{
   int nShadowOffsetX, nShadowOffsetY;
   Border_GetShadowOffsets(me, -1, &nShadowOffsetX, &nShadowOffsetY);

   pweOut->width = (me->nWidth[1] * 2) + pweClientIn->width + 
                    me->padding.left + me->padding.right + ABS(nShadowOffsetX);

   pweOut->height = (me->nWidth[1] * 2) + pweClientIn->height + cyTabHeight +
                     me->padding.top + me->padding.bottom + ABS(nShadowOffsetY);
}


static int TabWidget_Indexer(TabWidget *me, PFNINTERNALINDEXER pfn, void *pvData)
{
   IValueModel *piValueModel = 0;
   int nErr;
   
   IListModel *piListModel = MYMODEL(me);

   nErr = IWIDGET_GetModel(me->piwItem, AEEIID_VALUEMODEL, (IModel**)&piValueModel);

   if (!nErr && piListModel) {
      
      int i;
      int nModelSize = ILISTMODEL_Size(piListModel);

      for (i = 0; i < nModelSize; i++) {
         void *pv;
         // our built-in "indexer"
         if (SUCCESS == ILISTMODEL_GetAt(piListModel, i, (void**)&pv)) {
            IVALUEMODEL_SetValue(piValueModel, pv, -1, NULL);
         }

         // client installed indexer callback
         if (me->idx.pfnIndexer) {
            me->idx.pfnIndexer(me->idx.pCtx, i, (boolean)(i == me->nFocusPos));
         }

         // our internal indexer to do the work
         if (!pfn(me, i, pvData)) {
            break;
         }
      }
   }

   RELEASEIF(piValueModel);

   return nErr;
}


static __inline void MaxExtent(WExtent *pweResult, WExtent *pwe1, WExtent *pwe2)
{
   pweResult->width = MAX(pwe1->width, pwe2->width);
   pweResult->height = MAX(pwe1->height, pwe2->height);
}

static boolean TabWidget_CalcTabExtentsCB(TabWidget *me, int nIndex, void *pv)
{
   WExtent we;
   uint32 dw;

   // get active/selected extent
   IWIDGET_GetFlags(me->piwItem, &dw);

   if (me->dwFlags & TABWF_SHOWTEXTALWAYS) {
      IWIDGET_SetFontFromClass(me->piwItem, PROP_FONT, me->piShell, AEECLSID_FONTSYSBOLD);
   }
   IWIDGET_SetFlags(me->piwItem, dw & ~ISWF_NOTEXT);
   IWIDGET_SetBorderWidth(me->piwItem, me->base.border.nWidth[RGBINDEX_ACTIVE]);
   IWIDGET_GetPreferredExtent(me->piwItem, &we);

   MaxExtent(&me->weTabA, &me->weTabA, &we);

   if (me->dwFlags & TABWF_SHOWTEXTALWAYS) {
      IWIDGET_SetFlags(me->piwItem, dw & ~ISWF_NOTEXT);
      IWIDGET_SetFontFromClass(me->piwItem, PROP_FONT, me->piShell, AEECLSID_FONTSYSNORMAL);
   } else {
      IWIDGET_SetFlags(me->piwItem, dw | ISWF_NOTEXT);
   }
   IWIDGET_SetBorderWidth(me->piwItem, me->base.border.nWidth[RGBINDEX_INACTIVE]);
   IWIDGET_GetPreferredExtent(me->piwItem, &we);

   MaxExtent(&me->weTabI, &me->weTabI, &we);
   return TRUE;
}

// run an 'indexer' on the ImageStaticWidget and determine the largest height
static void TabWidget_CalcTabExtents(TabWidget *me)
{
   IListModel *piListModel = MYMODEL(me);

   int nTabWidth;
   int nTotalPad;
   
   me->nTabs = ILISTMODEL_Size(piListModel);
   me->nVisTabs = 0;

   if (me->nTabs && MYEXTENT(me).width > 0 && MYEXTENT(me).height > 0) {

      me->nVisTabs = me->nTabs;

      me->weTabA.width  =
      me->weTabI.width  = 0;
      me->weTabA.height = 
      me->weTabI.height = me->cyTabHeight;
   
      // find the largest extents for active and inactive tabs
      TabWidget_Indexer(me, TabWidget_CalcTabExtentsCB, 0);
      if (me->nTabWidthA != 0) {
         me->weTabA.width = me->nTabWidthA;
      }
      if (me->nTabWidthI != 0) {
         me->weTabI.width = me->nTabWidthI;
      }
   
      // make the inactive height equal to the active height minus the y pad
      me->weTabI.height = me->weTabA.height - me->nTabSpacingV;
      me->cyTabHeight = me->weTabA.height;
   
      nTabWidth = me->weTabA.width + ((me->weTabI.width + me->nTabSpacingH) * (me->nVisTabs-1));
      nTotalPad = (MYEXTENT(me).width - (TAB_LEDGE * 2)) - nTabWidth;
      
      if (nTotalPad < 0 && !TILEDIMAGE_HASIMAGE(&me->tiArrow)) {
         TiledImage_LoadResBitmap(&me->tiArrow, me->piShell, "widgets.mif", IDB_ARROWS, 2);
      }
   
      // remove any tabs that don't fit
      while (me->nVisTabs && nTotalPad < 0) {
         me->nVisTabs--;
         nTabWidth = me->weTabA.width + ((me->weTabI.width + me->nTabSpacingH) * (me->nVisTabs-1));
         nTotalPad = MYEXTENT(me).width - (me->tiArrow.cxTile * 2) - nTabWidth;
      }
   
      // if tabs are supposed to fill all available space, 
      //   and Active tab width not specified, add the extra padding 
      //   to the active tab
      if ((me->dwFlags & TABWF_FILLWIDTH) && (nTotalPad > 0) && !me->nTabWidthA) {
         me->weTabA.width += nTotalPad;
      }

   }
}


static void TabWidget_ModelChanged(TabWidget *me, ModelEvent *pEvent)
{
   if (pEvent->evCode == EVT_MDL_LIST_CHANGE) {
      TabWidget_CalcTabExtents(me);
   }
}


/////////////////////////////////////////////////////////////////////////////
// IWidget methods

int TabWidget_SetModel(IDecorator *po, IModel *piModel)
{
   TAB_FROM_DECORATOR;

   IListModel *pilm = NULL;

   if (SUCCESS != IMODEL_QueryInterface(piModel, AEEIID_LISTMODEL, (void **)&pilm)) {
      return ECLASSNOTSUPPORT;
   }
   RELEASEIF(pilm);

   WidgetBase_SetModel(IDECORATOR_TO_IWIDGET(po), (IModel*)piModel);
   IMODEL_AddListenerEx(piModel, &me->listener, (PFNLISTENER)TabWidget_ModelChanged, me);
   TabWidget_CalcTabExtents(me);

   // And invalidate content
   WidgetBase_InvalidateContent(IDECORATOR_TO_IWIDGET(po));

   return SUCCESS;
}

int TabWidget_GetModel(IDecorator *po, AEECLSID iid, IModel **ppiModel)
{
   TAB_FROM_DECORATOR;
   
   if (!MYMODEL(me)) {
      *ppiModel = 0;
      return EFAILED;
   }

   return IQI_QueryInterface((IQueryInterface *)MYMODEL(me), iid, (void**)ppiModel);
}


void TabWidget_SetExtent(IDecorator *po, WExtent *pe)
{
   TAB_FROM_DECORATOR;
   WExtent ce;

   WidgetBase_SetExtent((IWidget*)&me->base.base.base, pe);
   Border_CalcClientRect(&me->base.border);

   // subtract the height of the tab from rcClient
   ce.width  = me->base.border.rcClient.dx;
   ce.height = me->base.border.rcClient.dy - me->cyTabHeight;

   // set the extent of the child
   Decorator_SetExtent(po, &ce);

   TabWidget_CalcTabExtents(me);
}


void TabWidget_SetWidget(IDecorator *po, IWidget *piChild)
{
   TAB_FROM_DECORATOR;
   
   WExtent we;
   IModel *piModel = 0;

   Decorator_SetWidget(po, piChild);
   Decorator_GetExtent(po, &we);

   if (we.width && we.height) {
      TabBorder_CalcPreferredExtent(&me->base.border, me->cyTabHeight, &we, &we);
      TabWidget_SetExtent(po, &we);
   }

   // QI child for ICARDCONTAINER, then find index of active CardContainer widget
   {
      int nn = 0;
      ICardContainer *picc = 0;
      if (SUCCESS == IWIDGET_QueryInterface(piChild, AEEIID_CARDCONTAINER, (void**)&picc)) {
         IWidget *piw = 0;
         IWidget *piwa = ICARDCONTAINER_GetActive(picc);
         for (;;nn++) {
            piw = ICARDCONTAINER_GetWidget(picc, piw, 1, 0);
            if (!piw || piw == piwa) {
               break;
            }
         }
      }
      RELEASEIF(picc);
      // set tab index to correspond to active widget in cc
      TabWidget_SetFocusPos(me, nn);
   }

   // listen on the child's view model
   IWIDGET_GetViewModel(piChild, &piModel);
   if (piModel) {
      TabWidget_SetViewModel(me, piModel);
   }
   RELEASEIF(piModel);

   // connect the view model back the other way
   WidgetBase_GetViewModel(WBASE(me), &piModel);
   if (piModel) {
      IWIDGET_SetViewModel(piChild, piModel);
   }


   RELEASEIF(piModel);
}


void TabWidget_GetExtent(IDecorator *po, WExtent *pe)
{
   // Don't ask child, we control the extent
   WidgetBase_GetExtent((IWidget*)po, pe);
}


static void TabWidget_GetPreferredExtent(IDecorator *po, WExtent *pweOut)
{
   TAB_FROM_DECORATOR;
   WExtent we;

   Decorator_GetExtent(po, &we);
   TabBorder_CalcPreferredExtent(&me->base.border, me->cyTabHeight, pweOut, &we);
}

void TabWidget_Invalidate(IContainer *po, IWidget *piw, const AEERect *prcIn, uint32 dwFlags)
{
   TAB_FROM_CONTAINER;
   AEERect rc;

   SETAEERECT(&rc, 0, me->cyTabHeight, 
              WBASE(me)->extent.width, 
              WBASE(me)->extent.height - me->cyTabHeight);

   if (prcIn) {
      rc.x += prcIn->x;
      rc.y += prcIn->y;
      rc.dx = prcIn->dx;
      rc.dy = prcIn->dy;
   }
   BorderWidget_Invalidate(po, piw, &rc, dwFlags);
}

int TabWidget_Locate(IContainer *po, IWidget *piw, IContainer **ppRoot, AEERect *prcRel)
{
   TAB_FROM_CONTAINER;

   prcRel->y += me->cyTabHeight;

   return BorderWidget_Locate(po, piw, ppRoot, prcRel);
}



/////////////////////////////////////////////////////////////////////////////
// Drawing

typedef struct {
   int         x;
   int         y;
   ICanvas *   piCanvas;
} DrawContext;


static __inline boolean TabWidget_DrawTab(TabWidget *me, int nIndex, DrawContext *pdc)
{
   uint32 dw;
   WExtent we;
   AEERect rcTab, rcDisp;
   boolean bFocused = nIndex == me->nFocusPos;
   int rgbIndex;
   Border *pBorder = &me->base.border;
   RGBVAL rgba;

   if (nIndex < me->nScrollPos) {
      return TRUE;
   }

   IWIDGET_GetFlags(me->piwItem, &dw);

   if (bFocused) {
      rgbIndex = RGBINDEX_ACTIVE;
      dw &= ~ISWF_NOTEXT;
      if (me->dwFlags & TABWF_SHOWTEXTALWAYS) {
         IWIDGET_SetFontFromClass(me->piwItem, PROP_FONT, me->piShell, AEECLSID_FONTSYSBOLD);
      }
      we = me->weTabA;
   } else {
      rgbIndex = RGBINDEX_INACTIVE;
      if (me->dwFlags & TABWF_SHOWTEXTALWAYS) {
         dw &= ~ISWF_NOTEXT;
         IWIDGET_SetFontFromClass(me->piwItem, PROP_FONT, me->piShell, AEECLSID_FONTSYSNORMAL);
      } else {
         dw |= ISWF_NOTEXT;
      }
      we = me->weTabI;
   }

   IWIDGET_SetFlags(me->piwItem, dw|IDF_ALIGN_MIDDLE);
   IWIDGET_SetSelected(me->piwItem, bFocused);
   
   Border_GetBorderColor(pBorder, rgbIndex, &rgba);
   IWIDGET_SetBorderColor(me->piwItem, rgba);
   IWIDGET_SetBorderWidth(me->piwItem, pBorder->nWidth[rgbIndex]);
   
   Border_GetBGColor(pBorder, rgbIndex, &rgba);
   IWIDGET_SetBGColor(me->piwItem, rgba);

   SETAEERECT(&rcTab, pdc->x, pdc->y, we.width, we.height);

   if (bFocused) {

      WExtent wePref;

      we.height += pBorder->nWidth[RGBINDEX_ACTIVE];
      me->rcSelTab = rcTab;
      me->rcSelTab.dy += pBorder->nWidth[RGBINDEX_ACTIVE];

      // center the image & static by adding left padding
      IWIDGET_SetLeftPadding(me->piwItem, 0);
      IWIDGET_GetPreferredExtent(me->piwItem, &wePref);
      if (we.width > wePref.width) {
         int pad = (we.width - wePref.width + 1) / 2;
         IWIDGET_SetLeftPadding(me->piwItem, TAB_DEFPAD + pad);
      }
   }

   IWIDGET_SetExtent(me->piwItem, &we);

   IntersectRect(&rcDisp, &rcTab, &me->rcClip);
   ICANVAS_SetClipRect(pdc->piCanvas, &rcDisp);

   // draw the item widget (tab)
   IWIDGET_Draw(me->piwItem, pdc->piCanvas, pdc->x, pdc->y);

   // reset the padding to default (unfocused)
   IWIDGET_SetLeftPadding(me->piwItem, TAB_DEFPAD);
   IWIDGET_SetTopPadding(me->piwItem, 2);

   pdc->x += we.width + me->nTabSpacingH;

   // continue only if more visible tabs left to draw
   return (nIndex < me->nScrollPos + me->nVisTabs-1);
}




void TabWidget_Draw(IDecorator *po, ICanvas *piCanvas, int x, int y)
{
   TAB_FROM_DECORATOR;

   DrawContext dc;
   AEERect rc, rcDisp;
   RGBVAL rgbSave;
   int nActiveBorderWidth;
   Border *pBorder = &me->base.border;
   boolean bScroll = (me->nVisTabs < me->nTabs);
   IDisplay *piDisplay = 0;

   ICANVAS_GetClipRect(piCanvas, &me->rcClip);

   // force widget border to be active
   pBorder->bActive = 1;

   // get the active and inactive border widths
   nActiveBorderWidth = pBorder->nWidth[RGBINDEX_ACTIVE];

   // override the drawing of the border
   Border_GetBorderColor(pBorder, RGBINDEX_ACTIVE, &rgbSave);
   if (!(me->dwFlags & TABWF_SHOWBORDER)) {
      Border_SetBorderColor(pBorder, RGBINDEX_ACTIVE, RGBA_NONE);
   }

   GetRCWidget(me, x, y, &rc);
   // clip the tab area when drawing the border
   rc.y  += me->cyTabHeight;
   rc.dy -= me->cyTabHeight;
   IntersectRect(&rcDisp, &me->rcClip, &rc);
   ICANVAS_SetClipRect(piCanvas, &rcDisp);
   Border_Draw(&me->base.border, piCanvas, x, y);

   // restore the border color
   Border_SetBorderColor(pBorder, RGBINDEX_ACTIVE, rgbSave);

   // get full widget Rect
   GetRCWidget(me, x, y, &rc);
   IntersectRect(&rcDisp, &me->rcClip, &rc);
   ICANVAS_SetClipRect(piCanvas, &rcDisp);

   dc.x         = rc.x + TAB_LEDGE;
   dc.y         = rc.y;
   dc.piCanvas  = piCanvas;

   if (bScroll) {
      dc.x = rc.x + me->tiArrow.cxTile;
   }
      
   // index through the tabs and draw each
   TabWidget_Indexer(me, (PFNINTERNALINDEXER)TabWidget_DrawTab, &dc);
   
   if (SUCCESS == ICANVAS_GetDisplay(piCanvas, &piDisplay)) {

      // draw connecting lines between tabs and child
      int nAlpha;
      RGBVAL rgba;
      AEERect rcLine;

      Border_GetBorderColor(pBorder, RGBINDEX_ACTIVE, &rgba);
      nAlpha = RGBA_GETALPHA(rgba);

      // restore the clip rect
      IDISPLAY_SetClipRect(piDisplay, &me->rcClip);

      if (me->rcSelTab.x > 0) {
         rcLine.x = rc.x;
         rcLine.y = RCBOTTOM(me->rcSelTab) - nActiveBorderWidth;
         rcLine.dx = me->rcSelTab.x - rc.x + nActiveBorderWidth;
         rcLine.dy = nActiveBorderWidth;
         BlendRect(piDisplay, &rcLine, rgba, nAlpha);
      }
      if (RCRIGHT(me->rcSelTab) < RCRIGHT(rc)) {
         rcLine.x = RCRIGHT(me->rcSelTab) - nActiveBorderWidth;
         rcLine.y = RCBOTTOM(me->rcSelTab) - nActiveBorderWidth;
         rcLine.dx = RCRIGHT(rc) - RCRIGHT(me->rcSelTab) + nActiveBorderWidth;
         rcLine.dy = nActiveBorderWidth;
         BlendRect(piDisplay, &rcLine, rgba, nAlpha);
      }
   
      // draw left arrow
      if (bScroll && (me->nScrollPos > 0)) {
         TiledImage_DrawTileEx(&me->tiArrow, piDisplay, rc.x, y, 0, 0, me->weTabI.height, IDF_ALIGN_MIDDLE);
      }
      // draw right arrow
      if (bScroll && (me->nScrollPos < me->nTabs - me->nVisTabs)) {
         TiledImage_DrawTileEx(&me->tiArrow, piDisplay, rc.x, y, 1, rc.dx, me->weTabI.height, 
                               IDF_ALIGN_MIDDLE|IDF_ALIGN_RIGHT);
      }
   
      // clip the tab area when drawing the child
      {
         AEERect rcClip;
         GetRCClient(me, x, y, &rc);
         rc.y  += me->cyTabHeight;
         rc.dy -= me->cyTabHeight;
         IntersectRect(&rcClip, &rc, &me->rcClip);
         ICANVAS_SetClipRect(piCanvas, &rcClip);
      }
      
      Decorator_Draw(po, piCanvas, rc.x, rc.y);
   }

   RELEASEIF(piDisplay);
}


/////////////////////////////////////////////////////////////////////////////
// Focus model stuff

static void TabWidget_onFocusChange(TabWidget *me, ModelEvent *pEvent)
{
   if (pEvent->evCode == EVT_MDL_TAB_CHANGE) {
      TabWidget_SetFocusPos(me, (int)pEvent->dwParam);
      WidgetBase_InvalidateContent((IWidget*)me);
   }
}

static void TabWidget_NotifyFocus(TabWidget *me)
{
   if (WBASE(me)->piViewModel) {
      ModelEvent ev;
      ev.evCode   = EVT_MDL_TAB_CHANGE;
      ev.dwParam  = (uint32)me->nFocusPos;
      IMODEL_Notify(WBASE(me)->piViewModel, &ev);
   }
}

// This implementation of SetViewModel only hooks a listener onto the 
// supplied view model. Our internal view model is used for notifications
static __inline void TabWidget_SetViewModel(TabWidget *me, IModel *piModel)
{
   if (piModel) {
      IMODEL_AddListenerEx(piModel, &me->mlFocus, (PFNLISTENER)TabWidget_onFocusChange, me);
   }
}

/////////////////////////////////////////////////////////////////////////////

static void TabWidget_SetHasSelectImage(TabWidget *me, boolean bOn)
{
   IWidget *piwImage = 0;
   int nErr = IWIDGET_GetImageStaticImageWidget(me->piwItem, &piwImage);

   if (!nErr) {
      uint32 dwFlags;
      IWIDGET_GetFlags(piwImage, &dwFlags);
      if (bOn) {
         dwFlags |= IWF_HASSELECTIMAGE;
      } else {
         dwFlags &= ~IWF_HASSELECTIMAGE;
      }
      IWIDGET_SetFlags(piwImage, dwFlags);
   }

   RELEASEIF(piwImage);

   // update the tab extents
   TabWidget_CalcTabExtents(me);
}


static void TabWidget_SetFocusPos(TabWidget *me, int index)
{
   if (index >= 0 && index < me->nTabs) {

      if (index != me->nFocusPos) {
         me->nFocusPos = index;
         TabWidget_NotifyFocus(me);
      }

      if (me->nVisTabs > 0) {
         int nScrollSpan = index - me->nScrollPos;
         if (nScrollSpan > me->nVisTabs-1) {
            me->nScrollPos = index - (me->nVisTabs-1);
         } else if (nScrollSpan < 0) {
            me->nScrollPos = index;
         }
      }
   }
}

/////////////////////////////////////////////////////////////////////////////


boolean TabWidget_HandleEvent(IDecorator *po, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   TAB_FROM_DECORATOR;


   if (evt == EVT_WDG_CANTAKEFOCUS) {
      *((boolean *)dwParam) = 1;    // we always take focus
      return TRUE;

   } else if (evt == EVT_KEY && (wParam == AVK_LEFT || wParam == AVK_RIGHT)) {

      IListModel *piListModel = MYMODEL(me);

      if (piListModel) {
         int size = ILISTMODEL_Size(piListModel);
         int index = me->nFocusPos + ((wParam == AVK_RIGHT) ? 1 : -1);
         if (me->dwFlags & TABWF_WRAP) {
            if (index < 0) {
               index = size-1;
            } else if (index > size-1) {
               index = 0;
            }
         } else {
            index = CONSTRAIN(index, 0, size-1);
         }

         TabWidget_SetFocusPos(me, index);
         WidgetBase_InvalidateContent(IDECORATOR_TO_IWIDGET(po));
      }
      return TRUE;
   }

   if (evt == EVT_WDG_GETPROPERTY) {

      if (wParam == PROP_TABSPACING_HORZ) {
         *((uint32*)dwParam) = me->nTabSpacingH;
         return TRUE;

      } else if (wParam == PROP_TABSPACING_VERT) {
         *((uint32*)dwParam) = me->nTabSpacingV;
         return TRUE;
      
      } else if (wParam == PROP_FOCUSINDEX) {
         *((uint32*)dwParam) = (uint32)me->nFocusPos;
         return TRUE;

      } else if (wParam == PROP_ACTIVE_TABWIDTH) {
         *((uint32*)dwParam) = (uint32)me->nTabWidthA;
         return TRUE;
   
      } else if (wParam == PROP_INACTIVE_TABWIDTH) {
         *((uint32*)dwParam) = (uint32)me->nTabWidthI;
         return TRUE;

      } else if (wParam == PROP_TABHEIGHT) {
         *((uint32*)dwParam) = (uint32)me->cyTabHeight;
         return TRUE;
      }

   }
   
   if (evt == EVT_WDG_SETPROPERTY) {

      if (wParam == PROP_VIEWMODEL) {
         // listen on the supplied view model
         TabWidget_SetViewModel(me, (IModel*)dwParam);
         return TRUE;
      
      } else if (wParam == PROP_FLAGS) {

         uint32 dwDiff = me->dwFlags ^ dwParam;

         me->dwFlags = dwParam;

         if (dwDiff & TABWF_HASSELECTIMAGE) {
            TabWidget_SetHasSelectImage(me, (boolean)(me->dwFlags & TABWF_HASSELECTIMAGE));
         }

         WidgetBase_InvalidateContent(IDECORATOR_TO_IWIDGET(po));
         return TRUE;
     
      } else if (wParam == PROP_INDEXER) {
         me->idx = *((IndexerDesc*)dwParam);
         return TRUE;

      } else if (wParam == PROP_TABSPACING_HORZ) {
         me->nTabSpacingH = dwParam;
         TabWidget_CalcTabExtents(me);
         WidgetBase_InvalidateContent((IWidget*)me);
         return TRUE;

      } else if (wParam == PROP_TABSPACING_VERT) {
         me->nTabSpacingV = dwParam;
         TabWidget_CalcTabExtents(me);
         WidgetBase_InvalidateContent((IWidget*)me);
         return TRUE;
      
      } else if (wParam == PROP_FOCUSINDEX) {
         TabWidget_SetFocusPos(me, (int)dwParam);
         WidgetBase_InvalidateContent((IWidget*)me);
         return TRUE;
      
      } else if (wParam == PROP_ACTIVE_TABWIDTH) {
         me->nTabWidthA = (int)dwParam;
         TabWidget_CalcTabExtents(me);
         WidgetBase_InvalidateContent((IWidget*)me);
         return TRUE;
      
      } else if (wParam == PROP_INACTIVE_TABWIDTH) {
         me->nTabWidthI = (int)dwParam;
         TabWidget_CalcTabExtents(me);
         WidgetBase_InvalidateContent((IWidget*)me);
         return TRUE;

      } else if (wParam == PROP_TABHEIGHT) {
         me->cyTabHeight = (int)dwParam;
         TabWidget_CalcTabExtents(me);
         WidgetBase_InvalidateContent((IWidget*)me);
         return TRUE;
      }

   }

   return BorderWidget_HandleEvent(po, evt, wParam, dwParam);
}


static void TabWidget_Dtor(TabWidget *me)
{
   LISTENER_Cancel(&me->listener);
   LISTENER_Cancel(&me->mlFocus);
   CALLBACK_Cancel(&me->cbkFocus);

   RELEASEIF(me->piShell);
   RELEASEIF(me->piwItem);

   TiledImage_Dtor(&me->tiArrow);
   BorderWidget_Dtor(&me->base);
}

static void TabWidget_Delete(TabWidget *me)
{
   TabWidget_Dtor(me);
   FREE(me);
}

uint32 TabWidget_Release(IDecorator *po)
{
   TAB_FROM_DECORATOR;

   if (WBASE(me)->nRefs == 1) {
      TabWidget_Dtor(me);
   }

   return Decorator_Release(po);
}



int TabWidget_Construct(TabWidget *me, AEEVTBL(IDecorator) *pvt, IShell *piShell, 
                        IModule *piModule, PFNHANDLER pfnDefHandler)
{
   int nErr = 0;
   IListModel *piListModel = 0;

   BorderWidget_Ctor(&me->base, pvt, piShell, piModule, DEFHANDLER(TabWidget_HandleEvent)); 

   // override some interface methods
   pvt->Release            = TabWidget_Release;
   pvt->Draw               = TabWidget_Draw;
   pvt->GetPreferredExtent = TabWidget_GetPreferredExtent;
   pvt->GetExtent          = TabWidget_GetExtent;
   pvt->SetExtent          = TabWidget_SetExtent;
   pvt->SetWidget          = TabWidget_SetWidget;
   pvt->SetModel           = TabWidget_SetModel;
   pvt->GetModel           = TabWidget_GetModel;

   WCBASE(me)->vtContainer.Invalidate = TabWidget_Invalidate;
   WCBASE(me)->vtContainer.Locate     = TabWidget_Locate;

   me->base.border.bCanTakeFocus        = TRUE;

   me->cyTabHeight = 20;
   me->nTabSpacingH = TABSPACE_CX;
   me->nTabSpacingV = TABSPACE_CY;

   me->piShell = piShell;
   ISHELL_AddRef(me->piShell);

   if (!nErr) {
      nErr = VectorModel_New((IVectorModel**)&piListModel, piModule);
   }

   if (!nErr) {
      TabWidget_SetModel((IDecorator*)me, (IModel*)piListModel);
   }

   if (!nErr) {
      nErr = ISHELL_CreateInstance(piShell, AEECLSID_IMAGESTATICWIDGET, (void**)&me->piwItem);
   }

   if (!nErr) {
      IWIDGET_SetFontFromClass(me->piwItem, PROP_FONT, me->piShell, AEECLSID_FONTSYSBOLD);
      IWIDGET_SetBorderWidth(me->piwItem, 1);
      IWIDGET_SetPadding(me->piwItem, TAB_DEFPAD);
      IWIDGET_SetTopPadding(me->piwItem, 2);
      IWIDGET_SetBottomPadding(me->piwItem, 1);
   }

   RELEASEIF(piListModel);

   return nErr;
}


int TabWidget_New(IDecorator **ppo, IShell *piShell, IModule *piModule)
{
   int nErr = 0;
   TabWidget *me = MALLOCREC_VTBL(TabWidget, IDecorator);

   if (!me) {
      return ENOMEMORY;
   }

   nErr = TabWidget_Construct(me, GETVTBL(me,IDecorator), piShell, piModule, 0);

   if (!nErr) {
      *ppo = (IDecorator*)me;
   } else {
      TabWidget_Delete(me);
      *ppo = 0;
   }

   return nErr;
}


/*
  ========================================================================

  FILE:  PropContainer.c
  
  SERVICES:  

  GENERAL DESCRIPTION: Implementation of a proportional container

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#include "PropContainer.h"
#include "AEEWProperties.h"
#include "wutil.h"

typedef struct PropWidgetNode {
   WidgetNode base;
   uint16     prop;
   int16      wBefore;
   int16      wAfter;
   uint16     wBeforePixels;
   uint16     wAfterPixels;
} PropWidgetNode;

static __inline int PropWidgetNode_FindWidget(WidgetNode *head, IWidget *piw, PropWidgetNode **ppo) {
   return WidgetNode_FindWidget(head, piw, (WidgetNode **)ppo);
}

#define PROP_FROM_CONTAINER PropContainer *me = (PropContainer *)po
#define BASE_FROM_CONTAINER ContainerBase *me = (ContainerBase *)po
#define PROP_FROM_WIDGET    PropContainer *me = (PropContainer *)po->pMe

static void PropContainer_doLayout(ContainerBase *me, WidgetNode *pn, AEERect *prc)
{
   int nFixed = 0;
   int nTotalProp = 0;
   int nFree;
   AEERect rc;
   WExtent extent;
   WidgetNode *wn, *wnLastVisible;
   AEERect rcLastVisible;

#define MINOREXTENT(rc) (((PropContainer*)me)->fVertical ? rc.dx : rc.dy)
#define SET_MINOREXTENT(rc,v) if (((PropContainer*)me)->fVertical) rc.dx = v; else rc.dy = v
#define MAJOREXTENT(rc) (((PropContainer*)me)->fVertical ? rc.dy : rc.dx)
#define SET_MAJOREXTENT(rc,v) if (((PropContainer*)me)->fVertical) rc.dy = v; else rc.dx = v
#define MAJORCOORD(rc) (((PropContainer*)me)->fVertical ? rc.y : rc.x)
#define SET_MAJORCOORD(rc,v) if (((PropContainer*)me)->fVertical) rc.y = v; else rc.x = v
#define MINORCOORD(rc) (((PropContainer*)me)->fVertical ? rc.x : rc.y)
#define SET_MINORCOORD(rc,v) if (((PropContainer*)me)->fVertical) rc.x = v; else rc.y = v
#define PNODE(wn) ((PropWidgetNode *)wn)
#define IS_EXACT_PADDING(x) (x < 0)
#define IS_PROPORTIONAL_PADDING(x) (x > 0)

   SETAEERECT(&rc, 0, 0, CBASE(me)->border.rcClient.dx, CBASE(me)->border.rcClient.dy);

   if (prc) {
      *prc = me->border.rcClient;
   }

   // No layout required on invisible container
   if (ISRECTEMPTY(&rc))
      return;

   // Block out invalidates from children
   CBASE(me)->fInLayout = 1;

   WNODE_ENUM_FWD(wn, &CBASE(me)->head) {
      if (wn->fVisible) {
         
         // recalculate padding
         PNODE(wn)->wBeforePixels = PNODE(wn)->wAfterPixels = 0;

         // leading padding
         if (IS_EXACT_PADDING(PNODE(wn)->wBefore)) {
            PNODE(wn)->wBeforePixels = -(PNODE(wn)->wBefore);
            nFixed += PNODE(wn)->wBeforePixels;
         } else if (IS_PROPORTIONAL_PADDING(PNODE(wn)->wBefore)) {
            nTotalProp += PNODE(wn)->wBefore;
         }

         // the widget itself ...
         switch (PNODE(wn)->prop)
         {
         case WIDGET_SIZE_TO_FIT:
            SETWEXTENT(&extent, rc.dx, rc.dy);
            IWIDGET_SetExtent(wn->piWidget, &extent);
            IWIDGET_GetPreferredExtent(wn->piWidget, &extent);
            if (((PropContainer*)me)->fVertical) {
               extent.height = MIN(extent.height, rc.dy - nFixed); 
            } else {
               extent.width = MIN(extent.width, rc.dx - nFixed);
            }
            IWIDGET_SetExtent(wn->piWidget, &extent);
            SETAEERECT(&wn->rc, wn->rc.x, wn->rc.y, extent.width, extent.height);
            nFixed += MAJOREXTENT(wn->rc);
            break;
         case WIDGET_KEEP_EXTENT:
            IWIDGET_GetExtent(wn->piWidget, &extent);
            SETAEERECT(&wn->rc, wn->rc.x, wn->rc.y, extent.width, extent.height);
            nFixed += MAJOREXTENT(wn->rc);
            break;
         default:
            nTotalProp += PNODE(wn)->prop;
            break;
         }

         // trailing padding ...
         if (IS_EXACT_PADDING(PNODE(wn)->wAfter)) {
            PNODE(wn)->wAfterPixels = -(PNODE(wn)->wAfter);
            nFixed += PNODE(wn)->wAfterPixels;
         } else if (IS_PROPORTIONAL_PADDING(PNODE(wn)->wAfter)) {
            nTotalProp += PNODE(wn)->wAfter;
         }
      }
   }

   nFree = MAJOREXTENT(rc) - nFixed;
   if (nFree < 0)
      nFree = 0;

   wnLastVisible = &CBASE(me)->head;
   if (wnLastVisible) {  
      rcLastVisible = wnLastVisible->rc;
   }

   WNODE_ENUM_FWD(wn, &CBASE(me)->head) {
      int majorDim;

      // Ignore invisible nodes
      if (!wn->fVisible)
         continue;

      // calculate proportional padding if needed ...
      if (IS_PROPORTIONAL_PADDING(PNODE(wn)->wBefore)) {
         PNODE(wn)->wBeforePixels = (PNODE(wn)->wBefore * nFree) / (nTotalProp ? nTotalProp : 1);
      }
      if (IS_PROPORTIONAL_PADDING(PNODE(wn)->wAfter)) {
         PNODE(wn)->wAfterPixels = (PNODE(wn)->wAfter * nFree) / (nTotalProp ? nTotalProp : 1);
      }

      if (PNODE(wn)->prop == WIDGET_KEEP_EXTENT || PNODE(wn)->prop == WIDGET_SIZE_TO_FIT) {
         // Fixed size
         majorDim = MAJOREXTENT(wn->rc);
      } else if (wn == WNODE_ZTOP(&CBASE(me)->head)) {
         // Last element gets all the leftovers, except for trailing padding
         majorDim = MAJOREXTENT(rc) - MAJORCOORD(rcLastVisible) - MAJOREXTENT(rcLastVisible) - PNODE(wn)->wAfterPixels;
      } else {
         // give proportion of free space 
         majorDim = (PNODE(wn)->prop * nFree) / (nTotalProp ? nTotalProp : 1);
      }
      
      SET_MAJOREXTENT(wn->rc, majorDim);
      SET_MINOREXTENT(wn->rc, MINOREXTENT(rc));
      SET_MAJORCOORD(wn->rc,  MAJORCOORD(rcLastVisible) + MAJOREXTENT(rcLastVisible) + PNODE(wn)->wBeforePixels);
      SET_MINORCOORD(wn->rc,  0);
      
      SETWEXTENT(&extent, wn->rc.dx, wn->rc.dy);
      IWIDGET_SetExtent(wn->piWidget, &extent);

      rcLastVisible = wn->rc;
      SET_MAJORCOORD(rcLastVisible, MAJORCOORD(rcLastVisible) + PNODE(wn)->wAfterPixels);
   }

   // Allow children invalidates
   CBASE(me)->fInLayout = 0;

}

static int WidgetNode_ForProp(const WidgetProp *prop, IWidget *piw, WidgetNode **ppo)
{
   WExtent extent;
   AEERect rc;
   PropWidgetNode *node = MALLOCREC(PropWidgetNode);
   boolean bVisible = TRUE;

   *ppo = &node->base;

   if (!node)
      return ENOMEMORY;

   IWIDGET_GetExtent(piw, &extent);
   SETAEERECT(&rc, 0, 0, extent.width, extent.height);

   if (prop) {
      node->prop = prop->prop;
      bVisible = prop->bVisible;
   }
   else
      node->prop = WIDGET_KEEP_EXTENT;

   WidgetNode_Ctor(&node->base, piw, &rc, bVisible);
   return SUCCESS;
}

int PropContainer_SetProp(IPropContainer *po, IWidget *piw, IWidget *piwBefore, const WidgetProp *prop)
{
   PROP_FROM_CONTAINER;
   PropWidgetNode *node;
   PropWidgetNode *insertNode;

   if (PropWidgetNode_FindWidget(&CBASE(me)->head, piw, &node) != 0)
      return EBADPARM;

   if (piwBefore == WIDGET_ZTOPMOST)
      insertNode = (PropWidgetNode*)&CBASE(me)->head;
   else if (PropWidgetNode_FindWidget(&CBASE(me)->head, piwBefore, &insertNode) != 0)
      return EBADPARM;

   // if it's the same prop and there's no stack reordering, we're done.
   if (node->prop == prop->prop
       && node->base.fVisible == prop->bVisible
       && (node == insertNode || piwBefore == WIDGET_ZNORMAL))
      return SUCCESS;

   node->prop = prop->prop;
   node->base.fVisible = prop->bVisible;
   
   if (piwBefore != WIDGET_ZNORMAL && node != insertNode) {
      WNODE_REMOVE((WidgetNode *)node);
      WNODE_INSERTBEFORE((WidgetNode *)insertNode, (WidgetNode *)node);
   }
   
   ContainerBase_LayoutInvalidate((ContainerBase*)me);
   return 0;
}

int PropContainer_GetProp(IPropContainer *po, IWidget *piw, WidgetProp *prop)
{
   BASE_FROM_CONTAINER;
   PropWidgetNode *node;

   if (PropWidgetNode_FindWidget(&me->head, piw, &node) != 0)
      return EBADPARM;

   prop->bVisible = (boolean)node->base.fVisible;
   prop->prop = node->prop;
   
   return SUCCESS;
}

int PropContainer_SetPadding(IPropContainer *po, IWidget *piw, int32 dwBefore, int32 dwAfter)
{
   BASE_FROM_CONTAINER;
   PropWidgetNode *node;

   if (PropWidgetNode_FindWidget(&me->head, piw, &node) != 0)
      return EBADPARM;

   node->wBefore = (int16)dwBefore;
   node->wAfter = (int16)dwAfter;
   
   ContainerBase_LayoutInvalidate(me);
   return SUCCESS;
}

int PropContainer_GetPadding(IPropContainer *po, IWidget *piw, int32 *pdwBefore, int32 *pdwAfter)
{
   BASE_FROM_CONTAINER;
   PropWidgetNode *node;

   if (PropWidgetNode_FindWidget(&me->head, piw, &node) != 0)
      return EBADPARM;

   if (pdwBefore) {
      *pdwBefore = (int32)node->wBefore;
   }
   if (pdwAfter) {
      *pdwAfter = (int32)node->wAfter;
   }
   
   return SUCCESS;
}


/////////////////////////////////////////////////////////////////
// IContainer
int PropContainer_QueryInterface(IPropContainer *po, AEECLSID id, void **ppo)
{
   if ((id == AEEIID_PROPCONTAINER) || (id == AEEIID_PROPCONTAINER_1)) {
      *ppo = po;
      ICONTAINER_AddRef(IPROPCONTAINER_TO_ICONTAINER(po));
      return SUCCESS;
   }
   return ContainerBase_QueryInterface(IPROPCONTAINER_TO_ICONTAINER(po), id, ppo);
}

void PropContainer_Invalidate(IPropContainer *po, IWidget *piw, const AEERect *prcInWidget, uint32 dwFlags)
{
   BASE_FROM_CONTAINER;
   PropWidgetNode *node;

   if (me->fInLayout)
      return;

   if (0 == (dwFlags & (ICIF_EXTENT|ICIF_REDRAW)) 
       && SUCCESS == PropWidgetNode_FindWidget(&CBASE(me)->head, piw, &node) 
       && (node->prop == WIDGET_SIZE_TO_FIT)) {

      dwFlags |= ICIF_EXTENT;
   }
   

   ContainerBase_Invalidate(IPROPCONTAINER_TO_ICONTAINER(po), piw, prcInWidget, dwFlags);
}

/////////////////////////////////////////////////////////////////
// IWidget
void PropContainer_GetPreferredExtent(IWidget *po, WExtent *pe)
{
   PROP_FROM_WIDGET;
   WidgetNode *wn;
   WExtent extent;

   SETWEXTENT(pe, 0, 0);

   WNODE_ENUM_FWD(wn, &CBASE(me)->head) {

      // Ignore invisible nodes
      if (!wn->fVisible || !wn->piWidget)
         continue;

      if (PNODE(wn)->prop == WIDGET_KEEP_EXTENT)
         IWIDGET_GetExtent(wn->piWidget, &extent);
      else
         IWIDGET_GetPreferredExtent(wn->piWidget, &extent);

      if (me->fVertical) {
         pe->width = MAX(pe->width, extent.width);    // get maximum width
         pe->height += extent.height;        // add up heights of all widgets
         // add the padding before and after the widget 
         pe->height += PNODE(wn)->wBeforePixels + PNODE(wn)->wAfterPixels;    
      } else {
         pe->width += extent.width;          // add up widths of all widgets
         // add the padding before and after the widget 
         pe->width += PNODE(wn)->wBeforePixels + PNODE(wn)->wAfterPixels;   
         pe->height = MAX(pe->height, extent.height); // get maximum height
      }
   }
   
   Border_CalcPreferredExtent(&CBASE(me)->border, pe, pe);
   
}

boolean PropContainer_DefHandleEvent(IWidget *po, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   PROP_FROM_WIDGET;

   AEERect *prcClient = &me->base.border.rcClient;
   AEERect rcSave = *prcClient;

   if (ContainerBase_DefHandleEvent(po, evt, wParam, dwParam)) {
      // need to do a layout if the client rectangle changed
      if (rcSave.dx != prcClient->dx ||
          rcSave.dy != prcClient->dy) {
         ContainerBase_LayoutInvalidate((ContainerBase*)me);
      }
      return TRUE;
   }

   if (evt == EVT_WDG_SETPROPERTY) {
      switch(wParam) {
      case PROP_LAYOUTSTYLE:
         me->fVertical = (dwParam == LAYOUT_VERT);
         ContainerBase_LayoutInvalidate((ContainerBase*)me);
         return TRUE;
      }
   } else if (evt == EVT_WDG_GETPROPERTY) {
      switch(wParam) {
      case PROP_LAYOUTSTYLE:
         *(int *)dwParam = me->fVertical ? LAYOUT_VERT : LAYOUT_HORZ;
         return TRUE;
      }
   }

   return FALSE;
}


/////////////////////////////////////////////////////////////////
// PropContainer concrete inplementation
void PropContainer_Ctor(PropContainer *me, AEEVTBL(IPropContainer) *pvt, 
                        IShell *piShell, IModule *piModule, PFNHANDLER pfnDefHandler)
{
   ContainerBase_Ctor(&me->base, (AEEVTBL(IContainer) *)pvt, piShell, piModule, 
                      pfnDefHandler ? pfnDefHandler : (PFNHANDLER)PropContainer_DefHandleEvent,
                      (PFNMKNODE)WidgetNode_ForProp, PropContainer_doLayout);

   pvt->QueryInterface = PropContainer_QueryInterface;
   pvt->GetProp    = PropContainer_GetProp;
   pvt->SetProp    = PropContainer_SetProp;
   pvt->SetPadding = PropContainer_SetPadding;
   pvt->GetPadding = PropContainer_GetPadding;
   pvt->Invalidate = PropContainer_Invalidate;

   CBASE(me)->vtWidget.GetPreferredExtent = PropContainer_GetPreferredExtent;

   me->fVertical = 1;
}


int PropContainer_New(IPropContainer **ppo, IShell *piShell, IModule *piModule)
{
   typedef struct PropContainerImpl {
      PropContainer base;
      AEEVTBL(IPropContainer) vt;
   } PropContainerImpl;

   PropContainerImpl *me = MALLOCREC(PropContainerImpl);

   *ppo = (IPropContainer *)me;

   if (!me)
      return ENOMEMORY;

   PropContainer_Ctor(&me->base, &me->vt, piShell, piModule, 0);
   return SUCCESS;
}

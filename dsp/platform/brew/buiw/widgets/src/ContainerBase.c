/*
  ========================================================================

  FILE:  ContainerBase.c
  
  SERVICES:  

  GENERAL DESCRIPTION: Container base implementation

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#include "ContainerBase.h"
#include "AEEWProperties.h"
#include "ModelBase.h"
#include "wutil.h"

#define FARF_INVAL   0
#define FARF_DRAW    0
#define FARF_FOCUS   0
#define FARF_INVAL   0
#define FARF_TEST    0

#ifndef FARF
#define FARF(x, p)  if (0 != FARF_##x) DBGPRINTF p 
#endif



#define BASE_FROM_CONTAINER ContainerBase *me = (ContainerBase *)po
#define BASE_FROM_WIDGET    ContainerBase *me = (ContainerBase *)po->pMe


static int ContainerBase_LocateRaisedPosition(IContainer *po, WidgetNode *pNode, WidgetPos *pwpRel)
{
   IWidget *      piwRef = 0;
   IContainer *   picParent = 0;
   AEERect        rcChild;
   AEERect        rcContainer;

   int nErr = pNode->pixyRaise ? SUCCESS : EFAILED;

   ZEROAT(&rcChild);
   ZEROAT(&rcContainer);

   // Get the absolute position of the child we are raising
   if (!nErr) {
      nErr = ContainerBase_Locate(po, pNode->piWidget, NULL, &rcChild);
   }

   if (!nErr) {
      nErr = IXYCONTAINER_QueryInterface(pNode->pixyRaise, AEEIID_WIDGET, (void**)&piwRef);
   }

   // Get the absolute position of the raise container
   if (!nErr) {
      IWIDGET_GetParent(piwRef, &picParent);
      if (picParent) {
         nErr = ICONTAINER_Locate(picParent, piwRef, NULL, &rcContainer);
      } else {
         // if no parent, assume that we are a root container
         rcContainer.x = 0;
         rcContainer.y = 0;
      }
   }

   // Calculate the relative pos of the child with respect to the raise container
   if (!nErr) {
      pwpRel->bVisible = TRUE;
      pwpRel->x = rcChild.x - rcContainer.x;
      pwpRel->y = rcChild.y - rcContainer.y;
   }

   RELEASEIF(piwRef);
   RELEASEIF(picParent); 

   return nErr;
}


void ContainerBase_DoLayout(ContainerBase *me, WidgetNode *pn, AEERect *prc)
{
   if (me->fEnableLayout && me->pfnLayout) {

      // call the installed layout function
      me->pfnLayout(me, pn, prc);

      // if any of our children are raised, adjust their position
      // in the raise container, and invalidate them to the raise target
      {
         WidgetNode *node;
   
         // enumerate all child nodes
         WNODE_ENUM_FWD(node, &me->head) {
            // only deal with raised children
            if (node->pixyRaise) {
               // the above layout function may have changed the extent of the raised widget
               // so we need to make sure the raise target updates its internal node->rc
               IXYCONTAINER_Invalidate(node->pixyRaise, node->piWidget, NULL, ICIF_EXTENT);
            }
         }
      }

   }
}

static __inline int ContainerBase_mkWidgetNode(ContainerBase *me, const void *pDesc, IWidget *piw, WidgetNode **ppo)
{
   if (!me->pfnMkNode)
      return EBADSTATE;
   return me->pfnMkNode(pDesc, piw, ppo);
}



void ContainerBase_InvalidateMe(ContainerBase *me, uint32 dwflags)
{
   if (me->piParent) {
      AEERect rc;
      SETAEERECT(&rc, 0, 0, me->extent.width, me->extent.height);
      ICONTAINER_Invalidate(me->piParent, &me->widget, &rc, dwflags);
   }   
}


/////////////////////////////////////////////////////////////////
// IQueryInterface

uint32 ContainerBase_AddRef(IContainer *po)
{
   BASE_FROM_CONTAINER;

   return ++me->nRefs;
}

void ContainerBase_Dtor(ContainerBase *me)
{
   // Free all nodes
   WidgetNode *node = &me->head;
   while (node->pNext != node) {
      if (node->piWidget) {
         IWIDGET_SetParent(node->piWidget, NULL);
      }
      WidgetNode_Delete(node->pNext);
   }

   RELEASEIF(me->piModel);
   RELEASEIF(me->piModule);
   RELEASEIF(me->piViewModel);
   Border_Dtor(&me->border);
}

uint32 ContainerBase_Release(IContainer *po)
{
   BASE_FROM_CONTAINER;

   if (me->nRefs == 1) {
      // Call free hook for handler cxt
      HANDLERDESC_Free(&me->hd);
      ContainerBase_Dtor(me);
      FREE(me);
      return 0;
   }

   return --me->nRefs;
}

int ContainerBase_QueryInterface(IContainer *po, AEECLSID id, void **ppo)
{
   BASE_FROM_CONTAINER;

   if ((id == AEECLSID_QUERYINTERFACE) 
       || (id == AEEIID_CONTAINER)) {
      *ppo = po;
      ICONTAINER_AddRef(po);
      return SUCCESS;
   } else if ((id == AEEIID_WIDGET)
       || (id == AEEIID_HANDLER)) {
      *ppo = &me->widget;
      ICONTAINER_AddRef(po);
      return SUCCESS;
   } else {
      *ppo = 0;
      return ECLASSNOTSUPPORT;
   }
}

/////////////////////////////////////////////////////////////////
// IWidget

static __inline boolean WidgetNode_CanTakeFocus(WidgetNode *pn) {
   boolean bFocus = FALSE;
   if (pn->fVisible) 
      IWIDGET_CanTakeFocus(pn->piWidget, &bFocus);
   return bFocus;
}

// look for a visible, focusable node starting from 'pn', in
// either the forward or reverse direction
static WidgetNode *FindFocusNode(WidgetNode *pnHead, WidgetNode *pn, boolean bReverse)
{
   for (; pn && pn != pnHead; pn = (bReverse ? pn->pPrev : pn->pNext)) {
      if (WidgetNode_CanTakeFocus(pn)) {
         return pn;
      }
   }
   return 0;
}

// gets an IWidget* from an IContainer*
// object stays at same reference count
static __inline IWidget *IContainer_GetIWidget(IContainer *pic) {
   IWidget *piwc = 0;
   ICONTAINER_QueryInterface(pic, AEEIID_WIDGET, (void**)&piwc);
   ICONTAINER_Release(pic);
   return piwc;
}

boolean IWidget_FindInContainmentTree(IWidget *piw, IWidget *piwQueryContainer)
{
   boolean bFound = 0;
   ADDREFIF(piw);

   while (piw && !bFound) {
      IContainer *pic = 0;
      IWIDGET_GetParent(piw, &pic);
      RELEASEIF(piw);      // release the old piw
      if (!pic) {
         break;
      }
      piw = IContainer_GetIWidget(pic);
      if (piw == piwQueryContainer) {
         bFound = 1;
      }
   }

   RELEASEIF(piw);
   return bFound;
}

/* 

ContainerBase_MoveFocus

Return Value:

   Value of piwFocus       Returns
   -----------------       -------
   
   WIDGET_FOCUS_NONE       TRUE always
   
   WIDGET_FOCUS_FIRST
   WIDGET_FOCUS_LAST
   WIDGET_FOCUS_NEXT
   WIDGET_FOCUS_PREV       TRUE if container has a widget that accepts focus.
   
   <explicit widget>       TRUE if requested widget is in container and
                           accepts focus.

*/
boolean ContainerBase_MoveFocus(ContainerBase *me, uint16 flags, IWidget *piwFocus)
{
   // save old focus
   WidgetNode *pnOldFocus = me->pFocus;
   WidgetNode *pnFocus = 0;
   boolean bResult = 0;

   // if we don't have a current focus, translate next/prev to first/last
   if (!me->pFocus) {
      if (piwFocus == (IWidget*)WIDGET_FOCUS_NEXT) {
         piwFocus = (IWidget*)WIDGET_FOCUS_FIRST;
      } else if (piwFocus == (IWidget*)WIDGET_FOCUS_PREV) {
         piwFocus = (IWidget*)WIDGET_FOCUS_LAST;
      }
   }

   // clear existing focus
   if (piwFocus == (IWidget*) WIDGET_FOCUS_NONE) {
      me->pFocus = 0;
      bResult = TRUE;
      goto clear;

   } else if (piwFocus == (IWidget*) WIDGET_FOCUS_FIRST) {
      pnFocus = FindFocusNode(&me->head, WNODE_ZBOTTOM(&me->head), 0);

   } else if (piwFocus == (IWidget*) WIDGET_FOCUS_LAST) {
      pnFocus = FindFocusNode(&me->head, WNODE_ZTOP(&me->head), 1);

   } else if (piwFocus == (IWidget*) WIDGET_FOCUS_NEXT) {
      pnFocus = FindFocusNode(&me->head, me->pFocus->pNext, 0);

   } else if (piwFocus == (IWidget*) WIDGET_FOCUS_PREV) {
      pnFocus = FindFocusNode(&me->head, me->pFocus->pPrev, 1);
   
   } else {

      WidgetNode *pn = 0;

      WNODE_ENUM_FWD(pn, &me->head) {
         // see if widget is one of our direct children
         if (pn->piWidget == piwFocus) {
            if (WidgetNode_CanTakeFocus(pn)) {
               pnFocus = pn;
            }
            break;
         }
         // now see if piwFocus is contained by child
         if (IWidget_FindInContainmentTree(piwFocus, pn->piWidget)) {
            // if so, it's a container so call MoveFocus recursively
            if (IWIDGET_HandleEvent(pn->piWidget, EVT_WDG_MOVEFOCUS, (uint16)(flags | 1), (uint32)piwFocus)
                 && WidgetNode_CanTakeFocus(pn)) {
               pnFocus = pn;
            }
            break;
         }
      }
   }

   bResult = (pnFocus != 0);

   if (pnFocus) {
      me->pFocus = pnFocus;
   }

   // tell widget it has focus
   if (me->pFocus && !(flags & 1)) {
      IWIDGET_SetFocus(me->pFocus->piWidget, 1);
   }

clear:
   // Clear existing focus only if the new focus is different from the old
   if (pnOldFocus && pnOldFocus != me->pFocus) {
      IWIDGET_SetFocus(pnOldFocus->piWidget, 0);
   }

   // return TRUE if focus was set
   return bResult;
}


static __inline boolean ContainerBase_CanTakeFocus(ContainerBase *me)
{
   IWidget *piwParent = 0;
   boolean bVisible = 0;

   // some containers don't have parents like root container, so this is conditional
   if (SUCCESS == IWIDGET_GetParentWidget(&me->widget, &piwParent) &&
       SUCCESS == IWIDGET_GetChildVisibility(piwParent, &me->widget, &bVisible) && 
       !bVisible) {

      RELEASEIF(piwParent);
      return FALSE;
   }

   RELEASEIF(piwParent);

   // it must be visible or didn't have a parent e.g. Root Container.
   return (boolean)(NULL != me->pFocus || 
                    NULL != FindFocusNode(&me->head, WNODE_ZTOP(&me->head), 1));
}


int ContainerBase_GetViewModel(ContainerBase *me, IModel **ppiModel) 
{
   int nErr = 0;

   // if we've been requested to supply a view model,
   // and we don't have one, create it
   if (!me->piViewModel) {
      nErr = ModelBase_New(&me->piViewModel, me->piModule);
   }

   *ppiModel = me->piViewModel;
   
   // AddRef the returned view model
   if (me->piViewModel) {
      IMODEL_AddRef(me->piViewModel);
   }

   return nErr;
}


boolean ContainerBase_SetProperty(ContainerBase* me, uint16 wParam, uint32 dwParam)
{
   switch (wParam) {

   case PROP_RAISE: {
      WidgetNode *   node;
      RaiseDesc *    pDesc = (RaiseDesc*)dwParam;
      IXYContainer * picTemp = 0;

      // find the node for this widget
      if (SUCCESS != WidgetNode_FindWidget(&CBASE(me)->head, pDesc->piw, &node)) {
         return FALSE;     // bail, widget is not one of ours
      }

      // make sure we are actually passed an IXYContainer
      if (pDesc->pixyTarget &&
          SUCCESS != ICONTAINER_QueryInterface((IContainer*)pDesc->pixyTarget, AEEIID_XYCONTAINER, (void**)&picTemp)) {
         return FALSE;
      }

      RELEASEIF(picTemp);

      // Remove the widget from its previous raise container
      if (node->pixyRaise) {
         IXYCONTAINER_Remove(node->pixyRaise, pDesc->piw);
         // Reset the parent
         IWIDGET_SetParent(pDesc->piw, (IContainer*)me);
         node->pixyRaise = NULL;
      } 

      // We have to invalidate our own area while the child widget is unraised. Here is a good spot
      ICONTAINER_Invalidate((IContainer*)me,  node->piWidget, NULL, 0);

      // If someone is trying to raise the child to its original parent (me)
      // then we don't need to do anything
      if (pDesc->pixyTarget && pDesc->pixyTarget != (IXYContainer*)me) {
         WidgetPos wpRel;
         node->pixyRaise = pDesc->pixyTarget;

         // add the widget to the raise container
         ContainerBase_LocateRaisedPosition((IContainer*)me, node, &wpRel); 
         IXYCONTAINER_Insert(node->pixyRaise, node->piWidget, WIDGET_ZTOPMOST, &wpRel);

         // restore the parent 
         IWIDGET_SetParent(pDesc->piw, (IContainer*)me);
      }
      return TRUE;
   }
   case PROP_VISIBLE: {
      WidgetNode *node = NULL;

      // if the node is currently hidden
      if (SUCCESS == WidgetNode_FindWidget(&CBASE(me)->head, ((WidgetVis*)dwParam)->piw, &node)) {
         node->fVisible = ((WidgetVis*)dwParam)->bVisible;
         ICONTAINER_Invalidate((IContainer*)me, node->piWidget, NULL, 0);
      }
      return (node != NULL);
   }
   case PROP_FLAGS:
      me->fNoFilterKeys = 0 != (dwParam & CWF_NOFILTERKEYS);
      me->fBroadcast    = 0 != (dwParam & CWF_BROADCAST);
      me->fNoRouteFocus = 0 != (dwParam & CWF_NOROUTEFOCUS);
      return TRUE;
   }

   // Not handled
   return FALSE;
}


boolean ContainerBase_GetProperty(ContainerBase* me, uint16 wParam, uint32 dwParam)
{
   switch (wParam){

      case PROP_RAISE: {
         WidgetNode* node;
         RaiseDesc* pDesc = (RaiseDesc*)dwParam;
   
         // Find the node for this widget.
         if (SUCCESS == WidgetNode_FindWidget(&CBASE(me)->head, pDesc->piw, &node)) {
            pDesc->pixyTarget = node->pixyRaise;
            return TRUE;
         }
         // This widget is not one of our children.
         pDesc->pixyTarget = NULL;
         return FALSE;
      }
      case PROP_VISIBLE:{
         WidgetNode *node;
   
         if (SUCCESS == WidgetNode_FindWidget(&CBASE(me)->head, ((WidgetVis*)dwParam)->piw, &node)) {
            ((WidgetVis*)dwParam)->bVisible = node->fVisible;
            return TRUE;
         }
         else
            return FALSE;
      }
      case PROP_VIEWMODEL:
         ContainerBase_GetViewModel(me, (IModel**)dwParam);
         return TRUE;
   
      case PROP_FLAGS: {
         uint32 dwFlags = 0;
         if (me->fBroadcast) {
            dwFlags |= CWF_BROADCAST;
         }
         if (me->fNoFilterKeys) {
            dwFlags |= CWF_NOFILTERKEYS;
         }
         if (me->fNoRouteFocus) { 
            dwFlags |= CWF_NOROUTEFOCUS;
         }
         *((uint32 *)dwParam) = dwFlags;
         return TRUE;  
      }
   }

   // Not handled
   return FALSE;
}


/* ContainerBase_TraversePreorder
|| 
|| Perform a preorder traversal of container tree
|| 
|| Given the tree:
||            a
||        b       c
||      d   e   f   g
|| 
|| Where 'a' is the top-level container widget which receives EVT_WDG_TRAVERSE,
|| the nodes will be visited in the following order:  a, b, d, e, c, f, g
|| 
*/
static void ContainerBase_Traverse(ContainerBase *me, TraverseDesc *ptd)
{
   // the 'reserved' member is used as an abort flag
   // initialize to zero at entry to level
   ptd->wReserved = 0;

   // visit my node first
   if (ptd->pfnVisit(ptd->pvVisit, &me->widget)) {
      ptd->wReserved = 1;    // tell upper levels we've been aborted
      return;                // buh-bye!
   }
   
   // decrement current level first -- 
   // when called initially with zero, this will cause a wraparound to -1 (0xFFFF), 
   // which will effectively traverse all (conceivable) levels deep
   ptd->wDepth--;

   // descend into children if any levels left
   if (ptd->wDepth > 0) {

      WidgetNode *pn = 0;
      WNODE_ENUM_FWD(pn, &me->head) {
         if (!IWIDGET_HandleEvent(pn->piWidget, EVT_WDG_TRAVERSE, 0, (uint32)ptd)) {
            // if not handled, the child is not a container so
            // we just call the visit function on each child
            if (ptd->pfnVisit(ptd->pvVisit, pn->piWidget)) {
               ptd->wReserved = 1;
            }
         }
         if (ptd->wReserved) {
            break;   // lower level aborted, end enumeration
         }

      }
   }

   // now ascending
   ptd->wDepth++;
}


boolean ContainerBase_DefHandleEvent(IWidget *po, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   BASE_FROM_WIDGET;
   boolean bHandled;

   switch (evt) {

   case EVT_WDG_TRAVERSE:
      ContainerBase_Traverse(me, (TraverseDesc*)dwParam);
      return TRUE;

   case EVT_WDG_CANTAKEFOCUS:
      *((boolean *)dwParam) = ContainerBase_CanTakeFocus(me);
      return TRUE;
   
   case EVT_WDG_HASFOCUS:
      *((boolean*)dwParam) = (boolean)(me->pFocus != NULL);
      return TRUE;
   
   case EVT_WDG_GETFOCUS:
      *((IWidget **)dwParam) = me->pFocus ? me->pFocus->piWidget : NULL;
      ADDREFIF(*(IWidget **)dwParam);
      return TRUE;

   case EVT_WDG_SETPROPERTY:
      if (ContainerBase_SetProperty(me, wParam, dwParam))
         return TRUE;
      break;

   case EVT_WDG_GETPROPERTY:
      if (ContainerBase_GetProperty(me, wParam, dwParam))
         return TRUE;
      break;
   
   default:
      // Not handled yet
      break;
   }
  
   bHandled = Border_HandleEvent(&me->border, evt, wParam, dwParam);

   if (evt == EVT_WDG_SETLAYOUT) {
      if (wParam == 0 || wParam == 1)  {
         me->fEnableLayout = wParam;
      } else if (wParam == 2) {
         ContainerBase_LayoutInvalidate(me);
      }
      return TRUE;
   }

   if (!me->pFocus && evt == EVT_WDG_SETFOCUS && wParam != 0) {
      ContainerBase_MoveFocus(me, 0, (IWidget*)WIDGET_FOCUS_FIRST);
   }

   if (evt == EVT_WDG_MOVEFOCUS) {
      return ContainerBase_MoveFocus(me, wParam, (IWidget*)dwParam);
   }

   // If not a focus change pass to focused widget (if we have one)
   if (!bHandled) {

      // do special filtering of key events in case the focus changed
      // unless the 'compatibility' flag is on
      if (!me->fNoFilterKeys) {
         if (evt == EVT_KEY_PRESS) {
            me->pFocusPress = me->pFocus;    // save focus node

         } else if ((evt == EVT_KEY || evt == EVT_KEY_RELEASE) 
                        && (me->pFocusPress != me->pFocus)) {

            return FALSE;  // must be a focus change in mid-key, don't propagate
         }
      }

      if (me->fBroadcast) {
         // broadcast to all children
         WidgetNode *pn = 0;
         WNODE_ENUM_FWD(pn, &me->head) {
            bHandled = IWIDGET_HandleEvent(pn->piWidget, evt, wParam, dwParam);
            if (bHandled) break;
         }

      } else if (me->pFocus && !me->fNoRouteFocus) {
         // otherwise route to focus child
         bHandled = IWIDGET_HandleEvent(me->pFocus->piWidget, evt, wParam, dwParam);
      }
   }

   return bHandled;
}

void ContainerBase_GetPreferredExtent(IWidget *po, WExtent *pwe)
{
   SETWEXTENT(pwe, 0, 0);
}

void ContainerBase_GetExtent(IWidget *po, WExtent *pwe)
{
   BASE_FROM_WIDGET;

   *pwe = me->extent;
}

void ContainerBase_SetExtent(IWidget *po, WExtent *pwe)
{
   BASE_FROM_WIDGET;
   WExtent eOld = me->extent;

   me->extent = *pwe;

   if (me->piParent) {
      AEERect rc;
      rc.x = 0;
      rc.y = 0;
      rc.dx = MAX(pwe->width, eOld.width);
      rc.dy = MAX(pwe->height, eOld.height);
      ICONTAINER_Invalidate(me->piParent, po, &rc, ICIF_EXTENT);
   }

   Border_CalcClientRect(&me->border);
   ContainerBase_LayoutInvalidate(me);

   if (me->piViewModel) {
      ModelEvent ev;
      ev.evCode   = EVT_MDL_SETEXTENT;
      ev.dwParam  = (uint32)&me->extent;
      IMODEL_Notify(me->piViewModel, &ev);
   }
}


void ContainerBase_GetParent(IWidget *po, IContainer **ppo)
{
   BASE_FROM_WIDGET;

   *ppo = me->piParent;
   ADDREFIF(me->piParent);
}

void ContainerBase_SetParent(IWidget *po, IContainer *piContainer)
{
   BASE_FROM_WIDGET;

   // Parent is not addref'ed to avoid circular references
   me->piParent = piContainer;
}

void ContainerBase_Draw(IWidget *po, ICanvas *piCanvas, int x, int y)
{
   BASE_FROM_WIDGET;
   WidgetNode *node;
   AEERect rcDisp, rcClient;
   boolean bObscured = 0;

   Border_Draw(&me->border, piCanvas, x, y);
   Border_AdjustDisplayClipRect(&me->border, piCanvas, x, y);

   // Get clip from display
   // rcDisp is in SCREEN coordinates
   ICANVAS_GetClipRect(piCanvas, &rcDisp);

   // optimization #1
   // after Border_AdjustDisplayClipRect, rectangle may be empty
   if (ISRECTEMPTY(&rcDisp)) {
      return;
   }

   // convert rcDisp to CLIENT coordinates
   rcClient = rcDisp;
   rcClient.x -= x + me->border.rcClient.x;
   rcClient.y -= y + me->border.rcClient.y;

   // optimization #2
   // walk nodes from top to bottom and mark ones that need to be drawn
   WNODE_ENUM_REV(node, &me->head) {
      AEERect rcOut;

      node->fDraw = 0;

      // don't draw obscured, non-visible or raised widgets
      if (bObscured || !node->fVisible || node->pixyRaise) {
         continue;
      }

      // check if child intersects the clip rect (rcClient)
      node->fDraw = IntersectRect(&rcOut, &node->rc, &rcClient);
#if defined(OPTIMIZ3)
      node->rcDraw = rcOut;
      // convert to screen coordinates
      node->rcDraw.x += x + me->border.rcClient.x;
      node->rcDraw.y += y + me->border.rcClient.y;
#endif

      if (node->fDraw && WidgetNode_SubtractOpaque(node, &rcClient)) {
         bObscured = 1;
      }
   }

   // now draw nodes from bottom up
   WNODE_ENUM_FWD(node, &me->head) {
#if !defined(OPTIMIZ3)
      AEERect rcDraw, rcClip;
#endif
      if (!node->fDraw) {
         continue;
      }

#if !defined(OPTIMIZ3)
      // Get the node rectangle
      rcDraw = node->rc;
      // convert to screen coords
      rcDraw.x += x + me->border.rcClient.x;
      rcDraw.y += y + me->border.rcClient.y;

      // intersect the node rect w/ display clip rect
      if (IntersectRect(&rcClip, &rcDraw, &rcDisp)) {
         // Set the clip to intersection of the node rect and the display clip
         ICANVAS_SetClipRect(piCanvas, &rcClip);
         // And draw it
         IWIDGET_Draw(node->piWidget, piCanvas, rcDraw.x, rcDraw.y);
         FARF(DRAW, ("########## IWIDGET_Draw(%lx) %d,%d", node->piWidget, rcDraw.x, rcDraw.y));
      }
#else
      ICANVAS_SetClipRect(piCanvas, &node->rcDraw);
      // And draw it
      IWIDGET_Draw(node->piWidget, piCanvas, 
                   node->rc.x + x + me->border.rcClient.x, 
                   node->rc.y + y + me->border.rcClient.y);

      FARF(DRAW, ("########## IWIDGET_Draw(%lx) %d,%d", 
                  node->piWidget, 
                  node->rc.x + x + me->border.rcClient.x, 
                  node->rc.y + y + me->border.rcClient.y));
#endif
   }
}


boolean ContainerBase_IntersectOpaque(IWidget *po, AEERect *prcResult, const AEERect *prcIn)
{
   BASE_FROM_WIDGET;
   AEERect rcCompare, rc;
   WidgetNode *node;

   if (Border_IntersectOpaque(&me->border, prcResult, prcIn)) {
      return TRUE;
   }

   SETAEERECT(&rc, 0, 0, me->extent.width, me->extent.height);
   IntersectRect(&rcCompare, prcIn, &rc);
   
   WNODE_ENUM_FWD(node, &me->head) {
      if (node->fVisible
          && WidgetNode_IntersectOpaque(node, &rc, prcIn)
          && rc.dx == rcCompare.dx 
          && rc.dy == rcCompare.dy) {
         // Intersect completed and widget covers entire area
         // container is opaque
         *prcResult = rc;
         return TRUE;
      }
   }

   // Otherwise we're transparent
   prcResult->dx = prcResult->dy = 0;
   return FALSE;
}

int ContainerBase_GetModel(IWidget *po, AEECLSID iid, IModel **ppwm)
{
   BASE_FROM_WIDGET;
   
   if (me->piModel) {
      return IQI_QueryInterface((IQueryInterface *)me->piModel, iid, (void**)ppwm);
   } else {
      *ppwm = 0;
      return EFAILED;
   }
}

int ContainerBase_SetModel(IWidget *po, IModel *pwm)
{
   BASE_FROM_WIDGET;
   
   RELEASEIF(me->piModel);
   me->piModel = pwm;
   ADDREF_IF(IMODEL, pwm);

   if (me->piViewModel) {
      ModelEvent ev;
      ev.evCode   = EVT_MDL_SETMODEL;
      ev.dwParam  = (uint32)me->piModel;
      IMODEL_Notify(me->piViewModel, &ev);
   }

   return SUCCESS;
}


static boolean ContainerBase_HandleEvent(IWidget *po, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   BASE_FROM_WIDGET;
   return HANDLERDESC_Call(&me->hd, evt, wParam, dwParam);
}


void ContainerBase_SetHandler(IWidget *po, HandlerDesc *phd)
{
   BASE_FROM_WIDGET;

   // Reset on null descriptor
   if (!phd) {
      HANDLERDESC_Init(&me->hd, me->pfnDefHandler, &me->widget, 0);
   } else {
      // swap contents of *phd and me->hd
      HandlerDesc hdTemp = *phd;
      *phd = me->hd;
      me->hd = hdTemp;
   }
}


boolean ContainerBase_CalcInvalidRectForNode(ContainerBase *me, WidgetNode *node, const AEERect *prcInWidget, uint32 dwFlags, AEERect *prcInvalid)
{
   IWidget *   piw = node->piWidget;
   boolean     bReachedInv;
   AEERect     rcUnder;

   if (!prcInWidget) 
      *prcInvalid = node->rc;
   else{
      SETAEERECT(prcInvalid, prcInWidget->x + node->rc.x, prcInWidget->y + node->rc.y,
                 prcInWidget->dx, prcInWidget->dy);
   }

   // Adjust the invalid rectangle by our border
   prcInvalid->x += me->border.rcClient.x;
   prcInvalid->y += me->border.rcClient.y;

   // Extent change, update widget rect. and also the invalid rect.
   // Find the invalid area: the largest of:
   // a) passed in widget rect b) the old extent or c) the new extent.
   //
   if (dwFlags & ICIF_EXTENT) {
      WExtent extent;
      IWIDGET_GetExtent(piw, &extent);
      prcInvalid->dx = MAX(node->rc.dx, extent.width);
      prcInvalid->dy = MAX(node->rc.dy, extent.height);
      if (prcInWidget) {
         prcInvalid->dx = MAX(prcInvalid->dx, prcInWidget->dx);
         prcInvalid->dy = MAX(prcInvalid->dy, prcInWidget->dy);
      }
      node->rc.dx = extent.width;
      node->rc.dy = extent.height;
   }

   // If the widget isn't visible, then there's no need to continue figuring out the
   // invalidation region.  The reason this test is here instead of at the top of the
   // function is that we need the above code for dwFlags indicating an extent change
   // to update the WidgetNode rect (node->rc).
   if (!node->fVisible)
      return TRUE;

   {  // Constrain invalidation request to my size
      AEERect rcMe;
      SETAEERECT(&rcMe, 0, 0, me->extent.width, me->extent.height);
      IntersectRect(prcInvalid, prcInvalid, &rcMe);   
   }

   // Remember this to look for opaque area under invalid widget
   rcUnder = *prcInvalid;
   bReachedInv = FALSE;

   // Walk backwards looking for given widget, subtracting out
   // opaque areas from higher widgets. 
   // After widget is reached keep going looking for bottom widget
   // to draw since invalid widget may be transparent
   WNODE_ENUM_REV(node, &me->head) {
      if (!node->fVisible)
         continue;

      if (node->piWidget == piw) {
         bReachedInv = TRUE;
      }
      
      if (bReachedInv) {
         if (WidgetNode_SubtractOpaque(node, &rcUnder))
            // rcUnder is empty, meaning we've found the
            // lowest node to start drawing at
            break;
      } else {
         if (WidgetNode_SubtractOpaque(node, prcInvalid))
            // if prcInvalid is empty the invalid region is obscured
            // no need to draw a thing
            return FALSE;
      }
   }

   return TRUE;
}

boolean ContainerBase_CalcInvalidRect(ContainerBase *me, IWidget *piw, const AEERect *prcInWidget, uint32 dwFlags, AEERect *prcInvalid)
{
   WidgetNode *node;

   // Make sure we know about widget
   if (WidgetNode_FindWidget(&me->head, piw, &node) != 0)
      return FALSE;

   return ContainerBase_CalcInvalidRectForNode(me, node, prcInWidget, dwFlags, prcInvalid);
}


/* ContainerBase_FindWidget

   me          [in] ContainerBase
   piw         [in] widget to find
   ppo         [out] widget node
   ppiw        [out] widget found (useful if piw==WIDGET_ZTOPMOST)
   ppIndex     [out] index of found widget
*/
int ContainerBase_FindWidget(ContainerBase *me, IWidget *piw, WidgetNode **ppo, 
                             IWidget **ppiw, int *pnIndex)
{
   WidgetNode *pn = 0;
   int nIndex = 0;

   if (WNODE_EMPTY(&me->head)) {
      return EFAILED;
   }

   if (WIDGET_ZTOPMOST == piw || WIDGET_ZNORMAL == piw) {
      pn = WNODE_ZTOP(&me->head);
   
   } else if (WIDGET_ZBOTTOMMOST == piw) {
      pn = WNODE_ZBOTTOM(&me->head);
      if (pnIndex != NULL) {  // if index is requested
         piw = pn->piWidget;  // get the real widget pointer
         pn = 0;              // force enum through list to determine index
      }
   }

   if (!pn) {
      
      WNODE_ENUM_REV(pn, &me->head) {
         if (pn->piWidget == piw) {
            break;
         }
         nIndex++;
      }
      if (pn == &me->head) {
         pn = 0;  // failed
      }
   }

   if (pn) {

      if (ppo) {
         *ppo = pn;
      }

      if (ppiw) {
         *ppiw = pn->piWidget;
      }

      if (pnIndex) {
         *pnIndex = nIndex;
      }

      return SUCCESS;
   }

   return EFAILED;
}



/////////////////////////////////////////////////////////////////
// IContainer

void ContainerBase_DefDoLayout(ContainerBase *me, WidgetNode *pNode, AEERect *prcInvalid)
{
   // calculate invalid rect and update node on extent change
   if (pNode) {
      WExtent extent;
      IWIDGET_GetExtent(pNode->piWidget, &extent);

      pNode->rc.dx = extent.width;
      pNode->rc.dy = extent.height;

      prcInvalid->x  = pNode->rc.x;
      prcInvalid->y  = pNode->rc.y;
      prcInvalid->dx = MAX(pNode->rc.dx, extent.width);
      prcInvalid->dy = MAX(pNode->rc.dy, extent.height);

   } else {
      *prcInvalid = me->border.rcClient;
   }
}


void ContainerBase_LayoutInvalidate(ContainerBase *me)
{
   if (me->fEnableLayout && !me->fInLayout) {
      AEERect rc;
      ContainerBase_DoLayout(me, NULL, &rc);
      if (me->piParent) {
         ICONTAINER_Invalidate(me->piParent, &me->widget, &rc, 0);
      }
   }
}

void ContainerBase_Invalidate(IContainer *po, IWidget *piw, const AEERect *prcInWidget, uint32 dwFlags)
{
   BASE_FROM_CONTAINER;

   AEERect rcInvalid;
   WidgetNode *  pn;
   boolean bLayout = FALSE;        // did we perform a layout

   // DoLayout also checks the disabled flag for invalidation, but adding the check here avoids
   // bubbling invalidation up the tree
   if (!me->fEnableLayout || me->fInLayout || SUCCESS != WidgetNode_FindWidget(&me->head, piw, &pn)) {
      return;
   }

   if (!prcInWidget) {
      rcInvalid = pn->rc;
   } else {
      SETAEERECT(&rcInvalid, prcInWidget->x + pn->rc.x, prcInWidget->y + pn->rc.y,
                  prcInWidget->dx, prcInWidget->dy);
   }

   if (dwFlags & ICIF_EXTENT) {
      // Do layout before calculating the invalid rectangle
      ContainerBase_DoLayout(me, pn, &rcInvalid);
      bLayout = TRUE;
   }

   if (pn->pixyRaise) {
      // We don't need any fixup on rcInvalid because it's in the 
      // coordinate system of the child, not ourselves.
      // invalidate to the raise target
      IXYCONTAINER_Invalidate(pn->pixyRaise, pn->piWidget, prcInWidget, dwFlags);

   } else if (me->piParent) {
      // Invalidate parent. Note we don't pass on extent changes here since this
      // container didn't change extent. Don't need to do this if the widget is raised
      // since the raise frame will take care of invalidation.
      rcInvalid.x += me->border.rcClient.x;
      rcInvalid.y += me->border.rcClient.y;
      if (bLayout)
         dwFlags &= ~ICIF_EXTENT;   // we already did the layout.  Let parent know content changed.
      ICONTAINER_Invalidate(me->piParent, &me->widget, &rcInvalid, dwFlags);
   }
}


int ContainerBase_Locate(IContainer *po, IWidget *piw, IContainer **ppRoot, AEERect *prcRelative)
{
   BASE_FROM_CONTAINER;
   WidgetNode *node;

   if (SUCCESS != WidgetNode_FindWidget(&me->head, piw, &node))
      return EFAILED;

   prcRelative->x += me->border.rcClient.x + node->rc.x;
   prcRelative->y += me->border.rcClient.y + node->rc.y;

   if (me->piParent) {
      return ICONTAINER_Locate(me->piParent, &me->widget, ppRoot, prcRelative);

   } else if (ppRoot) {
      *ppRoot = po;
      ICONTAINER_AddRef(po);
   }

   return SUCCESS;
}

int ContainerBase_Insert(IContainer *po, IWidget *piw, IWidget *piwBefore, const void *pDesc)
{
   BASE_FROM_CONTAINER;
   int result;
   WidgetNode *node, *insertNode;

   // if inserting widget as topmost, insertNode is the head node
   if (piwBefore == WIDGET_ZNORMAL || piwBefore == WIDGET_ZTOPMOST) {
      insertNode = &CBASE(me)->head;
   
   // otherwise, for bottommost or a specified widget, find the widget's node
   } else if (SUCCESS != WidgetNode_FindWidget(&CBASE(me)->head, piwBefore, &insertNode)) {
      return EBADPARM;
   }

   result = ContainerBase_mkWidgetNode(me, pDesc, piw, &node);

   if (result == 0) {
      WNODE_INSERTBEFORE(insertNode, node);
      IWIDGET_SetParent(piw, po);
      
      ContainerBase_LayoutInvalidate(me);
   }
   return result;
}


// 'pn' MUST be a pointer to a valid WidgetNode that is in 
// the container, i.e., previously found by xxx_FindWidget()
void ContainerBase_WidgetNodeRemove(ContainerBase *me, WidgetNode *pn)
{
   // Tell widget and parent it's losing focus if it had it
   if (me->pFocus && me->pFocus->piWidget == pn->piWidget) {
      IWIDGET_HandleEvent(&me->widget, EVT_WDG_FOCUSCANCEL, 0, (uint32)pn->piWidget);
      IWIDGET_SetFocus(pn->piWidget, 0);
      me->pFocus = NULL;
      me->pFocusPress = NULL;
   }

   // de-parent widget and destroy node
   IWIDGET_SetParent(pn->piWidget, NULL);
   WidgetNode_Delete(pn);

   ContainerBase_LayoutInvalidate(me);
}


int ContainerBase_Remove(IContainer *po, IWidget *piw)
{
   BASE_FROM_CONTAINER;
   WidgetNode *pn;

   // find the widgetnode associated with piw
   if (SUCCESS != ContainerBase_FindWidget(me, piw, &pn, &piw, NULL)) {
      return EBADPARM;
   }

   ContainerBase_WidgetNodeRemove(me, pn);
   return SUCCESS;
}


IWidget *ContainerBase_GetWidget(IContainer *po, IWidget *piw, boolean bNext, boolean bWrap)
{
   BASE_FROM_CONTAINER;
   return WidgetNode_GetWidget(&me->head, piw, bNext, bWrap);
}

void ContainerBase_Ctor(ContainerBase *me, AEEVTBL(IContainer) *pvt, 
                        IModule *piModule, PFNHANDLER pfnDefHandler,
                        PFNMKNODE pfnMkNode, PFNLAYOUT pfnLayout)
{
   me->pvt = pvt;
   me->nRefs = 1;

   me->piModule = piModule;
   ADDREFIF(piModule);

#define VT(name) pvt->name = ContainerBase_##name
   VT(AddRef);
   VT(Release);
   VT(QueryInterface);
   VT(Invalidate);
   VT(Insert);
   VT(Remove);
   VT(GetWidget);
   VT(Locate);

   AEEBASE_INIT(me, widget, &me->vtWidget);

#define WVT(name) me->vtWidget.name = ContainerBase_##name
   me->vtWidget.AddRef         = AEEBASE_AddRef(IWidget);
   me->vtWidget.Release        = AEEBASE_Release(IWidget);
   me->vtWidget.QueryInterface = AEEBASE_QueryInterface(IWidget);
   WVT(HandleEvent);
   WVT(GetPreferredExtent);
   WVT(GetExtent);
   WVT(SetExtent);
   WVT(GetParent);
   WVT(SetParent);
   WVT(Draw);
   WVT(IntersectOpaque);
   WVT(GetModel);
   WVT(SetModel);
   WVT(SetHandler);
   
   Border_Ctor(&me->border, (PFNINVALIDATE)ContainerBase_InvalidateMe, me, &me->extent, TRUE, &CBASE(me)->piViewModel);
   me->border.nWidth[RGBINDEX_INACTIVE]   = 0;
   me->border.nWidth[RGBINDEX_ACTIVE]     = 0;
   
   WNODE_CTOR(&me->head);

   me->pfnDefHandler = pfnDefHandler ? pfnDefHandler 
                                     : (PFNHANDLER)ContainerBase_DefHandleEvent;

   me->fInLayout     = 0;
   me->fEnableLayout = 1;
   me->pfnMkNode     = pfnMkNode;
   me->pfnLayout     = pfnLayout ? pfnLayout : ContainerBase_DefDoLayout;
   
   HANDLERDESC_Init(&me->hd, me->pfnDefHandler, &me->widget, 0);
}

/////////////////////////////////////////////////////////////////
// WidgetNode functions

IWidget *WidgetNode_GetWidget(WidgetNode *head, IWidget *piw, boolean bNext, boolean bWrap)
{
   WidgetNode *node;

   if (WNODE_EMPTY(head))
      return NULL;

   if (!piw) 
      return (bNext ? WNODE_ZBOTTOM(head) : WNODE_ZTOP(head))->piWidget;

   if (WidgetNode_FindWidget(head, piw, &node) == 0) {
      node = bNext ? node->pNext : node->pPrev;
      if (node == head)
         node = !bWrap ? NULL : bNext ? node->pNext : node->pPrev;
      return node ? node->piWidget : NULL;
   }

   return NULL;
}


int WidgetNode_FindWidget(WidgetNode *head, IWidget *piWidget, WidgetNode **ppo)
{
   if (piWidget == WIDGET_ZTOPMOST || piWidget == WIDGET_ZNORMAL) {
      *ppo = WNODE_ZTOP(head);
      return 0;
   
   } else if (piWidget == WIDGET_ZBOTTOMMOST) {
      *ppo = WNODE_ZBOTTOM(head);
      return 0;
   
   } else {
      // Find a node given head and widget
      WNODE_ENUM_FWD(*ppo, head) {
         if ((*ppo)->piWidget == piWidget) {
            return 0;
         }
      }
   }

   *ppo = NULL;
   return EFAILED;
}


boolean WidgetNode_IntersectOpaque(WidgetNode *node, AEERect *rcOut, const AEERect *rcIn)
{
   AEERect rc;
   // Intersect rcIn with given widget's rectangle. Parameters are in container
   // co-ordinates
   rc = *rcIn;
   // Translate to widget coords
   rc.x -= node->rc.x;
   rc.y -= node->rc.y;

   if (IWIDGET_IntersectOpaque(node->piWidget, rcOut, &rc)) {
      // Convert rcOut back to container coords
      rcOut->x += node->rc.x;
      rcOut->y += node->rc.y;
      return TRUE;
   }
   return FALSE;
}


boolean WidgetNode_SubtractOpaque(WidgetNode *node, AEERect *rcInOut)
{
   AEERect rc;
   // Subtract a widget's opaque region from the given parameter
   // Return's TRUE only if result is an empty rect
   if (WidgetNode_IntersectOpaque(node, &rc, rcInOut)
       && SubtractRect(rcInOut, rcInOut, &rc)
       && ISRECTEMPTY(rcInOut))
      return TRUE;
   return FALSE;
}

void WidgetNode_Ctor(WidgetNode *me, IWidget *piw, AEERect *rc, boolean bVisible)
{
   WNODE_CTOR(me);

   RELEASEIF(me->piWidget);
   me->piWidget = piw;
   ADDREFIF(piw);

   me->rc = *rc;
   me->fVisible = bVisible;
   me->pixyRaise = NULL;
}

void WidgetNode_Dtor(WidgetNode *me)
{
   // If this widget is raised, unraise it by removing it from the raise container
   if (me->pixyRaise) {
      IXYCONTAINER_Remove(me->pixyRaise, me->piWidget);
   }
   WNODE_REMOVE(me);
   RELEASEIF(me->piWidget);
}

void WidgetNode_Delete(WidgetNode *me)
{
   WidgetNode_Dtor(me);
   FREE(me);
}



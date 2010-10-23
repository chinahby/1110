/*
  ========================================================================

  FILE:  XYContainer.c
  
  SERVICES:  

  GENERAL DESCRIPTION: Implementation of XYContainer

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#include "AEEStdLib.h"

#include "XYContainer.h"
#include "AEEWProperties.h"
#include "wutil.h"

#define BASE_FROM_CONTAINER ContainerBase *me = (ContainerBase *)po

static __inline int WidgetNode_NewForPos(const WidgetPos *pos, IWidget *piw, WidgetNode **ppo)
{
   AEERect rc;
   WExtent extent;
   boolean bVisible = TRUE;

   *ppo = MALLOCREC(WidgetNode);
   if (!*ppo)
      return ENOMEMORY;

   IWIDGET_GetExtent(piw, &extent);

   if (pos) {
      SETAEERECT(&rc, pos->x, pos->y, extent.width, extent.height);
      bVisible = pos->bVisible;
   }
   else
      SETAEERECT(&rc, 0, 0, extent.width, extent.height);

   WidgetNode_Ctor(*ppo, piw, &rc, bVisible);
   return SUCCESS;
}


uint32 XYContainer_Release(IXYContainer *po)
{
   BASE_FROM_CONTAINER;

   // when we are really going away...
   if (me->nRefs == 1) {
      WidgetNode *pNode;
      WidgetNode *pNext;

      // if any of our child widgets are raised, we need to unraise them
      for (pNode = me->head.pNext; pNode != &me->head; pNode = pNext) {
         IWidget *piwParent = 0;

         // get pNext now because unraise will unlink the node from our list
         pNext = pNode->pNext;

         if (SUCCESS == IWIDGET_GetParentWidget(pNode->piWidget, &piwParent) && 
             (IXYContainer*)me == IWIDGET_ContainerGetRaiseTo(piwParent, pNode->piWidget)) {

            // unraise the widget
            IWIDGET_ContainerRaiseWidget(piwParent, pNode->piWidget, NULL);
         }

         RELEASEIF(piwParent);
      }
   }

   return ContainerBase_Release((IContainer*)po);
}


static __inline int ICONTAINER_LocateRect(IContainer *me, IWidget *piw, AEERect *prc) {
   prc->x = prc->y = 0;
   return ICONTAINER_Locate(me, piw, NULL, prc);
}


// if any of our children are raised to us (not normal children)
// then we need to adjust their coordinates by the difference
// of the absolute positions of 1) ourselves and 2) the widget's parent
static void XYContainer_AdjustRaiseNode(XYContainer *me, WidgetNode *pn, AEERect *prcContainer)
{
   AEERect rc;    // rectangle of child
   IContainer *picParent = 0;
   IWIDGET_GetParent(pn->piWidget, &picParent);

   // if we're not the widgets parent, then widget must be raised
   // so we need to locate its position in the other container
   // and adjust its node rect
   if (picParent 
         && (picParent != (IContainer*)me) 
         && SUCCESS == ICONTAINER_LocateRect(picParent, pn->piWidget, &rc)) {

      // calculate the relative position of the child w/ respect
      // to us, the raise container
      pn->rc.x = rc.x - prcContainer->x;
      pn->rc.y = rc.y - prcContainer->y;
   }
   RELEASEIF(picParent);
}


void XYContainer_DoLayout(ContainerBase *me, WidgetNode *pNode, AEERect *prcInvalid)
{
   AEERect rcContainer;        // rect of ourselves (abs coords)
   SETAEERECT(&rcContainer, 0,0,0,0);
   
   // first locate 'me'
   // (if no parent, then we assume we're a root container and
   //  use the rcContainer.xy values of 0,0 initialized above)
   if (!me->piParent
         || (me->piParent
             && SUCCESS == ICONTAINER_LocateRect(me->piParent, &me->widget, &rcContainer))) {

      if (pNode) {
         // if we were passed a single node, then that's all we have to adjust
         XYContainer_AdjustRaiseNode((XYContainer*)me, pNode, &rcContainer);
      } else {
         // otherwise, enum all children and adjust if raised
         WidgetNode *pn = 0;
         WNODE_ENUM_FWD(pn, &me->head) {
            XYContainer_AdjustRaiseNode((XYContainer*)me, pn, &rcContainer);
         }
      }
   }

   // do default layout
   ContainerBase_DefDoLayout(me, pNode, prcInvalid);
}


int XYContainer_GetPos(IXYContainer *po, IWidget *piw, WidgetPos *pos)
{
   BASE_FROM_CONTAINER;
   WidgetNode *node;

   if (WidgetNode_FindWidget(&me->head, piw, &node) != 0)
      return EBADPARM;

   pos->x = node->rc.x;
   pos->y = node->rc.y;
   pos->bVisible = (boolean)node->fVisible;

   return SUCCESS;
}

int XYContainer_SetPos(IXYContainer *po, IWidget *piw, IWidget *piwBefore, const WidgetPos *pos)
{
   BASE_FROM_CONTAINER;
   WidgetNode *node, *insertNode;
   AEERect rcInvalid;
   
   if (SUCCESS != WidgetNode_FindWidget(&me->head, piw, &node)) {
      return EBADPARM;
   }

   if (piwBefore == WIDGET_ZTOPMOST) {
      insertNode = &me->head;
   
   } else if (SUCCESS != WidgetNode_FindWidget(&me->head, piwBefore, &insertNode)) {
      return EBADPARM;
   }

   SETAEERECT(&rcInvalid, 0, 0, node->rc.dx, node->rc.dy);

   // Invalidate old location
   if (node->fVisible)
      IXYCONTAINER_Invalidate(po, piw, &rcInvalid, 0);

   if (piwBefore != WIDGET_ZNORMAL && node != insertNode) {
      WNODE_REMOVE(node);
      WNODE_INSERTBEFORE(insertNode, node);
   }

   // Update fields
   node->rc.x = pos->x;
   node->rc.y = pos->y;
   node->fVisible = pos->bVisible;

   // Invalidate new location if visible
   if (pos->bVisible)
      IXYCONTAINER_Invalidate(po, piw, &rcInvalid, 0);

   return 0;
}

int XYContainer_QueryInterface(IXYContainer *po, AEECLSID id, void **ppo)
{
   if (id == AEEIID_XYCONTAINER) {
      *ppo = po;
      ICONTAINER_AddRef((IContainer*)po);
      return SUCCESS;
   } 

   return ContainerBase_QueryInterface((IContainer*)po, id, ppo);
}


void XYContainer_Ctor(XYContainer *me, AEEVTBL(IXYContainer) *pvt, 
                      IModule *piModule, PFNHANDLER pfnDefHandler, PFNLAYOUT pfnLayout)
{
   ContainerBase_Ctor(&me->base, (AEEVTBL(IContainer) *)pvt, piModule, pfnDefHandler,
                      (PFNMKNODE)WidgetNode_NewForPos, pfnLayout);

#define VT(name) pvt->name = XYContainer_##name
   VT(Release);
   VT(QueryInterface);
   VT(SetPos);
   VT(GetPos);
}

int XYContainer_New(IXYContainer **ppo, IModule *piModule)
{
   XYContainer *me = MALLOCREC_VTBL(XYContainer, IXYContainer);

   if (!me)
      return ENOMEMORY;

   *ppo = (IXYContainer *)me;

   XYContainer_Ctor(me, GETVTBL(me,IXYContainer), piModule, 0, 
                    XYContainer_DoLayout);

   return SUCCESS;
}


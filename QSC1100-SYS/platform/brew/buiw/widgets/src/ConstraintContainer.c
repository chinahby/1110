/*
  ========================================================================

  FILE:  ConstraintContainer.c
  
  SERVICES:  

  GENERAL DESCRIPTION: Implementation of a constraint container

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#include "ConstraintContainer.h"
#include "wutil.h"
#include "ContainerBase.h"

typedef struct ConstraintWidgetNode {
   WidgetNode base;
   
   Constraint constraint;
} ConstraintWidgetNode;

static __inline int ConstraintWidgetNode_FindWidget(WidgetNode *head, IWidget *piw, ConstraintWidgetNode **ppo) {
   return WidgetNode_FindWidget(head, piw, (WidgetNode **)ppo);
}

static __inline boolean Constraint_equals(const Constraint *c1, const Constraint *c2) {
#define ELEM_EQUALS(e1,e2) (e1.type == e2.type && e1.offset == e2.offset)

   return ELEM_EQUALS(c1->left, c2->left)
      && ELEM_EQUALS(c1->right, c2->right)
      && ELEM_EQUALS(c1->top, c2->top)
      && ELEM_EQUALS(c1->bottom, c2->bottom);
}
      
#define ME_FROM_CONTAINER   ConstraintContainer *me = (ConstraintContainer *)po
#define BASE_FROM_CONTAINER ContainerBase *me       = (ContainerBase *)po
#define ME_FROM_WIDGET      ConstraintContainer *me = (ConstraintContainer *)po->pMe


static int16 Constraint_valueFor(uint16 type, int16 offset, WidgetNode *wn, WExtent *e)
{
   int16 result;

   switch(type) {
   case CONSTRAINT_PARENT_LEFT:
      result = offset;
      break;
   case CONSTRAINT_PARENT_RIGHT:
      result = e->width + offset;
      break;
   case CONSTRAINT_PARENT_TOP:
      result = offset;
      break;
   case CONSTRAINT_PARENT_BOTTOM:
      result = e->height + offset;
      break;
   case CONSTRAINT_PARENT_WIDTH:
      result = (e->width * offset) / 100;
      break;
   case CONSTRAINT_PARENT_HEIGHT:
      result = (e->height * offset) / 100;
      break;
   case CONSTRAINT_PREV_LEFT:
      result = wn->pPrev->rc.x + offset;
      break;
   case CONSTRAINT_PREV_RIGHT:
      result = wn->pPrev->rc.x + wn->pPrev->rc.dx + offset;
      break;
   case CONSTRAINT_PREV_TOP:
      result = wn->pPrev->rc.y + offset;
      break;
   case CONSTRAINT_PREV_BOTTOM:
      result = wn->pPrev->rc.y + wn->pPrev->rc.dy + offset;
      break;            
   case CONSTRAINT_PARENT_HCENTER:
      result = (e->width)/2 + offset;
      break;
   case CONSTRAINT_PARENT_VCENTER:
      result = (e->height)/2 + offset;
      break;    
   default:
      result = 0;
   }

   return result;
}

static __inline void Constraint_layoutNode(Constraint *c, WidgetNode *wn, WExtent *e)
{
   int16 width, height;
   WExtent we;

   wn->rc.x = Constraint_valueFor(c->left.type, c->left.offset, wn, e);
   wn->rc.y = Constraint_valueFor(c->top.type, c->top.offset, wn, e);
   // check special case for centre 
   if (c->right.type == CONSTRAINT_PARENT_HCENTER && c->left.type == CONSTRAINT_PARENT_HCENTER)
      width = c->right.offset - c->left.offset;
   else
      width     = Constraint_valueFor(c->right.type, c->right.offset, wn, e) - wn->rc.x;
   wn->rc.dx = MAX(width, 0);
   if (c->bottom.type == CONSTRAINT_PARENT_VCENTER && c->top.type == CONSTRAINT_PARENT_VCENTER)
      height = c->bottom.offset - c->top.offset;
   else
      height    = Constraint_valueFor(c->bottom.type, c->bottom.offset, wn, e) - wn->rc.y;
   wn->rc.dy = MAX(height, 0);

   // Check for some constraints based on existing/preferred size
   IWIDGET_GetExtent(wn->piWidget, &we);
   if (c->left.type != WIDGET_KEEP_EXTENT && c->right.type  != WIDGET_KEEP_EXTENT)
      we.width = width;
   if (c->top.type  != WIDGET_KEEP_EXTENT && c->bottom.type != WIDGET_KEEP_EXTENT)
      we.height = height;
   // the child will get it's preferred extent in the size-to-fit and widget-centre
   // cases
   if (c->left.type == WIDGET_SIZE_TO_FIT || c->right.type  == WIDGET_SIZE_TO_FIT ||
       c->top.type  == WIDGET_SIZE_TO_FIT || c->bottom.type == WIDGET_SIZE_TO_FIT ||
       c->left.type == WIDGET_CENTER_SIZE_TO_FIT || c->right.type == WIDGET_CENTER_SIZE_TO_FIT ||
       c->top.type == WIDGET_CENTER_SIZE_TO_FIT  || c->bottom.type == WIDGET_CENTER_SIZE_TO_FIT)
   {
      if (c->left.type == WIDGET_SIZE_TO_FIT || c->right.type  == WIDGET_SIZE_TO_FIT ||
          c->left.type == WIDGET_CENTER_SIZE_TO_FIT || c->right.type == WIDGET_CENTER_SIZE_TO_FIT) 
         we.width = 0; // Start with 0 and work up 
      if (c->top.type  == WIDGET_SIZE_TO_FIT || c->bottom.type == WIDGET_SIZE_TO_FIT ||
          c->top.type == WIDGET_CENTER_SIZE_TO_FIT || c->bottom.type == WIDGET_CENTER_SIZE_TO_FIT)
         we.height = 0; // Start with 0 and work up 

      IWIDGET_SetExtent(wn->piWidget, &we);
      IWIDGET_GetPreferredExtent(wn->piWidget, &we);
      IWIDGET_SetExtent(wn->piWidget, &we);
   }

   // finish up in the x direction
   if (c->right.type == WIDGET_KEEP_EXTENT || c->right.type == WIDGET_SIZE_TO_FIT)
   {
      wn->rc.dx = we.width;
   }
   else if (c->left.type == WIDGET_KEEP_EXTENT || c->left.type == WIDGET_SIZE_TO_FIT)
   {
      wn->rc.x = wn->rc.dx - we.width;
      wn->rc.dx = we.width;
   }

   // we define WIDGET_CENTER_SIZE_TO_FIT to mean "centre using the preferred
   // extent of the widget"
   if (c->left.type == WIDGET_CENTER_SIZE_TO_FIT || c->right.type == WIDGET_CENTER_SIZE_TO_FIT)
   {
      wn->rc.dx = we.width;
      // parent has no width, so can't sensibly position child anyway
      if (e->width == 0)
         wn->rc.x = 0;
      else
         wn->rc.x = (e->width - we.width)/2;   
   }   

   // and in the y direction
   if (c->bottom.type == WIDGET_KEEP_EXTENT || c->bottom.type == WIDGET_SIZE_TO_FIT)
   {
      wn->rc.dy = we.height;
   }
   else if (c->top.type == WIDGET_KEEP_EXTENT || c->top.type == WIDGET_SIZE_TO_FIT)
   {
      wn->rc.y = wn->rc.dy - we.height;
      wn->rc.dy = we.height;
   }
   
   if (c->top.type == WIDGET_CENTER_SIZE_TO_FIT || c->bottom.type == WIDGET_CENTER_SIZE_TO_FIT)
   {
      wn->rc.dy = we.height;
      // parent has no height, so can't sensibly position child anyway   
      if (e->height == 0)
         wn->rc.y = 0;
      else
         wn->rc.y = (e->height - we.height)/2;
   }

}


static void ConstraintContainer_doLayoutWE(ConstraintContainer *me, WExtent *pwe, AEERect *prc)
{
   WidgetNode *wn;
   WExtent contExtent;
   AEERect rcPrev; 

#define CNODE(wn) ((ConstraintWidgetNode *)wn)

   if (prc) {
      *prc = me->base.border.rcClient;
   }

   // Block out invalidates from children
   CBASE(me)->fInLayout = 1;
   SETWEXTENT(&contExtent, CBASE(me)->border.rcClient.dx, CBASE(me)->border.rcClient.dy);

   // Begin with [0,0] extent and scale up as we find widgets farther out
   if (pwe)
      SETWEXTENT(pwe, 0, 0);

   // clear rectangle of last visible widget
   rcPrev.x = rcPrev.y = rcPrev.dx = rcPrev.dy = 0;

   WNODE_ENUM_FWD(wn, &CBASE(me)->head) {
      WExtent extent;
      if (wn->fVisible) {

         // If PreferredExtent is being calculated
         // save the widget's original extent.
         if (pwe) {
            IWIDGET_GetExtent(wn->piWidget, &extent);
         }
         Constraint_layoutNode(&CNODE(wn)->constraint, wn, &contExtent);
         // save rectangle of last visible widget
         rcPrev = wn->rc;

         // If PreferredExtent is being calculated
         // re-set the widget's original extent.
         // Otherwise use the extent returned by Constraint_layoutNode()
         if (!pwe) {
            SETWEXTENT(&extent, wn->rc.dx, wn->rc.dy);
         }
         IWIDGET_SetExtent(wn->piWidget, &extent);

         // Update preferred extent
         if (pwe) {
            // If this widget extends past our current width
            // Update the container's width
            if ((MAX(0, wn->rc.x)+wn->rc.dx) > pwe->width)
               pwe->width = (MAX(0, wn->rc.x)+wn->rc.dx);

            // If this widget extends past our current height
            // Update the container's height
            if ((MAX(0, wn->rc.y)+wn->rc.dy) > pwe->height)
               pwe->height = (MAX(0, wn->rc.y)+wn->rc.dy);
         }
      } else {
         // not visible, set widget rectangle equal to last visible
         wn->rc = rcPrev;
      }
   }

   // Allow children invalidates
   CBASE(me)->fInLayout = 0;

   if (pwe) {
      // just calculating extent
      Border_CalcPreferredExtent(&CBASE(me)->border, pwe, pwe);
   }
}

static void ConstraintContainer_doLayout(ConstraintContainer *me, WidgetNode *pn, AEERect *prc)
{
   ConstraintContainer_doLayoutWE(me, NULL, prc);
}


static int WidgetNode_ForConstraint(const WidgetConstraint *wconst, IWidget *piw, WidgetNode **ppo)
{
   WExtent extent;
   AEERect rc;
   boolean visible = TRUE;

   ConstraintWidgetNode *node = MALLOCREC(ConstraintWidgetNode);

   if (!node)
      return ENOMEMORY;

   *ppo = &node->base;

   IWIDGET_GetExtent(piw, &extent);
   SETAEERECT(&rc, 0, 0, extent.width, extent.height);

   if (wconst) {
      node->constraint = wconst->constraint;
      visible = wconst->bVisible;
   }
   else {
      ZEROAT(&node->constraint);
      node->constraint.bottom.type = WIDGET_KEEP_EXTENT;
      node->constraint.top.type = WIDGET_KEEP_EXTENT;
      node->constraint.left.type = WIDGET_KEEP_EXTENT;
      node->constraint.right.type = WIDGET_KEEP_EXTENT;
   }
   
   WidgetNode_Ctor(&node->base, piw, &rc, visible);
   return SUCCESS;
}


/////////////////////////////////////////////////////////////////
// IConstraintContainer
int ConstraintContainer_SetConstraint(IConstraintContainer *po, IWidget *piw, IWidget *piwBefore, const WidgetConstraint *pwc)
{
   ME_FROM_CONTAINER;
   ConstraintWidgetNode *node, *insertNode;

   if (ConstraintWidgetNode_FindWidget(&CBASE(me)->head, piw, &node) != 0)
      return EBADPARM;

   if (piwBefore == WIDGET_ZTOPMOST)
      insertNode = (ConstraintWidgetNode*)&CBASE(me)->head;
   else if (ConstraintWidgetNode_FindWidget(&CBASE(me)->head, piwBefore, &insertNode) != 0)
      return EBADPARM;

   // if it's the same constraint and there's no stack reordering, we're done.
   if (Constraint_equals(&node->constraint, &pwc->constraint)
       && node->base.fVisible == pwc->bVisible
       && (node == insertNode || piwBefore == WIDGET_ZNORMAL))
      return SUCCESS;

   node->constraint = pwc->constraint;
   node->base.fVisible = pwc->bVisible;

   if (piwBefore != WIDGET_ZNORMAL && node != insertNode) {
      WNODE_REMOVE((WidgetNode *)node);
      WNODE_INSERTBEFORE((WidgetNode *)insertNode, (WidgetNode *)node);
   }

   ContainerBase_LayoutInvalidate((ContainerBase*)me);
   return 0;
}

int ConstraintContainer_GetConstraint(IConstraintContainer *po, IWidget *piw, WidgetConstraint *pwc)
{
   BASE_FROM_CONTAINER;
   ConstraintWidgetNode *node;

   if (ConstraintWidgetNode_FindWidget(&me->head, piw, &node) != 0)
      return EBADPARM;

   pwc->bVisible = (boolean)node->base.fVisible;
   pwc->constraint = node->constraint;
   
   return SUCCESS;
}

/////////////////////////////////////////////////////////////////
// IContainer
int ConstraintContainer_QueryInterface(IConstraintContainer *po, AEECLSID id, void **ppo)
{
   if (id == AEEIID_CONSTRAINTCONTAINER) {
      *ppo = po;
      ICONTAINER_AddRef(ICONSTRAINTCONTAINER_TO_ICONTAINER(po));
      return SUCCESS;
   }
   return ContainerBase_QueryInterface(ICONSTRAINTCONTAINER_TO_ICONTAINER(po), id, ppo);
}

void ConstraintContainer_Invalidate(IConstraintContainer *po, IWidget *piw, const AEERect *prcInWidget, uint32 dwFlags)
{
   BASE_FROM_CONTAINER;
   ConstraintWidgetNode *node;

   if (me->fInLayout) {
      return;
   }

   if (0 == (dwFlags & (ICIF_EXTENT|ICIF_REDRAW)) 
       && SUCCESS == ConstraintWidgetNode_FindWidget(&CBASE(me)->head, piw, &node)
       && (node->constraint.left.type   == WIDGET_SIZE_TO_FIT ||
           node->constraint.left.type   == WIDGET_CENTER_SIZE_TO_FIT ||
           node->constraint.top.type    == WIDGET_SIZE_TO_FIT ||
           node->constraint.top.type    == WIDGET_CENTER_SIZE_TO_FIT ||
           node->constraint.right.type  == WIDGET_SIZE_TO_FIT ||
           node->constraint.right.type  == WIDGET_CENTER_SIZE_TO_FIT ||
           node->constraint.bottom.type == WIDGET_SIZE_TO_FIT ||
           node->constraint.bottom.type == WIDGET_CENTER_SIZE_TO_FIT)) {
       
      dwFlags |= ICIF_EXTENT;
   }

   ContainerBase_Invalidate(ICONSTRAINTCONTAINER_TO_ICONTAINER(po), piw, prcInWidget, dwFlags);
}


/////////////////////////////////////////////////////////////////
// IWidget

boolean ConstraintContainer_DefHandleEvent(IWidget *po, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   ME_FROM_WIDGET;

   AEERect *prcClient = &me->base.border.rcClient;
   AEERect rcSave = *prcClient;

   boolean bResult = ContainerBase_DefHandleEvent(po, evt, wParam, dwParam);

   // need to do a layout if the client rectangle changed
   if (rcSave.dx != prcClient->dx ||
       rcSave.dy != prcClient->dy) {
      ContainerBase_LayoutInvalidate((ContainerBase*)me);
   }

   return bResult;
}

void ConstraintContainer_GetPreferredExtent(IWidget *po, WExtent *pwe)
{ 
   ME_FROM_WIDGET;

   // Calculate preferred extent
   ConstraintContainer_doLayoutWE(me, pwe, NULL);
}

/////////////////////////////////////////////////////////////////
// ConstraintContainer concrete inplementation
void ConstraintContainer_Ctor(ConstraintContainer *me, AEEVTBL(IConstraintContainer) *pvt, 
                              IShell *piShell, IModule *piModule, PFNHANDLER pfnDefHandler)
{
   ContainerBase_Ctor(&me->base, (AEEVTBL(IContainer) *)pvt, piShell, piModule, 
                      pfnDefHandler ? pfnDefHandler : (PFNHANDLER)ConstraintContainer_DefHandleEvent,
                      (PFNMKNODE)WidgetNode_ForConstraint, (PFNLAYOUT)ConstraintContainer_doLayout);

   pvt->QueryInterface = ConstraintContainer_QueryInterface;
   pvt->GetConstraint = ConstraintContainer_GetConstraint;
   pvt->SetConstraint = ConstraintContainer_SetConstraint;
   pvt->Invalidate    = ConstraintContainer_Invalidate;

   CBASE(me)->vtWidget.GetPreferredExtent   = ConstraintContainer_GetPreferredExtent;
}


int ConstraintContainer_New(IConstraintContainer **ppo, IShell *piShell, IModule *piModule)
{
   ConstraintContainer *me = MALLOCREC_VTBL(ConstraintContainer, IConstraintContainer);

   *ppo = (IConstraintContainer *)me;

   if (!me)
      return ENOMEMORY;

   ConstraintContainer_Ctor(me, GETVTBL(me,IConstraintContainer), piShell, piModule, 0);
   return SUCCESS;
}



/*
  ========================================================================

  FILE:  CustomContainerBase.h
  
  SERVICES:  

  GENERAL DESCRIPTION: Base container definitions used by container
  implementations

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2005 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __CUSTOMCONTAINERBASE_H__
#define __CUSTOMCONTAINERBASE_H__

#include "AEEContainer.h"
#include "AEEXYContainer.h"
#include "AEEBase.h"
#include "Border.h"

typedef struct WidgetNode WidgetNode;

struct WidgetNode {
   WidgetNode *   pNext;
   WidgetNode *   pPrev;

   IWidget *      piWidget;
   AEERect        rc;

    // if non-null, then this widget is raised
   IXYContainer * pixyRaise;   // xy container that widget is raised to

#if defined(OPTIMIZ3)
   AEERect        rcDraw;
#endif
   flg            fVisible : 1;
   flg            fDraw : 1;
};

#define WNODE_CTOR(p)       ((p)->pNext = (p)->pPrev = (p))

// Insert 'pnew' after 'p'
#define WNODE_INSERTAFTER(p,pnew) \
( (pnew)->pPrev = (p),        \
  (pnew)->pNext = (p)->pNext, \
  (p)->pNext->pPrev = (pnew), \
  (p)->pNext = (pnew) )

// Insert 'pnew' before 'p'
#define WNODE_INSERTBEFORE(p,pnew) \
( (pnew)->pPrev = (p)->pPrev,     \
  (pnew)->pNext = (p),            \
  (p)->pPrev->pNext = (pnew),     \
  (p)->pPrev = (pnew) )


// Remove node 'p' from list
#define WNODE_REMOVE(p) \
( (p)->pPrev->pNext = (p)->pNext, \
  (p)->pNext->pPrev = (p)->pPrev )

#define WNODE_ENUM_FWD(pn,ph)    for ((pn) = (ph)->pNext; (pn) != (ph); (pn) = (pn)->pNext)
#define WNODE_ENUM_REV(pn,ph)    for ((pn) = (ph)->pPrev; (pn) != (ph); (pn) = (pn)->pPrev)

#define WNODE_INLIST(pn)    ((pn)->pNext != (pn))
#define WNODE_EMPTY(pn)     ((pn)->pNext == (pn))
#define WNODE_ZBOTTOM(ph)   ((ph)->pNext)
#define WNODE_ZTOP(ph)      ((ph)->pPrev)

void     WidgetNode_Ctor           (WidgetNode *me, IWidget *piWidget, AEERect *rc, boolean bVisible);
int      WidgetNode_FindWidget     (WidgetNode *head, IWidget *piWidget, WidgetNode **ppo);
IWidget *WidgetNode_GetWidget      (WidgetNode *head, IWidget *piw, boolean bNext, boolean bWrap);
boolean  WidgetNode_IntersectOpaque(WidgetNode *node, AEERect *rcOut, const AEERect *rcIn);
boolean  WidgetNode_SubtractOpaque (WidgetNode *node, AEERect *rcInOut);
void     WidgetNode_Dtor           (WidgetNode *me);
void     WidgetNode_Dtor           (WidgetNode *me);
void     WidgetNode_Delete         (WidgetNode *me);
void     WidgetNode_SetPos         (WidgetNode *me, int x, int y, boolean bVisible);
boolean  WidgetNode_PrefExChanged  (WidgetNode *me, IWidget *piWidget);

typedef struct ContainerBase ContainerBase;

struct IWidget {
   AEEBASE_INHERIT(IWidget, ContainerBase);
};

typedef int  (*PFNMKNODE) (const void *pDesc, IWidget *piw, WidgetNode **ppo);
typedef void (*PFNLAYOUT) (ContainerBase *me, WidgetNode *pn, AEERect *prc);

struct ContainerBase
{
   AEEVTBL(IContainer) *   pvt;
   uint32                  nRefs;
   IModule *               piModule;
   IModel *                piModel;

   WExtent                 extent;
   IContainer  *           piParent;

   IWidget                 widget;
   AEEVTBL(IWidget)        vtWidget;

   Border                  border;
   IModel *                piViewModel;

   HandlerDesc             hd;
   PFNHANDLER              pfnDefHandler;

   PFNMKNODE               pfnMkNode;
   PFNLAYOUT               pfnLayout;

   WidgetNode              head;
   WidgetNode *            pFocus;

   flg                     fInLayout : 1;
   flg                     fEnableLayout : 1;
   flg                     fBroadcast : 1;
};

//helper functions
//get at the IContainer from the IWidget object.  This ONLY works
//when IWidget is a pointer to the 'widget' member of ContainerBase
static __inline IContainer* ICONTAINER_FROM_WIDGET(IWidget *po){
   return (IContainer *)po->pMe;
}

#define CBASE(p) ((ContainerBase *)p)

void    ContainerBase_Ctor       (ContainerBase *me, AEEVTBL(IContainer) *pvt, 
                                  IModule *piModule, PFNHANDLER pfnDefHandler,
                                  PFNMKNODE pfnMkNode, PFNLAYOUT pfnLayout);
void    ContainerBase_Dtor       (ContainerBase *me);

// IQueryInterface
uint32  ContainerBase_AddRef           (IContainer *po);
uint32  ContainerBase_Release          (IContainer *po);
int     ContainerBase_QueryInterface   (IContainer *po, AEECLSID clsid, void **ppo);

// IWidget
boolean ContainerBase_DefHandleEvent    (IWidget *po, AEEEvent evt, uint16 wParam, uint32 dwParam);

void    ContainerBase_GetPreferredExtent(IWidget *po, WExtent *pwe);
void    ContainerBase_GetExtent         (IWidget *po, WExtent *pwe);
void    ContainerBase_SetExtent         (IWidget *po, WExtent *pwe);
void    ContainerBase_GetParent         (IWidget *po, IContainer **ppo);
void    ContainerBase_SetParent         (IWidget *po, IContainer *piContainer);
void    ContainerBase_Draw              (IWidget *po, ICanvas *piCanvas, int x, int y);
boolean ContainerBase_IntersectOpaque   (IWidget *po, AEERect *prcResult, const AEERect *prcIn);
int     ContainerBase_GetModel          (IWidget *po, AEECLSID iid, IModel **ppwm);
int     ContainerBase_SetModel          (IWidget *po, IModel *pwm);
void    ContainerBase_SetHandler        (IWidget *po, HandlerDesc *phd);

// IContainer
void    ContainerBase_Invalidate        (IContainer *po, IWidget *piw, const AEERect *prcInWidget, uint32 dwFlags);
int     ContainerBase_Locate            (IContainer *po, IWidget *piw, IContainer **ppRoot, AEERect *prcRelative);
int     ContainerBase_Insert            (IContainer *po, IWidget *piw, IWidget *piwBefore, const void *pDesc);
int     ContainerBase_Remove            (IContainer *po, IWidget *piw);
IWidget *ContainerBase_GetWidget        (IContainer *po, IWidget *piw, boolean bNext, boolean bWrap);

// Utility funcs for derived classes
boolean ContainerBase_CalcInvalidRect  (ContainerBase *me, IWidget *piw, const AEERect *prcInWidget, uint32 dwFlags, AEERect *rcInvalid);
void    ContainerBase_DoLayout         (ContainerBase *me, WidgetNode *pn, AEERect *prc);
int     ContainerBase_GetViewModel     (ContainerBase *me, IModel **ppiModel);
int     ContainerBase_FindWidget       (ContainerBase *me, IWidget *piw, WidgetNode **ppo, IWidget **ppiw, int *pnIndex);
void    ContainerBase_WidgetNodeRemove (ContainerBase *me, WidgetNode *pn);
boolean ContainerBase_SetProperty      (ContainerBase *me, uint16 wParam, uint32 dwParam);
boolean ContainerBase_GetProperty      (ContainerBase *me, uint16 wParam, uint32 dwParam);
void    ContainerBase_LayoutInvalidate (ContainerBase *me);
void    ContainerBase_DefDoLayout      (ContainerBase *me, WidgetNode *pNode, AEERect *prcInvalid);
void    ContainerBase_InvalidateMe     (ContainerBase *me, uint32 dwflags);

#endif /* __CUSTOMCONTAINERBASE_H__ */

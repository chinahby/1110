/*======================================================
FILE:       CustomContainer.c

SERVICES:   Custom Container class that does nothing

GENERAL DESCRIPTION:

  Reproduction and/or distribution of this file without the
  written consent of QUALCOMM, Incorporated. is prohibited.

        Copyright © 1999-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#include "CustomContainer_priv.h"
#include "CustomContainerBase.h"
#include "samplewidgets_util.h"

/////////////////////////////////////////////////////////////////////
// CustomContainer structure

typedef struct CustomContainer 
{
   ContainerBase     base;
} CustomContainer;


/////////////////////////////////////////////////////////////////////
// Function prototypes

static uint32 CustomContainer_Release(ICustomContainer *po);
static int CustomContainer_QueryInterface(ICustomContainer *po, AEECLSID id, void **ppo);
static void CustomContainer_Invalidate(ICustomContainer *po, IWidget *piw, const AEERect *prcInWidget, uint32 dwFlags);
static boolean CustomContainer_DefHandleEvent(IWidget *po, AEEEvent evt, uint16 wParam, uint32 dwParam);
static void CustomContainer_GetPreferredExtent(IWidget *po, WExtent *pwe);

static int WidgetNode_ForCustomContainer(void *prop, IWidget *piw, WidgetNode **ppo);
static void CustomContainer_DoLayout(CustomContainer *me);

static void CustomContainer_Ctor(CustomContainer *me, AEEVTBL(ICustomContainer) *pvt, IModule *piModule, PFNHANDLER pfnDefHandler);
static void CustomContainer_Dtor(CustomContainer *me);


/////////////////////////////////////////////////////////////////////
// CustomContainer implementation

/* -----------------------------------------------------------------------
    CustomContainer_Release 
   --- */
uint32 CustomContainer_Release(ICustomContainer *po)
{
   CustomContainer *me = (CustomContainer *)po;

   if (me->base.nRefs == 1) {
      CustomContainer_Dtor(me);
   }

   return ContainerBase_Release((IContainer*)po);
}


/* -----------------------------------------------------------------------
    CustomContainer_QueryInterface 
   --- */
int  CustomContainer_QueryInterface(ICustomContainer *po, AEECLSID id, void **ppo)
{
   if (id == AEEIID_CUSTOMCONTAINER) {
      *ppo = po;
      ICUSTOMCONTAINER_AddRef(po);
      return SUCCESS;
   }
   return ContainerBase_QueryInterface(ICUSTOMCONTAINER_TO_ICONTAINER(po), id, ppo);
}


/* -----------------------------------------------------------------------
    CustomContainer_Invalidate 
   --- */
void CustomContainer_Invalidate(ICustomContainer *po, IWidget *piw, const AEERect *prcInWidget, uint32 dwFlags)
{
   ContainerBase_Invalidate(ICUSTOMCONTAINER_TO_ICONTAINER(po), piw, prcInWidget, dwFlags);
}


/* -----------------------------------------------------------------------
    CustomContainer_DefHandleEvent 
   --- */
boolean CustomContainer_DefHandleEvent(IWidget *po, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   boolean bHandled;

   // Handle eventes.  For example ...
   bHandled = ContainerBase_DefHandleEvent(po, evt, wParam, dwParam);

   return bHandled;
}


/* -----------------------------------------------------------------------
    CustomContainer_GetPreferredExtent 
   --- */
void CustomContainer_GetPreferredExtent(IWidget *po, WExtent *pwe)
{ 
   // compute and return container's preferred size.
}


/* -----------------------------------------------------------------------
    WidgetNode_ForCustomContainer 
   --- */
static int WidgetNode_ForCustomContainer(void *prop, IWidget *piw, WidgetNode **ppo)
{
   // Make a widget node
   *ppo = NULL;
   return EUNSUPPORTED;
}


/* -----------------------------------------------------------------------
    CustomContainer_DoLayout 
   --- */
static void CustomContainer_DoLayout(CustomContainer *me)
{
   if(CBASE(me)->fEnableLayout) {

      CBASE(me)->fInLayout = 1;
   
      // do layout ...

      // Allow children invalidates
      CBASE(me)->fInLayout = 0;

      // invalidate
      ICONTAINER_Invalidate(CBASE(me)->piParent, &CBASE(me)->widget, NULL, 0);
   }
}


/* -----------------------------------------------------------------------
    CustomContainer_Ctor 
   --- */
void CustomContainer_Ctor(CustomContainer *me, AEEVTBL(ICustomContainer) *pvt, IModule *piModule, PFNHANDLER pfnDefHandler)
{

   // contruct base
   ContainerBase_Ctor(&me->base, 
                      (AEEVTBL(IContainer) *)pvt, 
                      piModule, 
                      pfnDefHandler ? pfnDefHandler : (PFNHANDLER)CustomContainer_DefHandleEvent,
                      (PFNMKNODE)WidgetNode_ForCustomContainer, 
                      (PFNLAYOUT)CustomContainer_DoLayout);

   // construct derived members
   pvt->Release         = CustomContainer_Release;
   pvt->QueryInterface  = CustomContainer_QueryInterface;
   pvt->Invalidate      = CustomContainer_Invalidate;

   CBASE(me)->vtWidget.GetPreferredExtent   = CustomContainer_GetPreferredExtent;

   // initialize other members
   //[...]
}


/* -----------------------------------------------------------------------
    CustomContainer_Dtor
   --- */
void CustomContainer_Dtor(CustomContainer *me)
{
   // release internal members (if any) ...

   // free ourselves

}


/* -----------------------------------------------------------------------
    CustomWidget_New  
   --- */
int CustomContainer_New(ICustomContainer **ppo, IShell *piShell, IModule *piModule)
{
   CustomContainer *me = MALLOCREC_EX(CustomContainer, sizeof(AEEVTBL(ICustomContainer)));

   *ppo = (ICustomContainer *)me;

   if (!me) {
      return ENOMEMORY;
   }

   CustomContainer_Ctor(me, GETVTBL(me,ICustomContainer), piModule, 0);
   return SUCCESS;
}

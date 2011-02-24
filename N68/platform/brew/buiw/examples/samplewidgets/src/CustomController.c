/*======================================================
FILE:       CustomController.c

SERVICES:   A sample controller that does nothing.

GENERAL DESCRIPTION:

  Reproduction and/or distribution of this file without the
  written consent of QUALCOMM, Incorporated. is prohibited.

        Copyright © 1999-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#include "CustomController_priv.h"
#include "CustomControllerBase.h"
#include "CustomWidgetBase.h"
#include "AEEWProperties.h"
#include "samplewidgets_util.h"

/////////////////////////////////////////////////////////////////////
// CustomController structure

typedef struct CustomController 
{
   ControllerBase     base;

   // some common members ...
   IShell             *piShell;
   IModel             *piModel;
   IModel             *piViewModel;
   ModelListener       mlModelChange;

} CustomController;

/////////////////////////////////////////////////////////////////////
// Function prototypes

static boolean CustomController_DefHandleEvent(IController *po, AEEEvent evt, uint16 wParam, uint32 dwParam);
static void CustomController_OnModelChanged(CustomController *me, ModelEvent *pev);
static int CustomController_SetWidget(IController *po, IWidget *piw);
static uint32 CustomController_Release(IController *po);

static void CustomController_Ctor(CustomController *me, AEEVTBL(IController) *pvt, IShell *piShell, IModule *piModule, PFNHANDLER pfnDefHandler);
static void CustomController_Dtor(CustomController *me);


/////////////////////////////////////////////////////////////////////
// CustomController implementation

/* -----------------------------------------------------------------------
    CustomController_DefHandleEvent 
   --- */
static boolean CustomController_DefHandleEvent(IController *po, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   CustomController *me = (CustomController *)po;

   if (!me->base.piw) {
      return FALSE;
   }
   
   // handle various events.  For example ...

   switch(evt) {
   
      case EVT_CHAR:
         break;

      case EVT_UPDATECHAR:
         break;

      case EVT_KEY_PRESS:
         break;

      case EVT_KEY_RELEASE: 
         break;

      case EVT_KEY: 
         break;
   
      case EVT_WDG_SETPROPERTY:
         break;

      case EVT_WDG_GETPROPERTY:
         break;
   }

   return ControllerBase_DefHandleEvent(po, evt, wParam ,dwParam);
}


/* -----------------------------------------------------------------------
    CustomController_OnModelChanged 
   --- */
static void CustomController_OnModelChanged(CustomController *me, ModelEvent *pev) 
{
   if (pev->evCode == EVT_MDL_SETMODEL) {
      RELEASEIF(me->piModel);
      IWIDGET_GetModel(me->base.piw, AEEIID_MODEL, (IModel**)&me->piModel);
   }
}


/* -----------------------------------------------------------------------
    CustomController_SetWidget 
   --- */
static int CustomController_SetWidget(IController *po, IWidget *piw)
{
   CustomController *me = (CustomController *)po;
   IModel *piModel = 0;

   if (piw && 
       (SUCCESS == IWIDGET_GetModel(piw, AEEIID_MODEL, (IModel**)&piModel))) {

      RELEASEIF(me->piModel);
      me->piModel = piModel;

      ControllerBase_SetWidget(po, piw);

      RELEASEIF(me->piViewModel);
      IWIDGET_GetViewModel(piw, &me->piViewModel);

      // listen on view model for notification of data model change
      if (me->piViewModel) {
         IMODEL_AddListenerEx(me->piViewModel, &me->mlModelChange, 
                              (PFNLISTENER)CustomController_OnModelChanged, me);
      }
      return SUCCESS;
   }

   return EFAILED;
}


/* -----------------------------------------------------------------------
    CustomController_Release 
   --- */
static uint32 CustomController_Release(IController *po)
{
   CustomController *me = (CustomController *)po;
   
   if (me->base.nRefs == 1) {
     CustomController_Dtor(me);
   }
   return ControllerBase_Release(po);
}


/* -----------------------------------------------------------------------
    CustomController_Ctor 
   --- */
static void CustomController_Ctor(CustomController *me, AEEVTBL(IController) *pvt, IShell *piShell, 
                                  IModule *piModule, PFNHANDLER pfnDefHandler)
{
   ControllerBase_Ctor(&me->base, pvt, piModule, DEFHANDLER(CustomController_DefHandleEvent));

   pvt->Release         = CustomController_Release;
   pvt->SetWidget       = CustomController_SetWidget;

   me->piShell = piShell;
   ISHELL_AddRef(me->piShell);

   // other initialization ...
   //[...]
}


/* -----------------------------------------------------------------------
    CustomController_Dtor
   --- */
static void CustomController_Dtor(CustomController *me)
{
   LISTENER_Cancel(&me->mlModelChange);

   RELEASEIF(me->piShell);
   RELEASEIF(me->piViewModel);
   RELEASEIF(me->piModel);
}


/* -----------------------------------------------------------------------
    CustomController_New  
   --- */
int CustomController_New(IController **ppo, IShell *piShell, IModule *piModule)
{
   CustomController *me = MALLOCREC_VTBL(CustomController, IController);
   if (!me) {
      return ENOMEMORY;
   }
   CustomController_Ctor(me, GETVTBL(me,IController), piShell, piModule, NULL);
   *ppo = (IController*) me;
   return SUCCESS;
}


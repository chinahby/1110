/*
  ========================================================================

  FILE:  AEECheckWidget.c
  
  SERVICES:  

  GENERAL DESCRIPTION: Implementation of the default check/radio button

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#include "CheckWidget.h"
#include "AEEWProperties.h"
#include "ModelBase.h"
#include "wutil.h"
#include "widget_res.h"

#include "AEEStdLib.h"
#include "AEEDisplayCanvas.h"


#define DEFCHECKSIZE    6
#define DEFRADIOSIZE    5

#if !defined(ARRAYSIZE)
#define ARRAYSIZE(a)    (sizeof(a) / sizeof(a[0]))
#endif

#define ME_FROM_WIDGET CheckWidget *me = (CheckWidget *)po


static __inline boolean CheckWidget_IsChecked(CheckWidget *me)
{
   return (me->base.piModel) ? IVALUEMODEL_GetBool((IValueModel*)me->base.piModel) : FALSE;
}

static __inline void CheckWidget_Invalidate(CheckWidget *me) 
{
   WidgetBase_InvalidateExtent(CHECKWIDGET_TO_IWIDGET(me));
}

static void CheckWidget_ModelChanged(CheckWidget *me, ModelEvent *pEvent)
{
   CheckWidget_Invalidate(me);
}

int Composite_SetModel(IWidget *po, IModel *piModel, PFNLISTENER pfnListner)
{
   ME_FROM_WIDGET;
   IValueModel *piValueModel = NULL;

   if (SUCCESS != IMODEL_QueryInterface(piModel, AEEIID_VALUEMODEL, (void **)&piValueModel)) {
      return ECLASSNOTSUPPORT;
   }
   // cuz we AddRef'ed in QI above
   RELEASEIF(piValueModel);

   WidgetBase_SetModel(po, piModel);
   IMODEL_AddListenerEx(piModel, &me->modelListener, (PFNLISTENER)pfnListner, me);
   CheckWidget_Invalidate(me);

   return SUCCESS;
}


int CheckWidget_SetModel(IWidget *po, IModel *piModel)
{
   return Composite_SetModel(po, piModel, (PFNLISTENER)CheckWidget_ModelChanged);
}


static void CheckWidget_BitmapDraw(IWidget *po, int16 idRes, ICanvas *piCanvas, int x, int y)
{
   ME_FROM_WIDGET;

   int nErr = 0;

   if (!TILEDIMAGE_HASIMAGE(&me->ti)) {
      nErr = TiledImage_LoadResBitmap(&me->ti, me->piShell, "widgets.mif", idRes, 3);
   }

   if (!nErr) {

      IDisplay *piDisplay = 0;
      if (SUCCESS == ICANVAS_GetDisplay(piCanvas, &piDisplay)) {
         // draw tile for the inactive or active border
         TiledImage_DrawTile(&me->ti, piDisplay, x, y, (me->bActive ? 2 : 1));
         // if checked, draw the check mark (or radio mark for radiowidget)
         if (CheckWidget_IsChecked(me)) {
            TiledImage_DrawTile(&me->ti, piDisplay, x, y, 0);
         }
      }
      RELEASEIF(piDisplay);
   }
}


void CheckWidget_Draw(IWidget *po, ICanvas *piCanvas, int x, int y)
{
   CheckWidget_BitmapDraw(po, IDB_CHECKSTRIP, piCanvas, x, y);
}


static void CheckWidget_BaseGetPreferredExtent(IWidget *po, int16 nBaseID, WExtent *pweOut)
{
   ME_FROM_WIDGET;
   int nErr = 0;

   if (!TILEDIMAGE_HASIMAGE(&me->ti)) {
      nErr = TiledImage_LoadResBitmap(&me->ti, me->piShell, "widgets.mif", nBaseID, 3);
   }

   if (!nErr) {
      pweOut->width  = me->ti.cxTile;
      pweOut->height = me->ti.cyTile;
   } else {
      pweOut->width = 10;
      pweOut->height = 10;
   }
}

void CheckWidget_GetPreferredExtent(IWidget *po, WExtent *pweOut)
{
   CheckWidget_BaseGetPreferredExtent(po, IDB_CHECKSTRIP, pweOut);
}

boolean CheckWidget_HandleEvent(IWidget *po, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   ME_FROM_WIDGET;

   switch (evt) {
   
   // yes, we take focus
      case EVT_WDG_CANTAKEFOCUS:
         *((boolean *)dwParam) = TRUE;
         return TRUE;

      case EVT_WDG_SETFOCUS:
         me->bActive = (boolean)wParam;
         CheckWidget_Invalidate(me);
         return TRUE;

      case EVT_KEY:
         if (wParam == AVK_SELECT) {
            if (me->base.piModel) {
               IValueModel *pivm = (IValueModel*)me->base.piModel;
               IVALUEMODEL_SetBool(pivm, (boolean)!IVALUEMODEL_GetBool(pivm));
            }
            return TRUE;
         }
         break;

      case EVT_WDG_SETPROPERTY:
         if (wParam == PROP_IMAGESTRIP) {
            TiledImage_SetBitmap(&me->ti, (IBitmap*)dwParam, 3);
            CheckWidget_Invalidate(me);
            return TRUE;
         }
         break;

   }

   return WidgetBase_DefHandleEvent(po, evt, wParam, dwParam);
}


uint32 CheckWidget_Release(IWidget *po)
{
   ME_FROM_WIDGET;

   if (me->base.nRefs == 1) {
      // checkwidget specific cleanup
      TiledImage_Dtor(&me->ti);

      RELEASEIF(me->piShell);
   }

   return WidgetBase_Release(po);
}


void CheckWidget_Ctor(CheckWidget *me, AEEVTBL(IWidget) *pvt, IValueModel *piModel, IShell *piShell, 
                      IModule *piModule, PFNHANDLER pfnDefHandler)
{
   WidgetBase_Ctor(&me->base, pvt, piModule, DEFHANDLER(CheckWidget_HandleEvent));

   pvt->Release            = CheckWidget_Release;
   pvt->GetPreferredExtent = CheckWidget_GetPreferredExtent;
   pvt->SetModel           = CheckWidget_SetModel;
   pvt->Draw               = CheckWidget_Draw;
      
   me->piShell = piShell;
   ADDREFIF(me->piShell);

   CheckWidget_SetModel((IWidget*)me, (IModel*)piModel);
}


/* CheckWidget_New
|| public
|| Create a CheckWidget and pass in an optional model to use. If NULL, use a 
|| default IValueModel implementation.
*/
int CheckWidget_New(IWidget **ppo, IValueModel *piModel, IShell *piShell, IModule *piModule)
{
   int nErr = SUCCESS;

   if (!piModel) {
      nErr = ValueModel_New(&piModel, piModule);
   } else {
      IVALUEMODEL_AddRef(piModel);
   }

   if (nErr == SUCCESS) {
      
      CheckWidget *me = MALLOCREC_VTBL(CheckWidget,IWidget);
      if (me) {
         CheckWidget_Ctor(me, GETVTBL(me,IWidget), piModel, piShell, piModule, 0);
         *ppo = (IWidget*)me;
      } else {
         nErr = ENOMEMORY;
      }
   }

   RELEASEIF(piModel);

   return nErr;
}

/////////////////////////////////////////////////////////////////////////////
// RadioWidget

#undef ME_FROM_WIDGET
#define ME_FROM_WIDGET  RadioWidget *me = (RadioWidget*)po


static void RadioWidget_SendRadioGroupEvent(RadioWidget *me)
{
   if (me->base.base.piViewModel) {
      ModelEvent event;
      event.evCode = EVT_MDL_GROUP_ITEMACTIVATE;
   
      me->bSentNotify = 1;
   
      IMODEL_Notify(me->base.base.piViewModel, &event);
   }
}


// we receive this event whenever any radio button in our group transitions to 'On'
// if we are the originator of the event, ignore it, otherwise reset our value model to 'Off'
static void RadioWidget_onEventRadioGroupOn(RadioWidget *me, ModelEvent *pEvent)
{
   if (pEvent->evCode == EVT_MDL_GROUP_ITEMACTIVATE) {
      if (!me->bSentNotify && me->base.base.piModel) {
         IVALUEMODEL_SetBool((IValueModel*)me->base.base.piModel, 0);
      }
      me->bSentNotify = 0;
   }
}


static void RadioWidget_SetViewModel(RadioWidget *me, IModel *piModel)
{
   LISTENER_Cancel(&me->mlGroup);

   RELEASEIF(me->base.base.piViewModel);

   me->base.base.piViewModel = piModel;

   if (me->base.base.piViewModel) {
      IMODEL_AddRef(me->base.base.piViewModel);
      IMODEL_AddListenerEx(me->base.base.piViewModel, &me->mlGroup, (PFNLISTENER)RadioWidget_onEventRadioGroupOn, me);
   }
}


static void RadioWidget_ModelChanged(RadioWidget *me, ModelEvent *pEvent)
{
   if (0 != IVALUEMODEL_GetBool((IValueModel*)pEvent->pModel)) {
      RadioWidget_SendRadioGroupEvent(me);
   }
   WidgetBase_InvalidateExtent((IWidget*)me);
}


int RadioWidget_SetModel(IWidget *po, IModel *piModel)
{
   return Composite_SetModel(po, piModel, (PFNLISTENER)RadioWidget_ModelChanged);
}


uint32 RadioWidget_Release(IWidget *po)
{
   ME_FROM_WIDGET;

   if (me->base.base.nRefs == 1) {
      // radio widget specific cleanup
      LISTENER_Cancel(&me->mlGroup);
   }

   return CheckWidget_Release(po);
}

void RadioWidget_GetPreferredExtent(IWidget *po, WExtent *pweOut)
{
   CheckWidget_BaseGetPreferredExtent(po, IDB_RADIOSTRIP, pweOut);
}


void RadioWidget_Draw(IWidget *po, ICanvas *piCanvas, int x, int y)
{
   CheckWidget_BitmapDraw(po, IDB_RADIOSTRIP, piCanvas, x, y);
}


boolean RadioWidget_HandleEvent(IWidget *po, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   ME_FROM_WIDGET;
   
   // radio widgets don't toggle off, they are only cleared 
   // by other radios in their group being checked

   switch (evt) {
      
      case EVT_KEY: 
         if (wParam == AVK_SELECT) {
            if (me->base.base.piModel) {
               IValueModel *pivm = (IValueModel*)me->base.base.piModel;
               IVALUEMODEL_SetBool(pivm, 1);
               // notify other widgets in our group
               RadioWidget_SendRadioGroupEvent(me);
            }
            return TRUE;
         }
         break;

      case EVT_WDG_SETPROPERTY:
         if (wParam == PROP_VIEWMODEL) {
            RadioWidget_SetViewModel(me, (IModel*)dwParam);
            return TRUE;
         }
         if (wParam == PROP_IMAGESTRIP) {
            TiledImage_SetBitmap(&me->base.ti, (IBitmap*)dwParam, 3);
            return TRUE;
         }
         break;

      case EVT_WDG_GETPROPERTY:
         if (wParam == PROP_VIEWMODEL) {
            if (SUCCESS == WidgetBase_GetViewModel(&me->base.base, (IModel**)dwParam)) {
               IMODEL_AddListenerEx(me->base.base.piViewModel, &me->mlGroup, (PFNLISTENER)RadioWidget_onEventRadioGroupOn, me);
            }
            return TRUE;
         }
         break;

   } // switch

   return CheckWidget_HandleEvent(po, evt, wParam, dwParam);
}



void RadioWidget_Ctor(RadioWidget *me, AEEVTBL(IWidget) *pvt, IValueModel *piModel, 
                      IShell *piShell, IModule *piModule, PFNHANDLER pfnDefHandler)
{
   CheckWidget_Ctor(&me->base, pvt, piModel, piShell, piModule, 
                    DEFHANDLER(RadioWidget_HandleEvent));

   pvt->Release             = RadioWidget_Release;
   pvt->Draw                = RadioWidget_Draw;
   pvt->GetPreferredExtent  = RadioWidget_GetPreferredExtent;
   pvt->SetModel            = RadioWidget_SetModel;

   RadioWidget_SetModel((IWidget*)me, (IModel*)piModel);
}


int RadioWidget_New(IWidget **ppo, IValueModel *piModel, IShell *piShell, IModule *piModule)
{
   int nErr = SUCCESS;

   if (!piModel) {
      nErr = ValueModel_New(&piModel, piModule);
   } else {
      IVALUEMODEL_AddRef(piModel);
   }

   if (nErr == SUCCESS) {
      RadioWidget *me = MALLOCREC_VTBL(RadioWidget,IWidget);
      if (me) {
         RadioWidget_Ctor(me, GETVTBL(me,IWidget), piModel, piShell, piModule, 0);
         *ppo = (IWidget *)me;
      } else {
         nErr = ENOMEMORY;
         *ppo = 0;
      }
   }

   RELEASEIF(piModel);

   return nErr;
}



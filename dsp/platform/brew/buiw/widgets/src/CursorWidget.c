/*
  ========================================================================

  FILE:  CursorWidget.c
  
  SERVICES: 

  GENERAL DESCRIPTION: CursorWidget implementation

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#include "CursorWidget.h"
#include "BitmapWidget.h"
#include "wutil.h"
#include "widget_res.h"
#include "modres.h"

#define ME_FROM_WIDGET     CursorWidget *me = ((CursorWidget*)po)
#define WBASE(p)          ((WidgetBase *)p)

static void CursorWidget_Dismiss(CursorWidget *me)
{
   IXYContainer *piContainer;

   // Get the parent container
   if (WBASE(me)->piContainer && 
       (SUCCESS == ICONTAINER_QueryInterface(WBASE(me)->piContainer, AEEIID_XYCONTAINER, 
                                             (void **)&piContainer))) {
      // And remove me
      IXYCONTAINER_Remove(piContainer, CURSORWIDGET_TO_IWIDGET(me));
      IXYCONTAINER_Release(piContainer);
   }
}

static void CursorWidget_Show(CursorWidget *me, CursorShow *pShow)
{
   // Hide if shown
   CursorWidget_Dismiss(me);

   if (pShow->piContainer) {
      WidgetPos pos;
      
      pos.x = pShow->x;
      pos.y = pShow->y;
      pos.bVisible = TRUE;

      IXYCONTAINER_Insert(pShow->piContainer, CURSORWIDGET_TO_IWIDGET(me), WIDGET_ZTOPMOST, &pos);
   }
}

boolean CursorWidget_HandleEvent(IWidget *po, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   ME_FROM_WIDGET;

   if (evt == EVT_WDG_SETPROPERTY) {

      if (wParam == PROP_CURSOR_SHOW) {
         CursorWidget_Show(me, (CursorShow *)dwParam);
         return TRUE;
      
      } else if (wParam == PROP_CURSOR_DISMISS) {
         CursorWidget_Dismiss(me);
         return TRUE;
      }
   } 
   return BitmapWidget_HandleEvent(po, evt, wParam, dwParam);
} 


int CursorWidget_QueryInterface(IWidget *po, AEECLSID clsid, void **ppo)
{
   if (SUCCESS == WidgetBase_QueryInterface(po, clsid, ppo)) {
      return SUCCESS;
   }

   if (clsid == AEECLSID_CURSORWIDGET) {
      *ppo = po;
      IWIDGET_AddRef(po);
      return SUCCESS;
   }

   *ppo = 0;
   return ECLASSNOTSUPPORT;
}



void CursorWidget_Dtor(CursorWidget *me)
{
   BitmapWidget_Dtor(&me->base);
}

void CursorWidget_Ctor(CursorWidget *me, AEEVTBL(IWidget) *pvt, IShell *piShell,
                       IModule *piModule, PFNHANDLER pfnDefHandler)
{
   BitmapWidget_Ctor(&me->base, pvt, piModule, DEFHANDLER(CursorWidget_HandleEvent));
   
   pvt->QueryInterface = CursorWidget_QueryInterface;

   me->piShell = piShell;
   ISHELL_AddRef(me->piShell);
}


int CursorWidget_Initialize(CursorWidget *me)
{
   int nErr;
   IBitmap *pib = 0;

   nErr = BitmapWidget_Initialize(&me->base, me->piShell);

   if (!nErr) {
      pib = WidgetLib_LoadResBitmap(me->piShell, "widgets.mif", IDB_CURSOR);
      if (!pib) {
         nErr = ERESOURCENOTFOUND;
      }
   }

   if (!nErr) {
      IWIDGET_SetTranspColor((IWidget*)me, MAKE_RGB(255,0,255));
      IWIDGET_SetBitmap((IWidget*)me, pib);
   }

   RELEASEIF(pib);

   return nErr;
}

/////////////////////////////////////////////////////////////////
// Concrete implementation

int CursorWidget_New(IWidget **ppo, IShell *piShell, IModule *piModule)
{
   int nErr;
   CursorWidget *me = MALLOCREC_VTBL(CursorWidget, IWidget);
   
   *ppo = 0;

   if (!me) {
      return ENOMEMORY;
   }

   CursorWidget_Ctor(me, GETVTBL(me, IWidget), piShell, piModule, 0);
   nErr = CursorWidget_Initialize(me);

   if (!nErr) {
      *ppo = (IWidget*)me;
   } else {
      CursorWidget_Dtor(me);
      FREE(me);
   }

   return nErr;
}




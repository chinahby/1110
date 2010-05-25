/*
  ========================================================================

  FILE:  SoftkeyWidget.c
  
  SERVICES:  

  GENERAL DESCRIPTION: Softkey widget implementation

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#include "SoftkeyWidget.h"
#include "AEEWProperties.h"
#include "AEEStaticWidget.h"
#include "AEEPropContainer.h"
#include "wutil.h"

boolean SoftkeyWidget_HandleEvent(SoftkeyWidget *me, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   int result = 0;

   if (HANDLERDESC_Call(&me->handler, evt, wParam, dwParam)) {
      if (evt == EVT_WDG_SETFOCUS || evt == EVT_WDG_CANTAKEFOCUS) {
         return FALSE;
      } else {
         return TRUE;
      }
   } else if (evt == EVT_WDG_GETPROPERTY) {
      switch(wParam) {

      case PROP_SOFTKEY1:
         *(IWidget **)dwParam = IPROPCONTAINER_GetWidget(me->piContainer, 0, TRUE, FALSE);
         ADDREFIF(*(IWidget **)dwParam);
         return TRUE;

      case PROP_SOFTKEY2:
         *(IWidget **)dwParam = IPROPCONTAINER_GetWidget(me->piContainer, 0, FALSE, FALSE);
         ADDREFIF(*(IWidget **)dwParam);
         return TRUE;

      }
   } else if (evt == EVT_WDG_SETPROPERTY) {
      IWidget *piWidget;
      WidgetProp prop;

      switch(wParam) {

      case PROP_SOFTKEY1:
         piWidget = IPROPCONTAINER_GetWidget(me->piContainer, 0, TRUE, FALSE);
         if (piWidget) {
            prop.prop = 1;
            prop.bVisible = 1;
            IPROPCONTAINER_Remove(me->piContainer, piWidget);
            IPROPCONTAINER_Insert(me->piContainer, (IWidget *)dwParam, WIDGET_ZBOTTOMMOST, &prop);
         }
         return TRUE;

      case PROP_SOFTKEY2:
         piWidget = IPROPCONTAINER_GetWidget(me->piContainer, 0, TRUE, FALSE);
         if (piWidget) {
            prop.prop = 1;
            prop.bVisible = 1;
            IPROPCONTAINER_Remove(me->piContainer, piWidget);
            IPROPCONTAINER_Insert(me->piContainer, (IWidget *)dwParam, WIDGET_ZTOPMOST, &prop);
         }
         return TRUE;
      }
   }


   {
      IWidget *piWidget;
      
      piWidget = IPROPCONTAINER_GetWidget(me->piContainer, 0, TRUE, FALSE);
      while (piWidget) {
         result |= IWIDGET_HandleEvent(piWidget, evt, wParam, dwParam);
         piWidget = IPROPCONTAINER_GetWidget(me->piContainer, piWidget, TRUE, FALSE);
      }
   }
   
   return result;
}

static void SoftkeyWidget_Delete(SoftkeyWidget *me)
{
   FREE(me);
}

int SoftkeyWidget_New(IWidget **ppo, IShell *piShell, IModule *piModule)
{
   int nErr, i;
   IWidget *piContWidget = 0;
   SoftkeyWidget *me = MALLOCREC(SoftkeyWidget);

   if (!me)
      return ENOMEMORY;

   nErr = ISHELL_CreateInstance(piShell, AEECLSID_PROPCONTAINER, (void **)&me->piContainer);
   
   if (!nErr)
      nErr = IPROPCONTAINER_QueryInterface(me->piContainer, AEEIID_WIDGET, (void **)&piContWidget);

   if (!nErr) {
      IWIDGET_SetLayoutStyle(piContWidget, LAYOUT_HORZ);
      IWIDGET_SetBorderWidth(piContWidget, 0);
      IWIDGET_SetRightPadding(piContWidget, 4);

      HANDLERDESC_Init(&me->handler, SoftkeyWidget_HandleEvent, me, SoftkeyWidget_Delete);
      IWIDGET_SetHandler(piContWidget, &me->handler);
      // Release container to avoid circular reference between handler and container
      IPROPCONTAINER_Release(me->piContainer);
   }

   // Create labels
   for (i = 0; i < 2; i++) {
      IWidget *piLabel;
      WidgetProp prop;

      prop.prop = 1;
      prop.bVisible = 1;

      nErr = ISHELL_CreateInstance(piShell, AEECLSID_STATICWIDGET, (void **)&piLabel);
      if (!nErr) {
         IWIDGET_SetFGColor(piLabel, RGBA_BLACK);
         IWIDGET_SetBGColor(piLabel, RGBA_NONE);
         IWIDGET_SetFlags(piLabel, (i == 0 ? IDF_ALIGN_LEFT : IDF_ALIGN_RIGHT) | IDF_ALIGN_MIDDLE);

         IPROPCONTAINER_Insert(me->piContainer, piLabel, WIDGET_ZNORMAL, &prop);
         IWIDGET_Release(piLabel);
      }
   }

   // set a default extent
   {
      WExtent we;
      we.width    = 50;
      we.height   = 10;
      IWIDGET_SetExtent(piContWidget, &we);
   }
   
   *ppo = piContWidget;
   return nErr;
}



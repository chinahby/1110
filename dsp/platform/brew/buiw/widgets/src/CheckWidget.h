/*
  ========================================================================

  FILE:  CheckWidget.h
  
  SERVICES:  

  GENERAL DESCRIPTION: Check and Radiobutton widgets

  Properties:

  PROP_BORDERCOLOR


  ========================================================================
  ========================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __CHECKWIDGET_H__
#define __CHECKWIDGET_H__

#include "AEECheckWidget.h"
#include "AEEWModel.h"
#include "WidgetBase.h"
#include "wutil.h"

typedef struct CheckWidget CheckWidget;

struct CheckWidget {
   WidgetBase     base;
   boolean        bActive;
   ModelListener  modelListener;
   IShell *       piShell;
   WExtent        we;
   TiledImage     ti;
};


typedef struct RadioWidget RadioWidget;

struct RadioWidget {
   CheckWidget    base;
//   IModel *       piViewModel;     // group model for radios
   ModelListener  mlGroup;          // group listener
   boolean        bSentNotify;
};


/* 
   Radio grouping works as follows:
   
   1) The 'primary' radio widget is created and it's
      group model is obtained:

         IWidget *piw[3];
         IModel *piViewModel;
   
         ISHELL_CreateInstance(piShell, AEECLSID_RADIOWIDGET, (void**)&piw[0]);
         IWIDGET_GetViewModel(piw, &piViewModel);
      
   2) Other widgets are passed the group model after creation:
   
         for (i=0; i < 2; i++) {
            ISHELL_CreateInstance(piShell, AEECLSID_RADIOWIDGET, (void**)&piw[i+1]);
            IWIDGET_SetViewModel(piw[i+1], &piViewModel);
         }
         
         
   3) When passed a group model, the widget just AddRef's it and
      adds it's own modelListener to the Listener chain. The 
      primary widget, when asked for a group model, allocates
      it on demand and also adds it's modelListener to the listener chain.
         
*/



// Safe upcasts
static __inline IWidget *CHECKWIDGET_TO_IWIDGET(CheckWidget *p) { 
   return (IWidget *)p; 
}

static __inline 
WidgetBase *CHECKWIDGET_TO_WIDGETBASE(CheckWidget *p) { 
   return (WidgetBase *)p; 
}

int      CheckWidget_New                  (IWidget **ppo, IValueModel *piModel, IShell *piShell, IModule *piModule);
void     CheckWidget_Ctor                 (CheckWidget *me, AEEVTBL(IWidget) *pvt, IValueModel *piModel, IShell *piShell, 
                                           IModule *piModule, PFNHANDLER pfnDefHandler);
void     CheckWidget_GetPreferredExtent   (IWidget *po, WExtent *pweOut);
void     CheckWidget_Draw                 (IWidget *po, ICanvas *piCanvas, int x, int y);
boolean  CheckWidget_HandleEvent          (IWidget *po, AEEEvent evt, uint16 wParam, uint32 dwParam);
int      CheckWidget_SetModel             (IWidget *po, IModel *piModel);

int      RadioWidget_New                  (IWidget **ppo, IValueModel *piModel, IShell *piShell, IModule *piModule);
void     RadioWidget_Ctor                 (RadioWidget *me, AEEVTBL(IWidget) *pvt, IValueModel *piModel, IShell *piShell, 
                                           IModule *piModule, PFNHANDLER pfnDefHandler);
void     RadioWidget_GetPreferredExtent   (IWidget *po, WExtent *pweOut);
void     RadioWidget_Draw                 (IWidget *po, ICanvas *piCanvas, int x, int y);
boolean  RadioWidget_HandleEvent          (IWidget *po, AEEEvent evt, uint16 wParam, uint32 dwParam);


#endif /* __CHECKWIDGET_H__ */

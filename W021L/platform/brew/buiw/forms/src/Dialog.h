/*
  ========================================================================

  FILE:  Dialog.h
  
  SERVICES:  

  GENERAL DESCRIPTION: A Dialog is a popup with a backdrop and a title

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __DIALOG_H__
#define __DIALOG_H__

#include "AEEDialog.h"
#include "Popup.h"

/////////////////////////////////////////////////////////////////
// Derivable struct, methods and macros

typedef struct Dialog {
   Popup base;

   IWidget *     piTitle;
   IWidget *     piBackground;

} Dialog;

int     Dialog_New(IForm **ppo, IModule *piModule, IShell *piShell);
int     Dialog_Construct(Dialog *me, AEEVTBL(IForm) *pvt, IModule *piModule, 
                         IShell *piShell, PFNHANDLER pfnDefHandler);
void    Dialog_Dtor(Dialog *me);
boolean Dialog_HandleEvent(IForm *po, AEEEvent evt, uint16 wParam, uint32 dwParam);
uint32  Dialog_Release(IForm *po);

static __inline IForm *DIALOG_TO_IFORM(Dialog *me) {
   return (IForm *)me;
}


/////////////////////////////////////////////////////////////////
// Dialog with a static field
typedef struct StaticDialog {
   Dialog base;
   IWidget *piText;
} StaticDialog;

int     StaticDialog_Construct(StaticDialog *me, AEEVTBL(IForm) *pvt, IModule *piModule, 
                               IShell *piShell, PFNHANDLER pfnDefHandler);
void    StaticDialog_Dtor(StaticDialog *me);
int     StaticDialog_New(IForm **ppo, IModule *piModule, IShell *piShell);
boolean StaticDialog_HandleEvent(IForm *po, AEEEvent evt, uint16 wParam, uint32 dwParam);
uint32  StaticDialog_Release(IForm *po);

static __inline IForm *STATICDIALOG_TO_IFORM(StaticDialog *me) {
   return (IForm *)me;
}

int     ResDialog_New(IForm **ppo, IModule *piModule, IShell *piShell, char *baseName);

/////////////////////////////////////////////////////////////////
// Dialog with a static field and a progress bar

typedef struct ProgressDialog {
   StaticDialog base;
   IWidget * piProgress;
} ProgressDialog;

int     ProgressDialog_New(IForm **ppo, IModule *piModule, IShell *piShell);
int     ProgressDialog_Construct(ProgressDialog *me, AEEVTBL(IForm) *pvt, IModule *piModule, 
                                 IShell *piShell, PFNHANDLER pfnDefHandler);
void    ProgressDialog_Dtor(ProgressDialog *me);
boolean ProgressDialog_HandleEvent(IForm *po, AEEEvent evt, uint16 wParam, uint32 dwParam);
uint32  ProgressDialog_Release(IForm *po);

static __inline IForm *PROGRESSDIALOG_TO_IFORM(ProgressDialog *me) {
   return (IForm *)me;
}

#endif /* __DIALOG_H__ */

/*
  ========================================================================

  FILE:  ListForm.h
  
  SERVICES:  

  GENERAL DESCRIPTION: Defines a form that contains a list as its main
  element. Responds to WID_LIST and WID_DECORATOR to fetch the listwidget
  itself. No listitem widget is set by default

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#ifndef __LISTFORM_H__
#define __LISTFORM_H__

#include "AEEListForm.h"
#include "Form.h"

typedef struct ListForm 
{
   Form base;
   IWidget *         piList;     // ListWidget (WID_LIST) is different than WID_FORM widget
   IWidget *         piScrollbar;
   ModelListener     selectListener;
   PFNSELECT         pfnSelect;
   void *            pvSelect;
} ListForm;


static __inline IForm *LISTFORM_TO_IFORM(ListForm *me) {
   return (IForm *)me;
}

int     ListForm_New        (IForm **ppo, IModule *piModule, IShell *piShell, boolean bScrollbar);
uint32  ListForm_Release    (IForm *po);

void    ListForm_Ctor       (ListForm *me, AEEVTBL(IForm) *pvt, IModule *piModule, IShell *piShell, 
                             IWidget *piList, IWidget *piScrollbar, IWidget *piFormWidget, PFNHANDLER pfnDefHandler);

void    ListForm_Dtor       (ListForm *me);

boolean ListForm_HandleEvent(IForm *po, AEEEvent evt, uint16 wParam, uint32 dwParam);

#endif /* __LISTFORM_H__ */

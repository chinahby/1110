/*
  ========================================================================

  FILE:  Popup.h
  
  SERVICES:  

  GENERAL DESCRIPTION: A popup is a form with background, dropshadow and
  tends to take up only part of the screen (ie. it's not a full size form)

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/


#ifndef __POPUP_H__
#define __POPUP_H__

#include "AEEPopup.h"

#include "Form.h"
#include "AEEMenuModel.h"
#include "AEETransition.h"
#include "AEEWProperties.h"

typedef struct Popup 
{
   Form base;
   
   IDecorator *  piDecorator;
   IWidget *     piBackdrop;
   ITransition * piBackFader; // for backdrop fadeout
   uint32        fadeMS; 
} Popup;

#define POPUP_DECORATOR(p)  (((Popup *)p)->piDecorator)
#define POPUP_BACKDROP(p)   (((Popup *)p)->piBackdrop)
#define POPUP_BACKFADER(p)  (((Popup *)p)->piBackFader)
#define POPUP_FADEMS(p)     (((Popup *)p)->fadeMS)

#define BACKDROP_TRANS    70

int     Popup_New(IForm **ppo, IModule *piModule, IShell *piShell);
int     Popup_Construct(Popup *me, AEEVTBL(IForm) *pvt, IModule *piModule, IShell *piShell, boolean bBackdrop, PFNHANDLER pfnDefHandler);
void    Popup_Dtor(Popup *me);
boolean Popup_HandleEvent(IForm *po, AEEEvent evt, uint16 wParam, uint32 dwParam);
void    Popup_removeBackdrop(Popup *me);
void    Popup_fadeRemoveBackdrop(Popup *me);

static __inline IForm *POPUP_TO_IFORM(Popup *me) {
   return (IForm *)me;
}

/////////////////////////////////////////////////////////////////
// PopupMenu

typedef struct PopupMenu
{
   Popup base;

   ITransition *     piFader; // for fadeout deactivate

   ModelListener     ml;
   PFNSELECT         pfnSelect;
   void *            pvSelect;


   IndexerDesc       indexer;

   IWidget *         piList;
   IWidget *         piItem;
   IWidget *         piScrollbar;
} PopupMenu;

int     PopupMenu_New(IPopupMenu **ppo, IModule *piModule, IShell *piShell, boolean bScrollbar);
int     PopupMenu_Construct(PopupMenu *me, AEEVTBL(IPopupMenu) *pvt, IModule *piModule, IShell *piShell, boolean bScrollbar, PFNHANDLER pfnDefHandler);
boolean PopupMenu_HandleEvent(IPopupMenu *po, AEEEvent evt, uint16 wParam, uint32 dwParam);

static __inline IForm *POPUPMENU_TO_IFORM(PopupMenu *me) {
   return (IForm *)me;
}




#endif /* __POPUP_H__ */

/*======================================================
FILE: TextController.h

SERVICES: 
   IController implementation for text widget

GENERAL DESCRIPTION:
  
  Reproduction and/or distribution of this file without the written consent of
  QUALCOMM, Incorporated. is prohibited.

        Copyright © 1999-2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#ifndef __TEXTCONTROLLER_H__
#define __TEXTCONTROLLER_H__

#include "ControllerBase.h"
#include "AEETextController.h"
#include "AEETextModel.h"
//#include "AEEXYContainer.h"
#include "WidgetBase.h"

/////////////////////////////////////////////////////////////////////////////
// TextController


typedef struct {
   ControllerBase    base;
   IShell *          piShell;
   ITextModel *      piTextModel;   // 
   AEECallback       cbkTimer;      // multitap text entry timer
   uint32            nMode;         // current text translation mode
   IWidget *         piwTT;         // tooltip for mode indication
   uint32            dwFlags;       // widget flags
   uint16            wParamKey;     // last AVK_LEFT or AVK_RIGHT key pressed (for auto-repeat)
   int               nKeyRep;       // key repeat delay in ms.
   flg               bForceCaps : 1;// force caps flag (used in multitap mode)
   flg               bEndSentence : 1;
   IModel *          piViewModel;   // text widget's view model
   int               nMaxChars;     // maximum char's allowed as input
   ModelListener     mlModelChange; // listens on widgets ViewModel for EVT_MDL_SETMODEL
   int               nMaskTimeout;  // password mask timeout in milliseconds
   AVKType           avkMode;       // AVK_key to consider as "input mode change" key
} TextController;


int      TextController_New            (IController **ppo, IShell *piShell, IModule *piModule);
void     TextController_Ctor           (TextController *me, AEEVTBL(IController) *pvt, IShell *piShell, 
                                        IModule *piModule, PFNHANDLER pfnDefHandler);
void     TextController_Dtor           (TextController *me);

uint32   TextController_AddRef         (IController *po);
uint32   TextController_Release        (IController *po);
int      TextController_QueryInterface (IController *po, AEECLSID id, void **ppo);
void     TextController_SetHandler     (IController *, HandlerDesc *pDesc);
int      TextController_SetWidget      (IController *, IWidget *piWidget);




#endif   //__TEXTCONTROLLER_H__



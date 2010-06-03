/*
  ========================================================================

  FILE:                 sampleapp.c
      
  SERVICES:             SampleApp

  GENERAL DESCRIPTION:  Example application that uses example widgets
  

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

// BREW Includes
#include "AEEShell.h"
#include "AEEStdLib.h"

// BUIW Includes
#include "AEEWidget.h"

// Custom Widget Includes
#include "CustomCaretWidget.h"
#include "CustomContainer.h"
#include "CustomController.h"
#include "CustomModel.h"
#include "CustomWidget.h"


//------------------------------------------------------------------------
//    Types and Macros
//-----------------------------------------------------------------------F

// My Class ID
#include "../inc/SAMPLEWIDGETSAPP.BID"

// SampleApp struct
typedef struct SampleApp 
{  
   IApplet           ia;
   IAppletVtbl       iavt;  
   int               nRefs;
   IShell           *piShell;
   IModule          *piModule;
   IDisplay         *piDisplay;
} SampleApp;

// ReleaseIf Macro
#ifndef RELEASEIF
#define RELEASEIF(p)       ReleaseIf((IBase**)&p)
static void ReleaseIf(IBase **ppif) {
   if (*ppif) {
      IBASE_Release(*ppif); 
      *ppif = 0;
   }
}
#endif

//--------------------------------------------------------------------
//   Globals 
//--------------------------------------------------------------------

static const AECHAR gszTextSuccess[] = {'C','r','e','a','t','e','d',' ','S','a','m','p','l','e',' ','W','i','d','g','e','t','s', '\0'};
static const AECHAR gszTextFailed[] = {'F','a','i','l','e','d','!', '\0'};

//--------------------------------------------------------------------
//   Function Prototypes 
//--------------------------------------------------------------------

static uint32 SampleApp_AddRef(IApplet *po);
static uint32 SampleApp_Release(IApplet *po);
static boolean SampleApp_HandleEvent(IApplet * pi, AEEEvent eCode, uint16 wParam, uint32 dwParam);

static int SampleApp_Ctor(SampleApp *me, IModule * piModule, IShell * piShell);
static void SampleApp_Dtor(SampleApp *me);

static void SampleApp_CreateCustomWidgets(SampleApp *me);


//--------------------------------------------------------------------
//   Function Definitions 
//--------------------------------------------------------------------

//--------------------------------------------------------------------
//   xmod CreateInstance
//--------------------------------------------------------------------
#include "xmod.h"
int xModule_CreateInstance(IModule *module, IShell *shell, AEECLSID idClass, void **pp)
{
   if (idClass == AEECLSID_SAMPLEWIDGETSAPP) {
      SampleApp *me = MALLOCREC(SampleApp);

      if (me == 0) {
         return ENOMEMORY;
      } else {
         *pp = &me->ia;
         return SampleApp_Ctor(me, module, shell);
      }
   }

   *pp = 0;
   return ECLASSNOTSUPPORT;
}

//--------------------------------------------------------------------
//   SampleApp Constructor
//--------------------------------------------------------------------
static int SampleApp_Ctor(SampleApp *me, IModule * piModule, IShell * piShell)
{
   int nErr = 0;

   me->ia.pvt           = &me->iavt;
   me->iavt.AddRef      = SampleApp_AddRef;
   me->iavt.Release     = SampleApp_Release;
   me->iavt.HandleEvent = SampleApp_HandleEvent;

   me->nRefs = 1;

   me->piShell = piShell;
   ISHELL_AddRef(piShell);

   me->piModule = piModule;
   IMODULE_AddRef(piModule);

   nErr = ISHELL_CreateInstance(me->piShell, AEECLSID_DISPLAY, (void **)&me->piDisplay);

   return nErr;
}

//--------------------------------------------------------------------
//   SampleApp Destructor
//--------------------------------------------------------------------
static void SampleApp_Dtor(SampleApp *me)
{
   RELEASEIF(me->piDisplay);
   RELEASEIF(me->piModule);
   RELEASEIF(me->piShell);
}

//--------------------------------------------------------------------
//   SampleApp AddRef
//--------------------------------------------------------------------
static uint32 SampleApp_AddRef(IApplet *po)
{
   SampleApp *me = (SampleApp *)po;
   return ++me->nRefs;
}

//--------------------------------------------------------------------
//   SampleApp Release
//--------------------------------------------------------------------
static uint32 SampleApp_Release(IApplet *po)
{
   SampleApp *me = (SampleApp *)po;
   uint32 nRefs = --me->nRefs;
   if (nRefs == 0) {
      SampleApp_Dtor(me);
      FREE(me);
   }
   return nRefs;
}

//--------------------------------------------------------------------
//   SampleApp Event Handler
//--------------------------------------------------------------------
static boolean SampleApp_HandleEvent(IApplet * pi, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{  
   SampleApp *me = (SampleApp *)pi;

   switch (eCode) {
      
      case EVT_APP_START:
         SampleApp_CreateCustomWidgets(me);
         return TRUE;

      case EVT_APP_STOP:
         return TRUE;   

   }  // switch(eCode)
   return FALSE;
}

//--------------------------------------------------------------------
//   Attempt to create the sample widgets
//--------------------------------------------------------------------
static void SampleApp_CreateCustomWidgets(SampleApp *me)
{
   int nErr;
   IContainer *pic = NULL;
   IController *picnt = NULL;
   IModel *pim = NULL;
   IWidget *piw = NULL;
   IWidget *picw = NULL;
   const AECHAR *pszText = gszTextFailed;

   // create sample widgets
   nErr = ISHELL_CreateInstance(me->piShell, AEECLSID_CUSTOMCONTAINER, (void **)&pic);
   if (nErr == SUCCESS) {
      nErr = ISHELL_CreateInstance(me->piShell, AEECLSID_CUSTOMCONTROLLER, (void **)&picnt);
   }
   if (nErr == SUCCESS) {
      nErr = ISHELL_CreateInstance(me->piShell, AEECLSID_CUSTOMMODEL, (void **)&pim);
   }
   if (nErr == SUCCESS) {
      nErr = ISHELL_CreateInstance(me->piShell, AEECLSID_CUSTOMWIDGET, (void **)&piw);
   }
   if (nErr == SUCCESS) {
      nErr = ISHELL_CreateInstance(me->piShell, AEECLSID_CUSTOMCARETWIDGET, (void **)&picw);
   }

   // cleanup
   RELEASEIF(picw);
   RELEASEIF(piw);
   RELEASEIF(pim);
   RELEASEIF(picnt);
   RELEASEIF(pic);

   // display result
   if (nErr == SUCCESS) {
      pszText = gszTextSuccess;
   }
   IDISPLAY_DrawText(me->piDisplay, AEE_FONT_NORMAL, pszText, -1, 0, 0,NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
   IDISPLAY_Update (me->piDisplay);
}


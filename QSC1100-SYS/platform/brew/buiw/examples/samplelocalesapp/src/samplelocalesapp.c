/*
  ========================================================================

  FILE:                 samplelocalesapp.c
      
  SERVICES:             SampleLocalesApp

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
#include "AEELocale.h"

//------------------------------------------------------------------------
//    Types and Macros
//------------------------------------------------------------------------

// My Class ID
#include "../inc/samplelocalesapp.bid"

// SampleLocalesApp struct
typedef struct SampleLocalesApp 
{  
   IApplet           ia;
   IAppletVtbl       iavt;  
   int               nRefs;
   IShell           *piShell;
   IModule          *piModule;
   IDisplay         *piDisplay;
} SampleLocalesApp;

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

// mime types for the default and sample ILocale objects
static const char gszMimeTypeGB[] = "text/plain;engb";
static const char gszMimeTypeDE[] = "text/plain;dede";

// other strings
static const AECHAR gszEnglish[] = {'e','n','g','b',':',0};
static const AECHAR gszGerman[]  = {'d','e','d','e',':',0};

//--------------------------------------------------------------------
//   Function Prototypes 
//--------------------------------------------------------------------

static uint32 SampleLocalesApp_AddRef(IApplet *po);
static uint32 SampleLocalesApp_Release(IApplet *po);
static boolean SampleLocalesApp_HandleEvent(IApplet * pi, AEEEvent eCode, uint16 wParam, uint32 dwParam);

static int SampleLocalesApp_Ctor(SampleLocalesApp *me, IModule * piModule, IShell * piShell);
static void SampleLocalesApp_Dtor(SampleLocalesApp *me);

static void SampleLocalesApp_UseSampleLocales(SampleLocalesApp *me);


//--------------------------------------------------------------------
//   Function Definitions 
//--------------------------------------------------------------------

//--------------------------------------------------------------------
//   xmod CreateInstance
//--------------------------------------------------------------------
#include "xmod.h"
int xModule_CreateInstance(IModule *module, IShell *shell, AEECLSID idClass, void **pp)
{
   if (idClass == AEECLSID_SAMPLELOCALESAPP) {
      SampleLocalesApp *me = MALLOCREC(SampleLocalesApp);

      if (me == 0) {
         return ENOMEMORY;
      } else {
         *pp = &me->ia;
         return SampleLocalesApp_Ctor(me, module, shell);
      }
   }

   *pp = 0;
   return ECLASSNOTSUPPORT;
}

//--------------------------------------------------------------------
//   SampleLocalesApp Constructor
//--------------------------------------------------------------------
static int SampleLocalesApp_Ctor(SampleLocalesApp *me, IModule * piModule, IShell * piShell)
{
   int nErr = 0;

   me->ia.pvt           = &me->iavt;
   me->iavt.AddRef      = SampleLocalesApp_AddRef;
   me->iavt.Release     = SampleLocalesApp_Release;
   me->iavt.HandleEvent = SampleLocalesApp_HandleEvent;

   me->nRefs = 1;

   me->piShell = piShell;
   ISHELL_AddRef(piShell);

   me->piModule = piModule;
   IMODULE_AddRef(piModule);

   nErr = ISHELL_CreateInstance(me->piShell, AEECLSID_DISPLAY, (void **)&me->piDisplay);

   return nErr;
}

//--------------------------------------------------------------------
//   SampleLocalesApp Destructor
//--------------------------------------------------------------------
static void SampleLocalesApp_Dtor(SampleLocalesApp *me)
{
   RELEASEIF(me->piDisplay);
   RELEASEIF(me->piModule);
   RELEASEIF(me->piShell);
}

//--------------------------------------------------------------------
//   SampleLocalesApp AddRef
//--------------------------------------------------------------------
static uint32 SampleLocalesApp_AddRef(IApplet *po)
{
   SampleLocalesApp *me = (SampleLocalesApp *)po;
   return ++me->nRefs;
}

//--------------------------------------------------------------------
//   SampleLocalesApp Release
//--------------------------------------------------------------------
static uint32 SampleLocalesApp_Release(IApplet *po)
{
   SampleLocalesApp *me = (SampleLocalesApp *)po;
   uint32 nRefs = --me->nRefs;
   if (nRefs == 0) {
      SampleLocalesApp_Dtor(me);
      FREE(me);
   }
   return nRefs;
}

//--------------------------------------------------------------------
//   SampleLocalesApp Event Handler
//--------------------------------------------------------------------
static boolean SampleLocalesApp_HandleEvent(IApplet * pi, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{  
   SampleLocalesApp *me = (SampleLocalesApp *)pi;

   switch (eCode) {
      
      case EVT_APP_START:
         SampleLocalesApp_UseSampleLocales(me);
         return TRUE;

      case EVT_APP_STOP:
         return TRUE;   

   }  // switch(eCode)
   return FALSE;
}

//--------------------------------------------------------------------
//   Attempt to use our default and two sample locales
//--------------------------------------------------------------------
static void SampleLocalesApp_UseSampleLocales(SampleLocalesApp *me)
{
   int nErr;
   int nAscent, nDescent, nLineY = 0;
   AEECLSID clsId;
   ILocale *pil;
   AECHAR *pText;
   
   // determine line height
   IDISPLAY_GetFontMetrics(me->piDisplay, AEE_FONT_NORMAL, &nAscent, &nDescent);

   // clear screen
   IDISPLAY_ClearScreen(me->piDisplay);

   // display the day names for the two  languages
   // supported in the sample: GB English and German.

   // GB English
   IDISPLAY_DrawText(me->piDisplay, AEE_FONT_NORMAL, gszEnglish, -1, 0, nLineY, NULL, 0);
   nLineY += nAscent + nDescent;

   clsId = ISHELL_GetHandler(me->piShell, AEEIID_LOCALE, gszMimeTypeGB);
   if (clsId) {
      nErr = ISHELL_CreateInstance(me->piShell, clsId, (void **)&pil);
      if ((SUCCESS == nErr) && pil) {

         // display the long date pattern for this language
         nErr = ILOCALE_GetDayNames(pil, &pText);
         if (SUCCESS == nErr) {
            IDISPLAY_DrawText(me->piDisplay, AEE_FONT_NORMAL, pText, -1, 0, nLineY, NULL, 0);
            nLineY += nAscent + nDescent;
         }
         ILOCALE_Release(pil);
      }
   }
   nLineY += nAscent + nDescent;

   // German
   IDISPLAY_DrawText(me->piDisplay, AEE_FONT_NORMAL, gszGerman, -1, 0, nLineY, NULL, 0);
   nLineY += nAscent + nDescent;

   clsId = ISHELL_GetHandler(me->piShell, AEEIID_LOCALE, gszMimeTypeDE);
   if (clsId) {
      nErr = ISHELL_CreateInstance(me->piShell, clsId, (void **)&pil);
      if ((SUCCESS == nErr) && pil) {

         // display the long date pattern for this language
         nErr = ILOCALE_GetDayNames(pil, &pText);
         if (SUCCESS == nErr) {
            IDISPLAY_DrawText(me->piDisplay, AEE_FONT_NORMAL, pText, -1, 0, nLineY, NULL, 0);
         }
         ILOCALE_Release(pil);
      }
   }

   // update the screen
   IDISPLAY_Update (me->piDisplay);
}


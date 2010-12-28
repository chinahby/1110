/*===========================================================================

FILE: helloformapp.c
===========================================================================*/


/*===============================================================================
INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */

#include "AEE.h"              // Standard AEE Declarations
#include "AEEShell.h"         // AEE Shell Services
#include "AEEFont.h"          // AEE Font Services
#include "AEEDisp.h"          // AEE Display Services
#include "AEEStdLib.h"        // AEE StdLib Services
#include "AEERootForm.h"
#include "AEEStaticWidget.h"
#include "AEEImageWidget.h"
#include "AEEImageStaticWidget.h"
#include "AEETitleWidget.h"
#include "AEESoftkeyWidget.h"

#include "../inc/helloformapp.bid"

#ifndef RELEASEIF
#define RELEASEIF(p)       ReleaseIf((IBase**)&p)
static void ReleaseIf(IBase **ppif) {
   if (*ppif) {
      IBASE_Release(*ppif); 
      *ppif = 0;
   }
}
#endif

/*-------------------------------------------------------------------
Applet structure. All variables in here are reference via "me->"
-------------------------------------------------------------------*/
// create an applet structure that's passed around. All variables in
// here will be able to be referenced as static.
typedef struct _helloformapp {

	IApplet              ia;   
	IAppletVtbl          iavt;  	
	int                  nRefs;
    IDisplay             *pIDisplay;  // give a standard way to access the Display interface
    IShell               *pIShell;    // give a standard way to access the Shell interface
	IModule				 *pIModule;
	AEEDeviceInfo        DeviceInfo;  // always have access to the hardware device information

    // add your own variables here...
	IRootForm            *piRootForm; // root form
	IForm                *piForm;     // form to be displayed
} helloformapp;

static const AECHAR awchSoftkey1[] = {'T','e','s','t', 0};
static const AECHAR awchSoftkey2[] = {'E','x','i','t', 0};
static const AECHAR awchTitle[] = {'S','a','m','p','l','e',' ','F','o','r','m', 0};
static const AECHAR awchText[] =  { 'H','e','l','l','o',' ','t','h','e','r','e',',',' ','c','a','n',' ','y','o','u',' ','s','e','e',' ','m','e','?', 0};
static const AECHAR awchNewText[] =  { 'W','e','l','l',',',' ','I',' ','s','u','p','p','o','s','e',' ','y','o','u',' ','s','a','w',' ','m','e',' ',':',')', 0};
const RGBAVAL rgbFrmBg = MAKE_RGBA(0,0,128,255);
const RGBAVAL rgbFrmFg = MAKE_RGBA(255,255,0,255);
const RGBAVAL rgbFrmBrd = MAKE_RGBA(255,255,255,255);

/*-------------------------------------------------------------------
Function prototypes
-------------------------------------------------------------------*/
static uint32  helloformapp_AddRef (IApplet *po);
static uint32  helloformapp_Release (IApplet *po);
static  boolean  helloformapp_HandleEvent (IApplet *pi, AEEEvent eCode, uint16 wParam, uint32 dwParam);

static boolean helloformapp_HandleEvent(IApplet * pi, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{  
	helloformapp *me = (helloformapp *)pi;
	IWidget *piw = NULL;

    switch (eCode) {
        // App is told it is starting up
        case EVT_APP_START:                        
            // Add your code here...
			return(TRUE);

        // App is told it is exiting
        case EVT_APP_STOP:
            // Add your code here...
      		return(TRUE);

        // App is being suspended 
        case EVT_APP_SUSPEND:
		    // Add your code here...
      		return(TRUE);

        // App is being resumed
        case EVT_APP_RESUME:
		    // Add your code here...
      		return(TRUE);


        // An SMS message has arrived for this app. Message is in the dwParam above as (char *)
        // sender simply uses this format "//BREW:ClassId:Message", example //BREW:0x00000001:Hello World
        case EVT_APP_MESSAGE:
		    // Add your code here...
      		return(TRUE);

        // A key was pressed. Look at the wParam above to see which key was pressed. The key
        // codes are in AEEVCodes.h. Example "AVK_1" means that the "1" key was pressed.
        case EVT_KEY:
			switch (wParam) {
			   // UP
			   case AVK_UP:
				   return TRUE;

			   // DOWN
			   case AVK_DOWN:
					return TRUE;

			   // LEFT
			   case AVK_LEFT:
				   return TRUE;

			   // RIGHT
			   case AVK_RIGHT:
				   return TRUE;

			   // SELECT
			   case AVK_SELECT:
				   return TRUE;

			   // CLEAR
			   case AVK_CLR:
				   return TRUE;

			   // SOFTKEY 1
			   case AVK_SOFT1:
				   // will change the text here
				   IFORM_GetWidget(me->piForm, WID_FORM, &piw);
				   IWIDGET_SetText(piw, awchNewText, TRUE); 
				   RELEASEIF(piw);
				   return TRUE;

			   // SOFTKEY 2
			   case AVK_SOFT2:
				   IROOTFORM_PopForm(me->piRootForm);
				   ISHELL_CloseApplet(me->pIShell, FALSE);
				   return TRUE;
			}

			return (TRUE);

        // If nothing fits up to this point then we'll just break out
        default:
            break;
   }

   return FALSE;
}

static int helloformapp_CreateForm(helloformapp *me)
{
	int nErr = SUCCESS;
	IWidget *piw = NULL;

	// create the root form
	nErr = ISHELL_CreateInstance(me->pIShell, AEECLSID_ROOTFORM, (void **)&me->piRootForm);

	// create the form
	if(SUCCESS == nErr) {
		nErr = ISHELL_CreateInstance(me->pIShell, AEECLSID_FORM, (void **)&me->piForm);
	}

	// set some primary widget properties
	nErr = ISHELL_CreateInstance(me->pIShell, AEECLSID_STATICWIDGET, (void **)&piw); 
	if(SUCCESS == nErr) {
		IWIDGET_SetBGColor(piw, rgbFrmBg);
		IWIDGET_SetBorderColor(piw, rgbFrmBrd);
		IWIDGET_SetBorderWidth(piw, 1);
		IWIDGET_SetFGColor(piw, rgbFrmFg);
		IWIDGET_SetFlags(piw, IDF_ALIGN_CENTER|IDF_ALIGN_MIDDLE);
		IWIDGET_SetText(piw, awchText, TRUE);

		nErr = IFORM_SetWidget(me->piForm, WID_FORM, piw);
		RELEASEIF(piw);
	}

	// set some keys and titles
	IFORM_SetText(me->piForm, FID_TITLE, awchTitle);
	IFORM_SetText(me->piForm, FID_SOFTKEY1, awchSoftkey1);
	IFORM_SetText(me->piForm, FID_SOFTKEY2, awchSoftkey2);

	// push the form to the root form
	nErr = IROOTFORM_PushForm(me->piRootForm, me->piForm);

    // if there have been no failures up to this point then return success
    return nErr;
}

// Destroy samplehtmlwidgetapp object
static void helloformapp_Dtor(helloformapp *me)
{
	RELEASEIF(me->piForm);
	RELEASEIF(me->piRootForm);
	RELEASEIF(me->pIDisplay);
	RELEASEIF(me->pIModule);
	RELEASEIF(me->pIShell);
}

static int helloformapp_Construct(helloformapp *me, IModule * piModule, IShell * piShell)
{
   me->ia.pvt           = &me->iavt;
   me->iavt.AddRef      = helloformapp_AddRef;
   me->iavt.Release     = helloformapp_Release;
   me->iavt.HandleEvent = helloformapp_HandleEvent;

   me->nRefs = 1;

   me->pIShell = piShell;
   ISHELL_AddRef(piShell);

   me->pIModule = piModule;
   IMODULE_AddRef(piModule);
   
   helloformapp_CreateForm(me);

   return 0;
}

static uint32 helloformapp_AddRef(IApplet *po)
{
   helloformapp *me = (helloformapp *)po;
   return ++me->nRefs;
}


static uint32 helloformapp_Release(IApplet *po)
{
   helloformapp *me = (helloformapp *)po;
   uint32 nRefs = --me->nRefs;
   if (nRefs == 0) {
      helloformapp_Dtor(me);
      FREE(me);
   }
   return nRefs;
}


#include "xmod.h"

int xModule_CreateInstance(IModule *module, IShell *shell, AEECLSID idClass, void **pp)
{
   if (idClass == AEECLSID_HELLOFORMAPP) {
      helloformapp *me = MALLOCREC(helloformapp);

      if (me == 0) {
         return ENOMEMORY;
      } else {
         *pp = &me->ia;
         return helloformapp_Construct(me, module, shell);
      }
   }

   *pp = 0;
   return ECLASSNOTSUPPORT;
}

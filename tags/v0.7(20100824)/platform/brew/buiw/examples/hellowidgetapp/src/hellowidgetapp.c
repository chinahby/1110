/*===========================================================================

FILE: hellowidgetapp.c
===========================================================================*/


/*===============================================================================
INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */

#include "AEE.h"              // Standard AEE Declarations
#include "AEEShell.h"         // AEE Shell Services
#include "AEEFont.h"          // AEE Font Services
#include "AEEDisp.h"          // AEE Display Services
#include "AEEStdLib.h"        // AEE StdLib Services
#include "AEEText.h"
#include "AEEWidget.h"
#include "AEERootContainer.h"
#include "AEEDisplayCanvas.h"
#include "AEEModel.h"
#include "AEEWModel.h"
#include "AEEStaticWidget.h"
#include "AEECheckWidget.h"
#include "AEETextWidget.h"
#include "AEEImageWidget.h"
#include "AEEImageStaticWidget.h"

#include "../inc/hellowidgetapp.bid"

#ifndef RELEASEIF
#define RELEASEIF(p)       ReleaseIf((IBase**)&p)
static void ReleaseIf(IBase **ppif) {
   if (*ppif) {
      IBASE_Release(*ppif); 
      *ppif = 0;
   }
}
#endif

#define HOME_PAGE "hsrc/index.htm"
#define GO_BACK 0

/*-------------------------------------------------------------------
Applet structure. All variables in here are reference via "me->"
-------------------------------------------------------------------*/
// create an applet structure that's passed around. All variables in
// here will be able to be referenced as static.
typedef struct _hellowidgetapp {

	IApplet              ia;   
	IAppletVtbl          iavt;  	
	int                  nRefs;
    IDisplay             *pIDisplay;  // give a standard way to access the Display interface
    IShell               *pIShell;    // give a standard way to access the Shell interface
	IModule				 *pIModule;
	AEEDeviceInfo        DeviceInfo; // always have access to the hardware device information

    // add your own variables here...
	//IController *        piTextCtl;		// text controller
	IRootContainer *     picRoot;		// root container
	IWidget *            piwRoot;		// root container's widget i/f
	AEERect              rcRoot;		// dimensions of root container
	ICanvas *            piCanvas;		// our drawing surface
	IWidget *            piwTitle;		// a title widget
	IWidget *            piwCheck;		// a check widget
	IWidget *            piwHello;		// a static widget
	IWidget *            piwText;		// a text widget
	IWidget *            piwCPrompt;    // a static widget
	IWidget *            piwTPrompt;    // a static widget

} hellowidgetapp;

static const AECHAR awchTitle[] = {'H','e','l','l','o',' ','W','i','d','g','e','t',' ','A','p','p', 0};
static const AECHAR awchHello[] = {'H','e','l','l','o',' ','t','h','e','r','e',',',' ','c','a','n',' ','y','o','u',' ','s','e','e',' ','m','e','?', 0};
static const AECHAR awchPrompt[] = {'T','h','i','s',' ','i','s',' ','a',' ','c','h','e','c','k',' ','w','i','d','g','e','t', 0};
static const AECHAR awchTextCtlTitle[] = {'T','h','i','s',' ','i','s',' ','a','n',' ','i','n','p','u','t', 0};

/*-------------------------------------------------------------------
Function prototypes
-------------------------------------------------------------------*/
static uint32  hellowidgetapp_AddRef (IApplet *po);
static uint32  hellowidgetapp_Release (IApplet *po);
static  boolean  hellowidgetapp_HandleEvent (IApplet *pi, AEEEvent eCode, uint16 wParam, uint32 dwParam);

static boolean hellowidgetapp_HandleEvent(IApplet * pi, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{  
	hellowidgetapp *me = (hellowidgetapp *)pi;

	// try text widget
	if(me->piwText){
		if (IWIDGET_HandleEvent(me->piwText, eCode, wParam, dwParam)) {
			return TRUE;
		}
	}

	// try check widget
	if(me->piwCheck){
		if (IWIDGET_HandleEvent(me->piwCheck, eCode, wParam, dwParam)) {
			return TRUE;
		}
	}

    switch (eCode) {
        // App is told it is starting up
        case EVT_APP_START:                        
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
				   IWIDGET_MoveFocus(me->piwRoot, (IWidget *)WIDGET_FOCUS_PREV);
				   return TRUE;

			   // DOWN
			   case AVK_DOWN:
				   IWIDGET_MoveFocus(me->piwRoot, (IWidget *)WIDGET_FOCUS_NEXT);
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
			}

			return (TRUE);

        // If nothing fits up to this point then we'll just break out
        default:
            break;
   }

   return FALSE;
}

static int hellowidgetapp_CreateHelloWidget(hellowidgetapp *me)
{

	int nErr=0;
	IBitmap *pib = 0;
	WidgetPos dwpos;
	WExtent we;
	WExtent oldwe;

	// reset widget position
	dwpos.x = dwpos.y = 0;

    me->DeviceInfo.wStructSize = sizeof(me->DeviceInfo);
    ISHELL_GetDeviceInfo(me->pIShell,&me->DeviceInfo);

    nErr = ISHELL_CreateInstance(me->pIShell, AEECLSID_DISPLAY, (void **)&(me->pIDisplay));

	// get an instance of root container
	nErr = ISHELL_CreateInstance(me->pIShell, AEECLSID_ROOTCONTAINER, (void *)(&me->picRoot));
	if (SUCCESS != nErr) {
		return FALSE;
	}

	// query interface to root containers widget
	nErr = IROOTCONTAINER_QueryInterface(me->picRoot, AEEIID_WIDGET, (void *)(&me->piwRoot));
	if (SUCCESS != nErr) {
		return FALSE;
	}

	// get the device bitmap
	nErr = IDISPLAY_GetDeviceBitmap(me->pIDisplay, &pib);
	if (SUCCESS != nErr) {
		return FALSE;
	}

	// set the root container extent, and get display canvas
	{
		AEEBitmapInfo  biDevice;
		IBITMAP_GetInfo(pib, &biDevice, sizeof(biDevice));
		SETAEERECT(&(me->rcRoot), 0, 0, (short) biDevice.cx, (short) biDevice.cy);
		nErr = ISHELL_CreateInstance(me->pIShell, AEECLSID_DISPLAYCANVAS, (void**)&(me->piCanvas));
		if (SUCCESS != nErr) {
			IBITMAP_Release(pib);
			return FALSE;
		}
	}
	
	nErr = IDISPLAYCANVAS_SetDisplay((IDisplayCanvas*)me->piCanvas, me->pIDisplay);
	if (SUCCESS != nErr) {
		return FALSE;
	}

	IROOTCONTAINER_SetCanvas(me->picRoot, me->piCanvas, &(me->rcRoot));

	// begin creating all widgets here
	// --- title widget
    nErr = ISHELL_CreateInstance(me->pIShell, AEECLSID_STATICWIDGET, (void **)&(me->piwTitle));
	if (SUCCESS != nErr) {
		return FALSE;
	}
	IWIDGET_SetFlags(me->piwTitle, IDF_ALIGN_CENTER|IDF_ALIGN_MIDDLE);
	IWIDGET_SetText(me->piwTitle, awchTitle, TRUE);
	// set extent	
	IWIDGET_GetPreferredExtent(me->piwTitle, &we);
	if(we.width < (me->DeviceInfo).cxScreen) {
		we.width = (me->DeviceInfo).cxScreen;
	}
	IWIDGET_SetExtent(me->piwTitle, &we);
	// set location
	dwpos.x += 0;
	dwpos.y += 0;
	dwpos.bVisible = TRUE;
	// save the extent
	oldwe = we;
	// insert widget into root container
	IROOTCONTAINER_Insert(me->picRoot, me->piwTitle, WIDGET_ZNORMAL,&dwpos); 
	RELEASEIF(me->piwTitle);

	// --- hello widget
    nErr = ISHELL_CreateInstance(me->pIShell, AEECLSID_STATICWIDGET, (void **)&(me->piwHello));
	if (SUCCESS != nErr) {
		return FALSE;
	}
	IWIDGET_SetFlags(me->piwHello, IDF_ALIGN_LEFT|IDF_ALIGN_TOP);
	IWIDGET_SetText(me->piwHello, awchHello, TRUE);
	// set extent	
	IWIDGET_GetPreferredExtent(me->piwHello, &we);
	if(we.width < (me->DeviceInfo).cxScreen) {
		we.width = (me->DeviceInfo).cxScreen;
	}
	IWIDGET_SetExtent(me->piwHello, &we);
	// set location
	dwpos.x += 0;
	dwpos.y += oldwe.height + 10;
	dwpos.bVisible = TRUE;
	// save the extent
	oldwe = we;
	// insert widget into root container
	IROOTCONTAINER_Insert(me->picRoot, me->piwHello, WIDGET_ZNORMAL,&dwpos); 
	RELEASEIF(me->piwHello);

	// --- check widget
    nErr = ISHELL_CreateInstance(me->pIShell, AEECLSID_CHECKWIDGET, (void **)&(me->piwCheck));
	if (SUCCESS != nErr) {
		return FALSE;
	}	
	// set extent	
	IWIDGET_GetPreferredExtent(me->piwCheck, &we);
	IWIDGET_SetExtent(me->piwCheck, &we);
	// set location
	dwpos.x += 0;
	dwpos.y += we.height + 7;
	dwpos.bVisible = TRUE;
	// save the extent
	oldwe = we;
	// insert widget into root container
	IROOTCONTAINER_Insert(me->picRoot, me->piwCheck, WIDGET_ZNORMAL,&dwpos); 
	
	// --- prompt widget for check input
    nErr = ISHELL_CreateInstance(me->pIShell, AEECLSID_STATICWIDGET, (void **)&(me->piwCPrompt));
	if (SUCCESS != nErr) {
		return FALSE;
	}
	IWIDGET_SetFlags(me->piwCPrompt, IDF_ALIGN_LEFT|IDF_ALIGN_TOP);
	IWIDGET_SetText(me->piwCPrompt, awchPrompt, TRUE);
	// set extent	
	IWIDGET_GetPreferredExtent(me->piwCPrompt, &we);
	we.width = (me->DeviceInfo).cxScreen - oldwe.width - 2;
	IWIDGET_SetExtent(me->piwCPrompt, &we);
	// set location
	dwpos.x += oldwe.width + 2;
	dwpos.bVisible = TRUE;
	// save the extent
	oldwe = we;
	// insert widget into root container
	IROOTCONTAINER_Insert(me->picRoot, me->piwCPrompt, WIDGET_ZNORMAL,&dwpos); 
	RELEASEIF(me->piwCPrompt);

	// --- prompt widget for text input
    nErr = ISHELL_CreateInstance(me->pIShell, AEECLSID_STATICWIDGET, (void **)&(me->piwTPrompt));
	if (SUCCESS != nErr) {
		return FALSE;
	}
	IWIDGET_SetFlags(me->piwTPrompt, IDF_ALIGN_LEFT|IDF_ALIGN_TOP);
	IWIDGET_SetText(me->piwTPrompt, awchTextCtlTitle, TRUE);
	// set extent	
	IWIDGET_GetPreferredExtent(me->piwTPrompt, &we);
	we.width = (me->DeviceInfo).cxScreen - we.width - 2;
	IWIDGET_SetExtent(me->piwTPrompt, &we);
	// set location
	dwpos.x = 0;
	dwpos.y += oldwe.height + 7;
	dwpos.bVisible = TRUE;
	// save the extent
	oldwe = we;
	// insert widget into root container
	IROOTCONTAINER_Insert(me->picRoot, me->piwTPrompt, WIDGET_ZNORMAL,&dwpos); 
	RELEASEIF(me->piwTPrompt);

	// --- text widget
    nErr = ISHELL_CreateInstance(me->pIShell, AEECLSID_TEXTWIDGET, (void **)&(me->piwText));
	if (SUCCESS != nErr) {
		return FALSE;
	}
	// set extent	
	IWIDGET_GetPreferredExtent(me->piwText, &we);
	we.width = (me->DeviceInfo).cxScreen - 2;
	IWIDGET_SetExtent(me->piwText, &we);
	// set location
	dwpos.x = 0;
	dwpos.y += oldwe.height + 7;
	dwpos.bVisible = TRUE;
	// save the extent
	oldwe = we;
	// insert widget into root container
	IROOTCONTAINER_Insert(me->picRoot, me->piwText, WIDGET_ZNORMAL, &dwpos); 
		
	// Set focus
	IWIDGET_MoveFocus((IWidget *)me->piwRoot, (IWidget *)WIDGET_FOCUS_FIRST);

	// release bitmap
	RELEASEIF(pib);

    // if there have been no failures up to this point then return success
    return TRUE;
}

static int hellowidgetapp_Construct(hellowidgetapp *me, IModule * piModule, IShell * piShell)
{
   me->ia.pvt           = &me->iavt;
   me->iavt.AddRef      = hellowidgetapp_AddRef;
   me->iavt.Release     = hellowidgetapp_Release;
   me->iavt.HandleEvent = hellowidgetapp_HandleEvent;

   me->nRefs = 1;

   me->pIShell = piShell;
   ISHELL_AddRef(piShell);

   me->pIModule = piModule;
   IMODULE_AddRef(piModule);
   
   hellowidgetapp_CreateHelloWidget(me);

   return 0;
}

// Destroy hellowidgetapp object
static void hellowidgetapp_Dtor(hellowidgetapp *me)
{
	RELEASEIF(me->piwTPrompt);
	RELEASEIF(me->piwCPrompt);
	RELEASEIF(me->piwText);
	RELEASEIF(me->piwHello);
	RELEASEIF(me->piwCheck);
	RELEASEIF(me->piwTitle);
	RELEASEIF(me->piwRoot);
	RELEASEIF(me->pIDisplay);
	RELEASEIF(me->piCanvas);
	RELEASEIF(me->pIModule);
	RELEASEIF(me->pIShell);
}


static uint32 hellowidgetapp_AddRef(IApplet *po)
{
   hellowidgetapp *me = (hellowidgetapp *)po;
   return ++me->nRefs;
}


static uint32 hellowidgetapp_Release(IApplet *po)
{
   hellowidgetapp *me = (hellowidgetapp *)po;
   uint32 nRefs = --me->nRefs;
   if (nRefs == 0) {
      hellowidgetapp_Dtor(me);
      FREE(me);
   }
   return nRefs;
}


#include "xmod.h"

int xModule_CreateInstance(IModule *module, IShell *shell, AEECLSID idClass, void **pp)
{
   if (idClass == AEECLSID_HELLOWIDGETAPP) {
      hellowidgetapp *me = MALLOCREC(hellowidgetapp);

      if (me == 0) {
         return ENOMEMORY;
      } else {
         *pp = &me->ia;
         return hellowidgetapp_Construct(me, module, shell);
      }
   }

   *pp = 0;
   return ECLASSNOTSUPPORT;
}

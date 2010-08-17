/*===========================================================================

FILE: samplehtmlwidgetapp.c
===========================================================================*/


/*===============================================================================
INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */

#include "AEE.h"              // Standard AEE Declarations
#include "AEEShell.h"         // AEE Shell Services
#include "AEEFont.h"          // AEE Font Services
#include "AEEDisp.h"          // AEE Display Services
#include "AEEStdLib.h"        // AEE StdLib Services
#include "AEEWeb.h"
#include "AEEWidget.h"
#include "AEERootContainer.h"
#include "AEEDisplayCanvas.h"
#include "AEEFile.h"			// File interface definitions
#include "AEEModel.h"
#include "AEEWModel.h"
#include "AEEDocModel.h"
#include "AEEHtmlViewModel.h"
#include "AEEHtmlWidget.h"
#include "AEEStaticWidget.h"
#include "AEEImageWidget.h"
#include "AEEImageStaticWidget.h"

#include "../inc/samplehtmlwidgetapp.bid"

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
typedef struct _samplehtmlwidgetapp {

	IApplet              ia;   
	IAppletVtbl          iavt;  	
	int                  nRefs;
    IDisplay             *pIDisplay;  // give a standard way to access the Display interface
    IShell               *pIShell;    // give a standard way to access the Shell interface
	IModule				 *pIModule;
	AEEDeviceInfo        DeviceInfo; // always have access to the hardware device information

    // add your own variables here...
	IRootContainer *     picRoot;		// root container
	IWidget *            piwRoot;		// root container's widget i/f
	AEERect              rcRoot;		// dimensions of root container
	ICanvas *            piCanvas;		// our drawing surface
	IWidget *            piwHtml;       // widget under test (HTMLWidget)
	IDocModel *          piDocModel;	// document model of HTMLWidget
	IHtmlViewModel *     piViewModel;	// view model of HTMLWidget
	IFileMgr *           piFileM;		// handle to file mgr
	ISourceUtil *        piSourceUtil;	// handle to source util
	IWeb *               piWeb;		// interface used to get sub-objects by html widget

	ModelListener        mlHtmlView;	// listner for HTML View Model
	ModelListener        mlHtmlDoc;	// listner for HTML Doc Model

	char *               pszHist[40];
	int32                ndxHist;		// history index
	int32                ndxAlink;	// current index of the <a> link in document

   char                 *pcBufstate;
   int                  nBufsize;
} samplehtmlwidgetapp;

/*-------------------------------------------------------------------
Function prototypes
-------------------------------------------------------------------*/
static uint32  samplehtmlwidgetapp_AddRef (IApplet *po);
static uint32  samplehtmlwidgetapp_Release (IApplet *po);
static  boolean  samplehtmlwidgetapp_HandleEvent (IApplet *pi, AEEEvent eCode, uint16 wParam, uint32 dwParam);
void  samplehtmlwidgetapp_loadhtmlfile (samplehtmlwidgetapp* me, const char * pszFilename);
void  samplehtmlwidgetapp_htmlviewlistener (samplehtmlwidgetapp* me, ModelEvent *pEv);
void  samplehtmlwidgetapp_htmldoclistener (samplehtmlwidgetapp* me, ModelEvent *pEv);



static boolean samplehtmlwidgetapp_HandleEvent(IApplet * pi, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{  
	samplehtmlwidgetapp *me = (samplehtmlwidgetapp *)pi;
	
	if(me->piwHtml){
	   if (IWIDGET_HandleEvent(me->piwHtml, eCode, wParam, dwParam)) {
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
			   // LEFT
			   case AVK_LEFT:
				   samplehtmlwidgetapp_loadhtmlfile(me,NULL);
				   return TRUE;

			   // RIGHT
			   case AVK_RIGHT:
				   return TRUE;

			   // SELECT
			   case AVK_SELECT:
				   return TRUE;

			   // CLEAR
			   case AVK_CLR:
				   samplehtmlwidgetapp_loadhtmlfile(me,HOME_PAGE);
				   return TRUE;

            // SOFTKEY
            case AVK_SOFT1:
               return TRUE;

            case AVK_SOFT2:
				   return TRUE;
			}

			return (TRUE);

        // If nothing fits up to this point then we'll just break out
        default:
            break;
   }

   return FALSE;
}

static int samplehtmlwidgetapp_CreateHtmlWidget(samplehtmlwidgetapp *me)
{
	int nErr=0;
	IBitmap *pib = 0;
	IModel *piModel = 0;
	WidgetPos dwpos;
	WExtent we;

   me->DeviceInfo.wStructSize = sizeof(me->DeviceInfo);
   ISHELL_GetDeviceInfo(me->pIShell,&me->DeviceInfo);

   nErr = ISHELL_CreateInstance(me->pIShell, AEECLSID_DISPLAY, (void **)&(me->pIDisplay));

	// initialize counter vars
	me->ndxAlink = 0;
	me->ndxHist = 0;

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

	// IWeb interface
    nErr = ISHELL_CreateInstance(me->pIShell, AEECLSID_WEB, (void**)&(me->piWeb));
	if (SUCCESS != nErr) {
		return FALSE;
	}

	// html widget creation process
	nErr = ISHELL_CreateInstance(me->pIShell, AEECLSID_HTMLWIDGET, (void**)&(me->piwHtml));
	if (SUCCESS != nErr) {
		return FALSE;
	}

	IWIDGET_SetBGColor(me->piwHtml, MAKE_RGBA(255,255,255,255));
	we.width = (me->rcRoot).dx;
	we.height = (me->rcRoot).dy;
	IWIDGET_SetExtent(me->piwHtml,&we);
	IWIDGET_SetIWeb(me->piwHtml, me->piWeb);

	nErr = IWIDGET_GetModel(me->piwHtml, AEEIID_DOCMODEL, (IModel**)&(me->piDocModel));
	if (SUCCESS != nErr) {
		return FALSE;
	}

	nErr = IWIDGET_GetViewModel(me->piwHtml, (IModel**)&piModel);
	if (SUCCESS != nErr) {
		IMODEL_Release(piModel);
		return FALSE;
	}

	nErr = IMODEL_QueryInterface(piModel, AEEIID_HTMLVIEWMODEL, (void**)&me->piViewModel);
	if (SUCCESS != nErr) {
		IMODEL_Release(piModel);
		return FALSE;
	}
	
	// Add Listeners for HTML Widget
	LISTENER_Init(&(me->mlHtmlView), samplehtmlwidgetapp_htmlviewlistener, me);
	LISTENER_Init(&(me->mlHtmlDoc), samplehtmlwidgetapp_htmldoclistener, me);
	IDOCMODEL_AddListener(me->piDocModel, &(me->mlHtmlDoc));
	IHTMLVIEWMODEL_AddListener(me->piViewModel, &(me->mlHtmlView));

	dwpos.x = 0;
	dwpos.y = 0;
	dwpos.bVisible = TRUE;

	// insert widget into root container
	IROOTCONTAINER_Insert(me->picRoot, me->piwHtml, WIDGET_ZNORMAL,&dwpos); 

	// get handle to file manager
	nErr = ISHELL_CreateInstance(me->pIShell, AEECLSID_FILEMGR, (void**)&(me->piFileM));
	if (SUCCESS != nErr) {
		return FALSE;
	}

	// get handle to source util
	nErr = ISHELL_CreateInstance(me->pIShell, AEECLSID_SOURCEUTIL, (void **)&(me->piSourceUtil));
	if (SUCCESS != nErr) {
		return FALSE;
	}

	samplehtmlwidgetapp_loadhtmlfile(me, HOME_PAGE);

	// release model
	RELEASEIF(piModel);

	// release bitmap
	RELEASEIF(pib);

    // if there have been no failures up to this point then return success
    return TRUE;
}

static int samplehtmlwidgetapp_Construct(samplehtmlwidgetapp *me, IModule * piModule, IShell * piShell)
{
   me->ia.pvt           = &me->iavt;
   me->iavt.AddRef      = samplehtmlwidgetapp_AddRef;
   me->iavt.Release     = samplehtmlwidgetapp_Release;
   me->iavt.HandleEvent = samplehtmlwidgetapp_HandleEvent;

   me->nRefs = 1;

   me->pIShell = piShell;
   ISHELL_AddRef(piShell);

   me->pIModule = piModule;
   IMODULE_AddRef(piModule);
   
   samplehtmlwidgetapp_CreateHtmlWidget(me);

   return 0;
}

// load html file
void samplehtmlwidgetapp_loadhtmlfile(samplehtmlwidgetapp* me, const char * pszFilename)
{
	int nErr = 0;

	ISource *piSource = NULL;
	IFile * piFile = NULL;
        
        (void)nErr;

	// log history
	if (GO_BACK == pszFilename) {
		if (me->ndxHist < 2) {
			return;
		}
		--me->ndxHist;
		pszFilename = me->pszHist[me->ndxHist-1];
	} else {
		do { 
			if (me->pszHist[me->ndxHist]) {
				FREE(me->pszHist[me->ndxHist]); 
			}
			me->pszHist[me->ndxHist] = pszFilename ? STRDUP(pszFilename) : 0; 
		} while (0);

		++me->ndxHist;
	}

	// open file from directory : simulating a web connection here
	piFile = IFILEMGR_OpenFile(me->piFileM, pszFilename,_OFM_READ);
	nErr = IFILEMGR_GetLastError(me->piFileM);
	nErr = ISOURCEUTIL_SourceFromAStream(me->piSourceUtil, (IAStream*)piFile, &piSource);

	if (NULL != piFile)	{
		nErr = IDOCMODEL_LoadSource(me->piDocModel, piSource);
	}

	// release handles
	RELEASEIF(piFile);
	RELEASEIF(piSource);
}
// VIEW MODEL LISTENERS
// Add application specific custom code for each event
void samplehtmlwidgetapp_htmlviewlistener(samplehtmlwidgetapp* me, ModelEvent *pEv)
{
	
	// base document (objects/images pending)
	if (pEv->evCode == EVT_HVM_DONE) {

	}
	// first visible page (including objects/images) - this page can be a part of a big html page
	if (pEv->evCode == EVT_HVM_PAGEDONE) {

	}
	// document load complete (including objects/images)
	if (pEv->evCode == EVT_HVM_CONTENTDONE) {

	}
	// user selected jump
	if (pEv->evCode == EVT_HVM_JUMP) {
		samplehtmlwidgetapp_loadhtmlfile(me, ((HtmlViewEvent *)pEv)->u.jump.pszURL);
	}
	// user submitted form
	if (pEv->evCode == EVT_HVM_SUBMIT) {

	}
	// focus has changed
	if (pEv->evCode == EVT_HVM_FOCUS) {

	}
	return;
}
// DOCUMENT MODEL LISTENERS
// Add application specific custom code for each event
void samplehtmlwidgetapp_htmldoclistener(samplehtmlwidgetapp* me, ModelEvent *pEv)
{
	// document has been reset
	if (pEv->evCode == EVT_HDM_RESET) {

	}
	// document load has been stopped
	if (pEv->evCode == EVT_HDM_STOP) {

	}
	return;
}

// Destroy samplehtmlwidgetapp object
static void samplehtmlwidgetapp_Dtor(samplehtmlwidgetapp *me)
{
	int i;
	for (i=0; i < ARRAY_SIZE(me->pszHist); ++i) {
		FREEIF(me->pszHist[i]);
	}

   FREEIF(me->pcBufstate);

	// cancel listeners
	LISTENER_Cancel(&me->mlHtmlView);
	LISTENER_Cancel(&me->mlHtmlDoc);


	RELEASEIF(me->pIDisplay);
	RELEASEIF(me->piSourceUtil);
	RELEASEIF(me->piFileM);
	RELEASEIF(me->piDocModel);
	RELEASEIF(me->piViewModel);
	RELEASEIF(me->piWeb);
	RELEASEIF(me->piwHtml);
	RELEASEIF(me->piCanvas);
	RELEASEIF(me->piwRoot);
	RELEASEIF(me->picRoot);
	RELEASEIF(me->pIModule);
	RELEASEIF(me->pIShell);
}


static uint32 samplehtmlwidgetapp_AddRef(IApplet *po)
{
   samplehtmlwidgetapp *me = (samplehtmlwidgetapp *)po;
   return ++me->nRefs;
}


static uint32 samplehtmlwidgetapp_Release(IApplet *po)
{
   samplehtmlwidgetapp *me = (samplehtmlwidgetapp *)po;
   uint32 nRefs = --me->nRefs;
   if (nRefs == 0) {
      samplehtmlwidgetapp_Dtor(me);
      FREE(me);
   }
   return nRefs;
}


#include "xmod.h"

int xModule_CreateInstance(IModule *module, IShell *shell, AEECLSID idClass, void **pp)
{
   if (idClass == AEECLSID_SAMPLEHTMLWIDGETAPP) {
      samplehtmlwidgetapp *me = MALLOCREC(samplehtmlwidgetapp);

      if (me == 0) {
         return ENOMEMORY;
      } else {
         *pp = &me->ia;
         return samplehtmlwidgetapp_Construct(me, module, shell);
      }
   }

   *pp = 0;
   return ECLASSNOTSUPPORT;
}

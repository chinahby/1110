/*
  ========================================================================

  FILE:  samplemediaapp.c
  
  SERVICES:  Sample media app

  GENERAL DESCRIPTION: 
  

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#include "AEE.h"              // Standard AEE Declarations
#include "AEEShell.h"         // AEE Shell Services
#include "AEEFont.h"          // AEE Font Services
#include "AEEDisp.h"          // AEE Display Services
#include "AEEStdLib.h"        // AEE StdLib Services
#include "AEEWidget.h"
#include "AEERootContainer.h"
#include "AEEDisplayCanvas.h"
#include "AEEMediaUtil.h"
#include "AEEFrameWidget.h"
#include "AEEListWidget.h"
#include "AEEMediaFrameModel.h"
#include "AEEImageWidget.h"
#include "AEEVectorModel.h"
#include "AEEStaticWidget.h"
#include "AEEFile.h"
#include "../inc/samplemediaapp.bid"
#include "../inc/samplemediaapp_res.h"

//------------------------------------------------------------------------
//    Types and Macros
//------------------------------------------------------------------------
#define RESFILE            "samplemediaapp.bar"
#define MEDIA_DIR          "media"
#define BUTTON_WIDTH       16
#define SEEK_TIME          5000

#define IDX_FOLDER         0
#define IDX_PLAY           1
#define IDX_STOP           2
#define IDX_REWIND         3
#define IDX_PAUSE          4
#define IDX_FF             5
#define NUM_BUTTONS        6

#ifndef ARRAYSIZE
#define ARRAYSIZE(a)       (sizeof(a)/sizeof(a[0]))
#endif

#ifndef RELEASEIF
#define RELEASEIF(p)       ReleaseIf((IBase**)&p)
static void ReleaseIf(IBase **ppif) {
   if (*ppif) {
      IBASE_Release(*ppif); 
      *ppif = 0;
   }
}
#endif

#ifndef ADDREFIF
#define ADDREFIF(p)        do { if (p) IBASE_AddRef((IBase*) (void *) p); } while(0)
#endif

#define IWIDGET_SetExtentEx(p,w,h)  \
   do { WExtent we; we.width = (w); we.height=(h); IWIDGET_SetExtent((p),&we); } while(0)


static __inline int IROOTCONTAINER_InsertEx(IRootContainer *me, IWidget *piwFrame, int x, int y) {
   WidgetPos wp;
   wp.x = x;
   wp.y = y;
   wp.bVisible = 1;
   return IROOTCONTAINER_Insert(me, piwFrame, WIDGET_ZNORMAL, &wp);
}


typedef struct MediaApp MediaApp;

static uint32  MediaApp_AddRef   (IApplet *po);
static uint32  MediaApp_Release  (IApplet *po);
static int     MediaApp_Start    (MediaApp *me);
static boolean MediaApp_IsPause  (MediaApp *me);


//------------------------------------------------------------------------
//    MediaApp struct
//------------------------------------------------------------------------

struct MediaApp {
   
   IApplet              ia;
   IAppletVtbl          iavt;  

   int                  nRefs;
   IShell *             piShell;
   IModule *            piModule;

   AEERect              rcContainer;
   IRootContainer *     pwcRoot;
   IWidget *            piwRoot;
   IWidget *            piwFrame;
   IWidget *            piwList;
   IWidget *            piwButtons[NUM_BUTTONS];

   IMediaFrameModel *   piModel;
   IMediaUtil *         piMediaUtil;
   IMedia *             piMedia;

   int                  nActiveButton;
   boolean              bListVisible;
};


//--------------------------------------------------------------------
//   MediaApp Event Handler
//--------------------------------------------------------------------

static boolean MediaApp_HandleEvent(IApplet * pi, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{  
   MediaApp *me = (MediaApp *)pi;

   switch (eCode) {
      
      case EVT_APP_START:
         return (SUCCESS == MediaApp_Start(me));

      case EVT_APP_STOP:
         return TRUE;   

      case EVT_APP_SUSPEND:
         return TRUE;

      case EVT_APP_RESUME:
         return TRUE;

      case EVT_KEY:
         return TRUE;

      case EVT_KEY_PRESS:

         if (me->bListVisible) {

            if (AVK_CLR == wParam) {

               me->bListVisible = FALSE;
               IROOTCONTAINER_Remove(me->pwcRoot, me->piwList);

            } else if (AVK_SELECT == wParam) {
   
               AEEMediaData md;
               IVectorModel *pivm = 0;
               int nIndex; 
               AECHAR *wstr = 0;
               char *str = 0;
               int nSize;

               if (SUCCESS == IWIDGET_GetFocusIndex(me->piwList, &nIndex) && 
                   SUCCESS == IWIDGET_GetModel(me->piwList, AEEIID_LISTMODEL, (IModel**)&pivm) && 
                   SUCCESS == IVECTORMODEL_GetAt(pivm, nIndex, (void**)&wstr)) {

                  nSize = (WSTRLEN(wstr)+1)*sizeof(char);
                  str = (char*)MALLOC(nSize);
                  WSTRTOSTR(wstr, str, nSize);

                  md.clsData = MMD_FILE_NAME;
                  md.pData   = str;
                  md.dwSize  = 0;

                  if (me->piMedia)
                     IMEDIA_Stop(me->piMedia);

                  IMEDIAFRAMEMODEL_SetIMedia(me->piModel, NULL);
                  RELEASEIF(me->piMedia);
                  IMEDIAUTIL_CreateMedia(me->piMediaUtil, &md, &me->piMedia);
                  IMEDIAFRAMEMODEL_SetIMedia(me->piModel, me->piMedia);

                  me->bListVisible = FALSE;
                  IROOTCONTAINER_Remove(me->pwcRoot, me->piwList);
               }

               FREEIF(str);
               RELEASEIF(pivm);

            } else {


               /// Default widget controllers handle EVT_KEY, not EVT_KEY_PRESS
               IWIDGET_HandleEvent(me->piwList, EVT_KEY, wParam, dwParam);
            }

         } else {

            if (AVK_RIGHT == wParam) {

               IWIDGET_SetBorderWidth(me->piwButtons[me->nActiveButton], 0);
               me->nActiveButton = MIN(me->nActiveButton+1, NUM_BUTTONS-1);
               IWIDGET_SetBorderWidth(me->piwButtons[me->nActiveButton], 1);

            } else if (AVK_LEFT == wParam) {

               IWIDGET_SetBorderWidth(me->piwButtons[me->nActiveButton], 0);
               me->nActiveButton = MAX(me->nActiveButton-1, 0);
               IWIDGET_SetBorderWidth(me->piwButtons[me->nActiveButton], 1);

            } else if (AVK_SELECT == wParam) {

               switch (me->nActiveButton) {

               case IDX_FOLDER:

                  if (me->piMedia)
                     IMEDIA_Pause(me->piMedia);

                  me->bListVisible = TRUE;
                  IROOTCONTAINER_InsertEx(me->pwcRoot, me->piwList, 0, 0);
                  break;

               case IDX_PLAY:

                  if (me->piMedia)
                     MediaApp_IsPause(me) ? IMEDIA_Resume(me->piMedia) : IMEDIA_Play(me->piMedia);
                  break;

               case IDX_PAUSE:

                  if (me->piMedia)
                     IMEDIA_Pause(me->piMedia);
                  break;

               case IDX_STOP:

                  if (me->piMedia)
                     IMEDIA_Stop(me->piMedia);
                  break;

               case IDX_REWIND:

                  if (me->piMedia)
                     IMEDIA_Rewind(me->piMedia, SEEK_TIME);
                  break;

               case IDX_FF:

                  if (me->piMedia)
                     IMEDIA_FastForward(me->piMedia, SEEK_TIME);
                  break;
               }
            }
         }

         return TRUE;

   }  // switch(eCode)

   return FALSE;
}


//--------------------------------------------------------------------
//   MediaApp Creation
//--------------------------------------------------------------------

static int MediaApp_CreateButton(MediaApp *me, int16 nButtonIdx, int16 nButtonBmp)
{
   int nErr = ISHELL_CreateInstance(me->piShell, AEECLSID_IMAGEWIDGET, (void**)&me->piwButtons[nButtonIdx]);

   // Load image
   if (!nErr) {
      IImage *piImage = ISHELL_LoadResImage(me->piShell, RESFILE, nButtonBmp);

      if (piImage) {
         nErr = IWIDGET_SetImage(me->piwButtons[nButtonIdx], piImage);
      } else {
         nErr = ERESOURCENOTFOUND;
      }

      RELEASEIF(piImage);
   }

   // Configure the button
   if (!nErr) {
      IWIDGET_SetExtentEx(me->piwButtons[nButtonIdx], BUTTON_WIDTH, BUTTON_WIDTH);
   }

   return nErr;
}

static int MediaApp_CreateWidgets(MediaApp *me)
{
   int nErr = 0;
   
   // Create frame widget to display the video
   nErr = ISHELL_CreateInstance(me->piShell, AEECLSID_FRAMEWIDGET, (void**)&me->piwFrame);

   // Configure the Frame widget
   if (!nErr) {
      IWIDGET_SetExtentEx(me->piwFrame, me->rcContainer.dx, me->rcContainer.dy - BUTTON_WIDTH);
      IWIDGET_SetBGColor(me->piwFrame, MAKE_RGB(0,0,0));
   }

   // Create IMedia object
   if (!nErr) {
      nErr = ISHELL_CreateInstance(me->piShell, AEECLSID_MEDIAUTIL, (void**)&me->piMediaUtil);
   }

   // Create media frame model instance to hold the IMedia object
   if (!nErr) {
      nErr = ISHELL_CreateInstance(me->piShell, AEECLSID_MEDIAFRAMEMODEL, (void**)&me->piModel);
   }

   // Set the model on the frame widget
   if (!nErr) {
      IWIDGET_SetModel(me->piwFrame, (IModel*)me->piModel);
   }

   // Create the List widget for media selection
   if (!nErr) {
      nErr = ISHELL_CreateInstance(me->piShell, AEECLSID_LISTWIDGET, (void**)&me->piwList);
   }

   // Configure the List Widget
   if (!nErr) {
      IWIDGET_SetExtentEx(me->piwList, me->rcContainer.dx, me->rcContainer.dy - BUTTON_WIDTH);
      IWIDGET_SetBorderWidth(me->piwList, 1);
      IWIDGET_SetItemHeight(me->piwList, 15);
   }

   // Use Static widget to draw list items
   if (!nErr) {

      IWidget *pisw = 0;

      nErr = ISHELL_CreateInstance(me->piShell, AEECLSID_STATICWIDGET, (void**)&pisw);

      if (!nErr) {

         IWIDGET_SetLeftPadding(pisw, 1);
         IWIDGET_SetRightPadding(pisw, 1);
         IWIDGET_SetBorderWidth(pisw, 1);
         IWIDGET_SetSelectedBorderColor(pisw, MAKE_RGBA(0,0,255,255));
         IDECORATOR_SetWidget((IDecorator *)me->piwList, pisw);
      }

      RELEASEIF(pisw);
   }

   // Add media files to list
   if (!nErr) {

      IFileMgr *pifm = 0;
      IVectorModel *pivm = 0;
      FileInfo fi;

      if (!nErr) {
         nErr = ISHELL_CreateInstance(me->piShell, AEECLSID_VECTORMODEL, (void**)&pivm);
      }

      if (!nErr) {
         nErr = ISHELL_CreateInstance(me->piShell, AEECLSID_FILEMGR, (void **)&pifm);
      }

      if (!nErr) {

         IWIDGET_SetModel(me->piwList, (IModel*)pivm);
         IVECTORMODEL_SetPfnFree(pivm, WidgetAEEFree);
         IFILEMGR_EnumInit(pifm, MEDIA_DIR, FALSE);

         while (IFILEMGR_EnumNext(pifm, &fi)) {
            int nSize = (STRLEN(fi.szName) + 1) * sizeof(AECHAR);
            AECHAR *wstr = (AECHAR*)MALLOC(nSize);
            IVECTORMODEL_Add(pivm, STRTOWSTR(fi.szName, wstr, nSize));
         } 
      }

      RELEASEIF(pifm);
      RELEASEIF(pivm);
   }

   // Create the Folder Button
   if (!nErr) {
      nErr = MediaApp_CreateButton(me, IDX_FOLDER, IDB_FOLDER);
   }

   // Create the Play button
   if (!nErr) {
      nErr = MediaApp_CreateButton(me, IDX_PLAY, IDB_PLAY);
   }

   // Create the Stop button
   if (!nErr) {
      nErr = MediaApp_CreateButton(me, IDX_STOP, IDB_STOP);
   }

   // Create the Rewind button
   if (!nErr) {
      nErr = MediaApp_CreateButton(me, IDX_REWIND, IDB_REWIND);
   }

   // Create the Pause button
   if (!nErr) {
      nErr = MediaApp_CreateButton(me, IDX_PAUSE, IDB_PAUSE);
   }

   // Create the FF button
   if (!nErr) {
      nErr = MediaApp_CreateButton(me, IDX_FF, IDB_FF);
   }

   return nErr;
}


static int MediaApp_Start(MediaApp *me)
{
   int i;
   int nErr = MediaApp_CreateWidgets(me);

   if (!nErr) {

      // Insert Frame widget
      IROOTCONTAINER_InsertEx(me->pwcRoot, me->piwFrame, 0, 0);

      // Insert Button widgets
      for (i=0; i<NUM_BUTTONS; i++) {
         IROOTCONTAINER_InsertEx(me->pwcRoot, me->piwButtons[i], i*(BUTTON_WIDTH+1), me->rcContainer.dy - BUTTON_WIDTH);
      }
   }

   // Set active button to Folder
   if (!nErr) {
      me->nActiveButton = IDX_FOLDER;
      IWIDGET_SetBorderWidth(me->piwButtons[me->nActiveButton], 1);
   }

   return nErr;
}


static int MediaApp_Construct(MediaApp *me, IModule * piModule, IShell * piShell)
{
   IBitmap *pb = 0;
   int nErr = 0;
   IDisplay *piDisplay = 0;
   ICanvas *piCanvas = 0;

   me->ia.pvt           = &me->iavt;
   me->iavt.AddRef      = MediaApp_AddRef;
   me->iavt.Release     = MediaApp_Release;
   me->iavt.HandleEvent = MediaApp_HandleEvent;

   me->nRefs = 1;
   me->bListVisible = FALSE;

   me->piShell = piShell;
   ISHELL_AddRef(piShell);

   me->piModule = piModule;
   IMODULE_AddRef(piModule);

   if (!nErr) {
      nErr = ISHELL_CreateInstance(piShell, AEECLSID_DISPLAY, (void **)&piDisplay);
   }

   if (!nErr) {
      nErr = IDISPLAY_GetDeviceBitmap(piDisplay, &pb);
   }

   if (!nErr) {
      AEEBitmapInfo  biDevice;
      IBITMAP_GetInfo(pb, &biDevice, sizeof(biDevice));
      IBITMAP_Release(pb);
      SETAEERECT(&me->rcContainer, 0, 0, (short) biDevice.cx, (short) biDevice.cy);
      nErr = ISHELL_CreateInstance(piShell, AEECLSID_ROOTCONTAINER, (void**)&me->pwcRoot);
   }

   if (!nErr) {
      nErr = ISHELL_CreateInstance(piShell, AEECLSID_DISPLAYCANVAS, (void**)&piCanvas);
   }

   if (!nErr) {
      nErr = IDISPLAYCANVAS_SetDisplay((IDisplayCanvas*)piCanvas, piDisplay);
   }

   if (!nErr) {
      IROOTCONTAINER_SetCanvas(me->pwcRoot, piCanvas, &me->rcContainer);
      // Get IWidget so we can call HandleEvent()
      nErr = IROOTCONTAINER_QueryInterface(me->pwcRoot, AEEIID_WIDGET, (void**)&me->piwRoot);
   }

   RELEASEIF(piDisplay);
   RELEASEIF(piCanvas);

   return nErr;
}


static boolean MediaApp_IsPause(MediaApp *me)
{
   boolean bChg;
   int nState;

   if (!me->piMedia)
      return FALSE;

   nState = IMEDIA_GetState(me->piMedia, &bChg);

   if (bChg)
      return FALSE;

   return (nState == MM_STATE_PLAY_PAUSE || nState == MM_STATE_RECORD_PAUSE);
}


// Destroy MediaApp object
//
static void MediaApp_Dtor(MediaApp *me)
{
   int i;

   if (me->piMedia) {
      IMEDIA_Stop(me->piMedia);
   }

   if (me->piModel) {
      IMEDIAFRAMEMODEL_SetIMedia(me->piModel, NULL);
   }

   RELEASEIF(me->pwcRoot);
   RELEASEIF(me->piShell);
   RELEASEIF(me->piModule);
   RELEASEIF(me->piMedia);
   RELEASEIF(me->piModel);
   RELEASEIF(me->piMediaUtil);
   RELEASEIF(me->piwRoot);
   RELEASEIF(me->piwFrame);
   RELEASEIF(me->piwList);
  
   for (i=0; i<NUM_BUTTONS; i++) {
      RELEASEIF(me->piwButtons[i]);
   }
}


static uint32 MediaApp_AddRef(IApplet *po)
{
   MediaApp *me = (MediaApp *)po;
   return ++me->nRefs;
}


static uint32 MediaApp_Release(IApplet *po)
{
   MediaApp *me = (MediaApp *)po;
   uint32 nRefs = --me->nRefs;
   if (nRefs == 0) {
      MediaApp_Dtor(me);
      FREE(me);
   }
   return nRefs;
}


#include "xmod.h"

int xModule_CreateInstance(IModule *module, IShell *shell, AEECLSID idClass, void **pp)
{
   if (idClass == AEECLSID_SAMPLEMEDIAAPP) {
      MediaApp *me = MALLOCREC(MediaApp);

      if (me == 0) {
         return ENOMEMORY;
      } else {
         *pp = &me->ia;
         return MediaApp_Construct(me, module, shell);
      }
   }

   *pp = 0;
   return ECLASSNOTSUPPORT;
}


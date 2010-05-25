/*
  ========================================================================

  FILE:  samplecameraapp.c
  
  SERVICES:  Sample Camera App

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
#include "AEEFrameWidget.h"
#include "AEECameraFrameModel.h"
#include "AEEImageWidget.h"
#include "AEEBitmapWidget.h"
#include "AEEImageWidget.h"
#include "AEEListWidget.h"
#include "AEEStaticWidget.h"
#include "AEEMimeTypes.h"
#include "AEEVectorModel.h"
#include "AEEFile.h"
#include "../inc/samplecameraapp.bid"
#include "../inc/samplecameraapp_res.h"

//------------------------------------------------------------------------
//    Types and Macros
//------------------------------------------------------------------------
#define RESFILE            "samplecameraapp.bar"
#define FRAMEWIDGET_DIR    "framewidget"
#define BUTTON_WIDTH       16

#define IDX_FOLDER         0
#define IDX_PLAY           1
#define IDX_CAPTURE        2
#define NUM_BUTTONS        3

#define MODE_PREVIEWING    0
#define MODE_SELECTING     1
#define MODE_VIEWING       2
#define MODE_CAPTURING     3

#define CAMERA_TIMER       2000 // Time between Camera Stop and Capture

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


typedef struct CameraApp CameraApp;

static uint32  CameraApp_AddRef   (IApplet *po);
static uint32  CameraApp_Release  (IApplet *po);
static int     CameraApp_Start    (CameraApp *me);
static void    CameraApp_Notify   (void * pUser, AEECameraNotify * pNotify);
static void    CameraApp_Snapshot (void * pUser);


//------------------------------------------------------------------------
//    CameraApp struct
//------------------------------------------------------------------------

struct CameraApp {
   
   IApplet              ia;
   IAppletVtbl          iavt;  

   
   IShell *             piShell;
   IModule *            piModule;
   IFileMgr *           piFileMgr;
   IVectorModel *       piListModel;
   ICamera *            piCamera;

   AEERect              rcContainer;
   IRootContainer *     pwcRoot;
   IWidget *            piwRoot;
   IWidget *            piwFrame;
   IWidget *            piwList;
   IWidget *            piwImage;
   IWidget *            piwButtons[NUM_BUTTONS];
   
   int                  nRefs;
   int                  nActiveButton;
   int                  nMode;
   char                 szImage[50];
};


//--------------------------------------------------------------------
//   CameraApp Event Handler
//--------------------------------------------------------------------

static boolean CameraApp_HandleEvent(IApplet * pi, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{  
   CameraApp *me = (CameraApp *)pi;

   switch (eCode) {
      
      case EVT_APP_START:
         CameraApp_Start(me);
         return TRUE;

      case EVT_APP_STOP:
         return TRUE;   

      case EVT_APP_SUSPEND:
         return TRUE;

      case EVT_APP_RESUME:
         return TRUE;

      case EVT_KEY:

         // Press CLEAR key to go back to Preview
         if (AVK_CLR == wParam) {

            // Show frame
            me->nMode = MODE_PREVIEWING;
            IROOTCONTAINER_Remove(me->pwcRoot, me->piwList);
            IROOTCONTAINER_Remove(me->pwcRoot, me->piwImage);

            // Put camera back in preview mode
            ICAMERA_Preview(me->piCamera);
            return TRUE;
         }

         if (me->nMode == MODE_SELECTING) {
            if (AVK_SELECT == wParam) {
               IImage *piImage = 0;

               int nIndex; 
               AECHAR *wstr = 0;
               int nSize;

               if (SUCCESS == IWIDGET_GetFocusIndex(me->piwList, &nIndex) &&
                   SUCCESS == IVECTORMODEL_GetAt(me->piListModel, nIndex, (void**)&wstr)) {

                  nSize = (WSTRLEN(wstr)+1)*sizeof(char);
                  WSTRTOSTR(wstr, me->szImage, nSize);

                  piImage = ISHELL_LoadImage(me->piShell, me->szImage);

                  if (piImage) {

                     // Set widget image
                     (void)IWIDGET_SetImage(me->piwImage, piImage);

                     // Show image
                     me->nMode = MODE_VIEWING;
                     IROOTCONTAINER_Remove(me->pwcRoot, me->piwList);
                     IROOTCONTAINER_InsertEx(me->pwcRoot, me->piwImage, 0, 0);
                  }
               }

               RELEASEIF(piImage);

            } else {

               IWIDGET_HandleEvent(me->piwList, eCode, wParam, dwParam);
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

               IROOTCONTAINER_Remove(me->pwcRoot, me->piwImage);

               switch (me->nActiveButton) {

                  case IDX_FOLDER:

                     // Show list
                     me->nMode = MODE_SELECTING;
                     IROOTCONTAINER_InsertEx(me->pwcRoot, me->piwList, 0, 0);
                     break;

                  case IDX_PLAY:

                     me->nMode = MODE_PREVIEWING;
                     (void)ICAMERA_Preview(me->piCamera);
                     break;

                  case IDX_CAPTURE:
                     {
                        int i = 1;
                        AEEMediaData md;

                        // Find suitable file name
                        do {
#ifdef AEE_SIMULATOR
                           SPRINTF(me->szImage, "%s/image%03i.bmp", FRAMEWIDGET_DIR, i++);
#else
                           SPRINTF(me->szImage, "%s/image%03i.png", FRAMEWIDGET_DIR, i++);
#endif
                        } while (IFILEMGR_Test(me->piFileMgr, me->szImage) == SUCCESS);

                        md.clsData = MMD_FILE_NAME;
                        md.pData = (void *)me->szImage;
                        md.dwSize = 0;

                        // Camera must be in ready state
                        (void)ICAMERA_Stop(me->piCamera);

#ifdef AEE_SIMULATOR
                        (void)ICAMERA_SetParm(me->piCamera, CAM_PARM_VIDEO_ENCODE, CAM_ENCODE_RAW, 0);
                        ICAMERA_SetMediaData(me->piCamera, &md, MT_BMP);
                        (void)ICAMERA_DeferEncode(me->piCamera, FALSE);
#else
#ifdef CAM_PARM_PREVIEW_TYPE
#ifdef CAM_PREVIEW_SNAPSHOT
                        (void)ICAMERA_SetParm(me->piCamera, CAM_PARM_PREVIEW_TYPE, CAM_PREVIEW_SNAPSHOT, 0);
#endif // CAM_PREVIEW_SNAPSHOT
#endif // CAM_PARM_PREVIEW_TYPE
                        (void)ICAMERA_SetParm(me->piCamera, CAM_PARM_VIDEO_ENCODE, AEECLSID_PNG, 0);
                        (void)ICAMERA_SetMediaData(me->piCamera, &md, MT_PNG);
                        (void)ICAMERA_DeferEncode(me->piCamera, FALSE);
#endif
                        // Set timer so camera has a chance to get to ready state
                        ISHELL_SetTimer(me->piShell, CAMERA_TIMER, CameraApp_Snapshot, (void*)me);

                     }
                     break;
                  }
            }
         }

         return TRUE;

   }  // switch(eCode)

   return FALSE;
}

static void CameraApp_Snapshot(void * pUser)
{
   CameraApp *me = (CameraApp*)pUser;
   int16 nMode;
   boolean bPaused;

   // Check for ready state
   if ((SUCCESS == ICAMERA_GetMode(me->piCamera, &nMode, &bPaused)) && nMode == CAM_MODE_READY) {
      // Take snapshot
      me->nMode = MODE_CAPTURING;
      (void)ICAMERA_RecordSnapshot(me->piCamera);
   }
}

static void CameraApp_Notify(void * pUser, AEECameraNotify * pNotify)
{
   CameraApp *me = (CameraApp*)pUser;

   if (pNotify->nCmd == CAM_CMD_ENCODESNAPSHOT) {

      if (pNotify->nStatus == CAM_STATUS_DONE) {

         AECHAR *wstr = 0;
         int nSize;

 /*
#ifdef AEE_SIMULATOR

         IBitmap *piBitmap = ISHELL_LoadBitmap(me->piShell, me->szImage);

         if (piBitmap) {
            IWIDGET_SetBitmap(me->piwImage, piBitmap);
            me->nMode = MODE_VIEWING;
            IROOTCONTAINER_InsertEx(me->pwcRoot, me->piwImage, 0, 0);
            RELEASEIF(piBitmap);
         }

#else

         IImage *piImage = ISHELL_LoadImage(me->piShell, me->szImage);

         if (piImage) {
            IWIDGET_SetImage(me->piwImage, piImage);
            me->nMode = MODE_VIEWING;
            IROOTCONTAINER_InsertEx(me->pwcRoot, me->piwImage, 0, 0);
            RELEASEIF(piImage);
         }

#endif

*/
         // Add to stored list of images
         nSize = (STRLEN(me->szImage) + 1) * sizeof(AECHAR);
         wstr = (AECHAR*)MALLOC(nSize);
         IVECTORMODEL_Add(me->piListModel, STRTOWSTR(me->szImage, wstr, nSize));  

      }else { // Failure
         me->nMode = MODE_PREVIEWING;
         (void)ICAMERA_Preview(me->piCamera);
      }
   }
}


//--------------------------------------------------------------------
//   CameraApp Creation
//--------------------------------------------------------------------

static int CameraApp_CreateButton(CameraApp *me, int16 nButtonIdx, int16 nButtonBmp)
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


static int CameraApp_CreateWidgets(CameraApp *me)
{
   int nErr = 0;
   ICameraFrameModel *piModel = 0;
   
   // Create frame widget to display the video
   nErr = ISHELL_CreateInstance(me->piShell, AEECLSID_FRAMEWIDGET, (void**)&me->piwFrame);

   // Configure the frame widget
   if (!nErr) {
      IWIDGET_SetExtentEx(me->piwFrame, me->rcContainer.dx, me->rcContainer.dy - BUTTON_WIDTH);
      IWIDGET_SetBGColor(me->piwFrame, MAKE_RGB(0,0,0));
   }

   // Create ICamera object
   if (!nErr) {
      nErr = ISHELL_CreateInstance(me->piShell, AEECLSID_CAMERA, (void**)&me->piCamera);
   }

   // Configure ICamera
   if (!nErr) {
      AEESize size;
      size.cx = me->rcContainer.dx;
      size.cy = me->rcContainer.dy - BUTTON_WIDTH;      
      ICAMERA_SetSize(me->piCamera, &size);
      ICAMERA_SetDisplaySize(me->piCamera, &size);
   }

   // Create camera frame model instance to hold the ICamera object
   if (!nErr) {
      nErr = ISHELL_CreateInstance(me->piShell, AEECLSID_CAMERAFRAMEMODEL, (void**)&piModel);
   }

   // Configure the camera frame model
   if (!nErr) {
      ICAMERAFRAMEMODEL_RegisterNotify(piModel, CameraApp_Notify, me);
   }

   // Set the ICamera into the ICameraFrameModel
   if (!nErr) {
      nErr = ICAMERAFRAMEMODEL_SetICamera(piModel, me->piCamera);
   }

   // Set the model on the frame widget
   if (!nErr) {
      IWIDGET_SetModel(me->piwFrame, (IModel*)piModel);
   }

   // Create the List widget for image selection
   if (!nErr) {
      nErr = ISHELL_CreateInstance(me->piShell, AEECLSID_LISTWIDGET, (void**)&me->piwList);
   }

   // Configure the List widget
   if (!nErr) {
      IWIDGET_SetExtentEx(me->piwList, me->rcContainer.dx, me->rcContainer.dy - BUTTON_WIDTH);
      IWIDGET_SetBorderWidth(me->piwList, 1);
      IWIDGET_SetItemHeight(me->piwList, 15);
      IWIDGET_SetModel(me->piwList, (IModel*)me->piListModel);
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

   // Create the Image widget
   if (!nErr) {
      //nErr = ISHELL_CreateInstance(me->piShell, AEECLSID_BITMAPWIDGET, (void**)&me->piwImage);
      nErr = ISHELL_CreateInstance(me->piShell, AEECLSID_IMAGEWIDGET, (void**)&me->piwImage);
   }

   // Configure the Image widget
   if (!nErr) {
      IWIDGET_SetExtentEx(me->piwImage, me->rcContainer.dx, me->rcContainer.dy - BUTTON_WIDTH);
      IWIDGET_SetBorderWidth(me->piwImage, 1);
   }

   // Create the Folder Button
   if (!nErr) {
      nErr = CameraApp_CreateButton(me, IDX_FOLDER, IDB_FOLDER);
   }

   // Create the Play Button
   if (!nErr) {
      nErr = CameraApp_CreateButton(me, IDX_PLAY, IDB_PLAY);
   }

   // Create the Capture button
   if (!nErr) {
      nErr = CameraApp_CreateButton(me, IDX_CAPTURE, IDB_CAPTURE);
   }

   RELEASEIF(piModel);

   return nErr;
}


static int CameraApp_Start(CameraApp *me)
{
   int i;
   int nErr = CameraApp_CreateWidgets(me);

   if (!nErr) {

      // Insert Frame widget
      IROOTCONTAINER_InsertEx(me->pwcRoot, me->piwFrame, 0, 0);

      // Insert Button widgets
      for (i=0; i<NUM_BUTTONS; i++) {
         IROOTCONTAINER_InsertEx(me->pwcRoot, me->piwButtons[i], i*(BUTTON_WIDTH+1), me->rcContainer.dy - BUTTON_WIDTH);
      }
   }

   // Set active button to Folder
   me->nActiveButton = IDX_FOLDER;
   IWIDGET_SetBorderWidth(me->piwButtons[me->nActiveButton], 1);

   // Start camera in preview mode
   nErr = ICAMERA_Preview(me->piCamera);

   return nErr;
}


static int CameraApp_Construct(CameraApp *me, IModule * piModule, IShell * piShell)
{
   IBitmap *pb = 0;
   int nErr = 0;
   IDisplay *piDisplay = 0;
   ICanvas *piCanvas = 0;

   me->ia.pvt           = &me->iavt;
   me->iavt.AddRef      = CameraApp_AddRef;
   me->iavt.Release     = CameraApp_Release;
   me->iavt.HandleEvent = CameraApp_HandleEvent;

   me->nRefs = 1;
   me->nMode = MODE_PREVIEWING;

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
      nErr = IROOTCONTAINER_QueryInterface(me->pwcRoot, AEEIID_WIDGET, (void**)&me->piwRoot);
   }

   if (!nErr) {
      nErr = ISHELL_CreateInstance(me->piShell, AEECLSID_FILEMGR, (void **)&me->piFileMgr);
   }

   if (!nErr) {
      nErr = ISHELL_CreateInstance(me->piShell, AEECLSID_VECTORMODEL, (void**)&me->piListModel);
   }

   if (!nErr) {
      IVECTORMODEL_SetPfnFree(me->piListModel, WidgetAEEFree);
   }

   // Add captured images to list model
   if (!nErr) {

      FileInfo fi;

      if (EFAILED == IFILEMGR_Test(me->piFileMgr, FRAMEWIDGET_DIR)) {
         IFILEMGR_MkDir(me->piFileMgr, FRAMEWIDGET_DIR);
      }

      IFILEMGR_EnumInit(me->piFileMgr, FRAMEWIDGET_DIR, FALSE);

      while (IFILEMGR_EnumNext(me->piFileMgr, &fi)) {
         int nSize = (STRLEN(fi.szName) + 1) * sizeof(AECHAR);
         AECHAR *wstr = (AECHAR*)MALLOC(nSize);
         IVECTORMODEL_Add(me->piListModel, STRTOWSTR(fi.szName, wstr, nSize));
      } 
   }

   RELEASEIF(piDisplay);
   RELEASEIF(piCanvas);

   return nErr;
}


// Destroy CameraApp object
//
static void CameraApp_Dtor(CameraApp *me)
{
   int i;

   RELEASEIF(me->pwcRoot);
   RELEASEIF(me->piShell);
   RELEASEIF(me->piModule);
   RELEASEIF(me->piCamera);
   RELEASEIF(me->piFileMgr);
   RELEASEIF(me->piListModel);
   RELEASEIF(me->piwRoot);
   RELEASEIF(me->piwFrame);
   RELEASEIF(me->piwList);
   RELEASEIF(me->piwImage);
  
   for (i=0; i<NUM_BUTTONS; i++) {
      RELEASEIF(me->piwButtons[i]);
   }
}


static uint32 CameraApp_AddRef(IApplet *po)
{
   CameraApp *me = (CameraApp *)po;
   return ++me->nRefs;
}


static uint32 CameraApp_Release(IApplet *po)
{
   CameraApp *me = (CameraApp *)po;
   uint32 nRefs = --me->nRefs;
   if (nRefs == 0) {
      CameraApp_Dtor(me);
      FREE(me);
   }
   return nRefs;
}


#include "xmod.h"

int xModule_CreateInstance(IModule *module, IShell *shell, AEECLSID idClass, void **pp)
{
   if (idClass == AEECLSID_SAMPLECAMERAAPP) {
      CameraApp *me = MALLOCREC(CameraApp);

      if (me == 0) {
         return ENOMEMORY;
      } else {
         *pp = &me->ia;
         return CameraApp_Construct(me, module, shell);
      }
   }

   *pp = 0;
   return ECLASSNOTSUPPORT;
}


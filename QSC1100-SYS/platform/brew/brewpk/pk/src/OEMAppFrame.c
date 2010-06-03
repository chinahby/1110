/*======================================================
FILE:  OEMAppFrame.c

SERVICES:  OEM functions for implementing the AppFrame interface

GENERAL DESCRIPTION:
   This provides a reference implementation for the AppFrame
   interface, which is used by BREW to dynamically resize BREW's
   virtual screen.

        Copyright 2003-2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#include "AEE_OEM.h"
#include "OEMAppFrame.h"
#include "OEMDisp.h"
#include "AEEStdLib.h"
#include "OEMBitmapFuncs_priv.h"
#include "AEEIAnnunciatorControl.h"
#include "AEEAnnunciatorControl.bid"
#if defined(FEATURE_BREW_DISPLAYROTATION)
#include "AEEBitmapRot.h"
#endif

#if defined(AEE_SIMULATOR)
#include "EmuDisp.h"
#endif

#define RELEASEIF(p) do { if (p) { IQI_Release((IQueryInterface*)(p)); p = 0; } } while (0)

static uint32 OEMAppFrame_AddRef(IAppFrame *po);
static uint32 OEMAppFrame_Release(IAppFrame *po);
static int OEMAppFrame_QueryInterface(IAppFrame *po, AEECLSID clsid, void **ppNew);
static int OEMAppFrame_Init(IAppFrame *po, IBitmap *pbmDev, IBitmap *pbmApp, const char *pchDisp, int cDisp);
static int OEMAppFrame_Redraw(IAppFrame *po);
static int OEMAppFrame_Update(IAppFrame *po);
static int OEMAppFrame_Disable(IAppFrame *po);

static const VTBL(IAppFrame) gOEMAppFrameFuncs = {
   OEMAppFrame_AddRef,
   OEMAppFrame_Release,
   OEMAppFrame_QueryInterface,
   OEMAppFrame_Init,
   OEMAppFrame_Redraw,
   OEMAppFrame_Update,
   OEMAppFrame_Disable
};

typedef struct OEMAppFrame OEMAppFrame;

struct OEMAppFrame {
   const AEEVTBL(IAppFrame) *pvt;
   uint32 uRefs;
   AEERect rcAppRegion;
   AEEBitmapInfo bi;
   IBitmap *pbmDev;
   IBitmap *pbmApp;
#if defined(AEE_SIMULATOR)
   AEECLSID clsEmuDisp;
#endif
   RGBVAL rgbBackground;  // color to clear the screen
   int      nAnnun;
   AEERect  rcAppFrame;
   AEERect  rcAnnunBar;
   IAnnunciatorControl *pAnnunCtl;
};


#define ELEMENT_DONE          0
#define ELEMENT_ERROR         1
#define ELEMENT_OK            2

#define ELEMENT_VAL_STAR -1
#define ELEMENT_VAL_DEF  -2
#define ELEMENT_VAL_PRES -3

struct Params
{
   int nWidth;
   int nHeight;
#if defined(FEATURE_BREW_DISPLAYROTATION)
   int nRotation;
#endif
   int nAnnunciators;
   int nColorscheme;
};

static int GetElement(const char **pchIn, int *pcIn, const char **pchKey, int *pcKey, const char **pchVal, int *pcVal);
static int GetElementInt(const char *pchVal, int cVal);
static int DecodeElement(int nVal, int nOrigVal, int nDefaultVal, int nMaxVal);


int OEMAppFrame_New(IShell * piShell, AEECLSID cls, void **ppif)
{
   OEMAppFrame *pMe = NULL;
   int nResult = SUCCESS;
   AEECLSID clsAnnunCtl;

   pMe = (OEMAppFrame*)MALLOC(sizeof(OEMAppFrame));
   if (pMe == NULL) {
      nResult = ENOMEMORY;
      goto Error;
   }

   if(AEE_GetAppContextCls(AEE_GetAppContext()) == AEECLSID_SHELL) {
      pMe->nAnnun = 0;
   } else {
      pMe->nAnnun = 1;
   }

   pMe->pvt = &gOEMAppFrameFuncs;
   pMe->uRefs = 1;

#if defined(AEE_SIMULATOR)
   switch (cls) {
   case AEECLSID_APPFRAME2:
      pMe->clsEmuDisp = AEECLSID_EMUDISP2;
      break;
   case AEECLSID_APPFRAME3:
      pMe->clsEmuDisp = AEECLSID_EMUDISP3;
      break;
   case AEECLSID_APPFRAME4:
      pMe->clsEmuDisp = AEECLSID_EMUDISP4;
      break;
   default:
      pMe->clsEmuDisp = AEECLSID_EMUDISP1;
      break;
   }
#endif

   switch (cls) {
   case AEECLSID_APPFRAME2:
      clsAnnunCtl = AEECLSID_AnnunciatorControl_Display2;
      break;
   case AEECLSID_APPFRAME3:
      clsAnnunCtl = AEECLSID_AnnunciatorControl_Display3;
      break;
   case AEECLSID_APPFRAME4:
      clsAnnunCtl = AEECLSID_AnnunciatorControl_Display4;
      break;
   default:
      clsAnnunCtl = AEECLSID_AnnunciatorControl_Display1;
      break;
   }

   if(SUCCESS != ISHELL_CreateInstance(piShell, clsAnnunCtl, 
                                       (void **)&pMe->pAnnunCtl)) {
      pMe->pAnnunCtl = NULL;
   }

   {
      RGBVAL aClrs[CLR_USER_BACKGROUND + 1];
      int nItem;
      int nSize = sizeof(aClrs);

      switch (cls) {
      case AEECLSID_APPFRAME2:
         nItem = AEE_DEVICEITEM_SYS_COLORS_DISP2;
         break;
      case AEECLSID_APPFRAME3:
         nItem = AEE_DEVICEITEM_SYS_COLORS_DISP3;
         break;
      case AEECLSID_APPFRAME4:
         nItem = AEE_DEVICEITEM_SYS_COLORS_DISP4;
         break;
      default:
         nItem = AEE_DEVICEITEM_SYS_COLORS_DISP1;
         break;
      }

      if (SUCCESS != ISHELL_GetDeviceInfoEx(piShell, nItem, aClrs, &nSize)) {
         OEMAppFrame_Release((IAppFrame*)pMe);
         return EFAILED;
      }

      pMe->rgbBackground = aClrs[CLR_USER_BACKGROUND];
   }

Error:
   *ppif = pMe;
   return nResult;
}


static uint32 OEMAppFrame_AddRef(IAppFrame *po)
{
   OEMAppFrame *pMe = (OEMAppFrame*)(void *)po;
   return ++(pMe->uRefs);
}

static uint32 OEMAppFrame_Release(IAppFrame *po)
{
   OEMAppFrame *pMe = (OEMAppFrame*)(void *)po;

   if (!pMe->uRefs)
      return 0;

   if (--pMe->uRefs > 0)
      return pMe->uRefs;

   // Ref count is zero, so clean up

   IQI_RELEASEIF(pMe->pbmDev);

   IQI_RELEASEIF(pMe->pAnnunCtl);

   // pbmApp was not addref'd so it should not be released
   // This is an unusual cirmumstance.  See related comment at the end of
   // OEMAppFrame_Init().

   FREE(pMe);

   return 0;
}

static int OEMAppFrame_QueryInterface(IAppFrame *po, AEECLSID clsid, void **ppNew)
{
   if (clsid == AEEIID_APPFRAME || clsid == AEECLSID_QUERYINTERFACE) {
      *ppNew = (void*)po;
      (void) OEMAppFrame_AddRef(po);
      return SUCCESS;
   }

   return ECLASSNOTSUPPORT;
}

static int OEMAppFrame_Init(IAppFrame *po, IBitmap *pbmDev, IBitmap *pbmApp, const char *pchDisp, int cDisp)
{
   OEMAppFrame *pMe = (OEMAppFrame*)(void *)po;
   IBitmapCtl *piBitmapCtl = 0;
   IBitmapDev *piBitmapDev = 0;
#if defined(FEATURE_BREW_DISPLAYROTATION)
   // Query the device bitmap and
   // application bitmap's IBitmapRot interface
   IBitmapRot *piBitmapRot = 0;
   IBitmapRot *piBitmapRotDev = 0;
#endif
#if defined(IMPLEMENT_MULTIDEPTH)
#if defined(AEE_SIMULATOR)
   uint32 *puDepthList = 0;
   int nDepthListSize = 0;
#endif
   int nMaxScheme; // Maximum available color scheme
   int nDefScheme; // Default color scheme
   int nScheme;    // Initial color scheme, used if nothing is specified.  Initialized to nDefScheme.
#endif
   int nWidth, nHeight, nPrevWidth, nPrevHeight, nAnnun = 1;
#if defined(FEATURE_BREW_DISPLAYROTATION)
   int nRotation, nRotationDev, nOrigWidth, nOrigHeight;
#endif
      
   int nErr = SUCCESS;
   struct Params setPrefs;
   setPrefs.nWidth = 
      setPrefs.nHeight = setPrefs.nColorscheme = 
      setPrefs.nAnnunciators = ELEMENT_VAL_DEF;
#if defined(FEATURE_BREW_DISPLAYROTATION)
   setPrefs.nRotation = ELEMENT_VAL_DEF;
#endif

   if(AEE_GetAppContextCls(AEE_GetAppContext()) == AEECLSID_SHELL)
   {
      nAnnun = 0;
   }
   else
   {
      nAnnun = 1;
   }

   if (pbmDev == NULL || pbmApp == NULL) {
      nErr = EBADPARM;
      goto bail;
   }

   nErr = IBITMAP_QueryInterface(pbmApp, AEEIID_BITMAPCTL, (void**)&piBitmapCtl);
   if (SUCCESS != nErr) {
      goto bail;
   }

   nErr = IBITMAP_QueryInterface(pbmApp, AEEIID_BITMAPDEV, (void**)&piBitmapDev);
   if (SUCCESS != nErr) {
      goto bail;
   }

#if defined(FEATURE_BREW_DISPLAYROTATION)
   // App's IBitmapRot
   nErr = IBITMAP_QueryInterface(pbmApp, AEEIID_BITMAPROT, (void**)&piBitmapRot);
   if(SUCCESS != nErr) {
      goto bail;
   }

   // Device's IBitmapRot
   nErr = IBITMAP_QueryInterface(pbmDev, AEEIID_BITMAPROT, (void**)&piBitmapRotDev);
   if(SUCCESS != nErr) {
      goto bail;
   }
#endif

#if defined(IMPLEMENT_MULTIDEPTH) && defined(AEE_SIMULATOR)
   {
      IEmuDisp   *piEmuDisp = 0;
      IEmuDisp2  *piEmuDisp2 = 0;
      AEEBitmapInfo bi;
      uint32 i;

      // Get ahold of the IEmuDisp2 interface to get the attributes for this display.
      // IEmuDisp2 is only implemented the version 3.1.4 and later of the Simulator.

      nErr = AEE_CreateInstanceSys(pMe->clsEmuDisp, (void**)&piEmuDisp);
      if (SUCCESS != nErr) {
         goto bail_emudisp;
      }

      nErr = IEMUDISP_QueryInterface(piEmuDisp, AEEIID_EMUDISP2, (void**)&piEmuDisp2);
      if (SUCCESS != nErr) {
         goto bail_emudisp;
      }

      // Get and save the display dimensions, then set the scheme and default scheme
      // to the default depth of the display.

      nErr = IEMUDISP2_GetInfo(piEmuDisp2, &bi, sizeof(bi));
      if (SUCCESS != nErr) {
         goto bail_emudisp;
      }
      nScheme = nDefScheme = bi.nDepth;

      // Get the list of supported depths for this display.

      nDepthListSize = 0;
      IEMUDISP2_GetDepthList(piEmuDisp2, 0, &nDepthListSize);

      if (0 != nDepthListSize) {
         puDepthList = MALLOC(nDepthListSize);
         if ((uint32 *)0 == puDepthList) {
            nErr = ENOMEMORY;
            goto bail_emudisp;
         }

         nErr = IEMUDISP2_GetDepthList(piEmuDisp2, puDepthList, &nDepthListSize);
         if (SUCCESS != nErr) {
            goto bail_emudisp;
         }
      }

      // Find the maximum depth supported by this display.

      nMaxScheme = nDefScheme;
      for (i = 0; i < nDepthListSize / sizeof(uint32); i++) {
         nMaxScheme = MAX((int)puDepthList[i], nMaxScheme);
      }

   bail_emudisp:
      RELEASEIF(piEmuDisp);
      RELEASEIF(piEmuDisp2);
      if (SUCCESS != nErr) {
         goto bail;
      }
   }
#endif // IMPLEMENT_MULTIDEPTH && AEE_SIMULATOR

   nErr = IBITMAP_GetInfo(pbmDev, &pMe->bi, sizeof(pMe->bi));
   if (SUCCESS != nErr) {
      goto bail;
   }

#if defined(FEATURE_BREW_DISPLAYROTATION)
   // Store the apps and device bitmaps rotation
   // before making any modifications
   nRotation = IBITMAPROT_GetRotation(piBitmapRot);
   nRotationDev = IBITMAPROT_GetRotation(piBitmapRotDev);

   // Get the original dimensions of the screen
   if(nRotationDev == 90 || nRotationDev == 270) {
      nOrigWidth = pMe->bi.cy;
      nOrigHeight = pMe->bi.cx;
   } else {
      nOrigWidth = pMe->bi.cx;
      nOrigHeight = pMe->bi.cy;
   }

   // Reset the App's bitmap to the maximum size
   // for the given rotation
   if(nRotation == 90 || nRotation == 270) {
      nWidth = nOrigHeight;
      nHeight = nOrigWidth;
   } else {
      nWidth = nOrigWidth;
      nHeight = nOrigHeight;
   }
#else
   nWidth = pMe->bi.cx;
   nHeight = pMe->bi.cy;
#endif // FEATURE_BREW_DISPLAYROTATION

   nErr = IBITMAP_GetInfo(pbmApp, &pMe->bi, sizeof(pMe->bi));
   if (SUCCESS != nErr) {
      goto bail;
   }

   nPrevWidth = pMe->bi.cx;
   nPrevHeight = pMe->bi.cy;

   SETAEERECT(&pMe->rcAppRegion, 0, 0, nWidth, nHeight);

   nErr = IBITMAPCTL_Restrict(piBitmapCtl, &pMe->rcAppRegion);
   if (SUCCESS != nErr) {
      goto bail;
   }

   for (;;) {
      int nElResult;
      const char *pchKey, *pchVal;
      int cKey, cVal;

      nElResult = GetElement(&pchDisp, &cDisp, &pchKey, &cKey, &pchVal, &cVal);
      if (ELEMENT_DONE == nElResult) {
         break;
      }

      if (ELEMENT_OK != nElResult) {
         // bad element, ignore
         continue;
      }

      if (1 != cKey) {
         // all of the keys we define have length 1, ignore undefined keys
         continue;
      }

      switch (pchKey[0]) {
      case 'w':
         setPrefs.nWidth = GetElementInt(pchVal, cVal);
         break;
      case 'h':
         setPrefs.nHeight = GetElementInt(pchVal, cVal);
         break;
#if defined(IMPLEMENT_MULTIDEPTH)
      case 'c':
         setPrefs.nColorscheme = GetElementInt(pchVal, cVal);
         break;
#endif
      case 'a':
         setPrefs.nAnnunciators = GetElementInt(pchVal, cVal);
         break;
#if defined(FEATURE_BREW_DISPLAYROTATION)
      case 'r':
         setPrefs.nRotation = GetElementInt(pchVal, cVal);
         break;
#endif
      default:
         break;
      }
   }

#if defined(IMPLEMENT_MULTIDEPTH)
   nScheme     = DecodeElement(setPrefs.nColorscheme, 0, nScheme, nMaxScheme);
#endif
#if defined(FEATURE_BREW_DISPLAYROTATION)
   nRotation   = DecodeElement(setPrefs.nRotation, nRotation, 0, 0);
#endif
   pMe->nAnnun = DecodeElement(setPrefs.nAnnunciators, pMe->nAnnun, nAnnun, nAnnun);


   /* Clear the annunciator rectangle */
   SETAEERECT(&pMe->rcAnnunBar, 0,0,0,0);

   if (NULL != pMe->pAnnunCtl) {

      if(SUCCESS != IAnnunciatorControl_GetRegion(pMe->pAnnunCtl,
                                                  &pMe->rcAnnunBar)) {
         pMe->nAnnun = 0;
      }
   }
   else {
      pMe->nAnnun = 0;
   }

#if defined(IMPLEMENT_MULTIDEPTH)
   if (nScheme != 0) { // 0 is our own private way of saying don't change mode
      IBitmap *pbmToClone = NULL;

      // OEM should determine which bit depths are supported
#if defined(AEE_SIMULATOR)
      {
         uint32 i;

         // determine whether selected depth is valid
         boolean bFoundScheme = FALSE;
         for (i = 0; i < nDepthListSize / sizeof(uint32); i++) {
            if (puDepthList[i] == (unsigned)nScheme) {
               bFoundScheme = TRUE;
               break;
            }
         }

         // if not valid, use default
         if (!bFoundScheme) {
            // default depth is considered valid, whether or not in list
            nScheme = nDefScheme;
         }
      }
#endif // AEE_SIMULATOR

      if (nScheme > nMaxScheme) {
         // don't allow mode to be set to something not supported by skin
         nScheme = nMaxScheme;
      }

      // This bitmap use only used as a source for copying the mode.
      // We instantiate this bitmap as a child because:
      //    a) We need it to be the same size as the global device bitmap for clone to work right.
      //    b) Children don't take up much memory.
      switch (nScheme) {
      case 1:
         OEMBitmap1_New_Child(pbmDev, NULL, &pbmToClone);
         break;
      case 2:
         OEMBitmap2_New_Child(pbmDev, NULL, &pbmToClone);
         break;
      case 8:
         OEMBitmap8_New_Child(pbmDev, NULL, &pbmToClone);
         break;
      case 12:
         OEMBitmap12_New_Child(pbmDev, NULL, &pbmToClone);
         break;
      case 16:
         OEMBitmap16_New_Child(pbmDev, NULL, &pbmToClone);
         break;
      case 18:
         OEMBitmap18_New_Child(pbmDev, NULL, &pbmToClone);
         break;
      case 24:
         OEMBitmap24_New_Child(pbmDev, NULL, &pbmToClone);
         break;
      }

      if (NULL != pbmToClone) {
         OEMBitmap_CopyMode(pbmApp, pbmToClone);
         IBITMAP_Release(pbmToClone);
      }


   }
#endif // IMPLEMENT_MULTIDEPTH

#if defined(FEATURE_BREW_DISPLAYROTATION)
   // Modify the apps bitmap rotation based on what the caller passed.
   {
      // Setting rotation
      OEMBitmap_SetRotation(pbmApp, nRotation);
   }
#endif

   // Reset pMe->bi based on rotation
   nErr = IBITMAP_GetInfo(pbmApp, &pMe->bi, sizeof(pMe->bi));
   if(SUCCESS != nErr) {
      goto bail;
   }

   nWidth      = DecodeElement(setPrefs.nWidth, nPrevWidth, pMe->bi.cx, pMe->bi.cx);
   nHeight     = DecodeElement(setPrefs.nHeight, nPrevHeight, pMe->bi.cy, pMe->bi.cy);

   {
      AEEBitmapInfo biApp;

      nErr = IBITMAP_GetInfo(pbmApp, &biApp, sizeof(biApp));
      if (SUCCESS != nErr) {
         goto bail;
      }

      if(pMe->nAnnun == 0) {
         //No Annuciator Support
         SETAEERECT(&pMe->rcAnnunBar, 0, 0, 0, 0);
         SETAEERECT(&pMe->rcAppFrame, 0, 0, (int16)biApp.cx, (int16)biApp.cy);
      }
      else {
         //Recalculate AppFrameRec
         if((pMe->rcAnnunBar.dx != (int16) biApp.cx) &&
            (pMe->rcAnnunBar.dy != (int16) biApp.cy)) {
            //Extend it the whole way
            if(pMe->rcAnnunBar.dx >= pMe->rcAnnunBar.dy) {
               pMe->rcAnnunBar.dx = (int16) biApp.cx;
            }
            else {
               pMe->rcAnnunBar.dy = (int16) biApp.cy;
            }
         }

         if(pMe->rcAnnunBar.dx == (int16) biApp.cx) {
            //Horizontal Bar
            if(((int16) biApp.cy - (pMe->rcAnnunBar.y + pMe->rcAnnunBar.dy)) >= pMe->rcAnnunBar.y) {
               //Annunciator Bar on Top
               SETAEERECT(&pMe->rcAppFrame,
                          0,
                          pMe->rcAnnunBar.y + pMe->rcAnnunBar.dy,
                          (int16) biApp.cx,
                          (int16) biApp.cy - pMe->rcAppFrame.y);
            }
            else {
               //Annunciator Bar on Bottom
               SETAEERECT(&pMe->rcAppFrame, 0, 0, (int16) biApp.cx, pMe->rcAnnunBar.y);
            }
         }
         else {
            //Vertical Bar
            if(((int16)biApp.cx - (pMe->rcAnnunBar.x + pMe->rcAnnunBar.dx)) >= pMe->rcAnnunBar.x) {
               //Annunciator Bar on left
               SETAEERECT(&pMe->rcAppFrame,
                          pMe->rcAnnunBar.x + pMe->rcAnnunBar.dx,
                          0,
                          (int16) biApp.cx - pMe->rcAppFrame.x,
                          (int16) biApp.cy);
            }
            else {
               //Annunciator Bar on Right
               SETAEERECT(&pMe->rcAppFrame, 0, 0, pMe->rcAnnunBar.x, (int16) biApp.cy);
            }
         }
      }

      if (nWidth <= 0 || nWidth > (int)pMe->rcAppFrame.dx) {
         nWidth = pMe->rcAppFrame.dx;
      }
      if (nHeight <= 0 || nHeight > (int)pMe->rcAppFrame.dy) {
         nHeight = pMe->rcAppFrame.dy;
      }

      SETAEERECT(&pMe->rcAppRegion,
                 pMe->rcAppFrame.x, pMe->rcAppFrame.y, nWidth, nHeight);

      if (biApp.nDepth < 8) {
         // The supplied restriction implementation for these bit depths only supports
         // byte boundaries of x (for efficiency reasons).
         pMe->rcAppRegion.x += (int16)(((pMe->rcAppFrame.dx - nWidth) / 2) / ((8 / (int16)biApp.nDepth) * (8 / (int16)biApp.nDepth)));
      } else {
         pMe->rcAppRegion.x += (int16)((pMe->rcAppFrame.dx - nWidth) / 2);
      }
      pMe->rcAppRegion.y += (int16)((pMe->rcAppFrame.dy - nHeight) / 2);

      nErr = IBITMAPCTL_Restrict(piBitmapCtl, &pMe->rcAppRegion);
      if (SUCCESS != nErr) {
         goto bail;
      }

   }

   IBITMAP_AddRef(pbmDev);
   if ((IBitmap*)0 != pMe->pbmDev) {
      IBITMAP_Release(pMe->pbmDev);
   }
   pMe->pbmDev = pbmDev;

   // We cannot addref pbmApp, since doing so will prevent the app from giving
   // up this display.  (An app gives up claim to a display when the device
   // bitmap is destroyed.)  We are trusting BREW to keep the app device bitmap
   // around for our entire lifetime.  This is an unusual circumstance.  It is
   // almost always vital to addref any interface you hold on to.
   pMe->pbmApp = pbmApp;

bail:
#if defined(AEE_SIMULATOR)
   FREEIF(puDepthList);
#endif
   if ((IBitmapDev *)0 != piBitmapDev) {
      IBITMAPDEV_Release(piBitmapDev);
   }
   if ((IBitmapCtl *)0 != piBitmapCtl) {
      IBITMAPCTL_Release(piBitmapCtl);
   }
#if defined(FEATURE_BREW_DISPLAYROTATION)
   if ((IBitmapRot*)0 != piBitmapRot) {
      IBITMAPROT_Release(piBitmapRot);
   }
   if ((IBitmapRot*)0 != piBitmapRotDev) {
      IBITMAPROT_Release(piBitmapRotDev);
   }
#endif
   return nErr;
}

static int OEMAppFrame_Redraw(IAppFrame *po)
{
   OEMAppFrame *pMe = (OEMAppFrame*)(void *)po;
#if defined(FEATURE_BREW_DISPLAYROTATION)
   IBitmapRot * pbmRotApp = 0;
   IBitmapRot * pbmRotDev = 0;
   int nAppRot, nDevRot;
#endif
   NativeColor nc;
   AEERect rc;
   int nErr = SUCCESS;

   if (pMe->pbmDev == NULL || pMe->pbmApp == NULL) {
      nErr = EFAILED;
      goto bail;
   }

#if defined(IMPLEMENT_MULTIDEPTH)
   if (AEEGETPVTBL(pMe->pbmDev, IBitmap) != AEEGETPVTBL(pMe->pbmApp, IBitmap)) {
      NativeColor c;
      OEMBitmap_CopyMode(pMe->pbmDev, pMe->pbmApp);
      c = IBITMAP_RGBToNative(pMe->pbmDev, pMe->rgbBackground);
      IBITMAP_FillRect(pMe->pbmDev, &pMe->rcAppRegion, c, AEE_RO_COPY);
   }
#endif // IMPLEMENT_MULTIDEPTH

#if defined(FEATURE_BREW_DISPLAYROTATION)
   // Translate apps rotation to the device bitmap
   nErr = IBITMAP_QueryInterface(pMe->pbmApp, AEEIID_BITMAPROT, (void**)&pbmRotApp);
   if (SUCCESS != nErr) {
      goto bail;
   }
   nErr = IBITMAP_QueryInterface(pMe->pbmDev, AEEIID_BITMAPROT, (void**)&pbmRotDev);
   if (SUCCESS != nErr) {
      goto bail;
   }
   nAppRot = IBITMAPROT_GetRotation(pbmRotApp);
   nDevRot = IBITMAPROT_GetRotation(pbmRotDev);

   if (nAppRot != nDevRot){
      OEMBitmap_SetRotation(pMe->pbmDev, nAppRot);
      nc = IBITMAP_RGBToNative(pMe->pbmDev, pMe->rgbBackground);
      IBITMAP_FillRect(pMe->pbmDev, &pMe->rcAppRegion, nc, AEE_RO_COPY);
   }
#endif // FEATURE_BREW_DISPLAYROTATION

   nc = IBITMAP_RGBToNative(pMe->pbmDev, MAKE_RGB(0x00, 0x00, 0x00));

   rc.x  = pMe->rcAppFrame.x;
   rc.y  = pMe->rcAppFrame.y;
   rc.dx = pMe->rcAppFrame.dx;
   rc.dy = pMe->rcAppRegion.y - rc.y;
   IBITMAP_FillRect(pMe->pbmDev, &rc, nc, AEE_RO_COPY);

   rc.y = pMe->rcAppRegion.y + pMe->rcAppRegion.dy;
   rc.dy = pMe->rcAppFrame.y + pMe->rcAppFrame.dy - rc.y;
   IBITMAP_FillRect(pMe->pbmDev, &rc, nc, AEE_RO_COPY);

   rc.y = pMe->rcAppRegion.y;
   rc.dx = pMe->rcAppRegion.x - rc.x;
   rc.dy = pMe->rcAppRegion.dy;
   IBITMAP_FillRect(pMe->pbmDev, &rc, nc, AEE_RO_COPY);

   rc.x = pMe->rcAppRegion.x + pMe->rcAppRegion.dx;
   rc.dx = pMe->rcAppFrame.x + pMe->rcAppFrame.dx - rc.x;
   IBITMAP_FillRect(pMe->pbmDev, &rc, nc, AEE_RO_COPY);

   if (NULL != pMe->pAnnunCtl) {
      IAnnunciatorControl_Enable(pMe->pAnnunCtl, (boolean)(pMe->nAnnun != 0));
   }

bail:
#if defined(FEATURE_BREW_DISPLAYROTATION)
   if ((IBitmapRot *)0 != pbmRotApp) {
      IBITMAPROT_Release(pbmRotApp);
   }
   if ((IBitmapRot *)0 != pbmRotDev) {
      IBITMAPROT_Release(pbmRotDev);
   }
#endif
   return nErr;

}

static int OEMAppFrame_Update(IAppFrame *po)
{
   (void) po;
   return SUCCESS;
}

static int OEMAppFrame_Disable(IAppFrame *po)
{
   // Since the only place we redraw is in OEMAppFrame_Redraw, there is nothing to do here.
   (void) po;
   return SUCCESS;
}




static int GetElement(const char **ppchIn, int *pcIn, const char **ppchKey, int *pcKey, const char **ppchVal, int *pcVal)
{
   while (*pcIn && *ppchIn[0] == ',') {
      (*pcIn)--;
      (*ppchIn)++;
   }

   if (0 == *pcIn) {
      return ELEMENT_DONE;
   }

   *ppchKey = *ppchIn;
   *pcKey = 0;

   while (*pcIn && *ppchIn[0] != ':' && *ppchIn[0] != ',') {
      (*pcIn)--;
      (*ppchIn)++;
      (*pcKey)++;
   }

   // pass : char
   if (*pcIn && *ppchIn[0] == ':') {
      (*pcIn)--;
      (*ppchIn)++;
   }

   *ppchVal = *ppchIn;
   *pcVal = 0;

   while (*pcIn && *ppchIn[0] != ',') {
      (*pcIn)--;
      (*ppchIn)++;
      (*pcVal)++;
   }

   return ELEMENT_OK;
}

static int GetElementInt(const char *pchVal, int cVal)
{
   int nAcc = 0;
   boolean bNegate = FALSE;

   if (1 == cVal) {
      if ('*' == *pchVal) {
         return ELEMENT_VAL_STAR;
      }
      if ('p' == *pchVal) {
         // preserve
         return ELEMENT_VAL_PRES;
      }
   }

   if (!cVal) {
      return ELEMENT_VAL_DEF;
   }

   while (cVal--) {
      if (*pchVal < '0' || *pchVal > '9') {
         return ELEMENT_VAL_DEF;
      }
      nAcc = nAcc * 10 + *pchVal - '0';
      pchVal++;
   }

   return bNegate ? -nAcc: nAcc;
}

static int DecodeElement(int nVal, int nOrigVal, int nDefaultVal, int nMaxVal) {

   switch(nVal) {
      case ELEMENT_VAL_STAR:
         return nMaxVal;
         break;
      case ELEMENT_VAL_DEF:
         return nDefaultVal;
         break;
      case ELEMENT_VAL_PRES:
         return nOrigVal;
         break;
      default:
         return nVal;
   }
}
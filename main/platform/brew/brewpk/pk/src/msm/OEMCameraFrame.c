/*===========================================================================

FILE: OEMCamera.c

SERVICES: ICamera interface implementation

DESCRIPTION
   This file implements BREW ICamera. ICamera controls the device camera
   and allows recording of snapshot or movie.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

        Copyright © 1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "OEMFeatures.h"

#if defined(FEATURE_BREW_CAMERA)

#ifdef FEATURE_YCBCR
#include "camera.h"
#include "qcamraw.h"
#include "IYCbCr.h"
#endif

#include "msm/OEMCameraDMSS.h"
#include "OEMCriticalSection.h"
#include "AEEStdLib.h"

/*===========================================================================

                      MACRO DEFINITIONS

===========================================================================*/
/*==================================================================

  Frame IBitmap implementation.

  Frames captured by camera are delivered to app as IBitmap object
  containing the frame. Users need to call ICAMERA_GetFrame() to get
  the IBitmap object in CAM_STATUS_FRAME callback.
  Typically, this IBitmap object is not an IDIB and users can BltOut 
  this object to a well-known IDIB and access the frame data.
  Please note that it is BltOut function that performs all
  processing like rotation, color conversion, etc. before bliting.

==================================================================*/
static uint32 FrameBitmap_AddRef(IBitmap *po);
static uint32 FrameBitmap_Release(IBitmap *po);
static int FrameBitmap_QueryInterface(IBitmap *po, AEECLSID clsid, void **ppNew);
static NativeColor FrameBitmap_RGBToNative(IBitmap *po, RGBVAL rgb);
static RGBVAL FrameBitmap_NativeToRGB(IBitmap *po, NativeColor color);
static int FrameBitmap_DrawPixel(IBitmap *po, unsigned x, unsigned y, NativeColor color, AEERasterOp rop);
static int FrameBitmap_GetPixel(IBitmap *po, unsigned x, unsigned y, NativeColor *pColor);
static int FrameBitmap_SetPixels(IBitmap *po, unsigned cnt, AEEPoint *pPoint, NativeColor color, AEERasterOp rop);
static int FrameBitmap_DrawHScanline(IBitmap *po, unsigned y, unsigned xMin, unsigned xMax, NativeColor color, AEERasterOp rop);
static int FrameBitmap_FillRect(IBitmap *po, const AEERect *pRect, NativeColor color, AEERasterOp rop);
static int FrameBitmap_BltIn(IBitmap *po, int xDst, int yDst, int dx, int dy, IBitmap *pSrc, int xSrc, int ySrc, AEERasterOp rop);
static int FrameBitmap_BltOut(IBitmap *po, int xDst, int yDst, int dx, int dy, IBitmap *pDst, int xSrc, int ySrc, AEERasterOp rop);
static int FrameBitmap_GetInfo(IBitmap *po, AEEBitmapInfo *pInfo, int nSize);
static int FrameBitmap_CreateCompatibleBitmap(IBitmap *po, IBitmap **ppIBitmap, uint16 w, uint16 h);
static int FrameBitmap_SetTransparencyColor(IBitmap *po, NativeColor color);
static int FrameBitmap_GetTransparencyColor(IBitmap *po, NativeColor *pColor);

static void FrameBitmap_Delete(FrameBitmap * pme);

static const VTBL(IBitmap) gFrameBitmapFuncs = {
   FrameBitmap_AddRef,
   FrameBitmap_Release,
   FrameBitmap_QueryInterface,
   FrameBitmap_RGBToNative,
   FrameBitmap_NativeToRGB,
   FrameBitmap_DrawPixel,
   FrameBitmap_GetPixel,
   FrameBitmap_SetPixels,
   FrameBitmap_DrawHScanline,
   FrameBitmap_FillRect,
   FrameBitmap_BltIn,
   FrameBitmap_BltOut,
   FrameBitmap_GetInfo,
   FrameBitmap_CreateCompatibleBitmap,
   FrameBitmap_SetTransparencyColor,
   FrameBitmap_GetTransparencyColor
};

#ifdef FEATURE_YCBCR
static int FrameBitmap_GetYUVBitmap(IBitmap *po, IBitmap **ppIBitmap);
extern int YCbCr_New(uint32 uScheme, uint32 cx, uint32 cy, IYCbCr **ppiYCbCr);
#endif

/*==================================================================

            USER MODE CODE SECTION

==================================================================*/
int OEMCamera_GetConfigInfo(CameraConfigInfo * pInfo)
{
   if (!pInfo)
      return EBADPARM;
   pInfo->wNotifyCount = OEMCAMERA_RSP_START_MAX + OEMCAMERA_RSP_COMMAND_MAX;
   return SUCCESS;
}

boolean OEMCamera_GetFrameTrigger(IBitmap * ppFrame)
{
   // This function is called from AEECamera within ICAMERA_GetFrame() API.
   // Here, OEM can do custom processing on this frame to get it ready for
   // the caller of ICAMERA_GetFrame().
   // Return TRUE is you perform custom processing.

   return FALSE;
}

int OEMCamera_FrameBitmapNew(IBitmap ** ppFrame, uint16 * pwSize)
{
   FrameBitmap *  pme;

   if (!ppFrame || !pwSize)
      return EBADPARM;

   pme = MALLOC(sizeof(FrameBitmap));
   if (pme)
   {
      *pwSize = sizeof(FrameBitmap);
      pme->m_dib.pvt = (AEEVTBL(IBitmap) *)&gFrameBitmapFuncs;
      pme->m_nRefs = 1;
   }
   else
      *pwSize = 0;

   *ppFrame = (IBitmap *)pme;
   return (pme ? SUCCESS : ENOMEMORY);
}

static void FrameBitmap_Delete(FrameBitmap * pme)
{
   FREE(pme);
}

static uint32 FrameBitmap_AddRef(IBitmap *po)
{
   FrameBitmap *  pme = (FrameBitmap *)po;
   return ++pme->m_nRefs;
}

static uint32 FrameBitmap_Release(IBitmap *po)
{
   FrameBitmap *  pme = (FrameBitmap *)po;

   if (pme->m_nRefs <= 0)
      return 0;

   if (--pme->m_nRefs > 0)
      return pme->m_nRefs;

   FrameBitmap_Delete(pme);

   return 0;
}

static int FrameBitmap_QueryInterface(IBitmap *po, AEECLSID clsid, void **ppNew)
{
   FrameBitmap *  pme = (FrameBitmap *)po;

   if (!ppNew)
      return EBADPARM;

   if (clsid == AEECLSID_BITMAP || clsid == AEECLSID_QUERYINTERFACE)// || clsid == AEECLSID_DIB) 
   {
      *ppNew = (void*)pme;
      FrameBitmap_AddRef((IBitmap *)pme);
      return SUCCESS;
   }
#ifdef FEATURE_YCBCR
   else if (clsid == AEEIID_YCBCR) 
   {
      return FrameBitmap_GetYUVBitmap((IBitmap *)pme, (IBitmap **)ppNew);
   }
#endif
   else
      *ppNew = NULL;
   
   return ECLASSNOTSUPPORT;
}

static NativeColor FrameBitmap_RGBToNative(IBitmap *po, RGBVAL rgb)
{
   return EUNSUPPORTED;
}

static RGBVAL FrameBitmap_NativeToRGB(IBitmap *po, NativeColor color)
{
   return EUNSUPPORTED;
}

static int FrameBitmap_DrawPixel(IBitmap *po, unsigned x, unsigned y, NativeColor color, AEERasterOp rop)
{
   return EUNSUPPORTED;
}

static int FrameBitmap_GetPixel(IBitmap *po, unsigned x, unsigned y, NativeColor *pColor)
{
   return EUNSUPPORTED;
}

static int FrameBitmap_SetPixels(IBitmap *po, unsigned cnt, AEEPoint *pPoint, NativeColor color, AEERasterOp rop)
{
   return EUNSUPPORTED;
}

static int FrameBitmap_DrawHScanline(IBitmap *po, unsigned y, unsigned xMin, unsigned xMax, NativeColor color, AEERasterOp rop)
{
   return EUNSUPPORTED;
}

static int FrameBitmap_FillRect(IBitmap *po, const AEERect *pRect, NativeColor color, AEERasterOp rop)
{
   return EUNSUPPORTED;
}

static int FrameBitmap_BltIn(IBitmap *po, int xDst, int yDst, int dx, int dy, IBitmap *pSrc, int xSrc, int ySrc, AEERasterOp rop)
{
   return EUNSUPPORTED;
}


static int FrameBitmap_BltOut(IBitmap *po, int xDst, int yDst, int dx, int dy, IBitmap *pDst, int xSrc, int ySrc, AEERasterOp rop)
{
   FrameBitmap *  pme = (FrameBitmap *)po;
   IDIB *         pDstDIB;
   int            nErr;

   if (SUCCESS != IBITMAP_QueryInterface(pDst, AEECLSID_DIB, (void **)&pDstDIB))
      return EFAILED;

   nErr = AEECamera_FrameBlt(pDst, sizeof(IDIB), xDst, yDst, dx, dy, &pme->m_rawFrame, sizeof(pme->m_rawFrame), xSrc, ySrc, AEE_RO_COPY);
   IBITMAP_Release((IBitmap *)pDstDIB);

   return nErr;

/*
#if defined(AEE_SIMULATOR)
   IDIB *   pDstDIB;
   int      nRet;
   AEERect  rc;

   if (SUCCESS != IBITMAP_QueryInterface(pDst, AEECLSID_DIB, (void **)&pDstDIB))
      return EFAILED;

   nRet = camera_blt(&pme->m_rawFrame, (uint16)xSrc, (uint16)ySrc, (void *)pDstDIB->pBmp, (uint16)xDst, (uint16)yDst, (uint16)dx, (uint16)dy);
   rc.x = xDst;
   rc.y = yDst;
   rc.dx = dx;
   rc.dy = dy;
   IBITMAP_Invalidate(pDst, &rc);

   IBITMAP_Release((IBitmap *)pDstDIB);
   
   return ICamera_AEEError(nRet);
#else
   //
   // QCT-specific code:
   //
//   AEEDIB *   pDstDIB;
   IDIB *   pDstDIB;
   int      nRet;
   AEERect  rc;
#if !defined(OEM_PIXEL_SIZE)
#define OEM_PIXEL_SIZE   2
#endif

//   if (SUCCESS != IBITMAP_QueryInterface(pDst, AEECLSID_AEEDIB, (void **)&pDstDIB))
//      return EFAILED;
   if (SUCCESS != IBITMAP_QueryInterface(pDst, AEECLSID_DIB, (void **)&pDstDIB))
      return EFAILED;

   nRet = camera_blt(&pme->m_rawFrame, (uint16)(pDstDIB->nPitch / OEM_PIXEL_SIZE), pDstDIB->cy, (void *)pDstDIB->pBmp, (uint16)xDst, (uint16)yDst, (uint16)dx, (uint16)dy);
   rc.x = xDst;
   rc.y = yDst;
   rc.dx = dx;
   rc.dy = dy;
   IBITMAP_Invalidate(pDst, &rc);

   return ICamera_AEEError(nRet);
#endif // defined(AEE_SIMULATOR)
*/
}

static int FrameBitmap_GetInfo(IBitmap *po, AEEBitmapInfo *pInfo, int nSize)
{
   FrameBitmap *  pme = (FrameBitmap *)po;

   pInfo->cx = pme->m_dib.cx;
   pInfo->cy = pme->m_dib.cy;
   pInfo->nDepth = pme->m_dib.nDepth;
   return SUCCESS;
}

static int FrameBitmap_CreateCompatibleBitmap(IBitmap *po, IBitmap **ppIBitmap, uint16 w, uint16 h)
{
   return EUNSUPPORTED;
}
#ifdef FEATURE_YCBCR
static int FrameBitmap_GetYUVBitmap(IBitmap *po, IBitmap **ppIBitmap)
{
    FrameBitmap *  pme = (FrameBitmap *)po;
    int rv = EBADPARM;
    IYCbCr * pYCbCr = NULL;
    //uint32 uScheme = IYCRCB_COLORSCHEME_420LP; //map to IPL_YCrCb420_LINE_PK
    uint32 uScheme = IYCBCR_COLORSCHEME_420LP; //map to MDP_YCBCR420 for IMMOverlay
    uint16 w = 0;
    uint16 h = 0;

    if (pme) 
    {
        rv =  YCbCr_New(uScheme, 0, 0, (IYCbCr **)&pYCbCr);
        w = pme->m_rawFrame.dx;
        h = pme->m_rawFrame.dy;
    }

    if (pYCbCr) 
    {
        // in snapshot state, we should use thumbnail_image for display
        if (CAMERA_H2V2 == pme->m_rawFrame.format)
        {
          pYCbCr->pLuma = qcamrawGetDataYPtr(pme->m_rawFrame.thumbnail_image);
          pYCbCr->pChroma = qcamrawGetDataCbCrPtr (pme->m_rawFrame.thumbnail_image);    
        }
        else
        {
          pYCbCr->pLuma = qcamrawGetDataYPtr(pme->m_rawFrame.buffer);
          pYCbCr->pChroma = qcamrawGetDataCbCrPtr (pme->m_rawFrame.buffer);    
        }

        pYCbCr->cx      = w;
        pYCbCr->cy      = h;
        pYCbCr->nYPitch = w;
        pYCbCr->nCPitch = w;

        *ppIBitmap = IYCBCR_TO_IBITMAP (pYCbCr);
    }
    else
        *ppIBitmap = NULL;

    return rv;

}
#endif

static int FrameBitmap_SetTransparencyColor(IBitmap *po, NativeColor color)
{
   return EUNSUPPORTED;
}

static int FrameBitmap_GetTransparencyColor(IBitmap *po, NativeColor *pColor)
{
   return EUNSUPPORTED;
}

#endif // defined(FEATURE_BREW_CAMERA)

/*=================================================================================
FILE:          YCbCr.c

SERVICES:      IYCbCr interface

DESCRIPTION:   This module implements the YCbCr class that can be created using
               YCbCr_New() and blt out image to RGB device using YCbCr_BltOut().
    
PUBLIC CLASSES:   Not Applicable

INITIALIAZTION AND SEQUENCEING REQUIREMENTS: Not Applicable

        Copyright © 2005 - 2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=================================================================================*/
/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/multimedia/camera/staticextensions/iycbcr/main/latest/src/YCbCr.c#5 $
 $DateTime: 2009/02/20 17:46:32 $
 $Author: shilpam $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------
 09/02/08  kdiv   ULC Write back cache changes
 07/01/08  RK     Feature defined for IPL when camera is not present in target.
 03/13/08  RK     Fixed issue to Display YCbCr image through IPL.
 04/18/06  NC     Fixed lint errors.
 03/03/06  NC     Fixed compiler warning when FEATURE_MDP is not defined.  
 09/28/05  JN     Use MDP color conversion only for IYCBCR_COLORSCHEME_420LP.
 08/04/05  enj    Added support for MDP color conversions
 07/24/05  ADM    Changes needed to support YCrCb422LP and YCrCb420LP
 03/02/05  JN     Color schemes supported are YCbCr422LP and YCbCr420LP.
 02/04/05  JN     Cleaned up version after code review with QIS.
============================================================================*/

//lint -e801 suppress errors on use of goto
//lint -e717 suppress errors on use of do ... while(0)
//lint -e740 suppress errors on pointer cast
//lint -e611

#include "ipl.h"
#include "IYCbCr.h"
#include "YCbCr_priv.h"
#include "AEEBitmap.h"
#include "AEEStdLib.h"
#include "AEEDisp.h"
#include "OEMFeatures.h"
#ifdef FEATURE_MDP
# include "mdp_api.h"
#ifdef T_MSM7500
#error code not present
#endif /* T_MSM7500 */
#endif /* FEATURE_MDP */
#include "jpegi.h"

//------------------------------------
//--------  Helper functions  --------
//------------------------------------

#define CLIP_TO_SRC(xDst,yDst,xSrc,ySrc,cxSrc,cySrc,cx,cy)                          \
do {                                                                                \
   if ((xSrc) < 0) {                                                                \
      (xDst) -= (xSrc);                                                             \
      (cx) += (xSrc);                                                               \
      (xSrc) = 0;                                                                   \
   }                                                                                \
   if ((ySrc) < 0) {                                                                \
      (yDst) -= (ySrc);                                                             \
      (cy) += (ySrc);                                                               \
      (ySrc) = 0;                                                                   \
   }                                                                                \
   if ((xSrc) + (cx) > (cxSrc)) {                                                   \
      (cx) = (cxSrc) - (xSrc);                                                      \
   }                                                                                \
   if ((ySrc) + (cy) > (cySrc)) {                                                   \
      (cy) = (cySrc) - (ySrc);                                                      \
   }                                                                                \
} while (0)

#define CLIP_TO_DST(xDst,yDst,cxDst,cyDst,xSrc,ySrc,cx,cy)                          \
do {                                                                                \
   if ((xDst) < 0) {                                                                \
      (xSrc) -= (xDst);                                                             \
      (cx) += (xDst);                                                               \
      (xDst) = 0;                                                                   \
   }                                                                                \
   if ((yDst) < 0) {                                                                \
      (ySrc) -= (yDst);                                                             \
      (cy) += (yDst);                                                               \
      (yDst) = 0;                                                                   \
   }                                                                                \
   if (cx > cxDst - xDst) {                                                         \
      cx = cxDst - xDst;                                                            \
   }                                                                                \
   if (cy > cyDst - yDst) {                                                         \
      cy = cyDst - yDst;                                                            \
   }                                                                                \
} while (0)

static int YCbCr_GetDstDIB(IBitmap *pDst, IDIB **ppDstDIB);
#ifndef FEATURE_IPL_NO_CAMERA   
static int YCbCr_BltOut_ipl(IBitmap *po, int xDst, int yDst, int dx, int dy, 
                            IBitmap *pDst, int xSrc, int ySrc, AEERasterOp rop);
#endif /*!FEATURE_IPL_NO_CAMERA*/
#ifndef FEATURE_CAMERA8X50
#ifdef FEATURE_MDP2
#error code not present
#endif //FEATURE_MDP
#endif /*!FEATURE_CAMERA8X50*/
/************************************************************************
 ** I N T E R F A C E   F U N C T I O N   D E C L A R A T I O N S
 ************************************************************************/

static uint32 YCbCr_AddRef(IBitmap *po);
static uint32 YCbCr_Release(IBitmap *po);
static int YCbCr_QueryInterface(IBitmap *po, AEECLSID clsid, void **ppNew);
static NativeColor YCbCr_RGBToNative(IBitmap *po, RGBVAL rgb);
static RGBVAL      YCbCr_NativeToRGB(IBitmap *po, NativeColor clr);
static int YCbCr_DrawPixel(IBitmap *po, unsigned x, unsigned y, NativeColor color,
                           AEERasterOp rop);
static int YCbCr_GetPixel(IBitmap *po, unsigned x, unsigned y, NativeColor *pColor);
static int YCbCr_SetPixels(IBitmap *po, unsigned int cnt, AEEPoint *pPoint, 
                           NativeColor color, AEERasterOp rop);
static int YCbCr_DrawHScanline(IBitmap *po, unsigned y, unsigned xMin, 
                               unsigned xMax, NativeColor color, AEERasterOp rop);
static int YCbCr_FillRect(IBitmap *po, const AEERect *pRect, NativeColor color,
                          AEERasterOp rop);
static int YCbCr_BltIn(IBitmap *po, int xDst, int yDst, int dx, int dy, 
                       IBitmap *pSrc, int xSrc, int ySrc, AEERasterOp rop);
static int YCbCr_BltOut(IBitmap *po, int xDst, int yDst, int dx, int dy, 
                        IBitmap *pDst, int xSrc, int ySrc, AEERasterOp rop);
static int YCbCr_GetInfo(IBitmap *po, AEEBitmapInfo *pInfo, int nSize);
static int YCbCr_CreateCompatibleBitmap(IBitmap *po, IBitmap **ppIBitmap,
                                        uint16 w, uint16 h);
static int YCbCr_SetTransparencyColor(IBitmap *po, NativeColor color);
static int YCbCr_GetTransparencyColor(IBitmap *po, NativeColor *pColor);

static const VTBL(IBitmap) gYCbCrFuncs = {
   YCbCr_AddRef,
   YCbCr_Release,
   YCbCr_QueryInterface,
   YCbCr_RGBToNative,
   YCbCr_NativeToRGB,
   YCbCr_DrawPixel,
   YCbCr_GetPixel,
   YCbCr_SetPixels,
   YCbCr_DrawHScanline,
   YCbCr_FillRect,
   YCbCr_BltIn,
   YCbCr_BltOut,
   YCbCr_GetInfo,
   YCbCr_CreateCompatibleBitmap,
   YCbCr_SetTransparencyColor,
   YCbCr_GetTransparencyColor
};


/*===========================================================================

FUNCTION: YCbCr_New

DESCRIPTION
  This function creates an IYCBCR bitmap with the specified dimensions and
  color format. 

DEPENDENCIES
  None

RETURN VALUE
  Returns AEE return values

  AEE_SUCCESS:  Bitmap was created
  EBADPARM:     One of the arguments was invalid
  ENOMEMORY:    There was not enough memory to create the bitmap
  EUNSUPPORTED: Unsupported format.

SIDE EFFECTS
  None

===========================================================================*/
int YCbCr_New(uint32 uScheme, uint32 cx, uint32 cy, IYCbCr **ppiYCbCr)
{
  YCbCr *me = NULL;
  uint32 cbBuf;

  if (!ppiYCbCr)
    return EBADPARM;

  *ppiYCbCr = 0;

  switch (uScheme)
  {
  case IYCBCR_COLORSCHEME_420LP:
    cbBuf = sizeof(YCbCr) + (cx * cy + cx * cy / 2);
    break;
  case IYCBCR_COLORSCHEME_422LP:
    cbBuf = sizeof(YCbCr) + (cx * cy * 2);
    break; 
  case IYCRCB_COLORSCHEME_420LP:
    cbBuf = sizeof(YCbCr) + (cx * cy + cx * cy / 2);
    break;
  case IYCRCB_COLORSCHEME_422LP:
    cbBuf = sizeof(YCbCr) + (cx * cy * 2);
    break;
  default:
    return EUNSUPPORTED;
  }

  me = (YCbCr *)MALLOC(cbBuf);
  if (!me) 
  {
    return ENOMEMORY;
  }

  me->bmp.pvt = (AEEVTBL(IBitmap) *)&gYCbCrFuncs;
  me->uRef = 1;
  me->rotate = YCBCR_ROTATION_NONE;
  me->scale  = YCBCR_SCALING_NONE;

  me->bmp.uScheme = uScheme;
  me->bmp.cx = cx;
  me->bmp.cy = cy;

  me->bmp.nYPitch = (int32)cx;
  me->bmp.nCPitch = (int32)cx;
  me->bmp.pLuma = me + 1; // just after YCbCr struct
  me->bmp.pChroma = (byte *)(me->bmp.pLuma) + (cx * cy); // just after Y plane

  *ppiYCbCr = (IYCbCr *)me;

  return SUCCESS;
}


/************************************************************************
 ** I N T E R F A C E   F U N C T I O N   D E F I N I T I O N S
 ************************************************************************/

static uint32 YCbCr_AddRef(IBitmap *po)
{
  YCbCr *me = (YCbCr *)po;
  if (!me)
    return 0;

  return ++me->uRef;
}


static uint32 YCbCr_Release(IBitmap *po)
{
  YCbCr *me = (YCbCr *)po;
  if (!me)
    return 0;

  if (--me->uRef != 0) 
  {
    return me->uRef;
  }

  FREE(me);
  return 0;
}


static int YCbCr_QueryInterface(IBitmap *po, AEECLSID clsid, void **ppNew)
{
  YCbCr *me = (YCbCr *)po;
  void *pif = NULL;

  if (!me || !ppNew)
    return EBADPARM;

  if (clsid == AEECLSID_QUERYINTERFACE || clsid == AEEIID_YCBCR ||
      clsid == AEECLSID_BITMAP)
  {
    pif = me;
    (void)YCbCr_AddRef(po);
  }

  *ppNew = pif;
  return (pif ? SUCCESS : ECLASSNOTSUPPORT);
}

//lint -save -e715 -e818
static NativeColor YCbCr_RGBToNative(IBitmap *po, RGBVAL rgb)
{
  return 0;
}

static RGBVAL YCbCr_NativeToRGB(IBitmap *po, NativeColor clr)
{
  return 0;
}

static int YCbCr_DrawPixel(IBitmap *po, unsigned int x, unsigned int y,
                           NativeColor color, AEERasterOp rop)
{
  return EUNSUPPORTED;
}

static int YCbCr_GetPixel(IBitmap *po, unsigned x, unsigned y, NativeColor *pColor)
{
  return EUNSUPPORTED;
}

static int YCbCr_SetPixels(IBitmap *po, unsigned int cnt, AEEPoint *pPoint, 
                           NativeColor color, AEERasterOp rop)
{
  return EUNSUPPORTED;
}

static int YCbCr_DrawHScanline(IBitmap *po, unsigned y, unsigned xMin, unsigned xMax,
                               NativeColor color, AEERasterOp rop)
{
  return EUNSUPPORTED;
}

static int YCbCr_FillRect(IBitmap *po, const AEERect *pRect, 
                          NativeColor color, AEERasterOp rop)
{
  return EUNSUPPORTED;
}

static int YCbCr_SetTransparencyColor(IBitmap *po, NativeColor color)
{
  return EUNSUPPORTED;
}

static int YCbCr_GetTransparencyColor(IBitmap *po, NativeColor *pColor)
{
  return EUNSUPPORTED;
}
//lint -restore

static int YCbCr_BltIn(IBitmap *po, int xDst, int yDst, int dx, int dy, 
                       IBitmap *pSrc, int xSrc, int ySrc, AEERasterOp rop)
{
  return IBITMAP_BltOut(pSrc, xDst, yDst, dx, dy, po, xSrc, ySrc, rop);
}

/*=====================================================================
FUNCTION YCbCr_GetDstDIB

  DESCRIPTION: This function get IDIB interface from pDst and does some checking.
               It assumes the output color scheme is 16 bit (565). It has to be updated
               if the display supports different color scheme.
  
  PARAMETERS:
    pDst[in]     -  Pointer to IBitmap interface
    pDstDIB[out] -  Pointer to return IDIB interface.

  RETURN VALUE:
    int Error code defined in AEEError.h
======================================================================*/
static int YCbCr_GetDstDIB(IBitmap *pDst, IDIB **ppDstDIB)
{
  IDIB *pDstTemp = NULL;

  if (!pDst || !ppDstDIB)
    return EBADPARM;

  // Get dst IDIB interface
  if (IBITMAP_QueryInterface(pDst, AEECLSID_DIB, (void **)&pDstTemp) != SUCCESS &&
      IBITMAP_QueryInterface(pDst, AEECLSID_DIB_20, (void **)&pDstTemp) != SUCCESS)
    return EUNSUPPORTED;

  // Check whether dst color scheme is supported
  if (pDstTemp->nColorScheme != IDIB_COLORSCHEME_565)
    return EUNSUPPORTED;

  // Check whether dst color depth is supported
  if (pDstTemp->nDepth != 16)
    return EUNSUPPORTED;

  // Check whether dst nPitch is supported
  // IPL cannot handle image bottom up
  if ((pDstTemp->cx * 2 != pDstTemp->nPitch) || (pDstTemp->nPitch < 0 )) 
    return EUNSUPPORTED;

  *ppDstDIB = pDstTemp;
  return SUCCESS;
}

#ifndef FEATURE_IPL_NO_CAMERA
/*===========================================================================

FUNCTION: YCbCr_BltOut_ipl

DESCRIPTION
  This function Blits out IYCbCr bitmap to RGB565 bitmap using IPL conversion.

DEPENDENCIES
  None

RETURN VALUE
  Returns AEE return values

SIDE EFFECTS
  None

===========================================================================*/
static int YCbCr_BltOut_ipl(IBitmap *po, int xDst, int yDst, int dx, int dy, 
                            IBitmap *pDst, int xSrc, int ySrc, AEERasterOp rop)
{
  IYCbCr *yc = (IYCbCr *)po;
  YCbCr  *me = (YCbCr *)po;
  IDIB   *pDstDIB = NULL;
  ipl_image_type  inImg;
  ipl_image_type  outImg;
  ipl_image_type  tempImg;
  ipl_rect_type   inCropRect;
  ipl_rect_type   outCropRect;
  ipl_rotate90_type outRot = IPL_NOROT;
  AEERect         outRect;
  int status = EFAILED;
  uint16    cxSrcTransform, cySrcTransform;

  if (!yc || !pDst)
    return EBADPARM;

  // Get IDIB pointer and check if it is valid 
  status = YCbCr_GetDstDIB(pDst, &pDstDIB);
  if (status != SUCCESS)
    goto Done;

  // Fill in input data for IPL
  switch (yc->uScheme)
  {
  case IYCBCR_COLORSCHEME_420LP:
    inImg.cFormat = IPL_YCbCr420_LINE_PK; // Is the uSchere wrong?
    break;                                // Could it be fixed elsewhere?
  case IYCBCR_COLORSCHEME_422LP:
    inImg.cFormat = IPL_YCbCr422_LINE_PK;
    break;
  case IYCRCB_COLORSCHEME_420LP:
    inImg.cFormat = IPL_YCrCb420_LINE_PK;
    break;
  case IYCRCB_COLORSCHEME_422LP:
    inImg.cFormat = IPL_YCrCb422_LINE_PK;
    break;
  default:
    status = EUNSUPPORTED;
    goto Done;
  }

  inImg.dx      = (uint32)yc->nYPitch;  // use nYPitch instead of cx
  inImg.dy      = yc->cy;
  inImg.imgPtr  = yc->pLuma;
  inImg.clrPtr  = yc->pChroma;


  cxSrcTransform = (uint16)yc->cx;
  cySrcTransform = (uint16)yc->cy;

  // Now that we have the original clip dimentions, scale & rotate them 
  // as needed.
  switch( me->scale )
  {
    case YCBCR_SCALING_0_25X:
      cxSrcTransform /= 4;
      cySrcTransform /= 4;
      break;
    case YCBCR_SCALING_0_5X:
      cxSrcTransform /= 2;
      cySrcTransform /= 2;
      break;
    case YCBCR_SCALING_0_75X:
      cxSrcTransform = cxSrcTransform * 3 / 4;
      cySrcTransform = cySrcTransform * 3 / 4;
      break;
    case YCBCR_SCALING_2X:
      cxSrcTransform *= 2;
      cySrcTransform *= 2;
      break; 
#ifdef MDPOP_ASCALE
    case YCBCR_SCALING_ASCALE:
      cxSrcTransform = (me->ascale.x / 2) * 2;
      cySrcTransform = (me->ascale.y / 2) * 2;
      break;
#endif
    case YCBCR_SCALING_NONE: /* fallthrough and do nothing */
    default:
      break;
  }
  switch( me->rotate )
  {
    uint16 t;
    case YCBCR_ROTATION_90CW:
      outRot = IPL_ROT90;
      t = cxSrcTransform;
      cxSrcTransform = cySrcTransform;
      cySrcTransform = t;
      break;
    case YCBCR_ROTATION_180:
      outRot = IPL_ROT180;
      break;
    case YCBCR_ROTATION_90CCW:
      outRot = IPL_ROT270;
      t = cxSrcTransform;
      cxSrcTransform = cySrcTransform;
      cySrcTransform = t;
      break;
    case YCBCR_ROTATION_NONE: /* fallthrough and do nothing */
    default:
      break;
  }

  // Refine crop rect
  CLIP_TO_SRC(xDst, yDst, xSrc, ySrc, cxSrcTransform, cySrcTransform, dx, dy);
  CLIP_TO_DST(xDst, yDst, pDstDIB->cx, pDstDIB->cy, xSrc, ySrc, dx, dy);

  // Color convert if crop rect is not empty
  if (dx > 0 && dy > 0)
  {
    uint16 *pTempRGBBuf = NULL;
    uint16 *pDstBuf = (void *)pDstDIB->pBmp;

    // Move pDstBuf to the start of the correct row
    pDstBuf += yDst * pDstDIB->cx;

    if( (cxSrcTransform != pDstDIB->cx) || xDst != 0 )
    {
      pTempRGBBuf = MALLOC( cxSrcTransform * cySrcTransform * 2 );
      if(!pTempRGBBuf)
      {
        status = EFAILED;
        goto Done;
      }
      pDstBuf = pTempRGBBuf;
    }

    // Fill in output data and cropping info
    outImg.dx      = cxSrcTransform;
    outImg.dy      = cySrcTransform;
    outImg.cFormat = IPL_RGB565;  //output display only supports RGB565
    outImg.imgPtr  = (void*)pDstBuf;
    outImg.clrPtr  = (void*)((uint32)pDstBuf - (uint32)((uint32)yc->pLuma - (uint32)yc->pChroma));

    inCropRect.dx  = yc->cx;
    inCropRect.dy  = yc->cy;
    inCropRect.x   = (uint32)xSrc;
    inCropRect.y   = (uint32)ySrc;

    outCropRect.dx = (uint32)dx;
    outCropRect.dy = (uint32)dy;
    outCropRect.x  = (uint32)xSrc;
    outCropRect.y  = (uint32)ySrc;

    // Convert YCbCr image buffer directly into display RGB565 buffer
    if(ipl_crop_resize_rot(&inImg, &outImg, &inCropRect, &outCropRect, 
                           outRot, IPL_QUALITY_MEDIUM) == IPL_SUCCESS)
    {
      IBitmap *pDstBmp = IDIB_TO_IBITMAP(pDstDIB);
      if (pTempRGBBuf)
      { 
        // Now we need to copy each row from the temp buffer into the destination
        // RGB buffer.
        int i;
        uint16 *pSrcBuf;
        if ((outRot != IPL_NOROT) && (me->scale == YCBCR_SCALING_NONE)) 
        {
// The crop_resize_rot function should be color-converting but isn't.
          uint16 *pTempRGBBuf2 = NULL;
          pTempRGBBuf2 = MALLOC( cxSrcTransform * cySrcTransform * 2 );
          tempImg.cFormat = outImg.cFormat;
          outImg.cFormat = inImg.cFormat;
          tempImg.dx = outImg.dx;
          tempImg.dy = outImg.dy;
          tempImg.imgPtr = (void*)pTempRGBBuf2;
          ipl_convert_image(&outImg, &tempImg);
          FREEIF(pTempRGBBuf);
          pTempRGBBuf = pTempRGBBuf2;
        }

        pSrcBuf = pTempRGBBuf;
        pDstBuf = (void *)pDstDIB->pBmp;
        // Move to the specified upper-left location
        pDstBuf += (yDst * pDstDIB->cx + xDst);

        // Copy each row accounting for the width of each buffer
        for( i = 0; i < dy; i++ )
        {
          // Copy dx * 16bpp
          memcpy( pDstBuf , pSrcBuf, (unsigned int)dx * 2 );
          // Move pDstBuf and pSrcBuf down a row
          pDstBuf += pDstDIB->cx;
          pSrcBuf += cxSrcTransform;
        }
      }
      SETAEERECT(&outRect, xDst, yDst, dx, dy);
      (void)IBITMAP_Invalidate(pDstBmp, &outRect);
      status = SUCCESS;
#ifndef FEATURE_CAMERA7500
 /* Ensure that this is not taken for 7k and 8k */
      JPEG_FLUSH_DATA_CACHE(outImg.imgPtr, (outImg.dx * outImg.dy * 2));  //output display only supports RGB565
#endif
    }
    FREEIF(pTempRGBBuf);
  }
  	
Done:
  if (pDstDIB)
  {
    (void)IDIB_Release(pDstDIB); 
    pDstDIB = NULL;
  }
  return status; 
  //lint -save -e715 ymbol 'rop' not referenced
}
#endif /*FEATURE_IPL_NO_CAMERA */
//lint -restore
/*===========================================================================

FUNCTION: YCbCr_BltOut_mdp

DESCRIPTION
  This function blits out IYCbCr bitmap to RGB565 bitmap using mdp hardware.

DEPENDENCIES
  None

RETURN VALUE
  Returns AEE return values

SIDE EFFECTS
  None

===========================================================================*/
#ifndef FEATURE_CAMERA8X50
#ifdef FEATURE_MDP2
#error code not present
#endif /* FEATURE_MDP2 */
#endif /*!FEATURE_CAMERA8X50*/

/*===========================================================================

FUNCTION: YCbCr_BltOut

DESCRIPTION
  This function blits out IYCbCr bitmap to supported screen formats 

DEPENDENCIES
  None

RETURN VALUE
  Returns AEE return values

SIDE EFFECTS
  None

===========================================================================*/
static int YCbCr_BltOut(IBitmap *po, int xDst, int yDst, int dx, int dy, 
                        IBitmap *pDst, int xSrc, int ySrc, AEERasterOp rop)
{
  IYCbCr *yc = (IYCbCr *)po;
  int retval = EUNSUPPORTED;

  if (!yc)
    return EBADPARM;
#ifndef FEATURE_CAMERA8X50
#ifdef FEATURE_MDP2
#error code not present
#endif /* FEATURE_MDP */
#endif /*!FEATURE_CAMERA8X50*/
#ifndef FEATURE_IPL_NO_CAMERA 
    retval = YCbCr_BltOut_ipl(po, xDst, yDst, dx, dy, pDst, xSrc, ySrc, rop);
#endif /* FEATURE_IPL_NO_CAMERA*/

  return retval;
}

/*===========================================================================

FUNCTION: YCbCr_GetInfo

DESCRIPTION
  This function returns YCbCr bitmap info. 

DEPENDENCIES
  None

RETURN VALUE
  Returns AEE return values

SIDE EFFECTS
  None

===========================================================================*/
static int YCbCr_GetInfo(IBitmap *po, AEEBitmapInfo *pInfo, int nSize)
{
  YCbCr *me = (YCbCr *)po;

  if (!me || !pInfo)
    return EBADPARM;

  if (nSize != (int)sizeof(*pInfo))
    return EUNSUPPORTED;

  pInfo->cx = me->bmp.cx;
  pInfo->cy = me->bmp.cy;
  pInfo->nDepth = 0;
#ifdef FEATURE_MDP
  // Now scale those dimensions appropriately
  switch( me->scale )
  {
    case YCBCR_SCALING_0_25X:
      pInfo->cx /= 4;
      pInfo->cy /= 4;
      break;
    case YCBCR_SCALING_0_5X:
      pInfo->cx /= 2;
      pInfo->cy /= 2;
      break;
    case YCBCR_SCALING_0_75X:
      pInfo->cx = pInfo->cx * 3 / 4;
      pInfo->cy = pInfo->cy * 3 / 4;
      break;
    case YCBCR_SCALING_2X:
      pInfo->cx *= 2;
      pInfo->cy *= 2;
      break; 
    case YCBCR_SCALING_ASCALE:
      pInfo->cx = me->ascale.x;
      pInfo->cy = me->ascale.y;
      return SUCCESS;
    case YCBCR_SCALING_NONE: /* fallthrough and do nothing */
    default:
      break;
  }
  // Rotate them if necessary
  switch( me->rotate )
  {
    uint16 t;
    case YCBCR_ROTATION_90CW:
    case YCBCR_ROTATION_90CCW:
      t= pInfo->cx;
      pInfo->cx = pInfo->cy;
      pInfo->cy = t;
      break;
    default:
      break;
  }
#endif /* FEATURE_MDP */

  return SUCCESS;
}

static int YCbCr_CreateCompatibleBitmap(IBitmap *po, IBitmap **ppIBitmap, 
                                        uint16 w, uint16 h)
{
  YCbCr *me = (YCbCr *)po;
  if (!me || !ppIBitmap)
    return EBADPARM;

  return YCbCr_New(me->bmp.uScheme, w, h, (IYCbCr **)ppIBitmap);
}



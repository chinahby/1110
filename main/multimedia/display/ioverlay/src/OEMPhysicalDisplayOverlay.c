/*
================================================================================

  FILE:  OEMPhysicalDisplay.c
  
  SERVICES: Proviced Physical Display services

  GENERAL DESCRIPTION:

================================================================================
================================================================================
    
               Copyright © 1999-2005 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
$Header: //source/qcom/qct/multimedia/display/ioverlay/main/latest/src/OEMPhysicalDisplayOverlay.c#8 $
$Author: jbrasen $
$DateTime: 2008/07/16 17:09:18 $

================================================================================
================================================================================
*/
#include "OEMFeatures.h"

#ifdef FEATURE_MMOVERLAY

#include "AEE.h"
#include "AEE_OEMDispatch.h"
#include "AEEStdLib.h"

#include "AEE_OEM.h"
#include "AEEBase.h"
#include "AEEPhysicalDisplay.h"
#include "AEECLSID_PHYSICALDISPLAY.BID"
#include "OEMPhysicalDisplay_Priv.h"

#include "OEMOverlay_priv.h"

#include "mdp_drv_api.h"
#include "mdp_drv.h"
#include "mdp_sys_api.h"
#include "cache_mmu.h"

#include "AEEPhysicalDisplayOverlay.h"
#include "AEEMMOverlay.h"
#include "AEEModel.h"
#include "OEMOverlayHelper.h"
#include "IYCbCr.h"
#include "YCbCr_priv.h"
#include "AEEBitmap.h"
#include "AEEHandle.h"

#ifndef FEATURE_UIONE_HDK
#include "AEEAnnunciator.h"
#endif

static AEEInstance gpInstances[MDP_MAX_IMGLIST * 2];
static AEEInstanceList gHandleList = { gpInstances,
                       sizeof(gpInstances)/sizeof(gpInstances[0]), 0 };


////////////////////////////////////////////////////////////////////////////////
// PhysicalDisplay data structures

typedef struct {
   uint32 dwMDPID;
   MDPIMG MDPImage;    //Current MDP Parameters
   IMMOverlay *pOverlay;
   IBitmap *pRegBitmap;
   IBitmap *pSetBitmap;
   IPhysicalDisplayOverlay *pPhysical;
   ModelListener listener;
   boolean bRequireDeregister;
   boolean bNeedReregister;
   uint32 scaleFlagsX, scaleFlagsY;
   uint32 alignmentFlags;
   AEERect rcSetSrc;
   boolean bEdgeScale;
   boolean bIsEnabled;
   uint32 dwBitmapScaleX, dwBitmapScaleY;
   uint32 dwBitmapRotate;
   uint32 dwOverlayRotate;
   boolean bReflectX, bReflectY;
} OverlayMDPType;

/*==============================================================================
Physical Display Helper Function Implementation
==============================================================================*/
/*======================================================================= 
=======================================================================*/
static void PhysicalDisplayOverlayModel_Notify( IModel *po, ModelEvent *pEvent )
{
   IPhysicalDisplayOverlayModel *pMe = (IPhysicalDisplayOverlayModel *)po;

   (void) IPHYSICALDISPLAYOVERLAY_AddRef(pMe->pMe);

   OverlayHelper_NotifyListeners( pMe->pMe->pListeners, pEvent );

   (void) IPHYSICALDISPLAYOVERLAY_Release(pMe->pMe);

}

#ifdef RELEASEIF
#undef RELEASEIF
#endif
#define RELEASEIF(p) \
   if (p != NULL) { (void)IBASE_Release((void*)(p)); (p) = NULL; }


/*==============================================================================
==============================================================================*/
int modifyBitmap(OverlayMDPType *pMDPInfo, MDPIMG *pImage, IBitmap *pBitmap)
{
   if(pImage == NULL)
   {
      return EFAILED;
   }

   if(pBitmap != NULL)
   {
      IDIB * pDIB;
      IYCbCr * pIYCbCr;
      if((IBITMAP_QueryInterface(pBitmap, AEECLSID_DIB, 
         (void **) &pDIB) == SUCCESS) && (pDIB != NULL))
      {
         if(pDIB->nColorScheme == IDIB_COLORSCHEME_565)
         {
            pImage->imgType = MDP_RGB_565;
         }
#ifdef FEATURE_MDP2
#error code not present
#endif
         else
         {
            (void) IDIB_Release(pDIB);
            return EUNSUPPORTED;
         }

         pImage->bmy_addr = (void *) pDIB->pBmp;
         pImage->cbcr_addr = NULL;
         pImage->width = (uint16) pDIB->nPitch /
            (pDIB->nDepth / 8);
         
         //Set the internal bitmap scale and rotate parameters
         pMDPInfo->dwBitmapScaleX = 256; 
         pMDPInfo->dwBitmapScaleY = 256; 
         pMDPInfo->dwBitmapRotate = 0;
         (void) IDIB_Release(pDIB);
      }
      else if((IBITMAP_QueryInterface(pBitmap, AEEIID_YCBCR,
         (void **) &pIYCbCr) == SUCCESS) && (pIYCbCr != NULL))
      {
         YCbCr * pYCbCr = (YCbCr *)pIYCbCr;
         if(pIYCbCr->uScheme == IYCBCR_COLORSCHEME_420LP)
         {
            pImage->imgType = MDP_Y_CBCR_H2V2;
         }
#ifdef FEATURE_MDP2
#error code not present
#endif
         else
         {
            (void) IYCBCR_Release(pIYCbCr);
            return EUNSUPPORTED;
         }
         pImage->bmy_addr = (uint32 *) pIYCbCr->pLuma;
         pImage->cbcr_addr = (uint32 *) pIYCbCr->pChroma;
         pImage->width = (uint16) pIYCbCr->cx;
         
         //Set the internal bitmap scale and rotate parameters
         switch(pYCbCr->rotate)
         {
            case YCBCR_ROTATION_NONE:
            {
               pMDPInfo->dwBitmapRotate = 0;
               break;
            }

            case YCBCR_ROTATION_90CW:
            {
               pMDPInfo->dwBitmapRotate = 90;
               break;
            }

            case YCBCR_ROTATION_180:
            {
               pMDPInfo->dwBitmapRotate = 180;
               break;
            }

            case YCBCR_ROTATION_90CCW:
            {
               pMDPInfo->dwBitmapRotate = 270;
               break;
            }
         }

         switch(pYCbCr->scale)
         {
            case YCBCR_SCALING_0_25X:
            {
               pMDPInfo->dwBitmapScaleX = 64; 
               pMDPInfo->dwBitmapScaleY = 64;               
               break;
            }

            case YCBCR_SCALING_0_5X:
            {
               pMDPInfo->dwBitmapScaleX = 128; 
               pMDPInfo->dwBitmapScaleY = 128;
               break;
            }

            case YCBCR_SCALING_0_75X:
            {
               pMDPInfo->dwBitmapScaleX = 192; 
               pMDPInfo->dwBitmapScaleY = 192;
               break;
            }
            case YCBCR_SCALING_2X:
            {
               pMDPInfo->dwBitmapScaleX = 512; 
               pMDPInfo->dwBitmapScaleY = 512;
               break;
            }
            default:
            case    YCBCR_SCALING_NONE:
            {
               pMDPInfo->dwBitmapScaleX = 256; 
               pMDPInfo->dwBitmapScaleY = 256;
               break;
            }
         }
         
         (void) IYCBCR_Release(pIYCbCr);
      }
   }
   else
   {
      return EFAILED;
   }

   return SUCCESS;
}

/*==============================================================================
==============================================================================*/
int modifySourceRegion(MDPIMG *pImage, const AEERect *pRec)
{
   if(pImage == NULL)
   {
      return EFAILED;
   }

   if(pRec != NULL)
   {
      pImage->cwin.x1 = pRec->x;
      pImage->cwin.y1 = pRec->y;
      pImage->cwin.x2 = pRec->x + pRec->dx -1;
      pImage->cwin.y2 = pRec->y + pRec->dy -1;
   }
   else
   {
      return EFAILED;
   }
   return SUCCESS;
}


/*==============================================================================
==============================================================================*/
int modifyRotateAndReflect(MDPIMG *pImage, uint32 dwRotateAngle,
                           boolean bReflectX, boolean bReflectY)
{
   if(pImage == NULL)
   {
      return EFAILED;
   }

   pImage->mdpOp &= ~(MDPOP_ROT90|MDPOP_UD|MDPOP_LR);

   switch(dwRotateAngle)
   {
      case 0:
         break;

      case 90:
         pImage->mdpOp |= MDPOP_ROT90;
         break;

      case 180:
         pImage->mdpOp |= MDPOP_ROT180;
         break;

      case 270:
         pImage->mdpOp |= MDPOP_ROT270;
         break;
      
      default:
         return EBADPARM;
   }

   if(bReflectX)
   {
      pImage->mdpOp ^= MDPOP_UD;
   }

   if(bReflectY)
   {
      pImage->mdpOp ^= MDPOP_LR;
   }

   return SUCCESS;
}

/*==============================================================================
==============================================================================*/
int modifyDestinationRegion(MDPIMG *pImage, 
                            const AEERect *pRecImg,
                            const OverlayMDPType *pMDPInfo)
{
   //Calcuate the destination region for this overlay.
   AEERect destRegion;
   AEERect clippingRegion = *pRecImg;
   AEERect srcRegion = pMDPInfo->rcSetSrc;
   uint32 dwScaleX, dwScaleY;
   AEEBitmapInfo bi;

   destRegion.x = 0;
   destRegion.y = 0;
   destRegion.dx = 0;
   destRegion.dy = 0;

   if(pImage == NULL)
   {
      return EFAILED;
   }

   if(pRecImg == NULL)
   {
      return EFAILED;
   }

   if(((pMDPInfo->dwOverlayRotate/ 90) % 2) != 1)
   {
      dwScaleX = (uint32)(((int32)pRecImg->dx *256) / (int32)srcRegion.dx);
      dwScaleY = (uint32)(((int32)pRecImg->dy *256) / (int32)srcRegion.dy);
   }
   else
   {
      dwScaleX = (uint32)(((int32)pRecImg->dx *256) / (int32)srcRegion.dy);
      dwScaleY = (uint32)(((int32)pRecImg->dy *256) / (int32)srcRegion.dx);
   }

   (void) OverlayHelper_CalculateScaleFactor(pMDPInfo->pOverlay,
                                             &srcRegion, &clippingRegion,
                                             (boolean)(((pMDPInfo->dwOverlayRotate/ 90) % 2) == 1),
                                             FALSE, &dwScaleX, &dwScaleY,
                                             pMDPInfo->scaleFlagsX, pMDPInfo->scaleFlagsX);

   (void) modifyRotateAndReflect(pImage, (pMDPInfo->dwOverlayRotate + pMDPInfo->dwBitmapRotate) % 360, pMDPInfo->bReflectX, pMDPInfo->bReflectY);

   //Modify the scale factor
   //Need to unrotate the source region
   if(((pMDPInfo->dwBitmapRotate / 90) % 2) == 1)
   {
      uint32 dwTempScale = dwScaleX;
      dwScaleX = dwScaleY;
      dwScaleY = dwTempScale;
   }

   if((((pMDPInfo->dwOverlayRotate + pMDPInfo->dwBitmapRotate) / 90) % 2) == 0)
   {
      dwScaleX = (dwScaleX * pMDPInfo->dwBitmapScaleX)/256;
      dwScaleY = (dwScaleY * pMDPInfo->dwBitmapScaleY)/256;
   }
   else
   {
      dwScaleX = (dwScaleX * pMDPInfo->dwBitmapScaleY)/256;
      dwScaleY = (dwScaleY * pMDPInfo->dwBitmapScaleX)/256;
   }

   if(SUCCESS == IBITMAP_GetInfo(pMDPInfo->pSetBitmap, &bi, sizeof(AEEBitmapInfo)))
   {
      AEEPoint orig1, orig2;
      AEEPoint newPt1 = {0,0};
      AEEPoint newPt2 = {0,0};
      //int16 xCenter, yCenter;

      orig1.x = srcRegion.x;
      orig1.y = srcRegion.y;
      orig2.x = srcRegion.x + srcRegion.dx - 1;
      orig2.y = srcRegion.y + srcRegion.dy - 1;

      //xCenter = (bi.cx-1)/2;
      //yCenter = (bi.cy-1)/2;

      //newPt1.x = (costheta * (orig1.x - xCenter)) - (sintheta * (orig1.y - yCenter)) + abs(sintheta * yCenter + costheta * xCenter);
      //newPt1.y = (sintheta * (orig1.x - xCenter)) + costheta * ((orig1.y - yCenter)) + abs(costheta * yCenter + sintheta * xCenter);

      //newPt2.x = (costheta * (orig2.x - xCenter)) - (sintheta * (orig2.y - yCenter)) + abs(sintheta * yCenter + costheta * xCenter);
      //newPt2.y = (sintheta * (orig2.x - xCenter)) + costheta * ((orig2.y - yCenter)) + abs(costheta * yCenter + sintheta * xCenter);

      if(pMDPInfo->dwBitmapRotate == 0)
      {
         //sintheta = 0;
         //costheta = 1;
         newPt1.x = orig1.x;
         newPt1.y = orig1.y;

         newPt2.x = orig2.x;
         newPt2.y = orig2.y;

      }
      else if(pMDPInfo->dwBitmapRotate == 90)
      {
         //sintheta = -1;
         //costheta = 0;
         newPt1.x = orig1.y;
         newPt1.y = ((int16)bi.cx - 1) - orig1.x;

         newPt2.x = orig2.y;
         newPt2.y = ((int16)bi.cx - 1) - orig2.x;

      }
      else if(pMDPInfo->dwBitmapRotate == 180)
      {
         //sintheta = 0;
         //costheta = -1;
         newPt1.x = ((int16)bi.cx - 1) - orig1.x;
         newPt1.y = ((int16)bi.cy - 1) - orig1.y;

         newPt2.x = ((int16)bi.cx - 1) - orig2.x;
         newPt2.y = ((int16)bi.cy - 1) - orig2.y;
      }
      else if(pMDPInfo->dwBitmapRotate == 270)
      {
         //sintheta = 1;
         //costheta = 0;
         newPt1.x = orig1.y;
         newPt1.y = orig1.x;

         newPt2.x = orig2.y;
         newPt2.y = orig2.x;
      }

      if(newPt1.x < newPt2.x)
      {
         srcRegion.x = newPt1.x;
         srcRegion.dx = (newPt2.x - newPt1.x) + 1;
      }
      else
      {
         srcRegion.x = newPt2.x;
         srcRegion.dx = (newPt1.x - newPt2.x) + 1;
      }

      if(newPt1.y < newPt2.y)
      {
         srcRegion.y = newPt1.y;
         srcRegion.dy = (newPt2.y - newPt1.y) + 1;
      }
      else
      {
         srcRegion.y = newPt2.y;
         srcRegion.dy = (newPt1.y - newPt2.y) + 1;
      }
   }

   //Remove the scale factor
   srcRegion.x  = (int16)(((int32)srcRegion.x * 256)/(int32)pMDPInfo->dwBitmapScaleX);
   srcRegion.dx = (int16)(((int32)srcRegion.dx* 256)/(int32)pMDPInfo->dwBitmapScaleX);
   srcRegion.y  = (int16)(((int32)srcRegion.y * 256)/(int32)pMDPInfo->dwBitmapScaleY);
   srcRegion.dy = (int16)(((int32)srcRegion.dy* 256)/(int32)pMDPInfo->dwBitmapScaleY);

   //Calculate Clipping Region
   (void) OverlayHelper_CalculateSrcDest(&srcRegion, &destRegion, &clippingRegion,
                                          dwScaleX, dwScaleY, 
                                         (boolean)((pImage->mdpOp & MDPOP_ROT90) != 0), 
                                         pMDPInfo->alignmentFlags);

   (void) modifySourceRegion(pImage, &srcRegion);


   pImage->lcd_x = destRegion.x - BREW_OFFSET_X;
   pImage->lcd_y = destRegion.y - BREW_OFFSET_Y;

   pImage->mdpOp &= ~(MDPOP_VDO_X2|MDPOP_VDO_1BY4|MDPOP_VDO_1BY2|MDPOP_VDO_3BY4|MDPOP_ASCALE);
#ifdef FEATURE_MDP_WSCALE
   pImage->mdpOp &= ~(MDPOP_WSCALE_EDGE_STRETCH|MDPOP_WSCALE_LINEAR_STRETCH|MDPOP_WSCALE_NO_STRETCH);
#endif

   if((pImage->mdpOp & MDPOP_ROT90))
   {
      pImage->ascale.width  = (uint16)destRegion.dy;
      pImage->ascale.height = (uint16)destRegion.dx;
   }
   else
   {
      pImage->ascale.width  = (uint16)destRegion.dx;
      pImage->ascale.height = (uint16)destRegion.dy;
   }

   {
      boolean bScaleSet = FALSE;
      if((pImage->mdpOp & MDPOP_ROT90))
      {
         if(((int16)destRegion.dy  == 
             ((pImage->cwin.x2 - pImage->cwin.x1) + 1)) && 
            ((int16)destRegion.dx == 
             ((pImage->cwin.y2 - pImage->cwin.y1) + 1)))
         {
            bScaleSet = TRUE;
         }
      }
      else
      {
         if(((int16)destRegion.dx  == 
             ((pImage->cwin.x2 - pImage->cwin.x1) + 1)) && 
            ((int16)destRegion.dy == 
             ((pImage->cwin.y2 - pImage->cwin.y1) + 1)))
         {
            bScaleSet = TRUE;
         }
      }
#if !defined(FEATURE_MDP2) 
      if(!bScaleSet)
      {
         if(pImage->imgType != MDP_RGB_565)
         {
            if((dwScaleX == 64) &&
               (dwScaleY == 64))
            {
               pImage->mdpOp |= MDPOP_VDO_1BY4;
               bScaleSet = TRUE;
            }
            else if((dwScaleX == 128) &&
                    (dwScaleY == 128))
            {
               pImage->mdpOp |= MDPOP_VDO_1BY2;
               bScaleSet = TRUE;
            }
            else if((dwScaleX == 192) &&
                     (dwScaleY == 192))
            {
               pImage->mdpOp |= MDPOP_VDO_3BY4;
               bScaleSet = TRUE;
            }
            else if((dwScaleX == 512) &&
                    (dwScaleY == 512))
            {
               if(destRegion.dx <= 352)
               {
                  pImage->mdpOp |= MDPOP_VDO_X2;
                  bScaleSet = TRUE;
               }
            }
         }
      }
#ifdef FEATURE_MDP_WSCALE
      if(!bScaleSet)
      {
         if((destRegion.x == 0) &&
            (destRegion.y == 0) &&
            (((destRegion.dx == 400) &&
              (destRegion.dy == 240)) ||
             (destRegion.dx == 240) &&
             (destRegion.dy == 400)))
         {
            if(((srcRegion.dx == 320) &&
                (srcRegion.dy == 240)) ||
               ((srcRegion.dx == 240) &&
                (srcRegion.dy == 320)))
            {
               if(pMDPInfo->bEdgeScale)
               {
                  pImage->mdpOp |= MDPOP_WSCALE_EDGE_STRETCH;
               }
               else
               {
                  pImage->mdpOp |= MDPOP_WSCALE_LINEAR_STRETCH;
               }

               bScaleSet = TRUE;
            }
            else if(((srcRegion.dx == 320) &&
                     (srcRegion.dy == 192)) ||
                    ((srcRegion.dx == 192) &&
                     (srcRegion.dy == 320)))
            {
               pImage->mdpOp |= MDPOP_WSCALE_NO_STRETCH;
               bScaleSet = TRUE;
            }
         }
      }
#endif //defined FEATURE_MDP_WSCALE
#endif //!defined(FEATURE_MDP2)
      if(!bScaleSet)
      {
         pImage->mdpOp |= (MDPOP_ASCALE);
      }
   }
   return SUCCESS;

}

/*==============================================================================
==============================================================================*/
int modifyAlpha(MDPIMG *pImage, uint32 dwAlpha)
{
   if(pImage == NULL)
   {
      return EFAILED;
   }

   pImage->mdpOp &= ~(MDPOP_ALPHAB);

   if(dwAlpha != 256)
   {
      pImage->mdpOp |= MDPOP_ALPHAB;
   }

   pImage->alpha = dwAlpha;

   return SUCCESS;
}

/*==============================================================================
==============================================================================*/
int modifyTransparency(MDPIMG *pImage, boolean bTransp, uint32 nativeClr)
{
   if(pImage == NULL)
   {
      return EFAILED;
   }

   pImage->mdpOp &= ~(MDPOP_TRANSP);

   if(bTransp)
   {
      pImage->mdpOp |= MDPOP_TRANSP;
      pImage->tpVal = nativeClr;
   }

   return SUCCESS;
}

/*==============================================================================
==============================================================================*/
int modifyLayer(MDPIMG *pImage, uint32 dwLayer)
{
   if(pImage == NULL)
   {
      return EFAILED;
   }

   if(dwLayer == OV_LAYER_BASE_LAYER)
   {
      pImage->layer = MDP_LAYER_0;
   }
#ifdef FEATURE_MDP2
#error code not present
#else
   else if (dwLayer == 0x7F)
   {
      pImage->layer = MDP_LAYER_1;
   }
   else if (dwLayer == OV_LAYER_TOP_LAYER)
   {
      pImage->layer = MDP_LAYER_2;
   }
#endif
   else
   {
      return EFAILED;
   }

   return SUCCESS;

}

/*==============================================================================
==============================================================================*/
static void OverlayListenerCB(void * pData, const ModelEvent * pEvent)
{
   OverlayMDPType *pMDPInfo = (OverlayMDPType *) pData;
   switch (pEvent->evCode)
   {
      case EVT_MDL_OV_CHANGES_COMMITTED:
      //Update the mdp
      {
         MDPIMG *pImage = &(pMDPInfo->MDPImage);
         uint32 dwNewID = 0;
         
         if(pMDPInfo->bRequireDeregister)
         {
            (void) mdp_lock_ovimg(pMDPInfo->dwMDPID, FALSE);
            if(IPHYSICALDISPLAY_IsEnabled(pMDPInfo->pPhysical->pMe) && pMDPInfo->bIsEnabled)
            {
               mdp_clear_ovimg(pMDPInfo->dwMDPID);
            }
            mdp_deregister_ovimg(pMDPInfo->dwMDPID);
            pMDPInfo->dwMDPID = 0;
            pMDPInfo->bRequireDeregister = FALSE;
            pMDPInfo->bNeedReregister = TRUE;
         }

         if(pMDPInfo->bNeedReregister == TRUE)
         {
            RELEASEIF(pMDPInfo->pRegBitmap);
            pMDPInfo->pRegBitmap = pMDPInfo->pSetBitmap;
            (void) IBITMAP_AddRef(pMDPInfo->pRegBitmap);

            dwNewID = mdp_replace_ovimg(pImage, pMDPInfo->dwMDPID);

            if(dwNewID != 0)
            {
               pMDPInfo->dwMDPID = dwNewID;
               (void) mdp_lock_ovimg(pMDPInfo->dwMDPID, TRUE);
               pMDPInfo->bNeedReregister = FALSE;
            }
         }
         return;
      }
      case EVT_MDL_OV_PARAMETER_CHANGED:
      {
         OverlayParamChangedType *pParam = (OverlayParamChangedType *)pEvent->dwParam;
         MDPIMG *pImage = &(pMDPInfo->MDPImage);
         pMDPInfo->bNeedReregister = TRUE;
         switch(pParam->nParamID)
         {
            case OV_PARM_ENABLE:
            {
               if(mdp_is_ovimg_valid(pMDPInfo->dwMDPID))
               {
                  mdp_enable_locked_ovimg(pMDPInfo->dwMDPID, (int) pParam->p1);
                  if((pParam->p1 == 0) && pMDPInfo->bIsEnabled)
                  {
                     if(IPHYSICALDISPLAY_IsEnabled(pMDPInfo->pPhysical->pMe)&& pMDPInfo->bIsEnabled)
                     {
                         mdp_clear_ovimg(pMDPInfo->dwMDPID);
                     }
                  }
                  pMDPInfo->bIsEnabled = (pParam->p1 == 1);
               }
               break;
            }
            case OV_PARM_BITMAP:
            {
               uint32 oldScaleX, oldScaleY;
               uint32 oldRotate;
               IBitmap *pBitmap = (IBitmap *)pParam->p1;


               RELEASEIF(pMDPInfo->pSetBitmap);
               pMDPInfo->pSetBitmap = pBitmap;
               (void) IBITMAP_AddRef(pMDPInfo->pSetBitmap);

               oldScaleX = pMDPInfo->dwBitmapScaleX;
               oldScaleY = pMDPInfo->dwBitmapScaleY;
               oldRotate = pMDPInfo->dwBitmapRotate;

               (void) modifyBitmap(pMDPInfo, pImage, pBitmap);

               if((oldScaleX != pMDPInfo->dwBitmapScaleX) ||
                  (oldScaleY != pMDPInfo->dwBitmapScaleY) ||
                  (oldRotate != pMDPInfo->dwBitmapRotate))
               {
                  AEERect rect = {0,0,0,0};
                  AEERect rcScreen;
                  uint32 p1, p2;

                  p1 = (uint32) &rect;
                  if(SUCCESS == IMMOVERLAY_GetParam(pMDPInfo->pOverlay, OV_PARM_DEST_REGION, &p1, &p2))
                  {
                     pMDPInfo->alignmentFlags = p2;
                     p1 = (uint32) &rect;
                     p2 = (uint32) &rcScreen;
                     if(SUCCESS == IMMOVERLAY_GetParam(pMDPInfo->pOverlay, OV_PARM_DEST_REGION_SCREEN, &p1, &p2))
                     {
                        (void) modifyDestinationRegion(pImage, &rect, pMDPInfo);
                     }
                  }
               }

               break;
            }
            case OV_PARM_SRC_REGION:
            {
               uint32 p1, p2;
               AEERect rect = {0,0,0,0};
               AEERect rcScreen;
               AEERect *pRec = (AEERect *)pParam->p1;
               pMDPInfo->rcSetSrc = *pRec;

               p1 = (uint32) &rect;
               if(SUCCESS == IMMOVERLAY_GetParam(pMDPInfo->pOverlay, OV_PARM_DEST_REGION, &p1, &p2))
               {
                  pMDPInfo->alignmentFlags = p2;
                  p1 = (uint32) &rect;
                  p2 = (uint32) &rcScreen;
                  if(SUCCESS == IMMOVERLAY_GetParam(pMDPInfo->pOverlay, OV_PARM_DEST_REGION_SCREEN, &p1, &p2))
                  {
                     (void) modifyDestinationRegion(pImage, &rect, pMDPInfo);
                  }
               }

               break;
            }
            case OV_PARM_DEST_REGION:
            {
               uint32 p1, p2;
               AEERect rcRegion = {0,0,0,0};
               AEERect rcScreen;
               p1 = (uint32) &rcRegion;
               p2 = (uint32) &rcScreen;
               pMDPInfo->alignmentFlags = pParam->p2; 


               if(SUCCESS == IMMOVERLAY_GetParam(pMDPInfo->pOverlay, OV_PARM_DEST_REGION_SCREEN, &p1, &p2))
               {
                  (void) modifyDestinationRegion(pImage, &rcRegion, pMDPInfo);
               }
               break;
            }
            case OV_PARM_ROTATE:
            {
               uint32 p1, p2;
               AEERect rect  = {0,0,0,0};
               AEERect rcScreen;
               pMDPInfo->dwOverlayRotate = pParam->p1;

               p1 = (uint32) &rect;
               if(SUCCESS == IMMOVERLAY_GetParam(pMDPInfo->pOverlay, OV_PARM_DEST_REGION, &p1, &p2))
               {
                  pMDPInfo->alignmentFlags = p2;
                  p1 = (uint32) &rect;
                  p2 = (uint32) &rcScreen;
                  if(SUCCESS == IMMOVERLAY_GetParam(pMDPInfo->pOverlay, OV_PARM_DEST_REGION_SCREEN, &p1, &p2))
                  {
                     (void) modifyDestinationRegion(pImage, &rect, pMDPInfo);
                  }
               }

               break;
            }
            case OV_PARM_REFLECT:
            {
               pMDPInfo->bReflectX = (0 != (pParam->p1 & OV_FLAG_REFLECT_X_AXIS));
               pMDPInfo->bReflectY = (0 != (pParam->p1 & OV_FLAG_REFLECT_Y_AXIS));

               (void) modifyRotateAndReflect(pImage, (pMDPInfo->dwOverlayRotate + pMDPInfo->dwBitmapRotate) % 360, pMDPInfo->bReflectX, pMDPInfo->bReflectY);

               break;
            }
            case OV_PARM_SCALE_X:
            {
               uint32 p1, p2;
               AEERect rect  = {0,0,0,0};
               AEERect rcScreen;
               pMDPInfo->scaleFlagsX = pParam->p1; 

               p1 = (uint32) &rect;
               if(SUCCESS == IMMOVERLAY_GetParam(pMDPInfo->pOverlay, OV_PARM_DEST_REGION, &p1, &p2))
               {
                  pMDPInfo->alignmentFlags = p2;
                  p1 = (uint32) &rect;
                  p2 = (uint32) &rcScreen;
                  if(SUCCESS == IMMOVERLAY_GetParam(pMDPInfo->pOverlay, OV_PARM_DEST_REGION_SCREEN, &p1, &p2))
                  {
                     (void) modifyDestinationRegion(pImage, &rect, pMDPInfo);
                  }
               }

               break;
            }
            case OV_PARM_SCALE_Y:
            {
               uint32 p1, p2;
               AEERect rect = {0,0,0,0};
               AEERect rcScreen;
               pMDPInfo->scaleFlagsY = pParam->p1; 
               
               p1 = (uint32)  &rect;
               if(SUCCESS == IMMOVERLAY_GetParam(pMDPInfo->pOverlay, OV_PARM_DEST_REGION, &p1, &p2))
               {
                  pMDPInfo->alignmentFlags = p2;
                  p1 = (uint32) &rect;
                  p2 = (uint32) &rcScreen;
                  if(SUCCESS == IMMOVERLAY_GetParam(pMDPInfo->pOverlay, OV_PARM_DEST_REGION_SCREEN, &p1, &p2))
                  {
                     (void) modifyDestinationRegion(pImage, &rect, pMDPInfo);
                  }
               }
               
               break;
            }
            case OV_PARM_EDGE_SCALING:
            {
               uint32 p1, p2;
               AEERect rect  = {0,0,0,0};
               AEERect rcScreen;
               pMDPInfo->bEdgeScale = (boolean)pParam->p1;

               p1 = (uint32) &rect;
               if(SUCCESS == IMMOVERLAY_GetParam(pMDPInfo->pOverlay, OV_PARM_DEST_REGION, &p1, &p2))
               {
                  pMDPInfo->alignmentFlags = p2;
                  p1 = (uint32) &rect;
                  p2 = (uint32) &rcScreen;
                  if(SUCCESS == IMMOVERLAY_GetParam(pMDPInfo->pOverlay, OV_PARM_DEST_REGION_SCREEN, &p1, &p2))
                  {
                     (void) modifyDestinationRegion(pImage, &rect, pMDPInfo);
                  }
               }

               break;
            }
            case OV_PARM_ALPHA_BLEND_PCT:
            {
               (void) modifyAlpha(pImage, pParam->p1);

               break;
            }
            case OV_PARM_TRANSPARENCY:
            {
               uint32 p1, p2;
               boolean bTransp = pParam->p1 == 1;
               RGBVAL rgb = pParam->p2;

               if(SUCCESS == IMMOVERLAY_GetParam(pMDPInfo->pOverlay, OV_PARM_BITMAP, &p1, &p2))
               {
                  IBitmap *pBitmap = (IBitmap *)p1;

                  if(NULL != pBitmap)
                  {
                     uint32 nativeClr = IBITMAP_RGBToNative(pBitmap, rgb);
                     (void) IBITMAP_Release(pBitmap);
                     (void) modifyTransparency(pImage, bTransp, nativeClr);
                  }
               }
               break;
            }
            case OV_PARM_LAYER:
            {
               uint32 dwLayer = pParam->p1;

               pMDPInfo->bRequireDeregister = TRUE;

               (void) modifyLayer(pImage, dwLayer);

               break;
            }
            case OV_PARM_VSYNC:
               if(pParam->p1 == 0)
               {
                  pImage->mdpOp |= MDPOP_VSYNC_OFF;
               }
               else
               {
                  pImage->mdpOp &= ~MDPOP_VSYNC_OFF;
               }
               break;

#ifndef FEATURE_MDP2
            case OV_PARM_MINIMIZE_CPU:
               if(pParam->p1 == 0)
               {
                  pImage->mdpOp |= MDPOP_QUEUE_COPY;
               }
               else
               {
                  pImage->mdpOp &= ~MDPOP_QUEUE_COPY;
               }
               break;
#endif
            default:
               return;
         }

         return;
      }

      default:
         return;
   }
}


/*==============================================================================
==============================================================================*/
/*lint -save -e818 pMe can not be const */
int buildMDPImage(IPhysicalDisplayOverlay *pMe, OverlayMDPType *pMDPInfo)
{
   uint32 p1, p2;
   AEERect rect  = {0,0,0,0};
   AEERect rectScreen;
   MDPIMG *pImage = &pMDPInfo->MDPImage;

   pImage->mdpOp = MDPOP_NOP;
   //Destination
   switch(pMe->pMe->clsID)
   {
      case AEECLSID_PHYSICALDISPLAY1:
         pImage->dest = DISPLAY_TYPE_1;
         break;

      case AEECLSID_PHYSICALDISPLAY2:
         pImage->dest = DISPLAY_TYPE_2;
         break;

      case AEECLSID_PHYSICALDISPLAY3:
         pImage->dest = DISPLAY_TYPE_3;
         break;

      case AEECLSID_PHYSICALDISPLAY4:
         pImage->dest = DISPLAY_TYPE_4;
         break;

      default:
         return EUNSUPPORTED;
   }

   //Bitmap
   if(SUCCESS == IMMOVERLAY_GetParam(pMDPInfo->pOverlay, OV_PARM_BITMAP, &p1, &p2))
   {
      IBitmap *pBitmap = (IBitmap *)p1;

      if(pBitmap == NULL)
      {
         DBGPRINTF_FATAL("GetParam returned SUCCESS, but bitmap is NULL");
         return EFAILED;
      }

      RELEASEIF(pMDPInfo->pSetBitmap);
      pMDPInfo->pSetBitmap = pBitmap;
      (void) IBITMAP_AddRef(pMDPInfo->pSetBitmap);

      if(SUCCESS != modifyBitmap(pMDPInfo, pImage, pBitmap))
      {
         (void) IBITMAP_Release(pBitmap);
         return EFAILED;
      }


      (void) IBITMAP_Release(pBitmap);
   }
   else
   {
      return EFAILED;
   }

   //Source Region
   p1 = (uint32) &pMDPInfo->rcSetSrc;
   if(SUCCESS != IMMOVERLAY_GetParam(pMDPInfo->pOverlay, OV_PARM_SRC_REGION, &p1, &p2))
   {
      return EFAILED;
   }


   //Rotate and Reflect
   if(SUCCESS == IMMOVERLAY_GetParam(pMDPInfo->pOverlay, OV_PARM_ROTATE, &p1, &p2))
   {
      pMDPInfo->dwOverlayRotate = p1;

      if(SUCCESS == IMMOVERLAY_GetParam(pMDPInfo->pOverlay, OV_PARM_REFLECT, &p1, &p2))
      {
         pMDPInfo->bReflectX = (0 != (p1 & OV_FLAG_REFLECT_X_AXIS));
         pMDPInfo->bReflectY = (0 != (p1 & OV_FLAG_REFLECT_Y_AXIS));
      }
      else
      {
         return EFAILED;
      }
   }
   else
   {
      return EFAILED;
   }

   //Vsync
   if(SUCCESS == IMMOVERLAY_GetParam(pMDPInfo->pOverlay, OV_PARM_VSYNC, &p1, &p2))
   {
      if(p1 == 0)
      {
         pImage->mdpOp |= MDPOP_VSYNC_OFF;
      }
      else
      {
          pImage->mdpOp &= ~MDPOP_VSYNC_OFF;
      }
   }

#ifndef FEATURE_MDP2
   if(SUCCESS == IMMOVERLAY_GetParam(pMDPInfo->pOverlay, OV_PARM_MINIMIZE_CPU, &p1, &p2))
   {
      if(p1 == 0)
      {
         pImage->mdpOp |= MDPOP_QUEUE_COPY;
      }
      else
      {
          pImage->mdpOp &= ~MDPOP_QUEUE_COPY;
      }
   }
#endif

   //Scale X and Y -- Must be the same for MDP1
   if(SUCCESS == IMMOVERLAY_GetParam(pMDPInfo->pOverlay, OV_PARM_SCALE_X, &p1, &p2))
   {
      pMDPInfo->scaleFlagsX = p1; 
      if(SUCCESS == IMMOVERLAY_GetParam(pMDPInfo->pOverlay, OV_PARM_SCALE_Y, &p1, &p2))
      {
         pMDPInfo->scaleFlagsY = p1; 
      }
   }
   else
   {
      return EFAILED;
   }

   //Edge Scaling
   if(SUCCESS == IMMOVERLAY_GetParam(pMDPInfo->pOverlay, OV_PARM_EDGE_SCALING, &p1, &p2))
   {
      pMDPInfo->bEdgeScale = (boolean)p1;
   }
   else
   {
      return EFAILED;
   }


   //Destination Region
   p1 = (uint32) &rect;
   if(SUCCESS == IMMOVERLAY_GetParam(pMDPInfo->pOverlay, OV_PARM_DEST_REGION, &p1, &p2))
   {
      pMDPInfo->alignmentFlags = p2;
      p1 = (uint32) &rect;
      p2 = (uint32) &rectScreen;
      if(SUCCESS == IMMOVERLAY_GetParam(pMDPInfo->pOverlay, OV_PARM_DEST_REGION_SCREEN, &p1, &p2))
      {
         if(SUCCESS != modifyDestinationRegion(pImage, &rect, pMDPInfo))
         {
            return EFAILED;
         }
      }
      else
      {
         return EFAILED;
      }
   }
   else
   {
      return EFAILED;
   }

   //Alpha
   if(SUCCESS == IMMOVERLAY_GetParam(pMDPInfo->pOverlay, OV_PARM_ALPHA_BLEND_PCT, &p1, &p2))
   {
      if(SUCCESS != modifyAlpha(pImage, p1))
      {
         return EFAILED;
      }
   }
   else
   {
      return EFAILED;
   }

   //Transparency
   if(SUCCESS == IMMOVERLAY_GetParam(pMDPInfo->pOverlay, OV_PARM_TRANSPARENCY, &p1, &p2))
   {
      boolean bTransp = p1 == 1;
      RGBVAL rgb = p2;
      uint32 nativeClr;

      if(SUCCESS == IMMOVERLAY_GetParam(pMDPInfo->pOverlay, OV_PARM_BITMAP, &p1, &p2))
      {
         IBitmap *pBitmap = (IBitmap *)p1;

         if(NULL != pBitmap)
         {
            nativeClr = IBITMAP_RGBToNative(pBitmap, rgb);
            (void) IBITMAP_Release(pBitmap);
         }
         else
         {
            return EFAILED;
         }
      }
      else
      {
         return EFAILED;
      }

      if(SUCCESS != modifyTransparency(pImage, bTransp, nativeClr))
      {
         return EFAILED;
      }
   }
   else
   {
      return EFAILED;
   }


   //Layer
   if(SUCCESS == IMMOVERLAY_GetParam(pMDPInfo->pOverlay, OV_PARM_LAYER, &p1, &p2))
   {
      if(SUCCESS != modifyLayer(pImage, p1))
      {
         return EFAILED;
      }
   }
   else
   {
      return EFAILED;
   }

   pImage->ccsPtr = NULL;

   pMDPInfo->bRequireDeregister = FALSE;
   pMDPInfo->bNeedReregister = FALSE;

   return SUCCESS;
}
/*lint -restore */

/*
================================================================================

IPHYSICALDISPLAYOVERLAY_OverlayRegister()

Description: 
   This function registers the IMMOverlay with the physical display

Prototype:

   int IPHYSICALDISPLAYOVERLAY_OverlayRegister(IPhysicalDisplayOverlay * pMe,
                                        IMMOverlay *pOverlay, 
                                        uint32    dwOldOverlayID,
                                        uint32 * pdwNewOverlayID)

Parameters:
===pre>
   pMe             : Pointer to the IPhysicalDisplay interface object.
   pOverlay        : Pointer to the IMMOverlay to register
   dwOldOverlayID  : ID of the overlay to replace with this registration
   pdwNewOverlayID : Pointer to a uint32 that will contain the overlay ID
===/pre>

Return Value:
   SUCCESS          - Operation Successful
   EFAILED          - Generic failure
   Other error codes depend on application.

Comments:  
   None.

Side Effects: 
   None.

See Also:
   None

================================================================================
*/

static int PhysicalDisplayOverlay_OverlayRegister(IPhysicalDisplayOverlay * pMe,
                                           IMMOverlay *pOverlay,
                                           uint32 * pdwOverlayID)
{
   OverlayMDPType *pOverlayInfo;
   IModel * pModel;
   uint32 p1, p2;

   if(pOverlay == NULL)
   {
      return EBADPARM;
   }

   if(pdwOverlayID == NULL)
   {
      return EBADPARM;
   }

   pOverlayInfo = MALLOCREC(OverlayMDPType);
   if(pOverlayInfo == NULL)
   {
      return ENOMEMORY;
   }

   pOverlayInfo->pOverlay = pOverlay;

   pOverlayInfo->pPhysical = pMe;

   if(SUCCESS != buildMDPImage(pMe, pOverlayInfo))
   {
      FREE(pOverlayInfo);
      return EFAILED;
   }

   (void) IPHYSICALDISPLAYOVERLAY_AddRef(pMe);   

   pOverlayInfo->pRegBitmap = pOverlayInfo->pSetBitmap;
   (void) IBITMAP_AddRef(pOverlayInfo->pRegBitmap);
   
   (pOverlayInfo)->dwMDPID = mdp_register_ovimg(&(pOverlayInfo->MDPImage));
   if(mdp_is_ovimg_valid(pOverlayInfo->dwMDPID))
   { 
      (void) mdp_lock_ovimg(pOverlayInfo->dwMDPID, TRUE);
   }
   else
   {
      FREE(pOverlayInfo);
      return EFAILED;
   }

   if(FALSE == AEEHandle_To(&gHandleList, (OEMINSTANCE) pOverlayInfo, pdwOverlayID))
   {
      FREE(pOverlayInfo);
      return EFAILED;
   }

   //Enable the Image if needed
   if(SUCCESS == IMMOVERLAY_GetParam(pOverlay, OV_PARM_ENABLE, &p1, &p2))
   {
      (void) IPHYSICALDISPLAYOVERLAY_OverlayEnable((IPhysicalDisplayOverlay *)pMe,
                               *pdwOverlayID,
                               (boolean)(p1 == 1)); 
   }

   LISTENER_Init((&(pOverlayInfo->listener)), OverlayListenerCB, (void *) pOverlayInfo);

   //Register the listener
   if(SUCCESS == IMMOVERLAY_QueryInterface(pOverlay, AEEIID_MODEL, (void **) &pModel))
   {
      if(SUCCESS != IMODEL_AddListener(pModel, (&(pOverlayInfo->listener))))
      {
         DBGPRINTF_FATAL("Failed to add listener");
         DBGPRINTF_FATAL("Changes in overlay will not be reflected");
      }
      (void)IMODEL_Release(pModel);
   }


   return SUCCESS;
}

/*
================================================================================

IPHYSICALDISPLAYOVERLAY_OverlayDeregister()

Description: 
   This function deregisters the specified overlay on this physical display.

Prototype:

   int IPHYSICALDISPLAYOVERLAY_OverlayDeregister(IPhysicalDisplayOverlay * pMe,
                                          uint32    dwOverlayID)

Parameters:
===pre>
   pMe             : Pointer to the IPhysicalDisplay interface object.
   dwOverlayID     : ID of the overlay that was obtained when it was registered
===/pre>

Return Value:
   SUCCESS          - Operation Successful
   EFAILED          - Generic failure
   Other error codes depend on application.

Comments:  
   None.

Side Effects: 
   None.

See Also:
   None

================================================================================
*/
/*lint -save -e818 pMe can not be const */
static int PhysicalDisplayOverlay_OverlayDeregister(IPhysicalDisplayOverlay * pMe,
                                             uint32    dwOverlayID)
{
   OverlayMDPType *pOverlayInfo;

   if(FALSE == AEEHandle_From(&gHandleList, dwOverlayID, (OEMINSTANCE *)&pOverlayInfo))
   {
      return EFAILED;
   }

   if(mdp_is_ovimg_valid(pOverlayInfo->dwMDPID))
   {
      (void) mdp_lock_ovimg(pOverlayInfo->dwMDPID, FALSE);

      if(IPHYSICALDISPLAY_IsEnabled(pMe->pMe)&& pOverlayInfo->bIsEnabled)
      {
         mdp_clear_ovimg(pOverlayInfo->dwMDPID);
      }
      mdp_deregister_ovimg(pOverlayInfo->dwMDPID);
      pOverlayInfo->dwMDPID = 0;
   }

   RELEASEIF(pOverlayInfo->pRegBitmap);
   RELEASEIF(pOverlayInfo->pSetBitmap);

   //(void)IMMOVERLAY_Release(pOverlayInfo->pOverlay);
   pOverlayInfo->pOverlay = NULL;

   LISTENER_Cancel(&pOverlayInfo->listener);

   (void) IPHYSICALDISPLAYOVERLAY_Release(pOverlayInfo->pPhysical);

   FREE(pOverlayInfo);

   if(FALSE == AEEHandle_Clear(&gHandleList, dwOverlayID))
   {
      DBGPRINTF("Handle was not cleared.");
   }

   return SUCCESS;
}
/*lint -restore */

/*
================================================================================

IPHYSICALDISPLAYOVERLAY_OverlayEnable()

Description: 
   This function enables or disables the specified overlay on for physical 
   display.

Prototype:

   int IPHYSICALDISPLAYOVERLAY_OverlayEnable(IPhysicalDisplayOverlay * pMe,
                               uint32 dwOverlayID,
                               boolean bEnable)

Parameters:
===pre>
   pMe             : Pointer to the IPhysicalDisplay interface object.
   dwOverlayID     : ID of the overlay that was obtained when it was registered
   bEnable         : TRUE to enable the overlay, FALSE to disable it
===/pre>

Return Value:
   SUCCESS          - Operation Successful
   EFAILED          - Generic failure
   Other error codes depend on application.

Comments:  
   None.

Side Effects: 
   None.

See Also:
   None

================================================================================
*/

static int PhysicalDisplayOverlay_OverlayEnable(IPhysicalDisplayOverlay * pMe,
                                         uint32 dwOverlayID,
                                         boolean bEnable)
{
   OverlayMDPType *pOverlayInfo;

   if(FALSE == AEEHandle_From(&gHandleList, dwOverlayID, (OEMINSTANCE *)&pOverlayInfo))
   {
      return EFAILED;
   }

   if(mdp_is_ovimg_valid(pOverlayInfo->dwMDPID))
   {
      mdp_enable_locked_ovimg(pOverlayInfo->dwMDPID, bEnable? 1: 0);
   }

   if(!bEnable && pOverlayInfo->bIsEnabled)
   {
      //Clear the region as well
      (void) IPHYSICALDISPLAYOVERLAY_OverlayClear(pMe, dwOverlayID);
   }
   pOverlayInfo->bIsEnabled = bEnable;

   return SUCCESS;
}

/*
================================================================================

IPHYSICALDISPLAYOVERLAY_OverlayUpdate()

Description: 
   This function updates the overlay on the physical display

Prototype:

   int IPHYSICALDISPLAYOVERLAY_OverlayUpdate(IPhysicalDisplayOverlay * pMe,
                                      uint32    dwOverlayID, 
                                      AEERect * pRec,
                                      uint32    dwUpdateID,
                                      boolean   bAsync
                                      )

Parameters:
===pre>
   pMe             : Pointer to the IPhysicalDisplay interface object.
   dwOverlayID     : ID of the overlay that was obtained when it was registered
   pRec            : Pointer to the rectangle to update
   dwUpdateID      : Unique ID for this update
   bAsync          : Should this update be processed Async
===/pre>

Return Value:
   SUCCESS          - Operation Successful
   EFAILED          - Generic failure
   Other error codes depend on application.

Comments:  
   None.

Side Effects: 
   None.

See Also:
   None

================================================================================
*/
static void PhysicalDisplayOverlay_UpdateCompleteInBREW(void *po)
{
   PhysicalDisplayUpdateEntry *pUpdateEntry = (PhysicalDisplayUpdateEntry *)po;
   PhysicalDisplayAsyncUpdateCompleteType notifyValue;

   if(NULL == pUpdateEntry)
   {
      return;
   }

   //Send off the notification
   notifyValue.dwUnique = pUpdateEntry->dwUpdateID;
   notifyValue.nErr = pUpdateEntry->nErr;
   notifyValue.pOverlay = pUpdateEntry->pOverlay;
   notifyValue.clsPhysicalDisplay = pUpdateEntry->pPhysicalDisplay->pMe->clsID;
   OverlayHelper_Notify((IQueryInterface*) pUpdateEntry->pPhysicalDisplay, 
            EVT_MDL_PDC_ASYNC_UPDATE_COMPLETE, (uint32)&notifyValue);

   //Release Everything
   (void)IBITMAP_Release(pUpdateEntry->pBitmap);
   (void)IPHYSICALDISPLAYOVERLAY_Release(pUpdateEntry->pPhysicalDisplay);
   (void)IMMOVERLAY_Release(pUpdateEntry->pOverlay);
   pUpdateEntry->bInUse = FALSE;

   return;
}

static void PhysicalDisplayOverlay_UpdateAsyncComplete(void *po, MDP_STATUS mdp_status)
{
   PhysicalDisplayUpdateEntry *pUpdateEntry = (PhysicalDisplayUpdateEntry *)po;

   if(NULL == pUpdateEntry)
   {
      return;
   }

   if(MDP_SUCCESSFUL == mdp_status)
   {
      pUpdateEntry->nErr = SUCCESS;
   }
   else
   {
      pUpdateEntry->nErr = EFAILED;
   }

   //Setup the callback
   CALLBACK_Init(&(pUpdateEntry->cb), PhysicalDisplayOverlay_UpdateCompleteInBREW, (void *)pUpdateEntry);
   AEE_SYS_RESUME(&(pUpdateEntry->cb));

   return;
}

#ifdef FEATURE_MDP2   
#error code not present
#endif

static int PhysicalDisplayOverlay_OverlayUpdate(IPhysicalDisplayOverlay * pMe,
                                      uint32    dwOverlayID, 
                                      const AEERect * pRec,
                                      uint32    dwUpdateID,
                                      boolean bAsync )
{
   OverlayMDPType *pOverlayInfo;

   if(FALSE == AEEHandle_From(&gHandleList, dwOverlayID, (OEMINSTANCE *)&pOverlayInfo))
   {
      return EFAILED;
   }

   if(mdp_is_ovimg_valid(pOverlayInfo->dwMDPID))
   {
      MDP_CWIN region;
      MDP_ASCALE newAscale = {0,0};

      int32 newLcd_x, newLcd_y;
      AEERect srcRegion = *pRec;
      AEEBitmapInfo bi;
      //int32 sintheta = 0;
      //int32 costheta = 0;

      //Remove the scale factor
      srcRegion.x  = (int16)(((int32)srcRegion.x * 256) / 
                             (int32)pOverlayInfo->dwBitmapScaleX);
      srcRegion.dx = (int16)(((int32)srcRegion.dx* 256) /
                             (int32)pOverlayInfo->dwBitmapScaleX);
      srcRegion.y  = (int16)(((int32)srcRegion.y * 256) /
                             (int32)pOverlayInfo->dwBitmapScaleY);
      srcRegion.dy = (int16)(((int32)srcRegion.dy* 256) /
                             (int32)pOverlayInfo->dwBitmapScaleY);


      if(SUCCESS == IBITMAP_GetInfo(pOverlayInfo->pSetBitmap, &bi, sizeof(AEEBitmapInfo)))
      {
         AEEPoint orig1, orig2;
         AEEPoint newPt1 = {0,0};
         AEEPoint newPt2 = {0,0};
         
         //int16 xCenter, yCenter;

         orig1.x = srcRegion.x;
         orig1.y = srcRegion.y;
         orig2.x = srcRegion.x + srcRegion.dx - 1;
         orig2.y = srcRegion.y + srcRegion.dy - 1;

         //xCenter = (bi.cx-1)/2;
         //yCenter = (bi.cy-1)/2;

         bi.cx = (uint32)(((int32)bi.cx* 256)/
                         (int32)pOverlayInfo->dwBitmapScaleX);
         bi.cy = (uint32)(((int32)bi.cy* 256)/
                         (int32)pOverlayInfo->dwBitmapScaleY);

         //newPt1.x = (costheta * (orig1.x - xCenter)) - (sintheta * (orig1.y - yCenter)) + abs(sintheta * yCenter + costheta * xCenter);
         //newPt1.y = (sintheta * (orig1.x - xCenter)) + costheta * ((orig1.y - yCenter)) + abs(costheta * yCenter + sintheta * xCenter);

         //newPt2.x = (costheta * (orig2.x - xCenter)) - (sintheta * (orig2.y - yCenter)) + abs(sintheta * yCenter + costheta * xCenter);
         //newPt2.y = (sintheta * (orig2.x - xCenter)) + costheta * ((orig2.y - yCenter)) + abs(costheta * yCenter + sintheta * xCenter);

         if(pOverlayInfo->dwBitmapRotate == 0)
         {
            //sintheta = 0;
            //costheta = 1;
            newPt1.x = orig1.x;
            newPt1.y = orig1.y;

            newPt2.x = orig2.x;
            newPt2.y = orig2.y;

         }
         else if(pOverlayInfo->dwBitmapRotate == 90)
         {
            //sintheta = -1;
            //costheta = 0;
            newPt1.x = orig1.y;
            newPt1.y = ((int16)bi.cx - 1) - orig1.x;

            newPt2.x = orig2.y;
            newPt2.y = ((int16)bi.cx - 1) - orig2.x;

         }
         else if(pOverlayInfo->dwBitmapRotate == 180)
         {
            //sintheta = 0;
            //costheta = -1;
            newPt1.x = ((int16)bi.cx - 1) - orig1.x;
            newPt1.y = ((int16)bi.cy - 1) - orig1.y;

            newPt2.x = ((int16)bi.cx - 1) - orig2.x;
            newPt2.y = ((int16)bi.cy - 1) - orig2.y;
         }
         else if(pOverlayInfo->dwBitmapRotate == 270)
         {
            //sintheta = 1;
            //costheta = 0;
            newPt1.x = orig1.y;
            newPt1.y = orig1.x;

            newPt2.x = orig2.y;
            newPt2.y = orig2.x;
         }

         if(newPt1.x < newPt2.x)
         {
            srcRegion.x = newPt1.x;
            srcRegion.dx = (newPt2.x - newPt1.x) + 1;
         }
         else
         {
            srcRegion.x = newPt2.x;
            srcRegion.dx = (newPt1.x - newPt2.x) + 1;
         }

         if(newPt1.y < newPt2.y)
         {
            srcRegion.y = newPt1.y;
            srcRegion.dy = (newPt2.y - newPt1.y) + 1;
         }
         else
         {
            srcRegion.y = newPt2.y;
            srcRegion.dy = (newPt1.y - newPt2.y) + 1;
         }
      }


      region.x1 = srcRegion.x;
      region.y1 = srcRegion.y;
      region.x2 = srcRegion.x + srcRegion.dx - 1;
      region.y2 = srcRegion.y + srcRegion.dy - 1;

      //Make sure we are not exceeding the original source region
      if(region.x1 < pOverlayInfo->MDPImage.cwin.x1)
      {
         region.x1 = pOverlayInfo->MDPImage.cwin.x1;
      }

      if(region.y1 < pOverlayInfo->MDPImage.cwin.y1)
      {
         region.y1 = pOverlayInfo->MDPImage.cwin.y1;
      }

      if(region.x2 > pOverlayInfo->MDPImage.cwin.x2)
      {
         region.x2 = pOverlayInfo->MDPImage.cwin.x2;
      }

      if(region.y2 > pOverlayInfo->MDPImage.cwin.y2)
      {
         region.y2 = pOverlayInfo->MDPImage.cwin.y2;
      }

      if((region.x2 == region.x1) && (region.y2 == region.y1))
      {
         region.x2++;
      }

      //Set the dest region
      newLcd_x = region.x1 - pOverlayInfo->MDPImage.cwin.x1;
      newLcd_y = region.y1 - pOverlayInfo->MDPImage.cwin.y1;

      if(pOverlayInfo->MDPImage.mdpOp & MDPOP_ASCALE)
      {
         newAscale.width = 
           ((((uint32)region.x2 - (uint32)region.x1) + 1) *
            (uint32)pOverlayInfo->MDPImage.ascale.width)/
           (((uint32)pOverlayInfo->MDPImage.cwin.x2 -
              (uint32)pOverlayInfo->MDPImage.cwin.x1) + 1); 
         newAscale.height = 
            ((((uint32)region.y2 - (uint32)region.y1) + 1) *
             (uint32)pOverlayInfo->MDPImage.ascale.height)/
            (((uint32)pOverlayInfo->MDPImage.cwin.y2 -
               (uint32)pOverlayInfo->MDPImage.cwin.y1) + 1); 
         newLcd_x = 
            (newLcd_x * 
             (int32)pOverlayInfo->MDPImage.ascale.width)/
            (((int32)pOverlayInfo->MDPImage.cwin.x2 - 
             (int32)pOverlayInfo->MDPImage.cwin.x1) + 1);
         newLcd_y = 
            (newLcd_y * 
             (int32)pOverlayInfo->MDPImage.ascale.height)/
            (((int32)pOverlayInfo->MDPImage.cwin.y2 - 
             (int32)pOverlayInfo->MDPImage.cwin.y1) + 1);
      }
      else if(pOverlayInfo->MDPImage.mdpOp & MDPOP_VDO_X2) 
      {
         newLcd_x *= 2;
         newLcd_y *= 2;
      }
      else if(pOverlayInfo->MDPImage.mdpOp & MDPOP_VDO_1BY4) 
      {
         newLcd_x /= 4;
         newLcd_y /= 4;
      }
      else if(pOverlayInfo->MDPImage.mdpOp & MDPOP_VDO_1BY2) 
      {
         newLcd_x /= 2;
         newLcd_y /= 2;
      }
      else if(pOverlayInfo->MDPImage.mdpOp & MDPOP_VDO_3BY4) 
      {
         newLcd_x += (newLcd_x * 3)/4;
         newLcd_y += (newLcd_y * 3)/4;
      }

      newLcd_x += pOverlayInfo->MDPImage.lcd_x;
      newLcd_y += pOverlayInfo->MDPImage.lcd_y;

      if((gPhysicalDisp1_Handler == pMe->pMe->clsID) ||
         (gPhysicalDisp2_Handler == pMe->pMe->clsID) ||
         (gPhysicalDisp3_Handler == pMe->pMe->clsID) ||
         (gPhysicalDisp4_Handler == pMe->pMe->clsID))
      {
         MDPIMG mdpImg;
         if(MDP_SUCCESSFUL == mdp_get_mdpImg(pOverlayInfo->dwMDPID, &mdpImg))
         {
            mdpImg.bmy_addr = pOverlayInfo->MDPImage.bmy_addr;
            mdpImg.cwin = region;
            mdpImg.lcd_x = newLcd_x;
            mdpImg.lcd_y = newLcd_y;

            mdpImg.ascale = newAscale;
#if defined(FEATURE_MDP2) 
#error code not present
#endif
            if(FALSE == bAsync)
            {
#ifdef FEATURE_MDP_WSCALE
               if((pOverlayInfo->MDPImage.mdpOp & MDPOP_WSCALE_LINEAR_STRETCH) ||
                  (pOverlayInfo->MDPImage.mdpOp & MDPOP_WSCALE_EDGE_STRETCH) ||
                  (pOverlayInfo->MDPImage.mdpOp & MDPOP_WSCALE_NO_STRETCH))
               {
                  (void)mdp_update_ovimg(pOverlayInfo->dwMDPID);
               }
               else
#endif //FEATURE_MDP_WSCALE
               {
                  (void)mdp_disp_update(&mdpImg);
               }
            }
            else
            {
               MDP_STATUS mdp_status;
               //Find an unused entry in the table
               int index;
               for(index = 0; index < MAX_PENDING_UPDATES; index++)
               {
                  if(FALSE == pMe->updateEntries[index].bInUse)
                  {
                     break;
                  }
               }
               if(index == MAX_PENDING_UPDATES)
               {
                  return EFAILED;
               }
               
               //Build the updateEntry Stucture
               pMe->updateEntries[index].bInUse = TRUE;
               pMe->updateEntries[index].pBitmap = pOverlayInfo->pRegBitmap;
               (void)IBITMAP_AddRef(pMe->updateEntries[index].pBitmap);
               pMe->updateEntries[index].pPhysicalDisplay = pMe;
               (void)IPHYSICALDISPLAYOVERLAY_AddRef(pMe->updateEntries[index].pPhysicalDisplay);
               pMe->updateEntries[index].pOverlay = pOverlayInfo->pOverlay;
               (void)IMMOVERLAY_AddRef(pMe->updateEntries[index].pOverlay);
               pMe->updateEntries[index].dwUpdateID = dwUpdateID;

               //Initiate the update
#ifdef FEATURE_DISP_TASK
               pMe->updateEntries[index].mdp_cb.cb_func = PhysicalDisplayOverlay_UpdateAsyncComplete;
               pMe->updateEntries[index].mdp_cb.arg = (void *)&(pMe->updateEntries[index]);
#ifdef FEATURE_MDP_WSCALE
               if((pOverlayInfo->MDPImage.mdpOp & MDPOP_WSCALE_LINEAR_STRETCH) ||
                  (pOverlayInfo->MDPImage.mdpOp & MDPOP_WSCALE_EDGE_STRETCH) ||
                  (pOverlayInfo->MDPImage.mdpOp & MDPOP_WSCALE_NO_STRETCH))
               {
                  mdp_status = mdp_update_ovimg_async(pOverlayInfo->dwMDPID, 
                                                      &(pMe->updateEntries[index].mdp_cb));
               }
               else
#endif //FEATURE_MDP_WSCALE
               {
                  mdp_status = mdp_disp_update_async(&mdpImg, 
                                                     &(pMe->updateEntries[index].mdp_cb));
               }

               if(MDP_SUCCESSFUL != mdp_status)
               {
                  PhysicalDisplayOverlay_UpdateAsyncComplete((void *)&(pMe->updateEntries[index]), mdp_status);
               }                
#else
#ifdef FEATURE_MDP_WSCALE
               if((pOverlayInfo->MDPImage.mdpOp & MDPOP_WSCALE_LINEAR_STRETCH) ||
                  (pOverlayInfo->MDPImage.mdpOp & MDPOP_WSCALE_EDGE_STRETCH) ||
                  (pOverlayInfo->MDPImage.mdpOp & MDPOP_WSCALE_NO_STRETCH))
               {
                  mdp_status = mdp_update_ovimg(pOverlayInfo->dwMDPID);
               }
               else
#endif //FEATURE_MDP_WSCALE
               {
                  mdp_status = mdp_disp_update(&mdpImg);
               }
               PhysicalDisplayOverlay_UpdateAsyncComplete((void *)&(pMe->updateEntries[index]), mdp_status);
#endif
            }
         }
      }
   }

   return SUCCESS;
}

/*
================================================================================

IPHYSICALDISPLAYOVERLAY_OverlayClear()

Description: 
   This function clears the area that this overlay takes on the physical display

Prototype:

   int IPHYSICALDISPLAYOVERLAY_OverlayClear(IPhysicalDisplayOverlay * pMe,
                                      uint32    dwOverlayID)

Parameters:
===pre>
   pMe             : Pointer to the IPhysicalDisplay interface object.
   dwOverlayID     : ID of the overlay that was obtained when it was registered
===/pre>

Return Value:
   SUCCESS          - Operation Successful
   EFAILED          - Generic failure
   Other error codes depend on application.

Comments:  
   None.

Side Effects: 
   None.

See Also:
   None

================================================================================
*/
/*lint -save -e818 pMe can not be const */
static int PhysicalDisplayOverlay_OverlayClear(IPhysicalDisplayOverlay * pMe,
                                        uint32    dwOverlayID)
{
   OverlayMDPType *pOverlayInfo;

   if(FALSE == AEEHandle_From(&gHandleList, dwOverlayID, (OEMINSTANCE *)&pOverlayInfo))
   {
      return EFAILED;
   }

   if(mdp_is_ovimg_valid(pOverlayInfo->dwMDPID))
   {
      if(IPHYSICALDISPLAY_IsEnabled(pMe->pMe) && pOverlayInfo->bIsEnabled)
      {
         mdp_clear_ovimg(pOverlayInfo->dwMDPID);
         
#ifndef FEATURE_UIONE_HDK
         if(pOverlayInfo->MDPImage.layer == MDP_LAYER_2)
         {
            PACONTEXT pacLast;
            IAnnunciator *pAnnun;
            pacLast = AEE_EnterAppContext(NULL);
            if(SUCCESS == ISHELL_CreateInstance(pMe->pMe->pShell, AEECLSID_ANNUNCIATOR, (void **) &pAnnun))
            {
               IANNUNCIATOR_Redraw(pAnnun);
               IANNUNCIATOR_Release(pAnnun);
            }
            AEE_LeaveAppContext(pacLast);
         }
#endif
      }
   }

   return SUCCESS;
}
/*lint -restore */
/*
================================================================================

IPHYSICALDISPLAYOVERLAY_ValidateBitmap()

Description: 
   This function checks to see if the bitmap is supported on the physical display

Prototype:

   int IPHYSICALDISPLAYOVERLAY_ValidateBitmap(IPhysicalDisplayOverlay * pMe,
                                       IBitmap *pBitmap)

Parameters:
===pre>
   pMe             : Pointer to the IPhysicalDisplay interface object.
   pBitmap         : Pointer to the IBitmap that will be checked
===/pre>

Return Value:
   SUCCESS          - Operation Successful
   EFAILED          - Generic failure
   Other error codes depend on application.

Comments:  
   None.

Side Effects: 
   None.

See Also:
   None

================================================================================
*/
/*lint -save -e818 pMe can not be const */
static int PhysicalDisplayOverlay_ValidateBitmap(IPhysicalDisplayOverlay * pMe,
                                    IBitmap *pBitmap)
{
   int nErr = SUCCESS;
   (void)pMe;

   if(pBitmap != NULL)
   {
      IDIB * pDIB;
      IYCbCr * pIYCbCr;
      if((IBITMAP_QueryInterface(pBitmap, AEECLSID_DIB, 
         (void **) &pDIB) == SUCCESS) && (pDIB != NULL))
      {
         if((pDIB->nColorScheme == IDIB_COLORSCHEME_565)
#ifdef FEATURE_MDP2
#error code not present
#endif
           )
         {
            nErr = SUCCESS;
         }
         else
         {
            nErr = EUNSUPPORTED;
         }

         (void)IDIB_Release(pDIB);
      }
      else if((IBITMAP_QueryInterface(pBitmap, AEEIID_YCBCR,
         (void **) &pIYCbCr) == SUCCESS) && (pIYCbCr != NULL))
      {
         if((pIYCbCr->uScheme == IYCBCR_COLORSCHEME_420LP)
#ifdef FEATURE_MDP2
#error code not present
#endif
            )
         {
            nErr = SUCCESS;
         }
         else
         {
            nErr = EUNSUPPORTED;
         }

         (void)IYCBCR_Release(pIYCbCr);
      }
      else
      {
         nErr = EUNSUPPORTED;
      }
   }
   else
   {
      nErr = EBADPARM;
   }

   return nErr;
}
/*lint -restore */
/*
================================================================================

IPHYSICALDISPLAYOVERLAY_AddListener()

Description:
   This function allows objects to attach a listener to the given interface.  
   The listener includes a callback function that BREW will call whenever the 
   interface has changed state, thereby notifying the owner of the listener that
   data has changed.

   The list of events that can be sent are listed in the comments section.

Prototype:

   int IPHYSICALDISPLAYOVERLAY_AddListener(IPhysicalDisplayOverlay *pMe, 
                                    ModelListener *pListener); 

Parameters:
===pre>
   pMe       : Pointer to the IPhysicalDisplay interface object.
   pListener : Pointer to the ModelListener that will be attached to the object.  
               The ModelListener will contain the PFNLISTENER callback function  
               BREW will call to notify the registering object of changes/events 
               to the IPhysicalDisplay.
===/pre>

Return Value:
None

Comments:  
   This is a table of valid events from this interface.

EVENT             Description                          dwParam
--------------------------------------------------------------------------------
EVT_MDL_PDC_ASYNC_UPDATE_COMPLETE
                  Will be sent when the async          ID that was passed to
                  display update is complete           the async update 
                                                       function

Side Effects: 
   None.

See Also:
   ModelListener
   PFNLISTENER

================================================================================
*/
static int PhysicalDisplayOverlay_AddListener( IPhysicalDisplayOverlay *pMe, ModelListener *pListener )
{
   return OverlayHelper_AddListener( &pMe->pListeners, pListener );
}


/*======================================================================= 
=======================================================================*/
static int PhysicalDisplayOverlayModel_AddListener( IModel *po, ModelListener *pListener )
{
   IPhysicalDisplayOverlayModel *pme = (IPhysicalDisplayOverlayModel *)po;

   return PhysicalDisplayOverlay_AddListener( pme->pMe, pListener );

}



//VTables
   const VTBL(IModel) gPhysicalDisplayOverlayModelFuncs = {
      AEEBASE_AddRef(IModel),
      AEEBASE_Release(IModel),
      AEEBASE_QueryInterface(IModel),
      PhysicalDisplayOverlayModel_AddListener,
      PhysicalDisplayOverlayModel_Notify
   };

   const VTBL(IPhysicalDisplayOverlay) gPhysicalDisplayOverlayFuncs = {
      AEEBASE_AddRef(IPhysicalDisplayOverlay),
      AEEBASE_Release(IPhysicalDisplayOverlay),
      AEEBASE_QueryInterface(IPhysicalDisplayOverlay),
      PhysicalDisplayOverlay_OverlayRegister,
      PhysicalDisplayOverlay_OverlayDeregister,
      PhysicalDisplayOverlay_OverlayEnable,
      PhysicalDisplayOverlay_OverlayUpdate,
      PhysicalDisplayOverlay_OverlayClear,
      PhysicalDisplayOverlay_ValidateBitmap,
   };


#endif /*FEATURE_MMOVERLAY*/


/*=====================================================================
FILE:  OEMBitmap_common.c

SERVICES:  OEMBitmap common functions

DESCRIPTION: Functions shared between OEMBitmap implementations

PUBLIC CLASSES: Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS: Not Applicable
        
        Copyright © 2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================================*/

#include "OEMBitmap_priv.h"

int OEMBitmap_CopyMode(IBitmap *pDst, IBitmap *pSrc)
{
   int nErr;
   OEMBitmap *pbmSrc = NULL, *pbmDst = NULL;

   nErr = IBITMAP_QueryInterface(pSrc, AEEIID_OEMBITMAP3, (void**)&pbmSrc);
   if (SUCCESS != nErr) {
      goto Done;
   }

   nErr = IBITMAP_QueryInterface(pDst, AEEIID_OEMBITMAP3, (void**)&pbmDst);
   if (SUCCESS != nErr) {
      goto Done;
   }

   pbmDst->m_dib.pvt = pbmSrc->m_dib.pvt;
   pbmDst->m_IBitmapDev.pvt = pbmSrc->m_IBitmapDev.pvt;
   pbmDst->m_IBitmapRot.pvt = pbmSrc->m_IBitmapRot.pvt;
   pbmDst->m_IBitmapCtl.pvt = pbmSrc->m_IBitmapCtl.pvt;
#if defined(IMPLEMENT_ITRANSFORM)
   pbmDst->m_ITransform.pvt = pbmSrc->m_ITransform.pvt;
#endif

   pbmDst->m_dib.nDepth = pbmSrc->m_dib.nDepth;
   pbmDst->m_dib.nColorScheme = pbmSrc->m_dib.nColorScheme;
   pbmDst->m_dib.nPitch = pbmSrc->m_dib.nPitch;

Done:
   if (NULL != pbmSrc) {
      IBitmap *pib = OEMBITMAP_TO_IBITMAP(pbmSrc);
      IBITMAP_Release(pib);
   }
   if (NULL != pbmDst) {
      IBitmap *pib = OEMBITMAP_TO_IBITMAP(pbmDst);
      IBITMAP_Release(pib);
   }

   return nErr;
}


int OEMBitmap_SetRotation(IBitmap *pBmp, int nDegrees)
{
   OEMBitmap *pbmApp = NULL;
   OEMBitmap *pOEMBmpComp = NULL;
   IBitmapCtl *pbmCtl = NULL;
   IBitmap * pbmComp = NULL;
   int nErr, nPitch;
   AEERect rcApp;
   boolean bSwap;
   
   // Check if the rotation is by a valid amount
   // else return without rotating
   switch(nDegrees){
   case 0:
   case 90:
   case 180:
   case 270:
      break;
   default:
      nErr = EBADPARM;
      goto Done;
   }
   
   nErr = IBITMAP_QueryInterface(pBmp, AEEIID_OEMBITMAP3, (void**)&pbmApp);
   if (SUCCESS != nErr) {
      goto Done;
   }
   
   nErr = IBITMAP_QueryInterface(pBmp, AEEIID_BITMAPCTL, (void**)&pbmCtl);
   if (SUCCESS != nErr) {
      goto Done;
   }
   
   // Swap dx and dy if rotation does not match
   if( ( nDegrees            == 90 || nDegrees            == 270 ) && 
       ( pbmApp->m_nRotation ==  0 || pbmApp->m_nRotation == 180 )) {
      bSwap = TRUE;
   } else if( ( nDegrees            ==  0 || nDegrees            == 180 ) && 
              ( pbmApp->m_nRotation == 90 || pbmApp->m_nRotation == 270 )) {
      bSwap = TRUE;
   } else {
      bSwap = FALSE;
   }

   rcApp.x = 0;
   rcApp.y = 0;
   
   if(bSwap) {
      rcApp.dx = pbmApp->m_dib.cy;
      rcApp.dy = pbmApp->m_dib.cx;
   } else {
      rcApp.dx = pbmApp->m_dib.cx;
      rcApp.dy = pbmApp->m_dib.cy;
   }
   
   // Create a compatible bitmap to get the pitch for the new width.
   // Use height as 1 pixel to keep memory consumption low
   nErr = IBITMAP_CreateCompatibleBitmap(pBmp, &pbmComp, rcApp.dx, 1);
   if (SUCCESS != nErr) {
      goto Done;
   }
   
   nErr = IBITMAP_QueryInterface(pbmComp, AEEIID_OEMBITMAP3, (void**)&pOEMBmpComp);
   if (SUCCESS != nErr) {
      goto Done;
   }
   
   nPitch = pbmApp->m_dib.nPitch;
   pbmApp->m_dib.nPitch = pOEMBmpComp->m_dib.nPitch;
   
   // restrict the apps Bitmap
   // based on rotated dimensions
   nErr = IBITMAPCTL_Restrict(pbmCtl, &rcApp);
   if (SUCCESS != nErr) {
      // restore pitch
      pbmApp->m_dib.nPitch = nPitch;
      goto Done;
   }
   
   pbmApp->m_nRotation = nDegrees;
Done:
   if (NULL != pbmApp) {
      IBITMAP_Release(OEMBITMAP_TO_IBITMAP(pbmApp));
   }
   if (NULL != pOEMBmpComp) {
      IBITMAP_Release(OEMBITMAP_TO_IBITMAP(pOEMBmpComp));
   }
   if (NULL != pbmComp) {
      IBITMAP_Release(pbmComp);
   }
   if (NULL != pbmCtl) {
      IBITMAPCTL_Release(pbmCtl);
   }
   return nErr;
}

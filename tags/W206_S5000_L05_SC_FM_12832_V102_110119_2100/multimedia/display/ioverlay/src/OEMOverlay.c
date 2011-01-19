/*======================================================
FILE:  OEMOverlay.c

SERVICES:  OEM functions for implementing the IOverlay interface.

GENERAL DESCRIPTION:
   This provides a reference implementation for the IOverlay
   interface.

        Copyright 2004, 2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR

$Header: //source/qcom/qct/multimedia/display/ioverlay/main/latest/src/OEMOverlay.c#2 $
$Author: jbrasen $
$DateTime: 2008/06/25 09:32:38 $

=====================================================*/

#include "OEMFeatures.h"
#include "AEE_OEM.h"
#include "AEEModTable.h"
#include "AEEStdLib.h"

#include "disp.h"

#include "AEEBase.h"
#include "IOverlay.h"
#include "Overlay.bid"

#if (!defined (PRIMARY_LCD_TYPE)) || (!defined(SECONDARY_LCD_TYPE))
   typedef unsigned int LCD_TYPE;
   #undef PRIMARY_LCD_TYPE
   #undef SECONDARY_LCD_TYPE
   #define PRIMARY_LCD_TYPE                1
   #define SECONDARY_LCD_TYPE              2
#endif


#ifdef FEATURE_MDP
#include "mdp_drv_api.h"
#endif

int OEMOverlay_New(IShell * piShell, AEECLSID cls, void **ppif);
static uint32 OEMOverlay_AddRef(IOverlay *pMe);
static uint32 OEMOverlay_Release(IOverlay *pMe);
static int OEMOverlay_QueryInterface(IOverlay *pMe, AEECLSID clsid, void **ppNew);

#ifdef FEATURE_MDP
static OVERLAY_ID OEMOverlay_Add(IOverlay * pMe, IBitmap * pbm, AEERect *prcDst, AEERect *prcSrc, OverlayLayerType layer, OverlayOpt  *pOpts);
static OVERLAY_ID OEMOverlay_Replace(IOverlay * pMe, IBitmap * pbm, AEERect *prcDst, AEERect *prcSrc, OverlayLayerType layer, OverlayOpt  *pOpts, OVERLAY_ID oId);
static void OEMOverlay_Remove(IOverlay * pMe, OVERLAY_ID id);
static void OEMOverlay_RemoveAll(IOverlay * pMe, OverlayLayerType layer);
static void OEMOverlay_Update(IOverlay * pMe, OVERLAY_ID id);
static void OEMOverlay_Enable(IOverlay * pMe, OVERLAY_ID id, boolean ovOn);
static void OEMOverlay_EnableAll(IOverlay * pMe, boolean ovOn, OverlayLayerType layer);
static boolean OEMOverlay_Verify(IOverlay *pMe, OVERLAY_ID id);
static void OEMOverlay_Clear(IOverlay *pMe, OVERLAY_ID id);
#endif


struct IOverlay {
   AEEVTBL(IOverlay)      *pvt;
   uint32                 nRefs;
   LCD_TYPE               dest;
   PACONTEXT              pAppContext;   
};

static const VTBL(IOverlay) gOEMOverlayFuncs = {
   OEMOverlay_AddRef,
   OEMOverlay_Release,
   OEMOverlay_QueryInterface,
#ifdef FEATURE_MDP
   OEMOverlay_Add,
   OEMOverlay_Replace,   
   OEMOverlay_Remove,
   OEMOverlay_RemoveAll,
   OEMOverlay_Update,
   OEMOverlay_Enable,
   OEMOverlay_EnableAll,
   OEMOverlay_Verify,
   OEMOverlay_Clear  
#endif 
};
      
int OEMOverlay_New(IShell * piShell, AEECLSID cls, void **ppif)
{
   IOverlay      *pMe = NULL;

   if ((AEECLSID_OVERLAY1 != cls) && (AEECLSID_OVERLAY2 != cls)) {
      return ECLASSNOTSUPPORT;
   }

   pMe = (IOverlay *)MALLOC(sizeof(IOverlay));
   if (NULL == pMe) {
      return ENOMEMORY;
   }

   pMe->pvt = (AEEVTBL(IOverlay) *)&gOEMOverlayFuncs;
   pMe->pAppContext = AEE_GetAppContext();   
   pMe->nRefs = 1;
   
   if (AEECLSID_OVERLAY1 == cls) 
   {
     pMe->dest = PRIMARY_LCD_TYPE;
   }
   else
   {
     pMe->dest = SECONDARY_LCD_TYPE;
   }

   *ppif = pMe;
   return SUCCESS;   
}

static uint32 OEMOverlay_AddRef(IOverlay *pMe)
{
   return ++pMe->nRefs;
}

static uint32 OEMOverlay_Release(IOverlay *pMe)
{  
   uint32 nRefs = (pMe->nRefs ? --pMe->nRefs : 0);

   if (!nRefs) {
      FREE(pMe);
   }

   return nRefs;
}

static int OEMOverlay_QueryInterface(IOverlay *pMe, AEECLSID clsid, void **ppNew)
{
   if (clsid == AEEIID_OVERLAY || clsid == AEECLSID_QUERYINTERFACE) {
      *ppNew = (void*)pMe;
      OEMOverlay_AddRef(pMe);
      return SUCCESS;
   }

   *ppNew = 0;
   return ECLASSNOTSUPPORT;
}

#ifdef FEATURE_MDP
static OVERLAY_ID OEMOverlay_Add(IOverlay * pMe, IBitmap * pbm, AEERect *prcDst,
                     AEERect *prcSrc, OverlayLayerType layer, OverlayOpt  *pOpts)
{
    return OEMOverlay_Replace(pMe, pbm, prcDst, prcSrc, layer, pOpts, NULL);
}

static OVERLAY_ID OEMOverlay_Replace(IOverlay * pMe, IBitmap * pbm, AEERect *prcDst,
                     AEERect *prcSrc, OverlayLayerType layer, OverlayOpt  *pOpts, OVERLAY_ID oId)
{
   MDPIMG mdpImg;
   IDIB *pDib;
	
   //IBITMAP_QueryInterface(pbm, AEECLSID_DIB, (void**)&pDib);	
   pDib = (IDIB *) pbm;
   mdpImg.dest = pMe->dest;		
   if (IDIB_COLORSCHEME_565 == pDib->nColorScheme) {
      mdpImg.imgType = MDP_RGB565;
   } else if (IDIB_COLORSCHEME_YCBCR420 == pDib->nColorScheme) {
      mdpImg.imgType = MDP_YCBCR420;   
   } else {
      return NULL;
   }   
   
   mdpImg.layer = (MDP_LAYER)layer;
   mdpImg.bmy_addr = (uint32 *)pDib->pBmp;
   
   if (MDP_YCBCR420 == mdpImg.imgType){
      mdpImg.cbcr_addr = (uint32 *)((uint32)mdpImg.bmy_addr + pDib->cx*pDib->cy);
   }
   
   mdpImg.width = pDib->cx;
   
   mdpImg.cwin.x1 = prcSrc->x;
   mdpImg.cwin.x2 = prcSrc->x+prcSrc->dx-1;
   mdpImg.cwin.y1 = prcSrc->y;
   mdpImg.cwin.y2 = prcSrc->y+prcSrc->dy-1;
   
   mdpImg.lcd_x = prcDst->x;
   mdpImg.lcd_y = prcDst->y;
   
   mdpImg.mdpOp = MDPOP_NOP;
   
   while ((NULL != pOpts) && (OVERLAYOPT_END != pOpts->nId)) {
      switch (pOpts->nId) {
         case OVERLAYOPT_FNR:
            if (pOpts->val & OVERLAY_FLIP_LR)
               mdpImg.mdpOp |= MDPOP_LR;
            if (pOpts->val & OVERLAY_FLIP_UD)
               mdpImg.mdpOp |= MDPOP_UD;
            if (pOpts->val & OVERLAY_ROT_90)
               mdpImg.mdpOp |= MDPOP_ROT90;
            break;
         
         case OVERLAYOPT_ALPHA:
            mdpImg.mdpOp |= MDPOP_ALPHAB;
            mdpImg.alpha = (pOpts->val*0x100)/100;
            break;
            
         case OVERLAYOPT_TRANSPARENCY:
               mdpImg.mdpOp |= MDPOP_TRANSP;          
               mdpImg.tpVal = pOpts->val;
               break;
            
         default: 
            return EFAILED;
      }
      pOpts++;
   }
   
   // video scale 
   if (MDP_YCBCR420 == mdpImg.imgType) {
      if ((prcDst->dx != prcSrc->dx) || (prcDst->dy != prcSrc->dy)) {
         if ((prcDst->dx == prcSrc->dx*2) && (prcDst->dy == prcSrc->dy*2)) {
             mdpImg.mdpOp |= MDPOP_VDO_X2;
         } else if ((prcDst->dx == prcSrc->dx/4) && (prcDst->dy == prcSrc->dy/4)) {
             mdpImg.mdpOp |= MDPOP_VDO_1BY4;
         } else if ((prcDst->dx == prcSrc->dx/2) && (prcDst->dy == prcSrc->dy/2)) {
             mdpImg.mdpOp |= MDPOP_VDO_1BY2;
         } else if ((prcDst->dx == prcSrc->dx*3/4) && (prcDst->dy == prcSrc->dy*3/4)) {
             mdpImg.mdpOp |= MDPOP_VDO_3BY4;
         }         
      }
   }
      
   return (OVERLAY_ID) mdp_replace_ovimg(&mdpImg, oId);
}

static void OEMOverlay_Remove(IOverlay * pMe, OVERLAY_ID id)
{
   mdp_deregister_ovimg((uint32)id);
}

static void OEMOverlay_RemoveAll(IOverlay * pMe, OverlayLayerType layer)
{
   if (OVERLAY_ALL_LAYER == layer) {
      mdp_deregister_all_ovimg(pMe->dest);
   } 
   else {   
      mdp_deregister_all_ovimg_in_layer(pMe->dest, (MDP_LAYER)layer);   
   }   
}

static void OEMOverlay_Update(IOverlay * pMe, OVERLAY_ID id)
{
   mdp_update_ovimg(id);
}

static void OEMOverlay_Enable(IOverlay * pMe, OVERLAY_ID id, boolean ovOn)
{
   mdp_enable_ovimg(id, ovOn);
}

static void OEMOverlay_EnableAll(IOverlay * pMe, boolean ovOn, OverlayLayerType layer)
{
   if (OVERLAY_ALL_LAYER == layer) {
      mdp_enable_all_ovimg(pMe->dest, ovOn);
   } else {
      mdp_enable_all_ovimg_in_layer(pMe->dest, ovOn, (MDP_LAYER)layer);   
   }      
}

static boolean OEMOverlay_Verify(IOverlay *pMe, OVERLAY_ID id)
{
   return mdp_is_ovimg_valid(id);
}

static void OEMOverlay_Clear(IOverlay *pMe, OVERLAY_ID id)
{
   mdp_clear_ovimg(id);
}

#endif

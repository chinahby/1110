/*======================================================
FILE:  OEMDisplayDev.c

SERVICES:  OEM functions for implementing the IDisplayDev interface.

GENERAL DESCRIPTION:
   This provides a reference implementation for the IDisplayDev
   interface, which is used by OEMBitmap for updating to the display.

        Copyright 2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#include "OEMDisplayDev.h"
#include "OEMBitmapFuncs_priv.h"
#include "AEE_OEM.h"
#include "AEEModTable.h"
#include "AEEStdLib.h"

#ifdef SIM_REND
#include "AEEIDIB.h"
#include "Rendering.h"
#endif

struct IDisplayDev {
   AEEVTBL(IDisplayDev)   *pvt;
   uint32                  nRefs;
   void                   *pif;
};

static int OEMDisplayDev_New(IShell * piShell, AEECLSID cls, void **ppif);
static int OEMBitmapDev_New(IShell * piShell, AEECLSID cls, void **ppif);
static int OEMBitmapDevChild_New(IShell * piShell, AEECLSID cls, void **ppif);
static int OEMSysFont_New_Sim(IShell * piShell, AEECLSID cls, void **ppif);
extern const AEEStaticClass gOEMDisplayDevClasses[];

extern const AEEStaticClass gOEMDisplayDevClasses_Sim[] = {
   {AEECLSID_DEVBITMAP1,      ASCF_UPGRADE, PL_SYSTEM,  0, OEMBitmapDev_New},
   {AEECLSID_DEVBITMAP2,      ASCF_UPGRADE, PL_SYSTEM,  0, OEMBitmapDev_New},
   {AEECLSID_DEVBITMAP3,      ASCF_UPGRADE, PL_SYSTEM,  0, OEMBitmapDev_New},
   {AEECLSID_DEVBITMAP4,      ASCF_UPGRADE, PL_SYSTEM,  0, OEMBitmapDev_New},
   {AEECLSID_DEVBITMAP1_CHILD,ASCF_UPGRADE, PL_SYSTEM,  0, OEMBitmapDevChild_New},
   {AEECLSID_DEVBITMAP2_CHILD,ASCF_UPGRADE, PL_SYSTEM,  0, OEMBitmapDevChild_New},
   {AEECLSID_DEVBITMAP3_CHILD,ASCF_UPGRADE, PL_SYSTEM,  0, OEMBitmapDevChild_New},
   {AEECLSID_DEVBITMAP4_CHILD,ASCF_UPGRADE, PL_SYSTEM,  0, OEMBitmapDevChild_New},
   {AEECLSID_DISPLAYDEV1,     ASCF_UPGRADE, PL_SYSTEM,  0, OEMDisplayDev_New},
   {AEECLSID_DISPLAYDEV2,     ASCF_UPGRADE, PL_SYSTEM,  0, OEMDisplayDev_New},
   {AEECLSID_DISPLAYDEV3,     ASCF_UPGRADE, PL_SYSTEM,  0, OEMDisplayDev_New},
   {AEECLSID_DISPLAYDEV4,     ASCF_UPGRADE, PL_SYSTEM,  0, OEMDisplayDev_New},
   {AEECLSID_FONTSYSNORMAL,   ASCF_UPGRADE, 0,          0, OEMSysFont_New_Sim},
   {AEECLSID_FONTSYSLARGE,    ASCF_UPGRADE, 0,          0, OEMSysFont_New_Sim},
   {AEECLSID_FONTSYSBOLD,     ASCF_UPGRADE, 0,          0, OEMSysFont_New_Sim},
   NULL
};

static uint32 OEMDisplayDev_AddRef(IDisplayDev *pMe);
static uint32 OEMDisplayDev_Release(IDisplayDev *pMe);
static int OEMDisplayDev_QueryInterface(IDisplayDev *pMe, AEECLSID clsid, void **ppNew);
#ifdef SIM_REND
static int OEMDisplayDev_Update1(IDisplayDev *pMe, IBitmap *pbmSrc, AEERect *prc);
#else
static int OEMDisplayDev_Update(IDisplayDev *pMe, IBitmap *pbmSrc, AEERect *prc);
#endif
static const VTBL(IDisplayDev) gOEMDisplayDevFuncs = {
   OEMDisplayDev_AddRef,
   OEMDisplayDev_Release,
   OEMDisplayDev_QueryInterface,
#ifdef SIM_REND
   OEMDisplayDev_Update1,
#else
   OEMDisplayDev_Update,
#endif
};
#ifdef SIM_REND
IBitmap *gpDevBitmap = NULL;
IDisplayDev *gpDevDisp = NULL;
#endif

extern boolean Rendering_IsPlayingEx(DisplayDev eDev);//wlh 20090409 add

extern int OEMDisplayDev_New(IShell * piShell, AEECLSID cls, void **ppif)
{
   IDisplayDev   *pMe;
   int nRet;

   *ppif = NULL;

   if (AEECLSID_DISPLAYDEV1 != cls) {
      return ECLASSNOTSUPPORT;
   }

   pMe = (IDisplayDev*)MALLOC(sizeof(IDisplayDev));
   if (NULL == pMe) {
      return ENOMEMORY;
   }

   pMe->pvt = (AEEVTBL(IDisplayDev) *)&gOEMDisplayDevFuncs;
   pMe->nRefs = 1;

   *ppif = pMe;
   nRet = gOEMDisplayDevClasses[8].pfnNew(piShell,cls,&pMe->pif);
#ifdef SIM_REND
   gpDevDisp = pMe;
#endif
   return nRet;
}

// global device bitmap
extern int OEMBitmapDev_New(IShell * piShell, AEECLSID cls, void **ppif)
{
	int nRet = gOEMDisplayDevClasses[0].pfnNew(piShell,cls,ppif);
#ifdef SIM_REND
	if(AEECLSID_DEVBITMAP1 == cls && nRet == SUCCESS && !gpDevBitmap)
	{
		gpDevBitmap = (IBitmap*)*ppif;
		Rendering_Initlize(DISPLAYDEV_MAIN, ((IDIB *)gpDevBitmap)->pBmp);
	}
#endif
    return nRet;
}

// child device bitmap
extern int OEMBitmapDevChild_New(IShell * piShell, AEECLSID cls, void **ppif)
{
   return gOEMDisplayDevClasses[4].pfnNew(piShell,cls,ppif);
}

static int OEMSysFont_New_Sim(IShell * piShell, AEECLSID cls, void **ppif)
{
   switch (cls) {
   case AEECLSID_FONTSYSNORMAL:
      return OEMFont_GetSystemFont(AEE_FONT_NORMAL, (void **)ppif);
   case AEECLSID_FONTSYSBOLD:
      return OEMFont_GetSystemFont(AEE_FONT_BOLD, (void **)ppif);
   case AEECLSID_FONTSYSLARGE:
      return OEMFont_GetSystemFont(AEE_FONT_LARGE, (void **)ppif);
   default:
      break;
   }
   return gOEMDisplayDevClasses[12].pfnNew(piShell,cls,ppif);
}


static uint32 OEMDisplayDev_AddRef(IDisplayDev *pMe)
{
   return ++pMe->nRefs;
}

static uint32 OEMDisplayDev_Release(IDisplayDev *pMe)
{
   uint32 nRefs = (pMe->nRefs ? --pMe->nRefs : 0);

   if (!nRefs) {
#ifdef SIM_REND
	   Rendering_Release(DISPLAYDEV_MAIN);
#endif
	   IDISPLAYDEV_Release(pMe->pif);
       FREE(pMe);
   }

   return nRefs;
}

static int OEMDisplayDev_QueryInterface(IDisplayDev *pMe, AEECLSID clsid, void **ppNew)
{
   if (clsid == AEEIID_DISPLAYDEV || clsid == AEECLSID_QUERYINTERFACE) {
      *ppNew = (void*)pMe;
      OEMDisplayDev_AddRef(pMe);
      return SUCCESS;
   }

   *ppNew = 0;
   return ECLASSNOTSUPPORT;
}

#ifndef SIM_REND
static 
#endif
int OEMDisplayDev_Update(IDisplayDev *pMe, IBitmap *pbmSrc, AEERect *prc)
{
	return IDISPLAYDEV_Update(pMe->pif,pbmSrc,prc);
}
#ifdef SIM_REND
static int OEMDisplayDev_Update1(IDisplayDev *pMe, IBitmap *pbmSrc, AEERect *prc)
{
	if(0)//wlh add 
	{
	if(SUCCESS == Rendering_Update(DISPLAYDEV_MAIN, (int16)prc->x, (int16)prc->y, (int16)prc->dx, (int16)prc->dy))
	{
		return SUCCESS;
	}
	}
	if(!Rendering_IsPlayingEx(DISPLAYDEV_MAIN))//wlh 20090408 add
    return OEMDisplayDev_Update(pMe, pbmSrc, prc);
	
	return SUCCESS;
}
#endif

#ifdef WIN32 //wlh add
#include "OEMSVC.h"
#include "oemhelperfunctype.h"
static boolean dispInfoInitialized = FALSE;
static disp_info_type dispInfo;

extern int  OEM_SVCdisp_get_info(disp_info_type *pInfo);
void OEMDisp_InitDispInfo(void)
{
   /* Intialize display device info.
   */
   if(!OEM_SVCdisp_get_info(&dispInfo))
   {
#ifdef FEATURE_SECONDARY_DISPLAY
      if(!OEM_SVCdisp_get_info2(&dispInfo2))
      {
         dispInfoInitialized = TRUE;
      }
#else
      dispInfoInitialized = TRUE;
#endif
   }
}

int OEMDisp_GetDispInfo(AEEDeviceInfo * pi)
{

   /* Make sure device info is initialized */
   if (!dispInfoInitialized) OEMDisp_InitDispInfo();

   pi->cxScreen         = dispInfo.disp_width;
   pi->cyScreen         = dispInfo.disp_height;
#ifdef FEATURE_SECONDARY_DISPLAY
   pi->cxAltScreen      = dispInfo2.disp_width;
   pi->cyAltScreen      = dispInfo2.disp_height;
#else
   pi->cxAltScreen      = 0;
   pi->cyAltScreen      = 0;
#endif

   if ( dispInfo.bpp == DISP_16BPP )
   {
      pi->nColorDepth = 16;
   }
   else
   {
      pi->nColorDepth = 8;
   }

   return(SUCCESS);
}
#endif//WIN32

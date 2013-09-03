/*======================================================
FILE:  OEMDisplayDev.c

SERVICES:  OEM functions for implementing the IDisplayDev interface.

GENERAL DESCRIPTION:
   This provides a reference implementation for the IDisplayDev
   interface, which is used by OEMBitmap for updating to the display.

        Copyright 2003, 2004, 2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#include "OEMFeatures.h"
#include "OEMDisplayDev.h"
#include "OEMDispToucan.h"
#include "OEMBitmapFuncs_priv.h"
#include "AEE_OEM.h"
#include "AEEModTable.h"
#include "AEEStdLib.h"
#include "err.h"

#ifndef CUST_EDITION
#ifndef FEATURE_UIONE_HDK
#ifdef FEATURE_UI_CORE
#include "CoreApp.h"
#else /* FEATURE_UI_CORE */
#  include "uistate.h" // DMSS header file
#endif/* FEATURE_UI_CORE */
#endif /* FEATURE_UIONE_HDK */
#endif
#include "disp.h"
#include "OEMSVC.h"
#ifdef FEATURE_UIONE_HDK
#include "ANNUNBITMAP.BID"
#endif // FEATURE_UIONE_HDK

static IBitmap *gpDevBitmap1 = NULL;
#ifdef FEATURE_SECONDARY_DISPLAY
static IBitmap *gpDevBitmap2 = NULL;
#endif

#ifdef FEATURE_UIONE_HDK
static IBitmap *gpAnnunBitmap = NULL;
static boolean gbAnnun1Enabled = FALSE;
#endif // FEATURE_UIONE_HDK

static boolean dispInfoInitialized = FALSE;
static disp_info_type dispInfo;
#ifdef FEATURE_SECONDARY_DISPLAY
static disp_info_type dispInfo2;     // Secondary display, if present
#endif
static boolean gbInit = 0;
static AEECallback gCallback;

#ifdef FEATURE_MMOVERLAY
#include "mdp_api.h"
#include "mdp_sys_api.h"
#include "AEECLSID_SysMMOverlayRegistry.bid"
#include "AEESysMMOverlayRegistry.h"
#include "cache_mmu.h"

#ifdef FEATURE_MDP2
static void OEMDIBDisplay_FlushCache(MDPIMG *pMDPImage)
{
   uint16 inpBpp;
   uint32 src_width = pMDPImage->width;
   switch (pMDPImage->imgType)
   {
      case MDP_RGB_565:
         inpBpp = 2;
         break;
      case MDP_RGB_888:
      case MDP_ARGB_8888:
      case MDP_XRGB_8888:
         inpBpp = 4;
         break;

      default: inpBpp = 0;
   }

   if (inpBpp != 0)
   {
      uint8 * cache_addr;
      if (NULL != pMDPImage->bmy_addr)
      {
         cache_addr = (uint8 *)pMDPImage->bmy_addr +
                      (pMDPImage->cwin.y1*src_width)*inpBpp;
         dcache_clean_region((void *)cache_addr,
                             (src_width*inpBpp) *
                             (pMDPImage->cwin.y2-pMDPImage->cwin.y1+1));
      }
   }
}
#endif /* FEATURE_MDP2 */
#endif /* FEATURE_MMOVERLAY */

struct IDisplayDev {
   AEEVTBL(IDisplayDev)   *pvt;
   uint32                  nRefs;
#ifdef FEATURE_MMOVERLAY
   ISysMMOverlayRegistry            *pSysMMOverlayRegistry;
   AEECLSID               displayClsID;
   MDPIMG                 mdpImg;
   uint32                 mdpOvID;
   boolean                bmdpOvEnabled;
#endif
};

const unsigned long int system_palette[256] =
{
  0x00000000, 0x00000080, 0x00008000, 0x00008080, 0x00800000, 0x00800080, 0x00808000, 0x00c0c0c0,
  0x00c0dcc0, 0x00f0caa6, 0x00002040, 0x00002060, 0x00002080, 0x000020a0, 0x000020c0, 0x000020e0,
  0x00004000, 0x00004020, 0x00004040, 0x00004060, 0x00004080, 0x000040a0, 0x000040c0, 0x000040e0,
  0x00006000, 0x00006020, 0x00006040, 0x00006060, 0x00006080, 0x000060a0, 0x000060c0, 0x000060e0,
  0x00008000, 0x00008020, 0x00008040, 0x00008060, 0x00008080, 0x000080a0, 0x000080c0, 0x000080e0,
  0x0000a000, 0x0000a020, 0x0000a040, 0x0000a060, 0x0000a080, 0x0000a0a0, 0x0000a0c0, 0x0000a0e0,
  0x0000c000, 0x0000c020, 0x0000c040, 0x0000c060, 0x0000c080, 0x0000c0a0, 0x0000c0c0, 0x0000c0e0,
  0x0000e000, 0x0000e020, 0x0000e040, 0x0000e060, 0x0000e080, 0x0000e0a0, 0x0000e0c0, 0x0000e0e0,
  0x00400000, 0x00400020, 0x00400040, 0x00400060, 0x00400080, 0x004000a0, 0x004000c0, 0x004000e0,
  0x00402000, 0x00402020, 0x00402040, 0x00402060, 0x00402080, 0x004020a0, 0x004020c0, 0x004020e0,
  0x00404000, 0x00404020, 0x00404040, 0x00404060, 0x00404080, 0x004040a0, 0x004040c0, 0x004040e0,
  0x00406000, 0x00406020, 0x00406040, 0x00406060, 0x00406080, 0x004060a0, 0x004060c0, 0x004060e0,
  0x00408000, 0x00408020, 0x00408040, 0x00408060, 0x00408080, 0x004080a0, 0x004080c0, 0x004080e0,
  0x0040a000, 0x0040a020, 0x0040a040, 0x0040a060, 0x0040a080, 0x0040a0a0, 0x0040a0c0, 0x0040a0e0,
  0x0040c000, 0x0040c020, 0x0040c040, 0x0040c060, 0x0040c080, 0x0040c0a0, 0x0040c0c0, 0x0040c0e0,
  0x0040e000, 0x0040e020, 0x0040e040, 0x0040e060, 0x0040e080, 0x0040e0a0, 0x0040e0c0, 0x0040e0e0,
  0x00800000, 0x00800020, 0x00800040, 0x00800060, 0x00800080, 0x008000a0, 0x008000c0, 0x008000e0,
  0x00802000, 0x00802020, 0x00802040, 0x00802060, 0x00802080, 0x008020a0, 0x008020c0, 0x008020e0,
  0x00804000, 0x00804020, 0x00804040, 0x00804060, 0x00804080, 0x008040a0, 0x008040c0, 0x008040e0,
  0x00806000, 0x00806020, 0x00806040, 0x00806060, 0x00806080, 0x008060a0, 0x008060c0, 0x008060e0,
  0x00808000, 0x00808020, 0x00808040, 0x00808060, 0x00808080, 0x008080a0, 0x008080c0, 0x008080e0,
  0x0080a000, 0x0080a020, 0x0080a040, 0x0080a060, 0x0080a080, 0x0080a0a0, 0x0080a0c0, 0x0080a0e0,
  0x0080c000, 0x0080c020, 0x0080c040, 0x0080c060, 0x0080c080, 0x0080c0a0, 0x0080c0c0, 0x0080c0e0,
  0x0080e000, 0x0080e020, 0x0080e040, 0x0080e060, 0x0080e080, 0x0080e0a0, 0x0080e0c0, 0x0080e0e0,
  0x00c00000, 0x00c00020, 0x00c00040, 0x00c00060, 0x00c00080, 0x00c000a0, 0x00c000c0, 0x00c000e0,
  0x00c02000, 0x00c02020, 0x00c02040, 0x00c02060, 0x00c02080, 0x00c020a0, 0x00c020c0, 0x00c020e0,
  0x00c04000, 0x00c04020, 0x00c04040, 0x00c04060, 0x00c04080, 0x00c040a0, 0x00c040c0, 0x00c040e0,
  0x00c06000, 0x00c06020, 0x00c06040, 0x00c06060, 0x00c06080, 0x00c060a0, 0x00c060c0, 0x00c060e0,
  0x00c08000, 0x00c08020, 0x00c08040, 0x00c08060, 0x00c08080, 0x00c080a0, 0x00c080c0, 0x00c080e0,
  0x00c0a000, 0x00c0a020, 0x00c0a040, 0x00c0a060, 0x00c0a080, 0x00c0a0a0, 0x00c0a0c0, 0x00c0a0e0,
  0x00c0c000, 0x00c0c020, 0x00c0c040, 0x00c0c060, 0x00c0c080, 0x00c0c0a0, 0x00f0fbff, 0x00a4a0a0,
  0x00808080, 0x000000ff, 0x0000ff00, 0x0000ffff, 0x00ff0000, 0x00ff00ff, 0x00ffff00, 0x00ffffff
};

unsigned short gPalette8To16Tbl[256] = {
  0x0000, 0x8000, 0x0400, 0x8400, 0x0010, 0x8010, 0x0410, 0xC618,
  0xC6F8, 0xA65E, 0x4100, 0x6100, 0x8100, 0xA100, 0xC100, 0xE100,
  0x0200, 0x2200, 0x4200, 0x6200, 0x8200, 0xA200, 0xC200, 0xE200,
  0x0300, 0x2300, 0x4300, 0x6300, 0x8300, 0xA300, 0xC300, 0xE300,
  0x0400, 0x2400, 0x4400, 0x6400, 0x8400, 0xA400, 0xC400, 0xE400,
  0x0500, 0x2500, 0x4500, 0x6500, 0x8500, 0xA500, 0xC500, 0xE500,
  0x0600, 0x2600, 0x4600, 0x6600, 0x8600, 0xA600, 0xC600, 0xE600,
  0x0700, 0x2700, 0x4700, 0x6700, 0x8700, 0xA700, 0xC700, 0xE700,
  0x0008, 0x2008, 0x4008, 0x6008, 0x8008, 0xA008, 0xC008, 0xE008,
  0x0108, 0x2108, 0x4108, 0x6108, 0x8108, 0xA108, 0xC108, 0xE108,
  0x0208, 0x2208, 0x4208, 0x6208, 0x8208, 0xA208, 0xC208, 0xE208,
  0x0308, 0x2308, 0x4308, 0x6308, 0x8308, 0xA308, 0xC308, 0xE308,
  0x0408, 0x2408, 0x4408, 0x6408, 0x8408, 0xA408, 0xC408, 0xE408,
  0x0508, 0x2508, 0x4508, 0x6508, 0x8508, 0xA508, 0xC508, 0xE508,
  0x0608, 0x2608, 0x4608, 0x6608, 0x8608, 0xA608, 0xC608, 0xE608,
  0x0708, 0x2708, 0x4708, 0x6708, 0x8708, 0xA708, 0xC708, 0xE708,
  0x0010, 0x2010, 0x4010, 0x6010, 0x8010, 0xA010, 0xC010, 0xE010,
  0x0110, 0x2110, 0x4110, 0x6110, 0x8110, 0xA110, 0xC110, 0xE110,
  0x0210, 0x2210, 0x4210, 0x6210, 0x8210, 0xA210, 0xC210, 0xE210,
  0x0310, 0x2310, 0x4310, 0x6310, 0x8310, 0xA310, 0xC310, 0xE310,
  0x0410, 0x2410, 0x4410, 0x6410, 0x8410, 0xA410, 0xC410, 0xE410,
  0x0510, 0x2510, 0x4510, 0x6510, 0x8510, 0xA510, 0xC510, 0xE510,
  0x0610, 0x2610, 0x4610, 0x6610, 0x8610, 0xA610, 0xC610, 0xE610,
  0x0710, 0x2710, 0x4710, 0x6710, 0x8710, 0xA710, 0xC710, 0xE710,
  0x0018, 0x2018, 0x4018, 0x6018, 0x8018, 0xA018, 0xC018, 0xE018,
  0x0118, 0x2118, 0x4118, 0x6118, 0x8118, 0xA118, 0xC118, 0xE118,
  0x0218, 0x2218, 0x4218, 0x6218, 0x8218, 0xA218, 0xC218, 0xE218,
  0x0318, 0x2318, 0x4318, 0x6318, 0x8318, 0xA318, 0xC318, 0xE318,
  0x0418, 0x2418, 0x4418, 0x6418, 0x8418, 0xA418, 0xC418, 0xE418,
  0x0518, 0x2518, 0x4518, 0x6518, 0x8518, 0xA518, 0xC518, 0xE518,
  0x0618, 0x2618, 0x4618, 0x6618, 0x8618, 0xA618, 0xFFDE, 0xA514,
  0x8410, 0xF800, 0x07E0, 0xFFE0, 0x001F, 0xF81F, 0x07FF, 0xFFFF
};

/* This table rounds up or down an 8-bit color to 2, 3, 5, or 6 bits
** with proper round up or down WRT the LSBs.
*/
#ifdef DISP_DEVICE_16BPP
unsigned char roundTo5[256];
unsigned char roundTo6[256];
#elif defined DISP_DEVICE_8BPP
unsigned char roundTo2[256];
unsigned char roundTo3[256];
#endif /* DISP_DEVICE_8 */


extern void OEMDisplayDev_AnnunBar1_GetExtent(int16 *dx, int16 *dy);

static void Display_Init(IShell *pShell);

int OEMDisp_GetDispInfo(AEEDeviceInfo * pi);
void OEMDisp_InitDispInfo(void);

void OEM_InitDispInfo(void)
{
   OEMDisp_InitDispInfo();
}

static void OEMBitmapDev_Init(IShell *ps);
static void OEMBitmapDev_Cleanup(void *pData);
#ifndef FEATURE_RANDOM_MENU_REND
static int OEMDisplayDev_New(IShell * piShell, AEECLSID cls, void **ppif);
#endif
static int OEMBitmapDev_New(IShell * piShell, AEECLSID cls, void **ppif);
static int OEMBitmapDevChild_New(IShell * piShell, AEECLSID cls, void **ppif);
static int OEMSysFont_New(IShell * piShell, AEECLSID cls, void **ppif);


int SVC_disp_update(IDIB *pDib, const AEERect *prc)
{
#ifndef FEATURE_UIONE_HDK
#endif // !FEATURE_UIONE_HDK

#ifndef CUST_EDITION
   // Temporary workaround to prevent BREW updating UI screen.
#ifndef FEATURE_UIONE_HDK
#ifdef FEATURE_UI_CORE
    if (CoreApp_IsSKUI() == TRUE) return 0;
#else
    if (ui_current_state() != UI_BREW_S) return 0;
#endif /* FEATURE_UI_CORE */
#endif /* FEATURE_UIONE_HDK */
#endif
   if ((IDIB*)0 == pDib || (AEERect*)0 == prc) {
      return 0;
   }

   if (0 == pDib->pBmp || pDib->cx > pDib->nPitch) {
      return 0;
   }

   if (prc->x < 0 || prc->y < 0 || prc->dx < 0 || prc->dy < 0 ||
       prc->x + prc->dx > pDib->cx || prc->y + prc->dy > pDib->cy) {
      return 0;
   }
   {
     if (pDib == (IDIB *)gpDevBitmap1)
     {
        uint16 width = pDib->nPitch / 2; /* 16bpp hardcoded ? */

        if ((prc->dx > 0) && (prc->dy > 0)) {
           /* Display image*/
           disp_update(pDib->pBmp,
                       width,
                       prc->y, prc->x,
                       prc->dy, prc->dx,
                       prc->y, prc->x);
        }
     }
#ifdef FEATURE_SECONDARY_DISPLAY
     else if (pDib == (IDIB *)gpDevBitmap2)
     {
        uint16 width = pDib->nPitch / 2; /* 16bpp hardcoded ? */
        if ((prc->dx > 0) && (prc->dy > 0)) {
           /* Display image*/
           disp_update2(pDib->pBmp,
                       width,
                       prc->y, prc->x,
                       prc->dy, prc->dx,
                       prc->y, prc->x);
        }

     }
#endif
   }

   return 0;
}

#ifdef FEATURE_GREYBIT
extern int GreyBitBrewFont_New(IShell *piShell, AEECLSID cls, void **ppif);
#endif
#ifdef FEATURE_RANDOM_MENU_REND
extern int OEMDisplayDevRend_New(IShell *piShell, AEECLSID cls, void **ppif);
#endif
extern const AEEStaticClass gOEMDisplayDevClasses[] = {
   {AEECLSID_DEVBITMAP1,      ASCF_UPGRADE, PL_SYSTEM,  Display_Init, OEMBitmapDev_New},
   {AEECLSID_DEVBITMAP2,      ASCF_UPGRADE, PL_SYSTEM,  0, OEMBitmapDev_New},
   {AEECLSID_DEVBITMAP3,      ASCF_UPGRADE, PL_SYSTEM,  0, OEMBitmapDev_New},
   {AEECLSID_DEVBITMAP4,      ASCF_UPGRADE, PL_SYSTEM,  0, OEMBitmapDev_New},
#ifdef FEATURE_UIONE_HDK
   {AEECLSID_ANNUNBITMAP,     ASCF_UPGRADE, PL_SYSTEM,  0, OEMBitmapDev_New},
#endif // FEATURE_UIONE_HDK
   {AEECLSID_DEVBITMAP1_CHILD,ASCF_UPGRADE, PL_SYSTEM,  0, OEMBitmapDevChild_New},
   {AEECLSID_DEVBITMAP2_CHILD,ASCF_UPGRADE, PL_SYSTEM,  0, OEMBitmapDevChild_New},
   {AEECLSID_DEVBITMAP3_CHILD,ASCF_UPGRADE, PL_SYSTEM,  0, OEMBitmapDevChild_New},
   {AEECLSID_DEVBITMAP4_CHILD,ASCF_UPGRADE, PL_SYSTEM,  0, OEMBitmapDevChild_New},
#ifdef FEATURE_RANDOM_MENU_REND
   {AEECLSID_DISPLAYDEV1,     ASCF_UPGRADE, PL_SYSTEM,  0, OEMDisplayDevRend_New},
   {AEECLSID_DISPLAYDEV2,     ASCF_UPGRADE, PL_SYSTEM,  0, OEMDisplayDevRend_New},
   {AEECLSID_DISPLAYDEV3,     ASCF_UPGRADE, PL_SYSTEM,  0, OEMDisplayDevRend_New},
   {AEECLSID_DISPLAYDEV4,     ASCF_UPGRADE, PL_SYSTEM,  0, OEMDisplayDevRend_New},
#else
   {AEECLSID_DISPLAYDEV1,     ASCF_UPGRADE, PL_SYSTEM,  0, OEMDisplayDev_New},
   {AEECLSID_DISPLAYDEV2,     ASCF_UPGRADE, PL_SYSTEM,  0, OEMDisplayDev_New},
   {AEECLSID_DISPLAYDEV3,     ASCF_UPGRADE, PL_SYSTEM,  0, OEMDisplayDev_New},
   {AEECLSID_DISPLAYDEV4,     ASCF_UPGRADE, PL_SYSTEM,  0, OEMDisplayDev_New},
#endif
#ifdef FEATURE_GREYBIT
   {AEECLSID_FONTSYSNORMAL,   ASCF_UPGRADE, 0,          0, GreyBitBrewFont_New},
   {AEECLSID_FONTSYSLARGE,    ASCF_UPGRADE, 0,          0, GreyBitBrewFont_New},
   {AEECLSID_FONTSYSBOLD,     ASCF_UPGRADE, 0,          0, GreyBitBrewFont_New},
   {AEECLSID_FONTSYSBIGNUMBER,ASCF_UPGRADE, 0,          0, GreyBitBrewFont_New},
   {AEECLAID_FONTSMALL,       ASCF_UPGRADE, 0,          0, GreyBitBrewFont_New},
   {AEECLSID_FONTSYSITALIC,   ASCF_UPGRADE, 0,          0, GreyBitBrewFont_New},
#else
   {AEECLSID_FONTSYSNORMAL,   ASCF_UPGRADE, 0,          0, OEMSysFont_New},
   {AEECLSID_FONTSYSLARGE,    ASCF_UPGRADE, 0,          0, OEMSysFont_New},
   {AEECLSID_FONTSYSBOLD,     ASCF_UPGRADE, 0,          0, OEMSysFont_New},
   {AEECLAID_FONTSMALL,       ASCF_UPGRADE, 0,          0, OEMSysFont_New},
   {AEECLSID_FONTSYSITALIC,   ASCF_UPGRADE, 0,          0, OEMSysFont_New},
#endif
   NULL
};

static uint32 OEMDisplayDev_AddRef(IDisplayDev *pMe);
static uint32 OEMDisplayDev_Release(IDisplayDev *pMe);
static int OEMDisplayDev_QueryInterface(IDisplayDev *pMe, AEECLSID clsid, void **ppNew);
static int OEMDisplayDev_Update(IDisplayDev *pMe, IBitmap *pbmSrc, AEERect *prc);

static const VTBL(IDisplayDev) gOEMDisplayDevFuncs = {
   OEMDisplayDev_AddRef,
   OEMDisplayDev_Release,
   OEMDisplayDev_QueryInterface,
   OEMDisplayDev_Update,
};


static void Display_Init(IShell *pShell)
{
      /* Used to generate the round tables that convert 8-bit color into
      ** 2, 3, 5 or 6 bits with proper round up or down WRT the LSBs
      */
      int i;
      for (i = 0; i < 256; i++)
      {
#  ifdef DISP_DEVICE_16BPP
        if (i < 0xF8) {
          roundTo5[i] = (i + 0x04) >> 3;
        }
        else {
          roundTo5[i] = i >> 3;
        }
        if (i < 0xFC) {
          roundTo6[i] = (i + 0x02) >> 2;
        }
        else {
          roundTo6[i] = i >> 2;
        }
#  elif defined DISP_DEVICE_8BPP
        if (i < 0xC0) {
          roundTo2[i] = (i + 0x20) >> 6;
        }
        else {
          roundTo2[i] = i >> 6;
        }
        if (i < 0xE0) {
          roundTo3[i] = (i + 0x10) >> 5;
        }
        else {
          roundTo3[i] = i >> 5;
        }
#  endif /* DISP_DEVICE_16BPP */
      }
}

extern void OEMBitmapDev_Init(IShell *ps)
{
   if (!gbInit) {
      CALLBACK_Init(&gCallback, OEMBitmapDev_Cleanup, NULL);
      ISHELL_RegisterSystemCallback(ps, &gCallback, AEE_SCB_AEE_EXIT);
      gbInit = TRUE;
   }
}

extern void OEMBitmapDev_Cleanup(void *pData)
{
   if (gpDevBitmap1) {
      IBITMAP_Release(gpDevBitmap1);
      gpDevBitmap1 = NULL;
   }
#ifdef FEATURE_SECONDARY_DISPLAY
   if (gpDevBitmap2) {
      IBITMAP_Release(gpDevBitmap2);
      gpDevBitmap2 = NULL;
   }
#endif
#ifdef FEATURE_UIONE_HDK
   if (gpAnnunBitmap) {
      IBITMAP_Release(gpAnnunBitmap);
      gpAnnunBitmap = NULL;
   }
#endif // FEATURE_UIONE_HDK
   gbInit = FALSE;
}

#ifdef FEATURE_UIONE_HDK
/*=============================================================================
FUNCTION: OEMDisplayDev_AnnunBar1_GetExtent
DESCRIPTION:  Gets the extent of the annunciator region
PARAMETERS:
   *dx  [out]:  Height of the annunciator region.
   *dy  [out]:  Width of the annunciator region.
RETURN VALUE:
   None
COMMENTS:
SIDE EFFECTS:
SEE ALSO:
=============================================================================*/
void OEMDisplayDev_AnnunBar1_GetExtent(int16 *dx, int16 *dy)
{
   if (dx != NULL) {
      *dx = dispInfo.disp_width;
   }
   if (dy != NULL) {
      *dy = 30;
   }
}

/*=============================================================================
FUNCTION: OEMDisplayDev_AnnunBar1_Draw
DESCRIPTION:  Draws the annunciator region
PARAMETERS:
    bUpdateDisplay  [in]:  TRUE if the display should be updated
RETURN VALUE:
   None
COMMENTS:
SIDE EFFECTS:
SEE ALSO:
=============================================================================*/
static void OEMDisplayDev_AnnunBar1_Draw(boolean bUpdateDisplay)
{
   // Check if we have a bitmap to draw to.
   if (gbAnnun1Enabled) {
      // The AnnunBar trig draws on the AnnunBitmap
      if (gpAnnunBitmap != NULL) {
         int16 dx, dy;
         OEMDisplayDev_AnnunBar1_GetExtent(&dx, &dy);
         // Blit the annunciator data from the Annun bitmap onto the annunciator
         // area on the bitmap
         IBITMAP_BltIn(gpDevBitmap1, 0, 0, dx, dy, gpAnnunBitmap, 0, 0, AEE_RO_COPY); // Force a display update if requested.
         if (bUpdateDisplay) {
            IBitmapDev *pBitmapDev = NULL;
            (void) IBITMAP_QueryInterface(
               gpDevBitmap1,
               AEEIID_BITMAPDEV,
               (void **) &pBitmapDev
            );
            (void) IBITMAPDEV_Update(pBitmapDev);
            IBITMAPDEV_Release(pBitmapDev);
            pBitmapDev = NULL;
         }
      }
   }
}

/*=============================================================================
FUNCTION: OEMDisplayDev_AnnunBar1_Activate
DESCRIPTION:  Activates the annunciator region on the given bitmap.
PARAMETERS:
   *pBitmap  [in]:  Bitmap to draw on.
RETURN VALUE:
   None
COMMENTS:
SIDE EFFECTS:
SEE ALSO:
=============================================================================*/
void OEMDisplayDev_AnnunBar1_Activate(void)
{
   gbAnnun1Enabled = TRUE;
   OEMDisplayDev_AnnunBar1_Draw(FALSE);
}

/*=============================================================================
FUNCTION: OEMDisplayDev_AnnunBar1_Deactivate
DESCRIPTION:  Deactivates the annunciator region.
PARAMETERS:
   None
RETURN VALUE:
   None
COMMENTS:
SIDE EFFECTS:
SEE ALSO:
=============================================================================*/
void OEMDisplayDev_AnnunBar1_Deactivate(void)
{
   gbAnnun1Enabled = FALSE;
}
static uint32 OEMDisplayDev_AnnunBar1DisplayDev_AddRef(IDisplayDev *pMe)
{
   return ++pMe->nRefs;
}
static uint32 OEMDisplayDev_AnnunBar1DisplayDev_Release(IDisplayDev *pMe)
{
   uint32 nRefs = (pMe->nRefs ? --pMe->nRefs : 0);
   if (!nRefs) {
      FREE(pMe);
   }
   return nRefs;
}
static int OEMDisplayDev_AnnunBar1DisplayDev_QueryInterface(IDisplayDev *pMe, AEECLSID clsid, void **ppNew)
{
   if (clsid == AEEIID_DISPLAYDEV || clsid == AEECLSID_QUERYINTERFACE) {
      *ppNew = (void*)pMe;
      OEMDisplayDev_AnnunBar1DisplayDev_AddRef(pMe);
      return SUCCESS;
   }
   *ppNew = 0;
   return ECLASSNOTSUPPORT;
}
static int OEMDisplayDev_AnnunBar1DisplayDev_Update(IDisplayDev *pMe, IBitmap *pbmSrc, AEERect *prc)
{
   OEMDisplayDev_AnnunBar1_Draw(TRUE);
   return SUCCESS;
}

/*=============================================================================
FUNCTION: OEMDisplayDev_AnnunBar1DisplayDev_New
DESCRIPTION:  Creates an IDisplayDev interface for the annunciator
              psuedo-display
PARAMETERS:
   *piShell  [in]:
  **ppif    [out]:
RETURN VALUE:
   int:
COMMENTS:
SIDE EFFECTS:
SEE ALSO:
=============================================================================*/
int OEMDisplayDev_AnnunBar1DisplayDev_New(IShell * piShell, void **ppif)
{
   static const VTBL(IDisplayDev) gOEMDisplayDev_AnnunBar1DisplayDevFuncs = {
      OEMDisplayDev_AnnunBar1DisplayDev_AddRef,
      OEMDisplayDev_AnnunBar1DisplayDev_Release,
      OEMDisplayDev_AnnunBar1DisplayDev_QueryInterface,
      OEMDisplayDev_AnnunBar1DisplayDev_Update,
   };
   IDisplayDev   *pMe;
   *ppif = NULL;

   pMe = (IDisplayDev*) MALLOC(sizeof(IDisplayDev));
   if (NULL == pMe) {
      return ENOMEMORY;
   }
   pMe->pvt = (AEEVTBL(IDisplayDev) *)&gOEMDisplayDev_AnnunBar1DisplayDevFuncs;
   pMe->nRefs = 1;
   *ppif = pMe;
   return SUCCESS;
}
#endif //FEATURE_UIONE_HDK

extern int OEMDisplayDev_New(IShell * piShell, AEECLSID cls, void **ppif)
{
   IDisplayDev   *pMe;

   *ppif = NULL;

   if ((AEECLSID_DISPLAYDEV1 != cls) && (AEECLSID_DISPLAYDEV2 != cls)) {
      return ECLASSNOTSUPPORT;
   }

   pMe = (IDisplayDev*)MALLOC(sizeof(IDisplayDev));
   if (NULL == pMe) {
      return ENOMEMORY;
   }

   pMe->pvt = (AEEVTBL(IDisplayDev) *)&gOEMDisplayDevFuncs;
   pMe->nRefs = 1;

#ifdef FEATURE_MMOVERLAY

   pMe->mdpOvID = 0;
   switch(cls)
   {
      case AEECLSID_DISPLAYDEV1:
         pMe->displayClsID = AEECLSID_DISPLAY1;
         break;
      case AEECLSID_DISPLAYDEV2:
         pMe->displayClsID = AEECLSID_DISPLAY2;
         break;
      case AEECLSID_DISPLAYDEV3:
         pMe->displayClsID = AEECLSID_DISPLAY3;
         break;
      case AEECLSID_DISPLAYDEV4:
         pMe->displayClsID = AEECLSID_DISPLAY4;
         break;
      default:
         pMe->displayClsID = 0;
   }

   if(0 != pMe->displayClsID)
   {
      if(SUCCESS != ISHELL_CreateInstance(piShell, AEECLSID_SysMMOverlayRegistry, (void*)&pMe->pSysMMOverlayRegistry))
      {
         pMe->pSysMMOverlayRegistry = NULL;
      }
   }

#endif

   *ppif = pMe;
   return SUCCESS;
}

// global device bitmap
extern int OEMBitmapDev_New(IShell * piShell, AEECLSID cls, void **ppif)
{
   AEEDeviceInfo  di;
   IDisplayDev   *pDispDev = NULL;
   int            nErr = SUCCESS;

   OEMDisp_GetDispInfo(&di);

   if ((AEECLSID_DEVBITMAP1 != cls)
#ifdef FEATURE_SECONDARY_DISPLAY
       && (AEECLSID_DEVBITMAP2 != cls)
#endif
#ifdef FEATURE_UIONE_HDK
       && (AEECLSID_ANNUNBITMAP != cls)
#endif // FEATURE_UIONE_HDK
       ) {
      return ECLASSNOTSUPPORT;
   }

   OEMBitmapDev_Init(piShell);


   switch(cls)
   {
      case AEECLSID_DEVBITMAP1:
         if (!gpDevBitmap1) {
            nErr = AEE_CreateInstanceSys(AEECLSID_DISPLAYDEV1,
                                         (void**)&pDispDev);
            if (SUCCESS != nErr) {
               return nErr;
            }

#ifdef DISP_DEVICE_16BPP
            nErr = OEMBitmap16_NewEx(di.cxScreen, di.cyScreen, NULL, NULL,
                                     pDispDev, (IBitmap**)ppif);
#else /* DISP_DEVICE_16BPP */
            nErr = OEMBitmap18_NewEx(di.cxScreen, di.cyScreen, NULL, NULL,
                                     pDispDev, (IBitmap**)ppif);
#endif  /* DISP_DEVICE_16BPP */
            if (SUCCESS != nErr) {
               goto Error;
            }
#ifdef GRP_ENABLED
            grp_register_display_buffer(ppif->pIBitmap->m_bmp.pBmp);
#endif /* GRP_ENABLED */

            gpDevBitmap1 = (IBitmap*)*ppif;
         } else {
            *ppif = gpDevBitmap1;
         }

         IBITMAP_AddRef(((IBitmap*)*ppif));
         break;

#ifdef FEATURE_SECONDARY_DISPLAY
      case AEECLSID_DEVBITMAP2:
         if (!gpDevBitmap2) {
            nErr = AEE_CreateInstanceSys(AEECLSID_DISPLAYDEV2,
                                         (void**)&pDispDev);
            if (SUCCESS != nErr) {
               return nErr;
            }

#ifdef DISP2_DEVICE_16BPP
            nErr = OEMBitmap16_NewEx(di.cxAltScreen, di.cyAltScreen, NULL, NULL,
                                     pDispDev, (IBitmap**)ppif);
#else /* DISP2_DEVICE_16BPP */
            nErr = OEMBitmap18_NewEx(di.cxAltScreen, di.cyAltScreen, NULL, NULL,
                                     pDispDev, (IBitmap**)ppif);
#endif  /* DISP2_DEVICE_16BPP */
            if (SUCCESS != nErr) {
               goto Error;
            }
#ifdef GRP_ENABLED
            grp_register_display_buffer(ppif->pIBitmap->m_bmp.pBmp);
#endif /* GRP_ENABLED */

            gpDevBitmap2 = (IBitmap*)*ppif;
         } else {
            *ppif = gpDevBitmap2;
         }

         IBITMAP_AddRef(((IBitmap*)*ppif));
         break;
#endif // FEATURE_SECONDARY_DISPLAY
#ifdef FEATURE_UIONE_HDK
      case AEECLSID_ANNUNBITMAP:
         if (!gpAnnunBitmap) {
            int16 dx, dy;
            OEMDisplayDev_AnnunBar1_GetExtent(&dx, &dy);
            nErr = OEMDisplayDev_AnnunBar1DisplayDev_New(piShell,
                                                         (void**)&pDispDev);
            if (SUCCESS != nErr) {
               return nErr;
            }
#ifdef DISP_DEVICE_16BPP
            nErr = OEMBitmap16_NewEx(di.cxScreen,
                                     (uint16)(di.cyScreen/2),
                                     NULL, NULL,
                                     pDispDev, (IBitmap**)ppif);
#else /* DISP_DEVICE_16BPP */
            nErr = OEMBitmap18_NewEx(di.cxScreen,
                                     (uint16)(di.cyScreen/2),
                                     NULL, NULL,
                                     pDispDev, (IBitmap**)ppif);
#endif  /* DISP_DEVICE_16BPP */
            if (SUCCESS != nErr) {
               goto Error;
            }
            gpAnnunBitmap = (IBitmap*)*ppif;
         } else {
            *ppif = gpAnnunBitmap;
         }
         IBITMAP_AddRef(((IBitmap*)*ppif));
         break;
#endif // FEATURE_UIONE_HDK

      default:
         nErr = ECLASSNOTSUPPORT;
         break;
   }

Error:
   if (pDispDev) {
      IDISPLAYDEV_Release(pDispDev);
   }
   return nErr;
}



// child device bitmap
extern int OEMBitmapDevChild_New(IShell * piShell, AEECLSID cls, void **ppif)
{
   IBitmap    *pDevBitmap = NULL;
   ACONTEXT   *pac;
   int         nErr;

   *ppif = NULL;

   if ((AEECLSID_DEVBITMAP1_CHILD != cls)
#ifdef FEATURE_SECONDARY_DISPLAY
        && (AEECLSID_DEVBITMAP2_CHILD != cls)
#endif
       ) {
      return ECLASSNOTSUPPORT;
   }

   switch(cls)
   {
      case AEECLSID_DEVBITMAP1_CHILD:
         pac = AEE_EnterAppContext(NULL);
         nErr = AEE_CreateInstanceSys(AEECLSID_DEVBITMAP1, (void**)&pDevBitmap);
         AEE_LeaveAppContext(pac);
         if (SUCCESS != nErr) {
            return nErr;
         }

#ifdef DISP_DEVICE_16BPP
         nErr = OEMBitmap16_New_Child(pDevBitmap, NULL, (IBitmap**)ppif);
#else /* DISP_DEVICE_18BPP */
         nErr = OEMBitmap18_New_Child(pDevBitmap, NULL, (IBitmap**)ppif);
#endif /* DISP_DEVICE_18BPP */
         if (SUCCESS != nErr) {
            goto Error;
         }
         break;
#ifdef FEATURE_SECONDARY_DISPLAY
      case AEECLSID_DEVBITMAP2_CHILD:
         pac = AEE_EnterAppContext(NULL);
         nErr = AEE_CreateInstanceSys(AEECLSID_DEVBITMAP2, (void**)&pDevBitmap);
         AEE_LeaveAppContext(pac);
         if (SUCCESS != nErr) {
            return nErr;
         }

#ifdef DISP2_DEVICE_16BPP
         nErr = OEMBitmap16_New_Child(pDevBitmap, NULL, (IBitmap**)ppif);
#else /* DISP2_DEVICE_18BPP */
         nErr = OEMBitmap18_New_Child(pDevBitmap, NULL, (IBitmap**)ppif);
#endif /* DISP2_DEVICE_18BPP */
         if (SUCCESS != nErr) {
            goto Error;
         }
         break;
#endif
      default:
         nErr = ECLASSNOTSUPPORT;
         break;
   }
Error:
   if (pDevBitmap) {
      IBITMAP_Release(pDevBitmap);
   }
   return nErr;
}

#ifndef FEATURE_GREYBIT
extern int OEMFont_GetSystemFont(AEEFont nFont, IFont **ppif);
static int OEMSysFont_New(IShell * piShell, AEECLSID cls, void **ppif)
{
    MSG_FATAL("OEMSysFont_New.........",0,0,0);
    switch (cls) {
    case AEECLSID_FONTSYSNORMAL:
        return OEMFont_GetSystemFont( AEE_FONT_NORMAL, (IFont **)ppif);

    case AEECLSID_FONTSYSBOLD:
        return OEMFont_GetSystemFont( AEE_FONT_BOLD, (IFont **)ppif);

    case AEECLSID_FONTSYSLARGE:
        return OEMFont_GetSystemFont( AEE_FONT_LARGE, (IFont **)ppif);

	case AEECLSID_FONTSYSITALIC:
	case AEECLAID_FONTSMALL:
        return OEMFont_GetSystemFont( AEE_FONT_SMALL, (IFont **)ppif);
	  
    default:
        return ECLASSNOTSUPPORT;
    }
}
#endif

static uint32 OEMDisplayDev_AddRef(IDisplayDev *pMe)
{
   return ++pMe->nRefs;
}

static uint32 OEMDisplayDev_Release(IDisplayDev *pMe)
{
   uint32 nRefs = (pMe->nRefs ? --pMe->nRefs : 0);

   if (!nRefs) {

#ifdef FEATURE_MMOVERLAY
      if(NULL != pMe->pSysMMOverlayRegistry)
      {
         (void)ISysMMOverlayRegistry_Release(pMe->pSysMMOverlayRegistry);
         pMe->pSysMMOverlayRegistry = NULL;
      }

      if(mdp_is_ovimg_valid(pMe->mdpOvID))
      {
         mdp_deregister_locked_ovimg(pMe->mdpOvID);
         pMe->mdpOvID = 0;
      }
#endif

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

boolean gbUpdateAnuu = FALSE;
void OEM_EnableAnuuUpdate(void)
{
    gbUpdateAnuu = TRUE;
}
static boolean gbNotUpdateScreen = FALSE;
void OEM_SetNotUpdateScreen(boolean bOn)
{
    gbNotUpdateScreen = bOn;
}

static int OEMDisplayDev_Update(IDisplayDev *pMe, IBitmap *pbmSrc, AEERect *prc)
{
   IDIB *pDib;
   int nErr = SUCCESS;
   
   if(gbNotUpdateScreen)
   {
      MSG_FATAL("OEM_SetNotUpdateScreen TRUE",0,0,0);
      return nErr;
   }
   
   nErr = IBITMAP_QueryInterface(pbmSrc, AEECLSID_DIB, (void**)&pDib);
   if (SUCCESS != nErr) {
      return nErr;
   }

   //MSG_FATAL("OEMDisplayDev_Update %d %d %d",prc->dx,prc->y,prc->dy);
   if(gbUpdateAnuu)
   {
        prc->dy = prc->dy+prc->y;
        prc->y  = 0;
        prc->x  = 0;
        prc->dx = DISP_WIDTH;
        gbUpdateAnuu = FALSE;
   }
   
#ifdef FEATURE_MMOVERLAY

   if((0 != pMe->displayClsID) && (NULL != pMe->pSysMMOverlayRegistry))
   {
      boolean bRegistered;
      if(0 == pMe->mdpOvID)
      {
         //Need to initialize the MDP image
         pMe->mdpImg.alpha = 0x100;
         pMe->mdpImg.bmy_addr = (void *)pDib->pBmp;
         switch (pMe->displayClsID)
         {
            case AEECLSID_DISPLAY1:
               pMe->mdpImg.dest = PRIMARY_LCD_TYPE;
               break;
            case AEECLSID_DISPLAY2:
               pMe->mdpImg.dest = SECONDARY_LCD_TYPE;
               break;
            default:
               return EFAILED;
         }

         pMe->mdpImg.cwin.x1 = 0;
         pMe->mdpImg.cwin.x2 = pDib->cx - 1;
         pMe->mdpImg.cwin.y1 = 0;
         pMe->mdpImg.cwin.y2 = pDib->cy - 1;

         if(pDib->nDepth == 16)
         {
            pMe->mdpImg.imgType = MDP_RGB_565;
         }
         else
         {
         	MSG_FATAL("OEMDisplayDev_Update....................EFAILED",0,0,0);
            return EFAILED;
         }

         pMe->mdpImg.layer = MDP_BGL;
         pMe->mdpImg.lcd_x = 0 - BREW_OFFSET_X;
         pMe->mdpImg.lcd_y = 0 - BREW_OFFSET_Y;
         pMe->mdpImg.mdpOp = MDPOP_QUEUE_COPY;

         pMe->mdpImg.width = (uint16) pDib->nPitch / (pDib->nDepth / 8);
         //MSG_FATAL("pMe->mdpImg.cwin.x1=======%d",pMe->mdpImg.cwin.x1,0,0);
		 //MSG_FATAL("pMe->mdpImg.cwin.x2=======%d",pMe->mdpImg.cwin.x2,0,0);
		 //MSG_FATAL("pMe->mdpImg.cwin.y1=======%d",pMe->mdpImg.cwin.y1,0,0);
		 //MSG_FATAL("pMe->mdpImg.cwin.y2=======%d",pMe->mdpImg.cwin.y2,0,0);
		 
         pMe->mdpOvID = mdp_register_ovimg(&pMe->mdpImg);
         if(0 == pMe->mdpOvID)
         {
            DBGPRINTF_FATAL("Unable to register MDP Image");
            nErr = SVC_disp_update(pDib, prc);
         }
         else
         {
            pMe->bmdpOvEnabled = TRUE;
            (void)mdp_lock_ovimg(pMe->mdpOvID, TRUE);
         }
      }

      if(SUCCESS == ISysMMOverlayRegistry_AreOverlaysRegistered(pMe->pSysMMOverlayRegistry,
                                                      pMe->displayClsID,
                                                      &bRegistered))
      {
         if(TRUE == bRegistered)
         {
            if(pMe->bmdpOvEnabled)
            {
               mdp_clear_ovimg(pMe->mdpOvID);
            }
            mdp_enable_locked_ovimg(pMe->mdpOvID, FALSE);
            pMe->bmdpOvEnabled = FALSE;
            nErr = ISysMMOverlayRegistry_Update(pMe->pSysMMOverlayRegistry, pMe->displayClsID, prc);
         }
         else
         {
            mdp_enable_locked_ovimg(pMe->mdpOvID, TRUE);
            pMe->bmdpOvEnabled = TRUE;
            pMe->mdpImg.cwin.x1 = prc->x;
            pMe->mdpImg.cwin.x2 = prc->x + prc->dx - 1;
            pMe->mdpImg.cwin.y1 = prc->y;
            pMe->mdpImg.cwin.y2 = prc->y + prc->dy - 1;
            pMe->mdpImg.lcd_x = prc->x - BREW_OFFSET_X;
            pMe->mdpImg.lcd_y = prc->y - BREW_OFFSET_Y;
            //MSG_FATAL("2pMe->mdpImg.cwin.x1==%d,prc->x=%d",pMe->mdpImg.cwin.x1,prc->x,0);
		 	//MSG_FATAL("2pMe->mdpImg.cwin.x2==%d,prc->y=%d",pMe->mdpImg.cwin.x2,prc->y,0);
		 	//MSG_FATAL("2pMe->mdpImg.cwin.y1==%d,prc->dx=%d",pMe->mdpImg.cwin.y1,prc->dx,0);
		 	//MSG_FATAL("2pMe->mdpImg.cwin.y2==%d,prc->dy=%d",pMe->mdpImg.cwin.y2,prc->dy,0);
		 	
		 	
#ifdef FEATURE_MDP2
            OEMDIBDisplay_FlushCache(&pMe->mdpImg);
#endif
            if(MDP_FAIL == mdp_disp_update(&pMe->mdpImg))
            {
               nErr = EFAILED;
            }
         }

      }
      else
      {
         DBGPRINTF_FATAL("Unable to check if overlays are enabled");
         nErr = SVC_disp_update(pDib, prc);
      }
   }
   else
#endif
   {
      nErr = SVC_disp_update(pDib, prc);
   }

   (void)IDIB_Release(pDib);
   //MSG_FATAL("OEMDisplayDev_Update END %d %d %d",prc->dx,prc->y,prc->dy);
   return nErr;

}


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

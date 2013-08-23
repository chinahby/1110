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

// These variables are used throughout this sample code.  OEM will need to
// replace all of the occurances of these variables in this file.
extern int undefined_height;
extern int undefined_width;
extern void *undefined_framebuffer;

static boolean gbInit = 0;
static IBitmap *gpDevBitmap = NULL;
static AEECallback gCallback;

struct IDisplayDev {
   AEEVTBL(IDisplayDev)   *pvt;
   uint32                  nRefs;
};


static void OEMBitmapDev_Init(IShell *ps);
static void OEMBitmapDev_Cleanup(void *pData);

static int OEMDisplayDev_New(IShell * piShell, AEECLSID cls, void **ppif);
static int OEMBitmapDev_New(IShell * piShell, AEECLSID cls, void **ppif);
static int OEMBitmapDevChild_New(IShell * piShell, AEECLSID cls, void **ppif);
static int OEMSysFont_New(IShell * piShell, AEECLSID cls, void **ppif);

extern const AEEStaticClass gOEMDisplayDevClasses[] = {
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
   {AEECLSID_FONTSYSNORMAL,   ASCF_UPGRADE, 0,          0, OEMSysFont_New},
   {AEECLSID_FONTSYSLARGE,    ASCF_UPGRADE, 0,          0, OEMSysFont_New},
   {AEECLSID_FONTSYSBOLD,     ASCF_UPGRADE, 0,          0, OEMSysFont_New},
   {AEECLAID_FONTSMALL,       ASCF_UPGRADE, 0,          0, OEMSysFont_New},
   {AEECLSID_FONTSYSITALIC,   ASCF_UPGRADE, 0,          0, OEMSysFont_New},
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
   if (gpDevBitmap) {
      IBITMAP_Release(gpDevBitmap);
      gpDevBitmap = NULL;
   }
   gbInit = FALSE;
}

extern int OEMDisplayDev_New(IShell * piShell, AEECLSID cls, void **ppif)
{
   IDisplayDev   *pMe;

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
   return SUCCESS;
}

// global device bitmap
extern int OEMBitmapDev_New(IShell * piShell, AEECLSID cls, void **ppif)
{
   IDisplayDev   *pDispDev = NULL;
   int            nErr = SUCCESS;

   *ppif = NULL;

   if (AEECLSID_DEVBITMAP1 != cls) {
      return ECLASSNOTSUPPORT;
   }

   OEMBitmapDev_Init(piShell);

   if (!gpDevBitmap) {
      nErr = AEE_CreateInstanceSys(AEECLSID_DISPLAYDEV1, (void**)&pDispDev);
      if (SUCCESS != nErr) {
         return nErr;
      }

      nErr = OEMBitmap16_NewEx(undefined_width, undefined_height, undefined_framebuffer, NULL, pDispDev, (IBitmap**)ppif);
      if (SUCCESS != nErr) {
         goto Error;
      }

      gpDevBitmap = (IBitmap*)*ppif;
   } else {
      *ppif = gpDevBitmap;
   }

   IBITMAP_AddRef(((IBitmap*)*ppif));

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

   if (AEECLSID_DEVBITMAP1_CHILD != cls) {
      return ECLASSNOTSUPPORT;
   }

   pac = AEE_EnterAppContext(NULL);
   nErr = AEE_CreateInstanceSys(AEECLSID_DEVBITMAP1, (void**)&pDevBitmap);
   AEE_LeaveAppContext(pac);
   if (SUCCESS != nErr) {
      return nErr;
   }

   nErr = OEMBitmap16_New_Child(pDevBitmap, NULL, (IBitmap**)ppif);
   if (SUCCESS != nErr) {
      goto Error;
   }

Error:
   if (pDevBitmap) {
      IBITMAP_Release(pDevBitmap);
   }
   return nErr;
}


static int OEMSysFont_New(IShell * piShell, AEECLSID cls, void **ppif)
{
   switch (cls) {
   case AEECLSID_FONTSYSNORMAL:
      return ISHELL_CreateInstance(piShell, AEECLSID_FONT_BASIC11, (void **)ppif);
   case AEECLSID_FONTSYSBOLD:
      return ISHELL_CreateInstance(piShell, AEECLSID_FONT_BASIC11B, (void **)ppif);
   case AEECLSID_FONTSYSLARGE:
      return ISHELL_CreateInstance(piShell, AEECLSID_FONT_BASIC14, (void **)ppif);
   case AEECLAID_FONTSMALL:
      return ISHELL_CreateInstance(piShell, AEECLSID_FONT_BASIC9, (void **)ppif);	  
   default:
      return ECLASSNOTSUPPORT;
   }
}


static uint32 OEMDisplayDev_AddRef(IDisplayDev *pMe)
{
   return ++pMe->nRefs;
}

static uint32 OEMDisplayDev_Release(IDisplayDev *pMe)
{
   uint32 nRefs = (pMe->nRefs ? --pMe->nRefs : 0);

   if (!nRefs) {
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

static int OEMDisplayDev_Update(IDisplayDev *pMe, IBitmap *pbmSrc, AEERect *prc)
{
   // OEM must provide update function
   return SUCCESS;
}


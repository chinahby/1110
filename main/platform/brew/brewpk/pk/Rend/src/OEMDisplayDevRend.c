/*======================================================
FILE:  OEMDisplayDevRend.c

SERVICES:  Implemention the IDisplayDevRend interface.

GENERAL DESCRIPTION:
   This provides a reference implementation for the
   IDisplayDevRend interface, which is used by OEMBitmap for
   updating to the display.

        Copyright 2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#include "AEEShell.h"
#include "AEEStdLib.h"
#include "AEEIDisplayDevRend.h"
#include "AEEIDIBDisplayDev.h"
#include "AEEIDIB.h"
#include "AEEStdErr.h"
#include "AEEDevBitmap.bid"
#include "AEEDIBDisplayDev.bid"
#include "AEEDisplayDev.bid"

//=============================================================================
//   Macro definitions
//=============================================================================

#define RELEASEIF(p) do { if (p) { IQI_Release((IQueryInterface*)(p)); p = 0; } } while (0)

struct IDisplayDevRend {
   const AEEVTBL(IDisplayDevRend) *pvt;
   IShell           *piShell;
   uint32            nRefs;
   IDIBDisplayDev   *piDIBDisplayDev;
   AEECLSID          clsDIBDisplayDev;
   RENDHANDLE        hRendDev;
};

static uint32 OEMDisplayDevRend_AddRef(IDisplayDevRend *me);
static uint32 OEMDisplayDevRend_Release(IDisplayDevRend *me);
static int OEMDisplayDevRend_QueryInterface(IDisplayDevRend *me, AEECLSID clsid, void **ppNew);
static int OEMDisplayDevRend_Update(IDisplayDevRend *me, IBitmap *pbmSrc, AEERect *prc);
static int OEMDisplayDevRend_SetRendEnable(IDisplayDevRend *me, boolean bEnable, int fps);
static int OEMDisplayDevRend_PushScreenEx(IDisplayDevRend *me, IBitmap *pbmSrc);
static int OEMDisplayDevRend_StartRend(IDisplayDevRend *me, RendType eType, int nDelay);
static int OEMDisplayDevRend_StopRend(IDisplayDevRend *me);
static boolean OEMDisplayDevRend_IsRendEnable(IDisplayDevRend *me);
static boolean OEMDisplayDevRend_IsRendPlaying(IDisplayDevRend *me);
static int OEMDisplayDevRend_Init(IDisplayDevRend *me);
static int OEMDisplayDevRend_Done(IDisplayDevRend *me);
static int OEMDisplayDevRend_UpdateScreen(IDisplayDevRend *me, void *pSrc, int nSize);

static const VTBL(IDisplayDevRend) gOEMDisplayDevRendFuncs = {
   OEMDisplayDevRend_AddRef,
   OEMDisplayDevRend_Release,
   OEMDisplayDevRend_QueryInterface,
   OEMDisplayDevRend_Update,
   OEMDisplayDevRend_SetRendEnable,
   OEMDisplayDevRend_PushScreenEx,
   OEMDisplayDevRend_StartRend,
   OEMDisplayDevRend_StopRend,
   OEMDisplayDevRend_Done,
   OEMDisplayDevRend_IsRendEnable,
   OEMDisplayDevRend_IsRendPlaying
};

extern int OEMDisplayDevRend_NewEx(IShell         *piShell,
                                   IDIBDisplayDev *piDIBDisplayDev,
                                   AEECLSID        clsDIBDisplayDev,
                                   void           **ppif)
{
   IDisplayDevRend *me = 0;

   *ppif = 0;

   me = (IDisplayDevRend *)MALLOC(sizeof(IDisplayDevRend));
   if (NULL == me) {
      return AEE_ENOMEMORY;
   }

   me->pvt = &gOEMDisplayDevRendFuncs;
   me->piShell = piShell;
   ISHELL_AddRef(me->piShell);
   me->nRefs = 1;
   me->piDIBDisplayDev = piDIBDisplayDev;
   IDIBDisplayDev_AddRef(piDIBDisplayDev);
   me->clsDIBDisplayDev = clsDIBDisplayDev;
   OEMDisplayDevRend_Init(me);
   *ppif = (void *)me;
   return SUCCESS;
}

extern int OEMDisplayDevRend_New(IShell *piShell, AEECLSID cls, void **ppif)
{
    IDIBDisplayDev *  piDIBDisplayDev = 0;
    AEECLSID          clsDIBDisplayDev;
    int nErr;
    
    switch (cls) {
    case AEECLSID_DisplayDev1:
        clsDIBDisplayDev = AEECLSID_DIBDisplayDev1;
        break;
    case AEECLSID_DisplayDev2:
        clsDIBDisplayDev = AEECLSID_DIBDisplayDev2;
        break;
    case AEECLSID_DisplayDev3:
        clsDIBDisplayDev = AEECLSID_DIBDisplayDev3;
        break;
    case AEECLSID_DisplayDev4:
        clsDIBDisplayDev = AEECLSID_DIBDisplayDev4;
        break;
    default:
        return AEE_ECLASSNOTSUPPORT;
    }
    
    nErr = ISHELL_CreateInstance(piShell, clsDIBDisplayDev, (void **)&piDIBDisplayDev);
    if(nErr != SUCCESS)
    {
        return nErr;
    }
    nErr = OEMDisplayDevRend_NewEx(piShell, piDIBDisplayDev, clsDIBDisplayDev, ppif);
    RELEASEIF(piDIBDisplayDev);
    return nErr;
}

static uint32 OEMDisplayDevRend_AddRef(IDisplayDevRend *me)
{
   return ++me->nRefs;
}

static uint32 OEMDisplayDevRend_Release(IDisplayDevRend *me)
{
   uint32 nRefs = --me->nRefs;

   if (!nRefs) {
      RELEASEIF(me->piShell);
      RELEASEIF(me->piDIBDisplayDev);
      FREE(me);
   }

   return nRefs;
}

static int OEMDisplayDevRend_QueryInterface(IDisplayDevRend *me, AEECLSID clsid, void **ppNew)
{
   if (clsid == AEEIID_IDisplayDevRend || clsid == AEEIID_QUERYINTERFACE) {
      *ppNew = (void*)me;
      OEMDisplayDevRend_AddRef(me);
      return SUCCESS;
   }

   *ppNew = 0;
   return AEE_ECLASSNOTSUPPORT;
}

static int OEMDisplayDevRend_Update(IDisplayDevRend *me, IBitmap *pbmSrc, AEERect *prc)
{
   IDIB *piDIB = 0;
   int nErr;

   nErr = IBitmap_QueryInterface(pbmSrc, AEEIID_IDIB, (void**)&piDIB);
   if (SUCCESS != nErr) {
      goto bail;
   }
   
   nErr = OEMDisplayDevRend_UpdateScreen(me, piDIB->pBmp, piDIB->nPitch * piDIB->cy);
   if (SUCCESS == nErr) {
      goto bail;
   }
   
   nErr = IDIBDisplayDev_Update(me->piDIBDisplayDev,
                                piDIB->pBmp, piDIB->nPitch * piDIB->cy,
                                piDIB->cx, piDIB->cy,
                                piDIB->nPitch, piDIB->nDepth,
                                piDIB->nColorScheme,
                                prc);
bail:
   RELEASEIF(piDIB);
   return nErr;
}

static int OEMDisplayDevRend_SetRendEnable(IDisplayDevRend *me, boolean bEnable, int fps)
{
    Rendering_SetFPS(me->hRendDev, fps);
    return Rendering_SetEnable(me->hRendDev, bEnable);
}

static int OEMDisplayDevRend_PushScreenEx(IDisplayDevRend *me, IBitmap *pbmSrc)
{
    IDIB *piDIB = 0;
    int nErr;
    
    if(pbmSrc){
        nErr = IBitmap_QueryInterface(pbmSrc, AEEIID_IDIB, (void**)&piDIB);
        if (SUCCESS != nErr) {
            goto bail;
        }
        
    }else{
        nErr = ISHELL_CreateInstance(me->piShell, AEECLSID_DevBitmap1Child, (void **)&pbmSrc);
        if (SUCCESS != nErr) {
           goto bail;
        }
        
        nErr = IBitmap_QueryInterface(pbmSrc, AEEIID_IDIB, (void**)&piDIB);
        RELEASEIF(pbmSrc);
        if (SUCCESS != nErr) {
            goto bail;
        }
    }

    nErr = Rendering_SetSrcScreen(me->hRendDev, piDIB->pBmp, piDIB->nPitch * piDIB->cy, piDIB->cx, piDIB->cy, piDIB->nPitch, piDIB->nDepth, piDIB->nColorScheme);
    if (SUCCESS != nErr) {
        goto bail;
    }
    
    nErr = Rendering_PushScreen(me->hRendDev);
bail:
    RELEASEIF(piDIB);
    return nErr;
}

static int OEMDisplayDevRend_StartRend(IDisplayDevRend *me, RendType eType, int nDelay)
{
    IDIB *piDIB = 0;
    int nErr;
    IBitmap *pbmSrc = 0;
    
    nErr = ISHELL_CreateInstance(me->piShell, AEECLSID_DevBitmap1Child, (void **)&pbmSrc);
    if (SUCCESS != nErr) {
       goto bail;
    }
    
    nErr = IBitmap_QueryInterface(pbmSrc, AEEIID_IDIB, (void**)&piDIB);
    RELEASEIF(pbmSrc);
    if (SUCCESS != nErr) {
        goto bail;
    }
    
    nErr = Rendering_SetType(me->hRendDev, eType);
    if (SUCCESS != nErr) {
        goto bail;
    }
    
    nErr = Rendering_SetSrcScreen(me->hRendDev, piDIB->pBmp, piDIB->nPitch * piDIB->cy, piDIB->cx, piDIB->cy, piDIB->nPitch, piDIB->nDepth, piDIB->nColorScheme);
    if (SUCCESS != nErr) {
        goto bail;
    }
    
    nErr = Rendering_Start(me->hRendDev, nDelay);
bail:
    RELEASEIF(piDIB);
    return nErr;
}

static int OEMDisplayDevRend_StopRend(IDisplayDevRend *me)
{
    return Rendering_Stop(me->hRendDev);
}

static boolean OEMDisplayDevRend_IsRendEnable(IDisplayDevRend *me)
{
    return Rendering_IsEnable(me->hRendDev);
}

static boolean OEMDisplayDevRend_IsRendPlaying(IDisplayDevRend *me)
{
    return Rendering_IsPlaying(me->hRendDev);
}

static int OEMDisplayDevRend_UpdateScreen(IDisplayDevRend *me, void *pSrc, int nSize)
{
    if(Rendering_IsEnable(me->hRendDev))
    {
        if(Rendering_IsPlaying(me->hRendDev))
        {
            return SUCCESS;
        }
    }
    return EFAILED;
}

static RENDHANDLE  g_hRendDev16Bit = NULL;
static int OEMDisplayDevRend_Init(IDisplayDevRend *me)
{
    if(g_hRendDev16Bit == NULL)
    {
        Rendering_Init(&g_hRendDev16Bit);
    }
    me->hRendDev = g_hRendDev16Bit;
    return SUCCESS;
}

static int OEMDisplayDevRend_Done(IDisplayDevRend *me)
{
    Rendering_Done(g_hRendDev16Bit);
    g_hRendDev16Bit = NULL;
    me->hRendDev    = g_hRendDev16Bit;
    return SUCCESS;
}


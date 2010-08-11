/*======================================================
FILE:  OEMMemCache.c

SERVICES:  OEM functions for implementing the MemCache interface

GENERAL DESCRIPTION:
   This provides a reference implementation for the MemCache
   interface, which is used by BREW to flush instruction and/or
   data caches.

        Copyright 2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#include "AEEMemCache.h"
#include "AEEStdLib.h"
#include "AEEMemCache.bid"

#define RELEASEIF(p) do { if (p) { IQI_Release((IQueryInterface*)(p)); p = 0; } } while (0)

extern int    OEMMemCache_ArchClean(uint32 dwAddr, uint32 dwSize, int nFlags);

static uint32 OEMMemCache_AddRef(IMemCache *po);
static uint32 OEMMemCache_Release(IMemCache *po);
static int    OEMMemCache_QueryInterface(IMemCache *po, AEECLSID clsid, void **ppNew);
static int    OEMMemCache_Clean(IMemCache *po, uint32 dwAddr, uint32 dwSize, int nFlags);

static const VTBL(IMemCache) gOEMMemCacheFuncs = {
   OEMMemCache_AddRef,
   OEMMemCache_Release,
   OEMMemCache_QueryInterface,
   OEMMemCache_Clean
};

typedef struct OEMMemCache OEMMemCache;

struct OEMMemCache {
   const AEEVTBL(IMemCache) *pvt;
   uint32 uRefs;
};

//====================================================================
//
//====================================================================
int OEMMemCache_New(IShell * piShell, AEECLSID cls, void **ppif)
{
   OEMMemCache *me = NULL;
   int nResult = SUCCESS;

   me = (OEMMemCache*)MALLOC(sizeof(OEMMemCache));
   if (me == NULL) {
      nResult = ENOMEMORY;
      goto bail;
   }

   me->pvt = &gOEMMemCacheFuncs;
   me->uRefs = 1;

bail:
   *ppif = me;
   return nResult;
}

//====================================================================
//
//====================================================================
static uint32 OEMMemCache_AddRef(IMemCache *po)
{
   OEMMemCache *me = (OEMMemCache*)po;
   return ++(me->uRefs);
}

//====================================================================
//
//====================================================================
static uint32 OEMMemCache_Release(IMemCache *po)
{
   OEMMemCache *me = (OEMMemCache*)po;

   if (!me->uRefs) {
      return 0;
   }

   if (--me->uRefs > 0) {
      return me->uRefs;
   }

   // Ref count is zero, so clean up
   FREE(me);

   return 0;
}

//====================================================================
//
//====================================================================
static int OEMMemCache_QueryInterface(IMemCache *po, AEECLSID clsid, void **ppNew)
{
   if (clsid == AEEIID_MEMCACHE || clsid == AEECLSID_QUERYINTERFACE) {
      *ppNew = (void*)po;
      OEMMemCache_AddRef(po);
      return SUCCESS;
   }

   return ECLASSNOTSUPPORT;
}
//====================================================================
//
//====================================================================
static int OEMMemCache_Clean(IMemCache *po, uint32 dwAddr, uint32 dwSize, int nFlags)
{
   return OEMMemCache_ArchClean(dwAddr, dwSize, nFlags);
}

//====================================================================
//
//====================================================================
#ifdef AEE_SIMULATOR
extern int OEMMemCache_ArchClean(uint32 dwAddr, uint32 dwSize, int nFlags)
{
   return SUCCESS;
}
#endif //AEE_SIMULATOR
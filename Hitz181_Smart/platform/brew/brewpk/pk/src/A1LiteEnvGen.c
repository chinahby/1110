/*===========================================================================

FILE: A1LiteEnv.c

DESCRIPTION
  This file contains code for implementation of the Env interface in BREW 3.x.

PUBLIC CLASSES:  
   N/A

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A


          Copyright © 1999-2007 QUALCOMM Incorporated.
                      All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
  ========================================================================== */

#include "AEE.h"
#include "AEEShell.h"
#include "AEEStdLib.h"
#include "A1LiteEnvGen.h"

typedef struct A1Env A1Env;

AEEINTERFACE(A1Env) {
   INHERIT_IEnv(A1Env);
};

struct A1Env {
   const A1EnvVtbl *pvt;
   uint32 uRef;
   IShell *piShell;
   IModule *piModule;
   A1EnvVtbl vt;
};

static uint32 A1Env_AddRef(A1Env *me)
{
   if (me->piModule)
      return IMODULE_AddRef(me->piModule);

   return ++me->uRef;
}

static uint32 A1Env_Release(A1Env *me)
{
   uint32 uRet;

   if (me->piModule)
      return IMODULE_Release(me->piModule);

   uRet = --me->uRef;
   if (uRet == 0) {
      FREE(me);
   }
   return uRet;
}

static int A1Env_QueryInterface(A1Env *me, AEEIID id, void **ppo)
{
   if (id == AEEIID_IQI ||
       id == AEEIID_IRealloc ||
       id == AEEIID_IEnv) {
      A1Env_AddRef(me);
      *ppo = (void *)me;
      return SUCCESS;
   }

   return ECLASSNOTSUPPORT;
}

static int A1Env_ErrRealloc(A1Env *me, int nSize, void **ppo)
{
   void *pOrg;
   void *pNew;

   if (nSize < 0)
      return ENOMEMORY;

   pOrg = *ppo;

   if (nSize == 0) {
      FREEIF(pOrg);
      *ppo = 0;
      return SUCCESS;
   }

   pNew = pOrg ?
      REALLOC(pOrg, nSize) :
      MALLOC(nSize);

   if (pNew == 0)
      return ENOMEMORY;

   *ppo = pNew;

   return SUCCESS;
}

static int A1Env_ErrReallocName(A1Env *me, int nSize, void **ppo,
                                const char *cpszName)
{
   (void)cpszName;  /* BREW heap can't implement A1 semantics - ignore it */

   return A1Env_ErrRealloc(me, nSize, ppo);
}

static int A1Env_ErrReallocNoZI(A1Env *me, int nSize, void **ppo)
{
   void *pOrg;
   void *pNew;

   if (nSize < 0)
      return ENOMEMORY;

   pOrg = *ppo;

   if (nSize == 0) {
      FREEIF(pOrg);
      *ppo = 0;
      return SUCCESS;
   }

   pNew = pOrg ?
      REALLOC(pOrg, nSize | ALLOC_NO_ZMEM) :
      MALLOC(nSize | ALLOC_NO_ZMEM);

   if (pNew == 0)
      return ENOMEMORY;

   *ppo = pNew;

   return SUCCESS;
}

static int A1Env_ErrReallocNameNoZI(A1Env *me, int nSize, void **ppo,
                                    const char *cpszName)
{
   (void)cpszName;  /* BREW heap can't implement A1 semantics - ignore it */

   return A1Env_ErrReallocNoZI(me, nSize, ppo);
}

static int A1Env_CreateInstance(A1Env *me, AEECLSID cls, void **ppo)
{
   return ISHELL_CreateInstance(me->piShell, cls, ppo);
}

static int A1Env_AtExit(A1Env *me, void (*pfn)(void *), void *pCtx)
{
   return EUNSUPPORTED;
}

void *A1LiteEnv_Alloc(void)
{
   return MALLOC(sizeof(A1Env));
}

void A1LiteEnv_Free(void *ptr)
{
   FREE(ptr);
}

IEnv *A1LiteEnv_New(void *ptr, IShell *piShell, IModule *piModule)
{
   A1Env *pEnv;

   pEnv = (A1Env *)ptr;

   pEnv->vt.AddRef = A1Env_AddRef;
   pEnv->vt.Release = A1Env_Release;
   pEnv->vt.QueryInterface = A1Env_QueryInterface;
   pEnv->vt.ErrRealloc = A1Env_ErrRealloc;
   pEnv->vt.ErrReallocName = A1Env_ErrReallocName;
   pEnv->vt.ErrReallocNoZI = A1Env_ErrReallocNoZI;
   pEnv->vt.ErrReallocNameNoZI = A1Env_ErrReallocNameNoZI;
   pEnv->vt.CreateInstance = A1Env_CreateInstance;
   pEnv->vt.AtExit = A1Env_AtExit;

   pEnv->pvt = &pEnv->vt;
   pEnv->piShell = piShell;
   pEnv->piModule = piModule;
   pEnv->uRef = 1;

   return (IEnv *)pEnv;
}

int A1LiteEnv_EnvFromShell(IShell *piShell, IEnv **ppo)
{
   void *pMem;

   if (NULL == (pMem = A1LiteEnv_Alloc()))
      return ENOMEMORY;

   *ppo = A1LiteEnv_New(pMem, piShell, NULL);

   return SUCCESS;
}

int A1LiteEnv_CIFromShell(int (*pfnCI)(IEnv *, AEECLSID, void **),
                          IShell *piShell,
                          AEECLSID cls,
                          void **ppo)
{
   IEnv *piEnv;
   int nErr;

   if (SUCCESS == (nErr = A1LiteEnv_EnvFromShell(piShell, &piEnv))) {
      nErr = (*pfnCI)(piEnv, cls, ppo);
      IEnv_Release(piEnv);
   }

   return nErr;
}

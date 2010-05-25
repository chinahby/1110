/*
  ========================================================================

  FILE:  fbase.c
  
  SERVICES:  

  GENERAL DESCRIPTION: Implements the base model methods

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#include "fbase.h"
#include "util.h"
#include "AEEStdLib.h"


#define ME_FROM_MODEL   ModelBase *me = (ModelBase *)po


/////////////////////////////////////////////////////////////////////////////
// Node list stuff

// initialize 'me'  (next & prev point to myself)
static __inline void LNode_Ctor(LNode *me) {
   me->pNext = me->pPrev = me;
}

// insert pNew after 'me'
static __inline void LNode_InsertAfter(LNode *me, LNode *pNew) {
   pNew->pPrev       = me;
   pNew->pNext       = me->pNext;
   me->pNext->pPrev  = pNew;
   me->pNext         = pNew;
}

// remove 'me'
static __inline void LNode_Remove(LNode *me) {
   me->pPrev->pNext = me->pNext;
   me->pNext->pPrev = me->pPrev;
   me->pNext = me->pPrev = me;
}

// return next node if valid, otherwise return NULL
static __inline LNode *LNode_Next(LNode *me, LNode *pHead) {
   return (me->pNext != pHead) ? me->pNext : 0;
}


// -- ModelListener specific -- 
static __inline void MLNode_Ctor(ModelListener *me) {
   LNode_Ctor((LNode*)me);
}

static __inline void MLNode_Remove(ModelListener *me) {
   LNode_Remove((LNode*)me);
}

static __inline ModelListener *MLNode_Next(ModelListener *me, LNode *pHead) {
   return (ModelListener*)LNode_Next((LNode*)me, pHead);
}

static __inline ModelListener *MLNode_First(LNode *pHead) {
   return (ModelListener*)LNode_Next(pHead, pHead);
}

static __inline void MLNode_InsertFirst(LNode *pHead, ModelListener *pNew) {
   LNode_InsertAfter(pHead, (LNode*)pNew);
}



/////////////////////////////////////////////////////////////////////////////
// forward decls


static void ModelBase_CancelAllListeners(ModelBase *me);



/////////////////////////////////////////////////////////////////////////////
// IModel methods


uint32 ModelBase_AddRef(IModel *po)
{
   ME_FROM_MODEL;
   return ++me->nRefs;
}

uint32 ModelBase_Release(IModel *po)
{
   ME_FROM_MODEL;

   if (1 == me->nRefs) {
      ModelBase_CancelAllListeners(me);
      RELEASEIF(me->piModule);
      FREE(me);
      return 0;
   }
   return --me->nRefs;
}


int ModelBase_QueryInterface(IModel *po, AEECLSID id, void **ppo)
{
   if (id == AEEIID_MODEL) {
      *ppo = po;
      ModelBase_AddRef(po);
      return 0;
   }
   return ECLASSNOTSUPPORT;
}


static void ModelListener_Cancel(ModelListener *me) 
{
   me->pfnCancel = 0;
   MLNode_Remove(me);
}


static void ModelBase_CancelAllListeners(ModelBase *me) 
{
   ModelListener *pml;

   while (0 != (pml = MLNode_First(&me->head))) {
      ModelListener_Cancel(pml);
   }
}


int ModelBase_AddListener(IModel *po, ModelListener *pml)
{
   ME_FROM_MODEL;

   if (!pml) {
      return EBADPARM;
   }

   // Cancel listener if already registered
   if (LISTENER_IsRegistered(pml)) {
      LISTENER_Cancel(pml);
   }

   // initialize cancel function
   pml->pfnCancel    = ModelListener_Cancel;
   pml->pCancelData  = me;    // not really needed

   MLNode_Ctor(pml);
   MLNode_InsertFirst(&me->head, pml);

   return SUCCESS;
}


void ModelBase_Notify(IModel *po, ModelEvent *pEvent)
{
   ME_FROM_MODEL;

   if (po && pEvent) {

      ModelListener *pml = (ModelListener*)&me->head;

      ADDREFIF(po);

      while (NULL != (pml = MLNode_Next(pml, &me->head))) {
         if (pml->pfnListener) {
            pEvent->pModel = po;
            pml->pfnListener(pml->pListenerData, pEvent);
         }
      }

      RELEASEIF(po);
   }
}


void ModelBase_Ctor(ModelBase *me, AEEVTBL(IModel) *pvt, IModule *piModule)
{
   me->pvt = pvt;

   pvt->AddRef          = ModelBase_AddRef;
   pvt->Release         = ModelBase_Release;
   pvt->QueryInterface  = ModelBase_QueryInterface;
   pvt->AddListener     = ModelBase_AddListener;
   pvt->Notify          = ModelBase_Notify;

   me->nRefs = 1;
   LNode_Ctor(&me->head);

   me->piModule = piModule;
   ADDREFIF(piModule);
}


int ModelBase_New(IModel **ppo, IModule *piModule)
{
   ModelBase *me = MALLOCREC_VTBL(ModelBase, IModel);
   
   if (!me) {
      return ENOMEMORY;
   }

   ModelBase_Ctor(me, GETVTBL(me,IModel), piModule);
   *ppo = (IModel*) me;
   return SUCCESS;
}






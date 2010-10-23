/*
  ========================================================================

  FILE:  AEEModelBase.c
  
  SERVICES:  

  GENERAL DESCRIPTION: Implements the base model methods

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#include "ModelBase.h"
#include "AEEStdLib.h" // For MEMMOVE, REALLOC
#include "wutil.h"


#define ME_FROM(type)            type *me = (type*)po

#define ME_FROM_MODEL            ModelBase *me = (ModelBase *)po
#define ME_FROM_VALUEMODEL       ValueModel *me = (ValueModel *)po
#define ME_FROM_LISTMODEL        ListModel *me = (ListModel *)po
#define ME_FROM_ARRAYMODEL       ArrayModel *me = (ArrayModel *)po
#define ME_FROM_INTERFACEMODEL   InterfaceModel *me = (InterfaceModel*)po


uint32 ModelBase_AddRef(IModel *po)
{
   ME_FROM_MODEL;
   
   return ++me->nRefs;
}

uint32 ModelBase_ReleaseStatic(IModel *po)
{
   ME_FROM_MODEL;

   if (me->nRefs && !--me->nRefs) {
      ModelListener *pListener = me->pListeners;
      while(pListener != 0) {
         pListener->pfnCancel = 0;
         pListener = pListener->pNext;
      }
   }
   return me->nRefs;
}

uint32 ModelBase_Release(IModel *po)
{
   ME_FROM_MODEL;
   uint32 nPrevRefs = me->nRefs;
   uint32 nRefs = ModelBase_ReleaseStatic(po);
   if (nPrevRefs == 1) {
      RELEASEIF(me->piModule);
      FREE(me);
   }
   return nRefs;
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


static
void ModelBase_CancelListener(ModelListener *pListener) 
{
   ModelBase *me = (ModelBase *)pListener->pCancelData;
   
   if (pListener->pNext)
      pListener->pNext->pPrev = pListener->pPrev;
   
   if (pListener->pPrev)
      pListener->pPrev->pNext = pListener->pNext;
   else 
      // No prev, this is head node
      me->pListeners = me->pListeners->pNext;

   pListener->pfnCancel = 0;
   pListener->pNext = pListener->pPrev = 0;
}

int ModelBase_AddListener(IModel *po, ModelListener *pListener)
{
   ME_FROM_MODEL;

   if (pListener == NULL)
      return EBADPARM;

   // Cancel listener if already registered
   if (LISTENER_IsRegistered(pListener))
      LISTENER_Cancel(pListener);

   pListener->pNext = pListener->pPrev = 0;
   pListener->pfnCancel = ModelBase_CancelListener;
   pListener->pCancelData = me;

   if (me->pListeners == 0) {
      // New list
      me->pListeners = pListener;
   } else {
      // Link element into head
      me->pListeners->pPrev = pListener;
      pListener->pNext = me->pListeners;
      me->pListeners = pListener;
   }

   return SUCCESS;
}

void ModelBase_Notify(IModel *po, ModelEvent *pEvent)
{
   ME_FROM_MODEL;
   ModelListener *pListener = me->pListeners;

   if (pEvent == 0)
      return;

   if (!po)
      return;

   pEvent->pModel = po;

   ADDREFIF(po);
   while (pListener != 0) {

      ModelListener *pNext = pListener->pNext;         
      if (pListener->pfnListener) {
         pListener->pfnListener(pListener->pListenerData, pEvent);
      }         
      pListener = pNext;      
   }
   RELEASEIF(po);
}

void ModelBase_NotifyEvent(IModel *po, uint32 evCode)
{
   ModelEvent evt;
   evt.evCode = evCode;

   ModelBase_Notify(po, &evt);
}


void ModelBase_Ctor(ModelBase *me, AEEVTBL(IModel) *pvt, IModule *piModule)
{
   me->pvt      = pvt;
   me->nRefs    = 1;
   me->piModule = piModule;
   ADDREFIF(piModule);

   pvt->AddRef = ModelBase_AddRef;
   pvt->Release = ModelBase_Release;
   pvt->QueryInterface = ModelBase_QueryInterface;
   pvt->AddListener = ModelBase_AddListener;
   pvt->Notify = ModelBase_Notify;

   me->pListeners = 0;
}

void ModelBase_CtorStatic(ModelBase *me, AEEVTBL(IModel) *pvt, IModule *piModule)
{
   ModelBase_Ctor(me, pvt, piModule);
   pvt->Release = ModelBase_ReleaseStatic;
}


int ModelBase_New(IModel **ppo, IModule *piModule)
{
   ModelBase *me = MALLOCREC_VTBL(ModelBase, IModel);
   
   if (me) {
      ModelBase_Ctor(me, GETVTBL(me,IModel), piModule);
      *ppo = (IModel *)me;
      return 0;
   }
   
   return ENOMEMORY;
}

/////////////////////////////////////////////////////////////////
//
// ValueModel implementation
//

void ValueModel_SetValue(IValueModel *po, void *value, int nLen, PFNVALUEFREE pfnFree)
{
   ME_FROM_VALUEMODEL;
   ModelEvent event;

   if (me->pfnSet) {
      me->pfnSet(me->pvSet, value, nLen, &me->value, &me->nLen, &me->pfnFree);
   } else {
      if (me->value && me->pfnFree)
         me->pfnFree(me->value);
      
      me->value = value;
      me->pfnFree = pfnFree;
      me->nLen = nLen;
   }

   event.evCode = EVT_MDL_VALUE;

   ModelBase_Notify(VALUEMODEL_TO_IMODEL(me), &event);
}

void *ValueModel_GetValue(IValueModel *po, int *pnLen)
{
   ME_FROM_VALUEMODEL;

   void *pval = me->value;
   int len    = me->nLen;

   if (me->pfnGet) {
      me->pfnGet(me->pvGet, pval, len, &pval, &len);
   }

   if (pnLen) {
      *pnLen = len;
   }

   return pval;
}

void ValueModel_AdaptGet(IValueModel *po, PFNADAPTGET pfn, void *pvCtx)
{
   ME_FROM_VALUEMODEL;
   
   me->pfnGet = pfn;
   me->pvGet  = pvCtx;
}

void ValueModel_AdaptSet(IValueModel *po, PFNADAPTSET pfn, void *pvCtx)
{
   ME_FROM_VALUEMODEL;
   
   me->pfnSet = pfn;
   me->pvSet  = pvCtx;
}

int ValueModel_QueryInterface(IValueModel *po, AEECLSID id, void **ppo)
{
   if (ModelBase_QueryInterface(IVALUEMODEL_TO_IMODEL(po), id, ppo) == 0)
      return 0;
   
   if (id == AEEIID_VALUEMODEL) {
      *ppo = po;
      IVALUEMODEL_AddRef(po);
      return 0;
   }

   return ECLASSNOTSUPPORT;
}

uint32 ValueModel_Release(IValueModel *po)
{
   ME_FROM_VALUEMODEL;

   if (me->base.nRefs == 1 && me->value && me->pfnFree)
      me->pfnFree(me->value);

   return ModelBase_Release(IVALUEMODEL_TO_IMODEL(po));
}

void ValueModel_Ctor(ValueModel *me, AEEVTBL(IValueModel) *pvt, void *value, PFNVALUEFREE pfnFree, IModule *piModule)
{
   ModelBase_Ctor(VALUEMODEL_TO_MODELBASE(me), (AEEVTBL(IModel) *)pvt, piModule);

   pvt->Release         = ValueModel_Release;
   pvt->QueryInterface  = ValueModel_QueryInterface;
   pvt->SetValue        = ValueModel_SetValue;
   pvt->GetValue        = ValueModel_GetValue;
   pvt->AdaptGet        = ValueModel_AdaptGet;
   pvt->AdaptSet        = ValueModel_AdaptSet;
   
   me->value   = value;
   me->pfnFree = pfnFree;
}


//
// Actual implementation
//

int ValueModel_New(IValueModel **ppo, IModule *piModule)
{
   ValueModel *me = MALLOCREC_VTBL(ValueModel, IValueModel);
   
   if (me) {
      ValueModel_Ctor(me, GETVTBL(me,IValueModel), 0, 0, piModule);
      *ppo = (IValueModel *)me;
      return 0;
   }
   
   return ENOMEMORY;
}


/////////////////////////////////////////////////////////////////
//
// ListModel
//

void ListModel_Notify(IListModel *me, int evtCode, int pos, int oldSize, int newSize)
{
   ListModelEvent event;
   
   LISTMODELEVENT_Init(&event, evtCode, pos, oldSize, newSize);
   IMODEL_Notify(ILISTMODEL_TO_IMODEL(me), LISTMODELEVENT_TO_MODELEVENT(&event));
}


int ListModel_QueryInterface(IListModel *po, AEECLSID id, void **ppo) 
{
   if (ModelBase_QueryInterface(ILISTMODEL_TO_IMODEL(po), id, ppo) == 0)
      return 0;

   if (id == AEEIID_LISTMODEL) {
      *ppo = po;
      ILISTMODEL_AddRef(po);
      return 0;
   }

   return ECLASSNOTSUPPORT;
}

int ListModel_Size(IListModel *po)
{
   return 0;
}

int ListModel_GetAt(IListModel *po, uint32 nIndex, void **ppo)
{
   return EBADPARM;
}

void ListModel_Ctor(ListModel *me, AEEVTBL(IListModel) *pvt, IModule *piModule)
{
   ModelBase_Ctor(LISTMODEL_TO_MODELBASE(me), (AEEVTBL(IModel) *)pvt, piModule);

   pvt->QueryInterface = ListModel_QueryInterface;
   pvt->GetAt = ListModel_GetAt;
   pvt->Size = ListModel_Size;
}

//
// Actual implementation
//

int ListModel_New(IListModel **ppo, IModule *piModule)
{
   ListModel *me = MALLOCREC_VTBL(ListModel, IListModel);
   
   if (me) {
      ListModel_Ctor(me, GETVTBL(me,IListModel), piModule);
      *ppo = (IListModel*)me;
      return 0;
   }
   
   return ENOMEMORY;
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
// InterfaceModel implementation


void InterfaceModel_SetIPtr(IInterfaceModel *po, IBase *piBase, AEECLSID clsidType)
{
   ME_FROM_INTERFACEMODEL;

   RELEASEIF(me->piBase);

   me->piBase  = piBase;
   me->clsid   = clsidType;
   ADDREFIF(me->piBase);

   ModelBase_NotifyEvent(INTERFACEMODEL_TO_IMODEL(me), EVT_MDL_VALUE);
}

int InterfaceModel_GetIPtr(IInterfaceModel *po, AEECLSID clsid, void **ppif)
{
   ME_FROM_INTERFACEMODEL;

   if (clsid == me->clsid && me->piBase) {
      *ppif = me->piBase;
      IBASE_AddRef(me->piBase);
      return SUCCESS;
   }

   return ECLASSNOTSUPPORT;
}

int InterfaceModel_QueryInterface(IInterfaceModel *po, AEECLSID id, void **ppo)
{
   if (id == AEEIID_INTERFACEMODEL) {
      *ppo = po;
      IINTERFACEMODEL_AddRef(po);
      return SUCCESS;
   }
   return ModelBase_QueryInterface((IModel*)po, id, ppo);
}


uint32 InterfaceModel_Release(IInterfaceModel *po)
{
   ME_FROM_INTERFACEMODEL;

   if (me->base.nRefs == 1) {
      RELEASEIF(me->piBase);
   }

   return ModelBase_Release(IINTERFACEMODEL_TO_IMODEL(po));
}


void InterfaceModel_Ctor(InterfaceModel *me, AEEVTBL(IInterfaceModel) *pvt, IModule *piModule)
{
   ModelBase_Ctor(&me->base, (AEEVTBL(IModel) *)pvt, piModule);

   pvt->Release         = InterfaceModel_Release;
   pvt->QueryInterface  = InterfaceModel_QueryInterface;
   pvt->SetIPtr         = InterfaceModel_SetIPtr;
   pvt->GetIPtr         = InterfaceModel_GetIPtr;
}


int InterfaceModel_New(IInterfaceModel **ppo, IModule *piModule)
{
   InterfaceModel *me = MALLOCREC_VTBL(InterfaceModel, IInterfaceModel);
   
   if (me) {
      InterfaceModel_Ctor(me, GETVTBL(me,IInterfaceModel), piModule);
      *ppo = (IInterfaceModel*)me;
      return 0;
   }
   
   return ENOMEMORY;
}


/////////////////////////////////////////////////////////////////////
// ArrayModel implementation - derived from ListModel


int ArrayModel_QueryInterface(IArrayModel *po, AEECLSID id, void **ppo)
{
   if (id == AEEIID_ARRAYMODEL) {
      *ppo = po;
      IARRAYMODEL_AddRef(po);
      return 0;
   } 
   return ListModel_QueryInterface((IListModel*)po, id, ppo);
}

int ArrayModel_GetAt(IArrayModel *po, uint32 nIndex, void **ppoItem)
{
   ME_FROM_ARRAYMODEL;
   uint8 *pItem = me->pItems;

   if (nIndex >= me->nItems) {
      return EBADPARM;
   }

   *ppoItem = (pItem + (me->nSize * nIndex));

   return SUCCESS;
}

int ArrayModel_Size(IArrayModel *po)
{
   ME_FROM_ARRAYMODEL;
   return me->nItems;
}

void ArrayModel_SetItems(IArrayModel *po, void *pItems, uint32 nItems, uint16 nItemSize)
{
   ME_FROM_ARRAYMODEL;

   uint32 nItemsOld = me->nItems;

   me->pItems  = pItems;
   me->nItems  = nItems;
   me->nSize   = nItemSize;
   
   ListModel_Notify((IListModel*)po, EVT_MDL_LIST_CHANGE, 0, nItemsOld, me->nItems);
}

void ArrayModel_Ctor(ArrayModel *me, AEEVTBL(IArrayModel) *pvt, IModule *piModule)
{
   ListModel_Ctor(&me->base, (AEEVTBL(IListModel)*)pvt, piModule);

   pvt->QueryInterface = ArrayModel_QueryInterface;
   pvt->Size           = ArrayModel_Size;
   pvt->GetAt          = ArrayModel_GetAt;
   pvt->SetItems       = ArrayModel_SetItems;
}


int ArrayModel_New(IArrayModel **ppo, IModule *piModule)
{
   ArrayModel *me = MALLOCREC_VTBL(ArrayModel, IArrayModel);

   *ppo = (IArrayModel*)me;

   if (me) {
      ArrayModel_Ctor(me, GETVTBL(me,IArrayModel), piModule);
      return 0;
   }
   
   return ENOMEMORY;
}




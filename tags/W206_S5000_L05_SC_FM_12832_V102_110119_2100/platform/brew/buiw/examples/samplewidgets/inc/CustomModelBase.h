
/*
  ========================================================================

  FILE:  AEEModelBase.h
  
  SERVICES:  

  GENERAL DESCRIPTION: Base implementation for models

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2005 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __AEEMODELBASE_H__
#define __AEEMODELBASE_H__

#include "AEEWModel.h"
#include "AEEListModel.h"
#include "AEEArrayModel.h"
#include "AEEVectorModel.h"

#include "Vector.h"

typedef struct ModelBase {
   AEEVTBL(IModel) * pvt;
   uint32            nRefs;
   IModule *         piModule;
   ModelListener *   pListeners;
} ModelBase;


typedef struct ValueModel {
   ModelBase         base;
   void *            value;
   int               nLen;
   PFNVALUEFREE      pfnFree;
   PFNADAPTGET       pfnGet;
   void *            pvGet;
   PFNADAPTSET       pfnSet;
   void *            pvSet;
} ValueModel;


typedef struct InterfaceModel {
   ModelBase      base;
   IBase *        piBase;
   AEECLSID       clsid;   // IID of interface we are holding
} InterfaceModel;


typedef struct ListModel {
   ModelBase  base;
} ListModel;


typedef struct ArrayModel {
   ListModel   base;
   void *      pItems;
   uint32      nItems;
   uint16      nSize;
} ArrayModel;


// 
// ModelBase
//
int    ModelBase_New(IModel **ppo, IModule *piModule);
void   ModelBase_Ctor(ModelBase *p, AEEVTBL(IModel) *pvt, IModule *piModule);
void   ModelBase_CtorStatic(ModelBase *me, AEEVTBL(IModel) *pvt, IModule *piModule);
uint32 ModelBase_AddRef(IModel *p);
uint32 ModelBase_Release(IModel *p);
uint32 ModelBase_ReleaseStatic(IModel *po);
int    ModelBase_QueryInterface(IModel *p, AEECLSID id, void **ppo);
int    ModelBase_AddListener(IModel *p, ModelListener *pListener);
void   ModelBase_Notify(IModel *p, ModelEvent *pEvent);
void   ModelBase_NotifyEvent(IModel *po, uint32 evCode);
// Safe upcast
static __inline 
IModel *MODELBASE_TO_IMODEL(ModelBase *p) { return (IModel *)(void*)p; }

//
// ValueModel
//
int    ValueModel_New(IValueModel **ppo, IModule *piModule);
uint32 ValueModel_Release(IValueModel *po);
void   ValueModel_Ctor(ValueModel *p, AEEVTBL(IValueModel) *pvt, void *value, PFNVALUEFREE pfnFree, IModule *piModule);
int    ValueModel_QueryInterface(IValueModel *po, AEECLSID id, void **ppo);
void   ValueModel_SetValue(IValueModel *p, void *value, int nLen, PFNVALUEFREE pfnFree);
void * ValueModel_GetValue(IValueModel *p, int *pnLen);
void   ValueModel_AdaptGet(IValueModel *po, PFNADAPTGET pfn, void *pvCtx);
void   ValueModel_AdaptSet(IValueModel *po, PFNADAPTSET pfn, void *pvCtx);

// Safe upcasts
static __inline 
IModel *VALUEMODEL_TO_IMODEL(ValueModel *p) { return (IModel *)(void*)p; }
static __inline 
ModelBase *VALUEMODEL_TO_MODELBASE(ValueModel *p) { return (ModelBase *)p; }


//
// InterfaceModel
//
int      InterfaceModel_New            (IInterfaceModel **ppo, IModule *piModule);
uint32   InterfaceModel_Release        (IInterfaceModel *po);
void     InterfaceModel_Ctor           (InterfaceModel *p, AEEVTBL(IInterfaceModel) *pvt, IModule *piModule);
int      InterfaceModel_QueryInterface (IInterfaceModel *po, AEECLSID id, void **ppo);
void     InterfaceModel_SetIPtr        (IInterfaceModel *po, IBase *piBase, AEECLSID clsidType);
int      InterfaceModel_GetIPtr        (IInterfaceModel *po, AEECLSID clsid, void **ppif);

static __inline 
   IModel *INTERFACEMODEL_TO_IMODEL(InterfaceModel *p) { return (IModel *)(void*)p; }

static __inline 
   ModelBase *INTERFACEMODEL_TO_MODELBASE(InterfaceModel *p) { return (ModelBase *)p; }


//
// ListModel
//
int     ListModel_New(IListModel **ppo, IModule *piModule);
void    ListModel_Ctor(ListModel *me, AEEVTBL(IListModel) *pvt, IModule *piModule);
int     ListModel_QueryInterface(IListModel *po, AEECLSID id, void **ppo);
int     ListModel_Size(IListModel *po);
int     ListModel_GetAt(IListModel *po, uint32 nIndex, void **ppoItem);
// Utility functions
void ListModel_Notify(IListModel *me, int evtCode, int pos, int oldSize, int newSize);

static __inline 
IModel *LISTMODEL_TO_IMODEL(ListModel *p) { return (IModel *)(void*)p; }
static __inline 
IListModel *LISTMODEL_TO_ILISTMODEL(ListModel *p) { return (IListModel *)(void*)p; }
static __inline 
ModelBase *LISTMODEL_TO_MODELBASE(ListModel *p) { return (ModelBase *)p; }


//
// ArrayModel
//
int   ArrayModel_QueryInterface  (IArrayModel *po, AEECLSID id, void **ppo);
int   ArrayModel_GetAt           (IArrayModel *po, uint32 nIndex, void **ppoItem);
int   ArrayModel_Size            (IArrayModel *po);
void  ArrayModel_SetItems        (IArrayModel *po, void *pItems, uint32 nItems, uint16 nItemSize);
void  ArrayModel_Ctor            (ArrayModel *me, AEEVTBL(IArrayModel) *pvt, IModule *piModule);
int   ArrayModel_New             (IArrayModel **ppo, IModule *piModule);


static __inline 
IModel *ARRAYMODEL_TO_IMODEL(ArrayModel *p) { return (IModel *)(void*)p; }

static __inline 
IListModel *ARRAYMODEL_TO_ILISTMODEL(ArrayModel *p) { return (IListModel *)(void*)p; }

static __inline 
ModelBase *ARRAYMODEL_TO_MODELBASE(ArrayModel *p) { return (ModelBase *)p; }


#endif /* __AEEMODELBASE_H__ */

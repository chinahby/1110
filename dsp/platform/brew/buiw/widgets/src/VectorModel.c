/*
  ========================================================================

  FILE:  VectorModel.c
  
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

#include "VectorModel.h"

#define ME_FROM_VECTORMODEL    VectorModel *me = (VectorModel*)po


/////////////////////////////////////////////////////////////////////
// VectorModel implementation - derived from ListModel


int VectorModel_QueryInterface(IVectorModel *po, AEECLSID id, void **ppo)
{
   if (id == AEEIID_VECTORMODEL) {
      *ppo = po;
      IVECTORMODEL_AddRef(po);
      return 0;
   } 
   return ListModel_QueryInterface((IListModel*)po, id, ppo);
}

uint32 VectorModel_Release(IVectorModel *po)
{
   ME_FROM_VECTORMODEL;

   if (me->base.base.nRefs == 1) {
      Vector_Dtor(&me->vector);
   }

   return ModelBase_Release((IModel*)po);
}

int VectorModel_GetAt(IVectorModel *po, uint32 nIndex, void **ppoItem)
{
   ME_FROM_VECTORMODEL;
   return Vector_GetAt(&me->vector, nIndex, ppoItem);
}


static __inline int VectorModel_NotifyListChange(IVectorModel *po, int nErr, int pos, int nOldSize, int nNewSize) {
   if (!nErr) {
      ListModel_Notify((IListModel*)po, EVT_MDL_LIST_CHANGE, pos, nOldSize, nNewSize);
   }
   return nErr;
}


int VectorModel_ReplaceAt(IVectorModel *po, uint32 nIndex, void *pvItem)
{
   ME_FROM_VECTORMODEL;
   
   int nItems  = Vector_Size(&me->vector);
   int nErr    = Vector_ReplaceAt(&me->vector, nIndex, pvItem);

   return VectorModel_NotifyListChange(po, nErr, nIndex, nItems, nItems);
}


int VectorModel_InsertAt(IVectorModel *po, uint32 nIndex, void *pvItem)
{
   ME_FROM_VECTORMODEL;

   int nItems  = Vector_Size(&me->vector);
   int nErr    = Vector_InsertAt(&me->vector, nIndex, pvItem);

   // If nIndex is greater than the previous size of the Vector 
   // (ie, VECTOR_LAST) then Vector adds a new item to the end of the list.
   // We want to ensure that the nIndex is set correctly, so we won't 
   // confuse whoever (ahem..ListWidget) is listening to our notifications
   //
   // Ugh.  We know too much about the implementation of Vector here!
   //
   if ( (nItems >= 0) && (nIndex >= (uint32) nItems) ) {
      nIndex = nItems;
   }

   return VectorModel_NotifyListChange(po, nErr, nIndex, nItems, nItems+1);
}

int VectorModel_DeleteAt(IVectorModel *po, uint32 nIndex)
{
   ME_FROM_VECTORMODEL;

   int nItems  = Vector_Size(&me->vector);
   int nErr    = Vector_DeleteAt(&me->vector, nIndex);

   return VectorModel_NotifyListChange(po, nErr, nIndex, nItems, nItems-1);
}


void VectorModel_DeleteAll(IVectorModel *po)
{
   ME_FROM_VECTORMODEL;

   int nItems  = Vector_Size(&me->vector);
   Vector_DeleteAll(&me->vector);
   VectorModel_NotifyListChange(po, SUCCESS, 0, nItems, 0);
}


int VectorModel_EnsureCapacity(IVectorModel *po, uint32 nRequired, uint32 nGrowBy)
{
   ME_FROM_VECTORMODEL;
   return Vector_EnsureCapacity(&me->vector, nRequired, nGrowBy);
}


PFNNOTIFY VectorModel_SetPfnFree(IVectorModel *po, PFNNOTIFY pfnFree)
{
   ME_FROM_VECTORMODEL;
   return Vector_SetPfnFree(&me->vector, pfnFree);
}


int VectorModel_Size(IVectorModel *po)
{
   ME_FROM_VECTORMODEL;
   return Vector_Size(&me->vector);
}


void VectorModel_Ctor(VectorModel *me, AEEVTBL(IVectorModel) *pvt, IModule *piModule)
{
   ListModel_Ctor(&me->base, (AEEVTBL(IListModel)*)pvt, piModule);

#define VTF(name)   pvt->name = VectorModel_##name
   
   VTF(Release);
   VTF(QueryInterface);
   VTF(Size);
   VTF(GetAt);
   VTF(ReplaceAt);
   VTF(InsertAt);
   VTF(DeleteAt);
   VTF(DeleteAll);
   VTF(EnsureCapacity);
   VTF(SetPfnFree);

   Vector_Ctor(&me->vector);
}

int VectorModel_New(IVectorModel **ppo, IModule *piModule)
{
   VectorModel *me = MALLOCREC_VTBL(VectorModel, IVectorModel);

   *ppo = (IVectorModel*)me;

   if (!me) {
      return ENOMEMORY;
   }

   VectorModel_Ctor(me, GETVTBL(me,IVectorModel), piModule);

   return SUCCESS;
}





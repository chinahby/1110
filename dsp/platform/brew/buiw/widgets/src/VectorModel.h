/*
  ========================================================================

  FILE:  PtrArrayModel.h
  
  SERVICES:  

  GENERAL DESCRIPTION: PtrArrayModel for lists

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#ifndef __PTRARRAYMODEL_H__
#define __PTRARRAYMODEL_H__

#include "ModelBase.h"
#include "Vector.h"

typedef struct VectorModel {
   ListModel      base;
   Vector         vector;
} VectorModel;


// Model creation
int       VectorModel_New           (IVectorModel **ppo, IModule *piModule);
void      VectorModel_Ctor          (VectorModel *me, AEEVTBL(IVectorModel) *pvt, IModule *piModule);
int       VectorModel_QueryInterface(IVectorModel *po, AEECLSID id, void **ppo);
uint32    VectorModel_Release       (IVectorModel *po);
int       VectorModel_GetAt         (IVectorModel *po, uint32 nIndex, void **ppoItem);
int       VectorModel_ReplaceAt     (IVectorModel *po, uint32 nIndex, void *pvItem);
int       VectorModel_Add           (IVectorModel *po, void *pvItem);
int       VectorModel_InsertAt      (IVectorModel *po, uint32 nIndex, void *pvItem);
int       VectorModel_DeleteAt      (IVectorModel *po, uint32 nIndex);
void      VectorModel_DeleteAll     (IVectorModel *po);
int       VectorModel_EnsureCapacity(IVectorModel *po, uint32 nRequired, uint32 nGrowBy);
PFNNOTIFY VectorModel_SetPfnFree    (IVectorModel *po, PFNNOTIFY pfnFree);
int       VectorModel_Size          (IVectorModel *po);

#endif   //__PTRARRAYMODEL_H__

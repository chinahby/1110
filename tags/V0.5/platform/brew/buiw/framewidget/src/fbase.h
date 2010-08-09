/*
  ========================================================================

  FILE:  fbase.h
  
  SERVICES:  

  GENERAL DESCRIPTION: Implements the base model methods.

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#ifndef __FBASE_H__
#define __FBASE_H__

#include "AEEModel.h"


// dummy ModelListener node
typedef struct LNode LNode;
struct LNode {
   LNode *    pNext;
   LNode *    pPrev;
};


// ModelBase struct
typedef struct ModelBase {
   AEEVTBL(IModel) * pvt;
   uint32            nRefs;
   IModule *         piModule;
   LNode             head;    // head of listener list -- dummy node, not a pointer
} ModelBase;


// ModelBase methods
int      ModelBase_New           (IModel **ppo, IModule *piModule);
void     ModelBase_Ctor          (ModelBase *me, AEEVTBL(IModel) *pvt, IModule *piModule);
uint32   ModelBase_AddRef        (IModel *po);
uint32   ModelBase_Release       (IModel *po);
int      ModelBase_QueryInterface(IModel *po, AEECLSID id, void **ppo);
int      ModelBase_AddListener   (IModel *po, ModelListener *pListener);
void     ModelBase_Notify        (IModel *po, ModelEvent *pEvent);



#endif   //__FBASE_H__

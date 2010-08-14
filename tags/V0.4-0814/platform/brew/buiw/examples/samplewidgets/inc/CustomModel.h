/*
  ========================================================================

  FILE:                 CustomModel.h
  
  SERVICES:             ICustomModel class 

  GENERAL DESCRIPTION:  If this were a real model, a description of what
                        this model does would be given here.

  ========================================================================
  ========================================================================
    
               Copyright © 2005 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#ifndef __CUSTOMMODEL_H__
#define __CUSTOMMODEL_H__

#include "AEEWModel.h"

// Offical class and interface IDs from the BREW Class ID Generator
#include "bid/AEECLSID_CUSTOMMODEL.BID"
#include "bid/AEEIID_CUSTOMMODEL.BID"


/////////////////////////////////////////////////////////////////
// ICustomModel interface

typedef struct ICustomModel ICustomModel;
#define INHERIT_ICustomModel(iname) \
   INHERIT_IModel(iname);\
   void  (*CustomFunc) (iname *po)


AEEINTERFACE(ICustomModel) {
   INHERIT_ICustomModel(ICustomModel);
};

#define ICUSTOMMODEL_AddRef(p)                AEEGETPVTBL((p),ICustomModel)->AddRef(p)
#define ICUSTOMMODEL_Release(p)               AEEGETPVTBL((p),ICustomModel)->Release(p)
#define ICUSTOMMODEL_QueryInterface(p,c,d)    AEEGETPVTBL((p),ICustomModel)->QueryInterface((p),(c),(d))
#define ICUSTOMMODEL_AddListener(p,pl)        AEEGETPVTBL((p),ICustomModel)->AddListener((p),(pl))
#define ICUSTOMMODEL_Size(p)                  AEEGETPVTBL((p),ICustomModel)->Size(p)
#define ICUSTOMMODEL_GetAt(p,i,ppo)           AEEGETPVTBL((p),ICustomModel)->GetAt((p),(i),(ppo))
#define ICUSTOMMODEL_CustomFunc(p,pi,n,s)     AEEGETPVTBL((p),ICustomModel)->CustomFunc(p)

// Safe upcasts
static __inline IModel *ICUSTOMMODEL_TO_IMODEL(ICustomModel *p) { 
   return (IModel *)p; 
}

#endif /* __CUSTOMMODEL_H__ */

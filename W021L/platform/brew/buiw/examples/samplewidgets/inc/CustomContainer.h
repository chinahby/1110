/*
  ========================================================================

  FILE:                 CustomContainer.h
  
  SERVICES:             ICustomContainer class 

  GENERAL DESCRIPTION:  If this were a real container, a description of 
                        what this continer does would be given here.

  ========================================================================
  ========================================================================
    
               Copyright © 2005 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#ifndef __CUSTOMCONTAINER_H__
#define __CUSTOMCONTAINER_H__

#include "AEEContainer.h"

// Offical class and interface IDs from the BREW Class ID Generator
#include "bid/AEECLSID_CUSTOMCONTAINER.BID"
#include "bid/AEEIID_CUSTOMCONTAINER.BID"

typedef struct ICustomContainer ICustomContainer;

#define INHERIT_ICustomContainer(iname) \
   INHERIT_IContainer(iname);\
   int      (*CustomFunc) (iname *)

AEEINTERFACE(ICustomContainer) {
   INHERIT_ICustomContainer(ICustomContainer);
};

#define ICUSTOMCONTAINER_AddRef(p)                 AEEGETPVTBL(p,ICustomContainer)->AddRef(p)
#define ICUSTOMCONTAINER_Release(p)                AEEGETPVTBL(p,ICustomContainer)->Release(p)
#define ICUSTOMCONTAINER_QueryInterface(p,i,p2)    AEEGETPVTBL(p,ICustomContainer)->QueryInterface((p),(i),(p2))
#define ICUSTOMCONTAINER_Invalidate(p,pw,prc,f)    AEEGETPVTBL(p,ICustomContainer)->Invalidate((p),(pw),(prc),(f))
#define ICUSTOMCONTAINER_Locate(p,pw,ppr,prc)      AEEGETPVTBL(p,ICustomContainer)->Locate((p),(pw),(ppr),(prc))
#define ICUSTOMCONTAINER_Remove(p,pw)              AEEGETPVTBL(p,ICustomContainer)->Remove((p),(pw))         
#define ICUSTOMCONTAINER_GetWidget(p,pw,d,w)       AEEGETPVTBL(p,ICustomContainer)->GetWidget((p),(pw),(d),(w)) 

static __inline int ICUSTOMCONTAINER_Insert(ICustomContainer *po, IWidget *pw ,IWidget *pib, void *pData) {
   return AEEGETPVTBL(po,ICustomContainer)->Insert(po,pw,pib,(const void *)pData);
}

static __inline IContainer *ICUSTOMCONTAINER_TO_ICONTAINER(ICustomContainer *me) {
   return (IContainer *)me;
}

#endif /* __CUSTOMCONTAINER_H__ */

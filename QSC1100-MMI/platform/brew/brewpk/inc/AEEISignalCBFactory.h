#ifndef AEEISIGNALCBFACTORY_H
#define AEEISIGNALCBFACTORY_H
/*======================================================================
Copyright © 2006-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
========================================================================

DESCRIPTION:  Definition of ISignalCBFactory interface.

======================================================================*/

#include "AEEIQI.h"
#include "AEEISignal.h"
#include "AEEISignalCtl.h"

#define AEEIID_ISignalCBFactory 0x01043541

#define INHERIT_ISignalCBFactory(iname) \
   INHERIT_IQI(iname); \
   int (*CreateSignal)(iname *, \
      void (*pfn)(void *pCx), \
      void *pCx, \
      ISignal * *ppiSig, \
      ISignalCtl * *ppiSigCtl \
   )

AEEINTERFACE_DEFINE(ISignalCBFactory);

static __inline uint32 ISignalCBFactory_AddRef(ISignalCBFactory *me)
{
   return AEEGETPVTBL(me,ISignalCBFactory)->AddRef(me);
}

static __inline uint32 ISignalCBFactory_Release(ISignalCBFactory *me)
{
   return AEEGETPVTBL(me,ISignalCBFactory)->Release(me);
}

static __inline int ISignalCBFactory_QueryInterface(ISignalCBFactory *me, AEEIID iid, void * *ppOut)
{
   return AEEGETPVTBL(me,ISignalCBFactory)->QueryInterface(me, iid, ppOut);
}

static __inline int ISignalCBFactory_CreateSignal(ISignalCBFactory *me, void (*pfn)(void *pCx), void *pCx, ISignal * *ppiSig, ISignalCtl * *ppiSigCtl)
{
   return AEEGETPVTBL(me,ISignalCBFactory)->CreateSignal(me, pfn, pCx, ppiSig, ppiSigCtl);
}

/*
===============================================================================
INTERFACES DOCUMENTATION
===============================================================================

ISignalCBFactory Interface

Description:
	 ISignalCBFactory allows creation of signal objecs.  Signals provide
         asynchronous notification of events.
   
===============================================================================

ISignalCBFactory_AddRef()

Description:
    This function is inherited from IQI_AddRef(). 

See Also:
    ISignalCBFactory_Release()

===============================================================================

ISignalCBFactory_Release()

Description:
    This function is inherited from IQI_Release(). 

See Also:
    ISignalCBFactory_AddRef()

===============================================================================

ISignalCBFactory_QueryInterface()

Description:
    This function is inherited from IQI_QueryInterface(). 

===============================================================================

ISignalCBFactory_CreateSignal()

Description:
         Create a signal object that invokes a callback function when set.

Prototype:

   int ISignalCBFactory_CreateSignal(ISignalCBFactory* pif, 
                                     void (*pfn)(void *pCx),
                                     void *pCx,
                                     ISignal** ppiSig, 
                                     ISignalCtl** ppiSigCtl)

Parameters:
   pif: Pointer to an ISignalCBFactory interface
   pfn: Pointer to the function to be invoked when signal is set.
   pCx: Argument to be passed to the callback when signal is set.
   ppiSig : [OUT] Optional. resulting signal object's ISignal interface.
   ppiSigCtl : [OUT] resulting signal object's ISignalCtl interface.

Return Value:

   Error code.

Comments:
   None

Side Effects:
   None

===============================================================================
*/

#endif /* AEEISIGNALCBFACTORY_H */


#ifndef AEEISIGNALCTL_H
#define AEEISIGNALCTL_H
/*======================================================================
Copyright © 2006-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
========================================================================

DESCRIPTION:  Definition of ISignalCtl interface.

======================================================================*/

#include "AEEISignal.h"

#define AEEIID_ISignalCtl 0x01041079

#define INHERIT_ISignalCtl(iname) \
   INHERIT_ISignal(iname); \
   int (*Detach)(iname *); \
   int (*Enable)(iname *)

AEEINTERFACE_DEFINE(ISignalCtl);

static __inline uint32 ISignalCtl_AddRef(ISignalCtl *me)
{
   return AEEGETPVTBL(me,ISignalCtl)->AddRef(me);
}

static __inline uint32 ISignalCtl_Release(ISignalCtl *me)
{
   return AEEGETPVTBL(me,ISignalCtl)->Release(me);
}

static __inline int ISignalCtl_QueryInterface(ISignalCtl *me, AEEIID iid, void * *ppOut)
{
   return AEEGETPVTBL(me,ISignalCtl)->QueryInterface(me, iid, ppOut);
}

static __inline int ISignalCtl_Set(ISignalCtl *me)
{
   return AEEGETPVTBL(me,ISignalCtl)->Set(me);
}

static __inline int ISignalCtl_Detach(ISignalCtl *me)
{
   return AEEGETPVTBL(me,ISignalCtl)->Detach(me);
}

static __inline int ISignalCtl_Enable(ISignalCtl *me)
{
   return AEEGETPVTBL(me,ISignalCtl)->Enable(me);
}

/*
===============================================================================
INTERFACES DOCUMENTATION
===============================================================================

ISignalCtl Interface

Description:

  ISignalCtl is the controlling interface of Signal object. ISignalCtl is 
  inherited from ISignal interface. Signal Object can be created with 
  ISignalQ_CreateSignal() or ISignalFactory_CreateSignal(). Depending on
  how it is created, a signal object when set causes either invokation or a
  callback.

  A signal created on SignalQ when set will be enqueued to the SignalQ object 
  enabling the threads waiting on ISignalQ_Pop() to receive the associated
  signal data. 

  A Signal created from Signal Factory when set will cause invocation of the
  associated SignalHandler's ISignalHandler_Notify() method.

===============================================================================

ISignalCtl_AddRef()

Description:
    This function is inherited from IQI_AddRef(). 

See Also:
    ISignalCtl_Release()

===============================================================================

ISignalCtl_Release()

Description:
    This function is inherited from IQI_Release(). 

See Also:
    ISignalCtl_AddRef()

===============================================================================

ISignalCtl_QueryInterface()

Description:
    This function is inherited from IQI_QueryInterface(). 

===============================================================================

ISignalCtl_Set()

Description:

   This function is inherited from ISignal_Set(), but has slightly different
   behavior:  ISignalCtl_Set() causes the signal to be dispatched even if 
   the Signal is not enabled.

===============================================================================

ISignalCtl_Detach()

Description:
   Detaches the signal object from causing notifications to the client. This 
   will also release the Signal object's association with client context.

Prototype:

   int ISignalCtl_Detach(ISignalCtl* pif)

Parameters:
   pif: Pointer to an ISignalCtl interface

Return Value:
   Error code.

===============================================================================

ISignalCtl_Enable()

Description:
   Enables the Signal to be dispatched when it becomes ready.

   New Signals are enabled when created.  Once a Signal has been
   set and dispatched, the Signal is disabled until the owner
   calls ISignalCtl_Enable().  When disabled, calls to ISignal_Set()
   are recorded, but not dispatched until the next call to 
   ISignalCtl_Enable().

Prototype:

   int ISignalCtl_Enable(ISignalCtl* pif)

Parameters:
   pif: Pointer to an ISignalCtl interface

Comments:
   If the signal has been added to a SignalBus, ISignalCtl_Enable() will
   also enable the SignalBus if it is not already in enabled state.
   SignalBus is enabled when at least one signal on the bus is enabled.

Return Value:
   Error code.

See Also:
   ISignalBus, ISignalCtl_Set(), ISignal_Set()

===============================================================================

*/

#endif /* AEEISIGNALCTL_H */


#ifndef AEEISIGNAL_H
#define AEEISIGNAL_H
/*
=======================================================================
Copyright © 2005-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=======================================================================
*/
#include "AEEIQI.h"

#define AEEIID_ISignal 0x010285f5

#define INHERIT_ISignal(iname) \
   INHERIT_IQI(iname);\
   int (*Set)   (iname*  me)

AEEINTERFACE_DEFINE(ISignal);

static __inline uint32 ISignal_AddRef(ISignal* me)
{
   return AEEGETPVTBL(me,ISignal)->AddRef(me);
}

static __inline uint32 ISignal_Release(ISignal* me)
{
   return AEEGETPVTBL(me,ISignal)->Release(me);
}

static __inline int ISignal_QueryInterface(ISignal* me, AEECLSID cls, void** ppo)
{
   return AEEGETPVTBL(me,ISignal)->QueryInterface(me, cls, ppo);
}

static __inline int ISignal_Set(ISignal* me)
{
   return AEEGETPVTBL(me,ISignal)->Set(me);
}

/*
===============================================================================
   INTERFACE DOCUMENTATION
===============================================================================

ISignal Interface

Description: 

   ISignal interface serves asynchronous notifications. These notifications are
  triggered by object being observed, using method ISignal_Set(). Signal notifications
  are received only when the Signal is enabled. Recepient of the signal notifications
  can control the Signal using ISignalCtl interface. Signal is auto-disabled when a 
  notification is retrieved.

   Signal Notifications are retrieved as a callback or as a invocation depending on
  how it is created.

   A signal created on SignalQ when set will be enqueued to the SignalQ object 
  enabling the threads waiting on ISignalQ_Pop() to receive the associated
  signal data. These are callbacks.

   A Signal created from Signal Factory when set will cause invocation of the
  associated SignalHandler's ISignalHandler_Notify() method.

===============================================================================

ISignal_AddRef()

Description:
    This function is inherited from IQI_AddRef(). 

See Also:
    ISignal_Release()

===============================================================================

ISignal_Release()

Description:
    This function is inherited from IQI_Release(). 

See Also:
    ISignal_AddRef()

===============================================================================

ISignal_QueryInterface()

Description:
    This function is inherited from IQI_QueryInterface(). 

===============================================================================

ISignal_Set()

Description:

   Marks a signal as "ready", queueing it on its assigned signal queue or 
   causing associated SignalHandler to be invoked.
   
   If the Signal has been detached, this call has no effect.

   If the Signal is not enabled, the set will be recorded and dispatched
   when the Signal becomes enabled.

Prototype:

   int ISignal_Set(ISignal* pif)

Parameters:
   pif: Pointer to an ISignal interface

Comments:

   ISignal_Set() will trigger the associated signal dispatcher to
   process the signal. The Signal is dispatched when it is marked
   "ready" and enabled.  The Signal can be enabled by the client using
   ISignalCtl_Enable(). Signals just created are enabled.  Signal
   dispatching is asynchronous to ISignal_Set() operation. The Signal
   when set will remain "ready" until it is enabled and
   dispatched. Signal dispatching involves notifying the client.

   If the signal is created using ISignalQ_CreateSignal(), thread blocking on 
   ISignalQ_Pop() will unblock retrieving the associated SignalData. If there 
   are no waiting threads, the Signal can be dequeued by subsequent 
   ISignalQ_Pop() operation.
   
   Similary, if the thread is created from ISignalFactory the associated 
   SignalHandler will be notified when a thread in the apartment becomes 
   available.

Return Value:
   Error code.

===============================================================================
*/
#endif /* #ifndef AEEISIGNAL_H */


#ifndef AEEISIGNALBUS_H
#define AEEISIGNALBUS_H
/*
=======================================================================
Copyright © 2005-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=======================================================================
*/
#include "AEEISignal.h"

#define AEEIID_ISignalBus  0x0103d14b

#define INHERIT_ISignalBus(iname) \
   INHERIT_IQI(iname); \
   int (*Strobe)  (iname* me); \
   int (*Set)     (iname* me); \
   int (*Clear)   (iname* me); \
   int (*OnEnable)(iname* me, ISignal* piSignal); \
   int (*Add)     (iname* me, ISignal* piSignal); \
   int (*Remove)  (iname* me, ISignal* piSignal)

AEEINTERFACE_DEFINE(ISignalBus);

static __inline uint32 ISignalBus_AddRef(ISignalBus* me)
{
   return AEEGETPVTBL(me, ISignalBus)->AddRef(me);
}

static __inline uint32 ISignalBus_Release(ISignalBus* me)
{
   return AEEGETPVTBL(me, ISignalBus)->Release(me);
}

static __inline int ISignalBus_QueryInterface(ISignalBus* me, AEEIID id, void** ppo)
{
   return AEEGETPVTBL(me, ISignalBus)->QueryInterface(me, id, ppo);
}

static __inline int ISignalBus_Strobe(ISignalBus* me)
{
   return AEEGETPVTBL(me, ISignalBus)->Strobe(me);
}

static __inline int ISignalBus_Set(ISignalBus* me)
{
   return AEEGETPVTBL(me, ISignalBus)->Set(me);
}

static __inline int ISignalBus_Clear(ISignalBus* me)
{
   return AEEGETPVTBL(me, ISignalBus)->Clear(me);
}

static __inline int ISignalBus_OnEnable(ISignalBus* me, ISignal* piSignal)
{
   return AEEGETPVTBL(me, ISignalBus)->OnEnable(me, piSignal);
}

static __inline int ISignalBus_Add(ISignalBus* me, ISignal* piSignal)
{
   return AEEGETPVTBL(me, ISignalBus)->Add(me, piSignal);
}

static __inline int ISignalBus_Remove(ISignalBus* me, ISignal* piSignal)
{
   return AEEGETPVTBL(me, ISignalBus)->Remove(me, piSignal);
}



/*
===============================================================================
   INTERFACE DOCUMENTATION
===============================================================================

ISignalBus Interface

Description: 

  ISignalBus allows client to Add multiple signals for dispatch on
      ISignalBus_Set().  It also tells its client when registration for 
      the events has occurred. An ISignalBus is enabled when any ISignal 
      on the bus is enabled.


  ISignalBus is designed to provide 3 important services:
      1. verification that an ISignal instance is compatible with an 
           implementation of ISignalBus.
      2. provide a mechanism for ISignal registration that obviates the 
           need to allocate memory to keep track of multiple registrations.
      3. provides a way to correlate multiple registrations of the 
           same ISignal, and allows ISignal semantics to be 
           single-registration.

============================================================================

ISignalBus_AddRef()

Description:
    This function is inherited from IQI_AddRef(). 

See Also:
    ISignalBus_Release()

============================================================================

ISignalBus_Release()

Description:
    This function is inherited from IQI_Release(). 

See Also:
    ISignalBus_AddRef()

============================================================================

ISignalBus_QueryInterface()

Description:
    This function is inherited from IQI_QueryInterface(). 

==============================================================================

ISignalBus_Strobe()

Description:

  Atomically set and clear the bus' state, signalling all enabled signals 
     on the bus.

Prototype:

   int ISignalBus_Strobe(ISignalBus* pif);

Parameters:
   pif: Pointer to an ISignalBus interface

Return Value:
   Error code.

Side Effects:
   Any signals in the bus are set, and will require ISignal_Enable()
     before being able to be set again.

   If the bus was already set, the bus will end up clear.

See Also:
   ISignal, ISignalBus_Add(), 

==============================================================================

ISignalBus_Set()

Description:

  Sets the state of the bus, and sets all enabled signals on the bus.

Prototype:

   int ISignalBus_Set(ISignalBus* pif);

Parameters:
   pif: Pointer to an ISignalBus interface

Return Value:
   Error code.

Side Effects:
   Any signals in the bus are set, and will require ISignal_Enable()
     before being able to be set again.

See Also:
   ISignal, ISignalBus_Add()

==============================================================================

ISignalBus_Clear()

Description:

  Clears the state of the bus.

Prototype:

   int ISignalBus_Clear(ISignalBus* pif);

Parameters:
   pif: Pointer to an ISignalBus interface

Return Value:
   Error code.

Side Effects:
   None.

See Also:
   ISignal, ISignalBus_Add()

==============================================================================

ISignalBus_OnEnable()

Description:

   Associates an ISignal to be set when the ISignalBus has been "enabled", 
     i.e. when an ISignal is added or when one of the ISignalBus' ISignals 
     have been re-enabled.

Prototype:

   int ISignalBus_OnEnable(ISignalBus* pif, ISignal* piSignal);

Parameters:
   pif: Pointer to an ISignalBus interface
   piSignal: ISignal to be Added with the bus

Comments:
   This function may only be called once, duplicate calls will return 
     AEE_EALREADY

Return Value:
   Error code.

See Also:
   ISignalBus_Set()

==============================================================================

ISignalBus_Add()

Description:

   Associates an instance of ISignal with the ISignalBus.

Prototype:

   int ISignalBus_Add(ISignalBus* pif, ISignal* piSignal);

Parameters:
   pif: Pointer to an ISignalBus interface
   piSignal: ISignal to be Added with the bus

Side Effects:
   If the signal has previously been enabled, the OnEnable signal 
     is set.

   If the signal is enabled and the state of the bus is set, the
      signal will be set immediately and disabled.

Comments:
   An instance of ISignal normally only supports registration with a 
     single instance of ISignalBus.

   This function will fail if the signal has been detached.

Return Value:
   Error code.

See Also:
   ISignal, ISignalBus_Set()

==============================================================================

ISignalBus_Remove()

Description:

   Dis-associates an instance of ISignal with the ISignalBus.

Prototype:

   int ISignalBus_Add(ISignalBus* pif, ISignal* piSignal);

Parameters:
   pif: Pointer to an ISignalBus interface
   piSignal: ISignal to be Added with the bus

Side Effects:
   None.

Return Value:
   Error code.

See Also:
   ISignal, ISignalBus_Set(), ISignalBus_Add()


=============================================================================
*/

#endif /* #ifndef AEEISIGNALBUS_H */
          

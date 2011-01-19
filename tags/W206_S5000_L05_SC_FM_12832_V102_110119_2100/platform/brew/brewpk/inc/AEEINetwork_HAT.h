#ifndef AEEINETWORK_HAT_H
#define AEEINETWORK_HAT_H

/*===================================================

FILE: AEEINetwork_HAT.h

SERVICES:  INetwork - GetHystActTimer and SetHystActTimer extensions

DESCRIPTION:
   Interface definitions, data structures, etc. 
   for INetwork GetHystActTimer and SetHystActTimer extensions

======================================================

      Copyright (c) 2008 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#include "AEEINetwork.h"

/*-------------------------------------------------------------------
      Defines and Type Declarations
-------------------------------------------------------------------*/

//
// for INetwork_GetHystActTimer
//
#define NETWORK_EXTFUNC_GET_HYST_ACT_TIMER      0x0107089c

//
// for INetwork_SetHystActTimer
//
#define NETWORK_EXTFUNC_SET_HYST_ACT_TIMER      0x0107089d

//-----------------------------------------------------------------
// Methods
//-----------------------------------------------------------------

static __inline int INetwork_GetHystActTimer(INetwork* me,
                                             int*      pnHystActTimer)
{
#if defined (AEEINTERFACE_CPLUSPLUS)
   return me->ExtFunc(NETWORK_EXTFUNC_GET_HYST_ACT_TIMER,
                      pnHystActTimer, sizeof(int));
#else /* #if defined(AEEINTERFACE_CPLUSPLUS) */
   return AEEGETPVTBL(me,INetwork)->ExtFunc(me,
                                            NETWORK_EXTFUNC_GET_HYST_ACT_TIMER,
                                            pnHystActTimer,
                                            sizeof(int));
#endif /* #if defined(AEEINTERFACE_CPLUSPLUS) */
}

static __inline int INetwork_SetHystActTimer(INetwork* me,
                                             int       nHystActTimer)
{
#if defined (AEEINTERFACE_CPLUSPLUS)
   return me->ExtFunc(NETWORK_EXTFUNC_SET_HYST_ACT_TIMER,
                      &nHystActTimer, sizeof(int));
#else /* #if defined(AEEINTERFACE_CPLUSPLUS) */
   return AEEGETPVTBL(me,INetwork)->ExtFunc(me,
                                            NETWORK_EXTFUNC_SET_HYST_ACT_TIMER,
                                            &nHystActTimer,
                                            sizeof(int));
#endif /* #if defined(AEEINTERFACE_CPLUSPLUS) */
}

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================

INETWORK_EXT_FUNC_GET_HYST_ACT_TIMER

Description:
   Unique ID which is used as an input parameter to ExtFunc() for extending the 
   INetwork interface to support the function INetwork_GetHystActTimer().

Definition:
   #define INETWORK_EXT_FUNC_GET_HYST_ACT_TIMER 0x0107089c

Version:
   Introduced BREW Client 3.1.5 SP02

See Also:
   INetwork_GetHystActTimer()

=============================================================================
INETWORK_EXT_FUNC_SET_HYST_ACT_TIMER

Description:
   Unique ID which is used as an input parameter to ExtFunc() for extending the 
   INetwork interface to support the function INetwork_SetHystActTimer().

Definition:
   #define INETWORK_EXT_FUNC_SET_HYST_ACT_TIMER 0x0107089d

Version:
   Introduced BREW Client 3.1.5 SP02

See Also:
   INetwork_SetHystActTimer()

=======================================================================
INTERFACES DOCUMENTATION
=======================================================================

INetwork_GetHystActTimer()

Description:
   This function returns the current status of the Hysteresis Activation Timer (HAT) 
   associated with this network. If the timer is active then the function will retrieve 
   the remaining value of the timer. 
   Applications can choose to introduce the hysteresis sooner by restarting the HAT 
   timer with a value smaller than the remaining value returned by the API.
   HAT is applicable to CDMA data networks (an IS707B feature). 
   It requires AEEPRIVID_NetHAT privilege.

Prototype:
   static int INetwork_GetHystActTimer(INetwork* me,
                                       int*      pnHystActTimer)

Parameters:
   me:              The interface pointer
   pnHystActTimer:  [out] - Pointer to integer in which the Hysteresis Activation 
                    Timer value is returned.

Return Value:
   AEE_NET_SUCCESS: Success.

   AEE_NET_EOPNOTSUPP: The operation is not supported.

   Other AEE designated error codes might be returned.

Comments:
   None  

Version:
   Introduced BREW Client 3.1.5 SP02

See Also:
   None.

=============================================================================

INetwork_SetHystActTimer()

Description:
   This function sets the Hysteresis Activation Timer (HAT) associated with this network.
   If the HAT is already active, then it will be stopped and started again, but with the 
   new value. 
   HAT is applicable to CDMA data networks (an IS707B feature). 
   It requires AEEPRIVID_NetHAT privilege.

Prototype:
   static int INetwork_SetHystActTimer(INetwork* me,
                                       int       nHystActTimer)

Parameters:
   me:             The interface pointer
   nHystActTimer:  The Hysteresis Activation Timer value.

Return Value:
   AEE_NET_SUCCESS: Success.

   AEE_NET_EOPNOTSUPP: The operation is not supported.

   Other AEE designated error codes might be returned.

Comments:
   None  

Version:
   Introduced BREW Client 3.1.5 SP02

See Also:
   None.

=====================================================================*/

#endif /* AEEINETWORK_HAT_H */

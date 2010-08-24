#ifndef AEESYSCLOCK_H
#define AEESYSCLOCK_H
/*===========================================================================
FILE:  AEESysClock.h

SERVICES: ISysClock Interface.

GENERAL DESCRIPTION:
   Base level definitions, typedefs, etc. for the System Clock Interface.

        Copyright © 2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                        INCLUDES
===========================================================================*/

#include "AEE.h"
#include "AEEQueryInterface.h"
#include "AEE64structs.h"

/*===========================================================================
                        ISYSCLOCK Interface
===========================================================================*/

#define AEEIID_SYSCLOCK 0x0102dcd5

#define INHERIT_ISysClock(iname) \
   INHERIT_IQueryInterface(iname); \
   int (*GetTimeUS)          (iname* me, uint64struct* pqwUS); \
   int (*SetTimeUS)          (iname* me, uint64struct qwUS); \
   int (*RegisterOnChangeCB) (iname* me, AEECallback *pcb)

AEEINTERFACE_DEFINE(ISysClock);

/*===========================================================================
                        PUBLIC FUNCTION DECLARATIONS
===========================================================================*/

static __inline uint32 ISYSCLOCK_AddRef(ISysClock *po) 
{
   return AEEGETPVTBL((po),ISysClock)->AddRef(po);
}

static __inline uint32 ISYSCLOCK_Release(ISysClock *po) 
{
   return AEEGETPVTBL((po),ISysClock)->Release(po);
}

static __inline int ISYSCLOCK_QueryInterface(ISysClock *po, AEECLSID iid, void **ppo)
{
   return AEEGETPVTBL((po),ISysClock)->QueryInterface(po, iid, ppo);
}

static __inline int ISYSCLOCK_GetTimeUS(ISysClock *po, uint64 *pqwUS) 
{
   if (0 == pqwUS) {
      return AEEGETPVTBL((po),ISysClock)->GetTimeUS(po, 0);
   } else {
      int nErr;
      uint64struct usUS;
      
      nErr = AEEGETPVTBL((po),ISysClock)->GetTimeUS(po, &usUS);
      *pqwUS = uint64struct_to_uint64(usUS);
      return nErr;
   }
}

static __inline int ISYSCLOCK_GetTimeS(ISysClock *po, uint32 *pdwS) 
{
   if (0 == pdwS) {
      return ISYSCLOCK_GetTimeUS(po, 0);
   } else {
      int    nErr;
      uint64 qwUS;
      
      nErr = ISYSCLOCK_GetTimeUS(po, &qwUS);
      *pdwS = (uint32)(qwUS / 1000000);
      return nErr;
   }
}

static __inline int ISYSCLOCK_SetTimeUS(ISysClock *po, uint64 qwUS)
{
   uint64struct usUS = uint64struct_from_uint64(qwUS);
   
   return AEEGETPVTBL((po),ISysClock)->SetTimeUS(po, usUS);
}

static __inline int ISYSCLOCK_SetTimeS(ISysClock *po, uint32 dwS)
{
   uint64 qwUS = (uint64)dwS * 1000000;
   
   return ISYSCLOCK_SetTimeUS(po, qwUS);
}

static __inline int ISYSCLOCK_RegisterOnChangeCB(ISysClock *po, AEECallback *pcb)
{
   return AEEGETPVTBL((po),ISysClock)->RegisterOnChangeCB(po, pcb);
}

/*========================================================================
  INTERFACES DOCUMENTATION
=======================================================================
ISysClock interface

   The ISysClock interface provides access APIs for getting and setting 
   system clocks, querying time validity, and registering to be
   notified of changes in time.  Times provided by this interface are
   given as offsets in both second and microsecond resolution from a
   defined time epoch.  The epoch used is dependent on the particular
   class implementation.  Actual time resolution may vary depending
   on the implementing class.

   Certain classes may support setting the current time from a secure and
   trusted time source, and if you use them, the correct time should be
   set for you automatically. Calling ISYSCLOCK_GetTimeUS() will indicate
   whether the time has been set to a valid time or not.

=======================================================================

ISYSCLOCK_AddRef()

Description:
    This function is inherited from IQI_AddRef(). 

See Also:
    ISYSCLOCK_Release()

=======================================================================

ISYSCLOCK_Release()

Description:
    This function is inherited from IQI_Release(). 

See Also:
    ISYSCLOCK_AddRef()

=======================================================================

ISYSCLOCK_QueryInterface()

Description:
    This function is inherited from IQI_QueryInterface(). 

=======================================================================

ISYSCLOCK_GetTimeUS()

Description:
   Obtains the number of microseconds elapsed since epoch and the  
   validity of the clock. If the input uint64 parameter is NULL, the
   function only returns the validity of time and not the current value. 
   The definition of validity is implementation specific.
  
Prototype:
   int ISYSCLOCK_GetTimeUS(ISysClock *po, uint64 *pqwUS) 
      
Parameters:
   po    : [IN]  Pointer to ISysClock object
   pqwUS : [OUT] Pointer to the number in microseconds elapsed since epoch

Return Value:
   AEE_SUCCESS:  if the operation was successful and current time is valid
   AEE_BADSTATE: if the operation was successful but current time is invalid
   Another appropriate error code if operation was not successful

Comments:
   Epoch is class dependent.

Side Effects:
   None
   
Version:
   Introduced BREW Client 3.1.5

See Also:
   ISYSCLOCK_GetTimeS()
   
=======================================================================

ISYSCLOCK_GetTimeS()

Description:
   Obtains the number of seconds elapsed since epoch and the validity
   of the clock. If the input uint64 parameter is NULL, the function
   only returns the validity of time and not the current value. The
   definition of validity is implementation specific.
  
Prototype:
   int ISYSCLOCK_GetTimeS(ISysClock *po, uint32 *pdwS) 
      
Parameters:
   po   : [IN]  Pointer to ISysClock object
   pdwS : [OUT] Pointer to the number in seconds elapsed since epoch

Return Value:
   AEE_SUCCESS:  if the operation was successful and current time is valid
   AEE_BADSTATE: if the operation was successful but current time is invalid
   Another appropriate error code if operation was not successful

Comments:
   Epoch is class dependent.

Side Effects:
   None
   
Version:
   Introduced BREW Client 3.1.5

See Also:
   ISYSCLOCK_GetTimeUS()

=======================================================================

ISYSCLOCK_SetTimeUS()

Description:
   Sets the number of microseconds elapsed since epoch.

Prototype:
  int ISYSCLOCK_SetTimeUS(ISysClock *po, uint64 qwUS)

Parameters:
   po   : [IN] Pointer to ISysClock object
   qwUS : [IN] Number of microseconds elapsed since epoch to set time to.

Return Value:
   Returns AEE_SUCCESS if the operation was successful or an error
   code specific to the failure.

Comments:
   Epoch is class dependent.
   Not all implementations support setting of the system clock.
   
Side Effects:
   None
  
Version:
   Introduced BREW Client 3.1.5

See Also:
   ISYSCLOCK_SetTimeS()
   
=======================================================================

ISYSCLOCK_SetTimeS()

Description:
   Sets the number of seconds elapsed since epoch.

Prototype:
  int ISYSCLOCK_SetTimeS(ISysClock *po, uint32 dwS)

Parameters:
   po  : [IN] Pointer to ISysClock object
   dwS : [IN] Number of seconds elapsed since epoch to set time to.

Return Value:
   Returns AEE_SUCCESS if the operation was successful or an error
   code specific to the failure.

Comments:
   Epoch is class dependent.
   Not all implementations support setting of the system clock.
   
Side Effects:
   None
  
Version:
   Introduced BREW Client 3.1.5

See Also:
   ISYSCLOCK_SetTimeUS()

=======================================================================

ISYSCLOCK_RegisterOnChangeCB()

Description:
   Registers a callback to be called when either the validity of time
   has changed or the value of time has changed.  The firing of this 
   callback simply indicates that time has changed either in validity
   or value.  It is then the callback function's responsibility to
   determine what action to take.  Possible actions are calling one of
   the ISYSCLOCK_GetTimeXX() calls to query time validity and acquire 
   the new time.
  
Prototype:
   int ISYSCLOCK_OnChangeCB(ISysClock *po, AEECallback *pcb)
      
Parameters:
   po  : [IN] Pointer to ISysClock object
   pcb : [IN] Pointer to the AEECallback to call when time has
              changed.
   
Return Value:
   Returns AEE_SUCCESS if the operation was successful or an error
   code specific to the failure.

Comments:
   None

Side Effects:
   None
   
Version:
   Introduced BREW Client 3.1.5

See Also:
   ISYSCLOCK_GetTimeUS()
   ISYSCLOCK_GetTimeS()
   
=============================================================================*/

#endif  // AEESYSCLOCK_H


/*===========================================================================
                    IMPORTANT NOTICE

                  OEM DISTRIBUTION ONLY

FILE:  AEEMutex.h

SERVICES:  Mutual Exclusion Mechanism

GENERAL DESCRIPTION:
	Shim layer between AEE and OEMMutex

        Copyright © 2003-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

#ifndef AEEMUTEX_H
#define AEEMUTEX_H

#include "OEMMutex.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef OEMMutex AEEMutex;

/**************************************************************************/
/*                     AEE MUTEX EXPORTED METHODS                         */
/**************************************************************************/

extern AEEMutex AEEMutex_Create(void);
extern void AEEMutex_Lock(AEEMutex m);
extern void AEEMutex_Unlock(AEEMutex m);
extern void AEEMutex_Destroy(AEEMutex m);

#if defined(__cplusplus)
}
#endif


/*=====================================================================
  INTERFACES DOCUMENTATION
=======================================================================
AEEMutex Interface
=======================================================================
Function: AEEMutex_Create()

Description:
   Dynamically allocates and initializes a mutex.

Prototype:
   AEEMutex AEEMutex_Create(void);

Parameters:
   None

Return Value:
   A valid (non-zero) AEEMutex in the case of success, zero otherwise

Comments:
   For example, the following code fragment illustrates the proper way to
   create a mutex, assuming a reference counted object:
===pre>
typedef struct {
   AEEMutex mutex;
   int ref_cnt;
} foo;

...

   foo* pf = sys_malloc(sizeof(foo));
   if ((foo*)0 != pf) {
      pf->mutex = AEEMutex_Create();
      if ((AEEMutex)0 != pf->mutex) {
         pf->ref_cnt = 1;
      }
   }
===/pre>
Side Effects:
   None

See Also:
   AEEMutex_Destroy()

========================================================================

Function: AEEMutex_Lock()

Description:
   Acquires a mutex.  Sleeps if another task already holds the mutex.

Prototype:
   void AEEMutex_Lock(AEEMutex m);

Parameters:
   A valid (non-zero) AEEMutex, which must have been returned by 
   a previous call to AEEMutex_Create().

Return Value:
   None.

Comments:
   This is for inter-task protection only.  Interrupt handlers, and so forth, will cause
   an abort if they call AEEMutex_Lock().

Side Effects:
   Any other task will be put to sleep if it tries to acquire the same mutex
   before it is released by this task.
   If a higher priority task attempts to acquire the mutex, this task's
   priority will be raised to the same higher priority, such that this task can
   release the mutex in a timely fashion.

See Also:
   AEEMutex_Unlock()

========================================================================

Function: AEEMutex_Unlock()

Description:
   Releases a mutex. 

Prototype:
   void AEEMutex_Unlock(AEEMutex m);

Parameters:
   A valid (non-zero) AEEMutex, which must have been returned by 
   a previous call to AEEMutex_Create().
   Must be the same AEEMutex as was passed to AEEMutex_Lock().

Return Value:
   None

Comments:
   This is for inter-task protection only.  Interrupt handlers, and so forth, will cause
   an abort if they call AEEMutex_Unlock().

Side Effects:
   Any other sleeping task that is waiting to acquire this mutex will be woken up.

See Also:
   AEEMutex_Lock()

========================================================================

Function: AEEMutex_Destroy()

Description:
   Uninitializes and dynamically deallocates a mutex.

Prototype:
   void AEEMutex_Destroy(AEEMutex m);

Parameters:
   A valid (non-zero) AEEMutex, which must have been returned by 
   a previous call to AEEMutex_Create().

Return Value:
   None

Comments:
   The mutex must be the unlocked before the call to AEEMutex_Destroy().

   For example, the following code fragment illustrates the proper way to
   destroy a mutex, assuming a reference counted object:
===pre>
   AEEMutex_Lock(pf->mutex);
   if (0 == --pf->ref_cnt) {
      AEEMutex_Unlock(pf->mutex);
      AEEMutex_Destroy(pf->mutex);
      sys_free(pf);
   } else {
      AEEMutex_Unlock(pf->mutex);
   }
===/pre>
Side Effects:
   None

See Also:
   AEEMutex_Create()

=========================================================================*/

#endif /* AEEMUTEX_H */


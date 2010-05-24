/*===========================================================================
                    IMPORTANT NOTICE

                  OEM DISTRIBUTION ONLY

FILE:  OEMMutex.h

SERVICES:  OEM Mutual Exclusion Mechanism

GENERAL DESCRIPTION:
	Definitions, typedefs, etc. for OEM Mutex usage

        Copyright © 2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

#ifndef OEMMUTEX_H
#define OEMMUTEX_H

#include "AEE_OEMComdef.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef OEMINSTANCE OEMMutex;

/**************************************************************************/
/*                     OEM MUTEX EXPORTED METHODS                         */
/**************************************************************************/

extern OEMMutex OEMMutex_Create(void);
extern void OEMMutex_Lock(OEMMutex m);
extern void OEMMutex_Unlock(OEMMutex m);
extern void OEMMutex_Destroy(OEMMutex m);

#if defined(__cplusplus)
}
#endif


/*=====================================================================
  INTERFACES DOCUMENTATION
=======================================================================
OEMMutex Interface
=======================================================================

Function: OEMMutex_Create()

Description:
   Dynamically allocates and initializes a mutex.

Prototype:
   OEMMutex OEMMutex_Create(void);

Parameters:
   None.

Return Value:
   An valid (non-zero) OEMMutex in the case of success, zero otherwise

Comments:
   For example, the following code fragment illustrates the proper way to
   create a mutex, assuming a reference counted object:
===pre>
typedef struct {
   OEMMutex mutex;
   int ref_cnt;
} foo;

...

   foo* pf = sys_malloc(sizeof(foo));
   if ((foo*)0 != pf) {
      pf->mutex = OEMMutex_Create();
      if ((OEMMutex)0 != pf->mutex) {
         pf->ref_cnt = 1;
      }
   }
===/pre>
Side Effects:
   None.

See Also:
   OEMMutex_Destroy

========================================================================

Function: OEMMutex_Lock()

Description:
   Acquires a mutex.  Sleeps if another task already holds the mutex.

Prototype:
   void OEMMutex_Lock(OEMMutex m);

Parameters:
   A valid (non-zero) OEMMutex, which must have been returned by 
   a previous call to OEMMutex_Create().

Return Value:
   None.

Comments:
   This is for inter-task protection only.  Interrupt handlers, etc. will cause
   an abort if they call OEMMutex_Lock().

Side Effects:
   Any other task will be put to sleep if it tries to acquire the same mutex
   before it is released by this task.
   If a higher priority task attempts to acquire the mutex, this task's
   priority will be raised to the same higher priority, such that this task can
   release the mutex in a timely fashion.

See Also:
   OEMMutex_Unlock()

========================================================================

Function: OEMMutex_Unlock()

Description:
   Releases a mutex. 

Prototype:
   void OEMMutex_Unlock(OEMMutex m);

Parameters:
   A valid (non-zero) OEMMutex, which must have been returned by 
   a previous call to OEMMutex_Create().
   Must be the same OEMMutex as was passed to OEMMutex_Lock().

Return Value:
   None.

Comments:
   This is for inter-task protection only.  Interrupt handlers, etc. will cause
   an abort if they call OEMMutex_Unlock().

Side Effects:
   Any other sleeping task that is waiting to acquire this mutex will be woken up.

See Also:
   OEMMutex_Lock()

========================================================================

Function: OEMMutex_Destroy()

Description:
   Uninitializes and dynamically deallocates a mutex.

Prototype:
   void OEMMutex_Destroy(OEMMutex m);

Parameters:
   A valid (non-zero) OEMMutex, which must have been returned by 
   a previous call to OEMMutex_Create().

Return Value:
   None.

Comments:
   The mutex must be the unlocked before the call to OEMMutex_Destroy().

   For example, the following code fragment illustrates the proper way to
   destroy a mutex, assuming a reference counted object:
===pre>
   OEMMutex_Lock(pf->mutex);
   if (0 == --pf->ref_cnt) {
      OEMMutex_Unlock(pf->mutex);
      OEMMutex_Destroy(pf->mutex);
      sys_free(pf);
   } else {
      OEMMutex_Unlock(pf->mutex);
   }
===/pre>
Side Effects:
   None.

See Also:
   OEMMutex_Create()

=========================================================================*/

#endif /* OEMMUTEX_H */

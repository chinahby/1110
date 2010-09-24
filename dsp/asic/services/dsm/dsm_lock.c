/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                                  D S M _ L O C K . C

GENERAL DESCRIPTION
  DMSS Data Services generalized locking functions.

EXTERNALIZED FUNCTIONS

  dsm_lock()
    Acquire exclusive access to critical resources.

  dsm_unlock()
    Release exclusive access to critical resoruces.

  dsm_lock_create()
    Create a locking mechanism.

  dsm_lock_destroy()
    Release a lock resource.
    
    
INITIALIZATION AND SEQUENCING REQUIREMENTS

  All WINCE locks MUST be initialized before using.
  ALL WINCE locks _should_ be destroyed after they are no longer needed.

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //depot/asic/msmshared/services/dsm/dsm_lock.c#2 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/24/07    mjb    Generalized. Specific macros in dsm_pool.h & dsm_queue.c
12/27/06    rsb    Created file.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/* Target-independent Include files */
#include "comdef.h"
#include "customer.h"
#include "dsm_lock.h"
#include "err.h"
#include "assert.h"
#ifdef FEATURE_QUBE
#include "qerror.h"
#endif


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           EXTERNALIZED FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*===========================================================================
FUNCTION DSM_LOCK

DESCRIPTION
   This function acquires exclusive access to a critical resource.

DEPENDENCIES
 If FEATURE_WINCE
   The parameter must NOT be NULL in the case of FEATURE_WINCE.
   The lock must have been initialized with dsm_create_lock.

 In all cases:  
   Must not be called from interrupt context.

   lock_ptr  - Pointer to lock cookie

RETURN VALUE
   None

SIDE EFFECTS
   The current thread may be blocked until the lock is available.
   Depending on the OS, the lock may get a value by reference.
===========================================================================*/
void dsm_lock
(
   dsm_lock_type * lock_ptr
)
{

#ifdef FEATURE_WINCE
#error code not present
#elif defined FEATURE_QUBE
  ASSERT( NULL != lock_ptr );
  qmutex_lock( (qmutex_t)*lock_ptr);
#endif

   return;
}

/*===========================================================================
FUNCTION DSM_UNLOCK

DESCRIPTION
   This function acquires exclusive access to a critical resource.

DEPENDENCIES
 If FEATURE_WINCE
   The parameter must NOT be NULL in the case of FEATURE_WINCE
   The queue is previously initialized using dsm_queue_init.

 In all cases:  
   Must not be called from interrupt context.

PARAMETERS
   lock_ptr - Pointer lock cookie

RETURN VALUE
   None

SIDE EFFECTS
   The current thread will become unlocked

===========================================================================*/
void dsm_unlock
(
   dsm_lock_type * lock_ptr
)
{
#ifdef FEATURE_WINCE
#error code not present
#elif defined FEATURE_QUBE
  ASSERT(NULL != lock_ptr);
  qmutex_unlock( (qmutex_t)*lock_ptr);
#endif

  return;
}


/*===========================================================================
FUNCTION DSM_LOCK_CREATE

DESCRIPTION
   This function initializes a locking mechanism.

DEPENDENCIES
 If FEATURE_WINCE
   The parameter must NOT be NULL in the case of FEATURE_WINCE

PARAMETERS
   lock_ptr - Pointer to lock type.

RETURN VALUE
   None

SIDE EFFECTS
   A lock resource might be allocated in the OS, depending on the OS.
   The lock cookie will be put in the lock_ptr by reference.

===========================================================================*/
void dsm_lock_create
(
   dsm_lock_type * lock_ptr
)
{
#ifdef FEATURE_WINCE
#error code not present
#elif defined FEATURE_QUBE
  ASSERT( lock_ptr != NULL );
  if ( qmutex_create ( lock_ptr, QMUTEX_LOCAL ) != EOK)
  {
    ERR_FATAL("dsm_lock_create: Failed creating qmutex.",0,0,0);
  }
#endif

  
   return;
}

/*===========================================================================
FUNCTION DSM_LOCK_DESTROY

DESCRIPTION
   This function tears down a locking mechanism.

DEPENDENCIES
 If FEATURE_WINCE
   The parameter must NOT be NULL in the case of FEATURE_WINCE
   The lock should be in existence.
   
PARAMETERS
   lock_ptr - Pointer to lock type.

RETURN VALUE
   None

SIDE EFFECTS
   The locking mechanism will cease to exist and OS resources might be
   freed. 

===========================================================================*/
void dsm_lock_destroy
(
   dsm_lock_type * lock_ptr
)
{

#ifdef FEATURE_WINCE
#error code not present
#elif defined FEATURE_QUBE
  ASSERT( lock_ptr != NULL );
  qmutex_delete(*lock_ptr);
  *lock_ptr = (qmutex_t)(0);
#endif

   return;
}


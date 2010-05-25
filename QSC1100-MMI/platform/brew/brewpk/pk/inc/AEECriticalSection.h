/*===========================================================================
                    IMPORTANT NOTICE

                  OEM DISTRIBUTION ONLY

FILE:  AEECriticalSection.h

SERVICES:  AEE Critical Section Mechanism

GENERAL DESCRIPTION:
	Definitions, typedefs, etc. for AEE Critical Section usage

        Copyright © 2003-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

#ifndef AEECRITICALSECTION_H
#define AEECRITICALSECTION_H

#include "OEMCriticalSection.h"

#if defined(__cplusplus)
extern "C" {
#endif

// the actual contents are opaque to the consumer
typedef OEMCriticalSection AEECriticalSection; // must be zero initialized!

/**************************************************************************/
/*               AEE CRITICAL SECTION EXPORTED METHODS                    */
/**************************************************************************/

extern void AEECriticalSection_Enter(AEECriticalSection* pm);
extern void AEECriticalSection_Leave(AEECriticalSection* pm);

#if defined(__cplusplus)
}
#endif


/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================

AEECriticalSection

Description:
   Declares storage space for the critical section.  The actual contents
   are opaque to the user.  Must be zero initialized before the first use.

Definition:
typedef byte AEECriticalSection[32];

Members:
None

===pre>
Example:
static AEECriticalSection cs;

===/pre>
See Also:
AEECriticalSection_Enter(), AEECriticalSection_Leave()

=======================================================================
  INTERFACES DOCUMENTATION
=======================================================================
AEECriticalSection Interface
=======================================================================

Function: AEECriticalSection_Enter()

Description:
   Enters a critical section.  Sleeps if another task is already in the
   critical section.

Prototype:
   void AEECriticalSection_Enter(AEECriticalSection* pm);

Parameters:
   Opaque pointer to an AEECriticalSection, which must be zero-initialized before the
   first call to AEECriticalSection_Enter().

Return Value:
   None

Comments:
   This is for inter-task protection only.  Interrupt handlers, and so forth, will cause
   an abort if they call AEECriticalSection_Enter().

Side Effects:
   Any other task will be put to sleep if it tries to enter the same critical
   section before it is left by this task.  
   If a higher priority task attempts to enter the critical section, this task's
   priority will be raised to the same higher priority, such that this task can
   leave the critical section in a timely fashion.

See Also:
   AEECriticalSection_Leave()

========================================================================

Function: AEECriticalSection_Leave()

Description:
   Leaves a critical section.

Prototype:
   void AEECriticalSection_Leave(AEECriticalSection* pm);

Parameters:
   Opaque pointer to AEECriticalSection.  Must be the same pointer as was passed to
   AEECriticalSection_Enter().

Return Value:
   None

Comments:
   This is for inter-task protection only.  Interrupt handlers, and so forth, will cause
   an abort if they call AEECriticalSection_Leave().

Side Effects:
   Any other sleeping task that is waiting to enter this critical section will be woken up.

See Also:
   AEECriticalSection_Enter()

=========================================================================*/

#endif /* AEECRITICALSECTION_H */

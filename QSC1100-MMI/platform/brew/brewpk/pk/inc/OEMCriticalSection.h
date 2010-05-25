/*===========================================================================
                    IMPORTANT NOTICE

                  OEM DISTRIBUTION ONLY

FILE:  OEMCriticalSection.h

SERVICES:  OEM Critical Section Mechanism

GENERAL DESCRIPTION:
	Definitions, typedefs, etc. for OEM Critical Section usage

        Copyright © 2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

#ifndef OEMCRITICALSECTION_H
#define OEMCRITICALSECTION_H

#include "AEEComdef.h"

#if defined(__cplusplus)
extern "C" {
#endif

// the actual contents are opaque to the consumer
typedef uint32 OEMCriticalSection[8];      // must be zero initialized!

/**************************************************************************/
/*               OEM CRITICAL SECTION EXPORTED METHODS                    */
/**************************************************************************/

extern AEE_EXPORTS void OEMCriticalSection_Enter(OEMCriticalSection* pm);
extern AEE_EXPORTS void OEMCriticalSection_Leave(OEMCriticalSection* pm);

#if defined(__cplusplus)
}
#endif


/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================

OEMCriticalSection

Description:
   Declares storage space for the critical section.  The actual contents
   are opaque to the user.  Must be zero initialized before first use.

Definition:
typedef byte OEMCriticalSection[32];

Members:
None

See Also:
OEMCriticalSection_Enter(), OEMCriticalSection_Leave()

=======================================================================
  INTERFACES DOCUMENTATION
=======================================================================
OEMCriticalSection Interface
=======================================================================
Function: OEMCriticalSection_Enter()

Description:
   Enters a critical section.  Sleeps if another task is already in the
   critical section.

Prototype:
   void OEMCriticalSection_Enter(OEMCriticalSection* pm);

Parameters:
   Opaque pointer to an OEMCriticalSection, which must be zero-initialized before the
   first call to OEMCriticalSection_Enter().

Return Value:
   None

Comments:
   This is for inter-task protection only.  Interrupt handlers, and so forth, will cause
   an abort if they call OEMCriticalSection_Enter().

Side Effects:
   Any other task will be put to sleep if it tries to enter the same critical
   section before it is left by this task.  
   If a higher priority task attempts to enter the critical section, this task's
   priority will be raised to the same higher priority, such that this task can
   leave the critical section in a timely fashion.

See Also:
   OEMCriticalSection_Leave()

========================================================================

Function: OEMCriticalSection_Leave()

Description:
   Leaves a critical section.

Prototype:
   void OEMCriticalSection_Leave(OEMCriticalSection* pm);

Parameters:
   OEMCriticalSection of interest.  Must be the same pointer as was passed to
   OEMCriticalSection_Enter().

Return Value:
   None

Comments:
   This is for inter-task protection only.  Interrupt handlers, and so forth, will cause
   an abort if they call OEMCriticalSection_Leave().

Side Effects:
   Any other sleeping task that is waiting to enter this critical section
   will be woken up.

See Also:
   OEMCriticalSection_Enter()

=========================================================================*/

#endif /* OEMCRITICALSECTION_H */

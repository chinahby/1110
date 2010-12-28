#ifndef TIME_MFLO_H
#define TIME_MFLO_H

/*=============================================================================

                  T I M E   S E R V I C E   S U B S Y S T E M

                   M E D I A F L O  T I M E   K E E P I N G


GENERAL DESCRIPTION

  Implements MediaFLO time-keeping functions using the MediaFLO time data


EXTERNALIZED FUNCTIONS

  time_mflo_set_at
    Sets the MediaFLO system time or invalidates it.

  time_mflo_get
    Returns system time based on the last MediaFLO time update.

  time_mflo_invalidate
    Invalidates the MediaFLO time.


INITIALIZATION AND SEQUENCING REQUIREMENTS

  MediaFLO System Time has a resolution of 1 second and is retrieved from 
  Overhead Information Symbols (OIS) portion of the MediaFLO Superframe. 
  MediaFLO Superframes are transmitted on a fixed interval of 1 second 
  synchronized to GPS time epoch.

  MediaFLO system time is invalidated if/when MediaFLO service is deemed
  unavailable.


Copyright (c) 2005 by QUALCOMM Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/services/time/time_mflo.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/18/05   mod/ajn Added MediaFLO time support to mainline
11/08/05   mod     Added time_mflo_invalidate()
11/03/05   mod     Code review comments
10/28/05   mod     Initial implementation.

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "time_svc.h"
#include "timetick.h"

#ifdef FEATURE_MFLO
#error code not present
#endif /* FEATURE_MFLO */

#endif /* TIME_MFLO_H */


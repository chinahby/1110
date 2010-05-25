#ifndef TIME_REMOTE_H
#define TIME_REMOTE_H

/*===========================================================================

                        T I M E R   S E R V I C E S
                        
              M O D E M   T O   A P P ' S   F U N C T I O N S

DESCRIPTION
  This header file contains definitions of:
  App's processor functions called remotely from modem processor (m to a).

REFERENCES
  None

Copyright (c) 2005 - 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
   
$Header: //depot/asic/msmshared/services/time/time_remote.h#5 $ $DateTime: 2006/05/16 15:39:47 $ $Author: twells $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/11/06   taw     Added function, time_remote_restore, to be used to resync
                   timers after apps power collapse.
04/25/06   taw     Added function, time_remote_slow_clock_sync_poll, to be 
                   called when the modem is in RESET and the timers need
                   to be synced.
03/09/06   jhs     Added time_remote_init_time_of_day to sync time of 
                   day before acquiring.
09/26/05   jhs     Added time_remote_init.
08/11/05   jhs     Updates.
02/07/05   jhs     Initial revision.

===========================================================================*/



/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "time_svc.h"
#include "timetick.h"



/*===========================================================================

                    DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items required by this module.

===========================================================================*/

/* If FEATURE_SLOW_CLOCK_MULTIPROC_SYNC is defined them so must
 * FEATURE_SLOW_CLOCK_MULTIPROC_SYNC_SMSM.
 */
#ifdef FEATURE_SLOW_CLOCK_MULTIPROC_SYNC
  #ifndef FEATURE_SLOW_CLOCK_MULTIPROC_SYNC_SMSM
    #error FEATURE DEFINE MISMATCH
  #endif /* FEATURE_SLOW_CLOCK_MULTIPROC_SYNC */
#endif /* FEATURE_SLOW_CLOCK_MULTIPROC_SYNC_SMSM */



/*===========================================================================

                             FUNCTION DEFINITIONS

===========================================================================*/


/*=============================================================================

FUNCTION TIME_REMOTE_INIT 
 
DESCRIPTION

  MODEM ONLY

  This is used to initialize and setup the modem copy of the time of day
  bases for the apps which is used in calculating whether or not to update
  the apps time of day bases when the modem time of day bases are updated.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_remote_init(void);



/*=============================================================================

FUNCTION TIME_REMOTE_SLOW_CLOCK_SYNC 
 
DESCRIPTION

  APPS VERSION

  Apps processor SMSM based code which synchronizes the time base for the
  slow clock timer.

  This is a blocking algorithm that uses SMSM to signal the apps state to
  the modem, but polls shared memory to get the modem state and time base
  data. It polls shared memory to minimize latencies.

  MODEM VERSION

  Modem processor SMSM based code which synchronizes the time base for the
  slow clock timer.

  Sets up the smsm event callback - which is where all the work occurs.

DEPENDENCIES
  SMSM must be initialized

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_remote_slow_clock_sync(void);



/*===========================================================================

FUNCTION TIME_REMOTE_SLOW_CLOCK_SYNC_POLL

DESCRIPTION
  This function is used to resync the timers on the apps side and the modem
  side.  This function is called after exiting from power collapse when the 
  modem is in the RESET state, and so polls for the timer state changes, and 
  then calls the time_remote callback when necessary.

PARAMETERS
  None

RETURN VALUE
  TRUE if timers were synced successfully; FALSE otherwise.

DEPENDENCIES
  None

SIDE EFFECTS
  Timers are synced with the apps processor. 

===========================================================================*/
boolean time_remote_slow_clock_sync_poll(void);



/*=============================================================================

FUNCTION TIME_REMOTE_RESTORE
 
DESCRIPTION
  This function is used to resync the timers with the modem processor after
  apps power collapse.

DEPENDENCIES
  SMSM must be initialized

RETURN VALUE
  TRUE if timers were restored successfully
  FALSE if timers were not restored successfully

SIDE EFFECTS
  None

=============================================================================*/

boolean time_remote_restore(void);



/*=============================================================================

FUNCTION TIME_REMOTE_UPDATE_APPS_TOD

DESCRIPTION
  Setup a the RPC proxy to call a call back in the future to make the
  RPC call to inform the Apps processor of the change in TOD bases. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_remote_update_apps_tod
(
  /* Timestamp used as base for slow-clock time-of-day */
  time_type                       base_timestamp,

  /* Time tick corresponding to above timestamp */
  timetick_type                   base_timetick
);



/*=============================================================================

FUNCTION TIME_REMOTE_INIT_TIME_OF_DAY
 
DESCRIPTION
  Modem processor SMSM based code which synchronizes the time of day base.

  Sets up the smsm time of day callback - which is where all the work occurs.

DEPENDENCIES
  SMSM must be initialized

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_remote_init_time_of_day
(
  /* Base time tick */
  timetick_type tick,

  /* Base time stamp */
  time_type stamp
);



/*============================================================================

FUNCTION TIME_TOD_SET_APPS_BASES
 
DESCRIPTION
  Call to inform Apps of updated timestamp and timetick bases.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  When called from Modem will cause RPC call to invoke this call on the Apps.

============================================================================*/

void time_tod_set_apps_bases( timetick_type tick, time_type stamp );
/*~ FUNCTION time_tod_set_apps_bases */

#endif /* TIME_REMOTE_H */


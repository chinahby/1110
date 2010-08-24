/*=============================================================================


                   T I M E   S E R V I C E   S U B S Y S T E M

                        G P S   T I M E   K E E P I N G

GENERAL DESCRIPTION
  Implements GPS time-keeping functions using the GPS data


EXTERNALIZED FUNCTIONS
  time_gps_sync80_event
    Set the system time at sync80 at the given timetick.

  time_gps_set_at
    Set the system time at any arbitrary point in time at the given timetick.

  time_gps_get
    Return system time based on the latest GPS time

  time_gps_wake
    Called to start the GPS timekeeping module.

  time_gps_sleep
    Called when GPS timekeeping is turned off.
  time_gps_get_base
    Read base time stamp based on GP
    
  time_gps_is_sync80_sync
    Query whether the current GPS base time is sync80 synchronized

INITIALIZATION AND SEQUENCING REQUIREMENTS
  To start, time_gps_wake must be called by GPS module to start the time
  keeping module.  Then the sync80 ISR will start incrementing time by
  calling time_gps_sync80_event.  From then, the GPS time becomes valid.
  In a power up case where no system time is available, GPS PE may set
  the time using time_gps_set_at once it acquires time through GPS demodulation.
  When the GPS tracking session ends, then time_gps_sleep will be called.

Copyright(c) 2005 by QUALCOMM, Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/services/time/time_gps.c#3 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
06/11/07   aar     Added time_gps_sync80_event(): self-encapsulated GPS sync80
                   based time.
09/20/05   ajn/cr  Added GPS based time.
07/18/05   cr      Original check in

=============================================================================*/


/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "time_gps.h"
#include "sclk.h"
#include "msg.h"

#ifdef FEATURE_GPSONE_PE
#error code not present
#endif /* FEATURE_GPSONE_PE */


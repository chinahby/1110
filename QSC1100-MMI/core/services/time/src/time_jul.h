#ifndef TIME_JUL_H
#define TIME_JUL_H
/*=============================================================================

                  T I M E   S E R V I C E   S U B S Y S T E M

                  J U L I A N   T I M E   C O N V E R S I O N


GENERAL DESCRIPTION
  This module contains procedures to convert seconds to and from Julian
  calendar units.

  IMPORTANT NOTE: The time calculations in this module are only valid for
  dates and times from 6 Jan 1988, 00:00:00 to 28 Feb 2100, 23:59:59.  This
  is due to the fact that this module assumes that all years evenly divisible
  by 4 are leap years. Unfortunately, century years (e.g., 1900, 2000, 2100)
  must also be evenly divisible by 400 in order to be leap years, and so the
  year 2100 does not qualify as a leap year.


EXTERNALIZED FUNCTIONS
  time_jul_from_secs
    This function converts a specified number of elapsed seconds since
    the base date to its equivalent Julian date and time.

  time_jul_to_secs
    This function converts a specified Julian date and time to an
    equivalent number of elapsed seconds since the base date.


INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.


Copyright (c) 1994, 1996 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1999, 2003 by QUALCOMM, Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/services/time/time_jul.h#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
08/08/03   ajn     Coding standard update.
07/24/03   ajn     Xfer'd clkjul.c from MSM#### CLOCK VU's to general TIME VU

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "comdef.h"



/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/


/*-----------------------------------------------------------------------------
  Days of the Week constants
-----------------------------------------------------------------------------*/

typedef enum
{
  MONDAY    = 0,
  TUESDAY   = 1,
  WEDNESDAY = 2,
  THURSDAY  = 3,
  FRIDAY    = 4,
  SATURDAY  = 5,
  SUNDAY    = 6
}
time_weekday_type;



/*-----------------------------------------------------------------------------
  Julian time structure
-----------------------------------------------------------------------------*/

typedef struct
{
  /* Year [1980..2100] */
  uint16                          year;

  /* Month of year [1..12] */
  uint16                          month;

  /* Day of month [1..31] */
  uint16                          day;

  /* Hour of day [0..23] */
  uint16                          hour;

  /* Minute of hour [0..59] */
  uint16                          minute;

  /* Second of minute [0..59] */
  uint16                          second;

  /* Day of the week [0..6] [Monday .. Sunday] */
  uint16                          day_of_week;
}
time_julian_type;



/*=============================================================================

FUNCTION TIME_JUL_FROM_SECS

DESCRIPTION
  This procedure converts a specified number of elapsed seconds   
  since the base date to its equivalent Julian date and time.     

DEPENDENCIES
  None

RETURN VALUE
  The specified Julian date record is filled in with equivalent date/time,
  and returned into the area pointed to by julian_ptr.

SIDE EFFECTS
  None

=============================================================================*/

void time_jul_from_secs
(
  /* Number of seconds since base date */
  uint32                          secs,

  /* OUT: Pointer to Julian date record */
  time_julian_type                *julian
);



/*=============================================================================

FUNCTION TIME_JUL_TO_SECS

DESCRIPTION
  This procedure converts a specified Julian date and time to an  
  equivalent number of elapsed seconds since the base date.    

DEPENDENCIES
  None

RETURN VALUE
  Number of elapsed seconds since base date.       

SIDE EFFECTS
  None

=============================================================================*/

uint32 time_jul_to_secs
(
  /* Pointer to Julian date record */
  const time_julian_type          *julian
);


#endif /* TIME_JUL_H */

/* =======================================================================
                               oscl_clock.cpp
DESCRIPTION
  Meaningful description of the definitions contained in this file.
  Description must specify if the module is portable specific, mobile
  specific, or common to both, and it should alert the reader if the
  module contains any conditional definitions which tailors the module to
  different targets.  Include any initialization and synchronizing
  requirements.

EXTERNALIZED FUNCTIONS
  List functions and a brief description that are exported from this file

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Detail how to initialize and use this service.  The sequencing aspect
  is only needed if the order of operations is important.
  
Portions copyrighted by PacketVideo Corporation; 
Copyright 1998, 2002, 2003 PacketVideo Corporation, All Rights Reserved; and
Portions copyrighted by QUALCOMM Incorporated; 
Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/utils/oscl/main/latest/src/oscl_clock.cpp#7 $
$DateTime: 2008/05/08 14:04:55 $
$Change: 656428 $


========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "oscl_clock.h"

/* ==========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                            Function Definitions
** ======================================================================= */

/*======================================================================
FUNCTION:
  OSCL_Clock

DESCRIPTION:
  The constructor sets a flag to indicate whether the play clock has 
  been initialized yet.

INPUT/OUTPUT PARAMETERS:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
======================================================================*/
OSCL_Clock::OSCL_Clock() :   
ClockOffset        (0),
IsInitialized      (false)
{
  memset(&systemTimeZero, 0, sizeof(systemTimeZero));
} // OSCL_Clock

/*======================================================================
FUNCTION:
  ~OSCL_Clock

DESCRIPTION:
  The destructor performs no tasks.

INPUT/OUTPUT PARAMETERS:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
======================================================================*/
OSCL_Clock::~OSCL_Clock()
{
} // ~OSCL_Clock

/*======================================================================
FUNCTION:
  InitializeClock

DESCRIPTION:
  This function resets the clock initialization flag to indicate that
  the clock has NOT been initialized.

    This loop may take up to One Second to complete on PDAs.
    We use this synchronization to trigger the realignment
    of the rtc and gtc gradients as a function of time.

INPUT/OUTPUT PARAMETERS:
  None

RETURN VALUE:
  PV_NO_ERROR

SIDE EFFECTS:
  None
======================================================================*/
int OSCL_Clock::InitializeClock( ) 
{
  ClockOffset = 0;

  //set system time zero
  zrex_get_current_time_offset(&systemTimeZero);

  IsInitialized = true;

  return 0;
} // InitializeClock

/*======================================================================
FUNCTION:
  SetClock

DESCRIPTION:
  This function sets the play clock to the value specified.

INPUT/OUTPUT PARAMETERS:
  ClockValue      - The play clock will be set to this time

RETURN VALUE:
  PV_NO_ERROR

SIDE EFFECTS:
  None
======================================================================*/
int OSCL_Clock::SetClock(int ClockValue)
{
  if ( !IsInitialized )
  {
    (void)InitializeClock();
  }

  // In milliSeconds
  int CurrentClock = GetClock();

  ClockOffset = ClockValue - (CurrentClock - ClockOffset);  

  return 0;
} // SetClock


/*======================================================================
FUNCTION:
  GetClock

DESCRIPTION:
  This function returns the current value of the play clock
  in milliseconds.

  This is a shared resource and is therefore protected by a lock 
  mechanism to enforce serial access from multiple threads.

INPUT/OUTPUT PARAMETERS:
  None

RETURN VALUE:
  Value of the play clock in milliseconds

SIDE EFFECTS:
  None
======================================================================*/
int OSCL_Clock::GetClock()
{
  zrex_time_offset_type timeNow;
  zrex_get_current_time_offset(&timeNow);

  unsigned long msec = 1000*(timeNow.seconds - systemTimeZero.seconds) 
                       + (timeNow.milliseconds - systemTimeZero.milliseconds);
  return msec;
} // GetClock

/*======================================================================
FUNCTION:
  CvtClock2Systime

DESCRIPTION:
  This function convert clock time to system time

INPUT/OUTPUT PARAMETERS:
  None

RETURN VALUE:

SIDE EFFECTS:
  None
======================================================================*/
uint64 OSCL_Clock::CvtClock2Systime(uint32 time)
{
  zrex_time_offset_type offset;
  offset.seconds = time / 1000 + systemTimeZero.seconds;
  offset.milliseconds = time % 1000 + systemTimeZero.milliseconds;
  if (offset.milliseconds >= 1000) 
  {
    // carry over
    offset.seconds += offset.milliseconds / 1000;
    offset.milliseconds = offset.milliseconds % 1000;
  }

  //2. convert back to system time (From 1980, Jan, 6, 0h)
  zrex_convert_time_to_systime(&offset);
  return ((uint64)offset.seconds * 1000 + offset.milliseconds);
}

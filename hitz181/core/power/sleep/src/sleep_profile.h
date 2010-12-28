#ifndef SLEEP_PROFILE_H
#define SLEEP_PROFILE_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     SLEEP TIMELINE PROFILING

GENERAL DESCRIPTION
  The events definition for the sleep task profiling

EXTERNALIZED FUNCTIONS
  none

INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

Copyright (c) 2005-2008 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE
$Header: 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/16/08   clm     Fix compile problem.
06/27/08   clm     1H08 sleep re-work.
02/16/06   taw     Removed compile warning for blank line at end of file
06/28/05   jyw     created the file

===========================================================================*/
#include "target.h"

#ifndef FEATURE_L4
  #define PROFILE_SLEEP
  #include "profile_common.h"
#endif /* !FEATURE_L4 */

/* ================ Sleep Profiling Sub Section =============== */

/* Events definitions 
   The number of events has to be less than PROFILE_COMMON_VAL_MASK. 
   See the definitions in the profile_common.h */

typedef enum
{
  SLEEP_PROFILE_TCXO_OFF = 0x00,
  SLEEP_PROFILE_ARM_HALT,
  SLEEP_PROFILE_SLEEPTASK_SLEEP,

  SLEEP_PROFILE_TCXO_ON,
  SLEEP_PROFILE_ARM_ON,
  SLEEP_PROFILE_SLEEPTASK_WAKEUP
} sleep_profile_event_type;

#ifndef FEATURE_L4
  #define PROFILE_SLEEP
#endif /* FEATURE_L4 */

#ifdef PROFILE_SLEEP
  #define SLEEP_PROFILE(event) \
    PROFILE_COMMON(PROFILE_COMMON_SUBTAG_SLEEP, SLEEP_PROFILE_##event)
#else /* !PROFILE_SLEEP */
  #define SLEEP_PROFILE(event)
#endif /* !PROFILE_SLEEP */

#endif /* SLEEP_PROFILE_H */


#ifndef TIME_LOCAL_H
#define TIME_LOCAL_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*==

             S E C U R E   T I M E   H E A D E R    F I L E

GENERAL DESCRIPTION

  This module provides interfaces to set and get the system, user, and
  local time. 

  The system time may only be set by privileged sources of time. 
  The user and local time may be set by various sources. 

  When the system time is requested, the system time is returned 
  unmodified. The system time is the amount of milliseconds since
  the epoch.

  When the user time is requested, an offset is added to the
  system time to create the user time. This user time offset is calculated
  when the user sets the time.

    user_time = system_time + user_time_offset
    
  When the local time is requested, an offset is added to the user
  time to create the local time. This local time offset is set by 
  the user/system.
    
    local_time = user_time + local_time_offset
  
EXTERNALIZED FUNCTIONS

  time_user_get
    This function gets the current user time in milliseconds since the
    epoch. User time is calculated by adding the user time offset to 
    the system time.

  time_user_set_time
    This function sets the current user time in seconds since the epoch.
    The required user time offset is calculated based on the current 
    system time, and stored in NV.
    

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright(c) 2005 - 2006 by QUALCOMM, Incorporated.  All Rights Reserved.

==*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*=============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.


  $Header: //depot/asic/msmshared/services/time/time_local.h#2 $ $DateTime: 2006/04/03 11:07:13 $ $Author: aneufeld $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/03/06   ajn     Featurized based on FEATURE_SEC_TIME
04/28/05   ty      Initial revision

=============================================================================*/

#include "comdef.h"

#ifdef FEATURE_SEC_TIME

#include "qw.h"



/*=============================================================================

                        DATA DECLARATIONS

=============================================================================*/


/*=============================================================================

FUNCTION    time_local_get

DESCRIPTION
  This function gets the current local time in milliseconds since the 
  epoch. Local time is calculated by adding the local time offset to
  the user time.

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  ret_val is filled with the local time

=============================================================================*/

extern void time_local_get
(
    uint64                      *ret_val
);



/*=============================================================================

FUNCTION       time_local_set_offset

DESCRIPTION
  This function sets the local time offset in seconds. This offset is 
  added to user time to get the local time. The new user time offset is not 
  stored to NV; a separate call to time_sec_save is required.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Initializes the system timestamp and sets system time.

=============================================================================*/

extern void time_local_set_offset
(
  /* the local time offset in seconds */
  int32                         offset,

  /* Calling task's TCB pointer */
  rex_tcb_type                  *task_ptr,

  /* Task signal to wait for when reading NV */
  rex_sigs_type                 task_wait_sig,

  /* Task's wait fn to be called when reading NV */
  void                          (*task_wait_handler)(rex_sigs_type)
);



/*=============================================================================

FUNCTION      time_local_get_offset

DESCRIPTION
  This function returns the local time offset in seconds. This offset is
  added to user time to get the local time. This offset is stored in NV.
  
DEPENDENCIES
  None

RETURN VALUE
  The local time offset in seconds.

SIDE EFFECTS
  None

=============================================================================*/

extern int32 time_local_get_offset(void);


#endif /* FEATURE_SEC_TIME */

#endif /* TIME_LOCAL_H */


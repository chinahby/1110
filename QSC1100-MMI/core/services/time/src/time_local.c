/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*==

                 T I M E    S E C U R E   S E R V I C E S

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS
  time_sec_init
    This function initializes the secure time functionality. 
    
  time_local_get
    This function gets the current local time in milliseconds since the 
    epoch. Local time is calculated by adding the local time offset to
    the user time.

  time_local_set_offset
    This function sets the local time offset in seconds. This offset is 
    added to user time to get the local time. This offset is stored in 
    NV.

  time_local_get_offset
    This function gets the local time offset in seconds. This offset is
    added to user time to get the local time. This offset is stored in 
    NV.
    
INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright(c) 2005 - 2006 by QUALCOMM, Incorporated.  All Rights Reserved.

==*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/services/time/time_local.c#2 $ $DateTime: 2006/04/03 11:07:13 $ $Author: aneufeld $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/03/06   ajn     Featurized based on FEATURE_SEC_TIME
03/24/06   ty      Migrating to TIME VU
10/18/05   ty      Return system time when user offset generates invalid time,
                   Reset time offsets if RTC reset, Lint fixes
10/06/05   ih      Fixed bug in get_local_time
04/29/05   ty      Initial Revision.

=============================================================================*/


/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/

#include "comdef.h"

#ifdef FEATURE_SEC_TIME

#include "hw.h"
#include "msg.h"

#include "msm.h"
#include "ts.h"
#include "clki.h"
#include "clkrtc.h"
#include "nv.h"
#ifdef FEATURE_USE_TIME_VU
#include "time_tod.h"
#endif

#include "time_sec.h"
#include "time_user.h"
#include "time_local.h"




/*=============================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

=============================================================================*/

LOCAL int64 time_local_offset = 0;


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

void time_local_get
(
  /* 64-bit local timestamp */
  uint64                        *ret_val
)
{
  /* Current user time */
  uint64                        user_time;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (ret_val == NULL)
  {
    return;
  }

  /* this casting depends on the structure of qword being an array of two 
   * 32 bit values stored in little endian format
   */
  time_user_get(&user_time);
  
  /* add local time offset, check for overflow, underflow */
  *ret_val = user_time + (uint64)time_local_offset;

  if ((*ret_val > user_time) && (time_local_offset < 0 ))
  {
    /* if time_local_offset negative, and result ends up greater than
     * sys time, underflow occurred. Return user time
     */
    *ret_val = user_time;
    MSG_LOW("Local time underflow, returning user time", 0, 0, 0);
  }
  else if ((*ret_val < user_time) && (time_local_offset > 0))
  {
    /* Note that this case should be impossible since we limit
     * the time setting functions to 32bit values specified in seconds
     */
    /* if time_user_offset positive, and result ends up less than
     * sys time, overflow occurred. Return user time.
     */
    *ret_val = user_time;
    MSG_LOW("Local time overflow, returning user time", 0, 0, 0);
  }

} /* time_local_get */



/*=============================================================================

FUNCTION    sectime_get_local_time

DESCRIPTION
  This function gets the current local time in milliseconds since the 
  epoch. Local time is calculated by adding the local time offset to
  the user time.

DEPENDENCIES
  *** Migration aid ***
  This function is deprecated.

RETURN VALUE
  None
  
SIDE EFFECTS
  ret_val is filled with the local time

=============================================================================*/

void sectime_get_local_time
(
  uint64 *ret_val
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  time_local_get(ret_val);

} /* sectime_get_local_time */



/*=============================================================================

FUNCTION       time_local_set_offset_internal

DESCRIPTION
  This function sets the local time offset in seconds. This offset is 
  added to user time to get the local time. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The local time offset is stored to time_local_offset.

=============================================================================*/

void time_local_set_offset_internal
(
  /* the local time offset in milliseconds */
  int64                         new_local_offset
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* set new local offset time in memory */
  time_local_offset = new_local_offset;

} /* time_local_set_offset_internal */



/*=============================================================================

FUNCTION       time_local_set_offset

DESCRIPTION
  This function sets the local time offset in seconds. This offset is 
  added to user time to get the local time. The new user time offset is
  stored to NV.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The local time offset is stored to time_local_offset.

=============================================================================*/

void time_local_set_offset
(
  /* the local time offset in seconds */
  int32                         offset,

  /* Calling task's TCB pointer */
  rex_tcb_type                  *task_ptr,

  /* Task signal to wait for when reading NV */
  rex_sigs_type                 task_wait_sig,

  /* Task's wait fn to be called when reading NV */
  void                          (*task_wait_handler)(rex_sigs_type)
)
{
  /* Local time-zone offset */
  int64                         local_offset;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* set new local offset time in memory */
  local_offset = (int64)offset * SEC_TO_MS_CONV_FACTOR;
  time_local_set_offset_internal(local_offset);

  /* prepare to save to NV */
  time_sec_save(task_ptr, task_wait_sig, task_wait_handler);

} /* time_local_set_offset */



/*=============================================================================

FUNCTION      time_local_get_offset_internal

DESCRIPTION
  This function returns the local time offset in milliseconds. 
  
DEPENDENCIES
  None

RETURN VALUE
  The local time offset in milliseconds.

SIDE EFFECTS
  None
=============================================================================*/

int64 time_local_get_offset_internal(void)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* time_local_offset is kept in milliseconds
   */
  return time_local_offset;

} /* time_local_get_offset_internal */



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

int32 time_local_get_offset(void)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* time_local_offset is provided as a 32 bit long value in seconds,
   * no overflow is expected
   */
  return (int32)(time_local_offset / SEC_TO_MS_CONV_FACTOR);

} /* time_local_get_offset */

#endif /* FEATURE_SEC_TIME */


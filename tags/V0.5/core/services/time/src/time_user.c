/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*==

                 T I M E    S E C U R E   S E R V I C E S

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS
  time_user_get
    This function gets the current user time in milliseconds since the
    epoch. User time is calculated by adding the user time offset to 
    the system time.

  time_user_set
    This function sets the current user time in seconds since the epoch.
    The required user time offset is calculated based on the current 
    system time, and stored in NV.
    
INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright(c) 2005 - 2006 by QUALCOMM, Incorporated.  All Rights Reserved.

==*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/services/time/time_user.c#2 $ $DateTime: 2006/04/03 11:07:13 $ $Author: aneufeld $

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

LOCAL int64 time_user_offset = 0;



/*=============================================================================

FUNCTION     time_user_get

DESCRIPTION
  This function gets the current user time in milliseconds since the
  epoch. User time is calculated by adding the user time offset to 
  the system time.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  ret_val is filled with the user time.

=============================================================================*/

void time_user_get
(
  /* Output: 64-bit user time */
  uint64                        *ret_val
)
{
  /* Current system time */
  uint64                        sys_time;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (ret_val == NULL)
  {
    return;
  }
  /* this casting depends on the structure of qword being an array of two 
   * 32 bit values stored in little endian format
   */
  /*lint -save -e740 */
  clk_read_ms(*((qword*)&sys_time));
  /*lint -restore */
  
  /* add user offset, check for overflow, underflow */
  *ret_val = sys_time + (uint64)time_user_offset;
  
  if ((*ret_val > sys_time) && (time_user_offset < 0 ))
  {
    /* if time_user_offset negative, and result ends up greater than
     * sys time, underflow occurred. Return system time
     */
    *ret_val = sys_time;
    MSG_LOW("User time underflow, returning system time", 0, 0, 0);
  }
  else if ((*ret_val < sys_time) && (time_user_offset > 0))
  {
    /* Note that this case should be impossible since we limit
     * the time setting functions to 32bit values specified in seconds
     */
    /* if time_user_offset positive, and result ends up less than
     * sys time, overflow occurred. Return system time
     */
    *ret_val = sys_time;
    MSG_LOW("User time overflow, returning system time", 0, 0, 0);
  }

} /* time_user_get */



/*=============================================================================

FUNCTION     time_user_set

DESCRIPTION
  This function sets the current user time in seconds since the epoch.
  The required user time offset is calculated based on the current 
  system time. The new user time offset is saved to NV.

  The user time offset is stored in milliseconds, 63 bits long. Therefore, 
  the maximum difference between the actual system time and the user time 
  must be +-292471208 years.

  The user time provided is a maximum of 136 years different from system 
  time since the times are set in 32 bit long seconds.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The user time offset is calculated and stored to time_user_offset.

=============================================================================*/

void time_user_set 
(
  /* new user time provided in seconds */
  uint32                        new_user_time,

  /* Calling task's TCB pointer */
  rex_tcb_type                  *task_ptr,

  /* Task signal to wait for when writing NV */
  rex_sigs_type                 wait_sig,

  /* Task's wait fn to be called when writing NV */
  void                          (*wait_handler)(rex_sigs_type)
)
{
  /* Current system time */
  uint64                        sys_time;

  /* User time, in milliseconds */
  uint64                        new_user_time_ms;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  time_sec_get(&sys_time);
  
  new_user_time_ms = (uint64)new_user_time * SEC_TO_MS_CONV_FACTOR;
 
  time_user_set_offset((int64)(new_user_time_ms - sys_time), 
                              task_ptr, wait_sig, wait_handler); 

  /* no overflow/underflow should be able to occur since provided time is
   * 32 bit long, in seconds.
   */

} /* time_user_set */



/*=============================================================================

FUNCTION     time_user_get_offset_internal

DESCRIPTION
  This function gets the current user time offset in milliseconds
  
DEPENDENCIES
  None

RETURN VALUE
  Returns user time offset in milliseconds

SIDE EFFECTS
  None
=============================================================================*/

int64 time_user_get_offset_internal(void)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  return time_user_offset;

} /* time_user_get_offset_internal */



/*=============================================================================

FUNCTION     time_user_get_offset

DESCRIPTION
  This function gets the current user time offset in milliseconds
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_user_get_offset(int64 *user_offset)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  *user_offset = time_user_offset;

} /* time_user_get_offset_internal */



/*=============================================================================

FUNCTION     time_user_set_offset_internal

DESCRIPTION
  This function sets the current user time offset in milliseconds.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

=============================================================================*/

void time_user_set_offset_internal
(
  /* New local timezone offset, in milliseconds */
  int64                         new_user_offset
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  time_user_offset = new_user_offset;

} /* time_user_set_offset_internal */

/*=============================================================================

FUNCTION     time_user_set_offset

DESCRIPTION
  This function sets the current user time offset in milliseconds and saves
  it to NV.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  User time offset is saved to NV

=============================================================================*/

void time_user_set_offset
(
  /* New user time offset */
  int64                         new_user_offset,

  /* Calling task's TCB pointer */
  rex_tcb_type                  *task_ptr,

  /* Task signal to wait for when writing NV */
  rex_sigs_type                 wait_sig,

  /* Task's wait fn to be called when writing NV */
  void                          (*wait_handler)(rex_sigs_type)
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  time_user_set_offset_internal(new_user_offset);
  time_sec_save(task_ptr, wait_sig, wait_handler);

} /* time_user_set_offset */


#endif /* FEATURE_SEC_TIME */



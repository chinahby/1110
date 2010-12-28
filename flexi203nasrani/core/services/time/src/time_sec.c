/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*==

                 S E C U R E   T I M E   S E R V I C E S

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS
  time_sec_init
    This function initializes the secure time functionality. 
    
  time_sec_get
    This function gets the current system time in milliseconds since the
    epoch. 

  time_sec_set
    This function sets the current system time in seconds since the epoch.
    The time is stored in the RTC.

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright(c) 2005 - 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
==*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/services/time/time_sec.c#3 $ $DateTime: 2006/04/03 13:31:26 $ $Author: aneufeld $

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



#ifndef FEATURE_USE_TIME_VU
/* Check if RTC has been reset */
extern ts_status_type           rtc_poweron_status;
#endif

int64 time_local_get_offset_internal(void);
int64 time_user_get_offset_internal(void);
void time_user_set_offset_internal(int64 new_user_offset);
void time_local_set_offset_internal(int64 new_local_offset);

/*=============================================================================

FUNCTION    time_sec_init

DESCRIPTION
  This function initializes the secure time functionality.   

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  Local time offset and user time offset are loaded from NV.
  
=============================================================================*/

void time_sec_init
(
  /* Calling task's TCB pointer */
  rex_tcb_type                  *task_ptr,

  /* Task signal to wait for when reading NV */
  rex_sigs_type                 wait_sig,

  /* Task's wait fn to be called when reading NV */
  void                          (wait_handler)(rex_sigs_type)
)
{
  /* to be used in TMC */
  nv_cmd_type                   nv_cmd_buf; 

  /* NV data buffer */
  nv_item_type                  nv_data;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ((task_ptr == NULL) || (wait_handler == NULL))
  {
    MSG_ERROR("time_sec_init() failed, task %x, handler %x", 
                                          task_ptr, wait_handler, 0);
    return;
  }

#ifdef FEATURE_USE_TIME_VU
  if (time_tod_status() == TS_VALID)
#else
  if (rtc_poweron_status == TS_VALID)
#endif
  {
    /* RTC time is valid, use the user offsets */
    /* set up command buffer */
    nv_cmd_buf.cmd = NV_READ_F;                /* Read request         */
  }
  else
  {
    /* RTC time is invalid, reset the user offsets */
    nv_cmd_buf.cmd = NV_WRITE_F;               /* Write request        */
    nv_data.sectime_time_offsets.local_offset = 0;
    nv_data.sectime_time_offsets.user_offset = 0;
  }

  nv_cmd_buf.tcb_ptr = task_ptr;               /* Notify back to me    */
  nv_cmd_buf.sigs = wait_sig;                  /* With this signal     */
  nv_cmd_buf.done_q_ptr = NULL;                /* No buffer to return  */
  nv_cmd_buf.item = NV_SECTIME_TIME_OFFSETS_I; /* Item to get          */
  nv_cmd_buf.data_ptr = &nv_data;              /* Where to return it   */

  /* Clear signal, issue the command, and wait for the respone. */
  (void) rex_clr_sigs(task_ptr, wait_sig);
  nv_cmd(&nv_cmd_buf);
  wait_handler(wait_sig);
  (void) rex_clr_sigs( task_ptr, wait_sig );
  
  /* Now, local time offset should have been read in */
  if (nv_cmd_buf.status != NV_DONE_S )
  {
    MSG_HIGH("time_sec_init() failed to read or reset time offsets in NV (%d)", 
                                                        nv_cmd_buf.cmd, 0, 0);
  }
  else
  {
    time_local_set_offset_internal(nv_data.sectime_time_offsets.local_offset);
    time_user_set_offset_internal(nv_data.sectime_time_offsets.user_offset);
  }

} /* time_sec_init */



/*=============================================================================

FUNCTION    time_sec_save

DESCRIPTION
  This function saves the user time offset and local time offset to NV

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  Local time offset and user time offset are saved to NV.
  
=============================================================================*/

void time_sec_save
(
  /* Calling task's TCB pointer */
  rex_tcb_type                  *task_ptr,

  /* Task signal to wait for when reading NV */
  rex_sigs_type                 wait_sig,

  /* Task's wait fn to be called when reading NV */
  void                          (wait_handler)(rex_sigs_type)
)
{
  /* to be used in TMC */
  nv_cmd_type                   nv_cmd_buf;

  /* NV data buffer */
  nv_item_type                  nv_data;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ((task_ptr == NULL) || (wait_handler == NULL))
  {
    MSG_ERROR("time_sec_save() failed, task %x, handler %x", 
                                          task_ptr, wait_handler, 0);
    return;
  }

  nv_data.sectime_time_offsets.local_offset = time_local_get_offset_internal();
  nv_data.sectime_time_offsets.user_offset = time_user_get_offset_internal();
  
  /* set up command buffer */
  nv_cmd_buf.cmd = NV_WRITE_F;                 /* Write request        */
  nv_cmd_buf.tcb_ptr = task_ptr;               /* Notify back to me    */
  nv_cmd_buf.sigs = wait_sig;                  /* With this signal     */
  nv_cmd_buf.done_q_ptr = NULL;                /* No buffer to return  */
  nv_cmd_buf.item = NV_SECTIME_TIME_OFFSETS_I; /* Item to get          */
  nv_cmd_buf.data_ptr = &nv_data;              /* Where to return it   */

  /* Clear signal, issue the command, and wait for the respone. */
  (void) rex_clr_sigs(task_ptr, wait_sig);
  nv_cmd(&nv_cmd_buf);
  wait_handler(wait_sig);
  (void) rex_clr_sigs(task_ptr, wait_sig);
  
  /* Now, local time offset should have been read in */
  if (nv_cmd_buf.status != NV_DONE_S )
  {
    MSG_HIGH("time_sec_save() failed to save sectime offsets to NV", 0, 0, 0);
  }

} /* time_sec_save */



/*=============================================================================

FUNCTION      time_sec_get

DESCRIPTION
  This function gets the current system time in milliseconds since the
  epoch. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  ret_val is filled with the system time.

=============================================================================*/

void time_sec_get
(
  /* Output: 64-bit timestamp */
  uint64                        *ret_val
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (ret_val == NULL)
  {
    return;
  }

  /* this casting depends on the structure of qword being an array of two 
   * 32 bit values stored in little endian format
   */
  /*lint -save -e740 */
  clk_read_ms(*((qword*)ret_val));
  /*lint -restore */

} /* time_sec_get */



/*=============================================================================

FUNCTION     time_sec_set

DESCRIPTION
  This function sets the current system time in seconds since the epoch.
  The time is stored in the RTC.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  User time offset is recalculated to maintain user time, and written to NV.

=============================================================================*/

void time_sec_set
(
  /* new system time in seconds */
  uint32                        new_sys_time,

  /* Calling task's TCB pointer */
  rex_tcb_type                  *task_ptr,

  /* Task signal to wait for when writing NV */
  rex_sigs_type                 wait_sig,

  /* Task's wait fn to be called when writing NV */
  void                          (*wait_handler)(rex_sigs_type)
)
{
  /* Old user time offset */
  int64                         old_user_offset;

  /* Old user time */
  uint64                        old_sys_time;

  /* offset bw new time and old time */
  int64                         time_offset;
  
  /* Local copy of RTC time */
  clk_julian_type               rtc_time;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* retain old user time */
  time_sec_get(&old_sys_time);

  clk_secs_to_julian(new_sys_time, &rtc_time);
  clk_rtc_set_time(&rtc_time);

  /* calculate offset and adjust user time offset */
  time_user_get_offset(&old_user_offset);
  if (old_user_offset != 0)
  {
    /* if old system time is so big as to be interpreted as
     * a negative time as a int64, reset the user offset.
     *
     * the new system time will never be able to be
     * interpreted as negative time as a int64
     */
    if ((int64)old_sys_time < 0)
    {
      MSG_HIGH("Old system time too big, user time offset reset to 0", 0, 0, 0);
      time_user_set_offset(0uLL, task_ptr, wait_sig, wait_handler);
    }
    else
    {
      time_offset = (int64)old_sys_time
                  - ((int64)new_sys_time * SEC_TO_MS_CONV_FACTOR);
      old_user_offset += time_offset;
      time_user_set_offset(old_user_offset, task_ptr, wait_sig, wait_handler);
    }
  }

} /* time_sec_set */


#endif /* FEATURE_SEC_TIME */


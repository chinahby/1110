/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                       S O  R E T R Y  D E L A Y  

GENERAL DESCRIPTION
  This file contains functions that are used to implement the retry delay
  processing that is part of IS2000.  Retry delays apply to all pkt 
  service options.
  
EXTERNALIZED FUNCTIONS
  DS707_SO_RETRYDEL_INIT
    Called at startup.  Initializes delay to zero (i.e., not
    delayed).
    
  DS707_SO_RETRYDEL_SET_DELAY
    Sets the delay for packet calls.  The delay is stored as the time in
    the future at which the phone is allowed to re-orig with a packet
    data calls.
    
  DS707_SO_RETRYDEL_CLR_DELAY
    Clears the retry delay for packet data calls.  The delay is set back
    to zero.
    
  DS707_SO_RETRYDEL_CLR_ALL_DELAYS
    Clears retry delay for packet data calls.
    
  DS707_SO_RETRYDEL_SO_IS_DELAYED
    Returns TRUE or FALSE, depending on if packet data calls are 
    delayed.
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  Must call ds707_so_retrydel_init() at startup.

 Copyright (c) 2001 by QUALCOMM Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_so_retrydel.c_v   1.2   21 Jan 2003 22:24:42   akhare  $
  $Header: //source/qcom/qct/modem/data/1x/707/main/lite/src/ds707_so_retrydel.c#1 $ $DateTime: 2007/11/06 01:17:08 $ $Author: nsivakum $

when       who        what, where, why
--------   ---        -------------------------------------------------------
04/11/03   ak         Updated with better msg when receiving retry delay.
01/13/03   ak         Updated to now have retry delay apply to all service
                      options, instead of being per service option.
11/17/02   ak         Updated file header comments.
03/30/01   ak         Initial revision.
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "comdef.h"
#include "target.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#include "clk.h"
#include "msg.h"
#include "qw.h"


/*===========================================================================
                        TYPEDEFS AND VARIABLES
===========================================================================*/
LOCAL qword ds_retry_exp_time;       /* when retry delay ends (absolute)   */

/*===========================================================================
                   INTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
                   EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
FUNCTION      DS707_SO_RETRYDEL_INIT
 
DESCRIPTION   Initializes the retry delay.  Called only at powerup.

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_so_retrydel_init(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  qw_set(ds_retry_exp_time, 0, 0);
} /* ds707_so_retrydel_init() */

/*===========================================================================
FUNCTION      DS707_SO_RETRYDEL_SET_DELAY
 
DESCRIPTION   Sets the delay for all pkt data calls.  If the delay is
              infinite, then the expiration_time is set to all F's (hex).

DEPENDENCIES  None.  

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_so_retrydel_set_delay
(
  boolean        delay_infinite,            /* is the delay infinite       */
  uint32         delay                      /* the delay in msec from now  */
)
{
  qword    curr_time = {0,0};               /* curr time since epoch       */
  qword    ep_delay  = {0,0};               /* delay in msecs from epoch   */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    If the delay is infinite, then set the expiration time to the max
    possible.  Else add the delay to the current time, and store it away
  -------------------------------------------------------------------------*/
  if (delay_infinite == TRUE)
  {
    qw_set(ds_retry_exp_time,0xFFFF,0xFFFF);
    MSG_MED("Pkt SOs retry delay is infinite", 0, 0, 0);
  }
  else
  {
    clk_read_ms(curr_time);
    qw_set(ep_delay,0,delay);
    qw_add(ds_retry_exp_time, curr_time, ep_delay);
    MSG_MED("Pkt SOs retry delay is for %d msec", delay, 0, 0);
    MSG_MED("Pkt delayed till %d curr %d",ds_retry_exp_time[0],
                                          curr_time[0],
                                          0);
  }
} /* ds707_so_retrydel_set_delay() */

/*===========================================================================
FUNCTION      DS707_SO_RETRYDEL_CLR_DELAY
 
DESCRIPTION   Clears the retry delay for pkt data so's.

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_so_retrydel_clr_delay(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  qw_set(ds_retry_exp_time, 0, 0);
  MSG_MED("Pkt Data Retry delay cleared",0,0,0);
} /* ds707_so_retrydel_clr_all_delays() */

/*===========================================================================
FUNCTION      DS707_SO_RETRYDEL_IS_DELAYED
 
DESCRIPTION   Returns TRUE or FALSE, indicating if packet data calls are 
              being blocked/delayed by the base station.

DEPENDENCIES  None.

RETURN VALUE  TRUE = cannot originate pkt data right now, since the base
                     station has delayed it.
              FALSE= can originate pkt data now.

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_so_retrydel_is_delayed(void)
{
  qword    curr_time = {0,0};               /* curr time since epoch       */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  clk_read_ms(curr_time);
  if (qw_cmp(ds_retry_exp_time,curr_time) > 0)
  {
    MSG_MED("Pkt Data is delayed",0,0,0);
    MSG_MED("delay %d curr %d",ds_retry_exp_time[0],
                               curr_time[0],
                               0);
    return(TRUE);
  }
  else
  {
    return(FALSE);
  }
} /* ds707_so_retrydel_is_delayed() */

#endif /* FEATURE_DATA_IS707 */


/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               3 G   D A T A   S E R V I C E S   M A N A G E R

                        T I M E R   F U N C T I O N S

GENERAL DESCRIPTION
  This software unit contains wrapper functions for REX timer services.

EXTERNALIZED FUNCTIONS
  ds3gi_timer_init()
    Initializes timers

  ds3gi_process_timer_expired_cmd()
    Called to process the DS_TIMER_EXPIRED_CMD

  ds3gi_start_timer()
    Starts a specified timer

  ds3gi_stop_timer()
    Stops a specified timer

INITIALIZATION AND SEQUENCING REQUIREMENTS
  ds3gi_timer_init() must be called once, during DS task startup, to
  initialize the timer services.


  Copyright (c) 2001-2008 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds3gtimer.c_v   1.1   13 Aug 2002 16:24:52   sramacha  $
  $Header: //source/qcom/qct/modem/data/common/dsmgr/main/lite/src/ds3gtimer.c#2 $ $DateTime: 2008/02/27 00:32:32 $ $Author: nsivakum $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/01/08   sn      Fixed lint errors.
11/01/06   an      Featurize ATCOP, RMSM, Async Fax
03/01/02   smp     Added DCD wink timer.
12/14/01   smp     Renamed ds3g_ring_serial_port() to ds3g_alert_user().
07/10/01   smp     Initial version.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA

#include "assert.h"
#include "msg.h"
#include "err.h"
#include "rex.h"
#include "dstask.h"
#include "ds3gmgr.h"
#include "ds3gtimer.h"
#include "ds3gmgrint.h"
#ifndef FEATURE_DATA_STRIP_ATCOP
#include "ds3gsiolib.h"
#endif


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*---------------------------------------------------------------------------
       TIMERS
---------------------------------------------------------------------------*/
typedef struct
{
  rex_timer_type    *timer_ptr;
}ds3gi_timer_type;

LOCAL ds3gi_timer_type ds3gi_timer_tbl[DS3G_TIMER_MAX];


/*===========================================================================

                      INTERNAL FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================

FUNCTION DS3GI_TIMER_CB

DESCRIPTION
  Callback function registered with REX, used to notify DS of timer
  expirations. This function sends a command to the DS task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void  ds3gi_timer_cb
(
  unsigned long  timer_id      /* Used to identify which timer has expired */
)
{
  ds_cmd_type  *cmd_ptr;                             /* Pointer to command */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Send a DS_TIMER_EXPIRED_CMD to the DS task, and indicate which timer
    expired.
  -------------------------------------------------------------------------*/
  if( ( cmd_ptr = ds_get_cmd_buf() ) != NULL )
  {
    cmd_ptr->hdr.cmd_id = DS_TIMER_EXPIRED_CMD;
    cmd_ptr->cmd.timer.timer_id = timer_id;

    ds_put_cmd( cmd_ptr );
  }
  else
  {
    ERR_FATAL( "Can't get cmd buf from DS task", 0, 0, 0 );
  }

} /* ds3gi_timer_cb() */


/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================

FUNCTION DS3GI_TIMER_INIT

DESCRIPTION
  This function initializes the timer services. This function must be called
  once, at Data Services Task startup.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void  ds3gi_timer_init( void )
{
  unsigned long i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   for (i = 0; i < DS3G_TIMER_MAX; i++)
   {
     ds3gi_timer_tbl[i].timer_ptr = NULL;
   }
} /* ds3gi_timer_init() */


/*===========================================================================

FUNCTION DS3GI_TIMER_REGISTER

DESCRIPTION
  Clients register their timer info with timer services.  The passed in
  ID must come from the ds3g_timer_enum_type.  Once registered, the 
  timers cannot be de-registered (though they can be stopped/started).
  
  Registers the timer with REX.  Timer is not started.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void  ds3gi_timer_register
(
  rex_timer_type       *timer_ptr,
  rex_timer_cb_type     timer_cb_ptr,
  ds3g_timer_enum_type  timer_id    
)
{
  int i;                    /* used to get rid of constant casts */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  i = (int)timer_id;

  /*-------------------------------------------------------------------------
    Do error checking.  If everything okay, then register timer with REX.
    if..then else if.. structure enforces timer being cleared only when
    input data is valid.
  -------------------------------------------------------------------------*/
  if ((timer_id <= DS3G_TIMER_MIN) ||
      (timer_id >= DS3G_TIMER_MAX))
  {
    ERR("Bad timer id on init %d",timer_id,0,0);
    ASSERT(0);
  }
  else if (ds3gi_timer_tbl[i].timer_ptr != NULL)
  {
    ERR("Timer already reg'd at loc %d",timer_id, 0, 0);
    ASSERT(0);
  }
  else
  {
    ds3gi_timer_tbl[i].timer_ptr = timer_ptr;
    rex_def_timer_ex(timer_ptr, timer_cb_ptr, i);
  }
} /* ds3gi_timer_register() */

/*===========================================================================

FUNCTION DS3GI_START_TIMER

DESCRIPTION
  This function starts the specified timer for the specified duration.

DEPENDENCIES
  A valid timer id must be specified, otherwise the timer is not started.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void  ds3gi_start_timer
(
  ds3g_timer_enum_type  timer_id,                   /* Timer to be started */
  dword                 duration         /* Timer duration in milliseconds */
)
{
  int i;                    /* used to get rid of constant casts */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  i = (int)timer_id;

  /*-------------------------------------------------------------------------
    Do error checking.  If everything okay, then start the timer with REX.
    if..then else if.. structure enforces timer being cleared only when
    input data is valid.
  -------------------------------------------------------------------------*/
  if ((timer_id <= DS3G_TIMER_MIN) ||
      (timer_id >= DS3G_TIMER_MAX))
  {
    ERR("Bad timer id on start %d",timer_id,0,0);
    ASSERT(0);
  }
  else if (ds3gi_timer_tbl[i].timer_ptr == NULL)
  {
    ERR("Trying to start unregd timer %d",timer_id,0,0);
    ASSERT(0);
  }
  else
  {
    rex_set_timer(ds3gi_timer_tbl[i].timer_ptr,(rex_timer_cnt_type)duration);
  }
} /* ds3gi_start_timer() */



/*===========================================================================

FUNCTION DS3GI_STOP_TIMER

DESCRIPTION
  This function stops the specified timer.

DEPENDENCIES
  A valid timer id must be specified, otherwise no action is taken.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void  ds3gi_stop_timer
(
  ds3g_timer_enum_type  timer_id                    /* Timer to be stopped */
)
{
  int i;                    /* used to get rid of constant casts */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  i = (int)timer_id;

  /*-------------------------------------------------------------------------
    Do error checking.  If everything okay, then clr the timer with REX.
    if..then...else if.. structure enforces timer being cleared only when
    input data is valid.
  -------------------------------------------------------------------------*/
  if ((timer_id <= DS3G_TIMER_MIN) ||
      (timer_id >= DS3G_TIMER_MAX))
  {
    ERR("Bad timer id on stop %d",timer_id,0,0);
    ASSERT(0);
  }
  else if (ds3gi_timer_tbl[i].timer_ptr == NULL)
  {
    ERR("Trying to stop unregd timer %d",timer_id,0,0);
    ASSERT(0);
  }
  else
  {
    rex_clr_timer(ds3gi_timer_tbl[i].timer_ptr);
  }
} /* ds3gi_stop_timer() */


/*===========================================================================

FUNCTION DS3GI_PROCESS_TIMER_EXPIRED_CMD

DESCRIPTION
  This function processes the DS_TIMER_EXPIRED_CMD. It determines which timer
  has expired, and calls the appropriate function to handle the timer expiry.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void  ds3gi_process_timer_expired_cmd
(
  ds_cmd_type  *cmd_ptr                              /* Pointer to command */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  /*-------------------------------------------------------------------------
    Call the appropriate function, based on the timer that expired.
  -------------------------------------------------------------------------*/
  switch( cmd_ptr->cmd.timer.timer_id )
  {
    #ifndef FEATURE_DATA_STRIP_ATCOP
    /* Adding the cases DS3G_TIMER_RING_TE and DS3G_TIMER_WINK_DCD under the
     * feature as this function is currently called only from siolib which is
     * not compiled if this feature is defined */
    case DS3G_TIMER_RING_TE:
      ds3g_siolib_alert_user( ds3g_siolib_state.ring_type, TRUE );
      break;

    case DS3G_TIMER_WINK_DCD:
      ds3g_siolib_set_cd_state( DS3G_SIOLIB_CD_WINKING_END );
      break;
    #endif /* FEATURE_DATA_STRIP_ATCOP */
    default:
      ERR( "Timer expired w/ bad timer id %d", cmd_ptr->cmd.timer.timer_id, 
                                               0, 
                                               0
         );
      break;

  } /* switch( timer_id ) */

} /* ds3gi_process_timer_expired_cmd() */


#endif /* FEATURE_DATA */

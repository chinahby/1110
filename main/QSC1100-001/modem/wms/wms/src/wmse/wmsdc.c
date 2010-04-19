/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


           W I R E L E S S    M E S S A G I N G   S E R V I C E S
                    -- Dedicated channel Group

GENERAL DESCRIPTION
  The WMS module which implements the User API for SMS. This source file
  implements the Dedicated Channel group functions which process the client
  commands of the Dedicated Channel group.

Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008
              by QUALCOMM INCORPORATED.
  All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/* ^L<EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$PVCSPath:  L:/src/asw/MSM_MM/Services/wms/vcs/wmsdc.c_v   1.1   08 May 2002 08:42:00   hqu  $
$Header: //source/qcom/qct/modem/wms/wms/rel/qsc1100_05.04.00/src/WMSE/wmsdc.c#2 $ $DateTime: 2008/12/02 13:02:59 $ $Author: pyaramad $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/06/08   YC      Disabled MO msg retry if DC can't be established
01/31/08   PMD     Updated Copyright Information to include 2008
04/03/07   HQ      Added support for Multi-processor Power Collapse.
03/16/07   HQ      Added support for LPM.
01/17/07   HQ      Updated copyright info to include 2007.
11/30/06   NP      WMS to release CDMA DC / traffic call when done with traffic
                   channel use
10/05/06   PMD     Fixed Compile Warnings.
10/05/06   PMD     Remove unused requested_so variable
                   Added code to read SMS Service Option from NV
05/03/06   HQ      Added auto-dc send feature.
04/24/06   HQ      Fixed lint warnings.
04/07/06   HQ      Restart Idle timer if MT or MO records are busy.
01/25/06   VK      Updated Copyright Info
12/21/05   VK      Lint fixes
11/18/05   VK      Cleaned up Lint warnings. Removed some redudant code. 
10/20/05   VK      Removed Lint Suppressions
10/20/05   VK      Updated Copyright info
10/19/05   PMD     Added Function Headers and Comments
02/04/04   PMD     Cleaned up Lint Warnings in code
09/07/03   PMD     Added Support for dc_event String Display
07/26/03   PMD     Changes to suppress / remove Lint Errors
07/15/03   AP      Send DTC_ABORTED when originating/answering a SMS call fails.
09/20/02   AP      Added support for FEATURE_IS95B_ALERT_ANS_BYPASS.
08/06/02   HQ      Added support of FEATURE_MULTIMODE_ARCH.
05/07/02   HQ      Added more support for FEATURE_CDSMS and Multimode.
03/09/02   HQ      Added support of Multimode WMS.
===========================================================================*/


/*===========================================================================
========================  INCLUDE FILES =====================================
===========================================================================*/

#include "comdef.h"

#if defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS)

#include "wmsdc.h"
#include "wmsnv.h"
#include "wmsmsg.h"
#include "wmscl.h"
#include "wmsdbg.h"
#include "msg.h"
#include "err.h"
#ifdef FEATURE_WMS_DEBUG_STRING
#include "string.h"
#endif /* FEATURE_WMS_DEBUG_STRING */
#include "wmscm.h"
#include "wmscfg.h"
#include "task.h"

#ifdef FEATURE_APPS_POWER_COLLAPSE
#error code not present
#endif /* FEATURE_APPS_POWER_COLLAPSE */


/*===========================================================================
======================== STATIC DATA ========================================
===========================================================================*/

#ifdef FEATURE_CDSMS
static wms_dc_s_type              *dc_s_ptr;
static wms_cm_s_type              *cm_s_ptr;
static wms_cfg_s_type             *cfg_s_ptr;
static wms_dc_event_info_s_type   dc_event_info;
static boolean                    is_dc_call_connected = FALSE;

#ifdef FEATURE_APPS_POWER_COLLAPSE
#error code not present
#endif /* FEATURE_APPS_POWER_COLLAPSE */

#endif /* FEATURE_CDSMS */


/*===========================================================================
============================ FUNCTIONS ======================================
===========================================================================*/

/*===========================================================================
FUNCTION wms_dc_s_ptr

DESCRIPTION
  Return the pointer to the DC data.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to the internal DC data.

SIDE EFFECTS
  None.
===========================================================================*/
wms_dc_s_type* wms_dc_s_ptr
(
  void
)
{
  static wms_dc_s_type  dc;

  return &dc;

} /* wms_dc_s_ptr() */


#ifdef FEATURE_CDSMS
/*===========================================================================
FUNCTION wms_dc_filter_events

DESCRIPTION
  Decide whether this event can be sent to the client or not based on
  power management settings.

DEPENDENCIES
  none

RETURN VALUE
  TRUE:  OK to send event to client
  FALSE: Not OK to send event to client

SIDE EFFECTS
  None.
===========================================================================*/
boolean wms_dc_filter_events
(
  wms_client_id_type       client_id,
  wms_client_s_type        * client_ptr,
  wms_dc_event_e_type      event
)
{
#ifdef FEATURE_APPS_POWER_COLLAPSE
#error code not present
#else /* FEATURE_APPS_POWER_COLLAPSE */
  return TRUE;
#endif /* FEATURE_APPS_POWER_COLLAPSE */

} /* wms_dc_filter_events() */

/*===========================================================================
FUNCTION wms_dc_event_notify

DESCRIPTION
  Notify all clients of a DC event.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_dc_event_notify
(
  wms_dc_event_e_type        event,
  wms_dc_event_info_s_type   *dc_event_info_ptr
)
{
  wms_client_s_type           *client_ptr;
  uint8                       i;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
#ifdef FEATURE_WMS_DEBUG_STRING
    MSG_SPRINTF_2(MSG_SSID_DFLT, MSG_LEGACY_HIGH, "Notify: %d %s", event, wms_dbg_get_dc_event_string(event));
#else
    MSG_HIGH("wms_dc_event_notify: %d ", event, 0, 0);
#endif /* FEATURE_WMS_DEBUG_STRING */
  
  /* Notify clients
  */
  for( i=0; i< (uint8) WMS_CLIENT_TYPE_MAX; i++ )
  {
    client_ptr = wms_client_ptr( (wms_client_id_type) i );
    if( client_ptr != NULL &&
        client_ptr->activated == TRUE &&
        client_ptr->dc_event_cb )
    {
      if( TRUE == wms_dc_filter_events( (wms_client_id_type) i, client_ptr, event ))
      {
        client_ptr->dc_event_cb( event, dc_event_info_ptr );
      }
    }
  }

  /* done */
  return;

} /* wms_dc_event_notify() */


/*===========================================================================
FUNCTION wms_dc_connect_proc

DESCRIPTION
  Process the request of connecting DC.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_dc_connect_proc
(
  wms_cmd_type      *cmd_ptr
)
{
  wms_cmd_err_e_type          cmd_err = WMS_CMD_ERR_NONE;
  wms_dc_so_e_type            dc_connect_so = WMS_DC_SO_6;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_MED("wms_dc_connect_proc()",0,0,0);

  if( TRUE == cfg_s_ptr->lpm )
  {
    cmd_err = WMS_CMD_ERR_DC_LPM;
  }
  else if( cm_s_ptr->call_info.call_state != CM_CALL_STATE_IDLE )
  {
    cmd_err = WMS_CMD_ERR_DC_BAD_STATE;
  }
  else
  {
    dc_s_ptr->auto_disc_active = FALSE;

    switch (cmd_ptr->cmd.dc_connect.so)
    {
      case WMS_DC_SO_AUTO:
        /* Change it to Default Value from NV */
        dc_connect_so = dc_s_ptr->default_so_from_nv;
        break;

      case WMS_DC_SO_6:
      case WMS_DC_SO_14:
        /* Pass through to CM */
        dc_connect_so = cmd_ptr->cmd.dc_connect.so;
        break;

      default:
        MSG_ERROR("Bad Service Option %d for wms_dc_connect", cmd_ptr->cmd.dc_connect.so, 0, 0);
        cmd_err = WMS_CMD_ERR_DC_BAD_PARM;
        break;
    }
  }

  wms_client_cmd_status( cmd_ptr, cmd_err );

  if( cmd_err == WMS_CMD_ERR_NONE )
  {
    wms_cm_call_orig( dc_connect_so );

    /* set timer for call origination
    */
    MSG_MED("Set DC call_out_timer: 0x%lx ms", WMS_TIMER_DC_CALL_OUT,0,0);
    (void) rex_set_timer( & dc_s_ptr->call_out_timer, WMS_TIMER_DC_CALL_OUT );

    /* Send event
    */
    dc_event_info.so = dc_connect_so;
    wms_dc_event_notify( WMS_DC_EVENT_CONNECTING, &dc_event_info );
  }

  /* done */
  return;

} /* wms_dc_connect_proc() */


/*===========================================================================
FUNCTION wms_dc_clear_timers

DESCRIPTION
  Clear the Dedicated Channel Timers

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_dc_clear_timers
(
  void
)
{
  (void) rex_clr_timer( & dc_s_ptr->call_out_timer );
  (void) rex_clr_timer( & dc_s_ptr->idle_timer );
  dc_s_ptr->auto_disc_active = FALSE;

} /* wms_dc_clear_timers */


/*===========================================================================
FUNCTION wms_dc_disconnect_proc

DESCRIPTION
  Process the request of disconnecting DC.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_dc_disconnect_proc
(
  wms_cmd_type      *cmd_ptr
)
{
  wms_cmd_err_e_type          cmd_err = WMS_CMD_ERR_NONE;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_MED("wms_dc_disconnect_proc()",0,0,0);

  /* Clear all DC timers
  */
  wms_dc_clear_timers();

  /* End the call
  */
  wms_cm_call_end();


  wms_client_cmd_status( cmd_ptr, cmd_err );

  /* Send event
  */
  dc_s_ptr->call_active = FALSE;
  dc_s_ptr->wms_initiated_call = FALSE;
  wms_dc_event_notify( WMS_DC_EVENT_DISCONNECTED, &dc_event_info );

  /* done */
  return;

} /* wms_dc_disconnect_proc() */


/*===========================================================================
FUNCTION wms_dc_enable_auto_disc_proc

DESCRIPTION
  Process the request of setting auto-disconnect timeout value.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  Auto-disconnect timeout value is updated.
===========================================================================*/
void wms_dc_enable_auto_disc_proc
(
  wms_cmd_type      *cmd_ptr
)
{
  wms_cmd_err_e_type          cmd_err = WMS_CMD_ERR_NONE;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_MED("wms_dc_enable_auto_disc_proc()",0,0,0);

  dc_s_ptr->idle_timeout = cmd_ptr->cmd.dc_enable_auto_disc.timeout * 1000;


  wms_client_cmd_status( cmd_ptr, cmd_err );

  /* Send event
  */
  
  dc_event_info.idle_timeout = cmd_ptr->cmd.dc_enable_auto_disc.timeout;
  wms_dc_event_notify( WMS_DC_EVENT_ENABLE_AUTO_DISCONNECT, &dc_event_info );

  /* done */
  return;

} /* wms_dc_enable_auto_disc_proc() */



/*===========================================================================
FUNCTION wms_dc_disable_auto_disc_proc

DESCRIPTION
  Process the request of disabling auto-disconnect.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  Auto-disconnect timeout is reset.
===========================================================================*/
void wms_dc_disable_auto_disc_proc
(
  wms_cmd_type      *cmd_ptr
)
{
  wms_cmd_err_e_type          cmd_err = WMS_CMD_ERR_NONE;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_MED("wms_dc_disable_auto_disc_proc()",0,0,0);

  dc_s_ptr->idle_timeout = 0;

  wms_client_cmd_status( cmd_ptr, cmd_err );

  /* Send event
  */
  dc_event_info.idle_timeout = 0;
  wms_dc_event_notify( WMS_DC_EVENT_DISABLE_AUTO_DISCONNECT, &dc_event_info );

  /* done */
  return;

} /* wms_dc_disable_auto_disc_proc() */


/*===========================================================================
FUNCTION wms_dc_incoming_call_proc

DESCRIPTION
  Process the incoming call event from CM.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_dc_incoming_call_proc
(
  wms_cmd_type      *cmd_ptr
)
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_MED("wms_dc_incoming_call_proc()",0,0,0);

  cm_s_ptr->call_info = cmd_ptr->cmd.call_info;


  /* Send event
  */
#ifdef FEATURE_IS95B_ALERT_ANS_BYPASS
    /* If the bypass flag is set, CM has already answered the call, therefore
    ** don't tell CM to answer the call again.
    */
#ifdef FEATURE_MULTIMODE_ARCH
    if( !cm_s_ptr->call_info.mode_info.info.cdma_call.alert_ans_bypass )
#else
    if( !cm_s_ptr->call_info.alert_ans_bypass )
#endif /* FEATURE_MULTIMODE_ARCH */
#endif /* FEATURE_IS95B_ALERT_ANS_BYPASS */
    {
      /* auto answer the SMS call
      */
      MSG_MED("Answering incoming call...",0,0,0);

      wms_cm_call_answer();
    }

    dc_event_info.so  = (wms_dc_so_e_type)
#ifdef FEATURE_MULTIMODE_ARCH
               cm_s_ptr->call_info.mode_info.info.cdma_call.srv_opt;
#else /* FEATURE_MULTIMODE_ARCH */
               cm_s_ptr->call_info.srv_opt;
#endif /* FEATURE_MULTIMODE_ARCH */

    dc_event_info.idle_timeout = dc_s_ptr->idle_timeout;

    wms_dc_event_notify( WMS_DC_EVENT_INCOMING, &dc_event_info );

    /* set timer for call origination
    */
    MSG_MED("Set DC call_out_timer: 0x%lx ms", WMS_TIMER_DC_CALL_OUT,0,0);
    (void) rex_set_timer( & dc_s_ptr->call_out_timer, WMS_TIMER_DC_CALL_OUT );


  /* done */
  return;

} /* wms_dc_incoming_call_proc() */



/*===========================================================================
FUNCTION wms_dc_connected_call_proc

DESCRIPTION
  Process the connected call event from CM.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_dc_connected_call_proc
(
  wms_cmd_type      *cmd_ptr
)
{
  wms_cmd_err_e_type          cmd_err = WMS_CMD_ERR_NONE;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
   
  MSG_MED("wms_dc_connected_call_proc()",0,0,0);
  
  is_dc_call_connected = TRUE; 
  /* Trigger retry of large messages over DC */
  wms_msg_auto_dc_send();

#ifdef FEATURE_MULTIMODE_ARCH
  if( cmd_ptr->cmd.call_info.call_id != cm_s_ptr->call_info.call_id )
  {
    MSG_ERROR("call_id mismatch in Call Connect!",0,0,0);
    cmd_err = WMS_CMD_ERR_DC_BAD_CALL_ID;
  }
#endif /* FEATURE_MULTIMODE_ARCH */

  /* Send event
  */
  if( cmd_err == WMS_CMD_ERR_NONE )
  {
    cm_s_ptr->call_info = cmd_ptr->cmd.call_info;

    /* notify the client of the event
    */
#ifdef FEATURE_MULTIMODE_ARCH
    dc_event_info.so = (wms_dc_so_e_type) cmd_ptr->cmd.call_info.mode_info.info.cdma_call.srv_opt;
#else /* FEATURE_MULTIMODE_ARCH */
    dc_event_info.so = (wms_dc_so_e_type) cmd_ptr->cmd.call_info.srv_opt;
#endif /* FEATURE_MULTIMODE_ARCH */
    dc_event_info.idle_timeout = dc_s_ptr->idle_timeout;

    dc_s_ptr->call_active = TRUE;
    wms_dc_event_notify( WMS_DC_EVENT_CONNECTED, &dc_event_info );

    wms_dc_clear_timers();

    if( dc_s_ptr->idle_timeout != 0)
    {
      MSG_MED("Set DC_idle_timer: 0x%lx ms", dc_s_ptr->idle_timeout,0,0);
      (void) rex_set_timer( & dc_s_ptr->idle_timer, dc_s_ptr->idle_timeout );
      dc_s_ptr->auto_disc_active = TRUE;
    }

  }

  /* done */
  return;

} /* wms_dc_connected_call_proc() */



/*===========================================================================
FUNCTION wms_dc_call_end_proc

DESCRIPTION
  Process the ended call event from CM.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_dc_call_end_proc
(
  wms_cmd_type      *cmd_ptr
)
{
  //wms_cmd_err_e_type          cmd_err = WMS_CMD_ERR_NONE;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_MED("wms_dc_call_end_proc()",0,0,0);

  // If the DC call is ended before the call is connected,
  // stop retry sending large MO messages over DC. 
  if (!is_dc_call_connected) {
    MSG_MED("wms_msg_stop_dc_resend()",0,0,0);
    wms_msg_stop_dc_resend();
  }

  is_dc_call_connected = FALSE;
   
#ifdef FEATURE_MULTIMODE_ARCH
  if( cmd_ptr->cmd.call_info.call_id != cm_s_ptr->call_info.call_id )
  {
    MSG_ERROR("call_id mismatch in Call Connect!",0,0,0);
    //cmd_err = WMS_CMD_ERR_DC_BAD_CALL_ID;
  }
#endif /* FEATURE_MULTIMODE_ARCH */

  /* Send event
  */
  //if( cmd_err == WMS_CMD_ERR_NONE )
  {
    cm_s_ptr->call_info = cmd_ptr->cmd.call_info;

    /* notify the client of the event
    */
#ifdef FEATURE_MULTIMODE_ARCH
    dc_event_info.so = (wms_dc_so_e_type) cmd_ptr->cmd.call_info.mode_info.info.cdma_call.srv_opt;
#else /* FEATURE_MULTIMODE_ARCH */
    dc_event_info.so = (wms_dc_so_e_type) cmd_ptr->cmd.call_info.srv_opt;
#endif /* FEATURE_MULTIMODE_ARCH */
    dc_event_info.idle_timeout = dc_s_ptr->idle_timeout;

    dc_s_ptr->call_active = FALSE;
    dc_s_ptr->wms_initiated_call = FALSE;
    wms_dc_event_notify( WMS_DC_EVENT_DISCONNECTED, &dc_event_info );

    wms_dc_clear_timers();

  }

  /* done */
  return;

} /* wms_dc_call_end_proc() */


/*===========================================================================
FUNCTION wms_dc_call_error_proc

DESCRIPTION
  Process the call cmd error event from CM.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_dc_call_error_proc
(
  wms_cmd_type      *cmd_ptr
)
{
  //wms_cmd_err_e_type          cmd_err = WMS_CMD_ERR_NONE;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_MED("wms_dc_call_end_proc()",0,0,0);

  if( cmd_ptr->cmd.call_error.call_cmd == CM_CALL_CMD_ANSWER )
  {
    /* need to abort the call request */
    MSG_MED("Ending the call due to error()...",0,0,0);
    wms_cm_call_end();

    /* notify the client of the event
    */
#ifdef FEATURE_MULTIMODE_ARCH
    dc_event_info.so = (wms_dc_so_e_type) cmd_ptr->cmd.call_info.mode_info.info.cdma_call.srv_opt;
#else /* FEATURE_MULTIMODE_ARCH */
    dc_event_info.so = (wms_dc_so_e_type) cmd_ptr->cmd.call_info.srv_opt;
#endif /* FEATURE_MULTIMODE_ARCH */

    dc_event_info.idle_timeout = dc_s_ptr->idle_timeout;

    wms_dc_event_notify( WMS_DC_EVENT_ABORTED, &dc_event_info );

    wms_dc_clear_timers();
  }
  else if( cmd_ptr->cmd.call_error.call_cmd == CM_CALL_CMD_ORIG )
  {
    /* Notify clients that orig failed */
    MSG_HIGH("SMS call orig failed!",0,0,0);

    dc_event_info.idle_timeout = dc_s_ptr->idle_timeout;
    wms_dc_event_notify( WMS_DC_EVENT_ABORTED, &dc_event_info );
    wms_dc_clear_timers();
  }

  /* done */
  return;

} /* wms_dc_call_error_proc() */


/*===========================================================================
FUNCTION wms_dc_call_out_timer_proc

DESCRIPTION
  Process the DC Call out timer

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_dc_call_out_timer_proc
(
  void
)
{
  MSG_HIGH( "got WMS DC_CALL_OUT_TIMER_SIG", 0, 0, 0 );

  (void) rex_clr_sigs( rex_self(), WMS_DC_CALL_OUT_TIMER_SIG );

  /* the base station didn't answer the call in time; abort the call
  */
  wms_cm_call_end();

  /* notify the client of the error
  */
  MSG_HIGH("Sending EVENT_DC_ABORTED to client", 0,0,0);
  wms_dc_event_notify( WMS_DC_EVENT_ABORTED, &dc_event_info );

} /* wms_dc_call_out_timer_proc() */

/*===========================================================================
FUNCTION wms_dc_idle_timer_proc

DESCRIPTION
  Process the DC Idle timer

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_dc_idle_timer_proc
(
  void
)
{
  MSG_HIGH( "got UASMS_DC_IDLE_TIMER_SIG", 0, 0, 0 );

  (void) rex_clr_sigs( rex_self(), WMS_DC_IDLE_TIMER_SIG );

  if( wms_msg_cdma_mt_busy() == TRUE || wms_msg_cdma_mo_busy() == TRUE )
  {
    MSG_HIGH("MT or MO records are in use; restart DC IDLE timer",0,0,0);
    (void) rex_set_timer( & dc_s_ptr->idle_timer, dc_s_ptr->idle_timeout );
  }
  else
  {
    wms_dc_clear_timers();

    /* end the SMS call after the idle period
    */
    wms_cm_call_end();

    /* Event will be sent when we receive call end event from CM */
  }

  return;

} /* wms_dc_idle_timer_proc() */


#endif /* FEATURE_CDSMS */



/*===========================================================================
FUNCTION wms_dc_process_cmd

DESCRIPTION
  Dispatch the DC command processing.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_dc_process_cmd
(
  wms_cmd_type  *cmd_ptr
)
{

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch( cmd_ptr->hdr.cmd_id )
  {

#ifdef FEATURE_CDSMS

    case WMS_CMD_DC_CONNECT:
      wms_dc_connect_proc( cmd_ptr );
      break;

    case WMS_CMD_DC_DISCONNECT:
      wms_dc_disconnect_proc( cmd_ptr );
      break;

    case WMS_CMD_DC_ENABLE_AUTO_DISCONNECT:
      wms_dc_enable_auto_disc_proc( cmd_ptr );
      break;

    case WMS_CMD_DC_DISABLE_AUTO_DISCONNECT:
      wms_dc_disable_auto_disc_proc( cmd_ptr );
      break;

    case WMS_CMD_DC_CALL_INCOM_E:
      wms_dc_incoming_call_proc( cmd_ptr );
      break;

    case WMS_CMD_DC_CALL_CONNECT_E:
      wms_dc_connected_call_proc( cmd_ptr );
      break;

    case WMS_CMD_DC_CALL_END_E:
      wms_dc_call_end_proc( cmd_ptr );
      break;

    case WMS_CMD_DC_CALL_ERROR_E:  /* in case of CM cmd errors */
      wms_dc_call_error_proc( cmd_ptr );
      break;

#endif /* FEATURE_CDSMS */

    default:
      MSG_ERROR("Invalid cmd: %d", cmd_ptr->hdr.cmd_id, 0,0);
      break;
  }
  
  /* done */
  return;

} /* wms_dc_process_cmd() */


/*===========================================================================
FUNCTION wms_dc_init

DESCRIPTION
  Do DC-related initialization.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  DC data initialized.
===========================================================================*/
void wms_dc_init
(
  void
)
{
#ifdef FEATURE_CDSMS
  nv_item_type nvi;
#endif /* FEATURE_CDSMS */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_HIGH("wms_dc_init()",0,0,0);


#ifdef FEATURE_CDSMS
  dc_s_ptr = wms_dc_s_ptr();
  cm_s_ptr = wms_cm_s_ptr();
  cfg_s_ptr = wms_cfg_s_ptr();

  dc_s_ptr->auto_disc_active      = FALSE;
  dc_s_ptr->idle_timeout          = 0;
  dc_s_ptr->wms_initiated_call    = FALSE;
  dc_s_ptr->call_active           = FALSE;

  /* ----------------------- define timers -------------------------- */

  rex_def_timer(
                 & dc_s_ptr->call_out_timer,
                 & wms_tcb,
                 WMS_DC_CALL_OUT_TIMER_SIG
               );

  rex_def_timer(
                 & dc_s_ptr->idle_timer,
                 & wms_tcb,
                 WMS_DC_IDLE_TIMER_SIG
               );

  if ( (NV_DONE_S == wms_nv_read_wait( NV_SMS_SERVICE_OPTION_I, &nvi ))
       && ( (WMS_DC_SO_AUTO == nvi.sms_service_option)
            || (WMS_DC_SO_6 == nvi.sms_service_option)
            || (WMS_DC_SO_14 == nvi.sms_service_option) ) )
  {
    dc_s_ptr->default_so_from_nv = (wms_dc_so_e_type)nvi.sms_service_option;
  }
  else
  {
    /* Not Valid Service Option */
    dc_s_ptr->default_so_from_nv = WMS_DC_SO_6; /* Defaulting to SO_6 */
    nvi.sms_service_option = (uint32)dc_s_ptr->default_so_from_nv;
    (void) wms_nv_write_wait( NV_SMS_SERVICE_OPTION_I, &nvi );
  }

#ifdef FEATURE_APPS_POWER_COLLAPSE
#error code not present
#endif /* FEATURE_APPS_POWER_COLLAPSE */

#endif /* FEATURE_CDSMS */


  /* done */
  return;

} /* wms_dc_init */


#endif /* FEATURE_CDSMS || FEATURE_GWSMS */

/* END of wms_dc.c */

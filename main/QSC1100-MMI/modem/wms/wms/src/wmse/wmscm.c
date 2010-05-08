/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


           W I R E L E S S    M E S S A G I N G   S E R V I C E S
                    -- Interface with CM

GENERAL DESCRIPTION
  The WMS module which implements the User API for SMS. This source file
  implements the interfaces with Call Manager.

Copyright (c) 2001 - 2008 by QUALCOMM INCORPORATED.
  All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/* ^L<EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/wms/wms/rel/qsc1100_05.04.00/src/WMSE/wmscm.c#5 $ $DateTime: 2008/12/02 13:02:59 $ $Author: pyaramad $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/23/08   HN      init is complete if RUIM phone doesn't have a RUIM
07/28/08   HN      Add check for type of SIM card present on phone before doing initialization
06/11/08   PMD     Removed wms_bc_*** APIs for CDMA Broadcast
06/04/08   PMD     Removed support for Auto / Transparent SMS 
01/30/08   PMD/PY  Created cm/nas interfacing layer in wmscm_gw.h and wmscm_gw.c
08/06/07   PMD     Plus code dialing enhancements
07/23/07   PMD     Added support for plus code dialing
07/23/07   NP      LPM broken, WMS does not go or wake up from LPM
05/02/07   HQ      Added FEATURE_WMS_NO_CM_MSG2. Mainlined some IWMSC fields.
04/06/07   HQ      Added detection of PLMN change.
03/16/07   HQ      Added support for LPM.
03/09/07   HQ      Fixed cell change.
03/06/07   HQ      Added support for IWMSC.
03/02/07   HQ      Switch to 1x if SMS over EVDO fails.
03/01/07   HQ      Cleaned up SS info for GW.
02/28/07   HQ      Added support of FEATURE_SMS_OVER_IP;
                   Handle new SIP errors.
02/20/07   HQ      Added support for FEATURE_WMS_CM_NAS_INTERFACES.
01/17/07   HQ      Updated copyright info to include 2007.
11/06/06   PMD     Replaced Feature T_MSM6250 with FEATURE_GWSMS
07/31/06   PMD     Removed unused feature - TBD
07/31/06   PMD     Removed unused feature - FEATURE_GWSMS_INIT_DELAY
04/28/06   PMD     Added support for SIM_INIT_COMPLETED Event for Game Mode
04/24/06   HQ      Fixed lint errors.
01/25/06   VK      Updated Copyright Info
12/23/05   VK      More/Lint fixes
12/22/05   VK      Removed ERR_FATALs
12/21/05   VK      Lint fixes
11/18/05   VK      Cleaned up Lint warnings. Removed some redudant code
10/20/05   VK      Changed byte = uint8, word = uint16, dword = uint32
10/20/05   VK      Removed Lint Suppressions
10/20/05   VK      Updated copyright info
10/19/05   PMD     Added Function Headers and Comments
01/10/05   Rex     Code cleanup for WMS decoupling from NAS module and
                   using CM, featurize as FEATURE_APIONE
                   Added new api wms_cm_nas_event_cb, wms_cm_nas_cmd_cb
04/15/04   PMD     Added support for new flag in cfg_s_ptr: refresh_registered
04/14/04   PMD     Added support for subscription not available event from CM
03/05/04   HQ      Removed use of ASSERT().
02/10/04   HQ      Call wms_cfg_do_cdma_ready_event() in start_full_service.
01/16/04   PMD     Featurization Fixes
01/09/04   HQ      Added check of CDMA and GW subscription available status
                   to kick start WMS CDMA and GW initialization.
11/04/03   HQ      Put wms_cm_get_current_acq_mode() under
                   FEATURE_SMS_TRANSPARENT_MO.
10/29/03   AP      Added utility to check for current acquisition mode.
07/26/03   PMD     Changes to suppress / remove Lint Errors
07/01/03   PMD     Added wms_card_register_for_refresh() in processing of
                   Start Full Service event
04/26/03   HQ/AP   Use WMS_CLIENT_TYPE_INTERNAL instead of DUMMY.
03/28/03   HQ      Send BC enable/disable cmd to lower layers when subscription
                   is available.
02/13/03   HQ      Updated protoype of cm_mm_call_cmd_end().
01/14/03   AP      Added info_type for end params when ending a call.
10/01/02   AP      Added called_number in cm_mm_call_cmd_orig to set up a DTC
                   correctly.  Initialized called_number in wms_cm_init().
08/06/02   HQ      Added support of FEATURE_MULTIMODE_ARCH.
06/15/02   HQ      Added support of FEATURE_BROADCAST_SMS.
06/03/02   SUN     SMMA should be sent only after location update
05/16/02   HQ      Use the new CM SS event to trigger SMMA notification.
05/14/02   SUN     Fixed SMMA Issue
05/09/02   HQ      Added FEATURE_GWSMS_INIT_DELAY.
05/07/02   HQ      Added more support for FEATURE_CDSMS and Multimode.
03/13/02   HQ      Added support of Multimode WMS - added FEATURE_CDSMS.
03/12/02   SUN     Added CM_PH_EVENT_SRV_STATE to callback function
03/07/02   SUN     Removed Compiler warning
01/28/02   PRK     Replaced CM_PH_EVENT_START_FULL_SRV with
                   CM_PH_EVENT_SUBSCRIPTION_AVAILABLE.
12/13/01   HQ      Renamed UASMS to WMS.
12/07/01   HQ      Initial version for MSM_MM WMS.
11/9/01    HQ      Cleaned up compiler warnings.
08/27/01   HQ      Initial version for FEATURE_GWSMS.
===========================================================================*/


/*===========================================================================
========================  INCLUDE FILES =====================================
===========================================================================*/

#include "comdef.h"

#if defined(FEATURE_CDSMS_RUIM) || defined(FEATURE_GWSMS)
#include "gsdi_exp.h"
#endif /* FEATURE_CDSMS_RUIM || FEATURE_GWSMS */

#if defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS)

#include "wmscm.h"
#include "wmscfg.h"
#include "cm.h"
#include "wmsi.h"
#include "err.h"
#include "wmsmsg.h"

#include "wmscm_gw.h"

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_CDSMS
#include "wmsruim.h"
#include "wmsbc.h"
#endif /* FEATURE_CDSMS */

#include "sys.h"

/*===========================================================================
======================== STATIC DATA ========================================
===========================================================================*/
static wms_cm_s_type              *cm_s_ptr;
static wms_cfg_s_type             *cfg_s_ptr;


/*===========================================================================
============================ FUNCTIONS ======================================
===========================================================================*/

/*===========================================================================
FUNCTION wms_cm_s_ptr

DESCRIPTION
  Return the pointer to the CM data.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to the internal CM data.

SIDE EFFECTS
  None.
===========================================================================*/
wms_cm_s_type* wms_cm_s_ptr
(
  void
)
{
  static wms_cm_s_type  cm;

  return &cm;

} /* wms_cm_s_ptr() */



/*===========================================================================
FUNCTION wms_cm_start_full_service

DESCRIPTION
  Forwards CM event to WMS about starting full service of the phone.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cm_start_full_service
(
  boolean       cdma_ready,
  boolean       gw_ready
)
{
  wms_cmd_type        *cmd_ptr;

  cmd_ptr = wms_get_cmd_buf();

  if(cmd_ptr != NULL)
  {
    cmd_ptr->hdr.cmd_id      = WMS_CMD_CM_START_FULL_SERVICE_E;
    cmd_ptr->hdr.client_id   = (uint8) WMS_CLIENT_TYPE_INTERNAL;
    cmd_ptr->hdr.cmd_cb      = NULL;
    cmd_ptr->hdr.user_data   = NULL;

    cmd_ptr->cmd.start_full_service.cdma_ready = cdma_ready;
    cmd_ptr->cmd.start_full_service.gw_ready   = gw_ready;

    wms_put_cmd( cmd_ptr );
  }

  /* done */
  return;

} /* wms_cm_start_full_service() */

/*===========================================================================
FUNCTION wms_cm_lpm_notify

DESCRIPTION
  Notify WMS that LPM is entered.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cm_lpm_notify
(
  boolean   in_lpm
)
{
  wms_cmd_type        *cmd_ptr;

  cmd_ptr = wms_get_cmd_buf();

  if(cmd_ptr != NULL)
  {
    cmd_ptr->hdr.cmd_id      = WMS_CMD_CM_LPM_E;
    cmd_ptr->hdr.client_id   = (uint8) WMS_CLIENT_TYPE_INTERNAL;
    cmd_ptr->hdr.cmd_cb      = NULL;
    cmd_ptr->hdr.user_data   = NULL;

    cmd_ptr->cmd.oprt_mode.in_lpm = in_lpm;
    wms_put_cmd( cmd_ptr );

    MSG_HIGH("Sending LPM event to WMS",0,0,0);
  }

  /* done */
  return;

} /* wms_cm_lpm_notify() */

#ifdef FEATURE_CDSMS
/*===========================================================================
FUNCTION wms_cm_online_proc

DESCRIPTION
  Forwards CM Online event to WMS

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void wms_cm_online_proc
(
  void
)
{

#ifdef FEATURE_CDSMS_BROADCAST
  wms_cmd_type        *cmd_ptr;

  cmd_ptr = wms_get_cmd_buf();

  if(cmd_ptr != NULL)
  {
    cmd_ptr->hdr.cmd_id      = WMS_CMD_CM_ONLINE_E;
    cmd_ptr->hdr.client_id   = (uint8) WMS_CLIENT_TYPE_INTERNAL;
    cmd_ptr->hdr.cmd_cb      = NULL;
    cmd_ptr->hdr.user_data   = NULL;
    /* No command data */

    wms_put_cmd( cmd_ptr );
  }

#endif /* FEATURE_CDSMS_BROADCAST */

  /* done */
  return;

} /* wms_cm_online_proc() */


#ifdef FEATURE_CDSMS_BROADCAST
/*===========================================================================
FUNCTION wms_cm_bc_enable_failure_proc

DESCRIPTION
  Forwards Broadcast Enable Failure event to WMS

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cm_bc_enable_failure_proc
(
  cm_sms_cmd_err_e_type  sms_cmd_err
)
{
  wms_cmd_type        *cmd_ptr;

  cmd_ptr = wms_get_cmd_buf();

  if(cmd_ptr != NULL)
  {
    cmd_ptr->hdr.cmd_id      = WMS_CMD_CM_BC_ENABLE_FAILURE_E;
    cmd_ptr->hdr.client_id   = (uint8) WMS_CLIENT_TYPE_INTERNAL;
    cmd_ptr->hdr.cmd_cb      = NULL;
    cmd_ptr->hdr.user_data   = NULL;

    cmd_ptr->cmd.bc_enable_failure.sms_cmd_err = sms_cmd_err;

    wms_put_cmd( cmd_ptr );
  }

  /* done */
  return;

} /* wms_cm_bc_enable_failure_proc() */

#endif /* FEATURE_CDSMS_BROADCAST */


#endif /* FEATURE_CDSMS */


/*===========================================================================
FUNCTION wms_cm_ph_event_cb

DESCRIPTION
  CM Phone event callback.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cm_ph_event_cb
(
  cm_ph_event_e_type         ph_event,
  const cm_ph_info_s_type    * ph_info_ptr
)
{
  switch( ph_event )
  {
    case CM_PH_EVENT_SUBSCRIPTION_AVAILABLE:
    case CM_PH_EVENT_SUBSCRIPTION_NOT_AVAILABLE:
    case CM_PH_EVENT_SUBSCRIPTION_CHANGED:
      wms_cm_start_full_service( ph_info_ptr->is_cdma_subscription_available,
                                 ph_info_ptr->is_gw_subscription_available );
      break;

    case CM_PH_EVENT_OPRT_MODE:
      if( SYS_OPRT_MODE_LPM == ph_info_ptr->oprt_mode )
      {
        wms_cm_lpm_notify( TRUE );
      } 
      else if( SYS_OPRT_MODE_ONLINE == ph_info_ptr->oprt_mode )
      {
        wms_cm_lpm_notify( FALSE );
      }
      break;

    default:
      break;
  }

  /* done */
  return;

} /* wms_cm_ph_event_cb() */


/*===========================================================================
FUNCTION wms_cm_ss_event_cb

DESCRIPTION
  CM SS event callback.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cm_ss_event_cb
(
  cm_ss_event_e_type            ss_event,
  const cm_mm_ss_info_s_type    * ss_info_ptr
)
{
  wms_cmd_id_e_type  cmd_id = WMS_CMD_DUMMY;
  wms_cmd_type       *cmd_ptr;


  if( NULL == ss_info_ptr )
  {
    MSG_ERROR("cm ss_info_ptr is NULL!",0,0,0);
    return;
  }

  switch( ss_event )
  {
    case CM_SS_EVENT_SRV_CHANGED:
    case CM_SS_EVENT_INFO:
      if( ss_info_ptr->changed_fields & CM_SS_SRV_DOMAIN_MASK ||
          ss_info_ptr->changed_fields & CM_SS_SYS_MODE_MASK ||
          ss_info_ptr->changed_fields & CM_SS_SRV_STATUS_MASK )
      {
        cmd_id = WMS_CMD_SS_CHANGE_INFO;
      }
      else if( (ss_info_ptr->changed_fields & CM_SS_SYS_ID_MASK) &&
               SYS_SYS_ID_TYPE_UMTS == ss_info_ptr->sys_id.id_type )
      {
        /* UMTS PLMN change detected */
        cmd_id = WMS_CMD_SS_CHANGE_INFO;
      }
      break;

#ifdef FEATURE_SMS_OVER_IP
    case CM_SS_EVENT_IPAPP_REG_STATUS:
      if( ss_info_ptr->ipapp_info.call_type_mask & CM_CALL_TYPE_SMS_MASK )
      {
        cmd_id = WMS_CMD_IPAPP_CHANGE_INFO;
      }
      break;
#endif /* FEATURE_SMS_OVER_IP */

    default:
      break;
  }

  if (cmd_id != WMS_CMD_DUMMY)
  {
    cmd_ptr = wms_get_cmd_buf();

    if( cmd_ptr != NULL )
    {
      cmd_ptr->hdr.cmd_id      = cmd_id;
      cmd_ptr->hdr.client_id   = WMS_CLIENT_TYPE_INTERNAL;
      cmd_ptr->hdr.cmd_cb      = NULL;
      cmd_ptr->hdr.user_data   = NULL;

      switch( cmd_id )
      {
        case WMS_CMD_SS_CHANGE_INFO:
          cmd_ptr->cmd.ss_change_info.sys_mode   = ss_info_ptr->sys_mode;
          cmd_ptr->cmd.ss_change_info.srv_domain = ss_info_ptr->srv_domain;
          cmd_ptr->cmd.ss_change_info.srv_status = ss_info_ptr->srv_status;
          if( (ss_info_ptr->changed_fields & CM_SS_SYS_ID_MASK) &&
              SYS_SYS_ID_TYPE_UMTS == ss_info_ptr->sys_id.id_type )
          {
            cmd_ptr->cmd.ss_change_info.umts_plmn_changed = TRUE;
          }
          else
          {
            cmd_ptr->cmd.ss_change_info.umts_plmn_changed = FALSE;
          }
#ifdef CM_API_PLUS_DIALING
          cmd_ptr->cmd.ss_change_info.hs_based_plus_dial_setting
             = ss_info_ptr->plus_dial_setting;
          cmd_ptr->cmd.ss_change_info.home_mobile_country_code 
             = ss_info_ptr->home_mcc;
          cmd_ptr->cmd.ss_change_info.current_mobile_country_code
             = ss_info_ptr->current_mcc;
#endif /* CM_API_PLUS_DIALING */
          break;

        case WMS_CMD_IPAPP_CHANGE_INFO:
          cmd_ptr->cmd.ipapp_change_info = ss_info_ptr->ipapp_info;
          break;
      }

      /* send command to SMS task
      */
      wms_put_cmd( cmd_ptr );
    }
    /* else, command buffer allocation failed. Nothing to do since
       error was reported/logged already; the command will be ignored.  */
  }
  /* else, event was ignored */

  /* done */
  return;

} /* wms_cm_ss_event_cb() */

#ifdef FEATURE_CDSMS

/*===========================================================================
FUNCTION wms_cm_call_event_cb

DESCRIPTION
  CM Call event callback.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cm_call_event_cb
(
  cm_call_event_e_type         call_event,
#ifdef FEATURE_MULTIMODE_ARCH
  const cm_mm_call_info_s_type    * call_info_ptr
#else /* FEATURE_MULTIMODE_ARCH */
  const cm_call_info_s_type    * call_info_ptr
#endif /* FEATURE_MULTIMODE_ARCH */
)
{
  wms_cmd_id_e_type      cmd_id = WMS_CMD_DUMMY;
  wms_cmd_type           * cmd_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( call_info_ptr == NULL )
  {
    MSG_ERROR("call_info_ptr is NULL!",0,0,0);
    return;
  }

  if( call_info_ptr->call_type != CM_CALL_TYPE_SMS )
  {
    /* ignore this non-SMS call event */
  }
  else
  {
    MSG_MED("Got CM event %d", call_event,0,0);

    /* look for events that we are interested in */
    switch( call_event )
    {
      case CM_CALL_EVENT_INCOM:
        cmd_id = WMS_CMD_DC_CALL_INCOM_E;
        break;

      case CM_CALL_EVENT_CONNECT:
        cmd_id = WMS_CMD_DC_CALL_CONNECT_E;
        break;

      case CM_CALL_EVENT_END:
        cmd_id = WMS_CMD_DC_CALL_END_E;
        break;

      default:
        /* events that we are not interested in */
        break;

    } /* switch */

  } /* if */


  if( cmd_id != WMS_CMD_DUMMY )
  {
    cmd_ptr = wms_get_cmd_buf();

    if( cmd_ptr != NULL )
    {
      cmd_ptr->hdr.cmd_id      = cmd_id;
      cmd_ptr->hdr.client_id   = (uint8) WMS_CLIENT_TYPE_INTERNAL;
      cmd_ptr->hdr.cmd_cb      = NULL;
      cmd_ptr->hdr.user_data   = NULL;

      cmd_ptr->cmd.call_info = *call_info_ptr;

      /* send command to SMS task
      */
      wms_put_cmd( cmd_ptr );
    }
    /* else, command buffer allocation failed. Nothing to do since
       error was reported/logged already; the command will be ignored.  */
  }
  /* else, event was ignored */

  /* done */
  return;

} /* wms_cm_call_event_cb() */


/*===========================================================================
FUNCTION   wms_cm_call_cmd_cb

DESCRIPTION
  This is the CM call command status callback function. It sends a command
  to the WMS task notifying the CM command error.

DEPENDENCIES
  WMS command buffer is available

RETURNS
  None

SIDE EFFECTS
  Send a command to the WMS task
===========================================================================*/
static void wms_cm_call_cmd_cb
(
  void                      * user_data,
  cm_call_cmd_e_type        call_cmd,
  cm_call_cmd_err_e_type    call_cmd_err
)
{
  wms_cmd_type         * cmd_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  if( call_cmd_err != CM_CALL_CMD_ERR_NOERR )
  {
    MSG_HIGH("CM cmd %d failed: %d", call_cmd, call_cmd_err, 0 );
    cmd_ptr = wms_get_cmd_buf();

    if( cmd_ptr != NULL )
    {
      cmd_ptr->hdr.cmd_id      = WMS_CMD_DC_CALL_ERROR_E;
      cmd_ptr->hdr.client_id   = (uint8) WMS_CLIENT_TYPE_INTERNAL;
      cmd_ptr->hdr.cmd_cb      = NULL;
      cmd_ptr->hdr.user_data   = NULL;

      cmd_ptr->cmd.call_error.call_cmd     = call_cmd;
      cmd_ptr->cmd.call_error.call_cmd_err = call_cmd_err;

      /* send command to SMS task
      */
      wms_put_cmd( cmd_ptr );
    }
    /* else, command buffer allocation failed. Nothing to do since
       error was reported/logged already; the command will be ignored.  */
  }
  /* else, no error happened; ignore it */

  /* done */
  return;

} /* end of wms_cm_call_cmd_cb() */


/* <EJECT> */
/*===========================================================================
FUNCTION   wms_cm_sms_cmd_cb

DESCRIPTION
  Callback function for CM to report SMS command status

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  Send a command the SMS Task.
===========================================================================*/
void wms_cm_sms_cmd_cb
(
  void                        * user_data,
  cm_sms_cmd_e_type           sms_cmd,
  cm_sms_cmd_err_e_type       sms_cmd_err
)
{
  wms_status_e_type  st = WMS_OK_S;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( sms_cmd_err != CM_SMS_CMD_ERR_NOERR )
  {
    MSG_HIGH("CM cmd %d failed: %d", sms_cmd, sms_cmd_err, 0 );
    if( sms_cmd == CM_SMS_CMD_MO_MSG )
    {
      switch( sms_cmd_err )
      {
        case CM_SMS_CMD_ERR_CLIENT_ID_P:
          ERR( "SMS cmd err: Bad client ID", 0,0,0);
          st = WMS_GENERAL_ERROR_S;
          break;

        case CM_SMS_CMD_ERR_NOT_IDLE_S:
          ERR( "SMS cmd err: Not idle",0,0,0);
          st = WMS_GENERAL_ERROR_S;
          break;


        case CM_SMS_CMD_ERR_OFFLINE_S:
        case CM_SMS_CMD_ERR_CDMA_LOCK_S:
#ifdef FEATURE_MULTIMODE_ARCH
        case CM_SMS_CMD_ERR_EMERGENCY_MODE_S:
#else /* FEATURE_MULTIMODE_ARCH */
        case CM_SMS_CMD_ERR_E911_MODE_S:
#endif /* FEATURE_MULTIMODE_ARCH */
          st = WMS_PHONE_NOT_READY_S;
          break;

        case CM_SMS_CMD_ERR_CALL_ORIG_S:
          st = WMS_NETWORK_NOT_READY_S;
          break;


        case CM_SMS_CMD_ERR_NO_SRV_S:
          st = WMS_NETWORK_NOT_READY_S;
          break;

        case CM_SMS_CMD_ERR_NOT_ALLOWED_IN_AMPS_S:
          st = WMS_NOT_ALLOWED_IN_AMPS_S;
          break;


        case CM_SMS_CMD_ERR_NOT_ANALOG_S:
          MSG_HIGH("AWI ACK was not sent successfully",0,0,0);
          st = WMS_GENERAL_ERROR_S;
          break;


        case CM_SMS_CMD_ERR_NO_BUF_L:
          st = WMS_OUT_OF_RESOURCES_S;
          break;

        case CM_SMS_CMD_ERR_SIP:
          st = WMS_SIP_PERM_ERROR_S;
          break;

        case CM_SMS_CMD_ERR_OTHER:
        default:
          st = WMS_GENERAL_ERROR_S;
          break;
      }

      wms_msg_mc_mo_status( WMS_ERROR_TEMP, st, cfg_s_ptr->last_used_bearer );

    }
#ifdef FEATURE_CDSMS_BROADCAST
    else if( sms_cmd == CM_SMS_CMD_BC_ENABLE )
    {
      wms_cm_bc_enable_failure_proc( sms_cmd_err );
    }
#endif /* FEATURE_CDSMS_BROADCAST */
    //else, don't care about the status of other commands
  }
  /* else, no error happened; ignore it */

  /* done */
  return;

} /* wms_cm_sms_cmd_cb() */


/*===========================================================================
FUNCTION   wms_map_cm_status

DESCRIPTION
  Map the command status from CM to the SMS status.

DEPENDENCIES
  None

RETURNS
  The SMS status.

SIDE EFFECTS
  None
===========================================================================*/
wms_status_e_type wms_map_cm_status
(
  cm_sms_mo_status_e_type cm_status
)
{
  wms_status_e_type st;

  switch( cm_status )
  {
    case CM_SMS_MO_OK:
      st = WMS_OK_S;
      break;

    case CM_SMS_MO_SIP_PERMANENT_FAILURE:
      st = WMS_SIP_PERM_ERROR_S;
      break;

    case CM_SMS_MO_SIP_TEMPORARY_FAILURE:
      st = WMS_SIP_TEMP_ERROR_S;
      break;

    case CM_SMS_MO_HOLD_ORIG:
    case CM_SMS_MO_ABORT:
    case CM_SMS_MO_NO_SRV:
    case CM_SMS_MO_HOLD_ORIG_RETRY_TIMEOUT:
      st = WMS_NETWORK_NOT_READY_S;
      break;

    case CM_SMS_MO_NOT_ALLOWED_IN_AMPS:
      st = WMS_NOT_ALLOWED_IN_AMPS_S;
      break;

    case CM_SMS_MO_L2_ACK_FAILURE:
      st = WMS_NETWORK_FAILURE_S;
      break;

    case CM_SMS_MO_OUT_OF_RESOURCES:
      st = WMS_OUT_OF_RESOURCES_S;
      break;

    case CM_SMS_MO_ACCESS_TOO_LARGE:
      st = WMS_ACCESS_TOO_LARGE_S;
      break;

    case CM_SMS_MO_DTC_TOO_LARGE:
      st = WMS_DTC_TOO_LARGE_S;
      break;

    //case CM_SMS_MO_ACCT_BLOCK:
      //st = WMS_ACCESS_BLOCK_S;
      //break;

    default:  // bad status code
      st = WMS_GENERAL_ERROR_S;
      break;
  }

  return st;

} /* wms_map_cm_status() */


/*===========================================================================
FUNCTION wms_cm_bearer_to_mode_pref

DESCRIPTION
  Map WMS bearer type to CM mode preference.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
cm_mode_pref_e_type wms_cm_bearer_to_mode_pref
(
  wms_bearer_e_type   bearer
)
{
  switch( bearer )
  {
    case WMS_BEARER_CDMA_1X:
      return CM_MODE_PREF_CDMA_ONLY;

    case WMS_BEARER_CDMA_EVDO:
      return CM_MODE_PREF_HDR_ONLY;

    case WMS_BEARER_CDMA_WLAN:
      return CM_MODE_PREF_WLAN_ONLY;

    case WMS_BEARER_GW_CS:
    case WMS_BEARER_GW_PS:
      return CM_MODE_PREF_GSM_WCDMA_ONLY;

    case WMS_BEARER_GW_PS_SIP:
      return CM_MODE_PREF_WCDMA_ONLY;

    case WMS_BEARER_GW_WLAN:
      return CM_MODE_PREF_WCDMA_WLAN;

    default:
      return CM_MODE_PREF_CDMA_ONLY;
  }
} /* wms_cm_bearer_to_mode_pref() */

/*===========================================================================
FUNCTION wms_cm_mode_pref_to_bearer

DESCRIPTION
  Map CM mode preference to WMS bearer type.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
wms_bearer_e_type wms_cm_mode_pref_to_bearer
(
  cm_mode_pref_e_type    mode_pref
)
{
  switch( mode_pref )
  {
    case CM_MODE_PREF_CDMA_ONLY:
      return WMS_BEARER_CDMA_1X;

    case CM_MODE_PREF_HDR_ONLY:
      return WMS_BEARER_CDMA_EVDO;

    case CM_MODE_PREF_CDMA_WLAN:
      return WMS_BEARER_CDMA_WLAN;

    /* These depend on whether WMS interfaces with CM or NAS for GW */
    case CM_MODE_PREF_GSM_WCDMA_ONLY:
      return WMS_BEARER_GW_CS;

    case CM_MODE_PREF_WCDMA_ONLY:
      return WMS_BEARER_GW_PS_SIP;

    case CM_MODE_PREF_WCDMA_WLAN:
      return WMS_BEARER_GW_WLAN;

    default:
      return WMS_BEARER_MAX;
  }
} /* wms_cm_mode_pref_to_bearer() */

/*===========================================================================
FUNCTION wms_cm_sys_mode_to_bearer

DESCRIPTION
  Map CM sys mode to WMS bearer type.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
wms_bearer_e_type wms_cm_sys_mode_to_bearer
(
  sys_sys_mode_e_type    sys_mode
)
{
  switch( sys_mode )
  {
    case SYS_SYS_MODE_CDMA:
      return WMS_BEARER_CDMA_1X;

    case SYS_SYS_MODE_GSM:
    case SYS_SYS_MODE_WCDMA:
    case SYS_SYS_MODE_GW:
      return WMS_BEARER_GW_CS;

    case SYS_SYS_MODE_HDR:
      return WMS_BEARER_CDMA_EVDO;

    case SYS_SYS_MODE_WLAN:
#ifdef FEATURE_CDSMS
      return WMS_BEARER_CDMA_WLAN;
#else
      return WMS_BEARER_GW_WLAN;
#endif

    default:
      return WMS_BEARER_MAX;
  }
} /* wms_cm_sys_mode_to_bearer() */


/*===========================================================================
FUNCTION   wms_cm_sms_event_cb

DESCRIPTION
  SMS event callback function, called when a sms event is happening

DEPENDENCIES
  SMS command buffer is available

RETURNS
  None

SIDE EFFECTS
  Send command to the SMS task
===========================================================================*/
#ifndef FEATURE_IWMSC
void wms_cm_sms_event_cb
(
  cm_sms_event_e_type         sms_event,
  const cm_sms_info_s_type    * sms_info_ptr
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( sms_info_ptr == NULL )
  {
    MSG_ERROR("sms_info_ptr is NULL!",0,0,0);
    return;
  }


  MSG_MED("Got CM SMS event %d", sms_event,0,0);

  /* look for events that we are interested in */
  switch( sms_event )
  {
    case CM_SMS_EVENT_MO_STATUS:
      wms_msg_mc_mo_status(
        sms_info_ptr->mo_status==CM_SMS_MO_OK ? WMS_ERROR_NONE : WMS_ERROR_TEMP,
        wms_map_cm_status( sms_info_ptr->mo_status ),
        wms_cm_sys_mode_to_bearer( sms_info_ptr->sys_mode ) );
      break;


    case CM_SMS_EVENT_MT_MSG:
      wms_msg_cdma_mt_msg( (wms_format_e_type) (sms_info_ptr->mt_sms_format),
                           sms_info_ptr->length,
                           sms_info_ptr->data,
                           wms_cm_sys_mode_to_bearer( sms_info_ptr->sys_mode ),
                           TRUE, // ack_pending
                           WMS_DUMMY_TRANSACTION_ID // not used
                         );
      break;


    default:
      /* events that we are not interested in */
      break;

  } /* switch */


  /* done */
  return;

} /* end of wms_cm_sms_event_cb() */
#endif /* !FEATURE_IWMSC */

#endif /* FEATURE_CDSMS */


/*===========================================================================
FUNCTION wms_cm_init

DESCRIPTION
  Do CM related initialization.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cm_init
(
  void
)
{
  cm_s_ptr = wms_cm_s_ptr();
  cfg_s_ptr = wms_cfg_s_ptr();

  if (cm_client_init( CM_CLIENT_TYPE_SMS, & cm_s_ptr->cm_client ) != CM_CLIENT_OK)
  {
    MSG_ERROR("Failure in cm_client_init",0,0,0);
  }

  if (cm_client_ph_reg( cm_s_ptr->cm_client,
                    wms_cm_ph_event_cb,
                    CM_CLIENT_EVENT_REG,
                    CM_PH_EVENT_OPRT_MODE,
                    (cm_ph_event_e_type) ((uint32)CM_PH_EVENT_MAX-(uint32)1),
                    NULL    // not interested in other clients' cmd errors
                  ) != CM_CLIENT_OK)
  {
    MSG_ERROR("Failure in cm_client_ph_reg",0,0,0);
  }


  if (cm_mm_client_ss_reg( cm_s_ptr->cm_client,
                       wms_cm_ss_event_cb,
                       CM_CLIENT_EVENT_REG,
                       CM_SS_EVENT_SRV_CHANGED,
                       (cm_ss_event_e_type) (CM_SS_EVENT_MAX-1),
                       NULL    // not interested in other clients' cmd errors
                     ) != CM_CLIENT_OK)
  {
    MSG_ERROR("Failure in cm_client_ss_reg",0,0,0);
  }

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_CDSMS
  if (cm_mm_client_call_reg(
                      cm_s_ptr->cm_client,
                      wms_cm_call_event_cb,
                      CM_CLIENT_EVENT_REG,
                      CM_CALL_EVENT_ORIG,
                      (cm_call_event_e_type)((uint32)CM_CALL_EVENT_MAX-(uint32)1),
                      NULL    // not interested in other clients' cmd errors
                    ) != CM_CLIENT_OK)
  {
    MSG_ERROR("Failure in cm_client_call_reg",0,0,0);
  }

#ifndef FEATURE_IWMSC
  if (cm_client_sms_reg(  cm_s_ptr->cm_client,
                      wms_cm_sms_event_cb,
                      CM_CLIENT_EVENT_REG,
                      CM_SMS_EVENT_MO_MSG,
                      (cm_sms_event_e_type)((uint32)CM_SMS_EVENT_MAX-(uint32)1),
                      NULL    // not interested in other clients' cmd errors
                    )!= CM_CLIENT_OK)
  {
    MSG_ERROR("Failure in cm_client_sms_reg",0,0,0);
  }
#endif /* !FEATURE_IWMSC */

#ifdef FEATURE_MULTIMODE_ARCH
  /* Initialize called_number, to be used for originating DTC. */
  cm_num_init(&cm_s_ptr->called_number);
#endif /* FEATURE_MULTIMODE_ARCH */
#endif /* FEATURE_CDSMS */

  if (cm_client_act( cm_s_ptr->cm_client) != CM_CLIENT_OK)
  {
    MSG_ERROR("Failure in cm_client_act",0,0,0);
  }

  /* done */
  return;

} /* wms_cm_init() */



/*===========================================================================
FUNCTION wms_cm_process_event

DESCRIPTION
  Process events from CM.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cm_process_event
(
  wms_cmd_type    *cmd_ptr
)
{
  switch( cmd_ptr->hdr.cmd_id )
  {
    case WMS_CMD_CM_START_FULL_SERVICE_E:
      /* Phone is ready for full service. Do SIM related initialization.
      */
#ifdef FEATURE_CDSMS
      if (cmd_ptr->cmd.start_full_service.cdma_ready == FALSE)
      {
        cfg_s_ptr->cdma_init_complete = FALSE;
      }
      else if( cmd_ptr->cmd.start_full_service.cdma_ready == TRUE 
               && cfg_s_ptr->cdma_init_complete           == FALSE)
      {
         boolean bRUIMCardPresent = FALSE;
         boolean bSIMCardPresent = FALSE;

         wmsi_get_card_presence(&bRUIMCardPresent, &bSIMCardPresent);

         if (bRUIMCardPresent) {
#ifdef FEATURE_CDSMS_RUIM
             wms_ruim_init();
#endif /* FEATURE_CDSMS_RUIM */             
         }
         else {
             wms_cfg_do_cdma_ready_event();
         }

#ifdef FEATURE_CDSMS_BROADCAST
        wms_bc_online_proc();
#endif /* FEATURE_CDSMS_BROADCAST */
      }
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
      break;


#ifdef FEATURE_CDSMS_BROADCAST

    case WMS_CMD_CM_ONLINE_E:
      wms_bc_online_proc();
      break;

    case WMS_CMD_CM_BC_ENABLE_FAILURE_E:
      wms_bc_enable_failure_proc( cmd_ptr );
      break;
#endif /* FEATURE_CDSMS_BROADCAST */

    case WMS_CMD_SS_CHANGE_INFO:
      wms_cfg_ss_change_info( & cmd_ptr->cmd.ss_change_info );
      break;

#ifdef FEATURE_SMS_OVER_IP
    case WMS_CMD_IPAPP_CHANGE_INFO:
      wms_cfg_ipapp_change_info( & cmd_ptr->cmd.ipapp_change_info );
      break;
#endif /* FEATURE_SMS_OVER_IP */

    case WMS_CMD_CM_LPM_E:
      wms_cfg_lpm_proc( cmd_ptr->cmd.oprt_mode );
      break;

    default:
      ERR("Invalid cmd: %d", cmd_ptr->hdr.cmd_id, 0,0);
      break;
  }

  /* done */
  return;

} /* wms_cm_process_event() */


#ifdef FEATURE_CDSMS
/*===========================================================================
FUNCTION wms_cm_call_orig

DESCRIPTION
  WMS CM Call Origrination Function

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cm_call_orig
(
  wms_dc_so_e_type   so
)
{
  cm_srv_type_e_type   srv_type;

  MSG_HIGH("Originating SMS call: so=%d", so, 0, 0);

  if( so == WMS_DC_SO_AUTO )
  {
    srv_type = CM_SRV_TYPE_CDMA_AUTOMATIC;
  }
  else
  {
    srv_type = CM_SRV_TYPE_CDMA_SPECIFIC;
  }


#ifdef FEATURE_MULTIMODE_ARCH
  {
    cm_cdma_orig_params_s_type  op;
    cm_orig_alpha_s_type        alpha;

    /*-------------------------------------------------------------------*/

    op.srv_opt        = (uint16)so;
    op.activate_code  = CM_OTASP_ACT_CODE_NONE;
    op.drs_bit        = FALSE;
    op.sr_id_included = FALSE;
    op.qos_parms_incl = FALSE;

    alpha.len = 3;
    memcpy( alpha.buf, "SMS", 3 );

    (void)cm_mm_call_cmd_orig(
                      wms_cm_call_cmd_cb,
                      NULL,
                      cm_s_ptr->cm_client,
                      CM_CALL_TYPE_SMS,
                      srv_type,
                      NULL,                      /* calling number */
                      & cm_s_ptr->called_number, /* called number */
                      & alpha,
                      & op,
                      NULL, /* gw cs param */
                      NULL, /* gw ps param */
                      & cm_s_ptr->call_info.call_id
                    );

  }

#else /* FEATURE_MULTIMODE_ARCH */

  cm_call_cmd_orig( wms_cm_call_cmd_cb,
                    NULL,    /* user data */
                    cm_s_ptr->cm_client,
                    CM_CALL_TYPE_SMS,
                    srv_type,
                    so,
                    (const uint8 *) "",     /* num_buf */
                    0,      /* num_len */
                    CM_DIGIT_MODE_4BIT_DTMF,  /* don't care */
                    (const uint8 *) "SMS",   /* alpha_buf */
                    3,                      /* alpha_len */
                    CM_OTASP_ACT_CODE_NONE
                  );

#endif /* FEATURE_MULTIMODE_ARCH */

} /* wms_cm_call_orig() */

/*===========================================================================
FUNCTION wms_cm_call_end

DESCRIPTION
  WMS CM Call End Function

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cm_call_end
(
  void
)
{
  MSG_HIGH("Ending SMS call...", 0, 0, 0);

#ifdef FEATURE_MULTIMODE_ARCH
  {
    cm_end_params_s_type   ep;

    ep.call_id = cm_s_ptr->call_info.call_id;
    ep.info_type = CM_CALL_MODE_INFO_CDMA;
    ep.end_params.cdma_end.end_reason_included = FALSE;

    (void)cm_mm_call_cmd_end(
                     wms_cm_call_cmd_cb,
                     NULL,
                     cm_s_ptr->cm_client,
                     1, /* one call is being released */
                     & ep );
  }
#else /* FEATURE_MULTIMODE_ARCH */
  cm_call_cmd_end( wms_cm_call_cmd_cb,
                   NULL,   /* user data */
                   cm_s_ptr->cm_client
                 );
#endif /* FEATURE_MULTIMODE_ARCH */

} /* wms_cm_call_end() */

/*===========================================================================
FUNCTION wms_cm_call_answer

DESCRIPTION
  WMS CM Call Answer Function

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cm_call_answer
(
  void
)
{
  MSG_HIGH("Answering SMS call...", 0, 0, 0);

#ifdef FEATURE_MULTIMODE_ARCH
  {
    cm_ans_params_s_type     ap;

    ap.info_type = CM_CALL_MODE_INFO_CDMA;
    ap.ans_params.cdma_ans.call_type = CM_CALL_TYPE_SMS;

    (void)cm_mm_call_cmd_answer(
                        wms_cm_call_cmd_cb,
                        NULL,
                        cm_s_ptr->cm_client,
                        cm_s_ptr->call_info.call_id,
                        & ap
                      );
  }
#else /* FEATURE_MULTIMODE_ARCH */
  cm_call_cmd_answer( wms_cm_call_cmd_cb,
                      NULL,        /* user data */
                      cm_s_ptr->cm_client,
                      CM_CALL_TYPE_SMS );
#endif /* FEATURE_MULTIMODE_ARCH */

} /* wms_cm_call_answer() */


#ifdef FEATURE_WMS_NO_CM_MSG2
/*===========================================================================
FUNCTION wms_cm_sms_send_cdma_msg

DESCRIPTION
  CM Send MO SMS Message Function

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cm_sms_send_cdma_msg
(
  uint8     * cdma_mc_msg
)
{
  (void)cm_sms_cmd_mo_msg( wms_cm_sms_cmd_cb,
                     NULL,   // user data
                     cm_s_ptr->cm_client,
                     TRUE,   // request L2 ack
                     cdma_mc_msg
                   );

} /* wms_cm_sms_send_cdma_msg() */

#else /* FEATURE_WMS_NO_CM_MSG2 */

/*===========================================================================
FUNCTION wms_cm_sms_send_cdma_msg2

DESCRIPTION
  CM Send MO SMS Message Function

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cm_sms_send_cdma_msg2
(
  const wms_address_s_type    * address_ptr,
  const wms_OTA_message_type  * OTA_ptr,
  wms_bearer_e_type           bearer
)
{
  cm_sms_msg_type       cm_sms_msg;
  cm_mode_pref_e_type   cm_mode_pref;

  cm_sms_msg.ota.format   = (cm_format_e_type)OTA_ptr->format;
  cm_sms_msg.ota.data_len = OTA_ptr->data_len;
  memcpy( (void*) cm_sms_msg.ota.data, 
          (void*) OTA_ptr->data,
          MIN(OTA_ptr->data_len,CM_SMS_MAX_LEN) );

  cm_sms_msg.address.digit_mode = (cm_digit_mode_e_type)address_ptr->digit_mode;
  cm_sms_msg.address.number_mode = (cm_num_mode_e_type)address_ptr->number_mode;
  cm_sms_msg.address.number_type = (cm_num_type_e_type)address_ptr->number_type;
  cm_sms_msg.address.number_plan = (cm_num_plan_e_type)address_ptr->number_plan;
  cm_sms_msg.address.number_of_digits = address_ptr->number_of_digits;
  memcpy( cm_sms_msg.address.digits, address_ptr->digits, address_ptr->number_of_digits );

  cm_mode_pref = wms_cm_bearer_to_mode_pref( bearer );

  (void)cm_sms_cmd_mo_msg2( wms_cm_sms_cmd_cb,
                            NULL,   // user data
                            cm_s_ptr->cm_client,
                            TRUE,   // request L2 ack
                            & cm_sms_msg,
                            cm_mode_pref
                          );

  return;

} /* wms_cm_sms_send_cdma_msg2() */

#endif /* FEATURE_WMS_NO_CM_MSG2 */


/*===========================================================================
FUNCTION wms_cm_awi_ack

DESCRIPTION
  CM AWI Acknowledgement Function

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cm_awi_ack
(
  uint8                   seq_no,
  boolean                 bf_bit,
  wms_error_class_e_type  error_class,
  wms_cdma_tl_status_e_type    tl_status
)
{
  (void)cm_sms_cmd_awi_ack( NULL, // no cmd cb
                      NULL, // no user data
                      cm_s_ptr->cm_client,
                      seq_no,
                      bf_bit,
                      (uint8)error_class,
                      (uint8)tl_status );
}


#endif /* FEATURE_CDSMS */

#ifdef FEATURE_CDSMS_BROADCAST
/*===========================================================================
FUNCTION wms_cm_bc_enable

DESCRIPTION
  CM Enable Broadcast Functin

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cm_bc_enable
(
  boolean  bc_enable
)
{
  (void) cm_sms_cmd_bc_enable( wms_cm_sms_cmd_cb,
                               NULL,   // user data
                               cm_s_ptr->cm_client,
                               bc_enable );
} /* wms_cm_bc_enable() */

#endif /* FEATURE_CDSMS_BROADCAST */

#endif /* FEATURE_CDSMS || FEATURE_GWSMS */


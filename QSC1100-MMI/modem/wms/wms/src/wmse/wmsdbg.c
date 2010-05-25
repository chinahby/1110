/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


           W I R E L E S S    M E S S A G I N G   S E R V I C E S
                    -- Debug Group

GENERAL DESCRIPTION
  The WMS module which implements the User API for SMS. This source file
  implements the Debug group functions which process the client
  commands of the Debug group.

  Copyright (c) 2002,2003,2004,2005,2006,2007,2008 by QUALCOMM INCORPORATED.
  All Rights Reserved.

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/* ^L<EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$PVCSPath:  L:/src/asw/MSM_MM/Services/wms/vcs/wmsdbg.c_v   1.1   08 May 2002 08:41:58   hqu  $
$Header: //source/qcom/qct/modem/wms/wms/rel/qsc1100_05.04.00/src/WMSE/wmsdbg.c#3 $ $DateTime: 2008/12/02 13:02:59 $ $Author: pyaramad $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/22/08   RG      Added WMS_BC_MM_EVENT_READING_PREF into 
                   wms_dbg_bc_mm_event_string
06/24/08   PMD     Removed Tabs from code
06/13/08   PMD     Removed wms_bc_*** APIs for CDMA Broadcast
04/22/08   PMD     Updated cmd_err Debug String Information
04/18/08   PMD     Removed FEATURE_WMS_CM_NAS_INTERFACES
01/31/08   PMD     Updated Copyright Information to include 2008
08/15/07   PY      split wmsmsg.h to form wmsmsgi.h a private header.
04/03/07   HQ      Added support for Multi-processor Power Collapse.
01/17/07   HQ      Updated copyright info to include 2007.
04/28/06   PMD     Added support for SIM_INIT_COMPLETED Event for Game Mode
04/25/06   HQ      Fixed lint warnings.
04/20/06   PMD     Removed Pending Queue Internal Commands
                   Mainlined FEATURE_SMS_RETRY
01/25/06   VK      Updated Copyright Info
12/21/05   VK      Lint fixes
11/18/05   VK      Cleaned up Lint warnings. Removed some redudant code. 
10/20/05   VK      Removed Lint Suppressions
10/19/05   PMD     Added Function Headers and Comments
10/18/05   VK      SMS_RETRY_Featurization fixes
10/17/05   PMD     Updated Copyright Information.
10/17/05   PMD     Added support for get / set retry interval 
03/10/05   PMD     Added support for wms_bc_mm_msg_delete_all_indication
02/01/05   PMD     Added support for wms_bc_mm_set/get_reading_preferences API 
01/27/05   PMD     Updated CMD Ids to reflect latest from WMS
01/11/05   PMD     Updated cmd, cmd_err and event information for Debug Strings
01/07/05   PMD     Added Debug Event Information for GSM
11/10/04   Rex     Remove compile warning
08/25/04   Rex     Added support for SMS link control
08/02/04   PMD     Featurization Cleanups
07/30/04   HQ      Removed ERR_FATALs.
07/15/04   PMD     Added support for CB Page Deletion Indication
03/23/04   PMD     Changed Primary Client and Memory Status Implementation.
02/04/04   PMD     Cleaned up Lint Warnings in code
12/15/03   PMD     Added 1x Broadcast Support in Multimode Broadcast API
12/02/03   HQ      Resolved some Lint errors.
10/20/03   PMD     Fixed more Compile Warnings for Cheetah
10/15/03   HQ      Fixed compiler warnings.
09/22/03   PMD     Changed Featurization from CDSMS to CDSMS || GWSMS
                   Added new functions to clear SMMA and Reset Ref #
09/07/03   PMD     Added Debugging Information String Display for cmd_id,
                   cmd_err, cfg_event, msg_event, dc_event, bc_event,
                   bc_mm_event, dbg_event
05/07/02   HQ      Added more support for FEATURE_CDSMS and Multimode.
03/26/02   HQ      Initial version for Multimode WMS.
===========================================================================*/


/*===========================================================================
========================  INCLUDE FILES =====================================
===========================================================================*/

#include "comdef.h"

#if (defined(FEATURE_GWSMS) || defined(FEATURE_CDSMS))

#include "wms.h"
#include "wmsi.h"
#include "wmscl.h"
#include "wmsnv.h"
#include "wmsdbg.h"
#include "wmsmsgi.h"
#include "wmscfg.h"

#include "msg.h"
#include "err.h"
#include "wmssim.h"

#ifdef FEATURE_APPS_POWER_COLLAPSE
#error code not present
#endif /* FEATURE_APPS_POWER_COLLAPSE */


/*===========================================================================
======================== STATIC DATA ========================================
===========================================================================*/
static wms_dbg_s_type              *dbg_s_ptr;
#ifdef FEATURE_GWSMS
#error code not present
#endif
static wms_msg_s_type              *msg_s_ptr;

#ifdef FEATURE_APPS_POWER_COLLAPSE
#error code not present
#endif /* FEATURE_APPS_POWER_COLLAPSE */



static wms_dbg_event_info_s_type   dbg_event_info;

/*===========================================================================
============================ FUNCTIONS ======================================
===========================================================================*/

/*===========================================================================
FUNCTION wms_dbg_s_ptr

DESCRIPTION
  Return the pointer to the Debug data.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to the internal data.

SIDE EFFECTS
  None.
===========================================================================*/
wms_dbg_s_type* wms_dbg_s_ptr
(
  void
)
{
  static wms_dbg_s_type  dbg;

  return &dbg;

} /* wms_dbg_s_ptr() */


/*===========================================================================
FUNCTION wms_dbg_filter_events

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
boolean wms_dbg_filter_events
(
  wms_client_id_type       client_id,
  wms_client_s_type        * client_ptr,
  wms_dbg_event_e_type     event
)
{
#ifdef FEATURE_APPS_POWER_COLLAPSE
#error code not present
#else /* FEATURE_APPS_POWER_COLLAPSE */
  return TRUE;
#endif /* FEATURE_APPS_POWER_COLLAPSE */

} /* wms_dbg_filter_events() */


/*===========================================================================
FUNCTION wms_dbg_event_notify

DESCRIPTION
  Notify all clients of a Debug event.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_dbg_event_notify
(
  wms_dbg_event_e_type        event,
  wms_dbg_event_info_s_type   *dbg_event_info_ptr
)
{
  wms_client_s_type             *client_ptr;
  uint8                         i;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_HIGH("dbg_event_notify: %d", event, 0,0);


  /* Notify clients
  */
  for( i=0; i<(uint8)WMS_CLIENT_TYPE_MAX; i++ )
  {
    client_ptr = wms_client_ptr( (wms_client_id_type) i );
    if( client_ptr != NULL &&
        client_ptr->activated == TRUE &&
        client_ptr->dbg_event_cb )
    {
      if( TRUE == wms_dbg_filter_events( (wms_client_id_type) i, client_ptr, event ))
      {
        client_ptr->dbg_event_cb( event, dbg_event_info_ptr );
      }
    }
  }

  /* done */
  return;

} /* wms_dbg_event_notify() */

#ifdef FEATURE_CDSMS

/*===========================================================================
FUNCTION wms_dbg_reset_tl_seq_num_proc

DESCRIPTION
  Process the request of resetting CDMA TL sequence number.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  NV is updated.
===========================================================================*/
void wms_dbg_reset_tl_seq_num_proc
(
  wms_cmd_type      *cmd_ptr
)
{
  wms_cmd_err_e_type          cmd_err = WMS_CMD_ERR_NONE;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_MED("wms_dbg_reset_tl_seq_num_proc()",0,0,0);

  msg_s_ptr->tl_seq_num = 0;

  wms_client_cmd_status( cmd_ptr, cmd_err );

 
 
  wms_dbg_event_notify( WMS_DBG_EVENT_RESET_TL_SEQ_NUM, &dbg_event_info );

  /* done */
  return;

} /* wms_dbg_reset_tl_seq_num_proc() */

#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

/*===========================================================================
FUNCTION wms_dbg_get_retry_interval_proc

DESCRIPTION
  Get the Retry Interval

DEPENDENCIES
  none

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wms_dbg_get_retry_interval_proc
(
  wms_cmd_type      *cmd_ptr
)
{
  wms_cmd_err_e_type   cmd_err = WMS_CMD_ERR_NONE;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  wms_client_cmd_status( cmd_ptr, cmd_err );

  /* Fill up Event Information */
  dbg_event_info.retry_interval = msg_s_ptr->mo_retry_interval; /* In seconds */

  /* Send event
  */
  wms_dbg_event_notify( WMS_DBG_EVENT_RETRY_INTERVAL, &dbg_event_info );
  
} /* wms_dbg_get_retry_interval_proc */

/*===========================================================================
FUNCTION wms_dbg_set_retry_interval_proc

DESCRIPTION
  Sets the Retry Interval

DEPENDENCIES
  none

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wms_dbg_set_retry_interval_proc
(
  wms_cmd_type      *cmd_ptr
)
{
  wms_cmd_err_e_type   cmd_err = WMS_CMD_ERR_NONE;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( cmd_ptr->cmd.dbg_set_retry_interval.retry_interval > WMS_MAX_RETRY_PERIOD )
  {
    MSG_ERROR("Retry Interval exceeds Retry Period", 0, 0, 0);
    cmd_err = WMS_CMD_ERR_DBG_BAD_PARAM;
  }
  else
  {
    nv_item_type          nvi; 
    
    /* In seconds */
    msg_s_ptr->mo_retry_interval = cmd_ptr->cmd.dbg_set_retry_interval.retry_interval;
    nvi.sms_mo_retry_interval    = (byte)cmd_ptr->cmd.dbg_set_retry_interval.retry_interval;
    (void) wms_nv_write_wait( NV_SMS_MO_RETRY_INTERVAL_I, &nvi );
  }

  wms_client_cmd_status( cmd_ptr, cmd_err );

  /* Send event
  */
  if( cmd_err == WMS_CMD_ERR_NONE )
  {
    /* Fill up Event Information */
    dbg_event_info.retry_interval = msg_s_ptr->mo_retry_interval;

    wms_dbg_event_notify( WMS_DBG_EVENT_RETRY_INTERVAL, &dbg_event_info );
  }

} /* wms_dbg_set_retry_interval_proc */

/*===========================================================================
FUNCTION wms_dbg_process_cmd

DESCRIPTION
  Dispatch the Debug command processing.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_dbg_process_cmd
(
  wms_cmd_type  *cmd_ptr
)
{

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch( cmd_ptr->hdr.cmd_id )
  {

#ifdef FEATURE_CDSMS
    case WMS_CMD_DBG_RESET_TL_SEQ_NUM:
      wms_dbg_reset_tl_seq_num_proc( cmd_ptr );
      break;
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

    case WMS_CMD_DBG_GET_RETRY_INTERVAL:
      wms_dbg_get_retry_interval_proc(cmd_ptr);
      break;

    case WMS_CMD_DBG_SET_RETRY_INTERVAL:
      wms_dbg_set_retry_interval_proc(cmd_ptr);
      break;

    default:
      MSG_ERROR("Invalid cmd: %d", cmd_ptr->hdr.cmd_id, 0,0);
      break;
  }

  /* done */
  return;

} /* wms_dbg_process_cmd() */


/*===========================================================================
FUNCTION wms_dbg_init

DESCRIPTION
  Do Debug-related initialization.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  Debug data initialized.
===========================================================================*/
void wms_dbg_init
(
  void
)
{

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_HIGH("wms_dbg_init()",0,0,0);

  dbg_s_ptr = wms_dbg_s_ptr();
#ifdef FEATURE_GWSMS
#error code not present
#endif
  msg_s_ptr = wms_msg_s_ptr();

#ifdef FEATURE_APPS_POWER_COLLAPSE
#error code not present
#endif /* FEATURE_APPS_POWER_COLLAPSE */


  dbg_s_ptr->init_complete = TRUE;

  /* done */
  return;

} /* wms_dbg_init */

#ifdef FEATURE_WMS_DEBUG_STRING

#define WMS_MAX_CMD_IDS 85
#define WMS_MAX_CMD_ERRS 55
#define WMS_MAX_CFG_EVENTS 12
#define WMS_MAX_MSG_EVENTS 22
#define WMS_MAX_DC_EVENTS 8
#define WMS_MAX_BC_EVENTS 12
#define WMS_MAX_BC_MM_EVENTS 15
#define WMS_MAX_DBG_EVENTS 4

typedef struct
{
  uint16 cmd_id;
  char *cmd_string;
}wms_dbg_cmd_info_s_type;

typedef struct
{
  uint16 cmd_err_id;
  char *cmd_err_string;
}wms_dbg_cmd_err_info_s_type;

static wms_dbg_cmd_info_s_type wms_dbg_cmd_info[WMS_MAX_CMD_IDS] =
{
  /* Configuration commands 
  */
  {WMS_CMD_CFG_SET_ROUTES, "WMS_CMD_CFG_SET_ROUTES"},
  {WMS_CMD_CFG_GET_ROUTES, "WMS_CMD_CFG_GET_ROUTES"},
  {WMS_CMD_CFG_GET_MEMORY_STATUS, "WMS_CMD_CFG_GET_MEMORY_STATUS"},
  {WMS_CMD_CFG_GET_MESSAGE_LIST, "WMS_CMD_CFG_GET_MESSAGE_LIST"},
  {WMS_CMD_CFG_SET_GW_DOMAIN_PREF, "WMS_CMD_CFG_SET_GW_DOMAIN_PREF"},
  {WMS_CMD_CFG_GET_GW_DOMAIN_PREF, "WMS_CMD_CFG_GET_GW_DOMAIN_PREF"},
  {WMS_CMD_CFG_SET_PRIMARY_CLIENT, "WMS_CMD_CFG_SET_PRIMARY_CLIENT"},
  {WMS_CMD_CFG_SET_MEMORY_FULL, "WMS_CMD_CFG_SET_MEMORY_FULL"},
  {WMS_CMD_CFG_SET_LINK_CONTROL, "WMS_CMD_CFG_SET_LINK_CONTROL"},
  {WMS_CMD_CFG_GET_LINK_CONTROL, "WMS_CMD_CFG_GET_LINK_CONTROL"},
  

  /* Message commands 
  */
  {WMS_CMD_MSG_SEND, "WMS_CMD_MSG_SEND"},
  {WMS_CMD_MSG_ACK, "WMS_CMD_MSG_ACK"},
  {WMS_CMD_MSG_READ, "WMS_CMD_MSG_READ"},
  {WMS_CMD_MSG_WRITE, "WMS_CMD_MSG_WRITE"},
  {WMS_CMD_MSG_DELETE, "WMS_CMD_MSG_DELETE"},
  {WMS_CMD_MSG_DELETE_ALL, "WMS_CMD_MSG_DELETE_ALL"},
  {WMS_CMD_MSG_MODIFY_TAG, "WMS_CMD_MSG_MODIFY_TAG"},
  {WMS_CMD_MSG_READ_TEMPLATE, "WMS_CMD_MSG_READ_TEMPLATE"},
  {WMS_CMD_MSG_WRITE_TEMPLATE, "WMS_CMD_MSG_WRITE_TEMPLATE"},
  {WMS_CMD_MSG_DELETE_TEMPLATE, "WMS_CMD_MSG_DELETE_TEMPLATE"},
  {WMS_CMD_MSG_DELETE_TEMPLATE_ALL, "WMS_CMD_MSG_DELETE_TEMPLATE_ALL"},

  {WMS_CMD_MSG_READ_STS_REPORT, "WMS_CMD_MSG_READ_STS_REPORT"},
  {WMS_CMD_MSG_WRITE_STS_REPORT, "WMS_CMD_MSG_WRITE_STS_REPORT"},
  {WMS_CMD_MSG_DELETE_STS_REPORT, "WMS_CMD_MSG_DELETE_STS_REPORT"},
  {WMS_CMD_MSG_DELETE_STS_REPORT_ALL, "WMS_CMD_MSG_DELETE_STS_REPORT_ALL"},

  /* DC commands
  */
  {WMS_CMD_DC_CONNECT, "WMS_CMD_DC_CONNECT"},
  {WMS_CMD_DC_DISCONNECT, "WMS_CMD_DC_DISCONNECT"},
  {WMS_CMD_DC_ENABLE_AUTO_DISCONNECT, "WMS_CMD_DC_ENABLE_AUTO_DISCONNECT"},
  {WMS_CMD_DC_DISABLE_AUTO_DISCONNECT, "WMS_CMD_DC_DISABLE_AUTO_DISCONNECT"},

  /* BC_MM commands
  */
  {WMS_CMD_BC_MM_GET_CONFIG, "WMS_CMD_BC_MM_GET_CONFIG"},
  {WMS_CMD_BC_MM_GET_PREF, "WMS_CMD_BC_MM_GET_PREF"},
  {WMS_CMD_BC_MM_SET_PREF, "WMS_CMD_BC_MM_SET_PREF"},
  {WMS_CMD_BC_MM_GET_TABLE, "WMS_CMD_BC_MM_GET_TABLE"},
  {WMS_CMD_BC_MM_SELECT_SRV, "WMS_CMD_BC_MM_SELECT_SRV"},
  {WMS_CMD_BC_MM_GET_ALL_SRV_IDS, "WMS_CMD_BC_MM_GET_ALL_SRV_IDS"},
  {WMS_CMD_BC_MM_GET_SRV_INFO, "WMS_CMD_BC_MM_GET_SRV_INFO"},
  {WMS_CMD_BC_MM_ADD_SRV, "WMS_CMD_BC_MM_ADD_SRV"},
  {WMS_CMD_BC_MM_DELETE_SRV, "WMS_CMD_BC_MM_DELETE_SRV"},
  {WMS_CMD_BC_MM_CHANGE_SRV_INFO, "WMS_CMD_BC_MM_CHANGE_SRV_INFO"},
  {WMS_CMD_BC_MM_DELETE_ALL_SERVICES, "WMS_CMD_BC_MM_DELETE_ALL_SERVICES"},
  {WMS_CMD_BC_MM_SELECT_ALL_SERVICES, "WMS_CMD_BC_MM_SELECT_ALL_SERVICES"},
  {WMS_CMD_BC_MM_SET_PRIORITY_ALL_SERVICES, "WMS_CMD_BC_MM_SET_PRIORITY_ALL_SERVICES"},
  {WMS_CMD_BC_MM_MSG_DELETE_INDICATION, "WMS_CMD_BC_MM_MSG_DELETE_INDICATION"},
  {WMS_CMD_BC_MM_MSG_DELETE_ALL_INDICATION, "WMS_CMD_BC_MM_MSG_DELETE_ALL_INDICATION"},
  {WMS_CMD_BC_MM_GET_READING_PREF, "WMS_CMD_BC_MM_GET_READING_PREF"},
  {WMS_CMD_BC_MM_SET_READING_PREF, "WMS_CMD_BC_MM_SET_READING_PREF"},


  /* DBG commands
  */
  {WMS_CMD_DBG_RESET_TL_SEQ_NUM, "WMS_CMD_DBG_RESET_TL_SEQ_NUM"},
  {WMS_CMD_DBG_SET_MSG_REF_NUM, "WMS_CMD_DBG_SET_MSG_REF_NUM"},
  {WMS_CMD_DBG_CLEAR_SMMA, "WMS_CMD_DBG_CLEAR_SMMA"},



  /* -- NOTE: the following are for internal use only --- */

  /* Internal: events from MC
  */
  {WMS_CMD_MSG_MC_MT_MSG_E, "WMS_CMD_MSG_MC_MT_MSG_E"},
  {WMS_CMD_MSG_MC_MO_STATUS_E, "WMS_CMD_MSG_MC_MO_STATUS_E"},
  {WMS_CMD_MSG_MC_STATE_CHANGED_E, "WMS_CMD_MSG_MC_STATE_CHANGED_E"},
  {WMS_CMD_MSG_MC_MWI_E, "WMS_CMD_MSG_MC_MWI_E"},
  {WMS_CMD_MSG_GSTK_EVT, "WMS_CMD_MSG_GSTK_EVT"},

  /* Internal: events from CM
  */
  {WMS_CMD_DC_CALL_INCOM_E, "WMS_CMD_DC_CALL_INCOM_E"},
  {WMS_CMD_DC_CALL_CONNECT_E, "WMS_CMD_DC_CALL_CONNECT_E"},
  {WMS_CMD_DC_CALL_END_E, "WMS_CMD_DC_CALL_END_E"},
  {WMS_CMD_DC_CALL_ERROR_E, "WMS_CMD_DC_CALL_ERROR_E"},  /* in case of CM cmd errors */

  {WMS_CMD_CM_BC_ENABLE_FAILURE_E, "WMS_CMD_CM_BC_ENABLE_FAILURE_E"},
  {WMS_CMD_CM_START_FULL_SERVICE_E, "WMS_CMD_CM_START_FULL_SERVICE_E"},
  {WMS_CMD_CM_ONLINE_E, "WMS_CMD_CM_ONLINE_E"},

  {WMS_CMD_GSDI_ASYNC_CB, "WMS_CMD_GSDI_ASYNC_CB"},
  {WMS_CMD_GSDI_CARD_STATUS_CB, "WMS_CMD_GSDI_CARD_STATUS_CB"},
  {WMS_CMD_GSDI_REFRESH_FCN_IND, "WMS_CMD_GSDI_REFRESH_FCN_IND"},
  {WMS_CMD_BC_SCPT_DATA, "WMS_CMD_BC_SCPT_DATA"},

  {WMS_CMD_MAX, "WMS_CMD_MAX"}
};

static wms_dbg_cmd_err_info_s_type wms_dbg_cmd_err_info[WMS_MAX_CMD_ERRS] = 
{
  {WMS_CMD_ERR_NONE, "WMS_CMD_ERR_NONE"},

  {WMS_CMD_ERR_CLIENT_ID, "WMS_CMD_ERR_CLIENT_ID"},
  {WMS_CMD_ERR_NO_RESOURCE, "WMS_CMD_ERR_NO_RESOURCE"},
  {WMS_CMD_ERR_UNSUPPORTED, "WMS_CMD_ERR_UNSUPPORTED"},
  {WMS_CMD_ERR_BUSY, "WMS_CMD_ERR_BUSY"},
  {WMS_CMD_ERR_NULL_PTR, "WMS_CMD_ERR_NULL_PTR"},
  {WMS_CMD_ERR_NO_NETWORK, "WMS_CMD_ERR_NO_NETWORK"},
  {WMS_CMD_ERR_BAD_PARM, "WMS_CMD_ERR_BAD_PARM"},
  {WMS_CMD_ERR_CS_ERROR, "WMS_CMD_ERR_CS_ERROR"},

  {WMS_CMD_ERR_CFG_NV_WRITE, "WMS_CMD_ERR_CFG_NV_WRITE"},
  {WMS_CMD_ERR_CFG_NV_READ, "WMS_CMD_ERR_CFG_NV_READ"},
  {WMS_CMD_ERR_CFG_ROUTE, "WMS_CMD_ERR_CFG_ROUTE"},
  {WMS_CMD_ERR_CFG_MEM_STORE, "WMS_CMD_ERR_CFG_MEM_STORE"},
  {WMS_CMD_ERR_CFG_MSG_CLASS, "WMS_CMD_ERR_CFG_MSG_CLASS"},
  {WMS_CMD_ERR_CFG_DOMAIN_PREF, "WMS_CMD_ERR_CFG_DOMAIN_PREF"},
  {WMS_CMD_ERR_CFG_UNPRIVILEGED_CLIENT, "WMS_CMD_ERR_CFG_UNPRIVILEGED_CLIENT"},
  {WMS_CMD_ERR_CFG_LINK_CONTROL, "WMS_CMD_ERR_CFG_LINK_CONTROL"},

  {WMS_CMD_ERR_MSG_MEMORY_STORE, "WMS_CMD_ERR_MSG_MEMORY_STORE"},
  {WMS_CMD_ERR_MSG_INDEX, "WMS_CMD_ERR_MSG_INDEX"},
  {WMS_CMD_ERR_MSG_EMPTY_MESSAGE, "WMS_CMD_ERR_MSG_EMPTY_MESSAGE"},
  {WMS_CMD_ERR_MSG_TAG, "WMS_CMD_ERR_MSG_TAG"},
  {WMS_CMD_ERR_MSG_ENCODE, "WMS_CMD_ERR_MSG_ENCODE"},
  {WMS_CMD_ERR_MSG_DECODE, "WMS_CMD_ERR_MSG_DECODE"},
  {WMS_CMD_ERR_MSG_SEND_MODE, "WMS_CMD_ERR_MSG_SEND_MODE"},
  {WMS_CMD_ERR_MSG_WRITE_MODE, "WMS_CMD_ERR_MSG_WRITE_MODE"},
  {WMS_CMD_ERR_MSG_MEMORY_FULL, "WMS_CMD_ERR_MSG_MEMORY_FULL"},
  {WMS_CMD_ERR_MSG_RAM_FULL, "WMS_CMD_ERR_MSG_RAM_FULL"},
  {WMS_CMD_ERR_MSG_SIM_FULL, "WMS_CMD_ERR_MSG_SIM_FULL"},
  {WMS_CMD_ERR_MSG_NV_FULL, "WMS_CMD_ERR_MSG_NV_FULL"},
  {WMS_CMD_ERR_MSG_TRANSACTION_ID, "WMS_CMD_ERR_MSG_TRANSACTION_ID"},
  {WMS_CMD_ERR_MSG_SIM_WRITE, "WMS_CMD_ERR_MSG_SIM_WRITE"},
  {WMS_CMD_ERR_MSG_SIM_READ, "WMS_CMD_ERR_MSG_SIM_READ"},
  {WMS_CMD_ERR_MSG_SIM_DELETE, "WMS_CMD_ERR_MSG_SIM_DELETE"},
  {WMS_CMD_ERR_MSG_RUIM_WRITE, "WMS_CMD_ERR_MSG_RUIM_WRITE"},
  {WMS_CMD_ERR_MSG_RUIM_READ, "WMS_CMD_ERR_MSG_RUIM_READ"},
  {WMS_CMD_ERR_MSG_RUIM_DELETE, "WMS_CMD_ERR_MSG_RUIM_DELETE"},
  {WMS_CMD_ERR_MSG_NV_WRITE, "WMS_CMD_ERR_MSG_NV_WRITE"},
  {WMS_CMD_ERR_MSG_NV_READ, "WMS_CMD_ERR_MSG_NV_READ"},
  {WMS_CMD_ERR_MSG_NV_DELETE, "WMS_CMD_ERR_MSG_NV_DELETE"},
  {WMS_CMD_ERR_MSG_TPDU_TYPE, "WMS_CMD_ERR_MSG_TPDU_TYPE"},
  {WMS_CMD_ERR_MSG_FORMAT, "WMS_CMD_ERR_MSG_FORMAT"},
  {WMS_CMD_ERR_MSG_NO_MO_MSG, "WMS_CMD_ERR_MSG_NO_MO_MSG"},
  {WMS_CMD_ERR_MSG_NO_SC_ADDRESS, "WMS_CMD_ERR_MSG_NO_SC_ADDRESS"},

  {WMS_CMD_ERR_BC_BAD_PREF, "WMS_CMD_ERR_BC_BAD_PREF"},
  {WMS_CMD_ERR_BC_CANNOT_ACTIVATE, "WMS_CMD_ERR_BC_CANNOT_ACTIVATE"},
  {WMS_CMD_ERR_BC_CANNOT_ACTIVATE_ALL, "WMS_CMD_ERR_BC_CANNOT_ACTIVATE_ALL"},
  {WMS_CMD_ERR_BC_NV_WRITE, "WMS_CMD_ERR_BC_NV_WRITE"},
  {WMS_CMD_ERR_BC_BAD_SRV_ID, "WMS_CMD_ERR_BC_BAD_SRV_ID"},
  {WMS_CMD_ERR_BC_TABLE_FULL, "WMS_CMD_ERR_BC_TABLE_FULL"},
  {WMS_CMD_ERR_BC_DUPLICATE_SRV, "WMS_CMD_ERR_BC_DUPLICATE_SRV"},
  {WMS_CMD_ERR_BC_SIM_WRITE, "WMS_CMD_ERR_BC_SIM_WRITE"},
  {WMS_CMD_ERR_BC_SIM_READ, "WMS_CMD_ERR_BC_SIM_READ"},

  {WMS_CMD_ERR_DC_BAD_STATE, "WMS_CMD_ERR_DC_BAD_STATE"},
  {WMS_CMD_ERR_DC_BAD_CALL_ID, "WMS_CMD_ERR_DC_BAD_CALL_ID"}, 

  {WMS_CMD_ERR_MAX, "WMS_CMD_ERR_MAX"}
};

static char *wms_dbg_cfg_event_string[WMS_MAX_CFG_EVENTS] =
{
  "WMS_CFG_EVENT_GW_READY",
  "WMS_CFG_EVENT_CDMA_READY",
  "WMS_CFG_EVENT_ROUTES",
  "WMS_CFG_EVENT_MEMORY_STATUS",
  "WMS_CFG_EVENT_MESSAGE_LIST",
  "WMS_CFG_EVENT_MEMORY_FULL",
  "WMS_CFG_EVENT_GW_DOMAIN_PREF",
  "WMS_CFG_EVENT_CELL_CHANGE",
  "WMS_CFG_EVENT_PRIMARY_CLIENT_SET",
  "WMS_CFG_EVENT_MEMORY_STATUS_SET",
  "WMS_CFG_EVENT_LINK_CONTROL",
  "WMS_CFG_EVENT_MAX"
};

static char *wms_dbg_msg_event_string[WMS_MAX_MSG_EVENTS] = 
{
  /* Command related events */
  "WMS_MSG_EVENT_SEND",
  "WMS_MSG_EVENT_ACK",
  "WMS_MSG_EVENT_READ",
  "WMS_MSG_EVENT_WRITE",
  "WMS_MSG_EVENT_DELETE",
  "WMS_MSG_EVENT_DELETE_ALL",
  "WMS_MSG_EVENT_MODIFY_TAG",
  "WMS_MSG_EVENT_READ_TEMPLATE",
  "WMS_MSG_EVENT_WRITE_TEMPLATE",
  "WMS_MSG_EVENT_DELETE_TEMPLATE",
  "WMS_MSG_EVENT_DELETE_TEMPLATE_ALL",
  "WMS_MSG_EVENT_READ_STS_REPORT",
  "WMS_MSG_EVENT_WRITE_STS_REPORT",
  "WMS_MSG_EVENT_DELETE_STS_REPORT",
  "WMS_MSG_EVENT_DELETE_STS_REPORT_ALL",

  /* Events from lower layer */
  "WMS_MSG_EVENT_RECEIVED_MESSAGE",
  "WMS_MSG_EVENT_SUBMIT_REPORT",
  "WMS_MSG_EVENT_STATUS_REPORT",
  "WMS_MSG_EVENT_MT_MESSAGE_ERROR", /* for example, MT msg ack timeout */
  "WMS_MSG_EVENT_ACK_REPORT",

  "WMS_MSG_EVENT_DUPLICATE_CB_PAGE",

  "WMS_MSG_EVENT_MAX"
};

static char *wms_dbg_dc_event_string[WMS_MAX_DC_EVENTS] = 
{
  "WMS_DC_EVENT_INCOMING",
  "WMS_DC_EVENT_CONNECTED",
  "WMS_DC_EVENT_ABORTED",
  "WMS_DC_EVENT_DISCONNECTED",
  "WMS_DC_EVENT_CONNECTING",
  "WMS_DC_EVENT_ENABLE_AUTO_DISCONNECT",
  "WMS_DC_EVENT_DISABLE_AUTO_DISCONNECT",
  "WMS_DC_EVENT_MAX"
};

static char *wms_dbg_bc_event_string[WMS_MAX_BC_EVENTS] = 
{
  "WMS_BC_EVENT_CONFIG",
  "WMS_BC_EVENT_PREF",
  "WMS_BC_EVENT_TABLE",
  "WMS_BC_EVENT_SRV_IDS",
  "WMS_BC_EVENT_SRV_INFO",
  "WMS_BC_EVENT_SRV_ADDED",
  "WMS_BC_EVENT_SRV_DELETED",
  "WMS_BC_EVENT_SRV_UPDATED",
  "WMS_BC_EVENT_ALL_SRV_DELETED",
  "WMS_BC_EVENT_ENABLE_FAILURE", 
  "WMS_BC_EVENT_DISABLE_FAILURE",
  "WMS_BC_EVENT_MAX"
};

static char *wms_dbg_bc_mm_event_string[WMS_MAX_BC_MM_EVENTS] = 
{
  "WMS_BC_MM_EVENT_CONFIG",
  "WMS_BC_MM_EVENT_PREF",
  "WMS_BC_MM_EVENT_TABLE",
  "WMS_BC_MM_EVENT_SRV_IDS",
  "WMS_BC_MM_EVENT_SRV_INFO",
  "WMS_BC_MM_EVENT_SRV_UPDATED",
  "WMS_BC_MM_EVENT_ADD_SRVS",
  "WMS_BC_MM_EVENT_DELETE_SRVS",
  "WMS_BC_MM_EVENT_SELECT_ALL_SRVS",
  "WMS_BC_MM_EVENT_SET_PRIORITY_ALL_SRVS",
  "WMS_BC_MM_EVENT_READING_PREF",
  "WMS_BC_MM_EVENT_DELETE_ALL_SRVS", 
  "WMS_BC_MM_EVENT_ENABLE_FAILURE",  
  "WMS_BC_MM_EVENT_DISABLE_FAILURE", 
  "WMS_BC_MM_EVENT_MAX"
};

static char *wms_dbg_dbg_event_string[WMS_MAX_DBG_EVENTS] = 
{
  "WMS_DBG_EVENT_RESET_TL_SEQ_NUM",
  "WMS_DBG_EVENT_SET_SIM_MESSAGE_NUMBER",
  "WMS_DBG_EVENT_CLEAR_SMMA_FLAG",
  "WMS_DBG_EVENT_MAX"
};

/*===========================================================================
FUNCTION wms_dbg_get_cmd_string

DESCRIPTION
  This function returns the Command given the Command ID

DEPENDENCIES
  Command Enumeration (wms_cmd_id_e_type)

RETURN VALUE
  Command String

SIDE EFFECTS
  None
===========================================================================*/
char *wms_dbg_get_cmd_string
(
  uint16 cmd_id
)
{
  uint16 i;
  for (i=0; i<WMS_MAX_CMD_IDS ; i++)
  {
    if (cmd_id == wms_dbg_cmd_info[i].cmd_id)
      return (wms_dbg_cmd_info[i].cmd_string);
  }
  return ("Unknown Command");
} /* wms_dbg_get_cmd_string */

/*===========================================================================
FUNCTION wms_dbg_get_cmd_err_string

DESCRIPTION
  This function returns the Command Error given the Command Error ID

DEPENDENCIES
  Command Error Enumeration (wms_cmd_err_e_type)

RETURN VALUE
  Command String

SIDE EFFECTS
  None
===========================================================================*/
char *wms_dbg_get_cmd_err_string
(
  uint16 cmd_err
)
{
  uint16 i;
  for (i=0; i<WMS_MAX_CMD_ERRS ; i++)
  {
    if (cmd_err == wms_dbg_cmd_err_info[i].cmd_err_id)
      return (wms_dbg_cmd_err_info[i].cmd_err_string);
  }
  return ("Unknown Command Error");
} /* wms_dbg_get_cmd_err_string */

/*===========================================================================
FUNCTION wms_dbg_get_cfg_event_string

DESCRIPTION
  This function returns the Configuration event String given the cfg_event

DEPENDENCIES
  Configuration Event Enumeration (wms_cfg_event_e_type)

RETURN VALUE
  Event String

SIDE EFFECTS
  None
===========================================================================*/
char *wms_dbg_get_cfg_event_string
(
  uint16 cfg_event
)
{
  if (cfg_event < WMS_MAX_CFG_EVENTS)
  {
    return wms_dbg_cfg_event_string[cfg_event];
  }
  return "Invalid Event";
} /* wms_dbg_get_cfg_event_string */

/*===========================================================================
FUNCTION wms_dbg_get_msg_event_string

DESCRIPTION
  This function returns the Message event String given the msg_event

DEPENDENCIES
  Message Event Enumeration (wms_msg_event_e_type)

RETURN VALUE
  Event String

SIDE EFFECTS
  None
===========================================================================*/
char *wms_dbg_get_msg_event_string
(
  uint16 msg_event
)
{
  if (msg_event < WMS_MAX_MSG_EVENTS)
  {
    return wms_dbg_msg_event_string[msg_event];
  }
  return "Invalid Event";
}

/*===========================================================================
FUNCTION wms_dbg_get_dc_event_string

DESCRIPTION
  This function returns the DC event String given the _event

DEPENDENCIES
  DC Event Enumeration (wms_dc_event_e_type)

RETURN VALUE
  Event String

SIDE EFFECTS
  None
===========================================================================*/
char *wms_dbg_get_dc_event_string
(
  uint16 dc_event
)
{
  if (dc_event < WMS_MAX_DC_EVENTS)
  {
    return wms_dbg_dc_event_string[dc_event];
  }
  return "Invalid Event";
} /* wms_dbg_get_dc_event_string */

/*===========================================================================
FUNCTION wms_dbg_get_bc_event_string

DESCRIPTION
  This function returns the Broadcast event String given the bc_event

DEPENDENCIES
  Broadcast Event Enumeration (wms_bc_event_e_type)

RETURN VALUE
  Event String

SIDE EFFECTS
  None
===========================================================================*/
char *wms_dbg_get_bc_event_string
(
  uint16 bc_event
)
{
  if (bc_event < WMS_MAX_BC_EVENTS)
  {
    return wms_dbg_bc_event_string[bc_event];
  }
  return "Invalid Event";
}/* wms_dbg_get_bc_event_string */

/*===========================================================================
FUNCTION wms_dbg_get_event_bc_mm_string

DESCRIPTION
  This function returns the Multimode Broadcast event String given the bc_mm_event

DEPENDENCIES
  Multimode Broadcast Event Enumeration (wms_bc_mm_event_e_type)

RETURN VALUE
  Event String

SIDE EFFECTS
  None
===========================================================================*/
char *wms_dbg_get_bc_mm_event_string
(
  uint16 bc_mm_event
)
{
  if (bc_mm_event < WMS_MAX_BC_MM_EVENTS)
  {
    return wms_dbg_bc_mm_event_string[bc_mm_event];
  }
  return "Invalid Event";
} /* wms_dbg_get_bc_mm_event_string */

/*===========================================================================
FUNCTION wms_dbg_get_dbg_event_string

DESCRIPTION
  This function returns the Debug event String given the dbg_event

DEPENDENCIES
  Debug Event Enumeration (wms_dbg_event_e_type)

RETURN VALUE
  Event String

SIDE EFFECTS
  None
===========================================================================*/
char *wms_dbg_get_dbg_event_string
(
  uint16 dbg_event
)
{
  if (dbg_event < WMS_MAX_DBG_EVENTS)
  {
    return wms_dbg_dbg_event_string[dbg_event];
  }
  return "Invalid Event";
} /* wms_dbg_get_dbg_event_string */

#endif /* FEATURE_WMS_DEBUG_STRING */

#endif /* defined(FEATURE_GWSMS) || defined(FEATURE_CDSMS) */

/* END of wmsdbg.c */

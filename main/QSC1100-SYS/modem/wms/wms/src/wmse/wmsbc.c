/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


           W I R E L E S S    M E S S A G I N G   S E R V I C E S
                    -- Broadcast Group

GENERAL DESCRIPTION
  The WMS module which implements the User API for SMS. This source file
  implements the BC group functions which process the client
  commands of the BC group.

Copyright (c) 1998 - 2008 
by QUALCOMM INCORPORATED.
  All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/* ^L<EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$PVCSPath:  L:/src/asw/MSM_MM/Services/wms/vcs/wmsbc.c_v   1.1   17 Jun 2002 15:46:10   hqu  $
$Header: //source/qcom/qct/modem/wms/wms/rel/qsc1100_05.04.00/src/WMSE/wmsbc.c#3 $ $DateTime: 2008/12/02 13:02:59 $ $Author: pyaramad $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/09/08   PMD     Added Null Pointer check
06/11/08   PMD     Removed wms_bc_*** APIs for CDMA Broadcast
03/07/08   PMD     Typecasted bc_table service to word for checking
01/30/08   PMD/PY  Created cm/nas interfacing layer in wmscm_gw.h and wmscm_gw.c
09/19/07   PMD     Default GW CB Config and Pref to Disallow and Deactivate
09/14/07   PMD     Fixed CBMID bug of not sending srv IDs to Lower Layers
07/11/07   PY      removed wms_bc_s_ptr dependency.
05/16/07   PMD     Fixed Null dem_s_ptr in wms_bc_filter_events
05/01/07   PMD     Fixed Search List Length while sending Search List
04/03/07   HQ      Added support for multi-processor power collapse.
03/08/07   HQ      Fixed CM_NAS_REQ_CB_DELETE_ALL_MESSAGES.
02/20/07   HQ      Added support for FEATURE_WMS_CM_NAS_INTERFACES.
01/30/07   HQ      Cleaned up some Lint errors.
01/17/07   HQ      Updated copyright info to include 2007.
10/17/06   PMD     Fixed Lint Warning by initializing i to a known value
09/27/06   PMD     Changed TASKLOCK and TASKFREE to Critical Sections
                   Removed featurization for bc_crit_sect
09/25/06   PMD     Eliminated static nv_item_type for preventing overwrites
07/31/06   PMD     Removed unused feature - BCSMS_FIXED_SRV
04/24/06   HQ      Fixed lint errors.
03/27/06   PMD     BC MM Event Information Change.
01/25/06   VK      Updated Copyright Info
12/21/05   VK      Lint fixes
11/18/05   VK      Cleaned up Lint warnings. 
10/20/05   VK      Changed byte = uint8, word = uint16, dword = uint32
10/20/05   VK      Lint error/warning fixes
10/20/05   VK      Updated Copyright info.
10/19/05   PMD     Added Function Headers and comments
10/05/05   PMD     Fix for opt_ptr crash
09/20/05   PMD     Commented out GW CB Language Filtering.
08/17/05   PMD     Fixed bc_s_ptr->gw_cb_num_active checking Logic.
08/04/05   PMD     Fixed certain Lint Warnings in code.
07/17/05   PMD     Added wms_bc_gw_cb_pre_init function.
07/08/05   Rex     Fixed Compile Warnings
06/29/05   PMD     Fixed bug in wms_bc_mm_change_srv_info_proc function.
03/25/05   PMD     Added Debug Messages while Sending Commands to CB
03/10/05   PMD     Added support for wms_bc_mm_msg_delete_all_indication
03/04/05   PMD     Fixed gw_cb_service_ptr Lint Warning.
02/20/05   PMD     Send Event when BC Set Pref has no change in Pref.
02/11/05   PMD     Support for UMTS BMC Reading Pref NV Items
02/01/05   PMD     Added support for wms_bc_mm_set/get_reading_preferences API
01/24/05   PMD     Checked Reading of BC Service Table to Eliminate Bogus Entries.
01/10/05   Rex     Code cleanup for WMS decoupling from NAS module and
                   using CM, featurize as FEATURE_APIONE
12/03/04   Rex     Fixed memory alocation for wms_bc_mm_srv_id_type
11/16/04   HQ      Use sim_cbmid_list_size in wms_bc_gw_cb_enable_to_nas().
10/04/04   PMD     Added support for CBMID
09/30/04   PMD     Fixes to default GW CB Config, Pref and Table Size
07/30/04   HQ      Removed ERR_FATALs.
07/15/04   PMD     Added Support for Static Search List Request to CB
07/15/04   PMD     Added support for CB Page Deletion Indication
07/06/04   PMD     Changed Design to Support Selected Entries in NV only
07/06/04   PMD     Added Debug Message to indicate exact cause of Failure
06/24/04   PMD     Fix for resyncing with CBMI and CBMIR data on SIM incase of Failure.
06/22/04   VK      Fixed BC bug - "Activate Broadcast messages" was not working.
06/08/04   PMD     Fix for informing CB about the Updated Search List
06/04/04   Rex     Fixed delete all services to write the right nv item
06/02/04   HQ      Changed class to msg_class to fix C++ compilation issue.
05/27/04   Rex     Added support for event WMS_SEARCH_REQUEST
04/08/04   PMD     Added changes to synchronize NV and SIM Tables inspite of SIM Failures
03/24/04   PMD     Fixed Compile Warnings
02/22/04   PMD     Changed handling of SIM and NV GW Broadcast Tables during power up
02/12/04   PMD     Added check if CBMI, CBMIR is supported
02/04/04   PMD     Cleaned up Lint Warnings in code
12/15/03   PMD     Added 1x Broadcast Support in Multimode Broadcast API
10/20/03   PMD     Fixed Search Table Problem for Deleting a Service
                   Fixed some more compile warnings
10/15/03   HQ      Fixed compiler warnings.
09/05/03   HQ      Added interfaces to NAS for GW CB.
09/07/03   PMD     Added Support for Debugging Information (bc_event and
                   bc_mm_event) String Display
07/29/03   HQ      Added support for FEATURE_BROADCAST_SMS_MULTIMODE:
                      - New BC_MM commands, events and structures.
                      - New BC_MM API functions.
                      - GSM CB support.
                      - 1x BC SMS is still supported via BC group, not BC_MM.
07/26/03   PMD     Changes to suppress / remove Lint Errors
06/19/03   AP      Fixed deletion of all services in NV.
06/12/03   AP      Added delete all operation for SCPT.
05/22/03   HQ      Make scpt_data and scpt_result to be pointers.
05/12/03   HQ      Set opt_ptr when free BC entry is not found but the table
                   size is increased;
                   write table size to NV during SCPT Add.
05/12/03   HQ      Removed WMS_BC_EVENT_SRV_ADDED_SCPT and
                   WMS_BC_EVENT_SRV_DELETED_SCPT.
04/26/03   HQ/AP   Use WMS_CLIENT_TYPE_INTERNAL instead of DUMMY.
04/21/03   HQ      Use WMS_MAX_NV_CDMA_MESSAGES and WMS_MAX_NV_GW_MESSAGES to
                   replace WMS_MAX_NV_MESSAGES.
04/16/03   HQ      Fixed a typo for opt_ptr.
04/02/03   HQ      Fixed table options fields for service info and
                   service table events.
01/15/03   HQ      Added support for FEATURE_CDSMS_IS637B_BROADCAST_SCPT.
11/19/02   fc      Added support of partial broadcast address match for
                   Universal Page Message.
11/13/02   AP      When adding a BC srv, update table size to NV if incremented;
                   when deleting a BC srv, set selected to FALSE;
                   when making srv id event, set the srv id list properly;
                   check bc_enable when adding/deleting BC srv.
11/09/02   HQ      Added new BC SMS commands: Delete All Services and
                   Set Priority For All.
06/15/02   HQ      Added support of FEATURE_BROADCAST_SMS.
03/09/02   HQ      Added support of Multimode WMS - added FEATURE_CDSMS.
===========================================================================*/


/*===========================================================================
========================  INCLUDE FILES =====================================
===========================================================================*/

#include "comdef.h"

#if defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS)


#include "wmsbc.h"
#include "wmscl.h"
#include "wmsdbg.h"
#include "nv.h"
#include "err.h"
#include "wmsnv.h"
#include "string.h"

#ifdef FEATURE_CDSMS_BROADCAST
#include "wmscm.h"
#endif /* FEATURE_CDSMS_BROADCAST */
#include "wmsmsg.h"
#include "tmc.h"

#ifdef FEATURE_IS2000_REL_A_CC_BCSMS
#include "cai.h"
#include "caii.h"
#endif /* FEATURE_IS2000_REL_A_CC_BCSMS */

#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
#include "wmsts.h"
#include "wmscfg.h"
#include "wmssim.h"
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */

#ifdef FEATURE_APPS_POWER_COLLAPSE
#error code not present
#endif /* FEATURE_APPS_POWER_COLLAPSE */



/*===========================================================================

                         DATA TYPE DECLARATIONS

===========================================================================*/
/* BC data
*/
typedef struct wms_bc_struct
{
  uint32                dummy;

#ifdef FEATURE_CDSMS_BROADCAST
  wms_bc_config_e_type          bc_config;
  wms_bc_pref_e_type            bc_pref;
  uint16                        bc_table_size;
  uint16                        bc_num_active; /* how many srv's active */
  nv_sms_bc_service_table_type  bc_table[WMS_BC_TABLE_MAX];

  uint16                        bc_num_selected; /* how many srv's selected */
  boolean                       bc_enable; /* BC is actually enabled or not */

  nv_sms_bc_service_table_options_type  bc_table_options[WMS_BC_TABLE_MAX];

#endif /* FEATURE_CDSMS_BROADCAST */

#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */

} wms_bc_s_type;

/*===========================================================================
======================== STATIC DATA ========================================
===========================================================================*/


#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
static wms_bc_s_type              *bc_s_ptr;

#ifdef FEATURE_APPS_POWER_COLLAPSE
#error code not present
#endif /* FEATURE_APPS_POWER_COLLAPSE */

#ifdef FEATURE_CDSMS_BROADCAST
static boolean                     bc_enable_bak;
#endif /* FEATURE_CDSMS_BROADCAST */
LOCAL rex_crit_sect_type          bc_crit_sect;
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */

#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
static wms_bc_mm_event_info_s_type   bc_mm_event_info;
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */


/*===========================================================================
============================ FUNCTIONS ======================================
===========================================================================*/

/*===========================================================================
FUNCTION wms_bc_s_ptr

DESCRIPTION
  Return the pointer to the BC data.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to the internal BC data.

SIDE EFFECTS
  None.
===========================================================================*/
wms_bc_s_type* wms_bc_s_ptr
(
  void
)
{
  static wms_bc_s_type  bc;

  return &bc;

} /* wms_bc_s_ptr() */

#ifdef FEATURE_CDSMS_BROADCAST
/*===========================================================================
FUNCTION   wms_bc_enable_check

DESCRIPTION
  Check whether we need to enable or disable BC SMS

DEPENDENCIES
  None

RETURNS
  boolean: TRUE - enable; FALSE - disable

SIDE EFFECTS
  None
===========================================================================*/
boolean wms_bc_enable_check
(
  void
)
{
  boolean  result = FALSE;

  MSG_MED("BC CONF=%d", bc_s_ptr->bc_config, 0,0);
  MSG_MED("BC PREF=%d", bc_s_ptr->bc_pref, 0,0);
  MSG_MED("BC num_sel=%d", bc_s_ptr->bc_num_selected, 0,0);

  if( bc_s_ptr->bc_config == WMS_BC_CONFIG_DISALLOW )
  {
    /* service provider disabled BC SMS
    */
    result = FALSE;
  }
  else if( bc_s_ptr->bc_pref == WMS_BC_PREF_DEACTIVATE )
  {
    /* user disabled BC SMS
    */
    result = FALSE;
  }
  else if( bc_s_ptr->bc_config == WMS_BC_CONFIG_ALLOW_ALL &&
           bc_s_ptr->bc_pref   == WMS_BC_PREF_ACTIVATE_ALL )
  {
    /* service provider enabled wild-card and the user activated it too,
       so there is no need to check the table entries
    */
    result = TRUE;
  }
  else if( bc_s_ptr->bc_config == WMS_BC_CONFIG_ALLOW_TABLE ||
           bc_s_ptr->bc_pref   == WMS_BC_PREF_ACTIVATE_TABLE )
  {
    result = ( bc_s_ptr->bc_num_selected == 0 ) ? FALSE : TRUE;
  }


  /* done */
  return result;

} /* wms_bc_enable_check */


/*===========================================================================
FUNCTION   wms_bc_search_table_entry

DESCRIPTION
  Search the Broadcast Table Entry within the Broadcast Table

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
static void wms_bc_search_table_entry
(
  const wms_bc_service_id_s_type   *srv_id,
  boolean                    *found_free,
  boolean                    *found_dup,
  uint32                     *index,
  nv_sms_bc_service_table_type **service_ptr,
  nv_sms_bc_service_table_options_type **opt_ptr
)
{
  uint32 i;
  *service_ptr = NULL;
  *found_free = FALSE;
  *found_dup  = FALSE;

  /* search the table for a free entry
  */
  for(i=0; i<bc_s_ptr->bc_table_size; i++)
  {
    if( bc_s_ptr->bc_table[i].active_service == TRUE &&
        bc_s_ptr->bc_table[i].service == (uint8)srv_id->service &&
        bc_s_ptr->bc_table[i].language == (uint8)srv_id->language )
    {
      * found_dup = TRUE;
      * index = i;
      * service_ptr = & bc_s_ptr->bc_table[i];
      * opt_ptr = & bc_s_ptr->bc_table_options[i];
      break;
    }
    else if(bc_s_ptr->bc_table[i].active_service == FALSE )
    {
      * found_free = TRUE;
      * index = i;
      * service_ptr = & bc_s_ptr->bc_table[i];
      * opt_ptr = & bc_s_ptr->bc_table_options[i];
      MSG_MED( "BC: found free entry %d",i,0,0);
    }
  }

  /* done */
  return;

} /* wms_bc_search_table_entry() */

/*===========================================================================
FUNCTION   wms_bc_scpt_data

DESCRIPTION
  Send an internal command to WMS task to process the Broadcast SCPT data

DEPENDENCIES
  None

RETURNS
  wms_status_e_type

SIDE EFFECTS
  None
===========================================================================*/
/* Send an internal command to WMS task
*/
wms_status_e_type wms_bc_scpt_data
(
  const wms_tl_message_type * tl_ptr
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_ptr = wms_get_cmd_buf();

  if( cmd_ptr != NULL)
  {
    cmd_ptr->hdr.cmd_id      = WMS_CMD_BC_SCPT_DATA;
    cmd_ptr->hdr.client_id   = (uint8)WMS_CLIENT_TYPE_INTERNAL;
    cmd_ptr->hdr.cmd_cb      = NULL;
    cmd_ptr->hdr.user_data   = NULL;

    cmd_ptr->cmd.bc_scpt_data.cdma_tl = * tl_ptr;

    wms_put_cmd( cmd_ptr );
  }
  else
  {
    st = WMS_OUT_OF_RESOURCES_S;
  }

  return st;

} /* wms_bc_scpt_data() */

#endif /* FEATURE_CDSMS_BROADCAST */

#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
/*===========================================================================
FUNCTION wms_bc_mm_filter_events

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
boolean wms_bc_mm_filter_events
(
  wms_client_id_type       client_id,
  wms_client_s_type        * client_ptr,
  wms_bc_mm_event_e_type     event
)
{
#ifdef FEATURE_APPS_POWER_COLLAPSE
#error code not present
#else /* FEATURE_APPS_POWER_COLLAPSE */
  return TRUE;
#endif /* FEATURE_APPS_POWER_COLLAPSE */

} /* wms_bc_mm_filter_events() */


/*===========================================================================
FUNCTION wms_bc_mm_event_notify

DESCRIPTION
  Notify all clients of a BC event.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_bc_mm_event_notify
(
  wms_bc_mm_event_e_type           event,
  wms_bc_mm_event_info_s_type   *bc_mm_event_info_ptr
)
{
  wms_client_s_type           *client_ptr;
  uint8                       i;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
#ifdef FEATURE_WMS_DEBUG_STRING
    MSG_SPRINTF_2(MSG_SSID_DFLT, MSG_LEGACY_HIGH, "Notify: %d %s", event, wms_dbg_get_bc_mm_event_string(event));
#else
    MSG_HIGH("wms_bc_mm_event_notify: %d ", event, 0, 0);
#endif /* FEATURE_WMS_DEBUG_STRING */

  /* Assigning Event Information in bc_mm_event_info_ptr */
  bc_mm_event_info_ptr->event = event;

  /* Notify clients
  */
  for( i=0; i<WMS_CLIENT_TYPE_MAX; i++ )
  {
    client_ptr = wms_client_ptr( (wms_client_id_type) i );
    if( client_ptr != NULL &&
        client_ptr->activated == TRUE &&
        client_ptr->bc_mm_event_cb )
    {
      if( TRUE == wms_bc_mm_filter_events( (wms_client_id_type) i, client_ptr, event ))
      {
        client_ptr->bc_mm_event_cb( event, bc_mm_event_info_ptr );
      }
    }
  }

  /* done */
  return;

} /* wms_bc_mm_event_notify() */
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */

#ifdef FEATURE_CDSMS_BROADCAST
/*=========================================================================
FUNCTION
  wms_bc_enable_failure_proc

DESCRIPTION
  Handle the BC Enable Failure and Notify Clients

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Broadcast Event is sent to all clients

=========================================================================*/
void wms_bc_enable_failure_proc
(
  wms_cmd_type    *cmd_ptr
)
{
  MSG_HIGH("WMS_BC_MM_ENABLE_FAILURE_E", 0, 0, 0);

  wms_bc_mm_event_notify( (wms_bc_mm_event_e_type)
                            ( bc_s_ptr->bc_enable ?
                              WMS_BC_MM_EVENT_ENABLE_FAILURE :
                              WMS_BC_MM_EVENT_DISABLE_FAILURE ),
                          & bc_mm_event_info );

  /* done */
  return;

} /* wms_bc_enable_failure_proc() */

/*=========================================================================
FUNCTION
  wms_bc_scpt_data_proc

DESCRIPTION
  Process internal SCPT data command

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Broadcast Event is sent to all clients

=========================================================================*/
/* Process internal SCPT data command
*/
static void wms_bc_scpt_data_proc
(
  wms_cmd_type    *cmd_ptr
)
{
  uint32                tbl_index, scpt_index;
  nv_item_type          nvi;
  wms_bc_scpt_status_e_type   scpt_status;
  boolean found_free = FALSE;
  boolean found_dup  = FALSE;
  nv_sms_bc_service_table_type           * service_ptr = NULL;
  nv_sms_bc_service_table_options_type   * opt_ptr = NULL;
  wms_tl_message_type           * tl_ptr = & cmd_ptr->cmd.bc_scpt_data.cdma_tl;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_HIGH("WMS_CMD_BC_SCPT_DATA", 0, 0, 0);

  /* ----------------------------------------------------------- */
  /* ----------------------------------------------------------- */
  /* ------------- Now process the SCPT data ------------------- */
  /* ------------------------------------------------------------*/
  /* ----------------------------------------------------------- */

  for( scpt_index = 0;
       scpt_index < tl_ptr->cl_bd.scpt_data_ptr->num_entries;
       scpt_index ++ )
  {
    scpt_status = WMS_BC_SCPT_STATUS_OK;

    switch( tl_ptr->cl_bd.scpt_data_ptr->entry[scpt_index].op_code )
    {
      /* --------------- */
      /* --- OP_ADD ---- */
      /* --------------- */
      case WMS_BC_SCPT_OP_ADD:
        if( bc_s_ptr->bc_num_active == WMS_BC_TABLE_MAX )
        {
          /* table is full
          */
          scpt_status = WMS_BC_SCPT_STATUS_SERVICE_MEMORY_FULL;
        }
        else
        {
          wms_bc_search_table_entry
            (
              & tl_ptr->cl_bd.scpt_data_ptr->entry[scpt_index].srv_id,
              & found_free,
              & found_dup,
              & tbl_index,
              & service_ptr,
              & opt_ptr
            );

          if( found_dup == TRUE )
          {
            scpt_status = WMS_BC_SCPT_STATUS_ALREADY_PROGRAMMED;
          }
          else
          {
            if( found_free == FALSE )
            {
              /* there is no free entry within the current table;
                 use the free entry just after the current table
              */
              tbl_index = bc_s_ptr->bc_table_size;
              service_ptr = & bc_s_ptr->bc_table[tbl_index];
              opt_ptr     = & bc_s_ptr->bc_table_options[tbl_index];

              /* increase the table size
              */
              bc_s_ptr->bc_table_size ++;
            }
            /* else, use the free entry found earlier */


            /* update the entry and bc info
            */
            service_ptr->index          = (uint8)tbl_index;
            service_ptr->active_service = TRUE;
            service_ptr->service = (uint16)
                     tl_ptr->cl_bd.scpt_data_ptr->entry[scpt_index].srv_id.service;
            service_ptr->language = (uint8)
                     tl_ptr->cl_bd.scpt_data_ptr->entry[scpt_index].srv_id.language;
            service_ptr->selected = FALSE; /* by default, disable this service*/
            service_ptr->priority = (uint8)WMS_PRIORITY_NORMAL;
            memcpy( (char*) service_ptr->label,
                    tl_ptr->cl_bd.scpt_data_ptr->entry[scpt_index].label,
                    WMS_BC_MM_SRV_LABEL_SIZE );

            opt_ptr->index = service_ptr->index;
            opt_ptr->label_encoding = (uint8)
                             tl_ptr->cl_bd.scpt_data_ptr->encoding;
            opt_ptr->bc_alert =
                             (uint8)tl_ptr->cl_bd.scpt_data_ptr->entry[scpt_index].bc_alert;
            opt_ptr->max_messages =
                        tl_ptr->cl_bd.scpt_data_ptr->entry[scpt_index].max_messages;

            /* We have one more active entry
            */
            bc_s_ptr->bc_num_active ++;


            /* write to NV
            */
            nvi.sms_bc_service_table = * service_ptr;
            if( wms_nv_write_wait( NV_SMS_BC_SERVICE_TABLE_I, & nvi )
                != NV_DONE_S )
            {
              scpt_status = WMS_BC_SCPT_STATUS_UNSPECIFIED_ERROR;
            }
            else
            {
              nvi.sms_bc_service_table_options = * opt_ptr;
              if( wms_nv_write_wait( NV_SMS_BC_SERVICE_TABLE_OPTIONS_I,
                                     & nvi ) != NV_DONE_S )
              {
                scpt_status = WMS_BC_SCPT_STATUS_UNSPECIFIED_ERROR;
              }
              else
              {
                nvi.sms_bc_service_table_size = bc_s_ptr->bc_table_size;
                if( wms_nv_write_wait( NV_SMS_BC_SERVICE_TABLE_SIZE_I, & nvi )
                    != NV_DONE_S )
                {
                  scpt_status = WMS_BC_SCPT_STATUS_UNSPECIFIED_ERROR;
                }
                else
                {
                  /* SCPT data is Ok, send the event to client
                  */
                  bc_mm_event_info.event = WMS_BC_MM_EVENT_SRV_UPDATED;
                  bc_mm_event_info.message_mode = WMS_MESSAGE_MODE_CDMA;
                  
                  bc_mm_event_info.u.bc_mm_srv_info.srv_id.bc_srv_id.service
                                = (wms_service_e_type) service_ptr->service;
                  bc_mm_event_info.u.bc_mm_srv_info.srv_id.bc_srv_id.language
                                = (wms_language_e_type) service_ptr->language;
                  bc_mm_event_info.u.bc_mm_srv_info.selected
                                = service_ptr->selected;
                  bc_mm_event_info.u.bc_mm_srv_info.priority
                                = (wms_priority_e_type) service_ptr->priority;
                  memcpy( bc_mm_event_info.u.bc_mm_srv_info.label,
                          (char*) service_ptr->label,
                          WMS_BC_MM_SRV_LABEL_SIZE );

                  bc_mm_event_info.u.bc_mm_srv_info.label_encoding
                     = (wms_user_data_encoding_e_type) opt_ptr->label_encoding;
                  bc_mm_event_info.u.bc_mm_srv_info.alert
                     = (wms_bc_alert_e_type)opt_ptr->bc_alert;
                  bc_mm_event_info.u.bc_mm_srv_info.max_messages  
                     = opt_ptr->max_messages;

                  wms_bc_mm_event_notify ( WMS_BC_MM_EVENT_SRV_UPDATED,
                                           & bc_mm_event_info );
                }
              }
            }
          }
        }
        break;



      /* --------------- */
      /* -- OP_DELETE -- */
      /* --------------- */
      case WMS_BC_SCPT_OP_DELETE:
        wms_bc_search_table_entry
            (
              & tl_ptr->cl_bd.scpt_data_ptr->entry[scpt_index].srv_id,
              & found_free,
              & found_dup,
              & tbl_index,
              & service_ptr,
              & opt_ptr
            );

        if( found_dup == FALSE )
        {
          scpt_status = WMS_BC_SCPT_STATUS_NOT_YET_PROGRAMMED;
        }
        else
        {
          /* de-activate this entry
          */
          service_ptr->active_service = FALSE;

          /* we have one less active entry
          */
          bc_s_ptr->bc_num_active --;

          /* we may have one less selected entry
          */
          if( service_ptr->selected == TRUE )
          {
            bc_s_ptr->bc_num_selected --;
            service_ptr->selected = FALSE;
          }

          /* write to NV
          */
          nvi.sms_bc_service_table = * service_ptr;
          if( wms_nv_write_wait( NV_SMS_BC_SERVICE_TABLE_I, & nvi )
              != NV_DONE_S )
          {
            scpt_status = WMS_BC_SCPT_STATUS_UNSPECIFIED_ERROR;
          }
          else
          {
            rex_enter_crit_sect(&bc_crit_sect);

            bc_enable_bak = bc_s_ptr->bc_enable;
            bc_s_ptr->bc_enable = wms_bc_enable_check();

            rex_leave_crit_sect(&bc_crit_sect);

            /* if bc_enable flag has changed, notify CM */
            if( bc_enable_bak != bc_s_ptr->bc_enable )
            {
              MSG_HIGH("BCSMS enable=%d", bc_s_ptr->bc_enable,0,0);
              wms_cm_bc_enable( bc_s_ptr->bc_enable );
            }

            /* Delete is Ok, send the event to client
            */
            bc_mm_event_info.event = WMS_BC_MM_EVENT_DELETE_SRVS;
            bc_mm_event_info.message_mode = WMS_MESSAGE_MODE_CDMA;

            /* Allocate Memory */
            bc_mm_event_info.u.bc_mm_srv_ids.size = 1;
            bc_mm_event_info.u.bc_mm_srv_ids.entries 
                 = mem_malloc( &tmc_heap, 
                               bc_mm_event_info.u.bc_mm_srv_ids.size 
                               * sizeof(wms_bc_mm_srv_id_type));

            if(NULL == bc_mm_event_info.u.bc_mm_srv_ids.entries )
            {
              MSG_ERROR("Memory Allocation failed!",0,0,0);
            }
            else
            {
              bc_mm_event_info.u.bc_mm_srv_ids.entries[0].bc_srv_id.service
                               = (wms_service_e_type) service_ptr->service;
              bc_mm_event_info.u.bc_mm_srv_ids.entries[0].bc_srv_id.language
                               = (wms_language_e_type) service_ptr->language;

              wms_bc_mm_event_notify ( WMS_BC_MM_EVENT_DELETE_SRVS, 
                                     & bc_mm_event_info );

              /* Free Memory */
              mem_free( &tmc_heap, bc_mm_event_info.u.bc_mm_srv_ids.entries );
            }
          }
        }
        break;

      /* ------------------ */
      /* -- OP_DELETE_ALL-- */
      /* ------------------ */
      case WMS_BC_SCPT_OP_DELETE_ALL:
        {
          uint32 i;

          /* delete all entries from table
          */
          for( i=0; i < bc_s_ptr->bc_table_size; i++ )
          {
            service_ptr = & bc_s_ptr->bc_table[i];

            /* If this table entry is active, delete it
            */
            if( service_ptr->active_service == TRUE )
            {
              /* de-activate this entry
              */
              service_ptr->active_service = FALSE;
              service_ptr->selected       = FALSE;

              /* write to NV
              */
              nvi.sms_bc_service_table = * service_ptr;
              if( wms_nv_write_wait( NV_SMS_BC_SERVICE_TABLE_I, & nvi )
                  != NV_DONE_S )
              {
                MSG_ERROR("BC Table NV write failed!", 0,0,0);
                scpt_status = WMS_BC_SCPT_STATUS_UNSPECIFIED_ERROR;
              }
            }
          }  // for

          bc_s_ptr->bc_num_active   = 0;
          bc_s_ptr->bc_num_selected = 0;

          if( scpt_status == WMS_BC_SCPT_STATUS_OK )
          {
            /* Delete all was successful
            */
            MSG_MED("BC Table Delete All OK", 0,0,0);

            /* update local data from new NV item
            */
            rex_enter_crit_sect(&bc_crit_sect);

            bc_enable_bak = bc_s_ptr->bc_enable;
            bc_s_ptr->bc_enable = wms_bc_enable_check();

            rex_leave_crit_sect(&bc_crit_sect);

            /* if bc_enable flag has changed, notify CM */
            if( bc_enable_bak != bc_s_ptr->bc_enable )
            {
              MSG_HIGH("BCSMS enable=%d", bc_s_ptr->bc_enable,0,0);
              wms_cm_bc_enable( bc_s_ptr->bc_enable );
            }

            /* Send the event to client
            */
            bc_mm_event_info.event = WMS_BC_MM_EVENT_DELETE_ALL_SRVS;
            bc_mm_event_info.message_mode = WMS_MESSAGE_MODE_CDMA;

            /* no additional event data needed */
            wms_bc_mm_event_notify( WMS_BC_MM_EVENT_DELETE_ALL_SRVS, 
                                    & bc_mm_event_info );
          }
        }
        break;


      default:
        /* Bad op code; skip this entry */
        scpt_status = WMS_BC_SCPT_STATUS_UNSPECIFIED_ERROR;
        break;

    } /* switch */

    /* update SCPT result
    */
    tl_ptr->cl_bd.scpt_result_ptr->entry[scpt_index].srv_id =
                            tl_ptr->cl_bd.scpt_data_ptr->entry[scpt_index].srv_id;
    tl_ptr->cl_bd.scpt_result_ptr->entry[scpt_index].status = scpt_status;

  } /* for */


  /* ----------------------------------------------------------- */
  /* ----------------------------------------------------------- */
  /* ------------- Send SCPT result to network ----------------- */
  /* ------------------------------------------------------------*/
  /* ----------------------------------------------------------- */

  tl_ptr->cl_bd.mask = WMS_MASK_BD_MSG_ID | WMS_MASK_BD_SCPT_RESULT;

  tl_ptr->cl_bd.message_id.type = WMS_BD_TYPE_SUBMIT;
  /* use the same message id number as in the incoming message */

  tl_ptr->cl_bd.scpt_result_ptr->num_entries = tl_ptr->cl_bd.scpt_data_ptr->num_entries;
  /* results for each operations were filled in already */

  /* Send the result to the network
  */
  wms_msg_send_scpt_result( cmd_ptr, tl_ptr );

  /* done */
  return;

} /* wms_bc_scpt_data_proc() */

/*===========================================================================
FUNCTION wms_bc_init

DESCRIPTION
  Do BC-related initialization.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  BC data initialized.
===========================================================================*/
void wms_bc_init
(
  void
)
{

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  uint16       i;
  nv_item_type nvi;

  MSG_HIGH("wms_bc_init()",0,0,0);

  bc_s_ptr  = wms_bc_s_ptr();

#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */

  /* --------------- read all BC SMS items from NV ---------------- */

  /* NV_SMS_BC_CONFIG_I:
  */
  (void) wms_nv_read_wait( NV_SMS_BC_CONFIG_I, &nvi );
  bc_s_ptr->bc_config = (wms_bc_config_e_type) nvi.sms_bc_config;

  if( bc_s_ptr->bc_config == WMS_BC_CONFIG_DISALLOW )
  {
    MSG_HIGH("BC SMS disabled in NV!", 0,0,0);

    bc_s_ptr->bc_pref      = WMS_BC_PREF_DEACTIVATE;
    nvi.sms_bc_user_pref = (uint8)WMS_BC_PREF_DEACTIVATE;
    (void)wms_nv_write_wait( NV_SMS_BC_USER_PREF_I, & nvi );

    bc_s_ptr->bc_table_size = 0;
    nvi.sms_bc_service_table_size = 0;
    (void)wms_nv_write_wait( NV_SMS_BC_SERVICE_TABLE_SIZE_I, & nvi );

    return;  /* SHORT-RETURN */
  }


  /* NV_SMS_BC_USER_PREF_I:
  */
  (void) wms_nv_read_wait( NV_SMS_BC_USER_PREF_I, &nvi );
  bc_s_ptr->bc_pref = (wms_bc_pref_e_type) nvi.sms_bc_user_pref;


  /* ----------- Do some checking on the preference ----------- */
  if( bc_s_ptr->bc_config == WMS_BC_CONFIG_ALLOW_TABLE &&
      bc_s_ptr->bc_pref   == WMS_BC_PREF_ACTIVATE_ALL )
  {
    MSG_ERROR("Enforcing BC Pref to TableOnly",0,0,0);
    bc_s_ptr->bc_pref      = WMS_BC_PREF_ACTIVATE_TABLE;
    nvi.sms_bc_user_pref = (uint8)WMS_BC_PREF_ACTIVATE_TABLE;
    (void)wms_nv_write_wait( NV_SMS_BC_USER_PREF_I, & nvi );
  }


  /* NV_SMS_BC_SERVICE_TABLE_SIZE_I:
  */
  (void) wms_nv_read_wait( NV_SMS_BC_SERVICE_TABLE_SIZE_I, &nvi );
  bc_s_ptr->bc_table_size = nvi.sms_bc_service_table_size;


  /* determine how many services are currently selected while traversing
     the whole table:
  */
  bc_s_ptr->bc_num_selected = 0;

  for( i=0; i<bc_s_ptr->bc_table_size && i<WMS_BC_TABLE_MAX; i++ )
  {
    nvi.sms_bc_service_table.index = (uint8)i;
    (void) wms_nv_read_wait( NV_SMS_BC_SERVICE_TABLE_I, &nvi );

    bc_s_ptr->bc_table[i] = nvi.sms_bc_service_table;

    if( bc_s_ptr->bc_table[i].active_service == TRUE &&
        bc_s_ptr->bc_table[i].selected == TRUE )
    {
      bc_s_ptr->bc_num_selected ++;
    }
  }

  rex_init_crit_sect(&bc_crit_sect);

  /* done */
  return;

} /* wms_bc_init */

void wms_bc_online_proc
(
  void
)
{
  /* ---- send BC enable command to lower layers ---- */
  bc_s_ptr->bc_enable = wms_bc_enable_check();

  MSG_HIGH("BCSMS enable=%d", bc_s_ptr->bc_enable,0,0);

  wms_cm_bc_enable( bc_s_ptr->bc_enable );

  /* done */
  return;

} /* wms_bc_online_proc() */


/*=========================================================================
FUNCTION
  wms_bc_addr_match

DESCRIPTION
  This function checks the BC_ADDR of a received broadcast page or
  broadcast message to see if it passes the filter.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - the mobile should process the received message
  FALSE - the mobile should discard the received message

SIDE EFFECTS
  None

=========================================================================*/
boolean wms_bc_addr_match
(
  const uint8 *bc_addr_ptr
)
{
  boolean   result = FALSE;
  uint16    i;

  /* Decode the data first.  See TIA/EIA-637A 3.3.1.
       byte0: | priority(2) | msg_id(6) |
       byte1: | zone_id(8)              |
       byte2: | service (16)            |
       byte3: |                         |
       byte4: | language(8)             |
  */
  wms_priority_e_type  priority =
            (wms_priority_e_type) ( ( bc_addr_ptr[0] & 0xc0 ) >> 6 );
  uint8                   msg_id   = ( bc_addr_ptr[0] & 0x3f );
  uint8                   zone_id  = bc_addr_ptr[1];
  wms_service_e_type   service  =
            (wms_service_e_type) ( ( bc_addr_ptr[2] << 8 ) | bc_addr_ptr[3] );
  wms_language_e_type  language =
            (wms_language_e_type) bc_addr_ptr[4];

  MSG_LOW("----BC_ADDR----", 0,0,0);
  MSG_LOW("BC srv=0x%lx",  service,  0,0);
  MSG_LOW("BC lan=%d",     language, 0,0);
  MSG_LOW("BC pri=%d",     priority, 0,0);
  MSG_LOW("BC msg_id=%d",  msg_id,   0,0);
  MSG_LOW("BC zone_id=%d", zone_id,  0,0);

  /* The following two lines are for removing compiler warnings, since these two
  ** variables are not used for now.
  */
  i = zone_id;
  i = msg_id;


  /* --------- Now do the filtering --------- */

  if( bc_s_ptr->bc_enable == FALSE )
  {
    result = FALSE;
  }
  else if( bc_s_ptr->bc_pref == WMS_BC_PREF_ACTIVATE_ALL )
  {
    result = TRUE;
  }
  else
  {
    /* Now we need to check all table entries:
    */
    for( i=0; i < bc_s_ptr->bc_table_size && i < WMS_BC_TABLE_MAX; i++ )
    {
      if( bc_s_ptr->bc_table[i].active_service      == TRUE      &&
          bc_s_ptr->bc_table[i].selected            == TRUE      &&
          bc_s_ptr->bc_table[i].service             == (word)service   &&
          bc_s_ptr->bc_table[i].language            == (uint8)language  &&
          bc_s_ptr->bc_table[i].priority            <= (uint8)priority )
      {
        result = TRUE;
        break;
      }
    }
  }

  if( result == TRUE )
  {
    MSG_HIGH("bc_addr MATCHED!", 0,0,0);
  }
  else
  {
    MSG_HIGH("bc_addr NO match :(", 0,0,0);
  }


  /* done with filtering */
  return result;

} /* wms_bc_addr_match */

#ifdef FEATURE_IS2000_REL_A_CC_BCSMS
/*=========================================================================
FUNCTION
  wms_bc_partial_addr_match

DESCRIPTION
  This function checks the partial broadcast address of received Universal
  Page message to see if it passes the filter.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void wms_bc_partial_addr_match
(
  caii_univ_page_interleaved_addr_type *addr_ptr
)
{
  uint16   service, shift_bits;
  uint16 i, j;

  /* --------- Now do the filtering --------- */

  if( !bc_s_ptr->bc_enable || !addr_ptr->bcast_included)
  {
    addr_ptr->bcast_match = FALSE;
  }
  else if( bc_s_ptr->bc_pref == WMS_BC_PREF_ACTIVATE_ALL )
  {
    addr_ptr->bcast_match = TRUE;
  }
  else if (addr_ptr->bcast_bit_matched < addr_ptr->iaddr_portion_rxed)
  {
    /* First, assume no match for all broadcast partial address */
    addr_ptr->bcast_match = FALSE;

    /* Figure out the number of broadcast partial address bits to be ignored */
    shift_bits = CAI_UNIV_PAGE_INTERLEAVED_ADDR_PORTION -
                 addr_ptr->iaddr_portion_rxed;

    /* Now we need to check all table entries */
    for( i=0; i < bc_s_ptr->bc_table_size && i < WMS_BC_TABLE_MAX; i++ )
    {
      if (bc_s_ptr->bc_table[i].active_service &&
          bc_s_ptr->bc_table[i].selected)
      {
        for (j=0; j<=addr_ptr->num_bcast; j++)
        {
          if (addr_ptr->burst_type[j] == CAI_SHORT_MSG_SERVICES)
          {
            service = (uint16) ((addr_ptr->bc_addr[j][0] << 8) |
                              addr_ptr->bc_addr[j][1]);
            if (((bc_s_ptr->bc_table[i].service ^ service) << shift_bits) == 0)

            {
              /* One of the broadcast partial address match */
              addr_ptr->bcast_match = TRUE;
              break;
            }
          }
        }
        if (addr_ptr->bcast_match)
        {
          /* Stop partial address match as soon as one match is found */
          break;
        }
      }
    }

    /* Record the number of broadcast partial address bits matched */
    addr_ptr->bcast_bit_matched = addr_ptr->iaddr_portion_rxed;
  }

} /* wsms_bc_partial_addr_match */
#endif /* FEATURE_IS2000_REL_A_CC_BCSMS */

#endif /* FEATURE_CDSMS_BROADCAST */

/*===========================================================================
 *===========================================================================
 *===========================================================================
 *===========================================================================

===========================================================================*
===========================================================================*
===========================================================================*
===========================================================================*/

#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
/*===========================================================================
FUNCTION wms_bc_mm_get_active_table_size

DESCRIPTION
  Get the Active Table size for the Broadcast Table based on the mode.

DEPENDENCIES
  none

RETURN VALUE
  Size of the Table

SIDE EFFECTS
  None.
===========================================================================*/
static uint32 wms_bc_mm_get_active_table_size
(
  wms_message_mode_e_type    message_mode
)
{
  uint32 i, size=0;

  if (message_mode == WMS_MESSAGE_MODE_GW)
  {
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */
  }
  else
  {
#ifdef FEATURE_CDSMS_BROADCAST
    for( i=0, size=0; i < bc_s_ptr->bc_table_size; i++ )
    {
      if( bc_s_ptr->bc_table[i].active_service == TRUE )
      {
        size ++;
      }
    }
#endif /* FEATURE_CDSMS_BROADCAST */
  }
  return size;
} /* wms_bc_get_active_table_size() */


/*===========================================================================
FUNCTION   wms_bc_mm_make_table_event

DESCRIPTION
  Use the stored information to make a table event.

DEPENDENCIES
  None

RETURNS
  TRUE:  Successful
  FALSE: Failed

SIDE EFFECTS
  None
===========================================================================*/
static boolean wms_bc_mm_make_table_event
(
  wms_message_mode_e_type    message_mode,
  wms_bc_mm_table_s_type     *output
)
{
  uint32   i, j=0;
  boolean  st = TRUE;

  if( output == NULL )
  {
    MSG_ERROR("Null pointer",0,0,0);
    return FALSE;
  }
  /* Allocate enough memory for the output */
  output->size = wms_bc_mm_get_active_table_size(message_mode);
  output->entries = mem_malloc( &tmc_heap,
                      output->size * sizeof(wms_bc_mm_service_info_s_type) );

  if( output->entries == NULL )
  {
    if (output->size > 0)
    {
      st = FALSE;
    }
  }
  else
  {
    if( message_mode == WMS_MESSAGE_MODE_GW )
    {
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */
    }
    else
    {
#ifdef FEATURE_CDSMS_BROADCAST
      for( i=0, j=0; i < bc_s_ptr->bc_table_size; i++ )
      {
        if( bc_s_ptr->bc_table[i].active_service == TRUE )
        {
          output->entries[j].srv_id.bc_srv_id.service  =
                            (wms_service_e_type) bc_s_ptr->bc_table[i].service;
          output->entries[j].srv_id.bc_srv_id.language =
                            (wms_language_e_type) bc_s_ptr->bc_table[i].language;
          output->entries[j].selected            = bc_s_ptr->bc_table[i].selected;
          output->entries[j].priority            =
                            (wms_priority_e_type) bc_s_ptr->bc_table[i].priority;
          memcpy( output->entries[j].label,
                  (char*) bc_s_ptr->bc_table[i].label,
                  WMS_BC_MM_SRV_LABEL_SIZE );

          {
            nv_sms_bc_service_table_options_type * opt_ptr =
                                                  & bc_s_ptr->bc_table_options[i];
            output->entries[j].label_encoding = (wms_user_data_encoding_e_type)
                                                        opt_ptr->label_encoding;
            output->entries[j].alert          = (wms_bc_alert_e_type)
                                                        opt_ptr->bc_alert;
            output->entries[j].max_messages   = opt_ptr->max_messages;
          }
          j++;
        }
      }
      output->size = j;
#endif /* FEATURE_CDSMS_BROADCAST */
    }
  }
  return st;
} /* wms_bc_mm_make_table_event() */


/* <EJECT> */
/*===========================================================================
FUNCTION   wms_bc_mm_make_srv_ids_event

DESCRIPTION
  Use the stored information to make a srv_ids event.

DEPENDENCIES
  None

RETURNS
  TRUE:  Sucessful
  FALSE: Failed

SIDE EFFECTS
  None
===========================================================================*/
static boolean wms_bc_mm_make_srv_ids_event
(
  wms_message_mode_e_type        message_mode,
  wms_bc_mm_service_ids_s_type   *output
)
{
  uint32   i, j=0;
  boolean  st = TRUE;


  if( output == NULL )
  {
    MSG_ERROR("Null pointer",0,0,0);
    return FALSE;
  }
  /* Allocate enough memory for the output */
  output->size = wms_bc_mm_get_active_table_size(message_mode);
  output->entries = mem_malloc( &tmc_heap,
                      output->size * sizeof(wms_bc_mm_srv_id_type));

  if( output->entries == NULL )
  {
    if (output->size > 0)
    {
      st = FALSE;
    }
  }
  else
  {
    if( message_mode == WMS_MESSAGE_MODE_GW )
    {
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */
    }
    else
    {
#ifdef FEATURE_CDSMS_BROADCAST
      for( i=0, j=0; i < bc_s_ptr->bc_table_size; i++ )
      {
        if( bc_s_ptr->bc_table[i].active_service == TRUE )
        {
          output->entries[output->size].bc_srv_id.service  =
                            (wms_service_e_type) bc_s_ptr->bc_table[i].service;
          output->entries[output->size].bc_srv_id.language =
                            (wms_language_e_type) bc_s_ptr->bc_table[i].language;
          j++;
        }
      }
      output->size = j;
#endif /* FEATURE_CDSMS_BROADCAST */
    }
  }
  return st;
} /* wms_bc_mm_make_srv_ids_event() */

#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */


/* ================================================================= */
/* ================= BC MM Command Processing ====================== */
/* ================================================================= */

/*=========================================================================
FUNCTION
  wms_bc_mm_get_config_proc

DESCRIPTION
  This function is used to retrieve the configuration for
  Multimode Broadcast SMS.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  On Success, Multimode Broadcast Event is sent to all WMS clients

=========================================================================*/
static void wms_bc_mm_get_config_proc
(
  wms_cmd_type  *cmd_ptr
)
{
  MSG_HIGH("WMS_CMD_BC_MM_GET_CONFIG", 0, 0, 0);

  bc_mm_event_info.message_mode = cmd_ptr->cmd.bc_mm_get_config.message_mode;

  /* call the command callback saying no error
  */
  wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_NONE );

  if( cmd_ptr->cmd.bc_mm_get_config.message_mode == WMS_MESSAGE_MODE_GW )
  {
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */
  }
  else
  {
#ifdef FEATURE_CDSMS_BROADCAST
    /* CDMA */
    /* send event to the client
    */
    bc_mm_event_info.u.bc_config = bc_s_ptr->bc_config;

    wms_bc_mm_event_notify( WMS_BC_MM_EVENT_CONFIG, & bc_mm_event_info );
#endif /* FEATURE_CDSMS_BROADCAST */
  }
  /* done */
  return;
} /* wms_bc_mm_get_config_proc() */

/*=========================================================================
FUNCTION
  wms_bc_mm_get_pref_proc

DESCRIPTION
  This function is used to retrieve the user preference for
  Multimode Broadcast SMS.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  On Success, Multimode Broadcast Event is sent to all WMS clients

=========================================================================*/
static void wms_bc_mm_get_pref_proc
(
  wms_cmd_type  *cmd_ptr
)
{
  MSG_HIGH("WMS_CMD_BC_MM_GET_PREF", 0, 0, 0);

  bc_mm_event_info.message_mode = cmd_ptr->cmd.bc_mm_get_pref.message_mode;

  /* call the command callback saying no error
  */
  wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_NONE );


  if( cmd_ptr->cmd.bc_mm_get_pref.message_mode == WMS_MESSAGE_MODE_GW )
  {
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */
  }
  else
  {
#ifdef FEATURE_CDSMS_BROADCAST
    /* CDMA */
    /* send event to the client
    */
    bc_mm_event_info.u.bc_pref = bc_s_ptr->bc_pref;

    wms_bc_mm_event_notify( WMS_BC_MM_EVENT_PREF, & bc_mm_event_info );
#endif /* FEATURE_CDSMS_BROADCAST */
  }
  /* done */
  return;
} /* wms_bc_mm_get_pref_proc() */

/*=========================================================================
FUNCTION
  wms_bc_mm_set_pref_proc

DESCRIPTION
  This function is used to set the user preference for
  Multimode Broadcast SMS.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  On Success, Multimode Broadcast Event is sent to all WMS clients

=========================================================================*/
static void wms_bc_mm_set_pref_proc
(
  wms_cmd_type  *cmd_ptr
)
{
  nv_item_type nvi;

  MSG_HIGH("WMS_CMD_BC_MM_SET_PREF", 0, 0, 0);

  bc_mm_event_info.message_mode = cmd_ptr->cmd.bc_mm_set_pref.message_mode;

  if( cmd_ptr->cmd.bc_mm_set_pref.message_mode == WMS_MESSAGE_MODE_GW )
  {
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */
  }
  else
  {
#ifdef FEATURE_CDSMS_BROADCAST
    /* CDMA */
    /* do error checking
    */
    if( bc_s_ptr->bc_config == WMS_BC_CONFIG_DISALLOW &&
        cmd_ptr->cmd.bc_mm_set_pref.bc_pref != WMS_BC_PREF_DEACTIVATE )
    {
      MSG_HIGH("BC Pref: CANNOT_ACTIVATE",0,0,0);
      wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_BC_CANNOT_ACTIVATE );
    }
    if( bc_s_ptr->bc_config == WMS_BC_CONFIG_ALLOW_TABLE &&
        cmd_ptr->cmd.bc_mm_set_pref.bc_pref == WMS_BC_PREF_ACTIVATE_ALL )
    {
      MSG_HIGH("BC Pref: CANNOT_ACTIVATE_ALL",0,0,0);
      wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_BC_CANNOT_ACTIVATE_ALL );
    }
    else if( cmd_ptr->cmd.bc_mm_set_pref.bc_pref >= WMS_BC_PREF_MAX )
    {
      MSG_HIGH("BC Pref: bad pref",0,0,0);
      wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_BC_BAD_PREF );
    }
    else if( cmd_ptr->cmd.bc_mm_set_pref.bc_pref == bc_s_ptr->bc_pref )
    {
      /* no change on preference */
      /* command is done */
      wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_NONE );

      /* send event to client */
      bc_mm_event_info.u.bc_pref = cmd_ptr->cmd.bc_mm_set_pref.bc_pref;
      wms_bc_mm_event_notify( WMS_BC_MM_EVENT_PREF, & bc_mm_event_info );
    }
    else
    {
      /* -- command parameters are OK; continue processing -- */

      /* update NV first, then send the event to client
      */

      nvi.sms_bc_user_pref = cmd_ptr->cmd.bc_mm_set_pref.bc_pref;

      if( wms_nv_write_wait( NV_SMS_BC_USER_PREF_I, & nvi )
          != NV_DONE_S )
      {
        MSG_ERROR("BC PREF NV write failed!", 0,0,0);
        wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_BC_NV_WRITE );
      }
      else
      {
        MSG_MED("BC PREF NV write Ok", 0,0,0);
        /* command is done */
        wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_NONE );

        /* send event to client */
        bc_mm_event_info.u.bc_pref = cmd_ptr->cmd.bc_mm_set_pref.bc_pref;
        wms_bc_mm_event_notify( WMS_BC_MM_EVENT_PREF, & bc_mm_event_info );

        /* update local data
        */
        rex_enter_crit_sect(&bc_crit_sect);

        bc_s_ptr->bc_pref = cmd_ptr->cmd.bc_mm_set_pref.bc_pref;
        bc_enable_bak = bc_s_ptr->bc_enable;
        bc_s_ptr->bc_enable = wms_bc_enable_check();

        rex_leave_crit_sect(&bc_crit_sect);

        /* if bc_enable flag has changed, notify CM */
        if( bc_enable_bak != bc_s_ptr->bc_enable )
        {
          /* bc_enable changed; need to notify CM */
          MSG_HIGH("BCSMS enable=%d", bc_s_ptr->bc_enable,0,0);
          wms_cm_bc_enable( bc_s_ptr->bc_enable );
        }
      }
    }
#endif /* FEATURE_CDSMS_BROADCAST */
  }
  /* done */
  return;
} /* wms_bc_mm_set_pref_proc() */

/*=========================================================================
FUNCTION
  wms_bc_mm_get_table_proc

DESCRIPTION
  This function is used to retrieve the service table for broadcast SMS.

  NOTE: To prevent retrieving a large service table, the function
  wms_bc_mm_get_all_service_ids() can be called to retrieve all service
  IDs, where each ID is much smaller than a complete table entry, and then
  the function wms_bc_mm_get_service_info() can be called for each service ID
  in order to retrieve the table entries one by one.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  On Success, Multimode Broadcast Event is sent to all WMS clients

=========================================================================*/
static void wms_bc_mm_get_table_proc
(
  wms_cmd_type  *cmd_ptr
)
{
  MSG_HIGH("WMS_CMD_BC_MM_GET_TABLE", 0, 0, 0);

  bc_mm_event_info.message_mode = cmd_ptr->cmd.bc_mm_get_table.message_mode;


  /* send event to the client
  */
  if( wms_bc_mm_make_table_event( cmd_ptr->cmd.bc_mm_get_table.message_mode,
                                & bc_mm_event_info.u.bc_mm_table ) == FALSE )
  {
    wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_NO_RESOURCE );
  }
  else
  {
    /* call the command callback saying no error
    */
    wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_NONE );

    wms_bc_mm_event_notify( WMS_BC_MM_EVENT_TABLE, & bc_mm_event_info );
    if (bc_mm_event_info.u.bc_mm_table.entries)
    {
      mem_free( &tmc_heap, bc_mm_event_info.u.bc_mm_table.entries );
      bc_mm_event_info.u.bc_mm_table.entries = NULL;
    }
  }
  /* done */
  return;
} /* wms_bc_mm_get_table_proc() */

/*=========================================================================
FUNCTION
  wms_bc_mm_select_srv_proc

DESCRIPTION
  This function is used to select a broadcast SMS service

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  On Success, Multimode Broadcast Event is sent to all WMS clients

=========================================================================*/
static void wms_bc_mm_select_srv_proc
(
  wms_cmd_type  *cmd_ptr
)
{
  uint32       i;
  nv_item_type nvi;


  MSG_HIGH("WMS_CMD_BC_SELECT_SRV", 0, 0, 0);

  bc_mm_event_info.message_mode = cmd_ptr->cmd.bc_mm_select_srv.message_mode;

  if( cmd_ptr->cmd.bc_mm_select_srv.message_mode == WMS_MESSAGE_MODE_GW )
  {
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */
  }
  else
  {
#ifdef FEATURE_CDSMS_BROADCAST
    boolean found = FALSE;
    nv_sms_bc_service_table_type   *service_ptr = NULL;

    /* find the entry matching service & language */
    for( i=0; i < bc_s_ptr->bc_table_size; i++ )
    {
      service_ptr = & bc_s_ptr->bc_table[i];

      if( service_ptr->active_service == TRUE &&
          service_ptr->service ==
            cmd_ptr->cmd.bc_mm_select_srv.srv_id.bc_srv_id.service &&
          service_ptr->language ==
            cmd_ptr->cmd.bc_mm_select_srv.srv_id.bc_srv_id.language)
      {
        found = TRUE;
        MSG_MED("BC SEL: found entry %d", i,0,0);
        break;
      }
    }

    if( found == FALSE )
    {
      MSG_ERROR("BC SEL: entry not found!", 0,0,0);
      wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_BC_BAD_SRV_ID );
    }
    else
    {
      if (service_ptr)
      {
        /* send event to client if anything changed */
        /* meanwhile, update num_selected */
        if( cmd_ptr->cmd.bc_mm_select_srv.selected == TRUE &&
            service_ptr->selected == FALSE )
        {
          bc_s_ptr->bc_num_selected ++;
        }
        else if( cmd_ptr->cmd.bc_mm_select_srv.selected == FALSE &&
                service_ptr->selected == TRUE )
        {
          bc_s_ptr->bc_num_selected --;
        }

        /* copy the old information to nv item */
        nvi.sms_bc_service_table = * service_ptr;

        /* make changes in the nv item */
        nvi.sms_bc_service_table.selected =
                                          cmd_ptr->cmd.bc_mm_select_srv.selected;

        /* write to NV */
        if( wms_nv_write_wait( NV_SMS_BC_SERVICE_TABLE_I, & nvi )
            != NV_DONE_S )
        {
          MSG_ERROR("BC Table NV write failed!", 0,0,0);
          wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_BC_NV_WRITE );
        }
        else
        {
          MSG_MED("BC Table NV write Ok", 0,0,0);

          /* update local data from new NV item
          */
          rex_enter_crit_sect(&bc_crit_sect);

          * service_ptr = nvi.sms_bc_service_table;
          bc_enable_bak = bc_s_ptr->bc_enable;
          bc_s_ptr->bc_enable = wms_bc_enable_check();

          rex_leave_crit_sect(&bc_crit_sect);

          /* if bc_enable flag has changed, notify CM */
          if( bc_enable_bak != bc_s_ptr->bc_enable )
          {
            /* bc_enable changed; need to notify CM */
            MSG_HIGH("BCSMS enable=%d", bc_s_ptr->bc_enable,0,0);
            wms_cm_bc_enable( bc_s_ptr->bc_enable );
          }

          /* send command status - no error
          */
          wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_NONE );

          /* send event to client
          */
          bc_mm_event_info.u.bc_mm_srv_info.srv_id.bc_srv_id.service
                              = (wms_service_e_type) service_ptr->service;
          bc_mm_event_info.u.bc_mm_srv_info.srv_id.bc_srv_id.language
                              = (wms_language_e_type) service_ptr->language;
          bc_mm_event_info.u.bc_mm_srv_info.selected
                              = service_ptr->selected;
          bc_mm_event_info.u.bc_mm_srv_info.priority
                              = (wms_priority_e_type) service_ptr->priority;
          memcpy( bc_mm_event_info.u.bc_mm_srv_info.label,
                  (char*) service_ptr->label,
                  WMS_BC_MM_SRV_LABEL_SIZE );

          {
            nv_sms_bc_service_table_options_type * opt_ptr =
                                                    & bc_s_ptr->bc_table_options[i];
            bc_mm_event_info.u.bc_mm_srv_info.label_encoding =
                       (wms_user_data_encoding_e_type) opt_ptr->label_encoding;
            bc_mm_event_info.u.bc_mm_srv_info.alert          = 
                       (wms_bc_alert_e_type) opt_ptr->bc_alert;
            bc_mm_event_info.u.bc_mm_srv_info.max_messages   = opt_ptr->max_messages;
          }

          wms_bc_mm_event_notify( WMS_BC_MM_EVENT_SRV_UPDATED, & bc_mm_event_info );
        }
      }
    }
#endif /* FEATURE_CDSMS_BROADCAST */
  }
  /* done */
  return;
} /* wms_bc_mm_select_srv_proc() */


/*=========================================================================
FUNCTION
  wms_bc_mm_get_all_srv_ids_proc

DESCRIPTION
  This function is used to retrieve all the service IDs in the broadcast
  SMS service table. After the retrieval of the service IDs, the function
  wms_bc_get_service_info() can be called to retrieve the complete
  table entries one by one.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  On Success, Multimode Broadcast Event is sent to all WMS clients

=========================================================================*/
static void wms_bc_mm_get_all_srv_ids_proc
(
  wms_cmd_type  *cmd_ptr
)
{
  MSG_HIGH("WMS_CMD_BC_MM_GET_ALL_SRV_IDS", 0, 0, 0);

  bc_mm_event_info.message_mode = cmd_ptr->cmd.bc_mm_get_all_srv_ids.message_mode;

  if( wms_bc_mm_make_srv_ids_event( bc_mm_event_info.message_mode,
                              & bc_mm_event_info.u.bc_mm_srv_ids ) == FALSE )
  {
    wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_NO_RESOURCE );
  }
  else
  {
    wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_NONE );
    wms_bc_mm_event_notify( WMS_BC_MM_EVENT_SRV_IDS, & bc_mm_event_info );
    if (bc_mm_event_info.u.bc_mm_srv_ids.entries)
    {
      mem_free( &tmc_heap, bc_mm_event_info.u.bc_mm_srv_ids.entries );
      bc_mm_event_info.u.bc_mm_srv_ids.entries = NULL;
    }
  }
  /* done */
  return;
} /* wms_bc_mm_get_all_srv_ids_proc() */

/*=========================================================================
FUNCTION
  wms_bc_mm_get_srv_info_proc

DESCRIPTION
  This function is used to retrieve a table entry in the broadcast SMS
  service table using a service ID.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  On Success, Multimode Broadcast Event is sent to all WMS clients

=========================================================================*/
static void wms_bc_mm_get_srv_info_proc
(
  wms_cmd_type  *cmd_ptr
)
{
  uint32                   i;

  MSG_HIGH("WMS_CMD_BC_MM_GET_SRV_INFO", 0, 0, 0);

  bc_mm_event_info.message_mode = cmd_ptr->cmd.bc_mm_get_srv_info.message_mode;

  if( cmd_ptr->cmd.bc_mm_get_srv_info.message_mode == WMS_MESSAGE_MODE_GW )
  {
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */
  }
  else
  {
#ifdef FEATURE_CDSMS_BROADCAST
    boolean                  found = FALSE;
    nv_sms_bc_service_table_type   *service_ptr = NULL;

    /* find the entry matching service & language */
    for( i=0; i < bc_s_ptr->bc_table_size; i++ )
    {
      service_ptr = & bc_s_ptr->bc_table[i];

      if( service_ptr->active_service == TRUE &&
          service_ptr->service ==
                          cmd_ptr->cmd.bc_mm_get_srv_info.srv_id.bc_srv_id.service &&
          service_ptr->language ==
                          cmd_ptr->cmd.bc_mm_get_srv_info.srv_id.bc_srv_id.language )
      {
        found = TRUE;
        MSG_MED("BC GET_SRV_INFO: found entry %d", i,0,0);
        break;
      }
    }

    if( found == FALSE )
    {
      MSG_ERROR("BC _SRV_INFO: entry not found!", 0,0,0);
      wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_BC_BAD_SRV_ID );
    }
    else
    {
      wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_NONE );

      if (service_ptr)
      {
        bc_mm_event_info.u.bc_mm_srv_info.srv_id.bc_srv_id.service
                          = (wms_service_e_type) service_ptr->service;
        bc_mm_event_info.u.bc_mm_srv_info.srv_id.bc_srv_id.language
                          = (wms_language_e_type) service_ptr->language;
        bc_mm_event_info.u.bc_mm_srv_info.selected
                          = service_ptr->selected;
        bc_mm_event_info.u.bc_mm_srv_info.priority
                          = (wms_priority_e_type) service_ptr->priority;
        memcpy( bc_mm_event_info.u.bc_mm_srv_info.label,
                (char*) service_ptr->label,
                WMS_BC_MM_SRV_LABEL_SIZE );
      }
      {
        nv_sms_bc_service_table_options_type * opt_ptr =
                                                & bc_s_ptr->bc_table_options[i];
        bc_mm_event_info.u.bc_mm_srv_info.label_encoding = 
                (wms_user_data_encoding_e_type) opt_ptr->label_encoding;
        bc_mm_event_info.u.bc_mm_srv_info.alert          = 
                (wms_bc_alert_e_type) opt_ptr->bc_alert;
        bc_mm_event_info.u.bc_mm_srv_info.max_messages   = opt_ptr->max_messages;
      }

      wms_bc_mm_event_notify( WMS_BC_MM_EVENT_SRV_INFO, & bc_mm_event_info );
    }
#endif /* FEATURE_CDSMS_BROADCAST */
  }
  /* done */
  return;
} /* wms_bc_mm_get_srv_info_proc() */

/*=========================================================================
FUNCTION
  wms_bc_mm_add_single_srv

DESCRIPTION
  This function is used to add a single entry

DEPENDENCIES
  None

RETURN VALUE
  WMS Command Error

SIDE EFFECTS
  None

=========================================================================*/
/* Add a single entry to service table
*/
wms_cmd_err_e_type wms_bc_mm_add_single_srv
(
  wms_message_mode_e_type            message_mode,
  wms_bc_mm_service_info_s_type    * srv_info_ptr
)
{
  uint32                i = 0;
  nv_item_type          nvi;
  boolean               found_free = FALSE;
  boolean               found_dup  = FALSE;
  wms_cmd_err_e_type    cmd_err = WMS_CMD_ERR_NONE;

  if (message_mode == WMS_MESSAGE_MODE_GW)
  {
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */
  }
  else
  {
#ifdef FEATURE_CDSMS_BROADCAST
    if( srv_info_ptr == NULL )
    {
      cmd_err = WMS_CMD_ERR_NULL_PTR;
    }
    else if( bc_s_ptr->bc_num_active == WMS_BC_TABLE_MAX )
    {
      /* table is full
      */
      cmd_err = WMS_CMD_ERR_BC_TABLE_FULL;
    }
    else
    {
      nv_sms_bc_service_table_type   *service_ptr = NULL;
      nv_sms_bc_service_table_options_type   *opt_ptr = NULL;

      wms_bc_search_table_entry
            (
              & srv_info_ptr->srv_id.bc_srv_id,
              & found_free,
              & found_dup,
              & i,
              & service_ptr,
              & opt_ptr
            );

      if( found_dup == TRUE )
      {
        MSG_ERROR("BC ADD_SRV: found dup entry %d", i,0,0);
        cmd_err = WMS_CMD_ERR_BC_DUPLICATE_SRV;
      }
      else
      {
        if( found_free == FALSE )
        {
          /* there is no free entry within the current table;
            use the free entry just after the current table
          */
          i = bc_s_ptr->bc_table_size;
          service_ptr = & bc_s_ptr->bc_table[i];
          opt_ptr = & bc_s_ptr->bc_table_options[i];

          /* increase the table size
          */
          bc_s_ptr->bc_table_size ++;

          /* Write the new table size back to NV
          */
          nvi.sms_bc_service_table_size = bc_s_ptr->bc_table_size;
          if( wms_nv_write_wait( NV_SMS_BC_SERVICE_TABLE_SIZE_I, & nvi )
              != NV_DONE_S )
          {
            MSG_ERROR("BC Table Size NV write failed!", 0,0,0);
            cmd_err = WMS_CMD_ERR_BC_NV_WRITE;
          }
        }
        /* else, use the free entry found earlier */


        /* update the entry and bc info
        */
        service_ptr->index          = (uint8)i;
        service_ptr->active_service = TRUE;
        service_ptr->service =
          srv_info_ptr->srv_id.bc_srv_id.service;
        service_ptr->language =
          srv_info_ptr->srv_id.bc_srv_id.language;
        service_ptr->selected = srv_info_ptr->selected;
        service_ptr->priority = srv_info_ptr->priority;
        memcpy( (char*) service_ptr->label,
                srv_info_ptr->label,
                WMS_BC_MM_SRV_LABEL_SIZE );

        opt_ptr->index          = service_ptr->index;
        opt_ptr->label_encoding = srv_info_ptr->label_encoding;
        opt_ptr->bc_alert       = srv_info_ptr->alert;
        opt_ptr->max_messages   = srv_info_ptr->max_messages;

        /* We have one more active entry
        */
        bc_s_ptr->bc_num_active ++;

        /* We may have one more selected entry
        */
        if( service_ptr->selected == TRUE)
        {
          bc_s_ptr->bc_num_selected ++;
        }

        /* write to NV
        */
        nvi.sms_bc_service_table = * service_ptr;
        if( wms_nv_write_wait( NV_SMS_BC_SERVICE_TABLE_I, & nvi )
            != NV_DONE_S )
        {
          MSG_ERROR("BC Table NV write failed!", 0,0,0);
          cmd_err = WMS_CMD_ERR_BC_NV_WRITE;
        }
        else
        {
          nvi.sms_bc_service_table_options = * opt_ptr;
          if( wms_nv_write_wait( NV_SMS_BC_SERVICE_TABLE_OPTIONS_I,
                                & nvi ) != NV_DONE_S )
          {
            MSG_ERROR("BC Table NV write failed!", 0,0,0);
            cmd_err = WMS_CMD_ERR_BC_NV_WRITE;
          }
        }
      }
    }
#endif /* FEATURE_CDSMS_BROADCAST */
  }
  return cmd_err;
} /* wms_bc_mm_add_single_srv() */

/*=========================================================================
FUNCTION
  wms_bc_mm_add_srv_proc

DESCRIPTION
  This function is used to add entries to the broadcast SMS service table.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  On Success, Multimode Broadcast Event is sent to all WMS clients

=========================================================================*/
static void wms_bc_mm_add_srv_proc
(
  wms_cmd_type  *cmd_ptr
)
{
  wms_cmd_err_e_type  cmd_err = WMS_CMD_ERR_NONE;
  uint32              i;

  MSG_HIGH("WMS_CMD_BC_MM_ADD_SRV", 0, 0, 0);

  bc_mm_event_info.message_mode = cmd_ptr->cmd.bc_mm_add_srv.message_mode;

  if( cmd_ptr->cmd.bc_mm_add_srv.message_mode == WMS_MESSAGE_MODE_GW )
  {
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */
  }
  else
  {
#ifdef FEATURE_CDSMS_BROADCAST
    for( i=0; i<cmd_ptr->cmd.bc_mm_add_srv.num_entries; i++ )
    {
      cmd_err = wms_bc_mm_add_single_srv(WMS_MESSAGE_MODE_CDMA,
                                         & cmd_ptr->cmd.bc_mm_add_srv.entries[i]);

      if( cmd_err != WMS_CMD_ERR_NONE )
      {
        /* Something goes wrong, send command error and stop processing. */
        wms_client_cmd_status( cmd_ptr, cmd_err );
        break;
      }
    } /* for() */

    /* Send table event to clients */
    if( cmd_err == WMS_CMD_ERR_NONE )
    {
      if( wms_bc_mm_make_table_event( WMS_MESSAGE_MODE_CDMA,
                                 & bc_mm_event_info.u.bc_mm_table ) == FALSE )
      {
        wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_NO_RESOURCE );
      }
      else
      {
        wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_NONE );

        /* Check to see if we need to enable BC to lower layers
        */
        rex_enter_crit_sect(&bc_crit_sect);

        bc_enable_bak = bc_s_ptr->bc_enable;
        bc_s_ptr->bc_enable = wms_bc_enable_check();

        rex_leave_crit_sect(&bc_crit_sect);

        /* if bc_enable flag has changed, notify CM */
        if( bc_enable_bak != bc_s_ptr->bc_enable )
        {
          MSG_HIGH("BCSMS enable=%d", bc_s_ptr->bc_enable,0,0);
          wms_cm_bc_enable( bc_s_ptr->bc_enable );
        }

        wms_bc_mm_event_notify(WMS_BC_MM_EVENT_ADD_SRVS, & bc_mm_event_info );
        if (bc_mm_event_info.u.bc_mm_table.entries)
        {
          mem_free( &tmc_heap, bc_mm_event_info.u.bc_mm_table.entries );
          bc_mm_event_info.u.bc_mm_table.entries = NULL;
        }
      }
    }
#endif /* FEATURE_CDSMS_BROADCAST */
  }
  /* done */
  return;
} /* wms_bc_mm_add_srv_proc() */

/*=========================================================================
FUNCTION
  wms_bc_mm_delete_single_srv

DESCRIPTION
  This function is used to delete a single entry

DEPENDENCIES
  None

RETURN VALUE
  WMS Command Error

SIDE EFFECTS
  None

=========================================================================*/
wms_cmd_err_e_type wms_bc_mm_delete_single_srv
(
  wms_message_mode_e_type         message_mode,
  wms_bc_mm_srv_id_type         * srv_id_ptr
)
{
  wms_cmd_err_e_type   cmd_err = WMS_CMD_ERR_NONE;
  uint32               i;
  nv_item_type         nvi;

  if (message_mode == WMS_MESSAGE_MODE_GW)
  {
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */
  }
  else
  {
#ifdef FEATURE_CDSMS_BROADCAST
    boolean found = FALSE;
    nv_sms_bc_service_table_type   *service_ptr = NULL;

    /* search the table for a free entry
    */
    for( i=0; i < bc_s_ptr->bc_table_size; i++ )
    {
      service_ptr = & bc_s_ptr->bc_table[i];

      if( service_ptr->active_service == TRUE &&
          service_ptr->service ==
                          srv_id_ptr->bc_srv_id.service &&
          service_ptr->language ==
                          srv_id_ptr->bc_srv_id.language )
      {
        found = TRUE;
        break;
      }
    }

    if( found == FALSE )
    {
      MSG_ERROR("BC DELETE_SRV: entry not found!", 0,0,0);
      cmd_err = WMS_CMD_ERR_BC_BAD_SRV_ID;
    }
    else
    {
      if (service_ptr)
      {
        /* de-activate this entry
        */
        service_ptr->active_service = FALSE;

        /* we have one less active entry
        */
        bc_s_ptr->bc_num_active --;

        /* we may have one less selected entry
        */
        if( service_ptr->selected == TRUE )
        {
          service_ptr->selected = FALSE;
          bc_s_ptr->bc_num_selected --;
        }

        /* write to NV
        */
        nvi.sms_bc_service_table = * service_ptr;
        if( wms_nv_write_wait( NV_SMS_BC_SERVICE_TABLE_I, & nvi )
            != NV_DONE_S )
        {
          MSG_ERROR("BC Table NV write failed!", 0,0,0);
          cmd_err = WMS_CMD_ERR_BC_NV_WRITE;
        }
      }
    }
#endif /* FEATURE_CDSMS_BROADCAST */
  }
  return cmd_err;
} /* wms_bc_mm_delete_single_srv() */

/*=========================================================================
FUNCTION
  wms_bc_mm_delete_srv_proc

DESCRIPTION
  This function is used to delete entries from the broadcast SMS service table.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  On Success, Multimode Broadcast Event is sent to all WMS clients

=========================================================================*/
static void wms_bc_mm_delete_srv_proc
(
  wms_cmd_type  *cmd_ptr
)
{
  uint32              i;
  wms_cmd_err_e_type  cmd_err = WMS_CMD_ERR_NONE;


  MSG_HIGH("WMS_CMD_BC_MM_DELETE_SRV", 0, 0, 0);

  bc_mm_event_info.message_mode = cmd_ptr->cmd.bc_mm_delete_srv.message_mode;

  if( cmd_ptr->cmd.bc_mm_delete_srv.message_mode == WMS_MESSAGE_MODE_GW )
  {
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */
  }
  else
  {
#ifdef FEATURE_CDSMS_BROADCAST
    for( i=0; i<cmd_ptr->cmd.bc_mm_delete_srv.num_entries; i++ )
    {
      cmd_err = wms_bc_mm_delete_single_srv( WMS_MESSAGE_MODE_CDMA,
                    & cmd_ptr->cmd.bc_mm_delete_srv.srv_ids[i]);

      if( cmd_err != WMS_CMD_ERR_NONE )
      {
        /* Something goes wrong, send command error and stop processing. */
        wms_client_cmd_status( cmd_ptr, cmd_err );
        break;
      }
    } /* for() */

    /* Send event to clients */
    if( cmd_err == WMS_CMD_ERR_NONE )
    {
      if( wms_bc_mm_make_srv_ids_event( WMS_MESSAGE_MODE_CDMA,
                             & bc_mm_event_info.u.bc_mm_srv_ids ) == FALSE )
      {
        wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_NO_RESOURCE );
      }
      else
      {
        wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_NONE );

        /* Check to see if we need to enable BC to lower layers
        */
        rex_enter_crit_sect(&bc_crit_sect);

        bc_enable_bak = bc_s_ptr->bc_enable;
        bc_s_ptr->bc_enable = wms_bc_enable_check();

        rex_leave_crit_sect(&bc_crit_sect);

        /* if bc_enable flag has changed, notify CM */
        if( bc_enable_bak != bc_s_ptr->bc_enable )
        {
          MSG_HIGH("BCSMS enable=%d", bc_s_ptr->bc_enable,0,0);
          wms_cm_bc_enable( bc_s_ptr->bc_enable );
        }

        wms_bc_mm_event_notify( WMS_BC_MM_EVENT_DELETE_SRVS, & bc_mm_event_info );
        if (bc_mm_event_info.u.bc_mm_srv_ids.entries)
        {
          mem_free( &tmc_heap, bc_mm_event_info.u.bc_mm_srv_ids.entries );
          bc_mm_event_info.u.bc_mm_srv_ids.entries = NULL;
        }
      }
    }
#endif /* FEATURE_CDSMS_BROADCAST */
  }
  /* done */
  return;
} /* wms_bc_mm_delete_srv_proc() */


/*=========================================================================
FUNCTION
  wms_bc_mm_change_srv_info_proc

DESCRIPTION
  This function is used to change an entry in the broadcast SMS service table.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  On Success, Multimode Broadcast Event is sent to all WMS clients

=========================================================================*/
static void wms_bc_mm_change_srv_info_proc
(
  wms_cmd_type  *cmd_ptr
)
{
  uint32       i;
  nv_item_type nvi;

  MSG_HIGH("WMS_CMD_BC_MM_CHANGE_SRV_INFO", 0, 0, 0);

  bc_mm_event_info.message_mode=cmd_ptr->cmd.bc_mm_change_srv_info.message_mode;

  if( cmd_ptr->cmd.bc_mm_change_srv_info.message_mode == WMS_MESSAGE_MODE_GW )
  {
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */
  }
  else
  {
#ifdef FEATURE_CDSMS_BROADCAST
    boolean                        found = FALSE;
    nv_sms_bc_service_table_type   *service_ptr = NULL;

    /* find the entry matching service & language */
    for( i=0; i < bc_s_ptr->bc_table_size; i++ )
    {
      service_ptr = & bc_s_ptr->bc_table[i];

      if( service_ptr->active_service == TRUE &&
          service_ptr->service ==
                          cmd_ptr->cmd.bc_mm_change_srv_info.srv_info.srv_id.bc_srv_id.service &&
          service_ptr->language ==
                          cmd_ptr->cmd.bc_mm_change_srv_info.srv_info.srv_id.bc_srv_id.language )
      {
        found = TRUE;
        MSG_MED("BC SEL: found entry %d", i,0,0);
        break;
      }
    }

    if( found == FALSE )
    {
      MSG_ERROR("BC MM CHange SRV INFO: entry not found!", 0,0,0);
      wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_BC_BAD_SRV_ID );
    }
    else
    {
      if (service_ptr)
      {
        memcpy( (char*) service_ptr->label,
                cmd_ptr->cmd.bc_mm_change_srv_info.srv_info.label,
                WMS_BC_MM_SRV_LABEL_SIZE );

        /* write to NV
        */
        nvi.sms_bc_service_table = * service_ptr;
        if( wms_nv_write_wait( NV_SMS_BC_SERVICE_TABLE_I, & nvi )
            != NV_DONE_S )
        {
          MSG_ERROR("BC Table NV write failed!", 0,0,0);
          wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_BC_NV_WRITE );
        }
        else
        {
          wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_NONE );

          /* command is Ok, send the event to client
          */
          bc_mm_event_info.u.bc_mm_srv_info.srv_id.bc_srv_id.service
                            = (wms_service_e_type) service_ptr->service;
          bc_mm_event_info.u.bc_mm_srv_info.srv_id.bc_srv_id.language
                            = (wms_language_e_type) service_ptr->language;
          bc_mm_event_info.u.bc_mm_srv_info.selected
                            = service_ptr->selected;
          bc_mm_event_info.u.bc_mm_srv_info.priority
                            = (wms_priority_e_type) service_ptr->priority;
          memcpy( bc_mm_event_info.u.bc_mm_srv_info.label,
                  (char*) service_ptr->label,
                  WMS_BC_MM_SRV_LABEL_SIZE );

          {
            nv_sms_bc_service_table_options_type  * opt_ptr
                                            = & bc_s_ptr->bc_table_options[i];
            bc_mm_event_info.u.bc_mm_srv_info.label_encoding =
                    (wms_user_data_encoding_e_type) opt_ptr->label_encoding;
            bc_mm_event_info.u.bc_mm_srv_info.alert         =
                    (wms_bc_alert_e_type) opt_ptr->bc_alert;
            bc_mm_event_info.u.bc_mm_srv_info.max_messages  = opt_ptr->max_messages;
          }

          wms_bc_mm_event_notify( WMS_BC_MM_EVENT_SRV_UPDATED, & bc_mm_event_info );
        }
      }
    }
#endif /* FEATURE_CDSMS_BROADCAST */
  }
  /* done */
  return;
} /* wms_bc_mm_change_srv_info() */


/*=========================================================================
FUNCTION
  wms_bc_mm_delete_all_services_proc

DESCRIPTION
  This function is used to delete all entries from the broadcast service table.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  On Success, Multimode Broadcast Event is sent to all WMS clients

=========================================================================*/
static void wms_bc_mm_delete_all_services_proc
(
  wms_cmd_type  *cmd_ptr
)
{
  uint32       i;
  nv_item_type nvi;
  wms_cmd_err_e_type cmd_err = WMS_CMD_ERR_NONE;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_HIGH("WMS_CMD_BC_MM_DELETE_ALL_SERVICES", 0, 0, 0);

  bc_mm_event_info.message_mode=cmd_ptr->cmd.bc_mm_delete_all_srvs.message_mode;

  if( cmd_ptr->cmd.bc_mm_delete_all_srvs.message_mode == WMS_MESSAGE_MODE_GW )
  {
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */
  }
  else
  {
#ifdef FEATURE_CDSMS_BROADCAST
    nv_sms_bc_service_table_type   *service_ptr = NULL;
    /* delete all entries from table
    */
    for( i=0; i < bc_s_ptr->bc_table_size; i++ )
    {
      service_ptr = & bc_s_ptr->bc_table[i];

      /* If this table entry is active, delete it
      */
      if( service_ptr->active_service == TRUE )
      {
        /* de-activate this entry
        */
        service_ptr->active_service = FALSE;
        service_ptr->selected       = FALSE;

        /* write to NV
        */
        nvi.sms_bc_service_table = * service_ptr;
        if( wms_nv_write_wait( NV_SMS_BC_SERVICE_TABLE_I, & nvi )
            != NV_DONE_S )
        {
          MSG_ERROR("BC Table NV write failed!", 0,0,0);
          cmd_err = WMS_CMD_ERR_BC_NV_WRITE;
        }
      }
    }

    bc_s_ptr->bc_num_active   = 0;
    bc_s_ptr->bc_num_selected = 0;

    /* Send command status
    */
    wms_client_cmd_status( cmd_ptr, cmd_err );

    if( cmd_err == WMS_CMD_ERR_NONE )
    {
      /* Delete all was successful
      */
      MSG_MED("BC Table Delete All OK", 0,0,0);

      /* update local data from new NV item
      */
      rex_enter_crit_sect(&bc_crit_sect);

      bc_enable_bak = bc_s_ptr->bc_enable;
      bc_s_ptr->bc_enable = wms_bc_enable_check();

      rex_leave_crit_sect(&bc_crit_sect);

      /* if bc_enable flag has changed, notify CM */
      if( bc_enable_bak != bc_s_ptr->bc_enable )
      {
        MSG_HIGH("BCSMS enable=%d", bc_s_ptr->bc_enable,0,0);
        wms_cm_bc_enable( bc_s_ptr->bc_enable );
      }

      /* Send the event to client
      */
      bc_mm_event_info.event = WMS_BC_MM_EVENT_DELETE_ALL_SRVS;
      bc_mm_event_info.message_mode = WMS_MESSAGE_MODE_CDMA;

      /* no event data needed */
      wms_bc_mm_event_notify( WMS_BC_MM_EVENT_DELETE_ALL_SRVS, & bc_mm_event_info );
    }
#endif /* FEATURE_CDSMS_BROADCAST */
  }
  /* done */
  return;
} /* wms_bc_mm_delete_all_services_proc() */


/*=========================================================================
FUNCTION
  wms_bc_mm_select_all_services_proc

DESCRIPTION
  This function is used to select/deselect all entries from the
  broadcast service table.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  On Success, Multimode Broadcast Event is sent to all WMS clients

=========================================================================*/
static void wms_bc_mm_select_all_services_proc
(
  wms_cmd_type  *cmd_ptr
)
{
  uint32       i;
  nv_item_type nvi;
  wms_cmd_err_e_type             cmd_err = WMS_CMD_ERR_NONE;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_HIGH("WMS_CMD_BC_MM_SELECT_ALL_SERVICES", 0, 0, 0);

  bc_mm_event_info.message_mode=cmd_ptr->cmd.bc_mm_select_all_srvs.message_mode;

  if( cmd_ptr->cmd.bc_mm_select_all_srvs.message_mode == WMS_MESSAGE_MODE_GW )
  {
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */
  }
  else
  {
#ifdef FEATURE_CDSMS_BROADCAST
    nv_sms_bc_service_table_type   *service_ptr = NULL;
    /* Update all entries in the table
    */
    for( i=0; i < bc_s_ptr->bc_table_size; i++ )
    {
      service_ptr = & bc_s_ptr->bc_table[i];

      if( service_ptr->active_service == TRUE )
      {
        rex_enter_crit_sect(&bc_crit_sect);
        if( service_ptr->selected == TRUE &&
            cmd_ptr->cmd.bc_mm_select_all_srvs.selected == FALSE )
        {
          bc_s_ptr->bc_num_selected --;
          service_ptr->selected = FALSE;
        }
        else if( service_ptr->selected == FALSE &&
                  cmd_ptr->cmd.bc_mm_select_all_srvs.selected == TRUE )
        {
          bc_s_ptr->bc_num_selected ++;
          service_ptr->selected = TRUE;
        }

        rex_leave_crit_sect(&bc_crit_sect);

        /* write to NV
        */
        nvi.sms_bc_service_table = * service_ptr;
        if( wms_nv_write_wait( NV_SMS_BC_SERVICE_TABLE_I, & nvi )
            != NV_DONE_S )
        {
          MSG_ERROR("BC Table NV write failed!", 0,0,0);
          cmd_err = WMS_CMD_ERR_BC_NV_WRITE;
        }
      }
    }

    /* Send command status
    */
    wms_client_cmd_status( cmd_ptr, cmd_err );

    if( cmd_err == WMS_CMD_ERR_NONE )
    {
      if( wms_bc_mm_make_table_event( WMS_MESSAGE_MODE_CDMA,
                            & bc_mm_event_info.u.bc_mm_table ) == FALSE )
      {
        wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_NO_RESOURCE );
      }
      else
      {
        wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_NONE );

        /* update local data from new NV item
        */
        rex_enter_crit_sect(&bc_crit_sect);

        bc_enable_bak = bc_s_ptr->bc_enable;
        bc_s_ptr->bc_enable = wms_bc_enable_check();

        rex_leave_crit_sect(&bc_crit_sect);

        /* if bc_enable flag has changed, notify CM */
        if( bc_enable_bak != bc_s_ptr->bc_enable )
        {
          MSG_HIGH("BCSMS enable=%d", bc_s_ptr->bc_enable,0,0);
          wms_cm_bc_enable( bc_s_ptr->bc_enable );
        }

        /* Send the event to client
        */
        wms_bc_mm_event_notify( WMS_BC_MM_EVENT_SELECT_ALL_SRVS,
                                & bc_mm_event_info);
        if (bc_mm_event_info.u.bc_mm_table.entries)
        {
          mem_free( &tmc_heap, bc_mm_event_info.u.bc_mm_table.entries );
          bc_mm_event_info.u.bc_mm_table.entries = NULL;
        }
      }
    }
#endif /* FEATURE_CDSMS_BROADCAST */
  }
  /* done */
  return;
} /* wms_bc_mm_select_all_services_proc() */

/*=========================================================================
FUNCTION
  wms_bc_mm_set_priority_for_all_services_proc

DESCRIPTION
  This function is used to set the priorities for all entries from the
  broadcast service table. (Specific to CDMA, Does not apply to GW)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  On Success, Multimode Broadcast Event is sent to all WMS clients
=========================================================================*/
static void wms_bc_mm_set_priority_all_services_proc
(
  wms_cmd_type  *cmd_ptr
)
{
#ifdef FEATURE_CDSMS_BROADCAST
  uint32       i;
  nv_item_type nvi;
  wms_cmd_err_e_type             cmd_err = WMS_CMD_ERR_NONE;
#endif /* FEATURE_CDSMS_BROADCAST */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_HIGH("WMS_CMD_BC_MM_SET_PRIORITY_ALL_SERVICES", 0, 0, 0);

  bc_mm_event_info.message_mode=cmd_ptr->cmd.bc_mm_set_priority_all_srvs.message_mode;

  if( cmd_ptr->cmd.bc_mm_set_priority_all_srvs.message_mode == WMS_MESSAGE_MODE_GW )
  {
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */
  }
  else
  {
#ifdef FEATURE_CDSMS_BROADCAST
    nv_sms_bc_service_table_type   *service_ptr = NULL;
    /* Update all entries in the table
    */
    for( i=0; i < bc_s_ptr->bc_table_size; i++ )
    {
      service_ptr = & bc_s_ptr->bc_table[i];

      /* If this table entry is active and selected, update priority
      */
      if( service_ptr->active_service == TRUE &&
          service_ptr->priority != cmd_ptr->cmd.bc_mm_set_priority_all_srvs.priority )
      {
        service_ptr->priority = cmd_ptr->cmd.bc_mm_set_priority_all_srvs.priority;

        /* write to NV
        */
        nvi.sms_bc_service_table = * service_ptr;
        if( wms_nv_write_wait( NV_SMS_BC_SERVICE_TABLE_I, & nvi )
            != NV_DONE_S )
        {
          MSG_ERROR("BC Table NV write failed!", 0,0,0);
          cmd_err = WMS_CMD_ERR_BC_NV_WRITE;
        }
      }
    }

    /* Send command status
    */
    wms_client_cmd_status( cmd_ptr, cmd_err );

   if( cmd_err == WMS_CMD_ERR_NONE )
    {
      if( wms_bc_mm_make_table_event( WMS_MESSAGE_MODE_CDMA,
                            & bc_mm_event_info.u.bc_mm_table ) == FALSE )
      {
        wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_NO_RESOURCE );
      }
      else
      {
        wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_NONE );
        /* Send the event to client
        */
        wms_bc_mm_event_notify( WMS_BC_MM_EVENT_SET_PRIORITY_ALL_SRVS,
                                & bc_mm_event_info);
        if (bc_mm_event_info.u.bc_mm_table.entries)
        {
          mem_free( &tmc_heap, bc_mm_event_info.u.bc_mm_table.entries );
          bc_mm_event_info.u.bc_mm_table.entries = NULL;
        }
      }
    }
#endif /* FEATURE_CDSMS_BROADCAST */
  }
  /* done */
  return;
} /* wms_bc_mm_set_priority_all_services_proc() */

/*=========================================================================
FUNCTION
  wms_bc_mm_msg_delete_indication_proc

DESCRIPTION
  This function is used to notify the lower layer of Message Deletion.
  This is applicable to GW mode only and is used by the WMS Clients to
  indicate the deletion of a GW CB page.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=========================================================================*/
static void wms_bc_mm_msg_delete_indication_proc
(
  wms_cmd_type  *cmd_ptr
)
{
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#else /* FEATURE_GWSMS_BROADCAST */
  /* Send Command Status */
  wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_UNSUPPORTED );
#endif /* FEATURE_GWSMS_BROADCAST */

  /* done */
  return;

} /* wms_bc_mm_msg_delete_indication_proc() */

/*=========================================================================
FUNCTION
  wms_bc_mm_msg_delete_all_indication_proc

DESCRIPTION
  This function is used to notify the lower layer of Message Deletion.
  This is applicable to GW mode only and is used by the WMS Client to
  indicate the deletion of ALL GW CB page.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=========================================================================*/
static void wms_bc_mm_msg_delete_all_indication_proc
(
  wms_cmd_type  *cmd_ptr
)
{
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#else /* FEATURE_GWSMS_BROADCAST */
  /* Send Command Status */
  wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_UNSUPPORTED );
#endif /* FEATURE_GWSMS_BROADCAST */

  /* done */
  return;
} /* wms_bc_mm_msg_delete_all_indication_proc() */

/*=========================================================================
FUNCTION
  wms_bc_mm_get_reading_pref_proc

DESCRIPTION
  This function is used to Get the Reading Preferences - Reading Advised
  and Reading Optional for UMTS CBS Messages
  This is applicable to GW mode only.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  On Success, Multimode Broadcast Event is sent to all WMS clients
=========================================================================*/
static void wms_bc_mm_get_reading_pref_proc
(
  wms_cmd_type  *cmd_ptr
)
{
  MSG_HIGH("WMS_CMD_BC_MM_GET_READING_PREF", 0, 0, 0);

  if( cmd_ptr->cmd.bc_mm_get_reading_pref.message_mode == WMS_MESSAGE_MODE_CDMA )
  {
    wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_UNSUPPORTED );
  }
  else if( cmd_ptr->cmd.bc_mm_get_reading_pref.message_mode == WMS_MESSAGE_MODE_GW )
  {
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */
  }
  else
  {
    MSG_ERROR("Invalid Message Mode for BC MM Get Reading Pref", 0, 0, 0);
    wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_UNSUPPORTED );
  }

  /* done */
  return;
} /* wms_bc_mm_get_reading_pref_proc() */

/*=========================================================================
FUNCTION
  wms_bc_mm_set_reading_pref_proc

DESCRIPTION
  This function is used to Set the Reading Preferences - Reading Advised
  and Reading Optional for UMTS CBS Messages
  This is applicable to GW mode only.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  On Success, Multimode Broadcast Event is sent to all WMS clients
=========================================================================*/
static void wms_bc_mm_set_reading_pref_proc
(
  wms_cmd_type  *cmd_ptr
)
{
#ifdef FEATURE_UMTS_BMC
#error code not present
#endif /* FEATURE_UMTS_BMC */

  MSG_HIGH("WMS_CMD_BC_MM_SET_READING_PREF", 0, 0, 0);

  if( cmd_ptr->cmd.bc_mm_set_reading_pref.message_mode == WMS_MESSAGE_MODE_CDMA )
  {
#ifdef FEATURE_CDSMS_BROADCAST
    wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_UNSUPPORTED );
#endif /* FEATURE_CDSMS_BROADCAST */
  }
  else if( cmd_ptr->cmd.bc_mm_set_reading_pref.message_mode == WMS_MESSAGE_MODE_GW )
  {
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */
  }
  else
  {
    MSG_ERROR("Invalid Message Mode for BC MM Set Reading Pref", 0, 0, 0);
    wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_UNSUPPORTED );
  }

  /* done */
  return;
} /* wms_bc_mm_set_reading_pref_proc() */

/*===========================================================================
FUNCTION wms_bc_mm_process_cmd

DESCRIPTION
  Dispatch the BC command processing.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_bc_mm_process_cmd
(
  wms_cmd_type  *cmd_ptr
)
{

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch( cmd_ptr->hdr.cmd_id )
  {
    /* BC commands
    */
    case WMS_CMD_BC_MM_GET_CONFIG:
      wms_bc_mm_get_config_proc( cmd_ptr );
      break;

    case WMS_CMD_BC_MM_GET_PREF:
      wms_bc_mm_get_pref_proc( cmd_ptr );
      break;

    case WMS_CMD_BC_MM_SET_PREF:
      wms_bc_mm_set_pref_proc( cmd_ptr );
      break;

    case WMS_CMD_BC_MM_GET_TABLE:
      wms_bc_mm_get_table_proc( cmd_ptr );
      break;

    case WMS_CMD_BC_MM_SELECT_SRV:
      wms_bc_mm_select_srv_proc( cmd_ptr );
      break;

    case WMS_CMD_BC_MM_GET_ALL_SRV_IDS:
      wms_bc_mm_get_all_srv_ids_proc( cmd_ptr );
      break;

    case WMS_CMD_BC_MM_GET_SRV_INFO:
      wms_bc_mm_get_srv_info_proc( cmd_ptr );
      break;

    case WMS_CMD_BC_MM_ADD_SRV:
      wms_bc_mm_add_srv_proc( cmd_ptr );
      break;

    case WMS_CMD_BC_MM_DELETE_SRV:
      wms_bc_mm_delete_srv_proc( cmd_ptr );
      break;

    case WMS_CMD_BC_MM_CHANGE_SRV_INFO:
      wms_bc_mm_change_srv_info_proc( cmd_ptr );
      break;

    case WMS_CMD_BC_MM_DELETE_ALL_SERVICES:
      wms_bc_mm_delete_all_services_proc( cmd_ptr );
      break;

    case WMS_CMD_BC_MM_SELECT_ALL_SERVICES:
      wms_bc_mm_select_all_services_proc( cmd_ptr );
      break;

    case WMS_CMD_BC_MM_SET_PRIORITY_ALL_SERVICES:
      wms_bc_mm_set_priority_all_services_proc( cmd_ptr );
      break;

    case WMS_CMD_BC_MM_MSG_DELETE_INDICATION:
      wms_bc_mm_msg_delete_indication_proc( cmd_ptr );
      break;

   case WMS_CMD_BC_MM_MSG_DELETE_ALL_INDICATION:
      wms_bc_mm_msg_delete_all_indication_proc( cmd_ptr );
      break;

    case WMS_CMD_BC_MM_GET_READING_PREF:
      wms_bc_mm_get_reading_pref_proc( cmd_ptr );
      break;

    case WMS_CMD_BC_MM_SET_READING_PREF:
      wms_bc_mm_set_reading_pref_proc( cmd_ptr );
      break;

#ifdef FEATURE_CDSMS_BROADCAST
    case WMS_CMD_BC_SCPT_DATA:
      wms_bc_scpt_data_proc( cmd_ptr );
      break;
#endif /* FEATURE_CDSMS_BROADCAST */

    default:
      MSG_ERROR("Invalid BC MM cmd: %d", cmd_ptr->hdr.cmd_id, 0,0);
      break;
  }

  /* done */
  return;

} /* wms_bc_mm_process_cmd() */


#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */

#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */

#endif /* FEATURE_CDSMS || FEATURE_GWSMS */

/* END of wmsbc.c */

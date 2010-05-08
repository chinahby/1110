/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


       W i r e l e s s     M e s s a g i n g  S e r v i c e s
       -- Configuration Group

GENERAL DESCRIPTION
  The WMS module which implements the User API for SMS. This source file
  implements the configuration group functions which process the client
  commands of the configuration group.

  Copyright (c) 2001 - 2008 by QUALCOMM INCORPORATED.
  All Rights Reserved.

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/* ^L<EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/wms/wms/rel/qsc1100_05.04.00/src/WMSE/wmscfg.c#3 $ $DateTime: 2008/12/02 13:02:59 $ $Author: pyaramad $

when       who     what, where, why
===========================================================================
10/20/08   RG      Change route and memory store to STORE_AND_NOTIFY & SIM
                   resp. for msg type WMS_GW_MSG_WAITING_DISCARD
07/24/08   PMD     Fixed compile Error found on 6290 SMS over SIP Build
                   Dropping CB_CELL_CHANGE event in special cases
06/11/08   PMD     Removed wms_bc_*** APIs for CDMA Broadcast
04/29/08   PMD     Ignore User Data Text check in wms_cfg_determine_pp_route()
04/18/08   PMD     Removed FEATURE_WMS_CM_NAS_INTERFACES
04/02/08   PMD     Initialize route in wms_cfg_determine_pp_route()
03/31/08   PMD     Added Buffer Overflow checks; Fixed Indentation
02/16/08   PMD     Fixed Compile Warning for 1x Targets
02/11/08   PMD     Unified Logic for Domain Selection Enhancements
01/30/08   PMD/PY  Created cm/nas interfacing layer in wmscm_gw.h and wmscm_gw.c
10/02/07   PMD     Fixed Indentation Warning in wms_cfg_ipapp_change_info()
07/23/07   PMD     Added support for plus code dialing
07/23/07   NP      LPM broken, WMS does not go or wake up from LPM
07/12/07   NP      SMS Over IP problems.
06/20/07   PMD     Include wmstsi.h
04/06/07   HQ      Added detection of PLMN change.
03/30/07   HQ      Added support for Power Collapse/Power Management
                   for Dual CPU chipsets.
03/16/07   HQ      Added support for LPM.
03/10/07   HQ      Revert domain pref on cell change.
03/08/07   HQ      Fixed cell change for FEATURE_WMS_CM_NAS_INTERFACES.
03/01/07   HQ      Cleaned up SS info for GW.
02/27/07   HQ      Added support of FEATURE_SMS_OVER_IP.
02/20/07   HQ      Added support for FEATURE_WMS_CM_NAS_INTERFACES.
01/25/07   HQ      Drop unwanted cell change events.
01/17/07   HQ      Updated copyright info to include 2007.
01/15/07   PMD     Defaulted Domain Preference to CS Only
12/06/06   PMD     Changes to allow any client to become Primary Client
11/13/06   HQ      Read phone's ESN during init for JCDMA2;
                   changed prototype for wms_ts_decode_CDMA_bd().
11/06/06   PMD     Replaced Feature T_MSM6250 with FEATURE_GWSMS
                   Eliminated FEATURE_SMS_LTK from Code
10/05/06   PMD     Fixed Featurization for WMS RPC Code
09/27/06   PMD     Changed TASKLOCK and TASKFREE to Critical Sections
                   Removed featurization for sts_rpt_crit_sect
09/18/06   PMD     Exported functions under FEATURE_GWSMS for HTORPC Functionality
08/14/06   PMD     Mainlined / Replaced FEATURE_GWSMS_UDH in code
08/14/06   PMD     Mainlined / Replaced FEATURE_GWSMS_STATUS_REPORTS in code
06/01/06   PMD     Featurized RUIM Cache under FEATURE_CDSMS_CACHE
05/19/06   PMD     Fixed compile warning
05/03/06   PMD     Modified wms_ts_decode_CDMA_bd prototype
04/28/06   PMD     Added support for SIM_INIT_COMPLETED Event for Game Mode
04/24/06   HQ      Fixed lint errors.
01/31/06   PMD     Updated Template List for NV_CDMA and NV_GW
01/31/06   PMD     Always Acknowledge a Status Report.
01/25/06   VK      Updated Copyright Info
12/21/05   VK      Lint fixes
01/20/06   PMD     Check for Null Pointer in wms_cfg_activate_client()
12/20/05   PMD     Added Support for CS Only and PS Only GW Domain Preference.
11/18/05   VK      Cleaned up Lint warnings. 
10/20/05   PMD     Removed client_ptr from wms_cfg_update_msg_info_cache()
10/20/05   VK      Removed lint suppressions.
10/18/05   VK      Added new feature: FEATURE_CDSMS_DUP_DETECTION
                   Added new feature: FEATURE_CDSMS_CACHE
10/17/05   PMD     Updated Copyright Information.
                   Added Function Headers.
10/17/05   PMD     Resetting SIM Message Reference to 0 if it exceeds 255.
08/04/05   PMD     Fixed certain Lint Warnings in code.
07/27/05   PMD     Set client_memory_full flag to FALSE on power-up.
07/19/05   PMD     Added wms_cfg_set_gw_domain_pref_internal_proc function.
07/13/05   PMD     Fix for broken wms_cfg_do_total_memory_full()
07/09/05   PMD     Fix for Mem Full Flag handling with Client Memory
07/06/05   Rex     Fix voicemail merge issue 
06/07/05   Rex     Fixes for voicemail
05/14/05   PMD     Added RAM Storage Support for Status Reports
05/12/05   PMD     Support for Tranfer Only Status Report 
04/04/05   PMD     Fix for not treating all Messages as Voicemail
03/31/05   PMD     Support for Cell Change Type Event Information.
03/21/05   Rex     Fixed voicemail storage for dcs discard group
03/08/05   PMD     Fixed Compile Warning.
02/25/05   Rex     Added additional VMWI support
02/14/05   Rex     Length checking for memcpy
01/10/05   Rex     Code cleanup for WMS decoupling from NAS module and
                   using CM, featurize as FEATURE_APIONE
01/10/05   Rex     Fixes for checking CPHS message
01/07/05   PMD     Added feature FEATURE_SMS_LTK
12/02/04   Rex     Send SMMA request when memory is filled up
11/03/04   PMD     Added wms_cfg_check_wap_push_message function
10/08/04   PMD     Changed Default Routes Configuration
09/24/04   Rex     Fix for CPHS
09/23/04   Rex     Featurization fix
09/22/04   Rex     Featurize SMS link control
09/21/04   Rex     Added support for CPHS
09/01/04   Rex     Fix featurization problem
08/25/04   Rex     Added support for SMS link control
08/02/04   PMD     Check for GW-PP Format while checking GW Voicemail Contents
07/30/04   HQ      Removed ERR_FATALs.
07/29/04   HQ      Fixed wrong display of MSG_ERRORs.
06/02/04   HQ      Changed class to msg_class to fix C++ compilation issue.
04/15/04   PMD     Added support for new flag: refresh_registered
03/29/04   Rex     Check for record length when getting next message number from RUIM
03/23/04   PMD     Changed Primary Client and Memory Status Implementation.
03/08/04   PMD     Added Support for SMMA Notification using Client Memory
02/10/04   HQ      Delay calling wms_cfg_do_cdma_ready_event().
02/04/04   PMD     Cleaned up Lint Warnings in code
01/13/04   HQ      Skip dup_detect cache for WAP and GSM1x messages;
                   Added utility wms_cfg_check_cdma_duplicate().
12/15/03   PMD     Added 1x Broadcast Support in Multimode Broadcast API
12/02/03   HQ      Resolved some Lint errors.
11/05/03   PMD     Added Checks in wms_cfg_set_gw_domain_pref
11/03/03   PMD     Added Checks for Memory Store and Tag values in
                   wms_cfg_get_mem_status_proc and wms_cfg_get_msg_list_proc
10/15/03   HQ      Fixed compiler warnings.
10/10/03   HQ      Fixed compile error.
10/03/03   AP      Added check for voicemail contents.
09/22/03   PMD     Removed Reset Message Reference Handling from this file
09/18/03   HQ/PMD  Use correct union field while sending memory full event.
09/07/03   PMD     Added Support for cfg_event String Display
08/14/03   PMD     Fixed Last Character of RAM message getting corrupted
08/11/03   PMD     Added routes_set(boolean flag) in wms_cfg_determine_pp_route
08/05/03   HQ      Set domain pref to WMS_GW_DOMAIN_PREF_PS_PREFERRED for
                   unsupported values in the NV item during init.
07/29/03   HQ      Added support for FEATURE_BROADCAST_SMS_MULTIMODE:
                      - New BC_MM commands, events and structures.
                      - New BC_MM API functions.
                      - GSM CB support.
                      - 1x BC SMS is still supported via BC group, not BC_MM.
07/26/03   PMD     Changes to suppress / remove Lint Errors
07/16/03   SUN     Free the Task in get_sts_rpt_index
07/01/03   PMD     Added initialization for new refresh_in_progress boolean flag
06/23/03   HQ      Removed compile warnings on sim_smsr.
06/05/03   PMD     Added Support for dynamic allocation of alpha_id
06/05/03   SUN     Put the task lock and fixed wms_msg_check_smsr_data
06/02/03   SUN     Added support for resetting TP-MR
06/02/03   AP      Update the message list's voice mail index and active flag
                   respective of the memory store.
05/30/03   HQ      Check for TAG_NONE in wms_cfg_update_msg_info_cache().
04/15/03   SUN     Check for invalid record ids for Status Reports
03/14/03   HQ      Fixed compiler warnings.
03/14/03   SUN     SMSR Index is filled out when the route for the status report is detremined
03/04/03   SUN     Fixed Msg Cache for RAM
02/13/03   SUN     Changed function names for status reports
02/03/03   HQ      Added support for MO SMS PS/CS domain preferences.
02/08/03   SUN     Cached the sim_smsr_sim_index, Added new function to get
                   smsr id,
                   Getthe status report list also in message list event
02/07/03   SUN     Added Support for Status Reports and Commands
                   Mainlined  FEATURE_ASYNC_SIM_INIT Cached Message list info for GW
01/30/03   SUN     Added Error Checking while unpacking data
12/04/02   HQ      Fixed problem of last used msg number in EF-SMSS by
                   incrementing it first and then save it to the card.
12/04/02   HQ      Removed compiler warnings.
11/14/02   HQ      Properly feature-ized msg info cache and dup detection to
                   resolve compile errors and warnings for GW targets.
11/13/02   AP      Added support for duplicate detection.  Fix for message
                   list info cache, copy correct data into ota structure,
                   mainlined support for message list info cache.
10/12/02   HQ      Added support of FEATURE_GWSMS_ASYNC_SIM_INIT.
09/24/02   HQ      Added support of FEATURE_CDSMS_MSG_INFO_CACHE and
                   FEATURE_GWSMS_MSG_INFO_CACHE.
09/18/02   SUN     Added cfg to the list of static variables and removed it from
                   function wms_cfg_s_ptr
08/07/02   AP      Added wms_cfg_pre_init() to initialize some values of the
                   cfg structure.
08/01/02   SUN     Fixed Compile errors without FEATURE_GWSMS_UDH_EMS Defined
07/31/02   HQ      Feature-ized sim_sms, sim_sms_unpacked under FEATURE_GWSMS.
07/31/02   SUN     Total Memory is full only when SIM and NV both are full. Call
                   wms_msg_notify_SMMA only if the MEM_FULL flag is set.
07/24/02   SUN     Made sim_sms, sim_sms_unpacked, client_ts static
07/10/02   HQ      Set msg_id to EF SMSS correctly.
06/22/02   HQ      Include wmsruim.h for FEATURE_CDSMS_RUIM.
06/15/02   HQ      Added more CDSMS code and CDMA voice mail processing.
06/03/02   HQ      Changed WMS_CFG_EVENT_SIM_READY to WMS_CFG_EVENT_GW_READY.
06/03/02   SUN     SMMA will be sent only after sim initialization is complete
05/14/02   SUN     Fixed SMMA Issue
05/07/02   HQ      Added more code for FEATURE_CDSMS and Multimode.
05/06/02   SUN     Added support for User Data Headers
04/24/02   HQ      Fixed a compiler warning about msg_s_ptr.
04/12/02   SUN     Set the in_use flag for SMMA when notification is sent
03/13/02   HQ      Added support of Multimode WMS - added FEATURE_CDSMS.
03/12/02   SUN     After clearing the MEM_FULL flag, wrote the data back
                   to SIM
03/11/02   SUN     Added support for SMMA
02/22/02   SUN     Updated used_tag_slots
02/07/02   HQ      Added CFG_EVENT_READY and CFG_EVENT_MEMORY_FULL.
01/25/02   HQ      Moved NV init to wmsnv.c; Removed CFG_EVENT_WRITE_ERROR.
01/20/02   SUN     Added support for error notification event, modified ui_sms_init
12/13/01   HQ      Renaming WMS to WMS.
12/07/01   HQ      Initial version for MSM_MM WMS.
11/28/01   Sun     Added additional check for valid route parameters
11/9/01    HQ      Cleaned up compiler warnings.
08/27/01   HQ      Initial version for FEATURE_GWSMS.
===========================================================================*/

/*===========================================================================
========================  INCLUDE FILES =====================================
===========================================================================*/

#include "comdef.h"

#if defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS)

#include "wmscfg.h"
#include "wmscl.h"
#include "msg.h"
#include "nv.h"
#include "err.h"
#include "wmssim.h"
#include "wmsnv.h"
#include "string.h"
#include "wmsts.h"
#include "wmsmsg.h"
#include "wmsruim.h"
#include "wmsdbg.h"
#include "tmc.h"
#include "wmstsi.h"

#ifdef FEATURE_CDSMS
#include "wmstscdma.h"
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_APPS_POWER_COLLAPSE
#error code not present
#endif /* FEATURE_APPS_POWER_COLLAPSE */


/*===========================================================================
======================== STATIC DATA ========================================
===========================================================================*/
static wms_cfg_s_type                    *cfg_s_ptr;
static wms_cfg_event_info_s_type         cfg_event_info;
static wms_cfg_s_type  cfg;
static wms_client_ts_data_s_type         client_ts;

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */


#ifdef FEATURE_CDSMS
static wms_OTA_message_type    cdma_ota;
static wms_tl_message_type     cdma_tl;
#endif /* FEATURE_CDSMS */

static wms_client_message_s_type     client_message;


#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_APPS_POWER_COLLAPSE
#error code not present
#endif /* FEATURE_APPS_POWER_COLLAPSE */


/*===========================================================================
============================ FUNCTIONS ======================================
===========================================================================*/

/*===========================================================================
FUNCTION wms_cfg_s_ptr

DESCRIPTION
  Return the pointer to the configuration data.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to the internal configuration data.

SIDE EFFECTS
  None.
===========================================================================*/
wms_cfg_s_type* wms_cfg_s_ptr
(
  void
)
{


  return &cfg;

} /* wms_cfg_s_ptr() */


/*===========================================================================
FUNCTION wms_cfg_filter_events

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
boolean wms_cfg_filter_events
(
  wms_client_id_type       client_id,
  wms_client_s_type        * client_ptr,
  wms_cfg_event_e_type     event
)
{
#ifdef FEATURE_APPS_POWER_COLLAPSE
#error code not present
#else /* FEATURE_APPS_POWER_COLLAPSE */
  return TRUE;
#endif /* FEATURE_APPS_POWER_COLLAPSE */

} /* wms_cfg_filter_events() */


/*===========================================================================
FUNCTION wms_cfg_event_notify

DESCRIPTION
  Notify all clients of a configuration event.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cfg_event_notify
(
  wms_cfg_event_e_type        event,
  wms_cfg_event_info_s_type   *cfg_event_info_ptr
)
{
  wms_client_s_type           *client_ptr;
  uint8                         i;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
#ifdef FEATURE_WMS_DEBUG_STRING
    MSG_SPRINTF_2(MSG_SSID_DFLT, MSG_LEGACY_HIGH, "Notify: %d %s", event, wms_dbg_get_cfg_event_string(event));
#else
    MSG_HIGH("wms_cfg_event_notify: %d ", event, 0, 0);
#endif /* FEATURE_WMS_DEBUG_STRING */

  /* Notify clients
  */
  for( i=0; i<(uint8)WMS_CLIENT_TYPE_MAX; i++ )
  {
    client_ptr = wms_client_ptr( (wms_client_id_type) i );
    if( client_ptr != NULL &&
        client_ptr->activated == TRUE &&
        client_ptr->cfg_event_cb )
    {
      if( TRUE == wms_cfg_filter_events( (wms_client_id_type) i, client_ptr, event ))
      {
        /* Send event to this client */
        client_ptr->cfg_event_cb( event, cfg_event_info_ptr );
      }
    }
  }

  /* done */
  return;

} /* wms_cfg_event_notify() */



/*===========================================================================
FUNCTION wms_cfg_set_routes_proc

DESCRIPTION
  Process the request of setting message routes.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  NV is updated.
===========================================================================*/
void wms_cfg_set_routes_proc
(
  wms_cmd_type      *cmd_ptr
)
{
  wms_cmd_err_e_type          cmd_err = WMS_CMD_ERR_NONE;
  wms_routes_s_type           *routes_ptr =
                                       &cmd_ptr->cmd.cfg_set_routes.routes;
  wms_routes_s_type           new_routes;
  int                           i;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_MED("wms_cfg_set_routes_proc()",0,0,0);

  /* Copy existing routes and update with the new routes
  */
  new_routes = cfg_s_ptr->routes;

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

  for( i=0; i<(uint8)WMS_MESSAGE_CLASS_MAX; i++ )
  {
    if( routes_ptr->pp_routes[i].route >= WMS_ROUTE_MAX )
    {
      cmd_err = WMS_CMD_ERR_CFG_ROUTE;
      break;
    }
    else if( routes_ptr->pp_routes[i].mem_store >= WMS_MEMORY_STORE_MAX )
    {
      cmd_err = WMS_CMD_ERR_CFG_MEM_STORE;
      break;
    }
    else if( routes_ptr->pp_routes[i].route != WMS_ROUTE_NO_CHANGE )
    {
      new_routes.pp_routes[i] = routes_ptr->pp_routes[i];
    }
  }

  /* Write the new routes to NV
  */
  if( cmd_err == WMS_CMD_ERR_NONE )
  {
    if( wms_nv_write_routing( & new_routes ) == FALSE )
    {
      cmd_err = WMS_CMD_ERR_CFG_NV_WRITE;
    }
  }

  wms_client_cmd_status( cmd_ptr, cmd_err );

  /* Send event
  */
  if( cmd_err == WMS_CMD_ERR_NONE )
  {
    cfg_s_ptr->routes       = new_routes;
    cfg_event_info.routes   = new_routes;
    wms_cfg_event_notify( WMS_CFG_EVENT_ROUTES, &cfg_event_info );
  }

  /* done */
  return;

} /* wms_cfg_set_routes_proc() */


/*===========================================================================
FUNCTION wms_cfg_get_routes_proc

DESCRIPTION
  Process the request of retrieving message routes.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cfg_get_routes_proc
(
  wms_cmd_type      *cmd_ptr
)
{

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_MED("wms_cfg_get_routes_proc()",0,0,0);

  wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_NONE );

  /* Set up event information
  */
  cfg_event_info.routes = cfg_s_ptr->routes;
  wms_cfg_event_notify( WMS_CFG_EVENT_ROUTES, &cfg_event_info );

  /* done */
  return;

} /* wms_cfg_get_routes_proc() */


/*===========================================================================
FUNCTION wms_cfg_get_mem_status_proc

DESCRIPTION
  Process the request of retrieving memory status of a memory store.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cfg_get_mem_status_proc
(
  wms_cmd_type      *cmd_ptr
)
{
  wms_cmd_err_e_type  cmd_err = WMS_CMD_ERR_NONE;
  wms_memory_store_e_type mem_store
              = cmd_ptr->cmd.cfg_get_mem_status.mem_store;
  wms_message_tag_e_type  tag
              = cmd_ptr->cmd.cfg_get_mem_status.tag;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_MED("wms_cfg_get_mem_status_proc()",0,0,0);

  switch( mem_store )
  {
#ifdef FEATURE_CDSMS
    //case WMS_MEMORY_STORE_RAM_CDMA,
    case WMS_MEMORY_STORE_RUIM:
    case WMS_MEMORY_STORE_NV_CDMA:
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
      break;

    default:
      cmd_err = WMS_CMD_ERR_CFG_MEM_STORE;
      break;
  }

  if( tag == WMS_TAG_MO_TEMPLATE )
  {
    switch(mem_store)
    {
#ifdef FEATURE_CDSMS
      case WMS_MEMORY_STORE_RUIM:
      case WMS_MEMORY_STORE_NV_CDMA:
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
        break;

      default:
        cmd_err = WMS_CMD_ERR_CFG_MEM_STORE;
        break;
    }
  }

  if (!WMS_IS_VALID_TAG(tag))
  {
    cmd_err = WMS_CMD_ERR_MSG_TAG;
  }

  wms_client_cmd_status( cmd_ptr, cmd_err);

  if( cmd_err == WMS_CMD_ERR_NONE )
  {
    wms_cfg_do_memory_status( mem_store, tag );
  }

} /* wms_cfg_get_mem_status_proc() */


/*===========================================================================
FUNCTION wms_cfg_do_message_list

DESCRIPTION
  Build data for message list event from a memory store.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cfg_do_message_list
(
  wms_memory_store_e_type     mem_store,
  wms_message_tag_e_type      tag
)
{
  wms_message_index_type      i;
  wms_cfg_event_info_s_type   * cfg_event_ptr = & cfg_event_info;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_LOW("wms_cfg_do_message_list()",0,0,0);


  /* Set up event information
  */
  cfg_event_ptr->message_list.len       = 0;
  cfg_event_ptr->message_list.mem_store = mem_store;
  cfg_event_ptr->message_list.tag       = tag;

  switch( mem_store )
  {
#ifdef FEATURE_CDSMS

/*
    case WMS_MEMORY_STORE_RAM_CDMA:
      for(i=0; i<WMS_MAX_RAM_MESSAGES; i++)
      {
        if( cfg_s_ptr->ram_cdma_sms[i].status == WMS_TAG_NONE )
        {
         continue;
        }

        if( tag == WMS_TAG_NONE ||
            tag == (cfg_s_ptr->ram_cdma_sms[i].status & 0x7) )
        {
          cfg_event_ptr->message_list.indices[cfg_event_ptr->message_list.len]
                    = i;
          cfg_event_ptr->message_list.tags[cfg_event_ptr->message_list.len]
                    = cfg_s_ptr->ram_cdma_sms[i].status & 0x7;
          cfg_event_ptr->message_list.len ++;
        }
      } // for
      break;
*/

#ifdef FEATURE_CDSMS_RUIM
    case WMS_MEMORY_STORE_RUIM:
      cfg_event_ptr->message_list.voice_mail_index  =
        cfg_s_ptr->cdma_voice_mail_index;
      cfg_event_ptr->message_list.voice_mail_active =
        cfg_s_ptr->cdma_voice_mail_active;
      for(i=0; i<WMS_MESSAGE_LIST_MAX && i<cfg_s_ptr->ruim_max_slots; i++)
      {
        if( cfg_s_ptr->ruim_tags[i] == WMS_TAG_NONE )
        {
         continue;
        }

        if( tag == WMS_TAG_NONE ||
            tag == cfg_s_ptr->ruim_tags[i] )
        {
          cfg_event_ptr->message_list.indices[cfg_event_ptr->message_list.len]
                    = i;
          cfg_event_ptr->message_list.tags[cfg_event_ptr->message_list.len]
                    = cfg_s_ptr->ruim_tags[i];
          cfg_event_ptr->message_list.len ++;
        }
      } // for
      break;
#endif /* FEATURE_CDSMS_RUIM */


    case WMS_MEMORY_STORE_NV_CDMA:
      cfg_event_ptr->message_list.voice_mail_index  =
        cfg_s_ptr->cdma_voice_mail_index;
      cfg_event_ptr->message_list.voice_mail_active =
        cfg_s_ptr->cdma_voice_mail_active;
      for(i=0; i<WMS_MAX_NV_CDMA_MESSAGES && i<cfg_s_ptr->nv_cdma_max_slots; i++)
      {
        if( cfg_s_ptr->nv_cdma_tags[i] == WMS_TAG_NONE )
        {
         continue;
        }

        if( tag == WMS_TAG_NONE ||
            tag == cfg_s_ptr->nv_cdma_tags[i] )
        {
          cfg_event_ptr->message_list.indices[cfg_event_ptr->message_list.len]
                    = i;
          cfg_event_ptr->message_list.tags[cfg_event_ptr->message_list.len]
                    = cfg_s_ptr->nv_cdma_tags[i];
          cfg_event_ptr->message_list.len ++;
        }
      } // for
      break;

#endif /* FEATURE_CDSMS */


#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

    default:
      MSG_ERROR("Invalid mem_store %d", mem_store, 0,0);
      break;
  }


  wms_cfg_event_notify( WMS_CFG_EVENT_MESSAGE_LIST, & cfg_event_info );

  // done
  return;

} /* wms_cfg_do_message_list() */



/*===========================================================================
FUNCTION wms_cfg_do_template_list

DESCRIPTION
  Build data for template list event from a memory store.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cfg_do_template_list
(
  wms_memory_store_e_type     mem_store
)
{
  wms_message_index_type      i = 0;
  wms_cfg_event_info_s_type   * cfg_event_ptr = & cfg_event_info;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_LOW("wms_cfg_do_template_list()",0,0,0);

  i = i; /* to remove compile warning */

  /* Set up event information
  */
  cfg_event_ptr->message_list.len       = 0;
  cfg_event_ptr->message_list.mem_store = mem_store;
  cfg_event_ptr->message_list.tag       = WMS_TAG_MO_TEMPLATE;

  switch (mem_store)
  {
#ifdef FEATURE_CDSMS
    case WMS_MEMORY_STORE_NV_CDMA:
      cfg_event_ptr->message_list.indices[0] = 0;
      cfg_event_ptr->message_list.tags[0] = WMS_TAG_MO_TEMPLATE;
      cfg_event_ptr->message_list.len = 1;
      break;

#ifdef FEATURE_CDSMS_RUIM
    case WMS_MEMORY_STORE_RUIM:
      for( i=0; i<cfg_s_ptr->ruim_max_templates; i++ )
      {
        if( cfg_s_ptr->ruim_templates[i] == TRUE )
        {
          cfg_event_ptr->message_list.indices[cfg_event_ptr->message_list.len]
                               = i;

          cfg_event_ptr->message_list.tags[cfg_event_ptr->message_list.len]
                               = WMS_TAG_MO_TEMPLATE;

          cfg_event_ptr->message_list.len ++;
        }
      }
      break;
#endif /* FEATURE_CDSMS_RUIM */

#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

    default:
      MSG_ERROR("Invalid Memory Store %d for do_template_list", mem_store, 0, 0);
      break;
  }

  wms_cfg_event_notify( WMS_CFG_EVENT_MESSAGE_LIST, & cfg_event_info );

  /* done */
  return;

} /* wms_cfg_do_template_list() */

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

/*===========================================================================
FUNCTION wms_cfg_get_msg_list_proc

DESCRIPTION
  Process the request of retrieving message list for a memory store.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cfg_get_msg_list_proc
(
  wms_cmd_type      *cmd_ptr
)
{
  wms_cmd_err_e_type  cmd_err = WMS_CMD_ERR_NONE;
  wms_memory_store_e_type mem_store
              = cmd_ptr->cmd.cfg_get_msg_list.mem_store;
  wms_message_tag_e_type  tag
              = cmd_ptr->cmd.cfg_get_msg_list.tag;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_MED("wms_cfg_get_msg_list_proc()",0,0,0);

  switch( mem_store )
  {
#ifdef FEATURE_CDSMS
    //case WMS_MEMORY_STORE_RAM_CDMA,
    case WMS_MEMORY_STORE_RUIM:
    case WMS_MEMORY_STORE_NV_CDMA:
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
      break;

    default:
      cmd_err = WMS_CMD_ERR_CFG_MEM_STORE;
      break;
  }

  if( tag == WMS_TAG_MO_TEMPLATE )
  {
    switch(mem_store)
    {
#ifdef FEATURE_CDSMS
      case WMS_MEMORY_STORE_RUIM:
      case WMS_MEMORY_STORE_NV_CDMA:
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
        break;

      default:
        cmd_err = WMS_CMD_ERR_CFG_MEM_STORE;
        break;
    }
  }

  if (( tag == WMS_TAG_STATUS_RPT )
      &&( mem_store != WMS_MEMORY_STORE_SIM ))
  {
    cmd_err = WMS_CMD_ERR_CFG_MEM_STORE;
  }

  if (!WMS_IS_VALID_TAG(cmd_ptr->cmd.cfg_get_msg_list.tag))
  {
    cmd_err = WMS_CMD_ERR_MSG_TAG;
  }

  wms_client_cmd_status( cmd_ptr, cmd_err);

  if (cmd_err == WMS_CMD_ERR_NONE)
  {
    if( tag == WMS_TAG_MO_TEMPLATE )
    {
      wms_cfg_do_template_list( mem_store );
    }
#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
    else
    {
      wms_cfg_do_message_list( mem_store, tag );
    }
  }

  /* done */
  return;

} /* wms_cfg_get_msg_list_proc() */


#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

/*===========================================================================
FUNCTION wms_cfg_set_primary_client_proc

DESCRIPTION
  Set the Primary Client GW Domain Preference Procedure

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wms_cfg_set_primary_client_proc
(
  wms_cmd_type      *cmd_ptr
)
{
  wms_cmd_err_e_type   cmd_err = WMS_CMD_ERR_NONE;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_MED("wms_cfg_set_primary_client_proc()",0,0,0);

  if (cfg_s_ptr == NULL)
  {
    cmd_err = WMS_CMD_ERR_NO_RESOURCE;
  }
  else
  {
    /* Check for Valid Client */
    if (cmd_ptr->hdr.client_id >= (uint8)WMS_CLIENT_TYPE_MAX)
    {
      cmd_err = WMS_CMD_ERR_CLIENT_ID;
    }
    else
    {
      if (cmd_ptr->cmd.cfg_set_primary_client.set_primary == TRUE)
      {
        /* Register a Primary Client */
        MSG_HIGH("Setting Primary Client: %d", cmd_ptr->hdr.client_id, 0, 0);
        cfg_s_ptr->primary_client     = cmd_ptr->hdr.client_id;    /* Register */
        cfg_s_ptr->use_client_memory  = cmd_ptr->cmd.cfg_set_primary_client.use_client_memory;
        cfg_s_ptr->client_memory_full = FALSE;
      }
      else
      {
        /* De-Register a Primary Client */
        if (cfg_s_ptr->primary_client != cmd_ptr->hdr.client_id)
        {
          /* Only Primary Client can De-Register Itself */
          cmd_err = WMS_CMD_ERR_CFG_UNPRIVILEGED_CLIENT;
        }
        else
        {
          MSG_HIGH("Resetting Primary Client: %d", cmd_ptr->hdr.client_id, 0, 0);
          cfg_s_ptr->primary_client     = (uint8)WMS_CLIENT_TYPE_MAX;  /*DeRegistered */
          cfg_s_ptr->use_client_memory  = FALSE;
          cfg_s_ptr->client_memory_full = FALSE;
        }
      }
    }
  }

  /* Notify Command Error in Command Callback */
  wms_client_cmd_status( cmd_ptr, cmd_err );

  if( cmd_err == WMS_CMD_ERR_NONE )
  {
    /* Event Information */
    cfg_event_info.set_primary.set_primary       = cmd_ptr->cmd.cfg_set_primary_client.set_primary;
    cfg_event_info.set_primary.use_client_memory = cmd_ptr->cmd.cfg_set_primary_client.use_client_memory;

    /* Notify Event */
    wms_cfg_event_notify( WMS_CFG_EVENT_PRIMARY_CLIENT_SET, & cfg_event_info );
  }

  /* done */
  return;

} /* wms_cfg_set_primary_client_proc() */

/*===========================================================================
FUNCTION wms_cfg_set_memory_full_proc

DESCRIPTION
  Set the Memory Full Status for the Primary Client

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wms_cfg_set_memory_full_proc
(
  wms_cmd_type      *cmd_ptr
)
{
  wms_cmd_err_e_type   cmd_err = WMS_CMD_ERR_NONE;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_MED("wms_cfg_set_memory_full_proc()",0,0,0);

  if (cfg_s_ptr == NULL)
  {
    cmd_err = WMS_CMD_ERR_NO_RESOURCE;
  }
  else
  {
    /* Check for Valid Client */
    if (cmd_ptr->hdr.client_id >= (uint8)WMS_CLIENT_TYPE_MAX)
    {
      cmd_err = WMS_CMD_ERR_CLIENT_ID;
    }
    else
    {
      /* Set / Reset Memory Full for Primary Client */
      if (cfg_s_ptr->primary_client != cmd_ptr->hdr.client_id)
      {
        /* Only Primary Client can Set / Reset Memory Full */
        cmd_err = WMS_CMD_ERR_CFG_UNPRIVILEGED_CLIENT;
      }
      else
      {
        cfg_s_ptr->client_memory_full = cmd_ptr->cmd.cfg_set_memory_full.memory_full;
      }
    }
  }

  /* Notify Command Error in Command Callback */
  wms_client_cmd_status( cmd_ptr, cmd_err );

  if( cmd_err == WMS_CMD_ERR_NONE )
  {
    /* Event Information */
    cfg_event_info.memory_full = cmd_ptr->cmd.cfg_set_memory_full.memory_full;

    /* Notify Event */
    wms_cfg_event_notify( WMS_CFG_EVENT_MEMORY_STATUS_SET, & cfg_event_info );

    /* Send SMMA to the Network */
#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
  }

  /* done */
  return;

} /* wms_cfg_set_memory_full_proc() */

/*===========================================================================
FUNCTION wms_cfg_do_memory_full

DESCRIPTION
  Send the Memory Full Event for the given Memory Store

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wms_cfg_do_memory_full
(
  wms_memory_store_e_type mem_store
)
{
  wms_cfg_event_info_s_type   * cfg_event_ptr = & cfg_event_info;
  cfg_event_ptr->mem_store = mem_store;
  wms_cfg_event_notify(WMS_CFG_EVENT_MEMORY_FULL,&cfg_event_info);
}

/*===========================================================================
FUNCTION wms_cfg_do_total_memory_full

DESCRIPTION
  Process the complete Memory Full / Available scenario by writing Flag 
  to SIM and notifying SMMA to the Network.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wms_cfg_do_total_memory_full
(
  void
)
{
#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

}


#ifdef FEATURE_CDSMS
/*===========================================================================
FUNCTION wms_cfg_do_cdma_ready_event

DESCRIPTION
  Notify CDMA Ready to all WMS Clients

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wms_cfg_do_cdma_ready_event
(
  void
)
{
  cfg_s_ptr->cdma_init_complete = TRUE;

  wms_cfg_event_notify( WMS_CFG_EVENT_CDMA_READY, &cfg_event_info );
}
#endif /* FEATURE_CDSMS */



#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

/*===========================================================================
FUNCTION wms_cfg_process_cmd

DESCRIPTION
  Dispatch the configuration command processing.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cfg_process_cmd
(
  wms_cmd_type  *cmd_ptr
)
{

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch( cmd_ptr->hdr.cmd_id )
  {
    case WMS_CMD_CFG_SET_ROUTES:
      wms_cfg_set_routes_proc( cmd_ptr );
      break;

    case WMS_CMD_CFG_GET_ROUTES:
      wms_cfg_get_routes_proc( cmd_ptr );
      break;

    case WMS_CMD_CFG_GET_MEMORY_STATUS:
      wms_cfg_get_mem_status_proc( cmd_ptr );
      break;

    case WMS_CMD_CFG_GET_MESSAGE_LIST:
      wms_cfg_get_msg_list_proc( cmd_ptr );
      break;

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

    case WMS_CMD_CFG_SET_PRIMARY_CLIENT:
      wms_cfg_set_primary_client_proc( cmd_ptr );
      break;

    case WMS_CMD_CFG_SET_MEMORY_FULL:
      wms_cfg_set_memory_full_proc( cmd_ptr );
      break;

    default:
      MSG_ERROR("Invalid cmd: %d", cmd_ptr->hdr.cmd_id, 0,0);
      break;
  }

  /* done */
  return;

} /* wms_cfg_process_cmd() */

/*===========================================================================
FUNCTION wms_cfg_pre_init

DESCRIPTION
  Do Pre-configuration-related initialization.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  Pre-Configuration data initialized.
===========================================================================*/
void wms_cfg_pre_init
(
  void
)
{
  cfg_s_ptr = wms_cfg_s_ptr();

#ifdef FEATURE_GSTK
  cfg_s_ptr->refresh_registered  = FALSE;
  cfg_s_ptr->refresh_in_progress = FALSE;
#endif /* FEATURE_GSTK */

#ifdef FEATURE_CDSMS
  cfg_s_ptr->cdma_init_complete = FALSE;
  cfg_s_ptr->nv_cdma_max_slots  = 0;
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
}

/*===========================================================================
FUNCTION wms_cfg_init

DESCRIPTION
  Do configuration-related initialization.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  Configuration data initialized.
===========================================================================*/
void wms_cfg_init
(
  void
)
{
  uint32            i = 0;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_HIGH("wms_cfg_init()",0,0,0);
  i = i; /* to remove compiler warning */

  /* cfg_s_ptr is initialized in wms_cfg_pre_init() */

  if( wms_nv_read_routing( & cfg_s_ptr->routes ) == FALSE )
  {
    /* Message routes init: these are the default settings
    */
#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_CDSMS
    cfg_s_ptr->routes.pp_routes[WMS_MESSAGE_CLASS_CDMA].route
            = WMS_ROUTE_STORE_AND_NOTIFY;
    cfg_s_ptr->routes.pp_routes[WMS_MESSAGE_CLASS_CDMA].mem_store
            = WMS_MEMORY_STORE_NV_CDMA;
#endif /* FEATURE_CDSMS */

    /* try to write it back
    */
    (void) wms_nv_write_routing( & cfg_s_ptr->routes );
  }


#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */



#ifdef FEATURE_CDSMS

#ifdef FEATURE_CDSMS_RUIM
  cfg_s_ptr->ruim_sms_rec_len               = 0;
  cfg_s_ptr->ruim_last_message_number       = 0;
  cfg_s_ptr->ruim_last_wap_message_number   = 0;
  cfg_s_ptr->ruim_mem_full_flag             = WMS_SIM_MEM_FULL_CLEAR;

  /* will send CDMA_READY event when RUIM init is finished */

#else /* FEATURE_CDSMS_RUIM */

  /* Without RUIM, init is finised, send the ready event to clients
  */
  //wms_cfg_do_cdma_ready_event();

#endif /* FEATURE_CDSMS_RUIM */

#ifdef FEATURE_JCDMA_2
  {
    nv_item_type  nvi;
    if( NV_DONE_S == wms_nv_read_wait( NV_ESN_I, & nvi ) )
    {
      cfg_s_ptr->phone_esn = nvi.esn.esn;
    }
    else
    {
      MSG_ERROR("Reading ESN failed!",0,0,0);
      cfg_s_ptr->phone_esn = 0;
    }
  }
#endif /* FEATURE_JCDMA_2 */


#endif /* FEATURE_CDSMS */

  /* Initialization for SMMA Notification using Client Memory */
  cfg_s_ptr->primary_client          = (uint8)WMS_CLIENT_TYPE_MAX; /* No Client Registered */
  cfg_s_ptr->use_client_memory       = FALSE;
  cfg_s_ptr->client_memory_full      = FALSE;

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

  /* Initialize Bearer Prefs */
  if( wms_nv_read_bearer_prefs( cfg_s_ptr->bearer_prefs ) == FALSE )
  {
    MSG_ERROR("NV read for bearer prefs failed -- reset all to ALLOWED!!",0,0,0);
    /* NV read failed; make default usage to be 'ALLOWED'/ used */
    for( i=0; i < WMS_BEARER_MAX; i++ )
    {
      cfg_s_ptr->bearer_prefs[i] = WMS_BEARER_USAGE_ALLOWED;
    }
#ifdef FEATURE_SMS_OVER_IP
    /* By default, prefer EVDO if SMS over IP is enabled */
    MSG_ERROR("Setting WMS_BEARER_CDMA_EVDO as PREFERRED bearer!!",0,0,0);
    cfg_s_ptr->bearer_prefs[WMS_BEARER_CDMA_EVDO] = WMS_BEARER_USAGE_PREFERRED;
#endif /* FEATURE_SMS_OVER_IP */
  }
  else
  {
    for( i=0; i < WMS_BEARER_MAX; i++ )
    {
      if( cfg_s_ptr->bearer_prefs[i] >= WMS_BEARER_USAGE_MAX )
      {
        /* bad usage value; make default usage to be 'used' */
        cfg_s_ptr->bearer_prefs[i] = WMS_BEARER_USAGE_ALLOWED;
        MSG_ERROR("Corrupt value? Resetting bearer_prefs[%d]=%d to"
                " WMS_BEARER_USAGE_ALLOWED(%d)", 
                i, cfg_s_ptr->bearer_prefs[i], WMS_BEARER_USAGE_ALLOWED);
      }
    }
  }
  (void) wms_nv_write_bearer_prefs( cfg_s_ptr->bearer_prefs );

  memset( cfg_s_ptr->active_bearers, FALSE, sizeof(cfg_s_ptr->active_bearers) );

  cfg_s_ptr->last_used_bearer = WMS_BEARER_MAX;

#ifdef FEATURE_APPS_POWER_COLLAPSE
#error code not present
#endif /* FEATURE_APPS_POWER_COLLAPSE */

  /* done */
  return;

} /* wms_cfg_init */


#ifdef FEATURE_CDSMS
/*===========================================================================
FUNCTION wms_cfg_get_ruim_next_message_number

DESCRIPTION
  Get the next message number from RUIM to use for an MO message.

DEPENDENCIES
  none

RETURN VALUE
  The next message number to use

SIDE EFFECTS
  The current message number is incremented.
===========================================================================*/
wms_message_number_type wms_cfg_get_ruim_next_message_number
(
  void
)
{
  wms_message_number_type     mn;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#ifdef FEATURE_CDSMS_RUIM
  uint8                       data[5];

  mn = ++ cfg_s_ptr->ruim_last_message_number;

  if (cfg_s_ptr->ruim_sms_rec_len != 0)
  {
    data[0]   = (uint8) ( cfg_s_ptr->ruim_last_message_number >> 8 );
    data[1]   = (uint8) ( cfg_s_ptr->ruim_last_message_number & 0xFF );

    data[2]   = (uint8) ( cfg_s_ptr->ruim_last_wap_message_number >> 8 );
    data[3]   = (uint8) ( cfg_s_ptr->ruim_last_wap_message_number & 0xFF );

    data[4]   = cfg_s_ptr->ruim_mem_full_flag;

    if( wms_ruim_write_smss( data ) == FALSE )
    {
      MSG_ERROR("SIM write error", 0,0,0 );
    }
  }
  else
  {
    MSG_ERROR("Unable to access R-UIM, ruim was not initialized",0,0,0);
  }
#else
  mn = ++ cfg_s_ptr->ruim_last_message_number;
#endif /* FEATURE_CDSMS_RUIM */

  return mn;

} /* wms_cfg_get_ruim_next_message_number() */
#endif /* FEATURE_CDSMS */


#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */


/*===========================================================================
FUNCTION wms_cfg_do_memory_status

DESCRIPTION
  Build data for memory status event.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cfg_do_memory_status
(
  wms_memory_store_e_type   mem_store,
  wms_message_tag_e_type    tag
)
{
  wms_message_index_type      i;

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  cfg_event_info.memory_status.mem_store      = mem_store;
  cfg_event_info.memory_status.tag            = tag;
  cfg_event_info.memory_status.free_slots     = 0;
  cfg_event_info.memory_status.used_tag_slots = 0;

  if( tag == WMS_TAG_MO_TEMPLATE )
  {
    /********************/
    /* Handle templates */
    /********************/
    switch( mem_store )
    {
#ifdef FEATURE_CDSMS

      case WMS_MEMORY_STORE_NV_CDMA:
        // TBD: support multiple templates in NV in the future
        {
          cfg_event_info.memory_status.max_slots      = 1;
          cfg_event_info.memory_status.free_slots     = 0;
          cfg_event_info.memory_status.used_tag_slots = 1;
        }
        break;

#ifdef FEATURE_CDSMS_RUIM
      case WMS_MEMORY_STORE_RUIM:
        cfg_event_info.memory_status.max_slots = cfg_s_ptr->ruim_max_templates;

        for( i=0; i<cfg_s_ptr->ruim_max_templates; i++ )
        {
          if( cfg_s_ptr->ruim_templates[i] == FALSE )
          {
            cfg_event_info.memory_status.free_slots ++;
          }
          else
          {
            cfg_event_info.memory_status.used_tag_slots ++;
          }
        }
        break;
#endif /* FEATURE_CDSMS_RUIM */
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

      default:
        MSG_ERROR("Invalid mem store %d", mem_store, 0,0);
        break;
    }
  }
#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
  else
  {
    /***************************/
    /* Handle regular messages */
    /***************************/
    switch( mem_store )
    {
#ifdef FEATURE_CDSMS
/*
      case WMS_MEMORY_STORE_RAM_CDMA:
        cfg_event_info.memory_status.max_slots  = WMS_MAX_RAM_MESSAGES;

        for(i=0; i<WMS_MAX_RAM_MESSAGES; i++)
        {
          if( cfg_s_ptr->ram_cdma_sms[i].status == WMS_TAG_NONE )
          {
            cfg_event_info.memory_status.free_slots ++;
          }
          else if( tag==WMS_TAG_NONE ||
                   (cfg_s_ptr->ram_cdma_sms[i].status & 0x7) == tag )
          {
            cfg_event_info.memory_status.used_tag_slots ++;
          }
        } // for
        break;
*/

#ifdef FEATURE_CDSMS_RUIM
      case WMS_MEMORY_STORE_RUIM:
        cfg_event_info.memory_status.max_slots = cfg_s_ptr->ruim_max_slots;

        for( i=0; i<cfg_s_ptr->ruim_max_slots; i++ )
        {
          if( cfg_s_ptr->ruim_tags[i] == WMS_TAG_NONE )
          {
            cfg_event_info.memory_status.free_slots ++;
          }
          else if( (tag ==WMS_TAG_NONE ) ||
                   ((cfg_s_ptr->ruim_tags[i] & 0x7) == tag ))
          {
            cfg_event_info.memory_status.used_tag_slots ++;
          }
        }
        break;
#endif /* FEATURE_CDSMS_RUIM */

      case WMS_MEMORY_STORE_NV_CDMA:
        cfg_event_info.memory_status.max_slots = cfg_s_ptr->nv_cdma_max_slots;

        for( i=0; i<cfg_s_ptr->nv_cdma_max_slots; i++ )
        {
          if( cfg_s_ptr->nv_cdma_tags[i] == WMS_TAG_NONE )
          {
            cfg_event_info.memory_status.free_slots ++;
          }
          else if( (uint8)(tag == WMS_TAG_NONE ) ||
                   ( (uint8)((uint8)cfg_s_ptr->nv_cdma_tags[i] & 0x7) == (uint8)tag ))
          {
            cfg_event_info.memory_status.used_tag_slots ++;
          }
        }
        break;

#endif /* FEATURE_CDSMS */


#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

      default:
        MSG_ERROR("Invalid mem store %d", mem_store, 0,0);
        break;
    }
  }


  wms_cfg_event_notify( WMS_CFG_EVENT_MEMORY_STATUS,
                          & cfg_event_info );

  /* done */
  return;

} /* wms_cfg_do_memory_status() */


#ifdef FEATURE_CDSMS
/*===========================================================================
FUNCTION wms_cfg_determine_cdma_route

DESCRIPTION
  Determine CDMA message route according to message class.

DEPENDENCIES
  none

RETURN VALUE
  The routing.

SIDE EFFECTS
  None.
===========================================================================*/
wms_routing_s_type wms_cfg_determine_cdma_route
(
  wms_teleservice_e_type   teleservice
)
{
  wms_routing_s_type  route;

#ifdef FEATURE_CDSMS_BROADCAST
  if( teleservice == WMS_TELESERVICE_BROADCAST )
  {
    route = cfg_s_ptr->routes.bc_routes[WMS_MESSAGE_CLASS_CDMA];
  }
  else
#endif /* FEATURE_CDSMS_BROADCAST */
  {
    route = cfg_s_ptr->routes.pp_routes[WMS_MESSAGE_CLASS_CDMA];
  }

  return route;
}

/*===========================================================================
FUNCTION wms_cfg_cdma_update_voice_mail_index

DESCRIPTION
  Update CDMA voice mail info

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cfg_cdma_update_voice_mail_index
(
  wms_format_e_type         format,
  wms_message_tag_e_type    tag,
  uint16                    data_len,
  const uint8                     * data,
  wms_message_index_type    i
)
{

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( ! WMS_IS_MT( tag ) )
  {
    return;
  }

  cdma_ota.format = format;
  cdma_ota.data_len = MIN(data_len, WMS_MAX_LEN);
  memcpy( (uint8*) cdma_ota.data, data, cdma_ota.data_len );

  if( wms_ts_decode_OTA( & cdma_ota, & cdma_tl ) != WMS_OK_S )
  {
    MSG_ERROR("NV msg %d decoding error!",i,0,0);
    return;
  }

  if( (cdma_tl.mask & WMS_MASK_TL_BEARER_DATA) &&
      (cdma_tl.cl_bd.mask & WMS_MASK_BD_NUM_OF_MSGS) )
  {
    /* This is a voice mail msg
    */
    cfg_s_ptr->cdma_voice_mail_index = i;
    cfg_s_ptr->cdma_voice_mail_active =
                              (cdma_tl.cl_bd.num_messages==0) ? FALSE : TRUE;
  }

  /* Done
  */
  return;

} /* wms_cfg_cdma_update_voice_mail_index() */

#endif /* FEATURE_CDSMS */

/*===========================================================================
FUNCTION wms_cfg_locate_voicemail_line2

DESCRIPTION
  Locate the Voicemail Line 2 slot information. Applicable only for GW 
  with ALS Feature.

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None.
===========================================================================*/
boolean wms_cfg_locate_voicemail_line2
(
  wms_message_index_type            *sms_index   /* INPUT/OUTPUT */
)
{
  MSG_MED("wms_cfg_locate_voicemail_line2 function is called",0,0,0);

#if defined (FEATURE_CPHS) && defined(FEATURE_ALS)
#error code not present
#else /* defined (FEATURE_CPHS) && defined(FEATURE_ALS) */
  return FALSE;
#endif /* defined (FEATURE_CPHS) && defined(FEATURE_ALS) */
}

/*===========================================================================
FUNCTION wms_cfg_check_cphs_msg

DESCRIPTION
  Check for CPHS Message. Applicable only for GW with CPHS Feature.

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None.
===========================================================================*/
boolean wms_cfg_check_cphs_msg
(
   const wms_address_s_type     *addr
)
{
  MSG_MED("wms_cfg_check_cphs_msg function is called",0,0,0);

#ifdef FEATURE_CPHS
#error code not present
#endif /* FEATURE_CPHS */

  return FALSE;

}

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

/*===========================================================================
FUNCTION wms_cfg_get_sts_rpt_index

DESCRIPTION
  Get the Status Report Index for the given SMS Message having the same
  Message Reference.

DEPENDENCIES
  none

RETURN VALUE
  The routing.

SIDE EFFECTS
  None.
===========================================================================*/
boolean wms_cfg_get_sts_rpt_index
(
  wms_message_index_type sms_index, /*IN*/
  wms_message_index_type *smsr_index /*OUT*/
)
{
  boolean msg_found = FALSE;
#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
  return msg_found;
}

/*===========================================================================
FUNCTION wms_cfg_locate_mo_msg_for_sts_report

DESCRIPTION
  Get the MO SMS Index for the given Message Reference.

DEPENDENCIES
  none

RETURN VALUE
  The routing.

SIDE EFFECTS
  None.
===========================================================================*/
boolean wms_cfg_locate_mo_msg_for_sts_report
(
  wms_message_index_type *rec_index,
  wms_message_number_type  message_ref
)
{
  boolean msg_found = FALSE;
#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
  return msg_found;
}

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

/*===========================================================================
FUNCTION wms_cfg_activate_client

DESCRIPTION
  Activate the WMS Client

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cfg_activate_client
(
  wms_client_id_type client_id
)
{
  wms_client_s_type           *client_ptr;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_HIGH("cfg_activate_client: %d", client_id, 0,0);


  /* Notify clients
  */
  client_ptr = wms_client_ptr( client_id );

  if( client_ptr != NULL &&
      client_ptr->activated == TRUE &&
      client_ptr->cfg_event_cb )
  {
#ifdef FEATURE_CDSMS
    if((cfg_s_ptr != NULL) && cfg_s_ptr->cdma_init_complete )
    {
      client_ptr->cfg_event_cb( WMS_CFG_EVENT_CDMA_READY, & cfg_event_info );
    }
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_CDSMS */
  }

  /* done */
  return;

} /* wms_cfg_activate_client() */

#ifdef FEATURE_CDSMS
/*===========================================================================
FUNCTION wms_cfg_delete_dup_info_cache

DESCRIPTION
  Deletes an entry in the duplicate detection message info cache.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cfg_delete_dup_info_cache
(
  wms_memory_store_e_type         mem_store,
  uint32                          index
)
{
#ifdef FEATURE_CDSMS_DUP_DETECTION
  uint8 i;

  /* Find the entry to delete from the cache */
  for(i=0; i<WMS_MESSAGE_LIST_MAX; i++)
  {
    if((cfg_s_ptr->dup_detect[i].mem_store == mem_store) &&
       (cfg_s_ptr->dup_detect[i].index == index))
    {
      /* Simply set the masks to 0 to signify an empty slot */
      cfg_s_ptr->dup_detect[i].tl_mask = 0;
      cfg_s_ptr->dup_detect[i].cl_mask = 0;

      /* break early */
      break;
    }
  }
#endif /* FEATURE_CDSMS_DUP_DETECTION */
} /* wms_cfg_delete_dup_info_cache */

/*===========================================================================
FUNCTION wms_cfg_update_dup_info_cache
 
DESCRIPTION
  Updates the duplicate detection message info cache.
 
DEPENDENCIES
  None
 
RETURN VALUE
  None
 
SIDE EFFECTS
  None.
===========================================================================*/
/* Updates the duplicate detection message info cache.
*/
void wms_cfg_update_dup_info_cache
(
  wms_memory_store_e_type         mem_store,
  uint32                          index,
  const wms_tl_message_type             *new_tl_msg
)
{
#ifdef FEATURE_CDSMS_DUP_DETECTION
  uint8 i;

  if( new_tl_msg->teleservice == WMS_TELESERVICE_WAP
#ifdef FEATURE_GSM1x
#error code not present
#endif /* FEATURE_GSM1x */
    )
  {
    /* No dup detection to be done for the above teleservices
    */
    return;
  }

  /* Find an empty slot for updating */
  for(i=0; i<WMS_MESSAGE_LIST_MAX; i++)
  {
    if((cfg_s_ptr->dup_detect[i].tl_mask == 0) &&
       (cfg_s_ptr->dup_detect[i].cl_mask == 0))
    {
      /* found an empty slot, break early */
      break;
    }
  }

  /* Update the memory store and index */
  cfg_s_ptr->dup_detect[i].mem_store  = mem_store;
  cfg_s_ptr->dup_detect[i].index      = index;

  /* Fill the info cache with the contents from the message being saved. */
  cfg_s_ptr->dup_detect[i].tl_mask = new_tl_msg->mask;

  cfg_s_ptr->dup_detect[i].cl_mask = new_tl_msg->cl_bd.mask;

  if(new_tl_msg->mask & WMS_MASK_TL_ADDRESS)
  {
    memcpy((void *)&cfg_s_ptr->dup_detect[i].address,
           (void *)&new_tl_msg->address,
           sizeof(wms_address_s_type));
  }

  if(new_tl_msg->mask & WMS_MASK_TL_SUBADDRESS)
  {
    memcpy((void *)&cfg_s_ptr->dup_detect[i].sub_address,
           (void *)&new_tl_msg->subaddress,
           sizeof(wms_subaddress_s_type));
  }

  if(new_tl_msg->mask & WMS_MASK_TL_BEARER_DATA)
  {
    if(new_tl_msg->cl_bd.mask & WMS_MASK_BD_MSG_ID)
    {
      memcpy((void *)&cfg_s_ptr->dup_detect[i].msg_id,
             (void *)&new_tl_msg->cl_bd.message_id,
             sizeof(wms_message_id_s_type));
    }

    if(new_tl_msg->cl_bd.mask & WMS_MASK_BD_MC_TIME)
    {
      memcpy((void *)&cfg_s_ptr->dup_detect[i].mc_time,
             (void *)&new_tl_msg->cl_bd.mc_time,
             sizeof(wms_timestamp_s_type));
    }
  }
#endif /* FEATURE_CDSMS_DUP_DETECTION */
}

#endif /* FEATURE_CDSMS */


#if defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS_CACHE)
/*===========================================================================
FUNCTION wms_cfg_update_msg_info_cache

DESCRIPTION
  Updates the message info cache.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cfg_update_msg_info_cache
(
  wms_message_tag_e_type    tag,
  wms_memory_store_e_type   mem_store,
  wms_message_index_type    index,
  uint8                     * data,
  uint8                     len
)
{
  wms_status_e_type           st = WMS_OK_S;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( wms_msg_info_cache_cb == NULL )
  {
    return;
  }


  client_message.msg_hdr.mem_store    = mem_store;
  client_message.msg_hdr.index        = index;

  if( tag == WMS_TAG_MO_TEMPLATE )
  {
    client_message.msg_hdr.tag          = WMS_TAG_MO_TEMPLATE;

    switch( mem_store )
    {
#ifdef FEATURE_GWSMS_CACHE
#error code not present
#endif/*FEATURE_GWSMS_CACHE*/

#ifdef FEATURE_CDSMS
#if (defined(FEATURE_CDSMS_RUIM) && defined(FEATURE_CDSMS_CACHE))
      case WMS_MEMORY_STORE_RUIM:
        client_message.msg_hdr.message_mode = WMS_MESSAGE_MODE_CDMA;
        if( wms_ts_cdma_decode_smsp( data,
                                     len,
                                     & client_message.u.cdma_template ) == FALSE )
        {
          st = WMS_GENERAL_ERROR_S;
        }
        else
        {
          /* Update the cache
          */
          wms_msg_info_cache_cb( & client_message,
                                   cfg_s_ptr->ruim_template_info_cache[index] );
        }
        break;
#endif /* defined(FEATURE_CDSMS_RUIM) && defined(FEATURE_CDSMS_CACHE) */
#endif /* FEATURE_CDSMS */

      default:
        break;
    }
  }
#ifdef FEATURE_GWSMS_CACHE
#error code not present
#endif /* FEATURE_GWSMS_CACHE */
  else /* not template */
  {
    switch( mem_store )
    {
#ifdef FEATURE_GWSMS_CACHE
#error code not present
#endif /*FEATURE_GWSMS_CACHE*/

#ifdef FEATURE_CDSMS
#if (defined(FEATURE_CDSMS_RUIM) && defined(FEATURE_CDSMS_CACHE))
      case WMS_MEMORY_STORE_RUIM:
        client_message.msg_hdr.message_mode = WMS_MESSAGE_MODE_CDMA;
        client_message.msg_hdr.tag          = (wms_message_tag_e_type)data[0];

        if( client_message.msg_hdr.tag == WMS_TAG_NONE)
        {
          break;
        }

        cdma_ota.format      = WMS_FORMAT_CDMA;
        cdma_ota.data_len    = MIN (data[1], WMS_MAX_LEN-2);
        memcpy( (uint8*) cdma_ota.data, data+2, cdma_ota.data_len );

        st = wms_ts_decode_CDMA_tl( & cdma_ota,
                                    & cdma_tl,
                                    & client_message.u.cdma_message.raw_ts );
        if( st == WMS_OK_S )
        {
          wms_ts_convert_tl2cl( & cdma_tl, & client_message.u.cdma_message );

          /* Update the cache
          */
          wms_msg_info_cache_cb( & client_message,
                                        cfg_s_ptr->ruim_sms_info_cache[index] );
        }
        break;
#endif /* defined(FEATURE_CDSMS_RUIM) && defined(FEATURE_CDSMS_CACHE) */

      case WMS_MEMORY_STORE_NV_CDMA:
        client_message.msg_hdr.message_mode = WMS_MESSAGE_MODE_CDMA;
        client_message.msg_hdr.tag          = (wms_message_tag_e_type) data[1];

        if( client_message.msg_hdr.tag == WMS_TAG_NONE)
        {
          break;
        }

        cdma_ota.format        = (wms_format_e_type) data[0];
        cdma_ota.data_len      = MIN (data[2], WMS_MAX_LEN-3);
        memcpy( (uint8*) cdma_ota.data, data+3, cdma_ota.data_len );

        st = wms_ts_decode_CDMA_tl( & cdma_ota,
                                    & cdma_tl,
                                    & client_message.u.cdma_message.raw_ts );
        if( st == WMS_OK_S )
        {
          wms_ts_convert_tl2cl( & cdma_tl, & client_message.u.cdma_message );

#ifdef FEATURE_CDSMS_CACHE
          /* Update the cache
          */
          wms_msg_info_cache_cb( & client_message,
                                         cfg_s_ptr->nv_cdma_info_cache[index] );
#endif /* FEATURE_CDSMS_CACHE */
        }
        break;
#endif /* FEATURE_CDSMS */

      default:
        break;
    }
  }

  st = st; /* remove lint warning */
  
  /* done */
  return;
} /* wms_cfg_update_msg_info_cache() */

#endif /*FEATURE_CDSMS || FEATURE_GWSMS_CACHE*/

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

/*===========================================================================
FUNCTION wms_cfg_check_voicemail_contents

DESCRIPTION
  Check the Message Content for Voicemail Information.

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None.
===========================================================================*/
boolean wms_cfg_check_voicemail_contents
(
  wms_message_mode_e_type         msg_mode,
  const wms_raw_ts_data_s_type    *raw_ts,              /* IN  */
  boolean                         *is_voicemail_active, /* OUT */
  uint8                           *count                /* OUT */
)
{
  boolean is_voicemail = FALSE;
  boolean is_vm_active    = FALSE;
  uint8   num_of_msgs  = 0;

#ifdef FEATURE_SMS_UDH
  int j=0;
#endif /*FEATURE_SMS_UDH*/

  /* Early return for null pointer or
  ** decode the message to check for voicemail contents.
  */
  if((raw_ts == NULL) ||
     (wms_ts_decode(raw_ts, &client_ts) != WMS_OK_S))
  {

    MSG_HIGH("Can not perform voicemail check!",0,0,0);
    return is_voicemail;
  }

  /* Check CDMA voicemail */
  if(msg_mode == WMS_MESSAGE_MODE_CDMA)
  {
    if(client_ts.u.cdma.mask & WMS_MASK_BD_NUM_OF_MSGS)
    {
      /* This is a voicemail message */
      is_voicemail  = TRUE;

      /* Assign the number of messages */
      num_of_msgs   = client_ts.u.cdma.num_messages;

      /* Active means there is voicemail waiting on the server */
      is_vm_active  = (client_ts.u.cdma.num_messages != 0) ? TRUE : FALSE;
    }
  }
  /* Check GW voicemail */
  else if ((msg_mode == WMS_MESSAGE_MODE_GW)&&(raw_ts->format == WMS_FORMAT_GW_PP))
  {
    if (client_ts.u.gw_pp.tpdu_type == WMS_TPDU_DELIVER &&
       (client_ts.u.gw_pp.u.deliver.pid == WMS_PID_RETURN_CALL ||
       ((client_ts.u.gw_pp.u.deliver.dcs.msg_waiting == WMS_GW_MSG_WAITING_STORE 
       ||client_ts.u.gw_pp.u.deliver.dcs.msg_waiting == WMS_GW_MSG_WAITING_DISCARD) 
          &&(client_ts.u.gw_pp.u.deliver.dcs.msg_waiting_kind == WMS_GW_MSG_WAITING_VOICEMAIL))))
    {
      is_voicemail = TRUE;

      if(client_ts.u.gw_pp.u.deliver.dcs.msg_waiting
         != WMS_GW_MSG_WAITING_NONE)
      {
        is_vm_active = client_ts.u.gw_pp.u.deliver.dcs.msg_waiting_active;
      }
    }

#ifdef FEATURE_CPHS
#error code not present
#endif /* FEATURE_CPHS */

#ifdef FEATURE_SMS_UDH
    for (j=0;j<client_ts.u.gw_pp.u.deliver.user_data.num_headers;j++)
    {
      if(client_ts.u.gw_pp.u.deliver.user_data.headers[j].header_id == WMS_UDH_SPECIAL_SM
         && ((client_ts.u.gw_pp.u.deliver.user_data.headers[j].u.special_sm.msg_waiting == WMS_GW_MSG_WAITING_STORE 
            || client_ts.u.gw_pp.u.deliver.user_data.headers[j].u.special_sm.msg_waiting == WMS_GW_MSG_WAITING_DISCARD) 
         &&(client_ts.u.gw_pp.u.deliver.user_data.headers[j].u.special_sm.msg_waiting_kind == WMS_GW_MSG_WAITING_VOICEMAIL)))
      {
        is_voicemail = TRUE;

        /* Get the number of messages */
        num_of_msgs = client_ts.u.gw_pp.u.deliver.user_data.headers[j].u.
                        special_sm.message_count;

        /* Active means messages waiting */
        is_vm_active = (num_of_msgs != 0) ? TRUE : FALSE;
      }
    }
#endif /*FEATURE_SMS_UDH*/
  }

  if(is_voicemail == TRUE)
  {
    /* make sure passed in pointers are valid before assignment */
    if(is_voicemail_active != NULL)
    {
      *is_voicemail_active = is_vm_active;
    }

    if(count != NULL)
    {
      *count = num_of_msgs;
    }
  }

  return is_voicemail;
} /* wms_cfg_check_voicemail_contents */


/*=========================================================================
FUNCTION
wms_cfg_check_cdma_duplicate

DESCRIPTION
  Check if a CDMA message is a duplicate in comparison to the messages
  stored in RUIM and NV.

  This is a Synchronous function call.

DEPENDENCIES
  None

RETURN VALUE
  TRUE:  duplicate
  FALSE: not a duplicate

SIDE EFFECTS
  None

=========================================================================*/
boolean wms_cfg_check_cdma_duplicate
(
  const wms_client_message_s_type     *msg_ptr
)
{
#ifdef FEATURE_CDSMS
  wms_ts_convert_cl2tl( & msg_ptr->u.cdma_message, & cdma_tl );

  if( wms_ts_decode_CDMA_bd( & msg_ptr->u.cdma_message.raw_ts, FALSE, FALSE,
                             & cdma_tl.cl_bd) != WMS_OK_S )
  {
    return FALSE;
  }

  return wms_msg_cdma_check_dups( & cdma_tl );
#else /* FEATURE_CDSMS */
  return FALSE;
#endif /* FEATURE_CDSMS */

} /* wms_cfg_check_cdma_duplicate() */


#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */

/*=========================================================================
FUNCTION
wms_cfg_check_wap_push_message

DESCRIPTION
  Check if a CDMA / GSM / WCDMA message is a WAP Push Message.

  This is a Synchronous function call.

DEPENDENCIES
  None

RETURN VALUE
  TRUE:  Is WAP Push Message
  FALSE: Is Not a WAP Push Message

SIDE EFFECTS
  None

=========================================================================*/
boolean wms_cfg_check_wap_push_message
(
  const wms_client_message_s_type     *msg_ptr
)
{
  boolean ret_value = FALSE;

  MSG_MED("Function wms_cfg_check_wap_push_message entered", 0, 0, 0);

  if (msg_ptr == NULL)
  {
    MSG_ERROR("Null Parameter Passed in wms_cfg_check_wap_push_message", 0, 0, 0);
    return FALSE;
  }

  if(msg_ptr->msg_hdr.message_mode == WMS_MESSAGE_MODE_CDMA)
  {
#ifdef FEATURE_CDSMS
    /* Check for WAP teleservice */
    if(msg_ptr->u.cdma_message.teleservice == WMS_TELESERVICE_WAP)
    {
      ret_value = TRUE;
    }
#endif /* FEATURE_CDSMS */
  }
  else if (msg_ptr->msg_hdr.message_mode == WMS_MESSAGE_MODE_GW)
  {
#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
  }
  else
  {
    MSG_ERROR("Incorrect Message Mode", 0, 0, 0);
  }

  MSG_MED("Function wms_cfg_check_wap_push_message result = %d", ret_value, 0, 0);
  return ret_value;

} /* wms_cfg_check_wap_push_message() */



/*
*/
void wms_cfg_print_active_bearers
(
  void
)
{

  /* Print out active bearers */
  MSG_ERROR("==== SMS Active Bearers ====",0,0,0);
#ifdef FEATURE_CDSMS
  MSG_ERROR("WMS_BEARER_CDMA_1X: %d", cfg_s_ptr->active_bearers[WMS_BEARER_CDMA_1X], 0,0);
  MSG_ERROR("WMS_BEARER_CDMA_EVDO: %d", cfg_s_ptr->active_bearers[WMS_BEARER_CDMA_EVDO], 0,0);
  MSG_ERROR("WMS_BEARER_CDMA_WLAN: %d", cfg_s_ptr->active_bearers[WMS_BEARER_CDMA_WLAN], 0,0);
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS_OVER_IP
#error code not present
#endif /* FEATURE_GWSMS_OVER_IP */

} /* wms_cfg_print_active_bearers() */

/*=========================================================================
FUNCTION
wms_cfg_ss_change_info

DESCRIPTION
  Process SS info change.

  This is a Synchronous function call.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Updated CFG database.

=========================================================================*/
void wms_cfg_ss_change_info
(
  const wms_cmd_ss_change_info_s_type   * ss_info_ptr
)
{
  boolean  sts = (SYS_SRV_STATUS_SRV==ss_info_ptr->srv_status)? TRUE : FALSE;

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

  /* Remember the serving system info
  */
  cfg_s_ptr->sys_mode   = ss_info_ptr->sys_mode;
  cfg_s_ptr->srv_domain = ss_info_ptr->srv_domain;
  cfg_s_ptr->srv_status = ss_info_ptr->srv_status;
  
  /* Update active bearers
  */
  switch( ss_info_ptr->sys_mode )
  {
    case SYS_SYS_MODE_CDMA:
      cfg_s_ptr->active_bearers[WMS_BEARER_CDMA_1X] = sts;
      break;

#ifdef FEATURE_GWSMS_OVER_IP
#error code not present
#endif /* FEATURE_GWSMS_OVER_IP */

  } /* switch */

  wms_cfg_print_active_bearers();

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

#ifdef CM_API_PLUS_DIALING
  cfg_s_ptr->hs_based_plus_dial_setting
     = ss_info_ptr->hs_based_plus_dial_setting;
  cfg_s_ptr->home_mobile_country_code
     = ss_info_ptr->home_mobile_country_code;
  cfg_s_ptr->current_mobile_country_code
     = ss_info_ptr->current_mobile_country_code;
#endif /* CM_API_PLUS_DIALING */

  return;

} /* wms_cfg_ss_change_info() */


#ifdef FEATURE_SMS_OVER_IP
/*=========================================================================
FUNCTION
wms_cfg_ipapp_change_info

DESCRIPTION
  Process IPApp info change.

  This is a Synchronous function call.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Updated CFG database.

=========================================================================*/
void wms_cfg_ipapp_change_info
(
  const cm_ipapp_info_s_type    * ipapp_info_ptr
)
{
  boolean  sts = (CM_IPAPP_REG_STATUS_REGISTERED==ipapp_info_ptr->reg_status)? TRUE : FALSE;
  int i = 0;

  MSG_HIGH("CFG: wms_cfg_ipapp_change_info with"
          " mode mask (%d), status (%d).", 
          ipapp_info_ptr->sys_mode, sts, 0);
#ifdef FEATURE_CDSMS
  if ( ipapp_info_ptr->sys_mode & SYS_SYS_MODE_MASK_HDR )
  {
    cfg_s_ptr->active_bearers[WMS_BEARER_CDMA_EVDO] = sts;
    MSG_HIGH("WMS_BEARER_CDMA_EVDO: %d",sts,0,0);
  }
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS_OVER_IP
#error code not present
#endif /* FEATURE_GWSMS_OVER_IP */

  if ( ipapp_info_ptr->sys_mode & SYS_SYS_MODE_MASK_WLAN )
  {
#ifdef FEATURE_CDSMS
    cfg_s_ptr->active_bearers[WMS_BEARER_CDMA_WLAN] = sts;
    MSG_HIGH("WMS_BEARER_CDMA_WLAN: %d",sts,0,0);
#endif /* FEATURE_CDSMS */
#ifdef FEATURE_GWSMS_OVER_IP
#error code not present
#endif /* FEATURE_GWSMS_OVER_IP */
  }

  // No service -- reset all active bearers
  if ( ipapp_info_ptr->sys_mode & SYS_SYS_MODE_MASK_NONE ||
       ipapp_info_ptr->sys_mode & SYS_SYS_MODE_MASK_NO_SRV )
  {
    MSG_HIGH("No service, resetting all active bearers",0,0,0);
    for( i=0; i < WMS_BEARER_MAX; i++ )
    {
      cfg_s_ptr->active_bearers[i] = FALSE;
    }
  }

  wms_cfg_print_active_bearers();
  return;

} /* wms_cfg_ipapp_change_info() */
#endif /* FEATURE_SMS_OVER_IP */


/*=========================================================================
FUNCTION
  wms_cfg_lpm_proc

DESCRIPTION
  Process LPM event.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Updated CFG database.

=========================================================================*/
void wms_cfg_lpm_proc
(
  const wms_cmd_cm_oprt_mode   oprt_mode
)
{
  if ( oprt_mode.in_lpm )
  {
    MSG_HIGH("LPM: low power mode set as TRUE",0,0,0);
  cfg_s_ptr->lpm = TRUE;

  wms_msg_offline();
  }
  else
  {
    MSG_HIGH("LPM: low power mode set as FALSE",0,0,0);
    cfg_s_ptr->lpm = FALSE;
  }
} /* wms_cfg_lpm_proc() */


#endif /* FEATURE_CDSMS || FEATURE_GWSMS */

/* END of wmscfg.c */

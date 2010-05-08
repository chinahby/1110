#ifndef WMSCFG_H
#define WMSCFG_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

        W i r e l e s s     M e s s a g i n g  S e r v i c e s
        -- Configuration Group

GENERAL DESCRIPTION
  The WMS module which implements the User API for SMS. This source file
  implements the configuration group functions which process the client
  commands of the configuration group.

  Copyright (c) 2001,2002,2003,2004,2005,2006,2007,2008
                by QUALCOMM INCORPORATED.
  All Rights Reserved.

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/* ^L<EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$PVCSPath:  L:/src/asw/MSM_MM/Services/wms/vcs/wmscfg.h_v   1.14   01 Jul 2002 16:41:50   hqu  $
$Header: //source/qcom/qct/modem/wms/wms/rel/qsc1100_05.04.00/src/WMSE/wmscfg.h#2 $ $DateTime: 2008/12/02 13:02:59 $ $Author: pyaramad $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/18/08   PMD     Removed FEATURE_WMS_CM_NAS_INTERFACES
02/07/08   PMD     Added support for MMGSDI Client and State
01/31/08   PMD     Updated Copyright Information to include 2008
01/14/07   PMD     Added support for MO SMS FDN Control 
07/23/07   PMD     Added support for plus code dialing
07/23/07   NP      LPM broken, WMS does not go or wake up from LPM
06/27/07   PMD     Include wmspriv.h
03/16/07   HQ      Added support for LPM.
03/14/07   HQ      Added curr_gw_domain_pref.
                   added wms_cfg_mm_set_domain_pref().
03/08/07   HQ      Fixed cell change for FEATURE_WMS_CM_NAS_INTERFACES.
03/01/07   HQ      Cleaned up SS info for GW.
02/27/07   HQ      Added support of FEATURE_SMS_OVER_IP.
02/20/07   HQ      Added support for FEATURE_WMS_CM_NAS_INTERFACES.
01/17/07   HQ      Updated copyright info to include 2007.
11/13/06   HQ      Added phone_esn for JCDMA2.
11/06/06   PMD     Replaced Feature T_MSM6250 with FEATURE_GWSMS
                   Eliminated FEATURE_SMS_LTK from Code
08/14/06   PMD     Mainlined / Replaced FEATURE_GWSMS_STATUS_REPORTS in code.
06/01/06   PMD     Featurized RUIM Cache under FEATURE_CDSMS_CACHE
01/25/06   VK      Updated Copyright Info
12/21/05   VK      Lint fixes
11/18/05   VK      Cleaned up Lint warnings. 
10/19/05   VK      Added new feature FEATURE_CDSMS_CACHE
                   Added new feature FEATURE_CDSMS_DUP_DETECTION
10/19/05   VK      Updated Copyright info
07/19/05   PMD     Added wms_cfg_set_gw_domain_pref_internal_proc prototype
05/14/05   PMD     Added RAM Storage Support for Status Reports
03/31/05   PMD     Changed wms_cfg_cell_change_ind_proc prototype
01/17/05   Rex     Featurization for APIONE
01/10/05   Rex     Fixes for CPHS featurization name
01/07/05   PMD     Added feature FEATURE_SMS_LTK
09/22/04   Rex     Featurize SMS link control
09/21/04   Rex     Added support for CPHS
09/01/04   Rex     Fix featurization problem
08/30/04   Rex     Fix struct to enum type for link control
08/25/04   Rex     Added support for SMS link control
04/15/04   PMD     Added new flag for Refresh Registered
03/08/04   PMD     Added Support for SMMA Notification using Client Memory
12/15/03   PMD     Added 1x Broadcast Support in Multimode Broadcast API
11/06/03   AP      Added global nv_cmd_err to use if nv_write_wait fails.
09/02/03   HQ      Added support for WMS_CFG_EVENT_CELL_CHANGE.
07/29/03   HQ      Added support for FEATURE_BROADCAST_SMS_MULTIMODE:
                      - New BC_MM commands, events and structures.
                      - New BC_MM API functions.
                      - GSM CB support.
                      - 1x BC SMS is still supported via BC group, not BC_MM.
07/26/03   PMD     Changes to suppress / remove Lint Errors
07/01/03   PMD     Added new refresh_in_progress boolean flag
05/20/03   SUN     Added new in_progress boolean to cfg_s_type
04/26/03   HQ/AP   Use WMS_MAX_CDMA_MESSAGES_IN_MEM for dup detect cache.
04/24/03   SUN     Added new function prototype to check for validity of smsr
04/21/03   HQ      Use WMS_MAX_NV_CDMA_MESSAGES and WMS_MAX_NV_GW_MESSAGES to
                   replace WMS_MAX_NV_MESSAGES.
03/26/03   HQ      Memory usage reduction changes.
03/14/03   SUN     Added new filed for Status Report Index
02/21/03   SUN     Moved Status Report related functions to wms.h
02/03/03   HQ      Added support for MO SMS PS/CS domain preferences.
02/10/03   SUN     Added new function to get the smsr id
02/07/03   SUN     Added Support for Status Reports and Commands
                   Added Support for FEATURE_GWSMS_CACHE
                   Mainlined FEATURE_GWSMS_ASYNC_INIT
11/14/02   HQ      Properly feature-ized msg info cache and dup detection to
                   resolve compile errors and warnings for GW targets.
11/13/02   AP      Mainlined support for message list info cache, added support
                   for duplicate detection.
10/11/02   HQ      Added support of FEATURE_GWSMS_ASYNC_SIM_INIT.
08/26/02   HQ      Added support of FEATURE_CDSMS_MSG_INFO_CACHE.
08/07/02   AP      Added wms_cfg_pre_init() to initialize some values of the
                   cfg structure.
07/31/02   SUN     Added func- wms_cfg_do_total_memory_full
06/29/02   HQ      Changed func name to wms_cfg_do_cdma_ready_event().
06/22/02   HQ      Added support of FEATURE_CDSMS_RUIM.
06/15/02   HQ      Added more CDSMS code and CDMA voice mail processing.
06/10/02   HQ      Removed WMS_SMMA_MSG_ID.
05/14/02   SUN     Fixed SMMA Issue
05/07/02   HQ      Added more support for FEATURE_CDSMS and Multimode.
03/12/02   HQ      Added support of Multimode WMS - added FEATURE_CDSMS.
03/11/02   HQ      Re-define WMS_SMMA_MSG_ID to fix compiler warnings.
03/11/02   SUN     Added Support for SMMA
02/22/02   SUN     Added wms_cfg_ready_event
02/08/02   SUN     Added new function that sends memory full notification
02/07/02   HQ      Added init_complete flag.
01/21/02   SUN     New Event for Error Notification to UI
12/13/01   HQ      Renamed UASMS to WMS.
12/07/01   HQ      Initial version for MSM_MM WMS.
11/9/01    HQ      Cleaned up compiler warnings.
08/27/01   HQ      Initial version for FEATURE_GWSMS.
===========================================================================*/


#if defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS)


/*===========================================================================
========================  INCLUDE FILES =====================================
===========================================================================*/

#include "wms.h"
#include "wmsi.h"
#include "wmsmsg.h"
#include "wmspriv.h"
#include "cm.h"

#if defined(FEATURE_GWSMS) || defined(FEATURE_CDSMS_RUIM)
#include "uim.h"
#endif

#ifdef FEATURE_MMGSDI
#include "mmgsdilib.h"
#endif

#ifdef FEATURE_GWSMS
#error code not present
#endif 

/*===========================================================================

                         DATA TYPE DECLARATIONS

===========================================================================*/

/* SIM/RUIM EFsmss memory full flag
*/
typedef enum
{
  WMS_SIM_MEM_FULL_SET    = 0xFE,
  WMS_SIM_MEM_FULL_CLEAR  = 0xFF
} wms_sim_mem_full_flag_e_type;

/* MMGSDI Init Status */
typedef enum
{
   WMS_SIM_MMGSDI_NOT_INIT = 0,
   WMS_SIM_MMGSDI_INIT_IN_PROGRESS,
   WMS_SIM_MMGSDI_INIT_COMPLETE
} wms_sim_mmgsdi_init_state_e_type;

/* Configuration data
*/
typedef struct wms_cfg_struct
{

  /* Routing configuration
  */
  wms_routes_s_type               routes;

  /* Global NV cmd err
  */
  wms_cmd_err_e_type              nv_cmd_err;

#ifdef FEATURE_GSTK
  /* Refresh Functionality common to both GW and CDMA */
  boolean                         refresh_registered;
  boolean                         refresh_in_progress;
#elif defined(FEATURE_UIM_TOOLKIT)
  boolean                         refresh_in_progress;
#endif /* FEATURE_GSTK */

#ifdef FEATURE_MMGSDI
  wms_sim_mmgsdi_init_state_e_type  wms_sim_mmgsdi_init_state;
  mmgsdi_client_id_type             wms_sim_mmgsdi_client;   
#endif /* FEATURE_MMGSDI */

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_CDSMS
  boolean                         cdma_init_complete;
#endif /* FEATURE_CDSMS */

  /* MO SMS FDN Control Variables */
  boolean                         is_fdn_enabled_slot_1;
  boolean                         is_fdn_enabled_slot_2;

#ifdef FEATURE_CDSMS

#ifdef FEATURE_CDSMS_DUP_DETECTION
  /* Cache for duplicate detection processing.
  */
  wms_msg_dup_detect_info_s_type  dup_detect[WMS_MAX_CDMA_MESSAGES_IN_MEM];
#endif /* FEATURE_CDSMS_DUP_DETECTION */

  wms_message_index_type          cdma_voice_mail_index;
  boolean                         cdma_voice_mail_active;
  wms_message_number_type         ruim_last_message_number;

#ifdef FEATURE_CDSMS_RUIM
  /* RUIM EFsms information.
  ** Tag = WMS_TAG_NONE: empty slots.
  */
  uim_items_enum_type             ruim_sms_file_name;
  uint32                          ruim_max_slots;
  uint32                          ruim_sms_rec_len;
  wms_message_tag_e_type          ruim_tags[WMS_MESSAGE_LIST_MAX];
#ifdef FEATURE_CDSMS_CACHE
  uint8      ruim_sms_info_cache[WMS_MESSAGE_LIST_MAX][WMS_MSG_INFO_CACHE_SIZE];
#endif /* FEATURE_CDSMS_CACHE */

  /* EFsmsp information
  */
  uim_items_enum_type             ruim_smsp_file_name;
  uint32                          ruim_max_templates;
  uint32                          ruim_smsp_rec_len;
  boolean                         ruim_templates[WMS_MAX_TEMPLATES];
#ifdef FEATURE_CDSMS_CACHE
  uint8    ruim_template_info_cache[WMS_MAX_TEMPLATES][WMS_MSG_INFO_CACHE_SIZE];
#endif /* FEATURE_CDSMS_CACHE */

  /* EFsmss information
  */
  uim_items_enum_type             ruim_smss_file_name;
  uint32                          ruim_smss_rec_len;
  wms_message_number_type         ruim_last_wap_message_number;
  wms_sim_mem_full_flag_e_type    ruim_mem_full_flag;
#endif /* FEATURE_CDSMS_RUIM */

  /* Message list for NV
  */
  uint32                          nv_cdma_max_slots;
  wms_message_tag_e_type          nv_cdma_tags[WMS_MAX_NV_CDMA_MESSAGES];

#ifdef FEATURE_CDSMS_CACHE
  uint8   nv_cdma_info_cache[WMS_MAX_NV_CDMA_MESSAGES][WMS_MSG_INFO_CACHE_SIZE];
#endif /* FEATURE_CDSMS_CACHE */

  /* Message storage in RAM
  */
  //wms_msg_ruim_sms_s_type          ram_cdma_sms[WMS_MAX_RAM_MESSAGES];
  
#ifdef FEATURE_JCDMA_2
  uint32        phone_esn;
#endif /* FEATURE_JCDMA_2 */

#endif /* FEATURE_CDSMS */


#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

  /* Variables for SMMA Notification using Client Memory */
  wms_client_id_type              primary_client;
  boolean                         use_client_memory;
  boolean                         client_memory_full;

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

  /* Bearers and Preferences
  */
  wms_bearer_prefs_arr_type       bearer_prefs;
  boolean                         active_bearers[WMS_BEARER_MAX];
  wms_bearer_e_type               last_used_bearer;

  /* Serving System info
  */
  sys_sys_mode_e_type       sys_mode;
  sys_srv_domain_e_type     srv_domain;
  sys_srv_status_e_type     srv_status;

  boolean                   lpm;

#ifdef CM_API_PLUS_DIALING
  cm_hs_based_plus_dial_e_type hs_based_plus_dial_setting;
  cm_country_code_type         home_mobile_country_code;
  cm_country_code_type         current_mobile_country_code;
#endif /* CM_API_PLUS_DIALING */

} wms_cfg_s_type;



/* <EJECT> */
/*===========================================================================

                       FUNCTION DEFINITIONS

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
);


void wms_cfg_do_memory_full
(
  wms_memory_store_e_type mem_store
);

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
);

/*
*/
void wms_cfg_pre_init
(
  void
);


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
);



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
);
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
);


#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */



void wms_cfg_update_voice_mail_index
(
  uint8                     * sim_data,
  wms_message_index_type  i
);



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
);

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
);

#ifdef FEATURE_GWSMS
#error code not present
#endif /*FEATURE_GWSMS*/

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
);


/*===========================================================================
FUNCTION wms_cfg_do_ruim_ready_event

DESCRIPTION
  Notifies UI when RUIM has been initialised

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void wms_cfg_do_cdma_ready_event
(
  void
);


/* Update CDMA voice mail info
*/
void wms_cfg_cdma_update_voice_mail_index
(
  wms_format_e_type         format,
  wms_message_tag_e_type    tag,
  uint16                    data_len,
  const uint8                     * data,
  wms_message_index_type    i
);

#endif /* FEATURE_CDSMS */


#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */


void wms_cfg_update_msg_info_cache
(
  wms_message_tag_e_type    tag,
  wms_memory_store_e_type   mem_store,
  wms_message_index_type    index,
  uint8                     * data,
  uint8                     len
);


void wms_cfg_activate_client( wms_client_id_type client_id );

/*
*/
boolean wms_cfg_set_gw_domain_pref_internal_proc
(
  wms_gw_domain_pref_e_type    gw_domain_pref
);

#endif /* FEATURE_CDSMS || FEATURE_GWSMS */

#ifdef FEATURE_CDSMS
void wms_cfg_update_dup_info_cache
(
  wms_memory_store_e_type         mem_store,
  uint32                          index,
  const wms_tl_message_type             *new_tl_msg
);

void wms_cfg_delete_dup_info_cache
(
  wms_memory_store_e_type         mem_store,
  uint32                          index
);
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */

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
);

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
);
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
);

#endif /* WMSCFG_H */


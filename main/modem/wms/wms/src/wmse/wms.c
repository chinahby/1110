/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


    W i r e l e s s   M e s s a g i n g   S e r v i c e s   M O D U L E

GENERAL DESCRIPTION
  The WMS module which implements the User API for SMS. This source file
  contains the functions for the client applications to call. It also
  implements some internal functions such as module initialization,
  processing of signals, commands, reports and timers.

  Copyright (c) 2001,2002,2003,2004,2005,2006,2007,2008
                by QUALCOMM INCORPORATED.
  All Rights Reserved.

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/* ^L<EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$PVCSPath:  L:/src/asw/MSM_MM/Services/wms/vcs/wms.c_v   1.13   22 Jun 2002 12:05:28   hqu  $
$Header: //source/qcom/qct/modem/wms/wms/rel/qsc1100_05.04.00/src/WMSE/wms.c#7 $ $DateTime: 2008/12/02 13:02:59 $ $Author: pyaramad $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/30/08   HN      Won't put cmd in the queue if malloc() for alpha ID fails
10/26/08   HN      Check if message_mode & mem_store combination is correct 
                   for wms_msg_write_template()
10/01/08   HN      Move wmsi_get_card_presence() out of FEATURE_GSTK guard
09/30/08   HN      Cmd_ptr's template's alpha ID should be initialized no matter what the message mode is
09/23/08   HN      init is complete if RUIM phone doesn't have a RUIM
07/28/08   HN      Add check for type of SIM card present on phone before doing initialization
06/27/08   PMD     WMS Always votes positively for Refresh
06/24/08   PMD     Removed Tabs from code
06/11/08   PMD     Removed wms_bc_*** APIs for CDMA Broadcast
04/18/08   PMD     Removed FEATURE_WMS_CM_NAS_INTERFACES
04/17/08   PMD     Replace setting TMC Signals, TMC Calls with Task Calls
01/31/08   PMD     Updated Copyright Information to include 2008
01/30/08   PMD/PY  Created cm/nas interfacing layer in wmscm_gw.h and wmscm_gw.c
08/21/07   PMD     Added wms_kick_dog() while processing commands and reports
08/07/07   PY      mainlined !FEATURE_WMS_NO_GS_SEND and relocated 
                   CM/NAS interfacing code.
07/23/07   NP      LPM broken, WMS does not go or wake up from LPM
07/12/07   NP      SMS Over IP problems.
07/02/07   NP      Allow processing of WMS_CMD_MSG_MC_MO_STATUS_E even when 
                   cdma is not ready since this event could be generated in 
                   response to an earlier request by us sending out an MO SMS. 
                   Fixes LPM issues.
05/24/07   PMD     Fix for processing new commands while not initialized
05/15/07   PMD     Removed encode_decode_data_crit_sect
05/11/07   PMD     Moved SS_INFO command out of Feature Flag
04/06/07   HQ      Added detection of PLMN change.
03/30/07   HQ      Added support for Power Collapse/Power Management
                   for Dual CPU chipsets.
03/06/07   HQ      Added support for IWMSC.
02/20/07   HQ      Added support for FEATURE_WMS_CM_NAS_INTERFACES.
01/17/07   HQ      Updated copyright info to include 2007.
12/04/06   PMD     Fixed Null Pointer dereference in wms_bc_change_label()
11/06/06   PMD     Replaced Feature T_MSM6250 with FEATURE_GWSMS
                   Eliminated FEATURE_SMS_LTK from Code
10/17/06   PMD     Fixed Lint Warning by removing condition evaluating to False
09/27/06   PMD     Changed TASKLOCK and TASKFREE to Critical Sections
                   Removed featurization for sms_card_refresh_crit_sect
08/15/06   PMD     Added Documentation for Status Report Storage Behavior.
08/14/06   PMD     Mainlined FEATURE_GWSMS_STATUS_REPORTS in code.
08/06/06   PMD     Modified GSDI Refresh Voting Callback Logic.
                   Added Debug Statements in wms_check_idle() function.
07/31/06   PMD     Removed unused feature - FEATURE_GWSMS_INIT_DELAY
06/20/06   PMD     Do Local WMS Init before Task Start Sig. 
                   Removed wms_task_init_done variable and check.
05/19/06   PMD     Fixed compile warning
05/02/06   HQ      Added check for wms_task_init_done.
05/01/06   PMD     Fixed Link Warning
04/28/06   PMD     Added support for SIM_INIT_COMPLETED Event for Game Mode
04/25/06   HQ      Fixed lint warnings.
04/20/06   PMD     Eliminated MO Ack Timer, Pending Msg and Pending Ack 
                   timer Logic for CDSMS.
                   Mainlined FEATURE_SMS_RETRY
01/25/06   VK      Updated Copyright Info
12/21/05   VK      Lint fixes
12/14/05   VK      More Lint error/warning fixes
10/20/05   VK      Lint error/warning fixes
10/20/05   VK      Changed byte = uint8, word = uint16, dword = uint32
10/18/05   VK      SMS_RETRY Featurization fixes
10/17/05   PMD     Updated Copyright Information.
                   Added Function Headers.
10/17/05   PMD     Added support for get / set retry interval
                   Support for new Retry Logic
07/22/05   PMD     Allow msg_send Command if gw_init_complete = FALSE.
07/19/05   PMD     Allow Commands even if gw_init_complete = FALSE.
07/08/05   Rex     Fixed Compile Warnings
05/17/05   PMD     Allow MT SMS Ack while Initialization
05/05/05   Rex     Added implementation for wms_offline
04/04/05   PMD     Fix in GW CB Cell Change Event Processing
03/31/05   PMD     Support for Cell Change Type Event Information.
03/10/05   PMD     Added support for wms_bc_mm_msg_delete_all_indication
02/14/05   Rex     Added critical section for encoding/decoding data
02/01/05   PMD     Added wms_bc_mm_set/get_reading_preferences API
01/10/05   Rex     Code cleanup for WMS decoupling from NAS module and
                   using CM, featurize as FEATURE_APIONE
01/07/05   PMD     Added feature FEATURE_SMS_LTK
11/17/04   PMD     Added Default Command Parameters in 
                   wms_card_refresh_file_change_notification_cb().
09/22/04   Rex     Featurize SMS link control
08/30/04   Rex     Fix valid commands to execute
08/25/04   Rex     Added support for SMS link control
08/02/04   PMD     Removed wms_dbg_init from CDSMS Feature Flag
07/30/04   HQ      Removed ERR_FATALs.
07/15/04   PMD     Added support for Duplicate Detection Indication from CB
                   Added support for CB Page Deletion Indication
06/22/04   PMD     Fix for WMS Dog Timeout
03/23/04   PMD     Changed Primary Client and Memory Status Implementation.
02/26/04   PMD     Fixed Compile Warnings
02/04/04   PMD     Cleaned up Lint Warnings in code
02/04/04   AP      Clear sigs of WMS_PENDING_ACK_TIMER_SIG after processing
                   pending messages.
01/24/04   tml     Refresh fix
01/16/04   AP/PMD  Added GW and CDMA Retry
12/15/03   PMD     Added 1x Broadcast Support in Multimode Broadcast API
11/03/03   PMD     Removed Checks in wms_cfg_get_memory_status and
                   wms_cfg_get_message_list.
10/23/03   PMD     Introduced Checks for Invalid Data / Null Pointers in
                   wms_cfg_set_routes, wms_cfg_get_memory_status,
                   wms_cfg_get_message_list, wms_msg_send, wms_msg_ack
                   and wms_msg_write
10/23/03   HQ      Kick the dog each time WMS processes signals.
10/07/03   AP      Removed feature flag for cfg_s_ptr in wms_init.
10/01/03   HQ      Removed feature flags for cfg_s_ptr.
09/22/03   PMD     Added Function to Clear SMMA Flag
09/16/03   PMD     Changes to not process commands during initialization
09/07/03   PMD     Added Support for Debugging Information (cmd_id and cmd_err)
                   String Display
09/02/03   HQ      Added interfaces to NAS for GW CB.
07/29/03   HQ      Added support for FEATURE_BROADCAST_SMS_MULTIMODE:
                      - New BC_MM commands, events and structures.
                      - New BC_MM API functions.
                      - GSM CB support.
                      - 1x BC SMS is still supported via BC group, not BC_MM.
07/26/03   PMD     Changes to suppress / remove Lint Errors
06/27/03   PMD     Added New Command Processing Logic for GSDI Refresh
                   Added new Command Type for file change Notification Processing
                   Added cfg_s_ptr in this module
                   Added functions for getting cmd and rpt queue count
                   Added Common GSDI Refresh Functionality along with chnage in
                   wms_process_cmd
06/20/03   HQ      Skip alpha ID processing when writing a non-GW template.
06/08/03   PMD     Added Support for dynamic allocation of Alpha ID in GW Template
05/30/03   SUN     Added support for setting TP-MR
04/09/03   HQ      Initialize done_q_ptr for cmd_backup.
04/02/03   HQ      Return UNSUPPORTED if an API function is not supported;
                   declare a static wms_cmd_type buffer in case of no
                   resource and we need to send client cmd error.
02/27/03   HQ      Added support of FEATURE_GWSMS_MO_CONTROL.
02/03/03   HQ      Added support for MO SMS PS/CS domain preferences.
02/07/03   SUN     Added Support for Status Reports and Commands
01/15/03   HQ      Added support for FEATURE_CDSMS_IS637B_BROADCAST_SCPT.
11/21/02   AP      Initialize wmsdiag group.
11/14/02   HQ      Properly feature-ized msg info cache and dup detection to
                   resolve compile errors and warnings for GW targets.
11/13/02   AP      Added support for registering message list callback w/WMS.
11/09/02   HQ      Added new BC SMS commands: Delete All Services and
                   Set Priority For All.
10/11/02   HQ      Added support of FEATURE_GWSMS_ASYNC_SIM_INIT.
10/09/02   SUN     Set the cmd buffer free if client is not activated and a cmd
                   is Received
09/06/02   AP      Set correct ACK_SIG for appropriate task for STOP_SIG and
                   OFFLINE_SIG.
08/07/02   AP      Added wms_cfg_pre_init() to initialize some values of the
                   cfg structure.  Moved cl_init in front of nv_init.
08/06/02   HQ      Added support of FEATURE_MULTIMODE_ARCH.
06/22/02   HQ      Added command wms_msg_delete_all() and
                   wms_msg_delete_template_all().
06/15/02   HQ      Added support of FEATURE_BROADCAST_SMS.
05/09/02   HQ      Added FEATURE_GWSMS_INIT_DELAY.
05/07/02   HQ      Added more support for FEATURE_CDSMS and Multimode.
03/28/02   HQ      Changed FEATURE_WMS_GS_SEND to FEATURE_WMS_NO_GS_SEND.
03/25/03   HQ      Added support of DC, BC and DBG groups for Multimode WMS;
                   Added FEATURE_CDSMS.
                   Feature-ized GS_SEND_MESSAGE under FEATURE_WMS_GS_SEND.
03/13/02   ATM     Added support for new GS_SEND_MESSAGE
02/22/02   SUN     Fixed Compiler errors
02/12/02   HQ      Acknowledge offline signal; fixed compiler warnings.
02/07/02   HQ      Moved cfg init to the end of the init.
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

#include "wmsi.h"
#include "wmsll.h"
#include "msg.h"
#include "err.h"
#include "task.h"
#include "dog.h"
#include "wmscl.h"
#include "wmscfg.h"
#include "wmsmsg.h"
#include "wmsnv.h"
#include "wmscm.h"
#include "queue.h"
#include "err.h"
#include "wmsdc.h"
#include "wmsbc.h"
#include "wmsts.h"
#include "wmsdbg.h"
#ifdef FEATURE_MMGSDI
#include "wmssim.h"
#endif /* FEATURE_MMGSDI */
#ifdef FEATURE_WMS_DIAG
#include "wmsdiag.h"
#endif /* FEATURE_WMS_DIAG */

#include "wmscm_gw.h"

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_CDSMS
#include "mc.h"
#ifdef FEATURE_CDSMS_RUIM
#include "wmsruim.h"
#endif /* FEATURE_CDSMS_RUIM */
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_APPS_POWER_COLLAPSE
#error code not present
#endif /* FEATURE_APPS_POWER_COLLAPSE */



/*===========================================================================
======================== STATIC DATA ========================================
===========================================================================*/
static q_type                        cmd_q;
static q_type                        cmd_free_q;
static wms_cmd_type                  cmds[WMS_MAX_CMDS];

/* This is used for handling the case where no cmd buf is available, but a cmd
** buf is needed for sending client cmd err.
*/
static wms_cmd_type                  cmd_backup;

static wms_cfg_s_type               *cfg_s_ptr;

/* Timer for kicking the watchdog. */
rex_timer_type  wms_rpt_timer;

#ifdef FEATURE_GSTK
wms_cmd_gsdi_refresh_fcn_ind_type   temp_refresh_fcn_ind;
#endif /* FEATURE_GSTK */

LOCAL rex_crit_sect_type            sms_card_refresh_crit_sect;

/* Define the wms_msg_info_cache_cb_type */
wms_msg_info_cache_cb_type wms_msg_info_cache_cb = NULL;

#ifdef CUST_EDITION
static wms_client_ts_data_s_type client_ts;
#endif

/* <EJECT> */
/*===========================================================================
=============================== MISCELLANEOUS =================================
================================= FUNCTIONS =================================
===========================================================================*/

static void wms_offline(void);

/* <EJECT> */
/*===========================================================================
FUNCTION WMS_KICK_DOG

DESCRIPTION
  Kick the watchdog and set the watchdog timer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
void wms_kick_dog
(
  void
)
{
  (void)rex_clr_sigs( &wms_tcb, WMS_RPT_TIMER_SIG );
  dog_report( DOG_WMS_RPT );
  (void) rex_set_timer( &wms_rpt_timer, DOG_WMS_RPT_TIME );
}


/* <EJECT> */
/*===========================================================================

FUNCTION wms_init

DESCRIPTION
  Initialize the WMS module.

DEPENDENCIES
  none

RETURN VALUE
  None

SIDE EFFECTS
  Initialization to internal data is done.

===========================================================================*/
void wms_init
(
  void
)
{
  int i;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* ------------- Initialize API Command Buffers ---------------*/
  (void) q_init( & cmd_q );
  (void) q_init( & cmd_free_q );

  for( i=0; i<WMS_MAX_CMDS; i++ )
  {
    cmds[i].hdr.done_q_ptr = & cmd_free_q;
    (void) q_link( & cmds[i], & cmds[i].hdr.link );
    q_put( & cmd_free_q, & cmds[i].hdr.link );
  }

  cmd_backup.hdr.done_q_ptr = & cmd_free_q;

  rex_def_timer( &wms_rpt_timer, &wms_tcb, WMS_RPT_TIMER_SIG );

  /* Perform Local WMS Task Initialization */
  wms_cfg_pre_init();
  wms_cl_init();

  /* Inform Main Control task that CM task has finished initialization */
  task_start(WMS_RPT_TIMER_SIG, DOG_WMS_RPT, &wms_rpt_timer);

  (void) rex_clr_sigs( rex_self(), TASK_START_SIG );

  /* Set the watchdog report timer signal so that we'll
  ** reset the watchdog as soon as we start checking for events
  ** and start our first tick timer
  */
  (void) rex_set_sigs( &wms_tcb, WMS_RPT_TIMER_SIG );

  rex_init_crit_sect(&sms_card_refresh_crit_sect);

  /* Initialize the objects
  */
  wms_nv_init();
  wms_cm_init();
  wms_msg_init();
  wms_dc_init();
  wms_dbg_init();
#ifdef FEATURE_MMGSDI
  wms_sim_gsdi_init();
#endif /* FEATURE_MMGSDI */

#ifdef FEATURE_CDSMS_BROADCAST
  wms_bc_init();
#endif /* FEATURE_CDSMS_BROADCAST */

  wms_cfg_init();
  wms_ts_init();

#ifdef FEATURE_WMS_DIAG
  wmsdiag_init();
#endif /* FEATURE_WMS_DIAG */
  /* SIM & RUIM init will be done after full service is started */

#ifdef FEATURE_APPS_POWER_COLLAPSE
#error code not present
#endif /* FEATURE_APPS_POWER_COLLAPSE */

  /* Setting the CFG pointer */
  cfg_s_ptr = wms_cfg_s_ptr();

  /* done */
  return;

} /* wms_init() */



/* <EJECT> */
/*===========================================================================

FUNCTION wms_get_cmd_buf

DESCRIPTION
  Allocate a WMS Task command buffer.

DEPENDENCIES
  wms_init() was done.

RETURN VALUE
  Pointer to allocated WMS command buffer.
  NULL pointer is returned if no more buffer is available.

SIDE EFFECTS
  Remove a command from the free queue.

===========================================================================*/
wms_cmd_type *wms_get_cmd_buf
(
  void
)
{
  wms_cmd_type     *cmd_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_LOW("cmd_free_q cnt=%d", q_cnt( & cmd_free_q ), 0, 0 );
  MSG_LOW("cmd_q cnt=%d", q_cnt( & cmd_q ), 0, 0 );

  cmd_ptr = (wms_cmd_type *) q_get( & cmd_free_q );

  if( cmd_ptr == NULL )
  {
    /* Command buffer allocation failed.
    */
    MSG_ERROR("NO buf in cmd_free_q: %d", WMS_MAX_CMDS, 0, 0 );
  }

  /* done */
  return cmd_ptr;

} /* wms_get_cmd_buf() */


/* <EJECT> */
/*===========================================================================
FUNCTION wms_check_for_valid_command

DESCRIPTION
  This function call will check for which commands
   to allow / disallow during special scenarios like
   1. WMS Refresh
   2. CDMA Init Not Completed
   3. GSM Init Not Completed
   It will return the appropriate Command Error to be
   returned to the client in the Callback

DEPENDENCIES
  General Rule: Dont Change Command Error if it is a
  Valid Command. (cmd_err initialized to NONE)

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
wms_cmd_err_e_type wms_check_for_valid_command
(
  wms_cmd_type *cmd_ptr
)
{
  wms_cmd_err_e_type ret_cmd_err = WMS_CMD_ERR_NONE;

//#ifdef FEATURE_GSTK
#if defined(FEATURE_GSTK) || defined(FEATURE_UIM_TOOLKIT) // Gemsea
  /* Checking for 1st Special Scenario - REFRESH */
  if (cfg_s_ptr->refresh_in_progress)
  {
    switch(cmd_ptr->hdr.cmd_id)
    {
      /* Valid Refresh Commands */
      case WMS_CMD_GSDI_ASYNC_CB:
      case WMS_CMD_GSDI_CARD_STATUS_CB:
      case WMS_CMD_CFG_SET_ROUTES:
      case WMS_CMD_CFG_GET_ROUTES:
      case WMS_CMD_GSDI_REFRESH_FCN_IND:
#ifdef CUST_EDITION	  
      case WMS_CMD_REFRESH_IND:
#endif /*CUST_EDITION*/	  
    #ifdef FEATURE_GWSMS
#error code not present
    #endif /* FEATURE_GWSMS */
      case WMS_CMD_SS_CHANGE_INFO:
    #ifdef FEATURE_SMS_OVER_IP
      case WMS_CMD_IPAPP_CHANGE_INFO:
    #endif /* FEATURE_SMS_OVER_IP */
    #ifdef FEATURE_APPS_POWER_COLLAPSE
#error code not present
    #endif /* FEATURE_APPS_POWER_COLLAPSE */
	    #ifdef FEATURE_APIONE
#error code not present
    #endif /* FEATURE_APIONE */
        break;

      default:
        /* Invalid Refresh Commands
           Return BUSY */
        ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;
    }
  }
#endif /*FEATURE_GSTK */

#ifdef FEATURE_CDSMS
  /* Checking for 2nd Special Scenario - CDMA Init */
  if (!cfg_s_ptr->cdma_init_complete)
  {
    switch(cmd_ptr->hdr.cmd_id)
    {
      /* Valid Commands */
      case WMS_CMD_CFG_SET_ROUTES:
      case WMS_CMD_CFG_GET_ROUTES:
      case WMS_CMD_CFG_SET_GW_DOMAIN_PREF:
      case WMS_CMD_CFG_GET_GW_DOMAIN_PREF:
      case WMS_CMD_CFG_SET_PRIMARY_CLIENT:
      case WMS_CMD_CFG_SET_MEMORY_FULL:
      case WMS_CMD_CFG_SET_LINK_CONTROL:
      case WMS_CMD_CFG_GET_LINK_CONTROL:
#ifdef CUST_EDITION	  
      case WMS_CMD_CFG_SET_MULTISEND:
#ifdef FEATURE_AUTOREPLACE
      case WMS_CMD_CFG_SET_AUTOREPLACE:
#endif        
#endif /*CUST_EDITION*/
        break;

      case WMS_CMD_CFG_GET_MEMORY_STATUS:
        switch (cmd_ptr->cmd.cfg_get_mem_status.mem_store)
        {
          case WMS_MEMORY_STORE_RAM_CDMA:
          case WMS_MEMORY_STORE_RUIM:
          case WMS_MEMORY_STORE_NV_CDMA:
            ret_cmd_err = WMS_CMD_ERR_BUSY;
            break;
        }
        break;

      case WMS_CMD_CFG_GET_MESSAGE_LIST:
        switch (cmd_ptr->cmd.cfg_get_msg_list.mem_store)
        {
          case WMS_MEMORY_STORE_RAM_CDMA:
          case WMS_MEMORY_STORE_RUIM:
          case WMS_MEMORY_STORE_NV_CDMA:
            ret_cmd_err = WMS_CMD_ERR_BUSY;
            break;
        }
        break;

      case WMS_CMD_MSG_SEND:
        if (cmd_ptr->cmd.msg_send.message.msg_hdr.message_mode == WMS_MESSAGE_MODE_CDMA)
          ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_MSG_ACK:
        if (cmd_ptr->cmd.msg_ack.ack_info.message_mode == WMS_MESSAGE_MODE_CDMA)
          ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_MSG_READ:
        switch(cmd_ptr->cmd.msg_read.mem_store)
        {
          case WMS_MEMORY_STORE_RAM_CDMA:
          case WMS_MEMORY_STORE_RUIM:
          case WMS_MEMORY_STORE_NV_CDMA:
            ret_cmd_err = WMS_CMD_ERR_BUSY;
            break;
        }
        break;

      case WMS_CMD_MSG_WRITE:
        if (cmd_ptr->cmd.msg_write.message.msg_hdr.message_mode == WMS_MESSAGE_MODE_CDMA)
          ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_MSG_DELETE:
        switch (cmd_ptr->cmd.msg_delete.mem_store)
        {
          case WMS_MEMORY_STORE_RAM_CDMA:
          case WMS_MEMORY_STORE_RUIM:
          case WMS_MEMORY_STORE_NV_CDMA:
            ret_cmd_err = WMS_CMD_ERR_BUSY;
            break;
        }
        break;
#ifdef CUST_EDITION
      case WMS_CMD_MSG_DELETE_BOX:
      case WMS_CMD_MSG_COPY:
        ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;
#endif /*CUST_EDITION*/
      case WMS_CMD_MSG_DELETE_ALL:
        switch (cmd_ptr->cmd.msg_delete_all.mem_store)
        {
          case WMS_MEMORY_STORE_RAM_CDMA:
          case WMS_MEMORY_STORE_RUIM:
          case WMS_MEMORY_STORE_NV_CDMA:
            ret_cmd_err = WMS_CMD_ERR_BUSY;
            break;
        }
        break;

      case WMS_CMD_MSG_MODIFY_TAG:
        switch (cmd_ptr->cmd.msg_modify_tag.mem_store)
        {
          case WMS_MEMORY_STORE_RAM_CDMA:
          case WMS_MEMORY_STORE_RUIM:
          case WMS_MEMORY_STORE_NV_CDMA:
            ret_cmd_err = WMS_CMD_ERR_BUSY;
            break;
        }
        break;

      case WMS_CMD_MSG_READ_TEMPLATE:
        switch (cmd_ptr->cmd.msg_read_template.mem_store)
        {
          case WMS_MEMORY_STORE_RAM_CDMA:
          case WMS_MEMORY_STORE_RUIM:
          case WMS_MEMORY_STORE_NV_CDMA:
            ret_cmd_err = WMS_CMD_ERR_BUSY;
            break;
        }
        break;

      case WMS_CMD_MSG_WRITE_TEMPLATE:
        if (cmd_ptr->cmd.msg_write_template.message.msg_hdr.message_mode == WMS_MESSAGE_MODE_CDMA)
          ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_MSG_DELETE_TEMPLATE:
        switch (cmd_ptr->cmd.msg_delete_template.mem_store)
        {
          case WMS_MEMORY_STORE_RAM_CDMA:
          case WMS_MEMORY_STORE_RUIM:
          case WMS_MEMORY_STORE_NV_CDMA:
            ret_cmd_err = WMS_CMD_ERR_BUSY;
            break;
        }
        break;

      case WMS_CMD_MSG_DELETE_TEMPLATE_ALL:
        switch (cmd_ptr->cmd.msg_delete_template_all.mem_store)
        {
          case WMS_MEMORY_STORE_RAM_CDMA:
          case WMS_MEMORY_STORE_RUIM:
          case WMS_MEMORY_STORE_NV_CDMA:
            ret_cmd_err = WMS_CMD_ERR_BUSY;
            break;
        }
        break;

      case WMS_CMD_MSG_READ_STS_REPORT:
      case WMS_CMD_MSG_WRITE_STS_REPORT:
      case WMS_CMD_MSG_DELETE_STS_REPORT:
      case WMS_CMD_MSG_DELETE_STS_REPORT_ALL:
        break;

      case WMS_CMD_DC_CONNECT:
      case WMS_CMD_DC_DISCONNECT:
      case WMS_CMD_DC_ENABLE_AUTO_DISCONNECT:
      case WMS_CMD_DC_DISABLE_AUTO_DISCONNECT:
        ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;
#ifdef CUST_EDITION
      case WMS_CMD_BC_GET_CONFIG:
      case WMS_CMD_BC_GET_PREF:
      case WMS_CMD_BC_SET_PREF:
      case WMS_CMD_BC_GET_TABLE:
      case WMS_CMD_BC_SELECT_SRV:
      case WMS_CMD_BC_GET_ALL_SRV_IDS:
      case WMS_CMD_BC_GET_SRV_INFO:
      case WMS_CMD_BC_ADD_SRV:
      case WMS_CMD_BC_DELETE_SRV:
      case WMS_CMD_BC_CHANGE_LABEL:
      case WMS_CMD_BC_DELETE_ALL_SERVICES:
      case WMS_CMD_BC_SET_PRIORITY_FOR_ALL:
        ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;
#endif /*CUST_EDITION*/

#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
      case WMS_CMD_BC_MM_GET_CONFIG:
        if (cmd_ptr->cmd.bc_mm_get_config.message_mode == WMS_MESSAGE_MODE_CDMA)
          ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_BC_MM_GET_PREF:
        if (cmd_ptr->cmd.bc_mm_get_pref.message_mode == WMS_MESSAGE_MODE_CDMA)
          ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_BC_MM_SET_PREF:
        if (cmd_ptr->cmd.bc_mm_set_pref.message_mode == WMS_MESSAGE_MODE_CDMA)
          ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_BC_MM_GET_TABLE:
        if (cmd_ptr->cmd.bc_mm_get_table.message_mode == WMS_MESSAGE_MODE_CDMA)
          ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_BC_MM_SELECT_SRV:
        if (cmd_ptr->cmd.bc_mm_select_srv.message_mode == WMS_MESSAGE_MODE_CDMA)
          ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_BC_MM_GET_ALL_SRV_IDS:
        if (cmd_ptr->cmd.bc_mm_get_all_srv_ids.message_mode == WMS_MESSAGE_MODE_CDMA)
          ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_BC_MM_GET_SRV_INFO:
        if (cmd_ptr->cmd.bc_mm_get_srv_info.message_mode == WMS_MESSAGE_MODE_CDMA)
          ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_BC_MM_ADD_SRV:
        if (cmd_ptr->cmd.bc_mm_add_srv.message_mode == WMS_MESSAGE_MODE_CDMA)
          ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_BC_MM_DELETE_SRV:
        if (cmd_ptr->cmd.bc_mm_delete_srv.message_mode == WMS_MESSAGE_MODE_CDMA)
          ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_BC_MM_CHANGE_SRV_INFO:
        if (cmd_ptr->cmd.bc_mm_change_srv_info.message_mode == WMS_MESSAGE_MODE_CDMA)
          ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_BC_MM_DELETE_ALL_SERVICES:
        if (cmd_ptr->cmd.bc_mm_delete_all_srvs.message_mode == WMS_MESSAGE_MODE_CDMA)
          ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_BC_MM_SELECT_ALL_SERVICES:
        if (cmd_ptr->cmd.bc_mm_select_all_srvs.message_mode == WMS_MESSAGE_MODE_CDMA)
          ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_BC_MM_SET_PRIORITY_ALL_SERVICES:
        if (cmd_ptr->cmd.bc_mm_select_all_srvs.message_mode == WMS_MESSAGE_MODE_CDMA)
          ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_BC_MM_MSG_DELETE_INDICATION:
      case WMS_CMD_BC_MM_MSG_DELETE_ALL_INDICATION:
        break;

      case WMS_CMD_BC_MM_GET_READING_PREF:
        if (cmd_ptr->cmd.bc_mm_get_reading_pref.message_mode == WMS_MESSAGE_MODE_CDMA)
          ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_BC_MM_SET_READING_PREF:
        if (cmd_ptr->cmd.bc_mm_set_reading_pref.message_mode == WMS_MESSAGE_MODE_CDMA)
          ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */

      case WMS_CMD_DBG_RESET_TL_SEQ_NUM:
      case WMS_CMD_DBG_GET_RETRY_INTERVAL:
      case WMS_CMD_DBG_SET_RETRY_INTERVAL:
        ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_DBG_SET_MSG_REF_NUM:
      case WMS_CMD_DBG_CLEAR_SMMA:
        break;
#ifndef CUST_EDITION
	//case WMS_CMD_MSG_MC_MT_MSG_E:
#endif        
      // Allow since we could be getting this back
      // after sending MO even if network goes down
      case WMS_CMD_MSG_MC_MO_STATUS_E:
        break;

      case WMS_CMD_MSG_MC_MT_MSG_E:
      case WMS_CMD_MSG_MC_STATE_CHANGED_E:
      case WMS_CMD_MSG_MC_MWI_E:
        ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_MSG_GSTK_EVT:
        break;

      case WMS_CMD_DC_CALL_INCOM_E:
      case WMS_CMD_DC_CALL_CONNECT_E:
      case WMS_CMD_DC_CALL_END_E:
      case WMS_CMD_DC_CALL_ERROR_E:
        ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_CM_BC_ENABLE_FAILURE_E:
        ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_CM_START_FULL_SERVICE_E:
      case WMS_CMD_CM_ONLINE_E:
        break;

      case WMS_CMD_GSDI_ASYNC_CB:
      case WMS_CMD_GSDI_CARD_STATUS_CB:
        break;
#ifdef CUST_EDITION
#if defined(FEATURE_UIM_TOOLKIT)
      case WMS_CMD_REFRESH_IND:
#endif        
#endif /*CUST_EDITION*/
      case WMS_CMD_GSDI_REFRESH_FCN_IND:
      case WMS_CMD_BC_SCPT_DATA:
        ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_SS_CHANGE_INFO:
      case WMS_CMD_CM_LPM_E:
#ifdef FEATURE_SMS_OVER_IP
      case WMS_CMD_IPAPP_CHANGE_INFO:
#endif /* FEATURE_SMS_OVER_IP */
#ifdef FEATURE_APPS_POWER_COLLAPSE
#error code not present
#endif /* FEATURE_APPS_POWER_COLLAPSE */
        break;

      default:
        ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;
    }
  }
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
  return ret_cmd_err;
}

/* <EJECT> */
/*===========================================================================

FUNCTION wms_put_cmd

DESCRIPTION
  The API functions calls uses this function to put commands to the WMS.

DEPENDENCIES
  wms_cmd_q already initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  Add a command to the command queue.

===========================================================================*/
void wms_put_cmd
(
  wms_cmd_type  *cmd_ptr
)
{
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_WMS_DEBUG_STRING
  MSG_SPRINTF_2(MSG_SSID_DFLT, MSG_LEGACY_HIGH, "Putting %d %s", cmd_ptr->hdr.cmd_id, wms_dbg_get_cmd_string(cmd_ptr->hdr.cmd_id));
#else
  MSG_HIGH("wms put cmd : %d", cmd_ptr->hdr.cmd_id, 0, 0);
#endif /* FEATURE_WMS_DEBUG_STRING */

  (void) q_link( cmd_ptr, & cmd_ptr->hdr.link );  /* init link */
  q_put( & cmd_q, & cmd_ptr->hdr.link );          /* and queue it */
  (void) rex_set_sigs( & wms_tcb, WMS_CMD_Q_SIG ); /* signal the task */

  MSG_LOW("cmd_free_q cnt=%d", q_cnt( & cmd_free_q ), 0, 0 );
  MSG_LOW("cmd_q cnt=%d", q_cnt( & cmd_q ), 0, 0 );

} /* wms_put_cmd() */

/* <EJECT> */
/*===========================================================================

FUNCTION wms_get_cmd_count

DESCRIPTION
  The API function call returns the number of commands to WMS

DEPENDENCIES
  wms_cmd_q already initialized.

RETURN VALUE
  Count of the number of commands

SIDE EFFECTS
  None.

===========================================================================*/
int wms_get_cmd_count
(
  void
)
{
  MSG_LOW("cmd_q cnt=%d", q_cnt( & cmd_q ), 0, 0 );
  return (q_cnt( & cmd_q ));
} /* wms_get_cmd_count() */

/* <EJECT> */
/*=========================================================================
FUNCTION
  wms_process_cmd

DESCRIPTION
  Process the commands from the API clients.

DEPENDENCIES
  wms_init() was done.

RETURN VALUE
  None

SIDE EFFECTS
  Command buffer is put back to the free queue.

=========================================================================*/
void wms_process_cmd
(
  wms_cmd_type  *cmd_ptr
)
{
  wms_cmd_err_e_type cmd_err;
  wms_client_s_type   *client_ptr = wms_client_ptr(cmd_ptr->hdr.client_id);

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_WMS_DEBUG_STRING
  MSG_SPRINTF_2(MSG_SSID_DFLT, MSG_LEGACY_HIGH, "Processing %d %s", cmd_ptr->hdr.cmd_id, wms_dbg_get_cmd_string(cmd_ptr->hdr.cmd_id));
#else
  MSG_HIGH("wms process cmd : %d", cmd_ptr->hdr.cmd_id, 0, 0);
#endif /* FEATURE_WMS_DEBUG_STRING */

  if( client_ptr == NULL || client_ptr->activated == FALSE )
  {
    if( cmd_ptr->hdr.cmd_cb )
    {
      cmd_ptr->hdr.cmd_cb( cmd_ptr->hdr.cmd_id,
                           cmd_ptr->hdr.user_data,
                           WMS_CMD_ERR_CLIENT_ID );
    }
  #ifdef FEATURE_CDSMS
    cmd_ptr->hdr.done_q_ptr = & cmd_free_q;
  #endif /* FEATURE_CDSMS */
    if( cmd_ptr->hdr.done_q_ptr )  /* Put it back on a queue */
    {
       q_put( cmd_ptr->hdr.done_q_ptr, & cmd_ptr->hdr.link );
    }
    return;
  }

  if ((cmd_err = wms_check_for_valid_command(cmd_ptr)) != WMS_CMD_ERR_NONE)
  {
    /* Dont Process the Command because of an Error
       Return Error Code returned by function */
    if( cmd_ptr->hdr.cmd_cb )
    {
      cmd_ptr->hdr.cmd_cb(cmd_ptr->hdr.cmd_id,
                          cmd_ptr->hdr.user_data,
                          cmd_err );
    }
  #ifdef FEATURE_CDSMS
    cmd_ptr->hdr.done_q_ptr = & cmd_free_q;
  #endif /* FEATURE_CDSMS */
    if( cmd_ptr->hdr.done_q_ptr )  /* Put it back on a queue */
    {
      q_put( cmd_ptr->hdr.done_q_ptr, & cmd_ptr->hdr.link );
    }
    return;
  }

  switch( cmd_ptr->hdr.cmd_id )
  {
    /* Configuration group commands
    */
    case WMS_CMD_CFG_SET_ROUTES:
    case WMS_CMD_CFG_GET_ROUTES:
    case WMS_CMD_CFG_GET_MEMORY_STATUS:
    case WMS_CMD_CFG_GET_MESSAGE_LIST:
#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
    case WMS_CMD_CFG_SET_PRIMARY_CLIENT:
    case WMS_CMD_CFG_SET_MEMORY_FULL:
    case WMS_CMD_CFG_SET_LINK_CONTROL:
    case WMS_CMD_CFG_GET_LINK_CONTROL:
#ifdef CUST_EDITION	
    case WMS_CMD_CFG_SET_MULTISEND:
#ifdef FEATURE_AUTOREPLACE
    case WMS_CMD_CFG_SET_AUTOREPLACE:
#endif        
#endif /*CUST_EDITION*/
      wms_cfg_process_cmd( cmd_ptr );
      break;

    /* Message group commands
    */
    case WMS_CMD_MSG_SEND:
    case WMS_CMD_MSG_ACK:
    case WMS_CMD_MSG_READ:
    case WMS_CMD_MSG_WRITE:
    case WMS_CMD_MSG_DELETE:
#ifdef CUST_EDITION	
    case WMS_CMD_MSG_DELETE_BOX:
    case WMS_CMD_MSG_COPY:
#endif /*CUST_EDITION*/	
    case WMS_CMD_MSG_DELETE_ALL:
    case WMS_CMD_MSG_MODIFY_TAG:
    case WMS_CMD_MSG_READ_TEMPLATE:
    case WMS_CMD_MSG_WRITE_TEMPLATE:
    case WMS_CMD_MSG_DELETE_TEMPLATE:
    case WMS_CMD_MSG_DELETE_TEMPLATE_ALL:

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_GWSMS_MO_CONTROL
#error code not present
#endif /* FEATURE_GWSMS_MO_CONTROL */

#ifdef FEATURE_CDSMS
    /* Internal: events from MC
    */
    case WMS_CMD_MSG_MC_MT_MSG_E:
    case WMS_CMD_MSG_MC_MO_STATUS_E:
    case WMS_CMD_MSG_MC_STATE_CHANGED_E:
    case WMS_CMD_MSG_MC_MWI_E:
#endif /* FEATURE_CDSMS */
      wms_msg_process_cmd( cmd_ptr );
      break;

    /* DC group commands
    */
#ifdef FEATURE_CDSMS
    case WMS_CMD_DC_CONNECT:
    case WMS_CMD_DC_DISCONNECT:
    case WMS_CMD_DC_ENABLE_AUTO_DISCONNECT:
    case WMS_CMD_DC_DISABLE_AUTO_DISCONNECT:
      wms_dc_process_cmd( cmd_ptr );
      break;
#endif /* FEATURE_CDSMS */


    /* BC commands
    */
#ifndef CUST_EDITION  /*FEATURE_BROADCAST_SMS*/
#ifdef FEATURE_BROADCAST_SMS
    case WMS_CMD_BC_GET_CONFIG:
    case WMS_CMD_BC_GET_PREF:
    case WMS_CMD_BC_SET_PREF:
    case WMS_CMD_BC_GET_TABLE:
    case WMS_CMD_BC_SELECT_SRV:
    case WMS_CMD_BC_GET_ALL_SRV_IDS:
    case WMS_CMD_BC_GET_SRV_INFO:
    case WMS_CMD_BC_ADD_SRV:
    case WMS_CMD_BC_DELETE_SRV:
    case WMS_CMD_BC_CHANGE_LABEL:
    case WMS_CMD_BC_DELETE_ALL_SERVICES:
    case WMS_CMD_BC_SET_PRIORITY_FOR_ALL:
#ifdef FEATURE_CDSMS_IS637B_BROADCAST_SCPT
    case WMS_CMD_BC_SCPT_DATA: /* Internal command */
#endif /* FEATURE_CDSMS_IS637B_BROADCAST_SCPT */
      wms_bc_process_cmd( cmd_ptr );
      break;
#endif /* FEATURE_BROADCAST_SMS */
#endif

    /* BC MM commands
    */
#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
    case WMS_CMD_BC_MM_GET_CONFIG:
    case WMS_CMD_BC_MM_GET_PREF:
    case WMS_CMD_BC_MM_SET_PREF:
    case WMS_CMD_BC_MM_GET_TABLE:
    case WMS_CMD_BC_MM_SELECT_SRV:
    case WMS_CMD_BC_MM_GET_ALL_SRV_IDS:
    case WMS_CMD_BC_MM_GET_SRV_INFO:
    case WMS_CMD_BC_MM_ADD_SRV:
    case WMS_CMD_BC_MM_DELETE_SRV:
    case WMS_CMD_BC_MM_CHANGE_SRV_INFO:
    case WMS_CMD_BC_MM_DELETE_ALL_SERVICES:
    case WMS_CMD_BC_MM_SELECT_ALL_SERVICES:
    case WMS_CMD_BC_MM_SET_PRIORITY_ALL_SERVICES:
    case WMS_CMD_BC_MM_MSG_DELETE_INDICATION:
    case WMS_CMD_BC_MM_MSG_DELETE_ALL_INDICATION:
    case WMS_CMD_BC_MM_GET_READING_PREF:
    case WMS_CMD_BC_MM_SET_READING_PREF:
#ifdef FEATURE_CDSMS_BROADCAST
  case WMS_CMD_BC_SCPT_DATA: /* Internal command */
#endif /* FEATURE_CDSMS_BROADCAST */
      wms_bc_mm_process_cmd( cmd_ptr );
      break;
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */


    /* DBG commands
    */
#ifdef FEATURE_CDSMS
    case WMS_CMD_DBG_RESET_TL_SEQ_NUM:
      wms_dbg_process_cmd( cmd_ptr );
      break;
#endif /* FEATURE_CDSMS */
#ifdef FEATURE_GWSMS
#error code not present
#endif/*FEATURE_GWSMS*/
   case WMS_CMD_DBG_GET_RETRY_INTERVAL:
   case WMS_CMD_DBG_SET_RETRY_INTERVAL:
     wms_dbg_process_cmd( cmd_ptr );
     break;

    /* Internal: events from CM
    */
#ifdef FEATURE_CDSMS
    case WMS_CMD_DC_CALL_INCOM_E:
    case WMS_CMD_DC_CALL_CONNECT_E:
    case WMS_CMD_DC_CALL_END_E:
    case WMS_CMD_DC_CALL_ERROR_E:  /* in case of CM cmd errors */
      wms_dc_process_cmd( cmd_ptr );
      break;
#endif /* FEATURE_CDSMS */


    case WMS_CMD_CM_START_FULL_SERVICE_E:
    case WMS_CMD_CM_BC_ENABLE_FAILURE_E:
    case WMS_CMD_CM_ONLINE_E:
    case WMS_CMD_SS_CHANGE_INFO:
    case WMS_CMD_CM_LPM_E:

#ifdef FEATURE_SMS_OVER_IP
    case WMS_CMD_IPAPP_CHANGE_INFO:
#endif /* FEATURE_SMS_OVER_IP */
      wms_cm_process_event(cmd_ptr);
      break;

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS*/

#ifdef FEATURE_MMGSDI
    case WMS_CMD_GSDI_CARD_STATUS_CB:
      wms_sim_gsdi_card_status_cb_proc( cmd_ptr );
      break;
#endif /* FEATURE_MMGSDI */

#if defined(FEATURE_GWSMS) || defined(FEATURE_CDSMS)
#ifdef FEATURE_GSTK
    case WMS_CMD_GSDI_REFRESH_FCN_IND:
      wms_card_refresh_fcn_ind_proc( cmd_ptr );
      break;
#endif /* FEATURE_GSTK */
#endif /* FEATURE_CDSMS || FEATURE_GWSMS */

#ifdef FEATURE_APPS_POWER_COLLAPSE
#error code not present
#endif /* FEATURE_APPS_POWER_COLLAPSE */

    default:
      MSG_ERROR("Invalid cmd: %d", cmd_ptr->hdr.cmd_id, 0,0);
      break;
  }


  /* ----------- done with command processing -------------- */


  /* clean up command item
  */

  if( cmd_ptr->hdr.done_q_ptr )  /* Put it back on a queue */
  {
    q_put( cmd_ptr->hdr.done_q_ptr, & cmd_ptr->hdr.link );
  }


#ifdef FEATURE_CDSMS
  /* -------------------------------------------------------------
  ** Reset the done_q_ptr to pointing at the free queue
  ** since it might be changed to pointing to the pending msg queue
  ** due to processing of certain commands.
  **
  ** Next time when a command in the pending queue is processed,
  ** it will be put back into the free queue, unless done_q is changed
  ** to pending queue again.
  ** ------------------------------------------------------------- */
  cmd_ptr->hdr.done_q_ptr = & cmd_free_q;

#endif /* FEATURE_CDSMS */


  /* done */
  return;

} /* wms_process_cmd() */


#ifdef FEATURE_GWSMS
#endif /* FEATURE_GWSMS */


/* <EJECT> */
/*=========================================================================
FUNCTION
  wms_process_signals

DESCRIPTION
  Process the signals for SMS related commands and timers.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void wms_process_signals
(
  rex_sigs_type              sigs
)
{
  wms_cmd_type             * cmd_ptr;
  rex_sigs_type              wms_sigs;  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(0 != (sigs & TASK_STOP_SIG)) {
     MSG_MED( "TASK_STOP_SIG received", 0,0,0 );
     (void) rex_clr_sigs(&wms_tcb, TASK_STOP_SIG);

     /* Process task stop procedure.*/
     task_stop();
  }


  if (0 != (sigs & TASK_OFFLINE_SIG)) {
     MSG_MED(" TASK_OFFLINE_SIG received",0,0,0);
     (void) rex_clr_sigs(&wms_tcb, TASK_OFFLINE_SIG);

     /* Process task offline procedure.*/
     wms_offline();
  }

  /* Dispatch other rex signals */
  if (0 == (sigs & WMS_SIGNALS)) {
     /* No WMS signals, early return */
     return;
  }

  /* Kick the dog */
  wms_kick_dog();

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */


  if( sigs & WMS_CMD_Q_SIG )
  {
    rex_sigs_type wms_sigs;
    
    MSG_MED( "got WMS_CMD_Q_SIG", 0, 0, 0 );

    (void) rex_clr_sigs( rex_self(), WMS_CMD_Q_SIG );

    while( ( cmd_ptr = (wms_cmd_type *) q_get( & cmd_q )) != NULL )
    {
      wms_sigs = rex_get_sigs(&wms_tcb);

      if (wms_sigs & WMS_RPT_TIMER_SIG)
      {
        /* Also clears WMS_RPT_TIMER_SIG */
        wms_kick_dog();
      }

      wms_process_cmd( cmd_ptr );     /* actually handle it */
    }

  } /* if WMS_CMD_Q_SIG */


#ifdef FEATURE_CDSMS

  if( sigs & WMS_DC_CALL_OUT_TIMER_SIG )
  {
    wms_dc_call_out_timer_proc();
  }

  if( sigs & WMS_DC_IDLE_TIMER_SIG )
  {
    wms_dc_idle_timer_proc();
  }

#ifndef CUST_EDITION
  if( sigs & WMS_MT_ACK_TIMER_SIG )
  {
    wms_msg_cdma_mt_ack_timer_proc();
  }
#endif

  if( sigs & WMS_AWISMS_ACK_TIMER_SIG )
  {
    wms_msg_awi_ack_timer_proc();
  }
#endif /* FEATURE_CDSMS */

#ifndef CUST_EDITION
  if(sigs & WMS_MO_RETRY_TIMER_SIG)
  {
    wms_msg_process_retry_sig();
  }
#endif

  /* done */
  return;

} /* end of wms_process_signals() */


/* <EJECT> */
/*=========================================================================
FUNCTION
  wms_offline

DESCRIPTION
  Get ready for phone going offline.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
static void wms_offline
(
  void
)
{

  wms_msg_offline();

  /* send ack to TMC/MC that WMS is offline*/
  task_offline();

} /* wms_offline() */

#ifndef FEATURE_IWMSC
/* <EJECT> */
/*===========================================================================
FUNCTION wms_task

DESCRIPTION
  The main WMS function.  This is the big loop outside of the
  state machine which handles signals and all sorts of inputs.

DEPENDENCIES
  REX is running.

RETURN VALUE
  Never exits.

SIDE EFFECTS

===========================================================================*/
void wms_task
(
  uint32 dummy                     
                                  /*ignore fact that this isn't used */
)
{
  rex_sigs_type  sigs;       /* hold signals */

  wms_init();              /* initialize queues, timers, etc. */

  for( ;; )
  {
    /* Wait on REX signals, repeat forever */

    sigs = rex_wait
             (
                WMS_SIGNALS
                | TASK_STOP_SIG
                | TASK_OFFLINE_SIG
             );

    wms_process_signals( sigs );

  } // for(;;)

} /* wms_task() */
#endif /* !FEATURE_IWMSC */


/* <EJECT> */
/*=========================================================================
FUNCTION
  wms_client_cmd_status

DESCRIPTION
  Issue client command status (error or no-error) to the client.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void wms_client_cmd_status
(
  wms_cmd_type        *cmd_ptr,
  wms_cmd_err_e_type  cmd_err
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (cmd_err != WMS_CMD_ERR_NONE)
  {
    /* There is some real error */
#ifdef FEATURE_WMS_DEBUG_STRING
    MSG_SPRINTF_2(MSG_SSID_DFLT, MSG_LEGACY_HIGH, "Error: %d %s", cmd_err, wms_dbg_get_cmd_err_string(cmd_err));
#else
    MSG_HIGH("Error: %d ", cmd_err, 0, 0);
#endif /* FEATURE_WMS_DEBUG_STRING */
  }

  if( cmd_ptr && cmd_ptr->hdr.cmd_cb )
  {
    cmd_ptr->hdr.cmd_cb( cmd_ptr->hdr.cmd_id,
                         cmd_ptr->hdr.user_data,
                         cmd_err );
  }

} /* wms_client_cmd_status() */




/* <EJECT> */
/*===========================================================================
=============================================================================
============================== API FUNCTIONS ================================
=============================================================================
===========================================================================*/

/*===========================================================================

                            Configuration Group

                        API FUNCTION DEFINITIONS

===========================================================================*/

/*=========================================================================
FUNCTION
  wms_cfg_set_routes

DESCRIPTION
  Allow the client to change the message routing configuration.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_cfg_set_routes
(
  wms_client_id_type               client_id,
  wms_cmd_cb_type                  cmd_cb,
  const void                       * user_data,
  const wms_routes_s_type          * routes_ptr
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_backup.hdr.cmd_id      = WMS_CMD_CFG_SET_ROUTES;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;

  if (routes_ptr != NULL)
  {
    cmd_backup.cmd.cfg_set_routes.routes      = *routes_ptr;

    cmd_ptr = wms_get_cmd_buf();

    if(cmd_ptr != NULL)
    {
      * cmd_ptr = cmd_backup;
      wms_put_cmd( cmd_ptr );
    }
    else
    {
      wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NO_RESOURCE );
      st = WMS_OUT_OF_RESOURCES_S;
    }
  }
  else
  {
    /* routes_ptr is NULL */
    wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NULL_PTR);
    st = WMS_NULL_PTR_S;
  }
  return st;
} /* wms_cfg_set_routes */

/* <EJECT> */
/*=========================================================================
FUNCTION
  wms_cfg_get_routes

DESCRIPTION
  Allow the client to retrieve the current message routing configuration.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_cfg_get_routes
(
  wms_client_id_type               client_id,
  wms_cmd_cb_type                  cmd_cb,
  const void                       * user_data
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_backup.hdr.cmd_id      = WMS_CMD_CFG_GET_ROUTES;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;

  cmd_ptr = wms_get_cmd_buf();
  if( cmd_ptr !=NULL )
  {
    * cmd_ptr = cmd_backup;
    wms_put_cmd( cmd_ptr );
  }
  else
  {
    wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NO_RESOURCE );
    st = WMS_OUT_OF_RESOURCES_S;
  }

  return st;
} /* wms_cfg_get_routes */

/* <EJECT> */
/*=========================================================================
FUNCTION
  wms_cfg_get_memory_status

DESCRIPTION
  Allow the client to retrieve the status for a memory store.
  If tag==WMS_TAG_NONE: all used slots are calculated;
  otherwise, only messages having this tag value are counted as used_tag_slots.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_cfg_get_memory_status
(
  wms_client_id_type               client_id,
  wms_cmd_cb_type                  cmd_cb,
  const void                       * user_data,
  wms_memory_store_e_type          mem_store,
  wms_message_tag_e_type           tag
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_backup.hdr.cmd_id      = WMS_CMD_CFG_GET_MEMORY_STATUS;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;

  cmd_backup.cmd.cfg_get_mem_status.mem_store = mem_store;
  cmd_backup.cmd.cfg_get_mem_status.tag       = tag;

  cmd_ptr = wms_get_cmd_buf();

  if( cmd_ptr != NULL)
  {
    * cmd_ptr = cmd_backup;
    wms_put_cmd( cmd_ptr );
  }
  else
  {
    wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NO_RESOURCE );
    st = WMS_OUT_OF_RESOURCES_S;
  }

  return st;
} /* wms_cfg_get_memory_status */

/* <EJECT> */
/*=========================================================================
FUNCTION
  wms_cfg_get_message_list

DESCRIPTION
  Allow the client to retrieve the message list for a memory store.
  If tag==WMS_TAG_NONE: all types of messages are included in the list;
  otherwise, only messages having this tag value are included.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_cfg_get_message_list
(
  wms_client_id_type               client_id,
  wms_cmd_cb_type                  cmd_cb,
  const void                       * user_data,
  wms_memory_store_e_type          mem_store,
  wms_message_tag_e_type           tag
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_backup.hdr.cmd_id      = WMS_CMD_CFG_GET_MESSAGE_LIST;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;

  cmd_backup.cmd.cfg_get_msg_list.mem_store = mem_store;
  cmd_backup.cmd.cfg_get_msg_list.tag       = tag;

  cmd_ptr = wms_get_cmd_buf();

  if( cmd_ptr != NULL)
  {
    * cmd_ptr = cmd_backup;
    wms_put_cmd( cmd_ptr );
  }
  else
  {
    wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NO_RESOURCE );
    st = WMS_OUT_OF_RESOURCES_S;
  }
  return st;
} /* wms_cfg_get_message_list */

/* <EJECT> */
/*=========================================================================
FUNCTION
  wms_cfg_set_gw_domain_pref

DESCRIPTION
  Allow the client to set the GW Domain Preference

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_cfg_set_gw_domain_pref
(
  wms_client_id_type               client_id,
  wms_cmd_cb_type                  cmd_cb,
  const void                       * user_data,
  wms_gw_domain_pref_e_type         gw_domain_pref
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr = & cmd_backup;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_backup.hdr.cmd_id      = WMS_CMD_CFG_SET_GW_DOMAIN_PREF;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;

#ifdef FEATURE_GWSMS
#error code not present
#else /* FEATURE_GWSMS */
  wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_UNSUPPORTED );
  st = WMS_UNSUPPORTED_S;
#endif /* FEATURE_GWSMS */

  return st;
} /* wms_cfg_set_gw_domain_pref */

/* <EJECT> */
/*=========================================================================
FUNCTION
  wms_cfg_get_gw_domain_pref

DESCRIPTION
  Allow the client to get the GW Domain Preference

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_cfg_get_gw_domain_pref
(
  wms_client_id_type               client_id,
  wms_cmd_cb_type                  cmd_cb,
  const void                       * user_data
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr = & cmd_backup;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_backup.hdr.cmd_id      = WMS_CMD_CFG_GET_GW_DOMAIN_PREF;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;


#ifdef FEATURE_GWSMS
#error code not present
#else /* FEATURE_GWSMS */
  wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_UNSUPPORTED );
  st = WMS_UNSUPPORTED_S;
#endif /* FEATURE_GWSMS */

  return st;
} /* wms_cfg_get_gw_domain_pref */

/* <EJECT> */
/*===========================================================================
FUNCTION wms_cfg_set_link_control

DESCRIPTION
  Send a request to the lower layers to disable/enable SMS link control for
  connection establishment and timer duration

  Parameter values:
    control_option
        0: Disable - Lower layers don't need to keep up the link when a
                    MO SMS is sent
        1: Enable - If the timer (link period) expires, close the link and
                    automatically switch back to 0 (Disable)
        2: Enable - If the timer (linke period) expires, close the link and
                    retain the perference to be 2
    link_period - timer between the previous and next send command
                    (1 to 5 second interval)
DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
wms_status_e_type wms_cfg_set_link_control
(
  wms_client_id_type                            client_id,
  wms_cmd_cb_type                               cmd_cb,
  const void                                    *user_data,
  wms_cfg_link_control_mode_e_type              control_option,
  uint8                                         idle_timer /* in seconds */
)
{
  wms_status_e_type             st = WMS_OK_S;
  wms_cmd_type                  *cmd_ptr = & cmd_backup;

  cmd_backup.hdr.cmd_id         = WMS_CMD_CFG_SET_LINK_CONTROL;
  cmd_backup.hdr.client_id      = client_id;
  cmd_backup.hdr.cmd_cb         = cmd_cb;
  cmd_backup.hdr.user_data      = (void *)user_data;

#ifdef FEATURE_GWSMS
#error code not present
#else /* FEATURE_GWSMS */
  wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_UNSUPPORTED );
  st = WMS_UNSUPPORTED_S;
#endif /* FEATURE_GWSMS */

  /* done */
  return st;
} /* wms_cfg_set_link_control */

/* <EJECT> */
/*===========================================================================
FUNCTION wms_cfg_get_link_control

DESCRIPTION
  Retrieves the current link control setting.  WMS will send a cfg event to
  all clients to infom if the link is up or down.and the mode

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
wms_status_e_type wms_cfg_get_link_control
(
  wms_client_id_type                            client_id,
  wms_cmd_cb_type                               cmd_cb,
  const void                                    * user_data
)
{
  wms_status_e_type             st = WMS_OK_S;
  wms_cmd_type                  *cmd_ptr = & cmd_backup;

  cmd_backup.hdr.cmd_id         = WMS_CMD_CFG_GET_LINK_CONTROL;
  cmd_backup.hdr.client_id      = client_id;
  cmd_backup.hdr.cmd_cb         = cmd_cb;
  cmd_backup.hdr.user_data      = (void *)user_data;

#ifdef FEATURE_GWSMS
#error code not present
#else /* FEATURE_GWSMS */
  wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_UNSUPPORTED );
  st = WMS_UNSUPPORTED_S;
#endif /* FEATURE_GWSMS */

  /* done */
  return st;
} /* wms_cfg_get_link_control()  */

/* <EJECT> */
/*=========================================================================
FUNCTION
  wms_cfg_set_primary_client

DESCRIPTION
  This Allow the client to make itself the primary client. The advantage of
  being a  primary client is that your memory status is checked for memory
  full and SMMA Request sent to the network.

USAGE
  Once a primary client is set, no other client can become the primary client.
  If the set_primay == FALSE; the primary client is deregistered

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

SEE ALSO
  wms_cfg_set_memory_full()

=========================================================================*/
wms_status_e_type wms_cfg_set_primary_client
(
  wms_client_id_type      client_id,
  wms_cmd_cb_type         cmd_cb,
  const void            * user_data,
  boolean                 set_primary,
  boolean                 use_client_memory
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr = & cmd_backup;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_backup.hdr.cmd_id      = WMS_CMD_CFG_SET_PRIMARY_CLIENT;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;

  cmd_backup.cmd.cfg_set_primary_client.set_primary = set_primary;
  cmd_backup.cmd.cfg_set_primary_client.use_client_memory = use_client_memory;

  cmd_ptr = wms_get_cmd_buf();

  if( cmd_ptr != NULL)
  {
    * cmd_ptr = cmd_backup;
    wms_put_cmd( cmd_ptr );
  }
  else
  {
    wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NO_RESOURCE );
    st = WMS_OUT_OF_RESOURCES_S;
  }

  return st;
} /* wms_cfg_set_primary_client */

/* <EJECT> */
/*=========================================================================
FUNCTION
  wms_cfg_set_memory_full

DESCRIPTION
  This allows the primary client to specify its Memory Status.

USAGE
  Only the primary client will be allowed to set/reset the Memory Status.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

SEE ALSO
  wms_cfg_set_primary()
=========================================================================*/
wms_status_e_type wms_cfg_set_memory_full
(
  wms_client_id_type               client_id,
  wms_cmd_cb_type                  cmd_cb,
  const void                     * user_data,
  boolean                          memory_full
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr = & cmd_backup;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_backup.hdr.cmd_id      = WMS_CMD_CFG_SET_MEMORY_FULL;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;

  cmd_backup.cmd.cfg_set_memory_full.memory_full = memory_full;

  cmd_ptr = wms_get_cmd_buf();

  if( cmd_ptr != NULL)
  {
    * cmd_ptr = cmd_backup;
    wms_put_cmd( cmd_ptr );
  }
  else
  {
    wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NO_RESOURCE );
    st = WMS_OUT_OF_RESOURCES_S;
  }

  return st;
} /* wms_cfg_set_memory_full */
#ifdef CUST_EDITION	
/*=========================================================================
FUNCTION
  wms_cfg_set_multisend

DESCRIPTION
  This allows the primary client to specify its multisend Status.

USAGE
  Only the primary client will be allowed to set/reset the multisend Status.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

SEE ALSO

=========================================================================*/
wms_status_e_type wms_cfg_set_multisend
(
  wms_client_id_type               client_id,
  wms_cmd_cb_type                  cmd_cb,
  const void                     * user_data,
  boolean                          multisend
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr = & cmd_backup;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_backup.hdr.cmd_id      = WMS_CMD_CFG_SET_MULTISEND;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;

  cmd_backup.cmd.cfg_set_multisend.multisend = multisend;

  cmd_ptr = wms_get_cmd_buf();

  if( cmd_ptr != NULL)
  {
    * cmd_ptr = cmd_backup;  
    wms_put_cmd( cmd_ptr );
  }
  else
  {
    wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NO_RESOURCE );
    st = WMS_OUT_OF_RESOURCES_S;
  }

  return st;
} /* wms_cfg_set_multisend */

#ifdef FEATURE_AUTOREPLACE
/*=========================================================================
FUNCTION
  wms_cfg_set_autoreplace

DESCRIPTION
  This allows the primary client to specify its autoreplace Status.

USAGE
  Only the primary client will be allowed to set/reset the autoreplace Status.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

SEE ALSO

=========================================================================*/
wms_status_e_type wms_cfg_set_autoreplace
(
  wms_client_id_type               client_id,
  wms_cmd_cb_type                  cmd_cb,
  const void                     * user_data,
  boolean                          autoreplace
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr = & cmd_backup;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_backup.hdr.cmd_id      = WMS_CMD_CFG_SET_AUTOREPLACE;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;

  cmd_backup.cmd.cfg_set_autoreplace.autoreplace = autoreplace;

  cmd_ptr = wms_get_cmd_buf();

  if( cmd_ptr != NULL)
  {    
    * cmd_ptr = cmd_backup;
    wms_put_cmd( cmd_ptr );
  }
  else
  {
    wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NO_RESOURCE );
    st = WMS_OUT_OF_RESOURCES_S;
  }

  return st;
} /* wms_cfg_set_autoreplace */
#endif /* FEATURE_AUTOREPLACE */
#endif /*CUST_EDITION*/
/* <EJECT> */
/*===========================================================================

                            Message Group

                        API FUNCTION DEFINITIONS

===========================================================================*/
/*=========================================================================
FUNCTION
  wms_msg_send

DESCRIPTION
  Allow the client to send a message.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_msg_send
(
  wms_client_id_type                 client_id,
  wms_cmd_cb_type                    cmd_cb,
  const void                         * user_data,
  wms_send_mode_e_type               send_mode,
  const wms_client_message_s_type    * message_ptr
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef CUST_EDITION	
#if defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
  return WMS_OUT_OF_RESOURCES_S;
#endif  
#endif /*CUST_EDITION*/
  cmd_backup.hdr.cmd_id      = WMS_CMD_MSG_SEND;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;

  if (message_ptr != NULL)
  {
    cmd_backup.cmd.msg_send.send_mode = send_mode;
    cmd_backup.cmd.msg_send.message   = *message_ptr;
#ifdef CUST_EDITION	    
    // 去掉 MO 不需要的时间戳
    if ((WMS_SEND_MODE_CLIENT_MESSAGE == send_mode) &&
        (cmd_backup.cmd.msg_send.message.u.cdma_message.raw_ts.format == WMS_FORMAT_CDMA))
    {
        wms_raw_ts_data_s_type    *raw_ts;
        
        raw_ts = &(cmd_backup.cmd.msg_send.message.u.cdma_message.raw_ts);
        
        if ((wms_ts_decode(raw_ts, &client_ts) == WMS_OK_S))
        {
            if (client_ts.u.cdma.mask & WMS_MASK_BD_MC_TIME)
            {
                client_ts.u.cdma.mask = client_ts.u.cdma.mask & (~WMS_MASK_BD_MC_TIME);
            }
            
            (void)wms_ts_encode(&client_ts, raw_ts);
        }
    }
    
#ifdef FEATURE_CARRIER_ISRAEL_PELEPHONE
    {
        extern void wmsutil_formatmessage(wms_client_message_s_type *pclt_msg);
        
        // 以色列对发出的短信格式有要求，为取得最小修改，故在消息发出前重新格式化消息内容
        wmsutil_formatmessage(&(cmd_backup.cmd.msg_send.message));
    }
#endif
#endif /*CUST_EDITION*/
    cmd_ptr = wms_get_cmd_buf();

    if( cmd_ptr != NULL)
    {
      * cmd_ptr = cmd_backup;
      wms_put_cmd( cmd_ptr );
    }
    else
    {
      wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NO_RESOURCE );
      st = WMS_OUT_OF_RESOURCES_S;
    }
  }
  else
  {
    /* message_ptr is NULL */
    wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NULL_PTR);
    st = WMS_NULL_PTR_S;
  }
  return st;
} /* wms_msg_send */

/* <EJECT> */
/*=========================================================================
FUNCTION
  wms_msg_ack

DESCRIPTION
  Allow the client to acknowledge a message if the message has not been
  acknowledged.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_msg_ack
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  const wms_ack_info_s_type       *ack_info_ptr
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_backup.hdr.cmd_id      = WMS_CMD_MSG_ACK;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;

  if (ack_info_ptr != NULL)
  {
    cmd_backup.cmd.msg_ack.ack_info = *ack_info_ptr;

    cmd_ptr = wms_get_cmd_buf();

    if( cmd_ptr != NULL)
    {
      * cmd_ptr = cmd_backup;
      wms_put_cmd( cmd_ptr );
    }
    else
    {
      wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NO_RESOURCE );
      st = WMS_OUT_OF_RESOURCES_S;
    }
  }
  else
  {
    /* ack_info_ptr is NULL */
    wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NULL_PTR);
    st = WMS_NULL_PTR_S;
  }
  return st;
} /* wms_msg_ack */

/* <EJECT> */
/*=========================================================================
FUNCTION
  wms_msg_read

DESCRIPTION
  Allow the client to read a message from a memory store.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_msg_read
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_memory_store_e_type         mem_store,
  wms_message_index_type          index
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_backup.hdr.cmd_id      = WMS_CMD_MSG_READ;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;

  cmd_backup.cmd.msg_read.mem_store   = mem_store;
  cmd_backup.cmd.msg_read.index       = index;



  cmd_ptr = wms_get_cmd_buf();

  if( cmd_ptr != NULL)
  {
    * cmd_ptr = cmd_backup;
    wms_put_cmd( cmd_ptr );
  }
  else
  {
    wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NO_RESOURCE );
    st = WMS_OUT_OF_RESOURCES_S;
  }

  return st;
} /* wms_msg_read */

/* <EJECT> */
/*=========================================================================
FUNCTION
  wms_msg_write

DESCRIPTION
  Allow the client to write a message to a memory store.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_msg_write
(
  wms_client_id_type                 client_id,
  wms_cmd_cb_type                    cmd_cb,
  const void                         * user_data,
  wms_write_mode_e_type              write_mode,
  const wms_client_message_s_type    * message_ptr
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_backup.hdr.cmd_id      = WMS_CMD_MSG_WRITE;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;

  if (message_ptr != NULL)
  {
    cmd_backup.cmd.msg_write.write_mode = write_mode;
    cmd_backup.cmd.msg_write.message    = *message_ptr;

    cmd_ptr = wms_get_cmd_buf();

    if( cmd_ptr != NULL)
    {
      * cmd_ptr = cmd_backup;
      wms_put_cmd( cmd_ptr );
    }
    else
    {
      wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NO_RESOURCE );
      st = WMS_OUT_OF_RESOURCES_S;
    }
  }
  else
  {
    /* message_ptr is NULL */
    wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NULL_PTR);
    st = WMS_NULL_PTR_S;
  }
  return st;
} /* wms_msg_write */

/* <EJECT> */
/*=========================================================================
FUNCTION
  wms_msg_delete

DESCRIPTION
  Allow the client to delete a message from a memory store.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

COMMENTS
  The corresponding Status Report for the Message if stored is also deleted.

=========================================================================*/
wms_status_e_type wms_msg_delete
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_memory_store_e_type         mem_store,
  wms_message_index_type          index
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_backup.hdr.cmd_id      = WMS_CMD_MSG_DELETE;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;

  cmd_backup.cmd.msg_delete.mem_store   = mem_store;
  cmd_backup.cmd.msg_delete.index       = index;



  cmd_ptr = wms_get_cmd_buf();

  if( cmd_ptr != NULL)
  {
    * cmd_ptr = cmd_backup;
    wms_put_cmd( cmd_ptr );
  }
  else
  {
    wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NO_RESOURCE );
    st = WMS_OUT_OF_RESOURCES_S;
  }

  return st;
} /* wms_msg_delete */
#ifdef CUST_EDITION	
/* <EJECT> */
/*==============================================================================
FUNCTION
    wms_msg_delete_box

DESCRIPTION
    Allow the client to delete all message in a message box.

RETURN VALUE
    WMS_OK_S
    WMS_OUT_OF_RESOURCES_S

note
    A command is sent to WMS task.
==============================================================================*/
wms_status_e_type wms_msg_delete_box
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_deletebox_e_type            box_deltype
)
{
    wms_status_e_type   st = WMS_OK_S;
    wms_cmd_type        *cmd_ptr;
    
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    
    cmd_backup.hdr.cmd_id      = WMS_CMD_MSG_DELETE_BOX;
    cmd_backup.hdr.client_id   = client_id;
    cmd_backup.hdr.cmd_cb      = cmd_cb;
    cmd_backup.hdr.user_data   = (void*)user_data;
    
    cmd_backup.cmd.msg_delete_box.box_deltype = box_deltype;
    
    
    cmd_ptr = wms_get_cmd_buf();

    if (cmd_ptr != NULL)
    {
        * cmd_ptr = cmd_backup;   
        wms_put_cmd( cmd_ptr );
    }
    else
    {
        wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NO_RESOURCE );
        st = WMS_OUT_OF_RESOURCES_S;
    }
    
    return st;
} /* wms_msg_delete_box */

/* <EJECT> */
/*==============================================================================
FUNCTION
    wms_msg_copy

DESCRIPTION
    Allow the client to copy a message from one Storage to another Storage.

RETURN VALUE
    WMS_OK_S
    WMS_OUT_OF_RESOURCES_S

note
    A command is sent to WMS task. Only use for MT message copy between UIM
    and NV. If index_dest not equal to WMS_DUMMY_MESSAGE_INDEX, the dest message
    will be replaced.
==============================================================================*/
wms_status_e_type wms_msg_copy
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_msg_copy_type               *pmsg_copy
)
{
    wms_status_e_type   st = WMS_OK_S;
    wms_cmd_type        *cmd_ptr;
    
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    
    cmd_backup.hdr.cmd_id      = WMS_CMD_MSG_COPY;
    cmd_backup.hdr.client_id   = client_id;
    cmd_backup.hdr.cmd_cb      = cmd_cb;
    cmd_backup.hdr.user_data   = (void*)user_data;
    
    (void)memcpy(&(cmd_backup.cmd.msg_copy.copypram), pmsg_copy, sizeof(wms_msg_copy_type));
    
    cmd_ptr = wms_get_cmd_buf();

    if (cmd_ptr != NULL)
    {
        * cmd_ptr = cmd_backup;
        wms_put_cmd( cmd_ptr );
    }
    else
    {
        wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NO_RESOURCE );
        st = WMS_OUT_OF_RESOURCES_S;
    }
    
    return st;
} /* wms_msg_copy */

#if defined(FEATURE_UIM_TOOLKIT)
/*==============================================================================
FUNCTION
    wms_msg_refresh_ruimmsg

DESCRIPTION
    Allow the client to rebuild ruim sms cache.

RETURN VALUE
    WMS_OK_S
    WMS_OUT_OF_RESOURCES_S

note

==============================================================================*/
wms_status_e_type wms_msg_refresh_ruimmsg
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data
)
{
    wms_status_e_type   st = WMS_OK_S;
    wms_cmd_type        *cmd_ptr;
    
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    
    cmd_backup.hdr.cmd_id      = WMS_CMD_REFRESH_IND;
    cmd_backup.hdr.client_id   = client_id;
    cmd_backup.hdr.cmd_cb      = cmd_cb;
    cmd_backup.hdr.user_data   = (void*)user_data;
    
    cmd_ptr = wms_get_cmd_buf();

    if (cmd_ptr != NULL)
    {    
        * cmd_ptr = cmd_backup; 
        wms_put_cmd( cmd_ptr );
    }
    else
    {
        wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NO_RESOURCE );
        st = WMS_OUT_OF_RESOURCES_S;
    }
    
    return st;
} /* wms_msg_refresh_ruimmsg */
#endif
#endif /*CUST_EDITION*/
/* <EJECT> */
/*=========================================================================
FUNCTION
  wms_msg_delete_all

DESCRIPTION
  Allow the client to delete all message from a memory store.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_msg_delete_all
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_memory_store_e_type         mem_store,
  wms_message_tag_e_type          tag
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_backup.hdr.cmd_id      = WMS_CMD_MSG_DELETE_ALL;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;

  cmd_backup.cmd.msg_delete_all.mem_store   = mem_store;
  cmd_backup.cmd.msg_delete_all.tag         = tag;


  cmd_ptr = wms_get_cmd_buf();

  if( cmd_ptr != NULL)
  {
    * cmd_ptr = cmd_backup;
    wms_put_cmd( cmd_ptr );
  }
  else
  {
    wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NO_RESOURCE );
    st = WMS_OUT_OF_RESOURCES_S;
  }

  return st;
} /* wms_msg_delete_all */

/* <EJECT> */
/*=========================================================================
FUNCTION
  wms_msg_modify_tag

DESCRIPTION
  Allow the client to modify the tag of a message.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_msg_modify_tag
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_memory_store_e_type         mem_store,
  wms_message_index_type          index,
  wms_message_tag_e_type          tag
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_backup.hdr.cmd_id      = WMS_CMD_MSG_MODIFY_TAG;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;

  cmd_backup.cmd.msg_modify_tag.mem_store   = mem_store;
  cmd_backup.cmd.msg_modify_tag.index       = index;
  cmd_backup.cmd.msg_modify_tag.tag         = tag;


  cmd_ptr = wms_get_cmd_buf();

  if( cmd_ptr != NULL)
  {
    * cmd_ptr = cmd_backup;
    wms_put_cmd( cmd_ptr );
  }
  else
  {
    wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NO_RESOURCE );
    st = WMS_OUT_OF_RESOURCES_S;
  }

  return st;
} /* wms_msg_modify_tag */

/* <EJECT> */
/*=========================================================================
FUNCTION
  wms_msg_read_template

DESCRIPTION
  Allow the client to read a template from a memory store. If memory store
  is NV, the message index is ignored since only one template is in NV.
  Otherwise, the memory store can only be SIM.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_msg_read_template
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_memory_store_e_type         mem_store,
  wms_message_index_type          index
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_backup.hdr.cmd_id      = WMS_CMD_MSG_READ_TEMPLATE;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;

  cmd_backup.cmd.msg_read_template.mem_store   = mem_store;
  cmd_backup.cmd.msg_read_template.index       = index;


  cmd_ptr = wms_get_cmd_buf();

  if( cmd_ptr != NULL)
  {
    * cmd_ptr = cmd_backup;
    wms_put_cmd( cmd_ptr );
  }
  else
  {
    wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NO_RESOURCE );
    st = WMS_OUT_OF_RESOURCES_S;
  }

  return st;
} /* wms_msg_read_template */

/* <EJECT> */
/*=========================================================================
FUNCTION
  wmsi_msg_check_for_validity

DESCRIPTION
  Check if the message template has the right combination of message mode
  and memory store

DEPENDENCIES
  None

RETURN VALUE
  FALSE: The memory store is not supported for that message mode 
  TRUE: The memory store might be supported for that message mode

SIDE EFFECTS
  None

=========================================================================*/
boolean wmsi_msg_check_for_validity
(
  wms_message_mode_e_type message_mode,
  wms_memory_store_e_type mem_store
)
{
  if ((message_mode == WMS_MESSAGE_MODE_CDMA && 
       (mem_store == WMS_MEMORY_STORE_SIM || mem_store == WMS_MEMORY_STORE_NV_GW)) 
      ||
      (message_mode == WMS_MESSAGE_MODE_GW &&
      (mem_store == WMS_MEMORY_STORE_RUIM || mem_store == WMS_MEMORY_STORE_NV_CDMA))) 
  {
    return FALSE;
  }

  return TRUE;
} /* wmsi_msg_check_for_validity */

/* <EJECT> */
/*=========================================================================
FUNCTION
  wms_msg_write_template

DESCRIPTION
  Allow the client to write a template from a memory store. If memory store
  is NV, the message index is ignored since only one template is in NV.
  Otherwise, only SIM memory store is allowed.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_msg_write_template
(
  wms_client_id_type               client_id,
  wms_cmd_cb_type                  cmd_cb,
  const void                       * user_data,
  wms_write_mode_e_type            write_mode,
  const wms_client_message_s_type  * message_ptr
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_backup.hdr.cmd_id      = WMS_CMD_MSG_WRITE_TEMPLATE;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;

  cmd_backup.cmd.msg_write_template.write_mode = write_mode;
  cmd_backup.cmd.msg_write_template.message    = *message_ptr;


  cmd_ptr = wms_get_cmd_buf();

  if( cmd_ptr != NULL)
  {
    * cmd_ptr = cmd_backup;

    // We cannot assume the client set the right combination of message mode and 
    // memory store. Without this check the phone might crash.
    // For example, without this check, a GW template with a wrong message mode (CDMA) 
    // would call wms_ts_encode_smsp() if mem_store == STORE_SIM. wms_ts_encode_smsp()
    // would crash because alpha ID field was not copied over to the cmd_ptr.
    if (wmsi_msg_check_for_validity(message_ptr->msg_hdr.message_mode, 
                                    cmd_ptr->cmd.msg_write_template.message.msg_hdr.mem_store) == FALSE)
    {
      wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_MSG_MEMORY_STORE );
      st = WMS_GENERAL_ERROR_S;
      goto bail;
    }
    else
    {  /* wmsi_msg_check_for_validity() == TRUE */

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
      wms_put_cmd( cmd_ptr );
    }
  } /* end if wmsi_msg_check_for_validity() == TRUE */
  else
  {
    wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NO_RESOURCE );
    st = WMS_OUT_OF_RESOURCES_S;
    goto bail;
  }

bail:
  return st;
} /* wms_msg_write_template */

/* <EJECT> */
/*=========================================================================
FUNCTION
  wms_msg_delete_template

DESCRIPTION
  Allow the client to delete a template from a memory store.
  Only NV or SIM memory store is allowed.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_msg_delete_template
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_memory_store_e_type         mem_store,
  wms_message_index_type          index
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_backup.hdr.cmd_id      = WMS_CMD_MSG_DELETE_TEMPLATE;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;

  cmd_backup.cmd.msg_delete_template.mem_store   = mem_store;
  cmd_backup.cmd.msg_delete_template.index       = index;


  cmd_ptr = wms_get_cmd_buf();

  if( cmd_ptr != NULL)
  {
    * cmd_ptr = cmd_backup;
    wms_put_cmd( cmd_ptr );
  }
  else
  {
    wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NO_RESOURCE );
    st = WMS_OUT_OF_RESOURCES_S;
  }

  return st;
} /* wms_msg_delete_template */

/* <EJECT> */
/*=========================================================================
FUNCTION
  wms_msg_delete_template_all

DESCRIPTION
  Allow the client to delete all templates from a memory store.
  Only NV or SIM memory store is allowed.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_msg_delete_template_all
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_memory_store_e_type         mem_store
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_backup.hdr.cmd_id      = WMS_CMD_MSG_DELETE_TEMPLATE_ALL;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;

  cmd_backup.cmd.msg_delete_template_all.mem_store   = mem_store;


  cmd_ptr = wms_get_cmd_buf();

  if( cmd_ptr != NULL)
  {
    * cmd_ptr = cmd_backup;
    wms_put_cmd( cmd_ptr );
  }
  else
  {
    wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NO_RESOURCE );
    st = WMS_OUT_OF_RESOURCES_S;
  }

  return st;
} /* wms_msg_delete_template_all */

/* <EJECT> */
/*=========================================================================
FUNCTION
  wms_msg_read_sts_report

DESCRIPTION
  Allow the client to read a Status Report from a memory store.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_msg_read_sts_report
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_memory_store_e_type         mem_store,
  wms_message_index_type          index
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr = & cmd_backup;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_backup.hdr.cmd_id      = WMS_CMD_MSG_READ_STS_REPORT;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;


#ifdef FEATURE_GWSMS
#error code not present
#else /* FEATURE_GWSMS */
  wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_UNSUPPORTED );
  st = WMS_UNSUPPORTED_S;
#endif /* FEATURE_GWSMS */

  return st;
} /* wms_msg_read_sts_report */

/* <EJECT> */
/*=========================================================================
FUNCTION
  wms_msg_write_sts_report

DESCRIPTION
  Allow the client to write a status report to a memory store.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

COMMENTS
  The Status Report will only be stored if the corresponding SMS Message 
  is also stored in the same Memory. If the SMS Message is not found, 
  WMS_CMD_ERR_MSG_NO_MO_MSG will be returned in Command Callback.

=========================================================================*/
wms_status_e_type wms_msg_write_sts_report
(
  wms_client_id_type                 client_id,
  wms_cmd_cb_type                    cmd_cb,
  const void                       * user_data,
  wms_write_mode_e_type              write_mode,
  const wms_client_message_s_type  * message_ptr
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr = & cmd_backup;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_backup.hdr.cmd_id      = WMS_CMD_MSG_WRITE_STS_REPORT;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;


#ifdef FEATURE_GWSMS
#error code not present
#else /* FEATURE_GWSMS */
  wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_UNSUPPORTED );
  st = WMS_UNSUPPORTED_S;
#endif /* FEATURE_GWSMS */

  return st;
} /* wms_msg_write_sts_report */

/* <EJECT> */
/*=========================================================================
FUNCTION
  wms_msg_delete_sts_report

DESCRIPTION
  Allow the client to delete a status report from a memory store.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_msg_delete_sts_report
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_memory_store_e_type         mem_store,
  wms_message_index_type          index
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr = & cmd_backup;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_backup.hdr.cmd_id      = WMS_CMD_MSG_DELETE_STS_REPORT;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;

#ifdef FEATURE_GWSMS
#error code not present
#else /* FEATURE_GWSMS */
  wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_UNSUPPORTED );
  st = WMS_UNSUPPORTED_S;
#endif /* FEATURE_GWSMS */

  return st;
} /* wms_msg_delete_sts_report */

/* <EJECT> */
/*=========================================================================
FUNCTION
  wms_msg_delete_sts_report_all

DESCRIPTION
  Allow the client to delete all status reports from a memory store.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_msg_delete_sts_report_all
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_memory_store_e_type         mem_store
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr = & cmd_backup;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_backup.hdr.cmd_id      = WMS_CMD_MSG_DELETE_STS_REPORT_ALL;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;

#ifdef FEATURE_GWSMS
#error code not present
#else /* FEATURE_GWSMS */
  wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_UNSUPPORTED );
  st = WMS_UNSUPPORTED_S;
#endif /* FEATURE_GWSMS */

  return st;
} /* wms_msg_delete_sts_report_all */

/* <EJECT> */
/*=========================================================================
FUNCTION
  wms_reg_msg_info_cache_cb

DESCRIPTION
  Allow the client to register its callback function for telling WMS how to
  cache the message information for the messages from the memory.

  If a null pointer is passed, the callback is de-registered. No message
  information will be cahced.

  NOTE: Only one cache callback is allowed.

DEPENDENCIES
  None

RETURN VALUE
  client error codes

SIDE EFFECTS
  Internal data updated.

=========================================================================*/
void wms_reg_msg_info_cache_cb
(
  wms_msg_info_cache_cb_type    msg_info_cache_cb
)
{
  wms_msg_info_cache_cb = msg_info_cache_cb;

  return;

} /* wms_reg_msg_info_cache_cb() */

/* <EJECT> */
/*===========================================================================

                      Dedicated Channel (DC) Group

                        API FUNCTION DEFINITIONS

===========================================================================*/

/*=========================================================================
FUNCTION
  wms_dc_enable_auto_disconnect

DESCRIPTION
  This function enables the feature of auto-disconnecting the DC connection
  when there is no message sent or received during the specified time out
  period. The client will receive an event if the connection is disconnected.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.

=========================================================================*/
wms_status_e_type wms_dc_enable_auto_disconnect
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  uint32                          timeout    /* in seconds */
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr = & cmd_backup;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_backup.hdr.cmd_id      = WMS_CMD_DC_ENABLE_AUTO_DISCONNECT;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;


#ifdef FEATURE_CDSMS
  cmd_backup.cmd.dc_enable_auto_disc.timeout = timeout;

  cmd_ptr = wms_get_cmd_buf();

  if( cmd_ptr != NULL)
  {
    * cmd_ptr = cmd_backup;
    wms_put_cmd( cmd_ptr );
  }
  else
  {
    wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NO_RESOURCE );
    st = WMS_OUT_OF_RESOURCES_S;
  }

#else /* FEATURE_CDSMS */
  wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_UNSUPPORTED );
  st = WMS_UNSUPPORTED_S;
#endif /* FEATURE_CDSMS */

  return st;

} /* wms_dc_enable_auto_disconnect() */

/* <EJECT> */
/*=========================================================================
FUNCTION
  wms_dc_disable_auto_disconnect

DESCRIPTION
  This function disables the feature of auto-disconnecting the DC connection
  when there is no message sent or received during the specified time out
  period.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.

=========================================================================*/
wms_status_e_type wms_dc_disable_auto_disconnect
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr = & cmd_backup;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_backup.hdr.cmd_id      = WMS_CMD_DC_DISABLE_AUTO_DISCONNECT;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;

#ifdef FEATURE_CDSMS

  cmd_ptr = wms_get_cmd_buf();

  if( cmd_ptr != NULL)
  {
    * cmd_ptr = cmd_backup;
    wms_put_cmd( cmd_ptr );
  }
  else
  {
    wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NO_RESOURCE );
    st = WMS_OUT_OF_RESOURCES_S;
  }

#else /* FEATURE_CDSMS */
  wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_UNSUPPORTED );
  st = WMS_UNSUPPORTED_S;
#endif /* FEATURE_CDSMS */

  return st;

} /* wms_dc_disable_auto_disconnect() */

/* <EJECT> */
/*=========================================================================
FUNCTION
  wms_dc_connect

DESCRIPTION
  This function requests to establish a DC connection. It will queue
  the request and signal the SMS task to process it. Once the DC connection
  is established or fails, an event will be sent to the client.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.

=========================================================================*/
wms_status_e_type wms_dc_connect
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_dc_so_e_type                so
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr = & cmd_backup;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_backup.hdr.cmd_id      = WMS_CMD_DC_CONNECT;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;


#ifdef FEATURE_CDSMS
  cmd_backup.cmd.dc_connect.so  = so;


  cmd_ptr = wms_get_cmd_buf();

  if( cmd_ptr != NULL)
  {
    * cmd_ptr = cmd_backup;
    wms_put_cmd( cmd_ptr );
  }
  else
  {
    wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NO_RESOURCE );
    st = WMS_OUT_OF_RESOURCES_S;
  }

#else /* FEATURE_CDSMS */
  wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_UNSUPPORTED );
  st = WMS_UNSUPPORTED_S;
#endif /* FEATURE_CDSMS */

  return st;

} /* wms_dc_connect() */

/* <EJECT> */
/*=========================================================================
FUNCTION
  wms_dc_disconnect

DESCRIPTION
  This function is used to disconnect the DC connection when there is a
  request from the client or from the base station. It will send CDMA
  messages to the base station to disconnect the DC connection.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.

=========================================================================*/
wms_status_e_type wms_dc_disconnect
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr = & cmd_backup;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_backup.hdr.cmd_id      = WMS_CMD_DC_DISCONNECT;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;

#ifdef FEATURE_CDSMS

  cmd_ptr = wms_get_cmd_buf();

  if( cmd_ptr != NULL)
  {
    * cmd_ptr = cmd_backup;
    wms_put_cmd( cmd_ptr );
  }
  else
  {
    wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NO_RESOURCE );
    st = WMS_OUT_OF_RESOURCES_S;
  }

#else /* FEATURE_CDSMS */
  wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_UNSUPPORTED );
  st = WMS_UNSUPPORTED_S;
#endif /* FEATURE_CDSMS */

  return st;

} /* wms_dc_disconnect() */


#ifdef CUST_EDITION	
/* <EJECT> */
/*===========================================================================

                            Broadcast Group

                        API FUNCTION DEFINITIONS

===========================================================================*/


/*=========================================================================
FUNCTION
  wms_bc_get_config

DESCRIPTION
  This function is used to retrieve the configuration for broadcast SMS.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.

=========================================================================*/
wms_status_e_type wms_bc_get_config
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr = & cmd_backup;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_backup.hdr.cmd_id      = WMS_CMD_BC_GET_CONFIG;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;

#if (defined(FEATURE_BROADCAST_SMS) && !defined(FEATURE_CDSMS_BROADCAST))

  cmd_ptr = wms_get_cmd_buf();

  if( cmd_ptr != NULL)
  {
    * cmd_ptr = cmd_backup;
    wms_put_cmd( cmd_ptr );
  }
  else
  {
    wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NO_RESOURCE );
    st = WMS_OUT_OF_RESOURCES_S;
  }

#else /* FEATURE_BROADCAST_SMS */
  wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_UNSUPPORTED );
  st = WMS_UNSUPPORTED_S;
#endif /* FEATURE_BROADCAST_SMS */

  return st;

} /* wms_bc_get_config() */

/* <EJECT> */
/*=========================================================================
FUNCTION
  wms_bc_get_pref

DESCRIPTION
  This function is used to retrieve the user preference for broadcast SMS.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.

=========================================================================*/
wms_status_e_type wms_bc_get_pref
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr = & cmd_backup;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_backup.hdr.cmd_id      = WMS_CMD_BC_GET_PREF;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;

#if (defined(FEATURE_BROADCAST_SMS) && !defined(FEATURE_CDSMS_BROADCAST))

  cmd_ptr = wms_get_cmd_buf();

  if( cmd_ptr != NULL)
  {    
    * cmd_ptr = cmd_backup;    
    wms_put_cmd( cmd_ptr );
  }
  else
  {
    wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NO_RESOURCE );
    st = WMS_OUT_OF_RESOURCES_S;
  }


#else /* FEATURE_BROADCAST_SMS */
  wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_UNSUPPORTED );
  st = WMS_UNSUPPORTED_S;
#endif /* FEATURE_BROADCAST_SMS */

  return st;

} /* wms_bc_get_pref() */

/* <EJECT> */
/*=========================================================================
FUNCTION
  wms_bc_set_pref

DESCRIPTION
  This function is used to set the user preference for broadcast SMS.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.

=========================================================================*/
wms_status_e_type wms_bc_set_pref
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_bc_pref_e_type              pref
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr = & cmd_backup;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_backup.hdr.cmd_id      = WMS_CMD_BC_SET_PREF;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;

#if (defined(FEATURE_BROADCAST_SMS) && !defined(FEATURE_CDSMS_BROADCAST))
  cmd_backup.cmd.bc_set_pref.bc_pref  = pref;


  cmd_ptr = wms_get_cmd_buf();

  if( cmd_ptr != NULL)
  {  
    * cmd_ptr = cmd_backup; 
    wms_put_cmd( cmd_ptr );
  }
  else
  {
    wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NO_RESOURCE );
    st = WMS_OUT_OF_RESOURCES_S;
  }

#else /* FEATURE_BROADCAST_SMS */
  wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_UNSUPPORTED );
  st = WMS_UNSUPPORTED_S;
#endif /* FEATURE_BROADCAST_SMS */


  return st;

} /* wms_bc_set_pref() */

/* <EJECT> */
/*=========================================================================
FUNCTION
  wms_bc_get_table

DESCRIPTION
  This function is used to retrieve the service table for broadcast SMS.

  NOTE: To prevent retrieving a large service table, the function
  wms_bc_get_all_service_ids() can be called to retrieve all service
  IDs, where each ID is much smaller than a complete table entry, and then
  the function wms_bc_get_service_info() can be called for each service ID
  in order to retrieve the table entries one by one.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.

=========================================================================*/
wms_status_e_type wms_bc_get_table
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr = & cmd_backup;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_backup.hdr.cmd_id      = WMS_CMD_BC_GET_TABLE;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;

#if (defined(FEATURE_BROADCAST_SMS) && !defined(FEATURE_CDSMS_BROADCAST))

  cmd_ptr = wms_get_cmd_buf();

  if( cmd_ptr != NULL)
  {    
    * cmd_ptr = cmd_backup;   
    wms_put_cmd( cmd_ptr );
  }
  else
  {
    wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NO_RESOURCE );
    st = WMS_OUT_OF_RESOURCES_S;
  }

#else /* FEATURE_BROADCAST_SMS */
  wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_UNSUPPORTED );
  st = WMS_UNSUPPORTED_S;
#endif /* FEATURE_BROADCAST_SMS */


  return st;

} /* wms_bc_get_table() */

/* <EJECT> */
/*=========================================================================
FUNCTION
  wms_bc_select_service

DESCRIPTION
  This function is used to select a broadcast SMS service

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.

=========================================================================*/
wms_status_e_type wms_bc_select_service
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  const wms_bc_service_id_s_type  *srv_id,
  boolean                   selected,
  wms_priority_e_type       priority
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr = & cmd_backup;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_backup.hdr.cmd_id      = WMS_CMD_BC_SELECT_SRV;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;

#if (defined(FEATURE_BROADCAST_SMS) && !defined(FEATURE_CDSMS_BROADCAST))
  cmd_backup.cmd.bc_select_srv.srv_id   = *srv_id;
  cmd_backup.cmd.bc_select_srv.selected = selected;
  cmd_backup.cmd.bc_select_srv.priority = priority;


  cmd_ptr = wms_get_cmd_buf();

  if( cmd_ptr != NULL)
  {   
    * cmd_ptr = cmd_backup;   
    wms_put_cmd( cmd_ptr );
  }
  else
  {
    wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NO_RESOURCE );
    st = WMS_OUT_OF_RESOURCES_S;
  }

#else /* FEATURE_BROADCAST_SMS */
  wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_UNSUPPORTED );
  st = WMS_UNSUPPORTED_S;
#endif /* FEATURE_BROADCAST_SMS */


  return st;

} /* wms_bc_select_service() */

/* <EJECT> */
/*=========================================================================
FUNCTION
  wms_bc_get_all_service_ids

DESCRIPTION
  This function is used to retrieve all the service IDs in the broadcast
  SMS service table. After the retrieval of the service IDs, the function
  wms_bc_get_service_info() can be called to retrieve the complete
  table entries one by one.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.

=========================================================================*/
wms_status_e_type wms_bc_get_all_service_ids
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr = & cmd_backup;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_backup.hdr.cmd_id      = WMS_CMD_BC_GET_ALL_SRV_IDS;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;

#if (defined(FEATURE_BROADCAST_SMS) && !defined(FEATURE_CDSMS_BROADCAST))

  cmd_ptr = wms_get_cmd_buf();

  if( cmd_ptr != NULL)
  {  
    * cmd_ptr = cmd_backup;  
    wms_put_cmd( cmd_ptr );
  }
  else
  {
    wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NO_RESOURCE );
    st = WMS_OUT_OF_RESOURCES_S;
  }

#else /* FEATURE_BROADCAST_SMS */
  wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_UNSUPPORTED );
  st = WMS_UNSUPPORTED_S;
#endif /* FEATURE_BROADCAST_SMS */


  return st;

} /* wms_bc_get_all_service_ids() */

/* <EJECT> */
/*=========================================================================
FUNCTION
  wms_bc_get_service_info

DESCRIPTION
  This function is used to retrieve a table entry in the broadcast SMS
  service table using a service ID.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.

=========================================================================*/
wms_status_e_type wms_bc_get_service_info
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  const wms_bc_service_id_s_type  *srv_id
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr = & cmd_backup;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_backup.hdr.cmd_id      = WMS_CMD_BC_GET_SRV_INFO;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;

#if (defined(FEATURE_BROADCAST_SMS) && !defined(FEATURE_CDSMS_BROADCAST))

  cmd_backup.cmd.bc_get_srv_info.srv_id = *srv_id;

  cmd_ptr = wms_get_cmd_buf();

  if( cmd_ptr != NULL)
  {    
    * cmd_ptr = cmd_backup;  
    wms_put_cmd( cmd_ptr );
  }
  else
  {
    wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NO_RESOURCE );
    st = WMS_OUT_OF_RESOURCES_S;
  }

#else /* FEATURE_BROADCAST_SMS */
  wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_UNSUPPORTED );
  st = WMS_UNSUPPORTED_S;
#endif /* FEATURE_BROADCAST_SMS */


  return st;

} /* wms_bc_get_service_info() */

/* <EJECT> */
/*=========================================================================
FUNCTION
  wms_bc_add_service

DESCRIPTION
  This function is used to add an entry to the broadcast SMS service table.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.

=========================================================================*/
wms_status_e_type wms_bc_add_service
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  const wms_bc_service_info_s_type      *srv_info
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr = & cmd_backup;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_backup.hdr.cmd_id      = WMS_CMD_BC_ADD_SRV;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;

#if (defined(FEATURE_BROADCAST_SMS) && !defined(FEATURE_CDSMS_BROADCAST))

  cmd_backup.cmd.bc_add_srv.srv_info = *srv_info;


  cmd_ptr = wms_get_cmd_buf();

  if( cmd_ptr != NULL)
  {    
    * cmd_ptr = cmd_backup; 
    wms_put_cmd( cmd_ptr );
  }
  else
  {
    wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NO_RESOURCE );
    st = WMS_OUT_OF_RESOURCES_S;
  }

#else /* FEATURE_BROADCAST_SMS */
  wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_UNSUPPORTED );
  st = WMS_UNSUPPORTED_S;
#endif /* FEATURE_BROADCAST_SMS */


  return st;

} /* wms_bc_add_serivice() */

/* <EJECT> */
/*=========================================================================
FUNCTION
  wms_bc_delete_service

DESCRIPTION
  This function is used to delete an entry to the broadcast SMS service table.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.

=========================================================================*/
wms_status_e_type wms_bc_delete_service
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  const wms_bc_service_id_s_type        *srv_id
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr = & cmd_backup;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_backup.hdr.cmd_id      = WMS_CMD_BC_DELETE_SRV;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;

#if (defined(FEATURE_BROADCAST_SMS) && !defined(FEATURE_CDSMS_BROADCAST))

  cmd_backup.cmd.bc_delete_srv.srv_id  = *srv_id;


  cmd_ptr = wms_get_cmd_buf();

  if( cmd_ptr != NULL)
  {
    * cmd_ptr = cmd_backup;  
    wms_put_cmd( cmd_ptr );
  }
  else
  {
    wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NO_RESOURCE );
    st = WMS_OUT_OF_RESOURCES_S;
  }

#else /* FEATURE_BROADCAST_SMS */
  wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_UNSUPPORTED );
  st = WMS_UNSUPPORTED_S;
#endif /* FEATURE_BROADCAST_SMS */


  return st;

} /* wms_bc_delete_service() */

/* <EJECT> */
/*=========================================================================
FUNCTION
  wms_bc_change_label

DESCRIPTION
  This function is used to change an entry in the broadcast SMS service table.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.

=========================================================================*/
wms_status_e_type wms_bc_change_label
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  const wms_bc_service_id_s_type        *srv_id,
  const char                            *label_ptr
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr = & cmd_backup;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_backup.hdr.cmd_id      = WMS_CMD_BC_CHANGE_LABEL;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;


#if (defined(FEATURE_BROADCAST_SMS) && !defined(FEATURE_CDSMS_BROADCAST))

  if(srv_id == NULL || label_ptr == NULL)
  {
    MSG_ERROR("Null Ptr: wms_bc_change_label",0,0,0);
    wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NULL_PTR );
    st = WMS_NULL_PTR_S;
  }
  else
  {
    cmd_backup.cmd.bc_change_label.srv_id = *srv_id;
    memcpy( cmd_backup.cmd.bc_change_label.label,
            label_ptr,
            WMS_BC_SRV_LABEL_SIZE );

    cmd_ptr = wms_get_cmd_buf();

    if( cmd_ptr != NULL)
    {   
      * cmd_ptr = cmd_backup;
      wms_put_cmd( cmd_ptr );
    }
    else
    {
      wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NO_RESOURCE );
      st = WMS_OUT_OF_RESOURCES_S;
    }
  }

#else /* FEATURE_BROADCAST_SMS */
  wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_UNSUPPORTED );
  st = WMS_UNSUPPORTED_S;
#endif /* FEATURE_BROADCAST_SMS */

  return st;

} /* wms_bc_change_label() */

/* <EJECT> */
/*=========================================================================
FUNCTION
  wms_bc_delete_all_services

DESCRIPTION
  This function is used to delete all entries from the broadcast service table.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.

=========================================================================*/
wms_status_e_type wms_bc_delete_all_services
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr = & cmd_backup;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_backup.hdr.cmd_id      = WMS_CMD_BC_DELETE_ALL_SERVICES;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;

  /* no command data needed */

#if (defined(FEATURE_BROADCAST_SMS) && !defined(FEATURE_CDSMS_BROADCAST))

  cmd_ptr = wms_get_cmd_buf();

  if( cmd_ptr != NULL)
  {  
    * cmd_ptr = cmd_backup;  
    wms_put_cmd( cmd_ptr );
  }
  else
  {
    wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NO_RESOURCE );
    st = WMS_OUT_OF_RESOURCES_S;
  }

#else /* FEATURE_BROADCAST_SMS */
  wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_UNSUPPORTED );
  st = WMS_UNSUPPORTED_S;
#endif /* FEATURE_BROADCAST_SMS */


  return st;

} /* wms_bc_delete_all_services() */

/* <EJECT> */
/*=========================================================================
FUNCTION
  wms_bc_set_priority_for_all

DESCRIPTION
  This function is used to change priority levels for all services in the
  broadcast service table.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.

=========================================================================*/
wms_status_e_type wms_bc_set_priority_for_all
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_priority_e_type             priority
)

{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr = & cmd_backup;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_backup.hdr.cmd_id      = WMS_CMD_BC_SET_PRIORITY_FOR_ALL;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;

#if (defined(FEATURE_BROADCAST_SMS) && !defined(FEATURE_CDSMS_BROADCAST))

  cmd_backup.cmd.bc_set_priority_for_all.priority = priority;

  cmd_ptr = wms_get_cmd_buf();

  if( cmd_ptr != NULL)
  {   
    * cmd_ptr = cmd_backup;  
    wms_put_cmd( cmd_ptr );
  }
  else
  {
    wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NO_RESOURCE );
    st = WMS_OUT_OF_RESOURCES_S;
  }

#else /* FEATURE_BROADCAST_SMS */
  wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_UNSUPPORTED );
  st = WMS_UNSUPPORTED_S;
#endif /* FEATURE_BROADCAST_SMS */


  return st;

} /* wms_bc_set_priority_for_all() */
#endif /*CUST_EDITION*/

/* <EJECT> */
/*===========================================================================

                       Multimode Broadcast Group

                        API FUNCTION DEFINITIONS

===========================================================================*/

/*=========================================================================
FUNCTION
  wms_bc_mm_get_config

DESCRIPTION
  This function is used to retrieve the configuration for broadcast SMS.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.

=========================================================================*/
wms_status_e_type wms_bc_mm_get_config
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_message_mode_e_type         message_mode
)
{
#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  boolean is_command_allowed = FALSE;
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr = & cmd_backup;

  cmd_backup.hdr.cmd_id      = WMS_CMD_BC_MM_GET_CONFIG;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;

  if(message_mode == WMS_MESSAGE_MODE_GW)
  {
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */
  }
  else if(message_mode == WMS_MESSAGE_MODE_CDMA)
  {
#ifdef FEATURE_CDSMS_BROADCAST
    is_command_allowed = TRUE;
#endif /* FEATURE_CDSMS_BROADCAST */
  }

#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  if (is_command_allowed)
  {
    cmd_backup.cmd.bc_mm_get_config.message_mode = message_mode;

    cmd_ptr = wms_get_cmd_buf();

    if( cmd_ptr != NULL)
    {
      * cmd_ptr = cmd_backup;
      wms_put_cmd( cmd_ptr );
    }
    else
    {
      wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NO_RESOURCE );
      st = WMS_OUT_OF_RESOURCES_S;
    }
  }
  else
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
  {
    wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_UNSUPPORTED );
    st = WMS_UNSUPPORTED_S;
  }
  return st;
} /* wms_bc_mm_get_config() */

/* <EJECT> */
/*=========================================================================
FUNCTION
  wms_bc_mm_get_pref

DESCRIPTION
  This function is used to retrieve the user preference for broadcast SMS.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.
=========================================================================*/
wms_status_e_type wms_bc_mm_get_pref
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_message_mode_e_type         message_mode
)
{
#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  boolean is_command_allowed = FALSE;
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr = & cmd_backup;

  cmd_backup.hdr.cmd_id      = WMS_CMD_BC_MM_GET_PREF;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;

  if(message_mode == WMS_MESSAGE_MODE_GW)
  {
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */
  }
  else if(message_mode == WMS_MESSAGE_MODE_CDMA)
  {
#ifdef FEATURE_CDSMS_BROADCAST
    is_command_allowed = TRUE;
#endif /* FEATURE_CDSMS_BROADCAST */
  }

#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  if (is_command_allowed)
  {
    cmd_backup.cmd.bc_mm_get_pref.message_mode = message_mode;

    cmd_ptr = wms_get_cmd_buf();

    if( cmd_ptr != NULL)
    {
      * cmd_ptr = cmd_backup;
      wms_put_cmd( cmd_ptr );
    }
    else
    {
      wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NO_RESOURCE );
      st = WMS_OUT_OF_RESOURCES_S;
    }
  }
  else
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
  {
    wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_UNSUPPORTED );
    st = WMS_UNSUPPORTED_S;
  }
  return st;
} /* wms_bc_mm_get_pref() */

/* <EJECT> */
/*=========================================================================
FUNCTION
  wms_bc_mm_set_pref

DESCRIPTION
  This function is used to set the user preference for broadcast SMS.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.
=========================================================================*/
wms_status_e_type wms_bc_mm_set_pref
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_message_mode_e_type         message_mode,
  wms_bc_pref_e_type              pref
)
{
#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  boolean is_command_allowed = FALSE;
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr = & cmd_backup;

  cmd_backup.hdr.cmd_id      = WMS_CMD_BC_MM_SET_PREF;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;

  if(message_mode == WMS_MESSAGE_MODE_GW)
  {
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */
  }
  else if(message_mode == WMS_MESSAGE_MODE_CDMA)
  {
#ifdef FEATURE_CDSMS_BROADCAST
    is_command_allowed = TRUE;
#endif /* FEATURE_CDSMS_BROADCAST */
  }

#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  if (is_command_allowed)
  {
    cmd_backup.cmd.bc_mm_set_pref.message_mode = message_mode;
    cmd_backup.cmd.bc_mm_set_pref.bc_pref      = pref;

    cmd_ptr = wms_get_cmd_buf();

    if( cmd_ptr != NULL)
    {
      * cmd_ptr = cmd_backup;
      wms_put_cmd( cmd_ptr );
    }
    else
    {
      wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NO_RESOURCE );
      st = WMS_OUT_OF_RESOURCES_S;
    }
  }
  else
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
  {
    wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_UNSUPPORTED );
    st = WMS_UNSUPPORTED_S;
  }
  return st;
} /* wms_bc_mm_set_pref() */

/* <EJECT> */
/*=========================================================================
FUNCTION
  wms_bc_mm_get_table

DESCRIPTION
  This function is used to retrieve the service table for broadcast SMS.

  NOTE: To prevent retrieving a large service table, the function
  wms_bc_get_all_service_ids() can be called to retrieve all service
  IDs, where each ID is much smaller than a complete table entry, and then
  the function wms_bc_get_service_info() can be called for each service ID
  in order to retrieve the table entries one by one.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.
=========================================================================*/
wms_status_e_type wms_bc_mm_get_table
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_message_mode_e_type         message_mode
)
{
#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  boolean is_command_allowed = FALSE;
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr = & cmd_backup;

  cmd_backup.hdr.cmd_id      = WMS_CMD_BC_MM_GET_TABLE;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;

  if(message_mode == WMS_MESSAGE_MODE_GW)
  {
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */
  }
  else if(message_mode == WMS_MESSAGE_MODE_CDMA)
  {
#ifdef FEATURE_CDSMS_BROADCAST
    is_command_allowed = TRUE;
#endif /* FEATURE_CDSMS_BROADCAST */
  }

#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  if(is_command_allowed)
  {
    cmd_backup.cmd.bc_mm_get_table.message_mode = message_mode;

    cmd_ptr = wms_get_cmd_buf();

    if( cmd_ptr != NULL)
    {
      * cmd_ptr = cmd_backup;
      wms_put_cmd( cmd_ptr );
    }
    else
    {
      wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NO_RESOURCE );
      st = WMS_OUT_OF_RESOURCES_S;
    }
  }
  else
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
  {
    wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_UNSUPPORTED );
    st = WMS_UNSUPPORTED_S;
  }
  return st;
} /* wms_bc_mm_get_table() */

/* <EJECT> */
/*=========================================================================
FUNCTION
  wms_bc_mm_select_service

DESCRIPTION
  This function is used to select a broadcast SMS service

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.
=========================================================================*/
wms_status_e_type wms_bc_mm_select_service
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_message_mode_e_type         message_mode,
  const wms_bc_mm_srv_id_type           *srv_id_ptr,
  boolean                         selected
)
{
#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  boolean is_command_allowed = FALSE;
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr = & cmd_backup;

  cmd_backup.hdr.cmd_id      = WMS_CMD_BC_MM_SELECT_SRV;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;

  if(message_mode == WMS_MESSAGE_MODE_GW)
  {
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */
  }
  else if(message_mode == WMS_MESSAGE_MODE_CDMA)
  {
#ifdef FEATURE_CDSMS_BROADCAST
    is_command_allowed = TRUE;
#endif /* FEATURE_CDSMS_BROADCAST */
  }

#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  if(is_command_allowed)
  {
    cmd_backup.cmd.bc_mm_select_srv.message_mode = message_mode;
    cmd_backup.cmd.bc_mm_select_srv.srv_id   = * srv_id_ptr;
    cmd_backup.cmd.bc_mm_select_srv.selected = selected;

    cmd_ptr = wms_get_cmd_buf();

    if( cmd_ptr != NULL)
    {
      * cmd_ptr = cmd_backup;
      wms_put_cmd( cmd_ptr );
    }
    else
    {
      wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NO_RESOURCE );
      st = WMS_OUT_OF_RESOURCES_S;
    }
  }
  else
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
  {
    wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_UNSUPPORTED );
    st = WMS_UNSUPPORTED_S;
  }
  return st;
} /* wms_bc_mm_select_service() */

/* <EJECT> */
/*=========================================================================
FUNCTION
  wms_bc_mm_get_all_service_ids

DESCRIPTION
  This function is used to retrieve all the service IDs in the broadcast
  SMS service table. After the retrieval of the service IDs, the function
  wms_bc_get_service_info() can be called to retrieve the complete
  table entries one by one.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.
=========================================================================*/
wms_status_e_type wms_bc_mm_get_all_service_ids
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_message_mode_e_type         message_mode
)
{
#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  boolean is_command_allowed = FALSE;
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr = & cmd_backup;

  cmd_backup.hdr.cmd_id      = WMS_CMD_BC_MM_GET_ALL_SRV_IDS;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;

  if(message_mode == WMS_MESSAGE_MODE_GW)
  {
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */
  }
  else if(message_mode == WMS_MESSAGE_MODE_CDMA)
  {
#ifdef FEATURE_CDSMS_BROADCAST
    is_command_allowed = TRUE;
#endif /* FEATURE_CDSMS_BROADCAST */
  }

#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  if(is_command_allowed)
  {
    cmd_backup.cmd.bc_mm_get_all_srv_ids.message_mode = message_mode;

    cmd_ptr = wms_get_cmd_buf();

    if( cmd_ptr != NULL)
    {
      * cmd_ptr = cmd_backup;
      wms_put_cmd( cmd_ptr );
    }
    else
    {
      wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NO_RESOURCE );
      st = WMS_OUT_OF_RESOURCES_S;
    }
  }
  else
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
  {
    wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_UNSUPPORTED );
    st = WMS_UNSUPPORTED_S;
  }
  return st;
} /* wms_bc_mm_get_all_service_ids() */

/* <EJECT> */
/*=========================================================================
FUNCTION
  wms_bc_mm_get_service_info

DESCRIPTION
  This function is used to retrieve a table entry in the broadcast SMS
  service table using a service ID.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.
=========================================================================*/
wms_status_e_type wms_bc_mm_get_service_info
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_message_mode_e_type         message_mode,
 const wms_bc_mm_srv_id_type           *srv_id_ptr
)
{
#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  boolean is_command_allowed = FALSE;
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr = & cmd_backup;

  cmd_backup.hdr.cmd_id      = WMS_CMD_BC_MM_GET_SRV_INFO;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;

  if(message_mode == WMS_MESSAGE_MODE_GW)
  {
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */
  }
  else if(message_mode == WMS_MESSAGE_MODE_CDMA)
  {
#ifdef FEATURE_CDSMS_BROADCAST
    is_command_allowed = TRUE;
#endif /* FEATURE_CDSMS_BROADCAST */
  }

#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  if(is_command_allowed)
  {
    cmd_backup.cmd.bc_mm_get_srv_info.message_mode = message_mode;
    cmd_backup.cmd.bc_mm_get_srv_info.srv_id = * srv_id_ptr;

    cmd_ptr = wms_get_cmd_buf();

    if( cmd_ptr != NULL)
    {
      * cmd_ptr = cmd_backup;
      wms_put_cmd( cmd_ptr );
    }
    else
    {
      wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NO_RESOURCE );
      st = WMS_OUT_OF_RESOURCES_S;
    }
  }
  else
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
  {
    wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_UNSUPPORTED );
    st = WMS_UNSUPPORTED_S;
  }
  return st;
} /* wms_bc_mm_get_service_info() */

/* <EJECT> */
/*=========================================================================
FUNCTION
  wms_bc_mm_add_services

DESCRIPTION
  This function is used to add entries to the broadcast SMS service table.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.
=========================================================================*/
wms_status_e_type wms_bc_mm_add_services
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_message_mode_e_type         message_mode,
  uint8                           num_entries,
  const wms_bc_mm_service_info_s_type   * entries
)
{
#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  boolean is_command_allowed = FALSE;
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr = & cmd_backup;

  cmd_backup.hdr.cmd_id      = WMS_CMD_BC_MM_ADD_SRV;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;


  if(entries == NULL || user_data == NULL)
  {
    MSG_ERROR("Null Ptr: wms_bc_mm_add_services",0,0,0);
    st = WMS_NULL_PTR_S;
  }


  if(message_mode == WMS_MESSAGE_MODE_GW)
  {
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */
  }
  else if(message_mode == WMS_MESSAGE_MODE_CDMA)
  {
#ifdef FEATURE_CDSMS_BROADCAST
    is_command_allowed = TRUE;
#endif /* FEATURE_CDSMS_BROADCAST */
  }

#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  if(is_command_allowed)
  {
    cmd_backup.cmd.bc_mm_add_srv.message_mode = message_mode;
    cmd_backup.cmd.bc_mm_add_srv.num_entries = num_entries;
    cmd_backup.cmd.bc_mm_add_srv.entries =
            mem_malloc( &tmc_heap,
                        num_entries * sizeof(wms_bc_mm_service_info_s_type) );

    if( cmd_backup.cmd.bc_mm_add_srv.entries == NULL )
    {
      MSG_ERROR("Null Ptr: wms_bc_mm_add_services",0,0,0);
      st = WMS_NULL_PTR_S;
    }

    if( cmd_backup.cmd.bc_mm_add_srv.entries == NULL )
    {
      wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NO_RESOURCE );
      st = WMS_OUT_OF_RESOURCES_S;
    }
    else
    {
      memcpy( cmd_backup.cmd.bc_mm_add_srv.entries,
              entries,
              num_entries * sizeof(wms_bc_mm_service_info_s_type) );

      cmd_ptr = wms_get_cmd_buf();

      if( cmd_ptr != NULL)
      {
        * cmd_ptr = cmd_backup;
        wms_put_cmd( cmd_ptr );
      }
      else
      {
        mem_free( &tmc_heap, cmd_backup.cmd.bc_mm_add_srv.entries );
        wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NO_RESOURCE );
        st = WMS_OUT_OF_RESOURCES_S;
      }
    }
  }
  else
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
  {
    wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_UNSUPPORTED );
    st = WMS_UNSUPPORTED_S;
  }
  return st;
} /* wms_bc_mm_add_serivices() */

/* <EJECT> */
/*=========================================================================
FUNCTION
  wms_bc_mm_delete_services

DESCRIPTION
  This function is used to delete entries from the broadcast SMS service table.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.
=========================================================================*/
wms_status_e_type wms_bc_mm_delete_services
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_message_mode_e_type         message_mode,
  uint8                           num_entries,
  const wms_bc_mm_srv_id_type           * srv_ids
)
{
#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  boolean is_command_allowed = FALSE;
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr = & cmd_backup;

  cmd_backup.hdr.cmd_id      = WMS_CMD_BC_MM_DELETE_SRV;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;

  if(message_mode == WMS_MESSAGE_MODE_GW)
  {
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */
  }
  else if(message_mode == WMS_MESSAGE_MODE_CDMA)
  {
#ifdef FEATURE_CDSMS_BROADCAST
    is_command_allowed = TRUE;
#endif /* FEATURE_CDSMS_BROADCAST */
  }

#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  if(is_command_allowed)
  {
    cmd_backup.cmd.bc_mm_delete_srv.message_mode = message_mode;
    cmd_backup.cmd.bc_mm_delete_srv.num_entries = num_entries;
    cmd_backup.cmd.bc_mm_delete_srv.srv_ids =
            mem_malloc( &tmc_heap,
                        num_entries * sizeof(wms_bc_mm_srv_id_type) );

    if( cmd_backup.cmd.bc_mm_delete_srv.srv_ids == NULL )
    {
      wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NO_RESOURCE );
      st = WMS_OUT_OF_RESOURCES_S;
    }
    else
    {
      memcpy( cmd_backup.cmd.bc_mm_delete_srv.srv_ids,
              srv_ids,
              num_entries * sizeof(wms_bc_mm_srv_id_type) );

      cmd_ptr = wms_get_cmd_buf();

      if( cmd_ptr != NULL)
      {
        * cmd_ptr = cmd_backup;
        wms_put_cmd( cmd_ptr );
      }
      else
      {
        mem_free( &tmc_heap, cmd_backup.cmd.bc_mm_delete_srv.srv_ids );
        wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NO_RESOURCE );
        st = WMS_OUT_OF_RESOURCES_S;
      }
    }
  }
  else
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
  {
    wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_UNSUPPORTED );
    st = WMS_UNSUPPORTED_S;
  }
  return st;
} /* wms_bc_mm_delete_services() */

/* <EJECT> */
/*=========================================================================
FUNCTION
  wms_bc_mm_change_service_info

DESCRIPTION
  This function is used to change an entry in the broadcast SMS service table.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.
=========================================================================*/
wms_status_e_type wms_bc_mm_change_service_info
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_message_mode_e_type         message_mode,
  const wms_bc_mm_service_info_s_type   *srv_info_ptr
)
{
#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  boolean is_command_allowed = FALSE;
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr = & cmd_backup;

  cmd_backup.hdr.cmd_id      = WMS_CMD_BC_MM_CHANGE_SRV_INFO;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;

  if(message_mode == WMS_MESSAGE_MODE_GW)
  {
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */
  }
  else if(message_mode == WMS_MESSAGE_MODE_CDMA)
  {
#ifdef FEATURE_CDSMS_BROADCAST
    is_command_allowed = TRUE;
#endif /* FEATURE_CDSMS_BROADCAST */
  }

#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  if(is_command_allowed)
  {
    cmd_backup.cmd.bc_mm_change_srv_info.message_mode = message_mode;
    cmd_backup.cmd.bc_mm_change_srv_info.srv_info = *srv_info_ptr;

    cmd_ptr = wms_get_cmd_buf();

    if( cmd_ptr != NULL)
    {
      * cmd_ptr = cmd_backup;
      wms_put_cmd( cmd_ptr );
    }
    else
    {
      wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NO_RESOURCE );
      st = WMS_OUT_OF_RESOURCES_S;
    }
  }
  else
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
  {
    wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_UNSUPPORTED );
    st = WMS_UNSUPPORTED_S;
  }
  return st;
} /* wms_bc_mm_change_service_info() */

/* <EJECT> */
/*=========================================================================
FUNCTION
  wms_bc_mm_delete_all_services

DESCRIPTION
  This function is used to delete all entries from the broadcast service table.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.
=========================================================================*/
wms_status_e_type wms_bc_mm_delete_all_services
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_message_mode_e_type         message_mode
)
{
#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  boolean is_command_allowed = FALSE;
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr = & cmd_backup;

  cmd_backup.hdr.cmd_id      = WMS_CMD_BC_MM_DELETE_ALL_SERVICES;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;

  if(message_mode == WMS_MESSAGE_MODE_GW)
  {
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */
  }
  else if(message_mode == WMS_MESSAGE_MODE_CDMA)
  {
#ifdef FEATURE_CDSMS_BROADCAST
    is_command_allowed = TRUE;
#endif /* FEATURE_CDSMS_BROADCAST */
  }

#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  if(is_command_allowed)
  {
    cmd_backup.cmd.bc_mm_delete_all_srvs.message_mode = message_mode;

    cmd_ptr = wms_get_cmd_buf();

    if( cmd_ptr != NULL)
    {
      * cmd_ptr = cmd_backup;
      wms_put_cmd( cmd_ptr );
    }
    else
    {
      wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NO_RESOURCE );
      st = WMS_OUT_OF_RESOURCES_S;
    }
  }
  else
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
  {
    wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_UNSUPPORTED );
    st = WMS_UNSUPPORTED_S;
  }
  return st;
} /* wms_bc_mm_delete_all_services() */

/* <EJECT> */
/*=========================================================================
FUNCTION
  wms_bc_mm_select_all_services

DESCRIPTION
  This function is used to select/deselect all entries from the
  broadcast service table.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.
=========================================================================*/
wms_status_e_type wms_bc_mm_select_all_services
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_message_mode_e_type         message_mode,
  boolean                         selected
)
{
#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  boolean is_command_allowed = FALSE;
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr = & cmd_backup;

  cmd_backup.hdr.cmd_id      = WMS_CMD_BC_MM_SELECT_ALL_SERVICES;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;

  if(message_mode == WMS_MESSAGE_MODE_GW)
  {
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */
  }
  else if(message_mode == WMS_MESSAGE_MODE_CDMA)
  {
#ifdef FEATURE_CDSMS_BROADCAST
    is_command_allowed = TRUE;
#endif /* FEATURE_CDSMS_BROADCAST */
  }

#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  if(is_command_allowed)
  {
    cmd_backup.cmd.bc_mm_select_all_srvs.message_mode = message_mode;
    cmd_backup.cmd.bc_mm_select_all_srvs.selected     = selected;

    cmd_ptr = wms_get_cmd_buf();

    if( cmd_ptr != NULL)
    {
      * cmd_ptr = cmd_backup;
      wms_put_cmd( cmd_ptr );
    }
    else
    {
      wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NO_RESOURCE );
      st = WMS_OUT_OF_RESOURCES_S;
    }
  }
  else
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
  {
    wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_UNSUPPORTED );
    st = WMS_UNSUPPORTED_S;
  }
  return st;
} /* wms_bc_mm_select_all_services() */

/* <EJECT> */
/*=========================================================================
FUNCTION
  wms_bc_mm_set_priority_for_all_services

DESCRIPTION
  This function is used to set the priorities for all entries from the
  broadcast service table. (Specific to CDMA, Does not apply to GW)

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.
=========================================================================*/
wms_status_e_type wms_bc_mm_set_priority_for_all_services
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_message_mode_e_type         message_mode,
  wms_priority_e_type             priority
)
{
#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  boolean is_command_allowed = FALSE;
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr = & cmd_backup;

  cmd_backup.hdr.cmd_id      = WMS_CMD_BC_MM_SET_PRIORITY_ALL_SERVICES;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;

  if(message_mode == WMS_MESSAGE_MODE_CDMA)
  {
#ifdef FEATURE_CDSMS_BROADCAST
    is_command_allowed = TRUE;
#endif /* FEATURE_CDSMS_BROADCAST */
  }

#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  if(is_command_allowed)
  {
    cmd_backup.cmd.bc_mm_select_all_srvs.message_mode = message_mode;
    cmd_backup.cmd.bc_mm_set_priority_all_srvs.priority = priority;

    cmd_ptr = wms_get_cmd_buf();

    if( cmd_ptr != NULL)
    {
      * cmd_ptr = cmd_backup;
      wms_put_cmd( cmd_ptr );
    }
    else
    {
      wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NO_RESOURCE );
      st = WMS_OUT_OF_RESOURCES_S;
    }
  }
  else
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
  {
    wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_UNSUPPORTED );
    st = WMS_UNSUPPORTED_S;
  }
  return st;
} /* wms_bc_mm_set_priority_all_services() */

/*=========================================================================
FUNCTION
  wms_bc_mm_msg_delete_indication

DESCRIPTION
  This function is used to notify the lower layer of Message Deletion.
  This is applicable to GW mode only and is used by the UI to indicate
  the deletion of a GW CB page.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.
=========================================================================*/
wms_status_e_type wms_bc_mm_msg_delete_indication
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                     *user_data,
  wms_gw_cb_page_header_s_type   *cb_page_header
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr = & cmd_backup;

  cmd_backup.hdr.cmd_id      = WMS_CMD_BC_MM_MSG_DELETE_INDICATION;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;

#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#else
  {
    wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_UNSUPPORTED );
    st = WMS_UNSUPPORTED_S;
  }
#endif /* FEATURE_GWSMS_BROADCAST */

  return st;
} /* wms_bc_mm_msg_delete_indication() */

/* <EJECT> */
/*=========================================================================
FUNCTION
  wms_bc_mm_msg_delete_all_indication

DESCRIPTION
  This function is used to notify the lower layer of Message Deletion.
  This is applicable to GW mode only and is used by the UI to indicate
  the deletion of ALL GW CB page.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.
=========================================================================*/
wms_status_e_type wms_bc_mm_msg_delete_all_indication
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                     *user_data
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr = & cmd_backup;

  cmd_backup.hdr.cmd_id      = WMS_CMD_BC_MM_MSG_DELETE_ALL_INDICATION;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;

#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#else
  {
    wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_UNSUPPORTED );
    st = WMS_UNSUPPORTED_S;
  }
#endif /* FEATURE_GWSMS_BROADCAST */

  return st;
} /* wms_bc_mm_msg_delete_all_indication() */

/* <EJECT> */
/*=========================================================================
FUNCTION
  wms_bc_mm_get_reading_pref

DESCRIPTION
  This function is used to Get the Reading Preferences - Reading Advised 
  and Reading Optional for UMTS CBS Messages
  This is applicable to GW mode only.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.
=========================================================================*/
wms_status_e_type wms_bc_mm_get_reading_pref
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                     *user_data,
  wms_message_mode_e_type         message_mode
)
{
#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  boolean is_command_allowed = FALSE;
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */

  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr = & cmd_backup;

  cmd_backup.hdr.cmd_id      = WMS_CMD_BC_MM_GET_READING_PREF;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;

  if(message_mode == WMS_MESSAGE_MODE_GW)
  {
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */
  }

#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  if (is_command_allowed == TRUE)
  {
    cmd_backup.cmd.bc_mm_get_reading_pref.message_mode = message_mode;

    cmd_ptr = wms_get_cmd_buf();

    if( cmd_ptr != NULL)
    {
      * cmd_ptr = cmd_backup;
      wms_put_cmd( cmd_ptr );
    }
    else
    {
      wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NO_RESOURCE );
      st = WMS_OUT_OF_RESOURCES_S;
    }
  }
  else
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
  {
    wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_UNSUPPORTED );
    st = WMS_UNSUPPORTED_S;
  }

  return st;
} /* wms_bc_mm_get_reading_pref */

/* <EJECT> */
/*=========================================================================
FUNCTION
  wms_bc_mm_set_reading_pref

DESCRIPTION
  This function is used to Set the Reading Preferences - Reading Advised 
  and Reading Optional for UMTS CBS Messages
  This is applicable to GW mode only.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.
=========================================================================*/
wms_status_e_type wms_bc_mm_set_reading_pref
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                     *user_data,
  wms_message_mode_e_type         message_mode,
  boolean                         reading_advised,
  boolean                         reading_optional
)
{
#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  boolean is_command_allowed = FALSE;
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */

  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr = & cmd_backup;

  cmd_backup.hdr.cmd_id      = WMS_CMD_BC_MM_SET_READING_PREF;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;

  if(message_mode == WMS_MESSAGE_MODE_GW)
  {
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */
  }

#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  if (is_command_allowed == TRUE)
  {
    cmd_backup.cmd.bc_mm_set_reading_pref.message_mode = message_mode;
    cmd_backup.cmd.bc_mm_set_reading_pref.reading_advised = reading_advised;
    cmd_backup.cmd.bc_mm_set_reading_pref.reading_optional = reading_optional;

    cmd_ptr = wms_get_cmd_buf();

    if( cmd_ptr != NULL)
    {
      * cmd_ptr = cmd_backup;
      wms_put_cmd( cmd_ptr );
    }
    else
    {
      wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NO_RESOURCE );
      st = WMS_OUT_OF_RESOURCES_S;
    }
  }
  else
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
  {
    wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_UNSUPPORTED );
    st = WMS_UNSUPPORTED_S;
  }

  return st;
} /* wms_bc_mm_set_reading_pref */

/* <EJECT> */
/*===========================================================================

                            Debugging Group

                        API FUNCTION DEFINITIONS

===========================================================================*/

/*=========================================================================
FUNCTION
  wms_dbg_reset_tl_seq_num

DESCRIPTION
  Reset internal count for the CDMA SMS TL sequence number.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

=========================================================================*/
wms_status_e_type wms_dbg_reset_tl_seq_num
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr = & cmd_backup;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_backup.hdr.cmd_id      = WMS_CMD_DBG_RESET_TL_SEQ_NUM;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;

#ifdef FEATURE_CDSMS

  cmd_ptr = wms_get_cmd_buf();

  if( cmd_ptr != NULL)
  {
    * cmd_ptr = cmd_backup;
    wms_put_cmd( cmd_ptr );
  }
  else
  {
    wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NO_RESOURCE );
    st = WMS_OUT_OF_RESOURCES_S;
  }

#else /* FEATURE_CDSMS */
  wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_UNSUPPORTED );
  st = WMS_UNSUPPORTED_S;
#endif /* FEATURE_CDSMS */

  return st;

} /* wms_dbg_reset_tl_seq_num() */

/* <EJECT> */
/*=========================================================================
FUNCTION
wms_dbg_set_msg_ref

DESCRIPTION
  Sets THE TP-MR to 0. Command is sent to WMS Task.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS


=========================================================================*/
wms_status_e_type wms_dbg_set_msg_ref
(
  wms_client_id_type               client_id,
  wms_cmd_cb_type                  cmd_cb,
  const void                       * user_data
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr = & cmd_backup;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_backup.hdr.cmd_id      = WMS_CMD_DBG_SET_MSG_REF_NUM;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;


#ifdef FEATURE_GWSMS
#error code not present
#else /* FEATURE_GWSMS */
  wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_UNSUPPORTED );
  st = WMS_UNSUPPORTED_S;
#endif /* FEATURE_GWSMS */

  return st;
} /* wms_dbg_set_msg_ref */

/* <EJECT> */
/*=========================================================================
FUNCTION
wms_dbg_clear_smma_flag

DESCRIPTION
  Resets the SMMA Flag. Command is sent to WMS Task.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
=========================================================================*/
wms_status_e_type wms_dbg_clear_smma_flag
(
  wms_client_id_type               client_id,
  wms_cmd_cb_type                  cmd_cb,
  const void                       * user_data
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr = & cmd_backup;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_backup.hdr.cmd_id      = WMS_CMD_DBG_CLEAR_SMMA;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;


#ifdef FEATURE_GWSMS
#error code not present
#else /* FEATURE_GWSMS */
  wms_client_cmd_status( cmd_ptr, WMS_CMD_ERR_UNSUPPORTED );
  st = WMS_UNSUPPORTED_S;
#endif /* FEATURE_GWSMS */

  return st;
} /* wms_dbg_clear_smma_flag */

/* <EJECT> */
/*=========================================================================
FUNCTION
wms_dbg_get_retry_interval

DESCRIPTION
  Retreive the SMS Retry Interval. i.e. the interval between the SMS Retry Attempts
  Value is specified in seconds. Command is sent to WMS Task.
  A Debug Event will be posted by WMS its clients with the Retry Interval.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
=========================================================================*/
wms_status_e_type wms_dbg_get_retry_interval
(
  wms_client_id_type               client_id,
  wms_cmd_cb_type                  cmd_cb,
  const void                       * user_data
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr = & cmd_backup;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_backup.hdr.cmd_id      = WMS_CMD_DBG_GET_RETRY_INTERVAL;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;

  cmd_ptr = wms_get_cmd_buf();

  if( cmd_ptr != NULL)
  {
    * cmd_ptr = cmd_backup;
    wms_put_cmd( cmd_ptr );
  }
  else
  {
    wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NO_RESOURCE );
    st = WMS_OUT_OF_RESOURCES_S;
  }

  return st;
} /* wms_dbg_get_retry_interval */

/* <EJECT> */
/*=========================================================================
FUNCTION
wms_dbg_set_retry_interval

DESCRIPTION
  Retreive the SMS Retry Interval. i.e. the interval between the SMS Retry 
  Attempts Value is specified in seconds. Command is sent to WMS Task.
  A Debug Event will be posted by WMS its clients with the Retry Interval.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
=========================================================================*/
wms_status_e_type wms_dbg_set_retry_interval
(
  wms_client_id_type               client_id,
  wms_cmd_cb_type                  cmd_cb,
  const void                       * user_data,
  uint32                           retry_interval /* seconds */
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr = & cmd_backup;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_backup.hdr.cmd_id      = WMS_CMD_DBG_SET_RETRY_INTERVAL;
  cmd_backup.hdr.client_id   = client_id;
  cmd_backup.hdr.cmd_cb      = cmd_cb;
  cmd_backup.hdr.user_data   = (void*)user_data;

  cmd_backup.cmd.dbg_set_retry_interval.retry_interval = retry_interval;

  cmd_ptr = wms_get_cmd_buf();

  if( cmd_ptr != NULL)
  {
    * cmd_ptr = cmd_backup;
    wms_put_cmd( cmd_ptr );
  }
  else
  {
    wms_client_cmd_status( & cmd_backup, WMS_CMD_ERR_NO_RESOURCE );
    st = WMS_OUT_OF_RESOURCES_S;
  }

  return st;
} /* wms_dbg_set_retry_interval */

/* <EJECT> */
/*===========================================================================
FUNCTION WMSI_GET_CARD_PRESENCE

DESCRIPTION
  Get the presence information for RUIM and SIM Cards.

RETURN VALUE
  None

DEPENDENCIES
  None
===========================================================================*/
void wmsi_get_card_presence(boolean* pbRUIM, boolean* pbSIM)
{
#if defined(FEATURE_CDSMS_RUIM) || defined(FEATURE_GWSMS)
   gsdi_card_apps_T card_apps;

   gsdi_get_apps_available(&card_apps);
#endif /* FEATURE_CDSMS_RUIM || FEATURE_GWSMS */

   *pbRUIM = FALSE;
   *pbSIM  = FALSE;
#ifdef FEATURE_CDSMS
#ifdef FEATURE_CDSMS_RUIM
   if (card_apps.slot1_present && card_apps.apps_enabled & GSDI_RUIM_APP_MASK 
#ifdef FEATURE_MMGSDI_DUAL_SLOT
       || card_apps.slot2_present && card_apps.apps2_enabled & GSDI_RUIM_APP_MASK
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
       )
   {
      *pbRUIM = TRUE;
   }
#endif /* FEATURE_CDSMS_RUIM */
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS
#error code not present
#endif
} /* wmsi_get_card_presence() */

#ifdef FEATURE_GSTK
/* <EJECT> */
/*===========================================================================
FUNCTION WMS_CARD_REFRESH_CB

DESCRIPTION
  Called by GSDI when it has completed servicing WMS's Refresh REQUEST.
  This is a handler for all Refresh callbacks.

RETURN VALUE
  None

DEPENDENCIES
  None
===========================================================================*/
void wms_card_refresh_cb
(
  gsdi_cnf_T      * gsdi_cnf
)
{
  if ( gsdi_cnf->message_header.gsdi_status == GSDI_SUCCESS)
  {
    MSG_HIGH(" WMS GSDI Refresh request successful",0,0,0);
  }
  else
  {
    MSG_ERROR(" WMS GSDI Refresh request Failed Code: %d",
    gsdi_cnf->message_header.gsdi_status,0,0);
  }
  return;
} /* wms_card_refresh_cb() */

/* <EJECT> */
/*===========================================================================
FUNCTION WMS_CARD_REFRESH_COMPLETE

DESCRIPTION
  Called within WMS to indicate that it has completed reading the files it
  was notified to read.

RETURN VALUE
  GSDI status.

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done, and it's only called from the WMS task.
===========================================================================*/
void wms_card_refresh_complete
(
  boolean               pass_fail
)
{
  gsdi_returns_T  gsdi_status = GSDI_SUCCESS;

  gsdi_status = gsdi_refresh_complete(temp_refresh_fcn_ind.num_files,
                                      temp_refresh_fcn_ind.file_list,
                                      pass_fail,
                                      NULL,
                                      wms_card_refresh_cb);

  MSG_MED(" Resetting Refresh in Progress",0,0,0);
  cfg_s_ptr->refresh_in_progress = FALSE;

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MSG_ERROR("WMS: Failed to make REFRESH COMPLETE REQ",0,0,0);
  }
} /* wms_card_refresh_complete */

/* <EJECT> */
/*==============================================================================

FUNCTION NAME WMS_CHECK_IDLE

DESCRIPTION
  Function that gets to check if WMS is idle or not
  for determining whether to refresh or not

RETURN VALUE
  TRUE: WMS is idle; refresh can proceed
  FALSE: WMS is not idle; refresh cannot proceed

PARAMETER
  allow_current_command: to return Idle even if the
  currently executed command is calling it
DEPENDENCIES
  None
==============================================================================*/
static boolean wms_check_idle
(
  boolean allow_current_command
)
{
  boolean wms_idle = TRUE;

  if (wms_msg_check_in_use_records())
  {
    /* Some MO, MT records are in use */
    MSG_HIGH("WMS Not Idle: Some Mo / MT Records in Use",0,0,0);
    wms_idle = FALSE;
  }
  else
  {
    if (allow_current_command)
    {
      if (wms_get_cmd_count() > 1)
      {
        /* WMS has other commands in the queue */
        MSG_HIGH("WMS Not Idle: Command Count > 1",0,0,0);
        wms_idle = FALSE;
      }
    }
    else
    {
      if (wms_get_cmd_count() > 0)
      {
        /* WMS is processing some command */
        MSG_HIGH("WMS Not Idle: Command Count > 0",0,0,0);
        wms_idle = FALSE;
      }
    }

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
  }

  return wms_idle;
} /* wms_check_idle */

/* <EJECT> */
/*==============================================================================

FUNCTION NAME WMS_CARD_REFRESH_VOTE_CB

DESCRIPTION
  Function that gets called by GSDI when a REFRESH Request is made and a
  determination must be made whether to proceed or not.

RETURN VALUE

  void

DEPENDENCIES
  None
==============================================================================*/
void wms_card_refresh_vote_cb
(
  uim_items_enum_type * file_list_p,
  uint8                 num_files,
  boolean               req_to_take_down_call_stack,
  boolean             * ok_to_take_down_call_stack_p,
  boolean             * ok_to_do_file_notify_p
)
{
  // boolean wms_is_totally_idle = TRUE;

  if ( (NULL == ok_to_take_down_call_stack_p)
       ||(NULL == ok_to_do_file_notify_p) )
  {
     MSG_ERROR("Null Pointer in wms_card_refresh_vote_cb",0,0,0);
     return;
  }

  // WMS Always votes positively for Refresh
  *ok_to_take_down_call_stack_p = TRUE;
  *ok_to_do_file_notify_p = TRUE;

  // Previous Code 
  /* 
  wms_is_totally_idle = wms_check_idle(FALSE);

  if (wms_is_totally_idle)
  {
    *ok_to_take_down_call_stack_p = TRUE;
    *ok_to_do_file_notify_p = TRUE;
  }
  else
  {
    uint32 file_count = 0;

    *ok_to_take_down_call_stack_p = FALSE;
    
    // Go through File List to make Informed Decision 
       Only if File is of Interest, say FALSE to Voting
    
    *ok_to_do_file_notify_p = TRUE;

    if (file_list_p != NULL)
    {
      for (file_count = 0; file_count < num_files ; file_count++)
      {
        if ( (file_list_p[file_count] == UIM_TELECOM_SMS)
             || (file_list_p[file_count] == UIM_TELECOM_SMSP)
             || (file_list_p[file_count] == UIM_TELECOM_SMSS)
             || (file_list_p[file_count] == UIM_TELECOM_SMSR)
             || (file_list_p[file_count] == UIM_USIM_SMS)
             || (file_list_p[file_count] == UIM_USIM_SMSP)
             || (file_list_p[file_count] == UIM_USIM_SMSS)
             || (file_list_p[file_count] == UIM_USIM_SMSR)
             || (file_list_p[file_count] == UIM_CDMA_SMS)
             || (file_list_p[file_count] == UIM_CDMA_SMS_PARAMS)
             || (file_list_p[file_count] == UIM_CDMA_SMS_STATUS)
             || (file_list_p[file_count] == UIM_GSM_CBMI)
             || (file_list_p[file_count] == UIM_GSM_CBMID)
             || (file_list_p[file_count] == UIM_GSM_CBMIR) )
        {
          *ok_to_do_file_notify_p = FALSE;
        }
      }
    }
  }
  */

} /* wms_card_refresh_vote_cb */

/* <EJECT> */
/*==============================================================================

FUNCTION NAME WMS_CARD_REFRESH_FILE_CHANGE_NOTIFICATION_CB

DESCRIPTION
  Function that gets called by GSDI when a FCN arrives

RETURN VALUE

  void

DEPENDENCIES
  None
==============================================================================*/
void wms_card_refresh_file_change_notification_cb
(
  uim_items_enum_type  * file_list_p,
  uint8                   num_files
)
{
  wms_cmd_type         *cmd_ptr;
  cmd_ptr = wms_get_cmd_buf();

  if(file_list_p == NULL)
  {
    MSG_ERROR("null ptr: wms_card_refresh_file_change_notification_cb",0,0,0);
    return;
  }

  if (cmd_ptr != NULL)
  {
    /* Fill in command type */
    cmd_ptr->hdr.cmd_id = WMS_CMD_GSDI_REFRESH_FCN_IND;
    cmd_ptr->hdr.client_id   = WMS_CLIENT_TYPE_INTERNAL;
    cmd_ptr->hdr.cmd_cb      = NULL;
    cmd_ptr->hdr.user_data   = NULL;

    /* Copy data to command buffer */
    cmd_ptr->cmd.gsdi_refresh_fcn_ind.num_files = num_files;
    memcpy(cmd_ptr->cmd.gsdi_refresh_fcn_ind.file_list,
           file_list_p,
           (sizeof(uim_items_enum_type) * num_files));

    rex_enter_crit_sect(&sms_card_refresh_crit_sect);

    /* Put message in wms task queue */
    wms_put_cmd(cmd_ptr);

    MSG_MED(" Setting Refresh in Progress",0,0,0);

    cfg_s_ptr->refresh_in_progress = TRUE;

    rex_leave_crit_sect(&sms_card_refresh_crit_sect);
  }
  else
  {
    MSG_ERROR("No WMS Command Buffers", 0, 0, 0);
  }
} /* wms_card_refresh_file_change_notification_cb */

/* <EJECT> */
/*==============================================================================
FUNCTION NAME WMS_CARD_REGISTER_FOR_REFRESH

DESCRIPTION
  Will Register WMS for File Change Notifications and Voting Privledges.

RETURN VALUE
  void

DEPENDENCIES
  None
==============================================================================*/
void wms_card_register_for_refresh
(
  void
)
{
  gsdi_returns_T gsdi_status = GSDI_SUCCESS;
  uim_items_enum_type file_list[WMS_CARD_NUM_REFRESH_FILES] = {
                         UIM_TELECOM_SMS,
                         UIM_TELECOM_SMSP,
                         UIM_TELECOM_SMSS,
                         UIM_TELECOM_SMSR,
                         UIM_USIM_SMS,
                         UIM_USIM_SMSP,
                         UIM_USIM_SMSS,
                         UIM_USIM_SMSR,
                         UIM_CDMA_SMS,
                         UIM_CDMA_SMS_PARAMS,
                         UIM_CDMA_SMS_STATUS,
                         UIM_GSM_CBMI,
                         UIM_GSM_CBMID,
                         UIM_GSM_CBMIR
                         };

  /* Register with GSDI for Voting
  ** Privledges.  To keep from
  ** resetting Stack when in a call
  */
  gsdi_status = gsdi_register_for_refresh_voting( wms_card_refresh_vote_cb,
                                                  NULL,
                                                  wms_card_refresh_cb );
  /* Check to see if REQ for Registration
  ** was ok
  */
  if ( gsdi_status != GSDI_SUCCESS)
  {
    MSG_ERROR( "Failed to Request Refresh Vote Registration",0,0,0);
  }

  /* Register with GSDI for File Change
  ** Notification using the file list
  ** in the array above
  */
  gsdi_status = gsdi_register_for_file_change_notifications(
                                  wms_card_refresh_file_change_notification_cb,
                                  WMS_CARD_NUM_REFRESH_FILES,
                                  file_list,
                                  NULL,
                                  wms_card_refresh_cb);

  /* Check to see if REQ for Registration
  ** was ok
  */
  if ( gsdi_status != GSDI_SUCCESS)
  {
    MSG_ERROR( "Failed to Request Refresh FCN Registration",0,0,0);
  }
} /* wms_card_register_for_refresh */


/* <EJECT> */
/*===========================================================================
FUNCTION WMS_CARD_REFRESH_FCN_IND_PROC

DESCRIPTION
  Called during WMS Command Buffer processing to refresh the File List
  returned to it by GSDI_REFRESH

RETURN VALUE
  None

DEPENDENCIES
  None
===========================================================================*/
void wms_card_refresh_fcn_ind_proc
(
  wms_cmd_type   * cmd_ptr
)
{

  /* Store it statically to be called from GSDI Async Proc */
  temp_refresh_fcn_ind = cmd_ptr->cmd.gsdi_refresh_fcn_ind;

  if (!wms_check_idle(TRUE))
  {
    /* WMS is not completely IDLE
       Fail the refresh */
    wms_card_refresh_complete(FALSE);
  }
  else
  {
    /* WMS is completely IDLE
       Do the Refresh */
      boolean bRUIMCardPresent = FALSE;
      boolean bSIMCardPresent = FALSE;

      wmsi_get_card_presence(&bRUIMCardPresent, &bSIMCardPresent);
#ifdef FEATURE_CDSMS_RUIM
      if (bRUIMCardPresent) {
         /* RUIM Card is present. Lets clr the CDMA init state and 
         ** initialize the RUIM card. If the card initialization completes
         ** successfully we set the CDMA init state */
         cfg_s_ptr->cdma_init_complete = FALSE;
#ifdef FEATURE_INIT_RUIM_SMSandADD_BYUIMTASK
    db_setuiminitmask(INITUIMSMSMASK);
    //wms_ruim_init_stepbystep();
#ifdef FEATURE_OMH_SMS
    wms_ruim_OMH_init();
#endif
#else
         wms_ruim_init();  /* this function sets cdma_init_complete to TRUE
                           ** on succesful (sort of) initialization */
#endif
      }
#endif /* FEATURE_CDSMS_RUIM */

#ifdef FEATURE_GWSMS
#error code not present
#else
    /* Refresh is Complete */
    wms_card_refresh_complete(TRUE);
#endif /* FEATURE_GWSMS */
  }

  /* done */
  return;

} /* wms_card_refresh_fcn_ind_proc() */

#endif /* FEATURE_GSTK */

#endif /* FEATURE_CDSMS || FEATURE_GWSMS */

/* END of wms.c */

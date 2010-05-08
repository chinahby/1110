/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  C A L L   M A N A G E R   M O D U L E
                        Task related functions

GENERAL DESCRIPTION
  This module makes up the Call Manager of the subscriber station software.

  The Call Manager provides out of bound call control and event notification
  services to its clients.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 1998 - 2009 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/cm/rel/08H1/src/cmtask.c#6 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/20/09   ks      added support for camped cell info requirement

04/23/09   rm      Updating correct mode pref of phone when data_avaialable
                   request comes.
04/09/09   aj      Added support for hold cmd reason for hold cmd Q
02/13/09   rk      Removed CDMA feature flag so that considering PS call
                   domain preference is applicable for GW target also.
01/15/09   np      Remove feature define for cm_hold_cmd_q_init
11/17/08   am      Replacing CM GSDI interface with CM MMGSDI interface
09/16/08   pk      Added support for WLAN UOoS
09/04/08   dm/sv   Added callback function for the WPS result
09/04/08   ks      Remove info for Neighbors Cell IDs
08/20/08   st      Resolve Klockwork defects
08/05/08   pk      Fixed a memory leak
07/18/08   ks      Added new info for Current Operator's Cell ID and 
                   Neighbors Cell IDs
07/01/08   rm      Mainlining of MBMS changes
06/18/08   sv      Fixed Compiler warnings
06/6/08    rn      Added check for below_ph prio act in compute_srv_domain
06/06/08   sv      Removing unneccesary snd.h
05/08/08   sv      Featurized snd.h under CDMA
05/02/08   vk      fixed messages for debug
05/06/08   ak      BCMCS 2.0 changes
03/31/08   ks      Add new CM client critical section
03/13/08   sk      Fix to allow CDMA scans during dormant GW PS data calls
                   without tearing down the PS data call.
01/24/08   vs      Reset orig_int_hybr_pref value in cmtask_orig_para_init_2
12/07/07   ra      Added support for IMS VCC in HOMER files
08/07/07   ic      Lint clean up
06/14/07   pk/cl   Added support for power suspend
05/16/07   cl      Fix typo in include file AEEstd.h
05/09/07   rp/cl   Allocate from tmc heap is size requested is greater than
                   CM_SIZE_OF_LARGEST_DYN_OBJ
03/09/07   sk      RAM usage reduction.
03/07/07   jqi     Added support for FEATURE_SMS_OVER_IP
02/26/07   ka      Changes to support Voip deregistration.
02/23/07   pk      Banned API lint fixes
02/18/07   rp      Replaced FEATURE_APIONE with FEATURE_WMS_CM_NAS_INTERFACES
01/19/07   pk      Reverting previous lint fixes
01/12/07   pk      Lint fixes
11/07/06   pk      Added support for HYBR_GW and UMTS -> 1X handover
10/30/06   ic      Featurization changes for 7600 / SC2X targets
08/16/06   ic      Lint fixes
07/07/06   pk      Added cmtask_add_cmd_to_hold_q, and supporting data
                   structures, fields and functions
06/27/06   ka      Adding feature flag FEATURE_IP_CALL
06/15/06   sk      Added support for command throttling.
06/14/06   pk      Added support for cmtask_orig_para_count_2 and helper
                   functions
04/06/06   sk      Fixed cmtask_compute_srv_domain_pref() to consider
                   CM_SRV_DOMAIN_PREF_NONE as a valid service domain pref.
04/06/06   sk      Added call to cmbcmcs_sd_rpt_proc() in cm_sdrpt_handle()
                   Removed call to cmbcmcs_timer_proc() in  cm_timer_handle()
04/01/06   ka      Changes for emerg serv category during orig, aborting
                   manual search and returning current band with srv ind.
03/27/06   pk      Added support for cmtask_orig_para_change_act_priority
                   Added helper functions for the support
03/15/06   sk      Added call to cmclient_map_init() in
                   cm_init_before_task_start()
03/09/06   pk      Added support for POWER COLLAPSE
01/30/06   sk      Added debug info to help catch CM cmd buffer memory leaks.
01/19/06   pk      Modified cm_dip_switch_set_ddtm_off to go to default DDTM
                   settings. Externed ddtm dip switch functions to cmi.h
01/12/06   ic      For specific calls / walks Lint warnings, replaced global
                   -fsp comment with -e{error#} comments where needed
01/06/06   ic      Lint cleanup
12/11/05   ic      Lint cleanup
11/18/05   sk      Doubled CM heap size from 10500 bytes to 21000 bytes.
11/08/05   sk      Separated CM reports and commands.
11/03/05   ic      Added CM_ASSERT() for ptr function parameters and lint
                   comments for 613 Lint error
10/20/05   pk      Cleaned RVCT compiler warnings and added mapping functions
10/20/05   ic      For all calls to cm_ph_cmd_ddtm_pref() within CM, pass
                   ARR_SIZE(so_list) instead of sizeof(so_list) as the
                   num_srv_opt parameter
10/13/05   ic      Added CM critical section support
09/15/05   pk      Changed Priority Queue related messages to CM_MSG_LOW
09/12/05   pk      Added cmpd_timer_proc(timer_event)
08/31/05   pk      Printing all the input parameters for priority Queue
                   functions
08/15/05   pk      Added support for priority Queues for origination
                   parameters
06/06/05   sk      Added calls to debug functions for SD and GW reports.
                   Cleaned up code so that the debug functions for reports
                   are called at the appropriate functions instead of at
                   cm_rpt_handle(). Removes confusing pointer type casts.
05/09/05   pk      Increased the size of Command Queue to 20. Removed
                   FEATURE_T53 featurization for the command queue size
03/14/05   ka      Adding new rpt queue for IP App reports and added a call
                   to cmipapp_init().
03/11/05   dk      Increased the CM_MC_RPT_POOL_SIZE to 20 to account for
                   CDMA, HDR and WLAN co-existance.
02/01/05   dk      Added WLAN Support.
01/07/05   ic      Removed all code under and referring to FEATURE_UASMS
11/22/04   ws      Additional support for Remote WMS.
11/15/04   ic      Lint cleanup with CM_DEBUG on
11/11/04   ic      Lint cleanup
09/08/04   jqi     More ps dormant optimization.
08/27/04   jqi     Added ps dormant optimization.
08/04/04   ic      CM Lint clean up - cmtask.c with Lint supression verified
                   and free of Lint issues in all CUTE configurations.
07/26/04   ic      Changed include file order to customer.h, then cm.h then
                   cmi.h
07/23/04   sj      Removed static for client_cmd_q_get to compile for
                   handover manager.
07/12/04   ic      Added support for FEATURE_ALS
07/07/04   ka      Fixed Lint errors
06/08/04   ank     Mainlined FEATURE_GPSONE_DBM.
06/09/04   dk      Added CM Heap support.
05/14/04   ka      Fixed Lint errors.
04/13/04   ic      CM_ASSERT() clean up
03/31/04   ic      Mainlined FEATURE_REX_DYNA_MEM in CM and unit test.
                   Removed all code related to static allocation of call
                   objects.
03/17/04   sj      Modified call to new DDTM api.
03/16/04   ka      Mereged changes from 6250.
                   Use service domain form service indicator for call orig.
                   Propagated service domain for call origination.
                   Fixed forcing service domain on the fly for GW system.
03/02/04   ic      Added BCMCS support (FEATURE_BCMCS)
02/09/04   ic      Changes so the code compiles for unit test.
02/06/04   ws      Initial jaguar/mainline merge.
10/20/03   ws      Merged SJ's changes for featurization problem
08/20/03   SJ      Added call to cmregprx_timer_proc.
08/12/03   ws      Added support for MM Silent Redial and Automatic Mode.
07/28/03   SJ      Support for circulating data suspend flag thru' SD.
07/24/03   vt      Fixed a compile warning with mccreg_reset_reg().
07/17/03   sj      Updated call to sd_si_reg() and added reg_done_cb func.
05/15/03   vt      Made cm init function available to cm unit test tool.
05/15/03   ic      Removed FEATURE_MONITOR / FEATURE_JCDMA_MONITOR
04/28/03   AT      Lint cleanup.
04/11/03   prk     Mainlined FEATURE_SUPS, FEATURE_AOC and FEATURE_MMGSDI.
02/25/03   prk     Modified call to cmdiag_init so it is called for all modes.
                   Added include of cmdiag.h.  Mainlined FEATURE_MUTLIMODE_ARCH.
                   Ignored return code from cmregprx_rpt_proc().
02/20/03   ATM     Added cmlog_init () for mode/preference logging.
02/14/03   ws      Added cmdiag_init ().
02/06/03   sj      Added fix for RegProxy rpts being processed twice.
01/21/03   ws      Updated copyright information for 2003
12/03/02   ws      Added FEATURE_CDSMS for call to cmsms_sd_rpt_proc
11/14/02   vt      Merged changes from GW mainline.
                   [ws]Merged in position determination changes.
                   [CS]Added support for invalid SIM handling. Init callback function
10/31/02   vt      Removed extra q_link before q_put.
                   Changed when regproxy init is called.
                   Mainlined feature SD20.
10/22/02   ic      In cm_init_before_task_start() added call
                   to cm_m51_mode_init().
                   Under FEATURE_MONITOR fixed compile error
                   (a field changed from client to client_id),
10/07/02   vt      Added regproxy handling.
10/05/02   vt      Added cm_sd_rpt(). Modified to use new cm_si_init()
                   interface.
09/17/02   az      Added DBM handling.
09/10/02   AP      Added support for FEATURE_UASMS_SHIM.
08/06/02   HQ      Added FEATURE_CDSMS.
06/11/02   RI      Modified cm_client_cmd_q_put() to link the client
                   commands.
06/07/02   jqi     Update with MultiMode task operation interface.
05/07/02   PRK     Included cmxpd.h instead of cmpd.h.
                   Included cmxsms.h instead of cmsms.h.
                   Included cmwsups.h instead of cmwsups.h.
04/16/02   ATT     Integrated AoC with GSDI and UI.
03/13/02   ATM     Added support for new GS_SEND_MESSAGE
04/01/02   ic      In cm_client_cmd_handle() shortened the string that prints
                   CM commands so they fit in the diagnostic monitor display
03/25/02   ic      Included cm.h to fix "inventing 'extern int cm_dbm_init();"
                   compiler warning.
                   Added printing fail reason for origination failures.
03/07/02   ic      Changes to cm_mc_send_cmd_print() and cm_mc_rpt_handle_print()
                   From MSM_CM.05.03.41 cm.h took
                   #define CM_NV_ASYNC_SIG     0x0080
                   #define CM_NV_SYNC_SIG      0x0081
                   to make SVD CM compile with MSM_UI.05.04.71 without
                   having to sync SVD CM to MSM_CM.05.03.41
                   (MSM_UI.05.04.70 has dependency on MSM_CM.05.03.41).
                   Then:
                   - in cm_task() replaced CM_NV_SIG with CM_NV_ASYNC_SIG
                   - in cm_nv_handle() replaced CM_NV_SIG with CM_NV_ASYNC_SIG
02/07/02   ic      Under FEATURE_IS2000_REL_A, added support for printing
                   strings that correspond to flash type enums
02/06/02   ic      In cm_mc_rpt_handle_print(), removed printing of call id
                   for CM_EXT_BRST_INTL_F as it had been removed from the
                   cm_ext_brst_intl_type structure.
01/29/02   ic      Removed "CM->MC" and "MC->CM" strings whenever call id is
                   printed, otherwise the call id was cut off the diag monitor
                   display.
                   Added extra notification to messages printed when the call id
                   received was CM_CALL_ID_INVALID, CM_CALL_ID_UNASSIGNED or
                   not currently allocated by the CM.
01/22/02   ic      In cm_mc_rpt_handle_print(), made the code print call id for
                   CM_CALL_ORIG_FAIL_F case.
01/18/02   ic      Merged MSM_CM.05.03.33 on top of tip CCS CM.
                   Diffed MSM_CM.05.03.32 with MSM_CM.05.03.33 and only the changes
                   between those two were merged with CCS CM.
                   Added support for printing call_id together with CM/MC messages
                   (if applicable).
12/13/01   ic      Merged MSM_CM.05.03.32 onto SVD CM tip.
                   CCS CM code started from CM VU MSM_CM.05.03.25 + WCDMA/GSM
                   stuff from SIMBA team merged on top of it.
                   We diff'ed MSM_CM.05.03.25 with MSM_CM.05.03.32 (CM VU in N*2165)
                   that we needed to merge with and changes between those two VUs
                   were merged onto CCS CM tip.
11/30/01   hxw     Added FEATURE_GPSONE_NEW_L2_ACK to support delay L2 Ack
                   mechanism.
11/21/01   RI      Corrected the FEATURE_NEWSS header file declaration.
10/23/01   VT/IC   Baseline for CCS
10/18/01   PRK     Added changes for WCDMA/GSM mode.
                     Moved processing of rssi from phone group to serving system
                     group. Added sending CM_SS_CMD_CLIENT_ACT command to
                     serving system. (FEATURE_NEWSS).
10/15/01   RI      Moved the CM task functions from cm.c to cmtask.c
08/23/01   HD      NAS header file reorg changes+ Inband and RSSI reporting for WCDMA.
04/27/01   HD      Added changes for WCDMA/GSM mode.
02/26/01   sko     Change CM monitor implementation from event to polling.
02/06/01   RC      Added support for System Determination 2.0.
01/29/01   HQ      Properly feature-ized NEWCM, UASMS and GPSONE_DBM.
01/23/01   ych     Merged T53 and JCDMA features.
12/28/00   fc      Added support for retry order.
12/15/00   HQ      Added CM_CALL_CMD_PRIVACY_PREF, CM_CALL_EVENT_PRIVACY_PREF
                   and CM_CALL_CMD_ERR_BAD_PRIVACY_PREF.
10/06/00   HD      Added support to pass the DRS bit setting to CP.
09/15/00   PJ      Added UAPDMS task. Added support for PD commands.
                   (FEATURE_GPSONE_DBM).
08/23/00   vt      Added new CM interface changes.
07/17/00   jq      Added cm_call_cmd_end_with_reason()
03/20/00   RY      Removed CM_CMD_TYPE_PH from cm_hold_cmd_q_check()
03/16/00   VT      Added BROADCAST SMS support
03/13/00   RY      Added OTAPA support
01/11/00   VT      Added cm_is_valid_mode_pref().
12/18/99   CS      Added cm_kick_dog function.
09/14/99   SH      Removed cm_clk_init
09/07/99   SH      Added uasms_init_cm
08/31/99   SH      Modified codes to featurize OTASP
08/31/99   SH      Modified codes to featurize UASMS
08/25/99   SH      Added cmcall_offline
08/17/99   SH      Used new sound API when new sound feature is defined
08/09/99   SH      Added SMS object
07/07/99   SH      Added CNAP feature
06/30/99   SH      Added activate_code for cm_call_cmd_orig
06/28/99   SH      Modified cm_nc_init
06/21/99   SH      Added processing for TASK_STOP_SIG and TASK_OFFLINE_SIG
05/31/99   SH      Added CM 2.0 features
04/09/99   jkl     Modified for new Sound API
01/07/99   RC      Changes per ARM compiler.
12/30/98   RC      Changes per 4th week of I&T.
12/08/98   RC      Changes per second week of I&T.
11/24/98   RC      Initial release.

===========================================================================*/

/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/

#include "customer.h"  /* Customer configuration file */
#include "cmnv.h"
#include "queue.h"     /* Interface to queue services */
#include "comdef.h"    /* Definition for basic types and macros */
#include "AEEstd.h"    /* Definition for basic types and macros */
#include <string.h>    /* Interface to string library */
#include "cmtask.h"    /* Interface to CM task services */
#include "cmdbg.h"     /* Interface to CM debug services */
#include "cmclient.h"  /* Interface to CM Client Interface Block */
#include "cmnv.h"      /* Interface to CM NV services */
#include "cmcall.h"
#include "cmph.h"      /* Interface to CM phone object */
#include "cmss.h"      /* Interface to CM SS object */
#include "cminband.h"  /* Interface to CM inband object */
#include "cmbcmcs.h"   /* Interface to CM BCMCS object */
#if (defined FEATURE_ALS) && (defined FEATURE_GSM)
#error code not present
#endif
#include "err.h"       /* Interface to error log services */
#include "cmreply.h"
#include "sd.h"

#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
#include "cai.h"       /* Over the air messages definitions */
#include "mc.h"        /* Interface to MC Task */
#include "mccreg.h"    /* For definition of mccreg_reset_reg() */
#include "cmxll.h"      /* MC to CM interface*/
#include "db.h"        /* Interface to Database services */
#include "clk.h"       /* Interface to clock services */
#include "cmxsms.h"    /* Interface to cmsms.c*/
#include "cmdiag.h"    /* Interface to diag */
#endif /* (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */
#include "cmlog.h"     /* Interface to diag logging */
#include "cmdiag.h"    /* Interface to diag packets */
#if ((defined FEATURE_MMGSDI) && (defined FEATURE_CM_MMGSDI))
#include "cmmmgsdi.h"
#else /* !((defined FEATURE_MMGSDI) && (defined FEATURE_CM_MMGSDI)) */
#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
#endif  /* (defined(FEATURE_WCDMA) || defined(FEATURE_GSM)) */
#endif /* !((defined FEATURE_MMGSDI) && (defined FEATURE_CM_MMGSDI)) */
#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */

#include "task.h"

#include "nv.h"        /* Interface to NV services */
#include "dog.h"       /* Interface to watchdog services */

#if (defined(FEATURE_GPSONE_MSBASED) || defined(FEATURE_GPSONE))
#error code not present
#endif  /* FEATURE_GPSONE_MSBASED || FEATURE_GPSONE */

#ifdef FEATURE_DBM
#include "cmxdbm.h"     /* Data Burst Messages - PD and SDB */
#endif

#include "cmpref.h"    /* Interface to CM pref module. */

#ifdef FEATURE_DDTM_CNTL
#include "hdrlog.h"
#endif /* FEATURE_DDTM_CNTL */

#if defined(FEATURE_IP_CALL)
#include "cmipcall.h" /* Interface to IP report handling function */
#include "cmipappi.h" /* Interface to CM IP APP functions */
#if defined(FEATURE_SMS_OVER_IP)
#include "cmipsms.h"  /* Interface to CM IP SMS functions */
#endif
#endif

#if defined (FEATURE_APPS_POWER_COLLAPSE)
#error code not present
#endif

/**--------------------------------------------------------------------------
** Datatypes
** --------------------------------------------------------------------------
*/

/* timer for checking/updating RSSI
*/
rex_timer_type  cm_rssi_timer;

/* Timer for kicking the watchdog.  Bad doggie!
*/
static rex_timer_type  cm_rpt_timer;

/* timer for updating CM
*/
static rex_timer_type  cm_timer;

#ifdef FEATURE_GPSONE_NEW_L2_ACK
#error code not present
#endif /* FEATURE_GPSONE_NEW_L2_ACK */


/*---------------------------------------------------------------------------
                                 CM QUEUES
---------------------------------------------------------------------------*/

/* CM cmd queue
*/
static cm_cmd_q_type      cm_client_cmd_q;
    /* Queue to manage clients commands. Clients command are first
    ** being queued in this queue and then serviced in a FIFO order */

#define CM_MAX_CMDS_PROC_PER_SEC      1000
static dword              cm_last_cmd_proc_timestamp;
static int                cm_cmd_proc_count;

/*-------------------------------------------------------------------------*/

/* CM cmd hold queue
*/
static cm_cmd_q_type           cm_hold_cmd_q;
/*-------------------------------------------------------------------------*/

/* Lower layer and system determination reports queue
*/
cm_cmd_q_type      cm_rpt_q;
    /* Queue to manage clients commands. Clients command are first
    ** being queued in this queue and then serviced in a FIFO order */

/*-------------------------------------------------------------------------*/

/* Origination Parameters Queue
*/

/* Main Instance
*/

#ifdef FEATURE_CM_LOW_MEM_TARGET
#define CM_PH_ORIG_PARA_MAIN_POOL_SIZE 7
#else
#define CM_PH_ORIG_PARA_MAIN_POOL_SIZE 15
#endif

static cm_orig_q_s_type cm_ph_orig_main_arr[ CM_PH_ORIG_PARA_MAIN_POOL_SIZE ];

static cm_cmd_q_type cm_ph_orig_main_free_q;
static cm_cmd_q_type cm_ph_orig_main_q;

/* HDR Instance
*/

#ifdef FEATURE_CM_LOW_MEM_TARGET
#define CM_PH_ORIG_PARA_HDR_POOL_SIZE 3
#else
#define CM_PH_ORIG_PARA_HDR_POOL_SIZE 10
#endif

static cm_orig_q_s_type cm_ph_orig_hdr_arr[ CM_PH_ORIG_PARA_HDR_POOL_SIZE ];

static cm_cmd_q_type cm_ph_orig_hdr_free_q;
static cm_cmd_q_type cm_ph_orig_hdr_q;

/* WLAN Instance
*/

#ifdef FEATURE_CM_LOW_MEM_TARGET
#define CM_PH_ORIG_PARA_WLAN_POOL_SIZE 3
#else
#define CM_PH_ORIG_PARA_WLAN_POOL_SIZE 10
#endif


static cm_orig_q_s_type cm_ph_orig_wlan_arr[ CM_PH_ORIG_PARA_WLAN_POOL_SIZE ];

static cm_cmd_q_type cm_ph_orig_wlan_free_q;
static cm_cmd_q_type cm_ph_orig_wlan_q;

/*-------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------
                                 CM HEAP
---------------------------------------------------------------------------*/

/*
** This union is of the largest objects for which memory is dynamically
** allocated in the code. The SOLE purpose of the datatype is to obtain
** the size of the largest object for the CM_SIZE_OF_LARGEST_DYN_OBJ macro.
** It is not used anywhere else in the code.
*/
/*lint -esym(552, cm_large_obj_unused) disable not accessed error */
/*lint -esym(551, cm_large_obj_unused) disable not accessed error */
/*lint -esym(728, cm_large_obj_unused) disable not accessed error */
typedef union
{
  cmcall_s_type     size_cal_dummy1;

  cm_cmd_s_type     size_cal_dummy2;

  #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
  cm_mc_rpt_type    size_cal_dummy3;
  #endif

  #if (defined( FEATURE_GSM ))
#error code not present
  #endif

} cm_large_obj_unused;

#define   CM_SIZE_OF_LARGEST_DYN_OBJ  ((unsigned long) \
                                          sizeof(cm_large_obj_unused))

/* The exact number of bytes that will be used to satisfy a malloc request
** for the largest object in CM.  As of not, the largest object is
** cmcall_s_type.
*/
#define   CM_MAX_ACTUAL_ALLOC_SIZE   ((( CM_SIZE_OF_LARGEST_DYN_OBJ \
                                         + sizeof(mem_block_header_type) \
                                         + kMinChunkSize - 1 ) / kMinChunkSize ) \
                                      * kMinChunkSize )

/* No. of fixed size requests that must be satisfied by the CM heap.
*/
#ifdef FEATURE_CM_LOW_MEM_TARGET
#define   CM_HEAP_SLOT_COUNT         3
#else
#define   CM_HEAP_SLOT_COUNT         6
#endif

/* The additional 15 bytes is to take into account the paragraph boundary
** alignment done by mem_init_heap().
*/
#define   CM_HEAP_BUFFER_SIZE        ((CM_MAX_ACTUAL_ALLOC_SIZE * \
                                      CM_HEAP_SLOT_COUNT) + 15)

static mem_heap_type     cm_heap;
static uint8 cm_heap_mem_buffer[CM_HEAP_BUFFER_SIZE];

/* No. of free slots in the CM heap.
*/
/*lint -esym(765, cm_heap_slots_left)
** Accessed by unit test cases.
*/
/*lint -esym(552, cm_heap_slots_left)
** Doesn't consider CM_MSG_HIGH as an access.
*/
uint8 cm_heap_slots_left;

/* No. of blocks currently allocated from the TMC heap.
*/
/*lint -esym(765, cm_tmc_heap_blocks)
** Accessed by unit test cases.
*/
/*lint -esym(552, cm_tmc_heap_blocks)
** Doesn't consider CM_MSG_HIGH as an access.
*/
uint8 cm_tmc_heap_blocks;



/*===========================================================================
=============================================================================
============================= MISCELLANEOUS BLOCK ===========================
================================= FUNCTIONS =================================
=============================================================================
===========================================================================*/



/*===========================================================================

FUNCTION cm_wait

DESCRIPTION
  Suspend CM from executing until at least one of the specified signals
  in the input mask is set.

  Note that this function keep reporting to the watchdog while waiting
  for the signal(s) to be set.

DEPENDENCIES
  cm_rpt_timer must have been defined with rex_def_timer().

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
void cm_wait(

    rex_sigs_type    mask
        /* Mask of signals to wait on */
)
{

  rex_sigs_type    sigs;
      /* Signals returned by rex_wait */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Loop while waiting for one of the specified signals
  ** to be set.
  */
  for( ;; )
  {
    /* Wait on specified signals, as well as,
    ** on the watchdog signal.
    */
    sigs = rex_wait( mask | CM_RPT_TIMER_SIG );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If watchdog signal is set, report back to the watchdog
    */
    if( sigs & CM_RPT_TIMER_SIG )
    {
      cm_kick_dog();
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If one or more of the specified signals is set
    ** return now.
    */
    if( sigs & mask )
    {
      return;
    }


  } /* for */

} /* cm_wait() */





/*===========================================================================
=============================================================================
================================= CM QUEUES =================================
=============================================================================
===========================================================================*/

/*
** There are three(3) queue-pairs that are maintained by CM. Typically,
** one queue holds the commands sent to CM and the other holds free buffers.
**
** 1. CM_CLIENT_CMD_Q <--> CM_FREE_CMD_Q      (From CM Clients(UI,DS,etc.))
** 2. CM_HOLD_Q       <--> CM_HOLD_CMD_FREE_Q (Temp holding of cmd for OTAPA)
**
** 3. CM_RPT_Q        <--> CM_MC_RPT_FREE_Q   (From Lower Layers + SD)
*/



/*---------------------------------------------------------------------------
                            CLIENT COMMAND QUEUE
---------------------------------------------------------------------------*/


/*===========================================================================

FUNCTION cm_cmd_q_init

DESCRIPTION
  Initialize a specified CM command queue.

  This function must be called on a CM command queue object before
  it is being used, in any way, place, or form.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cm_cmd_q_init(

    cm_cmd_q_type *q_ptr
        /* pointer to a command queue to be initialized */
)
{

  CM_ASSERT( q_ptr != NULL );
  CM_FUNC_START( "cm_cmd_q_init(), q_address=%p",q_ptr,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the specified command queue.
  */
  (void) q_init( q_ptr );

}




/*===========================================================================

FUNCTION cm_cmd_q_put

DESCRIPTION
  Enqueue a CM command buffer onto a specified CM queue.

DEPENDENCIES
  Specified queue must have already been initialized with
  cm_cmd_q_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cm_cmd_q_put(

    cm_cmd_q_type    *q_ptr,
        /* pointer to a command queue to enqueue command buffer onto */

    cm_cmd_s_type    *cmd_ptr
        /* pointer to a command buffer to be enqueued */
)
{

  CM_ASSERT( q_ptr != NULL );
  CM_ASSERT( cmd_ptr != NULL );
  CM_FUNC_START( "cm_cmd_q_put(), cmd_type=%d, q_address=%p",
                 cmd_ptr->cmd_type, q_ptr, 0 );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Enqueue the command buffer
  ** into the specified queue.
  */
  (void) q_link( cmd_ptr, &cmd_ptr->link);
  q_put( q_ptr, &cmd_ptr->link );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_LOW( "cm_cmd_q_put(), count=%d",q_cnt(q_ptr),0,0 );
}

/*===========================================================================

FUNCTION cm_cmd_q_get

DESCRIPTION
  Dequeue a CM command buffer from a specified CM queue.

DEPENDENCIES
  Specified queue must have already been initialized with
  cm_cmd_q_init().

RETURN VALUE
  A pointer to the dequeued command buffer.
  If Q is empty A NULL is returned.

SIDE EFFECTS
  none

===========================================================================*/
static cm_cmd_s_type *cm_cmd_q_get(

  cm_cmd_q_type  *q_ptr
    /* pointer to a command queue to dequeue command buffer from */
)
{

  CM_ASSERT( q_ptr != NULL );
  CM_FUNC_START( "cm_cmd_q_get(), q_add=%p, q_cnt=%d",
                 q_ptr, q_cnt(q_ptr), 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Dequeue command from specified queue
  */
  return (cm_cmd_s_type*) q_get( q_ptr );

}


/*===========================================================================

FUNCTION cm_client_cmd_q_cnt

DESCRIPTION
  Determine the number of commands in the client command queue.

DEPENDENCIES
  Client command queue must have already been initialized with
  cm_client_cmd_q_init().

RETURN VALUE
  Number of commands in the client command queue.

SIDE EFFECTS
  none

===========================================================================*/
static int cm_client_cmd_q_cnt( void )
{

  CM_FUNC_START( "cm_client_cmd_q_cnt()",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue the command in the CM client command queue
  ** and signal to ourself that we have a new command
  ** in the CM client command queue.
  */
  return q_cnt( &cm_client_cmd_q );

}


/*===========================================================================

FUNCTION cm_client_cmd_q_init

DESCRIPTION
  Initialize the client command queue.

  This function must be called before the client command queue is used in
  any way, place, or form.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cm_client_cmd_q_init( void )
{

  CM_FUNC_START( "cm_client_cmd_q_init()",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cm_cmd_q_init( &cm_client_cmd_q );

  /* Initialize the command processing rate statistic.
  */
  cm_last_cmd_proc_timestamp = 0;
  cm_cmd_proc_count = 0;
}



/*===========================================================================

FUNCTION cm_client_cmd_q_put

DESCRIPTION
  Enqueue a command buffer onto the client command queue.

DEPENDENCIES
  Client command queue must have already been initialized with
  cm_client_cmd_q_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cm_client_cmd_q_put(

    cm_cmd_s_type    *cmd_ptr
        /* pointer to a command buffer to be enqueued */
)
{

  CM_FUNC_START( "cm_client_cmd_q_put(), cmd_type=%d",
                 cmd_ptr->cmd_type,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue the command in the CM client command queue
  ** and signal to ourself that we have a new command
  ** in the CM client command queue.
  */
  cm_cmd_q_put( &cm_client_cmd_q, cmd_ptr );
  (void) rex_set_sigs( &cm_tcb, CM_CLIENT_CMD_Q_SIG );

}


/*===========================================================================

FUNCTION cm_client_cmd_q_get

DESCRIPTION
  Dequeue a command buffer from the client command queue.

DEPENDENCIES
  Client command queue must have already been initialized with
  cm_client_cmd_q_init().

RETURN VALUE
  A pointer to the dequeued command buffer.
  If the Q is empty A NULL is returned.

SIDE EFFECTS
  none

===========================================================================*/
cm_cmd_s_type *cm_client_cmd_q_get( void )
/*lint -esym(765,cm_client_cmd_q_get)
** Can't be static, CM unit test uses it
*/
{

  CM_FUNC_START( "cm_client_cmd_q_get()",0,0,0);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If the CM client command queue is empty, return without doing anything.
  */
  if( cm_client_cmd_q_cnt() < 1 )
  {
    return NULL;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Update the command processing rate statistic each time a command is
  ** taken out of the CM client command queue.
  */
  if( cm_last_cmd_proc_timestamp == clk_uptime() )
  {
    cm_cmd_proc_count++;
  }
  else
  {
    cm_last_cmd_proc_timestamp = clk_uptime();
    cm_cmd_proc_count = 1;
  }

  if( cm_cmd_proc_count > CM_MAX_CMDS_PROC_PER_SEC )
  {
    return NULL;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Get next command from the CM client command queue.
  */
  return cm_cmd_q_get( &cm_client_cmd_q );

}


/*===========================================================================

FUNCTION cm_client_cmd_q_peek
DESCRIPTION
  Look at the next command buffer from the client command queue.

DEPENDENCIES
  Client command queue must have already been initialized with
  cm_client_cmd_q_init().

RETURN VALUE

  True if there were any commands queued + command type and call id
  (if applicable) will be returned via the function parameters

  False if the command queue was empty

SIDE EFFECTS
  None

===========================================================================*/
static cm_cmd_s_type *cm_client_cmd_q_peek
(
  void
)
{
  return ( (cm_cmd_s_type *) q_check( &cm_client_cmd_q ) );

} /* cm_client_cmd_q_peek */



/*===========================================================================

FUNCTION cm_cmd_alloc

DESCRIPTION
  Allocate a CM command buffer from the Heap.

DEPENDENCIES
  Heap must have been initialized.

RETURN VALUE
  A pointer to the allocated command buffer.
  If Heap is empty a NULL is returned.

SIDE EFFECTS
  none

===========================================================================*/
cm_cmd_s_type *cm_cmd_alloc_debug(

  #ifdef CM_DEBUG
#error code not present
  #endif

  uint32 line
    /* Line number in the file above */
)
{

  cm_cmd_s_type *tmp;
  rex_tcb_type  *temp;  

  /* Allocate memory */
  tmp = (cm_cmd_s_type *)cm_mem_malloc(sizeof(cm_cmd_s_type));
  if(tmp != NULL)
  {
    tmp->alloc_time = clk_uptime();

    /*lint -save -e534 */
    #ifdef CM_DEBUG
#error code not present
    #endif

    tmp->line = line;
    tmp->passed_for_cc = 0;

    temp = rex_self();
    if(temp==NULL)
    {
      CM_MSG_HIGH( "temp_NULL",0,0,0);
    }
    else
    {
      (void)std_strlcpy(tmp->task, temp->task_name, ARR_SIZE(tmp->task) );
      tmp->task[ARR_SIZE(tmp->task)-1] = '\0';
      /*lint -restore */

      cmdbg_print_message("=CM= CMD alloc u=%d, tsk=%s", tmp->alloc_time, tmp->task, 0);
    }   
  }

  return tmp;

} /* cm_cmd_alloc() */



/*===========================================================================

FUNCTION cm_cmd_dealloc

DESCRIPTION
  Deallocate a CM command buffer by returning it to the
  heap.

DEPENDENCIES
  Heap must have already been initialized.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_cmd_dealloc( void  *ptr )
{
  /*
  ** Can't change void *ptr to const void *ptr as then at the very bottom
  ** of call stack I'd have to change mem_free() to take const void *ptr
  ** and it's not the right change as memheap.c might decide to change that
  ** memory (like clear it) before freeing it
  */

  cm_cmd_s_type *tmp = (cm_cmd_s_type*) ptr;
  if(tmp != NULL)
  {
    cmdbg_print_message("=CM= CMD free u=%d, tsk=%s, ftsk=%s", tmp->alloc_time, tmp->task, rex_self()->task_name);
  }

  cm_mem_free(ptr);

} /* cm_cmd_dealloc() */



/*===========================================================================

FUNCTION cm_mem_malloc

DESCRIPTION
  Allocate a chunk of Memory from the Heap. If CM heap is exhausted,
  then it tries to get memory from the TMC Heap.

DEPENDENCIES

  Heap is already initialized.

RETURN VALUE

  Pointer to the memory block.

SIDE EFFECTS
  none

===========================================================================*/
void *cm_mem_malloc_debug(

  size_t size,

  char *objname
    /* Used to see the name of the object being freed if "sizeof" operator is
    ** used.
    */
)
{
  void *ptr;
  cm_large_obj_unused *unused_ptr = NULL;

  /* To clear the compiler warning caused by 
  ** cm_large_obj_unused
  */
  SYS_ARG_NOT_USED(unused_ptr);

  /*
  ** Lets first allocate memory from CM heap. If it fails,
  ** then we go to TMC Heap.
  ** If size requested is larger than CM_SIZE_OF_LARGEST_DYN_OBJ then
  ** allocate from TMC Heap
  */

  if( size > CM_SIZE_OF_LARGEST_DYN_OBJ ||
     (ptr = mem_malloc(&cm_heap, (size_t) CM_SIZE_OF_LARGEST_DYN_OBJ)) == NULL)
  {
    CM_ERR("CM Heap full or size requested (%d) exceeds CM_SIZE_OF_LARGEST_DYN_OBJ %d"
           " requesting from TMC Heap",
           size,CM_SIZE_OF_LARGEST_DYN_OBJ,0);

    ptr =  mem_malloc(&tmc_heap,size);
    if(ptr != NULL)
    {
      cm_tmc_heap_blocks++;
    }
  }
  else
  {
    cm_heap_slots_left--;
  }
  CM_MSG_HIGH("CM malloc: CM Left, TMC Used = %d, %d", cm_heap_slots_left, cm_tmc_heap_blocks, 0);

  cmdbg_print_message("=CM= Malloc: obj: %s, addr: %x",objname, ptr);

  return ptr;

} /* cm_mem_malloc */



/*===========================================================================

FUNCTION cm_mem_free

DESCRIPTION
   This function returuns the specified memory back to the Heap.

   If the memory came from CM heap then it is returned their else
   it is returned to the TMC heap.

DEPENDENCIES

   CM and TMC heap must have been initialized.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_mem_free_debug(

  void *ptr,

  char *objname
    /* Name of object being freed. */
)
{
  /*
  ** Can't change void *ptr to const void *ptr as then at the very bottom
  ** of call stack I'd have to change mem_free() to take const void *ptr
  ** and it's not the right change as memheap.c might decide to change that
  ** memory (like clear it) before freeing it
  */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  cmdbg_print_message("Free: objname: %s, addr: %x", objname, ptr);

  /*
  ** If we allocated memory from CM Heap, then return there
  ** else we need to return back to the TMC heap.
  */

  /*lint -e{826} keep it suppressed for general / specific walks as well */
  if( (ptr >= (void *)(cm_heap_mem_buffer) &&
      (ptr <  (void *)(cm_heap_mem_buffer + CM_HEAP_BUFFER_SIZE)))
    )
  {
    cm_heap_slots_left++;
    mem_free( &(cm_heap),ptr );
  }
  else
  {
    CM_ASSERT( ptr >= (void *)(tmc_heap_mem_buffer) &&
               ptr <  (void *)(tmc_heap_mem_buffer + TMC_HEAP_MEM_BUFFER_SIZE)
             );
    cm_tmc_heap_blocks--;
    mem_free( &(tmc_heap),ptr);
  }

  CM_MSG_HIGH("CM free: CM Left, TMC Used = %d, %d", cm_heap_slots_left, cm_tmc_heap_blocks, 0);
} /* cm_mem_free() */



/*===========================================================================

FUNCTION cm_cmd_queue

DESCRIPTION
  Queue a CM command in its appropriate CM command queue.

DEPENDENCIES
  All CM command queues must have already been initialized.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_cmd_queue( cm_cmd_s_type *cmd_ptr )
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(cmd_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch( CMD_TYPE(cmd_ptr) )
  {
    case CM_CMD_TYPE_PH:
    case CM_CMD_TYPE_CALL:
    case CM_CMD_TYPE_INBAND:
    case CM_CMD_TYPE_SS:
    case CM_CMD_TYPE_SMS:

    #ifdef FEATURE_GPSONE
#error code not present
    #endif  /* FEATURE_GPSONE */

    #ifdef FEATURE_DBM
    case CM_CMD_TYPE_DBM:
    #endif  /* FEATURE_DBM */

    #if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
#error code not present
    #endif /* (FEATURE_GSM) || (FEATURE_WCDMA) */

    #if (defined FEATURE_BCMCS || defined FEATURE_MODEM_MBMS)
    case CM_CMD_TYPE_BCMCS:
    case CM_CMD_TYPE_MBMS: 
    #endif /* FEATURE_BCMCS || FEATURE_MODEM_MBMS */

    /* Queue clients commands in the
    ** client commands queue.
    */
    cm_client_cmd_q_put( cmd_ptr );
      break;

    default:
      CM_SWITCH_ERR( "Command type = %d",CMD_TYPE(cmd_ptr),0,0 );
      break;
  } /* switch( CMD_TYPE(cmd_ptr) ) */
} /* cm_cmd_queue() */


/*---------------------------------------------------------------------------
                             HOLD COMMAND QUEUE
---------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION cm_hold_cmd_q_init

DESCRIPTION
  Initialize the hold command queue.

  This function must be called before the hold command free queue is used.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_hold_cmd_q_init( void )
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_FUNC_START( "cm_hold_cmd_q_init()", 0, 0, 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the free cmclient queue.
  */
  (void) q_init( &cm_hold_cmd_q );

}

/*===========================================================================

FUNCTION cm_hold_cmd_q_get

DESCRIPTION
  Dequeue a command from the head of the CM hold command queue.

DEPENDENCIES
  cm_free_cmd_q_init() must have already been called.

RETURN VALUE
  A pointer to the dequeued command buffer.
  If the Q is empty A NULL is returned.

SIDE EFFECTS
  none

===========================================================================*/
cm_cmd_s_type *cm_hold_cmd_q_get( void )
{

  CM_FUNC_START( "cm_hold_cmd_q_get()",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Dequeue a command buffer
  ** from the free command Q.
  */
  return cm_cmd_q_get( &cm_hold_cmd_q );

}

/*===========================================================================

FUNCTION cm_hold_cmd_q_put

DESCRIPTION
  Enqueue a command buffer onto the CM hold command queue.

DEPENDENCIES
  cm_free_cmd_q_init() must have already been called.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_hold_cmd_q_put(

    cm_cmd_s_type *cmd_ptr
        /* pointer to a command buffer to be enqueued */
)
{

  CM_ASSERT( cmd_ptr != NULL );
  CM_FUNC_START( "cm_hold_cmd_q_put(), cmd_type=%d",cmd_ptr->cmd_type,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Enqueue the command buffer
  ** onto the CM free command Q.
  */
  cm_cmd_q_put( &cm_hold_cmd_q, cmd_ptr );

}

/*===========================================================================

FUNCTION cm_hold_cmd_q_exists_cmd

DESCRIPTION
  Check if the command type and the command are already in the hold q.
  If the NULL is passed as the command, then just the type is checked

DEPENDENCIES
  cm_hold_cmd_q_init() must have already been called.

RETURN VALUE
  TRUE if cmd type and cmd exists
  FALSE if not.

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_hold_cmd_q_exists_cmd(

  cm_cmd_type_e_type cmd_type,
    /* The type of the command
    */

  const void *cmd
    /* The command
    */
)
{
  cm_cmd_s_type   *hold_cmd_ptr = (cm_cmd_s_type *)NULL;
      /* CM command pointer */

  int             i;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_FUNC_START( "cm_hold_cmd_q_exists_cmd(), cmd_type=%d",cmd_type,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (i = 0, hold_cmd_ptr = (cm_cmd_s_type *)q_check(&cm_hold_cmd_q);
       i < q_cnt(&cm_hold_cmd_q);
       i++, hold_cmd_ptr = (cm_cmd_s_type *)q_next(&cm_hold_cmd_q, &hold_cmd_ptr->link)
      )
  {
    if(hold_cmd_ptr==NULL)
    {
        CM_MSG_HIGH( "hold_cmd_ptr_NULL",0,0,0);
        break;
    }
    /* Only same command types can be compared. */
    if ( CMD_TYPE(hold_cmd_ptr) == cmd_type )
    {
      /* If no command is given, return TRUE
      */

      if( cmd == NULL )
      {
        return TRUE;
      }

      /* An command is given, check it with the command on the hold Q
      */
      switch (CMD_TYPE(hold_cmd_ptr))
      {
        case CM_CMD_TYPE_CALL:
          if (CALL_CMD_PTR(hold_cmd_ptr)->cmd == *(cm_call_cmd_e_type*)cmd)
          {
            return TRUE;
          }
          break;

        case CM_CMD_TYPE_PH:
          if (PH_CMD_PTR(hold_cmd_ptr)->cmd ==  *(cm_ph_cmd_e_type*)cmd)
          {
            return TRUE;
          }
          break;

        default:
          break;
      }
    } /* if */
  }

  return FALSE;

} /* cm_hold_cmd_q_exists_cmd */


/*===========================================================================

FUNCTION cm_hold_cmd_q_check

DESCRIPTION
  Check if the passed in command matches with the items in hold command q.
  It only checks the cmd type and cmd. It does not check for the contents
  of command, for example, the dialed digits for orig cmd.

DEPENDENCIES
  cm_hold_cmd_q_init() must have already been called.

RETURN VALUE
  TRUE if cmd type and cmd are same.
  FALSE if not.

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_hold_cmd_q_check(

  const cm_cmd_s_type *cmd_ptr

)
{

  cm_cmd_s_type   *hold_cmd_ptr = (cm_cmd_s_type *)NULL;
      /* CM command pointer */

  int             i;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmd_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_FUNC_START( "cm_hold_cmd_q_check(), cmd_type=%d",cmd_ptr->cmd_type,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If cmd_ptr is NULL, this is a invalid function call, return FALSE
  */

  if( cmd_ptr == NULL ) /*lint !e774 */
  {
    return FALSE;
  }

  for (i = 0, hold_cmd_ptr = (cm_cmd_s_type *)q_check(&cm_hold_cmd_q);
       i < q_cnt(&cm_hold_cmd_q);
       i++, hold_cmd_ptr = (cm_cmd_s_type *)q_next(&cm_hold_cmd_q, &hold_cmd_ptr->link)
      )
  {
    if(hold_cmd_ptr==NULL)
    {
        CM_MSG_HIGH( "hold_cmd_ptr_NULL",0,0,0);
        break;
    }

    /* Only same command types can be compared. */
    if ( CMD_TYPE(cmd_ptr) == CMD_TYPE(hold_cmd_ptr) )
    {
      switch (CMD_TYPE(hold_cmd_ptr))
      {
        case CM_CMD_TYPE_CALL:
          if (CALL_CMD_PTR(cmd_ptr)->cmd == CALL_CMD_PTR(hold_cmd_ptr)->cmd)
          {
            CM_MSG_HIGH(" Hold Q existing cmd_type:%d cmd:%d",
                          CM_CMD_TYPE_CALL, CALL_CMD_PTR(cmd_ptr)->cmd, 0);
            return TRUE;
          }
          break;

        case CM_CMD_TYPE_PH:
          if (PH_CMD_PTR(cmd_ptr)->cmd == PH_CMD_PTR(hold_cmd_ptr)->cmd)
          {
            CM_MSG_HIGH(" Hold Q existing cmd_type:%d cmd:%d",
                          CM_CMD_TYPE_PH, PH_CMD_PTR(cmd_ptr)->cmd, 0);
            return TRUE;
          }
          break;

        default:
          break;
      }
    }
  }

  return FALSE;
}


/*===========================================================================

FUNCTION cm_hold_cmd_q_add

DESCRIPTION
  Adds a command to a hold queue

DEPENDENCIES
  None

RETURN VALUE
None


SIDE EFFECTS
  none

===========================================================================*/
void cm_hold_cmd_q_add
(
   const cm_cmd_s_type *cmd_ptr
)
{
  cm_cmd_s_type            *hold_cmd_ptr;

  CM_ASSERT( cmd_ptr != NULL );

  /* If the command is already in the hold wueue
  ** don't add another command
  */
  if( cm_hold_cmd_q_check(cmd_ptr) )
  {
    return;
  }

  if ((hold_cmd_ptr = cm_cmd_alloc()) != NULL)
  {
     hold_cmd_ptr->cmd_type = cmd_ptr->cmd_type;
     memcpy(&hold_cmd_ptr->cmd, &cmd_ptr->cmd, sizeof(cm_cmd_u_type));

     cm_hold_cmd_q_put(hold_cmd_ptr);
   }
   else
   {
     CM_ERR_FATAL("cm_hold_cmd_alloc() failed!!",0,0,0 );
   }
} /* cmtask_add_cmd_to_hold_q */


/*===========================================================================

FUNCTION cm_hold_cmd_q_peek
DESCRIPTION
  Look at the next command buffer from the hold command queue.

DEPENDENCIES
  Hold command queue must have already been initialized with
  cm_hold_cmd_q_init().

RETURN VALUE

  True if there were any commands queued + command type and call id
  (if applicable) will be returned via the function parameters

  False if the command queue was empty

SIDE EFFECTS
  None

===========================================================================*/
static cm_cmd_s_type *cm_hold_cmd_q_peek
(
  void
)
{
  return ( (cm_cmd_s_type *) q_check( &cm_hold_cmd_q ) );

} /* cm_hold_cmd_q_peek */


/*===========================================================================

FUNCTION cm_hold_cmd_q_next

DESCRIPTION
  Return a pointer to next entry in the hold Queue. The element is
  not de-queqed from the Queue

DEPENDENCIES
  cm_hold_cmd_q must have already been initialized with
  cm_hold_cmd_q_init().


RETURN VALUE
  Return a pointer to next buffer in the cm_hold_cmd_q.
  If cm_hold_cmd_q list is empty a NULL is returned.

SIDE EFFECTS
  None.

===========================================================================*/
static cm_cmd_s_type* cm_hold_cmd_q_next(

  cm_cmd_s_type* q_ptr
    /* Pointer to the current q element
    */
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_ASSERT( q_ptr != NULL );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_FUNC_START( "cm_hold_cmd_q_next() ",0,0,0 );

  return ( (cm_cmd_s_type *) q_next( &cm_hold_cmd_q, &q_ptr->link ) );
} /* cm_hold_cmd_q_next */


/*===========================================================================

FUNCTION cm_hold_cmd_q_delete

DESCRIPTION
  Deletes a buffer from the hold Queue
DEPENDENCIES
  cm_hold_cmd_q must have already been initialized with
  cm_hold_cmd_q_init().


RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
static void cm_hold_cmd_q_delete(

  cm_cmd_s_type* q_ptr
    /* Pointer to the current q element
    */
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_ASSERT( q_ptr != NULL );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_FUNC_START( "cm_hold_cmd_q_delete() ",0,0,0 );

  #ifdef FEATURE_Q_NO_SELF_QPTR
  q_delete( &cm_hold_cmd_q, &q_ptr->link );
  #else
  q_delete( &q_ptr->link );
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the pointers link item to make sure its next and prev
  ** pointers are no longer pointing at other clients.
  */
  (void) q_link( q_ptr, &q_ptr->link );

  /* Free the memory allocated to this command
  */
  cm_cmd_dealloc( q_ptr );

} /* cm_hold_cmd_q_delete */


/*---------------------------------------------------------------------------
                            CM-RPT QUEUE
---------------------------------------------------------------------------*/

/* The LL interface block groups functionality that is associated with LL
** interfacing, in both directions.
*/



/*===========================================================================

FUNCTION cm_rpt_q_init

DESCRIPTION
  Initialize the client command queue.

  This function must be called before the client command queue is used in
  any way, place, or form.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cm_rpt_q_init( void )
{

  CM_FUNC_START( "cm_rpt_q_init()",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  (void) q_init( &cm_rpt_q );
}



/*===========================================================================

FUNCTION cm_rpt_q_get

DESCRIPTION
  Dequeue a command buffer from the client command queue.

DEPENDENCIES
  Client command queue must have already been initialized with
  cm_mc_rpt_q_init().

RETURN VALUE
  A pointer to the dequeued command buffer.
  If the Q is empty A NULL is returned.

SIDE EFFECTS
  none

===========================================================================*/
static cm_hdr_type *cm_rpt_q_get( void )
{

  CM_FUNC_START( "cm_rpt_q_get()",0,0,0);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get next command from the CM client command queue.
  */
  return (cm_hdr_type *) q_get( &cm_rpt_q );

}

/*===========================================================================

FUNCTION cm_ph_orig_para_main_q_init

DESCRIPTION
  Initialize the cm_ph_orig_para_main_q queue object.

  This function must be called before the CM_PH_ORIG free command queue
  is used in any way, place, or form.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cm_ph_orig_para_main_q_init( void )
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_FUNC_START( "cm_ph_orig_para_main_q_init(), free commands=%d",
                 ARR_SIZE(cm_ph_orig_main_arr), 0, 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the CM free command queue.
  */

  (void)q_init( &cm_ph_orig_main_q );
  return;
}


/*===========================================================================

FUNCTION cm_ph_orig_para_hdr_q_init

DESCRIPTION
  Initialize the cm_ph_orig_para_hdr_q queue object.

  This function must be called before the CM_PH_ORIG free command queue
  is used in any way, place, or form.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cm_ph_orig_para_hdr_q_init( void )
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_FUNC_START( "cm_ph_orig_para_hdr_q_init(), free commands=%d",
                 ARR_SIZE(cm_ph_orig_hdr_arr), 0, 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the CM free command queue.
  */

  (void)q_init( &cm_ph_orig_hdr_q );
  return;
}


/*===========================================================================

FUNCTION cm_ph_orig_para_wlan_q_init

DESCRIPTION
  Initialize the cm_ph_orig_para_wlan_q queue object.

  This function must be called before the CM_PH_ORIG free command queue
  is used in any way, place, or form.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cm_ph_orig_para_wlan_q_init( void )
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_FUNC_START( "cm_ph_orig_wlan_q_init(), free commands=%d",
                 ARR_SIZE(cm_ph_orig_wlan_arr), 0, 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the CM free command queue.
  */

  (void)q_init( &cm_ph_orig_wlan_q );
  return;
}


/*===========================================================================

FUNCTION cm_ph_orig_para_main_free_q_init

DESCRIPTION
  Initialize the cm_ph_orig_para_main_free_q queue object.

  This function must be called before the CM_PH_ORIG free command queue
  is used in any way, place, or form.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cm_orig_para_main_free_q_init( void )
{
  unsigned int i;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_FUNC_START( "cm_orig_para_main_free_q_init(), free commands=%d",
                 ARR_SIZE(cm_ph_orig_main_arr), 0, 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the CM orig para Q.
  */

  (void)q_init( &cm_ph_orig_main_free_q );

  /* Link and place each of the orig para buffers
  ** into the free orig para queue.
  */
  for ( i=0; i < ARR_SIZE(cm_ph_orig_main_arr); i++ )
  {
    (void) q_link( &cm_ph_orig_main_arr[i], &cm_ph_orig_main_arr[i].link );
    q_put( &cm_ph_orig_main_free_q, &cm_ph_orig_main_arr[i].link );
  }

}


/*===========================================================================

FUNCTION cm_orig_para_hdr_free_q_init

DESCRIPTION
  Initialize the cm_ph_orig_hdr_free_q queue object.

  This function must be called before the CM_PH_ORIG free command queue
  is used in any way, place, or form.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cm_orig_para_hdr_free_q_init( void )
{
  unsigned int i;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_FUNC_START( "cm_orig_para_hdr_free_q_init(), free commands=%d",
                 ARR_SIZE(cm_ph_orig_hdr_arr), 0, 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the CM orig para Q.
  */

  (void)q_init( &cm_ph_orig_hdr_free_q );

  /* Link and place each of the orig para buffers
  ** into the free orig para queue.
  */
  for ( i=0; i < ARR_SIZE(cm_ph_orig_hdr_arr); i++ )
  {
    (void) q_link( &cm_ph_orig_hdr_arr[i], &cm_ph_orig_hdr_arr[i].link );
    q_put( &cm_ph_orig_hdr_free_q, &cm_ph_orig_hdr_arr[i].link );
  }

}


/*===========================================================================

FUNCTION cm_orig_para_wlan_free_q_init

DESCRIPTION
  Initialize the cm_ph_orig_wlan_free_q queue object.

  This function must be called before the CM_PH_ORIG free command queue
  is used in any way, place, or form.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cm_orig_para_wlan_free_q_init( void )
{
  unsigned int i;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_FUNC_START( "cm_orig_para_wlan_free_q_init(), free commands=%d",
                 ARR_SIZE(cm_ph_orig_wlan_arr), 0, 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the CM orig para Q.
  */

  (void)q_init( &cm_ph_orig_wlan_free_q );

  /* Link and place each of the orig para buffers
  ** into the free orig para queue.
  */
  for ( i=0; i < ARR_SIZE(cm_ph_orig_wlan_arr); i++ )
  {
    (void) q_link( &cm_ph_orig_wlan_arr[i], &cm_ph_orig_wlan_arr[i].link );
    q_put( &cm_ph_orig_wlan_free_q, &cm_ph_orig_wlan_arr[i].link );
  }

}


/*===========================================================================

FUNCTION cmtask_orig_para_init

DESCRIPTION
  Initialize the buffer to default values.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void cmtask_orig_para_init(
  cm_orig_q_s_type *orig_ptr
    /* The buffer to initialize
    */
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(orig_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  orig_ptr->act_type                  = CM_ACT_TYPE_NONE;
  cmtask_orig_para_init_2( &orig_ptr->orig );
  return;

}


/*===========================================================================

FUNCTION cmtask_orig_para_init_2

DESCRIPTION
  Initialize the buffer to default values.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void cmtask_orig_para_init_2(
  cm_act_orig_s_type *orig_ptr
    /* The buffer to initialize
    */
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(orig_ptr           != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  orig_ptr->act_id               = CM_ACT_ID_NONE;
  orig_ptr->act_priority         = CM_ACT_PRIORITY_NONE;
  orig_ptr->orig_band_pref       = CM_BAND_PREF_NONE;
  orig_ptr->orig_hybr_pref       = CM_HYBR_PREF_NONE;
  orig_ptr->orig_int_hybr_pref   = SD_SS_HYBR_PREF_NONE;
  orig_ptr->orig_mode            = SD_SS_ORIG_MODE_NONE;
  orig_ptr->orig_mode_pref       = CM_MODE_PREF_NONE;
  orig_ptr->orig_mode_uptime     = CM_PH_ORIG_MODE_UPTIME_NONE;
  orig_ptr->orig_pref_term       = CM_PREF_TERM_NONE;
  orig_ptr->orig_prl_pref        = CM_PRL_PREF_NONE;
  orig_ptr->orig_roam_pref       = CM_ROAM_PREF_NONE;
  orig_ptr->orig_srv_domain_pref = CM_SRV_DOMAIN_PREF_NONE;

} /* cmtask_orig_para_init_2 */

/*===========================================================================

FUNCTION cmtask_orig_para_get

DESCRIPTION
  Dequeue a CM orig para buffer from a specified CM queue.

DEPENDENCIES
  Specified queue must have already been initialized with
  cm_ph_orig_xxx_q_init().

RETURN VALUE
  A pointer to the dequeued command buffer.
  If Q is empty A NULL is returned.

SIDE EFFECTS
  none

===========================================================================*/
static cm_orig_q_s_type *cmtask_orig_para_get(

  cm_cmd_q_type  *q_ptr
    /* pointer to a orig queue to dequeue command buffer from */
)
{

  CM_ASSERT( q_ptr != NULL );
  CM_FUNC_START( "cmtask_orig_para_get(), q_add=%p, q_cnt=%d",
                 q_ptr, q_cnt(q_ptr), 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Dequeue command from specified queue
  */
  return (cm_orig_q_s_type*) q_get( q_ptr );

} /* cmtask_orig_para_get */

/*===========================================================================

FUNCTION cmtask_orig_para_put_2

DESCRIPTION
  Enqueue a CM orig para buffer to a specified CM queue.

DEPENDENCIES
  Specified queue must have already been initialized with
  cm_ph_orig_xxx_q_init().

RETURN VALUE
 None

SIDE EFFECTS
  none

===========================================================================*/
static void cmtask_orig_para_put_2(

  cm_cmd_q_type    *q_ptr,
    /* pointer to a command queue to put the buffer to */

  cm_orig_q_s_type *buf
    /* The buffer to add to the Q */
)
{

  CM_ASSERT( q_ptr != NULL );
  CM_ASSERT( buf   != NULL );
  CM_FUNC_START( "cm_ph_orig_q_put(), q_add=%p, q_cnt=%d",
                 q_ptr, q_cnt(q_ptr), 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Enqueue the command buffer
  ** into the specified queue.
  */
  (void) q_link( buf, &buf->link);
  q_put( q_ptr, &buf->link );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_LOW( "cm_ph_orig_q_put(), count=%d",q_cnt(q_ptr),0,0 );

}/* cmtask_orig_para_put_2 */


/*===========================================================================

FUNCTION cmtask_orig_para_delloc

DESCRIPTION
  Dellocate a CM orig buffer, and put it back on the free Q.

DEPENDENCIES
  CM free command queue must have already been initialized.

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
void cmtask_orig_para_delloc (
  cm_ss_e_type ss,
    /* The queue to deallocate from
    */
  cm_orig_q_s_type *q_ptr
    /* The pointer to the buffer
    */
)
{
  /* Initialize the buffer to default values
  */
  cmtask_orig_para_init(q_ptr);

  /* Put the buffer back on the free Q
  */

  switch( ss )
  {
    case CM_SS_MAIN:
      cmtask_orig_para_put_2( &cm_ph_orig_main_free_q, q_ptr );
      break;

    case CM_SS_HDR:
      cmtask_orig_para_put_2( &cm_ph_orig_hdr_free_q, q_ptr );
      break;

    case CM_SS_WLAN:
      cmtask_orig_para_put_2( &cm_ph_orig_wlan_free_q, q_ptr );
      break;

    default:
      CM_CASE_ERR(" Invalid ss %d",ss,0,0 );
      /*lint -save -e527 Unreachable
      **     CM_CASE_ERR exits when CM_DEBUG is on
      */
      CM_MSG_LOW("cmtask_orig_para_delloc() on ss %d", ss, 0, 0);
      /*lint -restore */
      break;
  } /* switch( ss ) */

  return;

}/* cm_ph_orig_dealloc */



/*===========================================================================

FUNCTION cmtask_orig_para_alloc

DESCRIPTION
  Allocate a CM command buffer from the CM free hold command queue.

DEPENDENCIES
  CM free command queue must have already been initialized.

RETURN VALUE
  A pointer to the allocated command buffer.
  If the Q is empty A NULL is returned.

SIDE EFFECTS
  none

===========================================================================*/
cm_orig_q_s_type *cmtask_orig_para_alloc( cm_ss_e_type ss )
{
  switch( ss )
  {
    case CM_SS_MAIN:
      return cmtask_orig_para_get( &cm_ph_orig_main_free_q );

    case CM_SS_HDR:
      return cmtask_orig_para_get( &cm_ph_orig_hdr_free_q );

    case CM_SS_WLAN:
      return cmtask_orig_para_get( &cm_ph_orig_wlan_free_q );

    default:
      CM_CASE_ERR(" Invalid ss %d", ss,0,0);
      /*lint -save -e527 Unreachable
      **     CM_CASE_ERR exits when CM_DEBUG is on
      */
      CM_MSG_LOW("cmtask_orig_para_alloc() on ss %d", ss, 0, 0);
      return NULL;
      /*lint -restore */
  } /* switch( ss ) */

}/* cmtask_orig_para_alloc */


/*===========================================================================

FUNCTION cmtask_orig_para_compare_func

DESCRIPTION
  Get the position in the queue to insert

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  TRUE: Insert position found
  FALSE: Insert position not found
  The Q function requires a return type of int so the return type is NOT
  boolean

SIDE EFFECTS
  None.

===========================================================================*/


static int cmtask_orig_para_compare_func(
  void * item_ptr, void * compare_val
)
{

  cm_orig_q_s_type *q_orig_ptr  = NULL;
  cm_orig_q_s_type *ph_orig_ptr = NULL;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(item_ptr    != NULL);
  CM_ASSERT(compare_val != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  q_orig_ptr  = (cm_orig_q_s_type *) item_ptr;
  ph_orig_ptr = (cm_orig_q_s_type *) compare_val;

  if( q_orig_ptr->orig.act_priority > ph_orig_ptr->orig.act_priority )
  {
    return( TRUE );
  }
  return( FALSE );
}/* cm_ph_orig_compare_func */


/*===========================================================================

FUNCTION cm_ph_orig_search_func

DESCRIPTION
  Search function for searching the queue for the act_id

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  TRUE: act_id found
  FALSE: act_id not found
  The Q function requires a return type of int so te return type is NOT
  boolean

SIDE EFFECTS
  None.

===========================================================================*/

static int cm_ph_orig_search_func(
  void * item_ptr, void * compare_val
)
{

  cm_orig_q_s_type *q_orig_ptr  = NULL;
  cm_act_id_type   *compare_ptr = NULL;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(item_ptr    != NULL);
  CM_ASSERT(compare_val != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  q_orig_ptr  = (cm_orig_q_s_type *) item_ptr;
  compare_ptr = (cm_act_id_type *) compare_val;

  if( q_orig_ptr->orig.act_id == *compare_ptr )
  {
    return( TRUE );
  }

  return( FALSE );

}/* cm_ph_orig_search_func */



/*===========================================================================

FUNCTION cmtask_orig_para_search_act_id_2

DESCRIPTION
  Searches the Origination Queues for the act_id

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  The pointer to the Queue element corresponding to act_id, if found
  NULL otherwise

SIDE EFFECTS
  None.

===========================================================================*/

static cm_orig_q_s_type* cmtask_orig_para_search_act_id_2(

  cm_cmd_q_type* q_ptr,
    /* The Queue on which the item is to be searched
    */
  cm_act_id_type act_id
    /* The id of the activity to be searched
    */
)
{
  cm_orig_q_s_type *q_search_pos;

  q_search_pos = q_linear_search( q_ptr, cm_ph_orig_search_func, &act_id );

  return( q_search_pos );


}/* cmtask_orig_para_search_act_id_2 */


/*===========================================================================

FUNCTION cmtask_orig_para_search_act_id

DESCRIPTION
  Searches the orig queue for the act_id

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  Pointer to the element, if found
  NULL otherwise

SIDE EFFECTS
  None.

===========================================================================*/

cm_orig_q_s_type* cmtask_orig_para_search_act_id(

  cm_ss_e_type ss,
    /* The ss, used to select a queue
    */
  cm_act_id_type act_id
    /* The id of the activity to be searched
    */
)
{

  CM_MSG_LOW( "cmtask_orig_para_search_act_id %d ss %d", act_id, ss, 0 );

  switch(ss)
  {
    case CM_SS_MAIN :
      /* Search on Main
      */
      return cmtask_orig_para_search_act_id_2( &cm_ph_orig_main_q, act_id );

    case CM_SS_HDR:
      /* Search on HDR
      */
      return cmtask_orig_para_search_act_id_2( &cm_ph_orig_hdr_q, act_id );


    case CM_SS_WLAN:
      /* Search on WLAN
      */
      return cmtask_orig_para_search_act_id_2( &cm_ph_orig_wlan_q, act_id );

    default:
      CM_CASE_ERR(" ss %d", ss,0,0);
      /*lint -save -e527 Unreachable
      **     CM_CASE_ERR exits when CM_DEBUG is on
      */
      CM_MSG_LOW("cmtask_orig_para_search_act_id() on ss %d", ss, 0, 0);
      return NULL;
      /*lint -restore */

  }

}/* cmtask_orig_para_search_act_id */

/*===========================================================================

FUNCTION cmtask_orig_search_func_act_id_act_priority

DESCRIPTION
  Search the queue for the act_id

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  TRUE: act_id and act_priority found
  FALSE: act_id and act_priority not found
  The Q function requires a return type of int so te return type is NOT
  boolean


SIDE EFFECTS
  None.

===========================================================================*/


static int cmtask_orig_search_func_act_id_act_priority(
  void * item_ptr, void * compare_val
)
{

  cm_orig_q_s_type   *q_orig_ptr  = NULL;
  cm_orig_q_s_type   *compare_ptr = NULL;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(item_ptr    != NULL);
  CM_ASSERT(compare_val != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  q_orig_ptr  = (cm_orig_q_s_type *) item_ptr;
  compare_ptr = (cm_orig_q_s_type *) compare_val;

  if( q_orig_ptr->orig.act_id          == compare_ptr->orig.act_id &&
      q_orig_ptr->orig.act_priority == compare_ptr->orig.act_priority )
  {
    return( TRUE );
  }

  return( FALSE );

}/* cmtask_orig_search_func_act_id_act_priority */

/*===========================================================================

FUNCTION cmtask_orig_para_search_act_id_act_priority_2

DESCRIPTION
  Searches the Origination Queues for the act_id

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  The pointer to the Queue element corresponding to act_type, if found
  NULL otherwise

SIDE EFFECTS
  None.

===========================================================================*/

static cm_orig_q_s_type* cmtask_orig_para_search_act_id_act_priority_2(

  cm_cmd_q_type              *q_ptr,
    /* The Queue on which the item is to be searched
    */

  cm_act_id_type             act_id,
    /* the activity type to be searched
    */

  cm_act_priority_e_type     act_priority
    /* The priority of the activity
    */

)
{
  cm_orig_q_s_type *q_search_pos;
  cm_orig_q_s_type q_temp;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(q_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  cmtask_orig_para_init(&q_temp);

  q_temp.orig.act_id = act_id;
  q_temp.orig.act_priority = act_priority;

  q_search_pos = q_linear_search( q_ptr,
                                  cmtask_orig_search_func_act_id_act_priority,
                                  &q_temp );

  return( q_search_pos );

}/* cmtask_orig_para_search_act_id_act_priority_2 */


/*===========================================================================

FUNCTION cmtask_orig_para_search_act_id_act_priority

DESCRIPTION
  Searches the orig queue for the act_type

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  Pointer to the element, if found
  NULL otherwise

SIDE EFFECTS
  None.

===========================================================================*/

static cm_orig_q_s_type* cmtask_orig_para_search_act_id_act_priority(

  cm_ss_e_type               ss,
    /* The ss, used to select a queue
    */

  cm_act_id_type             act_id,
    /* The act_type to search in the queue
    */

  cm_act_priority_e_type     act_priority
    /* The priority of the activity
    */
)
{

 CM_MSG_LOW( "cmtask_orig_para_search_act_id act_id %d act_priority %d ss %d",
                                                   act_id, act_priority, ss );

  switch(ss)
  {
    case CM_SS_MAIN :
      /* Search on Main
      */
      return cmtask_orig_para_search_act_id_act_priority_2(
                                &cm_ph_orig_main_q, act_id, act_priority );

    case CM_SS_HDR:
      /* Search on HDR
      */
      return cmtask_orig_para_search_act_id_act_priority_2(
                                 &cm_ph_orig_hdr_q, act_id, act_priority );


    case CM_SS_WLAN:
      /* Search on WLAN
      */
      return cmtask_orig_para_search_act_id_act_priority_2(
                                &cm_ph_orig_wlan_q, act_id, act_priority );

    default:
      CM_CASE_ERR(" Invalid ss %d", ss,0,0);
      /*lint -save -e527 Unreachable
      **     CM_CASE_ERR exits when CM_DEBUG is on
      */
      CM_MSG_LOW("cmtask_orig_para_search_act_id_act_priority_2() on ss %d", ss, 0, 0);
      return NULL;
      /*lint -restore */

  }

}/* cmtask_orig_para_search_act_id_act_priority */


/*===========================================================================

FUNCTION cmtask_orig_para_get_next_priority

DESCRIPTION
  Gets the next priority

DEPENDENCIES
  None

RETURN VALUE
  The next activity priority
  CM_ACT_PRIORITY_MAX if next priority is invalid

SIDE EFFECTS
  None.

===========================================================================*/
static cm_act_priority_e_type cmtask_orig_para_get_next_priority(

  cm_act_priority_e_type act_priority
    /* Act priority
    */
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  CM_ASSERT( BETWEEN( act_priority, CM_ACT_PRIORITY_NONE, CM_ACT_PRIORITY_MAX ));
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch( act_priority )
  {
    case CM_ACT_PRIORITY_0:
      return CM_ACT_PRIORITY_10;

    case CM_ACT_PRIORITY_10:
      return CM_ACT_PRIORITY_20;

    case CM_ACT_PRIORITY_20:
      return CM_ACT_PRIORITY_30;

    case CM_ACT_PRIORITY_30:
      return CM_ACT_PRIORITY_40;

    case CM_ACT_PRIORITY_40:
      return CM_ACT_PRIORITY_50;

    case CM_ACT_PRIORITY_50:
      return CM_ACT_PRIORITY_60;

    case CM_ACT_PRIORITY_60:
      return CM_ACT_PRIORITY_70;

    case CM_ACT_PRIORITY_70:
      return CM_ACT_PRIORITY_80;

    case CM_ACT_PRIORITY_80:
      return CM_ACT_PRIORITY_90;

    case CM_ACT_PRIORITY_90:
      return CM_ACT_PRIORITY_100;

    case CM_ACT_PRIORITY_100:
      return CM_ACT_PRIORITY_110;

    case CM_ACT_PRIORITY_110:
      return CM_ACT_PRIORITY_120;

    case CM_ACT_PRIORITY_120:
      return CM_ACT_PRIORITY_200;

    case CM_ACT_PRIORITY_200:
      return CM_ACT_PRIORITY_END;

    case CM_ACT_PRIORITY_END:
      return CM_ACT_PRIORITY_PH;

    case CM_ACT_PRIORITY_PH:
      return CM_ACT_PRIORITY_BELOW_PH;

    default:
      return CM_ACT_PRIORITY_MAX;
  }
} /* cmtask_orig_para_get_next_priority */

/*===========================================================================

FUNCTION cmtask_orig_para_get_prev_priority

DESCRIPTION
  Gets the pervious act priority

DEPENDENCIES
  None

RETURN VALUE
  The next activity priority
  CM_ACT_PRIORITY_MAX if next priority is invalid

SIDE EFFECTS
  None.

===========================================================================*/
static cm_act_priority_e_type cmtask_orig_para_get_prev_priority(

  cm_act_priority_e_type act_priority
    /* Act priority
    */
)
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  CM_ASSERT( BETWEEN( act_priority, CM_ACT_PRIORITY_NONE, CM_ACT_PRIORITY_MAX ));
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch( act_priority )
  {
    case CM_ACT_PRIORITY_10:
      return CM_ACT_PRIORITY_0;

    case CM_ACT_PRIORITY_20:
      return CM_ACT_PRIORITY_10;

    case CM_ACT_PRIORITY_30:
      return CM_ACT_PRIORITY_20;

    case CM_ACT_PRIORITY_40:
      return CM_ACT_PRIORITY_30;

    case CM_ACT_PRIORITY_50:
      return CM_ACT_PRIORITY_40;

    case CM_ACT_PRIORITY_60:
      return CM_ACT_PRIORITY_50;

    case CM_ACT_PRIORITY_70:
      return CM_ACT_PRIORITY_60;

    case CM_ACT_PRIORITY_80:
      return CM_ACT_PRIORITY_70;

    case CM_ACT_PRIORITY_90:
      return CM_ACT_PRIORITY_80;

    case CM_ACT_PRIORITY_100:
      return CM_ACT_PRIORITY_90;

    case CM_ACT_PRIORITY_110:
      return CM_ACT_PRIORITY_100;

    case CM_ACT_PRIORITY_120:
      return CM_ACT_PRIORITY_110;

    case CM_ACT_PRIORITY_200:
      return CM_ACT_PRIORITY_120;

    case CM_ACT_PRIORITY_END:
      return CM_ACT_PRIORITY_200;

    case CM_ACT_PRIORITY_PH:
      return CM_ACT_PRIORITY_END;

    case CM_ACT_PRIORITY_BELOW_PH:
      return CM_ACT_PRIORITY_PH;

    default:
      return CM_ACT_PRIORITY_MAX;
  }
} /* cmtask_orig_para_get_prev_priority */

/*===========================================================================

FUNCTION cmtask_orig_para_count_upto_first_act_priority

DESCRIPTION
  Returns the number of elements in the queue upto the act_priority

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE

  Count of the elements

SIDE EFFECTS
  None.

===========================================================================*/
static int cmtask_orig_para_count_upto_first_act_priority(

  cm_ss_e_type           ss,
    /* The ss for which the count is required
    */

  cm_orig_q_s_type      *q_ptr,
    /* Pointer to the head of the
    */

  cm_act_priority_e_type act_priority
    /* The priority of the activity
    */
)
{
  int act_count = 0;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT( q_ptr != NULL );
  CM_ASSERT( BETWEEN( act_priority, CM_ACT_PRIORITY_NONE, CM_ACT_PRIORITY_MAX ));
  CM_ASSERT( BETWEEN( ss, CM_SS_NONE, CM_SS_MAX ));

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  while( q_ptr != NULL &&
         cmph_cmp_act_priorities(q_ptr->orig.act_priority, act_priority ) > 0 )
  {
    act_count++;
    q_ptr = cmtask_orig_para_get_next(q_ptr, ss);
  }

  return( act_count );
} /* cmtask_orig_para_count_upto_first_act_priority */

/*===========================================================================

FUNCTION cmtask_orig_para_count_upto_last_act_priority

DESCRIPTION
  Returns the number of elements in the queue upto the act_priority

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE

  Count of the elements

SIDE EFFECTS
  None.

===========================================================================*/
static int cmtask_orig_para_count_upto_last_act_priority(

  cm_ss_e_type           ss,
    /* The ss for which the count is required
    */

  cm_orig_q_s_type      *q_ptr,
    /* Pointer to the head of the
    */

  cm_act_priority_e_type act_priority
    /* The priority of the activity
    */
)
{
  int act_count = 0;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT( q_ptr != NULL );
  CM_ASSERT( BETWEEN( act_priority, CM_ACT_PRIORITY_NONE, CM_ACT_PRIORITY_MAX ));

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  while( q_ptr != NULL &&
         cmph_cmp_act_priorities(q_ptr->orig.act_priority, act_priority ) >= 0 )
  {
    act_count++;
    q_ptr = cmtask_orig_para_get_next(q_ptr, ss);
  }

  return( act_count );
} /* cmtask_orig_para_count_upto_last_act_priority */



/*===========================================================================

FUNCTION cmtask_orig_para_count

DESCRIPTION
  Returns the number of elements in the queue

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE

  Count of the elements

SIDE EFFECTS
  None.

===========================================================================*/

int cmtask_orig_para_count(

  cm_ss_e_type ss
    /* The ss, used to select a queue
    */
)
{
  switch(ss)
  {
    case CM_SS_MAIN :
      /* Count on Main
      */
      return ( cm_ph_orig_main_q.cnt );

    case CM_SS_HDR:
      /* Count on HDR
      */
      return( cm_ph_orig_hdr_q.cnt );

    case CM_SS_WLAN:
      /* Count on WLAN
      */
      return ( cm_ph_orig_wlan_q.cnt );

    default:
      CM_CASE_ERR(" Invalid ss %d", ss,0,0);
      /*lint -save -e527 Unreachable
      **     CM_CASE_ERR exits when CM_DEBUG is on
      */
      CM_MSG_LOW("cmtask_orig_para_count() on ss %d", ss, 0, 0);
      return 0;
      /*lint -restore */
  }

} /* cmtask_orig_para_count */


/*===========================================================================

FUNCTION cmtask_orig_para_count_2

DESCRIPTION
  Returns the number of elements in the queue

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE

  Count of the elements

SIDE EFFECTS
  None.

===========================================================================*/

int cmtask_orig_para_count_2(

  cm_ss_e_type           ss,
    /* The ss for which the count is required
    */

  cm_act_priority_e_type act_priority,
    /* The priority of the activity
    */

  cm_cmp_criteria_e_type cmp_criteria
    /* Comparision criteria
    */
)
{

  cm_act_priority_e_type next_act_priority =
                          cmtask_orig_para_get_next_priority( act_priority );

  cm_act_priority_e_type prev_act_priority =
                          cmtask_orig_para_get_prev_priority( act_priority );

  int next_act_priority_count = 0;
  int prev_act_priority_count = 0;
  int total_act_count = 0;

  cm_orig_q_s_type* q_ptr;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT( BETWEEN( ss, CM_SS_NONE, CM_SS_MAX ));
  CM_ASSERT( BETWEEN( act_priority, CM_ACT_PRIORITY_NONE, CM_ACT_PRIORITY_MAX ));
  CM_ASSERT( BETWEEN( cmp_criteria, CM_CMP_CRITERIA_NONE, CM_CMP_CRITERIA_MAX ));

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( ss == CM_SS_MAX )
  {
    return 0;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  q_ptr           = cmtask_orig_para_get_first( ss );
  total_act_count = cmtask_orig_para_count( ss );

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( q_ptr == NULL )
  {
      return(0);
  }
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( next_act_priority != CM_ACT_PRIORITY_MAX )
  {
    next_act_priority_count = cmtask_orig_para_count_upto_first_act_priority(
                                              ss, q_ptr, next_act_priority );
  }

  if( prev_act_priority != CM_ACT_PRIORITY_MAX )
  {
    prev_act_priority_count = cmtask_orig_para_count_upto_last_act_priority(
                                              ss, q_ptr, prev_act_priority );
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


  switch( cmp_criteria )
  {

    case CM_CMP_CRITERIA_LESS_THAN:
       return ( total_act_count - next_act_priority_count );

      /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    case CM_CMP_CRITERIA_EQUAL_TO:

      if( next_act_priority != CM_ACT_PRIORITY_MAX )
      {
        return( next_act_priority_count - prev_act_priority_count );
      }

      return ( total_act_count - prev_act_priority_count );

      /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    case CM_CMP_CRITERIA_NOT_EQUAL_TO:

      /*lint -save -e834 Operator '-' followed by operator '+' is confusing.
      ** Use parentheses
      */
      return( total_act_count - next_act_priority_count + prev_act_priority_count );
      /*lint -restore */

      /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    case CM_CMP_CRITERIA_LESS_THAN_EQUAL_TO:
      return( total_act_count - prev_act_priority_count );

      /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    case CM_CMP_CRITERIA_GREATER_THAN:
      return( prev_act_priority_count );

      /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


    case CM_CMP_CRITERIA_GREATER_THAN_EQUAL_TO:
      return( next_act_priority_count );

      /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    default:
      return 0;
  }

} /* cmtask_orig_para_count_2 */

/*===========================================================================

FUNCTION cmtask_orig_search_func_act_type

DESCRIPTION
  Search the queue for the act_type

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  TRUE: act_type found
  FALSE: act_type not found
  The Q function requires a return type of int so te return type is NOT
  boolean


SIDE EFFECTS
  None.

===========================================================================*/


static int cmtask_orig_search_func_act_type(
  void * item_ptr, void * compare_val
)
{

  cm_orig_q_s_type   *q_orig_ptr  = NULL;
  cm_act_type_e_type *compare_ptr = NULL;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(item_ptr    != NULL);
  CM_ASSERT(compare_val != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  q_orig_ptr  = (cm_orig_q_s_type *) item_ptr;
  compare_ptr = (cm_act_type_e_type *) compare_val;

  if( q_orig_ptr->act_type == *compare_ptr )
  {
    return( TRUE );
  }

  return( FALSE );
}/* cmtask_orig_search_func_act_type */

/*===========================================================================

FUNCTION cmtask_orig_para_search_act_type_2

DESCRIPTION
  Searches the Origination Queues for the act_type

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  The pointer to the Queue element corresponding to act_type, if found
  NULL otherwise

SIDE EFFECTS
  None.

===========================================================================*/

static cm_orig_q_s_type* cmtask_orig_para_search_act_type_2(

  cm_cmd_q_type *q_ptr,
    /* The Queue on which the item is to be searched
    */
  cm_act_type_e_type act_type
    /* the activity type to be searched
    */
)
{

  cm_orig_q_s_type *q_search_pos;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(q_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  q_search_pos = q_linear_search( q_ptr,
                                  cmtask_orig_search_func_act_type,
                                  &act_type );

  return( q_search_pos );
}/* cmtask_orig_para_search_act_type */


/*===========================================================================

FUNCTION cmtask_orig_para_search_act_type

DESCRIPTION
  Searches the orig queue for the act_type

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  Pointer to the element, if found
  NULL otherwise

SIDE EFFECTS
  None.

===========================================================================*/

cm_orig_q_s_type* cmtask_orig_para_search_act_type(

  cm_ss_e_type ss,
    /* The ss, used to select a queue
    */
  cm_act_type_e_type act_type
    /* The act_type to search in the queue
    */
)
{

 CM_MSG_LOW( "cmtask_orig_para_search_act_type %d ss %d", act_type, ss, 0 );

  switch(ss)
  {
    case CM_SS_MAIN :
      /* Search on Main
      */
      return cmtask_orig_para_search_act_type_2( &cm_ph_orig_main_q, act_type );

    case CM_SS_HDR:
      /* Search on HDR
      */
      return cmtask_orig_para_search_act_type_2( &cm_ph_orig_hdr_q, act_type );


    case CM_SS_WLAN:
      /* Search on WLAN
      */
      return cmtask_orig_para_search_act_type_2( &cm_ph_orig_wlan_q, act_type );

    default:
      CM_CASE_ERR(" Invalid ss %d", ss,0,0);
      /*lint -save -e527 Unreachable
      **     CM_CASE_ERR exits when CM_DEBUG is on
      */
      CM_MSG_LOW("cmtask_orig_para_search_act_type() on ss %d", ss, 0, 0);
      return NULL;
      /*lint -restore */

  }

}/* cmtask_orig_para_search_act_type */


/*===========================================================================

FUNCTION cmtask_orig_search_func_act_type_act_priority

DESCRIPTION
  Search the queue for the act_type

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  TRUE: act_type and act_priority found
  FALSE: act_type and act_priority not found
  The Q function requires a return type of int so te return type is NOT
  boolean


SIDE EFFECTS
  None.

===========================================================================*/


static int cmtask_orig_search_func_act_type_act_priority(
  void * item_ptr, void * compare_val
)
{

  cm_orig_q_s_type   *q_orig_ptr  = NULL;
  cm_orig_q_s_type   *compare_ptr = NULL;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(item_ptr    != NULL);
  CM_ASSERT(compare_val != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  q_orig_ptr  = (cm_orig_q_s_type *) item_ptr;
  compare_ptr = (cm_orig_q_s_type *) compare_val;

  if( q_orig_ptr->act_type          == compare_ptr->act_type &&
      q_orig_ptr->orig.act_priority == compare_ptr->orig.act_priority )
  {
    return( TRUE );
  }

  return( FALSE );

}/* cmtask_orig_search_func_act_type */

/*===========================================================================

FUNCTION cmtask_is_orig_q_compare_act_type_mode_pref_band_pref

DESCRIPTION
  Search the queue for the act_type

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  TRUE: act_type and (mode_pref or band_pref) found
  FALSE: act_type and (mode_pref or band_pref) not found
  The Q function requires a return type of int so te return type is NOT
  boolean


SIDE EFFECTS
  None.

===========================================================================*/


static int cmtask_is_orig_q_compare_act_type_mode_pref_band_pref(
  void * item_ptr, void * compare_val
)
{

  cm_orig_q_s_type   *q_orig_ptr  = NULL;
  cm_orig_q_s_type   *compare_ptr = NULL;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(item_ptr    != NULL);
  CM_ASSERT(compare_val != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  q_orig_ptr  = (cm_orig_q_s_type *) item_ptr;
  compare_ptr = (cm_orig_q_s_type *) compare_val;

  if( q_orig_ptr->act_type            == compare_ptr->act_type &&
      ((q_orig_ptr->orig.orig_mode_pref != compare_ptr->orig.orig_mode_pref) ||
	   (q_orig_ptr->orig.orig_band_pref != compare_ptr->orig.orig_band_pref)
	  )
	)

  {
    return( TRUE );
  }

  return( FALSE );

}/* cmtask_is_orig_q_compare_act_type_mode_pref_band_pref */

/*===========================================================================

FUNCTION cmtask_orig_para_search_act_type_act_priority_2

DESCRIPTION
  Searches the Origination Queues for the act_type

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  The pointer to the Queue element corresponding to act_type, if found
  NULL otherwise

SIDE EFFECTS
  None.

===========================================================================*/

static cm_orig_q_s_type* cmtask_orig_para_search_act_type_act_priority_2(

  cm_cmd_q_type              *q_ptr,
    /* The Queue on which the item is to be searched
    */

  cm_act_type_e_type         act_type,
    /* the activity type to be searched
    */

  cm_act_priority_e_type     act_priority
    /* The priority of the activity
    */

)
{
  cm_orig_q_s_type *q_search_pos;
  cm_orig_q_s_type q_temp;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(q_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  cmtask_orig_para_init(&q_temp);

  q_temp.act_type = act_type;
  q_temp.orig.act_priority = act_priority;

  q_search_pos = q_linear_search( q_ptr,
                                  cmtask_orig_search_func_act_type_act_priority,
                                  &q_temp );

  return( q_search_pos );

}/* cmtask_orig_para_search_act_type */


/*===========================================================================

FUNCTION cmtask_orig_para_search_act_type_act_priority

DESCRIPTION
  Searches the orig queue for the act_type

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  Pointer to the element, if found
  NULL otherwise

SIDE EFFECTS
  None.

===========================================================================*/
cm_orig_q_s_type* cmtask_orig_para_search_act_type_act_priority(

  cm_ss_e_type ss,
    /* The ss, used to select a queue
    */

  cm_act_type_e_type act_type,
    /* The act_type to search in the queue
    */

  cm_act_priority_e_type     act_priority
    /* The priority of the activity
    */
)
{

 CM_MSG_LOW( "cmtask_orig_para_search_act_type %d ss %d", act_type, ss, 0 );

  switch(ss)
  {
    case CM_SS_MAIN :
      /* Search on Main
      */
      return cmtask_orig_para_search_act_type_act_priority_2(
                                &cm_ph_orig_main_q, act_type, act_priority );

    case CM_SS_HDR:
      /* Search on HDR
      */
      return cmtask_orig_para_search_act_type_act_priority_2(
                                 &cm_ph_orig_hdr_q, act_type, act_priority );


    case CM_SS_WLAN:
      /* Search on WLAN
      */
      return cmtask_orig_para_search_act_type_act_priority_2(
                                &cm_ph_orig_wlan_q, act_type, act_priority );

    default:
      CM_CASE_ERR(" Invalid ss %d", ss,0,0);
      /*lint -save -e527 Unreachable
      **     CM_CASE_ERR exits when CM_DEBUG is on
      */
      CM_MSG_LOW("cmtask_orig_para_search_act_type_act_priority_2() on ss %d", ss, 0, 0);
      return NULL;
      /*lint -restore */

  }

}/* cmtask_orig_para_search_act_type_act_priority_2 */


/*===========================================================================

FUNCTION cmtask_compute_srv_domain_pref

DESCRIPTION
  Gets the union of all the orig_srv_domain_pref

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  The union of all the orig_srv_domain pref's

SIDE EFFECTS
  None.

===========================================================================*/

cm_srv_domain_pref_e_type cmtask_compute_srv_domain_pref(

  cm_ss_e_type ss
    /* The ss, used to select a queue
    */
)
{

  cm_srv_domain_pref_e_type orig_srv_domain_pref = CM_SRV_DOMAIN_PREF_NONE;

  cm_orig_q_s_type* q_ptr = cmtask_orig_para_get_first(ss);

  CM_MSG_LOW( "cmtask_compute_srv_domain_pref ss %d ", ss, 0, 0 );

  while(q_ptr != NULL)
  {
    /* Add this srv_domain_pref to the existing one
    */

    if( (cmph_cmp_act_priorities(q_ptr->orig.act_priority, CM_ACT_PRIORITY_PH) >= 0 )
    #if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
    #endif
    )
    {
      orig_srv_domain_pref = cmph_add_srv_domain_pref(
                            orig_srv_domain_pref,
                            q_ptr->orig.orig_srv_domain_pref );
    }

    q_ptr = cmtask_orig_para_get_next(q_ptr, ss);

  }
  return(orig_srv_domain_pref);

}/* cmtask_compute_srv_domain_pref */


/*===========================================================================

FUNCTION cm_ph_get_top_ptr

DESCRIPTION
  Returns the pointer to the top element of the priority queue

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  Pointer to the top element if present
  NULL otherwise

SIDE EFFECTS
  None.

===========================================================================*/

cm_orig_q_s_type* cmtask_orig_para_get_top(
  cm_ss_e_type ss
    /* The ss whose top element is required
    */
)
{

  switch(ss)
  {
    case CM_SS_MAIN:

      return( q_check( &cm_ph_orig_main_q ) );

    case CM_SS_HDR:

      return( q_check( &cm_ph_orig_hdr_q ) );

    case CM_SS_WLAN:

      return( q_check( &cm_ph_orig_wlan_q ) );

    default:

      CM_CASE_ERR(" Invalid ss %d", ss,0,0);
       /*lint -save -e527 Unreachable
      **     CM_CASE_ERR exits when CM_DEBUG is on
      */
      CM_MSG_LOW("cmtask_orig_para_get_top() on ss %d", ss, 0, 0);
      return NULL;
      /*lint -restore */

  } /*  switch(ss) */

} /* cm_ph_get_top_ptr */

/*===========================================================================

FUNCTION cmtask_orig_para_delete_2

DESCRIPTION
  Deletes the origination parameters from the priority queue

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  TRUE: If the top pointer changed
  FALSE: Otherwise

SIDE EFFECTS
  None.

===========================================================================*/

static boolean cmtask_orig_para_delete_2(

  cm_orig_q_s_type          *ph_orig_ptr,
    /* The buffer to delete from the queue
    */

  cm_cmd_q_type             *q_ptr,
    /* The Queue on which the item is to be deleted
    */

  cm_cmd_q_type             *free_q_ptr,
    /* The Free Queue on which the item is to be returned
    */

  boolean                    free_buf
    /* Return the buffer to the free Q
    */
)
{
  boolean is_top_ptr_changed = FALSE;
  /* Delete the client from the priority queue.
  */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(ph_orig_ptr != NULL);
  CM_ASSERT(q_ptr       != NULL);
  CM_ASSERT(free_q_ptr  != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* If we are deleting the top, we should return true
  */

  if( ph_orig_ptr == q_check(q_ptr) )
  {
    is_top_ptr_changed = TRUE;
  }

  #ifdef FEATURE_Q_NO_SELF_QPTR
  q_delete( q_ptr, &ph_orig_ptr->link );
  #else
  q_delete( &ph_orig_ptr->link );
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the priority queue's link item to make sure its next and prev
  ** pointers are no longer pointing at other clients.
  */
  (void) q_link( ph_orig_ptr, &ph_orig_ptr->link );

  if( ! free_buf )
  {
    return(is_top_ptr_changed);
  }

  /* Initialize the values to default
  */

  cmtask_orig_para_init(ph_orig_ptr);

  cmtask_orig_para_put_2( free_q_ptr, ph_orig_ptr );

  return(is_top_ptr_changed);
}/* cmtask_orig_para_delete_2 */


/*===========================================================================

FUNCTION cmtask_orig_para_delete

DESCRIPTION
  Deletes the q_ptr from the origination queue

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  TRUE: If the top pointer changed
  FALSE: Otherwise

SIDE EFFECTS
  None.

===========================================================================*/

 boolean cmtask_orig_para_delete(

  cm_ss_e_type               ss,
    /* The ss, used to select a queue
    */

  cm_orig_q_s_type          *ph_orig_ptr,
    /* The buffer to delete from the queue
    */

  boolean                    free_buf
    /* Return the buffer to the free Q
    */

)
{

  CM_ASSERT( ph_orig_ptr != NULL );

  CM_MSG_HIGH( "cmtask_orig_para_delete act_type %d act_id %d ss %d",
                 ph_orig_ptr->act_type, ph_orig_ptr->orig.act_id, ss);

  switch(ss)
  {
    case CM_SS_MAIN :
      /* Delete on Main
      */
      return ( cmtask_orig_para_delete_2( ph_orig_ptr,
                            &cm_ph_orig_main_q,
                            &cm_ph_orig_main_free_q,
                             free_buf ) );

    case CM_SS_HDR:
      /* Delete on HDR
      */
      return ( cmtask_orig_para_delete_2( ph_orig_ptr,
                            &cm_ph_orig_hdr_q,
                            &cm_ph_orig_hdr_free_q,
                             free_buf ) );

    case CM_SS_WLAN:
      /* Delete on WLAN
      */
      return ( cmtask_orig_para_delete_2( ph_orig_ptr,
                            &cm_ph_orig_wlan_q,
                            &cm_ph_orig_wlan_free_q,
                             free_buf ) );

    default:
      CM_CASE_ERR(" Invalid ss %d", ss,0,0);
      /*lint -save -e527 Unreachable
      **     CM_CASE_ERR exits when CM_DEBUG is on
      */
      CM_MSG_LOW("cmtask_orig_para_delete() on ss %d", ss, 0, 0);
      /*lint -restore */
      return FALSE;
  }
} /* cmtask_orig_para_delete */


/*===========================================================================

FUNCTION cmtask_orig_para_insert_2

DESCRIPTION
  Queues the origination parameters in the priority queue

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  TRUE: If the top pointer changed
  FALSE: Otherwise

SIDE EFFECTS
  None.

===========================================================================*/

static boolean cmtask_orig_para_insert_2(
  cm_orig_q_s_type *ph_orig_ptr,
    /* The buffer to add to the queue
    */
  cm_cmd_q_type* q_ptr
    /* The Queue on which the item is to be inserted
    */
)
{
  cm_orig_q_s_type *q_insert_pos       = NULL;
  boolean           is_top_ptr_changed = FALSE;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(ph_orig_ptr     != NULL);
  CM_ASSERT(q_ptr           != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  (void) q_link( ph_orig_ptr, &ph_orig_ptr->link );  /* init link */

  q_insert_pos = q_linear_search( q_ptr, cmtask_orig_para_compare_func, ph_orig_ptr );

  /* If position is not NULL insert it before q_insert_pos
  **  Else Insert it at end of the Queue
  */

  /* If we are inserting at the top, we should return true
  */

  if(q_insert_pos == q_check(q_ptr))
  {
    is_top_ptr_changed = TRUE;
  }

  if( q_insert_pos != NULL )
  {
    q_insert( q_ptr, (q_link_type *)ph_orig_ptr, (q_link_type *)q_insert_pos );
  }
  else
  {
    cmtask_orig_para_put_2( q_ptr, ph_orig_ptr );
  }

  return(is_top_ptr_changed);

}/* cmtask_orig_para_insert_2 */


/*===========================================================================

FUNCTION cmtask_orig_para_insert

DESCRIPTION
  Queues the origination parameters in the priority queue

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  TRUE: If the top pointer changed
  FALSE: Otherwise

SIDE EFFECTS
  None.

===========================================================================*/

boolean cmtask_orig_para_insert(
  cm_ss_e_type ss,
    /* The ss, used to select a queue
    */
  cm_orig_q_s_type *ph_orig_ptr
    /* The buffer to add to the queue
    */
)
{

  CM_ASSERT( ph_orig_ptr != NULL );

  CM_MSG_HIGH( "cmtask_orig_para_insert act_type %d act_id %d ss %d",
                                             ph_orig_ptr->act_type,
                                             ph_orig_ptr->orig.act_id, ss );

  switch(ss)
  {
    case CM_SS_MAIN :
      /* Insert on Main
      */
      return( cmtask_orig_para_insert_2( ph_orig_ptr, &cm_ph_orig_main_q ));

    case CM_SS_HDR:
      /* Insert on HDR
      */
      return( cmtask_orig_para_insert_2( ph_orig_ptr, &cm_ph_orig_hdr_q ));

    case CM_SS_WLAN:
      /* Insert on WLAN
      */
      return( cmtask_orig_para_insert_2( ph_orig_ptr, &cm_ph_orig_wlan_q ));

    default:
      CM_CASE_ERR( "Invalid ss %d", ss, 0, 0 );
      /*lint -save -e527 Unreachable
      **     CM_CASE_ERR exits when CM_DEBUG is on
      */
      CM_MSG_LOW( "cmtask_orig_para_insert() on ss %d", ss, 0, 0 );
      /*lint -restore */
      return FALSE;
  }

}/* cmtask_orig_para_insert */


/*===========================================================================

FUNCTION cmtask_orig_para_get_first

DESCRIPTION
  Return a pointer to first orig_param in the cm_ph_orig_xxx_q.

DEPENDENCIES
  cm_ph_orig_xxx_q must have already been initialized with
  cm_ph_orig_xxx_q_init().

RETURN VALUE
  Return a pointer to first buffer in the cm_ph_orig_xxx_q list.
  If cm_ph_orig_xxx_q list is empty a NULL is returned.

SIDE EFFECTS
  none

===========================================================================*/
cm_orig_q_s_type* cmtask_orig_para_get_first(

  cm_ss_e_type ss
    /* The ss for which the first element is needed
    */
)
{
  CM_FUNC_START( "cmtask_orig_para_get_first() ss %d",ss,0,0 );

  switch(ss)
  {
    case CM_SS_MAIN:

      return( (cm_orig_q_s_type*) q_check( &cm_ph_orig_main_q ) );

    case CM_SS_HDR:

      return( (cm_orig_q_s_type*) q_check( &cm_ph_orig_hdr_q ) );

    case CM_SS_WLAN:

      return( (cm_orig_q_s_type*) q_check( &cm_ph_orig_wlan_q ) );

    default:
      CM_CASE_ERR( "Invalid ss %d ", ss, 0, 0 );
      /*lint -save -e527 Unreachable
      **     CM_CASE_ERR exits when CM_DEBUG is on
      */
      CM_MSG_LOW( "cmtask_orig_para_get_next() on ss %d", ss, 0, 0 );
      /*lint -restore */
      return NULL;

  }/* Switch (ss) */
} /* cmtask_orig_para_get_first */


/*===========================================================================

FUNCTION cmtask_orig_para_get_next

DESCRIPTION
  Return a pointer to next orig_param in the cm_ph_orig_xxx_q.

DEPENDENCIES
  cm_ph_orig_xxx_q must have already been initialized with
  cm_ph_orig_xxx_q_init().


RETURN VALUE
  Return a pointer to next buffer in the cm_ph_orig_xxx_q list.
  If cm_ph_orig_xxx_q list is empty a NULL is returned.

SIDE EFFECTS
  None.

===========================================================================*/
cm_orig_q_s_type* cmtask_orig_para_get_next(

  cm_orig_q_s_type* q_ptr,
    /* Pointer to the current q element
    */

  cm_ss_e_type ss
    /* The queue
    */
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_ASSERT( q_ptr != NULL );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_FUNC_START( "cmtask_orig_para_get_next() ss %d",ss,0,0 );

  switch(ss)
  {
    case CM_SS_MAIN:

      return ( (cm_orig_q_s_type *) q_next( &cm_ph_orig_main_q,
                                            &q_ptr->link ) );

    case CM_SS_HDR:

      return ( (cm_orig_q_s_type *) q_next( &cm_ph_orig_hdr_q,
                                            &q_ptr->link ) );
    case CM_SS_WLAN:

      return ( (cm_orig_q_s_type *) q_next( &cm_ph_orig_wlan_q,
                                            &q_ptr->link ) );

    default:

      CM_CASE_ERR( "Invalid ss %d ", ss, 0, 0 );
      /*lint -save -e527 Unreachable
      **     CM_CASE_ERR exits when CM_DEBUG is on
      */
      CM_MSG_LOW( "cmtask_orig_para_get_next() on ss %d", ss, 0, 0 );
      /*lint -restore */
      return NULL;

  }/* Switch (ss) */

} /* cmtask_orig_para_get_next */


/*===========================================================================

FUNCTION cmtask_change_act_priority2

DESCRIPTION
  Changes the priority of origination parameters in the priority queue.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
 TRUE If the top pointer changed
 FALSE Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
static boolean cmtask_orig_para_change_act_priority_2(

  cm_ss_e_type               ss,
    /* SS to use for origination, if hybrid operation is changed,
    ** then ss= SD_SS_MAX.
    */

  cm_act_type_e_type         act_type,
    /* Type of CM activity
    */

  cm_act_priority_e_type     curr_priority,
    /* The current priority of the activity
    */

  cm_act_priority_e_type     new_priority,
    /* The new priority of the activity
    */

  boolean                    global_change
    /* Apply the change to ALL the mathcing entries in the Q
    */
)
{

  cm_orig_q_s_type*  orig_q_ptr =
                       cmtask_orig_para_search_act_type_act_priority(
                                                             ss,
                                                             act_type,
                                                             curr_priority );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_ASSERT( ss != CM_SS_MAX );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( orig_q_ptr == NULL )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Remove the pointer from the Q and insert it in the correct position
  */

  (void)cmtask_orig_para_delete( ss, orig_q_ptr, FALSE );

  orig_q_ptr->orig.act_priority = new_priority;

  (void)cmtask_orig_para_insert( ss, orig_q_ptr );

  if( !global_change )
  {
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  while( (orig_q_ptr =
           cmtask_orig_para_search_act_type_act_priority(
                                                    ss,
                                                    act_type,
                                                    curr_priority )) != NULL )
  {

    /* Remove the pointer from the Q and insert it in the correct position
    */

    (void)cmtask_orig_para_delete(ss, orig_q_ptr, FALSE);

    orig_q_ptr->orig.act_priority = new_priority;

    (void)cmtask_orig_para_insert( ss, orig_q_ptr );
  }

  return TRUE;

} /* cmtask_orig_para_change_act_priority2 */



/*===========================================================================

FUNCTION cmtask_orig_para_search_act_type_mode_pref_2

DESCRIPTION
  Searches the Origination Queues for the act_type

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  The pointer to the Queue element corresponding to act_type, if found
  NULL otherwise

SIDE EFFECTS
  None.

===========================================================================*/

static cm_orig_q_s_type* cmtask_orig_para_search_act_type_mode_pref_2(

  cm_cmd_q_type              *q_ptr,
    /* The Queue on which the item is to be searched
    */

  cm_act_type_e_type         act_type,
    /* the activity type to be searched
    */

  cm_mode_pref_e_type         mode_pref
    /* The priority of the activity
    */

)
{
  cm_orig_q_s_type *q_search_pos;
  cm_orig_q_s_type q_temp;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(q_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  cmtask_orig_para_init(&q_temp);

  q_temp.act_type = act_type;
  q_temp.orig.orig_mode_pref = mode_pref;
  q_temp.orig.orig_band_pref = cmph_ptr()->band_pref;

  q_search_pos = q_linear_search( q_ptr,
                                  cmtask_is_orig_q_compare_act_type_mode_pref_band_pref,
                                  &q_temp );

  return( q_search_pos );

}/* cmtask_orig_para_search_act_type_mode_pref_2 */

/*===========================================================================

FUNCTION cmtask_orig_para_search_act_type_mode_pref

DESCRIPTION
  Searches the orig queue for the act_type and mode_pref

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  Pointer to the element, if found
  NULL otherwise

SIDE EFFECTS
  None.

===========================================================================*/
static cm_orig_q_s_type* cmtask_orig_para_search_act_type_mode_pref(

  cm_ss_e_type ss,
    /* The ss, used to select a queue
    */

  cm_act_type_e_type act_type,
    /* The act_type to search in the queue
    */

  cm_mode_pref_e_type         mode_pref
    /* The priority of the activity
    */
)
{

 CM_MSG_LOW( "cmtask_orig_para_search_act_type_mode_pref %d ss %d", act_type, ss, 0 );

  switch(ss)
  {
    case CM_SS_MAIN :
      /* Search on Main
      */
      return cmtask_orig_para_search_act_type_mode_pref_2(
                                &cm_ph_orig_main_q, act_type, mode_pref );

    case CM_SS_HDR:
      /* Search on HDR
      */
      return cmtask_orig_para_search_act_type_mode_pref_2(
                                 &cm_ph_orig_hdr_q, act_type, mode_pref );


    case CM_SS_WLAN:
      /* Search on WLAN
      */
      return cmtask_orig_para_search_act_type_mode_pref_2(
                                &cm_ph_orig_wlan_q, act_type, mode_pref );

    default:
      CM_CASE_ERR(" Invalid ss %d", ss,0,0);
      /*lint -save -e527 Unreachable
      **     CM_CASE_ERR exits when CM_DEBUG is on
      */
      CM_MSG_LOW("cmtask_orig_para_search_act_type_act_priority_2() on ss %d", ss, 0, 0);
      return NULL;
      /*lint -restore */

  }

}/* cmtask_orig_para_search_act_type_mode_pref */

/*===========================================================================

FUNCTION cmtask_orig_para_change_mode_pref_2

DESCRIPTION
  Changes the priority of origination parameters in the priority queue.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
 TRUE If the top pointer changed
 FALSE Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
static boolean cmtask_orig_para_change_mode_pref_2(

  cm_ss_e_type               ss,
    /* SS to use for origination, if hybrid operation is changed,
    ** then ss= SD_SS_MAX.
    */

  cm_act_type_e_type         act_type,
    /* Type of CM activity
    */

  cm_mode_pref_e_type         mode_pref,
    /* Mode pref of phone
    */

  boolean                    global_change
    /* Apply the change to ALL the mathcing entries in the Q
    */
)
{

  cm_orig_q_s_type*  orig_q_ptr =
                       cmtask_orig_para_search_act_type_mode_pref(
                                                             ss,
                                                             act_type,
                                                             mode_pref );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_ASSERT( ss != CM_SS_MAX );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( orig_q_ptr == NULL )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Remove the pointer from the Q and insert it in the correct position
  */

  (void)cmtask_orig_para_delete( ss, orig_q_ptr, FALSE );

  /* Update Mode and Band preferences of the orig_q.
  ** Other parameters of orig_q are not required to be changed.
  */
  orig_q_ptr->orig.orig_mode_pref = mode_pref;
  orig_q_ptr->orig.orig_band_pref = cmph_ptr()->band_pref;
  
  (void)cmtask_orig_para_insert( ss, orig_q_ptr );

  if( !global_change )
  {
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  while( (orig_q_ptr =
           cmtask_orig_para_search_act_type_mode_pref(
                                                    ss,
                                                    act_type,
                                                    mode_pref )) != NULL )
  {

    /* Remove the pointer from the Q and insert it in the correct position
    */

    (void)cmtask_orig_para_delete(ss, orig_q_ptr, FALSE);

    orig_q_ptr->orig.orig_mode_pref = mode_pref;
    orig_q_ptr->orig.orig_band_pref = cmph_ptr()->band_pref;

    (void)cmtask_orig_para_insert( ss, orig_q_ptr );
  }

  return TRUE;

} /* cmtask_orig_para_change_mode_pref_2 */
/*===========================================================================

FUNCTION cmtask_orig_para_change_mode_pref

DESCRIPTION
  Changes the mode_pref of origination parameters in the priority queue.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
 TRUE If the top pointer changed
 FALSE Otherwise

SIDE EFFECTS
  None.

===========================================================================*/

boolean cmtask_orig_para_change_mode_pref(

  cm_ss_e_type               ss,
    /* SS to use for origination, if hybrid operation is changed,
    ** then ss= SD_SS_MAX.
    */

  cm_act_type_e_type         act_type,
    /* Type of CM activity
    */

  cm_mode_pref_e_type         mode_pref,
    /* Mode pref of phone
    */

  boolean                    global_change
    /* Apply the change to ALL the mathcing entries in the Q
    */
)
{

  CM_ASSERT(INRANGE( ss, CM_SS_NONE, CM_SS_MAX));
  CM_ASSERT(BETWEEN( act_type, CM_ACT_TYPE_NONE, CM_ACT_TYPE_MAX));

  CM_MSG_HIGH( "cmtask_orig_para_change_mode_pref act_type %d ss %d fav_mode_pref %d",
                                                  act_type, ss, mode_pref );

  switch(ss)
  {
    case CM_SS_MAIN :
      /* Insert on Main
      */
      return( cmtask_orig_para_change_mode_pref_2( CM_SS_MAIN,
                                                      act_type,
                                                      mode_pref,
                                                      global_change ));

    case CM_SS_HDR:
      /* Insert on HDR
      */
      return( cmtask_orig_para_change_mode_pref_2( CM_SS_HDR,
                                                       act_type,
                                                       mode_pref,
                                                       global_change ));

    case CM_SS_WLAN:
      /* Insert on WLAN
      */
      return( cmtask_orig_para_change_mode_pref_2( CM_SS_WLAN,
                                                      act_type,
                                                      mode_pref,
                                                      global_change ));

    default:
      CM_CASE_ERR( "Invalid ss %d", ss, 0, 0 );
      /*lint -save -e527 Unreachable
      **     CM_CASE_ERR exits when CM_DEBUG is on
      */
      CM_MSG_LOW( "cmtask_orig_para_change_mode_pref() on ss %d", ss, 0, 0 );
      /*lint -restore */
      return FALSE;
  }

} /* cmtask_orig_para_change_mode_pref */


/*===========================================================================

FUNCTION cmtask_orig_para_change_act_priority

DESCRIPTION
  Changes the priority of origination parameters in the priority queue.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
 TRUE If the top pointer changed
 FALSE Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean cmtask_orig_para_change_act_priority(

  cm_ss_e_type               ss,
    /* SS to use for origination, if hybrid operation is changed,
    ** then ss= SD_SS_MAX.
    */

  cm_act_type_e_type         act_type,
    /* Type of CM activity
    */

  cm_act_priority_e_type     curr_priority,
    /* The current priority of the activity
    */

  cm_act_priority_e_type     new_priority,
    /* The new priority of the activity
    */

  boolean                    global_change
    /* Apply the change to ALL the mathcing entries in the Q
    */

)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_ASSERT(INRANGE( ss, CM_SS_NONE, CM_SS_MAX));
  CM_ASSERT(BETWEEN( act_type, CM_ACT_TYPE_NONE, CM_ACT_TYPE_MAX));
  CM_ASSERT(BETWEEN( curr_priority, CM_ACT_PRIORITY_NONE, CM_ACT_PRIORITY_MAX));
  CM_ASSERT(BETWEEN( new_priority, CM_ACT_PRIORITY_NONE, CM_ACT_PRIORITY_MAX));
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH( "cmtask_change_act_priority act_type %d ss %d new_priority %d",
                                                act_type, ss, new_priority );

  switch(ss)
  {
    case CM_SS_MAIN :
      /* Insert on Main
      */
      return( cmtask_orig_para_change_act_priority_2( CM_SS_MAIN,
                                                      act_type,
                                                      curr_priority,
                                                      new_priority,
                                                      global_change ));

    case CM_SS_HDR:
      /* Insert on HDR
      */
      return( cmtask_orig_para_change_act_priority_2( CM_SS_HDR,
                                                       act_type,
                                                       curr_priority,
                                                       new_priority,
                                                       global_change ));

    case CM_SS_WLAN:
      /* Insert on WLAN
      */
      return( cmtask_orig_para_change_act_priority_2( CM_SS_WLAN,
                                                      act_type,
                                                      curr_priority,
                                                      new_priority,
                                                      global_change ));

    case CM_SS_MAX:
      {
        boolean is_priority_changed = FALSE;
        is_priority_changed = cmtask_orig_para_change_act_priority_2( CM_SS_MAIN,
                                                            act_type,
                                                            curr_priority,
                                                            new_priority,
                                                            global_change );

        is_priority_changed = cmtask_orig_para_change_act_priority_2( CM_SS_HDR ,
                                                            act_type,
                                                            curr_priority,
                                                            new_priority,
                                                            global_change )
                             || is_priority_changed;

        is_priority_changed = cmtask_orig_para_change_act_priority_2( CM_SS_WLAN ,
                                                            act_type,
                                                            curr_priority,
                                                            new_priority,
                                                            global_change )
                             || is_priority_changed;

        return( BOOLEAN(is_priority_changed) );
      }

    default:
      CM_CASE_ERR( "Invalid ss %d", ss, 0, 0 );
      /*lint -save -e527 Unreachable
      **     CM_CASE_ERR exits when CM_DEBUG is on
      */
      CM_MSG_LOW( "cmtask_orig_para_change_act_priority() on ss %d", ss, 0, 0 );
      /*lint -restore */
      return FALSE;
  }
}


/*===========================================================================

FUNCTION cmtask_change_act_priority_4

DESCRIPTION
  Changes the priority of origination parameters in the priority queue.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
 TRUE If the top pointer changed
 FALSE Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
static boolean cmtask_orig_para_change_act_priority_4(

  cm_ss_e_type               ss,
    /* SS to use for origination, if hybrid operation is changed,
    ** then ss= SD_SS_MAX.
    */

  cm_act_id_type             act_id,
    /* Type of CM activity
    */

  cm_act_priority_e_type     curr_priority,
    /* The current priority of the activity
    */

  cm_act_priority_e_type     new_priority
    /* The new priority of the activity
    */

)
{

  cm_orig_q_s_type*  orig_q_ptr ;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_ASSERT( ss != CM_SS_MAX );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( curr_priority != CM_ACT_PRIORITY_NONE )
  {
    orig_q_ptr = cmtask_orig_para_search_act_id_act_priority( ss,
                                                              act_id,
                                                              curr_priority );
  }
  else
  {
    orig_q_ptr = cmtask_orig_para_search_act_id( ss, act_id );
  }

  if( orig_q_ptr == NULL )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Remove the pointer from the Q and insert it in the correct position
  */

  (void)cmtask_orig_para_delete( ss, orig_q_ptr, FALSE );

  orig_q_ptr->orig.act_priority = new_priority;

  (void)cmtask_orig_para_insert( ss, orig_q_ptr );

  return TRUE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

} /* cmtask_orig_para_change_act_priority_4 */


/*===========================================================================

FUNCTION cmtask_orig_para_change_act_priority_3

DESCRIPTION
  Changes the priority of origination parameters in the priority queue.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
 TRUE If the top pointer changed
 FALSE Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean cmtask_orig_para_change_act_priority_3(

  cm_ss_e_type               ss,
    /* SS to use for origination, if hybrid operation is changed,
    ** then ss= SD_SS_MAX.
    */

  cm_act_id_type             act_id,
    /* Type of CM activity
    */

  cm_act_priority_e_type     curr_priority,
    /* The current priority of the activity
    */

  cm_act_priority_e_type     new_priority
    /* The new priority of the activity
    */
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_ASSERT(INRANGE( ss, CM_SS_NONE, CM_SS_MAX));
  //CM_ASSERT(BETWEEN( curr_priority, CM_ACT_PRIORITY_NONE, CM_ACT_PRIORITY_MAX));
  CM_ASSERT(BETWEEN( new_priority, CM_ACT_PRIORITY_NONE, CM_ACT_PRIORITY_MAX));
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH( "cmtask_change_act_priority act_id %d ss %d new_priority %d",
                                                act_id, ss, new_priority );

  switch(ss)
  {
    case CM_SS_MAIN :
      /* Insert on Main
      */
      return( cmtask_orig_para_change_act_priority_4( CM_SS_MAIN,
                                                      act_id,
                                                      curr_priority,
                                                      new_priority ));

    case CM_SS_HDR:
      /* Insert on HDR
      */
      return( cmtask_orig_para_change_act_priority_4( CM_SS_HDR,
                                                       act_id,
                                                       curr_priority,
                                                       new_priority ));

    case CM_SS_WLAN:
      /* Insert on WLAN
      */
      return( cmtask_orig_para_change_act_priority_4( CM_SS_WLAN,
                                                      act_id,
                                                      curr_priority,
                                                      new_priority ));

    case CM_SS_MAX:
      {
        boolean is_priority_changed = FALSE;
        is_priority_changed = cmtask_orig_para_change_act_priority_4( CM_SS_MAIN,
                                                            act_id,
                                                            curr_priority,
                                                            new_priority );

        is_priority_changed = cmtask_orig_para_change_act_priority_4( CM_SS_HDR ,
                                                            act_id,
                                                            curr_priority,
                                                            new_priority )
                             || is_priority_changed;

        is_priority_changed = cmtask_orig_para_change_act_priority_4( CM_SS_WLAN ,
                                                            act_id,
                                                            curr_priority,
                                                            new_priority )
                             || is_priority_changed;

        return( BOOLEAN(is_priority_changed) );
      }

    default:
      CM_CASE_ERR( "Invalid ss %d", ss, 0, 0 );
      /*lint -save -e527 Unreachable
      **     CM_CASE_ERR exits when CM_DEBUG is on
      */
      CM_MSG_LOW( "cmtask_orig_para_change_act_priority_3() on ss %d", ss, 0, 0 );
      /*lint -restore */
      return FALSE;
  }
}

/*===========================================================================

FUNCTION cmtask_orig_para_reset

DESCRIPTION
  Reset the origination parameters queue

DEPENDENCIES
  cm_ph_orig_xxx_q must have already been initialized with
  cm_ph_orig_xxx_q_init().


RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

void cmtask_orig_para_reset(
  cm_ss_e_type ss
    /* The ss for which the Q is to be reset
    */
)
{

  cm_orig_q_s_type* q_ptr;
  cm_orig_q_s_type* q_next_ptr;

  CM_ASSERT( ss != CM_SS_MAX );

  /* Go through the orig queues and if a entry is found
  ** delete it
  */

  /* Go through the q
  */
  q_ptr = cmtask_orig_para_get_first( ss );

  while(q_ptr != NULL)
  {
    q_next_ptr = cmtask_orig_para_get_next( q_ptr, ss );

    /* Remove this q_ptr from the queue
    */
    (void)cmtask_orig_para_delete( ss, q_ptr, TRUE );
    q_ptr = q_next_ptr;
  }

}/* cmtask_orig_para_reset*/




/*===========================================================================
=============================================================================
==================== DEM Callback registration BLOCK ========================
=============================================================================
===========================================================================*/

#if defined (FEATURE_APPS_POWER_COLLAPSE)
#error code not present
#endif

/*===========================================================================
=============================================================================
==================== SYSTEM INDICATORS INTERFACE BLOCK ======================
=============================================================================
===========================================================================*/

/* This block group all the functionality that is associated with system
** indicators interfacing.
*/


/*===========================================================================

FUNCTION cm_si_cb

DESCRIPTION
  Service Indicators callback function.

  CM registers this function with System Determination (SD) during power-up
  initialization.

  SD calls on this function whenever one or more of the service indicators
  changes its value.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cm_si_cb(

  sd_ss_e_type              ss,
        /* System selection - MAIN or HDR.
        */
    const sd_si_info_s_type  *si_info_ptr
        /* A const pointer to a structure containing the current values of
        ** the service indicators */
)
/*lint -esym(765,cm_si_cb)
** Can't be static, callback
*/
{
  cm_sd_rpt_u_type *cm_rpt_ptr = cm_sd_rpt_get_buf_else_err_fatal();
      /* Try allocating a CM report buffer */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( si_info_ptr != NULL )

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue a service indicator command to CM.
  */
  cm_rpt_ptr->srv_ind_info.hdr.cmd  = CM_SRV_IND_INFO_F;
  cm_rpt_ptr->srv_ind_info.ss       = ss;
  cm_rpt_ptr->srv_ind_info.si_info  = *si_info_ptr;

  /* Convert the internal type.
  */
  if ( cm_rpt_ptr->srv_ind_info.si_info.srv_status ==
                                  SYS_SRV_STATUS_LIMITED_REGIONAL
     )
  {
    cm_rpt_ptr->srv_ind_info.si_info.srv_status =
                                        SYS_SRV_STATUS_LIMITED;
  }

  cm_sd_rpt( cm_rpt_ptr );
}




/*===========================================================================

FUNCTION cm_si_acq_fail_cb

DESCRIPTION
  Acquisition Failed callback function.

  CM registers this function with System Determination (SD) during power-up
  initialization.

  SD calls on this function whenever acquisition fails (i.e., SD have
  attempted full acquisitions on all systems with no success).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cm_si_acq_fail_cb(

  sd_ss_e_type              ss
        /* System selection - MAIN or HDR.
        */
)
{

  cm_sd_rpt_u_type *cm_rpt_ptr = cm_sd_rpt_get_buf_else_err_fatal();
      /* Try allocating a CM report buffer */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue an ok to originate command to CM.
  */
  cm_rpt_ptr->acq_fail.hdr.cmd  = CM_ACQ_FAIL_F;
  cm_rpt_ptr->acq_fail.ss       = ss;

  cm_sd_rpt( cm_rpt_ptr );
}





/*===========================================================================

FUNCTION cm_si_ok_to_orig_cb

DESCRIPTION
  Ok To Orig callback function.

  CM registers this function with System Determination (SD) during power-up
  initialization.

  SD calls on this function whenever the Ok To Orig status changes from FALSE
  to TRUE while the origination mode is other than none.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cm_si_ok_to_orig_cb(

  sd_ss_e_type                   ss,
        /* System selection - MAIN or HDR.
        */
  const sd_si_ok_to_orig_s_type  *si_ok_to_orig_ptr
        /* System indications related to call origination
        */

)
{
  cm_sd_rpt_u_type *cm_rpt_ptr = cm_sd_rpt_get_buf_else_err_fatal();
    /* Try allocating a CM report buffer */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(si_ok_to_orig_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Queue an ok to originate command to CM.
  */
  cm_rpt_ptr->ok_to_orig.hdr.cmd        = CM_OK_TO_ORIG_F;
  cm_rpt_ptr->ok_to_orig.ss             = ss;
  cm_rpt_ptr->ok_to_orig.si_ok_to_orig  = *si_ok_to_orig_ptr;

  cm_sd_rpt( cm_rpt_ptr );

}




/*===========================================================================

FUNCTION emerg_callbck_mode_cb

DESCRIPTION
  Emergecy Callback Mode callback function.

  CM registers this function with System Determination (SD) during power-up
  initialization.

  SD calls on this function whenever the Emergecy Callback Mode status
  changes from FALSE to TRUE.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void emerg_callbck_mode_cb( void )
/*lint -esym(765,emerg_callbck_mode_cb)
** Can't be static, callback
*/
{

  cm_sd_rpt_u_type *cm_rpt_ptr = cm_sd_rpt_get_buf_else_err_fatal();
      /* Try allocating a CM report buffer */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue emergency callback command to CM.
  */
  cm_rpt_ptr->e911_callback.hdr.cmd = CM_E911_CALLBACK_F;
  cm_sd_rpt( cm_rpt_ptr );

}




/*===========================================================================

FUNCTION autonam_change_cb

DESCRIPTION
  Auto-NAM change callback function.

  CM registers this function with System Determination (SD) during power-up
  initialization.

  SD calls on this function whenever an auto-NAM change is appropriate.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void autonam_change_cb(

    sd_nam_e_type            nam
        /* The NAM that is proposed by the auto-NAM functionality */
)
/*lint -esym(765,autonam_change_cb)
** Can't be static, callback
*/
{

  cm_sd_rpt_u_type *cm_rpt_ptr = cm_sd_rpt_get_buf_else_err_fatal();
      /* Try allocating a CM report buffer */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue auto-NAM change command to CM.
  */
  cm_rpt_ptr->nam_changed.hdr.cmd = CM_NAM_CHANGED_F;
  cm_rpt_ptr->nam_changed.nam     = (byte) nam;
  cm_sd_rpt( cm_rpt_ptr );

}



/*===========================================================================

FUNCTION cm_si_sim_state_cb

DESCRIPTION
  SIM state changed callback function.

  CM registers this function with System Determination (SD) during power-up
  initialization.

  SD calls on this function whenever SIM state changes.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cm_si_sim_state_cb(

     sys_sim_state_e_type       sim_state
        /* New sim State.
        */
)
/*lint -esym(715,sim_state)*/
/*lint -esym(765,cm_si_sim_state_cb)
** Can't be static, callback
*/
{
  /* Try allocating a CM report buffer */
  #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
  #endif
}
/*lint +esym(715,sim_state)*/



/*===========================================================================

FUNCTION cm_si_reg_done_cb

DESCRIPTION
  Registration done callback function.

  CM registers this function with System Determination (SD) during power-up
  initialization.

  SD calls on this function whenever registration is done.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cm_si_reg_done_cb(
     sd_ss_e_type              ss,
        /* System selection - MAIN or HDR.
        */


     sd_mode_e_type           mode
        /* Mode in which registration was done.
        */
)
/*lint -esym(715,ss)*/
/*lint -esym(715,mode)*/
/*lint -esym(765,cm_si_reg_done_cb)
** Can't be static, callback
*/
{
 #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
 /* Should be modified to inform MMOC instead.
 */
 if ( mode == SD_MODE_GSM )
 {
   /* Reset CDMA reg. state.
   */
    mccreg_reset_reg();
 }
 #endif
}
/*lint +esym(715,ss)*/
/*lint +esym(715,mode)*/


/*===========================================================================

FUNCTION cm_si_data_suspend_cb

DESCRIPTION
  Data suspend state changed callback function.

  CM registers this function with System Determination (SD) during power-up
  initialization.

  SD calls on this function whenever data suspend state changes.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cm_si_data_suspend_cb(

     boolean             data_suspend
        /* New Data suspend state.
        */
)
/*lint -esym(715,data_suspend) */
{
  #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
  #endif
} /* cm_si_data_suspend_cb() */
/*lint +esym(715,data_suspend) */



/*===========================================================================

FUNCTION cm_si_network_list_cb

DESCRIPTION
  SIM state changed callback function.

  CM registers this function with System Determination (SD) during power-up
  initialization.

  SD calls on this function whenever SIM state changes.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cm_si_network_list_cb
(

    sys_plmn_id_s_type       *rplmn_ptr,
       /* Pointer to a structure containing the last registered PLMN id.
       */

    sys_detailed_plmn_list_s_type *plmn_list_ptr,
       /* Pointer to a structure containing the list of available PLMNs.
       */

    sys_plmn_list_status_e_type    plmn_list_status
       /* Gives the status of plmn list request (success/abort).
       */

)
/*lint -esym(818, rplmn_ptr) */
/*lint -esym(818, plmn_list_ptr)
** Can't be const as sd_si_reg() declaration would have to change
*/
/*lint -esym(715, rplmn_ptr) */
/*lint -esym(715, plmn_list_ptr */
/*lint -esym(715, plmn_list_status */
{
  #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
  #endif

}
/*lint +esym(818, rplmn_ptr) */
/*lint +esym(818, plmn_list_ptr) */
/*lint +esym(715, rplmn_ptr) */
/*lint +esym(715, plmn_list_ptr */
/*lint +esym(715, plmn_list_status */

/*===========================================================================

FUNCTION cm_si_bss_list_cb

DESCRIPTION
  WLAN BSS List callback function.

  CM registers this function with System Determination (SD) during power-up
  initialization.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cm_si_bss_list_cb(

  sys_wlan_bss_info_list_s_type       *bss_list_ptr
       /* Pointer to a structure containing the WLAN BSS list.
       */
)
/*lint -esym(765,cm_si_bss_list_cb)
** Can't be static, callback
*/
/*lint -esym(818,bss_list_ptr)
** Can't be const as then sd_si_reg() would need to change
*/
{
  cm_sd_rpt_u_type *cm_rpt_ptr = cm_sd_rpt_get_buf_else_err_fatal();
      /* Try allocating a CM report buffer */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(bss_list_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Queue a network list report to CM.
  */

  cm_rpt_ptr->wlan_bss_list.hdr.cmd = CM_WLAN_BSS_LIST_F;
  cm_rpt_ptr->wlan_bss_list.list    = *bss_list_ptr;

  cm_sd_rpt( cm_rpt_ptr );

} /* cm_si_bss_list_cb() */
/*lint +esym(818,bss_list_ptr) */

/*===========================================================================

FUNCTION cm_si_wlan_wps_result_cb

DESCRIPTION
  WLAN WPS Start callback function.

  CM registers this function with System Determination (SD) during power-up
  initialization.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cm_si_wlan_wps_result_cb(

   const sys_wlan_wps_result_s_type             *wlan_wps_result_ptr
       /**< Pointer to a structure containing the result of a WPS procedure.
       */
)
/*lint -esym(765,cm_si_bss_list_cb)
** Can't be static, callback
*/ 
/*lint -esym(818,bss_list_ptr)
** Can't be const as then sd_si_reg() would need to change 
*/ 
{
  #ifdef FEATURE_WLAN_WPS
#error code not present
  #else
  SYS_ARG_NOT_USED(wlan_wps_result_ptr);
  #endif

} /* cm_si_wlan_wps_result_cb() */


#ifdef FEATURE_WCDMA
#error code not present
#endif


#if defined(FEATURE_WCDMA)
#error code not present
#endif


#if (defined(FEATURE_GSM) && defined(FEATURE_GSM_RR_CELL_CALLBACK))
#error code not present
#endif

/*===========================================================================

FUNCTION cm_srv_lost_cb

DESCRIPTION
  Acquisition Failed callback function.

  CM registers this function with System Determination (SD) during power-up
  initialization.

  SD calls on this function whenever acquisition fails (i.e., SD have
  attempted full acquisitions on all systems with no success).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cm_srv_lost_cb(

  sd_ss_e_type              ss
        /* System selection - MAIN or HDR.
        */
)
{

  cm_sd_rpt_u_type *cm_rpt_ptr = cm_sd_rpt_get_buf_else_err_fatal();
      /* Try allocating a CM report buffer */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue an ok to originate command to CM.
  */
  cm_rpt_ptr->srv_lost.hdr.cmd  = CM_SRV_LOST_F;
  cm_rpt_ptr->srv_lost.ss       = ss;

  cm_sd_rpt( cm_rpt_ptr );
}


/*===========================================================================

FUNCTION cm_si_init

DESCRIPTION
  Initialize the CM system indicators object.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cm_si_init( void )
{
  /* Register callback functions with the Service Indicators component of
  ** System Determination (SD).
  **
  ** SD calls on the Service Indicators callback function whenever one or
  ** more of the service indicators changes their value.
  **
  ** SD calls on the Ok To Orig callbck function whenever the Ok To Orig
  ** status changes from FALSE to TRUE while the origination mode is other
  ** than none.
  **
  ** SD calls on the Emergency Callback Mode callbck function whenever the
  ** Emergency Callback Mode status changes from FALSE to TRUE.
  **
  ** NOTE! A new call to this function overrides all previously registered
  ** callback functions.
  */
  sd_si_reg( cm_si_cb,
             cm_si_acq_fail_cb,
             cm_si_ok_to_orig_cb,
             emerg_callbck_mode_cb,
             autonam_change_cb,
             cm_si_network_list_cb,
             cm_si_sim_state_cb,
             cm_si_reg_done_cb,
             cm_si_data_suspend_cb,
             cm_si_bss_list_cb,
             cm_si_wlan_wps_result_cb,
             cm_srv_lost_cb
           );

} /* cm_si_init() */


#if defined (FEATURE_APPS_POWER_COLLAPSE)
#error code not present
#endif


/*===========================================================================
=============================================================================
============================ CM INTERFACE BLOCK =============================
=============================================================================
===========================================================================*/

#ifdef FEATURE_DDTM_CNTL
/*===========================================================================
FUNCTION cm_dip_switch_ddtm_cmd_cb

DESCRIPTION
  Called by Call Manager to indicate status
  of DDTM dip switch command

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
static void cm_dip_switch_ddtm_cmd_cb (
  void*  data_block_ptr,
  cm_ph_cmd_e_type cmd,
  cm_ph_cmd_err_e_type cmd_err
)
/*lint -esym(715,cmd) */
/*lint -esym(715,data_block_ptr) */
/*lint -esym(818,data_block_ptr)
** Can't be const as then cm_ph_cmd_ddtm_pref() would need to change
*/
{

  /* We don't use it */
  CM_ASSERT(data_block_ptr == NULL);

  switch(cmd_err) {
    case CM_PH_CMD_ERR_NOERR:
      return;

    case CM_PH_CMD_ERR_DDTM_PREF_P:
    case CM_PH_CMD_ERR_NO_BUF_L:
      CM_MSG_ERROR("DIPSW DDTM failed - %d",cmd_err,0,0);
      break;

    default:
      MSG_ERROR("Unexpected error - %d",cmd_err,0,0);
  }
}
/*lint +esym(715,cmd) */
/*lint +esym(715,data_block_ptr) */
/*lint +esym(818,data_block_ptr) */

/*===========================================================================
FUNCTION cm_dip_switch_set_ddtm_on

DESCRIPTION
  Registered by CM with HDR log object to be called
  when DDTM dip switch is set

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
void cm_dip_switch_set_ddtm_on(void)
{

  sys_srv_opt_type so_list[1] = { (sys_srv_opt_type)CAI_SO_NULL };

  CM_MSG_HIGH("DIPSW CM_DDTM_PREF_ON request",0,0,0);
  (void) cm_ph_cmd_ddtm_pref(
                      cm_dip_switch_ddtm_cmd_cb,
                      NULL,
                      CM_CLIENT_ID_ANONYMOUS,
                      CM_DDTM_PREF_ON,
                      SYS_DDTM_ACT_MASK_ALL,
                      CM_DDTM_SO_LIST_ACT_ADD,
                      ARR_SIZE(so_list),
                      so_list
                     );

}

/*===========================================================================
FUNCTION cm_dip_switch_set_ddtm_off

DESCRIPTION
  Registered by CM with HDR log object to be called
  when DDTM dip switch is reset

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
void cm_dip_switch_set_ddtm_off(void)
{
  sys_srv_opt_type so_list[5] =
                            {
                              (sys_srv_opt_type)CAI_SO_ASYNC_DATA_PRE707,
                              (sys_srv_opt_type)CAI_SO_G3_FAX_PRE707,
                              (sys_srv_opt_type)CAI_SO_ASYNC_DATA_13K_PRE707,
                              (sys_srv_opt_type)CAI_SO_G3_FAX_13K_PRE707,
                              (sys_srv_opt_type)CAI_SO_PPP_PKT_DATA_3G
                            };

  CM_MSG_HIGH("DIPSW CM_DDTM_PREF_DEFAULT request",0,0,0);
  (void) cm_ph_cmd_ddtm_pref(
                      cm_dip_switch_ddtm_cmd_cb,
                      NULL,
                      CM_CLIENT_ID_ANONYMOUS,
                      CM_DDTM_PREF_ON,
                      SYS_DDTM_ACT_MASK_IGN_SO_PAGES,
                      CM_DDTM_SO_LIST_ACT_REPLACE,
                      ARR_SIZE(so_list),
                      so_list
                     );

}
#endif /* FEATURE_DDTM_CNTL */

/*===========================================================================

FUNCTION cmtask_crit_sect_ptr

DESCRIPTION

  Return pointer to CM critical section.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
rex_crit_sect_type* cmtask_crit_sect_ptr(void)
{
  static rex_crit_sect_type crit_sect;
    /* The one and only CM critical section */

  return &crit_sect;

}

/*===========================================================================

FUNCTION cmtask_init_crit_sect

DESCRIPTION
  Initialize CM critical section.
  This function must be called before any other part of CM is initialized

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmtask_init_crit_sect(void)
{

  rex_init_crit_sect(cmtask_crit_sect_ptr());

}

/*===========================================================================

FUNCTION cm_init_before_task_start

DESCRIPTION
  Initialize CM queues and other objects that needs to be initialized
  before we report back to MC that CM is ready to run by calling on
  mc_task_start().

  NOTE! Any initialization that must be done before other tasks or clients
  tries to contact CM, must be done in this initialization function,
  as opposed to the cm_init_after_task_start() function.

  This function must be called before any other CM function is ever called.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cm_init_before_task_start( void )
{

  CM_FUNC_START( "cm_init_before_task_start()",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize CM critical section
  */
  cmtask_init_crit_sect();

  /* Initialize CM client critical section
  */
  cmclient_init_crit_sect();

  /* Initialize the client map array.
  */
  cmclient_map_init();

  /* Initialize CM object internal to this file
  */
  cmclient_free_q_init();

  /* Initialize the client list object.
  */
  cmclient_list_init();

  /* Initialize the NV queue object */
  cmnv_init();

  /* Initialize the client command queue object.
  */
  cm_client_cmd_q_init();

  /* Initialize the number-classfication interface object.
  */
  cm_nc_init();

  /* Initialize the M51 mode settings object
  */
  cm_m51_mode_init();

  #ifdef FEATURE_GPSONE_NEW_L2_ACK
#error code not present
  #endif /* FEATURE_GPSONE_NEW_L2_ACK */

  /* Initialize the client command queue object.
  */
  cm_rpt_q_init();

  /* Initialize the hold command queue object.
  */
  cm_hold_cmd_q_init();

  /* Initialize the origination parameters queue
  */

  cm_orig_para_main_free_q_init();
  cm_orig_para_hdr_free_q_init();
  cm_orig_para_wlan_free_q_init();

  cm_ph_orig_para_main_q_init();
  cm_ph_orig_para_hdr_q_init();
  cm_ph_orig_para_wlan_q_init();

  /*
  ** Initialize CM Heap.
  */

  mem_init_heap(&cm_heap,
                cm_heap_mem_buffer,
                CM_HEAP_BUFFER_SIZE,
                NULL);
  cm_heap_slots_left = CM_HEAP_SLOT_COUNT;
  cm_tmc_heap_blocks = 0;
} /* cm_init_before_task_start() */



/*===========================================================================

FUNCTION cm_init_after_task_start

DESCRIPTION
  Initialize CM objects that needs to be initialized AFTER we report back
  to MC that CM is ready to run by calling on mc_task_start().

  NOTE! Any initialization that result in CM contacting some other task,
  such as NV or sound, must be done in this initialization function, as
  opposed to the cm_init_before_task_start() function.

  This function must be called after cm_init_before_task_start() and
  before any other CM function is ever called.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cm_init_after_task_start( void )
{

  CM_FUNC_START( "cm_init_after_task_start()",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the NV interface.
  */
  cmnv_init();

  #if (defined (FEATURE_WCDMA) || defined (FEATURE_GSM))
#error code not present
  #endif /* FEATURE_WCDMA || FEATURE_GSM */

  /* Initialize the phone object.
  */
  cmph_init();


  /* Initialize the call object.
  */
  cmcall_init();

  /* Initialize the serving system object.
  */
  cmss_init();

  #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
  /* Initialize the SMS object.
  */
  cmsms_init();
  #endif /* (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */

  #ifdef FEATURE_GPSONE
#error code not present
  #endif /* FEATURE_GPSONE */

  /* Initialize the CM system indicators object.
  */
  cm_si_init();

  #if defined (FEATURE_APPS_POWER_COLLAPSE)
#error code not present
  #endif

  #ifdef FEATURE_DBM
  cmdbm_init();
  #endif
  
  #if (defined (FEATURE_MMGSDI) && defined (FEATURE_CM_MMGSDI) )  
  CM_MSG_HIGH("In cm_init_after_task_start(), cmmmgsdi_init()", 0, 0, 0);
  /* Initialize MMGSDI event callback */
  cmmmgsdi_init();
  #else
  #if (defined (FEATURE_WCDMA) || defined (FEATURE_GSM))
#error code not present
  #endif  /* (defined (FEATURE_WCDMA) || defined (FEATURE_GSM))*/
  #endif

  #if (defined (FEATURE_WCDMA) || defined (FEATURE_GSM))
#error code not present
  #endif /* FEATURE_WCDMA || FEATURE_GSM */

  /* Register call orig/end command */
  cmdiag_init ();

  /* Initialize DIAG logging client and register subsystem command */
  cmlog_init ();

  /* Init CM PREF module.
  */
  cmpref_init();

  #ifdef FEATURE_DDTM_CNTL
  /* Register callbacks with HDR that will turn DDTM on / off */
  hdrlog_set_dipsw_cb( HDRLOG_DIPSW_DDTM, cm_dip_switch_set_ddtm_on,
    cm_dip_switch_set_ddtm_off);
  #endif /* FEATURE_DDTM_CNTL */

  #if (defined FEATURE_BCMCS || defined FEATURE_MODEM_MBMS)
  /* Initialize the BCMCS object.
  */
  cmbcmcs_init();
  #endif /* FEATURE_BCMCS || MBMS */

  #if (defined FEATURE_ALS) && (defined FEATURE_GSM)
#error code not present
  #endif /* #if (defined FEATURE_ALS) && (defined FEATURE_GSM) */

  #if defined(FEATURE_IP_CALL)
  /* Initialize CM IP APP interface
  */
  cmipapp_init ();

  #endif  /* FEATURE_IP_CALL */

} /* cm_init_after_task_start() */


/*===========================================================================

FUNCTION cm_ret_rpt_buf

DESCRIPTION
  Dispose of a report buffer which is done being processed. Used for MC, SD
  and IP rpts free up currently.

DEPENDENCIES
  done_q_ptr

RETURN VALUE
  None

SIDE EFFECTS
  Can signal the task which is associated with the command.

===========================================================================*/
static void cm_ret_rpt_buf
(
  cm_hdr_type  *rpt_buf,
    /* Queue to place completed command on.  NULL, if no "done queue" */

  rex_tcb_type *task_ptr,
    /* Task pointer to signal when the command is done, NULL == no signal */

  rex_sigs_type sigs
    /* Signals to send *task_ptr if it is non-NULL */
)
{
  /* Enqueue item onto done queue if requested (done_q_ptr != NULL)
  ** Do this before we signal the task so that it has its buffer back
  ** before we call its attention to the fact that the operation has
  ** completed.
  */
  if ( rpt_buf != NULL ) {
    cm_mem_free(rpt_buf);
  }

  /* If the task pointer is non-NULL, signal the task that the tone is done.
  ** Note: The pointer was either passed as NULL, or was set to NULL because
  ** the signal was sent elsewhere.
  */
  if ( task_ptr != NULL ) {
    (void) rex_set_sigs( task_ptr, sigs );
  }

} /* end of cm_ret_rpt_buf() */


/*===========================================================================

FUNCTION cm_sdrpt_handle

DESCRIPTION
  Process SD reports.
  The pointer being passed is really a SD report pointer. The cm_hdr_type
  pointer is used here so that all report processing functions could have
  the same signature. The cm_hdr_type pointer could be compared to a base
  class pointer.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  NONE.

SIDE EFFECTS
  none

===========================================================================*/
static void cm_sdrpt_handle( cm_hdr_type       *sd_rpt_ptr )
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(sd_rpt_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Print the message */
  /*lint -e{826} */
  CMDBG_PRINT_SD_RPT_FUNC( (cm_sd_rpt_u_type *) sd_rpt_ptr );

  /* Do phone, call, serving system, SMS, & PD context processing.
  */
  cmph_sd_rpt_proc    (sd_rpt_ptr);
  cmcall_sd_rpt_proc  (sd_rpt_ptr);
  cmss_sd_rpt_proc    (sd_rpt_ptr);

  #ifdef FEATURE_CDSMS
  /*lint -e{826} */
  cmsms_sd_rpt_proc(sd_rpt_ptr);
  #endif /* FEATURE_CDSMS */

  #if (defined (FEATURE_GPSONE_MSBASED) || defined (FEATURE_GPSONE))
#error code not present
  #endif /* FEATURE_GPSONE_MSBASED */ 

  #ifdef FEATURE_DBM
  cmdbm_sd_rpt_proc(sd_rpt_ptr);
  #endif /* FEATURE_DBM */

  #ifdef FEATURE_BCMCS
#error code not present
  #endif /* FEATURE_BCMCS */

  cm_ret_rpt_buf( sd_rpt_ptr,
                    sd_rpt_ptr->task_ptr,
                    sd_rpt_ptr->sigs );

} /* cm_sdrpt_handle() */

/*===========================================================================

FUNCTION cm_sdrpt_handle

DESCRIPTION
  Process SD reports.
  The pointer being passed is really a SD report pointer. The cm_hdr_type
  pointer is used here so that all report processing functions could have
  the same signature. The cm_hdr_type pointer could be compared to a base
  class pointer.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  NONE.

SIDE EFFECTS
  none

===========================================================================*/
static void cm_genericrpt_handle( cm_hdr_type       *generic_rpt_ptr )
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(generic_rpt_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Print the message */
  /*lint -save -e826 */
  CMDBG_PRINT_GENERIC_RPT_FUNC( (cm_generic_rpt_s_type *) generic_rpt_ptr );
  /*lint -restore */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  #ifdef FEATURE_IMS_VCC
  if ( generic_rpt_ptr->cmd == CM_WLAN_RPT )
  {
    /** Report the new WLAN link quality provided by WLAN
    */
    cmss_report_wlan_link_quality();
  }
  else
  #endif
  {
    /* Phone object needs to be updated for generic reports
    */
    cmph_generic_rpt_proc(generic_rpt_ptr);
  }

  cm_ret_rpt_buf( generic_rpt_ptr,
                  generic_rpt_ptr->task_ptr,
                  generic_rpt_ptr->sigs );

} /* cm_genericrpt_handle() */


#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
#endif /* (defined(FEATURE_WCDMA) || defined(FEATURE_GSM)) */


/*===========================================================================

FUNCTION cm_iprpt_handle

DESCRIPTION
  Process app reports
  The pointer being passed is really a IPAPP report pointer. The cm_hdr_type
  pointer is used here so that all report processing functions could have
  the same signature. The cm_hdr_type pointer could be compared to a base
  class pointer.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  NONE.

SIDE EFFECTS
  none

===========================================================================*/
static void cm_iprpt_handle( cm_hdr_type   *ip_rpt_ptr )
{

  CM_ASSERT(ip_rpt_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_MSG_LOW("cm_iprpt_handle() ",0,0,0 );

  #if defined(FEATURE_IP_CALL)
  /* Call Voip app call processing function
  */
  cmipcall_rpt_proc (ip_rpt_ptr);

  /* Calling Ph function to
  ** execute IP related reports.
  */
  cmph_ip_rpt_proc  (ip_rpt_ptr);

  #endif /* FEATURE_IP_CALL */

  /* Returns rpt buffer back to buffer pool
  */
  cm_ret_rpt_buf( ip_rpt_ptr,
                    ip_rpt_ptr->task_ptr,
                    ip_rpt_ptr->sigs );

} /* cm_iprpt_handle() */


/*===========================================================================

FUNCTION cm_ipsmsrpt_handle

DESCRIPTION
  Process app reports
  The pointer being passed is really a IPAPP report pointer. The cm_hdr_type
  pointer is used here so that all report processing functions could have
  the same signature. The cm_hdr_type pointer could be compared to a base
  class pointer.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  NONE.

SIDE EFFECTS
  none

===========================================================================*/
static void cm_ipsmsrpt_handle( cm_hdr_type   *ip_rpt_ptr )
{

  CM_ASSERT(ip_rpt_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_MSG_LOW("cm_ipsmsrpt_handle() ",0,0,0 );

  #if defined(FEATURE_SMS_OVER_IP)
  /* Call Voip app call processing function
  */
  cmipsms_rpt_proc (ip_rpt_ptr);

  #endif /* FEATURE_IP_CALL */

  /* Returns rpt buffer back to buffer pool
  */
  cm_ret_rpt_buf( ip_rpt_ptr,
                    ip_rpt_ptr->task_ptr,
                    ip_rpt_ptr->sigs );

} /* cm_ipsmsrpt_handle() */


#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
/*===========================================================================

FUNCTION cm_xrpt_handle

DESCRIPTION
  Process MC reports
  The pointer being passed is really a MC report pointer. The cm_hdr_type
  pointer is used here so that all report processing functions could have
  the same signature. The cm_hdr_type pointer could be compared to a base
  class pointer.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  NONE.

SIDE EFFECTS
  none

===========================================================================*/
static void cm_xrpt_handle( cm_hdr_type       *mc_rpt_ptr )
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(mc_rpt_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Print the message and call id (if applicable) */
  /*lint -save -e826 */
  CMDBG_PRINT_MC_RPT_FUNC( (cm_mc_rpt_type *) mc_rpt_ptr );
  /*lint -restore */

  /* Do phone, call, and serving system inband and SMS context processing.
  */
  cmph_mc_rpt_proc    (mc_rpt_ptr);
  cmcall_mc_rpt_proc  (mc_rpt_ptr);
  cmss_mc_rpt_proc    (mc_rpt_ptr);
  cminband_mc_rpt_proc(mc_rpt_ptr);
  cmsms_mc_rpt_proc   (mc_rpt_ptr);

  #ifdef FEATURE_DBM
  cmdbm_mc_rpt_proc    (mc_rpt_ptr);
  #endif /* FEATURE_DBM */

  #if (defined (FEATURE_GPSONE_MSBASED) || defined (FEATURE_GPSONE))
#error code not present
  #endif /* FEATURE_GPSONE_MSBASED */

  #ifdef FEATURE_BCMCS
#error code not present
  #endif /* FEATURE_BCMCS */

  cm_ret_rpt_buf( mc_rpt_ptr,
                    mc_rpt_ptr->task_ptr,
                    mc_rpt_ptr->sigs );

} /* cm_xrpt_handle() */
#endif /* (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */



/*===========================================================================

FUNCTION cm_rpt_handle

DESCRIPTION
 Process lower layer reports

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  NONE.

SIDE EFFECTS
  none

===========================================================================*/
/*lint -esym(765, cm_rpt_handle)
** CM unit test uses it so it can't be static
*/
void cm_rpt_handle( void )
{
  cm_hdr_type       *rpt_ptr;

  /*-----------------------------------------------------------------------*/

  CM_MSG_LOW("cm_rpt_handle() ",0,0,0 );

  /*
  ** Dispatch the LL report to proper handler.
  */
  while( (rpt_ptr = (cm_hdr_type *) cm_rpt_q_get()) != NULL )
  {

    if ( CMLL_RPT_IS_SD( rpt_ptr->cmd ) )
    {
      cm_sdrpt_handle( rpt_ptr );
    }

    #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
    else if ( CMLL_RPT_IS_1X( rpt_ptr->cmd ) )
    {
      cm_xrpt_handle( rpt_ptr );
    }
    #endif /* (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */

    #if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
    #endif /* (defined(FEATURE_WCDMA) || defined(FEATURE_GSM)) */

    else if ( CMLL_RPT_IS_IP( rpt_ptr->cmd ) )
    {

      cm_iprpt_handle ( rpt_ptr);
    }

    else if ( CMLL_RPT_IS_IPSMS( rpt_ptr->cmd ) )
    {

      cm_ipsmsrpt_handle ( rpt_ptr);
    }

    else if ( CMLL_RPT_IS_GENERIC( rpt_ptr->cmd ) )
    {
      cm_genericrpt_handle( rpt_ptr );
    }

    else
    {
      CM_ERR_FATAL( "Invalid LL CM report",0,0,0 );
    }
  } /* while( (rpt_ptr = cm_rpt_q_get()) != NULL ) */

} /* cm_rpt_handle() */



/*===========================================================================

FUNCTION cm_client_cmd_handle

DESCRIPTION
  Handle CM clients commands.
  CM should call on this function when CM_CLIENT_CMD_Q_SIG is set.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_client_cmd_handle( void )
/*lint -esym(765,cm_client_cmd_handle)
** Can't be static, CM unit test uses it
*/
{

  cm_cmd_s_type    *cmd_ptr;
      /* CM command pointer */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_FUNC_START( "cm_client_cmd_handle()",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If phone or call objects is currently waiting for
  ** a reply from MC, do not process clients commands.
  */

  cmd_ptr = cm_client_cmd_q_peek();           /* Point at the next command */
  if ( cmd_ptr != NULL )
  {
    if ( cm_reply_should_cmd_wait( cmd_ptr ) )
    {
      CM_MSG_HIGH(" Command type %d command %d waiting for reply", 
                              cmd_ptr->cmd_type, cmd_ptr->cmd.ph.cmd, 0 );
      return;
    }
  } /* if ( cmd_ptr != NULL ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else, process client commands
  ** in a FIFO order.
  */
  while( (cmd_ptr = cm_client_cmd_q_peek()) != NULL )
  {

    if ( cm_reply_should_cmd_wait( cmd_ptr ) )
    {
      CM_MSG_HIGH(" Command type %d command %d waiting for reply", 
                              cmd_ptr->cmd_type, cmd_ptr->cmd.ph.cmd, 0 );
      break;
    }

    if( (cmd_ptr = cm_client_cmd_q_get()) == NULL )
    {
      break;
    }

    CMDBG_PRINT_CM_CMD_FUNC( cmd_ptr );

    /* Assume the command buffer is not going to be reused.
    */
    cmd_ptr->is_reused = FALSE;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Decide on processing according to the command type.
    */
    switch ( CMD_TYPE(cmd_ptr) )
    {
                       /*------------------------------------*/
                       /*     Client phone command           */
                       /*------------------------------------*/

      case CM_CMD_TYPE_PH:
        cmd_ptr->cmd.ph.client_ptr =
          cmclient_get_client_ptr( cmd_ptr->cmd.ph.client_id );
        cmph_client_cmd_proc( cmd_ptr );
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                       /*------------------------------------*/
                       /*     Client call command            */
                       /*------------------------------------*/

      case CM_CMD_TYPE_CALL:
        cmd_ptr->cmd.call.client_ptr =
          cmclient_get_client_ptr( cmd_ptr->cmd.call.client_id );
        cmcall_client_cmd_proc( cmd_ptr );
        break;


                       /*------------------------------------*/
                       /*     Client inband command          */
                       /*------------------------------------*/

      case CM_CMD_TYPE_INBAND:
        cmd_ptr->cmd.inband.client_ptr =
          cmclient_get_client_ptr( cmd_ptr->cmd.inband.client_id );
          cminband_client_cmd_proc( cmd_ptr );
        break;


                       /*------------------------------------*/
                       /*     Client serving system command  */
                       /*------------------------------------*/

      case CM_CMD_TYPE_SS:
        cmd_ptr->cmd.ss.client_ptr =
          cmclient_get_client_ptr( cmd_ptr->cmd.ss.client_id );
        cmss_client_cmd_proc( cmd_ptr );
        break;

      #ifdef FEATURE_CDSMS
                       /*------------------------------------*/
                       /*     Client SMS command             */
                       /*------------------------------------*/

      case CM_CMD_TYPE_SMS:
        cmd_ptr->cmd.sms.client_ptr =
          cmclient_get_client_ptr( cmd_ptr->cmd.sms.client_id );
        cmsms_client_cmd_proc( cmd_ptr );
        break;

      #endif /* FEATURE_CDSMS */

      #ifdef FEATURE_GPSONE
#error code not present
      #endif   /* FEATURE_GPSONE */

      #ifdef FEATURE_DBM
                      /*------------------------------------*/
                      /*     Client DBM command             */
                      /*------------------------------------*/

      case CM_CMD_TYPE_DBM:
        cmd_ptr->cmd.dbm_msg.client_ptr =
          cmclient_get_client_ptr(cmd_ptr->cmd.dbm_msg.client_id);

        cmdbm_client_cmd_proc( cmd_ptr );
        break;

      #endif   /* FEATURE_DBM */

      #if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM) )
#error code not present
      #endif /* FEATURE_WCDMA || FEATURE_GSM */


      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    #if (defined FEATURE_BCMCS || defined FEATURE_MODEM_MBMS)
                       /*------------------------------------*/
                       /*     Client BCMCS data command  */
                       /*------------------------------------*/

      case CM_CMD_TYPE_BCMCS:
        cmd_ptr->cmd.bcmcs.client_ptr =
          cmclient_get_client_ptr( cmd_ptr->cmd.bcmcs.client_id );
        cmbcmcs_client_cmd_proc( cmd_ptr );
        break;
      #endif /* FEATURE_BCMCS || MBMS */

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      
                       /*------------------------------------*/
                       /*     Client MBMS  command  */
                       /*------------------------------------*/

      case CM_CMD_TYPE_MBMS:
      #if defined (FEATURE_MODEM_MBMS)
#error code not present
      #endif   
      break;
      
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      #if (defined FEATURE_GSM) || (defined FEATURE_WCDMA)
#error code not present
      #endif /* FEATURE_GSM || FEATURE_WCDMA */

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      default:

        /* Unexpected command type.
        */
        CM_SWITCH_ERR( "Unexpected command type=%d",
                       CMD_TYPE(cmd_ptr), 0, 0 );
        break;

    } /* case */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If phone or call objects is now waiting for
    ** a reply from MC, stop processing clients commands.
    */

    if ( cm_reply_should_cmd_wait( cmd_ptr ) )
    {
      CM_MSG_HIGH(" Command type %d command %d waiting for reply", 
                              cmd_ptr->cmd_type, cmd_ptr->cmd.ph.cmd, 0 );

      break;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Else, (no need to wait on reply), deallocate command buffer
    ** and process next client command.
    */
    /* If the command is going to be reused, then don't deallocate it.
    */
    if(!cmd_ptr->is_reused)
    {
      cm_cmd_dealloc( cmd_ptr );
    }

  } /* while( (cmd_ptr = cm_client_cmd_q_get()) != NULL ) */

  CM_FUNC_END( "cm_client_cmd_handle()",0,0,0);

} /* cm_client_cmd_handle() */

/*===========================================================================

FUNCTION cm_hold_cmd_handle

DESCRIPTION
  Process commands from hold command q.

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_hold_cmd_handle(

    boolean                 chk_uptime,
    /* Check the uptime before executing the command
    */

    cm_hold_cmd_reas_e_type hold_reason
    /* Check the reason before executing the command
    */

)
{
  cm_cmd_s_type             *hold_cmd_ptr = NULL;
  cm_cmd_s_type             *next_cmd_ptr = NULL;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_FUNC_START( "cm_hold_cmd_handle()", 0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  hold_cmd_ptr = cm_hold_cmd_q_peek();

  while ( hold_cmd_ptr != NULL )
  {
    /* Assume the command buffer is not going to be reused.
    */
    hold_cmd_ptr->is_reused = FALSE;

    next_cmd_ptr = cm_hold_cmd_q_next( hold_cmd_ptr );

    /* Decide on processing according to the command type.
    */
    switch ( CMD_TYPE(hold_cmd_ptr) )
    {

      case CM_CMD_TYPE_CALL:
        cmcall_client_cmd_proc( hold_cmd_ptr );
        break;

      case CM_CMD_TYPE_PH:
        /* If hold reason is other than reason NONE and it differs from 
        ** the reason why the command was queued, dont process the command
        */
        if(hold_reason != PH_CMD_PTR(hold_cmd_ptr)->info.hold_cmd_reason &&
           hold_reason != CM_HOLD_CMD_REAS_NONE)
        {
          CM_MSG_HIGH("hold reason mismatch %d, %d",hold_reason,
                      PH_CMD_PTR(hold_cmd_ptr)->info.hold_cmd_reason,
                      0);
          hold_cmd_ptr = next_cmd_ptr;
          continue;
        }
        if( chk_uptime && PH_CMD_PTR(hold_cmd_ptr)->info.hold_cmd_uptime > clk_uptime())
        {
          CM_MSG_HIGH(" Still in hold cmd uptime: hold uptime: %d, uptime: %d",
                      PH_CMD_PTR(hold_cmd_ptr)->info.hold_cmd_uptime, clk_uptime(),0);
          hold_cmd_ptr = next_cmd_ptr;
          continue;
        }
        PH_CMD_PTR(hold_cmd_ptr)->info.is_command_on_hold = FALSE;
        cmph_client_cmd_proc( hold_cmd_ptr );
        break;

      default:
        break;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Deallocate command buffer and process next hold command.
    */
    if(hold_cmd_ptr->is_reused == FALSE)
    {
      cm_hold_cmd_q_delete( hold_cmd_ptr );
    }

    hold_cmd_ptr = next_cmd_ptr;

  } /* while */


  CM_FUNC_END( "cm_hold_cmd_handle()",0,0,0);

}

/*===========================================================================

FUNCTION cm_timer_handle

DESCRIPTION
  Handle CM timers events.
  CM should call on this function when CM_TIMER_SIG is set.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_timer_handle( cm_timer_event_type  timer_event )
/*lint -esym(765,cm_timer_handle)
** Can't be static, used by CM unit test
*/
{

  CM_FUNC_START( "cm_timer_handle()",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*  timer processing.
  **
  ** NOTE that the order of the timer processing sequence is
  ** is important :
       cmph_timer_proc
       cmcall_timer_proc
       cmxsms_timer_proc
  */

  cm_hold_cmd_handle( TRUE, CM_HOLD_CMD_REAS_NONE );
  cmph_timer_proc( timer_event );
  cmcall_timer_proc( timer_event );

  #ifdef FEATURE_CDSMS
  cmxsms_timer_proc(timer_event);
  #endif /* FEATURE_CDSMS */

  #ifdef FEATURE_DBM
  cmdbm_timer_proc(timer_event);
  #endif /* FEATURE_DBM */

  cmss_timer_proc(timer_event);

  #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
  #endif

  /* Invoke CM PREF timer handler.
  */
  cmpref_proc_timer();

  #ifdef FEATURE_GPSONE
#error code not present
  #endif

  /* If the CM client command queue is not empty and the last command was
  ** processed in the past second, then start processing the commands
  ** again.
  */
  if( cm_client_cmd_q_cnt() > 0 &&
      cm_last_cmd_proc_timestamp < clk_uptime() )
  {
    (void) rex_set_sigs( rex_self(), CM_CLIENT_CMD_Q_SIG );
  }

} /* cm_timer_handle() */



/*===========================================================================

FUNCTION cm_nv_handle

DESCRIPTION
  Handle CM NV write completions.
  CM should call on this function when CM_NV_ASYNC_SIG is set.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_nv_handle( void )
/*lint -esym(765,cm_nv_handle)
** Can't be static, used by CM unit test
*/
{
  cmnv_q_write_done_cb();
} /* cm_nv_handle() */



#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* FEATURE_WCDMA || FEATURE_GSM */



/*===========================================================================

FUNCTION CM_INIT

DESCRIPTION
  Initialize the Call Manager - called immediately after CM_TASK is
  started.  Initialize  queues, timers, etc.

  This function was static as it was used in this file only. However
  the static was removed because of CM unit test debugging tool needed to
  access it.

RETURN VALUE
  None

DEPENDENCIES
  Called only once from CM_TASK

===========================================================================*/
void cm_init( void )
/*lint -esym(765, cm_init)
** Can't be static, needed by CM unit test
*/
{
  cm_init_before_task_start( );

  /* Initialize timers */
  rex_def_timer( &cm_timer, &cm_tcb, CM_TIMER_SIG );

  rex_def_timer( &cm_rpt_timer, &cm_tcb, CM_RPT_TIMER_SIG );

  rex_def_timer( &cm_rssi_timer, &cm_tcb, CM_RSSI_TIMER_SIG );


  /* Inform MC task that CM task has finished initialization */
  #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
  #endif /* FEATURE_GSM||FEATURE_WCDMA */

  /* Wait for start signal from Main Control task */
  task_start( CM_RPT_TIMER_SIG, DOG_CM_RPT, &cm_rpt_timer );

  (void) rex_clr_sigs( rex_self(), TASK_START_SIG );

  /* Set the watchdog report timer signal so that we'll
  ** reset the watchdog as soon as we start checking for events
  ** and start our first tick timer
  */
  (void) rex_set_sigs( &cm_tcb, CM_RPT_TIMER_SIG );

  /* Set the timer */
  (void) rex_set_timer( &cm_timer, CM_TIME );

  cm_init_after_task_start( );

} /* cm_init() */



/*===========================================================================
FUNCTION CM_KICK_DOG

DESCRIPTION
  Kick the watchdog and set the watchdog timer.

DEPENDENCIES
  None.

RETURN VALUE
  Never exits.

SIDE EFFECTS

===========================================================================*/
void cm_kick_dog(void)
{
  dog_report( DOG_CM_RPT );
  (void) rex_set_timer( &cm_rpt_timer, DOG_CM_RPT_TIME );
} /* cm_kick_dog() */



/*===========================================================================

FUNCTION cm_offline

DESCRIPTION
  Perform offline processing for CM.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cm_offline(void)
{
  cmcall_offline();

  cmph_offline();

  /* send ack to TMC/MC that CM is offline*/
  task_offline();

} /* cm_offline() */




/*===========================================================================
FUNCTION CM_TASK

DESCRIPTION
  The main call manager function.  This is the big loop outside of the
  state machine which handles signals and all sorts of inputs.

DEPENDENCIES
  REX is running.

RETURN VALUE
  Never exits.

SIDE EFFECTS

===========================================================================*/
void cm_task
(
  dword dummy

)
/*lint -esym(715,dummy) */
{
  static rex_sigs_type  sigs;                              /* hold signals */

  /*-----------------------------------------------------------------------*/

  cm_init();                            /* initialize queues, timers, etc. */

  for( ;; )                         /* Wait on REX signals, repeat forever */
  {
    /*
    ** Wait for any signal
    */
    sigs = rex_wait
             (
               CM_RPT_TIMER_SIG
               | CM_TIMER_SIG
               | CM_RSSI_TIMER_SIG
               | CM_NV_ASYNC_SIG
               | CM_RPT_Q_SIG
               #if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
               #endif
               | CM_CLIENT_CMD_Q_SIG
               | TASK_STOP_SIG
               | TASK_OFFLINE_SIG
               #if (defined(FEATURE_GPSONE) && !defined(FEATURE_CGPS))
#error code not present
               #endif /* FEATURE_GPSONE && !FEATURE_CGPS */
               #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
               #endif /* FEATURE_GSM or FEATURE_WCDMA */
             );
    (void) rex_clr_sigs( &cm_tcb, sigs );


    /*
    ** Timer signal
    */
    if( sigs & CM_TIMER_SIG )
    {
      cm_timer_handle( 0 );                         /* Handle timer signal */
      (void) rex_set_timer( &cm_timer, CM_TIME );           /* Reset timer */
    }


    /*
    ** Report back to the watchdog.
    */
    if( sigs & CM_RPT_TIMER_SIG )
    {
      cm_kick_dog();                                 /* Kick the watch dog */
    }


    /*
    ** RSSI timer signal
    */
    if ( sigs & CM_RSSI_TIMER_SIG )
    {
      (void) rex_set_timer( &cm_rssi_timer, CM_RSSI_TIME ); /* Reset timer */
      #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
      cmss_rssi_timer_handle( );
      #endif /* FEATURE_CDMA_800, FEATURE_CDMA_1900 */
    }


    /*
    ** Report signal (MC/SD20/MN/REG/SM/RABM)
    */
    if( sigs & CM_RPT_Q_SIG )
    {
      /* Handle commands on queue till the queue is empty */
      cm_rpt_handle( );
    }


    /*
    ** Command queue signal
    */
    if( sigs & CM_CLIENT_CMD_Q_SIG )
    {
      (void) rex_clr_sigs( &cm_tcb, CM_CLIENT_CMD_Q_SIG );
      /* Handle commands on queue till the queue is empty */
      cm_client_cmd_handle( );
    }


    /*
    ** CM NV event.
    */
    if( sigs & CM_NV_ASYNC_SIG )
    {
      (void) rex_clr_sigs( &cm_tcb, CM_NV_ASYNC_SIG );
      cm_nv_handle();
    }


    /*
    ** STOP signal
    */
    if (sigs & TASK_STOP_SIG)
    {
       /* send ack to MC and then proceed */
       MSG_MED( "TASK_STOP_SIG received", 0,0,0 );
      (void) rex_clr_sigs( &cm_tcb, TASK_STOP_SIG );
      task_stop();
    }


    /*
    ** OFFLINE signal
    */
    if ((sigs & TASK_OFFLINE_SIG) != 0)
    {
       MSG_MED(" TASK_OFFLINE_SIG received",0,0,0);
       (void) rex_clr_sigs( &cm_tcb, TASK_OFFLINE_SIG );
       cm_offline();
    }


    #if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
    #endif // defined(FEATURE_WCDMA) || defined(FEATURE_GSM)


    #if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
    #endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */

    #if (defined(FEATURE_GPSONE) && !defined(FEATURE_CGPS))
#error code not present
    #endif /* FEATURE_GPSONE && !FEATURE_CGPS */

    /*
    ** Process hold commands
    */
    cm_hold_cmd_handle( TRUE, CM_HOLD_CMD_REAS_NONE );

  } /* for( ;; ) */

} /* cm_task() */
/*lint +esym(715,dummy) */

/*===========================================================================

FUNCTION cmtask_cmheap_ptr

DESCRIPTION
  Return pointer to CM heap.
  Needed by CM unit test cases that test heap memory allocation.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  Pointer to CM heap

SIDE EFFECTS
  None

===========================================================================*/
mem_heap_type* cmtask_cmheap_ptr( void )
{
  return &cm_heap;
}



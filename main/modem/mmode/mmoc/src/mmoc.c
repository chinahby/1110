/*====*====*===*====*====*====*====*====*====*====*====*====*====*====*====*
               M U L T I M O D E   C O N T R O L L E R ( MMoC )

                 I M P L E M E N T A T I O N    F I L E

GENERAL DESCRIPTION
  This module is the main implementation for the Multi-Mode Controller
  (MMoC) task. The MMoC is responsible for facilitating the switch between
  different modes, initializing the system determination (SD) module and
  interfacing with SD and various protocols to handle the operating mode
  change, subscription change and other commands. It also provides a task
  context for SD reselection timer.

  The MMoC task has 2 seperate message queues, command queue for messages
  from CM and report queue for messages from the various protocols.

EXTERNALIZED FUNCTIONS

  Command Interface:
    mmoc_cmd_subscription_chgd
    mmoc_cmd_oprt_mode_chgd
    mmoc_cmd_pref_sys_chgd
    mmoc_cmd_get_networks_gw
    mmoc_cmd_get_term_networks_gw
    mmoc_cmd_term_get_networks_wlan
    mmoc_cmd_term_get_networks_wlan
    mmoc_cmd_wakeup_from_pwr_save

  Report Interface:
    mmoc_rpt_prot_deactd_cnf
    mmoc_rpt_prot_auto_deactd_ind
    mmoc_rpt_ph_stat_chgd_cnf
    mmoc_rpt_prot_gen_cmd_cnf

  Functional Interface:
    mmoc_proc_prot_gen_cmd

  Others:
    mmoc_task

REGIONAL FUNCTIONS
  None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The MMoC task should be started before MMoC APIs are called.


Copyright (c) 2002 - 2009 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/mmoc/rel/08H1_QSC1110_3.X/src/mmoc.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/03/11   sg      MMOC would call TMC heap API to allocate memory dynamically.
05/31/09   pm      Change EXTERN define to MMEXTN to resolve compile error 
                   when other modules define EXTERN
05/05/09   aj      When MMOC and protocol are not in sync,print error
                   instead of crashing.04/13/09   aj      Change behavior to process auto deact rpt while waiting for
                   ph stat chgd cnf or deact_cnf
02/06/09   aj      WLAN subscription decoupling
02/05/09   aj      Modify mmoc_is_hdr_acq_allowed() to allow hdr acq in DED_MEAS mode
12/18/08   sg      Get "mmoc_state_info" ptr throug a function.
10/17/08   sv      Corrected featurization MMOC_LOW_MEM_TARGET to
                   FEATURE_MMOC_LOW_MEM_TARGET.
10/09/08   fj      Fixed Klocwork defects
09/19/08   jd      Memory reduction changes
09/12/08   fj/aj   Added support for Modem Reset
09/04/08   sv      Implemented WPS support for WLAN
09/05/08   ak      Fixing compiler warnings.
08/25/08   ak      Fixing compiler warnings.
08/20/08   st      Resolve Klockwork defects
07/30/08   sv      Added FEATURE_HICPS_STACK_SYNC_UP to buffer the dropped 
                   HDR acq
06/16/08   sv      Fixed Klocwork defects
04/28/08   fj      Lint cleanup, CR 144389
03/22/08   ic      Undo of "Replace custmmode.h with modem_mmode_cust.h" and 
                   "FEATURE_MMODE_CUSTMMODE_H with FEATURE_MODEM_MMODE_CUST_H"
01/31/08   ic      Replace custmmode.h with custmmode.h
                   Replace FEATURE_MMODE_CUSTMMODE_H with FEATURE_CUSTMMODE_H   
01/31/08   cl      Added in FEATURE_MMOC_DETECT_HDR_HW to support hardware 
                   run-time detection.
09/11/07   pk      Added support for FEATURE_DISABLE_DEEP_SLEEP
08/20/07   ic      Lint cleanup 
04/16/07   rp      Corrected handling of FTM mode
03/19/07   ka      Replacing banned apis
01/31/07   ka      Adding new state in MMOC to wait for TMC to ack operating
                   mode change.
01/04/07   pk      Added support for VoIP -> 1X Handoff
11/30/06   rp      Added support for background BPLMN search in limited service.
11/15/06   sk      Added hybrid GW support.
08/29/06   sk      Added WLAN Adhoc support.
04/24/06   ka      Featurizing ftm.h under GW flags.
04/03/06   ka      Changing copyright.
12/30/05   ka      Adding string maps to mmoc commands, reports and state
                   transitions.
11/28/05   ka      Fixing klockwork errors.
07/27/05   ka      Fixing implementation of common_hdr_deactd_cnf()
07/21/05   ka      Adding MMOC_DUP_CMD_REM_SIG to the list of signals
                   that mmoc task waits for.
07/07/05   ka      Request to activate GW on main when HDR is online in 
                   hybrid instance proceeds with request to deactivate hdr.
                   On hdr deactivate cnf GW acquisition gets sent.
06/27/05   ic      Fixed RVDS 2.1 compiler warnings 
06/17/05   ka      Rejecting Generic cmd to activate HDR hybrid instance
                   when GW is on Main instance.
                   Fixing issue where sd_ss_act_get() was called for Auto
                   deactivation indication from HDR hybrid instance resulting
                   in HDR hybrid staying online (sanity error fatal).
03/28/05   ka      Correcting Lint errors.
03/02/05   ka      Handling subscription available along with protocol name
                   that has subscription available.
02/19/05   dk      Fixed MMOC Pref_sys_cmd processing. MMOC does not queue
                   pref_sys_chgd_cmd for WLAN SS if WLAN feature is not
                   enabled.
01/12/05   sj      Fixed issue of mmoc activating ss-main after reset
                   because of SS timer expiry after processing RESET cmd.
12/21/04   sj      Fixed lint errors.
12/03/04   sj      Updated to issue standby sleep only after all instances
                   enter pwr_save.
09/27/04   ajn     Created an mmoc_timer_group for MMOC timers.
08/20/04   sj      Added support for prl_pref.
08/13/04   sj      Do not err_fatal if online command is received after
                   poweroff.
08/06/04   sj      Converted calls to _true_curr_act to sd_ss_act_get().
08/04/04   sj      For subsc_changed or sd_inited case, call user_pwr_up ind.
07/29/04   sj      Do not reset sd_initialized flag after LPM or FTM.
06/21/04   jqi     Use sd nam selection indication when there is a
                   subscription change.
06/07/04   sj      Send CM_STANDBY_SLEEP_F report upon entering power save.
05/25/04   jqi     Fixed mode preference reset issue after power up.
05/10/04   sj      Merged in changes for srch inactive state.
04/22/04   sj      Fixed issue of not starting pwr_save timer when next act
                   is also PWR_SAVE for SS-Main.
03/18/04   jqi     Used the true act type from SD System selection.
                   Replaced diag_enter_panic_mode with
                   ERR_FATAL_REPORT().
03/28/04   sj      Do not move srch to START upon getting ONLINE in LPM.
03/12/04   sj      Do not place srch in standby if entering pwr save from
                   HDR.
03/01/04   sj      Changed the order of deactivating protocols, SS_HDR first
                   followed by SS_MAIN.
02/24/04   sj      Removed LINT warnings.
01/13/03   sj      Merged in duplicate pref_sys_chgd cmd detection code.
                   Send HDR unlock RF command if 1x doesn't respond for
                   sometime.
                   Fixed sanity err_fatal when trans_state is AUTO_DEACT_IND.
10/22/03   sj      Send GPS_MODE_EXIT_F to CM before activating other
                   protocols.
09/15/03   sj      Fixed issues with HDR <-> 1x runtime switching.
08/02/03   dyc     Support for FEATURE_MULTI_RX_CHAIN
07/20/03   sj      Added hybrid support.
06/05/03   ht      Removed extra comma
07/08/03   sj      Changes for thin_ui support.
06/18/03   sj/ck   Vote off UIM power ctl after processing subscription
                   available.
05/14/03   sj      Fixed compiler error when FEATURE_FACTORY_TESTMODE is
                   not defined.
04/24/03   sj      Added support for early power scan.
03/18/03   sj      Process online even if subscription is not present.
03/18/03   lcl     Added reference to zrf library.
03/10/03   SJ      Added support for runtime FTM in leopard.
03/06/03   JQ/SJ   Moving phone to offline if rf init fails.
03/05/03   SJ      Fixed bug dring handling reset command wrt SD term.
02/26/03   SJ      Modified sanity timeout err_fatal to print the offending
                   protocol task's name instead of mmoc.c.
02/24/03   SJ      Handling GET_NET_GW in activate_prot_online().
02/24/03   SJ      Sending ONLINE command to TMC during online processing.
02/21/03   SJ      Calling sd_ss_ind_user_pwr_down() before powering down
                   the phone.
02/19/03   SJ      Handling subsc_chgd command only in powerup or online.
02/14/03   SJ      Added new MMOC API for wakeup from power save.
02/11/03   SJ/JQI  Added support for runtime FTM.
02/05/03   PK      Fixed compiler problem (missing end comment terminator).
02/05/03   SJ      Temporarily turning off rfr & rfl before activating GW.
01/23/03   SJ      Calling nam_sel SD indication only if the subsc_chgd CM
                   cmd was received, otherwise invoke pwr_up SD indication.
01/16/03   SJ      Updated _send_cm_rpt() to use cm_sd_rpt_u_type always.
01/16/03   SJ      Featurized DB accesses for non 1x targets.
12/19/02   SJ      Fixed activate_prot_offline for targets not supporting
                   CDMA or AMPS.
12/18/02   SJ      Added more debug messages.
12/06/02   SJ      Added fix to move protocol to same state after processing
                   subscription changed command for FTM/OFFLINE oprt mode.
11/22/02   SJ      Modified call to SD user_ss_pref -> user_ss_pref2().
09/17/02   SJ      Updated to send CM report on completion of LPM processing.
07/24/02   SJ      Cleared rex signal before processing event.
06/10/02   SJ      Cleared rex signal after processing event.
06/07/02   SJ      Renamed PROT_TRANS_NONE_PENDING to PROT_TRANS_NONE.
03/28/02   SJ      Initial release.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h" /* Customer configuration file */
#ifndef FEATURE_CUSTMMODE_H 
#error Need to pull in custmmode.h!!! /* Make sure custmmode.h is pulled in */
#endif 

#include "comdef.h"   /* Definition for basic types and macros             */
#include "msg.h"      /* Message output services.                          */
#include "err.h"      /* Error and error fatal services.                   */
#include "cmd.h"      /* Command header for inter-task communication.      */
#include "clk.h"      /* Interface for clock services.                     */
#include <stdio.h>    /* For calling sprintf.                               */

#include "sd.h"       /* Interface for System Determination services.      */
#include "prot.h"     /* Common header for mmoc <-> protocol communications*/
#include "mmoc.h"     /* External interface definitions for MMoC.          */
#include "mmoci.h"    /* Internal interface definitions for MMoC.          */
#ifndef FEATURE_MMOC_UNIT_TEST
#include "bio.h"      /* Interface for GPIO  and other bit i/o services.   */
#endif
#include "task.h"     /* Interface for global task resources.              */
#include "dog.h"      /* Interface to watchdog procedures.                 */
#ifdef MMOC_USES_DB_SERVICES
#include "db.h"       /* Interface to database services.                   */
#endif /* ifdef(MMOC_USES_DB_SERVICES) */

#if defined(MMOC_CDMA_SUPPORTED) || defined(MMOC_AMPS_SUPPORTED)
#include "cmxll.h"    /* Interface to CM report service.                   */
#endif /* defined(MMOC_CDMA_SUPPORTED) || defined(MMOC_AMPS_SUPPORTED) */

#include "cm.h"       /* Interface to CM client service.                   */

#if defined(MMOC_CDMA_SUPPORTED) || defined(MMOC_AMPS_SUPPORTED)
#include "mc.h"       /* Interface for CDMA and AMPS protocols.            */
#ifndef FEATURE_MMOC_UNIT_TEST
#include "rflib.h"
#endif
#endif /* defined(MMOC_CDMA_SUPPORTED) || defined(MMOC_AMPS_SUPPORTED) */

#ifdef MMOC_HDR_SUPPORTED
#error code not present
#endif /* defined(MMOC_HDR_SUPPORTED) */

#ifdef MMOC_GW_SUPPORTED
#include "cmregprx.h" /* Interface for RegProxy module.                    */
#endif /* MMOC_GW_SUPPORTED */

#ifdef MMOC_WLAN_SUPPORTED
#error code not present
#endif

#ifdef FEATURE_UIM
#include "uim.h"
#endif

#ifdef FEATURE_NV_RUIM
#include "nvruimi.h"
#endif

#ifdef FEATURE_MMOC_UNIT_TEST
#include "string.h"   /* Interface for string functions.                   */
#endif

#include "event.h"    /* Interface for system event report service.        */
#include "tmc.h"      /* Interface for Task mode services.                 */


#ifdef MMOC_FTM_SUPPORTED

#ifdef MMOC_GW_SUPPORTED
#include "ftm.h"
#else
#include "test_mc.h"
#endif

#endif /*FEATURE_FACTORY_TESTMODE*/



#if defined(RF_MULTIMODE) || defined(FEATURE_MULTIMODE_RFAPI)
#include "rfm.h"      /* Interface for multimode RF APIs                  */
#endif /* defined(RF_MULTIMODE) || defined(FEATURE_MULTIMODE_RFAPI)  */


#include "mmocdbg.h"  /* For mmocdbg_print_message support */

#ifdef FEATURE_MMOC_SELECTIVE_ACTIVATION
#include "msm_support.h"
#endif

/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
============================== DEPENDENT FEATURES ===========================
=============================================================================
=============================================================================
===========================================================================*/

/* This set of features must be defined for Multi-mode operation.
*/
#if !defined(FEATURE_SD20) || !defined(FEATURE_MULTIMODE_ARCH)
#error FEATURE_SD20 and FEATURE_MULTIMODE_ARCH must be defined.
#endif

/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
==================== MMoC Forward declarations ==============================
=============================================================================
=============================================================================
===========================================================================*/

static rex_sigs_type                   mmoc_wait(rex_sigs_type  mask);


#ifdef MMOC_GW_SUPPORTED
static void mmoc_reg_multimode_sanity_err_fatal( mmoc_sanity_err_e_type reason);
#endif


static void mmoc_send_cm_rpt( cm_name_type rpt_name);


/* <EJECT> */
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/


/* List of modes for which the phone status changed command has to be sent.
*/
static mmoc_mode_info_s_type ph_stat_cmd_tbl[(int)SD_MODE_MAX+1] =
{
  { "MODE_INACT",     NULL,                               PROT_TRANS_NONE },
  #ifdef MMOC_AMPS_SUPPORTED
  { "MC(AMPS)",       mc_amps_multimode_sanity_err_fatal, PROT_TRANS_NONE },
  #else
  { "MC(AMPS)",       NULL,                               PROT_TRANS_NONE },
  #endif
  #ifdef MMOC_CDMA_SUPPORTED
  { "MC(CDMA)",       mc_cdma_multimode_sanity_err_fatal, PROT_TRANS_NONE },
  #else
  { "MC(CDMA)",       NULL                              , PROT_TRANS_NONE },
  #endif
  { "GPS",            NULL,                               PROT_TRANS_NONE },
  #ifdef MMOC_HDR_SUPPORTED
#error code not present
  #else
  { "HDR"     ,       NULL,                               PROT_TRANS_NONE },
  #endif
  #ifdef MMOC_GW_SUPPORTED
  { "REG(GSM/WCDMA)", mmoc_reg_multimode_sanity_err_fatal,PROT_TRANS_NONE },
  { "REG(GSM/WCDMA)", mmoc_reg_multimode_sanity_err_fatal,PROT_TRANS_NONE },
  { "REG(GSM/WCDMA)", mmoc_reg_multimode_sanity_err_fatal,PROT_TRANS_NONE },
  #else
  { "REG(GSM/WCDMA)", NULL,                               PROT_TRANS_NONE },
  { "REG(GSM/WCDMA)", NULL,                               PROT_TRANS_NONE },
  { "REG(GSM/WCDMA)", NULL,                               PROT_TRANS_NONE },
  #endif
  #ifdef MMOC_WLAN_SUPPORTED
#error code not present
  #else
  { "WLAN",           NULL,                               PROT_TRANS_NONE },
  #endif
  { "Invalid",        NULL,                               PROT_TRANS_NONE }
};


/* MMoC's task info.
*/
static mmoc_task_info_s_type           mmoc_task_info;

/* Local buffer to printing debug messages.
*/
static char                            mmoc_debug_str[256];


#ifdef FEATURE_USE_TIME_VU
/* Timer group which will not be disabled during sleep.
** This group ensures MMOC can wake the mobile up from deep sleep.
*/
static timer_group_type                mmoc_timer_group;
#endif /* FEATURE_USE_TIME_VU */

#ifdef FEATURE_MMOC_TMC_HEAP
/*---------------------------------------------------------------------------
                                 MMOC HEAP
---------------------------------------------------------------------------*/


static mem_heap_type     mmoc_heap;

static uint8 mmoc_heap_mem_buffer[MMOC_HEAP_BUFFER_SIZE];

/* No. of free slots in the MMOC heap.
*/
/*lint -esym(765, mmoc_heap_slots_left)
** Accessed by unit test cases.
*/
/*lint -esym(552, mmoc_heap_slots_left)
** Doesn't consider MMOC_MSG_HIGH as an access.
*/
uint8 mmoc_heap_slots_left;

/* No. of blocks currently allocated from the TMC heap.
*/
/*lint -esym(765, mmoc_tmc_heap_blocks)
** Accessed by unit test cases.
*/
/*lint -esym(552, mmoc_tmc_heap_blocks)
** Doesn't consider MMOC_MSG_HIGH as an access.
*/
uint8 mmoc_tmc_heap_blocks;

#endif /* FEATURE_MMOC_TMC_HEAP */

/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
==================== MMoC IMPLEMENATATION RELATED HELPER FUNCTIONS ==========
=============================================================================
=============================================================================
===========================================================================*/

#ifdef FEATURE_MMOC_TMC_HEAP
/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_mem_free

DESCRIPTION
   This function returuns the specified memory back to the Heap.

   If the memory came from MMOC heap then it is returned there else
   it is returned to the TMC heap.

DEPENDENCIES

   MMOC and TMC heap must have been initialized.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void mmoc_mem_free(

  void *ptr

)
{

  if(ptr == NULL)
  {
    MMOC_MSG_HIGH("Null ptr to mmoc_mem_free",0,0,0);
    return;
  }

  /*
  ** If we allocated memory from MMOC Heap, then return there
  ** else we need to return back to the TMC heap.
  */
  /*lint -e{826} keep it suppressed for general / specific walks as well */
  if( (ptr >= (void *)(mmoc_heap_mem_buffer) &&
      (ptr <  (void *)(mmoc_heap_mem_buffer + MMOC_HEAP_BUFFER_SIZE)))
    )
  {
    mmoc_heap_slots_left++;
    mem_free( &(mmoc_heap),ptr );
  }
  else
  {
    if( ptr < (void *)(tmc_heap_mem_buffer) &&
                 ptr >=  (void *)(tmc_heap_mem_buffer + TMC_HEAP_MEM_BUFFER_SIZE)
      )
    {
      MMOC_MSG_ERROR("Invalid mem ptr",0,0,0);
      return;
    }
    mmoc_tmc_heap_blocks--;
    mem_free( &(tmc_heap),ptr);
  }

  MMOC_MSG_HIGH("MMOC free: MMOC Left, TMC Used = %d, %d", mmoc_heap_slots_left, mmoc_tmc_heap_blocks, 0);

} /* mmoc_mem_free() */
#endif /* FEATURE_MMOC_TMC_HEAP */

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_get_task_info_ptr

DESCRIPTION
  This function will return the pointer to the MMoC's task info struct.

DEPENDENCIES
  None.

RETURN VALUE
  mmoc_task_info_s_type* - Pointer to the local definition of MMoC's task
                           info struct.
  Note! Cannot be NULL.

SIDE EFFECTS
  None.
===========================================================================*/
static mmoc_task_info_s_type*          mmoc_get_task_info_ptr
(

       void

)
{
  /* Return the pointer to the MMoC's task info.
  */
  return &mmoc_task_info;
}

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_init_state_info

DESCRIPTION
  This function will initialize the MMoC state info to the power up state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void                            mmoc_init_state_info
(

       mmoc_state_info_s_type          *mmoc_info_ptr
          /* Pointer to MMoC's state information.
          */
)
{
  
  MMOC_ASSERT( mmoc_info_ptr != NULL );
  

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Initialize the state info.
  */
  mmoc_info_ptr->curr_nam                = (byte) SD_NAM_MAX;
  mmoc_info_ptr->is_cdma_subs_avail      = FALSE;
  mmoc_info_ptr->is_gw_subs_avail        = FALSE;
  mmoc_info_ptr->is_wlan_subs_avail      = FALSE;
  mmoc_info_ptr->curr_oprt_mode          = PROT_OPRT_MODE_NULL;
  mmoc_info_ptr->true_oprt_mode          = SYS_OPRT_MODE_NONE;
  mmoc_info_ptr->curr_trans              = MMOC_TRANS_NULL;
  mmoc_info_ptr->trans_state             = MMOC_TRANS_STATE_NULL;
  mmoc_info_ptr->trans_id                = 0;
  mmoc_info_ptr->prot_state[SD_SS_MAIN]  = PROT_STATE_NULL;
  mmoc_info_ptr->prot_state[SD_SS_HYBR_HDR]
                                         = PROT_STATE_NULL;
 mmoc_info_ptr->prot_state[SD_SS_HYBR_WLAN]
                                         = PROT_STATE_NULL;
  mmoc_info_ptr->is_sd_initialized       = FALSE;
  mmoc_info_ptr->is_rf_initialized       = TRUE;
  mmoc_info_ptr->exit_fn_ptr             = NULL;
  mmoc_info_ptr->is_standby_sleep        = FALSE;
  mmoc_info_ptr->prot_subsc_chg          = PROT_SUBSC_CHG_NONE;
  
}

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_next_trans_id

DESCRIPTION
  This function gets the next valid transaction Id. Also, sets the value
  in the MMoC's state information.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static prot_trans_type                 mmoc_next_trans_id
(

       mmoc_state_info_s_type         *mmoc_info_ptr
          /* Pointer to MMoC's state information.
          */

)
{
  MMOC_ASSERT( mmoc_info_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Print out debug messages.
  */
  MMOC_MSG_LOW("next_trans_id()", 0, 0, 0 );
  MMOC_MSG_LOW("Curr. transaction %d", mmoc_info_ptr->curr_trans, 0, 0 );
  MMOC_MSG_LOW("Curr. trans. state %d", mmoc_info_ptr->trans_state, 0, 0 );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  while ( (++mmoc_info_ptr->trans_id) == PROT_TRANS_NONE )
  {
    /* None.
    */
  }


  return mmoc_info_ptr->trans_id;

} /* mmoc_next_trans_id() */


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_get_curr_trans_id

DESCRIPTION
  Get current transaction id that is being held by MMOC.


DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static prot_trans_type                 mmoc_get_curr_trans_id
(

       const  mmoc_state_info_s_type  *mmoc_info_ptr
          /* Pointer to MMoC's state information.
          */

)
{
  MMOC_ASSERT( mmoc_info_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Print out debug messages.
  */
  MMOC_MSG_HIGH ("curr_trans_id() %d", mmoc_info_ptr->trans_id, 0, 0 );
  MMOC_MSG_LOW("Curr. transaction %d", mmoc_info_ptr->curr_trans, 0, 0 );
  MMOC_MSG_LOW("Curr. trans. state %d", mmoc_info_ptr->trans_state, 0, 0 );


  return mmoc_info_ptr->trans_id;

} /* mmoc_get_curr_trans_id() */



/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_update_standby_sleep_wakeup_state

DESCRIPTION
  This function checks if transitioned in/out of standby sleep. If so
  informs CM of the new standby state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void                            mmoc_update_standby_sleep_wakeup_state
(

    mmoc_state_info_s_type             *mmoc_info_ptr
       /* Pointer to MMoC's state information.
       */

)
{
  MMOC_ASSERT( mmoc_info_ptr != NULL );



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Print out debug messages.
  */
  MMOC_MSG_LOW("mmoc_update_standby_sleep_wakeup_state()", 0, 0, 0 );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Check if we are in pwr_save.
  */
  if ( mmoc_info_ptr->prot_state[SD_SS_MAIN] == PROT_STATE_PWR_SAVE &&
       ( mmoc_info_ptr->prot_state[SD_SS_HYBR_HDR] == PROT_STATE_NULL ||
         mmoc_info_ptr->prot_state[SD_SS_HYBR_HDR] == PROT_STATE_PWR_SAVE
       )
     )
  {
    /* Check if mmoc state is not in standby sleep.
    */
    if ( !mmoc_info_ptr->is_standby_sleep )
    {
      /* MMoc state in standby wakeup,
      ** Send the STANDBY SLEEP report to CM.
      */
      mmoc_send_cm_rpt( CM_STANDBY_SLEEP_F );
      mmoc_info_ptr->is_standby_sleep = TRUE;

    }

  }
  else
  {
    /* Phone is not in standby sleep, check mmoc state.
    */
    if ( mmoc_info_ptr->is_standby_sleep )
    {
      /* MMOC state in standby sleep,
      ** Send the STANDBY WAKEUP report to CM.
      */
      mmoc_send_cm_rpt( CM_STANDBY_WAKE_F );
      mmoc_info_ptr->is_standby_sleep = FALSE;

    }

  }


} /* mmoc_update_standby_sleep_wakeup_state() */

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_update_prot_state

DESCRIPTION
  This function sets the MMoC's protocol state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void                            mmoc_update_prot_state
(

    prot_state_e_type             new_prot_state,
       /* The new protocol state of the MMoC.
       */

    sd_ss_e_type                       ss_instance,
       /* Update prot. state corresponding to this SS instance.
       */

    mmoc_state_info_s_type             *mmoc_info_ptr
       /* Pointer to MMoC's state information.
       */

)
{
  MMOC_ASSERT_ENUM_IS_INRANGE(new_prot_state, PROT_STATE_MAX);
  MMOC_ASSERT( mmoc_info_ptr != NULL );



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Print out debug messages.
  */
  MMOC_MSG_LOW("update_prot_state()", 0, 0, 0 );
  MMOC_MSG_LOW("Curr. transaction %d", mmoc_info_ptr->curr_trans, 0, 0 );
  MMOC_MSG_LOW("Curr. trans. state %d", mmoc_info_ptr->trans_state, 0, 0 );
  MMOC_MSG_LOW("Curr. prot. state %d", mmoc_info_ptr->prot_state, 0, 0 );
  MMOC_MSG_LOW("New prot. state %d", new_prot_state, 0, 0 );
  MMOC_MSG_LOW("prot. instance %d", ss_instance, 0, 0 );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Update the new protocol state.
  */
  mmoc_info_ptr->prot_state[ss_instance] = new_prot_state;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update standby sleep /wakeup transition, if changed.
  */
  mmoc_update_standby_sleep_wakeup_state( mmoc_info_ptr );


} /* mmoc_update_prot_state() */

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_update_db_dmss_state

DESCRIPTION
  This function sets the DB DMSS state based on the current protocol
  state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
/*lint -esym(715,mmoc_info_ptr)*/
static void                            mmoc_update_db_dmss_state
(

       const  mmoc_state_info_s_type  *mmoc_info_ptr
          /* Pointer to MMoC's state information.
          */

)
{
  #ifdef MMOC_USES_DB_SERVICES
  static db_items_value_type  db_item;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  MMOC_MSG_LOW("update_db_dmss_state() %d", mmoc_info_ptr->prot_state, 0, 0);


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Determine what the DB state is.
  */
  switch( mmoc_info_ptr->prot_state[SD_SS_MAIN] )
  {

    case PROT_STATE_NULL:
      db_item.dmss_state =  DB_DMSS_POWERUP_S;
      break;

    case PROT_STATE_ONLINE_CDMA:
    case PROT_STATE_ONLINE_GW:
    case PROT_STATE_ONLINE_HDR:
    case PROT_STATE_ONLINE_DED_MEAS:
      db_item.dmss_state = DB_DMSS_ONLINE_DIGITAL_S;
      break;

    case PROT_STATE_OFFLINE_CDMA:
    case PROT_STATE_OFFLINE:
      db_item.dmss_state = DB_DMSS_OFFLINE_DIGITAL_S;
      break;

    case PROT_STATE_ONLINE_AMPS:
      db_item.dmss_state = DB_DMSS_ONLINE_ANALOG_S;
      break;


    case PROT_STATE_OFFLINE_AMPS:
      db_item.dmss_state = DB_DMSS_OFFLINE_ANALOG_S;
      break;

    case PROT_STATE_RESET:
      db_item.dmss_state = DB_DMSS_RESET_S;
      break;

    case PROT_STATE_LPM:
    case PROT_STATE_PWR_DOWN:
      db_item.dmss_state = DB_DMSS_POWERDOWN_S;
      break;

    case PROT_STATE_GPSONE_MSBASED:
      db_item.dmss_state = DB_DMSS_GPS_S;
      break;

    case PROT_STATE_FTM:
    case PROT_STATE_PWR_SAVE:
    case PROT_STATE_MAX:
    default:
      /* No DB state defined for these protocol state, don't update
      ** the DB state.
      */
      break;

  } /* switch( mmoc_info_ptr->prot_state ) */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Update the dmss state.
  */
  MMOC_MSG_MED("update_db_dmss_state() %d", db_item.dmss_state, 0, 0);
  db_put(DB_DMSS_STATE, &db_item);

  #endif /* defined(MMOC_USES_DB_SERVICES) */

} /* mmoc_update_db_dmss_state() */
/*lint +esym(715,mmoc_info_ptr)*/


#ifdef MMOC_HDR_SUPPORTED
#error code not present
#endif /*  MMOC_HDR_SUPPORTED */

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_update_trans_state

DESCRIPTION
  This function sets the MMoC's transaction state as per the value passed in.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void                            mmoc_update_trans_state
(
    mmoc_trans_state_e_type            new_trans_state,
       /* The new transaction state of the MMoC.
       */

    mmoc_state_info_s_type             *mmoc_info_ptr
       /* Pointer to MMoC's state information.
       */
)
{
  MMOC_ASSERT_ENUM_IS_INRANGE(new_trans_state, MMOC_TRANS_STATE_MAX);
  MMOC_ASSERT( mmoc_info_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Print out debug messages.
  */
  MMOC_MSG_LOW("update_trans_state()", 0, 0, 0 );
  MMOC_MSG_LOW("Curr. transaction %d", mmoc_info_ptr->curr_trans, 0, 0 );
  MMOC_MSG_LOW("Curr. trans. state %d", mmoc_info_ptr->trans_state, 0, 0 );
  MMOC_MSG_LOW("New trans. state %d", new_trans_state, 0, 0 );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Update the MMoC's new transaction state.
  */
  mmoc_info_ptr->trans_state = new_trans_state;


} /* mmoc_update_trans_state() */


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_update_curr_trans

DESCRIPTION
  This function sets the current MMoC transaction given the new transaction.
  Also, the new transaction state is set to NULL.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void                            mmoc_update_curr_trans
(

    mmoc_trans_e_type                  new_trans,
       /* The new transaction state of the MMoC.
       */

    mmoc_state_info_s_type             *mmoc_info_ptr,
       /* Pointer to MMoC's state information.
       */

    sys_oprt_mode_e_type               current_oprt_mode
      /* Flag to indicate if RESET_MODEM cmd is being processed.
      */
)
{
  rex_timer_cnt_type  sanity_timeout = MMOC_SANITY_TIME;


  MMOC_ASSERT_ENUM_IS_INRANGE(new_trans, MMOC_TRANS_MAX);
  MMOC_ASSERT( mmoc_info_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(mmoc_info_ptr->curr_trans >= MMOC_TRANS_MAX)
  {
      MMOC_MSG_ERROR( "curr_trans out-of-bound: %d, Aborting!!!",
                mmoc_info_ptr->curr_trans,0,0);
      return;
  }
     
  if(new_trans >= MMOC_TRANS_MAX)
  {
      MMOC_MSG_ERROR( "invalid new_trans: %d", new_trans,0,0);
      return;
  }

  if(mmoc_info_ptr->trans_state >= MMOC_TRANS_STATE_MAX)
  {
      MMOC_MSG_ERROR( "invalid trans_state: %d",
                mmoc_info_ptr->trans_state,0,0);
      return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_MMOC_LOW_MEM_TARGET
  /* Print debug info before calling transaction handler.
  */
  MMOC_PRINT_MSG_HIGH ( "update_curr_trans()" );
  MMOC_PRINT_MSG_HIGH ( "Curr_trans  %d(%s)", mmoc_info_ptr->curr_trans, 
                        mmocdbg_trans_e_type_string_map [mmoc_info_ptr->curr_trans]
                        );  
  MMOC_PRINT_MSG_HIGH ( "Curr. trans. state %d(%s)", mmoc_info_ptr->trans_state,
                      mmocdbg_trans_state_e_type_string_map [mmoc_info_ptr->trans_state]
                      );

  /* added to avoid lint error */
  if (new_trans < MMOC_TRANS_MAX)
  {
    MMOC_PRINT_MSG_HIGH ( "New transaction %d(%s)", 
                          new_trans, 
                          mmocdbg_trans_e_type_string_map [new_trans]
                        );
  }
  #endif /* FEATURE_MMOC_LOW_MEM_TARGET */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Update the new transaction.
  */
  mmoc_info_ptr->curr_trans  = new_trans;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the new true_oprt_mode
  */
  mmoc_info_ptr->true_oprt_mode = current_oprt_mode;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* When curr_trans is changed, always start from NULL trans_state.
  */
  mmoc_info_ptr->trans_state = MMOC_TRANS_STATE_NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Do common initialization.
  */
  mmoc_info_ptr->is_ph_stat_sent       = FALSE;
  mmoc_info_ptr->deact_req_index       = SD_SS_MAIN;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Clear the insanity_count and reset the timer.
  */
  mmoc_info_ptr->insanity_count = 0;
  (void) rex_clr_timer( &mmoc_info_ptr->sanity_timer );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Start the sanity timer if the current transaction is not NULL.
  */
  if ( mmoc_info_ptr->curr_trans != MMOC_TRANS_NULL )
  {
    /* If it is powerdown transaction, load the special timeout value.
    */
    if ( mmoc_info_ptr->curr_trans == MMOC_TRANS_PWR_DOWN )
    {
      sanity_timeout = MMOC_PWRDOWN_SANITY_TIME;
    }

    /* Start the sanity timer.
    */
    (void) rex_set_timer( &mmoc_info_ptr->sanity_timer,
                          sanity_timeout
                        );
  } /* if ( mmoc_info_ptr != MMOC_TRANS_NULL ) */




} /* mmoc_update_curr_trans() */


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_clear_transaction

DESCRIPTION
  This function clears the MMoC's transaction.

DEPENDENCIES
  None.

RETURN VALUE
 MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
 MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
static mmoc_evt_status_e_type          mmoc_clear_transaction
(
    mmoc_state_info_s_type             *mmoc_info_ptr
       /* Pointer to MMoC's state information.
       */
)
{

  MMOC_ASSERT( mmoc_info_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if exit fn. needs to be called.
  */
  if ( mmoc_info_ptr->exit_fn_ptr )
  {
    /* Invoke exit transaction fn.
    */
    if ( (mmoc_info_ptr->exit_fn_ptr)( mmoc_info_ptr ) ==
                         MMOC_EVT_STATUS_NOT_CONSUMED )
    {
      /* New transaction needs to be invoked.
      */
      mmoc_info_ptr->exit_fn_ptr = NULL;
      return MMOC_EVT_STATUS_NOT_CONSUMED;
    }

  } /*  if ( mmoc_info_ptr->exit_fn_ptr ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Print out debug messages.
  */
  MMOC_MSG_LOW("clear_transaction,prot_state %d ",
               mmoc_info_ptr->prot_state,
               0,
               0
              );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Clear the transaction and its state.
  */
  mmoc_update_curr_trans( MMOC_TRANS_NULL,
                          mmoc_info_ptr,
                          SYS_OPRT_MODE_NONE
                         );

  mmoc_update_trans_state( MMOC_TRANS_STATE_NULL,
                           mmoc_info_ptr
                         );
  mmoc_info_ptr->exit_fn_ptr = NULL;

  return MMOC_EVT_STATUS_CONSUMED;

} /* mmoc_clear_transaction() */

#if defined(MMOC_GW_SUPPORTED)
/*===========================================================================

FUNCTION MMOC_REG_MULTIMODE_SANITY_ERR_FATAL

DESCRIPTION
   This function is called since the GSM/WCDMA stack did not respond to
   CM_STOP_MODE_REQ or CM_SIM_AVAILABLE_REQ or CM_SIM_NOT_AVAILABLE_REQ.
   Response timeout for CM_STOP_MOE_REQ is 15 seconds for all cases expect when
   the reason is POWER_DOWN which 30 seconds.
   Response timeout for CM_SIM_AVAILABLE_REQ & CM_SIM_NOT_AVAILABLE_REQ is 15
   seconds.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void mmoc_reg_multimode_sanity_err_fatal(

     mmoc_sanity_err_e_type     reason
)
{

  (void)std_strlprintf(mmoc_debug_str, sizeof(mmoc_debug_str), "");

  switch ( reason )
  {
    case MMOC_SANITY_ERR_DEACT_CMD_CNF:
      /* The GSM/WCDMA stack did not respond to CM_STOP_MODE_REQ.
      */
      #ifdef FEATURE_WCDMA
#error code not present
      #else
      (void)std_strlprintf( mmoc_debug_str, sizeof(mmoc_debug_str), "GSM ");
      #endif
      (void)std_strlcat( mmoc_debug_str, 
                         "stack did not respond to CM_STOP_MODE_REQ",
                         sizeof(mmoc_debug_str));
      break;

    case MMOC_SANITY_ERR_PH_STAT_CMD_CNF:
      /* GSM/WCDMA stack did not respond to CM_SIM_AVAILABLE_REQ or
      ** CM_SIM_NOT_AVAILABLE_REQ.
      */
      #ifdef FEATURE_WCDMA
#error code not present
      #else
      (void)std_strlprintf( mmoc_debug_str, sizeof(mmoc_debug_str), "GSM ");
      #endif
      (void)std_strlcat( mmoc_debug_str, 
                         "stack did not respond to CM_SIM_XXXX_REQ",
                         sizeof(mmoc_debug_str));
      break;

    default:
     break;

  } /* switch ( reason ) */

  #ifndef FEATURE_DIAG_NON_STREAMING
  if ( strlen(mmoc_debug_str) != 0 )
  {
    ERR_FATAL_REPORT(mmoc_debug_str,0,0,0);
  }
  #endif

} /* mmoc_reg_multimode_sanity_err_fatal */
#endif /* defined(MMOC_GW_SUPPORTED) */


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_active_protocol_name

DESCRIPTION
  This function returns the active protocol name.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static char*                           mmoc_active_protocol_name
(
    sd_ss_e_type                       ss,
       /* SD instance
       */
    const mmoc_state_info_s_type      *mmoc_info_ptr
       /* Pointer to MMoC's state information.
       */
)
{
  char *s;
  MMOC_ASSERT( mmoc_info_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Determine the active protocol name using the protocol state.
  */
  switch ( mmoc_info_ptr->prot_state[ss] )
  {

    case PROT_STATE_ONLINE_CDMA:
      s = "MC(CDMA-online)";
      break;

    case PROT_STATE_OFFLINE_CDMA:
      s = "MC(CDMA-offline)";
      break;
    case PROT_STATE_ONLINE_AMPS:
      s = "MC(AMPS-online)";
      break;

    case PROT_STATE_OFFLINE_AMPS:
      s = "MC(AMPS-offline)";
      break;

    case PROT_STATE_ONLINE_DED_MEAS:
      #ifdef FEATURE_WLAN_MEASUREMENT
#error code not present
      #endif
      {
        s = "MC(DED. MEAS)";
      }
      break;

    case PROT_STATE_ONLINE_GW:
      s = "REG(GSM)";
      break;

    case PROT_STATE_ONLINE_HDR:
      s = "HDR";
      break;

    case PROT_STATE_ONLINE_WLAN:
      s = "WLAN(Online)";
      break;

    case PROT_STATE_FTM:
      s = "MC(FTM)";
      break;

    case PROT_STATE_GPSONE_MSBASED:
      s = "GPS-MSBASED";
      break;

    case PROT_STATE_OFFLINE:
    case PROT_STATE_PWR_SAVE:
    case PROT_STATE_RESET:
    case PROT_STATE_LPM:
    case PROT_STATE_PWR_DOWN:
    case PROT_STATE_NULL:
    case PROT_STATE_MAX:
    default:
      s = "No active protocols";
      break;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Return the active protocol name.
  */
  return s;

} /* mmoc_active_protocol_name() */


#ifndef FEATURE_MMOC_NO_SANITY_ERR_FATAL
/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_print_active_prot_sanity_err_fatal

DESCRIPTION
  This function returns the active protocol file name.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void                            mmoc_print_active_prot_sanity_err_fatal
(
    mmoc_sanity_err_e_type             reason,
       /* Reason for "insanity".
       */

    sd_ss_e_type                       ss,
      /* Which active protocol.
      */


    const mmoc_state_info_s_type      *mmoc_info_ptr
       /* Pointer to MMoC's state information.
       */
)
{
  MMOC_ASSERT( mmoc_info_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Determine the active protocol.
  */
  switch ( mmoc_info_ptr->prot_state[ss] )
  {
    case PROT_STATE_ONLINE_CDMA:
    case PROT_STATE_OFFLINE_CDMA:
      #ifdef MMOC_CDMA_SUPPORTED
      mc_cdma_multimode_sanity_err_fatal( reason );
      #endif
      break;

    case PROT_STATE_ONLINE_DED_MEAS:
      #if defined(MMOC_WLAN_SUPPORTED) && defined(FEATURE_WLAN_MEASUREMENT)
#error code not present
      #endif
      #ifdef MMOC_CDMA_SUPPORTED
      if( ss == SD_SS_MAIN || ss == SD_SS_HYBR_HDR )
      {
        mc_cdma_multimode_sanity_err_fatal( reason );
      }
      #endif
      break;

    case PROT_STATE_ONLINE_AMPS:
    case PROT_STATE_OFFLINE_AMPS:
      #ifdef MMOC_AMPS_SUPPORTED
      mc_amps_multimode_sanity_err_fatal( reason );
      #endif
      break;

    case PROT_STATE_ONLINE_GW:
      #ifdef MMOC_GW_SUPPORTED
      mmoc_reg_multimode_sanity_err_fatal( reason );
      #endif
      break;

    case PROT_STATE_ONLINE_HDR:
      /* Insert HDR fn here.
      */
      #ifdef MMOC_HDR_SUPPORTED
#error code not present
      #endif
      break;


    case PROT_STATE_ONLINE_WLAN:
      /* Insert WLAN fn here.
      */
      #ifdef MMOC_WLAN_SUPPORTED
#error code not present
      #endif
      break;

    case PROT_STATE_FTM:
      /* Insert FTM fn here.
      */
      break;

    case PROT_STATE_GPSONE_MSBASED:
    case PROT_STATE_OFFLINE:
    case PROT_STATE_PWR_SAVE:
    case PROT_STATE_RESET:
    case PROT_STATE_LPM:
    case PROT_STATE_PWR_DOWN:
    case PROT_STATE_NULL:
    case PROT_STATE_MAX:
    default:
      break;
  }

} /* mmoc_print_active_prot_sanity_err_fatal() */

#endif /* FEATURE_MMOC_NO_SANITY_ERR_FATAL */



/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_display_debug_info

DESCRIPTION
  This function displays the debug info as F3 messages.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void                            mmoc_display_debug_info
(
    const mmoc_state_info_s_type             *mmoc_info_ptr
       /* Pointer to MMoC's state information.
       */
)
{


  MMOC_ASSERT( mmoc_info_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  switch ( mmoc_info_ptr->trans_state )
  {
    case MMOC_TRANS_STATE_WAIT_DEACTD_CNF:
        (void) std_strlprintf( mmoc_debug_str, sizeof(mmoc_debug_str),
                      "Deactivate request sent to %s ",
                      mmoc_active_protocol_name( mmoc_info_ptr->deact_req_index,
                                                 mmoc_info_ptr )
                    );

      break;

    case MMOC_TRANS_STATE_WAIT_PH_STAT_CNF:
      {
        int i;
        /* Search for pending confirmation.
        */
        for (i=0; i < (int)SD_MODE_MAX; i++ )
        {
          if ( ph_stat_cmd_tbl[i].trans_id != PROT_TRANS_NONE )
          {
            (void) std_strlprintf( mmoc_debug_str, sizeof(mmoc_debug_str),
                        "Ph. Status request sent to %s ",
                        ph_stat_cmd_tbl[i].task_name
                      );

            /* there is pending cnf.
            */
            break;
          }
        }
        
      }
      break;

    case MMOC_TRANS_STATE_WAIT_GEN_CMD_CNF:
      (void) std_strlprintf( mmoc_debug_str, sizeof(mmoc_debug_str),
                      "Generic cmd request sent to %s ",
                      mmoc_active_protocol_name(
                        mmoc_info_ptr->trans_info.gen_prot_cmd.cmd_info.ss,
                        mmoc_info_ptr )
                    );
      break;

    case MMOC_TRANS_STATE_WAIT_AUTO_DEACTD_IND:
      (void) std_strlprintf( mmoc_debug_str, sizeof(mmoc_debug_str),
                      "Wait for deactd ind from %s",
                      mmoc_active_protocol_name(
                        mmoc_info_ptr->trans_info.gen_prot_cmd.cmd_info.ss,
                        mmoc_info_ptr )
                    );
      break;

    case MMOC_TRANS_STATE_PROT_DEACT_ENTER:
      (void) std_strlprintf( mmoc_debug_str, sizeof(mmoc_debug_str),
                      "Deactivate req. entry state"
                    );
      break;

    case MMOC_TRANS_STATE_PROT_PH_STAT_ENTER:
        (void) std_strlprintf( mmoc_debug_str, sizeof(mmoc_debug_str),
                        "Phone status req. entry state"
                      );
        break;

    case MMOC_TRANS_STATE_GEN_CMD_ENTER:
      (void) std_strlprintf( mmoc_debug_str, sizeof(mmoc_debug_str),
                      "Generic cmd req. entry state"
                    );
      break;

    case MMOC_TRANS_STATE_NULL:
      (void) std_strlprintf( mmoc_debug_str, sizeof(mmoc_debug_str),
                      "In NULL transaction state"
                    );
      break;

    case MMOC_TRANS_STATE_MAX:
    default:
      (void) std_strlprintf( mmoc_debug_str, sizeof(mmoc_debug_str),
                      "Invalid transaction state %d",
                      mmoc_info_ptr->trans_state
                    );
      break;
  } /* switch ( mmoc_info_ptr->trans_state ) */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Print the debug string as ERROR message.
  */
  MSG_ERROR(mmoc_debug_str, 0, 0, 0 );


} /* mmoc_display_debug_info() */

#ifndef FEATURE_MMOC_NO_SANITY_ERR_FATAL
/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_print_sanity_err_fatal

DESCRIPTION
  This function calls the insanity timeout err_fatal.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void                            mmoc_print_sanity_err_fatal
(
    const mmoc_state_info_s_type             *mmoc_info_ptr
       /* Pointer to MMoC's state information.
       */
)
{

  word   err_code_no;

  MMOC_ASSERT( mmoc_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize it as mmoc.c
  */
  (void) std_strlprintf( mmoc_debug_str, sizeof(mmoc_debug_str), "mmoc.c" );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  switch ( mmoc_info_ptr->trans_state )
  {
    case MMOC_TRANS_STATE_WAIT_DEACTD_CNF:
      mmoc_print_active_prot_sanity_err_fatal( MMOC_SANITY_ERR_DEACT_CMD_CNF,
                                               mmoc_info_ptr->deact_req_index,
                                               mmoc_info_ptr
                    );
      err_code_no = (word)__LINE__;
      break;

    case MMOC_TRANS_STATE_WAIT_PH_STAT_CNF:
      {
        int i;
        /* Search for pending confirmation.
        */
        for (i=0; i < (int)SD_MODE_MAX; i++ )
        {
          if ( ph_stat_cmd_tbl[i].trans_id != PROT_TRANS_NONE )
          {
            if ( ph_stat_cmd_tbl[i].sanity_err_fn )
            {
              (*ph_stat_cmd_tbl[i].sanity_err_fn)
              ( MMOC_SANITY_ERR_PH_STAT_CMD_CNF );
            }

            /* there is pending cnf.
            */
            break;
          }
        }

        
        err_code_no = (word)__LINE__;
      }
      break;

    case MMOC_TRANS_STATE_WAIT_GEN_CMD_CNF:
      mmoc_print_active_prot_sanity_err_fatal( MMOC_SANITY_ERR_GENERIC_CMD_CNF,
            mmoc_info_ptr->trans_info.gen_prot_cmd.cmd_info.ss,
            mmoc_info_ptr
          );
      err_code_no = (word)__LINE__;
      break;


    case MMOC_TRANS_STATE_WAIT_AUTO_DEACTD_IND:
      mmoc_print_active_prot_sanity_err_fatal( MMOC_SANITY_ERR_GENERIC_CMD_CNF,
            mmoc_info_ptr->trans_info.gen_prot_cmd.cmd_info.ss,
            mmoc_info_ptr
          );
      err_code_no = (word)__LINE__;
      break;

    case MMOC_TRANS_STATE_PROT_DEACT_ENTER:
      err_code_no = (word)__LINE__;
      break;

    case MMOC_TRANS_STATE_PROT_PH_STAT_ENTER:
        err_code_no = (word)__LINE__;
        break;

    case MMOC_TRANS_STATE_GEN_CMD_ENTER:
      err_code_no = (word)__LINE__;
      break;

    case MMOC_TRANS_STATE_NULL:
      err_code_no = (word)__LINE__;
      break;

    case MMOC_TRANS_STATE_WAIT_TMC_MODE_CNF:
      err_code_no = (word)__LINE__;
      break;

    case MMOC_TRANS_STATE_MAX:
    default:
      err_code_no = (word)__LINE__;
      break;
  } /* switch ( mmoc_info_ptr->trans_state ) */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Call err_fatal function.
  */
  err_fatal_put_log( err_code_no, mmoc_debug_str );

} /* mmoc_print_sanity_err_fatal() */

#endif /* FEATURE_MMOC_NO_SANITY_ERR_FATAL */






/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_set_sd_initialized

DESCRIPTION
  This function sets the MMoC's state to SD initialized.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void                            mmoc_set_sd_initialized
(

    mmoc_state_info_s_type             *mmoc_info_ptr
       /* Pointer to MMoC's state information.
       */
)
{
  MMOC_ASSERT( mmoc_info_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  MMOC_MSG_LOW("sd_initialized(),curr_trans %d,trans_state %d",
               mmoc_info_ptr->curr_trans,
               mmoc_info_ptr->trans_state,
               0
              );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Set the MMoC's is_sd_initialized to TRUE,
  */
  mmoc_info_ptr->is_sd_initialized = TRUE;


} /* mmoc_set_sd_initialized() */

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_set_sd_term

DESCRIPTION
  This function sets the MMoC's state to SD uninitialized.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void                            mmoc_set_sd_term
(

    mmoc_state_info_s_type             *mmoc_info_ptr
       /* Pointer to MMoC's state information.
       */
)
{
  MMOC_ASSERT( mmoc_info_ptr != NULL );



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/



  MMOC_MSG_LOW("sd_term(),curr_trans %d,trans_state %d",
               mmoc_info_ptr->curr_trans,
               mmoc_info_ptr->trans_state,
               0
              );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Set the MMoC's is_sd_initialized to FALSE.
  */
  if ( mmoc_info_ptr->is_sd_initialized )
  {
    /* Terminate system determination.
    */
    sd_term();
    mmoc_info_ptr->is_sd_initialized = FALSE;
  }


} /* mmoc_set_sd_term() */



/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_initialize_rf

DESCRIPTION
  This function initializes the rf if it was not before.

DEPENDENCIES
  None.

RETURN VALUE
  Returns TRUE if rf was initialized properly,
          FALSE otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean                            mmoc_initialize_rf
(

    mmoc_state_info_s_type             *mmoc_info_ptr
       /* Pointer to MMoC's state information.
       */
)
{
  boolean status = TRUE;

  MMOC_ASSERT( mmoc_info_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  MMOC_MSG_LOW("initialize_rf(),curr_trans %d,trans_state %d",
               mmoc_info_ptr->curr_trans,
               mmoc_info_ptr->trans_state,
               0
              );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Check if rf was initialized previously.
  */
  if ( !mmoc_info_ptr->is_rf_initialized )
  {
    /* RF not initialized before, initialize it.
    */
    #ifndef FEATURE_MMOC_UNIT_TEST
    #if defined(RF_MULTIMODE) || defined(FEATURE_MULTIMODE_RFAPI)
    status = rfm_init(
                       #ifdef FEATURE_MULTI_RX_CHAIN
                       RFCOM_TRANSCEIVER_0,
                       #endif /* FEATURE_MULTI_RX_CHAIN */
                       rex_self(),
                       MMOC_NV_CMD_SIG,
                       (void (*)( rex_sigs_type )) mmoc_wait
                     );
    #else /* RF_MULTIMODE */
    status = rf_init( rex_self(),
                      MMOC_NV_CMD_SIG,
                      (void (*)( rex_sigs_type )) mmoc_wait
                    );
    #endif /* RF_MULTIMODE */
    #endif

    /* Set RF initialized flag.
    */
    mmoc_info_ptr->is_rf_initialized = TRUE;

  } /* if ( !mmoc_info_ptr->is_rf_initialized ) */

  return status;
} /* mmoc_initialize_rf() */


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_map_sys_oprt_mode_to_mmoc

DESCRIPTION
  This function sets the MMoC's state to SD uninitialized.

DEPENDENCIES
  None.

RETURN VALUE
  mmoc type operating mode status.

SIDE EFFECTS
  None.
===========================================================================*/
static prot_oprt_mode_e_type           mmoc_map_sys_oprt_mode_to_mmoc
(

    sys_oprt_mode_e_type               sys_oprt_mode
       /* Operating mode - SYS type.
       */
)
{
  prot_oprt_mode_e_type  mmoc_oprt_mode = PROT_OPRT_MODE_MAX;

  MMOC_ASSERT_ENUM_IS_INRANGE( sys_oprt_mode, SYS_OPRT_MODE_MAX );
 

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Switch on the sys_oprt_mode.
  */
  switch ( sys_oprt_mode ) 
  {
    case SYS_OPRT_MODE_PWROFF:
      mmoc_oprt_mode = PROT_OPRT_MODE_PWROFF;
      break;

    case SYS_OPRT_MODE_OFFLINE:
      mmoc_oprt_mode = PROT_OPRT_MODE_OFFLINE;
      break;

    case SYS_OPRT_MODE_OFFLINE_AMPS:
      mmoc_oprt_mode = PROT_OPRT_MODE_OFFLINE_AMPS;
      break;

    case SYS_OPRT_MODE_OFFLINE_CDMA:
      mmoc_oprt_mode = PROT_OPRT_MODE_OFFLINE_CDMA;
      break;

    case SYS_OPRT_MODE_ONLINE:
      mmoc_oprt_mode = PROT_OPRT_MODE_ONLINE;
      break;

    case SYS_OPRT_MODE_FTM:
      mmoc_oprt_mode = PROT_OPRT_MODE_FTM;
      break;

    case SYS_OPRT_MODE_RESET:
    case SYS_OPRT_MODE_RESET_MODEM:
      mmoc_oprt_mode = PROT_OPRT_MODE_RESET;
      break;


    case SYS_OPRT_MODE_LPM:
      mmoc_oprt_mode = PROT_OPRT_MODE_LPM;
      break;

    case SYS_OPRT_MODE_NONE:
    case SYS_OPRT_MODE_NET_TEST_GW:
    case SYS_OPRT_MODE_OFFLINE_IF_NOT_FTM:
    case SYS_OPRT_MODE_PSEUDO_ONLINE:
    case SYS_OPRT_MODE_MAX:
    #ifdef FEATURE_RPC
#error code not present
    #endif /* FEATURE_RPC */

    default:
      MMOC_ERR_FATAL("Incorrect sys_oprt_mode", 0, 0, 0 );
      break;

  } /* switch( sys_oprt_mode ) */


  return mmoc_oprt_mode;

} /* mmoc_map_sys_oprt_mode_to_mmoc() */


/* ^L<EJECT> */
/*===========================================================================

FUNCTION mmoc_do_pwr_save_exit

DESCRIPTION
  This function will determine if the power save exit needs to be done.
  The power save exit transaction should be invoked if the current MMoC's
  protocol state in POWER_SAVE and sd_act is other than CONTINUE or power
  save.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  -  Power save exit needs to be done, invoke power save exit
           transaction handler.
  FALSE -  Power save exit need not be done, continue with normal
           processing.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean                         mmoc_do_pwr_save_exit
(
       sd_ss_act_e_type                sd_act,
          /* The current sd_act.
          */

       const mmoc_state_info_s_type   *mmoc_info_ptr
          /* Pointer to MMoC's state information.
          */
)
{
  boolean do_pwr_save_exit = FALSE;

  MMOC_ASSERT( mmoc_info_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Check if current protocol state is power save and sd_act is not continue
  ** or power save.
  */
  if ( mmoc_info_ptr->prot_state[SD_SS_MAIN] == PROT_STATE_PWR_SAVE &&
       mmoc_info_ptr->curr_trans  != MMOC_TRANS_PWR_SAVE_EXIT &&
       sd_act != SD_SS_ACT_CONTINUE &&
       sd_act != SD_SS_ACT_PWR_SAVE
     )
  {
    /* Perform power save exit, return TRUE.
    */
    do_pwr_save_exit = TRUE;

  } /* if ( mmoc_info_ptr->prot_state == PROT_STATE_PWR_SAVE) */


  return do_pwr_save_exit;

} /* mmoc_do_pwr_save_exit() */


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_accept_cmds

DESCRIPTION
  This function will determine if the commands have to be dequeued and
  processed. Based on the return value, the REX signal corresponding to the
  command queue has to be set or reset.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - MMoC can accept commands.
  FALSE - do not accept any commands at this time.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean                         mmoc_accept_cmds
(
       const mmoc_state_info_s_type    *mmoc_info_ptr
          /* Pointer to MMoC's state information.
          */
)
{
  MMOC_ASSERT( mmoc_info_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Check if transaction is in progress.
  */
  if ( mmoc_info_ptr->curr_trans  == MMOC_TRANS_NULL )
  {
    /* No transaction in progress, so can process next command.
    */
    return TRUE;
  }

  return FALSE;

} /* mmoc_accept_cmds() */


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_is_prot_deactivating

DESCRIPTION
  This function will determine if the protocol needs to be deactivated based
  on the SS action.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  -  Protocol is deactivating.
  FALSE -  No protocol is not deactivating.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean                         mmoc_is_prot_deactivating
(
       prot_state_e_type               prot_state,
          /* Active protocol.
          */

       sd_ss_e_type                    ss,
          /* Indicates the SS-Instance this
          ** action is intended for.
          */

       sd_ss_act_e_type                next_act
          /* Next SD SS-Action.
          */
)
{
  boolean is_prot_deactivating = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Check if protocol should deactivate.
  */
  switch ( prot_state )
  {
    case PROT_STATE_ONLINE_CDMA:
      if ( next_act != SD_SS_ACT_ACQ_CDMA  &&
           next_act != SD_SS_ACT_CONTINUE )
      {
        is_prot_deactivating = TRUE;
      }
      break;

    case PROT_STATE_ONLINE_HDR:
      if ( next_act != SD_SS_ACT_ACQ_HDR  &&
           next_act != SD_SS_ACT_CONTINUE )
      {
        is_prot_deactivating = TRUE;
      }
      break;

    case PROT_STATE_ONLINE_AMPS:
      if ( next_act != SD_SS_ACT_ACQ_AMPS  &&
           next_act != SD_SS_ACT_CONTINUE )
      {
        is_prot_deactivating = TRUE;
      }
      break;

    case PROT_STATE_ONLINE_GW:
      if ( next_act != SD_SS_ACT_ACQ_GW  &&
           next_act != SD_SS_ACT_PWR_SCAN_GW  &&
           next_act != SD_SS_ACT_GET_NET_GW  &&
           next_act != SD_SS_ACT_CONTINUE 
           #ifdef FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH
           && next_act != SD_SS_ACT_MEAS_BACK
           #endif
         )
      {
        is_prot_deactivating = TRUE;
      }
      break;

    case PROT_STATE_ONLINE_DED_MEAS:
      if ( ss != SD_SS_HYBR_WLAN &&
           next_act != SD_SS_ACT_MEAS_DED &&
           next_act != SD_SS_ACT_CONTINUE )
      {
        is_prot_deactivating = TRUE;
      }

      #ifdef FEATURE_WLAN_MEASUREMENT
#error code not present
      #endif
      break;


    case PROT_STATE_ONLINE_WLAN:
      if ( next_act != SD_SS_ACT_ACQ_WLAN  &&
           #ifdef FEATURE_WLAN_MEASUREMENT
#error code not present
           #endif
           next_act != SD_SS_ACT_SURVEY_WLAN  &&
           next_act != SD_SS_ACT_CONTINUE
         )
      {
        is_prot_deactivating = TRUE;
      }
      break;

    case PROT_STATE_NULL:
    default:
      MMOC_MSG_ERROR ("Invalid case %d", prot_state, 0, 0 );
      break;

  } /* switch( act_protocol ) */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* return.
  */
  return is_prot_deactivating;

} /* mmoc_is_prot_deactivating() */


/* <EJECT> */
/*===========================================================================

FUNCTION  mmoc_send_cm_rpt

DESCRIPTION
  This is a utility function called by the transaction state handlers to send
  the report to CM.

  Note! Only CM reports with no parameters can use this function.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void                            mmoc_send_cm_rpt
(
    cm_name_type                       rpt_name
       /* Type of report to send.
       */
)
{

  /* Pointer to CM report buffer. */
  cm_sd_rpt_u_type *rpt_ptr;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Get the message buffer.
  */
  rpt_ptr = cm_sd_rpt_get_buf_else_err_fatal();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Fill in the message buffer and place it in CM report queue.
  */
  MMOC_MSG_HIGH("Sending CM report %d", rpt_name, 0, 0 );
  rpt_ptr->hdr.cmd        = rpt_name;
  cm_sd_rpt(rpt_ptr);


} /* mmoc_send_cm_rpt() */


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_sanity_timer_exp

DESCRIPTION
  This function will check if a particular transaction is stuck in MMoC
  thereby validating the MMoC sanity.
  The function will do ERR_FATAL if it determines that the MMoC is insane.

  Note! This function should be called at the expiration of the sanity timer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
MMEXTN void                            mmoc_sanity_timer_exp
(

       mmoc_state_info_s_type          *mmoc_state_ptr
          /* Pointer to MMoC's state information.
          */

)
{
  rex_timer_cnt_type  sanity_timeout = MMOC_SANITY_TIME;

  MMOC_ASSERT( mmoc_state_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Transaction has lasted more than sanity time, print debug messages.
  */
  MMOC_MSG_ERROR( "Curr_trans  %d", mmoc_state_ptr->curr_trans,  0, 0 );
  MMOC_MSG_ERROR( "Trans_state %d", mmoc_state_ptr->trans_state, 0, 0 );
  MMOC_MSG_ERROR( "Prot_state  %d %d", mmoc_state_ptr->prot_state[SD_SS_MAIN],
                  mmoc_state_ptr->prot_state[SD_SS_HYBR_HDR], 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if this was a race condition, sanity timer expired at the
  ** same time the transaction was made NULL.
  */
  if ( mmoc_state_ptr->curr_trans == MMOC_TRANS_NULL )
  {

    /* Clear insanity_count.
    */
    mmoc_state_ptr->insanity_count = 0;
    return;

  } /* if ( mmoc_state_ptr->curr_trans == MMOC_TRANS_NULL ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Print debug info about the offending protocol stack.
  */
  MMOC_MSG_ERROR( "No response yet for...", 0, 0, 0 );
  mmoc_display_debug_info( mmoc_state_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if defined(MMOC_HDR_SUPPORTED)
#error code not present
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Check if MAX insanity count was reached.
  */
  if ( mmoc_state_ptr->insanity_count++ == MMOC_MAX_INSANITY_COUNT )
  {
    #ifdef FEATURE_MMOC_NO_SANITY_ERR_FATAL

    /* Simulate the lower layer response and allow the transaction to
    ** proceed.
    */
    if ( mmoc_state_ptr->trans_state == MMOC_TRANS_STATE_WAIT_DEACTD_CNF )
    {
      /* Send deactivated confirmation.
      */
      mmoc_rpt_prot_deactd_cnf( mmoc_state_ptr->trans_id );
      MMOC_MSG_ERROR( "Simulating deactd_cnf", 0, 0, 0 );
    }
    else if ( mmoc_state_ptr->trans_state == MMOC_TRANS_STATE_WAIT_PH_STAT_CNF )
    {
      /* Send Phone status changed confirmation
      */
      mmoc_rpt_ph_stat_chgd_cnf( mmoc_state_ptr->trans_id, SD_MODE_INACT );
      MMOC_MSG_ERROR( "Simulating ph_stat_chgd_cnf", 0, 0, 0 );
    }
    else if ( mmoc_state_ptr->trans_state == MMOC_TRANS_STATE_WAIT_GEN_CMD_CNF )
    {
      /* Send protocol generic command confirmation.
      */
      mmoc_rpt_prot_gen_cmd_cnf( FALSE, mmoc_state_ptr->trans_id, FALSE );
      MMOC_MSG_ERROR( "Simulating prot_gen_cmd_cnf", 0, 0, 0 );

    }
    else if ( mmoc_state_ptr->trans_state == MMOC_TRANS_STATE_WAIT_TMC_MODE_CNF )
    {
      /* Send protocol generic command confirmation.
      */
      mmoc_rpt_tmc_mode_cnf (mmoc_state_ptr->trans_id);
      MMOC_MSG_ERROR( "Simulating tmc_mode_cnf", 0, 0, 0 );

    }
    #else
    /* MMoC stuck at this transaction for more than MAX_COUNT.
    */
    mmoc_print_sanity_err_fatal( mmoc_state_ptr );
    #endif
  }
  else
  {
    /* If it is powerdown transaction, load the special timeout value.
    */
    if ( mmoc_state_ptr->curr_trans == MMOC_TRANS_PWR_DOWN )
    {
      sanity_timeout = MMOC_PWRDOWN_SANITY_TIME;
    }

    /* Restart the sanity timer for the new transaction.
    */
    (void) rex_set_timer( &mmoc_state_ptr->sanity_timer,
                          sanity_timeout
                        );

  } /* else */



} /* mmoc_sanity_timer_exp() */



/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_activate_prot_online_ss_main

DESCRIPTION
  This is a utility function called by the transaction state handlers to
  activate the protocol based on the SD20 action.
  It would set the MMoC's protocol state after activating the protocol based
  on main SS_instance.

  Note! For some SD actions( example power save ), new transactions have to
  be invoked. In those cases, this function will change the current
  transaction and return EVT_STATUS_NOT_CONSUMED.

DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion. No new
                                 transaction needs to be invoked.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the new transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
static mmoc_evt_status_e_type          mmoc_activate_prot_online_ss_main
(
    sd_ss_act_e_type                   sd_act,
       /* Activate protocol based on this SD action.
       */

    mmoc_state_info_s_type             *mmoc_info_ptr
       /* Pointer to MMoC's state information.
       */
)
{
  mmoc_evt_status_e_type  evt_status = MMOC_EVT_STATUS_CONSUMED;

  MMOC_ASSERT( mmoc_info_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Check if power save exit needs to be done now.
  */
  if ( mmoc_do_pwr_save_exit( sd_act, mmoc_info_ptr ) )
  {
    /* Do power save exit now.
    */
    mmoc_update_curr_trans( MMOC_TRANS_PWR_SAVE_EXIT,
                            mmoc_info_ptr,
                            SYS_OPRT_MODE_NONE
                          );
    return MMOC_EVT_STATUS_NOT_CONSUMED;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Initialize RF before activating any protocols.
  */
  if (!mmoc_initialize_rf( mmoc_info_ptr ))
  {
    MMOC_MSG_ERROR("rfm_init failed, going offline", 0, 0, 0);
    (void)cm_ph_cmd_oprt_mode( NULL,
                               NULL,
                               CM_CLIENT_ID_ANONYMOUS,
                               SYS_OPRT_MODE_OFFLINE
                             );

    return MMOC_EVT_STATUS_CONSUMED;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check of gps_exit rpt needs to be sent.
  */
  if ( mmoc_info_ptr->prot_state[SD_SS_MAIN] == PROT_STATE_GPSONE_MSBASED &&
       sd_act != SD_SS_ACT_CONTINUE &&
       sd_act != SD_SS_ACT_ACQ_GPS
     )
  {
    /* send GPS exit to CM.
    */
    mmoc_send_cm_rpt( CM_GPS_MODE_EXIT_F );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Activate the protocol based on the SD action or return
  ** EVT_STATUS_NOT_CONSUMED to invoke new transaction.
  */
  switch ( sd_act ) 
  {
    #ifdef MMOC_CDMA_SUPPORTED
    case SD_SS_ACT_ACQ_CDMA:
      /* If SD says that CDMA should be in pseudo online mode, then bring up
      ** the CDMA protocol in pseudo online mode, else bring it up in online
      ** mode.
      */

      if( sd_misc_is_cdma_pseudo_online() )
      {
        mc_cdma_activate_protocol( PROT_ACT_PSEUDO_ONLINE );
      }
      else if( sd_misc_is_handoff (SD_SS_HO_HDR_TO_CDMA) )
      {
        mc_cdma_activate_protocol( PROT_ACT_ONLINE_HDR_CDMA_HO );
      }
      else
      {
        mc_cdma_activate_protocol( PROT_ACT_ONLINE_ACQ );
      }
      mmoc_update_prot_state( PROT_STATE_ONLINE_CDMA,
                              SD_SS_MAIN,
                              mmoc_info_ptr
                            );
      mmoc_update_db_dmss_state( mmoc_info_ptr );
      break;
    #endif

    #ifdef MMOC_AMPS_SUPPORTED
    case SD_SS_ACT_ACQ_AMPS:
      mc_amps_activate_protocol( PROT_ACT_ONLINE_ACQ );
      mmoc_update_prot_state( PROT_STATE_ONLINE_AMPS,
                              SD_SS_MAIN,
                              mmoc_info_ptr
                            );
      mmoc_update_db_dmss_state( mmoc_info_ptr );
      break;
    #endif

    #ifdef MMOC_GW_SUPPORTED
    case SD_SS_ACT_ACQ_GW:
    case SD_SS_ACT_GET_NET_GW:
    case SD_SS_ACT_PWR_SCAN_GW:

      /* With GW acquisition request on main and if HDR is active on
      ** the hybrid instance deactivate hdr.
      */
      if (mmoc_info_ptr->prot_state[SD_SS_HYBR_HDR] == PROT_STATE_ONLINE_HDR)
      {             
        /* Deactivation request needs to be sent for online HDR.
        */
        mmoc_update_trans_state( MMOC_TRANS_STATE_HDR_DEACT_ENTER,
                                  mmoc_info_ptr
                                );

        MMOC_MSG_HIGH (" GW acq from SD, deact HDR ", 0, 0, 0);

        return MMOC_EVT_STATUS_NOT_CONSUMED;       
      }    

      cmregprx_cmd_gw_activate_protocol( PROT_ACT_ONLINE_ACQ );
      mmoc_update_prot_state( PROT_STATE_ONLINE_GW,
                             SD_SS_MAIN,
                             mmoc_info_ptr
                           );
      mmoc_update_db_dmss_state( mmoc_info_ptr );
      break;
    #endif

    #ifdef MMOC_HDR_SUPPORTED
#error code not present
    #endif

    #ifdef MMOC_CDMA_SUPPORTED
    case SD_SS_ACT_MEAS_DED:
      mc_cdma_activate_protocol( PROT_ACT_DED_MEAS_REQUEST );
      mmoc_update_prot_state( PROT_STATE_ONLINE_DED_MEAS,
                              SD_SS_MAIN,
                              mmoc_info_ptr
                            );
      mmoc_update_db_dmss_state( mmoc_info_ptr );
      break;
    #endif

    #ifdef FEATURE_GPSONE_OOS
#error code not present
    #endif

    case SD_SS_ACT_PWR_SAVE:
      /* New "power save enter transaction" needs to be invoked,
      ** curr_trans should be "pwr save enter".
      */
      (void)sd_ss_ind_misc_pwr_save_enter(NULL);
      MMOC_ASSERT( mmoc_info_ptr->curr_trans != MMOC_TRANS_PWR_SAVE_ENTER);
      if ( mmoc_info_ptr->prot_state[SD_SS_MAIN] != PROT_STATE_PWR_SAVE )
      {
        mmoc_update_curr_trans( MMOC_TRANS_PWR_SAVE_ENTER,
                                mmoc_info_ptr,
                                SYS_OPRT_MODE_NONE
                              );
        evt_status = MMOC_EVT_STATUS_NOT_CONSUMED;
      }
      mmoc_update_prot_state( PROT_STATE_PWR_SAVE,
                              SD_SS_MAIN,
                              mmoc_info_ptr
                            );
      /* Need a seperate db_dmss_state here.
      */
      break;


    #ifdef MMOC_WLAN_SUPPORTED
#error code not present
    #endif

    case SD_SS_ACT_CONTINUE:
      /* Do nothing.
      */
      break;

    case SD_SS_ACT_MEAS_BACK:
    case SD_SS_ACT_ACCEPT:
    case SD_SS_ACT_REJECT:
    case SD_SS_ACT_RELEASE:
    case SD_SS_ACT_MAX:
    default:
      MMOC_MSG_ERROR(" Err: SD act %d", sd_act, 0, 0);
      break;

  } /* switch( sd_act ) ) */ 


  return evt_status;

} /* mmoc_activate_prot_online_ss_main */

#if defined(MMOC_HYBR_1_SUPPORTED) || defined(MMOC_HYBR_SUPPORTED)
/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_activate_prot_online_ss_hdr

DESCRIPTION
  This is a utility function called by the transaction state handlers to
  activate the protocol based on the SD20 action for SS_HDR instance.
  It would set the MMoC's protocol state after activating the protocol.

  Note! For some SD actions( example power save ), new transactions have to
  be invoked. In those cases, this function will change the current
  transaction and return EVT_STATUS_NOT_CONSUMED.

DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion. No new
                                 transaction needs to be invoked.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the new transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
/*lint -esym(715,mmoc_info_ptr)*/
static mmoc_evt_status_e_type          mmoc_activate_prot_online_ss_hdr
(
    sd_ss_act_e_type                   sd_act,
       /* Activate protocol based on this SD action.
       */

    mmoc_state_info_s_type            *mmoc_info_ptr
       /* Pointer to MMoC's state information.
       */
)
{

  mmoc_evt_status_e_type  evt_status = MMOC_EVT_STATUS_NOT_CONSUMED;
  #ifdef FEATURE_HICPS_STACK_SYNC_UP
#error code not present
  #endif /* FEATURE_HICPS_STACK_SYNC_UP */

  MMOC_ASSERT( mmoc_info_ptr != NULL );

  /* Can't be const as functions called from here modify it.
  ** Example - mmoc_update_prot_state()  
  */ 
  SYS_ARG_NOT_CONST(mmoc_info_ptr);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  while ( evt_status != MMOC_EVT_STATUS_CONSUMED )
  {
    evt_status = MMOC_EVT_STATUS_CONSUMED;

    /* Activate the protocol based on the SD action.
    */
    switch ( sd_act )
    {
      #ifdef MMOC_HDR_SUPPORTED
#error code not present
      #endif

      #ifdef MMOC_HYBR_GW_SUPPORTED
      case SD_SS_ACT_ACQ_GW:
      case SD_SS_ACT_GET_NET_GW:
      case SD_SS_ACT_PWR_SCAN_GW:

        //Sarav: Should not check in with the following #if
        #if 0
        /* With GW acquisition request on HYBR_1 and if HDR is active on
        ** the hybrid instance deactivate hdr.
        */
        if (mmoc_info_ptr->prot_state[SD_SS_HYBR_1] == PROT_STATE_ONLINE_HDR)
        {             
          /* Deactivation request needs to be sent for online HDR.
          */
          mmoc_update_trans_state( MMOC_TRANS_STATE_HDR_DEACT_ENTER,
                                    mmoc_info_ptr
                                  );

          MMOC_MSG_HIGH (" GW acq from SD, deact HDR ", 0, 0, 0);

          return MMOC_EVT_STATUS_NOT_CONSUMED;       
        }
        #endif

        cmregprx_cmd_gw_activate_protocol( PROT_ACT_ONLINE_ACQ );
        mmoc_update_prot_state( PROT_STATE_ONLINE_GW,
                               SD_SS_HYBR_1,
                               mmoc_info_ptr
                             );
        mmoc_update_db_dmss_state( mmoc_info_ptr );
        break;
      #endif

      #ifdef MMOC_HYBR_1_SUPPORTED
      case SD_SS_ACT_PWR_SAVE:
        sd_act = sd_ss_ind_hybr_1_misc_pwr_save_enter( NULL );
        if ( sd_act != SD_SS_ACT_CONTINUE)
        {
          evt_status = MMOC_EVT_STATUS_NOT_CONSUMED;
        }
        mmoc_update_prot_state( PROT_STATE_PWR_SAVE,
                                SD_SS_HYBR_1,
                                mmoc_info_ptr
                              );
        break;
      #endif

      case SD_SS_ACT_CONTINUE:
        #if defined(MMOC_HDR_SUPPORTED) && \
            defined(FEATURE_HICPS_STACK_SYNC_UP)
#error code not present
        #endif /* MMOC_HDR_SUPPORTED */
        break;

      case SD_SS_ACT_ACQ_CDMA:
      case SD_SS_ACT_ACQ_AMPS:
      case SD_SS_ACT_MEAS_DED:
      case SD_SS_ACT_MEAS_BACK:
      case SD_SS_ACT_ACCEPT:
      case SD_SS_ACT_REJECT:
      case SD_SS_ACT_RELEASE:
      case SD_SS_ACT_ACQ_GPS:
      case SD_SS_ACT_ACQ_WLAN:
      case SD_SS_ACT_SURVEY_WLAN:
      case SD_SS_ACT_MAX:
      default:
        MMOC_MSG_ERROR(" Err: SD act %d", sd_act, 0, 0);
        break;

    } /* switch( sd_act ) ) */
 
  } /* while ( sd_act ) */

  return evt_status;

} /* mmoc_activate_prot_online_ss_hdr() */
/*lint +esym(715,mmoc_info_ptr)*/
/*lint +esym(529,is_hdr_acq_blocked) */ /* not referenced */
#endif /* (MMOC_HYBR_1_SUPPORTED) || (MMOC_HYBR_SUPPORTED) */

#ifdef MMOC_WLAN_SUPPORTED
#error code not present
#endif /* MMOC_WLAN_SUPPORTED */


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_activate_prot_online

DESCRIPTION
  This is a utility function called by the transaction state handlers to
  activate the protocol based on the SD20 action.


DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion. No new
                                 transaction needs to be invoked.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the new transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
static mmoc_evt_status_e_type          mmoc_activate_prot_online
(
    sd_ss_e_type                       ss,
       /* Activate protocol for this System selection instance.
       */

    mmoc_state_info_s_type             *mmoc_info_ptr
       /* Pointer to MMoC's state information.
       */
)
{
  mmoc_evt_status_e_type  evt_status = MMOC_EVT_STATUS_CONSUMED;

  MMOC_ASSERT( mmoc_info_ptr != NULL );
  

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Invoke the activate_prot function based on the SS instance.
  */
  switch ( ss )
  {
    case SD_SS_MAIN:
    evt_status =  mmoc_activate_prot_online_ss_main(
                                         sd_ss_act_get( NULL ),
                                         mmoc_info_ptr
                                                   );
      break;

    case SD_SS_HYBR_HDR:
    #ifdef  MMOC_HYBR_1_SUPPORTED
    evt_status = mmoc_activate_prot_online_ss_hdr(
                                     sd_ss_hybr_1_act_get( NULL ),
                                     mmoc_info_ptr
                                                 );
    #endif /*  MMOC_HYBR_1_SUPPORTED */
      break;


    case SD_SS_HYBR_WLAN:
      #ifdef MMOC_WLAN_SUPPORTED
#error code not present
      #endif /* MMOC_WLAN_SUPPORTED */
      break;


    case SD_SS_MAX:
    default:
    MMOC_ERR_FATAL("incorrect ss", 0, 0, 0 );

  } /* switch ( ss ) */

  return evt_status;

} /* mmoc_activate_prot_online() */

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_activate_prot_offline

DESCRIPTION
  This is a utility function called by the transaction state handlers to
  activate the protocol based the current operating mode.
  It would set the MMoC's protocol state after activating the protocol.

  Note! This function should be called only when the operating mode is
  OFFLINE_CDMA or OFFLINE_AMPS.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void                            mmoc_activate_prot_offline
(

    mmoc_state_info_s_type             *mmoc_info_ptr
       /* Pointer to MMoC's state information.
       */
)
{

  MMOC_ASSERT( mmoc_info_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Activate the protocol based on the offline operating mode.
  */
  switch ( mmoc_info_ptr->curr_oprt_mode )
  {
    case PROT_OPRT_MODE_OFFLINE_CDMA:
      #ifdef MMOC_CDMA_SUPPORTED
      #ifdef FEATURE_MMOC_SELECTIVE_ACTIVATION
      if (msm_support_is_available(MSM_SUPPORT_1X)==MSM_SUPPORT_ENABLED)
      #endif /* MMOC_GW_SUPPORTED */
      {
        mc_cdma_activate_protocol( PROT_ACT_OFFLINE );
      }
      #endif /* FEATURE_MMOC_SELECTIVE_ACTIVATION */
      mmoc_update_prot_state( PROT_STATE_OFFLINE_CDMA,
                              SD_SS_MAIN,
                              mmoc_info_ptr
                            );
      mmoc_update_db_dmss_state( mmoc_info_ptr );
      break;

    case PROT_OPRT_MODE_OFFLINE_AMPS:
      #ifdef MMOC_AMPS_SUPPORTED
      mc_amps_activate_protocol( PROT_ACT_OFFLINE );
      #endif
      mmoc_update_prot_state( PROT_STATE_OFFLINE_AMPS,
                              SD_SS_MAIN,
                              mmoc_info_ptr
                            );
      mmoc_update_db_dmss_state( mmoc_info_ptr );
      break;

    case PROT_OPRT_MODE_OFFLINE:
      mmoc_update_prot_state( PROT_STATE_OFFLINE,
                              SD_SS_MAIN,
                              mmoc_info_ptr
                            );
      /* Need a seperate db_dmss_state here.
      */
      mmoc_update_db_dmss_state( mmoc_info_ptr );
      break;

    case PROT_OPRT_MODE_FTM:
      /* Activate the protocol stack in FTM mode.
      */
      #if defined(MMOC_FTM_SUPPORTED)
      #if defined(MMOC_CDMA_SUPPORTED)
      #ifdef FEATURE_MMOC_SELECTIVE_ACTIVATION
      if (msm_support_is_available(MSM_SUPPORT_1X)==MSM_SUPPORT_ENABLED)
      #endif /* MMOC_GW_SUPPORTED */
      {
        mc_cdma_activate_protocol( PROT_ACT_FTM );
      }
      #endif /* MMOC_CDMA_SUPPORTED */
      #if defined(MMOC_GW_SUPPORTED) || defined(MMOC_CDMA_SUPPORTED)
      ftm_activate_protocol( PROT_ACT_FTM );
      #endif
      mmoc_update_prot_state( PROT_STATE_FTM,
                              SD_SS_MAIN,
                              mmoc_info_ptr
                            );
      mmoc_update_db_dmss_state( mmoc_info_ptr );
      #else
      MMOC_ERR_FATAL("FTM not supported", 0, 0, 0);
      #endif /* defined(MMOC_FTM_SUPPORTED) */
      break;

    case PROT_OPRT_MODE_NULL:                    
    case PROT_OPRT_MODE_PWROFF:
    case PROT_OPRT_MODE_ONLINE:
    case PROT_OPRT_MODE_LPM:
    case PROT_OPRT_MODE_RESET:
    case PROT_OPRT_MODE_MAX:
    default:
      MMOC_ERR_FATAL("Activate procotocol for incorrect offline mode",
                     0, 0, 0);
      break;

  } /* switch ( mmoc_info_ptr->curr_oprt_mode ) */

} /* mmoc_activate_prot_offline() */



/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_activate_prot_cdma_amps_ho

DESCRIPTION
  This is a utility function called by the transaction state handlers to
  activate the AMPS protocol in order to complete the CDMA to AMPS handoff.
  It would set the MMoC's protocol state after activating the protocol.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
/*lint -esym(715,mmoc_info_ptr)*/
static void                            mmoc_activate_prot_cdma_amps_ho
(

    mmoc_state_info_s_type      *mmoc_info_ptr
       /* Pointer to MMoC's state information.
       */
)
{
  /* Can't be const as for AMPS & CDMA we call mmoc_update_prot_state() 
  */ 
  SYS_ARG_NOT_CONST(mmoc_info_ptr); 

  MMOC_ASSERT( mmoc_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Activate the AMPS protocol stack to do CDMA -> AMPS handoff.
  */
  #if defined(MMOC_AMPS_SUPPORTED) && defined(MMOC_CDMA_SUPPORTED)
  mc_amps_activate_protocol( PROT_ACT_CDMA_AMPS_HANDOFF );
  mmoc_update_prot_state( PROT_STATE_ONLINE_AMPS,
                          SD_SS_MAIN,
                          mmoc_info_ptr
                        );
  mmoc_update_db_dmss_state( mmoc_info_ptr );
  #else
  MMOC_ERR_FATAL(" AMPS not supported", 0, 0, 0);
  #endif



} /* mmoc_activate_prot_cdma_amps_ho() */
/*lint +esym(715,mmoc_info_ptr)*/

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_deactivate_active_protocol

DESCRIPTION
  This is a utility function called by transaction state handlers to
  deactivate the active protocol if one exists.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - A protocol deactivate request was sent to the active protocol.
  FALSE - No request was sent.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean                         mmoc_deactivate_active_protocol
(
       sd_ss_e_type                    deact_req_index,
           /* SS instance to be deactivated.
           */

       prot_deact_e_type               deact_reason,
           /* Reason for deactivating the protocol.
           */

       mmoc_state_info_s_type          *mmoc_info_ptr
          /* Pointer to MMoC's state information.
          */
)
{
  boolean is_deact_req_sent = FALSE;

  MMOC_ASSERT( mmoc_info_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* check on the current MMoC's protocol state.
  */
  switch (mmoc_info_ptr->prot_state[deact_req_index])
  {
    #ifdef MMOC_CDMA_SUPPORTED
    case PROT_STATE_ONLINE_CDMA:
    case PROT_STATE_OFFLINE_CDMA:
      mc_cdma_deactivate_protocol( deact_reason,
                                   mmoc_next_trans_id(mmoc_info_ptr)
                                 );
      is_deact_req_sent = TRUE;
      break;
    #endif

    case PROT_STATE_ONLINE_DED_MEAS:
      #if defined(MMOC_WLAN_SUPPORTED) && defined(FEATURE_WLAN_MEASUREMENT)
#error code not present
      #endif
      #ifdef MMOC_CDMA_SUPPORTED
      if( deact_req_index == SD_SS_MAIN || 
          deact_req_index == SD_SS_HYBR_HDR )
      {
        mc_cdma_deactivate_protocol( deact_reason,
                                     mmoc_next_trans_id(mmoc_info_ptr)
                                   );
        is_deact_req_sent = TRUE;
      }
      #endif
      break;

    #ifdef MMOC_FTM_SUPPORTED
    case PROT_STATE_FTM:

    /* Preserve the deactivate order ( FTM first, MC next ), this
    ** FTM activity is completely deactivated, when we get the confirmation
    ** from MC. Note that ftm_deactivate_protocol() is executed in MMOC's
    ** context.
    */
    #if defined(MMOC_GW_SUPPORTED) || defined(MMOC_CDMA_SUPPORTED)
    ftm_deactivate_protocol( deact_reason,
                             mmoc_next_trans_id(mmoc_info_ptr)
                           );
    is_deact_req_sent = TRUE;
    #endif
    #ifdef MMOC_CDMA_SUPPORTED
    mc_cdma_deactivate_protocol( deact_reason,
                                 mmoc_next_trans_id(mmoc_info_ptr)
                               );
    is_deact_req_sent = TRUE;
    #endif
    /* Set rf to not initialized.
    */
    mmoc_info_ptr->is_rf_initialized = FALSE;
    break;
  #endif /* MMOC_FTM_SUPPORTED */

    #ifdef MMOC_AMPS_SUPPORTED
    case PROT_STATE_ONLINE_AMPS:
    case PROT_STATE_OFFLINE_AMPS:
      mc_amps_deactivate_protocol( deact_reason,
                                   mmoc_next_trans_id(mmoc_info_ptr)
                                 );
      is_deact_req_sent = TRUE;
      break;
    #endif

    #ifdef MMOC_GW_SUPPORTED
    case PROT_STATE_ONLINE_GW:
      cmregprx_cmd_gw_deactivate_protocol( deact_reason,
                                           mmoc_next_trans_id(mmoc_info_ptr)
                                         );
      is_deact_req_sent = TRUE;
      break;
    #endif


    #ifdef MMOC_HDR_SUPPORTED
#error code not present
    #endif


  #ifdef MMOC_WLAN_SUPPORTED
#error code not present
  #endif

    #ifdef FEATURE_GPSONE_OOS
#error code not present
    #endif /* FEATURE_GPSONE_OOS */


    case PROT_STATE_NULL:
    case PROT_STATE_OFFLINE:
    case PROT_STATE_PWR_SAVE:
    case PROT_STATE_RESET:
    case PROT_STATE_LPM:
    case PROT_STATE_PWR_DOWN:
    case PROT_STATE_MAX:
    default:
      MMOC_MSG_LOW(" No active protocol to deactivate",
                   0, 0, 0);
      break;
  } /* switch( mmoc_info_ptr->prot_state ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( !is_deact_req_sent )
  {

    mmoc_update_prot_state( PROT_STATE_NULL,
                            deact_req_index,
                            mmoc_info_ptr
                          );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return is_deact_req_sent;

} /* mmoc_deactivate_active_protocol() */

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_deactivate_all_active_protocol

DESCRIPTION
  This is a utility function called by transaction state handlers to
  deactivate all the active protocol.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - A protocol deactivate request was sent to the active protocol.
  FALSE - No request was sent.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean                         mmoc_deactivate_all_active_protocol
(
       prot_deact_e_type               deact_reason,
           /* Reason for deactivating the protocol.
           */

       mmoc_state_info_s_type          *mmoc_info_ptr
          /* Pointer to MMoC's state information.
          */
)
{
  boolean deact_req_sent = TRUE;

  /* First attempt deactivating SS_MAIN.
  */
  mmoc_info_ptr->deact_req_index = SD_SS_MAIN;
  #if defined(MMOC_HDR_SUPPORTED) && !defined(FEATURE_MMOC_UNIT_TEST)
#error code not present
  #endif
  deact_req_sent = mmoc_deactivate_active_protocol
                   (
                     mmoc_info_ptr->deact_req_index,
                     deact_reason,
                     mmoc_info_ptr
                   );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Next attempt deactivating SS_HYBR_HDR.
  */
  if ( !deact_req_sent )
  {
    mmoc_info_ptr->deact_req_index = SD_SS_HYBR_HDR;
    deact_req_sent = mmoc_deactivate_active_protocol
                   (
                     mmoc_info_ptr->deact_req_index,
                     deact_reason,
                     mmoc_info_ptr
                   );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Next attempt deactivating SS_HYBR_WLAN.
  */
  if ( !deact_req_sent )
  {
    mmoc_info_ptr->deact_req_index = SD_SS_HYBR_WLAN;
    deact_req_sent = mmoc_deactivate_active_protocol
                     (
                       mmoc_info_ptr->deact_req_index,
                       deact_reason,
                       mmoc_info_ptr
                     );
  }


  return deact_req_sent;

} /*  mmoc_deactivate_all_active_protocol */


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_is_wait_reqd_on_cmd_tmc

DESCRIPTION
  Sends given operating mode to TMC.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - MMOC needs to wait for TMC_MODE_CNF.
  FALSE - MMOC need not wait.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean                         mmoc_is_wait_reqd_on_cmd_tmc
(

       mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  
  prot_oprt_mode_e_type              curr_oprt_mode = PROT_OPRT_MODE_NULL;
     /* curr_oprt_mode
     */

  tmc_cmd_e_type                    cmd_to_tmc = TMC_CMD_TYPE_NONE;
    /* cmd being sent to TMC */

  #if defined (FEATURE_MMOC_TMC_FLOW_CNTRL)
  tmc_cmd_type                     *tmc_cmd_ptr = NULL; 
    /* TMC cmd ptr */  
  #endif

  /* Can't be const as FEATURE_MMOC_TMC_FLOW_CNTRL we call mmoc_update_prot_state() 
  */ 
  SYS_ARG_NOT_CONST(mmoc_info_ptr); 

  MMOC_ASSERT( mmoc_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  curr_oprt_mode = mmoc_info_ptr->curr_oprt_mode;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Send status change to TMC 
  */
  switch ( curr_oprt_mode)
  {

    /* TMC need not be informed when moving to FTM
    */
    case PROT_OPRT_MODE_FTM:
      cmd_to_tmc = TMC_CMD_TYPE_NONE;
      break;

    case PROT_OPRT_MODE_OFFLINE:
    case PROT_OPRT_MODE_OFFLINE_AMPS:
    case PROT_OPRT_MODE_OFFLINE_CDMA:
      cmd_to_tmc = TMC_CMD_TYPE_OFFLINE;
      break;      
 
    case PROT_OPRT_MODE_ONLINE:   
      cmd_to_tmc = TMC_CMD_TYPE_ONLINE;
      break;

    case PROT_OPRT_MODE_PWROFF:
      cmd_to_tmc = TMC_CMD_TYPE_POWERDOWN;  
      break;

    case PROT_OPRT_MODE_LPM:
      cmd_to_tmc = TMC_CMD_TYPE_LPM;        
      break;

    case PROT_OPRT_MODE_RESET:
      if(mmoc_info_ptr->true_oprt_mode == SYS_OPRT_MODE_RESET_MODEM)
      {
        #ifdef FEATURE_OPRT_MODE_MODEM_RESET
#error code not present
        #else
        MMOC_ERR_FATAL("Incorrect oprt_mode", 0, 0, 0);
        #endif
      }
      else
      {
        cmd_to_tmc = TMC_CMD_TYPE_RESET;
      }
              
      break;

    case PROT_OPRT_MODE_NULL:
    case PROT_OPRT_MODE_MAX:
    default:
      MMOC_ERR_FATAL("Incorrect oprt_mode", 0, 0, 0);
      break;

  } /* switch( curr_oprt_mode) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  

  if (cmd_to_tmc == TMC_CMD_TYPE_NONE)
  {       
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  

  #if defined (FEATURE_MMOC_TMC_FLOW_CNTRL)
  /* Send the cmd to tmc and wait for a cnf
  */
  tmc_cmd_ptr = tmc_get_cmd_buf ();

  if (!tmc_cmd_ptr)
  {
    MMOC_ERR_FATAL ("Can't get TMC buf", 0, 0, 0);
  }

  /* Populate cmd to tmc */
  tmc_cmd_ptr->cmd_id   = cmd_to_tmc;

  tmc_cmd_ptr->trans_id = mmoc_next_trans_id (mmoc_info_ptr);

  MMOC_MSG_HIGH (" Sending Ph status to TMC trans_id %d",
                   tmc_cmd_ptr->trans_id, 0, 0);

  mmoc_update_trans_state( MMOC_TRANS_STATE_WAIT_TMC_MODE_CNF,
                           mmoc_info_ptr
                         );

  tmc_put_cmd_buf (tmc_cmd_ptr);

  return TRUE;

  #else

  if (!tmc_cmd (cmd_to_tmc))
  {
    MMOC_MSG_ERROR (" Error calling tmc_cmd()", 0, 0, 0 );
  }

  /* For Lint 
  */
  SYS_ARG_NOT_USED (mmoc_info_ptr);

  return FALSE;

  #endif
    

} /* mmoc_is_wait_reqd_on_cmd_tmc */

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_send_ph_stat_chgd_cmd

DESCRIPTION
  This is a utility function called by the transaction state handlers to send
  the phone status changed command to the various supported modes. The command
  will be sent to one mode for every function call.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - Phone status command was sent to support mode.
  FALSE - Completed sending the phone status command to all supported modes.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean                         mmoc_send_ph_stat_chgd_cmd
(
    prot_ph_stat_chg_e_type            chg_type,
       /* Phone status changed type.
       */

    mmoc_state_info_s_type             *mmoc_info_ptr
       /* Pointer to MMoC's state information.
       */
)
{

  mmoc_cmd_subsc_chgd_s_type       *subsc_chgd_ptr =
                                      &mmoc_info_ptr->trans_info.subsc_chgd;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MMOC_ASSERT( mmoc_info_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if the ph. status changed commands have already been sent.
  */
  if (mmoc_info_ptr->is_ph_stat_sent )
  {
    /* Already sent, return FALSE,
    */
    return FALSE;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Decide to whom the next command should be sent.
  */
  mmoc_info_ptr->is_ph_stat_sent = TRUE;

  #ifdef MMOC_CDMA_SUPPORTED
  mc_cdma_ph_status_chgd_cmd( chg_type,
                              mmoc_info_ptr->curr_nam,
                              mmoc_info_ptr->is_cdma_subs_avail,
                              mmoc_info_ptr->curr_oprt_mode,
                              ph_stat_cmd_tbl[SD_MODE_CDMA].trans_id =
                                          mmoc_next_trans_id(mmoc_info_ptr),
                              subsc_chgd_ptr->prot_subsc_chg
                            );
  #endif

  #ifdef MMOC_AMPS_SUPPORTED
  mc_amps_ph_status_chgd_cmd( chg_type,
                              mmoc_info_ptr->curr_nam,
                              mmoc_info_ptr->is_cdma_subs_avail,
                              mmoc_info_ptr->curr_oprt_mode,
                              ph_stat_cmd_tbl[SD_MODE_AMPS].trans_id =
                                         mmoc_next_trans_id(mmoc_info_ptr),
                              subsc_chgd_ptr->prot_subsc_chg
                            );
  #endif

  #ifdef MMOC_HDR_SUPPORTED
#error code not present
  #endif

  #ifdef MMOC_GW_SUPPORTED
  cmregprx_cmd_gw_ph_stat_chgd( chg_type,
                                mmoc_info_ptr->is_gw_subs_avail,
                                mmoc_info_ptr->curr_oprt_mode,
                                ph_stat_cmd_tbl[SD_MODE_GW].trans_id =
                                       mmoc_next_trans_id(mmoc_info_ptr),
                                subsc_chgd_ptr->prot_subsc_chg
                              );
  #endif


  #ifdef MMOC_WLAN_SUPPORTED
#error code not present
  #endif


  mmoc_info_ptr->is_ph_stat_sent = TRUE;
  return mmoc_info_ptr->is_ph_stat_sent;

} /* mmoc_send_ph_stat_chgd_cmd() */


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_send_subsc_chg_ph_stat_chgd_cmd

DESCRIPTION
  This is a utility function called by the SUBSCRIPTION CHANGE transaction 
  handler to send   the phone status changed command to the various supported
   modes. The command will be sent to one mode for every function call.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - Phone status command was sent to support mode.
  FALSE - Completed sending the phone status command to all supported modes.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean                         mmoc_send_subsc_chg_ph_stat_chgd_cmd
(
    prot_ph_stat_chg_e_type            chg_type,
       /* Phone status changed type.
       */

    mmoc_state_info_s_type             *mmoc_info_ptr
       /* Pointer to MMoC's state information.
       */
)
{

  mmoc_cmd_subsc_chgd_s_type       *subsc_chgd_ptr =
                                      &mmoc_info_ptr->trans_info.subsc_chgd;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MMOC_ASSERT( mmoc_info_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if the ph. status changed commands have already been sent.
  */
  if (mmoc_info_ptr->is_ph_stat_sent )
  {
    /* Already sent, return FALSE,
    */
    return FALSE;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Decide to whom the next command should be sent.
  */
  mmoc_info_ptr->is_ph_stat_sent = TRUE;

  /* send the phone status changed command to WLAN only if WLAN
  ** subscription has changed
  */
  if(CHECK_PROTO(mmoc_info_ptr->prot_subsc_chg ,PROT_SUBSC_CHG_WLAN))
   
  {
    #ifdef MMOC_WLAN_SUPPORTED
#error code not present
    #endif
  }
  
  /* If only WLAN subscription has changed,
  ** no need to send phone status
  ** changed command to other protocols. 
  */
  if(mmoc_info_ptr->prot_subsc_chg == PROT_SUBSC_CHG_WLAN) 
  {
    return mmoc_info_ptr->is_ph_stat_sent;
  }

  #ifdef MMOC_CDMA_SUPPORTED
  mc_cdma_ph_status_chgd_cmd( chg_type,
                              mmoc_info_ptr->curr_nam,
                              mmoc_info_ptr->is_cdma_subs_avail,
                              mmoc_info_ptr->curr_oprt_mode,
                              ph_stat_cmd_tbl[SD_MODE_CDMA].trans_id =
                                          mmoc_next_trans_id(mmoc_info_ptr),
                              subsc_chgd_ptr->prot_subsc_chg
                            );
  #endif

  #ifdef MMOC_AMPS_SUPPORTED
  mc_amps_ph_status_chgd_cmd( chg_type,
                              mmoc_info_ptr->curr_nam,
                              mmoc_info_ptr->is_cdma_subs_avail,
                              mmoc_info_ptr->curr_oprt_mode,
                              ph_stat_cmd_tbl[SD_MODE_AMPS].trans_id =
                                         mmoc_next_trans_id(mmoc_info_ptr),
                              subsc_chgd_ptr->prot_subsc_chg
                            );
  #endif

  #ifdef MMOC_HDR_SUPPORTED
#error code not present
  #endif

  #ifdef MMOC_GW_SUPPORTED
  cmregprx_cmd_gw_ph_stat_chgd( chg_type,
                                mmoc_info_ptr->is_gw_subs_avail,
                                mmoc_info_ptr->curr_oprt_mode,
                                ph_stat_cmd_tbl[SD_MODE_GW].trans_id =
                                       mmoc_next_trans_id(mmoc_info_ptr),
                                subsc_chgd_ptr->prot_subsc_chg
                              );
  #endif


  mmoc_info_ptr->is_ph_stat_sent = TRUE;
  return mmoc_info_ptr->is_ph_stat_sent;

} /* mmoc_send_subsc_chg_ph_stat_chgd_cmd() */


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_send_prot_gen_cmd

DESCRIPTION
  This is a utility function called by the transaction state handlers to send
  the generic command to the active protocol.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - Command was sent to the active protocol.
  FALSE - Command was not sent, because no protocol was active.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean                         mmoc_send_prot_gen_cmd
(
       prot_gen_cmd_s_type             *gen_cmd_ptr,
          /* Pointer to the generic protocol command.
          */

       mmoc_state_info_s_type          *mmoc_info_ptr
          /* Pointer to MMoC's state information.
          */
)
{
  boolean is_cmd_sent = TRUE;

  MMOC_ASSERT( mmoc_info_ptr != NULL );
  MMOC_ASSERT( gen_cmd_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /*  Check for which SS instance.
  */
  switch ( gen_cmd_ptr->ss )
  {
    case SD_SS_MAIN:
    {
      /* Check on the active protocol.
      */
      switch (mmoc_info_ptr->prot_state[SD_SS_MAIN])
      {
        #ifdef MMOC_CDMA_SUPPORTED
        case PROT_STATE_ONLINE_CDMA:
        case PROT_STATE_ONLINE_DED_MEAS:
          gen_cmd_ptr->trans_id = mmoc_next_trans_id(mmoc_info_ptr);
          mc_cdma_generic_prot_cmd( gen_cmd_ptr );
          break;
        #endif

        #ifdef MMOC_AMPS_SUPPORTED
        case PROT_STATE_ONLINE_AMPS:
          gen_cmd_ptr->trans_id = mmoc_next_trans_id(mmoc_info_ptr);
          mc_amps_generic_prot_cmd( gen_cmd_ptr );
          break;
        #endif

        #ifdef MMOC_GW_SUPPORTED
        case PROT_STATE_ONLINE_GW:
          gen_cmd_ptr->trans_id = mmoc_next_trans_id(mmoc_info_ptr);
          cmregprx_cmd_gw_generic_cmd( gen_cmd_ptr );
          break;
        #endif


        #ifdef MMOC_HDR_SUPPORTED
#error code not present
        #endif

          #ifdef MMOC_WLAN_SUPPORTED
#error code not present
          #endif


        case PROT_STATE_FTM:
        case PROT_STATE_LPM:
        case PROT_STATE_OFFLINE:
        case PROT_STATE_OFFLINE_AMPS:
        case PROT_STATE_OFFLINE_CDMA:
        case PROT_STATE_PWR_DOWN:
        case PROT_STATE_RESET:
          MMOC_ERR_FATAL( " Send Generic command received in incorrect state %d",
                          mmoc_info_ptr->prot_state[SD_SS_MAIN], 0, 0);
          break;

        case PROT_STATE_GPSONE_MSBASED:
        case PROT_STATE_NULL:
        case PROT_STATE_PWR_SAVE:
        case PROT_STATE_MAX:
        default:
          is_cmd_sent = FALSE;
          MMOC_MSG_LOW(" No active protocol to send the generic command",
                       0, 0, 0);
          break;
      } /* switch( mmoc_info_ptr->prot_state ) */

    }  /* case SD_SS_MAIN: */
    break;

    case SD_SS_HYBR_HDR:
    {
      switch (mmoc_info_ptr->prot_state[SD_SS_HYBR_HDR])
      {
        #ifdef MMOC_HDR_SUPPORTED
#error code not present
        #endif

        #ifdef MMOC_HYBR_GW_SUPPORTED
        case PROT_STATE_ONLINE_GW:
          gen_cmd_ptr->trans_id = mmoc_next_trans_id(mmoc_info_ptr);
          cmregprx_cmd_gw_generic_cmd( gen_cmd_ptr );
          break;
        #endif

        case PROT_STATE_NULL:
        case PROT_STATE_ONLINE_CDMA:
        case PROT_STATE_OFFLINE_CDMA:
        case PROT_STATE_ONLINE_AMPS:
        case PROT_STATE_OFFLINE_AMPS:
        case PROT_STATE_ONLINE_WLAN:
        case PROT_STATE_OFFLINE:
        case PROT_STATE_FTM:
        case PROT_STATE_PWR_SAVE:
        case PROT_STATE_ONLINE_DED_MEAS:
        case PROT_STATE_RESET:
        case PROT_STATE_LPM:
        case PROT_STATE_PWR_DOWN:
        case PROT_STATE_GPSONE_MSBASED:
        case PROT_STATE_MAX:
        default:
          is_cmd_sent = FALSE;
          MMOC_MSG_LOW(" No active protocol to send the generic command",
                         0, 0, 0);
          break;
      } /* switch (mmoc_info_ptr->prot_state[SD_SS_HYBR_HDR]) */
 
    } /* case SD_SS_HYBR_HDR: */
    break;

    case SD_SS_HYBR_WLAN:
    {
      switch (mmoc_info_ptr->prot_state[SD_SS_HYBR_WLAN])
      {
        #ifdef MMOC_WLAN_SUPPORTED
#error code not present
        #endif

        case PROT_STATE_NULL:
        case PROT_STATE_ONLINE_CDMA:
        case PROT_STATE_OFFLINE_CDMA:
        case PROT_STATE_ONLINE_AMPS:
        case PROT_STATE_OFFLINE_AMPS:
        case PROT_STATE_ONLINE_GW:
        case PROT_STATE_ONLINE_HDR:
        case PROT_STATE_OFFLINE:
        case PROT_STATE_FTM:
        case PROT_STATE_PWR_SAVE:
        case PROT_STATE_RESET:
        case PROT_STATE_LPM:
        case PROT_STATE_PWR_DOWN:
        case PROT_STATE_GPSONE_MSBASED:
        case PROT_STATE_MAX:
        default:
          is_cmd_sent = FALSE;
          MMOC_MSG_LOW(" No active protocol to send the generic command",
                         0, 0, 0);
          break;
      } /* switch (mmoc_info_ptr->prot_state[SD_SS_HYBR_WLAN]) */
 
    } /* case SD_SS_HYBR_HDR: */
    break;


    default:
    {
      MMOC_ERR_FATAL("incorrect ss instance", 0, 0, 0 );
    }
    break;

  } /* switch ( gen_cmd_ptr->ss ) */


  return is_cmd_sent;

} /* mmoc_send_prot_generic_cmd() */



/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
==================== MMoC TASK RELATED HELPER FUNCTIONS =====================
=============================================================================
=============================================================================
===========================================================================*/

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_get_cmd_buf_else_err_fatal

DESCRIPTION
  Allocate a MMoC task command buffer. Error fatal if allocation is NOT
  successful.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to allocated MMoC command buffer.

SIDE EFFECTS
  none
===========================================================================*/
static mmoc_cmd_msg_s_type*            mmoc_get_cmd_buf_else_err_fatal
(

       void

)
{
  #ifdef FEATURE_MMOC_TMC_HEAP
  mmoc_cmd_msg_s_type *mmoc_cmd_ptr;
  #else
  /* Pointer to MMoC's task info.
  */
  mmoc_task_info_s_type*  mmoc_task_ptr = mmoc_get_task_info_ptr();

  /* Try allocating a MMoC Task command buffer.
  */
  mmoc_cmd_msg_s_type *mmoc_cmd_ptr =
               (mmoc_cmd_msg_s_type*) q_get( &mmoc_task_ptr->cmd_free_q );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* If allocation failed, error fatal.
  */
  if( mmoc_cmd_ptr == NULL )
  {
    MMOC_ERR_FATAL( "Can't get MMoC cmd buf",0,0,0 );
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #endif /* FEATURE_MMOC_TMC_HEAP */

  #ifdef FEATURE_MMOC_TMC_HEAP
     mmoc_cmd_ptr =
               (mmoc_cmd_msg_s_type*)mem_malloc(&mmoc_heap,
                                                sizeof(mmoc_cmd_msg_s_type));

  /* If allocation failed, try TMC heap
  */
  if( mmoc_cmd_ptr == NULL )
  {
    MMOC_MSG_HIGH("MMOC Heap full, requesting from TMC Heap",0,0,0);

    mmoc_cmd_ptr = (mmoc_cmd_msg_s_type*)mem_malloc(&tmc_heap,
                                     sizeof(mmoc_cmd_msg_s_type));
    if(mmoc_cmd_ptr != NULL)
    {
      mmoc_tmc_heap_blocks++;
    }
    else
    {
      /* Allocation failed, error fatal.
      */
      MMOC_ERR_FATAL( "Can't get MMoC cmd buf from TMC heap",0,0,0 );
    }
  }
  else
  {
    mmoc_heap_slots_left--;
  }

  MMOC_MSG_HIGH("MMOC malloc: MMOC Left=%d, TMC Used=%d", mmoc_heap_slots_left, mmoc_tmc_heap_blocks, 0);

  #endif /* FEATURE_MMOC_TMC_HEAP */

  /* If we got here, allocation is successful,
  ** so indicate queue to place command buffer on when done,
  ** and specify NO task to be signalled when done
  */
  #ifdef FEATURE_MMOC_TMC_HEAP
  mmoc_cmd_ptr->cmd.hdr.done_q_ptr      = NULL;
  #else
  mmoc_cmd_ptr->cmd.hdr.done_q_ptr      = &mmoc_task_ptr->cmd_free_q;
  #endif /* FEATURE_MMOC_TMC_HEAP */

  mmoc_cmd_ptr->cmd.hdr.task_ptr        = NULL;


  return mmoc_cmd_ptr;

} /* mmoc_get_cmd_buf_else_err_fatal() */

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_send_cmd

DESCRIPTION
  This function takes an already filled out mmoc_cmd_msg_type and places it
  on the mmoc command queue.

DEPENDENCIES
  The MMoC task must have been started up already.  All of the items in
  the mmoc_cmd_msg_s_type must be already set.

RETURN VALUE
  None.

SIDE EFFECTS
  The mmoc_cmd_msg_s_type is placed on the MMoC command queue.  It must not be
  modified until the command has been processed.
===========================================================================*/
static void                            mmoc_send_cmd
(

       mmoc_cmd_msg_s_type             *msg_ptr
          /* Pointer to MMoC command message buffer.
          */

)
{
  /* Pointer to MMoC's task info.
  */
  mmoc_task_info_s_type*  mmoc_task_ptr = mmoc_get_task_info_ptr();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Initialize and link the command onto the MC command queue.
  */
  (void) q_link(msg_ptr, &msg_ptr->cmd.hdr.link);
  q_put(&mmoc_task_ptr->cmd_q, &msg_ptr->cmd.hdr.link);


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Set the REX signal corresponding to the MMoC's command queue and
  ** return.
  */
  (void) rex_set_sigs(&mmoc_tcb, MMOC_CMD_Q_SIG);


} /* mmoc_send_cmd() */


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_get_rpt_buf_else_err_fatal

DESCRIPTION
  Allocate a MMoC task report buffer. Error fatal if allocation is NOT
  successful.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to allocated MMoC report buffer.

SIDE EFFECTS
  none
===========================================================================*/
static mmoc_rpt_msg_s_type*            mmoc_get_rpt_buf_else_err_fatal
(

       void

)
{
  #ifdef FEATURE_MMOC_TMC_HEAP    
  mmoc_rpt_msg_s_type *mmoc_rpt_ptr;
  #else

  /* Pointer to MMoC's task info.
  */
  mmoc_task_info_s_type*  mmoc_task_ptr = mmoc_get_task_info_ptr();

  /* Try allocating a MMoC Task report buffer.
  */
  mmoc_rpt_msg_s_type *mmoc_rpt_ptr =
               (mmoc_rpt_msg_s_type*) q_get( &mmoc_task_ptr->rpt_free_q );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* If allocation failed, error fatal.
  */
  if( mmoc_rpt_ptr == NULL )
  {
    MMOC_ERR_FATAL( "Can't get MMoC rpt buf",0,0,0 );
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #endif /* FEATURE_MMOC_TMC_HEAP */

  #ifdef FEATURE_MMOC_TMC_HEAP   

    mmoc_rpt_ptr =
               (mmoc_rpt_msg_s_type*) mem_malloc(&mmoc_heap,
                                                sizeof(mmoc_rpt_msg_s_type));

  /* If allocation failed, try TMC heap
  */
  if( mmoc_rpt_ptr == NULL )
  {
    MMOC_MSG_HIGH("MMOC Heap full, requesting from TMC Heap",0,0,0);
    mmoc_rpt_ptr = (mmoc_rpt_msg_s_type*)mem_malloc(&tmc_heap,
                                     sizeof(mmoc_rpt_msg_s_type));
    if(mmoc_rpt_ptr != NULL)
    {
      mmoc_tmc_heap_blocks++;
    }
    else
    {
      /* Allocation failed, error fatal.
      */
      MMOC_ERR_FATAL( "Can't get MMoC rpt buf",0,0,0 );
    }
  }
  else
  {
    mmoc_heap_slots_left--;
  }

  MMOC_MSG_HIGH("MMOC malloc: MMOC Left=%d, TMC Used=%d", mmoc_heap_slots_left, mmoc_tmc_heap_blocks, 0);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #endif /* FEATURE_MMOC_TMC_HEAP */

  /* If we got here, allocation is successful, so indicate queue to place
  ** command buffer on when done.
  */
  #ifdef FEATURE_MMOC_TMC_HEAP
  mmoc_rpt_ptr->rpt.hdr.done_q_ptr      = NULL;
  #else
  mmoc_rpt_ptr->rpt.hdr.done_q_ptr      = &mmoc_task_ptr->rpt_free_q;
  #endif /* FEATURE_MMOC_TMC_HEAP */
  mmoc_rpt_ptr->rpt.hdr.task_ptr        = NULL;


  return mmoc_rpt_ptr;

} /* mmoc_get_rpt_buf_else_err_fatal() */



/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_send_rpt

DESCRIPTION
  This function takes an already filled out mmoc_rpt_msg_type and places it
  on the mmoc report queue.

DEPENDENCIES
  The MMoC task must have been started up already.  All of the items in
  the mmoc_rpt_msg_s_type must be already set.

RETURN VALUE
  None.

SIDE EFFECTS
  The mmoc_rpt_msg_s_type is placed on the MMoC report queue.  It must not be
  modified until the command has been processed.
===========================================================================*/
static void                            mmoc_send_rpt
(

       mmoc_rpt_msg_s_type             *msg_ptr
          /* Pointer to MMoC report message buffer.
          */

)
{
  /* Pointer to MMoC's task info.
  */
  mmoc_task_info_s_type*  mmoc_task_ptr = mmoc_get_task_info_ptr();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Initialize and link the command onto the MC report queue.
  */
  (void) q_link(msg_ptr, &msg_ptr->rpt.hdr.link);
  q_put(&mmoc_task_ptr->rpt_q, &msg_ptr->rpt.hdr.link);


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Set the REX signal corresponding to MMoC's report queue and return.
  */
  (void) rex_set_sigs(&mmoc_tcb, MMOC_RPT_Q_SIG);


} /* mmoc_send_rpt() */


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_task_init

DESCRIPTION
  This function is responsible for initializing the MMoC task before task
  start. This includes
  1. Initializing command and report queues
  2. Performing system determination initializing before task start.

DEPENDENCIES
  This should be called only before task start.

RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/
MMEXTN  void                           mmoc_task_init
(

        mmoc_state_info_s_type         *mmoc_state_ptr
           /* Pointer to MMoC's state information.
           */

)
{
  #ifndef FEATURE_MMOC_TMC_HEAP
  /* loop counter.
  */
  unsigned int i;
  #endif /* FEATURE_MMOC_TMC_HEAP */

  /* Pointer to MMoC's task information.
  */
  mmoc_task_info_s_type          *mmoc_task_ptr = mmoc_get_task_info_ptr();


  #ifdef FEATURE_MMOC_TMC_HEAP
  mmoc_large_obj_unused *unused_ptr = NULL;

  /* To clear the compiler warning caused by
  ** mmoc_large_obj_unused
  */
  SYS_ARG_NOT_USED(unused_ptr);
  #endif /* FEATURE_MMOC_TMC_HEAP */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Initialize timers for watchdog report and sanity.
  */
  rex_def_timer( &mmoc_task_ptr->rpt_timer,
                 MMOC_TASK_PTR,
                 MMOC_RPT_TIMER_SIG
               );

  rex_def_timer( &mmoc_task_ptr->rex_wait_timer,
                 MMOC_TASK_PTR,
                 MMOC_REX_WAIT_SIG
               );

  rex_def_timer( &mmoc_state_ptr->sanity_timer,
                 MMOC_TASK_PTR,
                 MMOC_SANITY_TIMER_SIG
               );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Initialize report queues. */
  ( void ) q_init( &mmoc_task_ptr->rpt_q );
  #ifndef FEATURE_MMOC_TMC_HEAP
  ( void ) q_init( &mmoc_task_ptr->rpt_free_q );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Fill mmoc.rpt_free_q. i
  */
  for ( i = 0; i < ARR_SIZE(mmoc_task_ptr->rpt_pool); i++ )
  {
    mmoc_task_ptr->rpt_pool[i].rpt.hdr.done_q_ptr =
                                                &mmoc_task_ptr->rpt_free_q;
    q_put( &mmoc_task_ptr->rpt_free_q,
           q_link( &mmoc_task_ptr->rpt_pool[i],
                   &mmoc_task_ptr->rpt_pool[i].rpt.hdr.link
                 )
         );
  }

  #endif /* FEATURE_MMOC_TMC_HEAP  */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Initialize command queue.
  */
  (void) q_init( &mmoc_task_ptr->cmd_q );
  #ifndef FEATURE_MMOC_TMC_HEAP  
  (void) q_init( &mmoc_task_ptr->cmd_free_q );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Fill mmoc.cmd_free_q.
  */
  for (i = 0; i < ARR_SIZE(mmoc_task_ptr->cmd_pool); i++) {
    mmoc_task_ptr->cmd_pool[i].cmd.hdr.done_q_ptr =
                                                  &mmoc_task_ptr->cmd_free_q;
        q_put( &mmoc_task_ptr->cmd_free_q,
               q_link( &mmoc_task_ptr->cmd_pool[i],
                       &mmoc_task_ptr->cmd_pool[i].cmd.hdr.link
                     )
             );
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #endif /* FEATURE_MMOC_TMC_HEAP  */

  #ifdef FEATURE_MMOC_TMC_HEAP
  /*
  ** Initialize MMOC Heap.
  */

  mem_init_heap(&mmoc_heap,
                mmoc_heap_mem_buffer,
                MMOC_HEAP_BUFFER_SIZE,
                NULL);
  mmoc_heap_slots_left = MMOC_HEAP_SLOT_COUNT;
  mmoc_tmc_heap_blocks = 0;
  #endif /* FEATURE_MMOC_TMC_HEAP */

  /* Initialize err_fatal debug info.
  */
  #ifdef FEATURE_MMOC_UNIT_TEST
  mmoc_task_ptr->is_stop_err_fatal = TRUE;
  mmoc_task_ptr->err_fatal_msg[0] = '\0';
  #endif


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Initialization for mmoc_state_info.
  */
  mmoc_init_state_info( mmoc_state_ptr );


} /* mmoc_task_init() */


/* <EJECT> */
/*===========================================================================
FUNCTION mmoc_kick_dog

DESCRIPTION
  Kick the watchdog and set the watchdog timer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
 None.
===========================================================================*/
static void                            mmoc_kick_dog
(

       void

)
{

  /* Pointer to MMoC's task information.
  */
  mmoc_task_info_s_type          *mmoc_task_ptr = mmoc_get_task_info_ptr();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Report watchdog that MMoC is sane.
  */
  dog_report( DOG_MMOC_RPT );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Restart the MMoC's watchdog report timer.
  */
  (void) rex_set_timer( &mmoc_task_ptr->rpt_timer, DOG_MMOC_RPT_TIME );


} /* mmoc_kick_dog() */

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_ss_main_timer_done_cb

DESCRIPTION
  This function is the callback procedure used by system determination when
  system selection SS_MAIN timer expires.
  This function will queue the reselection timer expired generic command to
  the MMoC command queue.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
MMEXTN void                            mmoc_ss_main_timer_done_cb
(

       int4                            ms_interval
          /*lint -esym(715,ms_interval)
          ** Have lint not complain about the ignored parameter 'ms_interval'
          ** which is specified to make this routine match the template for
          ** clk_Reg().
          */

)
{

  mmoc_cmd_msg_s_type                 *msg_ptr;

  MMOC_MSG_MED("System selection timer expired", 0, 0, 0);


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Get the command buffer.
  */
  msg_ptr = mmoc_get_cmd_buf_else_err_fatal();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Fill in all the fields.
  */
  msg_ptr->cmd.name                = MMOC_CMD_PROT_GEN_CMD;
  msg_ptr->param.gen_prot_cmd.cmd_type = PROT_GEN_CMD_SS_TIMER_EXP;
  msg_ptr->param.gen_prot_cmd.ss   = SD_SS_MAIN;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Post the command.
  */
  mmoc_send_cmd( msg_ptr );


} /* mmoc_ss_main_timer_done_cb() */
/*lint +esym(715,ms_interval)*/

/*===========================================================================

FUNCTION mmoc_ss_main_timer_cb

DESCRIPTION
  This callback function enables or disables system selection SS_MAIN timer.
  It is used by system determination.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void                            mmoc_ss_main_timer_cb
(

       int4                            ss_timer_duration
          /* Duration in ms for the timer to be active.
          */

)
{

  /* Pointer to MMoC's task information.
  */
  mmoc_task_info_s_type          *mmoc_task_ptr = mmoc_get_task_info_ptr();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Determine whether to cancel the timer or start it.
  */
  if (ss_timer_duration == 0)
  {
    /* Disable System Selection timer.
    */
    #ifdef FEATURE_USE_TIME_VU
    (void) timer_clr(&mmoc_task_ptr->ss_main_timer, T_NONE);
    #else
    clk_dereg( &mmoc_task_ptr->ss_main_timer );
    #endif /* FEATURE_USE_TIME_VU */
  }
  else
  {
    /* Start System Selection timer.
    */
    #ifdef FEATURE_USE_TIME_VU
    timer_reg(&mmoc_task_ptr->ss_main_timer,
              (timer_t2_cb_type)mmoc_ss_main_timer_done_cb,
              (timer_cb_data_type) NULL,
              (timetick_type)ss_timer_duration,
              0
             );
    #else
    clk_reg( &mmoc_task_ptr->ss_main_timer,
             mmoc_ss_main_timer_done_cb,
             ss_timer_duration,
             0,
             FALSE
           );
    #endif /* FEATURE_USE_TIME_VU */
  }


} /* mmoc_ss_main_timer_cb() */

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_ss_hdr_timer_done_cb

DESCRIPTION
  This function is the callback procedure used by system determination when
  system selection timer for SS_HDR expires.
  This function will queue the reselection timer expired generic command to
  the MMoC command queue.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
MMEXTN void                            mmoc_ss_hybr_1_timer_done_cb
(

       int4                            ms_interval
          /*lint -esym(715,ms_interval)
          ** Have lint not complain about the ignored parameter 'ms_interval'
          ** which is specified to make this routine match the template for
          ** clk_Reg().
          */

)
{
  #if defined(MMOC_HYBR_1_SUPPORTED) || defined(MMOC_HYBR_SUPPORTED)
  mmoc_cmd_msg_s_type                 *msg_ptr;

  MMOC_MSG_MED("System selection timer expired", 0, 0, 0);


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Get the command buffer.
  */
  msg_ptr = mmoc_get_cmd_buf_else_err_fatal();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Fill in all the fields.
  */
  msg_ptr->cmd.name                = MMOC_CMD_PROT_GEN_CMD;
  msg_ptr->param.gen_prot_cmd.cmd_type = PROT_GEN_CMD_SS_TIMER_EXP;
  msg_ptr->param.gen_prot_cmd.ss   = SD_SS_HYBR_1;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Post the command.
  */
  mmoc_send_cmd( msg_ptr );
  #else
  SYS_ARG_NOT_USED(ms_interval);
  #endif /* (MMOC_HYBR_1_SUPPORTED) || (MMOC_HYBR_SUPPORTED) */

} /* mmoc_ss_hdr_timer_done_cb() */
/*lint +esym(715,ms_interval)*/

/*===========================================================================

FUNCTION mmoc_ss_hdr_timer_cb

DESCRIPTION
  This callback function enables or disables system selection timer for
  SS_HDR. It is used by system determination.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void                            mmoc_ss_hdr_timer_cb
(

       int4                            ss_timer_duration
          /* Duration in ms for the timer to be active.
          */

)
{
  #if defined(MMOC_HYBR_1_SUPPORTED) || defined(MMOC_HYBR_SUPPORTED)
  /* Pointer to MMoC's task information.
  */
  mmoc_task_info_s_type          *mmoc_task_ptr = mmoc_get_task_info_ptr();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Determine whether to cancel the timer or start it.
  */
  if (ss_timer_duration == 0)
  {

    /* Disable System Selection timer.
    */
    #ifdef FEATURE_USE_TIME_VU
    (void) timer_clr(&mmoc_task_ptr->ss_hdr_timer, T_NONE);
    #else
    clk_dereg( &mmoc_task_ptr->ss_hdr_timer );
    #endif /* FEATURE_USE_TIME_VU */
  }
  else
  {
    /* Start System Selection timer.
    */
    #ifdef FEATURE_USE_TIME_VU
    timer_reg( &mmoc_task_ptr->ss_hdr_timer,
               (timer_t2_cb_type)mmoc_ss_hybr_1_timer_done_cb,
               (timer_cb_data_type) NULL,
               (timetick_type)ss_timer_duration,
               0
             );
    #else
    clk_reg( &mmoc_task_ptr->ss_hdr_timer,
             mmoc_ss_hybr_1_timer_done_cb,
             ss_timer_duration,
             0,
             FALSE
           );
    #endif /* FEATURE_USE_TIME_VU */
  }
  #else
  SYS_ARG_NOT_USED(ss_timer_duration);
  #endif /* (MMOC_HYBR_1_SUPPORTED) || (MMOC_HYBR_SUPPORTED) */

} /* mmoc_ss_hdr_timer_cb() */


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_ss_hybr_wlan_timer_done_cb

DESCRIPTION
  This function is the callback procedure used by system determination when
  system selection timer for SS_HYBR_WLAN expires.
  This function will queue the reselection timer expired generic command to
  the MMoC command queue.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
MMEXTN void                            mmoc_ss_hybr_wlan_timer_done_cb
(

       int4                            ms_interval
          /*lint -esym(715,ms_interval)
          ** Have lint not complain about the ignored parameter 'ms_interval'
          ** which is specified to make this routine match the template for
          ** clk_Reg().
          */

)
{
  #ifdef MMOC_WLAN_SUPPORTED
#error code not present
  #else
  SYS_ARG_NOT_USED(ms_interval);
  #endif /* MMOC_WLAN_SUPPORTED */

} /* mmoc_ss_hybr_wlan_timer_done_cb() */
/*lint +esym(715,ms_interval)*/

/*===========================================================================

FUNCTION mmoc_ss_hybr_wlan_timer_cb

DESCRIPTION
  This callback function enables or disables system selection timer for
  SS_HYBR_WLAN. It is used by system determination.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void                            mmoc_ss_hybr_wlan_timer_cb
(

       int4                            ss_timer_duration
          /* Duration in ms for the timer to be active.
          */

)
{
  #ifdef MMOC_WLAN_SUPPORTED
#error code not present
  #else
  SYS_ARG_NOT_USED(ss_timer_duration);
  #endif /* MMOC_WLAN_SUPPORTED */
} /* mmoc_ss_hybr_wlan_timer_cb() */





/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_wait

DESCRIPTION
  This procedure waits for specified signals to be set.  While it waits,
  this procedure continues to set the Watchdog report signal and reset
  the mmoc_task_ptr->rpt_timer.

DEPENDENCIES
  mmoc_task_info->rpt_timer must have been defined with rex_def_timer().

RETURN VALUE
  Signal mask from signals that were set after rex_wait call.

SIDE EFFECTS
  None
===========================================================================*/
static rex_sigs_type                   mmoc_wait
(

       rex_sigs_type                   mask
          /* Mask of signals to wait on.
          */

)
{

  /* Signals returned by rex_wait.
  */
  rex_sigs_type    sigs;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Loop while waiting for one of the specified signals
  ** to be set.
  */
  for( ;; )
  {
    /* Wait on specified signals, as well as,
    ** on the watchdog signal.
    */
    sigs = rex_wait( mask | MMOC_RPT_TIMER_SIG );


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    /* If watchdog signal is set, report back to the watchdog.
    */
    if( sigs & MMOC_RPT_TIMER_SIG )
    {
      mmoc_kick_dog();
    }


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    /* If one or more of the specified signals is set
    ** return now.
    */
    if( sigs & mask )
    {
      return sigs;
    }


  } /* for( ;;) */


} /* mmoc_wait() */


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_subsc_chgd_deact_enter


DESCRIPTION
  This is a function for handling the transaction state DEACT_ENTER for the 
  subscription changed transaction . If a protocol needs to be deactivated, 
  it sets the next trans_state to DEACTD_CNF else sets the next trans_state 
  PH_STAT_ENTER. Unlike mmoc_process_common_deact_enter it need not deactivate
  all active protocols

DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Should wait for deactivated confirmation.
  MMOC_EVT_STATUS_NOT_CONSUMED - Start ph_stat_enter processing.


SIDE EFFECTS
  None
===========================================================================*/
static  mmoc_evt_status_e_type         mmoc_process_subsc_chgd_deact_enter
(
        prot_deact_e_type              deact_reason,
           /* Reason for deactivating the active protocol.
           */

        mmoc_state_info_s_type         *mmoc_info_ptr
           /* Pointer to MMoC's state information.
           */

)
{
  boolean is_deact_req_sent = FALSE;

  /* If the subscription change command is only for WLAN 
  ** deactivate only WLAN 
  */
  if(mmoc_info_ptr->prot_subsc_chg == PROT_SUBSC_CHG_WLAN)
  {
    mmoc_info_ptr->deact_req_index = SD_SS_HYBR_WLAN;
    is_deact_req_sent = mmoc_deactivate_active_protocol (
                        mmoc_info_ptr->deact_req_index,
                        deact_reason,
                        mmoc_info_ptr 
                        );
   }

  /* If subsc change is not for WLAN 
  ** deactivate only 1x and GW and NOT WLAN (
  ** ie. deactivate main stack and hybr_1 stack
  */
  else if (!CHECK_PROTO(mmoc_info_ptr->prot_subsc_chg ,PROT_SUBSC_CHG_WLAN))
  { 
    /* First deactivate SS_MAIN.
    */
    mmoc_info_ptr->deact_req_index = SD_SS_MAIN;
    is_deact_req_sent = mmoc_deactivate_active_protocol(
                        mmoc_info_ptr->deact_req_index,
                        deact_reason,
                        mmoc_info_ptr
                        );

    /* Next attempt deactivating SS_HYBR_HDR.
    */
    if ( !is_deact_req_sent )
    {
      mmoc_info_ptr->deact_req_index = SD_SS_HYBR_HDR;
      is_deact_req_sent = mmoc_deactivate_active_protocol(
                          mmoc_info_ptr->deact_req_index,
                          deact_reason,
                          mmoc_info_ptr
                          );
    }
  }
  else  
  {
    /* Send the deactivate request to all active protocols.
    */
    is_deact_req_sent = mmoc_deactivate_all_active_protocol( deact_reason,
                                                             mmoc_info_ptr
                                                             );
    
    
  }//mmoc_info_ptr->prot_subsc_chg == PROT_SUBSC_CHG_WLAN

  if (is_deact_req_sent)
  {
     mmoc_update_trans_state( MMOC_TRANS_STATE_WAIT_DEACTD_CNF,
                              mmoc_info_ptr
                            );

     return MMOC_EVT_STATUS_CONSUMED;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* No active protocols present, start sending the phone status
  ** changed command.
  */
  mmoc_update_trans_state( MMOC_TRANS_STATE_PROT_PH_STAT_ENTER,
                           mmoc_info_ptr
                         );


  return MMOC_EVT_STATUS_NOT_CONSUMED;


} /* mmoc_process_subsc_chgd_deact_enter() */


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_common_deact_enter

DESCRIPTION
  This is a common function for handling in the transaction state DEACT_ENTER.
  If protocol needs to be deactivated, it sets the next trans_state to
  DEACTD_CNF else sets the next trans_state PH_STAT_ENTER.

DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Should wait for deactivated confirmation.
  MMOC_EVT_STATUS_NOT_CONSUMED - Start ph_stat_enter processing.


SIDE EFFECTS
  None
===========================================================================*/
static  mmoc_evt_status_e_type         mmoc_process_common_deact_enter
(
        prot_deact_e_type              deact_reason,
           /* Reason for deactivating the active protocol.
           */

        mmoc_state_info_s_type         *mmoc_info_ptr
           /* Pointer to MMoC's state information.
           */

)
{

  /* Send the deactivate request to active protocol.
  */
  if ( mmoc_deactivate_all_active_protocol( deact_reason,
                                            mmoc_info_ptr
                                          )
     )
  {
     /* Protocol deactivate request sent, wait for confirmation.
     */
     mmoc_update_trans_state( MMOC_TRANS_STATE_WAIT_DEACTD_CNF,
                              mmoc_info_ptr
                            );


     return MMOC_EVT_STATUS_CONSUMED;

  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* No active protocols present, start sending the phone status
  ** changed command.
  */
  mmoc_update_trans_state( MMOC_TRANS_STATE_PROT_PH_STAT_ENTER,
                           mmoc_info_ptr
                         );


  return MMOC_EVT_STATUS_NOT_CONSUMED;


} /* mmoc_process_common_deact_enter() */
/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_common_auto_deact_ind_rpt

DESCRIPTION
 This function handles the common processing of an auto deactivation
 report when MMOC is waiting for other reports such as ph stat chgd cnf
 or deact_cnf.

  In this state, the state handler will
  1. Update the state
  2. Stay in same trans state, so that we can continue to wait for the desired 
     report

DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.

SIDE EFFECTS
  None.
===========================================================================*/
static  mmoc_evt_status_e_type         mmoc_process_common_auto_deact_ind_rpt
(

        const mmoc_rpt_msg_s_type      *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{
  /* Autonomous deactivated report received.
  */
  MMOC_MSG_LOW("Received autonomous deactivated report", 0, 0, 0);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifdef MMOC_HYBR_SUPPORTED
#error code not present
  #endif /* MMOC_HYBR_SUPPORTED */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifdef MMOC_HYBR_GW_SUPPORTED
  /* Check if the deactivation was from SS_HDR system selection instance.
  */
  if ( rpt_ptr->param.prot_deactd_ind.prot_state  == PROT_STATE_ONLINE_GW &&
       mmoc_info_ptr->prot_state[SD_SS_HYBR_1] == PROT_STATE_ONLINE_GW )
  {

    /* If hybrid preference is not ON then hybrid hdr instance should
    ** not be triggered to go ONLINE
    */
    if (sd_misc_is_hybr_opr())
    {
      /* It is for SS_HDR instance, activate SS_HDR and return.
      */
      (void)mmoc_activate_prot_online_ss_hdr( sd_ss_hybr_1_act_get(NULL),
                                              mmoc_info_ptr
                                            );
    }
    else
    {
      MMOC_MSG_ERROR ("trans_state WAIT_DEACT_CNF,AUTO_DEACTD_IND from HYBR",
                                                                    0, 0, 0);                   
      mmoc_update_prot_state( PROT_STATE_NULL,
                              SD_SS_HYBR_HDR,
                              mmoc_info_ptr
                            );

    }
    return MMOC_EVT_STATUS_CONSUMED;
  }
  #endif  //MMOC_HYBR_GW_SUPPORTED
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef MMOC_WLAN_SUPPORTED
#error code not present
  #endif /* MMOC_WLAN_SUPPORTED */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*  Activate main at the end of transaction.
  */
  mmoc_info_ptr->trans_info.gen_prot_cmd.is_activate_main = TRUE;
  mmoc_update_prot_state( PROT_STATE_NULL,
                         SD_SS_MAIN,
                         mmoc_info_ptr
                       );

  /* Wait for the acknowledgement for generic command.
  */
  SYS_ARG_NOT_USED(rpt_ptr->rpt.name);
  return MMOC_EVT_STATUS_CONSUMED;
}// mmoc_process_common_auto_deact_ind_rpt


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_common_deactd_cnf

DESCRIPTION
 This function handles the common processing  in the WAIT_DEACTD_CNF
 transaction state.

  In this state, the state handler will
  1. Validate the deactivated confirmation report received
  2. Move the trans. state to send the phone status command.

DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
static  mmoc_evt_status_e_type         mmoc_process_common_deactd_cnf
(

        const mmoc_rpt_msg_s_type      *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{
  MMOC_ASSERT( rpt_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Check if the rpt ptr is NULL
  */
  if ( rpt_ptr == NULL )
  {
    /* Not the expected report.
    */
    MMOC_MSG_ERROR("rpt NULL", 0, 0, 0);
    return MMOC_EVT_STATUS_CONSUMED;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Process the received report.
  */
  switch (rpt_ptr->rpt.name )
  {
    case MMOC_RPT_PROT_AUTO_DEACTD_IND:     
      return  mmoc_process_common_auto_deact_ind_rpt(rpt_ptr,mmoc_info_ptr);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case MMOC_RPT_PROT_DEACTD_CNF:
      if(mmoc_info_ptr->trans_id != rpt_ptr->param.prot_deactd_cnf.trans_id)
      {
        /* Not the expected report. Go back to waiting for cnf
        */
        MMOC_MSG_ERROR(" Unexpected event received ", 0, 0, 0);
        return MMOC_EVT_STATUS_CONSUMED;
      }
      /* Expected rpt. So update the prot state and continue processing
      */

      mmoc_update_prot_state( PROT_STATE_NULL,
                              mmoc_info_ptr->deact_req_index,
                              mmoc_info_ptr
                            );

      /* Multiple protocols have to be deactivated, trans_state has to be
      ** set to DEACT_ENTER.
      */
      mmoc_update_trans_state( MMOC_TRANS_STATE_PROT_DEACT_ENTER,
                               mmoc_info_ptr
                             );


      return MMOC_EVT_STATUS_NOT_CONSUMED;


    default:
        MMOC_MSG_ERROR( "Unexpected report received %d",
                        rpt_ptr->rpt.name, 0, 0
                      );
        return MMOC_EVT_STATUS_CONSUMED;


  } /* switch ( rpt_ptr->rpt.name ) */

} /* mmoc_process_common_deactd_cnf() */


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_common_hdr_deact_enter

DESCRIPTION
  This is a common function for handling transaction state hdr_deact_enter.
  It sets the next state to WAIT_DEACTD_CNF

DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Should wait for deactivated confirmation.
  MMOC_EVT_STATUS_NOT_CONSUMED - Start ph_stat_enter processing.


SIDE EFFECTS
  None
===========================================================================*/
/*lint -esym(715,mmoc_info_ptr)*/
/*lint -esym(715,deact_reason)*/
static  mmoc_evt_status_e_type         mmoc_process_common_hdr_deact_enter
(
        prot_deact_e_type              deact_reason,
           /* Reason for deactivating the active protocol.
           */

        mmoc_state_info_s_type        *mmoc_info_ptr
           /* Pointer to MMoC's state information.
           */

)
{
  /* Can't be const as when HDR features are on, we get
  ** assignment to const object error 
  */
  SYS_ARG_NOT_CONST(mmoc_info_ptr);

  #ifdef MMOC_HDR_SUPPORTED    
#error code not present
  #endif


  return MMOC_EVT_STATUS_CONSUMED;

} /* mmoc_process_common_hdr_deact_enter () */
/*lint +esym(715,deact_reason)*/
/*lint +esym(715,mmoc_info_ptr)*/


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_common_hdr_deactd_cnf

DESCRIPTION
  This function handles the hdr deactivation cnf and proceeds to kick the 
  MAIN instance to activate any protocol that was waiting on hdr deactf.  


DEPENDENCIES
  None

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
/*lint -esym(715,cmd_ptr)*/
static  mmoc_evt_status_e_type         mmoc_process_common_hdr_deactd_cnf 
(

        const mmoc_cmd_msg_s_type      *cmd_ptr,
            /* Pointer to the command event that was received.
            ** if this was not called because of command event, then
            ** cmd_ptr = null.
            */

        const mmoc_rpt_msg_s_type            *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{

  

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  MMOC_ASSERT( rpt_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  if (mmoc_process_common_deactd_cnf (rpt_ptr, mmoc_info_ptr) ==
                                              MMOC_EVT_STATUS_CONSUMED)
  {

    /* Rpt does not carry hdr deact cnf, so continue waiting on rpt queues
    */
    return MMOC_EVT_STATUS_CONSUMED;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  

  
  /* Protocol in ss instance is made to acquire
  */
  if ( mmoc_activate_prot_online (SD_SS_MAIN, mmoc_info_ptr)
                                            == MMOC_EVT_STATUS_NOT_CONSUMED
     )
  {

    /* Code reaches here only when we have to exit PWR save state
    */
    MMOC_ASSERT (mmoc_info_ptr->curr_trans == MMOC_TRANS_PWR_SAVE_EXIT);

    /* Invoke the new transaction handler.
    */
    return MMOC_EVT_STATUS_NOT_CONSUMED;

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If mmoc_clear_transaction gets called with is_activate_main as TRUE
  ** then activate_prot_online_ss_main will get called twice resulting in
  ** activation request being sent twice
  */
  if (mmoc_info_ptr->trans_info.gen_prot_cmd.is_activate_main)
  {
    MMOC_MSG_ERROR (" NOT Expecting is_activate_main to be TRUE", 0, 0, 0);
    MMOC_ASSERT (mmoc_info_ptr->trans_info.gen_prot_cmd.is_activate_main == FALSE);
  }


  /* Done with processing, clear the transaction.
  */
  return mmoc_clear_transaction( mmoc_info_ptr );

} /* mmoc_process_common_hdr_deactd_cnf() */
/*lint +esym(715,cmd_ptr)*/


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_common_ph_stat_cnf

DESCRIPTION
   This function handles the common processing in the
 WAIT_PH_STAT_CNF transaction state.

  In this state, the state handler will
  1. Validate the phone status command ack received from the protocols.
  2. Move to PH_STAT_ENTER state to determine if phone status command
     has to be sent.

DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
static  mmoc_evt_status_e_type         mmoc_process_common_ph_stat_cnf
(

        const mmoc_rpt_msg_s_type            *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{

  MMOC_ASSERT( rpt_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!MMOC_ENUM_IS_INRANGE(rpt_ptr->param.ph_stat_cnf.prot,SYS_SYS_MODE_MAX))
  {
      /* Not the expected report.
      */
      MMOC_MSG_ERROR("Unknown report", 0, 0, 0 );
      return MMOC_EVT_STATUS_CONSUMED;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if this is the correct ack.
  */
  if ( rpt_ptr == NULL ||
       (rpt_ptr->rpt.name == MMOC_RPT_PH_STAT_CHGD_CNF &&
        ph_stat_cmd_tbl[rpt_ptr->param.ph_stat_cnf.prot].trans_id  !=
                                        rpt_ptr->param.ph_stat_cnf.trans_id)
     )
  {
    /* Not the expected report.
    */
    MMOC_MSG_ERROR("Unknown report", 0, 0, 0 );
    return MMOC_EVT_STATUS_CONSUMED;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  switch(rpt_ptr->rpt.name)
  {
    case MMOC_RPT_PROT_AUTO_DEACTD_IND:  
      return  mmoc_process_common_auto_deact_ind_rpt(rpt_ptr,mmoc_info_ptr);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case MMOC_RPT_PH_STAT_CHGD_CNF:
      /* Clear the pending cnf for protocol.
      */
      ph_stat_cmd_tbl[rpt_ptr->param.ph_stat_cnf.prot].trans_id = PROT_TRANS_NONE;


      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


      /* Check if all the confirmation have been received.
      */
      {
        int i = 0;
        for ( i = 0 ; i < (int)(ARR_SIZE(ph_stat_cmd_tbl)); i ++ )
        {
          if (ph_stat_cmd_tbl[i].trans_id != PROT_TRANS_NONE )
          {
            /* Still waiting for additional "cnf".
            */
            return MMOC_EVT_STATUS_CONSUMED;
          }
         }
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* All cnf. recvd. move to PH_STAT_ENTER.
      */
      mmoc_update_trans_state( MMOC_TRANS_STATE_PROT_PH_STAT_ENTER,
                               mmoc_info_ptr
                             );


      return MMOC_EVT_STATUS_NOT_CONSUMED;

    default:
      /* Not the expected report.
      */
      MMOC_MSG_ERROR("Unknown report", 0, 0, 0 );
      return MMOC_EVT_STATUS_CONSUMED;

  } //switch(rpt_ptr->rpt.name)

} /* mmoc_process_common_ph_stat_cnf() */


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_common_tmc_mode_cnf

DESCRIPTION
   This function handles the common processing in the
   MMOC_TRANS_STATE_WAIT_TMC_MODE_CNF transaction state.

  In this state, the state handler will validate
  1. Rpt name and transaction id recvd from TMC.
  
 On receiving a valid report the transaction gets cleared. 

DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
static  mmoc_evt_status_e_type         mmoc_process_common_tmc_mode_cnf
(

        const mmoc_rpt_msg_s_type            *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{

  MMOC_ASSERT( rpt_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Check if this is the correct ack.
  */
  if ( rpt_ptr == NULL ||
       rpt_ptr->rpt.name != MMOC_RPT_TMC_MODE_CNF  ||
       rpt_ptr->param.tmc_mode_cnf.trans_id != 
                         mmoc_get_curr_trans_id (mmoc_info_ptr)
     )
  {
    /* Not the expected report.
    */
    MMOC_MSG_ERROR("Unknown report", 0, 0, 0 );
    return MMOC_EVT_STATUS_CONSUMED;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Done with processing, clear the transaction.
  */
  return mmoc_clear_transaction( mmoc_info_ptr );
  
} /* mmoc_process_common_tmc_mode_cnf () */



/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
==================== SUBSCRIPTION CHANGED TRANSACTION =======================
=============================================================================
=============================================================================
===========================================================================*/

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_subsc_chgd_null

DESCRIPTION
  This function handles the subscription changed transaction in the NULL
  transaction state.

  In this state, the state handler will

DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
/*lint -esym(715,rpt_ptr)*/
static  mmoc_evt_status_e_type         mmoc_process_subsc_chgd_null
(

        const mmoc_cmd_msg_s_type            *cmd_ptr,
            /* Pointer to the command event that was received.
            ** if this was not called because of command event, then
            ** cmd_ptr = null.
            */

        const mmoc_rpt_msg_s_type            *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{
  MMOC_ASSERT( cmd_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* The subscription changed command from CM should only create this
  ** transaction.
  */
  if ( cmd_ptr == NULL ||
       cmd_ptr->cmd.name != MMOC_CMD_SUBSCRIPTION_CHGD
     )
  {
    MMOC_MSG_ERROR("Subsc chgd trans incorrectly initiated", 0,0,0);

    return mmoc_clear_transaction( mmoc_info_ptr );
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Phone should be in either powerup or online.
  */
  if (  mmoc_info_ptr->curr_oprt_mode != PROT_OPRT_MODE_NULL   &&
        mmoc_info_ptr->curr_oprt_mode != PROT_OPRT_MODE_ONLINE
     )
  {
    /* Incorrect oprt_mode, clear transaction.
    */
    MMOC_MSG_ERROR("Incorrect oprt_mode", 0,0,0);
    return mmoc_clear_transaction( mmoc_info_ptr );
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Update the NAM and subscription availability status in
  ** the MMoC state info.
  */
  mmoc_info_ptr->curr_nam = cmd_ptr->param.subsc_chgd.nam;
  mmoc_info_ptr->is_cdma_subs_avail =
                 cmd_ptr->param.subsc_chgd.is_cdma_subsc_avail;
  mmoc_info_ptr->is_gw_subs_avail =
                 cmd_ptr->param.subsc_chgd.is_gw_subsc_avail;
  mmoc_info_ptr->is_wlan_subs_avail =
                 cmd_ptr->param.subsc_chgd.is_wlan_subsc_avail;
  mmoc_info_ptr->prot_subsc_chg = cmd_ptr->param.subsc_chgd.prot_subsc_chg;

  /* Initialize the transaction info.
  */
  mmoc_info_ptr->trans_info.subsc_chgd = cmd_ptr->param.subsc_chgd;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Flush the NV RUIM cache to cause NVRUIM module to read items from the
  ** card.
  */
  #ifdef FEATURE_NV_RUIM
  nvruim_flush_cache();
  #endif /* FEATURE_NV_RUIM */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Deactivate the active protocols, Move to DEACT_ENTER trans.
  ** state.
  */
  mmoc_update_trans_state( MMOC_TRANS_STATE_PROT_DEACT_ENTER,
                           mmoc_info_ptr
                         );



  return MMOC_EVT_STATUS_NOT_CONSUMED;

} /* mmoc_process_subsc_chgd_null() */
/*lint +esym(715,rpt_ptr)*/

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_subsc_ph_stat_enter

DESCRIPTION
  This function handles the offline transaction in the PH_STAT_ENTER
  transaction state.

  In this transaction state, the handler will
  1. Send the Phone status changed command to the next supported protocol
     and move to WAIT_PH_STAT_CNF.
     If the phone status changed command was sent to all the protocols,
     then SD is initialized with new subscription and activate the protocol
     if the operating mode is online.


DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
/*lint -esym(715,cmd_ptr)*/
/*lint -esym(715,rpt_ptr)*/
static  mmoc_evt_status_e_type       mmoc_process_subsc_chgd_ph_stat_enter(

        const mmoc_cmd_msg_s_type            *cmd_ptr,
            /* Pointer to the command event that was received.
            ** if this was not called because of command event, then
            ** cmd_ptr = null.
            */

        const mmoc_rpt_msg_s_type            *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{
  /* Phone status chg type.
  */
  prot_ph_stat_chg_e_type chg_type = PROT_PH_STAT_CHG_MAX;

  /* Is SD init called ?
  */
  boolean                 is_sd_init_called = FALSE;

  /* SD action.
  */
  sd_ss_act_e_type        sd_act;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Determine the phone status change type
  */
  if ( mmoc_info_ptr->trans_info.subsc_chgd.chg_type
                 == MMOC_SUBSC_CHG_SUBSC_AVAIL )
  {
    chg_type = PROT_PH_STAT_CHG_SUBSC;
  }
  else if ( mmoc_info_ptr->trans_info.subsc_chgd.chg_type
                  == MMOC_SUBSC_CHG_NAM_SEL )
  {
    chg_type = PROT_PH_STAT_CHG_NAM;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Send the phone status changed command to the next supported
  ** protocol.
  */
  if ( mmoc_send_subsc_chg_ph_stat_chgd_cmd( chg_type,
                                   mmoc_info_ptr
                                 )
     )
  {
    /* Phone status changed command sent, so wait for ack.
    */
    mmoc_update_trans_state( MMOC_TRANS_STATE_WAIT_PH_STAT_CNF,
                             mmoc_info_ptr
                           );
    return MMOC_EVT_STATUS_CONSUMED;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Start System Determination, if not initialized before.
  */
  if ( !mmoc_info_ptr->is_sd_initialized )
  {
    /* SD not initialized before, initialize now.
    */
    if (!sd_init(MMOC_NV_CMD_SIG, MMOC_RPT_TIMER_SIG, mmoc_kick_dog,
        mmoc_ss_main_timer_cb,
                  mmoc_ss_hdr_timer_cb,
                  mmoc_ss_hybr_wlan_timer_cb
                )
       )
    {
       /* System initialize failed, Move the Phone into offline state and
       ** end the transaction.
       */
       MMOC_MSG_ERROR("sd_init failed, going offline", 0, 0, 0);
       (void)cm_ph_cmd_oprt_mode( NULL,
                                  NULL,
                                  CM_CLIENT_ID_ANONYMOUS,
                                  SYS_OPRT_MODE_OFFLINE
                                );

       return mmoc_clear_transaction( mmoc_info_ptr );

    } /* if ( sd_init() ) */


   /* Set sd init called flag.
   */
    is_sd_init_called = TRUE;

  } /* if ( !mmoc_info_ptr->is_sd_initialized ) */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Done with sending the Phone status changed commands. Initialize sd
  ** with the new subscription information.
  */

  /* if wlan subscription has changed , update wlan info in SD*/
  if(CHECK_PROTO(mmoc_info_ptr->prot_subsc_chg ,PROT_SUBSC_CHG_WLAN))
  {
    sd_wlan_nam_sel((sd_nam_e_type)
                    mmoc_info_ptr->trans_info.subsc_chgd.nam,
                    mmoc_info_ptr->trans_info.subsc_chgd.orig_mode,
                    mmoc_info_ptr->trans_info.subsc_chgd.mode_pref,
                    mmoc_info_ptr->trans_info.subsc_chgd.band_pref,
                    mmoc_info_ptr->trans_info.subsc_chgd.prl_pref,
                    mmoc_info_ptr->trans_info.subsc_chgd.roam_pref,
                    mmoc_info_ptr->trans_info.subsc_chgd.hybr_pref,
                    &mmoc_info_ptr->trans_info.subsc_chgd.manual_sys_info,
                    mmoc_info_ptr->trans_info.subsc_chgd.domain_pref,
                    mmoc_info_ptr->trans_info.subsc_chgd.acq_order_pref,
                    mmoc_info_ptr->trans_info.subsc_chgd.tech_pref,
                    mmoc_info_ptr->trans_info.subsc_chgd.wlan_scan_pref,
                    mmoc_info_ptr->trans_info.subsc_chgd.wlan_bss_type_pref,
                    mmoc_info_ptr->trans_info.subsc_chgd.is_gw_subsc_avail,
                    mmoc_info_ptr->trans_info.subsc_chgd.is_cdma_subsc_avail,
                    mmoc_info_ptr->trans_info.subsc_chgd.is_wlan_subsc_avail
                    );
   
  } /* if wlan subscription has changed */

  /* If either CDMA or GW subscription has changed, update SD with this info
  */
  if (CHECK_PROTO(mmoc_info_ptr->prot_subsc_chg ,PROT_SUBSC_CHG_GW)||
      CHECK_PROTO(mmoc_info_ptr->prot_subsc_chg ,PROT_SUBSC_CHG_CDMA)) 
  {
  if ( !sd_nam_sel2((sd_nam_e_type)
                    mmoc_info_ptr->trans_info.subsc_chgd.nam,
                    mmoc_info_ptr->trans_info.subsc_chgd.orig_mode,
                    mmoc_info_ptr->trans_info.subsc_chgd.mode_pref,
                    mmoc_info_ptr->trans_info.subsc_chgd.band_pref,
                    mmoc_info_ptr->trans_info.subsc_chgd.prl_pref,
                    mmoc_info_ptr->trans_info.subsc_chgd.roam_pref,
                    mmoc_info_ptr->trans_info.subsc_chgd.hybr_pref,
                    &mmoc_info_ptr->trans_info.subsc_chgd.manual_sys_info,
                    mmoc_info_ptr->trans_info.subsc_chgd.domain_pref,
                    mmoc_info_ptr->trans_info.subsc_chgd.acq_order_pref,
                    mmoc_info_ptr->trans_info.subsc_chgd.tech_pref,
                    mmoc_info_ptr->trans_info.subsc_chgd.wlan_scan_pref,
                    mmoc_info_ptr->trans_info.subsc_chgd.wlan_bss_type_pref,
                    mmoc_info_ptr->trans_info.subsc_chgd.is_gw_subsc_avail,
                    mmoc_info_ptr->trans_info.subsc_chgd.is_cdma_subsc_avail,
                    mmoc_info_ptr->trans_info.subsc_chgd.is_wlan_subsc_avail
                   )
     )
   {
     /* Move the Phone into offline state.
     */
     MMOC_MSG_ERROR("sd_nam_sel2 failed, going offline", 0, 0, 0);
     (void)cm_ph_cmd_oprt_mode( NULL,
                                NULL,
                                CM_CLIENT_ID_ANONYMOUS,
                                SYS_OPRT_MODE_OFFLINE
                              );

     return mmoc_clear_transaction( mmoc_info_ptr );

   } /* if ( !sd_nam_sel2() ) */

  } /* If either CDMA or GW subscription has changed */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Set MMoC's state info to SD initialized.
  */
  mmoc_set_sd_initialized( mmoc_info_ptr );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if we can vote off UIM.
  */
  #ifdef FEATURE_UIM_RUIM
  if (  mmoc_info_ptr->trans_info.subsc_chgd.is_cdma_subsc_avail  ||
        mmoc_info_ptr->trans_info.subsc_chgd.is_gw_subsc_avail
     )
  {
    /* Indicate to power down the RUIM */
    (void)uim_power_control(UIM_MC, FALSE);

  }
  #endif /* FEATURE_UIM_RUIM */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* If the current operating mode status is ONLINE, activate the protocol.
  */
  if ( mmoc_info_ptr->curr_oprt_mode == PROT_OPRT_MODE_ONLINE )
  {
    /* Call the SD indication depending on whether sd was initialized here
    ** or if the subsc. changed type is because the subscription is
    ** available or not.
    */
    if ( is_sd_init_called  ||
         mmoc_info_ptr->trans_info.subsc_chgd.chg_type ==
                                        MMOC_SUBSC_CHG_SUBSC_AVAIL
       )
    {
      /*If only WLAN subscription has changed,no need to go to 
      ** user power up script. Just clear transaction and return.
      ** CM takes care to send wlan_user_ss_pref to start wlan 
      ** acquisitions
      */
      if(mmoc_info_ptr->prot_subsc_chg!=PROT_SUBSC_CHG_WLAN)
      {
        sd_act = sd_ss_ind_user_pwr_up( NULL );
      }
      else
      {
        return mmoc_clear_transaction( mmoc_info_ptr );
      }
    }
    else
    {
      sd_act   = sd_ss_ind_user_nam_sel( NULL );
    } /* if ( is_sd_init_called ) */


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    /* SD subscription init successful, activate the protocol or invoke
    ** transaction handler if this function returns EVT_STATUS_NOT_CONSUMED.
    */
    if ( mmoc_activate_prot_online_ss_main( sd_act,
                                            mmoc_info_ptr
                                          )
         == MMOC_EVT_STATUS_NOT_CONSUMED
       )
    {
      /* Invoke the new transaction handler.
      */
      return MMOC_EVT_STATUS_NOT_CONSUMED;

    }
  }
  else if ( mmoc_info_ptr->curr_oprt_mode == PROT_OPRT_MODE_OFFLINE_CDMA ||
            mmoc_info_ptr->curr_oprt_mode == PROT_OPRT_MODE_OFFLINE_AMPS ||
            mmoc_info_ptr->curr_oprt_mode == PROT_OPRT_MODE_FTM
          )
  {
    /* Activate the protocol, if needed in the offline mode.
    */
    mmoc_activate_prot_offline( mmoc_info_ptr );

  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Clear the transaction.
  */
  return mmoc_clear_transaction( mmoc_info_ptr );


} /* mmoc_process_subsc_chgd_ph_stat_enter() */
/*lint +esym(715,cmd_ptr)*/
/*lint +esym(715,rpt_ptr)*/

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_subsc_chgd

DESCRIPTION
  This function is the transaction handler for the subscription changed
  transaction.

  The transaction state transition will be,

 NULL -> DEACT_ENTER -> WAIT_DEACTD_CNF -> PH_STAT_ENTER <-> WAIT_PH_STAT_CNF
              |                              ^    |
              |______________________________|    |-----> NULL

DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
static  mmoc_evt_status_e_type         mmoc_process_subsc_chgd
(

        const mmoc_cmd_msg_s_type            *cmd_ptr,
            /* Pointer to the command event that was received.
            ** if this was not called because of command event, then
            ** cmd_ptr = null.
            */

        const mmoc_rpt_msg_s_type            *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{
  mmoc_evt_status_e_type evt_status = MMOC_EVT_STATUS_CONSUMED;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Invoke the appropriate transaction state handlers.
  */
  switch(mmoc_info_ptr->trans_state)
  {
    case MMOC_TRANS_STATE_NULL:
      evt_status = mmoc_process_subsc_chgd_null( cmd_ptr,
                                                 rpt_ptr,
                                                 mmoc_info_ptr
                                               );
      break;

    case MMOC_TRANS_STATE_PROT_DEACT_ENTER:
      evt_status =  mmoc_process_subsc_chgd_deact_enter( PROT_DEACT_SUBSC_CHGD,
                                                     mmoc_info_ptr
                                                   );
      break;

    case MMOC_TRANS_STATE_WAIT_DEACTD_CNF:
      evt_status = mmoc_process_common_deactd_cnf( rpt_ptr,
                                                   mmoc_info_ptr
                                                 );
      break;

    case MMOC_TRANS_STATE_PROT_PH_STAT_ENTER:
      evt_status = mmoc_process_subsc_chgd_ph_stat_enter( cmd_ptr,
                                                          rpt_ptr,
                                                          mmoc_info_ptr
                                                        );
      break;

    case MMOC_TRANS_STATE_WAIT_PH_STAT_CNF:
      evt_status = mmoc_process_common_ph_stat_cnf( rpt_ptr,
                                                    mmoc_info_ptr
                                                  );
      break;


    case MMOC_TRANS_STATE_HDR_DEACT_ENTER:
      evt_status = mmoc_process_common_hdr_deact_enter  
                                                     ( PROT_DEACT_ACQ_OTHERS,                                                         
                                                       mmoc_info_ptr
                                                     );
      break;



    case MMOC_TRANS_STATE_WAIT_HDR_DEACTD_CNF:
      evt_status = mmoc_process_common_hdr_deactd_cnf ( cmd_ptr,
                                                        rpt_ptr,
                                                        mmoc_info_ptr
                                                      );
      break;

    case MMOC_TRANS_STATE_WAIT_GEN_CMD_CNF:
    case MMOC_TRANS_STATE_MAX:
    default:
      MMOC_MSG_ERROR("trans_state", mmoc_info_ptr->trans_state, 0, 0);
      evt_status = mmoc_clear_transaction( mmoc_info_ptr );
      break;
  }


  return evt_status;

} /* mmoc_process_subsc_chgd() */



/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
==================== ONLINE COMMAND TRANSACTION =============================
=============================================================================
=============================================================================
===========================================================================*/

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_online_null

DESCRIPTION
  This function handles the online transaction in the NULL transaction state.

  In this state, the state handler will
  1. Call the power up SD indication if SD was initialized earlier and
     activate the protocol was indicated by SD action.


DEPENDENCIES

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
/*lint -esym(715,rpt_ptr)*/
static  mmoc_evt_status_e_type         mmoc_process_online_null
(

        const mmoc_cmd_msg_s_type            *cmd_ptr,
            /* Pointer to the command event that was received.
            ** if this was not called because of command event, then
            ** cmd_ptr = null.
            */

        const mmoc_rpt_msg_s_type            *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{
  MMOC_ASSERT( cmd_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* The online operating mode changed command from CM should only create this
  ** transaction.
  */
  if ( cmd_ptr == NULL ||
       cmd_ptr->cmd.name != MMOC_CMD_OPRT_MODE_CHGD  ||
       cmd_ptr->param.oprt_mode_chgd.oprt_mode != SYS_OPRT_MODE_ONLINE
     )
  {
    MMOC_MSG_ERROR("Online trans incorrectly initiated", 0,0,0);

    return mmoc_clear_transaction( mmoc_info_ptr );
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Should be in power off or online mode or LPM currently.
  */
  if ( mmoc_info_ptr->curr_oprt_mode != PROT_OPRT_MODE_NULL   &&
       mmoc_info_ptr->curr_oprt_mode != PROT_OPRT_MODE_ONLINE &&
       mmoc_info_ptr->curr_oprt_mode != PROT_OPRT_MODE_LPM &&
       mmoc_info_ptr->curr_oprt_mode != PROT_OPRT_MODE_FTM
     )
  {
    MMOC_MSG_ERROR( "Incorrect current oprt mode %d",
                    mmoc_info_ptr->curr_oprt_mode, 0, 0
                  );
    return mmoc_clear_transaction( mmoc_info_ptr );
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Initialize the operating mode info.
  */
  mmoc_info_ptr->curr_oprt_mode    = PROT_OPRT_MODE_ONLINE;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Send ph status to TMC */
  if (mmoc_is_wait_reqd_on_cmd_tmc (mmoc_info_ptr)
     )
  {
    return MMOC_EVT_STATUS_CONSUMED;
  }  

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Deactivate any protocols that are active.
  */
  mmoc_update_trans_state( MMOC_TRANS_STATE_PROT_DEACT_ENTER,
                           mmoc_info_ptr
                         );

  return MMOC_EVT_STATUS_NOT_CONSUMED;


} /* mmoc_process_online_null() */
/*lint +esym(715,rpt_ptr)*/

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_online_tmc_mode_cnf

DESCRIPTION
  This function handles online tmc mode transaction in the
  MMOC_TRANS_STATE_WAIT_TMC_MODE_CNF transaction state.

  In this transaction state, the handler will
  1. Enter protocol deactivation state after TMC reports MODE_CNF


DEPENDENCIES
  None


RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
static  mmoc_evt_status_e_type         mmoc_process_online_tmc_mode_cnf
(

        const mmoc_rpt_msg_s_type            *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{
  MMOC_ASSERT( rpt_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Check if this is the expected deactivated report.
  */
  if ( rpt_ptr == NULL ||
       rpt_ptr->rpt.name != MMOC_RPT_TMC_MODE_CNF ||
       rpt_ptr->param.tmc_mode_cnf.trans_id != 
                         mmoc_get_curr_trans_id (mmoc_info_ptr)
     )
  {
    /* Not the expected report.
    */
    MMOC_MSG_ERROR(" Unexpected event received ", 0, 0, 0);
    return MMOC_EVT_STATUS_CONSUMED;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Multiple protocols have to be deactivated, trans_state 
  ** has to be set to DEACT_ENTER.
  */
  mmoc_update_trans_state( MMOC_TRANS_STATE_PROT_DEACT_ENTER,
                           mmoc_info_ptr
                         );


  return MMOC_EVT_STATUS_NOT_CONSUMED;

} /* mmoc_process_online_tmc_mode_cnf() */

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_online_ph_stat_enter

DESCRIPTION
  This function handles the offline transaction in the PH_STAT_ENTER
  transaction state.

  In this transaction state, the handler will
  1. Send the Phone status changed command to the next supported protocol
     and move to WAIT_PH_STAT_CNF.
     If the phone status changed command was sent to all the protocols,
     then SD is initialized with new subscription and activate the protocol
     if the operating mode is online.


DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
/*lint -esym(715,cmd_ptr)*/
/*lint -esym(715,rpt_ptr)*/
static  mmoc_evt_status_e_type       mmoc_process_online_ph_stat_enter(

        const mmoc_cmd_msg_s_type            *cmd_ptr,
            /* Pointer to the command event that was received.
            ** if this was not called because of command event, then
            ** cmd_ptr = null.
            */

        const mmoc_rpt_msg_s_type            *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{
  /* Phone status chg type.
  */
  prot_ph_stat_chg_e_type chg_type = PROT_PH_STAT_CHG_OPRT_MODE;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Send the phone status changed command to the next supported
  ** protocol.
  */
  if ( mmoc_send_ph_stat_chgd_cmd( chg_type,
                                   mmoc_info_ptr
                                 )
     )
  {
    /* Phone status changed command sent, so wait for ack.
    */
    mmoc_update_trans_state( MMOC_TRANS_STATE_WAIT_PH_STAT_CNF,
                             mmoc_info_ptr
                           );
    return MMOC_EVT_STATUS_CONSUMED;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Check if SD was initialized earlier.
  */
  if ( mmoc_info_ptr->is_sd_initialized )
  {
     /* SD subscription init was successful, start from pwr_up.
     */
     if ( mmoc_activate_prot_online_ss_main( sd_ss_ind_user_pwr_up(NULL),
                                             mmoc_info_ptr
                                           )
           == MMOC_EVT_STATUS_NOT_CONSUMED
         )
      {
        /* Invoke the new transaction handler.
        */
        return MMOC_EVT_STATUS_NOT_CONSUMED;

      }

  } /*  if ( mmoc_info_ptr->is_sd_initialized ) */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Clear the transaction.
  */
  return mmoc_clear_transaction( mmoc_info_ptr );


} /* mmoc_process_online_ph_stat_enter() */
/*lint +esym(715,cmd_ptr)*/
/*lint +esym(715,rpt_ptr)*/



/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_online

DESCRIPTION
  This function is the transaction handler for the online transaction.

  The transaction state transition will be,

     NULL -> NULL


DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
static  mmoc_evt_status_e_type         mmoc_process_online
(

        const mmoc_cmd_msg_s_type            *cmd_ptr,
            /* Pointer to the command event that was received.
            ** if this was not called because of command event, then
            ** cmd_ptr = null.
            */

        const mmoc_rpt_msg_s_type            *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{
  mmoc_evt_status_e_type  evt_status = MMOC_EVT_STATUS_CONSUMED;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Invoke the appropriate transaction state handlers.
  */
  switch(mmoc_info_ptr->trans_state)
  {
    case MMOC_TRANS_STATE_NULL:
      evt_status = mmoc_process_online_null( cmd_ptr,
                                             rpt_ptr,
                                             mmoc_info_ptr
                                           );
      break;

    case MMOC_TRANS_STATE_WAIT_TMC_MODE_CNF:
      evt_status = mmoc_process_online_tmc_mode_cnf (rpt_ptr,
                                                     mmoc_info_ptr
                                                    );
      break;

    case MMOC_TRANS_STATE_PROT_DEACT_ENTER:
      evt_status = mmoc_process_common_deact_enter( PROT_DEACT_SUBSC_CHGD,
                                                    mmoc_info_ptr
                                                  );
      break;

    case MMOC_TRANS_STATE_WAIT_DEACTD_CNF:
      evt_status = mmoc_process_common_deactd_cnf( rpt_ptr,
                                                   mmoc_info_ptr
                                                 );
      break;

    case MMOC_TRANS_STATE_PROT_PH_STAT_ENTER:
      evt_status = mmoc_process_online_ph_stat_enter( cmd_ptr,
                                                      rpt_ptr,
                                                      mmoc_info_ptr
                                                    );
      break;

    case MMOC_TRANS_STATE_WAIT_PH_STAT_CNF:
      evt_status = mmoc_process_common_ph_stat_cnf( rpt_ptr,
                                                    mmoc_info_ptr
                                                  );
      break;


    case MMOC_TRANS_STATE_HDR_DEACT_ENTER:
      evt_status = mmoc_process_common_hdr_deact_enter  
                                                     ( PROT_DEACT_ACQ_OTHERS,                                                         
                                                       mmoc_info_ptr
                                                     );
      break;



    case MMOC_TRANS_STATE_WAIT_HDR_DEACTD_CNF:
      evt_status = mmoc_process_common_hdr_deactd_cnf ( cmd_ptr,
                                                        rpt_ptr,
                                                        mmoc_info_ptr
                                                      );
      break;

    case MMOC_TRANS_STATE_WAIT_GEN_CMD_CNF:
    case MMOC_TRANS_STATE_GEN_CMD_ENTER:
    case MMOC_TRANS_STATE_MAX:
    default:
      MMOC_MSG_ERROR("error Trans_state %d", mmoc_info_ptr->trans_state, 0, 0);
      evt_status = mmoc_clear_transaction( mmoc_info_ptr );
      break;
  }


  return evt_status;

} /* mmoc_process_online() */




/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
==================== OFFLINE COMMAND TRANSACTION ============================
=============================================================================
=============================================================================
===========================================================================*/

/*===========================================================================

FUNCTION mmoc_process_offline_null

DESCRIPTION
  This function handles the offline transaction in the NULL transaction
  state.

  In this state, the state handler will
  1. Verify that offline oprt mode command invoked this transaction.
  2. Initialize the current operating mode.
  3. Move the Deact. Enter state.


DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
/*lint -esym(715,rpt_ptr)*/
static  mmoc_evt_status_e_type         mmoc_process_offline_null
(

        const mmoc_cmd_msg_s_type            *cmd_ptr,
            /* Pointer to the command event that was received.
            ** if this was not called because of command event, then
            ** cmd_ptr = null.
            */

        const mmoc_rpt_msg_s_type            *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{
  MMOC_ASSERT( cmd_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* The offline operating mode changed command from CM should only create
  ** this transaction.
  */
  if ( cmd_ptr == NULL ||
       cmd_ptr->cmd.name != MMOC_CMD_OPRT_MODE_CHGD ||
       ( cmd_ptr->param.oprt_mode_chgd.oprt_mode !=
                                               SYS_OPRT_MODE_OFFLINE &&
         cmd_ptr->param.oprt_mode_chgd.oprt_mode !=
                                               SYS_OPRT_MODE_OFFLINE_CDMA &&
         cmd_ptr->param.oprt_mode_chgd.oprt_mode !=
                                               SYS_OPRT_MODE_OFFLINE_AMPS &&
         cmd_ptr->param.oprt_mode_chgd.oprt_mode !=
                                              SYS_OPRT_MODE_FTM
       )
     )
  {
    MMOC_MSG_ERROR("error cmd", 0,0,0);

    return mmoc_clear_transaction( mmoc_info_ptr );
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Initialize the current operating mode and phone status sent index.
  */
  mmoc_info_ptr->curr_oprt_mode = mmoc_map_sys_oprt_mode_to_mmoc
                                  (
                                    cmd_ptr->param.oprt_mode_chgd.oprt_mode
                                  );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Move to DEACT_ENTER trans. state.
  */
  mmoc_update_trans_state( MMOC_TRANS_STATE_PROT_DEACT_ENTER,
                           mmoc_info_ptr
                         );


  return MMOC_EVT_STATUS_NOT_CONSUMED;

} /* mmoc_process_offline_null() */
/*lint +esym(715,rpt_ptr)*/


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_offline_deact_enter

DESCRIPTION
  This function is responsible for the sending the deactivate protocol
  request to the active protocol if present. Else, moves the transaction
  to the state which sends the Phone status change command.

DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
/*lint -esym(715,rpt_ptr)*/
/*lint -esym(715,cmd_ptr)*/
static  mmoc_evt_status_e_type         mmoc_process_offline_deact_enter
(

        const mmoc_cmd_msg_s_type            *cmd_ptr,
            /* Pointer to the command event that was received.
            ** if this was not called because of command event, then
            ** cmd_ptr = null.
            */

        const mmoc_rpt_msg_s_type            *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{
  prot_deact_e_type  deact_reason = PROT_DEACT_MAX;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Send the deactivate request to active protocol.
  */
  switch ( mmoc_info_ptr->curr_oprt_mode)
  {
    case PROT_OPRT_MODE_OFFLINE:
    case PROT_OPRT_MODE_OFFLINE_CDMA:
    case PROT_OPRT_MODE_OFFLINE_AMPS:
      deact_reason = PROT_DEACT_OFFLINE;
      break;

    case PROT_OPRT_MODE_FTM:
      deact_reason = PROT_DEACT_FTM;
      break;

    case PROT_OPRT_MODE_NULL:
    case PROT_OPRT_MODE_PWROFF:
    case PROT_OPRT_MODE_ONLINE:
    case PROT_OPRT_MODE_LPM:
    case PROT_OPRT_MODE_RESET:
    case PROT_OPRT_MODE_MAX:
    default:
      MMOC_ERR_FATAL("Incorrect oprt_mode", 0, 0, 0);
      break;

   } /* switch( mmoc_info_ptr->curr_oprt_mode) */ 



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Perform the common deactivated enter state handling.
  */
  return mmoc_process_common_deact_enter( deact_reason,
                                          mmoc_info_ptr
                                        );

} /* mmoc_process_offline_deact_enter() */
/*lint +esym(715,rpt_ptr)*/
/*lint +esym(715,cmd_ptr)*/



/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_offline_ph_stat_enter

DESCRIPTION
  This function handles the offline transaction in the  PH_STAT_ENTER
  transaction state.

  In this transaction state, the handler will
  1. Send the Phone status changed command to the next supported protocol
     and move to WAIT_PH_STAT_CNF.
     If the phone status changed command was sent to all the protocols,
     SD is informed of the offline oprt mode and activate the protocol
     in offline mode if needed.


DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
/*lint -esym(715,rpt_ptr)*/
/*lint -esym(715,cmd_ptr)*/
static  mmoc_evt_status_e_type       mmoc_process_offline_ph_stat_enter(

        const mmoc_cmd_msg_s_type            *cmd_ptr,
            /* Pointer to the command event that was received.
            ** if this was not called because of command event, then
            ** cmd_ptr = null.
            */

        const mmoc_rpt_msg_s_type            *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Send the phone status changed command to the next supported
  ** protocol.
  */
  if ( mmoc_send_ph_stat_chgd_cmd( PROT_PH_STAT_CHG_OPRT_MODE,
                                   mmoc_info_ptr
                                 )
     )
  {
    /* Phone status changed command sent, so wait for ack.
    */
    mmoc_update_trans_state( MMOC_TRANS_STATE_WAIT_PH_STAT_CNF,
                             mmoc_info_ptr
                           );
    return MMOC_EVT_STATUS_CONSUMED;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* If SD was previously initialized, inform SD of offline mode.
  */
  if ( mmoc_info_ptr->is_sd_initialized )
  {
    /* Done with sending the Phone status changed commands, Call SD
    ** indication to inform of offline mode.
    */
    switch (mmoc_info_ptr->curr_oprt_mode)
    {
      case PROT_OPRT_MODE_OFFLINE:
      case PROT_OPRT_MODE_OFFLINE_CDMA:
        (void) sd_ss_ind_user_offline_cdma(NULL);
        break;

      case PROT_OPRT_MODE_OFFLINE_AMPS:
        (void) sd_ss_ind_user_offline_amps(NULL);
        break;

      case PROT_OPRT_MODE_FTM:
        (void) sd_ss_ind_user_offline_cdma(NULL);
        mmoc_set_sd_term( mmoc_info_ptr );
        break;

      case PROT_OPRT_MODE_NULL:
      case PROT_OPRT_MODE_PWROFF:
      case PROT_OPRT_MODE_ONLINE:
      case PROT_OPRT_MODE_LPM:
      case PROT_OPRT_MODE_RESET:
      case PROT_OPRT_MODE_MAX:
      default:
        MMOC_ERR_FATAL("Should be offline here", 0, 0, 0);
        break;

    } /* switch (mmoc_info_ptr->curr_oprt_mode) */

  } /* if ( mmoc_info_ptr->is_sd_initialized ) */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Activate the protocol, if needed in the offline mode.
  */
  mmoc_activate_prot_offline( mmoc_info_ptr );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Command TMC to move tasks to OFFLINE.
  */
  if (mmoc_is_wait_reqd_on_cmd_tmc (mmoc_info_ptr) )
  {
    return MMOC_EVT_STATUS_CONSUMED;
  } 

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Clear the transaction.
  */
  return mmoc_clear_transaction( mmoc_info_ptr );



} /* mmoc_process_offline_ph_stat_enter() */
/*lint +esym(715,rpt_ptr)*/
/*lint +esym(715,cmd_ptr)*/


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_offline

DESCRIPTION
  This function is the transaction handler for the offline transaction.

  The transaction state transition will be,

 NULL -> DEACT_ENTER -> WAIT_DEACTD_CNF -> PH_STAT_ENTER <-> WAIT_PH_STAT_CNF
              |                              ^    |
              |______________________________|    |-----> NULL
DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
static  mmoc_evt_status_e_type         mmoc_process_offline
(

        const mmoc_cmd_msg_s_type            *cmd_ptr,
            /* Pointer to the command event that was received.
            ** if this was not called because of command event, then
            ** cmd_ptr = null.
            */

        const mmoc_rpt_msg_s_type            *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{
  mmoc_evt_status_e_type  evt_status = MMOC_EVT_STATUS_CONSUMED;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Invoke the appropriate transaction state handler.
  */
  switch(mmoc_info_ptr->trans_state)
  {
    case MMOC_TRANS_STATE_NULL:
      evt_status =  mmoc_process_offline_null( cmd_ptr,
                                               rpt_ptr,
                                               mmoc_info_ptr
                                             );
      break;

    case MMOC_TRANS_STATE_PROT_DEACT_ENTER:
      evt_status =  mmoc_process_offline_deact_enter( cmd_ptr,
                                                      rpt_ptr,
                                                     mmoc_info_ptr
                                                   );
      break;

    case MMOC_TRANS_STATE_WAIT_DEACTD_CNF:
      evt_status =  mmoc_process_common_deactd_cnf( rpt_ptr,
                                                    mmoc_info_ptr
                                                   );
      break;

    case MMOC_TRANS_STATE_PROT_PH_STAT_ENTER:
      evt_status = mmoc_process_offline_ph_stat_enter( cmd_ptr,
                                                       rpt_ptr,
                                                       mmoc_info_ptr
                                                     );
      break;    


    case MMOC_TRANS_STATE_WAIT_PH_STAT_CNF:
      evt_status = mmoc_process_common_ph_stat_cnf( rpt_ptr,
                                                    mmoc_info_ptr
                                                  );
      break;

    case MMOC_TRANS_STATE_WAIT_TMC_MODE_CNF:
      evt_status = mmoc_process_common_tmc_mode_cnf( rpt_ptr,
                                                     mmoc_info_ptr
                                                   ); 
      break;

    case MMOC_TRANS_STATE_WAIT_GEN_CMD_CNF:
    case MMOC_TRANS_STATE_GEN_CMD_ENTER:
    case MMOC_TRANS_STATE_WAIT_AUTO_DEACTD_IND:
    case MMOC_TRANS_STATE_HDR_DEACT_ENTER:
    case MMOC_TRANS_STATE_WAIT_HDR_DEACTD_CNF:
    case MMOC_TRANS_STATE_MAX:
    default:
      MMOC_MSG_ERROR("Received event in unknown state", 0, 0, 0);
      evt_status = mmoc_clear_transaction( mmoc_info_ptr );
      break;
  }


  return evt_status;

} /* mmoc_process_offline() */


/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
==================== POWER DOWN/RESET/LPM COMMAND TRANSACTION ===============
=============================================================================
=============================================================================
===========================================================================*/

/*===========================================================================

FUNCTION mmoc_process_pwr_down_null

DESCRIPTION
  This function handles the power down/reset/lpm transaction in the NULL
  transaction state.

  In this state, the state handler will
  1. Verify that offline oprt mode command invoked this transaction.
  2. Initialize the current operating mode.
  3. Move the Deact. Enter state.


DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
/*lint -esym(715,rpt_ptr)*/
static  mmoc_evt_status_e_type         mmoc_process_pwr_down_null
(

        const mmoc_cmd_msg_s_type            *cmd_ptr,
            /* Pointer to the command event that was received.
            ** if this was not called because of command event, then
            ** cmd_ptr = null.
            */

        const mmoc_rpt_msg_s_type            *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{

  MMOC_ASSERT( cmd_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* The pwroff/reset/lpm operating mode changed command from CM should only
  ** create this transaction.
  */
  if ( cmd_ptr == NULL  ||
       cmd_ptr->cmd.name != MMOC_CMD_OPRT_MODE_CHGD ||
        ( cmd_ptr->param.oprt_mode_chgd.oprt_mode != SYS_OPRT_MODE_PWROFF       &&
          cmd_ptr->param.oprt_mode_chgd.oprt_mode != SYS_OPRT_MODE_RESET        &&
          cmd_ptr->param.oprt_mode_chgd.oprt_mode != SYS_OPRT_MODE_RESET_MODEM  &&
          cmd_ptr->param.oprt_mode_chgd.oprt_mode != SYS_OPRT_MODE_LPM
        )
     )
  {
    MMOC_MSG_ERROR("Powerdown/Reset/LPM command transaction incorrectly initiated",
                   0,0,0);
    return mmoc_clear_transaction( mmoc_info_ptr );
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Initialize the current operating mode and phone status sent index.
  */
  mmoc_info_ptr->curr_oprt_mode = mmoc_map_sys_oprt_mode_to_mmoc
                                  (
                                    cmd_ptr->param.oprt_mode_chgd.oprt_mode
                                  );



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Move to DEACT_ENTER trans. state.
  */
  mmoc_update_trans_state( MMOC_TRANS_STATE_PROT_DEACT_ENTER,
                           mmoc_info_ptr
                         );


  return MMOC_EVT_STATUS_NOT_CONSUMED;

} /* mmoc_process_pwr_down_null() */
/*lint +esym(715,rpt_ptr)*/

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_pwr_down_deact_enter

DESCRIPTION
  This function is responsible for the sending the deactivate protocol
  request to the active protocol if present. Else, moves the transaction
  to the state which sends the Phone status change command.

DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
/*lint -esym(715,rpt_ptr)*/
/*lint -esym(715,cmd_ptr)*/
static  mmoc_evt_status_e_type         mmoc_process_pwr_down_deact_enter
(

        const mmoc_cmd_msg_s_type            *cmd_ptr,
            /* Pointer to the command event that was received.
            ** if this was not called because of command event, then
            ** cmd_ptr = null.
            */

        const mmoc_rpt_msg_s_type            *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{
  prot_deact_e_type  deact_reason = PROT_DEACT_MAX;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Send the deactivate request to active protocol.
  */
  switch ( mmoc_info_ptr->curr_oprt_mode)
  {
    case PROT_OPRT_MODE_PWROFF:
      deact_reason = PROT_DEACT_PWR_DOWN;
      break;

    case PROT_OPRT_MODE_RESET:
      deact_reason = PROT_DEACT_RESET;
      break;

    case PROT_OPRT_MODE_LPM:
      deact_reason = PROT_DEACT_LPM;
      break;

    case PROT_OPRT_MODE_NULL:
    case PROT_OPRT_MODE_OFFLINE:
    case PROT_OPRT_MODE_OFFLINE_AMPS:
    case PROT_OPRT_MODE_OFFLINE_CDMA:
    case PROT_OPRT_MODE_ONLINE:
    case PROT_OPRT_MODE_FTM:
    case PROT_OPRT_MODE_MAX:
    default:
      MMOC_ERR_FATAL("Incorrect oprt_mode", 0, 0, 0);
      break;

   } /* switch( mmoc_info_ptr->curr_oprt_mode) */



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Inform the system event reporting service.
  */
  event_report( EVENT_POWER_DOWN );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Perform the common deactivated enter state handling.
  */
  return mmoc_process_common_deact_enter( deact_reason,
                                          mmoc_info_ptr
                                        );

} /* mmoc_process_pwr_down_deact_enter() */
/*lint +esym(715,rpt_ptr)*/
/*lint +esym(715,cmd_ptr)*/

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_pwr_down_ph_stat_enter

DESCRIPTION
  This function handles the offline transaction in the  PH_STAT_ENTER
  transaction state.

  In this transaction state, the handler will
  1. Send the Phone status changed command to the next supported protocol
     and move to WAIT_PH_STAT_CNF.
     If the phone status changed command was sent to all the protocols,
     Inform the SD with the  reset/lpm/powerdown indication  and
     Shutdown the SD if it is LPM or power down.



DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
/*lint -esym(715,rpt_ptr)*/
/*lint -esym(715,cmd_ptr)*/
static  mmoc_evt_status_e_type       mmoc_process_pwr_down_ph_stat_enter(

        const mmoc_cmd_msg_s_type            *cmd_ptr,
            /* Pointer to the command event that was received.
            ** if this was not called because of command event, then
            ** cmd_ptr = null.
            */

        const mmoc_rpt_msg_s_type            *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{
   /* Command to TMC.
   */
   
   #if 0
   //#if defined(FEATURE_MULTIMODE_RFAPI)
   rf_init_status_type rf_ret_stat;
   #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Send the phone status changed command to the next supported
  ** protocol.
  */
  if ( mmoc_send_ph_stat_chgd_cmd( PROT_PH_STAT_CHG_OPRT_MODE,
                                   mmoc_info_ptr
                                 )
     )
  {
    /* Phone status changed command sent, so wait for ack.
    */
    mmoc_update_trans_state( MMOC_TRANS_STATE_WAIT_PH_STAT_CNF,
                             mmoc_info_ptr
                           );
    return MMOC_EVT_STATUS_CONSUMED;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Done with sending the Phone status changed commands, Inform SD of the
  ** operating mode change.
  */
  switch ( mmoc_info_ptr->curr_oprt_mode)
  {
    case PROT_OPRT_MODE_PWROFF:      
      /* Inform system report service
      */
      event_report( EVENT_POWER_DOWN );
      mmoc_update_prot_state( PROT_STATE_PWR_DOWN,
                              SD_SS_MAIN,
                              mmoc_info_ptr
                            );
      mmoc_update_db_dmss_state( mmoc_info_ptr );
      break;

    case PROT_OPRT_MODE_LPM:
      /* Inform system report service
      */
      event_report( EVENT_POWER_DOWN );
      mmoc_update_prot_state( PROT_STATE_LPM,
                              SD_SS_MAIN,
                              mmoc_info_ptr
                            );
      mmoc_update_db_dmss_state( mmoc_info_ptr );

      /* Send the LPM OK report to CM.
      */
      mmoc_send_cm_rpt( CM_LPM_OK_F );
      break;

    case PROT_OPRT_MODE_RESET:      
      mmoc_update_prot_state( PROT_STATE_RESET,
                              SD_SS_MAIN,
                              mmoc_info_ptr
                            );
      mmoc_update_db_dmss_state( mmoc_info_ptr );
      break;

    case PROT_OPRT_MODE_NULL:
    case PROT_OPRT_MODE_OFFLINE:
    case PROT_OPRT_MODE_OFFLINE_AMPS:
    case PROT_OPRT_MODE_OFFLINE_CDMA:
    case PROT_OPRT_MODE_ONLINE:
    case PROT_OPRT_MODE_FTM:
    case PROT_OPRT_MODE_MAX:
    default:
      MMOC_ERR_FATAL("Incorrect oprt_mode", 0, 0, 0);
      break;

  } /* switch( mmoc_info_ptr->curr_oprt_mode) */ 



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Inform and terminate System Determination.
  */
  if ( mmoc_info_ptr->is_sd_initialized &&
       mmoc_info_ptr->curr_oprt_mode !=  PROT_OPRT_MODE_RESET )
  {
    (void)sd_ss_ind_user_pwr_down( NULL );
    mmoc_set_sd_term( mmoc_info_ptr );
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Inform TMC of the operating mode change.
  */
  if (mmoc_is_wait_reqd_on_cmd_tmc (mmoc_info_ptr))
  {
    return MMOC_EVT_STATUS_CONSUMED;
  }  

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Clear the transaction.
  */
  return mmoc_clear_transaction( mmoc_info_ptr );

} /* mmoc_process_pwr_down_ph_stat_enter() */
/*lint +esym(715,rpt_ptr)*/
/*lint +esym(715,cmd_ptr)*/


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_pwr_down

DESCRIPTION
  This function is the transaction handler for the power down transaction.

  The transaction state transition will be,

 NULL -> DEACT_ENTER -> WAIT_DEACTD_CNF -> PH_STAT_ENTER <-> WAIT_PH_STAT_CNF
              |                              ^    |
              |______________________________|    |-----> NULL


DEPENDENCIES
  None.


RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
static  mmoc_evt_status_e_type         mmoc_process_pwr_down
(

        const mmoc_cmd_msg_s_type            *cmd_ptr,
            /* Pointer to the command event that was received.
            ** if this was not called because of command event, then
            ** cmd_ptr = null.
            */

        const mmoc_rpt_msg_s_type            *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{
  mmoc_evt_status_e_type  evt_status = MMOC_EVT_STATUS_CONSUMED;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Invoke the appropriate transaction state handler.
  */
  switch(mmoc_info_ptr->trans_state)
  {
    case MMOC_TRANS_STATE_NULL:
      evt_status = mmoc_process_pwr_down_null( cmd_ptr,
                                               rpt_ptr,
                                               mmoc_info_ptr
                                             );
      break;

    case MMOC_TRANS_STATE_PROT_DEACT_ENTER:
      evt_status =  mmoc_process_pwr_down_deact_enter( cmd_ptr,
                                                       rpt_ptr,
                                                       mmoc_info_ptr
                                                     );
      break;

    case MMOC_TRANS_STATE_WAIT_DEACTD_CNF:
      evt_status = mmoc_process_common_deactd_cnf( rpt_ptr,
                                                   mmoc_info_ptr
                                                 );
      break;

    case MMOC_TRANS_STATE_PROT_PH_STAT_ENTER:
      evt_status = mmoc_process_pwr_down_ph_stat_enter( cmd_ptr,
                                                        rpt_ptr,
                                                        mmoc_info_ptr
                                                      );
      break;

    case MMOC_TRANS_STATE_WAIT_PH_STAT_CNF:
      evt_status = mmoc_process_common_ph_stat_cnf( rpt_ptr,
                                                    mmoc_info_ptr
                                                  );
      break;


    case MMOC_TRANS_STATE_WAIT_TMC_MODE_CNF:
      evt_status = mmoc_process_common_tmc_mode_cnf( rpt_ptr,
                                                     mmoc_info_ptr
                                                   ); 
      break;

    case MMOC_TRANS_STATE_WAIT_GEN_CMD_CNF:
    case MMOC_TRANS_STATE_GEN_CMD_ENTER:
    case MMOC_TRANS_STATE_WAIT_AUTO_DEACTD_IND:
    case MMOC_TRANS_STATE_HDR_DEACT_ENTER:
    case MMOC_TRANS_STATE_WAIT_HDR_DEACTD_CNF:
    case MMOC_TRANS_STATE_MAX:
    default:
      MMOC_MSG_ERROR("incorrect state", mmoc_info_ptr->trans_state, 0, 0);
      evt_status = mmoc_clear_transaction( mmoc_info_ptr );
      break;
  }


  return evt_status;

} /* mmoc_process_pwr_down() */

/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
==================== GENERIC PROTOCOL COMMAND TRANSACTION ===================
=============================================================================
=============================================================================
===========================================================================*/

/*===========================================================================

FUNCTION mmoc_process_prot_gen_cmd_null

DESCRIPTION
  This function handles the generic protocol command transaction in the NULL
  transaction state.

  In this state, the state handler will
  1. Send the generic command to  the active protocol if one is present.
     Else, if no protocol is active will call the mmoc_proc_prot_gen_cmd()
     and follow the return SD action.

DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
/*lint -esym(715,rpt_ptr)*/
static  mmoc_evt_status_e_type         mmoc_process_prot_gen_cmd_null
(

        mmoc_cmd_msg_s_type            *cmd_ptr,
            /* Pointer to the command event that was received.
            ** if this was not called because of command event, then
            ** cmd_ptr = null.
            */

        const mmoc_rpt_msg_s_type            *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{

  MMOC_ASSERT( cmd_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* The generic protocol command should only create this transaction.
  */
  if ( cmd_ptr == NULL ||
       cmd_ptr->cmd.name != MMOC_CMD_PROT_GEN_CMD
     )
  {
    MMOC_MSG_ERROR("incorrect cmd", 0,0,0);

    return mmoc_clear_transaction( mmoc_info_ptr );
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Check if phone is online.
  */
  if ( mmoc_info_ptr->curr_oprt_mode != PROT_OPRT_MODE_ONLINE )
  {
    /* Phone not ONLINE, ignore the generic command.
    */
    MMOC_MSG_ERROR("incorrect phone state", 0, 0, 0);
    return mmoc_clear_transaction( mmoc_info_ptr );
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (cmd_ptr->param.gen_prot_cmd.ss >= SD_SS_MAX)
  {
    /* Phone not ONLINE, ignore the generic command.
    */
    MMOC_MSG_ERROR("incorrect ss in cmd", 0, 0, 0);
    return mmoc_clear_transaction( mmoc_info_ptr );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the protocol state field and transaction info.
  */
  cmd_ptr->param.gen_prot_cmd.prot_state =
                 mmoc_info_ptr->prot_state[ cmd_ptr->param.gen_prot_cmd.ss ];
  mmoc_info_ptr->trans_info.gen_prot_cmd.cmd_info =
                 cmd_ptr->param.gen_prot_cmd;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Move the trans. state to send the Generic command to the active
  ** protocol.
  */
  mmoc_update_trans_state( MMOC_TRANS_STATE_GEN_CMD_ENTER,
                           mmoc_info_ptr
                         );


  return MMOC_EVT_STATUS_NOT_CONSUMED;

} /* mmoc_process_prot_gen_cmd_null() */
/*lint +esym(715,rpt_ptr)*/


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_prot_gen_cmd_enter

DESCRIPTION
  This function is responsible for the sending the Generic command to the
  active protocol, if none is present calls into SD directly.

DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
/*lint -esym(715,rpt_ptr)*/
/*lint -esym(715,cmd_ptr)*/
static  mmoc_evt_status_e_type         mmoc_process_prot_gen_cmd_enter
(

        const mmoc_cmd_msg_s_type            *cmd_ptr,
            /* Pointer to the command event that was received.
            ** if this was not called because of command event, then
            ** cmd_ptr = null.
            */

        const mmoc_rpt_msg_s_type            *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{

  MMOC_ASSERT( (cmd_ptr != NULL ) || (rpt_ptr != NULL) );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Send the generic command to the active protocol, if there is one.
  */
  if ( mmoc_send_prot_gen_cmd( &mmoc_info_ptr->trans_info.gen_prot_cmd.cmd_info,
                               mmoc_info_ptr
                             )
     )
  {
    /* Generic command sent, wait for acknowledgement.
    */
    mmoc_update_trans_state( MMOC_TRANS_STATE_WAIT_GEN_CMD_CNF,
                             mmoc_info_ptr
                           );
  }
  else
  {

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    /* Process the generic command in MMoC itself.
    ** Set the trans_id to NONE, since no generic command ACK is required.
    */
    mmoc_info_ptr->trans_info.gen_prot_cmd.cmd_info.trans_id =
             PROT_TRANS_NONE;
    (void) mmoc_proc_prot_gen_cmd( TRUE,
                                   TRUE,
                                   &mmoc_info_ptr->trans_info.gen_prot_cmd.cmd_info
                                 );


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    
    
    /* Activate the protocol or invoke
    ** transaction handler if this function returns EVT_STATUS_NOT_CONSUMED.
    */
    if ( mmoc_activate_prot_online( mmoc_info_ptr->trans_info.gen_prot_cmd.cmd_info.ss,
                                    mmoc_info_ptr
                                  )
         == MMOC_EVT_STATUS_NOT_CONSUMED
       )
    {
      /* Invoke the new transaction handler.
      */
      return MMOC_EVT_STATUS_NOT_CONSUMED;

    }
  
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    /* Done with processing, clear the transaction.
    */
    return mmoc_clear_transaction( mmoc_info_ptr );
  }


  return MMOC_EVT_STATUS_CONSUMED;

} /* mmoc_process_prot_gen_cmd_enter() */
/*lint +esym(715,rpt_ptr)*/
/*lint +esym(715,cmd_ptr)*/



/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_prot_gen_cmd_cnf

DESCRIPTION
  This function handles the generic protocol command transaction in the
  WAIT_GEN_CMD_CNF transaction state.

  In this state, the state handler will
  1. Validate the Ack received. Done with the transaction if ack is correct
     and ack says that the generic command was processed. If the ack says
     that generic command was not processed, it is race condition wherein
     the MMoC recvd the deactivated indication while waiting for the ack.

  Note! Race condition, if a protocol deactivated indication is received
  while waiting for ACK, the handler will activate the AMPS protocol only
  if it is CDMA to AMPS handoff case. Else the deactivated indication
  report will be dropped.

DEPENDENCIES
  Non

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
/*lint -esym(715,cmd_ptr)*/
static  mmoc_evt_status_e_type         mmoc_process_prot_gen_cmd_cnf
(

        const mmoc_cmd_msg_s_type            *cmd_ptr,
            /* Pointer to the command event that was received.
            ** if this was not called because of command event, then
            ** cmd_ptr = null.
            */

        const mmoc_rpt_msg_s_type            *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{
  MMOC_ASSERT( rpt_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Check if a report is received.
  */
  if ( rpt_ptr == NULL )
  {
    /* Not expecting a command.
    */
    MMOC_MSG_ERROR("Command unexpected", 0, 0, 0);
    return MMOC_EVT_STATUS_CONSUMED;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Process the received report.
  */
  switch (rpt_ptr->rpt.name )
  {
    case MMOC_RPT_PROT_AUTO_DEACTD_IND:
      /* Autonomous deactivated report received.
      */
      MMOC_MSG_LOW("Received autonomous deactivated report", 0, 0, 0);


      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


      /* Cannot drop the CDMA->AMPS handoff case, process it.
      */
      if (rpt_ptr->param.prot_deactd_ind.reason ==
                                               PROT_DEACT_CDMA_AMPS_HANDOFF)
      {
        MMOC_ASSERT( mmoc_info_ptr->prot_state[SD_SS_MAIN] ==
                                               PROT_STATE_ONLINE_CDMA );

        /* It is from CDMA protocol stack, activate the AMPS protocol for
        ** handoff.
        */
        mmoc_activate_prot_cdma_amps_ho( mmoc_info_ptr );
      }
      else
      {
         #ifdef MMOC_HYBR_SUPPORTED
#error code not present
         #endif /* MMOC_HYBR_SUPPORTED */

         #ifdef MMOC_HYBR_GW_SUPPORTED
         /* Check if the deactivation was from SS_HDR system selection instance.
         */
         if ( rpt_ptr->param.prot_deactd_ind.prot_state  == PROT_STATE_ONLINE_GW &&
              mmoc_info_ptr->prot_state[SD_SS_HYBR_1] == PROT_STATE_ONLINE_GW )
         {

           /* If hybrid preference is not ON then hybrid hdr instance should
           ** not be triggered to go ONLINE
           */
           if (sd_misc_is_hybr_opr())
           {

             /* It is for SS_HDR instance, activate SS_HDR and return.
             */
             (void)mmoc_activate_prot_online_ss_hdr( sd_ss_hybr_1_act_get(NULL),
                                                     mmoc_info_ptr
                                                   );
           }
           else
           {

             MMOC_MSG_ERROR ("trans_state WAIT_GEN_CMD_CNF, AUTO_DEACTD_IND from HYBR_HDR", 0, 0, 0);

             mmoc_update_prot_state( PROT_STATE_NULL,
                                     SD_SS_HYBR_HDR,
                                     mmoc_info_ptr
                                   );

           }

           return MMOC_EVT_STATUS_CONSUMED;
         }
         #endif


         #ifdef MMOC_WLAN_SUPPORTED
#error code not present
         #endif /* MMOC_WLAN_SUPPORTED */


        /*  Activate main at the end of transaction.
        */
        mmoc_info_ptr->trans_info.gen_prot_cmd.is_activate_main = TRUE;
        mmoc_update_prot_state( PROT_STATE_NULL,
                                SD_SS_MAIN,
                                mmoc_info_ptr
                              );

      }

      /* Wait for the acknowledgement for generic command.
      */
      return MMOC_EVT_STATUS_CONSUMED;

    case MMOC_RPT_PROT_GEN_CMD_CNF:

      /* Check if it is the right acknowledgement.
      */
      if ( mmoc_info_ptr->trans_id != rpt_ptr->param.gen_cmd_cnf.trans_id )
      {
        /* Not the expected transaction Id.
        */
        MMOC_MSG_LOW("Received incorrect trans_id for Gen cmd ack",
                     0, 0, 0
                    );
        return MMOC_EVT_STATUS_CONSUMED;
      }
      break;

    case MMOC_RPT_PROT_DEACTD_CNF:
    case MMOC_RPT_PH_STAT_CHGD_CNF:
    case MMOC_RPT_TMC_MODE_CNF:
    case MMOC_RPT_MAX:   
    default:
      MMOC_MSG_ERROR(" Received unexpected report %d", rpt_ptr->rpt.name, 0, 0);

      return MMOC_EVT_STATUS_CONSUMED;
  } /* switch (rpt_ptr->rpt.name ) */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Check if the generic command was processed by the protocols.
  */
  if ( !rpt_ptr->param.gen_cmd_cnf.is_cmd_processed )
  {
    /* Race condition: Protocol was deactivated just before the generic
    ** command was posted, either send generic command or process it in
    ** MMoC itself.
    ** Also, reset the need to activate main.
    */
    mmoc_update_trans_state( MMOC_TRANS_STATE_GEN_CMD_ENTER,
                             mmoc_info_ptr
                           );
    mmoc_info_ptr->trans_info.gen_prot_cmd.is_activate_main = FALSE;
    return MMOC_EVT_STATUS_NOT_CONSUMED;

  } /* if ( !rpt_ptr->param.gen_cmd_cnf.is_cmd_processed ) */



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Check if the protocol is deactivating.
  */
  if ( rpt_ptr->param.gen_cmd_cnf.is_prot_deactivating )
  {
     /* Wait for deactivated indication from protocol.
     */
     mmoc_update_trans_state( MMOC_TRANS_STATE_WAIT_AUTO_DEACTD_IND,
                              mmoc_info_ptr
                            );
     return MMOC_EVT_STATUS_CONSUMED;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Generic command was processed, end the transaction.
  */
  return mmoc_clear_transaction(mmoc_info_ptr);


} /* mmoc_process_prot_gen_cmd_cnf() */
/*lint +esym(715,cmd_ptr)*/

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_prot_gen_cmd_auto_deactd_ind

DESCRIPTION
  This function handles the generic protocol command transaction in the
  WAIT_AUTO_DEATD_IND transaction state.

  In this state, the state handler will


DEPENDENCIES
  Non

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
/*lint -esym(715,cmd_ptr)*/
static  mmoc_evt_status_e_type         mmoc_process_prot_gen_cmd_auto_deactd_ind
(

        const mmoc_cmd_msg_s_type            *cmd_ptr,
            /* Pointer to the command event that was received.
            ** if this was not called because of command event, then
            ** cmd_ptr = null.
            */

        const mmoc_rpt_msg_s_type            *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{
  MMOC_ASSERT( rpt_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Check if we got the autonomous deactivated report.
  */
  if (rpt_ptr == NULL)   
  {
    /* Unexpected report received.
    */
    MMOC_MSG_ERROR("unexpected report ", 0, 0, 0);
    return MMOC_EVT_STATUS_CONSUMED;

  }

  if (rpt_ptr->rpt.name  != MMOC_RPT_PROT_AUTO_DEACTD_IND )    
  {
    /* Unexpected report received.
    */
    MMOC_MSG_ERROR("unexpected report %d", rpt_ptr->rpt.name, 0, 0);
    return MMOC_EVT_STATUS_CONSUMED;

  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Process deactd report based on Gen. cmd to SS instance.
  */
  switch ( mmoc_info_ptr->trans_info.gen_prot_cmd.cmd_info.ss)
  {
    case SD_SS_HYBR_1:
      #ifdef MMOC_HYBR_SUPPORTED
#error code not present
      #endif /* MMOC_HYBR_SUPPORTED */

      #ifdef MMOC_HYBR_GW_SUPPORTED
      if ( rpt_ptr->param.prot_deactd_ind.prot_state == PROT_STATE_ONLINE_GW &&
           mmoc_info_ptr->prot_state[SD_SS_HYBR_1] == PROT_STATE_ONLINE_GW )
      {
        /* If hybrid preference is not ON then hybrid 1 instance should
        ** not be triggered to go ONLINE
        */
        if (sd_misc_is_hybr_opr())
        {
          /* It is for SS_HDR instance, activate SS_HDR and return.
          */
          (void)mmoc_activate_prot_online_ss_hdr( sd_ss_hybr_1_act_get(NULL),
                                                  mmoc_info_ptr
                                                );
        }
        else
        {

          MMOC_MSG_HIGH (" Hybr pref is OFF, AUTO_DEACTD_IND for HYBR_1(GW)", 0, 0, 0);

          mmoc_update_prot_state( PROT_STATE_NULL,
                                  SD_SS_HYBR_1,
                                  mmoc_info_ptr
                                );

        }

        break;
      }
      #endif

      #ifdef MMOC_WLAN_SUPPORTED
#error code not present
      #endif /* MMOC_WLAN_SUPPORTED */

      /* Check if SS_MAIN got deactivated.
      */
      if ( rpt_ptr->param.prot_deactd_ind.prot_state !=
                                      mmoc_info_ptr->prot_state[SD_SS_MAIN] )
      {
        /* Bogus report.
        */
        MMOC_MSG_ERROR("Invalid report", 0, 0, 0 );
        return MMOC_EVT_STATUS_CONSUMED;
      }
      else
      {
        /* SS_MAIN got deactivated.
        */
        mmoc_info_ptr->trans_info.gen_prot_cmd.is_activate_main = TRUE;
        mmoc_update_prot_state( PROT_STATE_NULL,
                                SD_SS_MAIN,
                                mmoc_info_ptr
                               );
        return MMOC_EVT_STATUS_CONSUMED;
      }

    case SD_SS_HYBR_WLAN:
      #ifdef MMOC_WLAN_SUPPORTED
#error code not present
      #endif /* MMOC_WLAN_SUPPORTED */


      #ifdef MMOC_HYBR_SUPPORTED
#error code not present
      #endif /* MMOC_HYBR_SUPPORTED */

      /* Check if SS_MAIN got deactivated.
      */
      if ( rpt_ptr->param.prot_deactd_ind.prot_state !=
                                      mmoc_info_ptr->prot_state[SD_SS_MAIN] )
      {
        /* Bogus report.
        */
        MMOC_MSG_ERROR("Invalid report", 0, 0, 0 );
        return MMOC_EVT_STATUS_CONSUMED;
      }
      else
      {
        /* SS_MAIN got deactivated.
        */
        mmoc_info_ptr->trans_info.gen_prot_cmd.is_activate_main = TRUE;
        mmoc_update_prot_state( PROT_STATE_NULL,
                                SD_SS_MAIN,
                                mmoc_info_ptr
                               );
        return MMOC_EVT_STATUS_CONSUMED;
      }

    case SD_SS_MAIN:
      #ifdef MMOC_HYBR_SUPPORTED
#error code not present
      #endif /* MMOC_HYBR_SUPPORTED */


      #ifdef MMOC_WLAN_SUPPORTED
#error code not present
      #endif /* MMOC_WLAN_SUPPORTED */


      /* Check if SS_MAIN got deactivated.
      */
      if ( rpt_ptr->param.prot_deactd_ind.prot_state !=
                                      mmoc_info_ptr->prot_state[SD_SS_MAIN] )
      {
        /* Bogus report.
        */
        MMOC_MSG_ERROR("Invalid report", 0, 0, 0 );
        return MMOC_EVT_STATUS_CONSUMED;
      }

      /* SS_MAIN protocol got deactivated.
      */
      mmoc_info_ptr->trans_info.gen_prot_cmd.is_activate_main = TRUE;
      mmoc_update_prot_state( PROT_STATE_NULL,
                              SD_SS_MAIN,
                              mmoc_info_ptr
                            );
      break;

    case SD_SS_MAX:
    default:
       MMOC_ERR_FATAL("incorrect SS instance", 0, 0, 0 );
       break;

  } /*  switch ( mmoc_info_ptr->trans_info.gen_prot_cmd.cmd_info.ss) */
 

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* End transaction.
  */
  return mmoc_clear_transaction(mmoc_info_ptr);

} /* mmoc_process_prot_gen_cmd_auto_deactd_ind() */
/*lint +esym(715,cmd_ptr)*/


/*===========================================================================

FUNCTION mmoc_postprocess_prot_gen_cmd

DESCRIPTION
  This function handles the generic protocol command transaction cleanup
  function.

DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
static  mmoc_evt_status_e_type         mmoc_postprocess_prot_gen_cmd
(

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Check if MAIN has to be activated.
  */
  if ( mmoc_info_ptr->trans_info.gen_prot_cmd.is_activate_main )
  {

    /* Since main is being kicked online turn the flag OFF
    */
    mmoc_info_ptr->trans_info.gen_prot_cmd.is_activate_main = FALSE;

    /* MAIN needs to be activated.
    */
    return mmoc_activate_prot_online_ss_main( sd_ss_act_get( NULL ),
                                         mmoc_info_ptr
                                            );
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Return event consumed.
  */
  return MMOC_EVT_STATUS_CONSUMED;

} /* mmoc_postprocess_prot_gen_cmd() */


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_prot_gen_cmd

DESCRIPTION
  This function is the transaction handler for the power down transaction.

  The transaction state transition will be,

     NULL <-> GEN_CMD_ENTER -> WAIT_GEN_CMD_CNF  -> NULL
                 ^                 |
                 |                 |
                 ------------------

DEPENDENCIES
  None.

DEPENDENCIES

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
static  mmoc_evt_status_e_type         mmoc_process_prot_gen_cmd
(

        mmoc_cmd_msg_s_type            *cmd_ptr,
            /* Pointer to the command event that was received.
            ** if this was not called because of command event, then
            ** cmd_ptr = null.
            */

        const mmoc_rpt_msg_s_type            *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{
  mmoc_evt_status_e_type  evt_status = MMOC_EVT_STATUS_CONSUMED;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Invoke the appropriate the transaction state handler.
  */
  switch(mmoc_info_ptr->trans_state)
  {
    case MMOC_TRANS_STATE_NULL:
      evt_status = mmoc_process_prot_gen_cmd_null( cmd_ptr,
                                                   rpt_ptr,
                                                   mmoc_info_ptr
                                                 );
      break;

    case MMOC_TRANS_STATE_GEN_CMD_ENTER:
      evt_status = mmoc_process_prot_gen_cmd_enter( cmd_ptr,
                                                    rpt_ptr,
                                                    mmoc_info_ptr
                                                  );
      break;

    case MMOC_TRANS_STATE_WAIT_GEN_CMD_CNF:
      evt_status = mmoc_process_prot_gen_cmd_cnf( cmd_ptr,
                                                  rpt_ptr,
                                                  mmoc_info_ptr
                                                );
      break;


    case MMOC_TRANS_STATE_WAIT_AUTO_DEACTD_IND:
      evt_status = mmoc_process_prot_gen_cmd_auto_deactd_ind( cmd_ptr,
                                                              rpt_ptr,
                                                              mmoc_info_ptr
                                                            );
      break;


    case MMOC_TRANS_STATE_HDR_DEACT_ENTER:
      evt_status = mmoc_process_common_hdr_deact_enter  
                                                     ( PROT_DEACT_ACQ_OTHERS,                                                         
                                                       mmoc_info_ptr
                                                     );
      break;



    case MMOC_TRANS_STATE_WAIT_HDR_DEACTD_CNF:
      evt_status = mmoc_process_common_hdr_deactd_cnf ( cmd_ptr,
                                                        rpt_ptr,
                                                        mmoc_info_ptr
                                                      );
      break;


    case MMOC_TRANS_STATE_WAIT_DEACTD_CNF:
    case MMOC_TRANS_STATE_WAIT_PH_STAT_CNF:
    case MMOC_TRANS_STATE_PROT_DEACT_ENTER:
    case MMOC_TRANS_STATE_PROT_PH_STAT_ENTER:
    case MMOC_TRANS_STATE_MAX:
    default:
      MMOC_MSG_ERROR("Received event in unknown state", 0, 0, 0);
      evt_status = mmoc_clear_transaction( mmoc_info_ptr );
      break;
  }


  return evt_status;

} /* mmoc_process_gen_cmd() */


/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
==================== AUTONOMOUS PROTOCOL DEACTIVATED TRANSACTION ============
=============================================================================
=============================================================================
===========================================================================*/


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_auto_deactd_ind_null

DESCRIPTION
  This function handles the autonomous protocol deactivated report
  transaction in the NULL transaction state.

  In this state, the state handler will
  1. based on the deactivated reason either call into SD to determine the
     next protocol to be activated or if the deactivated reason is CDMA to
     AMPS handoff, it will activate the AMPS protocol.


DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
/*lint -esym(715,cmd_ptr)*/
static  mmoc_evt_status_e_type         mmoc_process_auto_deactd_ind_null
(

        const mmoc_cmd_msg_s_type            *cmd_ptr,
            /* Pointer to the command event that was received.
            ** if this was not called because of command event, then
            ** cmd_ptr = null.
            */

        const mmoc_rpt_msg_s_type            *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{
  sd_ss_e_type   ss = SD_SS_MAIN;

  MMOC_ASSERT( rpt_ptr != NULL);


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* The deactivated report  should only create this transaction.
  */
  if ( rpt_ptr == NULL ||
       rpt_ptr->rpt.name != MMOC_RPT_PROT_AUTO_DEACTD_IND
     )
  {
    MMOC_MSG_ERROR("Autonomous deactivated transaction created incorrectly",
                   0,0,0);

    return mmoc_clear_transaction( mmoc_info_ptr );
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Check if phone is online.
  */
  if ( mmoc_info_ptr->curr_oprt_mode != PROT_OPRT_MODE_ONLINE )
  {
    MMOC_ERR_FATAL("Phone should be in online mode", 0, 0, 0);
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if the deactivation was from SS_HDR system selection instance.
  */
  if ( rpt_ptr->param.prot_deactd_ind.prot_state  == PROT_STATE_ONLINE_HDR &&
       mmoc_info_ptr->prot_state[SD_SS_HYBR_HDR] == PROT_STATE_ONLINE_HDR)
  {

    ss = SD_SS_HYBR_HDR;

  }

  #ifdef MMOC_HYBR_GW_SUPPORTED
  if ( rpt_ptr->param.prot_deactd_ind.prot_state  == PROT_STATE_ONLINE_GW &&
       mmoc_info_ptr->prot_state[SD_SS_HYBR_1] == PROT_STATE_ONLINE_GW)
  {

    ss = SD_SS_HYBR_1;

  }
  #endif


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef MMOC_WLAN_SUPPORTED
#error code not present
  #endif /* MMOC_WLAN_SUPPORTED */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Initialize the trans info.
  */
  switch (rpt_ptr->param.prot_deactd_ind.reason)  
  {
    case PROT_DEACT_CDMA_AMPS_HANDOFF:
      MMOC_ASSERT( mmoc_info_ptr->prot_state[SD_SS_MAIN] ==
                                         PROT_STATE_ONLINE_CDMA);
      MMOC_ASSERT( ss == SD_SS_MAIN );
      mmoc_activate_prot_cdma_amps_ho( mmoc_info_ptr );
      break;

    case PROT_DEACT_ACQ_OTHERS:


      /* If hybrid preference is NONE and autonomous deact  indication
      ** is received from protocol for HDR instance, update the state to
      ** NULL and do not try to acquire on Hybrid instance.
      */
      if ((ss == SD_SS_HYBR_HDR) &&  (!sd_misc_is_hybr_opr()))
      {

        MMOC_MSG_ERROR(" Err: Unexp state, hybrid is OFF, MMOC is TRANS_STATE_NULL", 0, 0, 0);

        mmoc_update_prot_state( PROT_STATE_NULL,
                                SD_SS_HYBR_HDR,
                                mmoc_info_ptr
                              );


        break;
      }
      
      /* Activate protocol on ss instance
      */
      if ( mmoc_activate_prot_online( ss, mmoc_info_ptr ) ==
                                           MMOC_EVT_STATUS_NOT_CONSUMED )
      {
        /* Invoke new transaction.
        */
        return MMOC_EVT_STATUS_NOT_CONSUMED;
      }
      
      break;

    case PROT_DEACT_PWR_DOWN:
    case PROT_DEACT_RESET:
    case PROT_DEACT_LPM:
    case PROT_DEACT_OFFLINE:
    case PROT_DEACT_SUBSC_CHGD:
    default:
      MMOC_MSG_ERROR( "deact reason %d",
                      rpt_ptr->param.prot_deactd_ind.reason, 0, 0
                    );
      break;
  }  /* switch (rpt_ptr->param.prot_auto_deactd_ind.reason) */   


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Transaction completed.
  */
  return mmoc_clear_transaction( mmoc_info_ptr );


} /* mmoc_process_auto_deactd_ind_null() */
/*lint +esym(715,cmd_ptr)*/


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_auto_deactd_ind

DESCRIPTION
  This function is the transaction handler for the autonomous protocol
  deactivated indication transaction.

  The transaction state transition will be,

     NULL -> NULL

DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
static  mmoc_evt_status_e_type         mmoc_process_auto_deactd_ind
(

        const mmoc_cmd_msg_s_type            *cmd_ptr,
            /* Pointer to the command event that was received.
            ** if this was not called because of command event, then
            ** cmd_ptr = null.
            */

        const mmoc_rpt_msg_s_type            *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{
  mmoc_evt_status_e_type evt_status = MMOC_EVT_STATUS_CONSUMED;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Invoke the appropriate transaction state handler.
  */
  switch(mmoc_info_ptr->trans_state)
  {
    case MMOC_TRANS_STATE_NULL:
      evt_status = mmoc_process_auto_deactd_ind_null( cmd_ptr,
                                                 rpt_ptr,
                                                 mmoc_info_ptr
                                               );
      break;    

    case MMOC_TRANS_STATE_HDR_DEACT_ENTER:
      evt_status = mmoc_process_common_hdr_deact_enter  
                                                     ( PROT_DEACT_ACQ_OTHERS,                                                         
                                                       mmoc_info_ptr
                                                     );
      break;

    case MMOC_TRANS_STATE_WAIT_HDR_DEACTD_CNF:
      evt_status = mmoc_process_common_hdr_deactd_cnf   ( cmd_ptr,
                                                          rpt_ptr,
                                                          mmoc_info_ptr
                                                        );
      break;

    case MMOC_TRANS_STATE_WAIT_DEACTD_CNF:
    case MMOC_TRANS_STATE_WAIT_PH_STAT_CNF:
    case MMOC_TRANS_STATE_WAIT_GEN_CMD_CNF:
    case MMOC_TRANS_STATE_GEN_CMD_ENTER:
    case MMOC_TRANS_STATE_PROT_DEACT_ENTER:
    case MMOC_TRANS_STATE_PROT_PH_STAT_ENTER:
    case MMOC_TRANS_STATE_MAX:
    default:
      MMOC_MSG_ERROR("Received event in unknown state", 0, 0, 0);
      evt_status = mmoc_clear_transaction( mmoc_info_ptr );
      break;
  }


  return evt_status;

} /* mmoc_process_auto_deactd_ind() */

/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
==================== POWER SAVE ENTER TRANSACTION ===========================
=============================================================================
=============================================================================
===========================================================================*/

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_pwr_save_enter_null

DESCRIPTION
  This function handles the power save transaction in the NULL transaction
  state.

  In this state, the state handler will
  1. Check if GPIO allows phone to go in power save mode, if so informs
     SD of power save enter and calls into SRCH or Sleep Controller.
     to send the phone to power save mode.
  2. If power save mode is not allowed, follows SD actions and activates the
     next protocol.

DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
/*lint -esym(715,cmd_ptr)*/
/*lint -esym(715,rpt_ptr)*/
static  mmoc_evt_status_e_type         mmoc_process_pwr_save_enter_null
(

        const mmoc_cmd_msg_s_type            *cmd_ptr,
            /* Pointer to the command event that was received.
            ** if this was not called because of command event, then
            ** cmd_ptr = null.
            */

        const mmoc_rpt_msg_s_type            *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Phone should be in ONLINE mode and SD action should be power save.
  */
  if ( (mmoc_info_ptr->curr_oprt_mode != PROT_OPRT_MODE_ONLINE) &&
       (sd_ss_act_get( NULL ) !=  SD_SS_ACT_PWR_SAVE )
     )
  {
    MMOC_ERR_FATAL("Phone should be in online mode", 0, 0, 0);
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  #ifndef FEATURE_MMOC_UNIT_TEST
  /* Check on GPIO to see if power save mode is allowed.
  */
  if ( 
      #ifndef FEATURE_DISABLE_DEEP_SLEEP
      BIO_GET_SW(BIO_SW9_M)  &&
      #endif
       sd_ss_ind_user_pwr_save_exit( NULL ) != SD_SS_ACT_PWR_SAVE
     )

  {
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Power save mode is not allowed and SD can activate a new protocol.
    ** Aborted power save enter, activate the protocol or invoke
    ** transaction handler if this function returns EVT_STATUS_NOT_CONSUMED.
    */
    if ( mmoc_activate_prot_online_ss_main( sd_ss_act_get(NULL),
                                            mmoc_info_ptr
                                          )
         == MMOC_EVT_STATUS_NOT_CONSUMED
       )
    {
      /* Invoke the new transaction handler.
      */
      return MMOC_EVT_STATUS_NOT_CONSUMED;

    }


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    /* Clear the transaction and return
    */
    return mmoc_clear_transaction( mmoc_info_ptr );

  } /* if (BIO_GET_SW(BIO_SW9_M)) */
  #endif /* #if (TG != T_PC) */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/





  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Update the MMoC's protocol state and clear the transaction.
  */
  mmoc_update_prot_state( PROT_STATE_PWR_SAVE,
                          SD_SS_MAIN,
                          mmoc_info_ptr
                        );

  return mmoc_clear_transaction( mmoc_info_ptr );


} /* mmoc_process_pwr_save_enter_null() */
/*lint +esym(715,cmd_ptr)*/
/*lint +esym(715,rpt_ptr)*/


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_pwr_save_enter

DESCRIPTION
  This function is the transaction handler for the power save enter
  transaction.

  The transaction state transition will be,

     NULL -> NULL

DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
static  mmoc_evt_status_e_type         mmoc_process_pwr_save_enter
(

        const mmoc_cmd_msg_s_type            *cmd_ptr,
            /* Pointer to the command event that was received.
            ** if this was not called because of command event, then
            ** cmd_ptr = null.
            */

        const mmoc_rpt_msg_s_type            *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{
  mmoc_evt_status_e_type evt_status = MMOC_EVT_STATUS_CONSUMED;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Invoke the current transaction state handler.
  */
  switch(mmoc_info_ptr->trans_state)
  {
    case MMOC_TRANS_STATE_NULL:
      evt_status = mmoc_process_pwr_save_enter_null( cmd_ptr,
                                                     rpt_ptr,
                                                     mmoc_info_ptr
                                                   );
      break;

    case MMOC_TRANS_STATE_HDR_DEACT_ENTER:
      evt_status = mmoc_process_common_hdr_deact_enter  
                                                     ( PROT_DEACT_ACQ_OTHERS,                                                         
                                                       mmoc_info_ptr
                                                     );
      break;



    case MMOC_TRANS_STATE_WAIT_HDR_DEACTD_CNF:
      evt_status = mmoc_process_common_hdr_deactd_cnf ( cmd_ptr,
                                                        rpt_ptr,
                                                        mmoc_info_ptr
                                                      );
      break;

    case MMOC_TRANS_STATE_WAIT_DEACTD_CNF:
    case MMOC_TRANS_STATE_WAIT_PH_STAT_CNF:
    case MMOC_TRANS_STATE_WAIT_GEN_CMD_CNF:
    case MMOC_TRANS_STATE_PROT_DEACT_ENTER:
    case MMOC_TRANS_STATE_PROT_PH_STAT_ENTER:
    case MMOC_TRANS_STATE_MAX:
    case MMOC_TRANS_STATE_GEN_CMD_ENTER:
    case MMOC_TRANS_STATE_WAIT_AUTO_DEACTD_IND:
    case MMOC_TRANS_STATE_WAIT_TMC_MODE_CNF:
    default:
      MMOC_MSG_ERROR("Received event in unknown state", 0, 0, 0);
      evt_status = mmoc_clear_transaction( mmoc_info_ptr );
      break;
  }


  return evt_status;

} /* mmoc_process_pwr_save_enter() */


/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
==================== POWER SAVE EXIT TRANSACTION ============================
=============================================================================
=============================================================================
===========================================================================*/

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_pwr_save_exit_null

DESCRIPTION
  This function handles the power save exit transaction in the NULL
  transaction state.

  In this state, the state handler will
  1. Inform SD of power save exit and follow the return SD action
     and activate the protocol.

DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
/*lint -esym(715,cmd_ptr)*/
/*lint -esym(715,rpt_ptr)*/
static  mmoc_evt_status_e_type         mmoc_process_pwr_save_exit_null
(

        const mmoc_cmd_msg_s_type            *cmd_ptr,
            /* Pointer to the command event that was received.
            ** if this was not called because of command event, then
            ** cmd_ptr = null.
            */

        const mmoc_rpt_msg_s_type            *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{
   /* Last SD action. */
   sd_ss_act_e_type  sd_act = sd_ss_act_get( NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Phone should be in ONLINE mode and SD action should be power save.
  */
  if ( mmoc_info_ptr->curr_oprt_mode != PROT_OPRT_MODE_ONLINE )
  {
    MMOC_ERR_FATAL("Phone should be in online mode", 0, 0, 0);
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Quit the pwr_save_exit transaction, if the next action is power_save.
  */
  if ( sd_act == SD_SS_ACT_PWR_SAVE )
  {
    /* The next action is power save, nothing to do.
    */
    (void)sd_ss_ind_misc_pwr_save_enter( NULL );
    MMOC_MSG_ERROR("Power save SD action in power_save_exit transaction",
                   0, 0, 0);
    return mmoc_clear_transaction( mmoc_info_ptr );
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Exit power save mode, activate the protocol or invoke
  ** transaction handler if this function returns EVT_STATUS_NOT_CONSUMED.
  */
  if ( mmoc_activate_prot_online( SD_SS_MAIN,
                                  mmoc_info_ptr
                                )
       == MMOC_EVT_STATUS_NOT_CONSUMED
     )
  {
    /* Invoke the new transaction handler.
    */
    return MMOC_EVT_STATUS_NOT_CONSUMED;

  }
  

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Clear the transaction.
  */
  return mmoc_clear_transaction( mmoc_info_ptr );


} /* mmoc_process_pwr_save_exit_null() */
/*lint +esym(715,cmd_ptr)*/
/*lint +esym(715,rpt_ptr)*/




/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_pwr_save_exit

DESCRIPTION
  This function is the transaction handler for the power save exit
  transaction.

  The transaction state transition will be,

     NULL -> NULL

DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
static  mmoc_evt_status_e_type         mmoc_process_pwr_save_exit
(

        const mmoc_cmd_msg_s_type            *cmd_ptr,
            /* Pointer to the command event that was received.
            ** if this was not called because of command event, then
            ** cmd_ptr = null.
            */

        const mmoc_rpt_msg_s_type            *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{
  mmoc_evt_status_e_type evt_status = MMOC_EVT_STATUS_CONSUMED;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Invoke the current transaction state handler.
  */
  switch(mmoc_info_ptr->trans_state)
  {
    case MMOC_TRANS_STATE_NULL:
      evt_status = mmoc_process_pwr_save_exit_null( cmd_ptr,
                                                    rpt_ptr,
                                                    mmoc_info_ptr
                                                  );
      break;

    case MMOC_TRANS_STATE_HDR_DEACT_ENTER:
      evt_status = mmoc_process_common_hdr_deact_enter  
                                                     ( PROT_DEACT_ACQ_OTHERS,                                                         
                                                       mmoc_info_ptr
                                                     );
      break;

    case MMOC_TRANS_STATE_WAIT_HDR_DEACTD_CNF:
      evt_status = mmoc_process_common_hdr_deactd_cnf   ( cmd_ptr,
                                                          rpt_ptr,
                                                          mmoc_info_ptr
                                                        );

      break;

    case MMOC_TRANS_STATE_WAIT_DEACTD_CNF:
    case MMOC_TRANS_STATE_WAIT_PH_STAT_CNF:
    case MMOC_TRANS_STATE_WAIT_GEN_CMD_CNF:
    case MMOC_TRANS_STATE_PROT_DEACT_ENTER:
    case MMOC_TRANS_STATE_PROT_PH_STAT_ENTER:
    case MMOC_TRANS_STATE_MAX:
    case MMOC_TRANS_STATE_GEN_CMD_ENTER:
    case MMOC_TRANS_STATE_WAIT_AUTO_DEACTD_IND:
    case MMOC_TRANS_STATE_WAIT_TMC_MODE_CNF:
    default:
      MMOC_MSG_ERROR("Received event in unknown state", 0, 0, 0);
      evt_status = mmoc_clear_transaction( mmoc_info_ptr );
      break;
  }


  return evt_status;

} /* mmoc_process_pwr_save_exit() */


/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
==================== WAKEUP FROM PWR_SAVE TRANSACTION =======================
=============================================================================
=============================================================================
===========================================================================*/

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_wakeup_pwr_save_null

DESCRIPTION
  This function handles the power save exit transaction in the NULL
  transaction state.

  In this state, the state handler will
  1. Inform SD of power save exit and follow the return SD action
     and activate the protocol.

DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
static  mmoc_evt_status_e_type         mmoc_process_wakeup_pwr_save_null
(

        const mmoc_cmd_msg_s_type            *cmd_ptr,
            /* Pointer to the command event that was received.
            ** if this was not called because of command event, then
            ** cmd_ptr = null.
            */


        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{

  MMOC_ASSERT( cmd_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(cmd_ptr->param.wakeup.ss >= SD_SS_MAX)
  {
    /* Transaction created incorrectly.
    */
    MMOC_MSG_ERROR("wrong trans", 0,0,0);
    return mmoc_clear_transaction( mmoc_info_ptr );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* pwr_save wakeup command from CM should only create this transaction &
  ** phone should already be in pwr_save
  */
  if ( cmd_ptr == NULL ||
       cmd_ptr->cmd.name != MMOC_CMD_WAKEUP_FROM_PWR_SAVE ||
       mmoc_info_ptr->prot_state[cmd_ptr->param.wakeup.ss] != PROT_STATE_PWR_SAVE
     )
  {
    /* Transaction created incorrectly.
    */
    MMOC_MSG_ERROR("pwr_save wakeup trans incorrectly initiated", 0,0,0);
    return mmoc_clear_transaction( mmoc_info_ptr );
  }



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* This is a power save exit command from CM, do power save exit.
  */
  switch ( cmd_ptr->param.wakeup.ss )
  {
    case SD_SS_MAIN:
      if ( sd_ss_ind_user_pwr_save_exit( NULL ) == SD_SS_ACT_PWR_SAVE )
      {
        /* Put SD back in power save mode.
        */
        (void)sd_ss_ind_misc_pwr_save_enter( NULL );

        /* Clear the transaction.
        */
        return mmoc_clear_transaction( mmoc_info_ptr );
      }
      break;

    case SD_SS_HYBR_1:
      #ifdef MMOC_HYBR_1_SUPPORTED
      if ( sd_ss_ind_hybr_1_user_pwr_save_exit( NULL ) == SD_SS_ACT_PWR_SAVE )
      {
        /* Put SD back in power save mode.
        */
        (void)sd_ss_ind_hybr_1_misc_pwr_save_enter( NULL );

        /* Clear the transaction.
        */
        return mmoc_clear_transaction( mmoc_info_ptr );
      }
      #endif
      break;


    case SD_SS_HYBR_WLAN:
      #ifdef MMOC_WLAN_SUPPORTED
#error code not present
      #endif
      break;

    case SD_SS_MAX:
    default:
      MMOC_MSG_ERROR("incorrect ss %d", cmd_ptr->param.wakeup.ss, 0, 0);
      return mmoc_clear_transaction( mmoc_info_ptr );

  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  
  /* Activate the protocol.
  */
  if ( mmoc_activate_prot_online( cmd_ptr->param.wakeup.ss, mmoc_info_ptr ) ==
                                    MMOC_EVT_STATUS_NOT_CONSUMED )
  {
    /* Invoke new transaction.
    */
    return MMOC_EVT_STATUS_NOT_CONSUMED;
  }
  

  /* Clear the transaction.
  */
  return mmoc_clear_transaction( mmoc_info_ptr );

} /* mmoc_process_wakeup_pwr_save_null() */




/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_wakeup_from_pwr_save

DESCRIPTION
  This function is the transaction handler for the wake up from pwr save
  transaction.

  The transaction state transition will be,

     NULL -> NULL

DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
static  mmoc_evt_status_e_type         mmoc_process_wakeup_from_pwr_save
(

        const mmoc_cmd_msg_s_type            *cmd_ptr,
            /* Pointer to the command event that was received.
            ** if this was not called because of command event, then
            ** cmd_ptr = null.
            */

        const mmoc_rpt_msg_s_type            *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{
  mmoc_evt_status_e_type evt_status = MMOC_EVT_STATUS_CONSUMED;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Invoke the current transaction state handler.
  */
  switch(mmoc_info_ptr->trans_state)
  {
    case MMOC_TRANS_STATE_NULL:
      evt_status = mmoc_process_wakeup_pwr_save_null( cmd_ptr,                                              
                                                      mmoc_info_ptr
                                                    );
      break;

   case MMOC_TRANS_STATE_HDR_DEACT_ENTER:
      evt_status = mmoc_process_common_hdr_deact_enter  
                                                     ( PROT_DEACT_ACQ_OTHERS,                                                         
                                                       mmoc_info_ptr
                                                     );
      break;


    case MMOC_TRANS_STATE_WAIT_HDR_DEACTD_CNF:
      evt_status = mmoc_process_common_hdr_deactd_cnf  ( cmd_ptr,
                                                         rpt_ptr,
                                                         mmoc_info_ptr
                                                       );
      break;


    case MMOC_TRANS_STATE_WAIT_DEACTD_CNF:
    case MMOC_TRANS_STATE_WAIT_PH_STAT_CNF:
    case MMOC_TRANS_STATE_WAIT_GEN_CMD_CNF:
    case MMOC_TRANS_STATE_PROT_DEACT_ENTER:
    case MMOC_TRANS_STATE_PROT_PH_STAT_ENTER:
    case MMOC_TRANS_STATE_MAX:
    case MMOC_TRANS_STATE_GEN_CMD_ENTER:
    case MMOC_TRANS_STATE_WAIT_AUTO_DEACTD_IND:
    case MMOC_TRANS_STATE_WAIT_TMC_MODE_CNF:
    default:
      MMOC_MSG_ERROR("Received event in unknown state", 0, 0, 0);
      evt_status = mmoc_clear_transaction( mmoc_info_ptr );
      break;
  }


  return evt_status;

} /* mmoc_process_wakeup_from_pwr_save() */


/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_setup_trans

DESCRIPTION
  The purpose of this function is to determine which transaction should be
  invoked in order to process the event. This will be based on the type of
  event received.

DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
static  mmoc_evt_status_e_type         mmoc_setup_trans
(

        const mmoc_cmd_msg_s_type            *cmd_ptr,
            /* Pointer to the command event that was received.
            ** if this was not called because of command event, then
            ** cmd_ptr = null.
            */

        const mmoc_rpt_msg_s_type            *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{
  mmoc_evt_status_e_type  evt_status = MMOC_EVT_STATUS_NOT_CONSUMED;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Set cleanup function to NULL.
  */
  mmoc_info_ptr->exit_fn_ptr         = NULL;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Check if the event is a command or report.
  */
  if ( cmd_ptr != NULL )
  {
    /* Event is a command.
    */
    switch (cmd_ptr->cmd.name)
    {
      case MMOC_CMD_OPRT_MODE_CHGD:
        {
          switch ( cmd_ptr->param.oprt_mode_chgd.oprt_mode)
          {
            case SYS_OPRT_MODE_OFFLINE:
            case SYS_OPRT_MODE_OFFLINE_CDMA:
            case SYS_OPRT_MODE_OFFLINE_AMPS:
            case SYS_OPRT_MODE_FTM:
              mmoc_update_curr_trans( MMOC_TRANS_OFFLINE,
                                      mmoc_info_ptr,
                                      cmd_ptr->param.oprt_mode_chgd.oprt_mode
                                    );
              break;

            case SYS_OPRT_MODE_RESET:
            case SYS_OPRT_MODE_RESET_MODEM:
            case SYS_OPRT_MODE_LPM:
            case SYS_OPRT_MODE_PWROFF:
              mmoc_update_curr_trans( MMOC_TRANS_PWR_DOWN,
                                      mmoc_info_ptr,
                                      cmd_ptr->param.oprt_mode_chgd.oprt_mode
                                    );
              break;

            case SYS_OPRT_MODE_ONLINE:
              mmoc_update_curr_trans( MMOC_TRANS_ONLINE,
                                      mmoc_info_ptr,
                                      cmd_ptr->param.oprt_mode_chgd.oprt_mode
                                    );
              break;

            default:
              MMOC_MSG_ERROR("Incorrect oprt_mode received %d",
                             cmd_ptr->param.oprt_mode_chgd.oprt_mode, 0, 0
                            );
              evt_status = MMOC_EVT_STATUS_CONSUMED;
              break;

          } /* switch( cmd_ptr->param.oprt_mode_chgd.oprt_mode) */
        }
        break;

      case MMOC_CMD_SUBSCRIPTION_CHGD:
        mmoc_update_curr_trans( MMOC_TRANS_SUBSC_CHGD,
                                mmoc_info_ptr,
                                SYS_OPRT_MODE_NONE
                              );
        break;

      case MMOC_CMD_PROT_GEN_CMD:
        mmoc_update_curr_trans( MMOC_TRANS_PROT_GEN_CMD,
                                mmoc_info_ptr,
                                SYS_OPRT_MODE_NONE
                              );
        mmoc_info_ptr->exit_fn_ptr = (mmoc_exit_fn_f_type *)mmoc_postprocess_prot_gen_cmd;
        mmoc_info_ptr->trans_info.gen_prot_cmd.is_activate_main = FALSE;
        break;

      case MMOC_CMD_WAKEUP_FROM_PWR_SAVE:
        mmoc_update_curr_trans( MMOC_TRANS_WAKEUP_FROM_PWR_SAVE,
                                mmoc_info_ptr,
                                SYS_OPRT_MODE_NONE
                              );
        break;

      default:
        MMOC_MSG_ERROR( "Received unknown command event %d",
                        cmd_ptr->cmd.name, 0, 0
                      );
        evt_status = MMOC_EVT_STATUS_CONSUMED;
        break;

    } /* switch ( cmd_ptr->cmd.name ) */

  } /* if ( cmd_ptr ) */

  else if ( rpt_ptr != NULL )
  {
    /* Event is a report.
    */
    switch ( rpt_ptr->rpt.name )
    {
      case MMOC_RPT_PROT_AUTO_DEACTD_IND:
        mmoc_update_curr_trans( MMOC_TRANS_PROT_AUTO_DEACTD_IND,
                                mmoc_info_ptr,
                                mmoc_info_ptr->true_oprt_mode
                              );
        break;

      case MMOC_RPT_PROT_DEACTD_CNF:
      case MMOC_RPT_PH_STAT_CHGD_CNF:
      case MMOC_RPT_PROT_GEN_CMD_CNF:
      case MMOC_RPT_TMC_MODE_CNF:
      default:
        MMOC_MSG_ERROR( "Unexpected report received %d",
                        rpt_ptr->rpt.name, 0, 0
                      );
        evt_status = MMOC_EVT_STATUS_CONSUMED;
        break;

    } /* switch ( rpt_ptr->rpt.name ) */

  } /* else if ( rpt_ptr ) */
  else
  {
    MMOC_ERR_FATAL("No event received", 0, 0, 0);
  } /* else */


  return evt_status;

} /* mmoc_setup_trans() */




/*===========================================================================

FUNCTION mmoc_call_trans_hndlr

DESCRIPTION
  This function will process the events( either command or report) until the
  event was processed to completion or consumed. This is done by calling the
  appropriate transaction handlers based on the current transaction.

DEPENDENCIES
  None.

RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           mmoc_call_trans_hndlr
(

        mmoc_cmd_msg_s_type            *cmd_ptr,
            /* Pointer to the command event that was received.
            ** if this was not called because of command event, then
            ** cmd_ptr = null.
            */

        const mmoc_rpt_msg_s_type            *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{

  mmoc_evt_status_e_type   evt_status = MMOC_EVT_STATUS_NOT_CONSUMED;
  mmoc_trans_e_type        last_trans;
  mmoc_trans_state_e_type  last_trans_state;

  MMOC_ASSERT( ((cmd_ptr != NULL) && (rpt_ptr == NULL)) ||
               ((cmd_ptr == NULL) && (rpt_ptr != NULL))
             );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(mmoc_info_ptr->curr_trans >= MMOC_TRANS_MAX)
  {
      MMOC_MSG_ERROR( "invalid curr_trans: %d", mmoc_info_ptr->curr_trans,0,0);
      return;
  }
     
  if(mmoc_info_ptr->trans_state >= MMOC_TRANS_STATE_MAX)
  {
      MMOC_MSG_ERROR( "invalid trans_state: %d", mmoc_info_ptr->trans_state,0,0);
      return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_MMOC_LOW_MEM_TARGET
  /* Print debug info before calling transaction handler.
  */
  MMOC_PRINT_MSG_HIGH ( "Before event was processed::");
  MMOC_PRINT_MSG_HIGH ( "Curr_trans  %d(%s)", mmoc_info_ptr->curr_trans, 
                        mmocdbg_trans_e_type_string_map [mmoc_info_ptr->curr_trans]
                        );
  MMOC_PRINT_MSG_HIGH ( "Trans_state %d(%s)", mmoc_info_ptr->trans_state, 
                        mmocdbg_trans_state_e_type_string_map [mmoc_info_ptr->trans_state]
                        );
  MMOC_PRINT_MSG_HIGH ( "Prot_state [MAIN] %d(%s) -- [HYBR_HDR] %d(%s) -- [HYBR_WLAN] %d(%s)", 
                        mmoc_info_ptr->prot_state [SD_SS_MAIN],
                        mmocdbg_prot_state_e_type_string_map [mmoc_info_ptr->prot_state[SD_SS_MAIN]],
                        mmoc_info_ptr->prot_state [SD_SS_HYBR_HDR],
                        mmocdbg_prot_state_e_type_string_map [mmoc_info_ptr->prot_state[SD_SS_HYBR_HDR]],
                        mmoc_info_ptr->prot_state [SD_SS_HYBR_WLAN],
                        mmocdbg_prot_state_e_type_string_map [mmoc_info_ptr->prot_state[SD_SS_HYBR_WLAN]] );
  #endif /* FEATURE_MMOC_LOW_MEM_TARGET */



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if transaction is already in progress.
  */
  if ( mmoc_info_ptr->curr_trans == MMOC_TRANS_NULL )
  {
    /* No transaction in progress, start new transaction.
    */
    evt_status = mmoc_setup_trans( cmd_ptr,
                                   rpt_ptr,
                                   mmoc_info_ptr
                                 );

  } /* if ( mmoc_info_ptr->curr_trans == MMOC_TRANS_NULL )  */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Call the transaction handlers until the event is consumed.
  */
  while ( evt_status != MMOC_EVT_STATUS_CONSUMED )
  {
    last_trans = mmoc_info_ptr->curr_trans;
    last_trans_state = mmoc_info_ptr->trans_state;


    switch( mmoc_info_ptr->curr_trans )
    {

      case MMOC_TRANS_PROT_GEN_CMD:
        evt_status = mmoc_process_prot_gen_cmd( cmd_ptr,
                                                rpt_ptr,
                                                mmoc_info_ptr
                                              );
        break;

      case MMOC_TRANS_SUBSC_CHGD:
        evt_status = mmoc_process_subsc_chgd( cmd_ptr,
                                              rpt_ptr,
                                              mmoc_info_ptr
                                            );
        break;

      case MMOC_TRANS_ONLINE:
        evt_status = mmoc_process_online( cmd_ptr,
                                          rpt_ptr,
                                          mmoc_info_ptr
                                        );
        break;


      case MMOC_TRANS_OFFLINE:
        evt_status = mmoc_process_offline( cmd_ptr,
                                           rpt_ptr,
                                           mmoc_info_ptr
                                         );
        break;

      case MMOC_TRANS_PWR_DOWN:
        evt_status = mmoc_process_pwr_down( cmd_ptr,
                                            rpt_ptr,
                                            mmoc_info_ptr
                                          );
        break;

      case MMOC_TRANS_PWR_SAVE_ENTER:
        evt_status = mmoc_process_pwr_save_enter( cmd_ptr,
                                                  rpt_ptr,
                                                  mmoc_info_ptr
                                                );
        break;

      case MMOC_TRANS_PWR_SAVE_EXIT:
        evt_status = mmoc_process_pwr_save_exit( cmd_ptr,
                                                 rpt_ptr,
                                                 mmoc_info_ptr
                                               );
        break;

      case MMOC_TRANS_PROT_AUTO_DEACTD_IND:
        evt_status = mmoc_process_auto_deactd_ind( cmd_ptr,
                                                   rpt_ptr,
                                                   mmoc_info_ptr
                                                 );
        break;

      case MMOC_TRANS_WAKEUP_FROM_PWR_SAVE:
        evt_status = mmoc_process_wakeup_from_pwr_save( cmd_ptr,
                                                        rpt_ptr,
                                                        mmoc_info_ptr
                                                      );
        break;


      case MMOC_TRANS_MAX:
      case MMOC_TRANS_NULL:
      default:
        MMOC_MSG_ERROR("Invalid transaction", 0, 0, 0);
        evt_status = mmoc_clear_transaction( mmoc_info_ptr );
        break;

    } /* switch ( mmoc_info_ptr->curr_trans ) */

    /* ASSERT: if the event is not consumed, at least the transaction
    ** should have been changed.
    */
    if ( evt_status == MMOC_EVT_STATUS_NOT_CONSUMED &&
         last_trans == mmoc_info_ptr->curr_trans &&
         last_trans_state == mmoc_info_ptr->trans_state
       )
    {
      MMOC_MSG_ERROR("Stuck in trans %d %d", last_trans, last_trans_state,0);

      /*lint -save -e506 -e774*/
      MMOC_ASSERT( FALSE );
      /*lint -restore*/
    }

  } /* while ( evt_status != MMOC_EVT_STATUS_CONSUMED ) */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  if(mmoc_info_ptr->curr_trans >= MMOC_TRANS_MAX)
  {
      MMOC_MSG_ERROR( "invalid curr_trans: %d", mmoc_info_ptr->curr_trans,0,0);
      return;
  }

  if(mmoc_info_ptr->trans_state >= MMOC_TRANS_STATE_MAX)
  {
      MMOC_MSG_ERROR( "invalid trans_state: %d", mmoc_info_ptr->trans_state,0,0);
      return;
  }

  #ifndef FEATURE_MMOC_LOW_MEM_TARGET
  /* Print debug info after calling transaction handler.
  */
  MMOC_PRINT_MSG_HIGH ( "After event was processed::");
  MMOC_PRINT_MSG_HIGH ( "Curr_trans  %d(%s)", mmoc_info_ptr->curr_trans, 
                        mmocdbg_trans_e_type_string_map [mmoc_info_ptr->curr_trans]
                        );
  MMOC_PRINT_MSG_HIGH ( "Trans_state %d(%s)", mmoc_info_ptr->trans_state, 
                        mmocdbg_trans_state_e_type_string_map [mmoc_info_ptr->trans_state]
                        );
  MMOC_PRINT_MSG_HIGH ( "Prot_state [MAIN] %d(%s) -- [HYBR_HDR] %d(%s) -- [HYBR_WLAN] %d(%s)", 
                        mmoc_info_ptr->prot_state [SD_SS_MAIN],
                        mmocdbg_prot_state_e_type_string_map [mmoc_info_ptr->prot_state[SD_SS_MAIN]],
                        mmoc_info_ptr->prot_state [SD_SS_HYBR_HDR],
                        mmocdbg_prot_state_e_type_string_map [mmoc_info_ptr->prot_state[SD_SS_HYBR_HDR]],
                        mmoc_info_ptr->prot_state [SD_SS_HYBR_WLAN],
                        mmocdbg_prot_state_e_type_string_map [mmoc_info_ptr->prot_state[SD_SS_HYBR_WLAN]] );
  #endif /* FEATURE_MMOC_LOW_MEM_TARGET */


  

  return;

} /* mmoc_call_trans_hndlr() */

/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
============================== CM -> MMOC commands ==========================
=============================================================================
=============================================================================
===========================================================================*/

/* The following section defines the API when the clients ( example CM )
** have to communicate to MMoC using the command queue.
*/

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_cmd_subscription_chgd

DESCRIPTION
  This function should be called when the subscription changed because of NAM
  change or GSM/WCDMA subscription availability change or CDMA/AMPS/HDR
  subscription availability change. This function will queue the subscription
  changed command to the MMoC task.


DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
MMEXTN  void                           mmoc_cmd_subscription_chgd
(
        mmoc_subsc_chg_e_type          chg_type,
             /* Subscription change type.
             */

        byte                           nam,
            /* Current NAM selection (NAM-1, NAM-2, etc.).
            */

        sd_ss_orig_mode_e_type         orig_mode,
            /* The origination mode (normal, OTASP, emergency, other, etc.)
            ** that is associated with the selected NAM.
            */

        sd_ss_mode_pref_e_type         mode_pref,
           /* The mode preference that is associated with the selected NAM.
           */

        sd_ss_band_pref_e_type         band_pref,
           /* Band preference that is associated with the selected NAM.
           */

        sd_ss_prl_pref_e_type          prl_pref,
           /* PRL preference that is associated with the selected NAM.
           */

        sd_ss_roam_pref_e_type         roam_pref,
           /* Roam preference that is associated with the selected NAM.
           */

        sd_ss_hybr_pref_e_type         hybr_pref,
           /* New hybrid preference (none, CDMA/HDR).
           */

        const sys_manual_sys_info_s_type    *manual_sys_info_ptr,
          /* Pointer to struct that defines the manual system.
           */

        sd_ss_srv_domain_pref_e_type   srv_domain_pref,
           /* Service domain preference that is associated with the selected
           ** NAM.
           ** Applies only for GSM/WCDMA modes.
           */

        sd_ss_acq_order_pref_e_type    acq_order_pref,
           /* Preference for the order of acquisition ( WCDMA before GSM,
           ** GSM before WCDMA etc).
           */

        sd_ss_tech_pref_e_type         tech_pref,
           /* Preference for the technology versions to use.
           */

        sys_wlan_scan_pref_e_type      wlan_scan_pref,
           /* WLAN scan preference.
           */

        boolean                        is_gw_subsc_avail,
           /* Current GSM/WCDMA subscription availability status.
           ** true = subscription available,
           ** false = subscription not available.
           */

        boolean                        is_cdma_subsc_avail,
           /* Current CDMA/AMPS/HDR subscription availability status.
           ** true = subscription available,
           ** false = subscription not available.
           */

        boolean                        is_wlan_subsc_avail,
           /* Current WLAN subscription availability status.
           ** true = subscription available,
           ** false = subscription not available.
           */

        prot_subsc_chg_e_type          prot_subsc_chg
           /* Enumeration of protocols that have a change in
           ** subscription available status
           */
)
{

  mmoc_cmd_subscription_chgd2(chg_type,
                              nam,
                              orig_mode,
                              mode_pref,
                              band_pref,
                              prl_pref,
                              roam_pref,
                              hybr_pref,
                              manual_sys_info_ptr,
                              srv_domain_pref,
                              acq_order_pref,
                              tech_pref,
                              wlan_scan_pref,
                              SYS_WLAN_BSS_TYPE_ANY,
                              is_gw_subsc_avail,
                              is_cdma_subsc_avail,
                              is_wlan_subsc_avail,
                              prot_subsc_chg
                             );

}

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_cmd_subscription_chgd2

DESCRIPTION
  This function should be called when the subscription changed because of NAM
  change or GSM/WCDMA subscription availability change or CDMA/AMPS/HDR
  subscription availability change. This function will queue the subscription
  changed command to the MMoC task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
MMEXTN  void                           mmoc_cmd_subscription_chgd2
(
        mmoc_subsc_chg_e_type          chg_type,
             /* Subscription change type.
             */

        byte                           nam,
            /* Current NAM selection (NAM-1, NAM-2, etc.).
            */

        sd_ss_orig_mode_e_type         orig_mode,
            /* The origination mode (normal, OTASP, emergency, other, etc.)
            ** that is associated with the selected NAM.
            */

        sd_ss_mode_pref_e_type         mode_pref,
           /* The mode preference that is associated with the selected NAM.
           */

        sd_ss_band_pref_e_type         band_pref,
           /* Band preference that is associated with the selected NAM.
           */

        sd_ss_prl_pref_e_type          prl_pref,
           /* PRL preference that is associated with the selected NAM.
           */

        sd_ss_roam_pref_e_type         roam_pref,
           /* Roam preference that is associated with the selected NAM.
           */

        sd_ss_hybr_pref_e_type         hybr_pref,
           /* New hybrid preference (none, CDMA/HDR).
           */

        const sys_manual_sys_info_s_type    *manual_sys_info_ptr,
          /* Pointer to struct that defines the manual system.
           */

        sd_ss_srv_domain_pref_e_type   srv_domain_pref,
           /* Service domain preference that is associated with the selected
           ** NAM.
           ** Applies only for GSM/WCDMA modes.
           */

        sd_ss_acq_order_pref_e_type    acq_order_pref,
           /* Preference for the order of acquisition ( WCDMA before GSM,
           ** GSM before WCDMA etc).
           */

        sd_ss_tech_pref_e_type         tech_pref,
           /* Preference for the technology versions to use.
           */

        sys_wlan_scan_pref_e_type      wlan_scan_pref,
           /* WLAN scan preference.
           */

        sys_wlan_bss_e_type            wlan_bss_type_pref,
           /* Preference for the WLAN BSS type.
           */

        boolean                        is_gw_subsc_avail,
           /* Current GSM/WCDMA subscription availability status.
           ** true = subscription available,
           ** false = subscription not available.
           */

        boolean                        is_cdma_subsc_avail,
           /* Current CDMA/AMPS/HDR subscription availability status.
           ** true = subscription available,
           ** false = subscription not available.
           */

        boolean                        is_wlan_subsc_avail,
           /* Current WLAN subscription availability status.
           ** true = subscription available,
           ** false = subscription not available.
           */

        prot_subsc_chg_e_type          prot_subsc_chg
           /* Enumeration of protocols that have a change in
           ** subscription available status
           */
)
{
  mmoc_cmd_msg_s_type                 *msg_ptr;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Get the command buffer.
  */
  msg_ptr = mmoc_get_cmd_buf_else_err_fatal();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Fill in all the fields.
  */
  msg_ptr->cmd.name                             = MMOC_CMD_SUBSCRIPTION_CHGD;
  msg_ptr->param.subsc_chgd.chg_type            = chg_type;
  msg_ptr->param.subsc_chgd.nam                 = nam;
  msg_ptr->param.subsc_chgd.orig_mode           = orig_mode;
  msg_ptr->param.subsc_chgd.mode_pref           = mode_pref;
  msg_ptr->param.subsc_chgd.band_pref           = band_pref;
  msg_ptr->param.subsc_chgd.prl_pref            = prl_pref;
  msg_ptr->param.subsc_chgd.roam_pref           = roam_pref;
  msg_ptr->param.subsc_chgd.hybr_pref           = hybr_pref;
  msg_ptr->param.subsc_chgd.manual_sys_info.sys_id.id_type
                                                = SYS_SYS_ID_TYPE_UNDEFINED;
  if ( manual_sys_info_ptr != NULL )
  {
    msg_ptr->param.subsc_chgd.manual_sys_info   = *manual_sys_info_ptr;
  }
  msg_ptr->param.subsc_chgd.domain_pref         = srv_domain_pref;
  msg_ptr->param.subsc_chgd.acq_order_pref      = acq_order_pref;
  msg_ptr->param.subsc_chgd.tech_pref           = tech_pref;
  msg_ptr->param.subsc_chgd.wlan_scan_pref      = wlan_scan_pref;
  msg_ptr->param.subsc_chgd.wlan_bss_type_pref  = wlan_bss_type_pref;
  msg_ptr->param.subsc_chgd.is_gw_subsc_avail   = is_gw_subsc_avail;
  msg_ptr->param.subsc_chgd.is_cdma_subsc_avail = is_cdma_subsc_avail;
  msg_ptr->param.subsc_chgd.is_wlan_subsc_avail = is_wlan_subsc_avail;
  msg_ptr->param.subsc_chgd.prot_subsc_chg      = prot_subsc_chg;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Post the command.
  */
  mmoc_send_cmd( msg_ptr );


  return;

} /* mmoc_cmd_subscription_chgd() */


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_cmd_oprt_mode_chgd

DESCRIPTION
  This function will be used to command the MMoC to do operating mode change,
  the new operating mode is specified in the oprt_mode parameter.  This
  function will queue the operating mode changed command to the MMoC task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
MMEXTN  void                           mmoc_cmd_oprt_mode_chgd
(

        sys_oprt_mode_e_type           oprt_mode
            /* The operating mode to be switched to.
            */

)
{
  mmoc_cmd_msg_s_type                 *msg_ptr;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Get the command buffer.
  */
  msg_ptr = mmoc_get_cmd_buf_else_err_fatal();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Fill in all the fields.
  */
  msg_ptr->cmd.name                       = MMOC_CMD_OPRT_MODE_CHGD;
  msg_ptr->param.oprt_mode_chgd.oprt_mode = oprt_mode;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Post the command.
  */
  mmoc_send_cmd( msg_ptr );

  return;

} /* mmoc_cmd_oprt_mode_chgd() */

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_cmd_pref_sys_chgd

DESCRIPTION
  This function is used to command the MMoC to do Preferred system selection
  change. This function will queue the preferred system selection changed
  command to the MMoC Task.


DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
MMEXTN  void                           mmoc_cmd_pref_sys_chgd
(

        sd_ss_e_type                   ss,
            /* Indicates the SS-Instance ( SS_MAIN or SS_HDR) this
            ** pref_sys_chgd command is intended for.
            */

        sd_ss_pref_reas_e_type         pref_reas,
            /* Reason for changing the SS-Preference.
            */

        sd_ss_orig_mode_e_type         orig_mode,
            /* The origination mode (normal, OTASP, emergency, other, etc.)
            ** that is associated with the selected NAM.
            */

        sd_ss_mode_pref_e_type         mode_pref,
           /* The mode preference that is associated with the selected NAM.
           */

        sd_ss_band_pref_e_type         band_pref,
           /* Band preference that is associated with the selected NAM.
           */

        sd_ss_prl_pref_e_type          prl_pref,
           /* PRL preference that is associated with the selected NAM.
           */

        sd_ss_roam_pref_e_type         roam_pref,
           /* Roam preference that is associated with the selected NAM.
           */

        sd_ss_hybr_pref_e_type         hybr_pref,
           /* New hybrid preference (none, CDMA/HDR).
           */

        sd_band_e_type                 otasp_band,
           /* The band-class that is associated with OTASP origination mode.
           ** Note that this parameter is ignored if orig_mode != OTASP.
           */

        sd_blksys_e_type               otasp_blksys,
          /* The PCS frequency block/Cellular system that is associated
          ** with OTASP origination mode. Note that this parameter is ignored
          ** if orig_mode != OTASP.
          */

        sd_ss_avoid_sys_e_type         avoid_type,
          /* Type of the system to be avoided, valid only
          ** if pref_reason = SD_SS_PREF_REAS_AVOID_SYS.
          */

        dword                          avoid_time,
          /* Time in seconds for which the system is to be avoided, valid
          ** only if pref_reason = SD_SS_PREF_REAS_AVOID_SYS.
          */

        const sys_manual_sys_info_s_type    *manual_sys_info_ptr,
          /* Pointer to struct that defines the manual system.
           */

        sd_ss_srv_domain_pref_e_type   srv_domain_pref,
           /* Service domain preference that is associated with the selected
           ** NAM.
           ** Applies only for GSM/WCDMA modes.
           */

        sd_ss_acq_order_pref_e_type    acq_order_pref,
           /* Preference for the order of acquisition ( WCDMA before GSM,
           ** GSM before WCDMA etc).
           */

        sd_ss_tech_pref_e_type         tech_pref,
           /* Preference for the technology versions to use.
           */

        sys_wlan_scan_pref_e_type      wlan_scan_pref
           /* WLAN scan preference.
           */
)
{
  mmoc_cmd_pref_sys_chgd2(ss,
                          pref_reas,
                          orig_mode,
                          mode_pref,
                          band_pref,
                          prl_pref,
                          roam_pref,
                          hybr_pref,
                          otasp_band,
                          otasp_blksys,
                          avoid_type,
                          avoid_time,
                          manual_sys_info_ptr,
                          srv_domain_pref,
                          acq_order_pref,
                          tech_pref,
                          wlan_scan_pref,
                          SYS_WLAN_BSS_TYPE_ANY
                         );
} /* mmoc_cmd_pref_sys_chgd() */


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_cmd_pref_sys_chgd2

DESCRIPTION
  This function is used to command the MMoC to do Preferred system selection
  change. This function will queue the preferred system selection changed
  command to the MMoC Task.


DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
MMEXTN  void                           mmoc_cmd_pref_sys_chgd2
(

        sd_ss_e_type                   ss,
            /* Indicates the SS-Instance ( SS_MAIN or SS_HDR) this
            ** pref_sys_chgd command is intended for.
            */

        sd_ss_pref_reas_e_type         pref_reas,
            /* Reason for changing the SS-Preference.
            */

        sd_ss_orig_mode_e_type         orig_mode,
            /* The origination mode (normal, OTASP, emergency, other, etc.)
            ** that is associated with the selected NAM.
            */

        sd_ss_mode_pref_e_type         mode_pref,
           /* The mode preference that is associated with the selected NAM.
           */

        sd_ss_band_pref_e_type         band_pref,
           /* Band preference that is associated with the selected NAM.
           */

        sd_ss_prl_pref_e_type          prl_pref,
           /* PRL preference that is associated with the selected NAM.
           */

        sd_ss_roam_pref_e_type         roam_pref,
           /* Roam preference that is associated with the selected NAM.
           */

        sd_ss_hybr_pref_e_type         hybr_pref,
           /* New hybrid preference (none, CDMA/HDR).
           */

        sd_band_e_type                 otasp_band,
           /* The band-class that is associated with OTASP origination mode.
           ** Note that this parameter is ignored if orig_mode != OTASP.
           */

        sd_blksys_e_type               otasp_blksys,
          /* The PCS frequency block/Cellular system that is associated
          ** with OTASP origination mode. Note that this parameter is ignored
          ** if orig_mode != OTASP.
          */

        sd_ss_avoid_sys_e_type         avoid_type,
          /* Type of the system to be avoided, valid only
          ** if pref_reason = SD_SS_PREF_REAS_AVOID_SYS.
          */

        dword                          avoid_time,
          /* Time in seconds for which the system is to be avoided, valid
          ** only if pref_reason = SD_SS_PREF_REAS_AVOID_SYS.
          */

        const sys_manual_sys_info_s_type    *manual_sys_info_ptr,
          /* Pointer to struct that defines the manual system.
           */

        sd_ss_srv_domain_pref_e_type   srv_domain_pref,
           /* Service domain preference that is associated with the selected
           ** NAM.
           ** Applies only for GSM/WCDMA modes.
           */

        sd_ss_acq_order_pref_e_type    acq_order_pref,
           /* Preference for the order of acquisition ( WCDMA before GSM,
           ** GSM before WCDMA etc).
           */

        sd_ss_tech_pref_e_type         tech_pref,
           /* Preference for the technology versions to use.
           */

        sys_wlan_scan_pref_e_type      wlan_scan_pref,
           /* WLAN scan preference.
           */

        sys_wlan_bss_e_type            wlan_bss_type_pref
           /* Preference for the WLAN BSS type.
           */
)
{
  mmoc_cmd_msg_s_type                 *msg_ptr;
  prot_cmd_pref_sys_chgd_s_type       pref_sys_chgd;

  #ifndef MMOC_WLAN_SUPPORTED
  if(ss == SD_SS_HYBR_WLAN)
  {
    return;
  }
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Get the command buffer.
  */
  msg_ptr = mmoc_get_cmd_buf_else_err_fatal();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Fill in all the fields.
  */
  msg_ptr->cmd.name                              = MMOC_CMD_PROT_GEN_CMD;
  msg_ptr->param.gen_prot_cmd.cmd_type           = PROT_GEN_CMD_PREF_SYS_CHGD;
  msg_ptr->param.gen_prot_cmd.ss                 = ss;

  pref_sys_chgd.pref_reas                        = pref_reas;
  pref_sys_chgd.orig_mode                        = orig_mode;
  pref_sys_chgd.mode_pref                        = mode_pref;
  pref_sys_chgd.band_pref                        = band_pref;
  pref_sys_chgd.prl_pref                         = prl_pref;
  pref_sys_chgd.roam_pref                        = roam_pref;
  pref_sys_chgd.hybr_pref                        = hybr_pref;
  pref_sys_chgd.otasp_band                       = otasp_band;
  pref_sys_chgd.otasp_blksys                     = otasp_blksys;
  pref_sys_chgd.avoid_type                       = avoid_type;
  pref_sys_chgd.avoid_time                       = avoid_time;
  pref_sys_chgd.manual_sys_info.sys_id.id_type   = SYS_SYS_ID_TYPE_UNDEFINED;
  if ( manual_sys_info_ptr != NULL )
  {
    pref_sys_chgd.manual_sys_info                = *manual_sys_info_ptr;
  }
  pref_sys_chgd.domain_pref                      = srv_domain_pref;
  pref_sys_chgd.acq_order_pref                   = acq_order_pref;
  pref_sys_chgd.tech_pref                        = tech_pref;
  pref_sys_chgd.wlan_scan_pref                   = wlan_scan_pref;
  pref_sys_chgd.wlan_bss_type_pref               = wlan_bss_type_pref;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Copy the parameters into generic command struct.
  */
  msg_ptr->param.gen_prot_cmd.param.pref_sys_chgd  = pref_sys_chgd;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Post the command.
  */
  mmoc_send_cmd( msg_ptr );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* To detect duplicate pref_sys_chgd commands, set the SIG.
  */
  (void)rex_set_sigs( &mmoc_tcb, MMOC_DUP_CMD_REM_SIG );


  return;

} /* mmoc_cmd_pref_sys_chgd() */


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_cmd_get_networks_gw

DESCRIPTION
  The function will be used to command the MMoC to get the networks on GSM/
  WCDMA technologies. This function will queue the Get networks command to
  the MMoC task.


DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
MMEXTN  void                           mmoc_cmd_get_networks_gw
(

        sd_ss_mode_pref_e_type         mode_pref,
           /* Mode preference that is associated with the Get networks
           ** command.
           */


        sd_ss_band_pref_e_type         band_pref
          /* Band preference that is associated with the Get networks
          ** command.
          */


)
{
  mmoc_cmd_msg_s_type                 *msg_ptr;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Get the command buffer.
  */
  msg_ptr = mmoc_get_cmd_buf_else_err_fatal();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Fill in all the fields.
  */
  msg_ptr->cmd.name                    = MMOC_CMD_PROT_GEN_CMD;
  msg_ptr->param.gen_prot_cmd.cmd_type = PROT_GEN_CMD_GET_NETWORKS_GW;
  msg_ptr->param.gen_prot_cmd.param.gw_get_net.mode_pref = mode_pref;
  msg_ptr->param.gen_prot_cmd.param.gw_get_net.band_pref = band_pref;
  msg_ptr->param.gen_prot_cmd.ss       = sd_misc_get_ss_for_mode(SYS_SYS_MODE_GW);
  if(msg_ptr->param.gen_prot_cmd.ss == SD_SS_MAX)
  {
    msg_ptr->param.gen_prot_cmd.ss = SD_SS_MAIN;
  }



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Post the command.
  */
  mmoc_send_cmd( msg_ptr );


  return;

} /* mmoc_cmd_get_networks_gw() */

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_cmd_term_get_networks_gw

DESCRIPTION
  This function will be used to inform the MMoC that the user has terminated
  the Get networks request on GSM/WCDMA networks. This function will queue
  the Terminate get networks command to the MMoC task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
MMEXTN  void                           mmoc_cmd_term_get_networks_gw
(

        void

)
{

  mmoc_cmd_msg_s_type                  *msg_ptr;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Get the command buffer.
  */
  msg_ptr = mmoc_get_cmd_buf_else_err_fatal();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Fill in all the fields.
  */
  msg_ptr->cmd.name                    = MMOC_CMD_PROT_GEN_CMD;
  msg_ptr->param.gen_prot_cmd.cmd_type = PROT_GEN_CMD_TERM_GET_NETWORKS_GW;
  msg_ptr->param.gen_prot_cmd.ss       = sd_misc_get_ss_for_mode(SYS_SYS_MODE_GW);
  if(msg_ptr->param.gen_prot_cmd.ss == SD_SS_MAX)
  {
    msg_ptr->param.gen_prot_cmd.ss = SD_SS_MAIN;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Post the command.
  */
  mmoc_send_cmd( msg_ptr );


  return;

} /* mmoc_cmd_term_get_networks_gw() */

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_cmd_get_networks_wlan

DESCRIPTION
  The function will be used to command the MMoC to get the networks on WLAN
  mode. This function will queue the Get networks command to the MMoC task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
MMEXTN  void                           mmoc_cmd_get_networks_wlan
(

        sd_ss_band_pref_e_type         bands
          /* Band preference that is associated with the Get networks
          ** command.
          */
)
{
  #ifdef MMOC_WLAN_SUPPORTED
#error code not present
  #else
  SYS_ARG_NOT_USED(bands);
  #endif /* MMOC_WLAN_SUPPORTED */


  return;
} /* mmoc_cmd_get_networks_wlan() */


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_cmd_term_get_networks_wlan

DESCRIPTION
  This function will be used to inform the MMoC that the user has terminated
  the Get networks request on WLAN networks. This function will queue
  the Terminate get networks command to the MMoC task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
MMEXTN  void                          mmoc_cmd_term_get_networks_wlan
(

        void
)
{
  #ifdef MMOC_WLAN_SUPPORTED
#error code not present
  #endif /* MMOC_WLAN_SUPPORTED */

  return;

} /* mmoc_cmd_term_get_networks_wlan() */

/*===========================================================================

FUNCTION mmoc_cmd_wlan_wps_start

DESCRIPTION
  The function is used to command the MMoC to get WLAN credentails using WPS.
  This function will queue the WPS start command to the MMoC task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
MMEXTN  void                           mmoc_cmd_wlan_wps_start
(

        sys_wlan_wps_start_param_s_type     start_param
          /* WPS start parameters that are associated with the WLAN WPS start
          ** command.
          */
)
/*lint -esym(818,start_param)*/
{
  #ifdef FEATURE_WLAN_WPS  
#error code not present
  #else
  SYS_ARG_NOT_USED(start_param.wps_mode);
  #endif
  return;
} /* mmoc_cmd_wlan_wps_start() */
/*lint +esym(818,start_param)*/


/*===========================================================================

FUNCTION mmoc_cmd_wlan_wps_abort

DESCRIPTION
  This function will be used to inform the MMoC that the user has aborted the
  WPS procedure on WLAN. This function will queue the WPS abort command to
  the MMoC task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
MMEXTN  void                           mmoc_cmd_wlan_wps_abort
(
        void
)
{

  #ifdef FEATURE_WLAN_WPS
#error code not present
  #endif

  return;

} /* mmoc_cmd_wlan_wps_abort() */

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_cmd_wakeup_from_pwr_save

DESCRIPTION
  This function will be used to command the MMoC to wakeup the phone from
  power save.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
MMEXTN  void                           mmoc_cmd_wakeup_from_pwr_save
(

        sd_ss_e_type                   ss
          /* Wakeup the 'ss' System Selection instance from power save.
          */

)
{
  mmoc_cmd_msg_s_type                 *msg_ptr;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Get the command buffer.
  */
  msg_ptr = mmoc_get_cmd_buf_else_err_fatal();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Fill in all the fields.
  */
  msg_ptr->cmd.name                       = MMOC_CMD_WAKEUP_FROM_PWR_SAVE;
  msg_ptr->param.wakeup.ss                = ss;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Post the command.
  */
  mmoc_send_cmd( msg_ptr );

  return;

} /* mmoc_cmd_wakeup_from_pwr_save() */



/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
============================== Protocols -> MMOC reports ====================
=============================================================================
=============================================================================
===========================================================================*/

/* The following section defines the API when the clients ( example protocols
** ) have to communicate to MMoC using the report queue.
*/

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_rpt_prot_deactd_cnf

DESCRIPTION
  This function will be used to confirm that the MMoC that the protocol has
  been deactivated. This function will queue the protocol deactivated report
  to the MMoC task.

  Note! This function should be called only in response to protocol
  deactivate request from MMoC.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
MMEXTN  void                           mmoc_rpt_prot_deactd_cnf
(

        prot_trans_type                trans_id
           /* Protocols set this same as the trans_id received in their
           ** Deactivate request.
           */

)
{
  mmoc_rpt_msg_s_type                  *msg_ptr;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Get the report buffer.
  */
  msg_ptr = mmoc_get_rpt_buf_else_err_fatal();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Fill in all the fields.
  */
  msg_ptr->rpt.name                       = MMOC_RPT_PROT_DEACTD_CNF;
  msg_ptr->param.prot_deactd_cnf.trans_id = trans_id;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Post the report.
  */
  mmoc_send_rpt( msg_ptr );


  return;

} /* mmoc_rpt_prot_deactd_cnf() */


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_rpt_prot_auto_deactd_ind

DESCRIPTION
   This function will be used to inform the MMoC that the protocol has been
  deactivated autonomously. This function will queue the deactivated
  indication report to the MMoC task.

  Note! This function should not called in response to protocol deactivate
  request from MMoC.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
MMEXTN  void                           mmoc_rpt_prot_auto_deactd_ind
(

        prot_deact_e_type              deactd_reason,
           /* Reason that the protocol was deactivated.
           */

        prot_state_e_type              prot_state
           /* Protocol which sent the autonomous deactivation.
           */

)
{
  mmoc_rpt_msg_s_type                  *msg_ptr;

  MMOC_ASSERT( deactd_reason != PROT_DEACT_PWR_DOWN   &&
               deactd_reason != PROT_DEACT_RESET      &&
               deactd_reason != PROT_DEACT_LPM        &&
               deactd_reason != PROT_DEACT_OFFLINE    &&
               deactd_reason != PROT_DEACT_SUBSC_CHGD &&
               deactd_reason != PROT_DEACT_MAX
             );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Get the report buffer.
  */
  msg_ptr = mmoc_get_rpt_buf_else_err_fatal();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Fill in all the fields.
  */
  msg_ptr->rpt.name                     = MMOC_RPT_PROT_AUTO_DEACTD_IND;
  msg_ptr->param.prot_deactd_ind.reason = deactd_reason;
  msg_ptr->param.prot_deactd_ind.prot_state   = prot_state;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Post the report.
  */
  mmoc_send_rpt( msg_ptr );


  return;

} /* mmoc_rpt_prot_auto_deactd_ind() */


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_rpt_ph_stat_chgd_cnf

DESCRIPTION
  This function should be used by the protocols to acknowledge the MMoC that
  the "phone status changed (due to either NAM change, Subscription
  availability change(GSM/WCDMA or CDMA/AMPS/HDR) or operating mode change
  request. This function will queue the phone status changed confirmation
  report to the MMoC task.

  Note! This should be called only after completing the necessary
  housekeeping actions based on the request.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
MMEXTN  void                           mmoc_rpt_ph_stat_chgd_cnf
(

        prot_trans_type                trans_id,
          /* Protocols set this same as the trans_id received in the phone
          ** status changed ack. trans_id cannot be MMOC_TRANS_AUTONOMOUS.
          */

        sd_mode_e_type                 prot
          /* Protocol responding to the Phone status changed command.
          */


)
{
  mmoc_rpt_msg_s_type                  *msg_ptr;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Get the report buffer.
  */
  msg_ptr = mmoc_get_rpt_buf_else_err_fatal();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Fill in all the fields.
  */
  msg_ptr->rpt.name                   = MMOC_RPT_PH_STAT_CHGD_CNF;
  msg_ptr->param.ph_stat_cnf.trans_id = trans_id;
  msg_ptr->param.ph_stat_cnf.prot     = prot;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Post the report.
  */
  mmoc_send_rpt( msg_ptr );


  return;

} /* mmoc_rpt_ph_stat_chgd_cnf() */


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_rpt_tmc_mode_cnf

DESCRIPTION
  Function gets called by TMC to acknowledge that it has processed the 
  mode change request. Mode change is given to TMC for Offline, online and
  power down transactions.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
MMEXTN  void                           mmoc_rpt_tmc_mode_cnf
(

        prot_trans_type                trans_id
          /* Protocols set this same as the trans_id received in the phone
          ** status changed ack. trans_id cannot be MMOC_TRANS_AUTONOMOUS.
          */
)
{
  mmoc_rpt_msg_s_type                  *msg_ptr;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Get the report buffer.
  */
  msg_ptr = mmoc_get_rpt_buf_else_err_fatal();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Fill in all the fields.
  */
  msg_ptr->rpt.name                    = MMOC_RPT_TMC_MODE_CNF;  
  

  msg_ptr->param.tmc_mode_cnf.trans_id = trans_id;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Post the report.
  */
  mmoc_send_rpt( msg_ptr );


  return;

} /* mmoc_rpt_tmc_mode_cnf() */


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_rpt_prot_gen_cmd_cnf

DESCRIPTION
  This function is used to acknowledge the generic protocol command.

  Note!  This is a local function and should be called by
  mmoc_proc_prot_gen_cmd.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
MMEXTN  void                           mmoc_rpt_prot_gen_cmd_cnf
(

        boolean                        is_cmd_processed,
           /* If the protocols received the generic command in the incorrect
           ** state, then this will be set to FALSE.
           */

        prot_trans_type                trans_id,
           /* Protocols set this same as the trans_id received in the generic
           ** command. trans_id cannot be MMOC_TRANS_AUTONOMOUS.
           */

        boolean                        is_prot_deactivating
          /* Is the protocol deactiavting because of processing the generic
          ** command. Valid when when is_cmd_processed = TRUE.
          */

)
{
  mmoc_rpt_msg_s_type                  *msg_ptr;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Get the report buffer.
  */
  msg_ptr = mmoc_get_rpt_buf_else_err_fatal();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Fill in all the fields.
  */
  msg_ptr->rpt.name                           = MMOC_RPT_PROT_GEN_CMD_CNF;
  msg_ptr->param.gen_cmd_cnf.is_cmd_processed = is_cmd_processed;
  msg_ptr->param.gen_cmd_cnf.trans_id         = trans_id;
  msg_ptr->param.gen_cmd_cnf.is_prot_deactivating = is_prot_deactivating;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Post the report.
  */
  mmoc_send_rpt( msg_ptr );


  return;

} /*  mmoc_rpt_prot_gen_cmd_cnf() */


/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
============================== Protocols -> MMOC functional interface =======
=============================================================================
=============================================================================
===========================================================================*/

/* The following section defines the API when the clients ( example
** protocols ) have to communicate to MMoC using the functional interface.
*/


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_proc_prot_gen_cmd

DESCRIPTION
  This function should be used by protocols to process the Generic command
  received from MMoC.  The Generic command could be received because of
  Reselection timeout, Get networks request, Terminate get networks or
  preferred system selection change.

  Note! This function will also send the Generic command ack to MMoC task.

DEPENDENCIES
  None.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system.

    SD_SS_ACT_ACQ_GW  -   Attempt acquiring an HDR system.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH.

    SD_SS_ACT_PWR_SAVE  - Enter power save mode.


SIDE EFFECTS
  None.
===========================================================================*/
MMEXTN  sd_ss_act_e_type               mmoc_proc_prot_gen_cmd
(

        boolean                        is_resel_allowed,
          /* Protocol indicated if reselection is allowed at this time or not.
          ** TRUE = reselection allowed. This flag is passed on to SD20 in the
          ** call to sd_ss_ind_misc_timer(). So, this parameter is used only
          ** if the gen_ss_cmd type is Resetecion timer expired, for other
          ** types it is not used.
          */

        boolean                        is_prot_active,
          /* Flag which specifies if the protocol calling this function is
          ** ACTIVE or not.
          */


        const prot_gen_cmd_s_type           *gen_cmd_ptr
           /* Pointer to generic system selection command information.
           ** Same as what was received by the protocols.
           */

)
{
  sd_ss_act_e_type    ss_act = SD_SS_ACT_CONTINUE;
  boolean             is_cmd_processed = FALSE;
  boolean             is_prot_deactivating = FALSE;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Check if the protocol is active?
  */
  if ( is_prot_active )
  {
    switch (gen_cmd_ptr->ss)
    {
      case SD_SS_MAIN:
      {
      /* Protocol is active,Check which SD indication needs to be called.
      */
      switch (gen_cmd_ptr->cmd_type )
      {
        case PROT_GEN_CMD_SS_TIMER_EXP:
          ss_act = sd_ss_ind_misc_timer( is_resel_allowed, NULL );
          break;

        case PROT_GEN_CMD_PREF_SYS_CHGD:
          ss_act = sd_ss_ind_user_ss_pref3
                   (
                     gen_cmd_ptr->param.pref_sys_chgd.pref_reas,
                     gen_cmd_ptr->param.pref_sys_chgd.orig_mode,
                     gen_cmd_ptr->param.pref_sys_chgd.mode_pref,
                     gen_cmd_ptr->param.pref_sys_chgd.band_pref,
                     gen_cmd_ptr->param.pref_sys_chgd.prl_pref,
                     gen_cmd_ptr->param.pref_sys_chgd.roam_pref,
                     gen_cmd_ptr->param.pref_sys_chgd.hybr_pref,
                     gen_cmd_ptr->param.pref_sys_chgd.otasp_band,
                     gen_cmd_ptr->param.pref_sys_chgd.otasp_blksys,
                     gen_cmd_ptr->param.pref_sys_chgd.avoid_type,
                     gen_cmd_ptr->param.pref_sys_chgd.avoid_time,
                       &gen_cmd_ptr->param.pref_sys_chgd.manual_sys_info,
                     gen_cmd_ptr->param.pref_sys_chgd.domain_pref,
                     gen_cmd_ptr->param.pref_sys_chgd.acq_order_pref,
                       gen_cmd_ptr->param.pref_sys_chgd.tech_pref,
                       gen_cmd_ptr->param.pref_sys_chgd.wlan_scan_pref,
                       gen_cmd_ptr->param.pref_sys_chgd.wlan_bss_type_pref,
                     NULL
                   );
          break;

        case PROT_GEN_CMD_GET_NETWORKS_GW:
          #ifdef MMOC_GW_SUPPORTED
          ss_act = sd_ss_ind_user_get_net_gw
                   (
                     gen_cmd_ptr->param.gw_get_net.mode_pref,
                     gen_cmd_ptr->param.gw_get_net.band_pref,
                     NULL
                   );
          #endif
          break;

        case PROT_GEN_CMD_TERM_GET_NETWORKS_GW:
          #ifdef MMOC_GW_SUPPORTED
          ss_act = sd_ss_ind_user_term_get_net_gw
                   (
                     NULL
                   );
          #endif
          break;

        default:
          MMOC_MSG_ERROR(" Unknown generic command ", 0, 0, 0);
          ss_act = SD_SS_ACT_CONTINUE;
          break;
      } /* switch (gen_cmd_ptr->cmd_type ) */
 
      } /*  case SD_SS_MAIN:  */
      break;

      #ifdef MMOC_HYBR_1_SUPPORTED
      case SD_SS_HYBR_1:
      {
        /* Protocol is active,Check which SD indication needs to be called.
        */
        switch (gen_cmd_ptr->cmd_type )
        {
          case PROT_GEN_CMD_SS_TIMER_EXP:
            ss_act = sd_ss_ind_hybr_1_misc_timer( is_resel_allowed, NULL );
            break;

          case PROT_GEN_CMD_PREF_SYS_CHGD:
            ss_act = sd_ss_ind_hybr_1_user_ss_pref3
                     (
                       gen_cmd_ptr->param.pref_sys_chgd.pref_reas,
                       gen_cmd_ptr->param.pref_sys_chgd.orig_mode,
                       gen_cmd_ptr->param.pref_sys_chgd.mode_pref,
                       gen_cmd_ptr->param.pref_sys_chgd.band_pref,
                       gen_cmd_ptr->param.pref_sys_chgd.prl_pref,
                       gen_cmd_ptr->param.pref_sys_chgd.roam_pref,
                       gen_cmd_ptr->param.pref_sys_chgd.hybr_pref,
                       gen_cmd_ptr->param.pref_sys_chgd.otasp_band,
                       gen_cmd_ptr->param.pref_sys_chgd.otasp_blksys,
                         &gen_cmd_ptr->param.pref_sys_chgd.manual_sys_info,
                         gen_cmd_ptr->param.pref_sys_chgd.domain_pref,
                         gen_cmd_ptr->param.pref_sys_chgd.acq_order_pref,
                         gen_cmd_ptr->param.pref_sys_chgd.tech_pref,
                         gen_cmd_ptr->param.pref_sys_chgd.wlan_scan_pref,
                         gen_cmd_ptr->param.pref_sys_chgd.wlan_bss_type_pref,
                         NULL
                       );
            break;
          case PROT_GEN_CMD_GET_NETWORKS_GW:
            #ifdef MMOC_HYBR_GW_SUPPORTED
            ss_act = sd_ss_ind_user_get_net_gw
                     (
                       gen_cmd_ptr->param.gw_get_net.mode_pref,
                       gen_cmd_ptr->param.gw_get_net.band_pref,
                       NULL
                     );
            #endif
            break;

          case PROT_GEN_CMD_TERM_GET_NETWORKS_GW:
            #ifdef MMOC_HYBR_GW_SUPPORTED
            ss_act = sd_ss_ind_user_term_get_net_gw
                     (
                       NULL
                     );
            #endif
            break;



          default:
            MMOC_MSG_ERROR(" Unknown generic command ", 0, 0, 0);
            ss_act = SD_SS_ACT_CONTINUE;
            break;
        } /* switch (gen_cmd_ptr->cmd_type ) */
 
      } /* case SD_SS_HYBR_HDR: */
      break;
      #endif


      #ifdef MMOC_WLAN_SUPPORTED
#error code not present
      #endif /* MMOC_WLAN_SUPPORTED */

      default:
      {
        MMOC_MSG_ERROR("Incorrect SS", 0, 0, 0 );
      }
    } /* switch ( gen_cmd_ptr->ss ) */

    /* Store command was processed.
    */
    is_cmd_processed = TRUE;

    is_prot_deactivating = mmoc_is_prot_deactivating( gen_cmd_ptr->prot_state,
                                                      gen_cmd_ptr->ss,
                                                      ss_act
                                                    );
  } /* if ( is_prot_active ) */
  else
  {
    /*If this happens, MMOC thinks the protocol on gen_cmd_ptr->ss is active while
    ** the protocol is actually not. So MMoc keeps fwding the generic cmd to an inactive protocol
  */
    MMOC_MSG_ERROR("MMOC and ss %d not in sync",
                     gen_cmd_ptr->ss, 0, 0);
  }/* if ( is_prot_active ) */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Send the Generic command ack to MMoC if trans_id != TRANS_NONE.
  */
  if ( gen_cmd_ptr->trans_id != PROT_TRANS_NONE )
  {
    mmoc_rpt_prot_gen_cmd_cnf( is_cmd_processed,
                               gen_cmd_ptr->trans_id,
                               is_prot_deactivating
                             );
  } /*  if ( gen_cmd_ptr->trans_id != PROT_TRANS_NONE ) */


  return ss_act;

} /* mmoc_proc_prot_gen_cmd() */


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_remove_dup_pref_sys_cmds

DESCRIPTION
  This function will detect duplicate preferred system changed commands from
  CM and remove them from the queue. It will retain the last preferred system
  changed command.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
/*lint -esym(715,mmoc_info_ptr)*/
LOCALF void                            mmoc_remove_dup_pref_sys_cmds
(

       sd_ss_e_type                    ss,
          /* Remove duplicate pref sys commands intended for ss.
          */

       const mmoc_state_info_s_type          *mmoc_info_ptr
          /* Pointer to MMoC's state information.
          */
)
{
  /* Pointer to MMoC's task information.
  */
  mmoc_task_info_s_type          *mmoc_task_ptr = mmoc_get_task_info_ptr();
  mmoc_cmd_msg_s_type            *next_cmd_ptr =
                   (mmoc_cmd_msg_s_type *)q_check( &mmoc_task_ptr->cmd_q );
  mmoc_cmd_msg_s_type            *pref_sys_cmd_ptr = NULL;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Search for first pref_sys_chgd command in the queue.
  */
  while ( next_cmd_ptr != NULL &&
          ( next_cmd_ptr->cmd.name != MMOC_CMD_PROT_GEN_CMD ||
            next_cmd_ptr->param.gen_prot_cmd.cmd_type !=
                                            PROT_GEN_CMD_PREF_SYS_CHGD  ||
            next_cmd_ptr->param.gen_prot_cmd.ss != ss
          )
        )
  {
    next_cmd_ptr = (mmoc_cmd_msg_s_type *)q_next(
                        &mmoc_task_ptr->cmd_q, &next_cmd_ptr->cmd.hdr.link);
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*  Check if pref_sys_chgd cmd was found in the queue.
  */
  if ( next_cmd_ptr == NULL )
  {
    /* None found.
    */
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize for duplicate pref_sys_chgd commands detection.
  */
  pref_sys_cmd_ptr = next_cmd_ptr;
  next_cmd_ptr = (mmoc_cmd_msg_s_type *)
                 q_next(&mmoc_task_ptr->cmd_q, &next_cmd_ptr->cmd.hdr.link);


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Search for duplicate pref_sys_chgd command in the queue.
  */
  for ( ;
        next_cmd_ptr != NULL;
        next_cmd_ptr = (mmoc_cmd_msg_s_type *)
                 q_next(&mmoc_task_ptr->cmd_q, &next_cmd_ptr->cmd.hdr.link)
      )
  {

     if ( next_cmd_ptr->cmd.name == pref_sys_cmd_ptr->cmd.name &&
          next_cmd_ptr->param.gen_prot_cmd.cmd_type ==
                            pref_sys_cmd_ptr->param.gen_prot_cmd.cmd_type &&
          next_cmd_ptr->param.gen_prot_cmd.ss ==
                            pref_sys_cmd_ptr->param.gen_prot_cmd.ss &&
              next_cmd_ptr->param.gen_prot_cmd.param.pref_sys_chgd.pref_reas 
                                              != SD_SS_PREF_REAS_HDR_CDMA_HO &&
          pref_sys_cmd_ptr->param.gen_prot_cmd.param.pref_sys_chgd.pref_reas 
                                              != SD_SS_PREF_REAS_HDR_CDMA_HO

        )
     {
        MMOC_MSG_HIGH(" Duplicate pref_sys_chgd", 0, 0, 0 );
        q_delete( &mmoc_task_ptr->cmd_q, &pref_sys_cmd_ptr->cmd.hdr.link );

        #ifdef FEATURE_MMOC_TMC_HEAP
        mmoc_mem_free( pref_sys_cmd_ptr );
        #else
        cmd_done( &pref_sys_cmd_ptr->cmd.hdr );
        #endif /* FEATURE_MMOC_TMC_HEAP */

        pref_sys_cmd_ptr = next_cmd_ptr;

     }

  }

} /* mmoc_remove_dup_pref_sys_cmds() */
/*lint +esym(715,mmoc_info_ptr)*/


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_event

DESCRIPTION
  This function will process the received event by calling the transaction
  handlers.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
MMEXTN void                            mmoc_process_event
(

       mmoc_state_info_s_type          *mmoc_info_ptr
          /* Pointer to MMoC's state information.
          */
)
{
  mmoc_cmd_msg_s_type  *cmd_ptr = NULL;
  mmoc_rpt_msg_s_type  *rpt_ptr = NULL;

  /* Pointer to MMoC's task information.
  */
  mmoc_task_info_s_type          *mmoc_task_ptr = mmoc_get_task_info_ptr();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Process the events.
  */
  while ( ( mmoc_accept_cmds(mmoc_info_ptr ) &&
          ((cmd_ptr = q_get(&mmoc_task_ptr->cmd_q)) != NULL)) ||
          ((rpt_ptr = q_get(&mmoc_task_ptr->rpt_q)) != NULL)
         )
  {
    if ( cmd_ptr != NULL )
    {
      if(cmd_ptr->cmd.name < MMOC_CMD_MAX)
      {
        #ifndef FEATURE_MMOC_LOW_MEM_TARGET
        /* Invoke the transaction handler normally and dequeue the command.
        */      
        MMOC_PRINT_MSG_HIGH ("Recvd command %d(%s)", cmd_ptr->cmd.name,       
                           mmocdbg_cmd_e_type_string_map [cmd_ptr->cmd.name]);
        #endif /* FEATURE_MMOC_LOW_MEM_TARGET */
      }
      else
      {
        MMOC_MSG_ERROR ("invalid command: %d", cmd_ptr->cmd.name,0,0);
      }
                                            
      mmoc_call_trans_hndlr(cmd_ptr, NULL, mmoc_info_ptr );
      #ifdef FEATURE_MMOC_TMC_HEAP
      mmoc_mem_free( cmd_ptr );
      #else
      cmd_done( &cmd_ptr->cmd.hdr );
      #endif /* FEATURE_MMOC_TMC_HEAP */
      cmd_ptr = NULL;
    }
    else if ( rpt_ptr != NULL )
    {

      if(rpt_ptr->rpt.name < MMOC_RPT_MAX)
      {
        #ifndef FEATURE_MMOC_LOW_MEM_TARGET
        MMOC_PRINT_MSG_HIGH ( "Recvd report %d(%s)", rpt_ptr->rpt.name,                           
                            mmocdbg_rpt_e_type_string_map [rpt_ptr->rpt.name]);
        #endif /* FEATURE_MMOC_LOW_MEM_TARGET */
      }
      else
      {
        MMOC_MSG_ERROR ("invalid report: %d", rpt_ptr->rpt.name,0,0);
      }

      mmoc_call_trans_hndlr(NULL, rpt_ptr, mmoc_info_ptr );
      #ifdef FEATURE_MMOC_TMC_HEAP
      mmoc_mem_free( rpt_ptr );
      #else
      cmd_done( &rpt_ptr->rpt.hdr );
      #endif /* FEATURE_MMOC_TMC_HEAP */
      rpt_ptr = NULL;
    }

  } /* while (  mmoc_accept_cmds() ... ) */

  return;

} /* mmoc_process_event() */

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_get_state_info_ptr

DESCRIPTION
  Return a pointer to the one and only mmoc_state_info object.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
mmoc_state_info_s_type *mmoc_get_state_info_ptr(void)
{
  static mmoc_state_info_s_type   mmoc_state_info;
  /* The one and only mmoc_state_info object */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return &mmoc_state_info;
}

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_task

DESCRIPTION
  This function is the entrance for the MMoC task. This is an infinite loop
  outside of the state machine which handles the input signals.

DEPENDENCIES
  None.

RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/
/*lint -esym(715,ignored)*/
MMEXTN  void                           mmoc_task
(
        dword                          ignored
           /* Parameter received from Rex - ignored.
           */

)
{
  mmoc_state_info_s_type   *mmoc_state_info_ptr = mmoc_get_state_info_ptr();
  rex_sigs_type                   mask = MMOC_CMD_Q_SIG       |
                                         MMOC_RPT_Q_SIG       |
                                         MMOC_DUP_CMD_REM_SIG |
                                         MMOC_SANITY_TIMER_SIG;
  rex_sigs_type                   sigs;

  /* Pointer to MMoC's task information.
  */
  mmoc_task_info_s_type          *mmoc_task_ptr = mmoc_get_task_info_ptr();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Perform MMoC task initialization.
  */
  mmoc_task_init( mmoc_state_info_ptr );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Initialize the DB state to Powerup.
  */
  mmoc_update_db_dmss_state( mmoc_state_info_ptr );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Wait for the start signal from the Main Control task.
  */
  task_start(
   MMOC_RPT_TIMER_SIG,           /* report timer signal for task */
   DOG_MMOC_RPT,                 /* watchdog report */
   &mmoc_task_ptr->rpt_timer     /* pointer to report timer */
  );

  (void) rex_clr_sigs( MMOC_TASK_PTR, TASK_START_SIG );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Send the first watchdog report and set the timer for report interval
  ** and start the MMoC sanity timer.
  */
  mmoc_kick_dog();

  (void) rex_set_timer( &mmoc_task_ptr->rpt_timer,
                        DOG_MMOC_RPT_TIME
                      );



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Initialize System Selection for Main & HDR timer */
  #ifdef FEATURE_USE_TIME_VU
  /* Defining this timer in mmoc_timer_group guarantees it will be called
  ** even when clocks are disabled.  This is necessary when this timer
  ** is used during deep sleep, since it _is_ the event that enables
  ** clocks again.
  */
  timer_def2( &mmoc_task_ptr->ss_main_timer, &mmoc_timer_group );
  timer_def2( &mmoc_task_ptr->ss_hdr_timer, &mmoc_timer_group );
  timer_def2( &mmoc_task_ptr->ss_hybr_wlan_timer, &mmoc_timer_group );
  #else
  clk_def( &mmoc_task_ptr->ss_main_timer );
  clk_def( &mmoc_task_ptr->ss_hdr_timer );
  clk_def( &mmoc_task_ptr->ss_hybr_wlan_timer );
  #endif /* FEATURE_USE_TIME_VU */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Enter the infinite loop and wait for events and process the received
  ** events.
  */
  for( ; ; )
  {
    sigs = mmoc_wait( mask );



    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Check if dup. command removal sig is set.
    */
    if (sigs & MMOC_DUP_CMD_REM_SIG)
    {
      /* Clear the rex sigs associated with sanity timer.
      */
      (void)rex_clr_sigs( &mmoc_tcb, MMOC_DUP_CMD_REM_SIG );

      /* Remove duplicate pref_sys commands for SD_SS_MAIN.
      */
      mmoc_remove_dup_pref_sys_cmds( SD_SS_MAIN, mmoc_state_info_ptr );

    } /* if ( sigs & MMOC_DUP_CMD_REM_SIG ) */


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    /* Check if it was an event.
    */
    if ( sigs & MMOC_CMD_Q_SIG || sigs & MMOC_RPT_Q_SIG )
    {
       /* Clear the rex sigs associated with cmd & rpt.
       */
       (void)rex_clr_sigs( &mmoc_tcb, MMOC_CMD_Q_SIG );
       (void)rex_clr_sigs( &mmoc_tcb, MMOC_RPT_Q_SIG );

       /* Process the received event( command or report ).
       */
       mmoc_process_event( mmoc_state_info_ptr );


       /* Check if MMoC can accept commands at this time.
       */
       if ( mmoc_accept_cmds( mmoc_state_info_ptr ) )
       {
         /* Enable REX signal mask for CMD q.
         */
         mask |= MMOC_CMD_Q_SIG;
       }
       else
       {
         /* Disable REX signal mask for CMD Q.
         */
         mask &= ~MMOC_CMD_Q_SIG;
       }


    } /* if ( sigs & MMOC_CMD_Q_SIG || sigs & MMOC_RPT_Q_SIG) */


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    /* Check if Sanity timer expired.
    */
    if (sigs & MMOC_SANITY_TIMER_SIG)
    {
      /* Clear the rex sigs associated with sanity timer.
      */
      (void)rex_clr_sigs( &mmoc_tcb, MMOC_SANITY_TIMER_SIG );

      /* Sanity timer expired.
      */
      mmoc_sanity_timer_exp( mmoc_state_info_ptr );

    } /* if ( sigs & MMOC_SANITY_TIMER_SIG ) */

  }  /* while ( TRUE ) */

} /* mmoc_task() */
/*lint +esym(715,ignored)*/


#ifdef FEATURE_MMOC_UNIT_TEST
/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_dbg_stop_on_err_fatal

DESCRIPTION
  This function should be invoked by the MMOC_ERR_FATAL when executed on PC
  environment.

DEPENDENCIES

RETURN VALUE
  TRUE  -  Stop execution on ERR_FATAL.
  FALSE -  Continue execution inspite of ERR_FATAL.

SIDE EFFECTS
  None.
===========================================================================*/
MMEXTN  boolean                        mmoc_dbg_stop_on_err_fatal
(

        const void                           *str
           /* Pointer to the error message.
           */

)
{

  /* Pointer to MMoC's task information.
  */
  mmoc_task_info_s_type          *mmoc_task_ptr = mmoc_get_task_info_ptr();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Check if ERR_FATAL message has to be copied to local buffer.
  */
  if ( !mmoc_task_ptr->is_stop_err_fatal)
  {
    MMOC_ASSERT (strlen(str) < MMOC_ERR_FATAL_MSG_MAX_LENGTH);

    /* added to avoid lint error */
    if (strlen(str) < MMOC_ERR_FATAL_MSG_MAX_LENGTH) 
    {
      memcpy( mmoc_task_ptr->err_fatal_msg, str, strlen(str)+1);
    }
  }


  return mmoc_task_ptr->is_stop_err_fatal;

} /* mmoc_dbg_stop_on_err_fatal() */

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_get_last_err_fatal_msg

DESCRIPTION
  This function returns the last MMOC_ERR_FATAL message when executed on PC
  environment.

DEPENDENCIES
  None.

RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/
MMEXTN  void*                           mmoc_get_last_err_fatal_msg
(

        void

)
{
  /* Pointer to MMoC's task information.
  */
  mmoc_task_info_s_type          *mmoc_task_ptr = mmoc_get_task_info_ptr();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Return the pointer to the ERR_FATAL message.
  */
  return mmoc_task_ptr->err_fatal_msg;

} /*  mmoc_get_last_err_fatal_msg() */

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_dbg_stop_err_fatal

DESCRIPTION
  This function would set the behaviour of MMOC_ERR_FATAL macro to stop
  execution when the macro is called.

DEPENDENCIES
  None.

RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/
MMEXTN  void                            mmoc_dbg_stop_err_fatal
(

        void

)
{
  /* Pointer to MMoC's task information.
  */
  mmoc_task_info_s_type          *mmoc_task_ptr = mmoc_get_task_info_ptr();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Set the is_stop_err_fatal to TRUE.
  */
  mmoc_task_ptr->is_stop_err_fatal = TRUE;


  return;

} /* mmoc_dbg_stop_err_fatal() */

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_dbg_continue_err_fatal

DESCRIPTION
  This function would set the behaviour of MMOC_ERR_FATAL macro to continue
  execution when this macro is called.

DEPENDENCIES
  None.

RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/
MMEXTN  void                            mmoc_dbg_continue_err_fatal
(

        void

)
{

  /* Pointer to MMoC's task information.
  */
  mmoc_task_info_s_type          *mmoc_task_ptr = mmoc_get_task_info_ptr();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Set the is_stop_err_fatal to FALSE.
  */
  mmoc_task_ptr->is_stop_err_fatal = FALSE;

  return;

} /* mmoc_dbg_continue_err_fatal() */
#endif


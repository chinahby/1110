#ifndef SRCH_STM_H
#define SRCH_STM_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                S R C H _ S T M     H E A D E R   F I L E

GENERAL DESCRIPTION
  This module contains header information for the search state machine

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

      Copyright (c) 2005 - 2009
                    by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/1x/srch/rel/1h08/inc/srch_stm.h#5 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/22/10   sst     Added Div's DIV_AGC_CK_CMD
10/30/09   bb      Added SRCH_SCLK_COMPLETE_CMD command
04/10/09   tjc     Abort OFREQ searching if we cannot register srch4
03/09/09   vks     Added DIV_AUTO_IS_DATA_CALL_CMD and removed 
                   DIV_AUTO_SCH_ENABLED_CMD for DIV_AUTO
11/04/08   adw     Added AFC_FREQ_TRACK_OFF_RTL_CMD for RTL.
08/28/08   adw     Cleaned up featurization and seperated feature dependencies.
08/18/08   adw     Added FEATURE_SRCH_HAS_NO_TRM to remove TRM for ULC.
07/18/08   adw     Renamed AFC_TCXO_ON/OFF to AFC_FREQ_TRACK_ON/OFF.
06/23/08   aps     Added ABORT_OFREQ_CMD
06/23/08   aps     Added RF_REQ_FAIL_TIMER_CMD
06/20/08   mca     Merge from //depot
04/29/08   sst     Revert previous checkin
04/25/08   sst     Added RF_REQ_FAIL_TIMER_CMD
03/17/08   pa      Added SRCH_STM_GET_BOOLEAN to extract boolean from payload
01/17/08   sst     Add REACQ_TIMER_EXPIRED_CMD
06/01/07   pa      Added AFC_XO_CMD
05/07/07   tjc     Add REQUEST_RF_INFO_CMD
05/01/07   tjc     Add FORCE_OFF, PAUSE and RESUME commands
04/26/07   aps     Change to prevent ofreq srch from happening in ISR context
03/09/07   pa      Renamed rotator push internal commands to be consistent.
02/15/07   rkc     Added SEND_PPM_LIST_UPDATE_CMD, removed
                   ENABLE_FREQ_EST_CMD and DISABLE_FREQ_EST_CMD,
02/15/07   tjc     Add an input to allow the diversity state machine to tell
                    us that the request has failed
02/13/07   pa      Added AFC_RPUSH_SAMPLE_TIMER_CMD and AFC_RPUSH_FLAG_CMD
01/23/07   rkc     Added MDSP_ENABLED_CMD and TCXO_OFF_DONE_CMD.
01/18/07   rkc     Add CGPS commands
01/09/07   sst     Merge from 11.02.02B branch
12/14/06   pa      Removed CLOSE_LOOP_CMD and renamed START_CLOSE_LOOP_CMD
11/30/06   tjc     Added new command to enable/disable FPC for RX diversity
                    autoswitching
11/15/06   aps     Added RX_TUNE_CMD
11/08/06   tjc     Added new command to enable/disable SCH for RX diversity
                    autoswitching
10/18/06   bn      Added new commands to support TT state machine
10/13/06   trc     Changes for new STM grammar
10/11/06   tjc     Add RX diversity autoswitching
09/07/06   tjc     Update the aset only after and abort has been performed if
                   we are not at the home band/chan
08/29/06   rkc     Added LOST_RF_RESOURCE_CMD.
08/21/06   rkc     Added srch_stm_print_debug_trace() and
                   srch_stm_print_int_cmd_q()
08/07/06   rkc     Added REQUEST_SYSTEM_RESTART_CMD.  Removed bail_cmd.
08/04/06   tjc     Remove all single point waits from the traffic state
08/03/06   tjc     Moved traffic signals to internal commmands
07/19/06   trc     Decoupling STM from SRCH
07/11/06   trc     STM interface changes
07/11/06   tjc     Implemented searcher peak module
06/06/06   pa      Added internal commands to support AFC SM.
05/31/06   tjc     Added govern timer cmd
03/22/06   kwo     Added FRAME_STROBE_CMD
01/16/06   awj     Moved to srch_stm interface
11/14/05   awj     Added inactive commands
10/18/05   awj     Lint cleanup
10/04/05   pa      Added ABORT_HOME_CMD
10/03/05   pa      Added DEACTIVATE_ZZ_CMD
09/26/05   awj     Added FEATURE_SRCH_RTL
09/20/05   pa      Added system measurement commands
09/16/05   ejv     Added DIV_RELEASE_CMD.
08/31/05   ejv     Added internal commands for TC SM.
08/29/05   kwo     Merged page match commands
08/18/05   bt      Reworked the no_rf_lock case.
06/27/05   kwo     implementation, first cut
===========================================================================*/

#include "stm.h"

/* Shared internal command queue */
extern  stm_int_q_type   srch_int_cmd_q;

/* Debug logging for all inputs */
#define SRCH_STM_DEBUG_ALL              ((uint32)~0)

/* Extracts boolean payload if payload is available; returns FALSE otherwise */
#define SRCH_STM_GET_BOOLEAN( payload ) ( ((payload) == NULL)     ?          \
                                          FALSE                   :          \
                                          *((boolean *) (payload)) )

/*-------------------------------------------------------------------------
      Internal Command IDs
-------------------------------------------------------------------------*/

typedef enum
{
  INTERNAL_CMD_BLOCK_START = 100,

  /* Common Internal Commands */
  SRCH_START_CMD,
  SRCH_STOP_CMD,
  SRCH_OFFLINE_CMD,
  SRCH_DUMP_CMD,
  SRCH_LOST_DUMP_CMD,
  ROLL_CMD,
  SRCH_TL_TIMER_CMD,
  SRCH_OFREQ_UPDATE_TIMER_CMD,
  RSSI_TIMER_CMD,
  TUNE_DONE_CMD,
  RX_TUNE_CMD,
  #ifdef FEATURE_SYS_MEAS
  SYS_MEAS_CMD,
  #endif /* FEATURE_SYS_MEAS */
  AFC_RPT_TIMER_CMD,
  FRAME_STROBE_CMD,
  SYNTH_FRAME_STROBE_CMD,
  GOVERN_TIMER_CMD,
  SRCH_PEAK_CMD,
  REQUEST_SYSTEM_RESTART_CMD,
  #ifndef FEATURE_SRCH_HAS_NO_TRM
  LOST_RF_RESOURCE_CMD,
  #endif /* FEATURE_SRCH_HAS_NO_TRM */
  MDSP_ENABLED_CMD,
  FREQ_TRACK_OFF_DONE_CMD,

  /* Idle State Internal Commands */
  ABORT_CMD,
  START_SLEEP_CMD,
  REACQ_CMD,
  QPCH_REACQ_CMD,
  QPCH_REACQ_DONE_CMD,
  OFREQ_CMD,
  PRIO_CMD,
  WAKEUP_DONE_CMD,
  WAKEUP_NOW_CMD,
  REACQ_DONE_CMD,
  TL_DONE_CMD,
  QPCH_DEMOD_DONE_CMD,
  WAKEUP_CMD,
  CX8_ON_CMD,
  FING_SLEW_DONE_CMD,
  PAGE_MATCH_CMD,
  ADJUST_TIMING_CMD,
  FLUSH_FILT_CMD,
  FAKE_REACQ_CMD,
  FAKE_QPCH_REACQ_CMD,
  QPCH_CONTINUE_CMD,
  SNOOZE_CMD,
  RF_TUNE_TIMER_CMD,
  OFREQ_DONE_SLEEP_CMD,
  OFREQ_HANDING_OFF_CMD,
  OFREQ_HANDOFF_CMD,
  OFREQ_HANDOFF_DONE_CMD,
  NSET_UPDATE_CMD,
  XFER_FROM_QPCH_CMD,
  CONFIG_RXC_CMD,
  LOG_STATS_CMD,
  RECORD_DONE_CMD,
  FREQ_TRACK_DONE_CMD,
  DEMOD_DONE_CMD,
  NO_RF_LOCK_CMD,
  ORIG_PENDING_CMD,
  REBUILD_LISTS_CMD,
  HO_REQUEST_CMD,
  SCAN_ALL_CMD,
  SCAN_ALL_RPT_CMD,
  ONLINE_CMD,
  BC_INFO_CMD,
  CDMA_CMD,
  CDMA_RSP_CMD,
  IDLE_CMD,
  IDLE_RSP_CMD,
  TIMED_CMD,
  OFREQ_SUSPEND_CMD,
  RESELECTION_CHECK_CMD,
  OCONFIG_NBR_FOUND_CMD,
  SLEEP_OK_CMD,
  SYS_MEAS_TIMER_CMD,
  ABORT_COMPLETE_CMD,
  RF_GRANTED_CMD,
  DOZE_CMD,
  SLEEP_TIMER_EXPIRED_CMD,
  TILL_REACQ_TIMER_EXPIRED_CMD,
  DEACTIVATE_ZZ_CMD,
  ABORT_HOME_CMD,
  DELAYED_ASET_CMD,
  ABORT_OFREQ_CMD,

  /* Inactive State Internal Commands */
  ENTER_AMPS_CMD,
  ENTER_CDMA_CMD,
  SRCH4_MDSP_REG_CMD,
  SRCH_SCLK_COMPLETE_CMD,
  WAIT_DONE_CMD,

  /* CDMA State Internal Commands */
  ENTER_ACQ_CMD,
  FIRST_SYS_MEAS_CMD,
  NEXT_SYS_MEAS_CMD,
  RF_REQ_FAIL_TIMER_CMD,

  /* ACQ State Internal Commands */
  TUNE_NOW_CMD,
  RSSI_CK_TIMER_CMD,
  FREQERR_CK_TIMER_CMD,
  VERIFY_TO_TIMER_CMD,
  PULLIN_TO_TIMER_CMD,
  ACQ_TO_TIMER_CMD,
  SRCH4_MDSP_APP_EN_CMD,
  SRCH4_MDSP_APP_DIS_CMD,
  AFC_ACQ_SUCCESS_CMD,
  AFC_START_VCO_PULLIN_CMD,

  /* Comman State Machine Internal Commands */
  TCXOMGR_CMD,
  RAND_BITS_CMD,

  /* Diversity State Machine Internal Commands */
  DIV_REQUEST_CMD,
  DIV_INIT_CMD,
  DIV_READY_CMD,
  DIV_PAUSE_CMD,
  DIV_STOP_CMD,
  DIV_RELEASE_CMD,
  DIV_AGC_CK_CMD,

  /* Traffic GPS State Machine Internal Commands */
  TCG_VISIT_REQUEST_CMD,
  TCG_RETRY_TIMER_CMD,
  TCG_MC_RESPONSE_CMD,
  TCG_CANCEL_VISIT_REQUEST_CMD,
  TCG_VISIT_DONE_CMD,

  /* AFC State Machine Internal Commands */
  AFC_START_ACQ_CMD,
  AFC_PULL_IN_DONE_CMD,
  AFC_PULL_IN_FAIL_CMD,
  AFC_NO_CONTROL_CMD,
  AFC_FAST_CMD,
  AFC_SLOW_CMD,
  AFC_SRL_CMD,
  AFC_XO_CMD,
  AFC_BIG_ROT_ERR_CMD,
  AFC_SMALL_ROT_ERR_CMD,
  AFC_LOG_TIMER_CMD,
  AFC_ACCESS_CMD,
  AFC_ACCESS_EXIT_CMD,
  AFC_TRAFFIC_CMD,
  AFC_CDMA_CMD,
  AFC_FREQ_TRACK_OFF_CMD,
  AFC_FREQ_TRACK_OFF_RTL_CMD,
  AFC_FREQ_TRACK_ON_CMD,
  AFC_TC_EXIT_CMD,
  AFC_START_RV_TRACK_CMD,
  AFC_OFF_CMD,
  AFC_SANITY_TIMER_CMD,
  AFC_ROT_PUSH_SAMPLE_TIMER_CMD,
  AFC_ROT_PUSH_FLAG_CMD,

  /* Traffic State Internal Commands */
  ACTION_TIME_CMD,
  DO_SRCH_CMD,
  REF_TIMER_CMD,
  CTR_TIMER_CMD,
  PPSMM_TIMER_CMD,

  /* System measurement Internal commands */
  RF_LOCKED_CMD,
  REGISTER_DONE_CMD,
  AGC_TIMER_CMD,
  MEAS_NEXT_CMD,
  MEAS_DONE_CMD,

  /* this is the DO timer transfer internal commands */
  TT_READY_CMD,
  TT_SLAMCOMB_CMD,

  /* RX diversity Autoswitching Internal Commands*/
  DIV_AUTO_ENABLE_CMD,
  DIV_AUTO_DISABLE_CMD,
  DIV_AUTO_PAUSE_CMD,
  DIV_AUTO_RESUME_CMD,
  DIV_AUTO_FORCE_OFF_CMD,
  DIV_AUTO_IS_DATA_CALL_CMD,
  DIV_AUTO_FPC_ENABLED_CMD,
  DIV_AUTO_AUTOSWITCH_CMD,

  /* CGPS Commands */
  REQUEST_1X_PPM_OP_CMD,
  RELEASE_1X_PPM_OP_CMD,
  REQUEST_1X_SYNC80_DATA_CMD,
  GET_SRCH_WINDOW_CENTER_CMD,
  GET_REF_PN_CMD,
  GET_ACT_CAND_SETS_CMD,
  ENABLE_TTD_RECORDING_CMD,
  DISABLE_TTD_RECORDING_CMD,
  REQUEST_SYSTIME_UNC_CMD,
  REQUEST_TTD_CMD,
  CANCEL_TTD_REQUEST_CMD,
  SEND_PPM_LIST_UPDATE_CMD,
  REQUEST_RF_INFO_CMD,

  LAST_INT_CMD,
} int_cmd_code_type;

/*===========================================================================

FUNCTION SRCH_STM_GET_MSG_BUF

DESCRIPTION    This function allocates a message buffer including header
               information for the state machine driver.

DEPENDENCIES   None

RETURN VALUE   Pointer to the payload field - header data will be in front,
               and clobbering it will have unsavory effects.

SIDE EFFECTS   None

===========================================================================*/
extern void *srch_stm_alloc_msg_buf
(
 stm_state_machine_type *sm,       /* state machine command is for */
 stm_group_type         *group,    /* group that the state machine is in */
 int_cmd_code_type       id,       /* internal command ID */
 uint32                  size      /* size of internal command payload */
);

/*===========================================================================

FUNCTION SRCH_STM_DEBUG_HOOK

DESCRIPTION    This function is called every time an input is processed.
               The debug_info structure pointer passed-in provides

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
extern void srch_stm_debug_hook(user_debug_info_type *debug_info);

/*===========================================================================

FUNCTION SRCH_STM_GET_MSG_BITMASK

DESCRIPTION    This function returns a 32 bit bitmask with a 1 in the
               position of the index of the passed-in message.

DEPENDENCIES   None

RETURN VALUE   If the passed-in message resides between index 0 and 31,
               a bitmask equivalent to (1 << index) will be returned.  Upon
               error or index out of range, the return value will be 0.

SIDE EFFECTS   None

===========================================================================*/
extern uint32 srch_stm_get_msg_bitmask
(
  stm_state_machine_type const *sm,
  stm_input_type                message_id
);

/*===========================================================================

FUNCTION       SRCH_STM_EXT_CMD_SIG_MAPPER

DESCRIPTION    This function maps signals to state machine inputs

DEPENDENCIES   None

RETURN VALUE   TRUE if mapped, FALSE otherwise

SIDE EFFECTS   sig_cmd structure appropriately populated upon success

===========================================================================*/
extern boolean srch_stm_ext_cmd_sig_mapper
(
  rex_tcb_type *tcb,          /* tcb of the process with the signal */
  rex_sigs_type sig,          /* signal which is set */
  stm_state_machine_type *sm, /* ptr to the sm which will handle the ext cmd */
  stm_sig_cmd_type  *sig_cmd  /* ptr to the sig_cmd structure to populate */
);

/*===========================================================================

FUNCTION       SRCH_STM_EXT_CMD_ERROR_HANDLER

DESCRIPTION    This function handles the case where an external command
               wasn't processed in the state machine that received it.
               As a recourse, we'll simply reply back to the caller with
               a SRCH_ERR_S to unblock them, since they're probably waiting
               on an ACK to the original command.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
extern void srch_stm_ext_cmd_error_handler
(
  stm_sig_cmd_type *sig_cmd  /* unprocessed sig_cmd structure ptr */
);

/*===========================================================================

FUNCTION SRCH_STM_PRINT_DEBUG_TRACE

DESCRIPTION    This function prints the diag messages of the entire
               stm_debug_trace structure

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
extern void srch_stm_print_debug_trace( void );

/*===========================================================================

FUNCTION SRCH_STM_PRINT_INT_CMD_Q

DESCRIPTION    This function prints the diag messages of the internal command
               queue entries from the read index to the write index.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
extern void srch_stm_print_int_cmd_q( void );

#endif /* SRCH_STM_H */

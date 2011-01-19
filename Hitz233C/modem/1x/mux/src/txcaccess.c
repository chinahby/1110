/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

          C D M A   T R A N S M I T   A C C E S S    M O D U L E

GENERAL DESCRIPTION
  Contains items associated with the access channel

EXTERNALIZED FUNCTIONS
  txc_acc_probe_resume - resumes the transmission of next Access probe
  txc_access_init - initialization section for access channel
  txc_access_att - state where access channel transmission is transmitted
  txc_hyperspace - performs jump to hyperspace for Transmit task
  txc_access_idl - Access channel idle state
  txc_flush_enc - Flush encoder with zeros at the entrance of CDMA state so 
                  encoder will output expected power level for first probe
                  after power up

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 1991 - 2010 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //source/qcom/qct/modem/1x/mux/rel/ulc/src/txcaccess.c#5 $ $DateTime: 2010/03/23 16:20:46 $ $Author: joshuam $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/23/10   jtm     Turn off RF in Hold Enc Clock state to avoid leaving RF on
                   while tuning to a different frequency.
08/14/08   jtm     Removed static declaration for tt_substate_type sub_state.
                   Also added debug messages to cmd processing states and did 
                   some code cleanup.
07/16/08   jtm     Remove FEATURE_IS2000_REL_C_DV, FEATURE_IS2000_REL_D_DV and 
                   FEATURE_DELAYED_TX_CLOCK_SHUTDOWN. Removed txc_flush_enc for
                   ULC effort in reducing ROM. Reduced debug string sizes and
                   eliminated chunks of commented out code.
05/23/08   vlc     Fixed compiler warnings/errors when feature jump enhancement
                   is turned off.                              
04/02/07   vlc     Added new PCG define for bin0 ENC ISRs specifically for loop
                   back calls.  Moved default PCG to 1 for bin0 ENC ISRs for 
                   all other calls.
02/23/07   vlc     Fixed compiler warnings/errors when SCH, Rel A, and P2 
                   features are turned off.
01/25/07   vlc     Allow the processing of TXC_ACC_STOP_F in hold encoder clock
                   state.  Allow TXC to sleep when ENC clocks are turned off.
01/22/07   trc     Parameterize the PCG for bin0 and bin3 ENC ISRs
11/14/06   vlc     In function txc_access_idl_init(), make sure 1X has primary
                   chain before turning off RF (to work with SHDR.)
10/30/06   bn      Added code to support DO/UMTS to 1x handoff
10/26/06   vlc     Changed TXC time line for traffic state.  Moved 1st ENC ISR
                   to bin0, PCG1.  Moved 2nd ENC ISR to bin3, PCG2.
09/19/06   vlc     Fixed lint errors.
09/05/06   vlc     Implemented TX gain enhancements to increase digital gain
                   resolution.
08/15/06   vlc     In function txc_flush_enc(), added call to enc_fch_rc().
08/03/06   vlc     Added function txc_flush_enc(). 
06/09/06   trc     Remove T_MSM6700 featurization
05/16/06   vlc     Reset tcb_ptr after notifying MC of the event to prevent
                   false notification in the future.
01/17/06   rkc     Call srch_mux_set_tx_adj(0x7F), was 0, to set gain
                   adjustments after last frame has been sent (R-EACH).
11/07/05   rkc     Add call to enc_fch_enable() in txc_access_cmd() when
                   TXC_ACCESS_F is received.  This takes care of NCC to
                   OCC ECAM redirection test case.
09/27/05   rkc     Remove redundant txc_hold_enc_clk() function prototype.
09/15/05   rkc     Check boolean hold_enc_clk_on is TRUE before transitioning to
                   TXC_HOLD_ENC_CLK_S.
09/08/05   ejv     Use new srch_mux interface calls.
08/11/05   ddh     Added change to correct access proble issue for 7500
08/09/05   bn      Fixed the RA trigger logic for number of RA = 2
07/26/05   vlc     Applied code review comments for RL jump to hyperspace 
                   enhancement.  Removed code to force RN value to 0 in 
                   txc_reach_msg_prep().
07/25/05   rkc     Fixed lint errors.
06/06/05   vlc     In functions txc_access_att() and txc_reach_access_att(), 
                   set jump source prior to transitioning to hold enc clock 
                   state.
05/23/05   vlc     Changed FEATURE_IS2000_REL_D_JUMP to FEATURE_RL_JUMP_ENHANCEMENT.
                   Implemented multiple changes for feature 
                   FEATURE_RL_JUMP_ENHANCEMENT:
                   Needed to NOT allow sleep while holding ENC clocks ON so 2nd 
                   message on ACCESS channel can be received by Base station.
                   Need to add RN value to TXC_JUMP_F command.
                   In function txc_reach_access_att(), put back code under feature
                   FEATURE_DELAYED_TX_CLOCK_SHUTDOWN.  This code does not belong in
                   FEATURE_TX_ACCESS_POWER_FIX, which had been removed.                
05/16/05   vlc     Removed feature FEATURE_TX_ACCESS_POWER_FIX which was a software
                   work around and was no longer needed for Jaguar and newer targets.
                   This feature was already turned off in Jaguar and newer targets.
05/12/05   vlc     Mainlined SRCH_HAS_TRK_LO_CNTL feature.
04/06/05   vlc     Added extra debug statements.  Moved code to set jump source 
                   inside txc_hold_enc_clk_init().  Changed TXC_JUMP_CMD to
                   TXC_JUMP.
03/15/05   vlc     Fixed mismatch of {}.  Added function prototype for 
                   txc_hold_enc_clk_init().
03/14/05   vlc     Fixed compiler warnings for MSM6700.
03/14/05   vlc     Added logic to eliminate Jump to Hyperspace from ACCESS state to
                   TRAFFIC state (still under feature FEATURE_IS2000_REL_D_JUMP.)
03/14/05   vlc     Merged in changes for MSM7500.  Changed T_QUASAR_BB to T_IO_CARD.
                   (dlb')
03/04/05   vlc     Added support for Fast Call Set Up - New Jump to 
                   Hyperspace command feature.
                   Changed txc_access_init() to txc_access_idl_init() to clarify
                   that this function is called upon entering TXC_ACC_IDLE_S.
03/07/05   bn      Cleaned up debug F3 messages
12/14/04   ljl     Added event report for R-EACH.
10/22/04   ljl     Called enc_interrupt_slot() to set txc_reach_interrupt_slot
                   if MSM6700 is undefined.
09/29/04   sr      Included rf.h and put Rel DV include files under feature
09/28/04   sr      Merged in changes from 6550 2009 build
09/27/04   ljl     Replaced pdch with rev_pdch.
09/01/04   jrp     Added back pre-6700 interfaces.
08/20/04   vlc     Removed MCC_TXC_JUMP_DONE_SIG, no longer needed by MC task.
08/16/04   sr      Enabled DV app if F-PDCH is included in TXC_TC_F
08/16/02   ljl     Rel D reverse channel PLT.
08/10/04   vlc     Set MCC_TXC_JUMP_DONE_SIG when TXC is done with Jump to Hyperspace, 
                   ready to enter traffic state.
07/22/04   bn      Replaced the call of srch_set_tx_rate_adj by rpc_set_tx_rate_adj
07/21/04   bn      For REACH, tell the MDSP to load the frame off immediately
07/20/04   bn      With the new MOD INT, no need to install the access IST 
                   in the txc_access_init function
07/15/04   sr/ll   Featurized out rf driver calls under T_QUASAR_BB
07/07/04   bn      Fixed the wrong featurization in txc_access_att(). REACH should 
                   be used regardless of FEATURWE_TX_ACCESS_POWER_FIX.
06/29/04   jrp     Pass-through gains, and modulator interrupt changes.
06/16/04   sr      Merged in changes from 6500 5004 build
05/21/04   jyw     introduce new RF PA backoff interface
05/18/04   jrp     Change order of ISR handler registration, and interrupt.
04/20/04   jrp     Fixed lint warnings.
03/17/04   jrp     Turn off pich and sch channels and set TX_RATE_ADJ and
                   TX_GAIN_ADJ to zero between R-EACH probes.
03/05/04   sr/bkm  Moved initializing the frame offset from MC to TXC_TC_F
02/11/04   jrp     Fixed compilation error when FEATURE_TX_ACCESS_POWER_FIX
                   is undefined.
11/05/03   sst     Changed trk_lo_adj wakeup/sleep calls to initiate change to
                   move trk_lo_adj functionality into a single module rather
                   than macro calls throughout the code base
10/30/03   jrp     Fixed compiler warnings.
09/15/03   jrp     For FEATURE_DELAYED_TX_CLOCK_SHUTDOWN, calls shutdown
                   function in task context rather than interrupt context.
09/09/03   dna     Merged porting changes for MSM6500
08/25/03   vlc     Removed shared rate_set from txc_so (txc_so_type) to avoid 
                   confusion between FCH and DCCH rate sets.  Now calculate
                   rate set from RC as it is needed.
06/24/03   jrp     Changed calls to srch_set_tx_rate_adj() to call instead
                   rpc_set_tx_rate_adj() so that TX_RATE_ADJ is written in the
                   appropriate units - either 1/12 or 1/10dB.
05/23/03   jrp     Code review changes for delayed TX clock shutdown.  
                   Fixed CAI_EACH_BA_?_ID and CAI_ACC_?_ID to increment at most 
                   once for each access attempt.  
                   Fixed ASSERT for REACH attempts based on RACH length limit.  
                   Added random selection of REACH channel.
04/15/03   jrp     Support for FEATURE_DELAYED_TX_CLOCK_SHUTDOWN.
03/04/03   bkm     Added support for REV_FCH_GATING event.
02/10/03   jrp     Removed lint warnings.
02/10/03   jrp     Implemented persistence delay for REACH.  Made backoff and
                   probe backoff standard compliant.  Corrected LOG_EA_PROBE
                   to give 1-based max probes per sequence, instead of 0-based.
12/03/02   bt      rename ulpn_advance to ulpn_adjust.
11/21/02   jrp     Featurized txc_copy_reach_parameters.
11/15/02   jrp     Added support for APHO.  Added band class to LOG_EA_PROBE_C
                   packet.  Added support for REACH counters.  Made REACH TA
                   counter standard compliant.
11/01/02   jrp     Fixed lost REACH probes due to reading time earlier than 
                   interrupt time, and programming the long code accordingly.
08/23/02   jrp     Changed REACH signaling logging's length calculation to
                   allow for extended message lengths.  Changed 
                   txc_reach_msg_prep() to allow for extended message sizes.
08/20/02   jrp     Support for logging REACH message and probes, log 
                   codes D7 and BD.
07/26/02   tc      Supported REACH 5ms and 10 ms frame.
07/16/02   bgc     Featurized txc_so.rev_fch.included in FEATURE_IS2000_P2.
07/01/02   tc      Supported REACH interference correction calculation.
06/24/02   tc      Supported REACH preamble and data transmission output power.
06/12/02   hrk     If FCH/DCCH information not included, then setting 
                   txc_so.rev_fch.included / txc_so.rev_dcch.included to FALSE.
06/07/02   hrk     Added VP2 support.
05/28/02   lcc     Turns off pilot gating when getting on TC just in case it
                   was gated from last call.
05/14/02   tc      Set txc_reach_si_include flag to true.
04/10/02   tc      Supported REACH pilot reporting command TXC_REACH_PROBE_F.
04/04/02   tc      Set REACH transmitting buffer length to message length.
04/03/02   tc      Supported REACH SI bits, txc_reach_probe_prep().
04/03/02   tc      Supported REACH Ext_Msg_length and Msg_Length.
04/02/02   tc      Supported REACH 5ms and 10ms frame size information bits.
03/08/02   bn      Added logic to log interference-correction 
03/04/02   bgc     Changed features to IS2000_R_SCH where appropriate.
02/28/02   hrk,fs  Replaced PILOT_FCH_9600 with PILOT_FCH 
02/25/02   hjr     Fixed featurization of FEATURE_IS2000_SCH with 
                   FEATURE_IS2000_DS_RS2 on.
02/22/02   tc      Supported FEATURE_IS2000_REL_A_CC.
02/11/02   tc      Supported Reverse Enhance Access Channel.
01/15/02   hrk     Changed RF_ENABLE_PWR_WAIT to RF_ENABLE_TX_PWR_WAIT when using
                   FEATURE_RF_WAIT_CHANGES.
01/15/02   hrk     Setting TX_RATE_ADJ register to 0 before access probe seq.
01/12/02   sst     Modified to use the new srch_* files that replaced srchdrv.c
                     Includes a function name change:
                       srch_get_ref_time -> srch_scomb_get_ref_time
01/03/02   bgc     Added SCH featurization to turn off for MSM6000.
11/15/01   lcc     In txc_access_cmd, added enabling of FCH when TXC_TC_F is
                   received with FCH included; otherwise FCH is disabled when
                   DCCH is disabled for 5000 and 5105.
11/02/01   lcc     In txc_access_cmd, when TXC_TC_F is received, added disabling
                   of FCH or DCCH appropriately when the channel is not included.
09/14/01   bn      Fixed P2 featurization 
08/22/01   bn      Support DCCH in FTM mode
07/12/01   vlc     Changed FEATURE_RC4_SCH to FEATURE_DS_IS2000_RS2 in order
                   to eliminate FEATURE_RC4_SCH.
06/28/01   vlc     Fixed r-sch parameter verification in PLT.
06/25/01   lcc     Properly initialize txtc.cur_dsch (dedicated signalling channel)
                   on TXC_TC_F command.
06/15/01   day     Mainlined FEATURE_RLGC, FEATURE_IS95B, FEATURE_IS95B_OLPEC,
                   FEATURE_IS95B_ACC_PRB_BKOFF_DLY, FEATURE_IS2000
                   Delete FEATURE_SPECIAL_MDR
06/12/01   vlc     Fixed a typo (rev_sch_rc was inadvertently changed to
                   vrev_sch_rc).
05/23/01   fas     Add hooks for rf_pa_backoff.c
05/18/01   kk      RS2 TDSO support
05/14/01   jal     Corrected log buffer allocation in tx_log_ac_msg() to ask
                   for exactly as much space as it needs.
05/11/01   lcc     In txc_access_cmd, when TXC_TC_F is received, added turning
                   off of rev. FCH 8th rate gating when it is disabled.
04/26/01   vlc     Added SCH RC4 support.
04/20/01   sr      Merged in from MSM5100 archive
04/19/01   tc      Merged with MSM_MUX1X.01.00.01
           fc      Added support of events report: Access denied.
                   Renamed mcc_report_event_msg_txed to
                   mclog_report_event_msg_txed.
04/18/01   sr      Merged with MSM_MUX1X.01.00.00
           lcc     Added support for FEATURE_IS2000_P2
03/27/01   vlc     Merged MSM_MUX1X.00.00.17.
           fc      Added support of events report: Message transmitted.
03/14/01   tc      Loaded enc_stmr_cmd at enc_user_pn_mask and enc_user_pn_state.
03/01/01   tc      Merged MSM_MUX1X.00.00.15.
02/16/01   bgc     Changed FEATURE_FACTORY_TESTMODE to enable a runtime switch.
01/12/01   tc      Merged MSM_MUX1X.00.00.06.
01/03/01   tc      Loaded BTF value by sending MOD_STMR_CMD.
12/15/00   tc      Fixed long code state loading.
11/09/00   tc      Defined feature T_MSM_5100.
11/08/00   tc      Supported MSM5100 modulator driver interface.
12/07/00   bgc     Set sch_rate for a PLT SCH channel setup.
11/04/00   fc      Changed the RT to 0 to BKOFF + 1 for IS-95B mobile with
                   which the base station supports only one access channel.
11/10/00   jc      This file was part of txc.c. See txccommon.c for full
                   change history

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include <memory.h>
#include "comdef.h"
#include "customer.h"
#include "qw.h"
#include "queue.h"
#include "rex.h"
#include "enc.h"
#include "task.h"
#include "assert.h"
#include "tx.h"
#include "txc.h"
#include "err.h"
#include "crc.h"
#include "bit.h"
#include "log.h"
#include "cai.h"
#include "tramp.h"
#include "srch_mux.h"
#include "ran.h"
#include "msg.h"
#include "ts.h"
#include "parm.h"
#include "rfm.h"
#include "ulpn.h"
#include "rxc.h"

#include "msm_drv.h"

#ifdef FEATURE_PLT
#error code not present
#endif

#include "ts.h"
#include "mccdma.h"
#include "rlgc.h"

#include "txccommon.h"
#include "txcmux.h"

#include "txctraffic.h"
#include "txcaccess.h"
#include "txcso.h"

#ifdef FEATURE_FACTORY_TESTMODE
#include "test_mc.h"
#endif
#include "event.h"
#include "mclog.h"
#include "rfnv.h"
#ifdef FEATURE_IS2000_REL_A_CC
#include "mux_logtypes.h"
#endif /* FEATURE_IS2000_REL_A_CC */

#include "muxmdsp.h"
#include "txcmodint.h"

#include "rf.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

#ifdef FEATURE_IS2000_REL_A_CC

// static boolean txc_reach_on = TRUE;
static boolean txc_reach_access;
static boolean txc_reach_si_include = TRUE;
static boolean txc_reach_first_frame_input_data;
static boolean txc_reach_program_preamble_prepare;
static boolean txc_reach_program_data_prepare;

static word txc_reach_preamble_frame_offset = 0;
static word txc_reach_data_frame_offset = 0;
static word txc_reach_frame_offset = 0;

static word txc_reach_preamble_count = 0;
static word txc_reach_data_count = 0;

// static word txc_reach_preamble_pcg_intr_cnt = 0;
// static word txc_reach_data_pcg_intr_cnt = 0;

static word txc_reach_intr_cnt = 0;

// (bkoff+rp)*(each_slot)
static word txc_reach_countdown = 0;

// (bkoff_frame_pcg/each_slot) remainder
static word txc_reach_count = 0;

// (bkoff)*(each_slot) + (rp)*(each_slot) + count
static word txc_reach_count_pcg = 0;

// ( (bkoff+rp)*(each_slot) + count )/16
static word txc_reach_count_frame = 0;

// ( (bkoff+rp)*(each_slot) + count ) mod 16
static word txc_reach_pcg_start = 0;


static word txc_reach_slot_offset = 0;

// SLOT LENGTH = 4 PCG = 5 ms
static word txc_reach_slot_length = 4;

static word txc_reach_slot_offset1 = 0;
static word txc_reach_slot_offset2 = 0;

// REACH Sub-attempt number, incremented with each access probe handoff
static byte txc_reach_sub_att_number = 0;

// PREAMBLE LENGTH = 4 PCG = 5 ms
static word txc_reach_preamble_length = 4;

// FRAME LENGTH = 20 ms
static word txc_reach_frame_length = ENC_SCH_FRAME_SIZE_20_V;


static word txc_reach_frame_size;

static int16 txc_reach_pch_adj_gain;

static byte txc_reach_rlgain_common_pilot;
static byte txc_reach_ic_thresh;
static byte txc_reach_ic_max;

static byte txc_reach_rate_word;

#define TXC_REACH_FRAME_1       1;
#define TXC_REACH_FRAME_2       2;
static byte txc_reach_input_frame;

// Number of 20ms duration frame count.
static word txc_reach_20ms_frame_cnt;

// Number of frame count for 5ms, 10ms, 20ms frame size.
static word txc_reach_frame_cnt;

// Number of psist failures for this probe
static word txc_reach_psist_delay;

// 10ms frame, 19200bps, 192 bits/frame, 24 bytes/frame
// 10ms frame, 38400bps, 384 bits/frame, 48 bytes/frame
//  5ms frame, 38400bps, 192 bits/frame, 24 bytes/frame
static byte txc_reach_frame_1_bin_0[48];
static byte txc_reach_frame_1_bin_1[48];
static byte txc_reach_frame_1_bin_2[48];
static byte txc_reach_frame_1_bin_3[48];
static byte txc_reach_frame_2_bin_0[48];
static byte txc_reach_frame_2_bin_1[48];
static byte txc_reach_frame_2_bin_2[48];
static byte txc_reach_frame_2_bin_3[48];

static qword txc_reach_current_frame_number;
static qword txc_reach_current_frame_pcg;

static qword txc_reach_next_frame_pcg;
static qword txc_reach_bkoff_frame_pcg;

static qword txc_reach_preamble_frame_pcg;
static qword txc_reach_data_frame_pcg;

static qword txc_reach_frame_pcg;


static enc_status_interrupt_slot_type txc_reach_interrupt_slot;
/* for logging REACH channel info */
static mux_reach_frame_info_log_type  txc_reach_frame_info_log;

#endif // FEATURE_IS2000_REL_A_CC


#if defined(FEATURE_DIAG_V7_LOG_IFACE)
  typedef struct
  {
    log_desc_type desc;             /* log descriptor for logging services */
    log_acc_info_type acc;          /* Access channel information */
  }txc_log_acc_type;
#endif /*FEATURE_DIAG_V7_LOG_IFACE*/

byte after_fo_handoff_count_isrs = NO_OF_ISRS_AFTER_FO;

/* Specific variables for the Access Channel */
txac_type txac;

#define TXC_ACCESS_SLIP 1

#if !defined(FEATURE_DIAG_V7_LOG_IFACE)
  log_acc_info_type acc_log;
#else
  #define                                 TXC_NUM_ACC_INFO_BUFS 4
  txc_log_acc_type txc_log_acc_info_pool[ TXC_NUM_ACC_INFO_BUFS ];
  q_type txc_log_acc_q;              /* logging access probe information */
#endif /*FEATURE_DIAG_V7_LOG_IFACE*/

#ifdef FEATURE_IS95B_ACCESS_PROBE_HO
  LOCAL void txc_acc_probe_suspend(txc_cmd_type * cmd_ptr);
  boolean txc_suspend_after_ta = FALSE;
#endif /* FEATURE_IS95B_ACCESS_PROBE_HO */

/* Frequency error which is used to fix non-linear access probes
 * in ideal lab conditions.  This error adds the value 3 to the LSB's
 * of the frequency tracking loop PDM. */
#define TXC_ACCESS_FREQ_ERR (3<<7)

#define TXC_ACC  88 /* Can send 88 information bits each Access Channel frame */

#define TXC_PWR_MAX 0x80           /* Tx gain adjust maximum value */

#ifdef FEATURE_RL_JUMP_ENHANCEMENT
/* Duration of wait in TXC_HOLD_ENC_CLK_S state before timing out and turning off ENC clocks.
** Delay in units of 20ms. 100 = 2secs.
*/
#define TXC_HOLD_ENC_CLK_WAIT_DURATION (100)
typedef struct 
{
  boolean txc_sleep_allowed;
  volatile word counter; /* accessed in interrupt and task context */ 

} txc_delay_enc_shutdown_type;
static txc_delay_enc_shutdown_type txc_delay_enc_shutdown = {TRUE, 0};

#endif /* FEATURE_RL_JUMP_ENHANCEMENT */

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

#if defined(FEATURE_IS95B_PILOT_RPT)
  void txc_acc_probe_prep( txc_cmd_type *cmd_ptr );
#endif /* FEATURE_IS95B_PILOT_RPT */
void txc_acc_msg_prep( txc_cmd_type *cmd_ptr );
void txc_access_isr
(
  uint32
);

#ifdef FEATURE_IS2000_REL_A_CC
// TXC_REACH_ON
void txc_reach_msg_prep( txc_cmd_type *cmd_ptr );

void txc_reach_isr
(
  uint32
);

void txc_reach_pcg_isr( void );
#endif // FEATURE_IS2000_REL_A_CC

#ifdef FEATURE_IS2000_REL_A_CC
#if defined(FEATURE_IS95B_PILOT_RPT)

  void txc_reach_probe_prep( txc_cmd_type *cmd_ptr );

#endif /* FEATURE_IS95B_PILOT_RPT */
#endif // FEATURE_IS2000_REL_A_CC

#ifdef FEATURE_RL_JUMP_ENHANCEMENT

void txc_hold_enc_clk_isr ( 
   uint32 arg 
);

void txc_hold_enc_clk_init (void);

#endif /* FEATURE_RL_JUMP_ENHANCEMENT */

#ifdef FEATURE_IS2000_REL_A_CC
/*===========================================================================

FUNCTION TXC_LOG_REACH_MSG

DESCRIPTION
  This function logs messages sent on the REACH channel.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void  txc_log_reach_msg
(
  txc_msg_buf_type *msg_ptr   /* pointer to sent message */
)
{
  word msg_len;

  mux_msg_log_type *log_ptr;   /* pointer to buffer to log msg in */

  msg_len = msg_ptr->msg[0];
  /* Correct msg_len in case of extended message length */
  if (msg_len & 0x0080)
  {
    msg_len &= 0x007F; /* Turn off extended message indicator */
    msg_len <<= 8;
    msg_len |= msg_ptr->msg[1];
  }

  log_ptr = (mux_msg_log_type *) log_alloc_ex (LOG_REACH_SIGNALING_C, msg_len +
                                                         sizeof(log_hdr_type));
  if (log_ptr != NULL)
  {
    /* copy reach msg packet  and send out*/
    (void) memcpy( (void *)log_ptr->msg, msg_ptr->msg, msg_len);
    log_commit((log_type *) log_ptr);
  }

}
#endif /* FEATURE_IS2000_REL_A_CC */

/*===========================================================================

FUNCTION TXC_LOG_AC_MSG

DESCRIPTION
  This function logs messages sent on the access channel.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void  txc_log_ac_msg
(
  txc_msg_buf_type *msg_ptr   /* pointer to sent message */
)
{

#if !defined (FEATURE_DIAG_V7_LOG_IFACE)
  log_ac_msg_type *log_ptr;   /* pointer to buffer to log msg in */

  log_ptr = (log_ac_msg_type *) log_alloc (LOG_AC_MSG_C, msg_ptr->msg[0] +
                                                         sizeof(log_hdr_type));
  if (log_ptr != NULL)
  {
    /* log vocoder packet */
    (void) memcpy( (void *)log_ptr->msg, msg_ptr->msg, msg_ptr->msg[0]);
    log_commit((log_type *) log_ptr);
  }

#else
  txc_log_msg_type *log_ptr;   /* pointer to buffer to log msg in */

  /* get a buffer off of the txc_log_msg_q to log the received message */
  if( ( log_ptr = (txc_log_msg_type *) q_get(&txc_log_msg_q) ) == NULL )
  {
     MSG_MED( "No buf from log_msg_q", 0, 0, 0 );
  }
  else
  {
    /* set up log buffer */
    log_ptr->entry.ac_msg.hdr.len =
      msg_ptr->msg[0] + sizeof( log_hdr_type );

    log_ptr->entry.ac_msg.hdr.code = LOG_AC_MSG_C;    /* set log type */
    ts_get( (unsigned long *)(log_ptr->entry.ac_msg.hdr.ts ));
    /* set timestamp */

    /* Copy message into buffer */
    (void) memcpy( (void *) log_ptr->entry.ac_msg.msg,
                   (void *) msg_ptr->msg,
                   msg_ptr->msg[0] );

    log_put( &log_ptr->desc );
  }
#endif /* !defined (FEATURE_DIAG_V7_LOG_IFACE) */

  /* Event report: Message transmitted */
  mclog_report_event_msg_txed(CAI_ACH, msg_ptr->msg);

}


/*===========================================================================

FUNCTION TXC_COMPUTE_ACCESS_CHAN

DESCRIPTION
  This function generates a random access channel number and then builds
  an Access Channel long code mask based on that access channel.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void txc_compute_access_chan( void )
{
  dword lo_mask;      /* Lower dword of Access Channel mask */
  dword hi_mask;      /* Upper dword of Access Channel mask */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* -----------------------------------------------------------------
  ** Before transmitting each access probe sequence, generate a random
  ** number, RA, from 0 to ACC_CHANs.  Use RA as the Access Channel
  ** number for all access probes in that access probe sequence.
  ** ----------------------------------------------------------------- */
  txac.ra = (byte) ran_dist( ran_next(), 0, txac.acc_chan+1 );

#ifdef TMOBILE
#error code not present
#endif

  MSG_HIGH( "RA = %d", txac.ra, 0, 0 );

  /* ---------------------------------------------------------
  ** Build Access Channel Long Code Mask (see IS-95 6.1.3.1.8)
  ** --------------------------------------------------------- */
  lo_mask =  ((dword) (txac.ra & 0xF) << 28);
  lo_mask |= ((dword) (txac.pagech & 0x7) << 25);
  lo_mask |= ((dword) (txac.base_id & 0xFFFF) << 9);
  lo_mask |=  txac.pilot_pn & 0x000001FF;

  hi_mask  = ((dword) (txac.ra & 0x10) >> 4);
  hi_mask |= ((dword) (0x18F << 1));

#ifdef TMOBILE
#error code not present
 #endif

  /* Save Access Channel long code mask */
  qw_set( txc_tx.buf.mask,  hi_mask,  lo_mask );

#ifdef FEATURE_RL_JUMP_ENHANCEMENT
  /* For 6500 and later targets, the user PN mask can be programmed
  ** immediately after it's computed.
  */

  /* Load user PN mask  */
  enc_user_pn_mask(txc_tx.buf.mask);
#endif /* FEATURE_RL_JUMP_ENHANCEMENT */

} /* end txc_compute_access_chan */


#ifdef FEATURE_IS2000_REL_A_CC
//txc_reach_on
/*===========================================================================

FUNCTION TXC_REACH_COMPUTE_ACCESS_CHAN

DESCRIPTION
  This function generates a random access channel number and then builds
  an Access Channel long code mask based on that access channel.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void txc_reach_compute_access_chan( void )
{
  dword lo_mask;      /* Lower dword of Access Channel mask */
  dword hi_mask;      /* Upper dword of Access Channel mask */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* -----------------------------------------------------------------
  ** Before transmitting each access probe sequence, generate a random
  ** number, RA, from 0 to ACC_CHANs.  Use RA as the Access Channel
  ** number for all access probes in that access probe sequence.
  ** ----------------------------------------------------------------- */

  txac.ra = (byte) ran_dist( ran_next(), 0, txac.acc_chan+1 );

  MSG_HIGH( "RA = %d", txac.ra, 0, 0 );

  lo_mask =  ((dword) (txac.ra & 0xF) << 28);
  lo_mask |= ((dword) (txac.pagech & 0x7) << 25);
  lo_mask |= ((dword) (txac.base_id & 0xFFFF) << 9);
  lo_mask |= ((dword) txc_reach_slot_offset) & 0x000001FF;

  hi_mask  = ((dword) (txac.ra & 0x10) >> 4);
  hi_mask |= ((dword) (0x18E << 1));


  /* Save Access Channel long code mask */
  qw_set( txc_tx.buf.mask,  hi_mask,  lo_mask );


//txc_reach_on
          MSG_HIGH( "enc_hi_mask = %x, enc_lo_mask = %x", 
                    hi_mask, lo_mask, 0 );

#ifdef FEATURE_RL_JUMP_ENHANCEMENT
  /* For 6500 and later targets, the user PN mask can be programmed
  ** immediately after it's computed.
  */

  /* Load user PN mask  */
  enc_user_pn_mask(txc_tx.buf.mask);
#endif /* FEATURE_RL_JUMP_ENHANCEMENT */

} /* end txc_reach_compute_access_chan */
//txc_reach_on
#endif // FEATURE_IS2000_REL_A_CC


/*===========================================================================

FUNCTION TXC_ACCESS_IDL_INIT

DESCRIPTION
  Initialization section for access channel.

DEPENDENCIES
  None
RETURN VALUE
  None.
SIDE EFFECTS
  None
===========================================================================*/
void txc_access_idl_init(
  txc_state_type      state           /* state we're coming from */
)
{
  /* check what state we're coming from and adjust accordingly */
  switch ( state ) {
    case TXC_CDMA_S:
      enc_set_mode( ENC_PC_MODE );  /* set up hardware for access */

      /* Load ENC_INT register with 18 ms (actually 17.92 ms)
      ** as 13 power control groups + 2 Walsh Symbols
      */
      txcmod_int_deregister_all(TXMOD_INT_MUX_CLT);

#if !defined(FEATURE_DIAG_V7_LOG_IFACE)
      acc_log.hdr.len = 0;
#else
      txac.log_ptr = NULL;
#endif

          /* IS95-A mode */
          enc_is95c_enable( FALSE );

          /* In IS95-A mode, the Pilot Channel enable bit must be cleared */
          enc_pch_enable( FALSE );

          /* In IS95-A mode, the Fundamental Channel enable bit must be set */
          enc_fch_enable( TRUE );

#ifndef FEATURE_NO_SCH_HW
          /* In IS95-A mode, the Supplemental Channel enable bit must be cleared */
          enc_sch_enable( FALSE );
#endif /*FEATURE_NO_SCH_HW*/

#ifndef FEATURE_NO_DCCH_HW
          /* In IS95-A mode, the cdma2000 Dedicated Control Channel enable bit
      ** must be cleared
      */
          enc_dcch_enable( FALSE );
#endif /*FEATURE_NO_DCCH_HW*/

          txc_pch_gain = 0;
          txc_fch_gain = 0;
          txc_sch_gain = 0;
          txc_dcch_gain = 0;

          TXC_SET_PCH_GAIN( txc_pch_gain );
          TXC_SET_FCH_GAIN( txc_fch_gain );
#ifndef FEATURE_NO_SCH_HW
          TXC_SET_SCH_GAIN( txc_sch_gain );
#endif
#ifndef FEATURE_NO_DCCH_HW
          TXC_SET_DCCH_GAIN( txc_dcch_gain );
#endif

      #ifdef FEATURE_FACTORY_TESTMODE
          if(ftm_mode == FTM_MODE)
          {
            enc_set_frame_off( 0x0, 0x0 ); // Add this in DMSS too?
          }
      #endif

      break;

    default:
      ERR( "Bad state trans cmd %d", txc_state, 0, 0 );
      break;

  } /* switch */

  txac.frame.dotx = FALSE;        /* don't transmit this frame */

  /* Should really initialize jump_source to default value here since even
  ** the pre jump command code has not received a command to jump at this
  ** time.  This init code is called upon reception of TXC_PC_F only (hence
  ** I changed the function name to clarify that fact.)
  */
  txc_tx.jump_source = TXC_READY_TO_JUMP;

#ifdef FEATURE_RL_JUMP_ENHANCEMENT
  /* Init counters to default, allow sleep since RL timing is not yet required.
  */
  txc_delay_enc_shutdown.counter = 0;
  txc_delay_enc_shutdown.txc_sleep_allowed = TRUE;
  /* Init to NOT hold encoder clock after TXC_JUMP_F */
  txac.hold_enc_clk_on = FALSE;
#endif /* FEATURE_RL_JUMP_ENHANCEMENT */
#ifdef FEATURE_IS95B_ACCESS_PROBE_HO
  txc_suspend_after_ta = FALSE;
#endif /* FEATURE_IS95B_ACCESS_PROBE_HO */

  if ( srch_mux_owns_tx() )
  {
#ifndef T_IO_CARD
    rfm_tx_disable(RFCOM_TRANSCEIVER_0);               /* TURN OFF VHF synth etc */
#endif
    enc_tx_enable (FALSE);          /* but, make sure power amp is off */
  }

} /* txc_access_idl_init */


#ifdef FEATURE_IS2000_REL_A_CC
/*===========================================================================

FUNCTION  TXC_COPY_REACH_PARAMETERS

DESCRIPTION
  Copies parameters from the TXC_REACH_F or TXC_REACH_PROBE_F command into
  the txac structure.

DEPENDENCIES
  None
RETURN VALUE
  None.
SIDE EFFECTS
  Modifies global variables, including txac.
===========================================================================*/
void txc_copy_reach_parameters
(
  txc_cmd_type *cmd_ptr            /* Pointer to received command */
)
{
  int i;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  txac.bkoff = cmd_ptr->reach.bkoff;              /* Sequence backoff */
  txac.probe_bkoff = cmd_ptr->reach.probe_bkoff;  /* Probe backoff */
  txac.acc_tmo = cmd_ptr->reach.acc_tmo;          /* Ack timeout */
  txac.max_seq = cmd_ptr->reach.seq_num;          /* Max probe sequences */
  txac.req = cmd_ptr->reach.req;                  /* Request or response */

  /* -----------------------------------------
  ** Calculate initial power offset for access
  ** ----------------------------------------- */
  if ((cmd_ptr->reach.init_pwr & 0x10))  /* Sign bit (5 bit field) */
  {
    /* Negative value --> sign extend to 8 bits */
    txac.power = (byte) (cmd_ptr->reach.init_pwr | 0xE0);
  }
  else
  {
    txac.power = (cmd_ptr->reach.init_pwr & 0x1F);
  }

  /* -----------------------------------------------------------------
  ** Add in nom_pwr which is already sign extended and, if necessary,
  ** adjusted according to nom_pwr_ext.
  ** ----------------------------------------------------------------- */
  txac.power += cmd_ptr->reach.nom_pwr;

  /* Invert to turn power adjustment into tx gain adjustment */
  txac.power = -(txac.power);  //lint !e732: Loss of sign (sign maintained manually)

  /* Convert to units of 1/2 dB */
  // Value can be at most 7 bits after multiplication.
  txac.power = txac.power * 2; //lint !e734: Loss of precision (9 to 8 bits)

  txac.init_pwr = txac.power;

  /* initialize the is95a power accumulation */
  txac.is95a_pwr = txac.init_pwr;

  /* Grab power step from message in 1dB steps */
  txac.pstep = (byte) (cmd_ptr->reach.pwr_step);

  /* Convert to units of 1/2 dB */
  // pwr_step is 3 bits, so product must be no more than 4 bits.
  txac.pstep = txac.pstep * 2; //lint !e734: Loss of precision (9 to 8 bits)

  txac.nstep = cmd_ptr->reach.num_step;      /* Number of access probes */
  txac.p = cmd_ptr->reach.p;                 /* Persistence test value */
  txac.acc_chan = cmd_ptr->reach.num_each_ba;   /* Number of access channels */
  txac.pagech   = cmd_ptr->reach.fccch;       /* Current Paging Channel */
  txac.base_id  = cmd_ptr->reach.base_id;    /* Base station identification */
  txac.pilot_pn = cmd_ptr->reach.pilot_pn;   /* Pilot PN seq offset index */
  txc_tx.buf.cmd_ptr = NULL;    /* Don't save command info */

  /* -----------------------------------------------------------------
  ** Before transmitting each access probe sequence, generate a random
  ** number, RA, from 0 to ACC_CHANs.  Use RA as the Access Channel
  ** number for all access probes in that access probe sequence.
  ** ----------------------------------------------------------------- */
  txc_reach_compute_access_chan();

  txc_reach_slot_length = (word) (cmd_ptr->reach.slot + 1);

  txc_reach_slot_offset1 = (word) (cmd_ptr->reach.slot_offset1);
  txc_reach_slot_offset2 = (word) (cmd_ptr->reach.slot_offset2);

// T = N(P+B)+A = N ( P + 0 ) + 0
// T = NP = (preamble_num_frac + 1)(preamble_frac_duration + 1)
// preamble_enabled = true
// preamble_num_frac = 0
// preamble_frac_duration = 3
// preamble_off_duration = 0
// preamble_add_duration = 0
// T = (preamble_num_frac + 1)(preamble_frac_duration + 1)
// T = (0 + 1)( 3 + 1 ) = 1 * 4 = 4

  txc_reach_preamble_length = 
  (word)((cmd_ptr->reach.preamble_num_frac + 1) * (cmd_ptr->reach.preamble_frac_duration + 1));


  txc_reach_rlgain_common_pilot = cmd_ptr->reach.rlgain_common_pilot;

  // IC_THRESH from the base station in units of 1/2 dB
  txc_reach_ic_thresh = cmd_ptr->reach.ic_thresh;  

  // IC_MAX from the base station in units of  1/2 dB
  txc_reach_ic_max = cmd_ptr->reach.ic_max;        

  // IC_THRESH in units of 1/2 dB for interference correction calculation
  // ic_thresh is 4 bit value.  Product must be no more than 5.
  txc_reach_ic_thresh = txc_reach_ic_thresh * 2;  //lint !e734: Loss of precision (9 to 8 bits)

  // IC_MAX in units of  1/2 dB for interference correction calculation
  // ic_max is 4 bit value.  Product must be no more than 5.
  txc_reach_ic_max = txc_reach_ic_max * 2; //lint !e734: Loss of precision (9 to 8 bits)


  MSG_HIGH("rlgain=%x ic_thresh=%x ic_max=%x",txc_reach_rlgain_common_pilot,txc_reach_ic_thresh,txc_reach_ic_max);


  for( i = 0; i < ENC_SCH_RC3_16X_SIZE; i++ )
  {
    txtc.supp_frame.data[ i ] = 0x5A;
  }


  switch ( cmd_ptr->reach.rate )
  {
    case TXC_EACH_RATE_9600_20_V:
      txc_reach_frame_length = ENC_SCH_FRAME_SIZE_20_V;
      txtc.supp_rate = ENC_SCH_FULL_RATE;
      txc_reach_frame_size = 172; /* 20ms, 9600bps, 172 information bits */
      break;

    case TXC_EACH_RATE_19200_20_V:
      txc_reach_frame_length = ENC_SCH_FRAME_SIZE_20_V;
      txtc.supp_rate = ENC_SCH_2X_RATE;
      txc_reach_frame_size = 360; /* 20ms, 19200bps, 360 information bits */
      break;

    case TXC_EACH_RATE_19200_10_V:
      txc_reach_frame_length = ENC_SCH_FRAME_SIZE_10_V;
      txtc.supp_rate = ENC_SCH_FULL_RATE;
      txc_reach_frame_size = 172; /* 10ms, 19200bps, 172 information bits */
      break;

    case TXC_EACH_RATE_38400_20_V:
      txc_reach_frame_length = ENC_SCH_FRAME_SIZE_20_V;
      txtc.supp_rate = ENC_SCH_4X_RATE;
      txc_reach_frame_size = 744; /* 20ms, 38400bps, 744 information bits */
      break;

    case TXC_EACH_RATE_38400_10_V:
      txc_reach_frame_length = ENC_SCH_FRAME_SIZE_10_V;
      txtc.supp_rate = ENC_SCH_2X_RATE;
      txc_reach_frame_size = 360; /* 10ms, 38400bps, 360 information bits */
      break;
      
    case TXC_EACH_RATE_38400_5_V:
      txc_reach_frame_length = ENC_SCH_FRAME_SIZE_5_V;
      txtc.supp_rate = ENC_SCH_FULL_RATE;
      txc_reach_frame_size = 172; /* 5ms, 38400bps, 172 information bits */
      break;
  }

  txc_reach_rate_word = cmd_ptr->reach.rate;

  /* Get REACH data gain based on the rate word for data channel. */
  txc_sch_gain = rpc_get_reach_data_gain( cmd_ptr->reach.rate );

  /* Get REACH pilot gain based on the rate word for data channel. */
  txc_pch_gain = rpc_get_reach_pilot_gain( cmd_ptr->reach.rate );

  /* Get REACH pilot gain adjustment for TX_ADJ based on the pilot gain. */
  txc_reach_pch_adj_gain = rpc_get_pch_adj_gain(PCH_GAIN_TRUNCATE_TO_8BITS(txc_pch_gain));

} /* txc_copy_reach_parameters */
#endif /* FEATURE_IS2000_REL_A_CC */

/*===========================================================================

FUNCTION TXC_ACCESS_CMD

DESCRIPTION
  Process commands which are common to all states of the idle state machine.

DEPENDENCIES
  None
RETURN VALUE
  Next state to be processed.
SIDE EFFECTS
  None
===========================================================================*/
txc_state_type txc_access_cmd
(
  txc_cmd_type *cmd_ptr,            /* Pointer to received command */
  txc_state_type state              /* Current state */
)
{
  word status;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Interpret the command from the queue */
  switch (cmd_ptr->hdr.command)
  {
#ifdef FEATURE_RL_JUMP_ENHANCEMENT
    case TXC_JUMP_F :
      MSG_MED ("Got TXC_JUMP_F, st=%d",state,0,0);
      if (state == TXC_ACC_IDLE_S) 
      {
        /* Get the PN randomization delay from Jump command.  This value is programmed
        ** during the Jump to hyperspace.
        */
        txac.rn = cmd_ptr->jump.rn;               /* PN randomization delay */
        txac.hold_enc_clk_on = cmd_ptr->jump.hold_enc_clk_on;  /* Hold encoder clock on */

        /* Set correct state and init duration of wait in TXC_HOLD_ENC_CLK_S state 
        ** to default value.
        */
        txc_delay_enc_shutdown.counter = 0;
        state = TXC_ACC_JUMP_S;

        /* Jump source is set to TXC_JUMP for 2 cases:
        **   In TXC_HOLD_ENC_CLK_S state, the jump is complete.
        **   In TXC_ACC_JUMP_S state, the jump is in progress.
        */
        txc_tx.jump_source = TXC_JUMP;

        /* Set up Hold ENC clocks ISR.
        */
        txc_hold_enc_clk_init();
      }
      /* Do not change state if MS is already done with Jump.  
      ** Do restart Jump timer.
      */
      else if (state == TXC_HOLD_ENC_CLK_S)
      {
        MSG_HIGH ("Got TXC_JUMP_F, old timer=%d", txc_delay_enc_shutdown.counter,0,0);
        INTLOCK ();
        /* Here, we are already holding the ENC clocks ON.  So just update the counter
        ** so we will extend the timer starting from this point.
        ** Int lock is required because txc_delay_enc_shutdown.counter is accessed in interrupt context.
        ** If the count down has reached the end, do not restart the count down as it's is safer
        ** not to interrupt the processing of TXC_TX_SHUTDOWN_SIG at this point.
        */
        if (txc_delay_enc_shutdown.counter > 0) 
        {
          txc_delay_enc_shutdown.counter = TXC_HOLD_ENC_CLK_WAIT_DURATION;
        }
        INTFREE ();
      }
      else if (state == TXC_ACC_JUMP_S)
      {
        MSG_HIGH ("TXC_JUMP_F ignored in TXC_ACC_JUMP_S", 0,0,0);
      }
      else
      {
        MSG_ERROR ("Unxpctd TXC_JUMP_F rxd in st %d", state,0,0);
      }

      status = TXC_DONE_S;
      break;
#endif /* FEATURE_RL_JUMP_ENHANCEMENT */

    case TXC_ACCESS_F :
      MSG_MED ("Got TXC_ACCESS_F, st=%d",state,0,0);
      /* We may be in TXC_ACC_JUMP_S state due to receiving the TXC_JUMP_F
      ** command from MC.  By changing the jump_source to TXC_JUMP_ACCESS
      ** here, we still transition to the correct state at the end of the jump.
      */
      txc_tx.jump_source = TXC_JUMP_ACCESS;

#ifdef FEATURE_IS95B_ACCESS_PROBE_HO
      ASSERT(txac.state != ACC_SUSPEND_S);
#endif /* FEATURE_IS95B_ACCESS_PROBE_HO */

      
#ifdef T_IO_CARD
#error code not present
#endif

      /* ------------------------------------------------------------------
      ** In IS95-A mode, the Fundamental Channel enable bit must be set.
      ** This bit is a misnomer because this needs to be set even for the
      ** access channel which shares the same h/w channel.  This call is
      ** needed in the NCC (no traffic) to OCC re-origination case.
      ** ------------------------------------------------------------------*/
      enc_fch_enable( TRUE );

      /* Prepare to send Access Channel message */
      txc_acc_msg_prep( cmd_ptr );

#ifdef FEATURE_IS2000_REL_A_CC
//txc_reach_on
      txc_reach_access = FALSE;
#endif // FEATURE_IS2000_REL_A_CC


      /* Initialize the stop_probes to FALSE */
      txac.stop_probes = FALSE;

#ifndef T_IO_CARD
      /* -------------------------------------------
      ** Power up and program the VHF TX synthesizer
      ** ------------------------------------------- */
      rfm_enable_tx_pwr(RFCOM_TRANSCEIVER_0);

#ifdef FEATURE_RF_WAIT_CHANGES
#error code not present
#else
      (void) rex_set_timer( &txc_rf_timer, RF_ENABLE_PWR_WAIT );
#endif // FEATURE_RF_WAIT_CHANGES

      (void) txc_wait( TXC_RF_TIMER_SIG );
      rfm_tx_enable(RFCOM_TRANSCEIVER_0);
#endif


#ifdef FEATURE_RL_JUMP_ENHANCEMENT
      /* Do not change state if MS has already started Jump or is
      ** already done with Jump.
      */
      if ((state != TXC_ACC_JUMP_S) && (state != TXC_HOLD_ENC_CLK_S))
      {
        /* Set correct state and init duration of wait in TXC_HOLD_ENC_CLK_S 
        ** state to default value.
        */
        MSG_HIGH ("No prec TXC_JUMP_F, or late TXC_ACCESS_F",0,0,0);
        txc_delay_enc_shutdown.counter = 0;
        state = TXC_ACC_JUMP_S;

        /* Set up Hold ENC clocks ISR.
        */
        txc_hold_enc_clk_init();
      }
#else
      /* Initialize encoder timing to be able to transmit */
      state = TXC_ACC_JUMP_S;
#endif /* FEATURE_RL_JUMP_ENHANCEMENT */

      status = TXC_DONE_S;
      break;


#ifdef FEATURE_IS2000_REL_A_CC
//txc_reach_on

      case TXC_REACH_F :
        MSG_MED ("Got TXC_REACH_F, st=%d",state,0,0);
  #ifdef FEATURE_IS95B_ACCESS_PROBE_HO
        ASSERT(txac.state != ACC_SUSPEND_S);
  #endif /* FEATURE_IS95B_ACCESS_PROBE_HO */

        /* Prepare to send Access Channel message */
        txc_reach_msg_prep( cmd_ptr );

        /* We may be in TXC_ACC_JUMP_S state due to receiving the TXC_JUMP_F
        ** command from MC.  By changing the jump_source to TXC_JUMP_ACCESS
        ** here, we still transition to the correct state at the end of the jump.
        */
        txc_tx.jump_source = TXC_JUMP_ACCESS;

        txc_reach_access = TRUE;

        // First sub-attempt
        txc_reach_sub_att_number = 0;

        /* Enhanced Access Channel Open Loop Power Offsets */
        txtc.offset_pwr = (byte) 8.5 * 12;

        /* Set RATE_ADJ to zero as a default value. */
        rpc_set_tx_rate_adj(0);

        /* Initialize the stop_probes to FALSE */
        txac.stop_probes = FALSE;

#ifndef T_IO_CARD
        /* -------------------------------------------
        ** Power up and program the VHF TX synthesizer
        ** ------------------------------------------- */
        rfm_enable_tx_pwr(RFCOM_TRANSCEIVER_0);
        (void) rex_set_timer( &txc_rf_timer, RF_ENABLE_PWR_WAIT );
        (void) txc_wait( TXC_RF_TIMER_SIG );
        rfm_tx_enable(RFCOM_TRANSCEIVER_0);
#endif



#ifdef FEATURE_RL_JUMP_ENHANCEMENT
        /* Do not change state if MS has already started Jump or is
        ** already done with Jump.
        */
        if ((state != TXC_ACC_JUMP_S) && (state != TXC_HOLD_ENC_CLK_S))
        {
          /* Set correct state and init duration of wait in TXC_HOLD_ENC_CLK_S 
          ** state to default value.
          */
          MSG_HIGH ("No prec TXC_JUMP_F, or late TXC_REACH_F",0,0,0);
          txc_delay_enc_shutdown.counter = 0;
          state = TXC_ACC_JUMP_S;

          /* Set up Hold ENC clocks ISR.
          */
          txc_hold_enc_clk_init();
        }
#else
        /* Initialize encoder timing to be able to transmit */
        state = TXC_ACC_JUMP_S;
#endif /* FEATURE_RL_JUMP_ENHANCEMENT */

        status = TXC_DONE_S;
        break;

//txc_reach_on
#endif // FEATURE_IS2000_REL_A_CC


#ifdef FEATURE_IS2000_REL_A_CC
#if defined(FEATURE_IS95B_PILOT_RPT)

    case TXC_REACH_PROBE_F :
      MSG_MED ("Got TXC_REACH_PROBE_F, st=%d",state,0,0);

#ifdef FEATURE_RL_JUMP_ENHANCEMENT
      /* This command is not valid for the TXC_HOLD_ENC_CLK_S state */
      if (state == TXC_HOLD_ENC_CLK_S)
      {
        status = TXC_BADCMD_S;
        break;
      }
#endif /* FEATURE_RL_JUMP_ENHANCEMENT */
      /* Check if new base_id is different from current.
       * If so, this is a access handoff.  Increment the
       * subattempt count.
       *
       *** Check this value before txac.base_id is updated ***/
      if (cmd_ptr->reach.base_id != txac.base_id)
      { 
        // This is an access probe handoff.
        ++txc_reach_sub_att_number;
      }

      if (txac.state == ACC_SUSPEND_S)
      {
        txc_copy_reach_parameters( cmd_ptr );
        /* Do not reset the sequence number.  See LAC 2.1.1.2.2.2. */
        //txac.seq_num = 0;                             /* Sequence number */
        txac.num_step = txac.nstep;              /* # of access probes remaining */
        txac.probe_count = 0;                    /* Access probe number */
        txac.first_frame = FALSE;                /* Not first frame */
        txc_acc_probe_resume();
        txac.stop_probes = FALSE;
      }

      /* Prepare Reverse Enhanced Access Channel Message. */
      txc_reach_probe_prep( cmd_ptr );

      status = TXC_DONE_S;
      break;

#endif /* FEATURE_IS95B_PILOT_RPT */
#endif // FEATURE_IS2000_REL_A_CC


#if defined(FEATURE_IS95B_PILOT_RPT)
    case TXC_ACC_PROBE_F :
      MSG_MED ("Got TXC_ACC_PROBE_F, st=%d",state,0,0);

#ifdef FEATURE_RL_JUMP_ENHANCEMENT
      /* This command is not valid for the TXC_HOLD_ENC_CLK_S state */
      if (state == TXC_HOLD_ENC_CLK_S)
      {
        status = TXC_BADCMD_S;
        break;
      }
#endif /* FEATURE_RL_JUMP_ENHANCEMENT */

#ifdef FEATURE_IS95B_ACCESS_PROBE_HO
      /* If the TXC is in Suspended State, TXC should comeout */
      /* of it's sleep!!                                      */
      if (txc_state == TXC_ACC_IDLE_S)
        ERR_FATAL("ACC_PROBE msg in wrong state",0,0,0);
      if (txac.state == ACC_SUSPEND_S)
      {
        txac.preambles = cmd_ptr->acc.pam_sz + 1;
        txac.slotting = (cmd_ptr->acc.pam_sz + 1) + (3 + cmd_ptr->acc.max_cap_sz);
        txac.bkoff = cmd_ptr->acc.bkoff;
        txac.probe_bkoff = cmd_ptr->acc.probe_bkoff;
        txac.acc_tmo = cmd_ptr->acc.acc_tmo;
        txac.max_seq = cmd_ptr->acc.seq_num;
        txac.num_step = cmd_ptr->acc.num_step;
        /* -----------------------------------------
        ** Calculate initial power offset for access
        ** ----------------------------------------- */
        if ((cmd_ptr->acc.init_pwr & 0x10))  /* Sign bit (5 bit field) */
        {
          /* Negative value --> sign extend to 8 bits */
          txac.power = (byte) (cmd_ptr->acc.init_pwr | 0xE0);
        }
        else
        {
          txac.power = (cmd_ptr->acc.init_pwr & 0x1F);
        }
        /* -----------------------------------------------------------------
        ** Add in nom_pwr which is already sign extended and, if necessary,
        ** adjusted according to nom_pwr_ext.
        ** ----------------------------------------------------------------- */
        txac.power += cmd_ptr->acc.nom_pwr;

        /* Invert to turn power adjustment into tx gain adjustment */
        txac.power = -(txac.power);  //lint !e732: Loss of sign (Sign maintained manually)

        /* Convert to units of 1/2 dB */
        // Product can only be 5 bits wide
        txac.power = txac.power * 2; //lint !e734: Loss of precision (9 to 8 bits)

        txac.init_pwr = txac.power;

        /* initialize the is95a power accumulation */
        txac.is95a_pwr = txac.init_pwr;

        /* Grab power step from message in 1dB steps */
        txac.pstep = (byte) (cmd_ptr->acc.pwr_step);

        /* Convert to units of 1/2 dB */
        // Product can only be 4 bits wide
        txac.pstep = txac.pstep * 2; //lint !e734 Loss of precision (9 to 8 bits)


        /* Get the new Masking parameters from the Access Channel Message */
        txac.rn = cmd_ptr->acc.rn;               /* PN randomization delay */
        txac.p = cmd_ptr->acc.p;                 /* Persistence test value */
        txac.acc_chan = cmd_ptr->acc.acc_chan;   /* Number of access channels */
        txac.use_chan = cmd_ptr->acc.use_chan;   /* Use specific channel */
        txac.pagech   = cmd_ptr->acc.pagech;     /* Current Paging Channel */
        txac.base_id  = cmd_ptr->acc.base_id;    /* Base station identification */
        txac.pilot_pn = cmd_ptr->acc.pilot_pn;   /* Pilot PN seq offset index */
        txc_acc_probe_resume();
        txac.stop_probes = FALSE;
      }
#endif /* FEATURE_IS95B_ACCESS_PROBE_HO */

      /* Prepare to send Access Channel message */
      /* Prepare to send Access Channel message */
      txc_acc_probe_prep( cmd_ptr );

      status = TXC_DONE_S;
      break;
#endif /* FEATURE_IS95B_PILOT_RPT */

#ifdef FEATURE_IS95B_ACCESS_PROBE_HO
    case TXC_ACC_SUSPEND_F :
      MSG_MED ("Got TXC_ACC_SUSPEND_F, st=%d",state,0,0);

#ifdef FEATURE_RL_JUMP_ENHANCEMENT
      /* This command is not valid for the TXC_HOLD_ENC_CLK_S state */
      if (state == TXC_HOLD_ENC_CLK_S)
      {
        status = TXC_BADCMD_S;
        break;
      }
#endif /* FEATURE_RL_JUMP_ENHANCEMENT */
      txc_acc_probe_suspend(cmd_ptr);
      status = TXC_DONE_S;
      break;
#endif /* FEATURE_IS95B_ACCESS_PROBE_HO */

    case TXC_TC_F:
    {
      // Default fundicated to FCH RC.  Will be updated with correct value later.
      // Put here to quite down compiler
      cai_radio_config_type fundicated_rc = cmd_ptr->tc.rev_fch.rc;

      MSG_MED ("Got TXC_TC_F, st=%d",state,0,0);
      enc_set_frame_off(cmd_ptr->tc.frame_offset, cmd_ptr->tc.frame_offset);
      txc_set_frame_offset(cmd_ptr->tc.frame_offset);

      /*
       * Make sure that we don't try to transmit anything before the TC is
       * ready to transmit the preambles.
      */
      txtc.ready_to_transmit = FALSE;

      qw_equ( txc_tx.mask, cmd_ptr->tc.mask );
        /* store initial rate set to use */

#ifdef FEATURE_IS2000_P2
      // Default to FCH for signalling
      txtc.dsch_map = txtc.cur_dsch = TXC_REV_FCH;

      if (cmd_ptr->tc.rev_dcch.included)
      {
        txc_so.rev_dcch.included = cmd_ptr->tc.rev_dcch.included;

        // Before Logical-to-physical mapping is support, just do the default
        // dsch_map specified in IS2000
        if ( !cmd_ptr->tc.rev_fch.included )
        {
          txtc.dsch_map = txtc.cur_dsch = TXC_REV_DCCH;
        }

        txc_so.rev_dcch.rc = cmd_ptr->tc.rev_dcch.rc;
        fundicated_rc = cmd_ptr->tc.rev_dcch.rc;
      }
      else
      {
        txc_so.rev_dcch.included = FALSE;

        // Disable DCCH if not included
        enc_dcch_enable( FALSE );
      }

      if (cmd_ptr->tc.rev_fch.included)
#endif /* FEATURE_IS2000_P2 */
      {
        #ifdef FEATURE_IS2000_P2
        txc_so.rev_fch.included = cmd_ptr->tc.rev_fch.included;
        #endif /* FEATURE_IS2000_P2 */

        txc_so.rev_fch.rc = cmd_ptr->tc.rev_fch.rc;
        enc_fch_rc( (enc_fch_radio_config_type) txc_so.rev_fch.rc );
        enc_fch_enable( TRUE );
        // Enabling of FCH is done in txctraffic later
      }
#ifdef FEATURE_IS2000_P2
      else
      {
        txc_so.rev_fch.included = FALSE;

        // Disable FCH if not included
        enc_fch_enable( FALSE );
      }
#endif // FEATURE_IS2000_P2

#ifdef FEATURE_IS2000_REL_A_SVD
      /* Choose either FCH or DCCH as signaling channel.
      ** FCH given priority over DCCH.
      */

#ifdef FEATURE_IS2000_P2
      if (txc_so.rev_fch.included)
#endif /* FEATURE_IS2000_P2 */
      {
        if (cmd_ptr->tc.signaling_phy_channel & MCTRANS_PHY_CHAN_FCH)
        {
          txtc.dsch_map = txtc.cur_dsch = TXC_REV_FCH;
        }
      }
#ifdef FEATURE_IS2000_P2
      else if (txc_so.rev_dcch.included &&
               (cmd_ptr->tc.signaling_phy_channel & MCTRANS_PHY_CHAN_DCCH))
      {
        txtc.dsch_map = txtc.cur_dsch = TXC_REV_DCCH;
      }
#endif /* FEATURE_IS2000_P2 */
      else
      {
        //
        // REMOVE AFTER DEBUG
        //
        ERR_FATAL("Illegal signaling channel assignment", 0, 0, 0);
      }
#endif /* FEATURE_IS2000_REL_A_SVD */

      #ifdef FEATURE_TX_POWER_BACK_OFF
      // Just set up channels, we need to run the RF PA backoff algorithm
      // to back off the power if necessary
      rf_reset_pa_backoff_module( 0 );   // Need to initialize gain accumulator at call setup
      txc_update_chan_config_to_rf(TRUE);
      #endif // FEATURE_TX_POWER_BACK_OFF

      // set txc_so.rlgain

#ifdef FEATURE_IS2000_REL_A
      txtc.p_rev_in_use = cmd_ptr->tc.p_rev_in_use;
#endif

#ifndef T_IO_CARD
      /* -------------------------------------------
      ** Power up and program the VHF TX synthesizer
      ** ------------------------------------------- */
      rfm_enable_tx_pwr(RFCOM_TRANSCEIVER_0);

#ifdef FEATURE_RF_WAIT_CHANGES
#error code not present
#else
      (void) rex_set_timer( &txc_rf_timer, RF_ENABLE_PWR_WAIT );
#endif // FEATURE_RF_WAIT_CHANGES

      (void) txc_wait( TXC_RF_TIMER_SIG );
      rfm_tx_enable(RFCOM_TRANSCEIVER_0);
#endif

      txc_tx.jump_source = TXC_JUMP_TRAFFIC;

      if (cmd_ptr->tc.tcb_ptr != NULL)
      {
        /* Save pointer to task to signal back and signal to use */
        txtc.tcb_ptr = cmd_ptr->tc.tcb_ptr;
        txtc.sigs =    cmd_ptr->tc.sigs;
      }
#ifdef FEATURE_FACTORY_TESTMODE
      if(ftm_mode == FTM_MODE)
      {
        txc_ftm_rfch_rate = cmd_ptr->tc.fch_rate;
  #ifdef FEATURE_IS2000_P2
        /* Support FEATURE_FTM_DCCH */
        txc_ftm_rdcch_rate = cmd_ptr->tc.dcch_rate;
        /* End of FEATURE_FTM_DCCH */
  #endif
        txtc.sch_on = cmd_ptr->tc.sch_on;
        if( cmd_ptr->tc.sch_on )
        {
  #if defined(FEATURE_PLT) && defined(FEATURE_IS2000_R_SCH)
#error code not present
  #endif /* FEATURE_PLT */

  #ifdef FEATURE_IS2000_R_SCH
          txtc.supp_rate = cmd_ptr->tc.sch_rate;
          txc_ftm_rsch_rate = cmd_ptr->tc.sch_rate;
          txtc.supp_turbo = cmd_ptr->tc.sch_turbo;
          txtc.supp_ltu_size = cmd_ptr->tc.sch_ltu_size;
  #endif /*FEATURE_IS2000_R_SCH*/
        }
  #ifdef FEATURE_IS2000_P2
        /* Support FEATURE_FTM_DCCH */
        if (txc_so.rev_dcch.included)
        {
          txtc.dcch_loopback = FALSE; /* Initially off */
          txtc.num_preambles = cmd_ptr->tc.num_preambles;
          {
            int i;
            byte *tx_pat_ptr = (byte *)&cmd_ptr->tc.tx_pattern;

            for( i = 0; i < ENC_14400_FULL_RATE_SIZE; i++ )
            {
              txc_ftm_rdcch_data.data[ i ] = tx_pat_ptr[i % 4];
            }
          }
        }
        /* End of FEATURE_FTM_DCCH */
        if (txc_so.rev_fch.included)
  #endif /* FEATURE_IS2000_P2 */
        {
          txtc.fch_loopback = FALSE; /* Initially off */
          txtc.num_preambles = cmd_ptr->tc.num_preambles;
          {
            int i;
            byte *tx_pat_ptr = (byte *)&cmd_ptr->tc.tx_pattern;

            for( i = 0; i < ENC_14400_FULL_RATE_SIZE; i++ )
            {
              txc_ftm_rfch_data.data[ i ] = tx_pat_ptr[i % 4];
            }
          }
        }
      }
#endif

#ifdef FEATURE_FACTORY_TESTMODE
      if(ftm_mode == DMSS_MODE)
      {
#endif
        /* already sign extended. */
        txtc.rl_gain_adj = (int8) cmd_ptr->tc.rl_gain_adj;
#ifdef FEATURE_FACTORY_TESTMODE
      }
#endif

      if ( fundicated_rc == CAI_RC_1 ||
           fundicated_rc == CAI_RC_2 )
      {
         /* taken care through linearizer tables in RF cal */
         txtc.offset_pwr = 0;
      }
      else  // RC 3 and 4
      {
         /*-------------------------------------------------------
         ** For CELLULAR : -73 - (-81.5),
         ** For PCS      : -76 - (-84.5) = 8.5
         **------------------------------------------------------*/
         txtc.offset_pwr = (byte) 8.5 * 12;
      }

      rpc_tc_cmd (cmd_ptr);

      if( cmd_ptr->tc.rev_fch_gating_on )
      {
        txtc.fch_8th_rate_gating = TRUE;
        // Note: don't need to program the encoder here yet since we
        // need to decide whether to gate or not depending on whether
        // it's a 8th rate frame when we are actually sending it.
        MSG_HIGH( "FCH 1/8R gating on", 0, 0, 0 );
      }
      else
      {
        txtc.fch_8th_rate_gating = FALSE;
        MSG_HIGH( "FCH NOT 1/8R gated", 0, 0, 0 );
      }
      
      /* R-FCH gating event has a 1 byte boolean parameter */
      event_report_payload(EVENT_REV_FCH_GATING_IN_USE,
                           sizeof(byte),
                           (void *) &txtc.fch_8th_rate_gating);
      
      // Turn off pilot gating regardless just in case it was left gated from the
      // last call.  For 1/8 rate gating, it will be taken care of later when sending
      // frames.
      enc_gating_mode( ENC_NO_GATING_MODE );
      enc_is95c_tx_patn( (word) ENC_NO_GATING_PATTERN );
      
#ifdef FEATURE_RL_JUMP_ENHANCEMENT
      /* Do not change state if MS has already started Jump or is
      ** already done with Jump.
      */
      if ((state != TXC_ACC_JUMP_S) && (state != TXC_HOLD_ENC_CLK_S))
      {
        /* Set correct state and init duration of wait in TXC_HOLD_ENC_CLK_S 
        ** state to default value.
        */
        MSG_HIGH ("No prec TXC_JUMP_F, or late TXC_TC_F",0,0,0);
        txc_delay_enc_shutdown.counter = 0;
        state = TXC_ACC_JUMP_S;

        /* Set up Hold ENC clocks ISR.
        */
        txc_hold_enc_clk_init();
      }
#else
      /* Initialize encoder timing to be able to transmit */
      state = TXC_ACC_JUMP_S;
#endif /* FEATURE_RL_JUMP_ENHANCEMENT */

      status = TXC_DONE_S;       /* dispose of command buffer */
      break;
    }

    /* Stop transmitting access message NOW */
    case TXC_ACC_STOP_F:
      MSG_MED ("Got TXC_ACC_STOP_F, st=%d",state,0,0);

      /* This command is not valid for the idle state */
      if (state == TXC_ACC_IDLE_S)
      {
        status = TXC_BADCMD_S;
      }
      else
      {
        state = TXC_ACC_IDLE_S;
        txc_tx.active_msg = FALSE;  /* Indicate no msg tx in progress */
        txac.frame.dotx = FALSE;    /* No transmission next interrupt */

#ifdef FEATURE_IS95B_ACCESS_PROBE_HO
        txc_suspend_after_ta = FALSE;
#endif /* FEATURE_IS95B_ACCESS_PROBE_HO */

        /* The only purpose of the resetting of txac.state is to satisfy */
        /* an assertion that the TXC_ACCESS_F command can not be sent by */
        /* MC->TXC when TXC is in suspended state. There is no need to   */
        /* have that assertion, other than, it helps in catching MC-TXC  */
        /* interface problems for T72-T40 timer & Access Probe Handoff   */
        /* once the interface is properly stablised, we can remove this  */
        /* statement and the assertion. However, there is no harm in     */
        /* leaving these things as it is, as they will always help in    */
        /* early bug detections.                                         */

        txac.state = ACC_SETUP1_S;

        if (cmd_ptr->stop.tcb_ptr != NULL)
        {
          /* Save pointer to task to signal back and signal to use */
          txac.tcb_ptr = cmd_ptr->stop.tcb_ptr;
          txac.sigs =    cmd_ptr->stop.sigs;
        }

        status = TXC_DONE_S;
      }
      break;

    case TXC_EXIT_F:
      MSG_MED ("Got TXC_EXIT_F, st=%d",state,0,0);
      /* exit cdma mode */

#ifdef FEATURE_RL_JUMP_ENHANCEMENT
      /* In EXIT state, definitely allow sleep */
      txc_delay_enc_shutdown.txc_sleep_allowed = TRUE; 
#endif /* FEATURE_RL_JUMP_ENHANCEMENT */

      txc_exit();  /* perform exit processing */
      state = TXC_EXIT_S;
      status = TXC_DONE_S;
      break;

    case TXC_CDMA_F:
      /* --------------------------------------------------
      ** Transition back to CDMA init state without exiting
      ** txc_subtask()
      ** -------------------------------------------------- */

      /* ----------------------------------------------------
      ** Set interrupt handler to null and turn off TX clocks
      ** ---------------------------------------------------- */
      MSG_MED ("Got TXC_CDMA_F, st=%d",state,0,0);

#ifdef FEATURE_RL_JUMP_ENHANCEMENT
      /* In CDMA state, definitely allow sleep */
      txc_delay_enc_shutdown.txc_sleep_allowed = TRUE; 
#endif /* FEATURE_RL_JUMP_ENHANCEMENT */

      txc_exit();

      /* ---------------------------
      ** Initialize encoder for CDMA
      ** --------------------------- */
      txc_cdma_enc_setup();

      /* ------------------------
      ** Switch back to CDMA mode
      ** ------------------------ */
      state = TXC_CDMA_S;
      status = TXC_DONE_S;
      break;

    case TXC_PC_F:
      MSG_MED ("Got TXC_PC_F, st=%d",state,0,0);
      /* this command is only valid for the idle state */
      if (state == TXC_ACC_IDLE_S)
      {
        status = TXC_DONE_S;
      }
      else
      {
        status = TXC_BADCMD_S;
      }
      /* either way don't change the state */
      break;

    case TXC_FRAME_OFFSET_F:
        MSG_MED ("Got TXC_FRAME_OFFSET_F, st=%d",state,0,0);
#ifdef FEATURE_RL_JUMP_ENHANCEMENT
      /* This command is not valid for the TXC_HOLD_ENC_CLK_S state */
      if (state == TXC_HOLD_ENC_CLK_S)
      {
        status = TXC_BADCMD_S;
        break;
      }
#endif /* FEATURE_RL_JUMP_ENHANCEMENT */
      txc_frame_offset_cmd.frame_offset = cmd_ptr->fo.frame_offset;
      qw_equ( txc_frame_offset_cmd.action_frame, cmd_ptr->fo.action_frame );
      txc_frame_offset_cmd.pending = TRUE;

      /*
      ** Leave the offset pending but set up the frame offset anyway
      ** since it affects the DECODER for traffic channel acquisition.
      */
      txc_frame_offset = txc_frame_offset_cmd.frame_offset;
      rxc_set_frame_offset( txc_frame_offset_cmd.frame_offset );
      enc_set_frame_off( txc_frame_offset, txc_frame_offset );
      status = TXC_DONE_S;
      break;

    default:
      MSG_HIGH ("Drpd MC cmd %d", cmd_ptr->hdr.command, 0, 0 );
      status = TXC_BADCMD_S;

      /* Don't change the state */
      break;

  }  /* switch */

  txc_cmd_done( cmd_ptr, status );   /* Dispose of command buffer */

  return (state);

} /* txc_access_cmd */

#ifdef FEATURE_RL_JUMP_ENHANCEMENT
/*===========================================================================

FUNCTION txc_access_or_traffic_cmd

DESCRIPTION
  Check jump source to see if it is desirable to jump to access or traffic state
  after jump to hyperspace is complete.

DEPENDENCIES
  None.
RETURN VALUE
  None.
SIDE EFFECTS
  None
===========================================================================*/

txc_state_type txc_access_or_traffic_cmd (txc_state_type state)
{
  if (txc_tx.jump_source == TXC_JUMP_ACCESS)
  {
#ifdef FEATURE_IS2000_REL_A_CC
    //txc_reach_on
    if ( txc_reach_access == TRUE )
    {
      enc_set_mode( ENC_TC_MODE );  /* set up hardware for reach */
      
      /* Load ENC_INT_ST register with BIN 0, BIN 1, BIN 2, BIN 3
      ** with power control groups 0 and Walsh Symbols 0
      */
      
      txac.state = ACC_SETUP1_S;
      
      txcmod_int_deregister_all(TXMOD_INT_MUX_CLT);
      (void) txcmod_int_regis_for_pcgint(0, txc_reach_isr, ENC_STATUS_INTERRUPT_SLOT_0, TXMOD_INT_MUX_CLT);
      (void) txcmod_int_regis_for_pcgint(4, txc_reach_isr, ENC_STATUS_INTERRUPT_SLOT_1, TXMOD_INT_MUX_CLT);
      (void) txcmod_int_regis_for_pcgint(8, txc_reach_isr, ENC_STATUS_INTERRUPT_SLOT_2, TXMOD_INT_MUX_CLT);
      (void) txcmod_int_regis_for_pcgint(12, txc_reach_isr, ENC_STATUS_INTERRUPT_SLOT_3, TXMOD_INT_MUX_CLT);
    }
    else
#endif /* FEATURE_IS2000_REL_A_CC */
    {
      enc_set_mode( ENC_PC_MODE );  /* set up hardware for access */

      txcmod_int_deregister_all(TXMOD_INT_MUX_CLT);
      (void) txcmod_int_regis_for_pcgint(8, txc_access_isr, ENC_STATUS_INTERRUPT_SLOT_2, TXMOD_INT_MUX_CLT);
    }

    /* Send command to interrupt handler to initialize frame count
        because receive interrupt could occur at this time and we
        could get the wrong frame count */
    txc_int_cmd.cmd = INT_FRAME_F;
    DO_INT_CMD();
    state = TXC_ACC_ATT_S;  /* ready to transmit message */
  }
  else if (txc_tx.jump_source == TXC_JUMP_TRAFFIC)
  {
    txc_traffic_init();
    state = TXC_TRAFFIC_S;  /* ready to jump to traffic */
  
    if (txtc.tcb_ptr != NULL)
    {
      /* Notify requesting task TXC is entering traffic state */
      (void) rex_set_sigs( txtc.tcb_ptr, txtc.sigs );
      txtc.tcb_ptr = NULL;
    }
  }
  else
  {
    MSG_ERROR ("Unxpctd jmp src val",0,0,0);
  }
  MSG_HIGH ("Acc or Traf, jump_source=%d,st=%d ",txc_tx.jump_source,state,0);
  return (state);

} /* end txc_access_or_traffic_cmd */
#endif /* FEATURE_RL_JUMP_ENHANCEMENT */

/*===========================================================================

FUNCTION TXC_HYPERSPACE

DESCRIPTION
  Performs jump to hyperspace for Transmit task which has not been able to
  jump since transmit clock has been turned off for power saving.

DEPENDENCIES
  Depends on the access channel mask to be used being placed in
  txc_tx.buf.mask.
RETURN VALUE
  None.
SIDE EFFECTS
  None
===========================================================================*/

txc_state_type txc_hyperspace
(
  txc_state_type state                /* txc_state - TXC_ACC_JUMP_S */
)
{
  word ref_time;
    /* time reference read from Searcher task used to determine phase of
       80 ms superframe */
  ulpn_type ulpn;
    /* type to place user long PN code read from Searcher task */


  /* States of hyperspace state machine */
  typedef enum
  {
    PHASE_2_WAIT_1,
    PHASE_2_WAIT_2,
    PHASE_0_WAIT
  }hype_substate_type;

  hype_substate_type sub_state = PHASE_2_WAIT_1;
    /* current substate of hyperspace jump */
  txc_cmd_type *cmd_ptr;
    /* pointer to received command */
  qword pn_state;

  int isav;
  rex_sigs_type sigs;

#define PHASE_80M 0xC000
  /* mask used to determine phase of 80 ms superframe from time reference
     read from Searcher task */
#define PHASE0 0x0000
  /* value for masked reference time when in 1st 26.66 ms period of 80 ms
     superframe */
#define PHASE1 0x4000
  /* value for masked reference time when in 2nd 26.66 ms period of 80 ms
     superframe */
#define PHASE2 0x8000
  /* value for masked reference time when in 3rd 26.66 ms period of 80 ms
     superframe */
#define MAX_REF_TIME 0xBFFF
  /* maximum value for reference time read from searcher */
#define TX_ENABLE_TIME 50
  /* number of 2-chip increments which must exist before the end of the
     80 ms superframe in order to be able to call enc_tx_clk_enable without
     having the 80ms superframe occur in the middle of the enc_tx_clk_enable
     call */

#ifdef FEATURE_RL_JUMP_ENHANCEMENT
  /* Do not allow sleep so FL timing is not lost.
  ** Need accurate FL timing to calculate RL timing.
  */
  txc_delay_enc_shutdown.txc_sleep_allowed = FALSE;
#endif /* FEATURE_RL_JUMP_ENHANCEMENT */

  ( void )rex_clr_sigs( &tx_tcb,  TXC_PN_ROLL_SIG );
  sigs = 0;

  while( state == TXC_ACC_JUMP_S )
  {
    ( void )rex_clr_sigs( &tx_tcb, TXC_CMD_Q_SIG );
    if( (cmd_ptr = (txc_cmd_type*) q_get( &txc_cmd_q )) != NULL )
    {
      state = txc_access_cmd( cmd_ptr, TXC_ACC_JUMP_S );
    }
    else if (sigs & TXC_PN_ROLL_SIG)
    {
      /* process the PN roll signal */
      ( void )rex_clr_sigs( &tx_tcb,  TXC_PN_ROLL_SIG );
      /* Process PN roll signal */
      ref_time = srch_mux_scomb_get_ref_time();
        /* get time reference from Searcher task */
      switch( sub_state )
      {
        case PHASE_2_WAIT_1:
        {
          MSG_MED ("PHASE_2_WAIT_1",0,0,0);
          /* Get the PN phase from Search */
          if( ((ref_time & PHASE_80M ) == PHASE2 )  &&
              ( ref_time < ( MAX_REF_TIME - TX_ENABLE_TIME )  ) )
          {
            INTLOCK_SAV (isav);

            enc_acp_pa_ctl(FALSE);      /* force off the PA       */
            enc_tx_clk_enable( TRUE );  /* turn on the Enc clocks */
            enc_tx_enable (FALSE);  /* set puncturing off now that clks on */

            /* -----------------------------------
            ** Set PN randomization transmit delay
            ** ----------------------------------- */
            if (txc_tx.jump_source == TXC_JUMP_TRAFFIC)
            {
              /* Reset to zero if moving to Traffic Channel */
              (void) enc_pn_rand_delay( 0 );
            }
            else
            {
#ifdef FEATURE_IS2000_REL_A_CC
              //txc_reach_on
              if ( txc_reach_access == FALSE )
              {
                (void) enc_pn_rand_delay( txac.rn );
              }
#else
              (void) enc_pn_rand_delay( txac.rn );
#endif // FEATURE_IS2000_REL_A_CC
            }

#ifdef FEATURE_MSMHWIO
            HWIO_OUT( MOD_STMR_CMD, 0x00 );
#else
            ENC_OUTH( HW_MOD_STMR_CMD, 0x00 );
#endif /* FEATURE_MSMHWIO */

            sub_state = PHASE_2_WAIT_2;

            INTFREE_SAV (isav);
          }
          break;
        }

        case PHASE_2_WAIT_2:
        {
          MSG_MED ("PHASE_2_WAIT_2",0,0,0);

          if( (ref_time & PHASE_80M ) == PHASE2 )
          {
            INTLOCK_SAV (isav);

            /* Read and advance ULPN state from Search */
            srch_mux_get_ulpn_state( &ulpn );
            /* advance ULPN by 1 26 ms period */
            ulpn_adjust( &ulpn, 1 );
            qw_set ( pn_state, ( dword ) ulpn.high, ulpn.low );

            enc_iq_pn_state( 0x4000, 0x4000 );   /* load I and Q with MSB 1 */
            enc_user_pn_state( pn_state );       /* load user PN state */

#ifndef FEATURE_RL_JUMP_ENHANCEMENT
            /* If feature is turned on, load user PN mask in 
            ** txc_compute_access_chan() or txc_reach_compute_access_chan().
            ** New HW in Jaguar onwards permits the programming of this
            ** mask immediately.  
            */
            enc_user_pn_mask( txc_tx.buf.mask ); /* load user PN mask  */
#endif /* !FEATURE_RL_JUMP_ENHANCEMENT */

            INTFREE_SAV (isav);


#ifndef T_IO_CARD
            /* -------------------------------------------
            ** Power up and program the VHF TX synthesizer
            ** ------------------------------------------- */
            rfm_enable_tx_pwr(RFCOM_TRANSCEIVER_0);

#ifdef FEATURE_RF_WAIT_CHANGES
#error code not present
#else
            (void) rex_set_timer( &txc_rf_timer, RF_ENABLE_PWR_WAIT );
#endif // FEATURE_RF_WAIT_CHANGES

            (void) txc_wait( TXC_RF_TIMER_SIG );
            rfm_tx_enable(RFCOM_TRANSCEIVER_0);
#endif

            sub_state = PHASE_0_WAIT;
          }
          break;
        }

        case PHASE_0_WAIT:
        {
          if( (ref_time & PHASE_80M ) != PHASE0 )
          {
            MSG_HIGH( "Phase err %d", ref_time, 0, 0 );
          }
          else
          {
#ifdef FEATURE_RL_JUMP_ENHANCEMENT
              MSG_HIGH ("PHASE_0_WAIT,jump_source=%d",txc_tx.jump_source,0,0);
              if (txc_tx.jump_source == TXC_READY_TO_JUMP) 
              {
                MSG_ERROR ("Unxpctd Jmp to Hypspc",0,0,0);
              }
              else if (txc_tx.jump_source == TXC_JUMP)
              {
                if ( txac.hold_enc_clk_on )
                {
                  /* Arm the timer to expire after allotted time if no command is received 
                  ** from MC to transition to Traffic or Access state.  Once timer 
                  ** expires, turn off ENC clocks.
                  */
                  txc_delay_enc_shutdown.counter = TXC_HOLD_ENC_CLK_WAIT_DURATION;
 
                  state = TXC_HOLD_ENC_CLK_S;  /* transition to Hold ENC clocks ON state */
                }
                else
                {
                  /* TXC was not told to hold the encoder clocks, so just shut down
                  ** the clocks and transition back to idle state to save battery.
                  */
                  state = TXC_ACC_IDLE_S; /* transition back to Access Idle state */

                  /* Set interrupt handler to null and turn off TX clocks */
                  txc_tx_shutdown();
                }
              }
              else
              {
                state = txc_access_or_traffic_cmd (TXC_ACC_JUMP_S);
              } /* end check for jump_source */ 
#else /* else of #ifdef FEATURE_RL_JUMP_ENHANCEMENT */
#ifdef FEATURE_IS2000_REL_A_CC
              //txc_reach_on
              if ( txc_reach_access == TRUE )
              {
                enc_set_mode( ENC_TC_MODE );  /* set up hardware for reach */

                /* Load ENC_INT_ST register with BIN 0, BIN 1, BIN 2, BIN 3
                ** with power control groups 0 and Walsh Symbols 0
                */

                txac.state = ACC_SETUP1_S;

                txcmod_int_deregister_all(TXMOD_INT_MUX_CLT);
                (void) txcmod_int_regis_for_pcgint(0, txc_reach_isr, ENC_STATUS_INTERRUPT_SLOT_0, TXMOD_INT_MUX_CLT);
                (void) txcmod_int_regis_for_pcgint(4, txc_reach_isr, ENC_STATUS_INTERRUPT_SLOT_1, TXMOD_INT_MUX_CLT);
                (void) txcmod_int_regis_for_pcgint(8, txc_reach_isr, ENC_STATUS_INTERRUPT_SLOT_2, TXMOD_INT_MUX_CLT);
                (void) txcmod_int_regis_for_pcgint(12, txc_reach_isr, ENC_STATUS_INTERRUPT_SLOT_3, TXMOD_INT_MUX_CLT);
              }
              else
              {
                enc_set_mode( ENC_PC_MODE );  /* set up hardware for access */

                txcmod_int_deregister_all(TXMOD_INT_MUX_CLT);
                (void) txcmod_int_regis_for_pcgint(8, txc_access_isr, ENC_STATUS_INTERRUPT_SLOT_2, TXMOD_INT_MUX_CLT);
              }
#else
              txcmod_int_deregister_all(TXMOD_INT_MUX_CLT);                                                         
              (void) txcmod_int_regis_for_pcgint(8, txc_access_isr, ENC_STATUS_INTERRUPT_SLOT_2, TXMOD_INT_MUX_CLT);
#endif // FEATURE_IS2000_REL_A_CC

            /* Send command to interrupt handler to initialize frame count
                because receive interrupt could occur at this time and we
                could get the wrong frame count */
            txc_int_cmd.cmd = INT_FRAME_F;
            DO_INT_CMD();
            if (txc_tx.jump_source != TXC_JUMP_TRAFFIC)
            {
              state = TXC_ACC_ATT_S;  /* ready to transmit message */
            }
            else
            {
              txc_traffic_init();
              state = TXC_TRAFFIC_S;  /* ready to jump to traffic */

              if (txtc.tcb_ptr != NULL)
              {
                /* Notify requesting task TXC is entering traffic state */
                (void) rex_set_sigs( txtc.tcb_ptr, txtc.sigs );
                txtc.tcb_ptr = NULL;
              }
            }
#endif /* FEATURE_RL_JUMP_ENHANCEMENT */
          }
          break;
        }

        default:
        {
          ERR_FATAL( "Bad jmp subst %d", sub_state, 0, 0 );
        }

      } /* switch */
    }
    if( state == TXC_ACC_JUMP_S )
    {
      sigs = txc_wait( TXC_PN_ROLL_SIG | TXC_CMD_Q_SIG  );
    }

  } /* while */


  if (state == TXC_ACC_IDLE_S)
  {
    /* Not going to transmit the access after all */
    MSG_HIGH( "TX Hyprspc jmp abrt", 0, 0, 0 );

    /* Turn off clocks, pa, etc. */
#ifdef FEATURE_RL_JUMP_ENHANCEMENT
    txc_tx_shutdown();
#else
    txc_cdma_enc_shutdown();
#endif /* FEATURE_RL_JUMP_ENHANCEMENT */

    if (txac.tcb_ptr != NULL)
    {
      /* Notify requesting task that access attempt is completed */
      (void) rex_set_sigs( txac.tcb_ptr, txac.sigs );
      txac.tcb_ptr = NULL;
    }
  }

  return (state);

} /* txc_hyperspace */


/*===========================================================================

FUNCTION TXC_ACCESS_ISR

DESCRIPTION
  Interrupt handler for the access channel.  Very simple, just send
  whatever it is given to send.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void txc_access_isr(
   uint32 arg 
)
{
  (void) arg; /* unused */

  txc_tx.int_cnt++;                    /* Count tx interrupts */
  qw_inc( txc_tx.frame_cnt, 1L);       /* Count frames */

  /* Check for commands */
  if (txc_int_cmd.iscmd)
  {
    /* Make sure power amp is off */
    enc_tx_enable( FALSE );

    switch (txc_int_cmd.cmd)
    {
      case INT_TC_F:
        MSG_MED("Got INT_TC_F", 0, 0, 0);
          /* --------------------------
        ** Go to Traffic Channel mode
        ** -------------------------- */
        enc_set_mode( ENC_TC_MODE );

        /* set initial RC */
        #ifdef FEATURE_IS2000_P2
        if ( txc_so.rev_dcch.included )
        {
          enc_dcch_rc( (enc_fch_radio_config_type) txc_so.rev_dcch.rc );
        }
        if ( txc_so.rev_fch.included )
        #endif // FEATURE_IS2000_P2
        {
          enc_fch_rc( (enc_fch_radio_config_type) txc_so.rev_fch.rc );
        }

        /* Load user PN mask */
        enc_user_pn_mask( txc_int_cmd.parm.tc.mask );

        /* Set the counter so that TXC ISR calculates the frame number for
           first NO_OF_ISRS_AFTER_FO isrs. */
        after_fo_handoff_count_isrs = NO_OF_ISRS_AFTER_FO;

        txcmod_int_deregister_all(TXMOD_INT_MUX_CLT);
        (void) txcmod_int_regis_for_pcgint(txc_slot0_isr_pcg,
                                           txc_traffic_isr,
                                           ENC_STATUS_INTERRUPT_SLOT_0,
                                           TXMOD_INT_MUX_CLT);
        (void) txcmod_int_regis_for_pcgint(TXC_SLOT3_ISR_PCG_NUM,
                                           txc_traffic_isr,
                                           ENC_STATUS_INTERRUPT_SLOT_3,
                                           TXMOD_INT_MUX_CLT);
        break;

      case INT_FRAME_F:
        MSG_MED("Got INT_FRAME_F", 0, 0, 0);
        /* ----------------------
        ** Initialize frame count
        ** ---------------------- */

        /* Derive frame from timestamp>>20 */
        ts_get_20ms_frame_time ( txc_tx.frame_cnt );

        /* Increment frame count for correct TX frame count */
        qw_inc( txc_tx.frame_cnt, 1L );

        MSG_MED ("set frame_cnt+1 (%x %x)", qw_hi(txc_tx.frame_cnt), qw_lo(txc_tx.frame_cnt),0);

        /* We are allowed to transmit */
        txc_tx.tx_on = TRUE;
        break;

      default:
        ERR( "Bad cmd %d", txc_int_cmd.cmd, 0, 0 );
    } /* switch */

    txc_int_cmd.iscmd = FALSE;     /* done with command */
    (void) rex_set_sigs( &tx_tcb, TXC_INT_CMD_SIG );

  } /* end if (txc_int_cmd.iscmd) */

  else
  {
    /* We are allowed to transmit */
    txc_tx.tx_on = TRUE;

    /* See if we transmit this frame or not */
    if (txac.frame.dotx)
    {
      /* Make sure power amp is on */
      enc_tx_enable( TRUE );

      TXC_SET_FCH_GAIN( TXC_IS95A_FCH_GAIN );

      /* Re-init TX_RATE_ADJ register */
      rpc_set_tx_rate_adj(0);

      /* Set transmit power */
      srch_mux_set_tx_adj( (byte) txac.power );
      txac.last_pwr = txac.power;    /* Save last power used for traffic */

      /* Always transmit half rate on the access channel */
      enc_fch_data_out( txac.frame.data, FALSE, ENC_HALF_RATE );

      if ((txac.state == ACC_MSG_S) && (txac.first_frame))
      {
        /* Log only first message frame */
        txac.first_frame = FALSE;

#if !defined (FEATURE_DIAG_V7_LOG_IFACE)
        if (acc_log.hdr.len != 0)
        {
          acc_log.hdr.code = LOG_ACC_INFO_C;
          ts_get((unsigned long *) acc_log.hdr.ts);
          acc_log.probe_num = txac.probe_count + 1;
          acc_log.rx_agc = srch_mux_get_rx_agc();
          acc_log.tx_adj = (byte) txac.power;
          /* log the interference-correction */
          acc_log.int_corr = txac.int_corr;
          (void) log_submit((log_type *) &acc_log );
          acc_log.hdr.len = 0;
        }
#else
        if (txac.log_ptr != NULL)
        {
          ts_get( (unsigned long *)(txac.log_ptr->acc.hdr.ts ));
          txac.log_ptr->acc.probe_num = txac.probe_count + 1;
          txac.log_ptr->acc.rx_agc = srch_mux_get_rx_agc();
          txac.log_ptr->acc.tx_adj = (byte) txac.power;
          /* log the interference-correction */
          txac.log_ptr->acc.int_corr = txac.int_corr;
          log_put( &txac.log_ptr->desc );
          txac.log_ptr = NULL;
        }
#endif /* !defined (FEATURE_DIAG_V7_LOG_IFACE)*/
      }
    } /* if (txac.frame.dotx) */

    else /* Not transmitting this frame */
    {
      /* Make sure power amp is off */
      enc_tx_enable( FALSE );
      TXC_SET_FCH_GAIN( 0 );
    }
  }

  /* Let the tx task know that we had an interrupt */

  (void) rex_set_sigs( &tx_tcb, TXC_INT_SIG );

} /* txc_access_isr */


#ifdef FEATURE_IS2000_REL_A_CC
/*===========================================================================

FUNCTION TXC_REACH_PCG_ISR

DESCRIPTION
  Interrupt handler for the enhanced access channel pcg interrupt.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void txc_reach_pcg_isr( void )
{


    MSG_ERROR("reach pcg isr", 0,0,0);

    /* Disable this ISR */
    tramp_set_isr( TRAMP_125_MS_ISR, NULL );

} /* txc_reach_pcg_isr */


/*===========================================================================

FUNCTION TXC_REACH_ISR

DESCRIPTION
  Interrupt handler for the enhanced access channel.  Very simple, just send
  whatever it is given to send.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void txc_reach_isr( 
  uint32 arg 
)
{
  txc_reach_interrupt_slot = (enc_status_interrupt_slot_type) arg;

  if ( txac.state == ACC_PRE_S )
  {
    /* Process preamble programming */

    if ( txc_reach_first_frame_input_data == FALSE )
    {
      /* Transmit REACH message first frame data */

      if (txc_reach_si_include == TRUE)
      {
        if (txc_reach_frame_length == ENC_SCH_FRAME_SIZE_20_V)
        {
          enc_reach_data_out( txtc.supp_frame.data, TRUE, txtc.supp_rate,
              txtc.supp_turbo, txtc.supp_ltu_size, txc_reach_frame_length );
        }
        else /* if (txc_reach_frame_length != ENC_SCH_FRAME_SIZE_20_V) */
        {
          if (txc_reach_frame_cnt != 0)
          {
            /* Input frame 1, bin 0 */
            enc_reach_data_out( txc_reach_frame_1_bin_0, TRUE, txtc.supp_rate,
                txtc.supp_turbo, txtc.supp_ltu_size, txc_reach_frame_length );

            txc_reach_frame_cnt--;

            txc_reach_input_frame = TXC_REACH_FRAME_1;
          }

        } /* end if (txc_reach_frame_length == ENC_SCH_FRAME_SIZE_20_V) */
      }
      else /* if (txc_reach_si_include == FALSE) */
      {

        enc_reach_data_out( txtc.supp_frame.data, TRUE, txtc.supp_rate,
            txtc.supp_turbo, txtc.supp_ltu_size, txc_reach_frame_length );

      } /* end if (txc_reach_si_include == TRUE) */

      txc_reach_first_frame_input_data = TRUE;
    }

    if ( txc_reach_program_preamble_prepare == TRUE )
    {
      if ( txc_reach_interrupt_slot == ENC_STATUS_INTERRUPT_SLOT_0 )
      {
        if ( txc_reach_preamble_count == 1 )
        {
          enc_set_frame_off ( 0x0, (byte)txc_reach_preamble_frame_offset );
          /* tell the MDSP and HW to load frame offset right away */
          muxmdsp_wr_fundFrameOffsetImmLoadCmd(TRUE);
          enc_stmr_cmd( HWIO_FMSK(MOD_STMR_CMD, OFFSET_LD_IMMD) );

          /* Set REACH Preamble gain to constant digital gain 255 (15 bit resolution.) */
          TXC_SET_PCH_GAIN( RPC_TOT_DIG_PWR_255 << 7);
          enc_pch_enable(TRUE);

          /* Set RATE_ADJ to increase Preamble 6 dB and adjust offset power.*/
          rpc_set_tx_rate_adj( txtc.offset_pwr - 72 );

          txc_reach_program_preamble_prepare = FALSE;
          txc_reach_program_data_prepare = TRUE;
          txc_reach_intr_cnt = 0;
        }
      }
      else if ( txc_reach_interrupt_slot == ENC_STATUS_INTERRUPT_SLOT_1 )
      {
        if ( txc_reach_preamble_count == 2 )
        {
          enc_set_frame_off ( 0x0, (byte)txc_reach_preamble_frame_offset );
          /* tell the MDSP and HW to load frame offset right away */
          muxmdsp_wr_fundFrameOffsetImmLoadCmd(TRUE);
          enc_stmr_cmd( HWIO_FMSK(MOD_STMR_CMD, OFFSET_LD_IMMD) );

          /* Set REACH Preamble gain to constant digital gain 255 (15 bit resolution.) */
          TXC_SET_PCH_GAIN( RPC_TOT_DIG_PWR_255 << 7);
          enc_pch_enable(TRUE);

          /* Set RATE_ADJ to increase Preamble 6 dB and adjust offset power.*/
          rpc_set_tx_rate_adj( txtc.offset_pwr - 72 );

          txc_reach_program_preamble_prepare = FALSE;
          txc_reach_program_data_prepare = TRUE;
          txc_reach_intr_cnt = 0;
        }
      }
      else if ( txc_reach_interrupt_slot == ENC_STATUS_INTERRUPT_SLOT_2 )
      {
        if ( txc_reach_preamble_count == 3 )
        {
          enc_set_frame_off ( 0x0, (byte)txc_reach_preamble_frame_offset );
          /* tell the MDSP and HW to load frame offset right away */
          muxmdsp_wr_fundFrameOffsetImmLoadCmd(TRUE);
          enc_stmr_cmd( HWIO_FMSK(MOD_STMR_CMD, OFFSET_LD_IMMD) );

          /* Set REACH Preamble gain to constant digital gain 255 (15 bit resolution.) */
          TXC_SET_PCH_GAIN(RPC_TOT_DIG_PWR_255 << 7);
          enc_pch_enable(TRUE);

          /* Set RATE_ADJ to increase Preamble 6 dB and adjust offset power.*/
          rpc_set_tx_rate_adj( txtc.offset_pwr - 72 );

          txc_reach_program_preamble_prepare = FALSE;
          txc_reach_program_data_prepare = TRUE;
          txc_reach_intr_cnt = 0;
        }
      }
    }
    else if ( txc_reach_program_data_prepare == TRUE )
    {
      if ( ++txc_reach_intr_cnt == txc_reach_data_count )
      {
        // txc_program_data
        enc_set_frame_off ( 0x0, (byte)txc_reach_data_frame_offset );
        /* tell the MDSP and HW to load frame offset right away */
        muxmdsp_wr_fundFrameOffsetImmLoadCmd(TRUE);
        enc_stmr_cmd( HWIO_FMSK(MOD_STMR_CMD, OFFSET_LD_IMMD) );
        TXC_SET_PCH_GAIN( txc_pch_gain );
        TXC_SET_SCH_GAIN( txc_sch_gain );
        enc_pch_enable(TRUE);
        enc_sch_enable(TRUE);

        /* Set RATE_ADJ to adjust pilot channel gain and offset power.*/
        rpc_set_tx_rate_adj( txc_reach_pch_adj_gain + txtc.offset_pwr );

        txc_reach_program_data_prepare = FALSE;

        txac.state = ACC_MSG_S;

          MSG_HIGH( "datoffset = %d, pch= %d, sch gain= %d", 
                    txc_reach_data_frame_offset, txc_pch_gain, txc_sch_gain );

      }
    }
    else if ( txc_reach_interrupt_slot == ENC_STATUS_INTERRUPT_SLOT_3 )
    {
      if ( txc_reach_preamble_count == 0 )
      {
        enc_set_frame_off ( 0x0, (byte)txc_reach_preamble_frame_offset );
        /* tell the MDSP and HW to load frame offset right away */
        muxmdsp_wr_fundFrameOffsetImmLoadCmd(TRUE);
        enc_stmr_cmd( HWIO_FMSK(MOD_STMR_CMD, OFFSET_LD_IMMD) );

        /* Set REACH Preamble gain to constant digital gain 255 (15 bit resolution.) */
        TXC_SET_PCH_GAIN( RPC_TOT_DIG_PWR_255 << 7);
        enc_pch_enable(TRUE);

        /* Set RATE_ADJ to increase Preamble 6 dB and adjust offset power.*/
        rpc_set_tx_rate_adj( txtc.offset_pwr - 72 );

        txc_reach_program_data_prepare = TRUE;
        txc_reach_intr_cnt = 0;

          MSG_HIGH( "pamoffset = %d, pch gain = %d, sch = %d", 
                    txc_reach_preamble_frame_offset, txc_pch_gain, txc_sch_gain );

      }
      else
      {
        txc_reach_program_preamble_prepare = TRUE;
      }
    } /* end if ( txc_reach_program_preamble_prepare == TRUE ) */


  }
  else /* not in preamble programming */
  {
    if ( txc_reach_interrupt_slot == ENC_STATUS_INTERRUPT_SLOT_3 )
    {
      if ( (txc_reach_si_include == TRUE) &&  
           (txc_reach_frame_length != ENC_SCH_FRAME_SIZE_20_V) )
      {
        if (txc_reach_frame_cnt != 0)
        {
          /* Transmit frame */
          enc_tx_enable( TRUE );

          /* Set transmit power */
          srch_mux_set_tx_adj( (byte) txac.power );
          txac.last_pwr = txac.power;    

          if (txc_reach_input_frame == 1)
          {
            enc_reach_data_out( txc_reach_frame_1_bin_0, TRUE, txtc.supp_rate,
                txtc.supp_turbo, txtc.supp_ltu_size, txc_reach_frame_length );
          }
          else
          {
            enc_reach_data_out( txc_reach_frame_2_bin_0, TRUE, txtc.supp_rate,
                txtc.supp_turbo, txtc.supp_ltu_size, txc_reach_frame_length );
          }

          txc_reach_frame_cnt--;

        }
        else /* Not transmit frame */
        {
          /* Make sure power amp is off */
          enc_tx_enable( FALSE );

          TXC_SET_PCH_GAIN( 0 );

          TXC_SET_SCH_GAIN( 0 );

        } /* end if (txc_reach_frame_cnt != 0) */

      }
      else /* if ( (si_include == FALSE) || (frame_length == SIZE_20_V) ) */
      {
        /* See if we transmit this frame or not */
        if (txac.frame.dotx)
        {
          /* Make sure power amp is on */
          enc_tx_enable( TRUE );

          /* Set transmit power */
          srch_mux_set_tx_adj( (byte) txac.power );
          txac.last_pwr = txac.power;    /* Save last power used for traffic */

          /* Transmit REACH on the RC3 supplemental channel */

          enc_reach_data_out( txtc.supp_frame.data, TRUE, txtc.supp_rate,
              txtc.supp_turbo, txtc.supp_ltu_size, txc_reach_frame_length
                          );

        } /* if (txac.frame.dotx) */

        else /* Not transmitting this frame */
        {
          /* Make sure power amp is off */
          enc_tx_enable( FALSE );

          TXC_SET_PCH_GAIN( 0 );

          TXC_SET_SCH_GAIN( 0 );
        }
      } /* end if ( (si_include == TRUE) && (frame_length != SIZE_20_V) ) */
    }
    else if ( txc_reach_interrupt_slot == ENC_STATUS_INTERRUPT_SLOT_0 )
    {

      txc_tx.int_cnt++;                    /* Count tx interrupts */

      /* Input bin 1 data at slot 0 for 5 ms frame only. */
      if ( (txc_reach_si_include == TRUE) &&  
           (txc_reach_frame_length == ENC_SCH_FRAME_SIZE_5_V) )
      {
        if (txc_reach_frame_cnt != 0)
        {
          /* Transmit frame */
          enc_tx_enable( TRUE );

          /* Set transmit power */
          srch_mux_set_tx_adj( (byte) txac.power );
          txac.last_pwr = txac.power;    

          if (txc_reach_input_frame == 1)
          {
            enc_reach_data_out( txc_reach_frame_1_bin_1, TRUE, txtc.supp_rate,
                txtc.supp_turbo, txtc.supp_ltu_size, txc_reach_frame_length );
          }
          else
          {
            enc_reach_data_out( txc_reach_frame_2_bin_1, TRUE, txtc.supp_rate,
                txtc.supp_turbo, txtc.supp_ltu_size, txc_reach_frame_length );
          }

          txc_reach_frame_cnt--;

        }
        else /* Not transmit frame */
        {
          /* Make sure power amp is off */
          enc_tx_enable( FALSE );

          TXC_SET_PCH_GAIN( 0 );

          TXC_SET_SCH_GAIN( 0 );

        } /* end if (txc_reach_frame_cnt != 0) */

      } /* end if ( (si_include == TRUE) && (frame_length == SIZE_5_V) ) */

      /* Check for commands */
      if (txc_int_cmd.iscmd)
      {
        /* Make sure power amp is off */
        enc_tx_enable( FALSE );

        switch (txc_int_cmd.cmd)
        {
          case INT_TC_F:
            MSG_MED("Got INT_TC_F", 0, 0, 0);
            /* --------------------------
            ** Go to Traffic Channel mode
            ** -------------------------- */
            enc_set_mode( ENC_TC_MODE );

            /* set initial RC */
            #ifdef FEATURE_IS2000_P2
            if ( txc_so.rev_dcch.included )
            {
              enc_dcch_rc( (enc_fch_radio_config_type)txc_so.rev_dcch.rc );
            }
            if ( txc_so.rev_fch.included )
            #endif // FEATURE_IS2000_P2
            {
              enc_fch_rc( (enc_fch_radio_config_type) txc_so.rev_fch.rc );
            }

            /* Load user PN mask */
            enc_user_pn_mask( txc_int_cmd.parm.tc.mask );

            /* Set the counter so that TXC ISR calculates the frame number for
               first NO_OF_ISRS_AFTER_FO isrs. */
            after_fo_handoff_count_isrs = NO_OF_ISRS_AFTER_FO;

            txcmod_int_deregister_all(TXMOD_INT_MUX_CLT);
            (void) txcmod_int_regis_for_pcgint(txc_slot0_isr_pcg,
                                               txc_traffic_isr, 
                                               ENC_STATUS_INTERRUPT_SLOT_0, 
                                               TXMOD_INT_MUX_CLT);
            (void) txcmod_int_regis_for_pcgint(TXC_SLOT3_ISR_PCG_NUM,
                                               txc_traffic_isr, 
                                               ENC_STATUS_INTERRUPT_SLOT_3, 
                                               TXMOD_INT_MUX_CLT);

            MSG_HIGH( "traf isr", 0, 0, 0 );

            break;

          case INT_FRAME_F:
            MSG_MED("Got INT_FRAME_F", 0, 0, 0);
            /* ----------------------
            ** Initialize frame count
            ** ---------------------- */

            /* Derive frame from timestamp>>22 */
            ts_get_20ms_frame_time ( txc_tx.frame_cnt );

            /* Increment frame count for correct TX frame count */
            qw_inc( txc_tx.frame_cnt, 1L );

            /* We are allowed to transmit */
            txc_tx.tx_on = TRUE;
            break;

          default:
            ERR( "Bad cmd %d", txc_int_cmd.cmd, 0, 0 );
        } /* switch */

        txc_int_cmd.iscmd = FALSE;     /* done with command */
        (void) rex_set_sigs( &tx_tcb, TXC_INT_CMD_SIG );

      } /* end if (txc_int_cmd.iscmd) */

      else
      {
        /* We are allowed to transmit */
        txc_tx.tx_on = TRUE;

        /* See if we transmit this frame or not */
        if (txac.frame.dotx)
        {
          if ((txac.state == ACC_MSG_S) && (txac.first_frame))
          {
            /* Log only first message frame */
            txac.first_frame = FALSE;
            if (txc_reach_frame_info_log.hdr.len != 0)
            {

              /* initialize the log header: log code, time stamp*/
              txc_reach_frame_info_log.hdr.code = LOG_EA_PROBE_C;
              ts_get((unsigned long *) txc_reach_frame_info_log.hdr.ts);
              /* fill out the REACH channel info structure 
              ** for now, all the fields we don't know, we will set them to zero
              ** later on, we will set them to the corrested values
              */

              txc_reach_frame_info_log.rlgain_com_pilot 
                = txc_reach_rlgain_common_pilot;
              txc_reach_frame_info_log.sub_att_number 
                = txc_reach_sub_att_number + 1;
              txc_reach_frame_info_log.sequence_number 
                = txac.seq_num + 1;
              txc_reach_frame_info_log.access_prob_number 
                = txac.probe_count + 1;
              txc_reach_frame_info_log.each_num_step 
                = txac.num_step+1;
              if (txc_reach_psist_delay<=255)
              {
                txc_reach_frame_info_log.psist_delay
                  = (uint8) txc_reach_psist_delay;
              }
              else
              {
                txc_reach_frame_info_log.psist_delay
                  = 255;
                MSG_ERROR("%d psist fails, logging 255",txc_reach_psist_delay,0,0);
              }
              txc_reach_frame_info_log.sequence_bkoff 
                = txac.bkoff;
              txc_reach_frame_info_log.probe_bkoff 
                = txac.probe_bkoff;
              txc_reach_frame_info_log.each_slot 
                = (uint8) txc_reach_slot_length - 1;
              txc_reach_frame_info_log.fccch_id 
                = txac.pagech;
              txc_reach_frame_info_log.each_id 
                = txac.ra;
              txc_reach_frame_info_log.each_slot_offset1 
                = (uint8) txc_reach_slot_offset1;
              txc_reach_frame_info_log.base_id 
                = txac.base_id;
              txc_reach_frame_info_log.each_slot_offset2 
                = (uint8) txc_reach_slot_offset2;
              txc_reach_frame_info_log.acc_rate 
                = txc_reach_rate_word;
              txc_reach_frame_info_log.rx_agc 
                = srch_mux_get_rx_agc();
              txc_reach_frame_info_log.tx_agc 
                = (byte)((rfm_get_cdma_tx_agc(RFCOM_TRANSCEIVER_0)) >> 2);
              txc_reach_frame_info_log.tx_adj 
                = (byte) txac.power;
              txc_reach_frame_info_log.int_corr 
                = txac.int_corr;
              txc_reach_frame_info_log.acc_preamble_tx_low32bit 
                = qw_lo(txc_reach_preamble_frame_pcg);
              txc_reach_frame_info_log.acc_preamble_tx_high16bit 
                = (uint16) qw_hi(txc_reach_preamble_frame_pcg);
              txc_reach_frame_info_log.pdu_length 
                = txc_tx.buf.len;
              txc_reach_frame_info_log.band_class 
                = cdma.band_class;

              MSG_HIGH("Sending LOG_EA_PROBE_C",0,0,0);
              /* call the function to send out the log */
              (void) log_submit((log_type *) &txc_reach_frame_info_log );
              /* reset the length to be ready for the next log */
              txc_reach_frame_info_log.hdr.len = 0;
            }

          }
        } /* if (txac.frame.dotx) */
      }

      /* Let the tx task know that we had an interrupt */

      (void) rex_set_sigs( &tx_tcb, TXC_INT_SIG );
    }
    else /* slot 1 or slot 2 */
    {
      if ( (txc_reach_si_include == TRUE) &&  
           (txc_reach_frame_length != ENC_SCH_FRAME_SIZE_20_V) )
      {
        /* 5ms frame inputs data at both slot 1 and slot 2 */
        if ( txc_reach_frame_length == ENC_SCH_FRAME_SIZE_5_V )
        {
          if (txc_reach_frame_cnt != 0)
          {
            /* Transmit frame */
            enc_tx_enable( TRUE );

            /* Set transmit power */
            srch_mux_set_tx_adj( (byte) txac.power );
            txac.last_pwr = txac.power;    

            if (txc_reach_input_frame == 1)
            {
              if ( txc_reach_interrupt_slot == ENC_STATUS_INTERRUPT_SLOT_1 )
              {
                /* Input bin 2 data at slot 1 for 5ms frame. */
                enc_reach_data_out( txc_reach_frame_1_bin_2, TRUE, txtc.supp_rate,
                    txtc.supp_turbo, txtc.supp_ltu_size, txc_reach_frame_length );
              }
              else /* SLOT_2 */
              {
                /* Input bin 3 data at slot 2 for 5 ms frame. */
                enc_reach_data_out( txc_reach_frame_1_bin_3, TRUE, txtc.supp_rate,
                    txtc.supp_turbo, txtc.supp_ltu_size, txc_reach_frame_length );

                /* Update input frame for 5ms frame. */
                txc_reach_input_frame = TXC_REACH_FRAME_2;
              }
            }
            else /* FRAME_2 */
            {
              if ( txc_reach_interrupt_slot == ENC_STATUS_INTERRUPT_SLOT_1 )
              {
                /* Input bin 2 data at slot 1 for 5ms frame. */
                enc_reach_data_out( txc_reach_frame_2_bin_2, TRUE, txtc.supp_rate,
                    txtc.supp_turbo, txtc.supp_ltu_size, txc_reach_frame_length );
              }
              else /* SLOT_2 */
              {
                /* Input bin 3 data at slot 2 for 5 ms frame. */
                enc_reach_data_out( txc_reach_frame_2_bin_3, TRUE, txtc.supp_rate,
                    txtc.supp_turbo, txtc.supp_ltu_size, txc_reach_frame_length );

                /* Update input frame for 5ms frame. */
                txc_reach_input_frame = TXC_REACH_FRAME_1;
              }
            }

            txc_reach_frame_cnt--;

          }
          else /* Not transmit frame */
          {
            /* Make sure power amp is off */
            enc_tx_enable( FALSE );

            TXC_SET_PCH_GAIN( 0 );

            TXC_SET_SCH_GAIN( 0 );

          } /* end if (txc_reach_frame_cnt != 0) */
        } /* end if ( txc_reach_frame_length == ENC_SCH_FRAME_SIZE_5_V ) */

        /* 10ms frame inputs data at slot 1 only */
        if ( txc_reach_frame_length == ENC_SCH_FRAME_SIZE_10_V )
        {
          if ( txc_reach_interrupt_slot == ENC_STATUS_INTERRUPT_SLOT_1 )
          {
            if (txc_reach_frame_cnt != 0)
            {
              /* Transmit frame */
              enc_tx_enable( TRUE );

              /* Set transmit power */
              srch_mux_set_tx_adj( (byte) txac.power );
              txac.last_pwr = txac.power;    

              if (txc_reach_input_frame == 1)
              {
                /* Input bin 2 data at slot 1 for 10ms frame. */
                enc_reach_data_out( txc_reach_frame_1_bin_2, TRUE, txtc.supp_rate,
                    txtc.supp_turbo, txtc.supp_ltu_size, txc_reach_frame_length );

                /* Update input frame for 10ms frame. */
                  txc_reach_input_frame = TXC_REACH_FRAME_2;
              }
              else /* FRAME_2 */
              {
                /* Input bin 2 data at slot 1 for 10ms frame. */
                enc_reach_data_out( txc_reach_frame_2_bin_2, TRUE, txtc.supp_rate,
                    txtc.supp_turbo, txtc.supp_ltu_size, txc_reach_frame_length );

                /* Update input frame for 10ms frame. */
                txc_reach_input_frame = TXC_REACH_FRAME_1;
              }

              txc_reach_frame_cnt--;

            }
            else /* Not transmit frame */
            {
              /* Make sure power amp is off */
              enc_tx_enable( FALSE );

              TXC_SET_PCH_GAIN( 0 );

              TXC_SET_SCH_GAIN( 0 );

            } /* end if (txc_reach_frame_cnt != 0) */
          } /* end if ( txc_reach_interrupt_slot == ENC_STATUS_INTERRUPT_SLOT_1 ) */
        } /* end if ( txc_reach_frame_length == ENC_SCH_FRAME_SIZE_10_V ) */

      } /* end if ( (si_include == TRUE) && (frame_length != SIZE_20_V) ) */


      /* Check for commands */
      if (txc_int_cmd.iscmd)
      {
        /* Make sure power amp is off */
        enc_tx_enable( FALSE );

        switch (txc_int_cmd.cmd)
        {
          case INT_TC_F:
            MSG_MED("Got INT_TC_F", 0, 0, 0);
            /* --------------------------
            ** Go to Traffic Channel mode
            ** -------------------------- */
            enc_set_mode( ENC_TC_MODE );

            /* set initial RC */
            #ifdef FEATURE_IS2000_P2
            if ( txc_so.rev_dcch.included )
            {
              enc_dcch_rc( (enc_fch_radio_config_type) txc_so.rev_dcch.rc );
            }
            if ( txc_so.rev_fch.included )
            #endif // FEATURE_IS2000_P2
            {
              enc_fch_rc( (enc_fch_radio_config_type) txc_so.rev_fch.rc );
            }

            /* Load user PN mask */
            enc_user_pn_mask( txc_int_cmd.parm.tc.mask );

            /* Set the counter so that TXC ISR calculates the frame number for
               first NO_OF_ISRS_AFTER_FO isrs. */
            after_fo_handoff_count_isrs = NO_OF_ISRS_AFTER_FO;

            txcmod_int_deregister_all(TXMOD_INT_MUX_CLT);
            (void) txcmod_int_regis_for_pcgint(txc_slot0_isr_pcg,
                                               txc_traffic_isr, 
                                               ENC_STATUS_INTERRUPT_SLOT_0, 
                                               TXMOD_INT_MUX_CLT);
            (void) txcmod_int_regis_for_pcgint(TXC_SLOT3_ISR_PCG_NUM,
                                               txc_traffic_isr, 
                                               ENC_STATUS_INTERRUPT_SLOT_3, 
                                               TXMOD_INT_MUX_CLT);

            MSG_HIGH( "traf isr", 0, 0, 0 );

            break;

          case INT_FRAME_F:
            MSG_MED("Got INT_FRAME_F", 0, 0, 0);
            /* ----------------------
            ** Initialize frame count
            ** ---------------------- */

            /* Derive frame from timestamp>>22 */
            ts_get_20ms_frame_time ( txc_tx.frame_cnt );

            /* Increment frame count for correct TX frame count */
            qw_inc( txc_tx.frame_cnt, 1L );

            /* We are allowed to transmit */
            txc_tx.tx_on = TRUE;
            break;

          default:
            ERR( "Bad cmd %d", txc_int_cmd.cmd, 0, 0 );
        } /* switch */

        txc_int_cmd.iscmd = FALSE;     /* done with command */
        (void) rex_set_sigs( &tx_tcb, TXC_INT_CMD_SIG );

      } /* end if (txc_int_cmd.iscmd) */

    } /* end if ( enc_interrupt_slot() == ENC_STATUS_INTERRUPT_SLOT_3 ) */

  } /*  end if ( txac.state == ACC_PRE_S) */

} /* txc_reach_isr */
#endif // FEATURE_IS2000_REL_A_CC

/*===========================================================================

FUNCTION TXC_ACC_MSG_PREP

DESCRIPTION
  Process request to send an Access Channel message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  txac and txc_tx buffers are set up for message transmission.

===========================================================================*/
void txc_acc_msg_prep
(
  txc_cmd_type *cmd_ptr
    /* Pointer to received command */
)
{
  word len;           /* Length of Access Channel message */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* --------------------------------------------------------------------
  ** Number of Access Channel frames that mobile stations are to transmit
  ** in each Access Channel preamble
  ** -------------------------------------------------------------------- */
  txac.preambles = cmd_ptr->acc.pam_sz + 1;

  /* ----------------------------------------------------------------
  ** Set Access Channel slot length ((1 + PAM_SZ) + (3 + MAX_CAP_SZ))
  ** ---------------------------------------------------------------- */
  txac.slotting = (1 + cmd_ptr->acc.pam_sz) + (3 + cmd_ptr->acc.max_cap_sz);

  txac.bkoff = cmd_ptr->acc.bkoff;              /* Sequence backoff */
  txac.probe_bkoff = cmd_ptr->acc.probe_bkoff;  /* Probe backoff */
  txac.acc_tmo = cmd_ptr->acc.acc_tmo;          /* Ack timeout */
  txac.max_seq = cmd_ptr->acc.seq_num;          /* Max probe sequences */
  txac.seq_num = 0;                             /* Sequence number */
  txac.req = cmd_ptr->acc.req;                  /* Request or response */

  /* -----------------------------------------
  ** Calculate initial power offset for access
  ** ----------------------------------------- */
  if ((cmd_ptr->acc.init_pwr & 0x10))  /* Sign bit (5 bit field) */
  {
    /* Negative value --> sign extend to 8 bits */
    txac.power = (byte) (cmd_ptr->acc.init_pwr | 0xE0);
  }
  else
  {
    txac.power = (cmd_ptr->acc.init_pwr & 0x1F);
  }

  /* -----------------------------------------------------------------
  ** Add in nom_pwr which is already sign extended and, if necessary,
  ** adjusted according to nom_pwr_ext.
  ** ----------------------------------------------------------------- */
  txac.power += cmd_ptr->acc.nom_pwr;

  /* Invert to turn power adjustment into tx gain adjustment */
  txac.power = -(txac.power); //lint !e732: Loss of sign (sign maintainied manually)

  /* Convert to units of 1/2 dB */
  // init_pwr is 5 bit, nom_pwr is 4 bit.  Sum can be 6 bit.  Product (*2) can be only 7.
  txac.power = txac.power * 2; //lint !e734: Loss of precision (9 to 8 bits)

  txac.init_pwr = txac.power;

  /* initialize the is95a power accumulation */
  txac.is95a_pwr = txac.init_pwr;

  /* Grab power step from message in 1dB steps */
  txac.pstep = (byte) (cmd_ptr->acc.pwr_step);

  /* Convert to units of 1/2 dB */
  // pwr_step is 3 bit. Product (*2) can only be 4.
  txac.pstep = txac.pstep * 2; //lint !e734: Loss of precision (9 to 8 bits)

  txac.nstep = cmd_ptr->acc.num_step;      /* Number of access probes */
  txac.num_step = txac.nstep;              /* # of access probes remaining */
  txac.probe_count = 0;                    /* Access probe number */
  txac.attempt_probe_count = 0;            /* Access attempt probe number */
  txac.first_frame = FALSE;                /* Not first frame */
  txac.rn = cmd_ptr->acc.rn;               /* PN randomization delay */
  txac.p = cmd_ptr->acc.p;                 /* Persistence test value */
  txac.acc_chan = cmd_ptr->acc.acc_chan;   /* Number of access channels */
  txac.use_chan = cmd_ptr->acc.use_chan;   /* Use specific channel */
  txac.pagech   = cmd_ptr->acc.pagech;     /* Current Paging Channel */
  txac.base_id  = cmd_ptr->acc.base_id;    /* Base station identification */
  txac.pilot_pn = cmd_ptr->acc.pilot_pn;   /* Pilot PN seq offset index */
  txc_tx.buf.cmd_ptr = NULL;    /* Don't save command info */
  len = cmd_ptr->acc.len;       /* Get Access Channel message length */

  /* Perform a check on the length of the Access Channel Message */
  /* Note that len is in bits.                                   */
  ASSERT(len <= (CAI_AC_MAX_SIZE - (CAI_AC_CRC_SIZE + CAI_AC_LGTH_SIZE)));

  /* Zero the byte in which the message body ends */
  txc_tx.buf.msg[(len >> 3) + 1] = 0; /* Pad end with 0's */

  /* -----------------------------------------------------------------------
  ** If the (len mod 8) is between 3 and 7 (i.e. the message body uses more
  ** than 2 bits of its last byte) then the CRC will begin in the byte after
  ** the one in which the message body ends.  If that is the case, we need
  ** to also zero the two bits preceeding the 30 bit CRC in that byte.
  ** ----------------------------------------------------------------------- */
  txc_tx.buf.msg[(len >> 3) + 2] = 0; /* Pad end with 0's */

  /* Copy the actual message */
  b_copy( cmd_ptr->acc.msg_ptr, 0,       /* Source */
          &txc_tx.buf.msg[1], 0, len );  /* Destination, leave room for len */

  /* Add in CRC and MSG_LENGTH fields */
  len += CAI_AC_CRC_SIZE + CAI_AC_LGTH_SIZE;

  if (len % 8)
  {
    /* Pad length out to byte boundary */
    len = (len & ~7) + 8;
  }

  txc_tx.buf.msg[0] = (byte) (len >> 3);  /* Save length in bytes */

  b_packd( crc_30_calc( txc_tx.buf.msg, len-30 ), /* Calculate CRC */
           txc_tx.buf.msg, len-30, 30 );          /* Add to message */

  /*
  ** Save length in bits to actually transmit. If we make this the size
  ** of the message capsule then we will transmit all the frames of the
  ** capsule and transmit zeroes when we run out of message.
  */
  txc_tx.buf.len = (3 + cmd_ptr->acc.max_cap_sz) * TXC_ACC;

  /* -----------------------------------------------------------------
  ** Before transmitting each access probe sequence, generate a random
  ** number, RA, from 0 to ACC_CHANs.  Use RA as the Access Channel
  ** number for all access probes in that access probe sequence.
  ** ----------------------------------------------------------------- */
  txc_compute_access_chan();

  txc_tx.active_msg = TRUE;

  if (cmd_ptr->acc.tcb_ptr != NULL)
  {
    /* Save pointer to task to signal back and signal to use */
    txac.tcb_ptr = cmd_ptr->acc.tcb_ptr;
    txac.sigs = cmd_ptr->acc.sigs;
  }

} /* txc_acc_msg_prep */


#ifdef FEATURE_IS2000_REL_A_CC
//txc_reach_on

/*===========================================================================

FUNCTION TXC_REACH_MSG_PREP

DESCRIPTION
  Process request to send an Access Channel message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  txac and txc_tx buffers are set up for message transmission.

===========================================================================*/
void txc_reach_msg_prep
(
  txc_cmd_type *cmd_ptr
    /* Pointer to received command */
)
{
  word len;           /* Length of Access Channel message */

  word reach_len;     /* Length of Enhanced Access Channel message */


  int i;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* --------------------------------------------------------------------
  ** Number of Access Channel frames that mobile stations are to transmit
  ** in each Access Channel preamble
  ** -------------------------------------------------------------------- */
  //txac.preambles = cmd_ptr->reach.pam_sz + 1;

  /* ----------------------------------------------------------------
  ** Set Access Channel slot length ((1 + PAM_SZ) + (3 + MAX_CAP_SZ))
  ** ---------------------------------------------------------------- */
  //txac.slotting = (1 + cmd_ptr->reach.pam_sz) + (3 + cmd_ptr->reach.max_cap_sz);

  txac.bkoff = cmd_ptr->reach.bkoff;              /* Sequence backoff */
  txac.probe_bkoff = cmd_ptr->reach.probe_bkoff;  /* Probe backoff */
  txac.acc_tmo = cmd_ptr->reach.acc_tmo;          /* Ack timeout */
  txac.max_seq = cmd_ptr->reach.seq_num;          /* Max probe sequences */
  txac.seq_num = 0;                             /* Sequence number */
  txac.req = cmd_ptr->reach.req;                  /* Request or response */

  /* -----------------------------------------
  ** Calculate initial power offset for access
  ** ----------------------------------------- */
  if ((cmd_ptr->reach.init_pwr & 0x10))  /* Sign bit (5 bit field) */
  {
    /* Negative value --> sign extend to 8 bits */
    txac.power = (byte) (cmd_ptr->reach.init_pwr | 0xE0);
  }
  else
  {
    txac.power = (cmd_ptr->reach.init_pwr & 0x1F);
  }

  /* -----------------------------------------------------------------
  ** Add in nom_pwr which is already sign extended and, if necessary,
  ** adjusted according to nom_pwr_ext.
  ** ----------------------------------------------------------------- */
  txac.power += cmd_ptr->reach.nom_pwr;

  /* Invert to turn power adjustment into tx gain adjustment */
  txac.power = -(txac.power); //lint !e732: Loss of sign (sign is maintained manually)

  /* Convert to units of 1/2 dB */
  // num_pwr is 5 bit, init_pwr is 5 bits.  Sum is 6 bits, product (*2) is 7
  txac.power = txac.power * 2; //lint !e734: Loss of precision (9 to 8 bits)

  txac.init_pwr = txac.power;

  /* initialize the is95a power accumulation */
  txac.is95a_pwr = txac.init_pwr;

  /* Grab power step from message in 1dB steps */
  txac.pstep = (byte) (cmd_ptr->reach.pwr_step);

  /* Convert to units of 1/2 dB */
  // pwr_step is 3 bits.  Product (*2) is 4 bits.
  txac.pstep = txac.pstep * 2; //lint !e734: Loss of precision (9 to 8 bits)

  txac.nstep = cmd_ptr->reach.num_step;      /* Number of access probes */
  txac.num_step = txac.nstep;              /* # of access probes remaining */
  txac.probe_count = 0;                    /* Access probe number */
  txac.attempt_probe_count = 0;            /* Access attempt probe number */
  txac.first_frame = FALSE;                /* Not first frame */
  txac.p = cmd_ptr->reach.p;                 /* Persistence test value */
  txac.acc_chan = cmd_ptr->reach.num_each_ba;   /* Number of access channels */
  txac.pagech   = cmd_ptr->reach.fccch;       /* Current Paging Channel */
  txac.base_id  = cmd_ptr->reach.base_id;    /* Base station identification */
  txac.pilot_pn = cmd_ptr->reach.pilot_pn;   /* Pilot PN seq offset index */
  txc_tx.buf.cmd_ptr = NULL;    /* Don't save command info */
  len = cmd_ptr->reach.len;       /* Get Access Channel message length */

  /* Perform a check on the length of the Access Channel Message */
  /* Note that len is in bits.                                   */
  ASSERT(len <= ( (MAX_REV_MSG_BYTES*8) - (CAI_EA_CRC_SIZE + CAI_EA_LGTH_SIZE))); //lint !e506: Constant value Boolean

  // CAI_AC_MAX_SIZE = 255*8 bits
  // CAI_AC_CRC_SIZE = 30 bits
  // CAI_AC_LGTH_SIZE = 8 bits
  // For REACH, MSG_LENGTH can be 8 bits or 16 bits
  // If len <= 978 bits, EXT_MSG_LENGTH = 0, MSG_LENGTH = 7 bits
  // If len > 978 bits, EXT_MSG_LENGTH = 1, MSG_LENGTH = 15 bits
  // Need to look at the place CAI_AC_LGTH_SIZE = 8 bits
  // Need to use 8 bits or 16 bits for checking
  // Need to use 8 bits or 16 bits for calculation


  if (len > 978) /* Ext_Msg_Length is 1 bit, Msg_Length is 15 bits */
  {
    /* Zero the byte in which the message body ends */
    txc_tx.buf.msg[(len >> 3) + 2] = 0; /* Pad end with 0's */

    /* -----------------------------------------------------------------------
    ** If the (len mod 8) is between 3 and 7 (i.e. the message body uses more
    ** than 2 bits of its last byte) then the CRC will begin in the byte after
    ** the one in which the message body ends.  If that is the case, we need
    ** to also zero the two bits preceeding the 30 bit CRC in that byte.
    ** ----------------------------------------------------------------------- */
    txc_tx.buf.msg[(len >> 3) + 3] = 0; /* Pad end with 0's */

    /* Copy the actual message */
    b_copy( cmd_ptr->reach.msg_ptr, 0,       /* Source */
              &txc_tx.buf.msg[2], 0, len );  /* Destination, leave room for len */

    /* Add in CRC and MSG_LENGTH fields */
    len += CAI_EA_CRC_SIZE + CAI_EA_LGTH_SIZE;


    // CAI_AC_CRC_SIZE = 30 bits
    // CAI_AC_LGTH_SIZE = 8 bits
    // For REACH, MSG_LENGTH can be 8 bits or 16 bits
    // If len <= 978 bits, EXT_MSG_LENGTH = 0, MSG_LENGTH = 7 bits
    // If len > 978 bits, EXT_MSG_LENGTH = 1, MSG_LENGTH = 15 bits
    // Need to look at the place CAI_AC_LGTH_SIZE = 8 bits
    // Need to use 8 bits or 16 bits for checking
    // Need to use 8 bits or 16 bits for calculation


    if (len % 8)
    {
      /* Pad length out to byte boundary */
      len = (len & ~7) + 8;
    }

    reach_len = (len >> 3);

    txc_tx.buf.msg[1] = (byte) reach_len;  /* Save Msg_Length in bytes */

    /* Save Ext_Msg_Length as 1 and Msg_length in bytes */
    txc_tx.buf.msg[0] = (byte) ((reach_len | 0x8000) >> 8);  

    b_packd( crc_30_calc( txc_tx.buf.msg, len-30 ), /* Calculate CRC */
             txc_tx.buf.msg, len-30, 30 );          /* Add to message */

  }
  else /* Msg_Length is 8 bits */
  {

    /* Zero the byte in which the message body ends */
    txc_tx.buf.msg[(len >> 3) + 1] = 0; /* Pad end with 0's */

    /* -----------------------------------------------------------------------
    ** If the (len mod 8) is between 3 and 7 (i.e. the message body uses more
    ** than 2 bits of its last byte) then the CRC will begin in the byte after
    ** the one in which the message body ends.  If that is the case, we need
    ** to also zero the two bits preceeding the 30 bit CRC in that byte.
    ** ----------------------------------------------------------------------- */
    txc_tx.buf.msg[(len >> 3) + 2] = 0; /* Pad end with 0's */

    /* Copy the actual message */
    b_copy( cmd_ptr->reach.msg_ptr, 0,       /* Source */
            &txc_tx.buf.msg[1], 0, len );  /* Destination, leave room for len */

    /* Add in CRC and MSG_LENGTH fields */
    len += CAI_AC_CRC_SIZE + CAI_AC_LGTH_SIZE;


    // CAI_AC_CRC_SIZE = 30 bits
    // CAI_AC_LGTH_SIZE = 8 bits
    // For REACH, MSG_LENGTH can be 8 bits or 16 bits
    // If len <= 978 bits, EXT_MSG_LENGTH = 0, MSG_LENGTH = 7 bits
    // If len > 978 bits, EXT_MSG_LENGTH = 1, MSG_LENGTH = 15 bits
    // Need to look at the place CAI_AC_LGTH_SIZE = 8 bits
    // Need to use 8 bits or 16 bits for checking
    // Need to use 8 bits or 16 bits for calculation


    if (len % 8)
    {
      /* Pad length out to byte boundary */
      len = (len & ~7) + 8;
    }

    txc_tx.buf.msg[0] = (byte) (len >> 3);  /* Save length in bytes */

    b_packd( crc_30_calc( txc_tx.buf.msg, len-30 ), /* Calculate CRC */
             txc_tx.buf.msg, len-30, 30 );          /* Add to message */

  } /*  end if (len > 978) */


  /* -----------------------------------------------------------------
  ** Before transmitting each access probe sequence, generate a random
  ** number, RA, from 0 to ACC_CHANs.  Use RA as the Access Channel
  ** number for all access probes in that access probe sequence.
  ** ----------------------------------------------------------------- */
  txc_reach_compute_access_chan();

  txc_reach_slot_length = (word) (cmd_ptr->reach.slot + 1);
  
  txc_reach_slot_offset1 = (word) (cmd_ptr->reach.slot_offset1);
  txc_reach_slot_offset2 = (word) (cmd_ptr->reach.slot_offset2);
  
  // T = N(P+B)+A = N ( P + 0 ) + 0
  // T = NP = (preamble_num_frac + 1)(preamble_frac_duration + 1)
  // preamble_enabled = true
  // preamble_num_frac = 0
  // preamble_frac_duration = 3
  // preamble_off_duration = 0
  // preamble_add_duration = 0
  // T = (preamble_num_frac + 1)(preamble_frac_duration + 1)
  // T = (0 + 1)( 3 + 1 ) = 1 * 4 = 4
  
  txc_reach_preamble_length = 
  (word)((cmd_ptr->reach.preamble_num_frac + 1) * (cmd_ptr->reach.preamble_frac_duration + 1));
  
  
  txc_reach_rlgain_common_pilot = cmd_ptr->reach.rlgain_common_pilot;
  
  // IC_THRESH from the base station in units of 1/2 dB
  txc_reach_ic_thresh = cmd_ptr->reach.ic_thresh;  
  
  // IC_MAX from the base station in units of  1/2 dB
  txc_reach_ic_max = cmd_ptr->reach.ic_max;        
  
  // IC_THRESH in units of 1/2 dB for interference correction calculation
  // ic_thresh is 4 bit, product (*2) is 5 bit.
  txc_reach_ic_thresh = txc_reach_ic_thresh * 2;   //lint !e734: Loss of precision (9 to 8 bits)
  
  // IC_MAX in units of  1/2 dB for interference correction calculation
  // ic_max is 4 bit, product (*2) is 5 bit.
  txc_reach_ic_max = txc_reach_ic_max * 2; //lint !e734: Loss of precision (9 to 8 bits)
  
  
  MSG_HIGH("rlgain=%x ic_thresh=%x ic_max=%x",txc_reach_rlgain_common_pilot,txc_reach_ic_thresh,txc_reach_ic_max);
  
  
  for( i = 0; i < ENC_SCH_RC3_16X_SIZE; i++ )
  {
    txtc.supp_frame.data[ i ] = 0x5A;
  }
  
  if ( cmd_ptr->reach.rate == TXC_EACH_RATE_9600_20_V )
  {
    MSG_HIGH("REACH rate 9600, 20ms",0,0,0);
  
    txc_reach_frame_length = ENC_SCH_FRAME_SIZE_20_V;
  
    txtc.supp_rate = ENC_SCH_FULL_RATE;
  
    txc_reach_frame_size = 172; /* 20ms, 9600bps, 172 information bits */
  }
  else if ( cmd_ptr->reach.rate == TXC_EACH_RATE_19200_20_V )
  {
    MSG_HIGH("REACH rate 19200, 20ms",0,0,0);
  
    txc_reach_frame_length = ENC_SCH_FRAME_SIZE_20_V;
  
    txtc.supp_rate = ENC_SCH_2X_RATE;
  
    txc_reach_frame_size = 360; /* 20ms, 19200bps, 360 information bits */
  }
  else if ( cmd_ptr->reach.rate == TXC_EACH_RATE_19200_10_V )
  {
    MSG_HIGH("REACH rate 19200, 10ms",0,0,0);
  
    txc_reach_frame_length = ENC_SCH_FRAME_SIZE_10_V;
  
    txtc.supp_rate = ENC_SCH_FULL_RATE;
  
    txc_reach_frame_size = 172; /* 10ms, 19200bps, 172 information bits */
  }
  else if ( cmd_ptr->reach.rate == TXC_EACH_RATE_38400_20_V )
  {
    MSG_HIGH("REACH rate 38400, 20ms",0,0,0);
  
    txc_reach_frame_length = ENC_SCH_FRAME_SIZE_20_V;
  
    txtc.supp_rate = ENC_SCH_4X_RATE;
  
    txc_reach_frame_size = 744; /* 20ms, 38400bps, 744 information bits */
  }
  else if ( cmd_ptr->reach.rate == TXC_EACH_RATE_38400_10_V )
  {
    MSG_HIGH("REACH rate 38400, 10ms",0,0,0);
  
    txc_reach_frame_length = ENC_SCH_FRAME_SIZE_10_V;
  
    txtc.supp_rate = ENC_SCH_2X_RATE;
  
    txc_reach_frame_size = 360; /* 10ms, 38400bps, 360 information bits */
  }
  else if ( cmd_ptr->reach.rate == TXC_EACH_RATE_38400_5_V )
  {
    MSG_HIGH("REACH rate 38400, 5ms",0,0,0);
  
    txc_reach_frame_length = ENC_SCH_FRAME_SIZE_5_V;
  
    txtc.supp_rate = ENC_SCH_FULL_RATE;
  
    txc_reach_frame_size = 172; /* 5ms, 38400bps, 172 information bits */
  }
  
  txc_reach_rate_word = cmd_ptr->reach.rate;
  
  /* Get REACH data gain based on the rate word for data channel. */
  txc_sch_gain = rpc_get_reach_data_gain( cmd_ptr->reach.rate );
  
  /* Get REACH pilot gain based on the rate word for data channel. */
  txc_pch_gain = rpc_get_reach_pilot_gain( cmd_ptr->reach.rate );
  
  /* Get REACH pilot gain adjustment for TX_ADJ based on the pilot gain. */
  txc_reach_pch_adj_gain = rpc_get_pch_adj_gain(PCH_GAIN_TRUNCATE_TO_8BITS(txc_pch_gain));
  
  /*
  ** Save length in bits to actually transmit. If we make this the multiple
  ** of the frame size then we will transmit all the frames of the
  ** message and transmit zeros when we run out of message.
  ** If the buf.len is not the integer number of the frame size, 
  ** need to make the buf.len round up to the frame size and  
  ** pad 0 bits to make the frame size.
  */
  if (txc_reach_si_include == TRUE)
  {
    /* 
    ** Set transmitting buffer length to message length.
    ** txc_reach_access_att will copy SI bits of frame buffer with zeros, 
    ** copy following message data bits from message buffer to frame buffer,
    ** copy remaining bits of frame buffer with zeros,
    ** transmit txc_reach_frame_size information bits. 
    */
  
    txc_tx.buf.len = len;
  }
  else /* if (txc_reach_si_include == FALSE) */
  {
    if ( len % txc_reach_frame_size )
    {
      // We need to pad 0 bits here. 
  
      // This set of operations will not overflow 16 bits because is divides by the
      // same amount it multiplies by, and the division occurs first.
      txc_tx.buf.len = ((len / txc_reach_frame_size) + 1) * txc_reach_frame_size; //lint !e734: Loss of precision (31 to 16 bits)
    }
    else
    {
      txc_tx.buf.len = len;
    }
  } /* end if (txc_reach_si_include == TRUE) */
  
  txtc.supp_turbo = FALSE;
  txtc.supp_ltu_size = 0;

  txc_tx.active_msg = TRUE;

  if (cmd_ptr->reach.tcb_ptr != NULL)
  {
    /* Save pointer to task to signal back and signal to use */
    txac.tcb_ptr = cmd_ptr->reach.tcb_ptr;
    txac.sigs = cmd_ptr->reach.sigs;
  }

} /* txc_reach_msg_prep */


//txc_reach_on
#endif // FEATURE_IS2000_REL_A_CC


#ifdef FEATURE_IS2000_REL_A_CC
#if defined(FEATURE_IS95B_PILOT_RPT)

/*===========================================================================

FUNCTION TXC_REACH_PROBE_PREP

DESCRIPTION
  Process request to send an Enhanced Access Channel message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  txac and txc_tx buffers are set up for message transmission.

===========================================================================*/
void txc_reach_probe_prep
(
  txc_cmd_type *cmd_ptr
    /* Pointer to received command */
)
{
  word len;           /* Length of Access Channel message */

  word reach_len;     /* Length of Enhanced Access Channel message */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  len = cmd_ptr->reach.len;       /* Get Access Channel message length */

  /* Perform a check on the length of the Access Channel Message */
  /* Note that len is in bits.                                   */
  ASSERT(len <= ( (MAX_REV_MSG_BYTES*8) - (CAI_EA_CRC_SIZE + CAI_EA_LGTH_SIZE))); //lint !e506: Constant value Boolean

  if (len > 978) /* Ext_Msg_Length is 1 bit, Msg_Length is 15 bits */
  {
    /* Zero the byte in which the message body ends */
    txc_tx.buf.msg[(len >> 3) + 2] = 0; /* Pad end with 0's */

    /* -----------------------------------------------------------------------
    ** If the (len mod 8) is between 3 and 7 (i.e. the message body uses more
    ** than 2 bits of its last byte) then the CRC will begin in the byte after
    ** the one in which the message body ends.  If that is the case, we need
    ** to also zero the two bits preceeding the 30 bit CRC in that byte.
    ** ----------------------------------------------------------------------- */
    txc_tx.buf.msg[(len >> 3) + 3] = 0; /* Pad end with 0's */

    /* Copy the actual message */
    b_copy( cmd_ptr->reach.msg_ptr, 0,       /* Source */
              &txc_tx.buf.msg[2], 0, len );  /* Destination, leave room for len */

    /* Add in CRC and MSG_LENGTH fields */
    len += CAI_EA_CRC_SIZE + CAI_EA_LGTH_SIZE;

    if (len % 8)
    {
      /* Pad length out to byte boundary */
      len = (len & ~7) + 8;
    }

    reach_len = (len >> 3);

    txc_tx.buf.msg[1] = (byte) reach_len;  /* Save Msg_Length in bytes */

    /* Save Ext_Msg_Length as 1 and Msg_length in bytes */
    txc_tx.buf.msg[0] = (byte) ((reach_len | 0x8000) >> 8);  

    b_packd( crc_30_calc( txc_tx.buf.msg, len-30 ), /* Calculate CRC */
             txc_tx.buf.msg, len-30, 30 );          /* Add to message */

  }
  else /* Msg_Length is 8 bits */
  {

    /* Zero the byte in which the message body ends */
    txc_tx.buf.msg[(len >> 3) + 1] = 0; /* Pad end with 0's */

    /* -----------------------------------------------------------------------
    ** If the (len mod 8) is between 3 and 7 (i.e. the message body uses more
    ** than 2 bits of its last byte) then the CRC will begin in the byte after
    ** the one in which the message body ends.  If that is the case, we need
    ** to also zero the two bits preceeding the 30 bit CRC in that byte.
    ** ----------------------------------------------------------------------- */
    txc_tx.buf.msg[(len >> 3) + 2] = 0; /* Pad end with 0's */

    /* Copy the actual message */
    b_copy( cmd_ptr->reach.msg_ptr, 0,       /* Source */
          &txc_tx.buf.msg[1], 0, len );  /* Destination, leave room for len */

    /* Add in CRC and MSG_LENGTH fields */
    len += CAI_AC_CRC_SIZE + CAI_AC_LGTH_SIZE;

    if (len % 8)
    {
      /* Pad length out to byte boundary */
      len = (len & ~7) + 8;
    }

    txc_tx.buf.msg[0] = (byte) (len >> 3);  /* Save length in bytes */

    b_packd( crc_30_calc( txc_tx.buf.msg, len-30 ), /* Calculate CRC */
             txc_tx.buf.msg, len-30, 30 );          /* Add to message */

  } /*  end if (len > 978) */

  /*
  ** Save length in bits to actually transmit. If we make this the multiple
  ** of the frame size then we will transmit all the frames of the
  ** message and transmit zeros when we run out of message.
  ** If the buf.len is not the integer number of the frame size, 
  ** need to make the buf.len round up to the frame size and  
  ** pad 0 bits to make the frame size.
  */
  if (txc_reach_si_include == TRUE)
  {
    /* 
    ** Set transmitting buffer length to message length.
    ** txc_reach_access_att will copy SI bits of frame buffer with zeros, 
    ** copy following message data bits from message buffer to frame buffer,
    ** copy remaining bits of frame buffer with zeros,
    ** transmit txc_reach_frame_size information bits. 
    */

    txc_tx.buf.len = len;
  }
  else /* if (txc_reach_si_include == FALSE) */
  {
    if ( len % txc_reach_frame_size )
    {
      // We need to pad 0 bits here. 

      // This set of operations will not overflow 16 bits because is divides by the
      // same amount it multiplies by, and the division occurs first.
      txc_tx.buf.len = ((len / txc_reach_frame_size) + 1) * txc_reach_frame_size; //lint !e734: Loss of precision (31 to 16 bits)
    }
    else
    {
      txc_tx.buf.len = len;
    }
  } /* end if (txc_reach_si_include == TRUE) */

  // We can just keep buf.len as len and do the pad remaining bits in transmittion
  // txc_tx.buf.len = len;

  txc_tx.active_msg = TRUE;

  if (cmd_ptr->reach.tcb_ptr != NULL)
  {
    /* Save pointer to task to signal back and signal to use */
    txac.tcb_ptr = cmd_ptr->reach.tcb_ptr;
    txac.sigs = cmd_ptr->reach.sigs;
  }

} /* txc_reach_probe_prep */

#endif /* FEATURE_IS95B_PILOT_RPT */
#endif // FEATURE_IS2000_REL_A_CC


#if defined(FEATURE_IS95B_PILOT_RPT)
/*===========================================================================

FUNCTION TXC_ACC_PROBE_PREP

DESCRIPTION
  Process request to send a probe of Access Channel message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  txac and txc_tx buffers are set up for message transmission.

===========================================================================*/
void txc_acc_probe_prep
(
  txc_cmd_type *cmd_ptr
    /* Pointer to received command */
)
{
  word len;           /* Length of Access Channel message */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  len = cmd_ptr->acc.len;       /* Get Access Channel message length */

  /* Zero the byte in which the message body ends */
  txc_tx.buf.msg[(len >> 3) + 1] = 0; /* Pad end with 0's */

  /* -----------------------------------------------------------------------
  ** If the (len mod 8) is between 3 and 7 (i.e. the message body uses more
  ** than 2 bits of its last byte) then the CRC will begin in the byte after
  ** the one in which the message body ends.  If that is the case, we need
  ** to also zero the two bits preceeding the 30 bit CRC in that byte.
  ** ----------------------------------------------------------------------- */
  txc_tx.buf.msg[(len >> 3) + 2] = 0; /* Pad end with 0's */

  /* Copy the actual message */
  b_copy( cmd_ptr->acc.msg_ptr, 0,       /* Source */
          &txc_tx.buf.msg[1], 0, len );  /* Destination, leave room for len */

  /* Add in CRC and MSG_LENGTH fields */
  len += CAI_AC_CRC_SIZE + CAI_AC_LGTH_SIZE;

  if (len % 8)
  {
    /* Pad length out to byte boundary */
    len = (len & ~7) + 8;
  }

  txc_tx.buf.msg[0] = (byte) (len >> 3);  /* Save length in bytes */

  b_packd( crc_30_calc( txc_tx.buf.msg, len-30 ), /* Calculate CRC */
           txc_tx.buf.msg, len-30, 30 );          /* Add to message */

  /*
  ** Save length in bits to actually transmit. If we make this the size
  ** of the message capsule then we will transmit all the frames of the
  ** capsule and transmit zeroes when we run out of message.
  */
  txc_tx.buf.len = (3 + cmd_ptr->acc.max_cap_sz) * TXC_ACC;

  txc_tx.active_msg = TRUE;

  if (cmd_ptr->acc.tcb_ptr != NULL)
  {
    /* Save pointer to task to signal back and signal to use */
    txac.tcb_ptr = cmd_ptr->acc.tcb_ptr;
    txac.sigs = cmd_ptr->acc.sigs;
  }
} /* txc_acc_probe_prep */
#endif /* FEATURE_IS95B_PILOT_RPT */


#ifdef FEATURE_IS95B_ACCESS_PROBE_HO
/*===========================================================================

FUNCTION TXC_ACC_PROBE_SUSPEND

DESCRIPTION
  Suspends the transmission of next Access probe.
  If the TA timer is running, sets the

DEPENDENCIES
  When this function is called, it is assumed that the TXC is in
  the middle of sending Access Probes. If it is not, then the function
  does not do anything.(We do not fatal error because this condition
  is possible in a Race Condition, when TXC is done with Max. Access Probes
  and MC meanwhile sends it SUSPEND).

RETURN VALUE
  None

SIDE EFFECTS
  If the TA timer is running, some variables are set so that to send
  ACC_PROBE_HO_OK sig for MC when the TA timer expires.
  If the TA timer is not running, ACC_PROBE_HO_OK sig is set.
===========================================================================*/
LOCAL void txc_acc_probe_suspend(txc_cmd_type * cmd_ptr)
{
  /* Check the state of TXC */
  if (txc_state != TXC_ACC_ATT_S)
  {
    /* This Suspend Command has come due to a Race Condition */
    /* Ignore it.                                            */
    MSG_MED("Ignrd SUSP Cmd in Non-Acc St",0,0,0);
  }
  else
  {
    switch (txac.state)
    {
      case ACC_SETUP1_S:
        /* Suspend command issued even before TXC could start the Acc  */
        /* Msg transmission. Suspend probing without waiting for any   */
        /* timer etc.                                                  */
        MSG_MED("Susp in Msg Setup",0,0,0);
        txac.state = ACC_SUSPEND_S;
        break;
      case ACC_BACK1_S:
      case ACC_BACK2_S:
        /* RT Timer is running. Suspend the transmission of next probe */
        /* Also command MC to initiate the Access Probe Handoff        */
        MSG_MED("Susp when RT runs",0,0,0);
        txac.state = ACC_SUSPEND_S;
        break;
      case ACC_PRE_S:
      case ACC_MSG_S:
        /* Transmitting the Probe. Wait till the transmission of the probe */
        /* is over. Signal MC to initiate Handoff when TA timer expires.   */

        /* Note the intentional "missing" break. Processing in this case   */
        /* is exactly as if TA timer is running.                           */
      case ACC_ENDPROBE_S:
        /* TA Timer is running. Wait till it expires */
        MSG_MED("Susp when TA Timer Runs",0,0,0);
        txc_suspend_after_ta = TRUE;
        break;
      case ACC_SETUP2_S:
      case ACC_MSG_DONE_S:
        /* This is supposed to be a transition state only!! */
        MSG_MED("Susp in Msg Setup",0,0,0);
        txac.state = ACC_SUSPEND_S;
        break;
      case ACC_SUSPEND_S:
        ERR("MC sent Susp in TXC Susp state",0,0,0);
        break;
    }
  }

  /* If TXC has already gone into Suspended state, signal MC */
  /* to initiate the Access Probe Handoff                    */
  if (txac.state == ACC_SUSPEND_S)
  {
    (void) rex_set_sigs(&mc_tcb,MCC_TXC_APHO_OK_SIG);
  }
  /* Save pointer to task to signal back and signal to use */
  txac.tcb_ptr = cmd_ptr->acc.tcb_ptr;
  txac.sigs = cmd_ptr->acc.sigs;
} /* txc_acc_probe_suspend */
#endif /* FEATURE_IS95B_ACCESS_PROBE_HO */


/*===========================================================================

FUNCTION TXC_ACC_PROBE_RESUME

DESCRIPTION
  ReSumes the transmission of next Access probe.

DEPENDENCIES
  When this function is called, it is assumed that the TXC is in
  the Access Probe Suspend State. If it is not, then the function
  performs a FATAL ERROR.

RETURN VALUE
  None

SIDE EFFECTS
  The Sequence Number etc is set as per the Access Probe Handoff Requirements.
===========================================================================*/

void txc_acc_probe_resume(void)
{
  MSG_MED("Resume Probing",0,0,0);

#ifdef FEATURE_IS2000_REL_A_CC
  if ( txc_reach_access == TRUE )
  {
    /* Compute REACH long code mask in txc_reach_access_att */

    txac.state = ACC_SETUP2_S; 
    txac.countdown = 0;       
  }
  else
#endif // FEATURE_IS2000_REL_A_CC
  {
    txc_compute_access_chan();
#ifndef FEATURE_RL_JUMP_ENHANCEMENT
    /* If feature is defined, mask is programmed inside txc_compute_access_chan().
    */
    enc_user_pn_mask(txc_tx.buf.mask);
#endif /* !FEATURE_RL_JUMP_ENHANCEMENT */

    txac.state = ACC_BACK2_S; /* Fake that RT timer has been running */
    txac.countdown = 0;       /* And, it has expired just now!!      */
  }
  
  /* Also set the Probe numbers etc to restart from first probe */
  /* of the Access Probe Sequence.                              */
  txac.probe_count = 0;       /* Probe no. in the current Probe Sequence  */
  txac.nstep = txac.num_step; /* No. of Probes left in the Probe Sequence */
  /* As per standard, We do not need to touch the Sequence Number */

  txac.power = txac.init_pwr; /* Transmit Power of the first probe in Sequence */

  /* We do not need to set the value of txac.first_frame. It must be */
  /* Properly set at this stage.                                     */

  txac.is95a_pwr = txac.power;
}


#ifdef FEATURE_IS2000_REL_A_CC
// txc_reach_on
// begin txc_reach_access_att
/*===========================================================================

FUNCTION TXC_REACH_ACCESS_ATT

DESCRIPTION
  State where access channel transmission is transmitted.

DEPENDENCIES
  None

RETURN VALUE
  New state

SIDE EFFECTS
  None

===========================================================================*/

txc_state_type txc_reach_access_att
(
  txc_state_type state
    /* txc_state - TXC_ACCESS_S */
)
{
  txc_cmd_type  *cmd_ptr;         /* Command from queue */
  rex_sigs_type sigs;             /* Task signals */
  word          dummy;            /* Temp var */
  static word   left;             /* Count bits left */

  boolean psist_test_passed = FALSE;

  byte          pilot_ecio;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  txac.state = ACC_SETUP1_S;      /* Set up for first probe */
  txac.probe_count = 0;           /* Initialize to first probe sequence */
  txac.attempt_probe_count = 0;   /* Access attempt probe number */
  
  enc_is95c_enable( TRUE );
  enc_pch_enable( TRUE );
  enc_sch_enable( TRUE );
  enc_fch_enable( FALSE );
  enc_dcch_enable( FALSE );

  TXC_SET_PCH_GAIN( 0 );
  TXC_SET_SCH_GAIN( 0 );
  TXC_SET_FCH_GAIN( 0 );
  TXC_SET_DCCH_GAIN( 0 );

  enc_sch_rc ( ENC_SCH_RC_3);

  txc_reach_first_frame_input_data = FALSE;
  txc_reach_program_preamble_prepare = FALSE;
  txc_reach_program_data_prepare = FALSE;

  txc_reach_preamble_count = 0;
  txc_reach_data_count = 0;
  txc_reach_intr_cnt = 0;


  txc_reach_preamble_frame_offset = 0;
  txc_reach_data_frame_offset = 0;
  txc_reach_frame_offset = 0;

  // Number of 20ms duration frame count.
  txc_reach_20ms_frame_cnt = 0;

  // Number of frame count for 5ms, 10ms, 20ms frame size.
  txc_reach_frame_cnt = 0;



  /* -----------------------------------------------------------------
  ** NOTE: The interrupt handler takes care of feeding the actual data
  **       to the encoder.
  ** ----------------------------------------------------------------- */

  (void) rex_clr_sigs( &tx_tcb, TXC_INT_SIG );

  do
  {
    /* Wait for command on the queue or a transmit frame interrupt */
    sigs = txc_wait( TXC_CMD_Q_SIG | TXC_INT_SIG );

    if ((sigs & TXC_CMD_Q_SIG) != 0)
    {
      (void) rex_clr_sigs( &tx_tcb, TXC_CMD_Q_SIG );

      if ((cmd_ptr = (txc_cmd_type*) q_get( &txc_cmd_q )) != NULL)
      {
        /* Process the received command */
        state = txc_access_cmd( cmd_ptr, TXC_ACC_ATT_S );
      }
    } /* end if ((sigs & TXC_CMD_Q_SIG) != 0) */

    /* Handle signal from interrupt handler */
    else if ((sigs & TXC_INT_SIG) != 0)
    {
      (void) rex_clr_sigs( &tx_tcb, TXC_INT_SIG );

      /* Assume no transmit */
      txac.frame.dotx = FALSE;

      if (txac.state == ACC_ENDPROBE_S)
      {
        MSG_HIGH( "ACC_ATT: END PROBE", 0, 0, 0 );

        /* Turn off the pilot and REACH data signals.  Set gain adjustments to zero 
         * This is done only after the last frame has been transmitted. */
        if (( txac.countdown == txac.acc_tmo - 1 )
              || ( txac.countdown == 0 ))
        {
          enc_pch_enable(FALSE);
          enc_sch_enable(FALSE);
          rpc_set_tx_rate_adj (0);
          srch_mux_set_tx_adj(0x7F);
        }


        /* -------------------------------------------------
        ** Waiting for the Ack Response Timer (TA) to expire
        ** ------------------------------------------------- */
        if (txac.countdown == 0)
        {
          txac.state = ACC_MSG_DONE_S;


        MSG_HIGH( "ACC_ATT: MSG DONE", 0, 0, 0 );


#ifdef FEATURE_IS95B_ACCESS_PROBE_HO
          if (txc_suspend_after_ta)
          {
            txc_suspend_after_ta = FALSE;
            txac.state = ACC_SUSPEND_S;
            /* Signal MC to initiate Access Probe Handoff */
            (void) rex_set_sigs(&mc_tcb,MCC_TXC_APHO_OK_SIG);
          }
#endif /* FEATURE_IS95B_ACCESS_PROBE_HO */
        }
        else
        {
          txac.countdown--;
        }
      } /* end if (txac.state == ACC_ENDPROBE_S) */

      if (txac.state == ACC_MSG_DONE_S)
      {
      /* Check if the Probes are suspended due to T72M timer */
      if (!txac.stop_probes)
      {
        if (txac.nstep--)
        {
          /* ------------------------------------------------------
          ** More probes remaining in current access probe sequence
          ** ------------------------------------------------------ */

          txac.power = txac.is95a_pwr;

          /* Check if next power step will be greater than MAX */
          if ((txac.power >= 0x80) && ((txac.power - txac.pstep) < 0x80))
          {
            /* Limit power */
            txac.power = TXC_PWR_MAX;
          }
          else
          {
            /* Increase power */
            txac.power -= txac.pstep;
          }

          txac.is95a_pwr = txac.power;


          /* Redo the preamble */
          txac.state = ACC_SETUP2_S;


        MSG_HIGH( "ACC_ATT: SETUP2", 0, 0, 0 );


        }

        else
        {
          /* ------------------------------------
          ** Have completed access probe sequence
          ** ------------------------------------ */

          if (++txac.seq_num >= txac.max_seq)
          {
            /* ---------------------------------------------------
            ** Maximum access probe sequence have been transmitted
            ** in this access attempt
            ** --------------------------------------------------- */
            state = TXC_ACC_IDLE_S;
            MSG_HIGH( "ACC_ATT: probes done; back to idle", 0, 0, 0 );

            /* Set flag to indicate no msg transmission in progress */
            txc_tx.active_msg = FALSE;
          }

          else
          {
            /* -----------------------------------
            ** Start another access probe sequence
            ** ----------------------------------- */
            txac.state = ACC_SETUP1_S;

            MSG_HIGH( "ACC_ATT: SET UP 1", 0, 0, 0 );


            txac.first_frame = FALSE;     /* Not first frame */
            txac.probe_count = 0;
            txac.power = txac.init_pwr;
            txac.nstep = txac.num_step;

            /* initialize the is95a power accumulation */
            txac.is95a_pwr = txac.init_pwr;
          }
        }
      }
      else
      {
        /* Temporarily disable the transmitter and stop sending
        ** any probes until further enabled */
        txac.state = ACC_SUSPEND_S;
      }
      } /* end if (txac.state == ACC_MSG_DONE_S) */

      if (txac.state == ACC_SUSPEND_S)
      {
        /*
        ** There is no need to perform any processing in this
        ** state at this time. Below is the comment from the IS-95B
        ** development branch
        */
        /* -------------------------------------------------------
        ** The TXC is in Suspended state, Transmitter is disabled
        ** and the Probe transmission is suspended. The TXC will
        ** resume transmission of the probes once it is re-enabled
        ** ----------------------------------------------------- */
      }

      if (txac.state == ACC_SETUP1_S || txac.state == ACC_SETUP2_S)
      {
        // Report event
        mclog_report_event_msg_txed(CAI_EACH, txc_tx.buf.msg);

        ts_get( txc_reach_current_frame_number );

        (void) qw_div_by_power_of_2( txc_reach_current_frame_number,
                                     txc_reach_current_frame_number, 16 );

        /*
        ** At this point, txc_reach_current_frame_number is a PCG
        ** count.  Since the interrupt occurs using reverse link timing
        ** and the ts_get() uses forward link timing, the ts_get()
        ** could return a value earlier than the bin for which the interrupt
        ** occurred.  When this happens, the frame number computed here will
        ** be incorrect, leading to an incorrect long code mask, and the probe
        ** not being seen by the BS.
        **
        ** Fix for this is to add one to the PCG count here.  The last four
        ** bits are truncated below by the >>=4 and *=16.  So, adding one will
        ** not cause a problem.  In the case where the ts_get() time is read
        ** at a time earlier than the interrupt occurred, this will correct
        ** for the back to the future constant.
        */
        qw_inc( txc_reach_current_frame_number, 1L);

        qw_dec( txc_reach_current_frame_number, (dword) txc_reach_frame_offset );

        (void) qw_div_by_power_of_2( txc_reach_current_frame_number,
                                     txc_reach_current_frame_number, 4 );

        // current_frame_pcg = current_frame_number * 16 + frame_offset;

        qw_mul( txc_reach_current_frame_pcg, txc_reach_current_frame_number, 16L );

        qw_inc( txc_reach_current_frame_pcg, (dword) txc_reach_frame_offset );

        MSG_HIGH( "curr pcg = %x, lo= %x", 
                  qw_hi(txc_reach_current_frame_pcg), qw_lo(txc_reach_current_frame_pcg), 0 );


        // next_frame_pcg = current_frame_pcg + 16;

        qw_equ( txc_reach_next_frame_pcg, txc_reach_current_frame_pcg );

        qw_inc( txc_reach_next_frame_pcg, 16L );

        MSG_HIGH( "next pcg = %x, lo= %x", 
                  qw_hi(txc_reach_next_frame_pcg), qw_lo(txc_reach_next_frame_pcg), 0 );


        // bkoff_frame_pcg = next_frame_pcg + (bkoff + rp) * each_slot

        dummy=0;

        if (txac.state == ACC_SETUP2_S)
        {
          /* ----------------------------------------------------------------
          ** If more than one Access Channel is supported by the Base Station
          ** ---------------------------------------------------------------- */
          if (txac.acc_chan > 1)
          {
            /* -------------------------------------------------------------
            ** Compute probe backoff delay, RT, from 0 to PROBE_BKOFF slots.
            **-------------------------------------------------------------- */
            dummy = ran_dist( ran_next(), 0, txac.probe_bkoff+1 );
          }
          else
          {
            /* ----------------------------------------------------------
            ** Compute probe backoff delay, RT, from 0 to 1 + PROBE_BKOFF
            ** slots.
            **----------------------------------------------------------- */
            dummy = ran_dist( ran_next(), 0, txac.probe_bkoff+2 );
          }
        } /*  end if (txac.state == ACC_SETUP2_S) */

        /* ----------------------------------------------------------
        ** Before transmitting each access probe sequence other than
        ** the first access probe sequence, the mobile station shall
        ** generate a random number, RS, from 0 to (BKOFFs + 1).  The
        ** mobile station shall delay the transmission of the access
        ** probe sequence for RS slots.
        ** ---------------------------------------------------------- */
        if ((txac.state == ACC_SETUP1_S) && (txac.seq_num != 0))
        {
          dummy = ran_dist( ran_next(), 0, txac.bkoff+2 );
        }

        // slot_length is 6 bits, and dummy is less than 8. So the product is less than 16
        txc_reach_countdown = dummy * txc_reach_slot_length; //lint !e734: Loss of precision (31 to 16 bits)


        // Persistence Test
        if ((txac.req) && (txac.state == ACC_SETUP1_S))
        {
          int num_psist_failures = 0;

          psist_test_passed = FALSE;

          do {
            
            dword ran;
            ran = ran_next();
            if (ran < txac.p)
            {
              psist_test_passed = TRUE;
            }
            else
            {
              ++num_psist_failures;
              ++txc_reach_psist_delay;
            }
          } while ( (psist_test_passed==FALSE) && (num_psist_failures<96) );

          // If none of the persistence tests pass, send event
          if (!psist_test_passed) 
          {
            event_report(EVENT_ACCESS_DENIED);
          }

          // Following product is 7bits * 6bits <= 13bits.
          txc_reach_countdown += num_psist_failures * txc_reach_slot_length; //lint !e734: Loss of precision (31 to 16 bits)
        }

        qw_equ( txc_reach_bkoff_frame_pcg, txc_reach_next_frame_pcg );

        qw_inc( txc_reach_bkoff_frame_pcg, (dword) txc_reach_countdown );


        txc_reach_count = qw_div( txc_reach_frame_pcg, 
                           txc_reach_bkoff_frame_pcg, txc_reach_slot_length );

        if ( txc_reach_count != 0 )
        {
          txc_reach_count = txc_reach_slot_length - txc_reach_count;
        }

        // preamble_frame_pcg = bkoff_frame_pcg + count;

        qw_equ( txc_reach_preamble_frame_pcg, txc_reach_bkoff_frame_pcg );

        qw_inc( txc_reach_preamble_frame_pcg, (dword) txc_reach_count );

        MSG_HIGH( "preamble pcg = %x, lo= %x", 
                  qw_hi(txc_reach_preamble_frame_pcg), qw_lo(txc_reach_preamble_frame_pcg), 0 );



        qw_equ( txc_reach_data_frame_pcg, txc_reach_preamble_frame_pcg );

        qw_inc( txc_reach_data_frame_pcg, (dword) txc_reach_preamble_length );


        txc_reach_preamble_frame_offset = 
        qw_div_by_power_of_2( txc_reach_frame_pcg,
                              txc_reach_preamble_frame_pcg, 4 );


        txc_reach_data_frame_offset = 
        qw_div_by_power_of_2( txc_reach_frame_pcg,
                              txc_reach_data_frame_pcg, 4 );


        txc_reach_frame_offset = txc_reach_data_frame_offset;


        (void) qw_div( txc_reach_frame_pcg, 
                       txc_reach_preamble_frame_pcg, txc_reach_slot_length );

        txc_reach_slot_offset = 
        qw_div_by_power_of_2( txc_reach_frame_pcg, txc_reach_frame_pcg, 9 );




        MSG_HIGH( "slot = %d, hex = %x, data = %d", 
                  txc_reach_slot_offset, txc_reach_slot_offset, txc_reach_data_frame_offset );


        /* txc_compute_reach_chan( txc_reach_slot_offset ); */
        txc_reach_compute_access_chan();

#ifndef FEATURE_RL_JUMP_ENHANCEMENT
        /* If feature is defined, mask is programmed inside 
        ** txc_reach_compute_access_chann().
        */

        /* Load user PN mask immediately */
        enc_user_pn_mask( txc_tx.buf.mask );
#endif /* !FEATURE_RL_JUMP_ENHANCEMENT */

        enc_stmr_cmd( ENC_STMR_CMD_LC_MASK_IMMED_V );

        txc_reach_count_pcg = txc_reach_countdown + txc_reach_count;

        txc_reach_count_frame = txc_reach_count_pcg / 16;

        txc_reach_pcg_start = txc_reach_count_pcg % 16;

        txac.countdown = txc_reach_count_frame;

        txc_reach_preamble_count = txc_reach_pcg_start / 4;

        txc_reach_data_count = txc_reach_preamble_length / 4;



        ts_get( txc_reach_current_frame_number );

        (void) qw_div_by_power_of_2( txc_reach_current_frame_number,
                                     txc_reach_current_frame_number, 16 );

        MSG_HIGH( "curr time = %x, lo= %x", 
                  qw_hi(txc_reach_current_frame_number), qw_lo(txc_reach_current_frame_number), 0 );


        if (txac.state == ACC_SETUP1_S)
        {
          txac.state = ACC_BACK1_S;

          MSG_HIGH( "ACC_ATT: SETUP1 BACK1", 0, 0, 0 );

        }
        else
        {
          txac.state = ACC_BACK2_S;

          MSG_HIGH( "ACC_ATT: SETUP2 BACK2", 0, 0, 0 );

        }
      } /* end if (txac.state == ACC_SETUP1_S || ACC_SETUP2_S) */

      if (txac.state == ACC_BACK1_S || txac.state == ACC_BACK2_S)
      {
        if ( (txac.req) && (txac.state == ACC_BACK1_S) && (!psist_test_passed) && (txac.countdown == 0) )
        {
          // The persistence test failed for all the attempts.
          // Retry in the setup state again.
          txac.state = ACC_SETUP1_S;
        }
        else if (txac.countdown == 0)
        {
          /* -------------------------------------------------------------
          ** the interference correction adjustment for the transmit power
          **
          ** note: srch_get_pilot_ecio return pilot_ecio in 1/2db
          ** ------------------------------------------------------------- */
          pilot_ecio = srch_mux_get_pilot_ecio();
          /*lint -e732 */
          // txac.int_corr = MIN( MAX(-14+pilot_ecio,0), 14 );
          /*lint +e732 */

          txac.int_corr = MIN( MAX( (pilot_ecio - txc_reach_ic_thresh) ,0), txc_reach_ic_max );


          /* Check if add interference correction power will be greater than MAX */
          if ((txac.is95a_pwr >= 0x80) && ((txac.is95a_pwr - txac.int_corr) < 0x80))
          {
             /* Limit power */
             txac.power = TXC_PWR_MAX;
          }
          else
          {
             /* Increase power */
             txac.power = txac.is95a_pwr - txac.int_corr;
          }

          MSG_MED("Seq=%d prb=%d ecio=%d",txac.seq_num+1,txac.probe_count+1,pilot_ecio/2);
          MSG_MED("Tx adj=%d 95A=%d i-corr=%d", (-((signed char)txac.power))/2,(-((signed char)txac.is95a_pwr))/2,txac.int_corr/2);

          /* Begin transmitting Access Channel preamble */
          txac.state = ACC_PRE_S;

          txc_reach_first_frame_input_data = FALSE;

          txac.frame.dotx = TRUE;

          /* Make sure power amp is on */
          enc_tx_enable( TRUE );

          /* Set transmit power */
          srch_mux_set_tx_adj( (byte) txac.power );
          txac.last_pwr = txac.power;    /* Save last power used for traffic */

          txc_tx.buf.pos   = 0;         /* Start at msg buffer position 0 */
          txac.first_frame = TRUE;

          // Input (first frame) data at interrupt during ACC_PRE_S
          // txc_tx.buf.pos needs to add TXC_ACC here

          // Number of 20ms duration frame count.
          txc_reach_20ms_frame_cnt = 1;

          /* Calculate how much of the message is left to send */
          left = txc_tx.buf.len - txc_tx.buf.pos;

          if (txc_reach_si_include == TRUE)
          {
            // Number of frame count for 5ms, 10ms, 20ms frame size.
            txc_reach_frame_cnt = txc_tx.buf.len/(txc_reach_frame_size - 2);

            if ( txc_tx.buf.len % (txc_reach_frame_size - 2) )
            {
              txc_reach_frame_cnt++;
            }

            if (txc_reach_frame_length == ENC_SCH_FRAME_SIZE_20_V)
            {
              if (left >= (txc_reach_frame_size - 2))   /* Lots of message left */
              {
                /* Copy SI bits of frame buffer with zeroes */
                b_copy( zeros, 0, txtc.supp_frame.data, 0, 2 );

                /* Copy full frame of msg data from msg buffer to frame buffer */
                b_copy( txc_tx.buf.msg, txc_tx.buf.pos,
                        txtc.supp_frame.data, 2, (txc_reach_frame_size - 2) );
              }
              else
              {
                /* Copy SI bits of frame buffer with zeroes */
                b_copy( zeros, 0, txtc.supp_frame.data, 0, 2 );

                /* Copy remaining bits of msg data from msg buffer to frame buf */
                b_copy( txc_tx.buf.msg, txc_tx.buf.pos,
                        txtc.supp_frame.data, 2, left );

                /* Pad remaining bits of frame buffer with zeroes */
                b_copy( zeros, 0, txtc.supp_frame.data, (left + 2), 
                                 (txc_reach_frame_size - left - 2) );
              }

              /* Advance message buffer index */
              txc_tx.buf.pos += (txc_reach_frame_size - 2);
            }
            else /* if (txc_reach_frame_length != ENC_SCH_FRAME_SIZE_20_V)*/
            {
              // 10ms frame, build frame 1, bin 0, bin 2
              //  5ms frame, build frame 1, bin 0, bin 1, bin 2, bin 3

              /* Lots of message left, build frame 1, bin 0 */
              if (left >= (txc_reach_frame_size - 2))   
              { 
                /* Copy SI bits of frame buffer with zeroes */
                b_copy( zeros, 0, txc_reach_frame_1_bin_0, 0, 2 );

                /* Copy full frame of msg data from msg buffer to frame buffer */
                b_copy( txc_tx.buf.msg, txc_tx.buf.pos,
                        txc_reach_frame_1_bin_0, 2, (txc_reach_frame_size - 2) );

                /* Advance message buffer index */
                txc_tx.buf.pos += (txc_reach_frame_size - 2);

                /* Calculate how much of the message is left to send 
                   after building frame 1, bin 0 buffer. */
                left = txc_tx.buf.len - txc_tx.buf.pos;

                //5ms
                /* 5ms frame builds data for bin1, bin2, bin3 */
                if ( txc_reach_frame_length == ENC_SCH_FRAME_SIZE_5_V )
                {  

                /* Lots of message left, build frame 1, bin 1 */
                if (left >= (txc_reach_frame_size - 2))   
                {
                  /* Copy SI bits of frame buffer with zeroes */
                  b_copy( zeros, 0, txc_reach_frame_1_bin_1, 0, 2 );

                  /* Copy full frame of msg data from msg buffer to 
                     frame 1, bin 1 buffer. */
                  b_copy( txc_tx.buf.msg, txc_tx.buf.pos,
                          txc_reach_frame_1_bin_1, 2, (txc_reach_frame_size - 2) );

                  /* Advance message buffer index after frame 1, bin 1 */
                  txc_tx.buf.pos += (txc_reach_frame_size - 2);

                  /* Calculate how much of the message is left to send 
                     after building frame 1, bin 1 buffer. */
                  left = txc_tx.buf.len - txc_tx.buf.pos;

                  /* Lots of message left, build frame 1, bin 2 */
                  if (left >= (txc_reach_frame_size - 2))   
                  {
                    /* Copy SI bits of frame buffer with zeroes */
                    b_copy( zeros, 0, txc_reach_frame_1_bin_2, 0, 2 );

                    /* Copy full frame of msg data from msg buffer to 
                       frame 1, bin 2 buffer. */
                    b_copy( txc_tx.buf.msg, txc_tx.buf.pos,
                            txc_reach_frame_1_bin_2, 2, (txc_reach_frame_size - 2) );

                    /* Advance message buffer index after frame 1, bin 2*/
                    txc_tx.buf.pos += (txc_reach_frame_size - 2);

                    /* Calculate how much of the message is left to send 
                       after building frame 1, bin 2 buffer. */
                    left = txc_tx.buf.len - txc_tx.buf.pos;

                    /* Lots of message left, build frame 1, bin 3 */
                    if (left >= (txc_reach_frame_size - 2))   
                    {
                      /* Copy SI bits of frame buffer with zeroes */
                      b_copy( zeros, 0, txc_reach_frame_1_bin_3, 0, 2 );

                      /* Copy full frame of msg data from msg buffer to 
                         frame 1, bin 3 buffer. */
                      b_copy( txc_tx.buf.msg, txc_tx.buf.pos,
                              txc_reach_frame_1_bin_3, 2, (txc_reach_frame_size - 2) );

                      /* Advance message buffer index after frame 1, bin 3 */
                      txc_tx.buf.pos += (txc_reach_frame_size - 2);

                    }
                    else /* if (one frame of message left, build frame 1, bin 3) */
                    {
                      /* Copy SI bits of frame buffer with zeroes */
                      b_copy( zeros, 0, txc_reach_frame_1_bin_3, 0, 2 );

                      /* Copy remaining bits of msg data from msg buffer to frame buf */
                      b_copy( txc_tx.buf.msg, txc_tx.buf.pos,
                              txc_reach_frame_1_bin_3, 2, left );

                      /* Pad remaining bits of frame buffer with zeroes */
                      b_copy( zeros, 0, txc_reach_frame_1_bin_3, (left + 2), 
                                       (txc_reach_frame_size - left - 2) );

                      /* Advance message buffer index */
                      txc_tx.buf.pos += (txc_reach_frame_size - 2);
                    } /* endif (Lots of message left, build frame 1, bin 3) */

                  }
                  else /* if (one frame of message left, build frame 1, bin 2) */
                  {
                    /* Copy SI bits of frame buffer with zeroes */
                    b_copy( zeros, 0, txc_reach_frame_1_bin_2, 0, 2 );

                    /* Copy remaining bits of msg data from msg buffer to frame buf */
                    b_copy( txc_tx.buf.msg, txc_tx.buf.pos,
                            txc_reach_frame_1_bin_2, 2, left );

                    /* Pad remaining bits of frame buffer with zeroes */
                    b_copy( zeros, 0, txc_reach_frame_1_bin_2, (left + 2), 
                                     (txc_reach_frame_size - left - 2) );

                    /* Advance message buffer index after frame 1, bin 2 */
                    txc_tx.buf.pos += (txc_reach_frame_size - 2);
                  } /* endif (Lots of message left, build frame 1, bin 2) */

                }
                else /* if (one frame of message left, build frame 1, bin 1) */
                {
                  /* Copy SI bits of frame buffer with zeroes */
                  b_copy( zeros, 0, txc_reach_frame_1_bin_1, 0, 2 );

                  /* Copy remaining bits of msg data from msg buffer to frame buf */
                  b_copy( txc_tx.buf.msg, txc_tx.buf.pos,
                          txc_reach_frame_1_bin_1, 2, left );

                  /* Pad remaining bits of frame buffer with zeroes */
                  b_copy( zeros, 0, txc_reach_frame_1_bin_1, (left + 2), 
                                   (txc_reach_frame_size - left - 2) );

                  /* Advance message buffer index after frame 1, bin 1 */
                  txc_tx.buf.pos += (txc_reach_frame_size - 2);

                } /* endif (Lots of message left, build frame 1, bin 1) */

                } //5ms
                else /* if ( txc_reach_frame_length == ENC_SCH_FRAME_SIZE_10_V ) */
                { //10ms builds data for bin2

                  /* Lots of message left, build frame 1, bin 2 */
                  if (left >= (txc_reach_frame_size - 2))   
                  {
                    /* Copy SI bits of frame buffer with zeroes */
                    b_copy( zeros, 0, txc_reach_frame_1_bin_2, 0, 2 );

                    /* Copy full frame of msg data from msg buffer to 
                       frame 1, bin 2 buffer. */
                    b_copy( txc_tx.buf.msg, txc_tx.buf.pos,
                    txc_reach_frame_1_bin_2, 2, (txc_reach_frame_size - 2) );

                    /* Advance message buffer index after frame 1, bin 2*/
                    txc_tx.buf.pos += (txc_reach_frame_size - 2);

                  }
                  else /* if (one frame of message left, build frame 1, bin 2) */
                  {
                    /* Copy SI bits of frame buffer with zeroes */
                    b_copy( zeros, 0, txc_reach_frame_1_bin_2, 0, 2 );

                    /* Copy remaining bits of msg data from msg buffer to frame buf */
                    b_copy( txc_tx.buf.msg, txc_tx.buf.pos,
                            txc_reach_frame_1_bin_2, 2, left );

                    /* Pad remaining bits of frame buffer with zeroes */
                    b_copy( zeros, 0, txc_reach_frame_1_bin_2, (left + 2), 
                            (txc_reach_frame_size - left - 2) );

                    /* Advance message buffer index after frame 1, bin 2 */
                    txc_tx.buf.pos += (txc_reach_frame_size - 2);
                  } /* endif (Lots of message left, build frame 1, bin 2) */

                  //10ms
                } /* endif ( txc_reach_frame_length == ENC_SCH_FRAME_SIZE_5_V ) */

              } 
              else /* if (one frame of message left, build frame 1, bin 0) */
              {
                /* Copy SI bits of frame buffer with zeroes */
                b_copy( zeros, 0, txc_reach_frame_1_bin_0, 0, 2 );

                /* Copy remaining bits of msg data from msg buffer to frame buf */
                b_copy( txc_tx.buf.msg, txc_tx.buf.pos,
                        txc_reach_frame_1_bin_0, 2, left );

                /* Pad remaining bits of frame buffer with zeroes */
                b_copy( zeros, 0, txc_reach_frame_1_bin_0, (left + 2), 
                                 (txc_reach_frame_size - left - 2) );

                /* Advance message buffer index after frame 1, bin 0 */
                txc_tx.buf.pos += (txc_reach_frame_size - 2);

              } /* endif (lots of message left, build frame 1, bin 0) */

            } /* endif (txc_reach_frame_length == ENC_SCH_FRAME_SIZE_20_V)*/
          }
          else /* if (txc_reach_si_include == FALSE) */
          {

            if (left >= txc_reach_frame_size)   /* Lots of message left */
            {
              /* Copy full frame of msg data from msg buffer to frame buffer */
              b_copy( txc_tx.buf.msg, txc_tx.buf.pos,
                      txtc.supp_frame.data, 0, txc_reach_frame_size );
            }
            else
            {
              /* Copy remaining bits of msg data from msg buffer to frame buf */
              b_copy( txc_tx.buf.msg, txc_tx.buf.pos,
                      txtc.supp_frame.data, 0, left );

              /* Pad remaining bits of frame buffer with zeroes */
              b_copy( zeros, 0, txtc.supp_frame.data, left, 
                                (txc_reach_frame_size - left) );
            }

            /* Advance message buffer index */
            txc_tx.buf.pos += txc_reach_frame_size;

          } /* end if (txc_reach_si_include == TRUE) */
        }
        else /* Backoff timer has not expired */
        {
          txac.countdown--;
          //MSG_ERROR("Backoff Countdown %d", txac.countdown,0,0);
        }
      } /* end if (txac.state == ACC_BACK1_S || txac.state == ACC_BACK2_S) */

      if (txac.state == ACC_MSG_S) /* Message transmit substate */
      {
        /* REACH builds the first frame of data at ACC_PRE_S and updates
        ** txc_tx.buf.pos += (txc_reach_frame_size - 2) for SI included.
        ** If the complete message fits into the first frame of data, 
        ** then the buf.pos will go over the buf.len.  Therefore, we
        ** need to first check and make sure that we meet the condition of 
        ** (txc_tx.buf.pos < txc_tx.buf.len), then we build the second
        ** frame of data at ACC_MSG_S.  Otherwase, we need to set ENDPROBE_S
        ** when (txc_tx.buf.pos >= txc_tx.buf.len). 
        ** REACH uses CAI_REV_CSCH_MAX_MSG_BYTES (345 bytes) to allocate
        ** the message buffer.  345 bytes = 2760 bits = AC8 bits.
        ** Because word buf.len, word buf.pos are 16 bits unsigned data,
        ** therefore, they can hold the maximum REACH message length.
        ** Also, the maximum buf.pos will fit into 16 bits unsigned data,
        ** because of 2760 + 744 = 3504 = DB0 hex, where 2760 is the maximum
        ** number of message data, and 744 is the maximum information bits
        ** for 20 ms frame size, 38400 bps data rates.    
        */
        if (txc_tx.buf.pos < txc_tx.buf.len)
        {

          txac.frame.dotx = TRUE; /* Enable transmit */

          // txc_tx.buf.len = ( 3 + MAX_CAP_SZ ) * ACH_FRAME_SIZE
          // txc_tx.buf.len = ( 3 + MAX_CAP_SZ ) * TXC_ACC
          
          // Input (first frame) data at interrupt during ACC_PRE_S
          // Input (second frame to last frame) data at interrupt during ACC_MSG_S
          // txc_tx.buf.len should substract TXC_ACC during ACC_PRE_S data input
          // txc_tx.buf.len should take one less TXC_ACC here for calculation
          // If MAX_CAP_SZ = 0, then txc_tx.buf.len = 3 * TXC_ACC
          // After ACC_PRE_S data input, txc_tx.buf.len = 2 * TXC_ACC
          // During ACC_MSG_S here, we use 2 * TXC_ACC for txc_tx.buf.len

          /* Calculate how much of the message is left to send */
          left = txc_tx.buf.len - txc_tx.buf.pos;

          // Increment number of 20ms duration frame count.
          txc_reach_20ms_frame_cnt++;

          if (txc_reach_si_include == TRUE)
          {
            if (txc_reach_frame_length == ENC_SCH_FRAME_SIZE_20_V)
            {

              if (left >= (txc_reach_frame_size - 2))   /* Lots of message left */
              {
                /* Copy SI bits of frame buffer with zeroes */
                b_copy( zeros, 0, txtc.supp_frame.data, 0, 2 );

                /* Copy full frame of msg data from msg buffer to frame buffer */
                b_copy( txc_tx.buf.msg, txc_tx.buf.pos,
                        txtc.supp_frame.data, 2, (txc_reach_frame_size - 2) );
              }
              else
              {
                /* Copy SI bits of frame buffer with zeroes */
                b_copy( zeros, 0, txtc.supp_frame.data, 0, 2 );

                /* Copy remaining bits of msg data from msg buffer to frame buf */
                b_copy( txc_tx.buf.msg, txc_tx.buf.pos,
                        txtc.supp_frame.data, 2, left );

                /* Pad remaining bits of frame buffer with zeroes */
                b_copy( zeros, 0, txtc.supp_frame.data, (left + 2), 
                                 (txc_reach_frame_size - left - 2) );
              }

              /* Advance message buffer index */
              txc_tx.buf.pos += (txc_reach_frame_size - 2);

            }
            else /* if (txc_reach_frame_length != ENC_SCH_FRAME_SIZE_20_V)*/
            {
              // 10ms frame, build frame 1, bin 0, bin 2
              //  5ms frame, build frame 1, bin 0, bin 1, bin 2, bin 3

              /* Lots of message left, build frame 1, bin 0 */
              if (left >= (txc_reach_frame_size - 2))   
              {
                if (txc_reach_20ms_frame_cnt % 2)
                {
                  /* Copy SI bits of frame buffer with zeroes */
                  b_copy( zeros, 0, txc_reach_frame_1_bin_0, 0, 2 );

                  /* Copy full frame of msg data from msg buffer to frame buffer */
                  b_copy( txc_tx.buf.msg, txc_tx.buf.pos,
                          txc_reach_frame_1_bin_0, 2, (txc_reach_frame_size - 2) );
                }
                else /* Build frame 2 */
                {
                  /* Copy SI bits of frame buffer with zeroes */
                  b_copy( zeros, 0, txc_reach_frame_2_bin_0, 0, 2 );

                  /* Copy full frame of msg data from msg buffer to frame buffer */
                  b_copy( txc_tx.buf.msg, txc_tx.buf.pos,
                          txc_reach_frame_2_bin_0, 2, (txc_reach_frame_size - 2) );
                } 

                /* Advance message buffer index */
                txc_tx.buf.pos += (txc_reach_frame_size - 2);

                /* Calculate how much of the message is left to send 
                   after building frame 1, bin 0 buffer. */
                left = txc_tx.buf.len - txc_tx.buf.pos;

                //5ms
                /* 5ms frame builds data for bin1, bin2, bin3 */
                if ( txc_reach_frame_length == ENC_SCH_FRAME_SIZE_5_V )
                {  

                  /* Lots of message left, build frame 1, bin 1 */
                  if (left >= (txc_reach_frame_size - 2))   
                  {
                    if (txc_reach_20ms_frame_cnt % 2)
                    {
                      /* Copy SI bits of frame buffer with zeroes */
                      b_copy( zeros, 0, txc_reach_frame_1_bin_1, 0, 2 );

                      /* Copy full frame of msg data from msg buffer to 
                         frame 1, bin 1 buffer. */
                      b_copy( txc_tx.buf.msg, txc_tx.buf.pos,
                              txc_reach_frame_1_bin_1, 2, (txc_reach_frame_size - 2) );
                    }
                    else /* Build frame 2 */
                    {
                      /* Copy SI bits of frame buffer with zeroes */
                      b_copy( zeros, 0, txc_reach_frame_2_bin_1, 0, 2 );

                      /* Copy full frame of msg data from msg buffer to 
                         frame 1, bin 1 buffer. */
                      b_copy( txc_tx.buf.msg, txc_tx.buf.pos,
                              txc_reach_frame_2_bin_1, 2, (txc_reach_frame_size - 2) );
                    }

                    /* Advance message buffer index after frame 1, bin 1 */
                    txc_tx.buf.pos += (txc_reach_frame_size - 2);

                    /* Calculate how much of the message is left to send 
                       after building frame 1, bin 1 buffer. */
                    left = txc_tx.buf.len - txc_tx.buf.pos;

                    /* Lots of message left, build frame 1, bin 2 */
                    if (left >= (txc_reach_frame_size - 2))   
                    {
                      if (txc_reach_20ms_frame_cnt % 2)
                      {
                        /* Copy SI bits of frame buffer with zeroes */
                        b_copy( zeros, 0, txc_reach_frame_1_bin_2, 0, 2 );

                        /* Copy full frame of msg data from msg buffer to 
                           frame 1, bin 2 buffer. */
                        b_copy( txc_tx.buf.msg, txc_tx.buf.pos,
                                txc_reach_frame_1_bin_2, 2, (txc_reach_frame_size - 2) );
                      }
                      else /* Build frame 2 */
                      {
                        /* Copy SI bits of frame buffer with zeroes */
                        b_copy( zeros, 0, txc_reach_frame_2_bin_2, 0, 2 );

                        /* Copy full frame of msg data from msg buffer to 
                           frame 1, bin 2 buffer. */
                        b_copy( txc_tx.buf.msg, txc_tx.buf.pos,
                                txc_reach_frame_2_bin_2, 2, (txc_reach_frame_size - 2) );
                      }

                      /* Advance message buffer index after frame 1, bin 2*/
                      txc_tx.buf.pos += (txc_reach_frame_size - 2);

                      /* Calculate how much of the message is left to send 
                         after building frame 1, bin 2 buffer. */
                      left = txc_tx.buf.len - txc_tx.buf.pos;

                      /* Lots of message left, build frame 1, bin 3 */
                      if (left >= (txc_reach_frame_size - 2))   
                      {
                        if (txc_reach_20ms_frame_cnt % 2)
                        {
                          /* Copy SI bits of frame buffer with zeroes */
                          b_copy( zeros, 0, txc_reach_frame_1_bin_3, 0, 2 );

                          /* Copy full frame of msg data from msg buffer to 
                             frame 1, bin 3 buffer. */
                          b_copy( txc_tx.buf.msg, txc_tx.buf.pos,
                                  txc_reach_frame_1_bin_3, 2, (txc_reach_frame_size - 2) );
                        }
                        else /* Build frame 2 */
                        {
                          /* Copy SI bits of frame buffer with zeroes */
                          b_copy( zeros, 0, txc_reach_frame_2_bin_3, 0, 2 );

                          /* Copy full frame of msg data from msg buffer to 
                             frame 1, bin 3 buffer. */
                          b_copy( txc_tx.buf.msg, txc_tx.buf.pos,
                                  txc_reach_frame_2_bin_3, 2, (txc_reach_frame_size - 2) );
                        }

                        /* Advance message buffer index after frame 1, bin 3 */
                        txc_tx.buf.pos += (txc_reach_frame_size - 2);

                      }
                      else /* if (one frame of message left, build frame 1, bin 3) */
                      {
                        if (txc_reach_20ms_frame_cnt % 2)
                        {
                          /* Copy SI bits of frame buffer with zeroes */
                          b_copy( zeros, 0, txc_reach_frame_1_bin_3, 0, 2 );

                          /* Copy remaining bits of msg data from msg buffer to frame buf */
                          b_copy( txc_tx.buf.msg, txc_tx.buf.pos,
                                  txc_reach_frame_1_bin_3, 2, left );

                          /* Pad remaining bits of frame buffer with zeroes */
                          b_copy( zeros, 0, txc_reach_frame_1_bin_3, (left + 2), 
                                           (txc_reach_frame_size - left - 2) );
                        }
                        else /* Build frame 2 */
                        {
                          /* Copy SI bits of frame buffer with zeroes */
                          b_copy( zeros, 0, txc_reach_frame_2_bin_3, 0, 2 );

                          /* Copy remaining bits of msg data from msg buffer to frame buf */
                          b_copy( txc_tx.buf.msg, txc_tx.buf.pos,
                                  txc_reach_frame_2_bin_3, 2, left );

                          /* Pad remaining bits of frame buffer with zeroes */
                          b_copy( zeros, 0, txc_reach_frame_2_bin_3, (left + 2), 
                                           (txc_reach_frame_size - left - 2) );
                        }

                        /* Advance message buffer index */
                        txc_tx.buf.pos += (txc_reach_frame_size - 2);
                      } /* endif (Lots of message left, build frame 1, bin 3) */

                    }
                    else /* if (one frame of message left, build frame 1, bin 2) */
                    {
                      if (txc_reach_20ms_frame_cnt % 2)
                      {
                        /* Copy SI bits of frame buffer with zeroes */
                        b_copy( zeros, 0, txc_reach_frame_1_bin_2, 0, 2 );

                        /* Copy remaining bits of msg data from msg buffer to frame buf */
                        b_copy( txc_tx.buf.msg, txc_tx.buf.pos,
                                txc_reach_frame_1_bin_2, 2, left );

                        /* Pad remaining bits of frame buffer with zeroes */
                        b_copy( zeros, 0, txc_reach_frame_1_bin_2, (left + 2), 
                                         (txc_reach_frame_size - left - 2) );
                      }
                      else /* Build frame 2 */
                      {
                        /* Copy SI bits of frame buffer with zeroes */
                        b_copy( zeros, 0, txc_reach_frame_2_bin_2, 0, 2 );

                        /* Copy remaining bits of msg data from msg buffer to frame buf */
                        b_copy( txc_tx.buf.msg, txc_tx.buf.pos,
                              txc_reach_frame_2_bin_2, 2, left );

                        /* Pad remaining bits of frame buffer with zeroes */
                        b_copy( zeros, 0, txc_reach_frame_2_bin_2, (left + 2), 
                                         (txc_reach_frame_size - left - 2) );
                      }

                      /* Advance message buffer index after frame 1, bin 2 */
                      txc_tx.buf.pos += (txc_reach_frame_size - 2);
                    } /* endif (Lots of message left, build frame 1, bin 2) */

                  }
                  else /* if (one frame of message left, build frame 1, bin 1) */
                  {
                    if (txc_reach_20ms_frame_cnt % 2)
                    {
                      /* Copy SI bits of frame buffer with zeroes */
                      b_copy( zeros, 0, txc_reach_frame_1_bin_1, 0, 2 );

                      /* Copy remaining bits of msg data from msg buffer to frame buf */
                      b_copy( txc_tx.buf.msg, txc_tx.buf.pos,
                              txc_reach_frame_1_bin_1, 2, left );

                      /* Pad remaining bits of frame buffer with zeroes */
                      b_copy( zeros, 0, txc_reach_frame_1_bin_1, (left + 2), 
                                       (txc_reach_frame_size - left - 2) );
                    }
                    else /* Build frame 2 */
                    {
                      /* Copy SI bits of frame buffer with zeroes */
                      b_copy( zeros, 0, txc_reach_frame_2_bin_1, 0, 2 );

                      /* Copy remaining bits of msg data from msg buffer to frame buf */
                      b_copy( txc_tx.buf.msg, txc_tx.buf.pos,
                              txc_reach_frame_2_bin_1, 2, left );

                      /* Pad remaining bits of frame buffer with zeroes */
                      b_copy( zeros, 0, txc_reach_frame_2_bin_1, (left + 2), 
                                       (txc_reach_frame_size - left - 2) );
                    }

                    /* Advance message buffer index after frame 1, bin 1 */
                    txc_tx.buf.pos += (txc_reach_frame_size - 2);

                  } /* endif (Lots of message left, build frame 1, bin 1) */

                } //5ms
                else /* if ( txc_reach_frame_length == ENC_SCH_FRAME_SIZE_10_V ) */
                { //10ms builds data for bin2

                  /* Lots of message left, build frame 1, bin 2 */
                  if (left >= (txc_reach_frame_size - 2))   
                  {
                    if (txc_reach_20ms_frame_cnt % 2)
                    {
                      /* Copy SI bits of frame buffer with zeroes */
                      b_copy( zeros, 0, txc_reach_frame_1_bin_2, 0, 2 );

                      /* Copy full frame of msg data from msg buffer to 
                         frame 1, bin 2 buffer. */
                      b_copy( txc_tx.buf.msg, txc_tx.buf.pos,
                      txc_reach_frame_1_bin_2, 2, (txc_reach_frame_size - 2) );
                    }
                    else /* Build frame 2 */
                    {
                      /* Copy SI bits of frame buffer with zeroes */
                      b_copy( zeros, 0, txc_reach_frame_2_bin_2, 0, 2 );

                      /* Copy full frame of msg data from msg buffer to 
                         frame 1, bin 2 buffer. */
                      b_copy( txc_tx.buf.msg, txc_tx.buf.pos,
                      txc_reach_frame_2_bin_2, 2, (txc_reach_frame_size - 2) );
                    }

                    /* Advance message buffer index after frame 1, bin 2*/
                    txc_tx.buf.pos += (txc_reach_frame_size - 2);
                  }
                  else /* if (one frame of message left, build frame 1, bin 2) */
                  {
                    if (txc_reach_20ms_frame_cnt % 2)
                    {
                      /* Copy SI bits of frame buffer with zeroes */
                      b_copy( zeros, 0, txc_reach_frame_1_bin_2, 0, 2 );

                      /* Copy remaining bits of msg data from msg buffer to frame buf */
                      b_copy( txc_tx.buf.msg, txc_tx.buf.pos,
                              txc_reach_frame_1_bin_2, 2, left );

                      /* Pad remaining bits of frame buffer with zeroes */
                      b_copy( zeros, 0, txc_reach_frame_1_bin_2, (left + 2), 
                             (txc_reach_frame_size - left - 2) );
                    }
                    else /* Build frame 2 */
                    {
                      /* Copy SI bits of frame buffer with zeroes */
                      b_copy( zeros, 0, txc_reach_frame_2_bin_2, 0, 2 );

                      /* Copy remaining bits of msg data from msg buffer to frame buf */
                      b_copy( txc_tx.buf.msg, txc_tx.buf.pos,
                              txc_reach_frame_2_bin_2, 2, left );

                      /* Pad remaining bits of frame buffer with zeroes */
                      b_copy( zeros, 0, txc_reach_frame_2_bin_2, (left + 2), 
                             (txc_reach_frame_size - left - 2) );
                    }

                    /* Advance message buffer index after frame 1, bin 2 */
                    txc_tx.buf.pos += (txc_reach_frame_size - 2);
                  } /* endif (Lots of message left, build frame 1, bin 2) */
                      
                  //10ms
                } /* endif ( txc_reach_frame_length == ENC_SCH_FRAME_SIZE_5_V ) */

              } 
              else /* if (one frame of message left, build frame 1, bin 0) */
              {
                if (txc_reach_20ms_frame_cnt % 2)
                {
                  /* Copy SI bits of frame buffer with zeroes */
                  b_copy( zeros, 0, txc_reach_frame_1_bin_0, 0, 2 );

                  /* Copy remaining bits of msg data from msg buffer to frame buf */
                  b_copy( txc_tx.buf.msg, txc_tx.buf.pos,
                  txc_reach_frame_1_bin_0, 2, left );

                  /* Pad remaining bits of frame buffer with zeroes */
                  b_copy( zeros, 0, txc_reach_frame_1_bin_0, (left + 2), 
                                   (txc_reach_frame_size - left - 2) );
                }
                else /* Build frame 2 */
                {
                  /* Copy SI bits of frame buffer with zeroes */
                  b_copy( zeros, 0, txc_reach_frame_2_bin_0, 0, 2 );

                  /* Copy remaining bits of msg data from msg buffer to frame buf */
                  b_copy( txc_tx.buf.msg, txc_tx.buf.pos,
                          txc_reach_frame_2_bin_0, 2, left );

                  /* Pad remaining bits of frame buffer with zeroes */
                  b_copy( zeros, 0, txc_reach_frame_2_bin_0, (left + 2), 
                                   (txc_reach_frame_size - left - 2) );
                } 

                /* Advance message buffer index after frame 1, bin 0 */
                txc_tx.buf.pos += (txc_reach_frame_size - 2);

              } /* endif (lots of message left, build frame 1, bin 0) */

            } /* endif (txc_reach_frame_length == ENC_SCH_FRAME_SIZE_20_V)*/

          }
          else /* if (txc_reach_si_include == FALSE) */
          {

            if (left >= txc_reach_frame_size)   /* Lots of message left */
            {
              /* Copy full frame of msg data from msg buffer to frame buffer */
              b_copy( txc_tx.buf.msg, txc_tx.buf.pos,
                      txtc.supp_frame.data, 0, txc_reach_frame_size );
            }
            else
            {
              /* Copy remaining bits of msg data from msg buffer to frame buf */
              b_copy( txc_tx.buf.msg, txc_tx.buf.pos,
                      txtc.supp_frame.data, 0, left );

              // Zeros buffer has 36 bytes od zero now at txccommon 
              // Need to increase zeros buffer at txccommon 
              // for reach frame at 20ms, 38400bps, 744 bits

              /* Pad remaining bits of frame buffer with zeroes */
              b_copy( zeros, 0, txtc.supp_frame.data, left, 
                               (txc_reach_frame_size - left) );
            }

            /* Advance message buffer index */
            txc_tx.buf.pos += txc_reach_frame_size;

          } /* end if (txc_reach_si_include == TRUE) */

          //txc_reach_on
          MSG_HIGH( "ACC_ATT: msg", 0, 0, 0 );

        } /* end if (txc_tx.buf.pos < txc_tx.buf.len) */


        /* Done with message? */
        if (txc_tx.buf.pos >= txc_tx.buf.len)
        {
          txac.state = ACC_ENDPROBE_S;

          ++txac.probe_count;
          switch (++txac.attempt_probe_count)
          {
            case 2:
              (void) parm_inc( CAI_EACH_BA_3_ID, 1 );
              break;
            case 3:
              (void) parm_inc( CAI_EACH_BA_4_ID, 1 );
              break;
            case 4:
              (void) parm_inc( CAI_EACH_BA_5_ID, 1 );
              break;
            case 5:
              (void) parm_inc( CAI_EACH_BA_6_ID, 1 );
              break;
            case 6:
              (void) parm_inc( CAI_EACH_BA_7_ID, 1 );
              break;
            default:
              break;
          }


          /* ------------------------------
          ** Log the Enhanced Access Channel message
          ** ------------------------------ */
          txc_log_reach_msg( &txc_tx.buf );


          if (txc_reach_frame_info_log.hdr.len == 0)
          {
            txc_reach_frame_info_log.hdr.len = sizeof(mux_reach_frame_info_log_type);
          }

          if (txc_reach_frame_info_log.hdr.len != 0)
          {
            /* initialize the REACH channel info */
            txc_reach_psist_delay = 0;
          }


#if defined(FEATURE_IS95B_PILOT_RPT)
          if ( P_REV_IS_4_OR_GREATER)
          {
            (void) rex_set_sigs( &mc_tcb, MCC_TXC_PROBE_SIG );
            MSG_MED( "Acc Prb sgnl txd",0,0,0);
          }
#endif /* FEATURE_IS95B_PILOT_RPT */

          /* ----------------------------------------------------------
          ** After transmitting each access probe, the mobile station
          ** waits a specified period, TA = (1 + ACC_TMO) * 20 ms, from
          ** the end of the slot.  NOTE: Add number of frames remaining
          ** until the end of this REACH slot to TA.
          ** See LAC 2.1.1.2.2.1
          ** ---------------------------------------------------------- */
          txac.countdown = 1 + txac.acc_tmo;

        } /*  end if (txc_tx.buf.pos >= txc_tx.buf.len) */

      } /*  end if (txac.state == ACC_MSG_S) */

    } /* Handle interrupt signal */

  } while (state == TXC_ACC_ATT_S); /* until done with this state */

  enc_is95c_enable( FALSE );
  enc_pch_enable( FALSE );
  enc_sch_enable( FALSE );

  txc_pch_gain = 0;
  txc_sch_gain = 0;

  TXC_SET_PCH_GAIN( txc_pch_gain );
  TXC_SET_SCH_GAIN( txc_sch_gain );

  txc_reach_frame_offset = 0;
  enc_set_frame_off ( 0x0, (byte)txc_reach_frame_offset );
  /* tell the MDSP and HW to load frame offset right away */
  muxmdsp_wr_fundFrameOffsetImmLoadCmd(TRUE);
  enc_stmr_cmd( HWIO_FMSK(MOD_STMR_CMD, OFFSET_LD_IMMD) );

  txc_clr_msgs();                  /* Clear any pending messages to send */
  txc_data_rate = CAI_BLANK_RATE;  /* Initialize data rate indicator */
  txac.frame.dotx = FALSE;         /* Prevent last frame from transmitting */

#ifdef FEATURE_RL_JUMP_ENHANCEMENT
  /* If the PN randomization for ACCESS state was 0, transition to 
  ** TXC_HOLD_ENC_CLK_S state to save from having to Jump to Hyperspace
  ** between ACCESS and TRAFFIC states.
  ** Otherwise, turn off ENC clocks and transition to TXC_ACC_IDLE_S since
  ** the PN randomization must be programmed to 0 for TRAFFIC state.
  ** The re-programming of the PN will take place in the Jump to Hyperspace
  ** between ACCESS and TRAFFIC states.
  ** REACH processing does not include PN randomization so this check is not necessary
  ** but is there for error checking purpose.
  */
  if ( (txac.rn == 0) && ( txac.hold_enc_clk_on ) )
  {
    state = TXC_HOLD_ENC_CLK_S;
    
    /* Arm the timer to expire after allotted time if no command is received 
    ** from MC to transition to Traffic or Access state.  Once timer 
    ** expires, turn off ENC clocks.
    */
    txc_delay_enc_shutdown.counter = TXC_HOLD_ENC_CLK_WAIT_DURATION;

    /* Jump source is set to TXC_JUMP for 2 cases:
    **   In TXC_HOLD_ENC_CLK_S state, the jump is complete.
    **   In TXC_ACC_JUMP_S state, the jump is in progress.
    */
    txc_tx.jump_source = TXC_JUMP;

    /* Set up Hold ENC clocks ISR.
    */
    txc_hold_enc_clk_init();
  }
  else
  {
    MSG_ERROR ("PN rndm val=%d for REACH", txac.rn,0,0);
    /* Set interrupt handler to null and turn off TX clocks */
    txc_tx_shutdown();
  }
  MSG_HIGH ("TX Reach Att done, new state=%d",state,0,0);
#else /* else of FEATURE_RL_JUMP_ENHANCEMENT */

  /* Set interrupt handler to null and turn off TX clocks */
  txc_cdma_enc_shutdown();

#endif /* FEATURE_RL_JUMP_ENHANCEMENT */

#if defined(FEATURE_DIAG_V7_LOG_IFACE)
  if (txac.log_ptr != NULL)
  {
    /* Clean up log entry */
    q_put( txac.log_ptr->desc.q_ptr, &txac.log_ptr->desc.link );
    txac.log_ptr = NULL;
  }
#endif

  if (txac.tcb_ptr != NULL)
  {
    /* Notify requesting task that access attempt is completed */
    (void) rex_set_sigs( txac.tcb_ptr, txac.sigs );
    txac.tcb_ptr = NULL;
  }
  return (state);

} /* txc_reach_access_att */

#endif // FEATURE_IS2000_REL_A_CC



/*===========================================================================

FUNCTION TXC_ACCESS_ATT

DESCRIPTION
  State where access channel transmission is transmitted.

DEPENDENCIES
  None

RETURN VALUE
  New state

SIDE EFFECTS
  None

===========================================================================*/

txc_state_type txc_access_att
(
  txc_state_type state
    /* txc_state - TXC_ACCESS_S */
)
{
  txc_cmd_type  *cmd_ptr;         /* Command from queue */
  rex_sigs_type sigs;             /* Task signals */
  word          dummy;            /* Temp var */
  static word   preamble_cnt = 0; /* Preamble count */
  static word   left;             /* Count bits left */
  qword         slots;            /* Number of slots from time 0, unused */

  byte          pilot_ecio;

#ifdef FEATURE_IS2000_REL_A_CC
// txc_reach_on
  if ( txc_reach_access == TRUE )
  {
    state = txc_reach_access_att(state);

    return (state);
  }
#endif // FEATURE_IS2000_REL_A_CC

  txac.state = ACC_SETUP1_S;      /* Set up for first probe */
  txac.probe_count = 0;           /* Initialize to first probe sequence */
  txac.attempt_probe_count = 0;   /* Access attempt probe number */

  /* -----------------------------------------------------------------
  ** NOTE: The interrupt handler takes care of feeding the actual data
  **       to the encoder.
  ** ----------------------------------------------------------------- */

  (void) rex_clr_sigs( &tx_tcb, TXC_INT_SIG );

  do
  {
    /* Wait for command on the queue or a transmit frame interrupt */
    sigs = txc_wait( TXC_CMD_Q_SIG | TXC_INT_SIG );

    if ((sigs & TXC_CMD_Q_SIG) != 0)
    {
      (void) rex_clr_sigs( &tx_tcb, TXC_CMD_Q_SIG );

      if ((cmd_ptr = (txc_cmd_type*) q_get( &txc_cmd_q )) != NULL)
      {
        /* Process the received command */
        state = txc_access_cmd( cmd_ptr, TXC_ACC_ATT_S );
      }
    } /* end if ((sigs & TXC_CMD_Q_SIG) != 0) */

    /* Handle signal from interrupt handler */
    else if ((sigs & TXC_INT_SIG) != 0)
    {
      (void) rex_clr_sigs( &tx_tcb, TXC_INT_SIG );

      /* Assume no transmit */
      txac.frame.dotx = FALSE;

      if (txac.state == ACC_ENDPROBE_S)
      {
        /* -------------------------------------------------
        ** Waiting for the Ack Response Timer (TA) to expire
        ** ------------------------------------------------- */
        if (txac.countdown == 0)
        {
          txac.state = ACC_MSG_DONE_S;
#ifdef FEATURE_IS95B_ACCESS_PROBE_HO
          if (txc_suspend_after_ta)
          {
            txc_suspend_after_ta = FALSE;
            txac.state = ACC_SUSPEND_S;
            /* Signal MC to initiate Access Probe Handoff */
            (void) rex_set_sigs(&mc_tcb,MCC_TXC_APHO_OK_SIG);
          }
#endif /* FEATURE_IS95B_ACCESS_PROBE_HO */
        }
        else
        {
          txac.countdown--;
        }
      } /* end if (txac.state == ACC_ENDPROBE_S) */

      if (txac.state == ACC_MSG_DONE_S)
      {
      /* Check if the Probes are suspended due to T72M timer */
      if (!txac.stop_probes)
      {
        if (txac.nstep--)
        {
          /* ------------------------------------------------------
          ** More probes remaining in current access probe sequence
          ** ------------------------------------------------------ */

          txac.power = txac.is95a_pwr;

          /* Check if next power step will be greater than MAX */
          if ((txac.power >= 0x80) && ((txac.power - txac.pstep) < 0x80))
          {
            /* Limit power */
            txac.power = TXC_PWR_MAX;
          }
          else
          {
            /* Increase power */
            txac.power -= txac.pstep;
          }

          txac.is95a_pwr = txac.power;

          /* -------------------------------------------------------------
          ** If more than one Access Channel supported by the Base Station
          ** ------------------------------------------------------------- */
          if (txac.acc_chan > 0)  // BNFIX
          {
            /* ----------------------------------------------------------
            ** Refer to IS-95-B, the mobile can choose the Access Channel
            ** before each probe.
            ** ---------------------------------------------------------- */
            txc_compute_access_chan();

#ifndef FEATURE_RL_JUMP_ENHANCEMENT
            /* If feature is defined, mask is programmed inside txc_compute_access_chan().
            */

            /* Load user PN mask  */
            enc_user_pn_mask( txc_tx.buf.mask );
#endif /* !FEATURE_RL_JUMP_ENHANCEMENT */

          }

          /* Redo the preamble */
          txac.state = ACC_SETUP2_S;
        }

        else
        {
          /* ------------------------------------
          ** Have completed access probe sequence
          ** ------------------------------------ */

          if (++txac.seq_num >= txac.max_seq)
          {
            /* ---------------------------------------------------
            ** Maximum access probe sequence have been transmitted
            ** in this access attempt
            ** --------------------------------------------------- */
            state = TXC_ACC_IDLE_S;
            MSG_HIGH( "ACC_ATT: probes done; back to idle", 0, 0, 0 );

            /* Set flag to indicate no msg transmission in progress */
            txc_tx.active_msg = FALSE;
          }

          else
          {
            /* ---------------------------------------------------------
            ** This is the last access probe in this access probe
            ** sequence.  Compute the next random access channel, RA,
            ** and load the new Access Channel long code mask into the
            ** encoder now so that we meet the minimum time required
            ** for the new PN mask to take effect before the next access
            ** probe sequence.
            ** --------------------------------------------------------- */
            txc_compute_access_chan();

#ifndef FEATURE_RL_JUMP_ENHANCEMENT
            /* If feature is defined, mask is programmed inside txc_compute_access_chan().
            */

            /* Load user PN mask  */
            enc_user_pn_mask( txc_tx.buf.mask );
#endif /* !FEATURE_RL_JUMP_ENHANCEMENT */

            /* -----------------------------------
            ** Start another access probe sequence
            ** ----------------------------------- */
            txac.state = ACC_SETUP1_S;
            txac.first_frame = FALSE;     /* Not first frame */
            txac.probe_count = 0;
            txac.power = txac.init_pwr;
            txac.nstep = txac.num_step;

            /* initialize the is95a power accumulation */
            txac.is95a_pwr = txac.init_pwr;
          }
        }
      }
      else
      {
        /* Temporarily disable the transmitter and stop sending
        ** any probes until further enabled */
        txac.state = ACC_SUSPEND_S;
      }
      } /* end if (txac.state == ACC_MSG_DONE_S) */

      if (txac.state == ACC_SUSPEND_S)
      {
        /*
        ** There is no need to perform any processing in this
        ** state at this time. Below is the comment from the IS-95B
        ** development branch
        */
        /* -------------------------------------------------------
        ** The TXC is in Suspended state, Transmitter is disabled
        ** and the Probe transmission is suspended. The TXC will
        ** resume transmission of the probes once it is re-enabled
        ** ----------------------------------------------------- */
      }

      if (txac.state == ACC_SETUP1_S || txac.state == ACC_SETUP2_S)
      {
        /* ------------------------------
        ** Log the Access Channel message
        ** ------------------------------ */
        txc_log_ac_msg( &txc_tx.buf );

#if !defined (FEATURE_DIAG_V7_LOG_IFACE)
        if (acc_log.hdr.len == 0)
        {
          acc_log.hdr.len = sizeof(log_acc_info_type);
        }

        if (acc_log.hdr.len != 0)
        {
          acc_log.seq_num = txac.seq_num + 1;
          acc_log.rt      = 0;
          acc_log.rs      = 0;
          acc_log.psist   = 0;
          acc_log.ra      = txac.ra;
          acc_log.rn      = (byte) txac.rn;
        }

#else
        if (txac.log_ptr == NULL)
        {
          /* ---------------------------------------------------
          ** Get a buffer for logging Access Channel information
          ** --------------------------------------------------- */
          if ((txac.log_ptr =
                  (txc_log_acc_type*) q_get( &txc_log_acc_q )) == NULL)
          {
            MSG_MED( "0 bufs on log_acc_q", 0, 0, 0 );
          }
        }

        if (txac.log_ptr != NULL)
        {
          txac.log_ptr->acc.seq_num = txac.seq_num + 1;
          txac.log_ptr->acc.rt      = 0;
          txac.log_ptr->acc.rs      = 0;
          txac.log_ptr->acc.psist   = 0;
          txac.log_ptr->acc.ra      = txac.ra;
          txac.log_ptr->acc.rn      = (byte) txac.rn;
        }
#endif /* if !FEATURE_DIAG_V7_LOG_IFACE */

        /* Initialize preamble counter */
        preamble_cnt = 0;

        /* Set slots = current frame number */
        qw_equ( slots, txc_tx.frame_cnt );
        /* Test next frame */
        qw_inc( slots, TXC_ACCESS_SLIP );


        /* Get number of frames into this Access Channel slot */
        txac.countdown = qw_div( slots, slots, txac.slotting );

        if (txac.countdown != 0)
        {
          /* Make it a countdown to the next slot, not a countup */
          txac.countdown = txac.slotting - txac.countdown;
        }

        if (txac.state == ACC_SETUP2_S)
        {
          /* ----------------------------------------------------------------
          ** If more than one Access Channel is supported by the Base Station
          ** ---------------------------------------------------------------- */
          if (txac.acc_chan > 1)
          {
            /* -------------------------------------------------------------
            ** Compute probe backoff delay, RT, from 0 to PROBE_BKOFF slots.
            **-------------------------------------------------------------- */
            dummy = ran_dist( ran_next(), 0, txac.probe_bkoff+1 );
          }
          else
          {
            /* ----------------------------------------------------------
            ** Compute probe backoff delay, RT, from 0 to 1 + PROBE_BKOFF
            ** slots.
            **----------------------------------------------------------- */
            dummy = ran_dist( ran_next(), 0, txac.probe_bkoff+2 );
          }

          MSG_MED("# Acc Chan = %d, RT = %d", txac.acc_chan, dummy, 0);
          /* -------------------------------------
          ** Add probe backoff delay to countdown.
          ** ------------------------------------- */
          // dummy is 5 bits, slotting is 4 bits.  Product is 9 bits.
          txac.countdown += dummy * txac.slotting; //lint !e734: Loss of precision (31 to 16 bits)

#if !defined(FEATURE_DIAG_V7_LOG_IFACE)
          acc_log.rt = (byte)dummy;
#else
          if (txac.log_ptr != NULL)
          {
            txac.log_ptr->acc.rt = (byte) dummy;
          }
#endif
        }

        /* ----------------------------------------------------------
        ** Before transmitting each access probe sequence other than
        ** the first access probe sequence, the mobile station shall
        ** generate a random number, RS, from 0 to (BKOFFs + 1).  The
        ** mobile station shall delay the transmission of the access
        ** probe sequence for RS slots.
        ** ---------------------------------------------------------- */
        if ((txac.state == ACC_SETUP1_S) && (txac.seq_num != 0))
        {
          dummy = ran_dist( ran_next(), 0, txac.bkoff+2 );
          // dummy is 5 bits, slotting is 4 bits.  Product is 9 bits.
          txac.countdown += dummy * txac.slotting; //lint !e734: Loss of precision (31 to 16 bits)

#if !defined(FEATURE_DIAG_V7_LOG_IFACE)
          acc_log.rs = (byte) dummy;
#else
          if (txac.log_ptr != NULL)
          {
            txac.log_ptr->acc.rs = (byte) dummy;
          }
#endif
        }

        if (txac.state == ACC_SETUP1_S)
        {
          txac.state = ACC_BACK1_S;
        }
        else
        {
          txac.state = ACC_BACK2_S;
        }
      } /* end if (txac.state == ACC_SETUP1_S || ACC_SETUP2_S) */

      if (txac.state == ACC_BACK1_S || txac.state == ACC_BACK2_S)
      {
        if (txac.countdown == 0)
        {
          /* --------------------------------------------------------------
          ** Backoff timer has expired --> If the access attempt is an
          ** Access Channel request, then before transmitting the first
          ** access probe in the access probe sequence, the mobile station
          ** shall perform a persistence test for each Access Channel slot.
          ** -------------------------------------------------------------- */
#if !defined(FEATURE_DIAG_V7_LOG_IFACE)
          if ((txac.req) && (txac.state == ACC_BACK1_S))
          {
              acc_log.psist++;
          }
#else
          if ((txac.req) && (txac.state == ACC_BACK1_S) &&
              (txac.log_ptr != NULL))
          {
            /* Increment number of persistence tests performed */
            txac.log_ptr->acc.psist++;
          }
#endif

          if ((txac.req) && (txac.state == ACC_BACK1_S) &&
              ((ran_next()) > txac.p))
          {
            /* Persistence test failed - try again next slot */
            txac.countdown += (txac.slotting-1);

            /* Events report: Access Denied */
            event_report(EVENT_ACCESS_DENIED);
          }
          else
          {

            /* -------------------------------------------------------------
            ** the interference correction adjustment for the transmit power
            **
            ** note: srch_get_pilot_ecio return pilot_ecio in 1/2db
            ** ------------------------------------------------------------- */
            pilot_ecio = srch_mux_get_pilot_ecio();
            /*lint -e732 */
            txac.int_corr = MIN( MAX(-14+pilot_ecio,0), 14 );
            /*lint +e732 */

            /* Check if add interference correction power will be greater than MAX */
            if ((txac.is95a_pwr >= 0x80) && ((txac.is95a_pwr - txac.int_corr) < 0x80))
            {
               /* Limit power */
               txac.power = TXC_PWR_MAX;
            }
            else
            {
               /* Increase power */
               txac.power = txac.is95a_pwr - txac.int_corr;
            }

            MSG_MED("Seq=%d prb=%d ecio=%d",txac.seq_num+1,txac.probe_count+1,pilot_ecio/2);
            MSG_MED("Tx adj=%d 95A=%d i-corr=%d", (-((signed char)txac.power))/2,(-((signed char)txac.is95a_pwr))/2,txac.int_corr/2);

            /* Begin transmitting Access Channel preamble */
            txac.state = ACC_PRE_S;
            preamble_cnt = 0;
          }
        }
        else /* Backoff timer has not expired */
        {
          txac.countdown--;
        }
      } /* end if (txac.state == ACC_BACK1_S || txac.state == ACC_BACK2_S) */

      if (txac.state == ACC_PRE_S)  /* Preamble transmit substate */
      {
        if (preamble_cnt < txac.preambles)
        {
          preamble_cnt++;
          txac.frame.dotx = TRUE; /* Enable transmit */

#ifdef TMOBILE
#error code not present
#endif
        }
        else
        {
          /* -------------------------------------------------------
          ** Preamble transmitted -- transmit Access Channel message
          ** capsule immediately following the preamble
          ** ------------------------------------------------------- */
          txac.state       = ACC_MSG_S; /* Message transmit substate */
          txc_tx.buf.pos   = 0;         /* Start at msg buffer position 0 */
          txac.first_frame = TRUE;
        }
      } /* end if (txac.state == ACC_PRE_S) */

      if (txac.state == ACC_MSG_S) /* Message transmit substate */
      {
        txac.frame.dotx = TRUE; /* Enable transmit */

        /* Calculate how much of the message is left to send */
        left = txc_tx.buf.len - txc_tx.buf.pos;

        if (left >= TXC_ACC)   /* Lots of message left */
        {
          /* Copy full frame of msg data from msg buffer to frame buffer */
          b_copy( txc_tx.buf.msg, txc_tx.buf.pos,
                  txac.frame.data, 0, TXC_ACC );
        }
        else
        {
          /* Copy remaining bits of msg data from msg buffer to frame buf */
          b_copy( txc_tx.buf.msg, txc_tx.buf.pos,
                  txac.frame.data, 0, left );

          /* Pad remaining bits of frame buffer with zeroes */
          b_copy( zeros, 0, txac.frame.data, left, (TXC_ACC - left) );
        }

        /* Advance message buffer index */
        txc_tx.buf.pos += TXC_ACC;

        /* Done with message? */
        if (txc_tx.buf.pos >= txc_tx.buf.len)
        {
          txac.state = ACC_ENDPROBE_S;

          ++txac.probe_count;
          switch (++txac.attempt_probe_count)
          {
            case 2:
              (void) parm_inc( CAI_ACC_3_ID, 1 );
              break;
            case 3:
              (void) parm_inc( CAI_ACC_4_ID, 1 );
              break;
            case 4:
              (void) parm_inc( CAI_ACC_5_ID, 1 );
              break;
            case 5:
              (void) parm_inc( CAI_ACC_6_ID, 1 );
              break;
            case 6:
              (void) parm_inc( CAI_ACC_7_ID, 1 );
              break;
            default:
              break;
          }

#if defined(FEATURE_IS95B_PILOT_RPT)
          if ( P_REV_IS_4_OR_GREATER)
          {
            (void) rex_set_sigs( &mc_tcb, MCC_TXC_PROBE_SIG );
            MSG_MED( "Acc Probe signal sent",0,0,0);
          }
#endif /* FEATURE_IS95B_PILOT_RPT */

          /* ----------------------------------------------------------
          ** After transmitting each access probe, the mobile station
          ** waits a specified period, TA = (2 + ACC_TMO) * 80 ms, from
          ** the end of the slot.  NOTE: Add number of frames remaining
          ** until the end of this Access Channel slot to TA.
          ** ---------------------------------------------------------- */
          txac.countdown = ((txac.slotting - txac.preambles) -
                            ((txc_tx.buf.len + TXC_ACC) / TXC_ACC)) +
                           ((2 + txac.acc_tmo) * 4);

          /* ---------------------------------------------------------
          ** In the above calculation, if txc_tx.buf.len is an integer
          ** multiple of TXC_ACC, the countdown will be one frame too
          ** small.
          ** --------------------------------------------------------- */
          if ((txc_tx.buf.len % TXC_ACC) == 0)
          {
            txac.countdown++;
          }
        }
      } /* end if (txac.state == ACC_MSG_S) */

      else if (txac.frame.dotx)
      {
        /* Send frame of all zeroes */
        b_copy( zeros, 0, txac.frame.data, 0, TXC_ACC );
      }
    } /* Handle interrupt signal */

  } while (state == TXC_ACC_ATT_S); /* until done with this state */

  txc_clr_msgs();                  /* Clear any pending messages to send */
  txc_data_rate = CAI_BLANK_RATE;  /* Initialize data rate indicator */
  txac.frame.dotx = FALSE;         /* Prevent last frame from transmitting */

#ifdef FEATURE_RL_JUMP_ENHANCEMENT
  /* If the PN randomization for ACCESS state was 0, transition to 
  ** TXC_HOLD_ENC_CLK_S state to save from having to Jump to Hyperspace
  ** between ACCESS and TRAFFIC states.
  ** Otherwise, turn off ENC clocks and transition to TXC_ACC_IDLE_S since
  ** the PN randomization must be programmed to 0 for TRAFFIC state.
  ** The re-programming of the PN will take place in the Jump to Hyperspace
  ** between ACCESS and TRAFFIC states.
  ** REACH processing does not include PN randomization.
  */
  if ( ( txac.rn == 0 ) && ( txac.hold_enc_clk_on ) )
  {
    state = TXC_HOLD_ENC_CLK_S;
    
    /* Arm the timer to expire after allotted time if no command is received 
    ** from MC to transition to Traffic or Access state.  Once timer 
    ** expires, turn off ENC clocks.
    */
    txc_delay_enc_shutdown.counter = TXC_HOLD_ENC_CLK_WAIT_DURATION;

    /* Jump source is set to TXC_JUMP for 2 cases:
    **   In TXC_HOLD_ENC_CLK_S state, the jump is complete.
    **   In TXC_ACC_JUMP_S state, the jump is in progress.
    */
    txc_tx.jump_source = TXC_JUMP;

    /* Set up Hold ENC clocks ISR.
    */
    txc_hold_enc_clk_init();
  }
  else
  {
    /* Set interrupt handler to null and turn off TX clocks */
    txc_tx_shutdown();
  }
  MSG_HIGH ("TX Acc Att stopped, new state=%d",state,0,0);
#else
  /* Set interrupt handler to null and turn off TX clocks */
  txc_cdma_enc_shutdown();
#endif /* FEATURE_RL_JUMP_ENHANCEMENT */

#if defined(FEATURE_DIAG_V7_LOG_IFACE)
  if (txac.log_ptr != NULL)
  {
    /* Clean up log entry */
    q_put( txac.log_ptr->desc.q_ptr, &txac.log_ptr->desc.link );
    txac.log_ptr = NULL;
  }
#endif

  if (txac.tcb_ptr != NULL)
  {
    /* Notify requesting task that access attempt is completed */
    (void) rex_set_sigs( txac.tcb_ptr, txac.sigs );
    txac.tcb_ptr = NULL;
  }

  return (state);

} /* txc_access_att */

/*===========================================================================

FUNCTION TXC_ACCESS_IDL

DESCRIPTION
  Access channel idle state.  Waits for command to transmit an access
  attempt or leave the access state.

DEPENDENCIES
  None

RETURNVALUE
  New state

SIDE EFFECTS
  None

===========================================================================*/
txc_state_type txc_access_idl
(
  txc_state_type state
    /* txc_state - TXC_ACC_IDLE_S */
)
{
  txc_cmd_type *cmd_ptr;
    /* Command from queue */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    (void) rex_clr_sigs( &tx_tcb, TXC_CMD_Q_SIG );

    /* Get items off the queue */
    if ((cmd_ptr = (txc_cmd_type*) q_get( &txc_cmd_q )) != NULL)
    {
      /* Process messages common to all idle substates */
      state = txc_access_cmd( cmd_ptr, TXC_ACC_IDLE_S );
    }

    else
    {
      (void) txc_wait( TXC_CMD_Q_SIG );
    }
  } while (state == TXC_ACC_IDLE_S); /* Until done with this state */

  return (state);

} /* txc_access_idl */

#ifdef FEATURE_RL_JUMP_ENHANCEMENT
/*===========================================================================

FUNCTION TXC_OK_TO_SLEEP

DESCRIPTION
  This function returns true if MC can begin sleep.  This mechanism is in
  place to delay shutdown for a period after an access attempt to prevent
  the need to jump to hyperspace for another attempt that occurs soon
  thereafter.

DEPENDENCIES

RETURN VALUE
  True if it is okay to sleep.  False if sleep is to be delayed.

SIDE EFFECTS
  None.
===========================================================================*/
boolean txc_ok_to_sleep( void )
{
  return txc_delay_enc_shutdown.txc_sleep_allowed;
}

/*===========================================================================

FUNCTION TXC_TX_SHUTDOWN

DESCRIPTION
  Turns off tx clock and disables TXC interrupt.  Allows MC to command SRCH to
  sleep.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void txc_tx_shutdown( void )
{
  MSG_HIGH("Shutdown TX clock",0,0,0);
  txc_cdma_enc_shutdown();

  /* Allow MC/SRCH to sleep.
  */
  txc_delay_enc_shutdown.txc_sleep_allowed = TRUE;
}

/*===========================================================================

FUNCTION txc_hold_enc_clk_init

DESCRIPTION
  Initialization section used in TXC_HOLD_ENC_CLK_S and TXC_ACC_JUMP_S states.
  In both states, ISR needs to be txc_hold_enc_clk_isr.
  In both states, jump source needs to be set to TXC_JUMP.

DEPENDENCIES
  None
RETURN VALUE
  None.
SIDE EFFECTS
  None
===========================================================================*/
void txc_hold_enc_clk_init (void)
{
  MSG_MED ("Init Hold ENC Clocks, TX disable", 0,0,0);

  /* Make sure power amp is off */
  rfm_tx_disable(RFCOM_TRANSCEIVER_0);  /* Notify MDSP that TX is OFF */
  
  /* Reset shut down signal, do not allow MC/SRCH to sleep.
  */
  (void) rex_clr_sigs (&tx_tcb, TXC_TX_SHUTDOWN_SIG);
  txc_delay_enc_shutdown.txc_sleep_allowed = FALSE;

  /* Timer count down is done in interrupt context.  Actual shut down
  ** is done in task context.  Set up interrupt function here.
  */
  txcmod_int_deregister_all (TXMOD_INT_MUX_CLT);
  (void) txcmod_int_regis_for_pcgint(TXC_SLOT0_ISR_PCG_NUM,
                                     txc_hold_enc_clk_isr, 
                                     ENC_STATUS_INTERRUPT_SLOT_0,
                                     TXMOD_INT_MUX_CLT);

} /* txc_hold_enc_clk_init */

/*===========================================================================

FUNCTION txc_hold_enc_clk

DESCRIPTION
  In this state, hold the encoder clocks ON for a specified time while waiting
  for the appropriate command(s) from MC to transition to the next state.
  If timer expires, turn off ENC clocks and transition back to Access Idle state.
  
DEPENDENCIES
  None

RETURNVALUE
  New state

SIDE EFFECTS
  None

===========================================================================*/
txc_state_type txc_hold_enc_clk
(
  txc_state_type state
    /* txc_state - TXC_HOLD_ENC_CLK_S */
)
{
  txc_cmd_type  *cmd_ptr;         /* Command from queue */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    (void) rex_clr_sigs( &tx_tcb, TXC_CMD_Q_SIG );

    /* Get items off the queue */
    if ((cmd_ptr = (txc_cmd_type*) q_get( &txc_cmd_q )) != NULL)
    {
      /* Process messages common to all idle substates */
      state = txc_access_cmd (cmd_ptr, TXC_HOLD_ENC_CLK_S);

      /* Perform state transition upon reception of access (and reach) or
      ** traffic command from MC.
      */
      switch (txc_tx.jump_source)
      {
        case TXC_JUMP:
          break;

        case TXC_JUMP_ACCESS:
        case TXC_JUMP_TRAFFIC:
          state = txc_access_or_traffic_cmd (TXC_HOLD_ENC_CLK_S);
          break;

        case TXC_READY_TO_JUMP:
        default:
          MSG_ERROR ("Unxpctd src in jmp done st",0,0,0);
          break;
      } /* end switch */

    }
    else
    {
      word sigs;
      sigs = txc_wait (TXC_CMD_Q_SIG | TXC_TX_SHUTDOWN_SIG);
      if (sigs & TXC_TX_SHUTDOWN_SIG)
      {
        (void) rex_clr_sigs (&tx_tcb, TXC_TX_SHUTDOWN_SIG);
        state = TXC_ACC_IDLE_S;
      }
    }

  } while (state == TXC_HOLD_ENC_CLK_S); /* Until done with this state */

  if (state == TXC_ACC_IDLE_S) 
  {
    MSG_HIGH("Rtrn to Idle from Hold Enc Clk st",0,0,0);
    txc_tx_shutdown ();
    if (txac.tcb_ptr != NULL)
    {
      /* Notify requesting task that access attempt is completed */
      (void) rex_set_sigs( txac.tcb_ptr, txac.sigs );
      txac.tcb_ptr = NULL;
    }
  }

  return (state);

} /* end txc_hold_enc_clk */

/*===========================================================================

FUNCTION txc_hold_enc_clk_isr

DESCRIPTION
  Interrupt handler for the TXC_HOLD_ENC_CLK_S state.  Also used when jumping
  to hyperspace in TXC_ACC_JUMP_S.  Transition to traffic state is done here.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void txc_hold_enc_clk_isr ( 
   uint32 arg 
)
{
  (void) arg; /* unused */

  /* Check for commands */
  if (txc_int_cmd.iscmd)
  {
    /* Make sure power amp is off */
    enc_tx_enable( FALSE );

    switch (txc_int_cmd.cmd)
    {
      case INT_TC_F:
        MSG_MED("Got INT_TC_F", 0, 0, 0);
            /* --------------------------
        ** Go to Traffic Channel mode
        ** -------------------------- */
        enc_set_mode( ENC_TC_MODE );

        /* set initial RC */
        #ifdef FEATURE_IS2000_P2
        if ( txc_so.rev_dcch.included )
        {
          enc_dcch_rc( (enc_fch_radio_config_type) txc_so.rev_dcch.rc );
        }
        if ( txc_so.rev_fch.included )
        #endif // FEATURE_IS2000_P2
        {
          enc_fch_rc( (enc_fch_radio_config_type) txc_so.rev_fch.rc );
        }

        /* Load user PN mask */
        enc_user_pn_mask( txc_int_cmd.parm.tc.mask );

        /* Set the counter so that TXC ISR calculates the frame number for
           first NO_OF_ISRS_AFTER_FO isrs. */
        after_fo_handoff_count_isrs = NO_OF_ISRS_AFTER_FO;

        txcmod_int_deregister_all(TXMOD_INT_MUX_CLT);
        (void) txcmod_int_regis_for_pcgint(txc_slot0_isr_pcg,
                                           txc_traffic_isr,
                                           ENC_STATUS_INTERRUPT_SLOT_0,
                                           TXMOD_INT_MUX_CLT);
        (void) txcmod_int_regis_for_pcgint(TXC_SLOT3_ISR_PCG_NUM, 
                                           txc_traffic_isr,
                                           ENC_STATUS_INTERRUPT_SLOT_3,
                                           TXMOD_INT_MUX_CLT);
        break;

      default:
        ERR( "Bad cmd %d", txc_int_cmd.cmd, 0, 0 );
    } /* switch */

    txc_int_cmd.iscmd = FALSE;     /* done with command */
    (void) rex_set_sigs( &tx_tcb, TXC_INT_CMD_SIG );

  } /* end if (txc_int_cmd.iscmd) */

  /* Count down Jump done timer.  Signal Transmit task when timer expires.
  */
  if (txc_delay_enc_shutdown.counter > 0) 
  {
    txc_delay_enc_shutdown.counter--;
    if (txc_delay_enc_shutdown.counter == 0) 
    {
      /* Let the TXC task know that the Jump done timer has expired. 
      */
      (void) rex_set_sigs (&tx_tcb, TXC_TX_SHUTDOWN_SIG);
    }
  }

} /* end txc_hold_enc_clk_isr */

#endif /* FEATURE_RL_JUMP_ENHANCEMENT */

#if defined(FEATURE_VOIP_1X_HANDOFF) || defined(FEATURE_UMTS_1X_HANDOVER_1XMSM)
/*===========================================================================

FUNCTION TXC_TT_INIT

DESCRIPTION
  Initialize TXC to go in Timing Transfer State.

DEPENDENCIES
 
RETURN VALUE
  None.
SIDE EFFECTS
  None
===========================================================================*/
void txc_tt_init(void)
{
  int isav;

  /* Critical section, lock the INT, turn on the ENC clock, disable TX and
  reset the PN delay. In this case, the BTF will be load in the RL timing
  */
  INTLOCK_SAV (isav);
  enc_acp_pa_ctl(FALSE);      /* force off the PA       */
  enc_tx_clk_enable( TRUE );  /* turn on the Enc clocks */
  enc_tx_enable (FALSE);      /* set puncturing off now that clks on */ 

  /* this function call will load the enc_btf_chipx8 into the TX time. 
  Make sure TX time offsets by a enc_btf_chipx8 in respect to the RX time. 
  The new hardware allows us to load in the BTF value at any RX 26.6 ms 
  boundary. The load value will be computed as : 
  ((phase + 1) % 3)* (2^15)*8 + enc_btf_chipx8 + 2, where phase from 0 to 2,
  but the HW is smart enough, it knows what is the current phase, so it will 
  take care the ((phase + 1) % 3)* (2^15)*8 part. The SW only needs 
  to load in enc_btf_chipx8 + 2 */
  (void) enc_pn_rand_delay( 0 );
  HWIO_OUT( MOD_STMR_CMD, 0x00 );
  INTFREE_SAV (isav);

  /* In the normal call, the power level of the last access probe will 
  be use for the start power level of the traffic. Since in this case, 
  we skip the access probe so we use purely open-loop, no adjust or 
  anything*/
  txac.last_pwr = 0; 

  txcmod_int_deregister_all(TXMOD_INT_MUX_CLT);

  /* set up hardware for TCI Init, select MOD for 1x */
  enc_set_mode( ENC_PC_MODE ); 

  /* IS95-A mode */
  enc_is95c_enable( FALSE );

  /* the Pilot Channel enable bit must be cleared */
  enc_pch_enable( FALSE );

  /* the Fundamental Channel enable bit must be cleared */
  enc_fch_enable( FALSE );

  /* the Supplemental Channel enable bit must be cleared */
  enc_sch_enable( FALSE );

  /* Dedicated Control Channel enable bit must be cleared */
  enc_dcch_enable( FALSE );

  /* zero out all the digital gains */
  txc_pch_gain = 0;
  txc_fch_gain = 0;
  txc_sch_gain = 0;
  txc_dcch_gain = 0;
  TXC_SET_PCH_GAIN( txc_pch_gain );
  TXC_SET_FCH_GAIN( txc_fch_gain );
  TXC_SET_SCH_GAIN( txc_sch_gain );
  TXC_SET_DCCH_GAIN( txc_dcch_gain );
}
/*===========================================================================

FUNCTION TXC_TT_STATE

DESCRIPTION
 During the timing transfer from DO to 1x, after the FL timing is established
 This function will call to setup the RL timing which involving the loading 
 ULPN state process the TXC_TC_F command.

DEPENDENCIES
 
RETURN VALUE
  None.
SIDE EFFECTS
  None
===========================================================================*/
txc_state_type txc_tt_state
(
  txc_state_type state                
)
{
  ulpn_type ulpn;
  /* type to place user long PN code read from Searcher task */

  txc_cmd_type *cmd_ptr;
    /* pointer to received command */

  qword pn_state;
  int isav;
  rex_sigs_type sigs;
  boolean turn_on_transmitter = FALSE;

  /* States of TXC time transfer state machine */
  typedef enum
  {
    TX_TT_INIT,                     /* start state of TXC_TT state machine */
    TX_TT_WAIT_FOR_TC,              /* wait for command to traffic */
    TX_TT_TC_INIT                   /* traffic initialization */
  } tt_substate_type;

  tt_substate_type sub_state = TX_TT_INIT;
  /* initial sub-state of TXC timing transfer */

  ( void )rex_clr_sigs( &tx_tcb,  TXC_PN_ROLL_SIG );
  sigs = 0;
  while( state == TXC_TT_S )
  {
    switch( sub_state )
    {
      case TX_TT_INIT:
      {
        if(sigs & TXC_PN_ROLL_SIG)
        {
          /* process the PN roll signal */
          ( void )rex_clr_sigs( &tx_tcb,  TXC_PN_ROLL_SIG );
          INTLOCK_SAV (isav);
          /* Read and advance ULPN state from Search */
          srch_mux_get_ulpn_state( &ulpn );
          /* advance ULPN by 1 26 ms period */
          ulpn_adjust( &ulpn, 1 );
          qw_set ( pn_state, ( dword ) ulpn.high, ulpn.low );
          enc_iq_pn_state( 0x4000, 0x4000 );   /* load I and Q with MSB 1 */
          enc_user_pn_state( pn_state );       /* load user PN state */
          INTFREE_SAV (isav);
          sub_state = TX_TT_WAIT_FOR_TC;
          MSG_HIGH("Done init RL for timing transfer, waiting for TXC_TC_F "
                    ,0,0,0);
        }
        else
        {
          sigs = txc_wait( TXC_PN_ROLL_SIG);
        }
        break;
      }
      case TX_TT_WAIT_FOR_TC:
      {
        /* jsut wait for CP to command to go to traffic */
        if (sigs & TXC_CMD_Q_SIG)
        {
          ( void )rex_clr_sigs( &tx_tcb, TXC_CMD_Q_SIG );
          if( (cmd_ptr = (txc_cmd_type*) q_get( &txc_cmd_q )) != NULL )
          {
            if(cmd_ptr->hdr.command == TXC_TC_F)
            {
              MSG_HIGH ("Got TXC_TC_F cmd in TXC_TT_S",0,0,0);
              turn_on_transmitter = cmd_ptr->tc.turn_on_transmitter;
              (void) txc_access_cmd(cmd_ptr,TXC_ACC_JUMP_S);
              MSG_HIGH("Done with TXC_TC_F, go to TX_TT_TC_INIT",0,0,0);
              sub_state = TX_TT_TC_INIT;
            }
            else if (cmd_ptr->hdr.command == TXC_EXIT_F) 
            {
              MSG_HIGH("Got TXC_EXIT_F cmd in TXC_TT_S",0,0,0);
              txc_exit();  /* perform exit processing */
              state = TXC_EXIT_S;
              txc_cmd_done( cmd_ptr, TXC_DONE_S );
            }
            else
            {
              txc_cmd_done( cmd_ptr, TXC_BADCMD_S );
              MSG_ERROR("TX_TT_WAIT_FOR_TC, Expct TXC_TC_F, got cmd %d",
                         cmd_ptr->hdr.command,0,0);
            }
          }
        }
        else
        {
          sigs = txc_wait( TXC_CMD_Q_SIG);
        }
        break;
      }
      case TX_TT_TC_INIT:
      {
        /* traffic initialization and change TXC sub-state to TXC_TRAFFIC_S */
        txc_tt_tc_init(turn_on_transmitter);
        state = TXC_TRAFFIC_S;  /* ready to jump to traffic */
        MSG_HIGH("From TXC_TT_S go to TXC_TRAFFIC_S ",0,0,0);
        break;
      }
    }
  } /* while */
  return (state);
} /* txc_tt_state */
#endif /* FEATURE_VOIP_1X_HANDOFF and FEATURE_UMTS_1X_HANDOVER_1XMSM */


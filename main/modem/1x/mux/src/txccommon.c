/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           C D M A    T R A N S M I T   C O M M O N    M O D U L E

GENERAL DESCRIPTION
  Responsible for transmit command processing and utility routines

EXTERNALIZED FUNCTIONS
  txc_cmd - transmit task
  txc_cdma_enc_setup - sets up encoder for CDMA
  txc_cdma_enc_shutdown - shuts down encoder, turns off clock
  txc_data_pump_init - initialize data pump
  txc_flag_erasure - sets erasure flag
  txc_flag_next_init
  txc_get_fch_rate_set - Returns the rate set for FCH
  txc_get_sch_info - returns current configuration information for the SCHs
  txc_get_time_in_1_25ms_unit -  get the current time in 1.25 ms unit and
  factor in the frame offset as well
  txc_int_occured
  txc_powerup_init - Initializes timers and queues for CDMA Transmit task
  txc_register_action_time - enable MCC_TXC_FRAME_SIGs for MC's HO action
  time and for RF tune time
  txc_rpt_voc_rate - Reports the vocoder rate
  txc_set_fpc_mode - ets the value of FPC_MODE in variable txc_so
  txc_set_frame_offset
  txc_set_pch_fch_gain - Sets the gain values in txc_pch_fch_gain_tab
  txc_subtask - Transmit subtask to process CDMA
  txc_suspend_tx - enables/disables the Transmitter and enables/disables the
  transmission of Access probes

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 1991 through 2010 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: O:/src/asw/COMMON/vcs/txccommon.c_v   1.36   15 Oct 2002 18:30:58   donb  $
$Header: //source/qcom/qct/modem/1x/mux/rel/ulc/src/txccommon.c#7 $ $DateTime: 2010/11/22 03:07:58 $ $Author: bbatchu $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/24/10   bb      Changes to call txc_tx_shutdown() to clean up TXC and etc
                   resources, while processing TXC_EXIT_F command
09/07/10   trc     Always delay by a frame+ at the end of access or traffic
                   in txc_cdma_enc_shutdown().  Do not minimize tx_adjust.
                   Ensure TX_ON goes low 20us prior to RF teardown.
04/28/10   jtm     Removed check in txc_cmd() for qcnt > 0 to prevent false
                   err_fatals from occuring since it is expected to have multiple
                   *unique* items in the queue.
07/16/08   jtm     Removed FEATURE_IS2000_REL_C_DV and FEATURE_DELAYED_TX_CLOCK_SHUTDOWN.
                   Removed unused variables and functions for ULC effort to
                   reduce memory. Reduced debug string sizes.
03/31/08   vlc     Added check for illegal consecutive calls to txc_cmd().
04/03/07   vlc     Removed txc_flush_enc() call because it clobbered HDR
                   encoded data in SHDR Rel A mode.  No PA issue has been
                   reported with this code base.
04/02/07   vlc     Added new PCG define for bin0 ENC ISRs specifically for loop
                   back calls.  Moved default PCG to 1 for bin0 ENC ISRs for
                   all other calls.
10/30/06   bn      Added code to support DO/UMTS to 1x handoff
09/05/06   vlc     Implemented TX gain enhancements to increase digital gain
                   resolution.
08/03/06   vlc     Added call to txc_flush_enc() in function txc_subtask().
06/09/06   trc     Remove T_MSM6700 featurization
05/19/06   vlc     In function txc_cdma_enc_shutdown(), check if 1X is
                   operating on the primary transceiver before disabling the
                   transmit chain.
                   Change was merged from MSM6550 code base.
03/20/06   vlc     In traffic state, wait for 1 extra frame before turning off
                   PA so the last encoded frame (if any) could be transmitted.
10/17/05   rkc     Remove wrapper function txc_enc_set_frame_off().
10/12/05   rkc     Add wrapper function txc_enc_set_frame_off().
09/08/05   ejv     Use new srch_mux interface calls.
07/26/05   vlc     Applied code review comments for RL jump to hyperspace
                   enhancement.
07/25/05   rkc     Fixed lint errors.
05/23/05   vlc     Changed FEATURE_IS2000_REL_D_JUMP to FEATURE_RL_JUMP_ENHANCEMENT.
                   Added F3 debug message to function txc_cdma_enc_shutdown().
                   In function txc_subtask(), added support for Fast Call Set Up
                   - New Jump to Hyperspace command feature.
04/27/05   vlc     Main lined code in current targets, cleaned out obsolete targets,
                   cleaned out obsolete features.
03/14/05   vlc     Fixed compiler warnings for MSM6700.
03/14/05   vlc     Merged in changes for MSM7500.  Changed T_QUASAR_BB to T_IO_CARD.
                   (dlb')
03/07/05   vlc     Added support for Fast Call Set Up - New Jump to
                   Hyperspace command feature.
                   Removed check for OK to sleep in txc_exit(), now always
                   turn off ENC clocks in txc_exit().
01/26/05   vlc     Removed obsolete ds.h inclusion.
09/28/04   sr      Merged in changes from 6550 2009 build
09/16/04   jrp     Fixed passThruEnable in support of DO.
09/01/04   jrp     Fixed featurization.
07/28/04   jrp     Renamed rpc_get_sch2pich_gain_ratio.
07/15/04   sr/ll   Featurized out rf driver calls under T_QUASAR_BB
07/15/04   jrp     Added call to muxmdsp_wr_passThruEnable in txc_subtask.
06/29/04   jrp     Added call to txcmod_int_init().
06/16/04   sr      Merged in changes from 6500 5004 build
06/14/04   sr/va   Added HHO cleanup under FEATURE_HHO_ENHANCEMENTS
06/09/04   jyw     modified the txc_subtask() to allow TXC_EXIT_F as the first
                   command issued to TXC from MC; This modification corresponds the
                   new SEARCH_INACTIVE_STATE.
05/21/04   jyw     Intorduce the new RF PA backoff interface
05/18/04   jrp     Reset SCH RC to 3 at txc_exit() for subsequent access.
05/07/04   vlc     Modified RLP logging extensively to decouple MUX processing
                   and RLP processing, moved RLP processing to RLP module.
04/26/04   vlc     Implemented new way of handling RXTX confirmation of all
                   signalling messages for voice calls/data calls/svd calls.
04/26/04   bn      Added a function to turn off the TX code in encoder shutdown function
04/20/04   jrp     Fixed lint warnings.
03/29/04   sr      Removed SAM check before turning off PA in
                   txc_cdma_enc_shutdown()
03/10/04   vlc     Added support for centralized transaction engine handling
                   under feature FEATURE_TRANS_MANAGER.
03/09/04   sr/bkm  Featurized DV code
12/16/03   aaj     Do not disable Tx if 1x does not own RF
10/30/03   jrp     Fixed compiler warnings.
09/15/03   jrp     Added check for FEATURE_DELAYED_TX_CLOCK_SHUTDOWN when
                   leaving CDMA mode.
09/09/03   dna     Merged porting changes for MSM6500
04/15/03   jrp     Support for FEATURE_DELAYED_TX_CLOCK_SHUTDOWN.
10/15/02   dlb     Added new power control logging to combine power control
                   and frame type logging.
09/24/02   jrp     Handled FPC_MODE_FFPC_DISABLED case in txc_set_fpc_mode().
08/30/02   sr      Moved the clearing of TX_ISR up to top of
                   txc_cdma_enc_shutdown() before resetting the RC to 1
08/08/02   vlc     Fixed compiler error when FEATURE_IS95B_MAHHO is turned off.
08/01/02   jrp     Removed txc_flag_eib(), txc_flag_qib(), and txc_flag_sch_eib().
                   These are now modified directly by rxctraffic.  Added featurization
                   for FPC modes 4,5,6.  Changed FPC_MODE enums to be more descriptive
                   due to SVD.
07/17/02   hrk     Support for RPC logging for simulataneous R-FCH and R-DCCH.
06/18/02   jrp     Added support for FFPC Modes 4,5,6.
05/29/02   jrp     Changed txc_suspend_tx() to call txc_acc_probe_resume() to
                   resume probing only if probing was previously suspended.
05/22/02   bn      Added new logics to support VP2 RPC
04/08/02   tc      Supported REACH 20ms frame, 38400bps pad size.
02/25/02   bn      Added support for  MUX SVD signaling
04/02/02   sr      Changed 32X gains to 8 bit values for P2 also
01/15/02   hrk     Changed RF_ENABLE_PWR_WAIT to RF_ENABLE_TX_PWR_WAIT when using
                   FEATURE_RF_WAIT_CHANGES.
12/10/01   hrk     Renamed txc_erasure_flag to txc_eib_flag. Renamed function
                   txc_flag_erasure() to txc_flag_eib().
11/09/01   hrk     Setting MOD_PCBIT_TEST_MASK=0x0000 for FPC_MODE != 3.
11/08/01   hrk     Setting MOD_PCBIT_TEST_MASK for FPC_MODE=3 for MSM5100.
10/30/01   bn      Added support for Release A RDCH logging
08/13/01   bn,hrk  Fixed :
                   1. incorrect turn around constant during R-SCH transition in RPC logs.
                   2. multiple channel adjustment gain for FCH was delayed by 1 frame in
                      RPC logs.
06/15/01   snn     Added two new functions for DCCH UI display.
06/15/01   day     Mainlined FEATURE_RLGC, FEATURE_FFPC, FEATURE_IS95B, FEATURE_IS2000
                   Delete FEATURE_SPECIAL_MDR, FEATURE_FINGER_POLL
06/04/01   vlc     Changed FEATURE_GPSONE to FEATURE_GPS_MODE.
06/01/01   snn     Added new parameter to txc_get_sch_info to return sch rc.
05/15/01   sr      Changed 32X gains to 8 bit values
05/11/01   vlc     Merged in datapump changes for PLT (ks.)
05/03/01   sr      Supported reverse link DCCH PLT.
04/20/01   sr      Merged in from MSM5100 archive
04/18/01   sr      Merged with MSM_MUX1X.01.00.00
           hrk     Default gain table, txc_pch_fch_sch_gain_tab, used by PLT/FTM
                   is based on Nominal Attribute gains specified in IS2000.
           lcc,hrk Added support for FEATURE_IS2000_P2
                   Moved txc_scr_action() and txc_scr_trans_cmd to another file.
           hrk     updated txtc.offset_pwr in Handoff scenario.
03/27/01   vlc     Merged MSM_MUX1X.00.00.17.
           hrk     updated txtc.offset_pwr in Handoff scenario.
03/23/01   sr      Added entry in gain table for 32X
03/01/01   tc      Merged MSM_MUX1X.00.00.15.
02/22/01   ak      Took rlp logging functions out of IRAM.
02/16/01   bgc     Changed FEATURE_FACTORY_TESTMODE to enable a runtime switch.
02/16/01   hrk     Flushing RPC log at the end of call.
01/31/01   tc      Merged GPS support.
01/12/01   tc      Merged MSM_MUX1X.00.00.06.
11/13/00   tc      Defined txc_pch_gain, txc_fch_gain, txc_sch_gain.
11/09/00   tc      Defined feature T_MSM_5100.
11/08/00   tc      Supported MSM5100 modulator driver interface.
01/05/01   lcc     Merged back code inside PLT and T_MOBILE that was accidentally
                   removed.
12/27/00   lcc     In txc_gating_trans_cmd, explicitly turns off rev fch 1/8th
                   rate gating when a "disable 1/8 rate gating" command is received.
11/12/00   sr      (merge) Added support for Full TDSO
11/10/00   jc      repartioned txc.c into several files from Build TR2064
10/19/00   bgc     Fixed warnings for factory test mode.
10/05/00   lcc     Removed call to mctrans_confirm_transaction during transaction
                   processing.  This function is local to the transaction engine.
10/04/00   jq      Added FTM FCH and SCH loopback feature
10/04/00   sr      Took care of programming the modulator every frame with
                   the proper gating mode and pattern information while in
                   FCH 8th rate gating mode.
09/25/00   ak      Removed call to dsrlp_init.  Redundant, since called by
                   call processing.
09/20/00   hrk     Implemented maximum power protection scheme.
09/19/00   hrk     Supported changes to Nominal Gain values for 1/8 R-FCH gating.
09/19/00   sr      Fixed problem with setting SCH gain to 0 if
                   txc_rev_fr_block.num_frames !> 1. This condition is
                   only true for 3G packet data calls.
09/15/00   ak      Some SCH Mux stats fixes.
09/14/00   sr      Supported single-sized PDUs in STDSO
09/12/00   ak      Fixed up parentheses for so 33 data Tx.
09/12/00   sr      Changed ts_get_20ms_frame_time() to
                   ts_get_20ms_frame_offset_time() for action timing
09/06/00   bgc     Added FEATURE_FACTORY_TESTMODE.
09/05/00   ttl     Added DTX duration counting.
                   Enabled the data TX on SCH when the FCH is muted for data.
09/05/00   sr      Removed unused declaration of qword current_frame_time
08/25/00   rc      Added changes to support NULL returns from
                   dsm_new_buffer().
08/24/00   hrk     Removed redundant action time code.
08/21/00   hrk     Using the rate of the frame being currently transmitted to
                   compute channel power. Earlier the rate of the frame being
                   built (i.e. next frame) was used - resulting in incorrect
                   values for R-PICH, R-FCH ... during rate transitions.
08/14/00   sr      Added support for pilot gating and FCH 1/8 rate gating in
                   PLT
08/02/00   sr      Fixed deprecated declaration of txc_cdma_enc_setup()
08/02/00   sr      Fixed typo in feature name
07/31/00   lad     Added R-SCH0 to logging, fixed sub-record count bug.
07/31/00   sr      Fixed STDSO bug re Mux PDU header not being inserted
07/31/00   sr      Marked several functions with #ifndef MSM5000_IRAM_REV
07/25/00   sr      Added FEATURE_DATA_TIMELINE_ADVANCE to move to encoder
                   interrupt to the 5ms mark for 3G packet data calls
07/24/00   hrk     Collecting AGC values for Reverse Power Control logging
                   in rxc_tc_isr().
07/24/00   ak      Added R-SCH logging.
07/20/00   ak      Removed old mux sch stats struct (replaced by parm db).
                   Also removed FEATURE_IS2000_SCH_STATS.  Fixes for mux
                   stats with SO 33 and TDSO.
07/18/00   sr      Fixed improper TDSO featurization
                   Changed tc to ret while checking for fch_gating in
                   TXC_RETURNED_F
07/17/00   sr      Added #ifndef FEATURE_PLT around FCH gating code
07/17/00   sr      Added FCH 1/8 rate gating support
07/14/00   sr      Added FCH 1/8 rate gating support
                   Added support for hard handoff preamble in PCG units
                   Turned off sio interrupts (under FEATURE_SIO_INT_LOCKOUT)
                   around txc_tx_frame_proc()
07/12/00   sr      Fixed tdso 1x SCH
07/12/00   sr      Changed argument type for txc_tdso_traffic()
07/10/00   sr      Fixed incomplete featurization of TDSO
07/10/00   sr      Added STDSO support
07/10/00   sr      Swapped arguments to txc_rlp_fch_frame_len_tbl
07/10/00   sr      Changed txc_rev_sch_trans_cmd() to set ltu_size to 0 for
                   turbo codes
                   Added boolean argument to enc_sch_data_out_cfg()
06/21/00   lcc     Increased number of action items to 10.  Change message level
                   when out of action items to error.
06/15/00   lcc     Changes to handle new HO action interface:
                   1. num_preamble now comes with unit (frames or PCGs) as well
                   2. Inclusion of nom_pwr is now optional.
                   3. Corrected a problem with quadword comparison.
06/15/00   ak      Fixes for 1x R-SCH:  zero LTUs with 1x or 2x.  Also make
                   sure to pre-pend the MM bit on 1x frames before sending
                   to sch encoder.
06/13/00   ak      Added SCH0 mux stats for parm database.
06/13/00   sr      Enable FCH after preambles are done, because in HHO case
                   txc does not get any TXC_NULLS_F to tell it that we are
                   done with preambles
06/09/00   hrk     Added support for new MC-RLGC interface.
06/02/00   ak      Added R-SCH stats.
05/25/00   sr      Fixed a FEATURE_IS2000_R_SCH bug where some essential common
                   cleanup code was being excluded.
05/24/00   fc      Merged the following changes:
           ry      Added OTAPA support
05/22/00   sr      Inserted some missing FEATURE_IS2000_R_SCH compiler flags
05/16/00   sr      Set sch_on to FALSE in TXC_RELEASE_F
05/16/00   sr      Added support for TXC_MC_TRANS_F action transaction
                   processing for R_SCH command
05/16/00   na      Added R_SCH support
05/08/00   hrk     Added support for FFPC FPC_MODE 011 (EIB 50Hz).
                   In TXC_RELEASE_F, frame offset should be reset to 0
                   only for PLT case.
                   Deleted ERR mesg in txc_scr_action if fch_incl is FALSE.
04/20/00   lad     Updated frame types and pctrl logging.
04/14/00   sr      Swapped arguments to txc_hho() to the proper order
                   Changed the treatment of implicit actions
04/12/00   hrk     Renamed rxc_rpctrl_dec_history to rxc_rpc_history.
04/11/00   lad     Added reverse power control and frame types logging.
                   Cleaned up RLP logging.
04/06/00   na      Fixed RLP logging problem for mobile transmitted frames -
                   the len table was transposed. Fix under FEATURE_RLP_LOGGING
04/05/00   sr      Added programming of power control step size in HO
                   command
                   In RC3 and 4 turned off FCH during preamble as per std.
04/04/00   hrk     Added function rpc_get_relative_gains() to support
                   Reverse Power Control Logging.
03/24/00   bgc     Added extra #ifdef for FEATURE_RLP_LOGGING so that it can
                   be turned off.
03/23/00   lad     Added partial RLP frames logging under FEATURE_RLP_LOGGING.
03/16/00   na      Added support for RLP3 on the R-FCH - dsrlp_build_tx_frames
                   under FEATURE_DS_RLP3.
03/16/00   sr      Fixed a bug with the timing of action timed commands.
                   Added programming of power control step size as part
                   of TXC_PWR_CTRL_F.
02/24/00   lad     Added support for streaming DIAG and backward compatable
                   FEATURE_DIAG_V7_LOG_IFACE.
02/24/00   hrk     Added parameter offset_pwr to txtc for IS2000
                   Added parameter rl_gain_adj for Open Loop Est for IS2000
                   Added function rpc_plt_get_fch_rate to get FCH rates in PLT
02/17/00   hrk     Fixed error in commenting.
02/14/00   hrk     Temporarily commented out TX_RATE_ADJ functionality.
02/12/00   hrk     Added RLGC support.
02/10/00   hrk     Added function to get FCH rate set.
02/08/00   hrk     Modified action time processing to use existing structure
                   definitions.
                   Added action time support for Power Control Message.
                   Added calls to RLGC functions.
02/02/00   sr      Reset frame offsets to 0 and stop tx (FEATURE_PLT only)
                   during TXC_RELEASE_F
01/31/00   sr      Added action time processing and implementation of
                   txc_ho_cmd, txc_hho_ok_cmd and txc_returned_cmd.
                   Parameters related to reverse link gain adjustment and
                   power control step size haven't yet been implemented
                   Merged in Changes from GV40401:
           ks      Changed ERR_FATAL to ERR when txc_acc_probe_suspend is
                   called while the mobile is already in SUSPEND state.
           kmp     Changed txc_acc_probe_resume() definiton and prototype
                   to be for FEATURE_IS95B, not FEATURE_IS95B_ACCESS_PROBE_HO.
01/07/00   sr      In PLT we can now specify the number of preambles to be
                   sent on the reverse traffic channel
12/22/99   hrk     Added gain table for RC3 and above. Gain values are
                   now written to HW in non-PLT code.
12/20/99   rm      Removed calls to srch_pctl_rate_set for MSM5000. The power
                   control puncturing on different RC's  for Fwd link
                   is done in RXC.
11/19/99   sr      Minimum implementation of new mc-txc interface
                   including support for new TXC_SO_F, TXC_CONNECT_SO_F
                   and TXC_RELEASE_F commands
                   Added changes to support data pump and supplemental channels
11/03/99   lcc     Merged from GD4021:
           abh     Changes for updating all the Access Params after APHO
           kmp     Fixed a problem in which SMS service options were not
                   bringing up primary traffic in txc_pri().
           fc      Added FEATURE_IS95B_ACC_PRB_BKOFF_DLY for feature reduce
                   access probe backoff delay.
           fc      Added support for feature reduce access probe backoff delay.
10/20/99   lcc     Removed some unnecessary debug messages.
10/07/99   sr      Fixed frame offset fixes
10/04/99   sr      Merged in MSM5000 support from PLT archive.  Added mux layer
                   support for MSM5000.
09/13/99   abh     Changes for TXC_ACC_SUSPEND_F message processing in
                   ACC_SETUP1_S state.
08/26/99   bgc     Added FEATURE_TX_ACCESS_POWER_FIX.  See txc_acc_freq_track()
                   for details.
08/26/99   aaj     Merged Panther Branch 3.1 to Mainline
08/21/99   ks      Changed Signal MCC_ACCPR_HO_OK_SIG to MCC_TXC_APHO_OK_SIG.
08/13/99   jq      Added casting for ts_get() parameter to clear compiler
                   warnings
08/09/99   ry      Changed FEATURE_IS95B_HARD_HO to FEATURE_IS95B_MAHHO
           ry      Changed FEATURE_IS95B_CFS to FEATURE_IS95B_MAHHO
           ry      Updated txc_send_mc_frame_sig() with the following changes:
                     o Support implicit action time
                     o Generate the second MCC_TXC_FRAME_SIG only if requested
                     o Stop the transmitter only if requested
           ry      Added the following arguments to txc_register_action_time():
                     o Explicit action time indicator
                     o Generate second MCC_TXC_FRAME_SIG indicator
                     o Stop transmitter indicator
           ry      Added txc_get_time_in_1_25ms_unit()
08/06/99   nmn     Removed IS-96
07/29/99   hq      Changes for SMS SO6 and 14 support.
07/24/99   abh     Changes for APHO because of Paging Channel Loss.
07/21/99   aaj     Fixed reverse link markov sync problem by incrementing the frame number
                   passed to tx_mar() function by 1 (txc_mar_traffic() ).
07/19/99   abh     Changes for Access Probe Handoff.
07/09/99   ry      Added txc_stop_tx() and txc_set_frame_offset()
                   Added txc_send_mc_frame_sig() and txc_register_action_time()
                   as part of HHO action time fix
                   Added txc_flag_next_int() and txc_int_occurred() to support
                   IS95B HHO and CFS
           ry      Removed all frame offset processing from txc_traffic()
06/24/99   ks      Fixed a typo in txc_suspend_tx
06/17/99   kmp     Merged in the one change listed below.
06/08/99   aaj     Fixed one problem in TXC_TRAFFIC function under feature_minimize_isr_loading
                   It would cause premature exit from txc_traffic loop resulting in
       possibly trucated long traffic channel messages
                   Also corrected few comments as suggested in the code review.
06/07/99   ks      Added code to suspend/enable the Probe Transmission, this
                   is used when T72M expires, added TXC_SUSPEND_S state.
05/10/99   kmp     Merge IS-95B changes listed below.
           kmp     Added additional diagnostic messaging for Open Loop Power
                   Estimate Correction testing in txc_access_att().
           fc      Put in changes based on input from code review.
           fc      Changed the timing for setting MCC_TXC_PROBE_SIGNAL.
           fc      Put access channel message probe signalling under
                   FEATURE_IS95B_PILOT_RPT.
           fc      Added Access Channel Message Probe signalling.
           sh      Added open loop power estimate correction feature
04/30/99   abh     Increased the buffer size for the msg buffer to allow upto 255 bytes
                   of Reverse Link Messages.
04/22/99   bgc     Added in srch_finger_lock_handler as
                   FEATURE_FINGER_POLL.
04/22/99   aaj     Undone the (cast) operator changes made for armtools 2.5
04/19/99   aaj     Usage of (cast) operator in ts_get to support armtools 2.5
04/15/99   aaj     Removed MDR only timeline for the ENC Interrupt.
04/08/99   aaj     Added FEATURE_DS around a DS_ISDATA macro used in txc_traffic_init
                   under FEATURE_MINIMIZE_ISR_LOADING
03/25/99   aaj     Made some changes in TXC processing and enc isr timeline
                     1. Two different enc intr timelines for data and voice
                     2. Moved RLP TX and frame building to TXC task
                     3. Moved rxc_voc_xchg to RXC task
                     4. Optimized/changed frame number counting in encoder ISR
                   The above mentioned changes go under FEATURE_MINIMIZE_ISR_LOADING
02/26/99   ejv     Removed srch_finger_lock_handler due to interrupt loading.
01/15/99   aaj     Merged MDR branch with Panther mainline
                   Edit History for MDR branch
                        12/09/98   smp     Include mdrrlp.h instead of rlp.h for MDR
                        12/03/98   lcc     Added support for DPT service options.
11/03/98   abh     Changes for frame no. correction after frame offset change.
10/22/98   pms     Backed out nearing of variables.
10/02/98   pms     Neared variables to save ROM space.
09/14/98   bgc,br  Added 5ms callback of srch_finger_lock_handler() to
                   monitor the state of the fingers during TX on the traffic
                   channel and turn off power control when the fingers go out
                   of lock. This solves a TX power burst problem when the
                   fingers go out of lock.
08/11/98   ck      Added support for EVRC Option
04/06/98   lcc     Added comments for r1.118 (and this one).
04/01/98   lcc     Fixed CR 4994 where we try to transmit when waiting for the
                   good frames during TCI (we shouldn't be transmitting at all).
                   See revision description for more details.
03/25/98   dna     Fixed bug where 2 reserved bits before CRC were non-zero.
03/12/98   aks     Removed pre-Gemini conditionals for Encoder (Tx) interrupt
                   offset.
01/27/98   ldg     Commented usages of the unusual DS_CASE macro.
01/26/98   fkm     FEATURE_xxx Updates (via Automatic Scripts)
11/03/97   ldg     Replaced references to specific data service options with
                   a macro which represents the same information.
06/17/97   dhh     Configured for Q (T_Q).
05/29/97   na      Added correct data service option numbers - fixes service
                   option change while in traffic channel. Changes under ifdef
                   DS.
03/02/97   ptw     Fixed mishandling of txac.log_ptr that resulted in log
                   buffers being lost or re-freed.
11/14/96   jjw     Added Rate Set 2 data service capabilities.
10/21/96   gb      Fixed repeated full rate frames at very end of call.
09/26/96   gb      Reduced multiple dim&burst requests for signalling.
09/24/96   gb      Moved frame offset pending checking to background task.
09/19/96   gb      Ifdef'd vocoder logging TMOBILE to save RAM and ROM.
09/06/96   gb      Sped up erasure indicator bit timing in tc_isr
                   Restored reverse link voice logging capability.
08/16/96   gb      Power level after hard handoff includes current gain_adj.
                   Power down before hard handoff sync to frame boundary.
08/08/96   gb      Fixed frame time calculation to account for frame offset
                   assignments/handoffs in Markov calls.
07/10/96   gb      Modified timing of data flow from voc_traffic to
                   build_frame and added lookahead to un-dim next frame if
                   we no longer need it.  Made sure that we never try to
                   build message frames when active_msg is FALSE which in a
                   weird race condition will cause the High FER powerdown
                   if lots of PMRM messages are being generated in 13K.
06/05/96   dna     Modified TXC_TC_F in traffic state to change mux option.
05/09/96   gb      Do not ask vocoder for BLANK frames if not transmitting.
04/29/96   gb      Set puncturing off after turning clocks on as a precaution.
04/09/96   gb      TX now transmits all frames in a probe, filling with 0s.
04/08/96   gb      PA now turns off nicely at end of last frame in probe.
03/18/96   rah     Added VOC_FULL_ONLY full rate voice only flag. This is
                   for hearing aid users.
03/14/96   dna     Removed sign extention of nom_pwr, now done in mccsa.c.
                   Changed txc_hho() to convert nom_pwr to proper units.
03/04/96   rah     Updated CSS1 conditionals.
03/01/96   gb      Made vocoder pkt xchg callable by rxc_tc_isr.
                   Called rxc_reset_voc_init whenever we change frame offsets.
02/07/96   gb      Fixed countdown to probe when persistence test fails.
12/14/95   gb      Tightened up tx jump to hyperspace.
11/16/95   jca     Removed INTLOCK/INTFREE pair in function txc_cmd().
11/07/95   gb      Linted and removed old unused variables and code.
10/20/95   gb      Fixed internal mux1 array bounds problem by removing it.
10/06/95   gb      Changed call release to switch tx to SO_NULL.
10/03/95   dna     Modified txc_hho for case where num_preambles = 0
09/28/95   rah     Fixed forward power control.
09/18/95   jca     Fixed Markov sync problem for Gemini target.
09/07/95   gb      Moved enc_data_out from ISR to task to lessen delay.
08/25/95   gb      Inform TX of the HHO parameters num_preamble and nom_pwr.
08/23/95   ptw     Crippled vocoder logging to save a little RAM.
08/15/95   gb      Added MUX2 signalling parameter lookups for TXC.C
08/14/95   gb      Added MUX2 parameter support.
08/14/95   gb      Added new Loopback_13K service option support.
08/10/95   gb      Added New Markov Service Options and made all service
                     options CAI_SO_*.
07/27/95   gb      Fixed frame offset Markov sync bug for 8k and 13k.
07/26/95   gb      Fixed the heinous frame offset bug in txc_access_cmd.
07/20/95   rah     Changed tx interrupt offsets. Moved the txc task run to
                   the traffic ISR for GEMINI and ISS2 targets.
07/13/95   gb      Changed mux option initialization.
07/06/95   rah     Removed refs. to T_13K.
06/19/95   gb      Fixed zero frame after transmitter turn off and on.
06/16/95   jah     Fixed transmit frame time for Frame Offset > 3.
06/14/95   gb      Added support for Rate set 2 service options.
06/07/95   ptw     Removed special boost for BTU mobiles.  Not required.
05/22/95   gb      Fixed multiple cmd_done() bug in acc_msg_prep.
05/19/95   ptw     Added support for BTU mobiles.
05/09/95   rdh     Fixed zeroing of TC power after probing up, disable Boost.
05/01/95   rdh     Fixed 1dB-1/2dB mismatch in pwr_step step size units.
03/29/95   jca     Now setting signal when transitioning to traffic state.
01/04/95   gb      Fixed frame_build Markov frame for new frame timing.
12/23/94   jca     Added nom_pwr to initial transmit power calculations.
11/29/94   gb      Changed traffic_isr for new TX frame timing.
11/22/94   gb      Rhometer test functionality changed to standard TMOBILE.
11/11/94   jca     Added code to support access probe PN randomization.
                   Modified code so that TXC signals MC task at
                   completion of access attempt.  Rewrote txc_wait().
11/01/94   gb      Sector Lock test functionality added.
10/17/94   gb      Fixed traffic channel preamble crc setting to OFF.
10/13/94   jca     Fixed txac.countdown bug and other misc bug fixes per
                   Dodge Johnson's email.
06/07/94   jca     Power step size now the same for PCN and cellular modes.
05/12/94   gb      Added support for new test commands.
05/05/94   gb      Fixed power ctl disable in tx-power-boost.
04/20/94   jca     Mods for IS-95 Phase 2 upgrade.
04/04/94   gb      Use ts_get to set tx access frame timing always.
01/06/94   jah     Changed watchdog iface to use dog_report().
12/22/93   jca     Mods for IS-95 Phase 1 upgrade.
09/22/93   jca     Now processing TXC_CDMA_F command in ACC and TC states.
06/11/93   jca     Changed code so that we now power up TX portion of RF
                   h/w, delay 5 msecs, and THEN program synths as per the
                   new RF hardware spec.
10/22/92   jai     Fixed problem with double access timeout waits.
10/20/92   jai     Made fix to access probe logging queue processing to
                   prevent losing buffers from queue.
10/15/92   jai     Changed processing when vocoder returns blank while
                   expecting a dim frame.
10/06/92   jai     Added access probe logging support.
09/21/92   jai     Added ACC_ENDPROBE_S as valid state to receive
                   TXC_TIME_STOP_F command during access state.
04/20/91   rrd     Massive overhaul and upgrade to full body
04/08/91   rrd     Created

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
#include "bio.h"
#include "enc.h"
#include "task.h"
#include "tx.h"
#include "txc.h"
#include "err.h"
#include "bit.h"
#include "log.h"
#include "cai.h"
#include "tramp.h"
#include "srch_mux.h"
#include "ran.h"
#include "msg.h"
#include "ts.h"
#include "dog.h"
#include "rfm.h"
#include "ulpn.h"
#include "rxc.h"
#include "clk.h"

#ifdef FEATURE_DS
  #if  defined (FEATURE_IS95B_MDR)
    #include "mdrrlp.h"           /* MDR RLP routines */
  #else
    #include "rlp.h"
  #endif /* FEATURE_IS95B_MDR */
#endif /* FEATURE_DS */

#include "msm_drv.h"

#ifdef FEATURE_PLT
#error code not present
#endif

#include "ts.h"
#include "mctrans.h"
#include "mccdma.h"
#include "ffpc.h"
#include "rlgc.h"

#include "txccommon.h"
#include "txcmux.h"
#include "txctraffic.h"
#include "txcaccess.h"
#include "txcso.h"
#include "txcmodint.h"

#ifdef FEATURE_TX_POWER_BACK_OFF
#include "rf.h"
#endif

#if defined(FEATURE_RLP_LOGGING)
#include "dsrlp_stats.h"
#endif

#include "muxmdsp.h"
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

word txc_pch_gain;
word txc_fch_gain;
word txc_sch_gain;
word txc_dcch_gain;

#ifdef FEATURE_PLT
#error code not present
#endif //FEATURE_PLT

/* Queue definition */
q_type txc_cmd_q;

/* Timer definition */
rex_timer_type txc_rpt_timer;

/* Timer to wake Transmit task so that it can set its Watchdog task
   Report signal */
rex_timer_type  txc_rf_timer;

#define TXC_PWR_MIN 0x7F    /* Tx gain adjust minimum value */
#define TXC_PWR_BOOST (3*2) /* 1/2 dB increments for boost on full rate */

cai_data_rate_type txc_data_rate = CAI_BLANK_RATE; /* Data rate of current frame */

#ifdef FEATURE_IS2000_REL_A_CC

byte zeros[TXC_REACH_20MS_FRAME_38400_DATA_RATE_SIZE]={
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };   /* 20ms, 38400 data rate all zeros packet */

#else

byte zeros[ENC_14400_FULL_RATE_SIZE]={
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };   /* Full rate all zeros packet */

#endif // FEATURE_IS2000_REL_A_CC


/* Curent state of TXC state machine */
txc_state_type  txc_state;

/* command block for interrupt handler commands */
txc_int_cmd_type_main txc_int_cmd;

/* hold most of the global variables for the transmit task */
txc_tx_type txc_tx;

txc_frame_offset_cmd_type_pend txc_frame_offset_cmd;

/* Offset of the transmit interrupt from the frame boundary for Traffic
** Channel.
*/
byte txc_frame_offset;

#if defined (FEATURE_DIAG_V7_LOG_IFACE)
  q_type txc_log_msg_q;              /* for logging sent messages */

  #ifdef TMOBILE
#error code not present
  #endif

  /* buffers for txc_log_voc_q */
  /* 4 would be a reasonable number of log buffers,
     if logging vocoder packets actually worked well
     enough to be useful.  Since it doesn't, use only
     one buffer to save RAM. */
  #ifdef TMOBILE
#error code not present
  #endif
  /* pointer to current log buffer */
  txc_log_mux1_type *txc_mux1_ptr;

  /* buffers for txc_log_msg_q */
  txc_log_msg_type txc_log_msg_pool[ TXC_NUM_MSG_BUFS ];

#endif /* FEATURE_DIAG_V7_LOG_IFACE */


txc_so_type txc_so;

#ifdef FEATURE_IS2000_REL_A

txc_srid_ref_type  txc_srid_ref[SR_ID_MAX];

#endif /* FEATURE_IS2000_REL_A */

#ifndef FEATURE_IS2000_P2
/* Gain table based on Nominal Attribute Gains in IS2000.
** Each entry is shown in 8 bit value for reference.  Since MDSP requires
** 15 bit gain programming, each entry is left shifted by 7.
*/
txc_gain_type txc_pch_fch_sch_gain_tab[ ENC_NUM_SCH_RATES ]
                                         = {
                               /* PCH     FCH     SCH */
                                { 106<<7, 164<<7, 164<<7 },
                                { 106<<7, 164<<7, 164<<7 },
                                { 106<<7, 164<<7, 164<<7 },
                                { 106<<7, 164<<7, 164<<7 },
                                { 93<<7,  141<<7, 191<<7 },
                                { 88<<7,  116<<7, 209<<7 },
                                { 80<<7,  91<<7,  225<<7 },
                                { 71<<7,  65<<7,  236<<7 }
#ifdef FEATURE_32X_SCH_DATA_RATE
                               ,{ 61<<7,  43<<7,  244<<7 }
#endif /* FEATURE_32X_SCH_DATA_RATE */
                                           };
#else

txc_gain_type txc_pch_fch_sch_gain_tab[ ENC_NUM_SCH_RATES ]
                                         = {
                               /* PCH     FCH     DCCH    SCH */
                                { 106<<7, 164<<7, 164<<7, 164<<7 },
                                { 106<<7, 164<<7, 164<<7, 164<<7 },
                                { 106<<7, 164<<7, 164<<7, 164<<7 },
                                { 106<<7, 164<<7, 164<<7, 164<<7 },
                                { 93<<7,  141<<7, 141<<7, 191<<7 },
                                { 88<<7,  116<<7, 116<<7, 209<<7 },
                                { 80<<7,  91<<7,  91<<7,  225<<7 },
                                { 71<<7,  65<<7,  65<<7,  236<<7 }
#ifdef FEATURE_32X_SCH_DATA_RATE
                               ,{ 61<<7,  43<<7,  43<<7,  244<<7 }
#endif /* FEATURE_32X_SCH_DATA_RATE */
                                           };
#endif /* FEATURE_IS2000_P2 */

/* Gain table based on Nominal Attribute Gains in IS2000.
** Each entry is shown in 8 bit value for reference.  Since MDSP requires
** 15 bit gain programming, each entry is left shifted by 7.
** This table is used when there is no R-SCH.
*/
txc_gain_type txc_pch_fch_gain_tab[ ENC_NUM_FRAC_RATES ]
                                       = {
#ifdef FEATURE_IS2000_P2
                                { 147<<7, 208<<7, 0, 0 }, /* FULL */
                                { 180<<7, 180<<7, 0, 0 }, /* HALF */
                                { 208<<7, 147<<7, 0, 0 }, /* QTR  */
                                { 228<<7, 114<<7, 0, 0 }  /* 8TH  */
#else
                                { 147<<7, 208<<7, 0 }, /* FULL */
                                { 180<<7, 180<<7, 0 }, /* HALF */
                                { 208<<7, 147<<7, 0 }, /* QTR  */
                                { 228<<7, 114<<7, 0 }  /* 8TH  */
#endif /* FEATURE_IS2000_P2 */
                                         };

/* Adjusted R-PCH Gain Table when only R-FCH is active
** and R-SCH is not assigned */
int16 txc_pch_adj_gain_tab[ ENC_NUM_FRAC_RATES ] =
                                         {
                                              43,  /* FULL */
                                              26,  /* HALF */
                                              12,  /* QTR  */
                                              2    /* 8TH  */
                                         };
#ifdef FEATURE_IS2000_R_SCH
  int16 txc_sch_pch_adj_gain_tab[ ENC_NUM_FRAC_RATES ];
#endif /* FEATURE_IS2000_R_SCH */

#ifdef FEATURE_IS2000_R_SCH
  /* Gain Table when R-FCH and R-SCH is assigned and R-SCH is active */
  txc_gain_type txc_gain_tab[ ENC_NUM_FRAC_RATES ];

  /* Gain table when R-FCH and R-SCH is assigned but R-SCH is inactive */
  txc_gain_type txc_dtx_gain_tab[ ENC_NUM_FRAC_RATES ];

  int16 txc_dtx_pch_adj_gain_tab[ ENC_NUM_FRAC_RATES ];
#endif /* FEATURE_IS2000_R_SCH */

#ifdef FEATURE_IS2000_P2
  txc_dcch_sch_gain_type txc_dcch_sch_gain_tab[TXC_DCCH_SCH_CONFIG_MODES];
#endif /* FEATURE_IS2000_P2 */

#ifdef FEATURE_IS2000_REL_A_SVD
/* VP2 RPC new structure */
txc_fch_dcch_sch_gain_type txc_fch_dcch_sch_gain_tab[ENC_NUM_FRAC_RATES][TXC_DCCH_SCH_CONFIG_MODES];
#endif /* FEATURE_IS2000_REL_A_SVD */

/* System Action Time as specified in OTA message */
word  txc_action_time;

boolean txc_action_time_chk_enabled = FALSE;
  /* When TRUE, txc_traffic() should check if the action time is up
   * while processing "TXC_INT_SIG". If the action time is up,
   * TXC will send "MCC_TXC_FRAME_SIG" to MCC
   */

boolean txc_action_time_stop_tx_enabled;
  /* When TRUE, TXC will command the transmitter to be shut down at
   * action time, i.e. while the first MCC_TXC_FRAME_SIG is generated
   */


#if defined(FEATURE_IS95B_MAHHO) || defined (FEATURE_GPS_MODE)
  boolean txc_next_int_occurred;
#endif /* FEATURE_IS95B_MAHHO || FEATURE_GPS_MODE */

/* PCG value for bin0 ENC ISRs is set to default value. */
byte txc_slot0_isr_pcg = TXC_SLOT0_ISR_PCG_NUM;

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION TXC_CMD_DONE

DESCRIPTION
  Dispose of a command buffer when we're done with it.  Depending of
  the contents of the buffer, this will involve placing it on a free
  queue if a free queue is designated,  and if a tcb is given,  the
  appropriate task will be notified.

DEPENDENCIES
  None.
RETURN VALUE
  Just returns the status passed to it
SIDE EFFECTS
  Could signal another task
===========================================================================*/
void txc_cmd_done(
  txc_cmd_type   *cmd_ptr,         /* buffer to dispose of */
  word          status            /* set status of buffer */
)
{
  cmd_ptr->hdr.status = (txc_status_type) status;

  cmd_done( &cmd_ptr->hdr.cmd_hdr );

}

#ifdef FEATURE_PLT
#error code not present
#endif

/*===========================================================================

FUNCTION TXC_SUSPEND_TX

DESCRIPTION
  This functions enables/Disables the Transmitter and enables/disables the
  transmission of Access probes.

DEPENDENCIES
  Depends on the current state of the TX task.

RETURN VALUE
  None.

SIDE EFFECTS
  This function is called when the T72 fade timer expires to temporarily
  enable/disable the transmission of the Access Probes.

  Note that there are two mechanisms to suspend the probing. First is by
  calling this function and second mechanism is by sending a SUSPEND message
  to TXC. The difference between these two mechanism(other than one being
  message level interface and another being message level interface), that
  the message level interface waits for TA timer to expire and then responds
  back to MC with ACCPR_HO_OK sig.  Function based interface does not do so.

===========================================================================*/
void txc_suspend_tx (boolean cntrl_flag)
{
  boolean prev_stop_probes;

  prev_stop_probes = txac.stop_probes;
  txac.stop_probes = cntrl_flag;

  /* If the Transmission was suspended during Access attempt, set the
  ** TXAC state properly to resume the transmission of probes */

  if((!txac.stop_probes) && (prev_stop_probes) && (txc_state == TXC_ACC_ATT_S))
  {
    txc_acc_probe_resume();
  }
}


/*===========================================================================

FUNCTION TXC_RPT_VOC_RATE

DESCRIPTION
  Reports the vocoder rate to the calling routine.

DEPENDENCIES
  None.

RETURN VALUE
  Current frame data rate.

SIDE EFFECTS
  None.

===========================================================================*/
cai_data_rate_type txc_rpt_voc_rate( void )
{
  return (txc_data_rate);
}


/*===========================================================================

FUNCTION TXC_CLR_MSGS

DESCRIPTION
  Move any pending messages in the messages buffer over to the free
  queue,  effectively erasing any queued messages.

DEPENDENCIES
  None.
RETURN VALUE
  None.
SIDE EFFECTS
  None.
===========================================================================*/
void txc_clr_msgs( void )
{

  if( txc_tx.active_msg ) {               /* if we're working on a message */
    txc_cmd_done( txc_tx.buf.cmd_ptr, TXC_DONE_S );
    txc_tx.active_msg = FALSE;                     /* and null the pointer */
    txc_tx.delay = TXC_NO_CONF;                   /* no confirmation delay */
  }

}


/*===========================================================================

FUNCTION TXC_WAIT

DESCRIPTION
  Wait for the watchdog and for any other signals that might get set.
  Return if one of the other signals is set.  Always kick the watchdog
  whenever we wake up.

DEPENDENCIES
  None.

RETURN VALUE
  Returns the signals for the current task.

SIDE EFFECTS
  None.

===========================================================================*/
rex_sigs_type txc_wait
(
  word requested_mask
    /* Mask of signals to wait for */
)
{
  rex_sigs_type queue_mask;        /* Mask of signals indicating queue status */
  rex_sigs_type rex_signals_mask;  /* Mask of signals returned by rex_get_sigs() */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (requested_mask == 0)
  {
    /* No sigs to wait for --> Kick the watchdog and return to caller */
    dog_report( DOG_TX_RPT );
    return (0);
  }

  /* Initialize temporary mask holder for queue information */
  queue_mask = 0;

  /* --------------------------------------------------------------------
  ** Check TXC command queue: If the request mask contains a check for
  ** TXC_CMD_Q_SIG then we should check txc_cmd_q, and set in queue_mask
  ** an indication that something is waiting to be serviced on the queue.
  ** The rex signal is also cleared regardless of the queue count.  This
  ** is to cover the case where there is a signal set but nothing on the
  ** queue.
  ** -------------------------------------------------------------------- */
  if (requested_mask & TXC_CMD_Q_SIG)
  {
    (void) rex_clr_sigs( rex_self(), TXC_CMD_Q_SIG );

    if (q_cnt( &txc_cmd_q ))
    {
      queue_mask |= TXC_CMD_Q_SIG;
    }
  }

  /* Get current state of rex signals */
  rex_signals_mask = rex_get_sigs( rex_self() );

  /* --------------------------------------------------------------------
  ** Loop until the current signals from rex (including the queue signals
  ** faked due to an item on a queue, if any) match with one or more of
  ** the requested signals.
  ** -------------------------------------------------------------------- */
  do
  {
    /* Kick the watchdog */
    dog_report(DOG_TX_RPT);
    (void) rex_set_timer( &txc_rpt_timer, DOG_TX_RPT_TIME );

    /* Perform the 'real' wait (if needed) */
    if (((rex_signals_mask | queue_mask) & requested_mask) == 0)
    {
      rex_signals_mask = rex_wait( requested_mask | TXC_RPT_TIMER_SIG );
    }

    /* ---------------------------------------------------------------
    ** Make sure that if we got a queue signal, that there is actually
    ** an item on the queue indicated
    ** --------------------------------------------------------------- */
    if (rex_signals_mask & TXC_CMD_Q_SIG)
    {
      (void) rex_clr_sigs( rex_self(), TXC_CMD_Q_SIG );

      if (q_cnt( &txc_cmd_q ) == 0) {  /* No elements in queue ? */
        rex_signals_mask &= ~TXC_CMD_Q_SIG;  /* Turn off bit */
      }
    }

    /* Do while mask requested does not match current mask */
  } while (((rex_signals_mask | queue_mask) & requested_mask) == 0);

  /* Kick watchdog one last time on the way out */
  dog_report( DOG_TX_RPT );

  (void) rex_set_timer( &txc_rpt_timer, DOG_TX_RPT_TIME );

  return ((rex_signals_mask | queue_mask));

} /* end txc_wait() */


/***************************************************************************/
/*                         CDMA State Procedures                           */
/***************************************************************************/

/*===========================================================================

FUNCTION TXC_CDMA_ENC_SETUP

DESCRIPTION
  Set up the encoder / interleaver / deinterleaver for CDMA

DEPENDENCIES
  Assume interleaver already hard reset
RETURN VALUE
  None
SIDE EFFECTS
  None
===========================================================================*/
void txc_cdma_enc_setup(void)
{
  qword high_bit;

  /* this is special - all this chip stuff would normally be done in the */
  /* interrupt handler, but we don't know if the interrupt handler for   */
  /* the start state will be up,  so do it here,  since we can only get  */
  /* to this state from START state                                      */
  /* Send a soft reset to the circuit */

  enc_set_mode( ENC_CDMA_MODE );  /* init the encoder for CDMA */

  /* Set bozo mode if switch 2 is on !!! */
  enc_bozo_mode( (boolean) BIO_GET_SW( BIO_SW2_M ) );


  if (BIO_GET_SW( BIO_SW6_M )) {       /* If dipswitch 6 is ON */
    /* fake it out - set up the interleaver before we know what the */
    /* PN codes really are - this is so we can see an output CDMA */
    /* waveform before we acquire anything... */

    enc_tx_clk_enable( TRUE );

    enc_iq_pn_state( 0x4000, 0x4000 ); /* Load I and Q with MSB 1      */

    qw_set( high_bit, 0x200L, 0L );    /* Init quad word, set bit 41 [0..41] */
    enc_user_pn_state( high_bit );     /* Load User PN state */
    enc_user_pn_mask( high_bit );      /* Load User PN mask  */

    /*
    ** Load the TX long code state.
    ** Load the TX long code mask.
    */
    enc_stmr_cmd( ENC_STMR_CMD_LC_STATE_80MS_V |
                  ENC_STMR_CMD_LC_MASK_20MS_V
                );

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


    enc_tx_enable (TRUE);

    srch_mux_set_tx_adj( 0x3C );  /* set TX adjust for for reasonable transmit
                                     level */
  }

  /* if switch 1 then do IF loopback */
  if( BIO_GET_SW( BIO_SW1_M )) {
    enc_tx_if_loopback_init();
  }

}


/*===========================================================================

FUNCTION TXC_CDMA_ENC_SHUTDOWN

DESCRIPTION
  Shut down the encoder / interleaver / deinterleaver & PA for CDMA

DEPENDENCIES
  None
RETURN VALUE
  None
SIDE EFFECTS
  None
===========================================================================*/
void txc_cdma_enc_shutdown()
{
  /* Always delay at least 1 frame before traffic/access teardown */
  {
    MSG_HIGH ("Delay shutdown enc clocks by 1 frame",0,0,0);

    /* kick the dog before using the report timer */
    dog_report( DOG_TX_RPT );

    /* Wait for at least a frame before turning off clocks */
    (void) rex_timed_wait(
             TXC_RPT_TIMER_SIG, &txc_rpt_timer, 20+CLK_MS_PER_TICK );
  }

  /* FIRST set interrupt handler to null so it won't bother us */
  tramp_set_isr( TRAMP_TX_ISR, NULL);

  /* Switch back to rate set 1 for accesses */
  enc_fch_rc( ENC_FCH_RC_1 );
  enc_sch_rc( ENC_SCH_RC_3 );

  txc_tx.tx_on = FALSE;

  if ( srch_mux_owns_tx() )
  {
    MSG_HIGH ("1X has pri chain, shutdown enc clks",0,0,0);

    /* Ensure TX_ON is off */
    HWIO_OUTM( MODEM_PA_ON_CTL,
               HWIO_FMSK( MODEM_PA_ON_CTL, TX_ON_EN),
               0 );

    /* Wait sufficient time for RTR DA_OUT to be off before PA_ON goes low */
    clk_busy_wait( 20 );

  enc_tx_enable ( FALSE );

  /* Force the PA off.  The TX_PUNCT was set "off" during the last Tx interrupt
  */
  enc_acp_pa_ctl(FALSE);

#ifndef T_IO_CARD
  /* kick the dog before using the report timer */
  dog_report( DOG_TX_RPT );

  /* Make sure power amp is off */
  rfm_tx_disable(RFCOM_TRANSCEIVER_0);

  /* Wait for at least a frame before turning off clocks */
  (void) rex_timed_wait(
           TXC_RPT_TIMER_SIG, &txc_rpt_timer, 20+CLK_MS_PER_TICK );
#endif /* T_IO_CARD */


  enc_set_mode( ENC_SLEEP_MODE);   /* Turn off clocks */
  enc_tx_clk_enable( FALSE ); /* Turn off TX clocks */
  }
  else
  {
    MSG_HIGH("MUX does not own TX, exit without turn off TX ",0,0,0);
  }

  /* kick the dog before resetting the report timer */
  dog_report( DOG_TX_RPT );
  (void)rex_set_timer( &txc_rpt_timer, DOG_TX_RPT_TIME );

} /* txc_cdma_enc_shutdown */


/*===========================================================================

FUNCTION TXC_EXIT

DESCRIPTION
  This function performs processing necessary for exiting CDMA Transmit
  subtask.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void txc_exit (void )
{

    /* Turn off clocks, pa, etc. */
  #ifdef FEATURE_RL_JUMP_ENHANCEMENT
    txc_tx_shutdown();
  #else
    txc_cdma_enc_shutdown();
  #endif /* FEATURE_RL_JUMP_ENHANCEMENT */

  /* clear timer */
  ( void )rex_clr_timer( &txc_rpt_timer );

  /* clear signals */
  ( void )rex_clr_sigs( &tx_tcb,  0xFFFF );

  if (log_status(LOG_REV_PCTRL_C))
  {
    txc_rpctrl_send_log();
  }
#ifdef FEATURE_LOG_PC_INFO_2
  if (log_status(LOG_RPC_INFO_C))
  {
    txc_rpc_info_send_log();
  }
#endif /* FEATURE_LOG_PC_INFO_2 */
#ifdef FEATURE_IS2000_REL_A
  if (log_status(LOG_RDCH_FRAME_INFO_C))
  {
    txc_rdch_frame_info_send_log();
  }
#else
  if (log_status(LOG_REV_FRAME_TYPES_C))
  {
    txc_frame_types_send_log();
  }
#endif /* FEATURE_IS2000_REL_A */
#ifdef FEATURE_TX_POWER_BACK_OFF
  /* Channel configuration has changed, we need to run the RF PA backoff algorithm
  to reset all the PA power*/
  txc_update_chan_config_to_rf(FALSE);
#endif /* FEATURE_TX_POWER_BACK_OFF */
} /* txc_exit */


/*===========================================================================

FUNCTION TXC_CDMA

DESCRIPTION
  CDMA state of tx state machine.

DEPENDENCIES
  None
RETURN VALUE
  Next state to process.
SIDE EFFECTS
  None
===========================================================================*/
txc_state_type txc_cdma(
  txc_state_type  state                  /* current txc_state - TXC_CDMA_S */
)
{
  txc_cmd_type *cmd_ptr;              /* command from queue */

  do {

    (void)rex_clr_sigs( &tx_tcb, TXC_CMD_Q_SIG );  /* clear queued signal */

    /* get all items off the queue */
    while ( (cmd_ptr = (txc_cmd_type*) q_get( &txc_cmd_q )) != NULL ) {

      /* interpret the command from the queue */
      switch ( cmd_ptr->hdr.command ) {

        case TXC_PC_F:            /* switch to paging/access state */
          txc_access_idl_init( state );
          state = TXC_ACC_IDLE_S;
          txc_cmd_done( cmd_ptr, TXC_DONE_S);
          break;

        case TXC_EXIT_F:
          /* exit cdma mode */
          txc_exit();  /* perform exit processing */
          state = TXC_EXIT_S;
          txc_cmd_done( cmd_ptr, TXC_DONE_S );
          break;
#if defined(FEATURE_VOIP_1X_HANDOFF) || defined(FEATURE_UMTS_1X_HANDOVER_1XMSM)
        case TXC_TT_F:
          txc_tt_init();
          /* enter Timing Transfer State*/
          state = TXC_TT_S;
          txc_cmd_done( cmd_ptr, TXC_DONE_S );
          break;
#endif /* FEATURE_VOIP_1X_HANDOFF and FEATURE_UMTS_1X_HANDOVER_1XMSM */
        default:                /* bad command */
          /* dispose of command buffer */
          txc_cmd_done( cmd_ptr, TXC_BADCMD_S );
          break;

      }  /* switch */
    } /* Items off of queue */

    /* wait for command on the queue */
    if ( state == TXC_CDMA_S ) {
       (void)txc_wait( TXC_CMD_Q_SIG );
    }

  } while ( state == TXC_CDMA_S ); /* until done with this state */

  return( state );

} /* txc_cdma */

/*===========================================================================

FUNCTION TXC_RPCTRL_SEND_LOG

DESCRIPTION
  Terminate and send the accumulated reverse power control frames log.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  A reverse power control log may be sent.

===========================================================================*/
void txc_rpctrl_send_log(void)
{
  if (txc_log_rpctrl_buf.hdr.len != 0)
  {
    /* Will send log if enabled. */
    (void) log_submit((log_type *) &txc_log_rpctrl_buf);

    /* Flag to init start of log */
    txc_log_rpctrl_buf.hdr.len = 0;
  }

} /* txc_rpctrl_send_log */



/*===========================================================================

FUNCTION TXC_RPCTRL_LOG_INIT

DESCRIPTION
  This function initializes the Reverse Power Control log subrecord, as well as the
  pointer to it and the frame entry, in the log buffer.  If necessary, the log is
  sent and a new one is generated.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  A reverse power control log may be sent.

===========================================================================*/
log_rev_pctrl_frame_type * txc_rpctrl_log_frame_init(void)
{
  log_rev_pctrl_frame_type *frm_ptr = NULL;
  log_ded_rev_chan_set_type chan_set_mask;
  static boolean txc_sch_log_data = FALSE;

  /* If logging is enabled. */
  if (log_status(LOG_REV_PCTRL_C)
#ifdef FEATURE_LOG_PC_INFO_2_EXCLUSIVE
      /* The REV_PCTRL log message is a subset of the RPC_INFO log */
      /* message, so only log it if RPC_INFO is not being logged.   */
      && !log_status(LOG_RPC_INFO_C)
#endif /* FEATURE_LOG_PC_INFO_2_EXCLUSIVE */
     )
  {
    /* If we reached accumulation threshold, send it. */
    if (txc_log_rpctrl_buf.hdr.len >= LOG_REV_PCTRL_BUF_THRESHOLD) //lint !e413 FPOS inside macro
    {
      txc_rpctrl_send_log();
    }

    if (txc_log_rpctrl_buf.hdr.len == 0) /* Need to re-init */
    {
      txc_log_rpctrl_buf.hdr.len = FPOS(log_rev_pctrl_type, var_len_buf);//lint !e734 !e413 FPOS

      txc_log_rpctrl_buf.hdr.code = LOG_REV_PCTRL_C;

      /* First frame, log time stamp. */
      ts_get((unsigned long *) txc_log_rpctrl_buf.hdr.ts);

      /*------------------------------
        Fill in beginning of record
      ------------------------------*/
      /* Frame offset specified by base station */
      txc_log_rpctrl_buf.frame_offset = txc_frame_offset;

      txc_log_rpctrl_buf.band_class = cdma.band_class; /* From MC */

      /* RC for each active channel */
      #ifdef FEATURE_IS2000_P2
      if ( txc_so.rev_fch.included )
      #endif  // FEATURE_IS2000_P2
      {
        txc_log_rpctrl_buf.rev_ch_rc.r_fch_rc  = txc_so.rev_fch.rc;
      }
      #ifdef FEATURE_IS2000_P2
      else
      {
        txc_log_rpctrl_buf.rev_ch_rc.r_fch_rc  = 0;
      }

      if ( txc_so.rev_dcch.included )
      {
        txc_log_rpctrl_buf.rev_ch_rc.r_dcch_rc  = txc_so.rev_dcch.rc;
      }
      else
      #endif  // FEATURE_IS2000_P2
      {
        txc_log_rpctrl_buf.rev_ch_rc.r_dcch_rc  = 0;
      }

#ifdef FEATURE_IS2000_R_SCH
      if ( txtc.sch_on )
      {
        txc_log_rpctrl_buf.rev_ch_rc.r_sch0_rc = txtc.r_sch_rc;
      }
      else
#endif /* FEATURE_IS2000_R_SCH */
      {
        txc_log_rpctrl_buf.rev_ch_rc.r_sch0_rc = 0;
      }

      txc_log_rpctrl_buf.rev_ch_rc.r_sch1_rc = 0; /* Not supported! */

      /* Pilot gating rate not used until the control hold state is supported */
      txc_log_rpctrl_buf.pilot_gating_rate = 0;

      /* Power Control step size */
      txc_log_rpctrl_buf.step_size = txtc.pwr_ctrl_step_size;

      /* We no longer have a valid sub-record. */
      txc_log_rpctrl_buf.sr_cnt = 0;
      txc_log_rpctrl_sr_ptr     = NULL;

    }
    else
    {
      /* If a new subrecord is needed, i.e. the channel set has changed,
      ** check for the condition here.
      */
      chan_set_mask.mask = 0;

#ifdef FEATURE_IS2000_P2
      chan_set_mask.chan_set.r_dcch = FALSE;
#endif /* FEATURE_IS2000_P2 */
      chan_set_mask.chan_set.r_fch  = FALSE;

#ifdef FEATURE_IS2000_P2
      if ( txc_so.rev_dcch.included )
      {
        chan_set_mask.chan_set.r_dcch = TRUE;
      }

      if ( txc_so.rev_fch.included )
#endif /* FEATURE_IS2000_P2 */
      {
        chan_set_mask.chan_set.r_fch  = TRUE;
      }

      /* Check if SCH is active. */
      chan_set_mask.chan_set.r_sch0 = (txc_sch_log_data) ? TRUE : FALSE;

      if (chan_set_mask.mask != txc_log_rpctrl_sr_ptr->ch_set_mask.mask)
      {
        /* End this sub-record and start a new one below. */
        txc_log_rpctrl_sr_ptr = NULL;
      }
    }

    /* Need to init a new subrecord? */
    if (txc_log_rpctrl_sr_ptr == NULL)
    {
      /* Init sub-record pointer by finding the offset into the variable length buf.
      ** TXC_RPCTRL_LOG_OFFSET is calculated from the length of the log packet.
      */
      /* See structure definition for explaination of buf_ref */
      txc_log_rpctrl_sr_ptr = (log_rev_pctrl_subrecord_type *)
        &txc_log_rpctrl_buf.var_len_buf.buf_ref[TXC_RPCTRL_LOG_OFFSET]; //lint !e413 FPOS inside macro

      /* Increment sub-record count. */
      txc_log_rpctrl_buf.sr_cnt++;

      /* Init all bits in mask, and len_per_frame */
      txc_log_rpctrl_sr_ptr->ch_set_mask.mask = 0;
      txc_log_rpctrl_sr_ptr->len_per_frame =
        FPOS(log_rev_pctrl_frame_type, tx_ch_pwr);//lint !e734 FPOS

#ifdef FEATURE_IS2000_P2
      txc_log_rpctrl_sr_ptr->ch_set_mask.chan_set.r_dcch = FALSE;
#endif /* FEATURE_IS2000_P2 */
      txc_log_rpctrl_sr_ptr->ch_set_mask.chan_set.r_fch  = FALSE;

#ifdef FEATURE_IS2000_P2
      if ( txc_so.rev_fch.included )
#endif /* FEATURE_IS2000_P2 */
      {
        txc_log_rpctrl_sr_ptr->ch_set_mask.chan_set.r_fch  = TRUE;

        /* Increase length per frame value. */
        txc_log_rpctrl_sr_ptr->len_per_frame +=
          FSIZ(log_rev_pctrl_frame_type, tx_ch_pwr[0]);
      }

#ifdef FEATURE_IS2000_P2
      if ( txc_so.rev_dcch.included )
      {
        txc_log_rpctrl_sr_ptr->ch_set_mask.chan_set.r_dcch = TRUE;

        /* Increase length per frame value. */
        txc_log_rpctrl_sr_ptr->len_per_frame +=
          FSIZ(log_rev_pctrl_frame_type, tx_ch_pwr[0]);
      }
#endif /* FEATURE_IS2000_P2 */

      if (txc_sch_log_data)
      {
        /* R-SCH0 is active. */
        txc_log_rpctrl_sr_ptr->ch_set_mask.chan_set.r_sch0 = TRUE;

        /* Increase length per frame value. */
        txc_log_rpctrl_sr_ptr->len_per_frame +=
          FSIZ(log_rev_pctrl_frame_type, tx_ch_pwr[0]);
      }

      /* Init frame count */
      txc_log_rpctrl_sr_ptr->frame_cnt = 0;

      /* Update packet length */
      txc_log_rpctrl_buf.hdr.len += FPOS(log_rev_pctrl_subrecord_type, var_len_buf);//lint !e734 !e413 FPOS

    }

    /* Increment frame count */
    txc_log_rpctrl_sr_ptr->frame_cnt++;

    /* Init frame pointer by finding the offset into the variable length buf.
    ** TXC_RPCTRL_LOG_OFFSET is calculated from the length of the log packet.
    */
    frm_ptr = (log_rev_pctrl_frame_type *)
      &txc_log_rpctrl_buf.var_len_buf.buf_ref[TXC_RPCTRL_LOG_OFFSET]; //lint !e413 FPOS inside macro
  }
  else
  {
    txc_log_rpctrl_buf.hdr.len = 0; /* Flag that this record has stopped */
  }
  /* reset the flag for the last TX frame of the call */
  txc_sch_log_data  = txtc.sch_on;
  return frm_ptr;

} /* txc_rpctrl_log_frame_init */


/***************************************************************************/
/*                         Switch Between States                           */
/***************************************************************************/

/*===========================================================================

FUNCTION TXC_POWERUP_INIT

DESCRIPTION
  Initializes timers and queues for CDMA Transmit task processing.

DEPENDENCIES
  None.
RETURN VALUE
  Never returns.
SIDE EFFECTS
  None.
===========================================================================*/
void txc_powerup_init()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* initialize timers */
  rex_def_timer (&txc_rpt_timer, &tx_tcb, TXC_RPT_TIMER_SIG );
  rex_def_timer (&txc_rf_timer, &tx_tcb, TXC_RF_TIMER_SIG );

  /* initialize all the msg queues and buffers */
  (void)q_init( &txc_cmd_q );

#if defined(FEATURE_RLP_LOGGING)
  /* Initialize RLP frames logging buffer */
  dsrlp_log_tx_init_frm_buf();
#endif

#if !defined (FEATURE_DIAG_V7_LOG_IFACE)
  /* initialize txc_log_mux_buf */
  txc_log_mux_buf.count = 0;
  txc_log_mux_buf.entry.hdr.len = FSIZ(log_r_mux1_type, hdr);

#else
  (void)q_init( &txc_log_mux1_q );
  (void)q_init( &txc_log_msg_q );
#ifdef TMOBILE
#error code not present
#endif
  (void)q_init( &txc_log_acc_q );
  txc_tx.active_msg = FALSE;               /* no current message */
  txc_tx.delay = TXC_NO_CONF;              /* no confirmation delay */

  /* initialize frame rate logging bufs and place on txc_log_mux1_q */
  for ( i = 0; i < TXC_NUM_MUX1_BUFS; i++ ) {
    txc_log_mux1_pool[ i ].desc.q_ptr   = &txc_log_mux1_q;
    txc_log_mux1_pool[ i ].desc.log_ptr = &txc_log_mux1_pool[ i ].entry.hdr;
    txc_log_mux1_pool[ i ].entry.hdr.len =
                                    sizeof( txc_log_mux1_pool[i].entry );
    txc_log_mux1_pool[ i ].entry.hdr.code = LOG_R_MUX1_C;

    q_put ( &txc_log_mux1_q,
            q_link ( &txc_log_mux1_pool[ i ],
                     &txc_log_mux1_pool[ i ].desc.link ) );
  }

  txc_mux1_ptr = NULL;   /* initialize buffer pointer to null */

  /* initialize message logging bufs and place on txc_log_msg_q */
  for ( i = 0; i < TXC_NUM_MSG_BUFS; i++ ) {
    txc_log_msg_pool[ i ].desc.q_ptr   = &txc_log_msg_q;
    txc_log_msg_pool[ i ].desc.log_ptr = &txc_log_msg_pool[ i ].entry.hdr;

    q_put ( &txc_log_msg_q,
            q_link ( &txc_log_msg_pool[ i ],
                     &txc_log_msg_pool[ i ].desc.link ) );
  }

#ifdef TMOBILE
#error code not present
#endif

  for( i=0; i< TXC_NUM_ACC_INFO_BUFS; i++ )
  {
    txc_log_acc_info_pool[i].desc.q_ptr = &txc_log_acc_q;
    txc_log_acc_info_pool[i].desc.log_ptr = &txc_log_acc_info_pool[i].acc.hdr;
    txc_log_acc_info_pool[i].acc.hdr.len =
        sizeof( txc_log_acc_info_pool[i].acc );
    txc_log_acc_info_pool[i].acc.hdr.code = LOG_ACC_INFO_C;
    q_put( &txc_log_acc_q,
           q_link( &txc_log_acc_info_pool[i],
                   &txc_log_acc_info_pool[i].desc.link ) );
  }
#endif /* !FEATURE_DIAG_V7_LOG_IFACE */

  /* -----------------------------------
  ** No TXC_FRAME_OFFSET command pending
  ** ----------------------------------- */
  txc_frame_offset_cmd.pending = FALSE;

  txcmod_int_init();

} /* txc_powerup_init */


/*===========================================================================

FUNCTION TXC_SUBTASK

DESCRIPTION
  Transmit subtask to process CDMA.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void txc_subtask (void )
{

  txc_cmd_type *cmd_ptr;          /* pointer to command from txc_cmd_q */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  (void)rex_clr_sigs(&tx_tcb, TXC_CMD_Q_SIG );
  if( (cmd_ptr = (txc_cmd_type*) q_get(&txc_cmd_q)) == NULL )
  {
    ERR_FATAL("TXC subtsk activ err",0,0,0 );
  }
  else
  {
    /* To correspond to the new search inactive state, the first command from MC
       MC to TXC can be the TXC_EXIT_F;
       If this is the case, the txc FSM is put at the TXC_EXIT_S and return w/o
       doing anything further. */
    if( !((cmd_ptr->hdr.command == TXC_CDMA_F) ||
        (cmd_ptr->hdr.command == TXC_EXIT_F)))
    {
      ERR_FATAL("TXC subtsk activ err",0,0,0 );
    }
    else
    {
      cmd_ptr->hdr.status = TXC_DONE_S;
      cmd_done( &cmd_ptr->hdr.cmd_hdr );
      /* Begin CDMA subtask processing */
      if(cmd_ptr->hdr.command == TXC_EXIT_F)
      {
        txc_state = TXC_EXIT_S;
      }
      else //== TXC_CDMA_F
      {

        txc_state = TXC_CDMA_S;           /* initialize state machine state */
        txc_cdma_enc_setup();             /* initialize encoder for CDMA */

        /* main state machine loop */
        do
        {
          switch ( txc_state )
          {
          case TXC_CDMA_S:             /* to cdma state */
            txc_state = txc_cdma( txc_state);
            break;
          case TXC_ACC_IDLE_S:         /* access channel idle */
            txc_state = txc_access_idl(txc_state);
            break;
          case TXC_ACC_JUMP_S:         /* access channel jump to hyperspace */
            txc_state = txc_hyperspace(txc_state);
            break;
#if defined(FEATURE_VOIP_1X_HANDOFF) || defined(FEATURE_UMTS_1X_HANDOVER_1XMSM)
          case TXC_TT_S:        /* Handle the timing transfer from DO to 1X */
            txc_state = txc_tt_state(txc_state);
            break;
#endif /* FEATURE_VOIP_1X_HANDOFF and FEATURE_UMTS_1X_HANDOVER_1XMSM */
          case TXC_ACC_ATT_S:            /* Access attempt transmission */
            muxmdsp_wr_passThruEnable(TRUE);
            txc_state = txc_access_att(txc_state);
            muxmdsp_wr_passThruEnable(FALSE);
            break;
          case TXC_TRAFFIC_S:          /* traffic channel */
            muxmdsp_wr_passThruEnable(TRUE);
            txc_state = txc_traffic(txc_state);
            muxmdsp_wr_passThruEnable(FALSE);
            break;

#ifdef FEATURE_RL_JUMP_ENHANCEMENT
          case TXC_HOLD_ENC_CLK_S: /* Leave Encoder clocks ON for now */
            txc_state = txc_hold_enc_clk (txc_state);
            break;
#endif /* FEATURE_RL_JUMP_ENHANCEMENT */

          default:                    /* should never get here */
            ERR( "Bad TX state %d", txc_state, 0, 0 );
            txc_state = TXC_EXIT_S;
            break;
          } /* switch */
        } while ( txc_state != TXC_EXIT_S );  /* never exit this loop... */
      } /* if (cmd_ptr->hdr.command == TXC_EXIT_F ) */
    } /* if (cmd_ptr->hdr.command != TXC_CDMA_F ) */
  } /* if( (cmd_ptr = (txc_cmd_type*) q_get(&txc_cmd_q)) == NULL ) */
}  /* txc_subtask */

/***************************************************************************/
/*              Interface for the other tasks to call TX                   */
/***************************************************************************/

/*===========================================================================

FUNCTION TXC_CMD

DESCRIPTION
  This is how the outside world interfaces with the tx task.  Call this
  procedure with the address of the txc_cmd_type and the command will be
  placed on the txc_cmd_q

DEPENDENCIES
  None.
RETURN VALUE
  None.
SIDE EFFECTS
  None.
===========================================================================*/
void txc_cmd
(
  txc_cmd_type *cmd_ptr                    /* the command to queue up */
)
{
  INTLOCK();
  (void) q_link( cmd_ptr, &cmd_ptr->hdr.cmd_hdr.link );  /* init link */
  cmd_ptr->hdr.status = TXC_BUSY_S;        /* mark it as queued */

  q_put( &txc_cmd_q, &cmd_ptr->hdr.cmd_hdr.link ); /* and queue it */

  (void)rex_set_sigs( &tx_tcb, TXC_CMD_Q_SIG ); /* signal the transmit task */
  INTFREE();
}

/*===========================================================================

FUNCTION TXC_SET_FRAME_OFFSET

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void txc_set_frame_offset (
  byte frame_offset
)
{
  /* start computing frame number from sys time in enc isr */
  after_fo_handoff_count_isrs = NO_OF_ISRS_AFTER_FO;

  txc_frame_offset = frame_offset;
}


/*===========================================================================

FUNCTION TXC_GET_TIME_IN_1_25MS_UNIT

DESCRIPTION
  This function get the current time in 1.25 ms unit and factor in the
  frame offset as well

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

dword txc_get_time_in_1_25ms_unit( void )
{
  qword ts_time;

  ts_get( ts_time );
  (void) qw_div_by_power_of_2( ts_time, ts_time, 16 );
  qw_dec( ts_time, ( dword ) txc_frame_offset );
  qw_inc( ts_time, 1L );

  return( qw_lo( ts_time ) );
} /* mcctcho_get_time_in_1_25ms_unit */


#if defined(FEATURE_IS95B_MAHHO) || defined(FEATURE_GPS_MODE)
/*===========================================================================

FUNCTION TXC_FLAG_NEXT_INT

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void txc_flag_next_int ( void )
{
  txc_next_int_occurred = FALSE;
}


/*===========================================================================

FUNCTION TXC_INT_OCCURRED

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

boolean txc_int_occurred ( void )
{
  return( txc_next_int_occurred );
}
#endif /* FEATURE_IS95B_MAHHO || FEATURE_GPS_MODE */

/*===========================================================================

FUNCTION TXC_GET_FCH_RATE_SET

DESCRIPTION
  Returns the rate set for FCH.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

enc_rate_set_type txc_get_fch_rate_set ( void )
{
  return ENC_RC_TO_RATE_SET( txc_so.rev_fch.rc );
}

/*===========================================================================

FUNCTION TXC_SET_PCH_FCH_GAIN

DESCRIPTION
  Sets the gain values in txc_pch_fch_gain_tab for the specified rate

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void txc_set_pch_fch_gain
(
 byte          rate,
 txc_gain_type gain,
 int16         pch_adj_gain,
 txc_chan_status_type chan_status
)
{
   if (chan_status == TXC_FCH_ONLY)
   {
      txc_pch_fch_gain_tab [ rate ] = gain;
      txc_pch_adj_gain_tab [ rate ] = pch_adj_gain;
   }
#ifdef FEATURE_IS2000_R_SCH
   else if (chan_status == TXC_SCH_ACTIVE)
   {
      txc_gain_tab[ rate ] = gain;
      txc_sch_pch_adj_gain_tab[ rate ] = pch_adj_gain;
   }
   else if (chan_status == TXC_SCH_DTX)
   {
      txc_dtx_gain_tab[ rate ] = gain;
      txc_dtx_pch_adj_gain_tab [ rate ] = pch_adj_gain;
   }
#endif /* FEATURE_IS2000_R_SCH */
}

/*===========================================================================

FUNCTION TXC_SET_FPC_MODE

DESCRIPTION
  Sets the value of FPC_MODE in txc_so.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void txc_set_fpc_mode
(
 fpc_mode_type fpc_mode
)
{
   /* save current FPC_MODE value */
   txc_so.fpc_mode = fpc_mode;

   switch (txc_so.fpc_mode)
   {


     case FPC_MODE_PRI_EIB:
#ifdef FEATURE_IS2000_REL_A
     case FPC_MODE_PRI_QIB:
     case FPC_MODE_PRI_QIB_SEC_EIB:
#endif /* FEATURE_IS2000_REL_A */
       /* Setting MOD_PCBIT_TEST_MASK for MOD_PCBIT_TEST_DATA register.
       ** enable all bits for FPC_MODE_EIB.
       */
       enc_pcbit_test_mask (0xFFFF);
       break;

#ifdef FEATURE_IS2000_REL_A
     case FPC_MODE_PRI_400Hz_SEC_EIB:
       /* Setting MOD_PCBIT_TEST_MASK for MOD_PCBIT_TEST_DATA register.
       ** enable half of bits for EIB.
       */
       enc_pcbit_test_mask (0xaaaa);
       break;
#endif /* FEATURE_IS2000_REL_A */

     case FPC_MODE_PRI_800Hz:
     case FPC_MODE_PRI_400Hz_SEC_400Hz:
     case FPC_MODE_PRI_200Hz_SEC_600Hz:
     case FPC_MODE_FFPC_DISABLED:
       /* Disabling MOD_PCBIT_TEST_MASK. PC bits generated by DEMOD.
       */
       enc_pcbit_test_mask (0x0000);
       break;

     default:
       ERR("Bad FPC_MODE %d",fpc_mode,0,0 );
   }

}

#ifdef FEATURE_DS_IS2000
/*===========================================================================

FUNCTION TXC_GET_SCH_INFO

DESCRIPTION This function returns current configuration information for
            the SCHs. This is used by the UI in the data display.

            PARAMETERS:
            sch_rc -       gives the Radio configuration of the R-SCH
            sch_rate_ptr - points to the current SCH rate.
            double_ptr   - is TRUE when double sized PDUs are being used
                           is FALSE when single sized PDUs are being used

            Note, the above two pointer return values are only meaningful
            when SCH is assigned

DEPENDENCIES None

RETURN VALUE TRUE:  If SCH are assigned.
             FALSE: If no SCH are assigned.

SIDE EFFECTS None

===========================================================================*/

boolean txc_get_sch_info
(
  enc_sch_radio_config_type* sch_rc,                      /* ptr to SCH RC */
  enc_sch_rate_type* sch_rate_ptr,                   /* ptr to rate of SCH */
  boolean* double_ptr                   /* Ptr to single/double sized PDUs */
)
{

#ifdef FEATURE_IS2000_R_SCH
  if( txtc.sch_on == TRUE)
  {
    *sch_rate_ptr = txtc.supp_rate;
    *double_ptr   = txtc.double_sized;
    *sch_rc       = (enc_sch_radio_config_type) txtc.r_sch_rc;
    return( TRUE);
  }
  else
  {
    return( FALSE);
  }
#else
  /* ------------------------------------------------------------
  ** For right now, R-SCH are not assinged, so always return FALSE
  ** ------------------------------------------------------------ */
  return( FALSE);
#endif // FEATURE_IS2000_R_SCH

} /* txc_get_sch_info() */
#endif  /* FEATURE_DS_IS2000 */

#ifdef FEATURE_TX_POWER_BACK_OFF
/*===========================================================================

FUNCTION TXC_UPDATE_CHAN_CONFIG_TO_RF

DESCRIPTION This function calls by MUX to update the actual channel config to RF



DEPENDENCIES None


SIDE EFFECTS None

===========================================================================*/
void txc_update_chan_config_to_rf (boolean  valid_config)
{
  rf_txc_chan_config_type updated_chan_config;

  if ( valid_config )
  {

    updated_chan_config.backoff_config = BACKOFF_CHAN_CONFIG_VALID;
    /* intialize the chan config based on the MUX current setup */
    updated_chan_config.fch_rc  = txc_so.rev_fch.rc;

#ifdef FEATURE_IS2000_P2
    updated_chan_config.dcch_on  = txc_so.rev_dcch.included;
    updated_chan_config.dcch_rc  = txc_so.rev_dcch.rc;
    updated_chan_config.fch_on  = txc_so.rev_fch.included;
#else
    updated_chan_config.fch_on  = TRUE; /* no DCCH, we must have FCH */
#endif /* FEATURE_IS2000_P2 */

#ifdef FEATURE_IS2000_R_SCH
    if (txtc.sch_on)
    {
      int16 sch2pch;
      /* get the SCH-TO-PCH ratio */
      if (txtc.supp_turbo)
      {
        sch2pch = rpc_get_sch2pich_gain_ratio (txtc.supp_rate, CAI_TURBO_CODING);
      }
      else
      {
        sch2pch = rpc_get_sch2pich_gain_ratio (txtc.supp_rate, CAI_CONVOLUTIONAL_CODING);
      }
      /* check of the SCH to PCH ratio is bigger than 6 dB*/
      if (sch2pch > 48) /* SCH2PCH in the unit of 1/8 Db, so 6*8 = 48 */
      {
        updated_chan_config.sch_to_pch_over_6dB = TRUE;
      }
      else
      {
        updated_chan_config.sch_to_pch_over_6dB = FALSE;
      }
      updated_chan_config.sch_on  = TRUE;
      updated_chan_config.sch_rc  = txtc.r_sch_rc;
      updated_chan_config.sch_rate  = txtc.supp_rate;
    }
    else
    {
      updated_chan_config.sch_on  = FALSE;
    }
#endif /* #ifdef FEATURE_IS2000_R_SCH */
  }
  else
  {
    updated_chan_config.backoff_config = NO_BACK_OFF_CONFIG;
  }

  rf_update_tx_pwr_limit(&updated_chan_config);
}
#endif /* FEATURE_TX_POWER_BACK_OFF */



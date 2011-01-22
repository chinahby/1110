/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

       C D M A    R E C E I V E   C O M M O N   M O D U L E

GENERAL DESCRIPTION
  Responsible for receive command processing, utility routines,
  action timer registration/deregistration.

EXTERNALIZED FUNCTIONS
  rxc_cmd - send a command to the Receive task
  rxc_flag_good_frame -  inform RXC that it's okay to start processing good
  frame during a Return-If-Fail HHO
  rxc_get_mode - returns the current Receive task primary service option
  rxc_get_pc_crc - retrieves number of messages received on the Paging
  Channel with good and bad CRCs
  rxc_get_sc_crc - retrieves number of messages received on the Sync
  Channel with good and bad CRC
  rxc_get_sch_info - returns current configuration information for the SCHs
  rxc_get_slot_status- returns whether or not we are in our paging slot,
  or in the previous or next slot
  rxc_get_state - returns the current Receive task state
  rxc_load_pc_isr -  installs the PC ISR
  rxc_markov_reset - the rxc_zero_count variable in order to zero symbol
  error rate and Markov statistics
  rxc_powerup_init - initialization of Receive task variables and
  queues
  rxc_power_comb_config - configures the power control puncturing position
  rxc_rpt_voc_rate - Reports the current frame vocoder rate
  rxc_subtask - state machine for the CDMA rx subtask
  rxc_get_chan_demod_config_param - 
  rxc_get_chan_tc_demod_config_param - 
  rxc_dec_version - Wrapper function for dev_version()

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 1990 through 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath: O:/src/asw/COMMON/vcs/rxccommon.c_v   1.38   15 Oct 2002 18:30:52   donb  $
$Header: //source/qcom/qct/modem/1x/mux/rel/ulc/src/rxccommon.c#3 $ $DateTime: 2008/07/28 09:31:07 $ $Author: joshuam $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/24/08   jtm     Klockwork corrections.
07/16/08   jtm     Removed FEATURE_RXCCHAIN_DEBUG. Removed unused variables and
                   functions and reduced debug strings for ULC effort. 
05/03/07   cab     Moved to new sleep voting interface
02/23/07   vlc     Fixed compiler warnings/errors when SCH, Rel A, and P2 
                   features are turned off.
01/11/07   vlc     Fix featurization compiler error when FEATURE_IS2000_REL_A
                   is turned off.
01/04/07   trc     MSM7600 support
11/14/06   vlc     In function rxc_wait(), changed lockup recovery time from 6
                   watchdog timeouts to 3 since 6 is too long to wait.
10/30/06   bn      Added code to support DO to 1x handoff
10/24/06   bn      Attempt to recover lookup in rxc_wait(),by reseting DEC/DINT HW 
09/19/06   vlc     Fixed lint errors.
06/09/06   trc     Remove T_MSM6700 featurization
05/16/06   vlc     In function rxc_subtask(), check if 1X is operating on the 
                   primary RF chain before disabling decoder clock regime.
05/09/06   vlc     In function rxc_subtask(), added check for RXC_CHAN_CONFIG_F
                   command.  Ignore this command but account for it as it may 
                   be sent to RXC in certain HO cases.
                   This change was merged from 6500 target.
10/12/05   rkc     Add wrapper function rxc_dec_version().
09/20/05   rkc     Mainline feature FEATURE_SRCH_INACTIVE_STATE.
09/08/05   ejv     Remove inclusion of srch.h.
08/29/05   bn      Clear the all the decoder status it after installing the isr
07/25/05   rkc     Fixed lint errors.
04/27/05   vlc     Main lined code in current targets, cleaned out obsolete targets,
                   cleaned out obsolete features.                                   
03/14/05   bn      Modified the rxc_isr_cmd not to wait when the queue is not empty
                   also modified the rxc_wait() not to use msg_fatal instead of error_fatal
02/24/05   vlc     Lock interrupt between clearing DEC_DONE_INT_STATUS and
                   installing RXC ISR.
02/23/05   vlc     Clear the DEC_DONE_INT_STATUS when setting RXC ISR.
01/26/05   vlc     Removed obsolete ds.h inclusion.
12/07/04   ejv     Added new function rxc_get_chan_tc_demod_config_param.
09/28/04   sr      Merged in changes from 6550 2009 build
08/09/04   bn/kwo  Added new interfaces to expose chan mod setup parameters
06/16/04   sr      Merged in changes from 6500 5004 build
06/14/04   sr      Added debug message in rxc_flag_good_frame()
06/09/04   jyw     Corresponding modifications for the SEARCH inactive state
05/07/04   vlc     Implemented new RLP interface for EV-DV.  The new interface 
                   replaces the current interface for 1x channels as well.
                   Also, modified RLP logging extensively to work with new 
                   interface.
04/28/04   jrp     Reverted change of MSM_LOCK to INTLOCK_SAV to fix compilation
                   error with new search headers.
04/20/04   jrp     Fixed lint warnings.
03/09/04   jrp     Modified rxc_wait() to ERR_FATAL after petting watchdog 11
                   times without receiving any signal for which we are
                   waiting.
01/06/04   vlc     Modified function rxc_config_demod_ch1() to use MAC engine.
10/23/03   sr/vlc  Clear decoder ISR before disabling clock regime in function
                   rxcdma_subtask().
10/22/03   sr      Fixed compilation warning errors when using RVCT compiler 
10/14/03   vlc     In function rxc_config_demod_ch1(), removed call to 
                   rxc_power_comb_config() which sets power control channel 
                   registers.
09/09/03   dna     Merged porting changes for MSM6500
08/08/03   vlc     Use dec_dcch_status_type instead of dec_fch_status_type for
                   rxc_dcch_status.
07/18/03   vlc     Release A changes:  Changed FCH and DCCH
                   channel set up to better indicate when decoder data is
                   available for the appropriate channel(s) for the RXC layer.
06/16/03   bn      Fixed the "out of buffer dec_free_q" problem.
04/18/03   bn      Called function mux1xdiag_init() to register all the MUX cmd to DIAG
03/20/03   bn,sr   Added logic to support simultaneously monitor BCCH and FCCCH
03/05/03   fc      Consolidated f-csh fade timer implementation.
10/15/02   dlb     Added new power control logging to combine power control
                   and frame type logging.
10/15/02   sr      Cleaned up comments in rxc_isr_cmd()
10/14/02   sr      Added MSM6100 changes
10/11/02   sr      Called rxcidle_clr_fade_timer() in rxc_exit()
08/06/02   cr      Added MSM6100 changes.
08/27/01   vlc     Changed power measurement report process for VP2.
08/13/02   sr      Added Fast RAHO support
                   Disable CH0_RC4 bit for non-RC4 configurations in
                   rxc_config_demod()
08/07/02   bn      Remove obsolete reference to event_report_data() 
05/30/02   vlc     VP2 changes are now featurized under FEATURE_IS2000_REL_A_SVD
                   and FEATURE_IS2000_P2 per request of development team.
05/13/02   vlc     Added support for simultaneous DCCH & FCH channel set up 
                   (feature FEATURE_IS2000_REL_A_VP2.)
02/27/02   lad     Added rxc_clear_decoder_queue() and modified rxc_isr_cmd()
                   to call this routine.
                   Merged IS2000 Rel A Common Channels:
                   Added BCCH/FCCCH support in state machine.
                   Moved unused/obsolete code under "unused" feature.
02/25/02   bn      Added support for  MUX SVD signaling 
12/10/01   hrk     Renamed function rxc_flag_erasure() to rxc_flag_eib() and moved
                   the function to rxctraffic.c
11/20/01   bn      Added RDA logging logic
11/16/01   vlc     Added call to dec_clk_select () before switching rxc state.
10/30/01   bn      Added support for Release A FDCH logging
08/02/01   lcc     Moved some traffic channel related functions and data to
                   rxctraffic.c.
06/20/01   sr/tc   Fixed HHO between high RCs to low RCs in rxc_config_demod()
06/15/01   snn     Added two new functions for DCCH UI display.
06/15/01   day     Mainlined FEATURE_FFPC, FEATURE_IS95B, FEATURE_IS2000
                   Delete FEATURE_SPECIAL_MDR
06/04/01   vlc     Changed FEATURE_GPSONE to FEATURE_GPS_MODE.
05/27/01   snn     Added a new function rxc_get_fch_rate_set() for UI and
                   modified rxc_get_sch_info to return SCH RC
04/20/01   sr      Merged in from MSM5100 archive
04/19/01   tc      Merged with MSM_MUX1X.01.00.01
           bgc     Moved rxc_scr_trans_cmd to rxctraffic.c
04/18/01   sr      Merged with MSM_MUX1X.01.00.00
           lcc     Added support for FEATURE_IS2000_P2
03/27/01   vlc     Merged MSM_MUX1X.00.00.17.
           lcc     Merged in support for SO 17 (CAI_SO_VOICE_13K_IS733).
           lcc     Changes in rxc_get_sch_info for new type used for
                   rxc_stored_sch_packet.
           lcc     Optimized rxc_atmr_dispatcher.  Also added checking for phase
                   change after the ISR is done and execute the ISR for the next
                   phase if so; otherwise the ISR for the next phase will be missed.
           ych     Merged JCDMA features
03/13/01   tc      Cleaned up rxc_atmr_dispatcher..
03/02/01   tc      Merged MSM_MUX1X.00.00.15.
02/22/01   rm      Disable deinterleaver/decoder clock regime when exiting
                   FM Mode.
02/16/01   bgc     Changed FEATURE_FACTORY_TESTMODE to enable a runtime switch.
02/01/01   tc      Updated to rxc_rifhho_cfs_gps_in_progress.
01/30/01   tc      Merged GPS support.
01/18/01   ejv     Modified some combiner macros to match new names.
01/16/01   jq      Added FEATURE_IS2000_SCH ifdefs around SCH codes.
01/12/01   tc      Merged MSM_MUX1X.00.00.06.
12/22/00   hrk     Flushing FFPC log packets at end of call.
12/12/00   ejv     Added changes for CH0.
11/23/00   tc      Fixed MSM5100 PLT ftm_sc_msg, ftm_pc_msg, ftm_rxc_pc_cmd.
11/17/00   bgc     Fixed warnings with typecasts.
11/10/00   jc      repartioned rxc.c into several files from Build TR2064
10/16/00   hrk     Fixed problem ending back-to-back SCH burst.
10/16/00   hrk     Elaborated description for the overlapping SCH burst fix.
10/12/00   sr/hrk  Fixed forward overlapping burst assignment problem.
10/05/00   lcc     Moved processing of SMS traffic in rxc_pri together with OTAPA.
                   All these service options do not use data (primary or secondary)
10/04/00   jq      Added FTM FCH and SCH loopback feature
10/04/00   ak      More fixes in SCH logging.
10/03/00   sr      Excluded DTX frames from SCH bad frame stats for PMRM
09/29/00   hrk     Checking at rxc_sch.rc to determine if F-SCH is ON.
09/29/00   ejv     Added frame count while waiting for first frame in slot.
09/28/00   bgc     Added support for turbo DTX in rxc_tc_isr().
09/21/00   lcc     Combined freeing of TC resources on fade and on normal release
                   into rxc_free_tc_resources.  Also protected with MSM_LOCK
                   since it can happen both from interrupt context and task context.
                   Checking is made before proceeding to make sure that it wasn't
                   already done.  This fixes CR 14669.
09/18/00   lcc     1. Added clearing of rxc_sch_shutdown when processing a start
                      SCH action.  It is protected by INTLOCK becasue a race
                      condition can happen if a decoder ISR occurs (which checks
                      for rxc_sch_shutdown) at the same time.  Fixes CR 14711.
                   2. Added clearing of sigs in action command header in
                      rxc_process_at_cmd since no signal is expected when the
                      command is done processing.  If this is not done, spurious
                      signals may be sent.  This fixes CR 14732.
09/13/00   bgc     Turned on DTX detection for RC4 supplimental channels
                   in rxc_tc_isr().
09/12/00   sr      Changed ts_get_20ms_frame_time() to
                   ts_get_20ms_frame_offset_time() for action timing
                   Call to ts_set_frame_offset() in rxc_set_frame_offset()
09/06/00   bgc     Added FEATURE_FACTORY_TESTMODE.
08/23/00   ak      Removed sch_crc_err counts, as no longer useful.
09/05/00   rm      Added Fixes to correctly decode the last FCH frame
                   before the burst.
09/03/00   rm      Added changes to support MUX PDU type 1 for 1X SCH.
08/29/00   ak      Added missing ; to a return statement (l. 4168).
08/25/00   rc      Added changes to support NULL returns from
                   dsm_new_buffer().
08/24/00   hrk     CHanged FEATURE_CS_ACTION_TIME_TRANS to FEATURE_IS2000.
                   Removed redundant action time code.
08/22/00   rm      Fixed compiler warning.
08/18/00   rm      Free up TC resources when fade timer expires in TC state.
08/14/00   sr      Fixed SCH erasure reporting for TDSO calls
08/04/00   rm      Fixed timing issues related to SCH burst assigments.
08/02/00   sr      Added FCH statistics to power measurement report
                   Changed TDSO_BLANK to TDSO_ERASURE
07/31/00   lad     Fixed sub-record count in logging.
07/24/00   hrk     Collecting AGC values for Reverse Power Control logging.
07/24/00   ak      Changes to support some PDU's being BLANK when doing
                   RLP 3 logging.
07/20/00   ak      Removed old IS2000 mux stat structs and FEATURE_SCH_STATS.
                   Made fixes for Mux stats for SO33 and TDSO.
07/19/00   hrk     Added support for BS configurable Reverse Power Control Delay.
07/18/00   eh      Set the viterbi decoder to normal state in rx_pc_isr when
                   servicing interrupt for the first frame of paging slot.
07/14/00   ks      Setting the rxc_fade_time properly during RXC_ACC_FADE_F.
07/13/00   rm      Added PLT support for Supplemental Channel Multiframe Inter
                   leaving.
07/12/00   rm      Added suuport for 5ms frames.
07/12/00   sr      Fixed some indexing problems while building tdso frame block
                   Fixed the number of bits sent to tdso for 1x case.
07/11/00   sr      Set rxc_sup_dec_int_enable to TRUE for turbo codes
07/10/00   sr      Added STDSO support
07/06/00   ks      Added p_rev_in_use checking while setting T72M timer.
06/29/00   sr      Fixed a problem with SCH burst assignment where the last
                   SCH frame wasn't being received because the demod/deint/dec
                   was being shut down 1 frame too soon
06/26/00   kjn     Added vuid argument to rxc_dec_rate_sel.
06/22/00   hrk     Introduced function rxc_rda_correction() to improve SCH
                   DTX detection.
06/20/00   ak      Bumped up the rxc_dec_free_q count, to avoid dropped fr's
                   when timing may go bad.
06/19/00   bgc     Rearranged DTX detection to favor CRC pass over DTX.
06/16/00   lcc     1. Miscellaneous cleanup
                   2. Fixed CR 13635
                   3. Corrected a problem in comparison only the lower 2 words
                      of action time.
06/13/00   ak      Added SCH0 mux stats for parm database.
06/09/00   hrk     Added support for new MC-RXC interface.
06/09/00   bgc     Added in MSM5000_IRAM_FWD for moving functions into
                   internal RAM.
06/09/00   hrk     Added back fix for CR 13583 that got overwritten.
06/09/00   ak      Added support for F-SCH 1x.
06/08/00   hrk     Added support for Outer Loop Report
06/06/00   bgc     Added SER check for full rate for RC3 and RC4 to improve
                   full rate detection when frame bits are mostly zeros.
06/05/00   hrk     rxc_frame_type is passed as param to ffpc_update_setpt
                   instead of a boolean.
06/05/00   jq      Fixed CR 13583 by comment out the fade timer reset call in
                   rxc_pc.
                   Took out a redundant ts_get_20ms_frame_time() call in
                   rxc_pc_isr()
06/02/00   lad     Added SCH logging for frame types and FFPC.
06/02/00   ak      Added f-sch mux stats.  Also set dtx threshhold in
                   rxc_Fwd_sch_trans_cmd.
05/30/00   hrk     Moved FFPC FCH setpt updates during FCH interrupt.
05/26/00   jq      Added support for BCSMS feature (merged from PP)
                     - Added BC_DONE bit checking in idle page match
                     - Added the concept of slot_mask
                     - Allow early sleep in BC SMS slot by peeking into msg hdr
05/24/00   fc      Merged the following changes:
           ry      Added OTAPA support
05/22/00   bgc     Fixed some typos for DMSS in the DTX support.
                   Added fixed support for turbo SCH interrupts.
                   Added extra logging/msg PLT support for DTX.
                   Cleaned up and rearranged some rxc_tc_isr() code.
05/18/00   et      added support for dtx in sch
05/10/00   rm      Made Fixes to support 1x SCH.
05/10/00    jl     Added RLP statistics logging.
05/08/00   hrk     In rxc_atmr_set(), making sure that rxc_atmr_handle is
                   valid before calling atmr_set().
05/02/00   na      Fixed the SCH error message, which was printed many times
                   on getting a CRC error.
04/25/00   sr      Added support for FWD SCH transaction processing.
                   Changed bit-wise AND to a logical AND operation in rxc_tc_isr.
           hrk     Added support for SCH FFPC transaction processing.
                   Modified RXC-FFPC interface to handle SCH.
                   Updating SCH RC in SRCH for logging.
                   Changed FEATURE_IS2000_SCH to FEATURE_IS2000_F_SCH.
                   Re-arranged braces in rxc_tc_isr() for non MSM5000 case.
           dgy     Protected rxc_atmr_set () with INTLOCK, INTFREE.
04/25/00   hrk     Fixed brace mismatch in rxc_tc_isr() for non MSM5000 case.
04/24/00   hrk     done_q_ptr in action time buffer was overwritten. As a result
                   the buffer never got back into action item free Q.
04/21/00   hrk     Removed call to rxc_send_scr_action_time_rpt () for
                   RXC_HO_F and RXC_FPC_SUBCHAN_GAIN_F.
                   calling srch_update_rc () to update RC info in SRCH.
04/20/00   lad     Added subrecord count and len_per_frame to frame type log.
04/12/00   hrk     Added prototype for ffpc_log_frame() and ffpc_send_log().
                   Call to rxc_send_rlp_frames_log() was not under
                   FEATURE_RLP_LOGGING.
04/12/00   hrk     FPC_HISTORY was assigned into rxc_rpctrl_dec_history
                   instead of RPC_HISTORY.
                   Renamed global rxc_ffpc_dec_history to rxc_fpc_history,
                   rxc_rpctrl_dec_history to rxc_rpc_history.
                   Moved call to ffpc_send_log () from ffpc_ho_cmd() to
                   rxc_service_ho_cmd ().
04/11/00   lad     Added Fast Fowrward Power Control and forward frame types
                   logging.  Cleaned up RLP logging.
04/10/00   dgy     Removed FEATURE_IS2000_QPCH.
04/07/00   va      Added the processing of RXC_TUNE_RF_F (HHO fix from PP)
04/05/00   na      CR 13264: scc_crc was not being reset before call to
                   dec_read_scch_data. This was causing erasures on SCCH to
                   be detected as Full Rate frames. Now fixed.
03/31/00   na/ak   In the RX task, rxc_dsm_arr_cnt was not being checked
                   within array range. Fixed this.
03/24/00   bgc     Fixed FEATURE_RLP_LOGGING so that it can be turned off.
                   Updated rxc_fch_rda() for rate determination at RC3-RC5.
                   Added a "Skip TCI" option in PLT so that the test for good
                   frames is ignored.
03/23/00   lad     Added partial RLP frames logging under FEATURE_RLP_LOGGING.
                   Cosmetic changes and changed len in event_report_data call.
03/16/00   na      Added support for RLP3 - dsrlp_process_rx_frames() under
                   FEATURE_DS_RLP3, and support for data SCH Mux layer under
                   FEATURE_DS_IS2000.
                   Removed extraneous write to the timetest port.
03/16/00   bgc     Fixed reacquisition of Paging channel after RC4 call in
                   rxc_pc_init() with COMBINER_CLEAR_ULPN_OPTION().
02/28/00   hrk     Switched back to old RDA.
02/24/00   lad     Added support for streaming DIAG and backward compatable
                   FEATURE_DIAG_V7_LOG_IFACE.
02/22/00   bgc     Fixed some typos in the PLT code and removed some
                   unnecessary #ifdefs that made compilation fail.
02/04/00   rm      atmr_get is now called in rxc_pc_init instead
                   of rxc_tc_init. This will need to change when
                   QPCH is turned on.
02/02/00   rm      Free up resources/clean up on exit from TC state.
02/02/00   hrk     Merged following changes from GV40401:
           hrk     Merged function rxc_scc_rate_sel_rlp().
           aaj     Fixed Markov Packet anamoly in Data Services service option
           dna     Fixed CR12117 and CR12166 by moving handoff timer into RXC.
           aaj     Moved rxc_voc_xchg() from rxc_pri to the rxc_tc_isr(). This
                   was done to take care of a bug in EVRC vocoder code.
           aaj     Added FEATURE_DETECT_SUP_MUTING which detects the MDR sup
                   channel muted frames and erases those even if the CRC is ok.
           kk      Removed the panther version of the voc_config() function.
02/01/00   hrk     Added support for Implicit Action Time.
01/31/00   rm      Enable decoder clock regime on wakeup. DEC clock
                   regime is disabled when the MSM is armed to sleep.
01/25/00   hrk     Added support to process commands in RXC_TC_STATE at
                   Action Time.
01/24/00   rm      Added in support to do event sequencing in deint driver.
01/24/00   rm      Added in new interfaces to support ACTION TIME processing.
                   The changes are under FEATURE_RXC_ACTION_TIME_PROCESSING.
01/19/00   hrk     Moved FFPC logging to FFPC module. Also fixed typo in
                   defining FEATURE_FFPC.
01/07/00   bgc     Changed RX action time report to "RXC_AT_R" to make it
                   more general
01/04/00   hrk     Fixed compiler warnings. Changes for updated MC-RXC
                   interface.
12/17/99   bgc     Added CRC summary logging for verification tests.
12/16/99   rm      Implemented the new Rate Determination Algorithm
                   for FCH (RC3, RC4, RC5). This algorithm is based on
                   Daisy Chen's Simultaions.
12/13/99   hrk     Moved FFPC functionality to a separate module. Older
                   implementation featurized under FEATURE_FFPC_OLD.
                   Added support for new MC-RXC interface.
                   Removed HHO FIX per Vanitha.
12/01/99   hrk     Changed interface to FFPC support functions to allow
                   scaled setpoint values to have 64 bits.
12/01/99   hrk     (For Vanitha) Added RXC_RELEASE_F in Traffic Channel state.
                   Added HHO FIX in RXC_RETURNED_F.
11/15/99   rm      Commented out processing for RXC_HO_F. This is going
                   to change.
11/15/99   rm      Added support to send MC report indicating that SCR has
                   has been acted on based on the ACTION TIME from MC.
11/12/99   rm      Made temp. fixes for frame offset timing issues affecting
                   deinterleaver/decoder for MSM5000. More general solution
                   to come later.
11/09/99   rm      Made Changes to support Turbo decoding of RC3, RC4 and
                   RC5 SCH.
11/04/99   bgc     Changed some calls to plt_put_log to conform to the
                   more obvious function parameters.
11/03/99   lcc     Merged from GD4021:
           kmp     Mainlained the FEATURE_ENHANCED_STANDBY_II and the
                   #ifdef FEATURE_ENHANCED_STANDBY by removing the #ifdef's
                   in the following areas:
                    - removed #ifdefs in the rxc_slot structure
                    - removed #ifdefs around MACROS RXC_MAX_INT_CNT and
                      RXC_SLOT_INT_CNT, and global var rxc_last_ser.
                    - removed #ifdefs in rxc_pc_isr(), rxc_pc_parse(),
                      rxc_pc(), rxc_exit(), and mainlined rxc_load_pc_isr().
           kmp     Merged in the new Vocoder changes required for Pink Panther
                   in rxc_tc(). Fixed a problem in which SMS service options
                   were not bringing up primary traffic in rxc_pri().
           ry      Updated to process good frame during a Return-If-Fail HHO
                   only when RXC is told to (by calling rxc_flag_good_frame);
                   otherwise, ignore all good frames
           ry      Fixed a problem in which the rxc_rif_hho_or_cfs_in_progress
                   flag is not reset properly.  Also fixed how the mobile waits
                   for two consecutive good frames before turning on the
                   transmitter in the case of a failed Return-If-Fail HHO
           rc      Changed occurence of ds_process_rlp_rx_frame() to
                   ds_process_rlp_rx_frames().
11/02/99   wjn     Added code to perform quick page match for MSM5000's 72-bit
                   decoder delay.
10/27/99   ajn     Added QPCH support
10/21/99   rm      Added new Interface RXC_SCCH_F to support MDR for MSM5000
                   and other various changes for SCCH.
10/21/99   rm      Replaced rxc_tc_isr with a MSM5000 specific rxc_tc_isr.
10/07/99   lcc     Merged from PLT 2.0:
           jcw     Added F3 message control for PLT decoder messages
           bgc     Fixed size/rate table for PLT logging and moved MSG code to
                   a section where it will see erasures.
           rm      Added SCH logging support.
           hrk     Added support for Outer Loop on F-FCH
           rm      Added SCH support. Configure deinterlevaer/decoder and
                   programming MAC Engine.
09/21/99   lcc     1. Changes to use new interface to MC.
                   2. Added MSM5000 support to service options
                   3. Optimized rate determination functions for supp.
                   3. Merged in following MSM5000 and PLT support from PLT
                      archive:
           jcw     Changes to tc_isr messages, logging improvements from bgc
           rm      Fixed RC4 descrambling.
           bgc     Modified logging to log with data or without data.
           rm      Made PLT changes  to fix jump to hyperspace timing issues.
           bgc     Fixed spacing problems and changed some RC problems in the
                   logging functions.
           rm      Added rxc_power_comb_config to configure the power control
                   puncturing based on Radio Configuration.
           bgc     Added PLT logging for decoder information.
           hrk     Added support for Fast Forward Power Control.

           rm      Added RC parameter in rxc_tci command for 95c
           rm      Made Changes for sync and paging channel frame processing
                   . The decoder delay in continous mode for MSM5000 is 72 bits.
           rm      Made changes for new deint/dec driver interfaces and
                   MSM5000 PLT support
           jcw     Added dgy's changes to fix paging channel PLT operation
           jcw     Merged in the following three PLT changes
           dgy     Supported the option so that user can decide whether to
                   wait for a PC message before proceeding.
           dgy     Added code so that RXC task continuously demodulates and
                   decodes Sync Channel frames while in SC state. Sent message
                   to PLT_MCC when RXC task receives the first message in SC
                   or PC state.
           dgy     Added support for PLT.
09/02/99   ry      Removed obsolete MSG_MEDs from rxc_tc_data()
08/26/99   aaj     Merged Panther Branch 3.1 to Mainline
08/25/99   aaj     Changed threshold for sup erasure detection for RS2 (from 77 to 50)
08/13/99   jq      Added casting for ts_get()'s parameter to clear compiler
                   warnings.
08/09/99   ry      Combined FEATURE_IS95B_HARD_HO and FEATURE_IS95B_CFS into
                   FEATURE_IS95B_MAHHO
           ry      Added check in rxc_tc_parse() to ignore any signalling
                   message that the mobile might have received right before
                   it is about to tune to the CF/TF
08/09/99   lss     Fixed a problem with the blackout counters under MDR_FER_F3MSG
08/06/99   nmn     Removed IS-96
07/29/99   hq      Changes for SMS SO6 and 14 support.
07/27/99   lss     added a feature FEATURE_MDR_FER_F3MSG for collecting MDR frame
                   error statistics and printing it on on DM F3 screen.
07/09/99   ry      Replaced txc_flag_erasure() with rxc_flag_erasure()
           ry      Updated pwr_msr_check() to suspend counting
                   rxc_pwr.num_frames during a Return-If-Fail HHO
           ry      Added rxc_start_tx()
           ry      Splited up fade_check() into fade_check_bad_frame(),
                   fade_check_good_frame(), fade_check_good_frame_hho(), and
                   fade_check_good_frame_non_hho() and added IS95B HHO support
           ry      Updated rxc_tc_data() to suspend counting
                   rxc_pwr.num_bad_frames and start counting rxc_lost_frames
                   during a Return-If-Fail HHO
           ry      Added the processing of RXC_RIF_HHO_OR_CFS_F and
                   RXC_RETURNED_F
07/06/99   aaj     Advanced freeing of decoder buffer queue element to avoid "no
                   free dec buf" problem in the traffic channel state
06/23/99   ks      resetting tx_is_disabled to false when T40M report is sent.
06/17/99   kmp     Merged in the one change listed below.
06/07/99   ks      Added T72M timer implementation during PC monitoring while
                   the Mobile is in System Access state.
05/27/99   kmp     Changed FEATURE_JSTD008 to FEATURE_TMSI.
06/08/99   aaj     Added ERR_MESSAGE if a sup channel intr occurs without sup channels
                   Also corrected few comments as suggested in the code review.
05/25/99   aaj     Fixed reverse link EIB setting for LSPD,voice and sup=0 cases
04/22/99   aaj     Undone the (cast) operator changes made for armtools 2.5
04/21/99   aaj     Replaced MSG_ERROR with MSG_HIGH in rxc_pc_isr()
04/19/99   aaj     Usage of (cast) operator in ts_get to support armtools 2.5
04/15/99   aaj     Changed rxc_tc_isr() to handle two traffic channel interrupts
                   in a MDR call. First intr is for FCC and second for SCC
04/14/99   aaj     Corrected rate determination threshold for 8th rate RS2.
04/14/99   aaj     Changed the mux table variabels from 4 byte int to 2 byte word
04/10/99   aaj     De-Featurized (mainlined) FEATURE_CLASS_1_IMSI
04/06/99   aaj     Removed functions for RXDSP clock changes.RXDSP runs at 2/3tcxo
04/06/99   aaj     Replaced b_copy_litend_to_bigend_word with optimized for MDR
                   only version b_copy_ltobw_s1d0 and b_copy_ltobw_s2d0 functions
04/01/99   aaj     Interface to new PARAM database for supplemental channel FER
03/25/99   aaj     FEATURE_MINIMIZE_ISR_LOADING added to RXC. txc_voc_xchg is
                   moved out of rxc_tc_isr to TXC task. rxc_voc_xchg is run in
                   the context of RXC task.
03/23/99   aaj     Support for mux dim and burst for data frames was added.
02/26/99   ejv     Merged FEATURE_ENHANCED_STANDBY_II changes, including fast
                   slotted page match and 2nd half frame MER issue.
02/18/99   aaj     Moved contents of mux.c into rxc.c as it contained only
                   data declarations. Also changed the mux look up tables to
                   consts to move them into ROM. That saves some RAM space.
02/03/99   aaj     Added functions to switch RXDSP clock between 2/3TCXO and chipx8.
                   Also added functions for clock change from MC for MDR calls.
02/01/99   aaj     Decoder buffer read optimization in RXC_TC_ISR. Reads only
                   the required read from decoder as agaist all rates.
01/19/99   lcc     Corrected a problem in rxc_tc_data where dec_bits may be
                   used when it is not initialized yet.
01/15/99   aaj     Merged MDR branch with Panther mainline.
                   Edit History for MDR branch.
                        01/05/99   smp  Fixed bugs in supplemental channel processing for RLP.
                        12/23/98   lcc  Fixed a problem with DPT supplemental frames
                        11/19/98   aaj  MDR RLP interface. Under #define FEATURE_IS95B_MDR.
11/20/98   ht      Add FEATURE_ENHANCED_STANDBY label for code already
                   checked in.
11/3/98    abh     Changes for frame no. correction after the frame offset
                   change. Also made some changes to remove compiler warnings.
10/22/98   pms     Backed out nearing of variables.
10/02/98   pms     Neared variables to save ROM space.
09/14/98   ajn     Valid messages before slot boundary are once again ignored.
09/14/98   gb      Allow report to mc, resulting in sleep, after any valid
                   message, since there is no use staying awake for the
                   entire slot cycle.
09/14/98   ajn     Valid messages BEFORE slot boundary should not satisfy
                   the "valid messages in first two slots" condition to go
                   to sleep.
09/14/98   cly     Pulled in new fade_check function from -r1.118 to provide
                   IS-95A 6.6.6.2.8 compliance. Also, pulled in changes from
                   -r1.118 for 14400 rate determination and more err logging.
09/14/98   ajn     Empties decoder queue after uninstalling decoder ISR, to
                   prevent a remnant message from reenabling the FADE timer
                   as the phone goes to sleep.
08/31/98   pms     Added Tan's Pascal calling convention code for ROM savings
08/20/98   ck      Using VOC_CAP_IS733 instead of VOC_CAP_ISXXXX
                   Using VOC_CAP_IS127 instead of VOC_CAP_EVRC.
08/11/98   ck      Added support for EVRC Service Option
07/28/98   lcc     Merged in ARM support.  MSM3 support is also merged in.
03/12/98   aks     Removed pre-Gemini conditionals.  Changed target-specific
                   sections into FEATURE_AGC_LOGGING.
01/27/98   ldg     Commented usages of the unusual DS_CASE macro.
01/15/98   lh      Included customer.h.
01/15/98   lh      Resend RXC report to MC if out of buffers on mcc_rx_free_q.
11/03/97   ldg     Replaced references to specific data service options with
                   a macro which represents the same information.
07/16/97   lh      Added logging out-of-buffer errors.
07/09/97   na      Fixed a RLP bug where the RLP layer was not getting informed
                   when the mux layer got an erasure. Change under ifdef DS.
06/17/97   dhh     Configured for Q (T_Q).
06/11/97   gb      UnFixed Lint catch in rate determination since this uncovered
                   several problems with the thresholds that are data dependant.
                   Fixed a paging channel data buffering problem causing occasional
                   messages to be processed very late (on re-entry to idle state).
05/29/97   na      Added correct data service option numbers - fixes service
                   option change while in traffic channel. Changes under ifdef
                   DS.
04/09/97   gb      Fixed Lint catch in 14400 rate determination where
                   if CRC pass and yamamoto good and SER below thresh
                   but another rate SER is lower we don't declare erasure.
12/23/96   rah     Added ONIST TMOBILE Support.
11/14/96   jjw     Added Rate Set 2 data service capabilities.
10/22/96   dna     Flush 8 instead of 3 half-frames on idle handoff.
10/22/96   rdh     Consolidated a few TRAMP function calls to save ROM.
09/26/96   gb      Restore TMOBILE function inadvertantly excised.
09/24/96   gb      Added more frame rate checks, reserved bits == 0, valid
                   signalling data LEN or CRC.
09/19/96   rdh     Consolidated markov statistics functions to save ROM.
09/19/96   gb      Ifdef'd vocoder logging TMOBILE to save RAM and ROM.
09/13/96   gb      Added checks for valid mux types that we use to throw
                   away some of the bogus false rate determination frames.
09/06/96   gb      Sped up erasure indicator bit timing in tc_isr.
08/05/96   gb      Fixed faulty fade check logic.
07/25/96   gb      Cleaned up decoder buffer management to avoid ERRs.
07/10/96   gb      Fixed paging channel frame time stamping to avoid going to
                   sleep too early from a page in previous slot.
                   Changed to new 14400 rate determination algorithm.
                   Fixed a number of possible buffer misuse problems which
                   would result in buffers being held too long.
06/11/96   rdh     Changed slot_status fnc to detect slot, slot-1, slot+1.
05/02/96   rdh     Changed slot_status function to detect slot and slot-1.
04/09/96   rdh     Added funct to return whether or not we're in Paging slot.
03/14/96   rdh     Priority changes now only allowed when using idle fade timer..
03/06/96   jca     Receipt of RXC_PWR_REP_DELAY command now triggers
                   PWR_REP_DELAY countdown for Power Measurement reporting.
03/04/96   rah     Updated CSS1 conditionals.
03/01/96   gb      Made vocoder pkt xchg callable by txc_traffic_isr.
02/28/96   rdh     Added mods for robust pkt/cont mode operation during Jump.
02/26/96   rdh     Fixed bad ifdef for sleep tcb for Beta2.
02/01/96   rdh     Added MC priority bump, removed ZZ ISR for sleep speedup.
01/17/96   gb      Changed ts_get_frame calls ensure correct frame timing.
11/16/95   jca     Removed INTLOCK/INTFREE pair in function rxc_cmd().
11/09/95   jjw     Completed changes to support fully compliant IS-99
11/07/95   gb      Linted and removed old unused variables and code.
11/02/95   fkm     Added calls to snd_set_voc_gain_adjust() for Vocoder Gains.
10/20/95   gb      Fixed internal mux1 array bounds problem by removing it.
10/12/95   gb      Changed markov logging formats to suit data analysis.
                   Changed msg_q size to 5 to buffer more ota msgs.
10/06/95   gb      Changed call release to switch rx to SO_NULL.
09/28/95   rah     Added MAR_RATE_SEL call. Changed rate deturmination
                   algorithm.
09/22/95   dna     Now call fade_check for 13K calls and drop if necessary.
08/23/95   ptw     Crippled vocoder logging to save a little RAM.
08/16/95   gb      Changed voc_rate_sel and mar_rx to support rate set 2.
08/14/95   gb      Added MUX2 parameter support.
08/14/95   gb      Added new Loopback_13K service option support.
08/10/95   gb      Added New Markov Service Options and made all service
                     options CAI_SO_*.
07/28/95   rah     Removed AUDIO_CONTROL_PATCH.
07/26/95   rah     Added rxc_reset_voc_init call.
07/21/95   jjw     Added local variables within '#ifdef FEATURE_DS' to support IS-99
07/20/95   rah     Moved voc_init to the tc ISR for GEMINI and ISS2 targets.
                   Removed txc task run from rxc_tc_isr for GEMINI and ISS2.
07/13/95   gb      Changed mux option initialization.
07/06/95   rah     Removed refs. to vcmd and modified for 13k init.
06/28/95   rdh     Move agc log back into 20ms isrs til DA group analyzes needs.
06/21/95   rdh     Fixed wakeup problem now that RXC controls chipx8.
06/19/95   rdh     Temporary mods for audio path muting during origination.
06/15/95   rdh     Moved all chipx8 bring-down code into new RXC sleep ISR.
06/14/95   gb      Added support for Rate set 2 service options.
06/13/95   rdh     Made T_SLEEP handling explicit and consistent DMSS-wide.
05/27/95   rdh     Moved AGC/power control logging over to PN roll interrupt.
05/15/95   gb      Changed constants to match new DEC declarations for gemini.
04/13/95   rdh     Call AGC/Power control logging in RX ISRs if Gemini.
03/30/95   rdh     Added srch_control_chipx8_clk call to fix runt pulse problem.
01/19/95   jca     Enabling the vocoder whenever we transition to TC state.
01/09/95   gb      Changed wake processing to delay frame count reinit.
12/23/94   jca     Now reinitalizing the vocoder on a hard handoff command.
11/29/94   gb      Changed rxc_tc_isr to signal TX for new TX frame timing.
10/27/94   rdh     Told SRCH @ bad slot rather than messing with MC transitions.
10/24/94   rdh     Additions for PN autoswitch/error correction.
10/12/94   gb      Changed voc_rx ERR_FATAL to ERR due to loopback modes.
08/09/94   jca     Added support for IS96/IS96A dual-capability vocoder.
04/25/94   gb      Parameterised all IS95 timeouts in cai_tmo.
04/04/94   gb      Set non-slotted mode when RXC gets an access_fade cmd.
03/24/94   gb      Added called interface to set isr & timing fix code.
01/25/94   jah     Go back to hunt state on wakeup from sleep
01/06/94   jah     Changed watchdog iface to use dog_report().
12/22/93   jca     Modified for IS-95 Phase 1 upgrade.
12/20/93   gb      Sleep version with frame# set in pc-isr and reset pc_rate.
08/24/93   jca     Now processing RXC_CDMA_F command in PC, TCI and TC states.
01/14/93   jai     Added hard handoff processing.
01/05/93   jai     Added data service support and fixed Markov reset problem.
10/15/92   jai     Added vocoder test.
10/08/92   jai     Added fixes for problems found in unit test.
10/06/92   jai     Fixed signal set in rxc_mc_rpt.
09/17/92   jai     Changed Vox functionality
06/24/92   jai     Started again.
03/28/91   jai     Create file.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include <memory.h>
#include "comdef.h"
#include "target.h"
#include "customer.h"
#include "qw.h"
#include "rex.h"
#include "queue.h"
#include "task.h"
#include "err.h"
#include "log.h"
#include "event.h"
#include "voc.h"
#include "crc.h"
#include "enc.h"
#include "mar.h"
#include "cmd.h"
#include "rxc.h"
#include "mccdma.h"
#include "rxtx.h"
#include "msg.h"
#include "dog.h"
#include "tramp.h"
#include "mccrx.h"
#include "ts.h"
#include "cai.h"
#include "rx.h"
#include "tx.h"
#include "mccrxtx.h"

#include "deint.h"
#include "msm_drv.h"
#include "atmr.h"
#include "ffpc.h"

#if  defined (FEATURE_IS95B_MDR)
#include "mccscm.h"             /* supplemental channel processing routines */
#endif /* FEATURE_IS95B_MDR */

#include "snd.h"

#ifdef FEATURE_DS
#if  defined (FEATURE_IS95B_MDR)
#include "mdrrlp.h"              /* New MDR RLP routines */
#else
#include "rlp.h"                 /* RLP routines */
#endif /* FEATURE_IS95B_MDR */
#endif /* FEATURE_DS */

#ifdef TMOBILE
#error code not present
#endif

#ifdef FEATURE_PLT
#error code not present
#endif /* FEATURE_PLT */

#ifdef FEATURE_IS2000_TDSO
#include "tdso.h"
#endif // FEATURE_IS2000_TDSO

#ifdef FEATURE_FACTORY_TESTMODE
/* need some txc buffers to do loopback */
#include "txc.h"
#endif
#include "clkregim.h"

#include "rxccommon.h"
#include "rxctraffic.h"
#include "rxcsync.h"
#include "rxcrda.h"
#include "rxcdemux.h"
#include "rxcpaging.h"
#ifdef FEATURE_IS2000_REL_A_CC

#include "rxcbcch.h"
#include "rxcfccch.h"
#ifdef FEATURE_IS2000_REL_A_CC_SMMC
#include "rxcnccch.h" 
#endif /* FEATURE_IS2000_REL_A_CC_SMMC */
#endif /* FEATURE_IS2000_REL_A_CC */
#include "rxcidle.h"

#ifdef FEATURE_FACTORY_TESTMODE
#include "test_mc.h"
#endif
#include "mux1xdiag.h"

#include "muxmsm.h"

#include "sleep.h"

#if defined(FEATURE_RLP_LOGGING)
#include "dsrlp_stats.h"
#endif
#include "srch_mux.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/


  /* Table to convert forward link RC to rate set */
  /* Note: RC starts from 1, zeroth entry is a filler.  All RCs entries defined */
  /* here, but Tiger 2 only supports 1 - 5. */
const dec_rate_set_type fwd_rc_to_rate_set[10] =
  { (dec_rate_set_type)0, DEC_RATE_9600, DEC_RATE_14400, DEC_RATE_9600,
    DEC_RATE_9600, DEC_RATE_14400, DEC_RATE_9600, DEC_RATE_9600,
    DEC_RATE_14400, DEC_RATE_14400 };

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/* Timers */
rex_timer_type  rxc_rpt_timer;
  /* Timer to wake Receive task so that it can set its Watchdog task
     report signal */

/* Queues */

q_type          rxc_cmd_q;
  /* Queue holding commands for the Receive task */

q_type          rxc_dec_q;
  /* Queue for signaling data packets received from the decoder */

q_type          rxc_dec_free_q;
  /* Queue for free buffers to be placed, when filled,
     on the rxc_dec_q queue */

q_type          rxc_isr_q;
  /* Queue for command packets to be processed by the Receive interrupt
     handler */

q_type          rxc_isr_free_q;
/* Queue for free buffers to be placed, when filled,
     on the rxc_isr_q queue */

byte rxc_frame_offset = 0;
  /* Stores the current frame offset of the forward link traffic
     channel */
byte rxc_frame_offset_countdown = 0;
  /* Used as a counter to determine of rxc_dec_frame needs to be recomputed */

rxc_state_type rxc_state;     /* State of RXC state machine */


byte rxc_rate;        /* Command buffer */

rxc_isr_cmd_type *rxc_isr_cmd_buf_ptr;

/*-------------------------------------------------------------------------*/
/* Buffers for RXC_DEC_FREE_Q                                              */
/*-------------------------------------------------------------------------*/
/* BN increasing the buffer to handle SIFCCCH/BCCH */
#define                  RXC_NUM_DEC_BUFS 6 
rxc_dec_type rxc_dec_bufs[ RXC_NUM_DEC_BUFS ];

/*-------------------------------------------------------------------------*/
/* Buffers for RXC_ISR_FREE_Q                                              */
/*-------------------------------------------------------------------------*/
#define                  RXC_NUM_ISR_CMD_BUFS  3
rxc_isr_cmd_type rxc_isr_cmd_bufs[ RXC_NUM_ISR_CMD_BUFS ];
/*-------------------------------------------------------------------------*/


dec_fch_status_type rxc_fch_status;

#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
/* Used in simultaneous F-FCH and F-DCCH channel set up.
*/
dec_dcch_status_type rxc_dcch_status;
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */

/* Total symbol error rates returned from decoder */
rxc_ser_type rxc_ser_total = { 0, 0, 0, 0, 0 }; /*lint -e552 used for logging */

byte  rxc_zero_count; /* Indicator of whether SER statistics should be set to
                        zero - anything other than 0 = reset */

int rxc_time_cnt = RXC_TIMER_NOT_SET;


/*-------------------------------------------------------------------------*/
/* Interrupt Service Routine variables                                     */
/*-------------------------------------------------------------------------*/

qword  rxc_dec_frame;
  /* Frame number */



  /* logging AGC values */
byte log_rx_agc = 0;
byte log_tx_pwr = 0;
byte log_tx_gain_adj = 0;


#if defined (FEATURE_DIAG_V7_LOG_IFACE)
  q_type rxc_log_msg_q;  /* for logging received messages */
  typedef struct
  {
    log_desc_type desc;           /* log descriptor */
    union
    {
      log_hdr_type       hdr;    /* log header */
      log_sc_msg_type sc_msg;    /* Sync Channel message */
      log_pc_msg_type pc_msg;    /* Paging Channel message */
      log_f_tc_msg_type tc_msg;  /* Traffic Channel message */
    }entry;
  } rxc_log_msg_type;

  /* buffers for rxc_log_msg_q */
  #define                          RXC_NUM_MSG_BUFS 5
  rxc_log_msg_type rxc_log_msg_pool[ RXC_NUM_MSG_BUFS ];
#endif


/*-------------------------------------------------------------------------*/
/*           Forward Traffic Channel Power Control Parameters              */
/*-------------------------------------------------------------------------*/

rxc_pwr_type rxc_pwr;

/*-------------------------------------------------------------------------*/
/* Service Option variables                                                */
/*-------------------------------------------------------------------------*/

rxc_so_type rxc_so;

#ifdef FEATURE_IS2000_REL_A_SVD

rxc_srid_ref_type  rxc_srid_ref[RXC_SR_ID_MAX]; /*service option data for reference */

#endif /* FEATURE_IS2000_REL_A_SVD */


#ifdef FEATURE_IS95B_MDR

/*-------------------------------------------------------------------------*/
/* Supplemental Code Channel (IS95B SCCH )variable                         */
/*-------------------------------------------------------------------------*/

  rxc_scch_type rxc_scch;
#endif  /* FEATURE_IS95B_MDR */

boolean rxc_sup_dec_int_enable=FALSE;
boolean rxc_turbo_dec_enabled = FALSE;
/* Indicates if Convolutional SCH/SCCH decoder interrupt is enabled */

#ifdef FEATURE_PLT
#error code not present
#endif  /* FEATURE_PLT */

/* Variables for counting slots                                            */
/*-------------------------------------------------------------------------*/

rxc_slot_type rxc_slot;


word rxc_log_msg_drop = 0;     /* number of messages dropped due to no
                                 available buffer */

#if defined (FEATURE_IS95B_MAHHO) || defined (FEATURE_GPS_MODE)
  /* Indicator to ignore good frame while doing a Return-If-Fail HHO      */
  boolean rxc_ignore_good_frame = FALSE;
#endif /* FEATURE_IS95B_MAHHO || FEATURE_GPS_MODE */

#ifdef FEATURE_IS2000_TDSO
  tdso_frame_block_type rxc_tdso_frame_block;
#endif // FEATURE_IS2000_TDSO

static sleep_okts_handle rxc_sleep_handle;
  /* unique handle used to vote for sleep (tcxo shutdown) */

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

#ifdef FEATURE_DS_IS2000
/*========================================================================

FUNCTION RXC_CLEANUP_SCH_MUX

DESCRIPTION This function cleansup the array that is used to hold
            the SCH MUX frames

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS None

=========================================================================*/
void rxc_cleanup_sch_mux( void )
{
  byte i;                                                    /* loop ctr */

  /* setup SCH MuxPDU array to hold new items */
  for( i=0; i< SCH_MAX_FWD_PDU; i++)
  {
    dsm_free_packet( &rxc_dsm_arr[i]);
  }
  rxc_dsm_arr_cnt = RXC_DSM_ARR_NOT_ALLOC;
} /* rxc_cleanup_sch_mux() */

#endif /* FEATURE_DS_IS2000 */

/*===========================================================================
FUNCTION RXC_RPT_VOC_RATE

DESCRIPTION
  Reports the current frame vocoder rate to caller.

DEPENDENCIES
  None.

RETURN VALUE
  Current frame vocoder rate.

SIDE EFFECTS
  None.

===========================================================================*/
word rxc_rpt_voc_rate( void )
{
  return( rxc_voc_rate );
}


/*===========================================================================
FUNCTION RXC_RESET_FRAME_COUNT

DESCRIPTION
  This function resets the frame counters of valid message reporting

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  Some RXC global counters will be changed.

===========================================================================*/
void rxc_reset_frame_count( void )
{
  rxc_slot.valid_msg_mask = 0;
  rxc_slot.slot_frames = 0;
#ifdef FEATURE_BROADCAST_SMS
  rxc_slot.bcpg_slot_frames = 0;
  rxc_slot.bcsms_slot_frames = 0;
#endif /* FEATURE_BROADCAST_SMS */
}


/*===========================================================================

FUNCTION RXC_GET_STATE

DESCRIPTION
  This function returns the current Receive task state.

DEPENDENCIES
  None.

RETURN VALUE
  Current Receive task state.

SIDE EFFECTS
  None.

===========================================================================*/

rxc_state_type rxc_get_state( void )
{

  return( rxc_state );

}/* rxc_get_state */


/*===========================================================================

FUNCTION RXC_GET_MODE

DESCRIPTION
  This function returns the current Receive task primary service option


DEPENDENCIES
  None.

RETURN VALUE
  Current Receive task mode.

SIDE EFFECTS
  None.

===========================================================================*/

word rxc_get_mode( void )
{

  return( rxc_so.pri );

}/* rxc_get_mode */



/*===========================================================================

FUNCTION RXC_SER_REPORT

DESCRIPTION
  This function returns the symbol error rate statistics.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void rxc_ser_report
(
  rxc_ser_type *ser_ptr
    /* pointer to where to put statistics */
)
{
  /* Unused parameter */
  (void) ser_ptr;
}/* rxc_ser_report */


/*===========================================================================

FUNCTION RXC_MARKOV_RESET

DESCRIPTION
  This function sets the rxc_zero_count variable in order to zero symbol
  error rate and Markov statistics.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void rxc_markov_reset( void )
{

  rxc_zero_count = TRUE;

}/* rxc_markov_reset */



/*==========================================================================
FUNCTION RXC_SC_ser_ACCUM

DESCRIPTION
  This function accumulates the status information for SYNC Channel.
  The status information includes CRC/Quality, Energy Metrics and SER

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void rxc_sc_ser_accum( void )
{
  rxc_sc_ser_total.frame_cnt++; /* Increment the sync channel frame count */
  rxc_sc_ser_total.ser += rxc_sc_status.ser;
}  /* rxc_sc_ser_accum */

/*===========================================================================

FUNCTION RXC_SER_RESET

DESCRIPTION
  This function resets the symbol error rate statistics.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void rxc_ser_reset( void )
{
  rxc_ser_total.frame_cnt = 0;
  rxc_ser_total.ser1 = 0;
  rxc_ser_total.ser2 = 0;
  rxc_ser_total.ser4 = 0;
  rxc_ser_total.ser8 = 0;

} /* rxc_ser_reset */


/*===========================================================================

FUNCTION RXC_GET_SC_CRC

DESCRIPTION
  This function retrieves the number of messages received on the Sync
  Channel with good and bad CRCs.

DEPENDENCIES
  None.

RETURN VALUE
  See above.

SIDE EFFECTS
  None.

===========================================================================*/

void rxc_get_sc_crc
(
  rxc_crc_cnt_type *crc_ptr
    /* pointer to where crc counters should be placed */
)
{
  crc_ptr->good_crcs = rxc_sc_crc_good;
  crc_ptr->bad_crcs = rxc_sc_crc_bad;

} /* rxc_get_sc_crc */


/*===========================================================================

FUNCTION RXC_GET_PC_CRC

DESCRIPTION
  This function retrieves the number of messages received on the Paging
  Channel with good and bad CRCs.

DEPENDENCIES
  None.

RETURN VALUE
  See above.

SIDE EFFECTS
  None.

===========================================================================*/

void rxc_get_pc_crc
(
  rxc_crc_cnt_type *crc_ptr
    /* pointer to where crc counters should be placed */
)
{
  crc_ptr->good_crcs = rxc_pc_crc_good;
  crc_ptr->bad_crcs = rxc_pc_crc_bad;

} /* rxc_get_pc_crc */


/*===========================================================================

FUNCTION RXC_WAIT

DESCRIPTION
  This function continues to reset the Watchdog signal and timer while
  waiting for any signal in a specified signal mask to be set. If this
  procedure detects that any signal in the specified signal mask is set,
  it will return the entire mask of signals set in the task'stcb.

DEPENDENCIES
  None.

RETURN VALUE
  Mask of signals returned by the rex_wait call.

SIDE EFFECTS
  None.

===========================================================================*/

rex_sigs_type rxc_wait
(
  rex_sigs_type wait_sigs
    /* signals to wait on */
)
{
  rex_sigs_type sigs;
  int isr_cmd_sig_wait_cnt = 0;
    /* signals returned by rex_wait call */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - - -*/

  /* keep kicking watchdog while waiting for requested signals */
  while ( ( (sigs = rex_get_sigs( rex_self()) ) & wait_sigs) == 0 )
  {
    if ((wait_sigs & RXC_ISR_CMD_SIG)
         && (sigs & RXC_RPT_TIMER_SIG)
         && (isr_cmd_sig_wait_cnt++ == 2))
    {
      /* 2 watchdog petting intervals have elapsed since we started
       * to wait. This should not happen, but for some reasons, the decoder 
       * ISR is not installed or the decoder HW can be frozeen, to avoid 
       * locked up, MUX resets DEC/DINT HW and install the ISR by itself. 
      */
      MSG_FATAL("No resp from ISR, avoid MC lockup, reset DEC/DINT, install the ISR",0,0,0);
#ifdef T_MSM7600
#error code not present
#else /* !T_MSM7600 */
      HWIO_OUT(VD_RESET, 0x00);
#endif
      HWIO_OUT(DINT_RESET, 0x00);
#ifdef FEATURE_IS2000_REL_A_CC
      rxc_idle_activate();
#else
      rxc_set_pc_isr ();
#endif /* FEATURE_IS2000_REL_A_CC */
    }

    /*  Check in with Watchdog task. */
    dog_report ( DOG_RX_RPT );
    (void)rex_set_timer ( &rxc_rpt_timer, DOG_RX_RPT_TIME );

    (void)rex_wait( wait_sigs | RXC_RPT_TIMER_SIG );
  }

  /* kick watchdog one more time on the way out */
  dog_report ( DOG_RX_RPT );
  (void)rex_set_timer ( &rxc_rpt_timer, DOG_RX_RPT_TIME );

  return( sigs );

} /* rxc_wait */


/*===========================================================================

FUNCTION RXC_MC_RPT

DESCRIPTION
  This function generates a report to the Main Control task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void rxc_mc_rpt
(
  mccrx_rpt_type *rpt_ptr
    /* pointer to report to be sent to the Main Control task */
)
{

  rpt_ptr->hdr.rpt_hdr.done_q_ptr = &mcc_rx_free_q;
  rpt_ptr->hdr.rpt_hdr.task_ptr = NULL;

  /* Enqueue report to Main Control task */
  q_put( &mcc_rx_rpt_q, &rpt_ptr->hdr.rpt_hdr.link );
  (void) rex_set_sigs( &mc_tcb, MCC_Q_SIG );

}/* rxc_mc_rpt */


/*===========================================================================

FUNCTION SEND_PWR_RPT

DESCRIPTION
  This function sends a power report to the Main Control task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void send_pwr_rpt( void )
{
  mccrx_rpt_type *rpt_ptr;
    /* Pointer to report to be sent to the Main Control task */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - - -*/

  if ((rpt_ptr = (mccrx_rpt_type *)q_get( &mcc_rx_free_q )) == NULL)
  {
    ERR( "0 bufs on mcc_rx_free_q", 0, 0, 0 );

    /* Reset counts */
    rxc_pwr.num_frames     = 0;
    rxc_pwr.num_bad_frames = 0;

#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
    rxc_pwr.dcch_num_frames     = 0;
    rxc_pwr.dcch_num_bad_frames = 0;
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */

  }
  else
  {
    /* Generate power measurement report for MC */
    rpt_ptr->pwr.hdr.rpt = RXC_PWR_R;

#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
    /* Report power measurements detected as follows:
    **
    ** FCH & DCCH:  Report measurements for FCH and DCCH separately.  Set dcch 
    **              measurement included flag to indicate reporting of both 
    **              channels.
    ** FCH Only:    Report power measurement for FCH only.  
    **              Indicate 1 channel reporting.
    ** DCCH Only:   Report power measurement for DCCH only.
    **              Indicate 1 channel reporting.
    */                
    rpt_ptr->pwr.pwr_meas_frames      = 0;
    rpt_ptr->pwr.errors_detected      = 0;
    rpt_ptr->pwr.dcch_pwr_meas_incl   = FALSE;
    rpt_ptr->pwr.dcch_pwr_meas_frames = 0;
    rpt_ptr->pwr.dcch_errors_detected = 0;
    if ( (rxc_so.for_fch.included) && (rxc_so.for_dcch.included) )
    {
      rpt_ptr->pwr.pwr_meas_frames      = rxc_pwr.num_frames;
      rpt_ptr->pwr.errors_detected      = rxc_pwr.num_bad_frames;
      rpt_ptr->pwr.dcch_pwr_meas_incl   = TRUE;
      rpt_ptr->pwr.dcch_pwr_meas_frames = rxc_pwr.dcch_num_frames;
      rpt_ptr->pwr.dcch_errors_detected = rxc_pwr.dcch_num_bad_frames;
    }
    else if (rxc_so.for_fch.included) 
    {
      rpt_ptr->pwr.pwr_meas_frames      = rxc_pwr.num_frames;
      rpt_ptr->pwr.errors_detected      = rxc_pwr.num_bad_frames;
    }
    /* Default to DCCH only scenario.  For this scenario, standard indicates 
    ** that power measurement report represents DCCH errors and 
    ** dcch_pwr_meas_incl is set to FALSE. 
    */
    else if (rxc_so.for_dcch.included) 
    {
      rpt_ptr->pwr.pwr_meas_frames      = rxc_pwr.dcch_num_frames;
      rpt_ptr->pwr.errors_detected      = rxc_pwr.dcch_num_bad_frames;
    }
#else /* else of FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */
    rpt_ptr->pwr.pwr_meas_frames = rxc_pwr.num_frames;
    rpt_ptr->pwr.errors_detected = rxc_pwr.num_bad_frames;
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */

    #ifdef FEATURE_IS2000_F_SCH
    rpt_ptr->pwr.sch_pwr_meas_included = FALSE;
    #endif // FEATURE_IS2000_F_SCH
    rxc_mc_rpt( rpt_ptr );

    /* Turn power control reporting off until PWR_REP_DELAY countdown */
    rxc_pwr.threshold_on = FALSE;
    rxc_pwr.periodic_on = FALSE;
  }
} /* send_pwr_rpt */



#if defined (FEATURE_IS95B_MAHHO) || defined (FEATURE_GPS_MODE)
/*===========================================================================

FUNCTION RXC_FLAG_GOOD_FRAME

DESCRIPTION
  This function is called by SRCH during a Return-If-Fail HHO, after
  verifying none of the HHO failure conditions is met.  This is to
  inform RXC that it's okay to start processing good frame.

DEPENDENCIES
  This function must not be called unless a return-if-fail hard handoff
  is in progress.

RETURN VALUE
  None.

SIDE EFFECTS
  RXC will start processing good frame

===========================================================================*/
void rxc_flag_good_frame( void )
{
  MSG_HIGH("SRCH: don't ignr ok fr %d, %d", rxc_lost_frames,
            rxc_rif_hho_timeout_frames,0);
  /* If the handoff has already timed out, leave this flag set to TRUE */
  if (rxc_lost_frames < rxc_rif_hho_timeout_frames)
  {
    rxc_ignore_good_frame = FALSE;
  }
}
#endif /* FEATURE_IS95B_MAHHO || FEATURE_GPS_MODE */


/*===========================================================================

FUNCTION RXC_ISR_CMD

DESCRIPTION
  This procedure posts a command for the interrupt handler, continues to
  kick the watchdog while waiting for a response and returns the response.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void rxc_isr_cmd
(
  rxc_isr_cmd_type *cmd_ptr
    /* pointer to command to be posted for the interrupt handler */
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* put command on queue */
  (void) rex_clr_sigs (&rx_tcb, RXC_ISR_CMD_SIG);
  q_put (&rxc_isr_q, &cmd_ptr->link);

  /* Clear buffers from decoder queue */
  rxc_clear_decoder_queue ();

  /* SRCH or MC could send MUX multiple cmds at a time, so if the queue is not empty
  the rxc task should go on and process the pending cmd in the queue
  */
  if (rxc_cmd_q.cnt == 0 )
  {
    /* Wait for interrupt handler to process command
    ** Also wait for a command from SRCH
    */
    (void) rxc_wait (RXC_ISR_CMD_SIG | RXC_CMD_Q_SIG);

    (void) rex_clr_sigs (rex_self(), RXC_ISR_CMD_SIG);
  }

} /* rxc_isr_cmd */


/*===========================================================================

FUNCTION RXC_CDMA

DESCRIPTION
  This function processes the state of the RXC state machine after the
  Decoder Chip has been initialized.  During this state the Receive task
  is waiting for a command to go the Sync Channel state or to Exit.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Next state to be processed.

===========================================================================*/

word rxc_cdma( void )
{

  word next_state;
    /* next state to be processed */
  rxc_cmd_msg_type *cmd_ptr;
    /* pointer to command buffer received on rxc_cmd_q */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  next_state = RXC_CDMA_STATE;
  while( next_state == RXC_CDMA_STATE )
  {
    /* check for received commands */
    (void)rex_clr_sigs( rex_self(), RXC_CMD_Q_SIG );

    if( (cmd_ptr = (rxc_cmd_msg_type *)q_get(&rxc_cmd_q ) ) != NULL )
    {
      switch( cmd_ptr->hdr.command )
      {

        case RXC_EXIT_F:
        {
          cmd_ptr->hdr.status = RXC_DONE_S;
          next_state = RXC_EXIT_STATE;

#if defined(FEATURE_RLP_LOGGING)
          /* If enabled, send accumulated RLP Frames log. */
          dsrlp_log_rx_send_frames();
#endif
#ifndef FEATURE_IS2000_REL_A
          /* If enabled, send accumulated frame types log. */
          rxc_frame_types_send_log();
#else
          rxc_fdch_frame_info_send_log();
#endif /* FEATURE_IS2000_REL_A */
#ifdef FEATURE_LOG_PC_INFO_2
          /* If enabled, send accumulated FFPC Info log. */
          ffpc_info_send_log();
#endif /* FEATURE_LOG_PC_INFO_2 */

          /* If enabled, send accumulated FFPC log. */
          ffpc_send_log();
          /* If enabled, send accumulated RDA log. */
          rxc_rda_frame_info_send_log();
          break;
        }

        case RXC_SC_F:
        {
          rxc_sc_init( RXC_CDMA_STATE);
          cmd_ptr->hdr.status = RXC_DONE_S;
          next_state = RXC_SC_STATE;
          break;
        }
#ifdef FEATURE_VOIP_1X_HANDOFF
        case RXC_IDLE_F:
        {
          if (cmd_ptr->idle.chan_info.chan_id == CAI_PCH) 
          {
            rxc_pc_init (RXC_CDMA_STATE, 
              cmd_ptr->idle.chan_info.config.pch.rate);
            cmd_ptr->hdr.status = RXC_DONE_S;
            next_state = RXC_PC_STATE;
            break;
          }
        }
#endif /* FEATURE_VOIP_1X_HANDOFF */
#ifdef FEATURE_FACTORY_TESTMODE
        case RXC_CDMA_F:
        {
          if(ftm_mode == FTM_MODE)
          {
            cmd_ptr->hdr.status = RXC_DONE_S;
            break;
          }
          /* fall through to the default: */
        }
#endif /* FEATURE_FACTORY_TESTMODE */

        default:
        {
#ifdef FEATURE_FACTORY_TESTMODE
          if(ftm_mode == FTM_MODE)
          {
          MSG_HIGH( "Bad RXC cmd %d", cmd_ptr->hdr.command ,0, 0 );
          } else {
            ERR_FATAL( "Bad cmd %d", cmd_ptr->hdr.command ,0, 0 );
          }
#else
          ERR_FATAL( "Bad cmd %d", cmd_ptr->hdr.command ,0, 0 );
#endif /* FEATURE_FACTORY_TESTMODE */
        }
      }/* switch */

      /* Place item on queue if requested */
      cmd_done( &cmd_ptr->hdr.cmd_hdr );
    }
    else
    {
      /* wait for command or interrupt */
      (void)rxc_wait( RXC_CMD_Q_SIG );
    }
  } /* while */

  return (next_state );

}/* rxc_cdma */



/*===========================================================================

FUNCTION RXC_VOTE_OK_TO_SLEEP

DESCRIPTION
  Indicates to sleep subsystem that MUX is voting ok to sleep (do tcxo
  shutdown)

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/

void rxc_vote_ok_to_sleep( void )
{
  sleep_assert_okts( rxc_sleep_handle );
} /* rxc_vote_ok_to_sleep */ 

/*===========================================================================

FUNCTION RXC_VOTE_NOT_OK_TO_SLEEP

DESCRIPTION
  Indicates to sleep subsystem that MUX is voting not ok to sleep (do not do
  tcxo shutdown)

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/

void rxc_vote_not_ok_to_sleep( void )
{
  sleep_negate_okts( rxc_sleep_handle );
} /* rxc_vote_not_ok_to_sleep */

/*===========================================================================

FUNCTION RXC_POWERUP_INIT

DESCRIPTION
  This function performs initialization of Receive task variables and
  queues.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Queues can be used.

===========================================================================*/

void  rxc_powerup_init( void )
{
  word i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set frame count to 0 */
  qw_set( rxc_dec_frame, 0L, 0L );

  /* initialize timers */
  rex_def_timer (&rxc_rpt_timer, &rx_tcb,  RXC_RPT_TIMER_SIG );
  rxcidle_def_timer();

  /* Initialize queues */
  (void)q_init( &rxc_cmd_q );
  (void)q_init( &rxc_isr_q );
  (void)q_init( &rxc_dec_q );
  (void)q_init( &rxc_dec_free_q );
  (void)q_init( &rxc_isr_free_q );

#if defined(FEATURE_DIAG_V7_LOG_IFACE)
  (void)q_init( &rxc_log_msg_q );
  (void)q_init( &rxc_log_mar_q );
#ifdef TMOBILE
#error code not present
#endif

#endif

  /* Fill rxc_dec_free_q */
  for( i=0; i< RXC_NUM_DEC_BUFS ; i++ )
  {
    q_put( &rxc_dec_free_q,
           q_link( &rxc_dec_bufs[i], &rxc_dec_bufs[i].link ) );
  }

  /* Fill rxc_isr_free_q */
  for( i=0; i< RXC_NUM_ISR_CMD_BUFS ; i++ )
  {
    q_put( &rxc_isr_free_q,
           q_link( &rxc_isr_cmd_bufs[i], &rxc_isr_cmd_bufs[i].link ) );
  }



#if defined(FEATURE_RLP_LOGGING)
  /* Initialize RLP frames logging buffer */
  dsrlp_log_rx_init_frm_buf();
#endif

#if !defined(FEATURE_DIAG_V7_LOG_IFACE)
  /* Initialize markov log */
  rxc_log_mar_buf.hdr.code = LOG_MAR_C;
  rxc_log_mar_buf.hdr.len  = FPOS(log_mar_type, entry);//lint !e734 FPOS
  rxc_log_mar_buf.count    = 0;

#else

  /* Fill rxc_log_mar_q */
  for ( i = 0; i < RXC_NUM_MAR_BUFS; i++ )
  {
    rxc_log_mar_pool[ i ].desc.q_ptr   = &rxc_log_mar_q;
    rxc_log_mar_pool[ i ].desc.log_ptr = &rxc_log_mar_pool[ i ].entry.hdr;
    rxc_log_mar_pool[ i ].entry.hdr.len =
                                    sizeof( rxc_log_mar_pool[i].entry );
    rxc_log_mar_pool[ i ].entry.hdr.code = LOG_MAR_C;

    q_put ( &rxc_log_mar_q,
            q_link ( &rxc_log_mar_pool[ i ],
                     &rxc_log_mar_pool[ i ].desc.link ) );
  }


  /* Fill rxc_log_msg_q */
  for ( i = 0; i < RXC_NUM_MSG_BUFS; i++ )
  {
    rxc_log_msg_pool[ i ].desc.q_ptr   = &rxc_log_msg_q;
    rxc_log_msg_pool[ i ].desc.log_ptr = &rxc_log_msg_pool[ i ].entry.hdr;

    q_put ( &rxc_log_msg_q,
            q_link ( &rxc_log_msg_pool[ i ],
                     &rxc_log_msg_pool[ i ].desc.link ) );
  }

  /* Fill rxc_log_voc_q */
#ifdef TMOBILE
#error code not present
#endif

  /* initialize markov buffer pointer */
  rxc_mar_buf_ptr = NULL;
  rxc_mar_cnt = 0;

#endif /* !defined(FEATURE_DIAG_V7_LOG_IFACE) */

#ifdef FEATURE_PLT
#error code not present
#endif

  /* register to DIAG all the MUX1X command handlers */
  mux1xdiag_init();

  /* register with sleep for voting for/against tcxo shutdown, vote okts */
  rxc_sleep_handle = sleep_register( "RXC", TRUE );

}/* rxc_powerup_init */

/* <EJECT> */
/*===========================================================================

FUNCTION RXC_EXIT

DESCRIPTION
  This function performs processing necessary when leaving the RX CDMA
  subtask

DEPENDENCIES
  none.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rxc_exit( void )
{
  /* set interrupt handler to null */
  rxc_clear_isr();

  /* kick watchdog one more time */
  dog_report ( DOG_RX_RPT );

  /* clear timers */
  (void)rex_clr_timer(&rxc_rpt_timer);
  rxcidle_clr_fade_timer();

  /* clear signals */
  (void)rex_clr_sigs(&rx_tcb, 0xFFFF );

  /* Make sure page matching is completely disabled */
  rxc_slot.skipping   = FALSE;
  rxc_slot.page_match = FALSE;
  rxc_slot.blank      = FALSE;

  ffpc_disable ();

  /* If FFPC logging enabled ... */
  if (log_status(LOG_FFWD_PCTRL_C))
  {
    /* flush out accumulated logs */
    ffpc_send_log();
  }
#ifdef FEATURE_LOG_PC_INFO_2
  /* If FFPC logging enabled ... */
  if (log_status(LOG_FFPC_INFO_C))
  {
    /* flush out accumulated logs */
    ffpc_info_send_log();
  }
#endif /* FEATURE_LOG_PC_INFO_2 */

} /* rxc_exit */


/*===========================================================================

FUNCTION RXC_SUBTASK

DESCRIPTION
  This function contains the state machine for the CDMA rx subtask.

DEPENDENCIES
  none.

RETURN VALUE
  None.

SIDE EFFECTS
  Receive task will start.

===========================================================================*/

void rxc_subtask()
{

  rxc_cmd_msg_type *cmd_ptr;
    /* pointer to command from MC */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  (void)rex_clr_sigs(&rx_tcb, RXC_CMD_Q_SIG );
  if( (cmd_ptr = (rxc_cmd_msg_type *)q_get(&rxc_cmd_q)) == NULL )
  {
    ERR_FATAL("RXC subtsk act err",0,0,0 );
  }
  else
  {
    /* Due to the exit sequence RXC->TXC->SRCH which is introduced by 
       the srch inactive state, rxc may received the RXC_CHAN_CONFIG_F 
       for the idle handoff from SRCH; the command is ignored here and 
       the function is return to avoid fatal error */

    if( cmd_ptr->hdr.command != RXC_CDMA_F 
#ifdef FEATURE_IS2000_REL_A_CC
        && cmd_ptr->hdr.command != RXC_CHAN_CONFIG_F
#endif
      )
    {
      ERR_FATAL("RXC subtsk act err",0,0,0 );
    }
    else
    {
      /* Begin CDMA subtask processing */
      cmd_ptr->hdr.status = RXC_DONE_S;
      cmd_done( &cmd_ptr->hdr.cmd_hdr );

#ifdef FEATURE_IS2000_REL_A_CC
      if (cmd_ptr->hdr.command == RXC_CHAN_CONFIG_F)
      {
        rxc_state = RXC_EXIT_STATE;
        MSG_HIGH("Unexp RXC_CHAN_CONFIG_F, exit", 0, 0, 0);
      }
      else  
#endif
      {
      rxc_state = RXC_CDMA_STATE;
      /* Enable decoder clock upon entering CDMA Substate */
      clk_regime_enable(CLK_RGM_DEC_M);
      }

      while( rxc_state != RXC_EXIT_STATE )
      {

        switch( rxc_state )
        {
          case RXC_CDMA_STATE:
          {
            rxc_state = (rxc_state_type)rxc_cdma();  /* CDMA state */
            break;
          }

          case RXC_SC_STATE:
          {
            rxc_state = (rxc_state_type)rxc_sc(); /* Process Sync Chan. data */
            break;
          }

          case RXC_PC_STATE:
          {
            /* Process Paging Chan data */
            rxc_state = (rxc_state_type)rxc_pc();
            break;
          }

#ifdef FEATURE_IS2000_REL_A_CC
#ifdef FEATURE_IS2000_REL_A_CC_SMMC
          case RXC_NCCCH_STATE:
          {
            /* Monitor the both BCCH and FCCCH*/
            rxc_state = rxcnccch_state();
            break;
          }
#endif /* FEATURE_IS2000_REL_A_CC_SMMC */
          case RXC_BCCH_STATE:
          {
            /* Monitor the BCCH */
            rxc_state = rxcbcch_state();
            break;
          }

          case RXC_FCCCH_STATE:
          {
            /* Monitor the FCCCH */
            rxc_state = rxcfccch_state();
            break;
          }
#endif 
          case RXC_TCI_STATE:
          {
            /* Process Traffic Channel initialization */
            rxc_state = (rxc_state_type)rxc_tci();
            break;
          }

          case RXC_TC_STATE:
          {
            /* Process Traffic Channel data */
            rxc_state = (rxc_state_type)rxc_tc();
            break;
          }

          default:
          {
            ERR_FATAL( "Bad state %d", rxc_state, 0, 0 );
          }

        } /* switch */

      } /* while */

      /* Clear decoder ISR before disabling clock regime in case there
      ** was a pending decoder interrupt which gets serviced after we
      ** turn off the clock.  
      */
      rxc_clear_isr();

      /* Disable decoder clock when exiting CDMA state to save power.
      ** Only do this if 1x is using TXRX0.  If it is using RX1, leave
      ** the clock as is (for HDR.)     
      */
      if ( srch_mux_owns_tx() )
      {
      clk_regime_disable(CLK_RGM_DEC_M);
      }
      rxc_exit();  /* perform exit processing */

      /* RXC no longer needs TCXO.  This allows us to enter deep sleep */
      rxc_vote_ok_to_sleep();
    }
  }
} /* rxcdma_subtask */


/*===========================================================================

FUNCTION RXC_CMD

DESCRIPTION
  This function should be called to send a command to the Receive task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Receive task will start.

===========================================================================*/

void rxc_cmd
(
  rxc_cmd_msg_type *cmd_ptr
    /* pointer to command to be enqueued */
)
{
  cmd_ptr->hdr.status = RXC_BUSY_S;

  /* Initialize link field */
  (void) q_link( cmd_ptr, &cmd_ptr->hdr.cmd_hdr.link );

  /* Put item on queue for rxc_task and set signal for task */
  q_put( &rxc_cmd_q, &cmd_ptr->hdr.cmd_hdr.link );
  (void)rex_set_sigs( &rx_tcb, RXC_CMD_Q_SIG );

} /* rxc_cmd */


/*===========================================================================

FUNCTION RXC_CLEAR_ISR

DESCRIPTION
  This function sets the current isr pointer to NULL.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void rxc_clear_isr ( void )
{
  /* turn off interrupts */
  tramp_set_isr(TRAMP_RX_ISR, NULL );

  (void)rex_clr_sigs( rex_self(), RXC_INT_SIG );
}


/*===========================================================================

FUNCTION RXC_LOAD_PC_ISR

DESCRIPTION
  This externally-called function installs the PC ISR and controls
  whether or not the decoder should have its state metrics
  cleared until the first frame in the slot.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void rxc_load_pc_isr ( boolean blank_frame )
{
  /* Decoder clk is CHIPX8 in paging state */
  dec_clk_select (DEC_CLK_CHIPX8);
  INTLOCK ();
  /* Install the interrupt handler - now! */
  tramp_set_isr( TRAMP_RX_ISR, rxc_pc_isr);
  /* Clear DEC DONE int status to make sure ISR is properly serviced.
  */
  HWIO_OUT(DEC_DONE_INT_STATUS, 0x7);
  INTFREE ();

  /* Remember whether we should clear path metrics or not. */
  rxc_slot.blank = blank_frame;
}



/*========================================================================

FUNCTION RXC_POWER_COMB_CONFIG

DESCRIPTION This function configures the power control puncturing position
            and PUNCT_POS_SEL based on Radio Configuration

DEPENDENCIES Power control and puncturing must be enabled before.

RETURN VALUE None

SIDE EFFECTS None

=========================================================================*/

void rxc_power_comb_config(deint_rc_type rc)
{

  switch(rc)
  {
    case DEINT_RC1:
    {
      /* Note that the mechanism used to set the bits in this register
       * was not portable so I had to change it.
       */
      MUXMSM_RPC_SET_POWER_CTL( FALSE,  /* no power_ctl_pct */ 
                                 TRUE,   /* set punc_pos_sel */
                                 FALSE); /* no pc_even */
      break;
    }
    case DEINT_RC2:
    {
      MUXMSM_RPC_SET_POWER_CTL( TRUE,   /* set power_ctl_pct */ 
                                 TRUE,   /* set punc_pos_sel */
                                 FALSE); /* no pc_even */
      break;
    }
    case DEINT_RC3:
    {
      MUXMSM_RPC_SET_POWER_CTL( FALSE,   /* no power_ctl_pct */ 
                                 FALSE,   /* no punc_pos_sel */
                                 FALSE);  /* no pc_even */
      break;
    }
    case DEINT_RC4:
    {
      MUXMSM_RPC_SET_POWER_CTL( FALSE,   /* no power_ctl_pct */ 
                                 TRUE,    /* set punc_pos_sel */
                                 TRUE);   /* set pc_even */
      break;
    }
    case DEINT_RC5:
    {
      MUXMSM_RPC_SET_POWER_CTL( FALSE,   /* no power_ctl_pct */ 
                                  FALSE,   /* no punc_pos_sel */
                                  FALSE);  /* no pc_even */
      break;
    }
    default:
      MSG_FATAL("Bad RC ", 0,0,0);
      break;
  }
}
/*========================================================================

FUNCTION RXC_CONFIG_DEMOD

DESCRIPTION This function configures the demodulator and combiner based on
            Radio Configuration

DEPENDENCIES ??

RETURN VALUE None

SIDE EFFECTS None

=========================================================================*/
void rxc_config_demod( deint_rc_type rc )
{
  MSM_LOCK();
  rxc_power_comb_config( rc );

  if ( rc >= DEINT_RC3 )
  {
     DEMOD_ENABLE_Q_CHANNEL();

     if( rc == DEINT_RC4 )
     {
       COMBINER_SELECT_CH0_128_CHIP_SYMBOL(); /* Tell the combiner to accumulate
                                             symbols over 128 pn chips */
       DEMOD_SET_CH0_TD_SIGN( 0x3 );
       DEMOD_ENABLE_CH0_RC4( );
       /* for RC4, the beta is 0x17 */
       DEMOD_SET_SW_BETA_CH0_IMMED(0x17);
     }
     else
     {
       COMBINER_SELECT_CH0_64_CHIP_SYMBOL();  /* Tell the combiner to accumulate
                                             symbols over 64 pn chips */
       DEMOD_SET_CH0_TD_SIGN( 0x5 );
       DEMOD_DISABLE_CH0_RC4( );
       /* for RC3, the beta value is 0x20 */
       DEMOD_SET_SW_BETA_CH0_IMMED(0x20);
     }
  }
  else
  {
     DEMOD_DISABLE_Q_CHANNEL();          /* Disable Q channel for RC1 and RC2 */
     COMBINER_SELECT_CH0_64_CHIP_SYMBOL();   /* Tell the combiner to accumulate
                                            symbols over 64 pn chips */
     DEMOD_SET_CH0_TD_SIGN( 0x0 );
     DEMOD_DISABLE_CH0_RC4( );
     /* for RC1 and RC2, the beta value is 0x20 */
     DEMOD_SET_SW_BETA_CH0_IMMED(0x20);
  }
  MSM_UNLOCK();
}
#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
/*========================================================================

FUNCTION RXC_CONFIG_DEMOD_CH1

DESCRIPTION This function configures the demodulator and combiner for
            Channel 1 based on Radio Configuration

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS None

=========================================================================*/
void rxc_config_demod_ch1 ( deint_rc_type rc )
{
  ASSERT ((rc == DEINT_RC3) || (rc == DEINT_RC4) || (rc == DEINT_RC5));
  MSG_HIGH ("Config MAC engine for traf Ch1" ,0,0,0);
  MSM_LOCK();

  DEMOD_ENABLE_CH1_HW_COMBINING ();
  DEMOD_SET_MAC_RND_128_64_CHIPS_CH1 (DMOD_COMB_RND_CONVOLUTIONAL);

  if( rc == DEINT_RC4 )
  {
    DEMOD_SET_CH1_TD_SIGN (0x3);
    DEMOD_SET_SW_BETA_CH1 (0x5B);
    DEMOD_SET_CH1_SPR (SPR_128_CHIP_V);
  }
  else
  {
    DEMOD_SET_CH1_TD_SIGN (0x5);
    DEMOD_SET_SW_BETA_CH1 (0x80 );
    DEMOD_SET_CH1_SPR (SPR_64_CHIP_V);
  }
  MSM_UNLOCK();
} /* end rxc_config_demod_ch1 */

/*========================================================================

FUNCTION RXC_SET_DCCH_MODE

DESCRIPTION This function handles the process of setting up DCCH.

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS None

=========================================================================*/
void rxc_set_dcch_mode
( 
  deint_rc_type dcch_rc,
  boolean power_control_channel 
)
{
  /* Since there are 2 frames lag time between when the DCCH is set up and 
  ** when data is valid, need to count down these 2 frames.  Without this
  ** mechanism, MUX may deliver stale data to its clients.  The time line is
  ** shown below:
  **    Frame 0:  Start Deint programming by calling deint_set_dcch_mode()
  **    Frame 1:  In ATMR ISR, decrement countdown counter
  **    Frame 2:  In rxc_tc_isr, if count down counter is 1, decrement it
  **              and set data_valid flag.
  ** If the current data is already valid (not new channel, may be an RC change),
  ** do not activate the count down mechanism.
  */
  if (!(rxc_so.for_dcch.data_valid))
  {
    rxc_so.for_dcch.data_valid_countdown = 2;
  }
  deint_set_dcch_mode (dcch_rc, power_control_channel);

} /* end rxc_set_dcch_mode */

/*========================================================================

FUNCTION RXC_SET_FCH_MODE

DESCRIPTION This function handles the process of setting up FCH.

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS None

=========================================================================*/
void rxc_set_fch_mode
( 
  deint_rc_type fch_rc,
  boolean power_control_channel 
)
{
  /* Since there are 2 frames lag time between when the FCH is set up and 
  ** when data is valid, need to count down these 2 frames.  Without this
  ** mechanism, MUX may deliver stale data to its clients.  The time line is
  ** shown below:
  **    Frame 0:  Start Deint programming by calling deint_set_fch_mode()
  **    Frame 1:  In ATMR ISR, decrement countdown counter
  **    Frame 2:  In rxc_tc_isr, if count down counter is 1, decrement it
  **              and set data_valid flag.
  ** If the current data is already valid (not new channel, may be an RC change),
  ** do not activate the count down mechanism.
  */
  if (!(rxc_so.for_fch.data_valid))
  {
    rxc_so.for_fch.data_valid_countdown = 2;
  }
  deint_set_fch_mode (fch_rc, power_control_channel);

} /* end rxc_set_fch_mode */

#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */

/*===========================================================================

FUNCTION RXC_CLEAR_DECODER_QUEUE

DESCRIPTION
  This function clears the RXC decoder queue by placing all items on the
  decoder free queue.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rxc_clear_decoder_queue (void)
{
  rxc_dec_type *buf_ptr = NULL;
  
  while ((buf_ptr = (rxc_dec_type *) q_get(&rxc_dec_q)) != NULL) {
    q_put (&rxc_dec_free_q, &buf_ptr->link);
  }
}

/*===========================================================================

FUNCTION RXC_CLEAR_ISR_CMD_QUEUE

DESCRIPTION
  This function clears the RXC ISR COMMAND queue by placing all items on the
  ISR COMMAND free queue.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rxc_clear_isr_cmd_queue (void)
{
  while ((rxc_isr_cmd_buf_ptr = (rxc_isr_cmd_type *) q_get (&rxc_isr_q)) != NULL) 
  {
    /* Free up the buffer for the ISR */
    q_put (&rxc_isr_free_q, &rxc_isr_cmd_buf_ptr->link);
  }
}

#ifdef FEATURE_DS_IS2000
/*========================================================================

FUNCTION RXC_GET_SCH_INFO

DESCRIPTION This function returns current configuration information for
            the SCHs. This is used by the UI in the data display.

            PARAMETERS:
            sch_rc       - gives the Radio configuration of the R-SCH
            sch_rate_ptr - points to the current SCH rate.
            double_ptr   - is TRUE when double sized PDUs are being used
                           is FALSE when single sized PDUs are being used


DEPENDENCIES None

RETURN VALUE TRUE:  If SCH are assigned.
             FALSE: If no SCH are assigned.

SIDE EFFECTS None

=========================================================================*/
boolean rxc_get_sch_info
(
  dec_rc_type* sch_rc,         /* ptr to SCH RC */
  dec_sch_rate_type* sch_rate_ptr,
  boolean* double_ptr
)
{
  #ifdef FEATURE_IS2000_SCH
  /* If Supps are active, set the rate and double size PDUs */
  if ((deint_rc_type)rxc_sch.rc != DEINT_RC_INVALID)
  {
    *sch_rate_ptr = (dec_sch_rate_type) rxc_stored_sch_packet.sch_rate;
    *double_ptr = rxc_stored_sch_packet.double_size_pdu;
    *sch_rc = (dec_rc_type) rxc_stored_sch_packet.rc;
    return( TRUE);
  }
  else
  #endif
  {
    /* Supps are not active, so return FALSE */
    return( FALSE);
  }

} /* rxc_get_sch_info() */

/*========================================================================

FUNCTION RXC_GET_FCH_RATE_SET

DESCRIPTION This function returns current Rate set used by FCH.
            This is used by the UI in the data display.

            PARAMETERS:
            None

DEPENDENCIES None

RETURN VALUE

SIDE EFFECTS None

=========================================================================*/
dec_rate_set_type rxc_get_fch_rate_set(void )
{
  return (fwd_rc_to_rate_set[rxc_so.for_fch.rc]);
}
#endif  /* FEATURE_DS_IS2000 */

#ifdef FEATURE_IS2000_P2
/*========================================================================

FUNCTION RXC_GET_DCCH_RATE_SET

DESCRIPTION This function returns current Rate set used by FCH.
            This is used by the UI in the data display.

            PARAMETERS:
            None

DEPENDENCIES None

RETURN VALUE
           Rate set of the DCCH.

SIDE EFFECTS None

=========================================================================*/
dec_rate_set_type rxc_get_dcch_rate_set(void )
{
  return (fwd_rc_to_rate_set[rxc_so.for_dcch.rc]);
}

/*===========================================================================

FUNCTION RXC_DCCH_IS_ON

DESCRIPTION
  Returns if DCCH is ON.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: If DCCH is On
  FALSE: If DCCH is Off

SIDE EFFECTS
  None
===========================================================================*/
boolean rxc_dcch_is_on(void )
{
  if (rxc_so.for_dcch.included)
  {
      return TRUE;
  }
  else
  {
      return FALSE;
  }

}
#endif  /* FEATURE_IS2000_P2 */

#ifdef FEATURE_JCDMA_MONITOR
/* <EJECT> */
/*===========================================================================

FUNCTION RXC_GET_RATE_SET

DESCRIPTION
  This function returns the current Receive task rate set in use.

DEPENDENCIES
  None.

RETURN VALUE
  Current Rate set.

SIDE EFFECTS
  None.

===========================================================================*/

dec_rate_set_type rxc_get_rate_set( void )
{

  return( rxc_so.rate_set );

}/* rxc_get_rate_set */

#endif /* FEATURE_JCDMA_MONITOR */

#ifdef FEATURE_IS2000_REL_A_CC
/*===========================================================================

FUNCTION RXC_GET_CHAN_DEMOD_CONFIG_PARAM

DESCRIPTION
  This function returns the demod setup parameters for FCCCH/BCCH.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if paramaters could be found; otherwise, FALSE 

SIDE EFFECTS
  None.

===========================================================================*/
extern rxcbcch_info_type   rxcbcch_info;
extern rxcfccch_info_type   rxcfccch_info;

boolean rxc_get_chan_demod_config_param
( 
  /* pointer to channel info (typically from a chan config msg) */       
  rxc_idle_chan_info_type          *chan_info,  

  /* pointer to struct to return info in */
  rxc_chan_demod_config_param_type *demod_params 
)
{
  boolean status = FALSE;

  /* check if the request is for BCCH */
  switch(chan_info->chan_id)
  {
    case CAI_BCCH:
      /* valid data and code rate */
      if ((chan_info->config.bcch.crate < CAI_CODE_RATE_COUNT) && 
          (chan_info->config.bcch.rate  < CAI_BCCH_RATE_COUNT))
      {
        /* look up the table and store in the output pointer */
        *demod_params = 
          rxc_bcch_demod_config_param_tab[chan_info->config.bcch.crate]
                                         [chan_info->config.bcch.rate];

        status = TRUE;
      }
      break;

    case CAI_F_CCCH:
      /* check for valid data and code rate */
      if ((chan_info->config.fccch.crate < CAI_CODE_RATE_COUNT) && 
          (chan_info->config.fccch.rate  < CAI_FCCCH_DATA_RATE_INVALID))
      {
        /* look up the table and store in the output pointer */
        *demod_params = 
          rxc_fccch_demod_config_param_tab[chan_info->config.fccch.crate]
                                          [chan_info->config.fccch.rate];

        status = TRUE;
      }
      break;

    /* other channels are not support at the moment */
    default:
      status = FALSE;
  }

  MSG_6(MSG_SSID_DFLT,MSG_LEGACY_HIGH,
    "chan %d, cr %d, dr %d => spr %d, rep %d, rnd %d",
    chan_info->chan_id,
    chan_info->config.bcch.crate,
    chan_info->config.bcch.rate,
    demod_params->spr_rate,
    demod_params->rep_factor,
    demod_params->rnd_factor);

  return status;
} /* rxc_get_chan_demod_config_param */

#endif /* FEATURE_IS2000_REL_A_CC */

/*===========================================================================

FUNCTION RXC_GET_CHAN_TC_DEMOD_CONFIG_PARAM

DESCRIPTION
  This function returns the demod setup parameters for SCH.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if paramaters could be found; otherwise, FALSE 

SIDE EFFECTS
  None.

===========================================================================*/

boolean rxc_get_chan_tc_demod_config_param
( 
  /* pointer to channel info */
  rxc_tc_chan_info_type          *chan_info,

  /* pointer to struct to return info in */
  rxc_chan_demod_config_param_type *demod_params
)
{
  boolean               status = FALSE;
  deint_rc_type         rc;
  cai_sch_rate_type     sch_rate;       // 1X, 2X, 4X, 8X, 16X

  /* check what channel the request is for */
  switch(chan_info->chan_id)
  {
    case RXC_F_SCH:

      /* Copy data to local vars */
      rc        = (deint_rc_type) chan_info->config.sch.rc;
      sch_rate  = chan_info->config.sch.sch_rate;

      ASSERT ((rc == DEINT_RC3) || (rc == DEINT_RC4) || (rc == DEINT_RC5));

      if ( rc < DEINT_RC3 )
      {
        /* Return bad status */
        status = FALSE;

        MSG_ERROR("Call with bad RC %d", rc, 0, 0);
        break;
      }

      /* Update spreading rate from table */
      demod_params->spr_rate   =
        rxc_spr_rate_tab[DEINT_SCH_RC_INDEX(rc)][RXC_UPPER_BOUNDS_CHECK(sch_rate, NUM_SPR_RATES)];
      /* Update rounding factor from table */
      demod_params->rnd_factor =
        rxc_rnd_factor_tab[DEINT_SCH_RC_INDEX(rc)][RXC_UPPER_BOUNDS_CHECK(sch_rate, NUM_RND_FACT)];

      /* Repetition factor is always 1 */
      demod_params->rep_factor = DMOD_REP_FACTOR_1;

      #ifdef FEATURE_32X_SCH_DATA_RATE
      /* Check for bad combo of RC and rate */
      if ( ((rc == DEINT_RC3) || (rc == DEINT_RC5)) &&
           ((dec_sch_rate_type) sch_rate == DEC_SCH_RATE_32X) )
      {
        /* Return bad status */
        status = FALSE;

        MSG_ERROR("Bad comb: RC %d and Rate %dx",
                  DEINT_SCH_RC_INDEX(rc), 1<<sch_rate, 0);
      }
      else
#endif /* FEATURE_32X_SCH_DATA_RATE */
      {
        status = TRUE;
      }

      break;

    case RXC_F_PDCCH0:
    case RXC_F_PDCCH1:

      /* PDCCH parms are fixed, for now */
      demod_params->spr_rate   = SPR_64_CHIP_V;
      demod_params->rnd_factor = 3;

      status = TRUE;

      break;

    case RXC_F_GCH0:
    case RXC_F_GCH1:

      /* GCH parms are fixed, for now */
      demod_params->spr_rate   = SPR_256_CHIP_V;
      demod_params->rnd_factor = 3;

      status = TRUE;

      break;

    /* other channels are not supported at the moment */
    default:
      status = FALSE;
  }

  MSG_7(MSG_SSID_DFLT,MSG_LEGACY_HIGH,
    "chan %d, rc %d, dr %d, cr %d => spr %d, rep %d, rnd %d",
    chan_info->chan_id,
    chan_info->config.sch.rc,
    chan_info->config.sch.sch_rate,
    chan_info->config.sch.code_type,
    demod_params->spr_rate,
    demod_params->rep_factor,
    demod_params->rnd_factor);

  return status;
} /* rxc_get_chan_tc_demod_config_param */

/*===========================================================================

FUNCTION RXC_DEC_VERSION

DESCRIPTION
  Wrapper function for dev_version().
  Return the hardware version number for this circuit.

DEPENDENCIES
  None

RETURN VALUE
  Returns the version number of the circuit

SIDE EFFECTS
  None

===========================================================================*/
byte rxc_dec_version
(
  void
)
{
  return 0;
}

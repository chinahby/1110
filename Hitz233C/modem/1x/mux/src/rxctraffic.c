/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            C D M A    R E C E I V E   T R A F F I C    M O D U L E

GENERAL DESCRIPTION
  Items associated with traffic channels. Action timers and transaction
  processing

EXTERNALIZED FUNCTIONS
  rxc_frame_types_send_log -  Terminate and send the accumulated forward
  frame types log.
  rxc_tci - Traffic Channel Initialization processing state
  rxc_tc - Traffic Channel processing state
  rxc_log_mar - logs the Forward Traffic Channel Markov test data
  rxc_tc_isr - interrupt handler for the Decoder interrupt used
  when the Receive task is processing the Traffic Channel
  rxc_rlp_frame_log_init - initializes the next frame packet, as well
  as the pointer to it, in the RLP frames log buffer
  rxc_log_rlp_statistics - Logs the current values of the RLP statistics
  to the off-target diagnostic tool
  pwr_msr_check - processes the updated rxc_pwr structure to determine if a
  power measurement report should be sent to MC
  rxc_log_frame_type - populates frame type logs
  rxc_tc_parse - collects the signaling bits of the Traffic Channel
  rxc_tci_init - initializes the Traffic Channel initialization state
  fade_check - processes frame rate decision and updates the fade
  good and bad frame counts, determine if fade report should be sent to
  MC.


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 1990 through 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: L:/src/asw/COMMON/vcs/rxctraffic.c_v   1.115   16 Oct 2002 16:34:18   randyp  $
$Header: //source/qcom/qct/modem/1x/mux/rel/ulc/src/rxctraffic.c#6 $ $DateTime: 2009/03/24 10:30:17 $ $Author: joshuam $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/09/09   tjc     Update SRCH when the current SO specifies a data call
08/11/08   jtm     Restored debugging variable saved_dec_int_task_id.
07/24/08   jtm     Corrected the function rxc_configure_ffpc_channel().
07/16/08   jtm     Removed FEATURE_IS2000_REL_C_DV and FEATURE_IS2000_REL_D_DV.
                   Replaced macro RXC_SEND_MAR_LOG with function rxc_send_mar_log()
                   to reduce ROM size and reduced debug string sizes for ULC 
                   effort. Replaced code within rxc_tci_init() with function 
                   rxc_configure_ffpc_channel() since it is duplicated elsewhere.
                   Added debug strings for monitoring commands.
11/29/07   vlc     Fixed lint errors.
02/23/07   vlc     Fixed compiler warnings/errors when SCH, Rel A, and P2 
                   features are turned off.
11/22/06   vlc     Fixed RVCT compiler warnings.
09/19/06   vlc     Fixed lint errors.
07/13/06   vlc     In rxc_hho_begin_cmd(), reset lost frame count if the RIF
                   timer is set.
01/26/04   vlc     Added code to increment T79 timer to fade_check_fch() and
                   fade_check_dcch() under FEATURE_IS2000_REL_D.
06/13/06   vlc     In rxc_tc_isr(), added check for decoder rates.  In case
                   decoder rates are invalid, perform rate determination 
                   algorithm.
06/12/06   awj     #include tramp.h; no longer included through clk.h
06/09/06   trc     Remove T_MSM6700 featurization
05/08/06   vlc     Added return if failure indicator to HHO begin action.  This
                   change is required so the return if failure timer won't 
                   be activated in cases where return if failure is FALSE.
04/17/06   vlc     Added debug information in case RXC time line is extended over
                   20ms frame.
09/08/05   ejv     Use new srch_mux interface calls.
08/29/05   bn      Clear the all the decoder status it after installing the isr
07/26/05   rkc     Clear all service options upon reception of RXC_RELEASE_F.
07/25/05   rkc     Fixed lint errors.
07/18/05   vlc     Modified RXC to not allow GPS tune away if RXC is in fade 
                   condition for 12 consecutive frames.  Also check TX enable 
                   flag to make the check for GPS more strict, to avoid 
                   possible race condition between MC, GPS Search and TXC/RXC.
05/09/05   vlc     Added check for running out of DSM buffers for rel A branch.  
                   If the number of DSM buffers obtained is less than number 
                   requested, return all DSM buffers and try again next frame.
04/29/05   vlc     Added 4GV support under FEATURE_VOC_4GV.
04/27/05   vlc     Main lined code in current targets, cleaned out obsolete targets,
                   cleaned out obsolete features.                                   
04/08/05   fc/vlc  Added support for FEATURE_IS2000_REL_D TCI to IDLE state
                   transition (for DCA - Direct Channel Assignment).
03/14/05   vlc     Fixed compiler warnings for MSM6700.
02/24/05   vlc     Lock interrupt between clearing DEC_DONE_INT_STATUS and
                   installing RXC ISR.
02/23/05   vlc     Clear the DEC_DONE_INT_STATUS when setting RXC ISR.
01/26/04   vlc     Moved code to increment T79 timer to fade_check() to keep accurate
                   count of frames, rather than in fade_check_bad_frame().
01/26/05   vlc     Removed obsolete ds.h inclusion.
01/25/05   jyw     Added featurization to cover rxcpdcch_isr.
12/07/04   ejv     Added rxc_spr_rate_tab and rxc_rnd_factor_tab.
11/12/04   bn      MUX no longer need to call srch_mux_update_sch_walsh_len(). 
                   the new interface, SRCH has the walsh len info
11/09/04   vlc     Added support for Rel D Fast Call Set Up Bypass 2 Good Frame
                   feature.
09/28/04   sr      Merged in changes from 6550 2009 build
09/03/04   jyw     Added the relD plt support
09/01/04   jrp     Added call to muxdvlog_process_frame_boundary() for DMSS build.
08/17/04   jyw     Added the rxcpdch_plt_process_pdch in the PLT featurization
08/11/04   vlc     In TCI state, only free traffic channel resources if RXC
                   fails to go to TC state.
08/09/04   sr      Added line to rxc_config_sch_hw() to clear PDCCH bit in 
                   DEM1X_CHANNEL2 register for MSM6700
08/06/04   sr/va   Set rxc_sent_bad_rpt to TRUE unconditionally in 
                   rxc_hho_begin_cmd()
08/06/04   vlc     Added code to free traffic channel resources when existing 
                   TCI state (similar to TC state.)
07/30/04   vlc     Added support for rel C TDSO.
07/27/04   jrp     Fixed casting problem for 7500 compiler.
06/24/04   sr      Added 2nd level interrupt support for dec_done_int in ISR 
                   plus misc clean up for DV
06/24/04   sr      Put back rxc_clear_rlp_statistics()
06/16/04   sr      Merged in changes from 6500 5004 build
06/14/04   sr/va   Added HHO cleanup under FEATURE_HHO_ENHANCEMENTS
06/01/04   vlc     Added support for F-PDCH.  Changed function rxc_log_tc_msg()
                   to account for the addition of RXC_FOR_PDCH in rxc_dsch_type.
05/13/05   bn      Fixed errors in PDU5 parser functions
05/12/04   vlc     Added MCTRANS_MAX_ACTION under feature FEATURE_TRANS_MANAGER.
05/07/04   vlc     Implemented new RLP interface for EV-DV.  The new interface 
                   replaces the current interface for 1x channels as well.
                   Also, modified RLP logging extensively to work with new 
                   interface.
                   Added MCTRANS_MAX_ACTION.
05/06/04   ll      Fixed setpoint adjustment problem after CFS.
04/28/04   jrp     Reverted change of MSM_LOCK to INTLOCK_SAV to fix compilation
                   error with new search headers.
04/26/04   sr      Added rxcpdch initialization
03/16/04   sr      Made TEMP_HACKS_FOR_DV depend on T_MSM6700
04/27/04   vlc     Added function rxc_check_for_sch_update() to fix rate change 
                   issue for bursts with 1 frame gap.  Remove similar code
                   from rxc_tc_isr().
04/20/04   jrp     Fixed lint warnings.
04/14/04   sr      Removed unused ZIF logging code
04/08/04   bn      Add SCH DTX frame counter in the F3 debug message
03/10/04   vlc     Added support for centralized transaction engine handling
                   under feature FEATURE_TRANS_MANAGER.
03/05/04   sr/bkm  Moved clearing of frame offset from MC to RXC_EXIT_F and 
                   RXC_IDLE_F
03/03/04   sr      Changes for DV plus included changes to MSMSHARED tip
02/10/04   vlc     Need to check for STDSO and TDSO for F-SCH, as well as 
                   3G data calls (change in function rxc_tc_isr()).
01/06/04   vlc     Removed macro DEMOD_REP_FACTOR from function 
                   rxc_config_mac_for_ch1().  
12/15/03   jw      In function rxc_shutdown_sch()
                   Do not disable the sch OLPC in case of 1 frame gap.
10/22/03   vlc     Fixed RVCT compiler warnings.
10/22/03   sr      Fixed compilation warning errors when using RVCT compiler 
10/21/03   vlc     Re-instated the use of MUXMSM_GET_TD_STATUS_DONE macro 
                   instead of accessing the register directly.
10/20/03   bn      Reset the DCCH,FCH data valid when entering TCI state
10/20/03   vlc/ejv Modified MUXMSM_GET_TD_STATUS_DONE macro call for 6500.
10/17/03   vlc     Removed superfluous F3 messages.
10/06/03   vlc     Added #include rxcbcch.h.
09/18/03   ejv     Update rxc_sw_turbo_beta for 6500.
09/10/03   dnn     Changed FEATURE_TC_FRAMES_INTEGRATION to 
                   FEATURE_MSM6500_TC_FRAMES_INTEGRATION.
09/10/03   dnn     Added frame offset reprogram to FEATURE_TC_FRAMES_INTEGRATION. 
09/09/03   dna     Merged porting changes for MSM6500
09/04/03   bn      Removed 16x, RC3 energy saturation workaround by putting 
                   back the correct beta value.
09/04/03   bn      Corrected the beta value for CH1 MAC, since we still use 
                   the old Tiger values for the COMB_RND3, we need to derive
                   the new beta value to matche it.
08/08/03   vlc     Use dec_dcch_status_type instead of dec_fch_status_type for
                   rxc_dcch_status.
08/01/03   vlc     Confirm current call has RLP service before reading SCH data.
07/29/03   ejv     Update rxc_sw_beta_sch for 6500.
07/24/03   ejv     Added call to srch_mux_update_sch_walsh_len(...).
07/23/03   vlc     When switching FPC Pri Chan, make sure to re-configure the 
                   DEMOD with the appropriate RC in function 
                   rxc_ffpc_trans_cmd().
07/18/03   vlc     Release A changes:  Changed FCH and DCCH
                   channel set up to better indicate when decoder data is
                   available for the appropriate channel(s) for the RXC layer.
06/27/03   bn      Corrected the Beta value for F-SCH TURBO 32, RC4
06/16/03   bn      Fixed the "out of buffer dec_free_q" problem.
06/10/03   jrp     Read FPC and RPC history from mDSP rather than hardware.
05/29/03   vlc     For SVD, created fade_check_fch() and fade_check_dcch() to  
                   replace fade_check().  Reset various fade counters upon
                   primary power control channel switch.
05/28/03   vlc     Added support for FEATURE_IS2000_REL_B traffic to idle 
                   transition.
05/22/03   vlc     Fixed compiler warnings for MSM6000 target.
04/18/03   bn      Removed JCDMA Features in FER so that it can apply to DIAG
03/20/03   bn,sr   Added logic to support simultaneously monitor BCCH and FCCCH
04/02/03   vlc     Added call to new deint functions deint_atmr_isr2 () and 
                   deint_atmr_isr2_init () for PLT mode.
03/11/03   jrp     Corrected generation of QIB and EIB in the case of both
                   FCH and DCCH active.  Now, calculates using information
                   only from the primary channel.
02/26/03   jrp     Changed diagnostic message each SCH erasure to give only
                   summary of erasures every 100 frames.
02/25/03   vlc     Changes are for MSM6100, Release A:
                   Added new function rxc_atmr_isr2().  Added call to new deint
                   functions deint_atmr_isr2 () and deint_atmr_isr2_init ().
                   These functions support the use of a second action timer
                   ISR which is set to go off at the 4th bin in a frame to 
                   accommodate SCH Turbo timing changes.
01/31/03   bkm     Adjusted msg level on "Start MC txn" F3 message
01/21/03   vlc     Changed struct pwr_on to pwr_rep_delay when processing 
                   RXC_PWR_REP_DELAY_F.
                   Shortened Turbo CRC fail F3 message.
                   Put turbo SCH interrupt handling change to 3rd bin under
                   release A feature.
12/12/02   ph      rxc_tc_parse() logs signaling channel correctly now.
12/05/02   vlc     Removed MuxPD5 debug message occuring once per frame.
11/26/02   sr/vlc  For MSM6100, changed turbo SCH task ID to DEC_TURBO_SCH_TASK
                   (corresponds to 3rd bin for the dummy task.)
11/19/02   hxw     Modified gps tune-back logic to wait 4 good frames.
10/16/02   jrp     Fixed SCH EIB to write 1 in case of DTX - not just in case 
                   of erasure.  Also, call to write pcbit data is made in Rx
                   context rather than sending to Tx via a static variable.
10/15/02   dlb     Added new power control logging to combine power control
                   and frame type logging.
10/14/02   bn      Using the correct feature F_SCH in PLT
10/10/02   bn      Fixed the in-corrected increasing of the DTX counter in FSCH
                   turbo PLT logging. In PLT, printout the first few bytes of FSCH
10/08/02   az      Added support for voice SO SMV
09/19/02   bgc     Merged feature changes for MSM6000 for IS2000_SCH and MDR.
09/18/02   vlc     Function rxc_init_sch_mux() now check for NULL ptr before
                   getting DSM buffers.
08/06/02   cr      Added MSM6100 changes.
08/27/01   vlc     Changed power measurement report process for VP2.
08/08/02   vlc     Fixed compiler error when FEATURE_IS95B_MAHHO is turned off.
08/02/02   vlc     Updated rxc_tc_isr() to call ffpc_update_setpt() for both
           hrk     FCH and DCCH in VP2 mode regardless of the primary power
           bn      control channel selection.
           jrp     Now keep frame status separately for FCH and DCCH in 
                   rxc_tc_isr().
                   As a result of these fixes, modified DCCH frame supervision 
                   code and QIB/EIB handling accordingly.
                   Also fixed featurization in rxc_tc_isr() so that SVD will work
                   even when feature P2 is not defined.
08/01/02   jrp     Added featurization for FPC modes 4,5,6.  Changed FPC_MODE enums
                   to be more descriptive due to SVD.  Now modifies TXC QIB and EIB
                   flags directly, both set and reset (used to set in RXC and reset
                   in TXC each frame).  Fixed null pointer write in rxc_tc_isr().
                   Determines QIB now in SVD section of rxc_tc_isr().
07/23/02   vlc     Modified rxc_ffpc_trans_cmd() to allow switching primary 
                   power control channel when only 1 channel is active.
07/16/02   bgc     Featurized DCCH in FEATURE_IS2000_P2.  Featurized SCH in 
                   FEATURE_IS2000_F_SCH.  Featurized pilot gating in 
                   FEATURE_IS2000_CHS and P2.
07/11/02   bn      Added logic to support new QIB algorithm
06/18/02   jrp     Added support for FFPC Modes 4,5,6.
06/13/01   vlc     Reset fundicated channel deinterleaver state in function 
                   rxc_free_tc_resources().
06/07/02   lcc     Removed unused variable in rxc_control_hold_trans_cmd.
06/04/02   vlc     Added ability to switch primary power control channel for
                   VP2.
06/04/02   bn      Modified the FDCH logging function parameters for VP2
06/03/02   bgc     Adjusted ifdef to allow one to turn off FEATURE_IS2000_P2.
06/03/02   lcc     Merged back changes in r1.92 overwritten by r1.93.
05/31/02   lcc     1. Added support for control hold gating.
                   2. Corrected programming of reverse power delay to be like
                      MSM6050.
05/30/02   vlc     VP2 changes are now featurized under FEATURE_IS2000_REL_A_SVD
                   and FEATURE_IS2000_P2 per request of development team.
                   Implemented comments from VP2 code review.
05/16/02   bgc     Featurized rxc_sch_frame_status inside FEATURE_IS2000_F_SCH.
05/13/02   vlc     Added support for simultaneous DCCH & FCH channel set up 
                   (feature FEATURE_IS2000_REL_A_VP2.)
05/10/02   bn      Fixed the cause of Discrepancy in Forward Frame Type and 
                   Fast Forward Power Control Packet 
04/19/02   jrp     Added support RXC_GET_SETPT_F.
04/18/02   az      Added support for new voice service option SMV (SO 56)
04/03/02   bn      Added logic to flush the RDA and FDCH logs upon existing the call
03/27/02   bn      fixed a possible mis-alignment error in FDCH log
03/05/02   ems     Merged in changes for MSM6050.  Added zif logging code.
02/27/02   lad     Merged in IS2000 Release A Common Channels:
                   Replaced inline code with rxc_clear_decoder_queue().
                   Moved action timer alloc from rxcpaging to rxc_tci_init().
                   Added rxctraffic_frame_bnd_init() for timing-specific
                   config needed for starting traffic channel state.
                   Moved TCI code to rxc_tci_init().
02/27/02   vlc     Merged the following changes from SVD branch.
           lh      Added back data abort protection code when parsing MuxPDU 5.
                   secondary to primary traffic
           lh      Removed data_ptr even/odd checking because TDSO now frees
                   dsm items after processing them.
           vlc     Removed printf message in function read_mux_pdu5_sch_data().
                   Added MuxPDU 5 counter maintenance. (lh)
           bn      Added support for MUX SVD signaling
02/27/02   az      Featurized 13K voice service option.
02/04/02   bn      Featurization the PLT SCH logging for REL_A
01/28/02   sy      Changed logging to point to sr_id instead of sr_id_index.
01/22/02   vlc     Determine rate set of forward SCH in rxc_update_sch_info().
01/14/02   bgc     Added fixes for reverse power control delay problem.
01/12/02   sst     Modified to use the new srch_* files that replaced srchdrv.c
                     Includes a function name change:
                       srch_get_vsc_time -> srch_scomb_get_vsc_time
01/10/02   bn      Added support for PLT SCCH logging
01/03/02   bgc     Added SCH featurization to turn off for MSM6000.
12/10/01   hrk     Renamed rxc_erasure_flags to rxc_eib_flags. Renamed
                   rxc_fch_frame_status to rxc_fdch_frame_status.
                   Moved function rxc_flag_erasure() from rxccommon.c to this
                   module and renamed it to rxc_flag_eib().
12/10/01   hrk     Renamed RXC_FULL_FRAME to RXC_GOOD_FRAME.
12/07/01   bn      Used the correct FEATURE_DCCH_FRAME_QUALITY_SUPERVISION
                   instead of FEATURE_CHAN_SUP_TCI_INIT
11/27/01   hrk, bn Wait for spurious interrupts to get done before starting
                   FFPC logging.
11/13/01   bn      Added the enhance DCCH DTX detection and channel supervision logics
11/12/01   sr      Merged 1.58.1 branch back to trunk
11/09/01   sr      Removed call to rxc_free_tc_resources() in 
                   fade_check_bad_frame() and RELEASE_F. Added call to
                   rxc_free_tc_resources() when we exit the rxc_tc() while loop
11/07/01   hrk     Setting rxc_log_ffpc_sch_data=FALSE in rxc_free_tc_resources().
10/30/01   bn      Added support for Release A FDCH logging
10/26/01   hrk/bn  Disable FFPC logging for RC less than or equal to RC2. 
10/16/01   hrk     FFPC logging is done in the context of FCH decoder ISR.
                   The setpt values reported for both FCH and SCH correspond
                   the values at the beginning of the frame in which they are
                   reported. 
10/05/01   lcc     Added handling of MCTRANS_NULL_ACTION in rxc_service_trans_cmd.
09/20/01   lcc     1. Removed clearing of fundicated channel frame counters when
                      PMRM is sent due to ending of F-SCH burst which shouldn't
                      have been done.
                   2. Calls rxc_init_action_item in rxc_tci_init.
09/27/01   bn      Fixed log packet exists and contains bursts of F-SCH info
                   during SCCH calls
09/27/01   bgc     Added ifdef for a warning when FEATURE_RLP_LOGGING is off.
                   Added ifdef of IS2000_SCH in rxc_atmr_isr() for SCH
                   processing.
09/21/01   lcc     Corrected an error in an "if" statement while waiting for
                   N5M frame in rxc_tci which cause erasures to be considered
                   good frames.
09/14/01   rama    SR_ID fix. Differentiate between SR_ID and RLP instance.
                   Changed ds_curr_sr_id to ds_curr_sr_id_index.
09/21/01   bgc     Added initializations for DTX filter.
                   Moved rxc_dtx_det_sch() to rxcrda.c
08/28/01   lh      Back out SI bits changes for Release A Addendum 2.
08/28/01   bn      Fixed forward frame log types packet
08/22/01   bn      Support DCCH in FTM mode
08/15/01   sr      Fixed REL_A featurization
08/13/01   sr      Removed unnecessary if() condition to check validity of RC;
                   error case is now combined with check for valid RC for DTX
                   detection.
08/13/01   ak      Do not call dec_read_sch_data when RC is invalid.
08/10/01   lh      Added MuxPDU5 support for F-SCH.
08/07/01   hrk     Added back parameter "code_type" to dec_dtx_thresh_init().
08/07/01   hrk     Parameter "code_type" removed from call to dec_dtx_thresh_init().
                   Added DTX dectection for RC5 Turbo codes.
08/02/01   lcc     1. Moved some functions from rxccommon.c which really belongs
                      here.
                   2. Modified the way F-SCH shutdown is done such that even 1-frame
                      burst will work.  Previously, 1-frame burst will result in
                      2 frames with 1 "phantom" burst.
07/26/01   ak      Moved LTU_CRC feature so LTU_ID counter inc'd correctly.
06/28/01   baon    Incorporated the features FEATURE_IS2000_REL_A/32X
06/25/01   lcc     Corrected problem with frame type logging on F-DCCH.
06/15/01   day     Mainlined FEATURE_FFPC, FEATURE_IS2000
                   Delete FEATURE_SPECIAL_MDR, FEATURE_MDR_FER_F3MSG
06/06/01   lcc     1. Allowed DTX frames to be considered as good frames when
                      waiting for N5m good frames in TCI.
                   2. Added DCCH erasure frame categories in fade_check.
06/04/01   bgc     Added #ifdefs to turn off data services.
06/04/01   vlc     Changed FEATURE_GPSONE to FEATURE_GPS_MODE.
06/01/01   hrk/snn added support for DTX detection in RC5
                   Backed off some changes on turbo.
06/01/01   jq      Wrapped some code in FEATURE_IS95B_MAHHO.
05/23/01   lad     Added work-around for 8x and 16x turbo decoder's status bit
                   not being set.
05/14/01   jal     Corrected RLP NAK statistics logging
05/11/01   jq      Modified SCH frame type logging to include the last frame.
05/03/01   hrk     moved rxc_fch_frame_status outside FEATURE_FFPC.
05/03/01   sr      Made changes in rxc_plt_fch_logging() for dcch
05/03/01   lcc     Changes to use new definition of rxc_returned_type.
04/24/01   jq      Fixed an error in frame logging
04/23/01   sr      Changes to PLT to support new interface
04/20/01   sr      Merged in from MSM5100 archive
04/19/01   tc      Fixed incorrect dsch checking in rxc_log_tc_msg()
04/19/01   tc      Merged with MSM_MUX1X.01.00.01
           fc      Renamed mcc_report_event_msg_rxed to
                   mclog_report_event_msg_rxed, mcc_report_event_timer to
                   mclog_report_event_timer and mcc_report_event_counter to
                   mclog_report_event_counter
04/18/01   sr      Merged with MSM_MUX1X.01.00.00
           bgc,lcc Moved rxc_scr_trans_cmd() to this file.
           hrk     Added FEATURE_IS2000_P2 and DCCH and DTX handling.
           ych     Added support for FEATURE_JCDMA_MONITOR
           lcc     Corrected problem with back-to-back rate-changing burst.
           hrk     Collecting Reverse Power Control logging info in the context of
                   F-FCH decoder interrupt.
           jal     Corrected a couple of mis-reinitialized fields in
                   rxc_clear_rlp_statistics()
03/27/01   vlc     Merged MSM_MUX1X.00.00.17.
           lcc     1. Merged in support for SO 17 (CAI_SO_VOICE_13K_IS733).
                   2. Moved rxc_tc_prse outside IRAM to free up some IRAM.
           lcc     1. Cleaned up the way SCH shutdown and "start" is scheduled to take
                   care of the "one frame" gap and back-to-back scenarios.
                   2. Changed rxc_stored_sch_packet to use type mctrans_rxc_f_sch_action_type.
           fc      Fixed timer ID for T5m.
           fc      Added support of events report: Message received, Timer
                   expired and Counter threshold.
           lcc     1. Moved code for shutting down SCH into
                   rxc_shutdown_sch_if_action_time.
                   2. Corrected SCH shutdown problem when new burst starts at
                   1 frame after current stop time.
                   3. Removed some unnecessary debug messages.
03/23/01   tc      Fixed FEATURE_IS2000_REL_A.
03/20/01   ych     Merged JCDMA Features.
03/15/01   ejv     Added FEATURE_IS2000_REL_A.
03/15/01   tc      Fixed GPS at rxc_tc.
03/08/01   lh      Bug fix for compile error.
03/07/01   va/lh   Added support for LAC changes in Rel A.
03/02/01   tc      Merged MSM_MUX1X.00.00.15.
02/20/01   sr      Added support for 32X
                   Merged in from common archive:
                   02/13/01   jal     Support to reset RLP statistics
02/16/01   bgc     Changed FEATURE_FACTORY_TESTMODE to enable a runtime switch.
02/15/01   tc      Fixed Forward Supplemental Channel RC4 1X.
02/13/01   jal     Support to reset RLP statistics
02/05/01   bgc     Renamed RXC_RIFHHO_CFS_GPS_F.
01/16/01   jq      Added FEATURE_IS2000_SCH ifdefs around SCH codes.
01/09/00   ak      Put LTU CRC checking under an ifdef, since CPU intensive.
12/11/00   hrk     Fixed - F-FCH CURR_SETPT_DB seemed to be starting from
                   INIT_SETPT + 0.5 dB instead of INIT_SETPT, FFPC logging showed
                   that F-SCH setpoint started at the default value instead of the
                   SCH_INIT_SETPT value specified in ESCAM.
02/01/01   tc      Updated RXC_RIF_HHO_OR_CFS_F to RXC_RIFHHO_CFS_GPS_F.
01/30/01   tc      Merged GPS support.
01/12/01   tc      Merged MSM_MUX1X.00.00.06.
12/07/00   bgc     Commented out some unused PLT code.
11/29/00   ak      Implemented LTU CRC checking.
11/17/00   bgc     Fixed some warnings with typecasts.
11/13/00   bgc     Moved PLT traffic tables to here from common.  Added
                   accessor functions for FTM loopback flags.
11/13/00   hrk     Moved updating of FFPC setpoints from task context to ISR.
11/10/00   jc      This file was part of rxc.c. See rxccommon.c for full
                   change history

===========================================================================*/

/*lint -esym(715,unused_*) parameters prefixed with "unused_" are not used */

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include <memory.h>
#include "comdef.h"
#include "target.h"
#include "customer.h"
#include "assert.h"
#include "qw.h"
#include "rex.h"
#include "bit.h"
#include "queue.h"
#include "task.h"
#include "err.h"
#include "log.h"
#include "voc.h"
#include "crc.h"
#include "enc.h"
#include "mar.h"
#include "cmd.h"
#include "rxc.h"
#include "mccdma.h"
#include "parm.h"
#include "rxtx.h"
#include "msg.h"
#include "mccrx.h"
#include "ts.h"
#include "cai.h"
#include "rx.h"
#include "tx.h"
#include "mccrxtx.h"
#include "loopback.h"
#include "srch_mux.h"
#include "tramp.h"
#include "txc.h"
#include "rfm.h"

#if defined( FEATURE_RLP_LOGGING )
#include "diagcmd.h"
#include "diagpkt.h"
#include "diagbuf.h"
#include "dsrlp_stats.h"
#endif

#include "deint.h"
#include "msm_drv.h"
#include "atmr.h"
#include "ffpc.h"

#if  defined (FEATURE_IS95B_MDR)
#include "mccscm.h"             /* supplemental channel processing routines */
#endif /* FEATURE_IS95B_MDR */

#ifdef T_SLEEP
 #include "sleep.h"
#endif

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

#ifdef FEATURE_HHO_ENHANCEMENTS
void rxc_prep_for_rifhho_cfs_gps
(
  boolean is_gps
);
#endif

#ifdef FEATURE_DS_RLP3
#include "dsrlp.h"
#endif

#ifdef FEATURE_FACTORY_TESTMODE
/* need some txc buffers to do loopback */
#include "txc.h"
#endif

#include "rxccommon.h"
#include "rxctraffic.h"
#include "rxcsync.h"
#include "rxcrda.h"
#include "rxcdemux.h"
#include "rxcpaging.h"

#ifdef FEATURE_FACTORY_TESTMODE
#include "test_mc.h"
#endif
#include "mclog.h"

/* for Release A logging */
#ifdef FEATURE_IS2000_REL_A
#include "mux_logtypes.h"
#ifdef FEATURE_IS2000_REL_A_CC
#include "rxcbcch.h"
#endif
#endif /* FEATURE_IS2000_REL_A */

#include "mdspext.h"
#include "muxmsm.h"
#include "dec5000.h"

#ifdef FEATURE_IS2000_REL_B
#include "rxcidle.h"
#endif /* FEATURE_IS2000_REL_B */


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* Power control sysmbol gating mask */
#ifdef FEATURE_IS2000_CHS
#define DEM_FULL_RATE_GATING_MASK     0x0
#define DEM_HALF_RATE_GATING_MASK     0x6
#define DEM_QUARTER_RATE_GATING_MASK  0xd
#endif /* FEATURE_IS2000_CHS */

#ifdef FEATURE_IS2000_F_SCH
rxc_frame_type rxc_sch_frame_status = RXC_NO_FRAME;
#endif

rxc_tc_var_type rxc_tc_var;


#define RXC_TC_TMO_FRAMES (cai_tmo.t5m / 20)
  /* number of 20 ms frames in the timeout for the Traffic Channel */

/* Vocoder Buffers */
byte rxc_bits[36];
byte *rxc_vbits = rxc_bits;        /* Command buffer */


dec_sch_status_type rxc_sch_status;


/* indicator of whether to treat blank and burst as erasures */
boolean rxc_erase;

/* Indicator of whether the vocoder is initialized */
boolean rxc_voc_init_done; /*lint -e552 referenced in rxcmc.h */

/* Indicator of whether the vocoder has been configured for voice
   service option */
boolean rxc_voc_config_done;

/* Countdown to re-initialize vocoder after frame offset change */
#define RXC_VOC_FRAME_DELAY 4

#define RXC_NO_RATE 65535
  /* When rxc_fcc_rate is uninitialized, it should be set to RXC_NO_RATE */
  word rxc_fcc_rate = RXC_NO_RATE; /* To store the FCC rate in MDR rxc_tc_isr */

/* for MSM6700, each PCG can be installed ATMR */
static rxc_atmr_func_type rxc_atmr_tab[16] = 
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
atmr_handle_type rxc_atmr_handle = ATMR_INVALID_HANDLE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*         TASK VARIABLES                                                  */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*-------------------------------------------------------------------------*/
/* Buffers and Queues For Logging                                          */
/*-------------------------------------------------------------------------*/
  /* Decision history for FFPC logging */
  word rxc_fpc_history = 0;

  /* Decision history for rev pctrl */
  word rxc_rpc_history = 0; /*lint -e552 need this to debug RL power control */

  /* Forward frame types log accumulation buffer */
  /* Accumulation buffer */
  log_fwd_frame_types_type           rxc_log_frm_types_buf = {0};
  /* Sub-record pointer */
  log_fwd_frame_types_subrecord_type *rxc_log_frm_types_sr_ptr = NULL;

  #define RXC_FRM_TYPES_LOG_OFFSET \
  (rxc_log_frm_types_buf.hdr.len - FPOS(log_fwd_frame_types_type, var_len_buf))

#ifdef FEATURE_RLP_LOGGING
  /* Contains various channel information used in RLP 3 payload logging.
  */
  rxc_rlp_logging_chan_info_type rxc_rlp_logging_chan_info;
#endif /* FEATURE_RLP_LOGGING */

#if !defined (FEATURE_DIAG_V7_LOG_IFACE)
  log_mar_type rxc_log_mar_buf; /* Log buffer for accumulated Markov frame log */
#else
  q_type rxc_log_mar_q;  /* For logging MAR data */

  #ifdef TMOBILE
#error code not present
  #endif

  /*-------------------------------------------------------------------------*/
  /* Buffers for Logging Queues */

  /* buffers for rxc_log_mar_q */
  rxc_log_mar_type rxc_log_mar_pool[ RXC_NUM_MAR_BUFS ];

  /* Number of mar entries currently in Markov Frame log buffer */
  byte rxc_mar_cnt;

  /* pointer to buffer to buffer to log Markov data in */
  rxc_log_mar_type *rxc_mar_buf_ptr;

  /* Structure for logging vocoder packets */
  typedef struct {
    log_desc_type desc;
    log_voc_type  voc;
  } rxc_log_voc_type;

  #ifdef TMOBILE
#error code not present
  #endif

#endif /* defined(FEATURE_DIAG_V7_LOG_IFACE) */

/*-------------------------------------------------------------------------*/
/* Variables for Fade timers                                               */
/*-------------------------------------------------------------------------*/

word rxc_good_frames = 0; /* number of non-category 10 framesreceived after
                            at least one category 10 frame */
word rxc_t5m_bad_frames = 0;  /* number of consecutive category 10frames */
word rxc_n2m_bad_frames = 0;  /* number of consecutive category 10frames */
word rxc_ho_cnt = 0;      /* number of frames before handoff processing */
boolean rxc_fade = FALSE; /* indicator of whether fade processingis in
                            progress */
boolean rxc_sent_bad_rpt = FALSE;
boolean rxc_sent_pwron_rpt = FALSE;            /* used for hard handoff */
boolean rxc_hard_handoff_in_progress = FALSE;  /* fade due to hard handoff */
boolean rxc_sent_valid_rpt = FALSE;           /* RXC_VALID_R sent indicator*/

#ifdef FEATURE_IS2000_REL_D
#error code not present
#endif /* FEATURE_IS2000_REL_D */

/*-------------------------------------------------------------------------*/
/*  Supplemental Channel variable                                          */
dec_sch_cfg_type rxc_sch;
dec_sch_type sch_buf;


/*-------------------------------------------------------------------------*/
/* DEBUG VARIABLES                                                         */
/*-------------------------------------------------------------------------*/

word rxc_lost_voc_pkts = 0;   /* Number of lost logged vocoder packets */
byte rxc_voc_rate = VOC_0_RATE; /* Current vocoder frame rate */


/*-------------------------------------------------------------------------*/
/* Variables for Hard Handoff with Return-on-Failure                       */
/*-------------------------------------------------------------------------*/

#if defined (FEATURE_IS95B_MAHHO) || defined (FEATURE_GPS_MODE)
   word rxc_lost_frames = 0;
   /* Lost frame count during a Return-If-Fail HHO                         */
   word rxc_rif_hho_timeout_frames = 0;
   /* Equivalent of the Return-If-Fail hard handoff timer                  */
   boolean rxc_delay_after_hho_failure = FALSE;
   boolean rxc_rifhho_cfs_gps_in_progress = FALSE;
   
   // CFS tuneback count
   byte rxc_cfs_tuneback_count = 0;


   /* Indicator for a Return-If-Fail HHO or CFS                            */
   rxc_eib_flags_type rxc_eib_flags;
#ifdef FEATURE_IS2000_REL_A
   rxc_eib_flags_type rxc_sch_eib_flags;
   rxc_eib_flags_type rxc_qib_flags;
#endif /* FEATURE_IS2000_REL_A */
   boolean rxc_use_stored_eibs = FALSE;
#ifdef FEATURE_IS2000_REL_A
   boolean rxc_use_stored_qibs = FALSE;
   boolean rxc_use_stored_sch_eibs = FALSE;
#endif /* FEATURE_IS2000_REL_A */
#endif /* FEATURE_IS95B_MAHHO || FEATURE_GPS_MODE */

#ifdef FEATURE_GPSONE_OVERRIDE_RDA
#error code not present
#endif
#ifdef FEATURE_GPS_MODE
#error code not present
#endif /* FEATURE_GPS_MODE */

#ifdef FEATURE_DETECT_SUP_MUTING
  byte sup_qmet;                 /* supplemental channel Q metrics */
#endif /* FEATURE_DETECT_SUP_MUTING*/

#ifdef FEATURE_PLT
#error code not present
#endif  /* FEATURE_PLT */


#ifdef FEATURE_IS2000_SCH
/*-------------------------------------------------------------------------*/
/* RXC SCH                                                                 */
/*-------------------------------------------------------------------------*/

// It takes 2 decoder ISRs to reach the time to shutdown SCH:
// first interrupt happens right after the ATMR ISR that sets
// up this counter.
static enum
{
  RXC_SCH_SHUTDOWN_INACTIVE,
  RXC_SCH_SHUTDOWN_ARMED,
  RXC_SCH_SHUTDOWN_NOW
} rxc_sch_shutdown_state = RXC_SCH_SHUTDOWN_INACTIVE;
static boolean rxc_sch_rep = FALSE;         // Contain report indicator from MC
static byte rxc_sch_id = 0;
static boolean rxc_sch_update_now = FALSE;
static byte rxc_sch_update_counter = 0;
#endif

boolean rxc_log_ffpc_sch_data = FALSE;

#ifdef FEATURE_DS_IS2000
  dsm_item_type * rxc_dsm_arr[ SCH_MAX_FWD_PDU];
  byte rxc_dsm_arr_cnt = RXC_DSM_ARR_NOT_ALLOC;    /* Current index in arr */
#endif /* FEATURE_DS_IS2000 */

mctrans_rxc_f_sch_action_type rxc_stored_sch_packet;

#ifdef TMOBILE
#error code not present
#endif

#define RXC_INVALID 0

/* Lookup table for spreading rate */
uint8 const rxc_spr_rate_tab[3][6] =
{
  { /* RC3 */
    SPR_64_CHIP_V,/* 1x  */
    SPR_32_CHIP_V,/* 2x  */
    SPR_16_CHIP_V,/* 4x  */
    SPR_8_CHIP_V, /* 8x  */
    SPR_4_CHIP_V, /* 16x */
    RXC_INVALID   /* 32x */
  },
  { /* RC4 */
    SPR_128_CHIP_V,/* 1x  */
    SPR_64_CHIP_V, /* 2x  */
    SPR_32_CHIP_V, /* 4x  */
    SPR_16_CHIP_V, /* 8x  */
    SPR_8_CHIP_V,  /* 16x */
    SPR_4_CHIP_V   /* 32x */
  },
  { /* RC5 */
    SPR_64_CHIP_V,/* 1x  */
    SPR_32_CHIP_V,/* 2x  */
    SPR_16_CHIP_V,/* 4x  */
    SPR_8_CHIP_V, /* 8x  */
    SPR_4_CHIP_V, /* 16x */
    RXC_INVALID   /* 32x */
  }
};

/* Lookup table for rounding factor */
uint8 const rxc_rnd_factor_tab[3][6] =
{
  { /* RC3 */
    3,          /* 1x  */
    2,          /* 2x  */
    2,          /* 4x  */
    1,          /* 8x  */
    1,          /* 16x */
    RXC_INVALID /* 32x */
  },
  { /* RC4 */
    3,          /* 1x  */
    3,          /* 2x  */
    2,          /* 4x  */
    2,          /* 8x  */
    1,          /* 16x */
    1           /* 32x */
  },
  { /* RC5 */
    3,          /* 1x  */
    2,          /* 2x  */
    2,          /* 4x  */
    1,          /* 8x  */
    1,          /* 16x */
    RXC_INVALID /* 32x */
  }
};

#ifdef FEATURE_PLT
#error code not present
#endif /* FEATURE_PLT */

typedef struct
{
  uint32  good_sdus;
  uint32  incorrect_ltus;

  uint32  sdu_erasure;
  uint32  recovered_ltus;
}ltu_stats_type;

ltu_stats_type ltu_stats;

#ifdef FEATURE_IS2000_REL_A_SVD
/*-------------------------------------------------------------------------*/
/*  Simultaneous Voice and Data variable                                   */
/*-------------------------------------------------------------------------*/
/* Used in simultaneous F-FCH and F-DCCH channel set up.  Indicates which
** channel handles FFPC.
*/
cai_fpc_pri_chan_type ffpc_channel;
#ifdef FEATURE_IS2000_P2
word rxc_dcch_fcc_rate = RXC_NO_RATE;  /* Store the FCC rate in rxc_tc_isr */
#endif /* FEATURE_IS2000_P2 */
#endif /* FEATURE_IS2000_REL_A_SVD */

/*-------------------------------------------------------------------------*/
/*  FFPC external variables                                                */
/*-------------------------------------------------------------------------*/
extern boolean txc_eib_flag;
#ifdef FEATURE_IS2000_REL_A
extern boolean txc_qib_flag;
extern boolean txc_sch_eib_flag;
#endif /* FEATURE_IS2000_REL_A */

/* this flag indicated the traffic status */
boolean rxc_tx_in_traffic_on = FALSE;

/* Save timing information of demuxing operation for debugging purpose.
** Used to determine if RXC task time line is OK. 
*/
qword  rxc_dec_int0_fr_time;
qword  rxc_dec_int1_fr_time;

/* Save Deint Task ID for debugging pupose */
byte   saved_dec_int_task_id = 0; /*lint -e552 */
/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/
/*-------------------------------------------------------------------------*/
/* Functions used locally                                                  */
/*-------------------------------------------------------------------------*/
#ifdef FEATURE_IS2000_SCH
void rxc_update_sch_info( mctrans_rxc_f_sch_action_type *fwd_sch_packet );
void rxc_config_sch_hw( mctrans_rxc_f_sch_action_type *fwd_sch_packet );
void rxc_shutdown_sch( void );
void rxc_init_sch_mux( void );  /*lint !e762 Redundantly declared symbol due to ifdef's */
#endif
void rxc_free_tc_resources (void);
void rxc_process_at_cmd (rxc_cmd_msg_type *cmd_ptr);
void rxc_process_scr (rxc_scr_info_type  *scr);
void rxc_ho_trans_cmd (mctrans_action_packet_type *);
void rxc_scr_trans_cmd (mctrans_action_packet_type *cmd_ptr);
#ifdef FEATURE_IS2000_REL_A_CC
static boolean rxc_atmr_init (void);
#endif
void rxc_register_action_handlers_notifier_interrupt_func (void);
void (* rxc_action_time_notifier_func_ptr)(mctrans_server_token_type, qword);
void rxc_atmr_dispatcher(void);
void rxc_atmr_isr(void);
#ifdef FEATURE_IS2000_REL_A
void rxc_atmr_isr2(void);
#endif /* FEATURE_IS2000_REL_A */
#ifdef FEATURE_IS2000_REL_A_CC
void rxc_set_mac_rnd_ccch
(
  deint_code_rate_type code_rate,
  dec_ccch_data_rate_type data_rate,
  deint_ch_id_type chan_id
);
#endif
#ifdef FEATURE_IS2000_CHS
void rxc_control_hold_trans_cmd( mctrans_action_packet_type *chs_packet );
#endif //f FEATURE_IS2000_CHS

#ifdef FEATURE_DS
#define RXC_ISTDSO(so)                 \
          ( ( (so) == CAI_SO_TDSO ) || \
            ( (so) == CAI_SO_FULL_TDSO ) )
#endif /* FEATURE_DS */
      

/*===========================================================================
FUNCTION      RXC_IS_DATA_CALL

DESCRIPTION   This function returns whether the current SOs indicate a data
              call is in progress.

DEPENDENCIES  None.

RETURN VALUE  Boolean.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL boolean rxc_is_data_call(void)
{
  #ifdef FEATURE_DS
  return ( ( ( (rxc_so.pri) != CAI_SO_NULL ) &&                         
             ( DS_ISDATA(rxc_so.pri) || RXC_ISTDSO(rxc_so.pri) ) ) ||
           ( ( (rxc_so.sec) != CAI_SO_NULL ) &&                         
             ( DS_ISDATA(rxc_so.sec) || RXC_ISTDSO(rxc_so.sec) ) ) );
  #else
  return FALSE;
  #endif
}

#if !defined (FEATURE_DIAG_V7_LOG_IFACE)
/*========================================================================

FUNCTION RXC_SEND_MAR_LOG

DESCRIPTION This function sends an accumulated MAR log

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS None

=========================================================================*/
void rxc_send_mar_log (void)
{
  (void) log_submit((log_type *) &rxc_log_mar_buf);          

    /* Re-initialize counters */                   
  rxc_log_mar_buf.hdr.len = (word) FPOS(log_mar_type, entry); 
  rxc_log_mar_buf.count = 0;                        
} /*rxc_send_mar_log()*/
#endif /* ! FEATURE_DIAG_V7_LOG_IFACE*/

#ifdef FEATURE_DS_IS2000
/*========================================================================

FUNCTION RXC_INIT_SCH_MUX

DESCRIPTION This function initializes the array that will be used to hold
            the SCH Mux frames for RLP

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS None

=========================================================================*/
void rxc_init_sch_mux( void )
{
  byte i;                                                    /* loop ctr */
  boolean rlp_buffer_error = FALSE;
  word    rlp_partial_buf_cnt = 0;

  /* setup SCH MuxPDU array to hold new items */
  for( i=0; i< SCH_MAX_FWD_PDU; i++)
  {
    if (rxc_dsm_arr[i] == NULL) 
    {
      rxc_dsm_arr[i] = dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL);

      /* If less than 8 buffers are available, treat as error case.
      */
      if (rxc_dsm_arr[i] == NULL)
      {
        rlp_buffer_error = TRUE;
        break;
      }
    }
  }
  /* Error case, return the rest of the buffers.
  */
  if (rlp_buffer_error == TRUE) 
  {
    for( i=0; i< SCH_MAX_FWD_PDU; i++)
    {
      if (rxc_dsm_arr[i] != NULL)
      {
        dsm_free_packet( &rxc_dsm_arr[i]);
        rxc_dsm_arr[i] = NULL;
        rlp_partial_buf_cnt++;
      }
    }
    MSG_ERROR ("Not enough DSM items %d",rlp_partial_buf_cnt,0,0);

    /* Set array count to RXC_DSM_ARR_NOT_ALLOC to indicate buffers have
    ** not been assigned.
    */
    rxc_dsm_arr_cnt = RXC_DSM_ARR_NOT_ALLOC;
  }
  else
  {
    rxc_dsm_arr_cnt = 0;
  }
} /* rxc_init_sch_mux() */
#endif



/*===========================================================================

  FUNCTION RXC_SET_FRAME_OFFSET

DESCRIPTION
  This function sets the rxc_frame_offset which is used for frame number
  timing correction computations.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Sets rxc_frame_offset. Also sets frame offset in ts.

===========================================================================*/

void rxc_set_frame_offset (
  byte frame_offset
)
{
  rxc_frame_offset_countdown = 6;
  rxc_frame_offset = frame_offset;
  ts_set_frame_offset( frame_offset );
  MUXMSM_DEMOD_SET_FRAME_OFFSET( frame_offset );
  MSG_HIGH("Set Fwd Fr Offset to %d", frame_offset, 0, 0);
}

#ifdef FEATURE_IS2000_F_SCH
/*===========================================================================

FUNCTION SEND_FOR_SCH_FER_RPT

DESCRIPTION
  This function sends a fwd. SCH FER report to the Main Control task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void send_for_sch_fer_rpt( byte sch_id )
{
  mccrx_rpt_type *rpt_ptr;
    /* Pointer to report to be sent to the Main Control task */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - - -*/

  if ((rpt_ptr = (mccrx_rpt_type *)q_get( &mcc_rx_free_q )) == NULL)
  {
    ERR( "0 bufs on mcc_rx_free_q", 0, 0, 0 );

    /* Reset counts */
    rxc_pwr.sch_tot_frames[ sch_id ] = 0;
    rxc_pwr.sch_bad_frames[ sch_id ] = 0;
  }
  else
  {
    /* Generate power measurement report for MC */
    rpt_ptr->pwr.hdr.rpt = RXC_PWR_R;
    // set a flag to indicate that other stuff is not included
    rpt_ptr->pwr.sch_pwr_meas_included = TRUE;
    rpt_ptr->pwr.sch_id = sch_id;
    rpt_ptr->pwr.sch_pwr_meas_frames = rxc_pwr.sch_tot_frames[ sch_id ];
    rpt_ptr->pwr.sch_errors_detected = rxc_pwr.sch_bad_frames[ sch_id ];

#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
    /* It is mandatory to report FCH and/or DCCH at this time also.
    ** Report power measurements of the fundicated channel(s) as follows:
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
    /* DCCH only scenario.  For this scenario, standard indicates 
    ** that power measurement report represents DCCH errors and 
    ** dcch_pwr_meas_incl is set to FALSE. 
    */
    else if (rxc_so.for_dcch.included) 
    {
      rpt_ptr->pwr.pwr_meas_frames      = rxc_pwr.dcch_num_frames;
      rpt_ptr->pwr.errors_detected      = rxc_pwr.dcch_num_bad_frames;
    }
#else /* else of FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */
      // Include FCH data as it is mandatory
    rpt_ptr->pwr.pwr_meas_frames = rxc_pwr.num_frames;
    rpt_ptr->pwr.errors_detected = rxc_pwr.num_bad_frames;
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */

    rxc_mc_rpt( rpt_ptr );

  }
} /* send_for_sch_fer_rpt */
#endif // FEATURE_IS2000_F_SCH



/*===========================================================================

FUNCTION rxc_send_two_valid_frame_rpt

DESCRIPTION
  This function sends the 2 valid frames Rx'ed report to the Main Control task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rxc_send_two_valid_frame_rpt (void)
{
  mccrx_rpt_type *rpt_ptr; /* ptr to report to send to Main Control task */

  /* Event report: Counter N5m threshold reached */
  mclog_report_event_counter(CAI_COUNTER_N5M);

  /* send report of valid frame to Main Control task */
  if ( (rpt_ptr = (mccrx_rpt_type *)q_get(&mcc_rx_free_q) ) != NULL )
  {
    rxc_sent_valid_rpt = TRUE;
    rpt_ptr->hdr.rpt = RXC_VALID_R;
    rxc_mc_rpt(rpt_ptr );
    MSG_MED ("Send 2 frame valid rpt",0,0,0);
  }
  else
  {
    ERR( "0 bufs on mcc_rx_free_q", 0, 0, 0 );
  }
} /* end rxc_send_two_valid_frame_rpt */

/*===========================================================================

FUNCTION RXC_LOG_MAR

DESCRIPTION
  This function logs the Forward Traffic Channel Markov test data.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void  rxc_log_mar
(
  byte mar,   /* expected mux1 * 16 + rate decision */
  byte ber    /* bit error rate */
)
{

#if !defined(FEATURE_DIAG_V7_LOG_IFACE)
  if (rxc_log_mar_buf.count == 0)
  {
    ts_get((unsigned long *) rxc_log_mar_buf.hdr.ts);
  }

  /* Fill in log data */
  rxc_log_mar_buf.entry[rxc_log_mar_buf.count].mar_inx = mar;
  rxc_log_mar_buf.entry[rxc_log_mar_buf.count].bit_errs = ber;
  rxc_log_mar_buf.count++;
  rxc_log_mar_buf.hdr.len += sizeof(rxc_log_mar_buf.entry[0]);


  if (rxc_log_mar_buf.count == LOG_MAR_CNT)
  {
    rxc_send_mar_log();
  }
#else
  if( rxc_mar_buf_ptr == NULL )
  {
    /* need new item from queue */
    if ((rxc_mar_buf_ptr = (rxc_log_mar_type *)q_get(&rxc_log_mar_q)) == NULL )
    {
      MSG_MED("0 bufs on log_mar_q",0,0,0);
    }
    else
    {
      rxc_mar_cnt = 0;   /* initialize number of items in buffer to 0 */
      (void) ts_get((unsigned long *)(rxc_mar_buf_ptr->entry.hdr.ts));
      /* timestamp buffer */
    }
  }

  /* log value */
  if( rxc_mar_buf_ptr != NULL )
  {
    /* buffer available to log */
    rxc_mar_buf_ptr->entry.entry[rxc_mar_cnt].mar_inx = mar;
    rxc_mar_buf_ptr->entry.entry[rxc_mar_cnt].bit_errs = ber;
    rxc_mar_cnt++;

    if( rxc_mar_cnt == LOG_MAR_CNT )
    {
      /* entry is full - log it */
      rxc_mar_buf_ptr->entry.count = rxc_mar_cnt;

      log_put( &rxc_mar_buf_ptr->desc );

      rxc_mar_buf_ptr = NULL;
        /* indicate new buffer is required */
    }
  }
#endif /* !defined (FEATURE_DIAG_V7_LOG_IFACE) */
} /* rxc_log_mar */


#ifdef FEATURE_IS2000_REL_A
/*            Variables for FDCH Release A logging             */
static uint32 raw_fdch_log_buffer[ FRDCH_LOG_BUFFER_SIZE/4   +1] = {0};
static log_fdch_information_type * rxc_fdch_log_buffer = 
  (log_fdch_information_type *) raw_fdch_log_buffer; //lint !e740 Unusual pointer cast
static fdch_sub_record_type *rxc_fdch_sub_record_ptr = NULL; 
static fwd_sch_information_type *fsch_frame_info = NULL;
static int rxc_fdch_log_buff_offset = 0;
static int rxc_fdch_log_sub_record_offset = 0;
static word rxc_fdch_curr_sch_mux_opt = 0;

/*               FUNCTIONS TO HANDLE THE LOGGING FOR FDCH               */

/*===========================================================================

FUNCTION RXC_FDCH_FRAME_INFO_SEND_LOG

DESCRIPTION
  Terminate and send the accumulated FDCH frame information log.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  A FDCH frame information log may be sent.

===========================================================================*/
void rxc_fdch_frame_info_send_log(void)
{
  if (rxc_fdch_log_buffer->hdr.len > 0)
  {
    /* Will send log if submitted. */
    (void) log_submit((log_type *) rxc_fdch_log_buffer);

    /* Flag to init start of log */
    rxc_fdch_log_buffer->hdr.len = 0;
  }

} /* rxc_fdch_frame_info_send_log */

/*===========================================================================

FUNCTION RXC_FDCH_LOG_FRAME_INFO_INIT

DESCRIPTION
  This function initializes the FDCH frame info log subrecord, as well as the
  pointer to it and the entry for this frame, in the frame types log buffer.
  If necessary, the log is sent and a new one is generated.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to frame log entry.

SIDE EFFECTS
  A FDCH frame info. log may be sent.

===========================================================================*/
fdch_frame_type  * rxc_fdch_log_frame_info_init(void)
{
  fdch_frame_type  *fdch_frame_ptr = NULL;
  channels_info_type  fdch_chan_set_mask;

  /* If logging is enabled. */
  if (log_status(LOG_FDCH_FRAME_INFO_C)) 
  {
    /* If we reached accumulation threshold, send it. 
    the threshold = the buffer size - the maximum of byte per record in the 
    worst case analysic */
    if (rxc_fdch_log_buffer->hdr.len >= FRDCH_LOG_BUFFER_THRESHOLD)
    {
      rxc_fdch_frame_info_send_log();
    }
    /* check if there is a need to re-initialize */
    if (rxc_fdch_log_buffer->hdr.len == 0)
    {
      rxc_fdch_log_buffer->hdr.len = FPOS(log_fdch_information_type,reference_buf);//lint !e734 FPOS
      rxc_fdch_log_buffer->hdr.code = LOG_FDCH_FRAME_INFO_C;
      /* First frame, log time stamp. */
      ts_get((unsigned long *) rxc_fdch_log_buffer->hdr.ts);

      /*  Fill in beginning of record   */
      rxc_fdch_log_buffer->frame_offset = rxc_frame_offset;
      rxc_fdch_log_buffer->num_sub_records = 0;

      /* reset all the control variables */
      rxc_fdch_log_buff_offset = 0;
      rxc_fdch_sub_record_ptr = NULL; 
    }
    else
    {
      /* If a new sub-record is needed, i.e. the channel set has changed,
      ** check for the condition here.
      */
      /* clear the fdch mask */
      fdch_chan_set_mask.mask = 0;
      /* set the the mask based on the active FDCH channels */
      if (rxc_so.for_fch.included)
      {
        fdch_chan_set_mask.individual_channels.fch = TRUE;
      }
#ifdef FEATURE_IS2000_P2
      if (rxc_so.for_dcch.included)
      {
        fdch_chan_set_mask.individual_channels.dcch = TRUE;
      }
#endif
#ifdef FEATURE_IS2000_F_SCH
      if (rxc_sch_frame_status!=RXC_NO_FRAME)
      {
        fdch_chan_set_mask.individual_channels.sch0 = TRUE;
      }
#endif /* FEATURE_IS2000_F_SCH */

      /* SCCH channels */
      fdch_chan_set_mask.individual_channels.scch = rxc_scch.num_sup;
      /* I don't know how to do for PICH channel, set to FALSE for now */
      fdch_chan_set_mask.individual_channels.rpich = FALSE;

      /* start a new sub-record if there is a new channel been added or the SCH 
      MUX option is changing */
      if(fdch_chan_set_mask.mask != rxc_fdch_sub_record_ptr->assigned_channels.mask)
      {
        /* We no longer have a valid sub-record. */
        rxc_fdch_sub_record_ptr = NULL; 
      }
      else
      {
#ifdef FEATURE_IS2000_F_SCH
        if ((rxc_sch_frame_status!=RXC_NO_FRAME) && 
            (rxc_fdch_curr_sch_mux_opt != rxc_so.for_sch_mux_option))
        {
          /* We no longer have a valid sub-record. */
          rxc_fdch_sub_record_ptr = NULL; 
        }
#endif /* FEATURE_IS2000_F_SCH */
      }

    }
    /* Need to init a new subrecord? */
    if (rxc_fdch_sub_record_ptr == NULL)
    {
      rxc_fdch_sub_record_ptr = 
        (fdch_sub_record_type *) &rxc_fdch_log_buffer->reference_buf[rxc_fdch_log_buff_offset];
      /* Increment sub-record count. */
      rxc_fdch_log_buffer->num_sub_records++;
      /* reset the number of frame counter */
      rxc_fdch_sub_record_ptr->num_frames = 0;

      /* reset the fdch log sub-record offset */
      rxc_fdch_log_sub_record_offset = 0;

      /* reset the channel mask */
      rxc_fdch_sub_record_ptr->assigned_channels.mask = 0;

      /********Initialize the channel assignment and RC for each FDCHs ************/

      /* check for PICH */
      /* I don't know how to do for PICH channel, set to FALSE for now */
      rxc_fdch_sub_record_ptr->assigned_channels.individual_channels.rpich = FALSE;

      /* check for FCH */
      if (rxc_so.for_fch.included)
      {
        rxc_fdch_sub_record_ptr->assigned_channels.individual_channels.fch
                                   = TRUE;
        rxc_fdch_sub_record_ptr->reference_buf[rxc_fdch_log_sub_record_offset++]
                                   = rxc_so.for_fch.rc;
      }
#ifdef FEATURE_IS2000_P2
      /* check for DCCH */
      if (rxc_so.for_dcch.included)
      {
        rxc_fdch_sub_record_ptr->assigned_channels.individual_channels.dcch
                                   = TRUE;
        rxc_fdch_sub_record_ptr->reference_buf[rxc_fdch_log_sub_record_offset++]
                                   = rxc_so.for_dcch.rc;
      }
#endif
      
#ifdef FEATURE_IS2000_F_SCH
      /* check for SCH and initialize FSCH data*/
      if (rxc_sch_frame_status!=RXC_NO_FRAME)
      {
        /* set the channel bit mask */
        rxc_fdch_sub_record_ptr->assigned_channels.individual_channels.sch0
                                   = TRUE;
        fsch_frame_info = (fwd_sch_information_type *) 
                 &rxc_fdch_sub_record_ptr->reference_buf[rxc_fdch_log_sub_record_offset];
        
        /* FSCH RC */
        fsch_frame_info->for_sch_rc = rxc_sch.rc;
        /* SCH MUX OPTION */
        if (rxc_so.for_sch_mux_option != 0)
        {
          /* FSCH0 MUX option LSB */
          fsch_frame_info->for_sch0_mux_option_lo = rxc_so.for_sch_mux_option & 0xFF;
          /* FSCH0 MUX option MSB */
          fsch_frame_info->for_sch0_mux_option_hi = (rxc_so.for_sch_mux_option >> 8) & 0xFF;

          /* store for comparation later */
          rxc_fdch_curr_sch_mux_opt = rxc_so.for_sch_mux_option;
        }
        else
        {
          /* FSCH0 MUX option MSB to zero*/
          fsch_frame_info->for_sch0_mux_option_hi = 0;
          /* FSCH0 MUX option LSB to zero*/
          fsch_frame_info->for_sch0_mux_option_lo = 0;
        }
#ifdef FEATURE_MULTIFRAME_INTERLEAVING
#error code not present
#else
        fsch_frame_info->sch_frame_length = 0;
#endif
        /* FSCH0 number of bits */
        fsch_frame_info->sch0_num_bits = rxc_sch.sch_rate;

        /* number of MUX PDU per SCH frame */
        fsch_frame_info->num_mux_pdu_per_sch_frame = rxc_dsm_arr_cnt;
        /* number of LTU per SCH frame */
        fsch_frame_info->num_ltu_per_sch_frame = rxc_sch.num_ltu;
        /* update the sub-record offset */
        rxc_fdch_log_sub_record_offset += (int) sizeof(fwd_sch_information_type);
      }
#endif /* FEATURE_IS2000_F_SCH */

      /* Initialize the number SCCH channels */
      rxc_fdch_sub_record_ptr->assigned_channels.individual_channels.scch
                                           = rxc_scch.num_sup;
      /* Update packet length */
      rxc_fdch_log_buffer->hdr.len += 
              (FPOS(fdch_sub_record_type,reference_buf) + rxc_fdch_log_sub_record_offset); //lint !e734 !e737 FPOS
      /*Update the buffer pointer offset */
      rxc_fdch_log_buff_offset += (rxc_fdch_log_sub_record_offset + 
                                    (FPOS(fdch_sub_record_type,reference_buf)));//lint !e713 !e737 FPOS
    }
    /* Increment frame count */
    rxc_fdch_sub_record_ptr->num_frames++;

    /* Initialize the FDCH frame record pointer */
    fdch_frame_ptr = (fdch_frame_type *)
                          &rxc_fdch_sub_record_ptr->reference_buf[rxc_fdch_log_sub_record_offset];
    
  }
  else
  {
    rxc_fdch_log_buffer->hdr.len = 0; /* Flag that this record has stopped */
  }
  return fdch_frame_ptr;
}

/*=========================================================================
FUNCTION RXC_FDCH_LOG_FRAME_INFO

DESCRIPTION
  This function populates FDCH frame information logs.

DEPENDENCIE
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=========================================================================*/
#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
void rxc_fdch_log_frame_info (byte fch_mux, byte dcch_mux ) /* mux type for FCH/DCCH logging */
#else
void rxc_fdch_log_frame_info (byte mux ) /* mux type for FCH/DCCH logging */
#endif /* FEATURE_IS2000_P2 and FEATURE_IS2000_REL_A_SVD */
{
  int fdch_frame_offset = 0;
  int index;
  fdch_frame_type  *fdch_frame_record_ptr = NULL;

  /* Call the initialized function to init the fdch frame pointer */
  fdch_frame_record_ptr = rxc_fdch_log_frame_info_init();

  if (fdch_frame_record_ptr != NULL)
  {
    /* If FCH is assigned, then including FCH category */
    if (rxc_fdch_sub_record_ptr->assigned_channels.individual_channels.fch)
    {
#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
      fdch_frame_record_ptr->reference_buf[fdch_frame_offset++] = fch_mux;
#else
      fdch_frame_record_ptr->reference_buf[fdch_frame_offset++] = mux;
#endif /*  FEATURE_IS2000_P2 and FEATURE_IS2000_REL_A_SVD */
    }

    /* If DCCH is assigned, then including DCCH category */
    if (rxc_fdch_sub_record_ptr->assigned_channels.individual_channels.dcch)
    {
#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
      /* See structure definition for explaination of reference_buf */
      fdch_frame_record_ptr->reference_buf[fdch_frame_offset++] = dcch_mux; //lint !e661 Possible out-of-bounds
#else
      fdch_frame_record_ptr->reference_buf[fdch_frame_offset++] = mux;
#endif /* FEATURE_IS2000_P2 and FEATURE_IS2000_REL_A_SVD */
    }
 
    /* If SCCH is assigned, then including SCCH category 
    note that, SCCH can have up to 7 channels*/
    for(index = 0; index < rxc_fdch_sub_record_ptr->assigned_channels.individual_channels.scch; index++)
    {
      /* scch category, FULL or ERASURE */
      /* See structure definition for explaination of reference_buf */
      fdch_frame_record_ptr->reference_buf[fdch_frame_offset++] = rxc_scch.rate_sel[index]; //lint !e661 !e662 Possible out-of-bounds
    }


#ifdef FEATURE_IS2000_F_SCH
    /* If SCH is assigned, then including SCH category 
    the possible values for SCH category are: sufficient quality, insufficient
    quality and DTX*/
    if (rxc_fdch_sub_record_ptr->assigned_channels.individual_channels.sch0)
    {
      
      /* Including the SCH frame category */
      /* See structure definition for explaination of reference_buf */
      fdch_frame_record_ptr->reference_buf[fdch_frame_offset++] 
        = rxc_sch_frame_status; //lint !e661 !e662 !e797 !e796 Conceivable out-of-bounds

      /* Including LTU quality information 
      This field is only included if SCH is assigned and this frame is an erasure frame*/
      /* check if the FSCH category is INSUFFICIENT_QUALITY */
      if (rxc_sch_frame_status == RXC_ERASURE_FRAME && (rxc_sch.num_ltu != 0))
      {
        
        /* inluding LTU CRC bits */
        /* See structure definition for explaination of reference_buf */
        fdch_frame_record_ptr->reference_buf[fdch_frame_offset++] 
          = (uint8) rxc_sch_status.conv_sch_status.ltu_crc; //lint !e661 !e662 !e796 !e797 Conceivable out-of-bounds
      }
    }
#endif /* FEATURE_IS2000_F_SCH */

    /* Updated the total lenght of the record header */
    rxc_fdch_log_buffer->hdr.len += (word) fdch_frame_offset;
    /* update the offset variables */
    rxc_fdch_log_sub_record_offset += fdch_frame_offset;
    rxc_fdch_log_buff_offset += fdch_frame_offset;
  }

}
#else
/*===========================================================================

FUNCTION RXC_FRAME_TYPES_SEND_LOG

DESCRIPTION
  Terminate and send the accumulated forward frame types log.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  A forward frame types log may be sent.

===========================================================================*/
void rxc_frame_types_send_log(void)
{
  if (rxc_log_frm_types_buf.hdr.len > 0)
  {
    /* Will send log if submitted. */
    log_submit((log_type *) &rxc_log_frm_types_buf);

    /* Flag to init start of log */
    rxc_log_frm_types_buf.hdr.len = 0;
  }

} /* rxc_frame_types_send_log */



/*===========================================================================

FUNCTION RXC_LOG_FRAME_TYPE_FRM_INIT

DESCRIPTION
  This function initializes the frame type log subrecord, as well as the
  pointer to it and the entry for this frame, in the frame types log buffer.
  If necessary, the log is sent and a new one is generated.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to frame log entry.

SIDE EFFECTS
  A forward frame types log may be sent.

===========================================================================*/
/* indicate when the FSCH RC has been changed */
static boolean rxc_sch_rc_change = FALSE;
#ifdef FEATURE_IS2000_P2
log_fwd_frame_types_frm_type * rxc_log_frame_type_frm_init(cai_chind_type ch_ind)
#else
log_fwd_frame_types_frm_type * rxc_log_frame_type_frm_init(void)
#endif /* FEATURE_IS2000_P2 */
{
  log_fwd_frame_types_frm_type *frm_ptr = NULL;
  log_ded_fwd_chan_set_type chan_set_mask;

  /* If logging is enabled. */
  if (log_status(LOG_FWD_FRAME_TYPES_C)
#ifdef FEATURE_LOG_PC_INFO_2_EXCLUSIVE
      /* The FWD_FRAME_TYPES log message is a subset of the FFPC_INFO log */
      /* message, so only log it if FFPC_INFO is not being logged.        */
      && !log_status(LOG_FFPC_INFO_C)
#endif /* FEATURE_LOG_PC_INFO_2_EXCLUSIVE */
     )
  {
    /* If we reached accumulation threshold or if there is a RC change,
        then send it, so that the new frame header will be created . */
    if ((rxc_log_frm_types_buf.hdr.len >= LOG_FWD_FRAME_TYPES_BUF_THRESHOLD) ||
                (rxc_sch_rc_change))
    {
      rxc_frame_types_send_log();
          rxc_sch_rc_change = FALSE;
    }

    if (rxc_log_frm_types_buf.hdr.len == 0) /* Need to re-init */
    {
      rxc_log_frm_types_buf.hdr.len = FPOS(log_fwd_frame_types_type, var_len_buf);

      rxc_log_frm_types_buf.hdr.code = LOG_FWD_FRAME_TYPES_C;

      /* First frame, log time stamp. */
      (void) ts_get((unsigned long *) rxc_log_frm_types_buf.hdr.ts);

      /*------------------------------
        Fill in beginning of record
      ------------------------------*/
      /* Frame offset in 1.25 ms units */
      rxc_log_frm_types_buf.frame_offset = rxc_frame_offset;

      /* Service Option */
      rxc_log_frm_types_buf.so = rxc_so.pri;

      /* for now, only FCH is supported */
      #ifdef FEATURE_IS2000_P2
      rxc_log_frm_types_buf.fwd_ch_rc.f_fch_rc = rxc_so.for_fch.included?rxc_so.for_fch.rc:0;
      rxc_log_frm_types_buf.fwd_ch_rc.f_dcch_rc = rxc_so.for_dcch.included?rxc_so.for_dcch.rc:0;
      #else
      rxc_log_frm_types_buf.fwd_ch_rc.f_fch_rc = rxc_so.for_fch.rc;
      rxc_log_frm_types_buf.fwd_ch_rc.f_dcch_rc = 0; /* not yet supported */
      #endif /* FEATURE_IS2000_P2 */
      #ifdef FEATURE_IS2000_SCH
      rxc_log_frm_types_buf.fwd_ch_rc.f_sch0_rc = rxc_sch.rc;
      rxc_log_frm_types_buf.fwd_ch_rc.f_sch1_rc = 0; /* not yet supported */
      #endif /* FEATURE_IS2000_SCH */

      /* We no longer have a valid sub-record. */
      rxc_log_frm_types_buf.sr_cnt = 0;
      rxc_log_frm_types_sr_ptr     = NULL;

    }
    else
    {
      /* If a new subrecord is needed, i.e. the channel set has changed,
      ** check for the condition here.
      */
      chan_set_mask.mask = 0;
      #ifdef FEATURE_IS2000_P2
      /* for now only one type of channel is supported */
      chan_set_mask.chan_set.f_fch = (ch_ind&CAI_CHIND_FCH_ONLY) != FALSE;
      chan_set_mask.chan_set.f_dcch = (ch_ind&CAI_CHIND_DCCH_ONLY)!= FALSE;
      #else
      chan_set_mask.chan_set.f_fch = TRUE; /* For now, fwd CH always active. */
      #endif /* FEATURE_IS2000_P2 */

      #ifdef FEATURE_IS2000_SCH
      chan_set_mask.chan_set.f_sch0 = (rxc_sch_frame_status!=RXC_NO_FRAME) ? TRUE : FALSE;
      #endif /* FEATURE_IS2000_SCH */

      if(chan_set_mask.mask != rxc_log_frm_types_sr_ptr->ch_set_mask.mask)
      {
        /* We no longer have a valid sub-record. */
        rxc_log_frm_types_sr_ptr = NULL;
      }
    }

    /* Need to init a new subrecord? */
    if (rxc_log_frm_types_sr_ptr == NULL)
    {
      /* Init sub-record pointer by finding the offset into the variable length buf.
      ** RXC_FRM_TYPES_LOG_OFFSET is calculated from the length of the log packet.
      */
      rxc_log_frm_types_sr_ptr = (log_fwd_frame_types_subrecord_type *)
          &rxc_log_frm_types_buf.var_len_buf.buf_ref[RXC_FRM_TYPES_LOG_OFFSET];

      /* Increment sub-record count. */
      rxc_log_frm_types_buf.sr_cnt++;

      /* Init all bits in mask, and len_per_frame */
      rxc_log_frm_types_sr_ptr->ch_set_mask.mask = 0;
      rxc_log_frm_types_sr_ptr->len_per_frame    = 0;

      #ifdef FEATURE_IS2000_P2
      rxc_log_frm_types_sr_ptr->ch_set_mask.chan_set.f_fch=(ch_ind&CAI_CHIND_FCH_ONLY)!=FALSE;
      rxc_log_frm_types_sr_ptr->ch_set_mask.chan_set.f_dcch=(ch_ind&CAI_CHIND_DCCH_ONLY)!=FALSE;
      #else
      /* For now, FCH is always active. */
      rxc_log_frm_types_sr_ptr->ch_set_mask.chan_set.f_fch = TRUE;
      #endif /* FEATURE_IS2000_P2 */

      /* Increase length per frame value for FCH/DCCH */
      /* if FCH and DCCH are supported at the same time, need to add for DCCH here */
      rxc_log_frm_types_sr_ptr->len_per_frame +=
        sizeof(log_fwd_frame_types_frm_type);

      #ifdef FEATURE_IS2000_SCH
      /* Check if SCH0 is active for this frame */
      if (rxc_sch_frame_status!=RXC_NO_FRAME)
      {
        rxc_log_frm_types_sr_ptr->ch_set_mask.chan_set.f_sch0 = TRUE;

        /* Increase length per frame value for SCH0 */
        rxc_log_frm_types_sr_ptr->len_per_frame +=
          sizeof(log_fwd_frame_types_frm_type);
      }
      #else
      rxc_log_frm_types_sr_ptr->ch_set_mask.chan_set.f_sch0 = FALSE;
      #endif /* FEATURE_IS2000_SCH */

      /* Init frame count */
      rxc_log_frm_types_sr_ptr->frame_cnt = 0;

      /* Update packet length */
      rxc_log_frm_types_buf.hdr.len +=
        FPOS(log_fwd_frame_types_subrecord_type, var_len_buf);
    }

    /* Increment frame count */
    rxc_log_frm_types_sr_ptr->frame_cnt++;

    /* Init frame pointer by finding the offset into the variable length buf.
    ** RXC_FRM_TYPES_LOG_OFFSET is calculated from the length of the log.
    */
    frm_ptr = (log_fwd_frame_types_frm_type *)
        &rxc_log_frm_types_buf.var_len_buf.buf_ref[RXC_FRM_TYPES_LOG_OFFSET];
  }
  else
  {
    rxc_log_frm_types_buf.hdr.len = 0; /* Flag that this record has stopped */
  }
  /* if the FSCH RC has been changed, then set the flag so that the next frame,
  the new header frame with new the RC will be updated and sent out */
  if (rxc_log_frm_types_buf.fwd_ch_rc.f_sch0_rc != rxc_sch.rc)
  {
    rxc_sch_rc_change = TRUE;
  }
  return frm_ptr;

} /* rxc_log_frame_type_frm_init */



/*=========================================================================
FUNCTION RXC_LOG_FRAME_TYPE

DESCRIPTION
  This function populates frame type logs.

DEPENDENCIE
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=========================================================================*/
void
rxc_log_frame_type
(
  int16 frame_rate,                /* FCH  Frame rate */
  log_fch_type_enum_type fch_type, /* FCH  Frame type */
  log_sch_rate_enum_type sch0_type /* SCH0 Frame type */
  #ifdef FEATURE_IS2000_P2
  , cai_chind_type ch_ind     /* indicate which channels are active */
  , byte mux           /* mux type for DCCH logging */
  #endif /* FEATURE_IS2000_P2 */
)
{
  log_fwd_frame_types_frm_type *frm_ptr;

  #ifdef FEATURE_IS2000_P2
  frm_ptr = rxc_log_frame_type_frm_init(ch_ind);
  #else
  frm_ptr = rxc_log_frame_type_frm_init();
  #endif /* FEATURE_IS2000_P2 */

  if (frm_ptr != NULL)
  {
    /* Record data for each channel */
    if (rxc_log_frm_types_sr_ptr->ch_set_mask.chan_set.f_fch)
    {
      frm_ptr->f_fch.rate = frame_rate;
      frm_ptr->f_fch.type = fch_type;

      frm_ptr++; /* Increase sizeof 1 frame entry */
    }

    if(rxc_log_frm_types_sr_ptr->ch_set_mask.chan_set.f_sch0)
    {
      frm_ptr->f_sch0 = sch0_type;

      frm_ptr++; /* Increase sizeof 1 frame entry */
    }

    #ifdef FEATURE_IS2000_P2
    /* log DCCH */
    if (rxc_log_frm_types_sr_ptr->ch_set_mask.chan_set.f_dcch)
    {
      frm_ptr->f_dcch = mux;
      frm_ptr++;
    }
    #endif /* FEATURE_IS2000_P2 */

    /* TODO: Add other channels here! */

    /* end per channel data */

    /* Update packet length */
    rxc_log_frm_types_buf.hdr.len += rxc_log_frm_types_sr_ptr->len_per_frame;

  } /* If logging enabled */
} /* rxc_log_frame_type */
#endif /* FEATURE_IS2000_REL_A */

/*===========================================================================

FUNCTION RXC_VOC_XCHG

DESCRIPTION
  This function transfers the decoder data received from the Traffic Channel
  to the vocoder.

DEPENDENCIES
  None.

RETURN VALUE
  Decoder rate.

SIDE EFFECTS
  None.

===========================================================================*/
#ifdef TMOBILE
#error code not present
#endif

void rxc_voc_xchg (void)
{
  /* vocoder VFR stabliz1ng frame counter */
  if (rxc_time_cnt > 0)
  {
    rxc_time_cnt -= 1;
  }
  else if (rxc_time_cnt != RXC_TIMER_NOT_SET)
  {
    /* wait for active SO */
    if (     (rxc_so.pri == CAI_SO_VOICE_IS96A)
         ||  (rxc_so.pri == CAI_SO_VOICE_EVRC)
#ifndef FEATURE_NO_13K_VOICE_SUPPORT
         ||  (rxc_so.pri == CAI_SO_VOICE_13K)
         ||  (rxc_so.pri == CAI_SO_VOICE_13K_IS733)
#endif
#ifdef FEATURE_SO_VOICE_SMV
         ||  (rxc_so.pri == CAI_SO_VOICE_SMV)
#endif
#ifdef FEATURE_VOC_4GV
         ||  (rxc_so.pri == CAI_SO_VOICE_4GV_NB)
         ||  (rxc_so.pri == CAI_SO_VOICE_4GV_WB)
#endif /* FEATURE_VOC_4GV */

       )
    {
#ifdef TMOBILE
#error code not present
#endif
      /* Transfer frame to vocoder */
      if ( voc_rx_cmd ( rxc_voc_rate, rxc_vbits ) != VOC_DONE_S )
      {
        ERR( "Vocoder not resp: status %d", 0, 0, 0 );
      }
#ifdef TMOBILE
#error code not present
#endif
    }
  } /* rxc_time_cnt */
} /* rxc_voc_xchg */

/* <EJECT> */

#ifdef FEATURE_IS2000_F_SCH
/*========================================================================
FUNCTION RXC_UPDATE_SCH_INFO

DESCRIPTION
   This function updates the struct rxc_sch with the buffered F-SCH
   parameters.

   Rate set is derived using the received rc parameter.  This is done here 
   so rate set information can be retained for the processing of the final 
   frame, which may occur after the rc is reset in rxc_shutdown_sch ().

DEPENDENCIES
  None

SIDE EFFECTS
  None

RETURN VALUE
  None

========================================================================*/
void rxc_update_sch_info (mctrans_rxc_f_sch_action_type *fwd_sch_packet)
{
  MSM_LOCK();
  rxc_sch.rc = (dec_rc_type) fwd_sch_packet->rc;

  /* Determine rate set using rc.  Do it here so rate set information can be
  ** retained for the processing of the final frame which may occur after
  ** the rc is reset in rxc_shutdown_sch ().
  */
  rxc_sch.sch_rs   = fwd_rc_to_rate_set[rxc_sch.rc];
  rxc_sch.sch_rate = (dec_sch_rate_type)fwd_sch_packet->sch_rate;
  rxc_sch.mux_pdu_double = fwd_sch_packet->double_size_pdu;
  rxc_sch.code_type = (dec_coding_type)fwd_sch_packet->code_type;

#ifdef FEATURE_IS2000_REL_A //32X
  rxc_sch.mux_pdu_type =
    (dec_mux_pdu_type_type)fwd_sch_packet->mux_pdu_type;
  rxc_sch.num_ltu = fwd_sch_packet->num_ltu;
  rxc_sch.ltu_len = fwd_sch_packet->ltu_len;
#endif /* FEATURE_IS2000_REL_A */


  /* For SRCH logging */
  #ifdef FEATURE_IS2000_P2
  srch_mux_update_rc ( rxc_so.for_fch.included?
                       (deint_rc_type)rxc_so.for_fch.rc:DEINT_RC_INVALID,
                       (deint_rc_type)rxc_sch.rc,
                       DEINT_RC_INVALID,
                       rxc_so.for_dcch.included?
                       (deint_rc_type)rxc_so.for_dcch.rc:DEINT_RC_INVALID );
  #else
  srch_mux_update_rc ( (deint_rc_type)rxc_so.for_fch.rc,
                       (deint_rc_type)rxc_sch.rc,
                       DEINT_RC_INVALID,
                       DEINT_RC_INVALID );
  #endif  // FEATURE_IS2000_P2

#ifdef FEATURE_RDA_CORRECTION
  /* initialize DTX filter counter to -1 */
  rxcrda_sch_dtx_fix_counter = -1;
#endif /* FEATURE_RDA_CORRECTION */

  MSM_UNLOCK()

  /* Initialize the MuxPDU dsm array, if it hasn't been initialized */
  if( rxc_dsm_arr_cnt == RXC_DSM_ARR_NOT_ALLOC)
  {
    rxc_init_sch_mux();
  }

  rxc_pwr.sch_fer_rep[ fwd_sch_packet->sch_id ] = fwd_sch_packet->for_sch_fer_rep;
}
#endif /* FEATURE_IS2000_F_SCH */

#if defined (FEATURE_IS95B_MAHHO) || defined (FEATURE_GPS_MODE)
/*===========================================================================

FUNCTION RXC_FLAG_EIB

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void rxc_flag_eib( boolean eib_flag )
{
  /* Save EIBS for last 2 frames.  Upon return from a failed handoff where
   * we were away for less than 12 frames, we turn the transmitter on
   * immediately and use the 'previous' EIB in the first frame we transmit
   * and the 'current' EIB in the second frame.  The eib_flag associated
   * with the first call to this function after the return is garbage because
   * it is from a frame received while tuning back.  So we only want to
   * update the previous and current EIB values after we have transmitted
   * two frames.
   */
  if ( ( ! rxc_rifhho_cfs_gps_in_progress ) && ( ! rxc_use_stored_eibs ) )
  {
    rxc_eib_flags.previous = rxc_eib_flags.current;
    rxc_eib_flags.current = eib_flag;
  }

  if ( rxc_use_stored_eibs )
  {
    /* If txc has not received an encoder interrupt since we
     * returned, then TXC has not yet used the first EIB we
     * had stored yet, so we need to wait one more frame before
     * injecting the second one.
     */
    if ( txc_int_occurred() )
    {
      rxc_use_stored_eibs = FALSE;
    }
    else
    {
      return;
    }
  }

  txc_eib_flag = rxc_eib_flags.current;
}

#ifdef FEATURE_IS2000_REL_A
/*===========================================================================

FUNCTION RXC_FLAG_SCH_EIB

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void rxc_flag_sch_eib( boolean eib_flag )
{
  /* Save EIBS for last 2 frames.  Upon return from a failed handoff where
   * we were away for less than 12 frames, we turn the transmitter on
   * immediately and use the 'previous' EIB in the first frame we transmit
   * and the 'current' EIB in the second frame.  The eib_flag associated
   * with the first call to this function after the return is garbage because
   * it is from a frame received while tuning back.  So we only want to
   * update the previous and current EIB values after we have transmitted
   * two frames.
   */
  if ( ( ! rxc_rifhho_cfs_gps_in_progress ) && ( ! rxc_use_stored_sch_eibs ) )
  {
    rxc_sch_eib_flags.previous = rxc_sch_eib_flags.current;
    rxc_sch_eib_flags.current = eib_flag;
  }

  if ( rxc_use_stored_sch_eibs )
  {
    /* If txc has not received an encoder interrupt since we
     * returned, then TXC has not yet used the first EIB we
     * had stored yet, so we need to wait one more frame before
     * injecting the second one.
     */
    if ( txc_int_occurred() )
    {
      rxc_use_stored_sch_eibs = FALSE;
    }
    else
    {
      return;
    }
  }

  txc_sch_eib_flag = rxc_sch_eib_flags.current;
}

/*===========================================================================

FUNCTION RXC_FLAG_QIB

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void rxc_flag_qib( boolean qib_flag )
{
  /* Save QIBS for last 2 frames.  Upon return from a failed handoff where
   * we were away for less than 12 frames, we turn the transmitter on
   * immediately and use the 'previous' QIB in the first frame we transmit
   * and the 'current' QIB in the second frame.  The qib_flag associated
   * with the first call to this function after the return is garbage because
   * it is from a frame received while tuning back.  So we only want to
   * update the previous and current QIB values after we have transmitted
   * two frames.
   */
  if ( ( ! rxc_rifhho_cfs_gps_in_progress ) && ( ! rxc_use_stored_qibs ) )
  {
    rxc_qib_flags.previous = rxc_qib_flags.current;
    rxc_qib_flags.current = qib_flag;
  }

  if ( rxc_use_stored_qibs )
  {
    /* If txc has not received an encoder interrupt since we
     * returned, then TXC has not yet used the first QIB we
     * had stored yet, so we need to wait one more frame before
     * injecting the second one.
     */
    if ( txc_int_occurred() )
    {
      rxc_use_stored_qibs = FALSE;
    }
    else
    {
      return;
    }
  }

  txc_qib_flag = rxc_qib_flags.current;
}
#endif /* FEATURE_IS2000_REL_A */
#endif /* FEATURE_IS95B_MAHHO || FEATURE_GPS_MODE */

/*===========================================================================

FUNCTION RXC_TC_ISR

DESCRIPTION
  This function is the interrupt handler for the Decoder interrupt used
  when the Receive task is processing the Traffic Channel.
  Due to timing requirements imposed by the 13k decoder and the erasure
  indicator bit turnaround time we need to first process the decoded frame
  and decide what rate it is before building the final tx frame at this
  point in time.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
#ifdef FEATURE_MSM6500_TC_FRAMES_INTEGRATION
/* Declared and stored in dec5000.c dec_tc_init(); */
extern word org_frame_offset; 
#endif
void rxc_tc_isr( void )
{
  rxc_dec_type *buf_ptr;
    /* pointer to dequeued receive task buffer */

  qword tmp_qw_frame_offset;
    /* quadword to hold the rxc_frame_offset */
  byte task_id;
    /* task_id indicates if the decoder interrupt if FCH or SCH/SCCH. */

  /* frame status for FCH and DCCH
  */
  rxc_frame_type rxc_fch_frame_status  = RXC_ERASURE_FRAME;
#ifdef FEATURE_IS2000_P2
  rxc_frame_type rxc_dcch_frame_status = RXC_ERASURE_FRAME;
#endif /* FEATURE_IS2000_P2 */

  boolean log_ffpc_fch_data    = FALSE;
  boolean log_ffpc_dcch_data   = FALSE;

#ifdef FEATURE_IS2000_REL_A
  boolean qib;
#endif /* FEATURE_IS2000_REL_A */

#ifdef FEATURE_DCCH_FRAME_QUALITY_SUPERVISION  
  /* Static storage for DCCH channel quality indicator
   * between FCH/DCCH interrupt and SCH interrupt when 
   * both are present.  This value is calculated in the
   * first interrupt, and is stored here to be used in
   * building the dec message in the second interrupt.
   */
  static rxc_dcch_quality_type  dcch_chan_quality;
#endif /* End of FEATURE_DCCH_FRAME_QUALITY_SUPERVISION   */

#if defined(FEATURE_IS2000_F_SCH) && !defined(FEATURE_PLT)
  /* Variables to keep track of CRC ERR MSG */
  #define RXC_SCH_CRC_MAX_CNT 100
  static uint32 sch_crc_err = 0; // number of crc errors during last RXC_SCH_CRC_MAX_CNT framse
  static uint32 sch_crc_err_dtx_frame_cnt = 0; // number of frames since clearing sch_crc_err
  static uint32 sch_dtx_frame_cnt = 0;// number of DTX frames during last RXC_SCH_CRC_MAX_CNT framse
#endif /* FEATURE_IS2000_F_SCH && !FEATURE_PLT */

#ifdef FEATURE_PLT
#error code not present
#endif  /*  FEATURE_PLT */

#ifdef FEATURE_IS2000_SCH
  boolean ltu_flag;    /* indicates if the LTU CRC passed/failed */
  word    index;       /* dummy loop counter for LTU CRCs        */
  word    ltu_cntr;    /* number ltu's in the SDU                */
#ifdef FEATURE_IS2000_LTU_CRCS
  word    mask;        /* dummy mask for LTU CRCs                */
  word    ltu_cnt;     /* dummy counter for loops                */
#endif /* FEATURE_IS2000_LTU_CRCS */
#if defined(FEATURE_IS2000_LTU_CRCS) || defined (FEATURE_IS2000_REL_A)/* 32X */
  word    ltu_crc;     /* the ltu crc bits, after being shifted  */
#endif /* FEATURE_IS2000_LTU_CRCS || FEATURE_IS2000_REL_A */
#ifdef FEATURE_IS2000_REL_A /* 32X */
  byte    good_ltu;    /* number of good LTUs in a frame         */
#endif /* FEATURE_IS2000_REL_A */

  word    turbo_done_val = 0; 
                       /* indicates if Turbo Decoding is done    */
#endif /* FEATURE_IS2000_SCH */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - - -*/

  uint8 dec_int_status;


  dec_int_status = (uint8) HWIO_IN(DEC_DONE_INT_STATUS);
  HWIO_OUT(DEC_DONE_INT_STATUS, dec_int_status);

  if(!(dec_int_status & 0x2))
  {
    return;
  }
  
  task_id = DEINT_TASK_ID_GET();
  saved_dec_int_task_id = task_id; /* Save the task ID for debugging purpose */

#ifdef FEATURE_MSM6500_TC_FRAMES_INTEGRATION
  HWIO_OUT(DEM1X_FRAME_OFFSET, org_frame_offset);
  HWIO_OUT(VD_TESTCON,0x00);
  HWIO_OUT(VD_TEST_MEM_SEL,0x00);
  HWIO_OUT(VD_POLY2IJ, 0x1c75);
  HWIO_OUT(VD_POLY3IJ, 0x2cb7);
  HWIO_OUT(VD_POLY3K, 0x64);
  HWIO_OUT(VD_POLY4IJ, 0x2e7a);
  HWIO_OUT(VD_POLY4KL, 0x0ea5);
  HWIO_OUT(VD_RESET, 0);
  HWIO_OUT(VD_MODE, 0x3);
#endif /*FEATURE_TC_FRAMES_INTEGRATION */

  // Decerement tuneback count
  if (
      (
       (task_id == DEC_FCH_TASK) 
#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
|| (task_id == DEC_DCCH_TASK)
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */
      ) && (rxc_cfs_tuneback_count > 0)
     )
  {
    rxc_cfs_tuneback_count--;
  }

  if (
      (task_id == DEC_FCH_TASK) 
#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
      || (task_id == DEC_DCCH_TASK)
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */
      )
  {
    /* Latch the reverse power control decision history from register. */
    rxc_rpc_history = MUXMSM_RPC_GET_HISTORY();

    /* Latch the FFPC decision history from register. */
    rxc_fpc_history = MUXMSM_FPC_GET_HISTORY();

    /* Record AGC vals */
    log_rx_agc = (byte) rfm_get_cdma_rx_agc (RFCOM_TRANSCEIVER_0);
    log_tx_pwr = (byte)((rfm_get_cdma_tx_agc(RFCOM_TRANSCEIVER_0)) >>2);
    log_tx_gain_adj = srch_mux_get_tx_adj ();

    /* Start logging FFPC data after spurious interrupts are over
    */
    if (rxc_frame_offset_countdown == 0)
    {
#ifdef FEATURE_IS2000_P2
      if (rxc_so.for_dcch.included)
      {
        log_ffpc_dcch_data = TRUE;
      }
      if (rxc_so.for_fch.included)
#endif /* FEATURE_IS2000_P2 */
      {
        log_ffpc_fch_data  = TRUE;
      }

      /* Log FFPC data only if RC > 2.
      */
      if (rxc_so.high_rc)
      {
        ffpc_log_frame (rxc_frame_offset, rxc_so.pri, rxc_fpc_history,
                   log_ffpc_fch_data, log_ffpc_dcch_data, rxc_log_ffpc_sch_data);
      }
    }
  }

  if ((
       (task_id == DEC_FCH_TASK) 
#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
       || (task_id == DEC_DCCH_TASK)
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */
      ) && (rxc_sup_dec_int_enable)
     )
  {

  #ifdef FEATURE_PLT
#error code not present
  #endif  /* FEATURE_PLT */

#ifdef FEATURE_IS2000_REL_A_SVD
#ifdef FEATURE_IS2000_P2
    /* Since there are 2 frames lag time between when the DCCH/FCH is set up and 
    ** when data is valid, need to count down these 2 frames.  Without this
    ** mechanism, MUX may deliver stale data to its clients.  The time line is
    ** shown below:
    **    Frame 0:  Start Deint programming by calling deint_set_xxch_mode()
    **    Frame 1:  In ATMR ISR, decrement countdown counter
    **    Frame 2:  In rxc_tc_isr, if count down counter is 1, decrement it
    **              and set data_valid flag.
    ** The count down is set to 2 when the channel is set up.
    ** In the next frame, it is decremented in the ATMR ISR.
    ** In the frame after that, it is decremented in the TC ISR and data valid
    ** is set to TRUE only in the TC ISR.  
    ** Reason for this scheme:  in TCI state, there is no TC ISR so we must use
    ** the ATMR ISR to count down.
    ** However, in the 2nd frame, when data is valid, we must make sure data valid
    ** flag is set to TRUE to read it at the TC ISR interrupt.  ATMR interrupt happens
    ** in the next bin, when we already serviced the TC ISR.
    */
    if (
        (rxc_so.for_dcch.included) && 
        (!(rxc_so.for_dcch.data_valid)) &&
        (rxc_so.for_dcch.data_valid_countdown <= 1)
       )
    {
      rxc_so.for_dcch.data_valid_countdown = 0;
      rxc_so.for_dcch.data_valid = TRUE;
    }
    if (
        (rxc_so.for_fch.included) && 
        (!(rxc_so.for_fch.data_valid)) &&
        (rxc_so.for_fch.data_valid_countdown <= 1)
       )
    {
      rxc_so.for_fch.data_valid_countdown = 0;
      rxc_so.for_fch.data_valid = TRUE;
    }

    /* Get a time stamp of this decoder interrupt for debugging purpose.
    */
    ts_get_20ms_frame_offset_time (rxc_dec_int0_fr_time);

    /* Service DCCH channel. */
    if (rxc_so.for_dcch.included )
    {
      dec_read_dcch_status( (dec_rc_type) rxc_so.for_dcch.rc, &rxc_dcch_status);
      rxc_dcch_fcc_rate = (word) rxc_dtx_det_dcch( (dec_rc_type) rxc_so.for_dcch.rc); // determine rate
      if (rxc_dcch_fcc_rate == CAI_BLANK_RATE)
      {
        rxc_dcch_frame_status = RXC_ERASURE_FRAME;
        ffpc_update_setpt (rxc_dcch_frame_status, FFPC_DCCH);
      }
      else if(rxc_dcch_fcc_rate == CAI_NULL_RATE)
      {
        rxc_dcch_frame_status = RXC_DTX_FRAME;
        /* no need to update FFPC setpts in case of DTX frames. */
      }
      else
      {
        rxc_dcch_frame_status = RXC_GOOD_FRAME;
        ffpc_update_setpt (rxc_dcch_frame_status, FFPC_DCCH);
      }
    }

    /* Service FCH channel. */
    if (rxc_so.for_fch.included )
#endif /* FEATURE_IS2000_P2 */
    {
      dec_read_fch_status( (dec_rc_type) rxc_so.for_fch.rc, &rxc_fch_status);
      rxc_fcc_rate = rxc_dec_rate_sel(); // determine rate
      if (rxc_fcc_rate == CAI_BLANK_RATE)
      {
        rxc_fch_frame_status = RXC_ERASURE_FRAME;
      }
      else
      {
        rxc_fch_frame_status = RXC_GOOD_FRAME;
      }
      ffpc_update_setpt (rxc_fch_frame_status, FFPC_FCH);
    }
#else /* else of FEATURE_IS2000_REL_A_SVD */
    // rxc_fch_status is shared between FCH and DCCH
#ifdef FEATURE_IS2000_P2
    dec_read_fch_status( rxc_so.for_fch.included?
                         (dec_rc_type)rxc_so.for_fch.rc:
                         (dec_rc_type)rxc_so.for_dcch.rc,
                         &rxc_fch_status);
#else
    dec_read_fch_status( (dec_rc_type)rxc_so.for_fch.rc, &rxc_fch_status);
#endif // FEATURE_IS2000_P2

    rxc_fcc_rate = rxc_dec_rate_sel();

#ifdef FEATURE_IS2000_P2
    if (rxc_so.for_dcch.included)
    {
      if (rxc_fcc_rate == CAI_BLANK_RATE)
      {
        rxc_dcch_frame_status = RXC_ERASURE_FRAME;
        ffpc_update_setpt (rxc_dcch_frame_status, FFPC_DCCH);
      }
      else if (rxc_fcc_rate == CAI_NULL_RATE)
      {
        rxc_dcch_frame_status = RXC_DTX_FRAME;
        /* no need to update FFPC setpts.
        */
      }
      else
      {
        rxc_dcch_frame_status = RXC_GOOD_FRAME;
        ffpc_update_setpt (rxc_dcch_frame_status, FFPC_DCCH);
      }
    }

    if (rxc_so.for_fch.included)
#endif /* FEATURE_IS2000_P2 */
    {
      if (rxc_fcc_rate == CAI_BLANK_RATE)
      {
        rxc_fch_frame_status = RXC_ERASURE_FRAME;
      }
      else
      {
        rxc_fch_frame_status = RXC_GOOD_FRAME;
      }
      ffpc_update_setpt (rxc_fch_frame_status, FFPC_FCH);
    }
#endif /* FEATURE_IS2000_REL_A_SVD */

#ifdef FEATURE_DCCH_FRAME_QUALITY_SUPERVISION  
    /* Call the function to determine DCCH channel quality */
    if (rxc_so.for_dcch.included )
    {
      dcch_chan_quality = rxc_dcch_quality_det (rxc_dcch_frame_status);
    }
#endif /* End of FEATURE_DCCH_FRAME_QUALITY_SUPERVISION   */
    

#ifdef FEATURE_IS2000_REL_A
#ifdef FEATURE_IS2000_P2
#ifdef FEATURE_IS2000_REL_A_SVD
    if (ffpc_channel == CAI_FPC_PRI_CHAN_DCCH)
#else
    if (rxc_so.for_dcch.included)
#endif /* FEATURE_IS2000_REL_A_SVD */
    {
      qib = (dcch_chan_quality == RXC_POOR_CHANNEL );
    }
    else /* fch case below */
#endif /* FEATURE_IS2000_P2 */
    {
      qib = (rxc_fch_frame_status != RXC_GOOD_FRAME);
    }

#if defined(FEATURE_IS95B_MAHHO) || defined(FEATURE_GPS_MODE)
    rxc_flag_qib(qib);
#else
    txc_qib_flag = qib;
#endif /* FEATURE_IS95B_MAHHO || FEATURE_GPS_MODE */
#endif /* FEATURE_IS2000_REL_A */



    if 
    (
      (
#ifdef FEATURE_IS2000_P2
#ifdef FEATURE_IS2000_REL_A_SVD
        (ffpc_channel == CAI_FPC_PRI_CHAN_DCCH) &&
#else
        (rxc_so.for_dcch.included) &&
#endif /* FEATURE_IS2000_REL_A_SVD */
        (rxc_dcch_frame_status == RXC_GOOD_FRAME)
      )
      || 
      (
#ifdef FEATURE_IS2000_REL_A_SVD
        (ffpc_channel == CAI_FPC_PRI_CHAN_FCH) &&
#else
        (rxc_so.for_fch.included) &&
#endif /* FEATURE_IS2000_REL_A_SVD */
#endif /* FEATURE_IS2000_P2 */
        (rxc_fch_frame_status == RXC_GOOD_FRAME)
      )
    )
    {
#if defined(FEATURE_IS95B_MAHHO) || defined(FEATURE_GPS_MODE)
      rxc_flag_eib (FALSE) ;
#else
      txc_eib_flag = FALSE;
#endif /* FEATURE_IS95B_MAHHO || FEATURE_GPS_MODE */
    }
    else
    {
      /* flag to indicate EIB bit pattern to be punctured on reverse link
      ** for FCH since there will be no DTX nothing changes,
      ** for DCCH, ERASURE and DTX frames are to be treated identically.
      */
#if defined(FEATURE_IS95B_MAHHO) || defined(FEATURE_GPS_MODE)
      rxc_flag_eib (TRUE) ;
#else
      txc_eib_flag = TRUE;
#endif /* FEATURE_IS95B_MAHHO || FEATURE_GPS_MODE */
    }

    /* Return from the interrupt because sup channel interrupt will
    ** follow this */
    return;
  }

  /* Perform Normal interrupt processing */
  if(rxc_frame_offset_countdown > 0) {

    /* rxc_frame_offset_countdown is used to skip over the troublesome,
    ** sporadic interrupts and correct any timing errors that may have occured.
    */

    rxc_frame_offset_countdown--;

    if(rxc_frame_offset_countdown == 0) {
      /* The MSM3000 should be settled into normal 20 ms traffic frames now.
      ** Compute the correct rxc_dec_frame and then increment from there.  This
      ** is a time correction to fix any off by 1 errors on rxc_dec_frame.
      */
      ts_get ( rxc_dec_frame );

      /* Divide by 64*1024 to truncate the low 16 bits of the time stamp,
      ** then divide the 1.25 ms units by 16 to get 20ms units.
      */
      (void) qw_div_by_power_of_2( rxc_dec_frame, rxc_dec_frame, 16 );
      qw_set( tmp_qw_frame_offset, 0L, rxc_frame_offset);
      qw_sub( rxc_dec_frame, rxc_dec_frame, tmp_qw_frame_offset );
      (void) qw_div_by_power_of_2( rxc_dec_frame, rxc_dec_frame, 4 );

      /* rxc_dec_frame holds the number of the decoded frame, which is 20ms
       * less than the current frame time. */
      qw_dec( rxc_dec_frame, 1L );
    } else {
      qw_inc( rxc_dec_frame, 1L );
    }
  }
  else
  {
    qw_inc( rxc_dec_frame, 1L );
  }


  /* Copy decoder data into buffer for task */
  if ( (buf_ptr = (rxc_dec_type *)q_get( &rxc_dec_free_q)) == NULL )
  {
    ERR( "0 bufs on dec_free_q", 0, 0, 0 );
  }
  else
  {
#if defined (FEATURE_IS95B_MDR)
    /* Initialize the number of SCCH to 0 */
    buf_ptr->buf.tc_buf.scc.num_sup = 0;
#endif /* (FEATURE_IS95B_MDR) */
    /* Determine the rate of the received forward traffic channel frame.
       Note that this rate determination is independent of decoded data and
       uses only CRC, QBITS, energy, and SER parameters */
#ifdef FEATURE_IS2000_REL_A_SVD
    /* If rate determination has not been done, then do rate determination.
    ** This check used to be for rxc_sup_dec_int_enable only.  Checking
    ** rxc_sup_dec_int_enable only is not robust enough in case this flag
    ** is set but the FCH/DCCH interrupt which precedes the SCH interrupt 
    ** in the same frame did not execute.  This anomaly is very rare but 
    ** can occur due to possible INTLOCK or other SW conditions which 
    ** prevent the FCH/DCCH interrupt from happening.  By the time the SCH 
    ** interrupt happens, the Deint task ID will only reflects the SCH 
    ** interrupt, the FCH/DCCH interrupt is completely lost.  In this 
    ** bad case, even though rxc_sup_dec_int_enable is set, rate determination
    ** has not been done.
    **
    ** So it is safer to add validation for rxc_dcch_fcc_rate and rxc_fcc_rate
    ** as well.  If the content of these variables is greater than the max 
    ** possible valid rate (they are reset to an invalid value every frame, 
    ** after they are used), then do rate determination.  If the content of 
    ** these rates is valid, then use them as decoder rate for the frame and 
    ** skip rate determination since it has been done earlier in the frame.
    */
    if (
        (!rxc_sup_dec_int_enable) ||
        ((rxc_so.for_dcch.included) && (rxc_dcch_fcc_rate > CAI_NULL_RATE)) ||
        ((rxc_so.for_fch.included) && (rxc_fcc_rate > CAI_NULL_RATE))
       )
    {
#ifdef FEATURE_IS2000_P2
      /* This is a FCH/DCCH Interrupt, Supplemental Channel not enabled 
      ** and rate determination has not been done yet. 
      */

      /* Since there are 2 frames lag time between when the DCCH/FCH is set up and 
      ** when data is valid, need to count down these 2 frames.  Without this
      ** mechanism, MUX may deliver stale data to its clients.  The time line is
      ** shown below:
      **    Frame 0:  Start Deint programming by calling deint_set_xxch_mode()
      **    Frame 1:  In ATMR ISR, decrement countdown counter
      **    Frame 2:  In rxc_tc_isr, if count down counter is 1, decrement it
      **              and set data_valid flag.
      ** The count down is set to 2 when the channel is set up.
      ** In the next frame, it is decremented in the ATMR ISR.
      ** In the frame after that, it is decremented in the TC ISR and data valid
      ** is set to TRUE only in the TC ISR.  
      ** Reason for this scheme:  in TCI state, there is no TC ISR so we must use
      ** the ATMR ISR to count down.
      ** However, in the 2nd frame, when data is valid, we must make sure data valid
      ** flag is set to TRUE to read it at the TC ISR interrupt.  ATMR interrupt happens
      ** in the next bin, when we already serviced the TC ISR.
      */
      if (
          (rxc_so.for_dcch.included) && 
          (!(rxc_so.for_dcch.data_valid)) &&
          (rxc_so.for_dcch.data_valid_countdown <= 1)
         )
      {
        rxc_so.for_dcch.data_valid_countdown = 0;
        rxc_so.for_dcch.data_valid = TRUE;
      }

      if (
          (rxc_so.for_fch.included) && 
          (!(rxc_so.for_fch.data_valid)) &&
          (rxc_so.for_fch.data_valid_countdown <= 1)
         )
      {
        rxc_so.for_fch.data_valid_countdown = 0;
        rxc_so.for_fch.data_valid = TRUE;
      }

      if (rxc_so.for_dcch.included) 
      {
        /* Read status */
        dec_read_dcch_status( (dec_rc_type) rxc_so.for_dcch.rc, &rxc_dcch_status);    

        /* Determine rate */
        buf_ptr->dcch_dec_rate = (cai_data_rate_type) rxc_dtx_det_dcch( (dec_rc_type) rxc_so.for_dcch.rc); 
      }
#endif /* FEATURE_IS2000_P2 */
      if (rxc_so.for_fch.included)  
      {
        /* Read status */
        dec_read_fch_status( (dec_rc_type) rxc_so.for_fch.rc, &rxc_fch_status);   

        /* Determine rate */
        buf_ptr->dec_rate = (cai_data_rate_type) rxc_dec_rate_sel();                     
      }
    }
    else
    {
      /* Get a time stamp of this decoder interrupt for debugging purpose.
      */
      ts_get_20ms_frame_offset_time (rxc_dec_int1_fr_time);

      /* This is the SCH interrupt after an FCH/DCCH interrupt and
      ** rate determination has already been done. 
      */
      if (rxc_so.for_dcch.included) 
      {
        /* Copy rate from saved value. */
        buf_ptr->dcch_dec_rate = (cai_data_rate_type) rxc_dcch_fcc_rate;
        ASSERT (buf_ptr->dcch_dec_rate <= CAI_NULL_RATE);

        /* Clear saved value after use */
        rxc_dcch_fcc_rate = RXC_NO_RATE; 
      }
      if (rxc_so.for_fch.included)
      {
        /* Copy rate from saved value. */
        buf_ptr->dec_rate = (cai_data_rate_type) rxc_fcc_rate;
        ASSERT (buf_ptr->dec_rate <= CAI_NULL_RATE);

        /* Clear saved value after use */
        rxc_fcc_rate = RXC_NO_RATE; 
      }
    }

    if (rxc_so.for_dcch.included )
    {
      if (buf_ptr->dcch_dec_rate == CAI_BLANK_RATE)
      {
        rxc_dcch_frame_status = RXC_ERASURE_FRAME;
      }
      else if(buf_ptr->dcch_dec_rate == CAI_NULL_RATE)
      {
        rxc_dcch_frame_status = RXC_DTX_FRAME;
      }
      else
      {
        rxc_dcch_frame_status = RXC_GOOD_FRAME;
      }
    }
    if (rxc_so.for_fch.included )
    {
      if (buf_ptr->dec_rate == CAI_BLANK_RATE)
      {
        rxc_fch_frame_status = RXC_ERASURE_FRAME;
      }
      else
      {
        rxc_fch_frame_status = RXC_GOOD_FRAME;
      }
    }

#ifdef FEATURE_DCCH_FRAME_QUALITY_SUPERVISION  
    /* Call the function to determine DCCH channel quality */
    if (rxc_so.for_dcch.included )
    {
      if (rxc_sup_dec_int_enable)
      {
        /* indication was already calculated in previous interrupt. */
        buf_ptr->dcch_chan_quality = dcch_chan_quality;
      }
      else
      {
        dcch_chan_quality = buf_ptr->dcch_chan_quality = rxc_dcch_quality_det(rxc_dcch_frame_status);
      }
    }
#endif /* End of FEATURE_DCCH_FRAME_QUALITY_SUPERVISION   */

    /* If rxc_sup_dec_int_enable is TRUE, then this eib/qib reporting
     * has already occurred.  Skip the duplicate reporting for efficiency
     * and correctness, since rxc_flag_eib() and rxc_flag_qib() have
     * current and previous flags.
     */
    if (!rxc_sup_dec_int_enable)
    {

#ifdef FEATURE_IS2000_REL_A
#ifdef FEATURE_IS2000_P2
#ifdef FEATURE_IS2000_REL_A_SVD
      if (ffpc_channel == CAI_FPC_PRI_CHAN_DCCH)
#else
      if (rxc_so.for_dcch.included)
#endif /* FEATURE_IS2000_REL_A_SVD */
      {
        qib = (dcch_chan_quality == RXC_POOR_CHANNEL );
      }
      else /* fch case below */
#endif /* FEATURE_IS2000_P2 */
      {
        qib = (rxc_fch_frame_status != RXC_GOOD_FRAME);
      }

      /* set status only if primary  */
#if defined(FEATURE_IS95B_MAHHO) || defined(FEATURE_GPS_MODE)
      rxc_flag_qib(qib);
#else
      txc_qib_flag = qib;
#endif /* FEATURE_IS95B_MAHHO || FEATURE_GPS_MODE */
#endif /* FEATURE_IS2000_REL_A */



      if 
      (
        (
#ifdef FEATURE_IS2000_P2
#ifdef FEATURE_IS2000_REL_A_SVD
          (ffpc_channel == CAI_FPC_PRI_CHAN_DCCH) &&
#else
          (rxc_so.for_dcch.included) &&
#endif /* FEATURE_IS2000_REL_A_SVD */
          (rxc_dcch_frame_status == RXC_GOOD_FRAME)
        )
        || 
        (
#ifdef FEATURE_IS2000_REL_A_SVD
          (ffpc_channel == CAI_FPC_PRI_CHAN_FCH) &&
#else
          (rxc_so.for_fch.included) &&
#endif /* FEATURE_IS2000_REL_A_SVD */
#endif /* FEATURE_IS2000_P2 */
          (rxc_fch_frame_status == RXC_GOOD_FRAME)
        )
      )
      {
#if defined(FEATURE_IS95B_MAHHO) || defined(FEATURE_GPS_MODE)
        rxc_flag_eib (FALSE) ;
#else
        txc_eib_flag = FALSE;
#endif /* FEATURE_IS95B_MAHHO || FEATURE_GPS_MODE */
      }
      else
      {
        /* flag to indicate EIB bit pattern to be punctured on reverse link
        ** for FCH since there will be no DTX nothing changes,
        ** for DCCH, ERASURE and DTX frames are to be treated identically.
        */
#if defined(FEATURE_IS95B_MAHHO) || defined(FEATURE_GPS_MODE)
        rxc_flag_eib (TRUE) ;
#else
        txc_eib_flag = TRUE;
#endif /* FEATURE_IS95B_MAHHO || FEATURE_GPS_MODE */
      }
    } /* !rxc_sup_dec_int_enable */

    if(rxc_frame_offset_countdown == 0)
    {
      /* troublesome, sporadic interrupts over, should be settled into
      ** normal 20 ms traffic frames now. Start processing FFPC.
      */

      /* SETPT already updated and set in HW if SCH is enabled.
      ** Update setpoint and write to HW here if only F-FCH/DCCH is on.
      */
      if (!rxc_sup_dec_int_enable)
      {
        /* Used in simultaneous F-FCH and F-DCCH channel set up.  
        */
#ifdef FEATURE_IS2000_P2
        if (rxc_so.for_dcch.included )
        {
          ffpc_update_setpt (rxc_dcch_frame_status, FFPC_DCCH);
        }

        if (rxc_so.for_fch.included )
#endif /* FEATURE_IS2000_P2 */
        {
          ffpc_update_setpt (rxc_fch_frame_status, FFPC_FCH);
        }
      }
    }
    /* If determined rate is not erasure or blank then read the data from
    ** the decoder buffer only for the selected rate */

    if (rxc_so.for_fch.included )
    {
      if ((buf_ptr->dec_rate != CAI_BLANK_RATE) &&
          buf_ptr->dec_rate != CAI_NULL_RATE)
      {
    
        // Read in FCH data here
        dec_read_fch_data( &buf_ptr->buf.tc_buf.bits, (dec_rc_type)rxc_so.for_fch.rc,
                       DEC_RATE_ALL);
      }
    }
    if (rxc_so.for_dcch.included )
    {
      if ((buf_ptr->dcch_dec_rate != CAI_BLANK_RATE) &&
        buf_ptr->dcch_dec_rate != CAI_NULL_RATE)
      {
        dec_read_dcch_data( &buf_ptr->buf.tc_buf.bits, (dec_rc_type)rxc_so.for_dcch.rc);
      }
    }
#else /* else of FEATURE_IS2000_REL_A_SVD */
    if(!rxc_sup_dec_int_enable || (rxc_fcc_rate == RXC_NO_RATE))
    {
      /* This is a FCH/DCCH Interrupt: Supplemental Channel not enabled
       * or this is the SCH interrupt and rate determination has not
       * been done yet. */
      // rxc_fch_status is shared between FCH and DCCH
      #ifdef FEATURE_IS2000_P2
      dec_read_fch_status( rxc_so.for_fch.included?
                           (dec_rc_type)rxc_so.for_fch.rc:
                           (dec_rc_type)rxc_so.for_dcch.rc,
                           &rxc_fch_status);
      #else
      dec_read_fch_status( rxc_so.for_fch.rc, &rxc_fch_status);
      #endif // FEATURE_IS2000_P2

      /* Determine Rate */
      buf_ptr->dec_rate = (cai_data_rate_type) rxc_dec_rate_sel();
    }
    else
    {
      /* This is the SCH interrupt after an FCH interrupt and
       * rate determination has already been done. */
      buf_ptr->dec_rate = (cai_data_rate_type) rxc_fcc_rate;
      rxc_fcc_rate = RXC_NO_RATE; /* Clear it after use */
    }

#ifdef FEATURE_IS2000_P2
    if (rxc_so.for_dcch.included )
    {
      if (buf_ptr->dec_rate == CAI_BLANK_RATE)
      {
        rxc_dcch_frame_status = RXC_ERASURE_FRAME;
      }
      else if (buf_ptr->dec_rate == CAI_NULL_RATE)
      {
        rxc_dcch_frame_status = RXC_DTX_FRAME;
      }
      else
      {
        rxc_dcch_frame_status = RXC_GOOD_FRAME;
      }
    }

    if (rxc_so.for_fch.included )
#endif /* FEATURE_IS2000_P2 */
    {
      if (buf_ptr->dec_rate == CAI_BLANK_RATE)
      {
        rxc_fch_frame_status = RXC_ERASURE_FRAME;
      }
      else
      {
        rxc_fch_frame_status = RXC_GOOD_FRAME;
      }
    }

#ifdef FEATURE_DCCH_FRAME_QUALITY_SUPERVISION  
    /* Call the function to determine DCCH channel quality */
    if (rxc_so.for_dcch.included )
    {
      if (rxc_sup_dec_int_enable)
      {
        /* indication was already calculated in previous interrupt. */
        buf_ptr->dcch_chan_quality = dcch_chan_quality;
      }
      else
      {
        dcch_chan_quality = buf_ptr->dcch_chan_quality = rxc_dcch_quality_det (rxc_dcch_frame_status);
      }
    }
#endif /* End of FEATURE_DCCH_FRAME_QUALITY_SUPERVISION   */

    /* If rxc_sup_dec_int_enable is TRUE, then this eib/qib reporting
     * has already occurred.  Skip the duplicate reporting for efficiency
     * and correctness, since rxc_flag_eib() and rxc_flag_qib() have
     * current and previous flags.
     */
    if (!rxc_sup_dec_int_enable)
    {
#ifdef FEATURE_IS2000_REL_A
#ifdef FEATURE_IS2000_P2
#ifdef FEATURE_IS2000_REL_A_SVD
    if (ffpc_channel == CAI_FPC_PRI_CHAN_DCCH)
#else
    if (rxc_so.for_dcch.included)
#endif /* FEATURE_IS2000_REL_A_SVD */
    {
      qib = (dcch_chan_quality == RXC_POOR_CHANNEL );
    }
    else /* fch case below*/
#endif /* FEATURE_IS2000_P2 */
    {
      qib = (rxc_fch_frame_status != RXC_GOOD_FRAME);
    }

#if defined(FEATURE_IS95B_MAHHO) || defined(FEATURE_GPS_MODE)
    rxc_flag_qib(qib);
#else
    txc_qib_flag = qib;
#endif /* FEATURE_IS95B_MAHHO || FEATURE_GPS_MODE */
#endif /* FEATURE_IS2000_REL_A */

      if 
      (
        (
#ifdef FEATURE_IS2000_P2
#ifdef FEATURE_IS2000_REL_A_SVD
          (ffpc_channel == CAI_FPC_PRI_CHAN_DCCH) &&
#else
          (rxc_so.for_dcch.included) &&
#endif /* FEATURE_IS2000_REL_A_SVD */
          (rxc_dcch_frame_status == RXC_GOOD_FRAME)
        )
        || 
        (
#ifdef FEATURE_IS2000_REL_A_SVD
          (ffpc_channel == CAI_FPC_PRI_CHAN_FCH) &&
#else
          (rxc_so.for_fch.included) &&
#endif /* FEATURE_IS2000_REL_A_SVD */
#endif /* FEATURE_IS2000_P2 */
          (rxc_fch_frame_status == RXC_GOOD_FRAME)
        )
      )
      {
#if defined(FEATURE_IS95B_MAHHO) || defined(FEATURE_GPS_MODE)
        rxc_flag_eib (FALSE) ;
#else
        txc_eib_flag = FALSE;
#endif /* FEATURE_IS95B_MAHHO || FEATURE_GPS_MODE */
      }
      else
      {
        /* flag to indicate EIB bit pattern to be punctured on reverse link
        ** for FCH since there will be no DTX nothing changes,
        ** for DCCH, ERASURE and DTX frames are to be treated identically.
        */
#if defined(FEATURE_IS95B_MAHHO) || defined(FEATURE_GPS_MODE)
        rxc_flag_eib (TRUE) ;
#else
        txc_eib_flag = TRUE;
#endif /* FEATURE_IS95B_MAHHO || FEATURE_GPS_MODE */
      }

    } /* !rxc_sup_dec_int_enable */

    if(rxc_frame_offset_countdown == 0)
    {
      /* troublesome, sporadic interrupts over, should be settled into
      ** normal 20 ms traffic frames now. Start processing FFPC.
      */

      /* SETPT already updated and set in HW if SCH is enabled.
      ** Update setpoint and write to HW here if only F-FCH/DCCH is on.
      */
      if (!rxc_sup_dec_int_enable)
      {
#ifdef FEATURE_IS2000_P2
        if ( rxc_so.for_dcch.included )
        {
          ffpc_update_setpt (rxc_dcch_frame_status, FFPC_DCCH);
        }

        if ( rxc_so.for_fch.included )
#endif /* FEATURE_IS2000_P2 */
        {
          ffpc_update_setpt (rxc_fch_frame_status, FFPC_FCH);
        }
      }
    }


    /* If determined rate is not erasure or blank then read the data from
    ** the decoder buffer only for the selected rate */

    if ((buf_ptr->dec_rate != CAI_BLANK_RATE) &&
        buf_ptr->dec_rate != CAI_NULL_RATE)
    {

      /* get bits from decoder */
#ifdef FEATURE_IS2000_P2
      if ( rxc_so.for_dcch.included )
      {
        // Read in DCCH data here.  FCH and DCCH are shared
        dec_read_fch_data( &buf_ptr->buf.tc_buf.bits, (dec_rc_type)rxc_so.for_dcch.rc,
                           DEC_RATE_FULL);
      }
      else  // We have an else here since only one of FCH/DCCH is supported
#endif // FEATURE_IS2000_P2
      {
        // Read in DCCH data here
        dec_read_fch_data( &buf_ptr->buf.tc_buf.bits, (dec_rc_type)rxc_so.for_fch.rc,
                         DEC_RATE_ALL);
      }
    }
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */

#ifdef FEATURE_PLT
#error code not present
#endif /* FEATURE_PLT */

#ifdef FEATURE_IS2000_F_SCH
    /*
     * NOTE: rxc_update_sch_info() needs to get called before struct rxc_sch
     * is accessed.
     * rxc_sch_update_counter > 0 indicates that a new burst has started.
     * This counter will eqaul 0 when the first SCH interrupt corresponding
     * to the new burst is received, and rxc_sch needs to be updated to reflect
     * the new SCH parameter values.
     */
#ifdef FEATURE_IS2000_REL_A
    if ( (rxc_turbo_dec_enabled && (task_id == DEC_TURBO_SCH_TASK))
          || (rxc_sup_dec_int_enable && (task_id == DEC_SCCH_SCH_TASK)) )
#else
    if ((rxc_turbo_dec_enabled || rxc_sup_dec_int_enable) &&
        (task_id == DEC_SCCH_SCH_TASK))
#endif /* FEATURE_IS2000_REL_A */
    {
       if (rxc_sch_update_counter > 0)
       {
        if ( --rxc_sch_update_counter == 0 )
          {
             rxc_update_sch_info( &rxc_stored_sch_packet );
          }
       }
    }

    if (!rxc_turbo_dec_enabled && !rxc_sup_dec_int_enable )
    {
      rxc_sch_frame_status = RXC_NO_FRAME; // initialize to default
    }
#endif
    /* If this is a turbo SCH interrupt */
#ifdef FEATURE_IS2000_REL_A
    if ( rxc_turbo_dec_enabled && (task_id == DEC_TURBO_SCH_TASK) )
#else
    if ( rxc_turbo_dec_enabled && (task_id == DEC_SCCH_SCH_TASK) )
#endif /* FEATURE_IS2000_REL_A */
    {
#if defined(FEATURE_IS2000_F_SCH) && defined(MSMHW_TURBO_CODING)
      DEC_DYNAMIC_CLK_OFF ();
      turbo_done_val = MUXMSM_GET_TD_STATUS_DONE(); 
      DEC_DYNAMIC_CLK_ON ();
      if (turbo_done_val > 0) 
      {
#if defined(FEATURE_IS2000_F_SCH) && !defined(FEATURE_PLT)
          rxc_frame_type frame_type = RXC_ERASURE_FRAME;

#ifdef FEATURE_RDA_CORRECTION
        rxc_frame_type det_frame_type = RXC_ERASURE_FRAME;
#endif /* FEATURE_RDA_CORRECTION */

#ifdef FEATURE_IS2000_REL_A /* 32X */
        dec_read_sch_status((dec_mux_pdu_type_type) rxc_sch.mux_pdu_type, (dec_rc_type)rxc_sch.rc, rxc_sch.sch_rate,
                            &rxc_sch_status, DEC_TURBO);
#else
        dec_read_sch_status((dec_rc_type)rxc_sch.rc, rxc_sch.sch_rate,
                            &rxc_sch_status, DEC_TURBO);
#endif /* 32X */

        // Statistics for power measurement report
        if( rxc_pwr.sch_fer_rep[0] == TRUE )
        {
          rxc_pwr.sch_tot_frames[0]++;
        }

        /* DTX only supported for RC3 and RC4 */
        if ((rxc_sch.rc==DEC_RC3) || (rxc_sch.rc==DEC_RC4) ||
            (rxc_sch.rc==DEC_RC5))
        {
#ifdef FEATURE_RDA_CORRECTION
          det_frame_type = rxc_dtx_det_sch((dec_rc_type)rxc_sch.rc,
                             rxc_sch_status.turbo_sch_status.crc_pass);
          frame_type = rxc_sch_dtx_filter (det_frame_type);
#else
          frame_type = rxc_dtx_det_sch((dec_rc_type)rxc_sch.rc,
                             rxc_sch_status.turbo_sch_status.crc_pass);
#endif /* FEATURE_RDA_CORRECTION */
        }
        else
        {
          frame_type = RXC_DTX_FRAME;
          ERR("BAD RC in RXC_TC_ISR %d",rxc_sch.rc,0,0);
        }

        if (frame_type == RXC_GOOD_FRAME)
        {
#ifndef FEATURE_PLT
          /* SCH may be assigned for a voice call.  In this case, don't
          ** read SCH data, otherwise debug printf may be printed every
          ** frame, causing confusion.
          */
          if ( 
              (rxc_so.pri == CAI_SO_PPP_PKT_DATA_3G) ||
              (rxc_so.sec == CAI_SO_PPP_PKT_DATA_3G) ||
              (rxc_so.pri == CAI_SO_TDSO)            ||
              (rxc_so.sec == CAI_SO_TDSO)            ||
              (rxc_so.pri == CAI_SO_FULL_TDSO)       ||
              (rxc_so.sec == CAI_SO_FULL_TDSO)
             )
          {
            /* Copy SCH data into dsm_item array */
            if( rxc_dsm_arr_cnt == 0)
            {
#ifdef FEATURE_IS2000_REL_A /* 32X */
              if (rxc_sch.mux_pdu_type == DEC_MUX_PDU_TYPE_5)
              {
                // 32X turbo support here
                dec_read_mux_pdu5_sch_data(&rxc_sch,
                           &rxc_dsm_arr[0],
                           &rxc_dsm_arr_cnt,
                           0xFF,
                           &good_ltu
                           );
              }
              else /* MuxPDU type 1, 2, and 3, use old way */
#endif /* FEATURE_IS2000_REL_A */
              {
                dec_read_sch_data( &rxc_sch, &rxc_dsm_arr[0], &rxc_dsm_arr_cnt);
              }
        
              for (index = 0; index < rxc_dsm_arr_cnt; index++)
              {
                rxc_dsm_arr[index]->app_field = TRUE;
              }
            }
            else /* dsm array has frames */
            {
              ERR("RX tsk can't do SCH, arr_cnt=%d",
                    rxc_dsm_arr_cnt, 0, 0);
            }
          }
#else   
#error code not present
#endif
          
#ifdef FEATURE_IS2000_REL_A /* 32X */
          if (rxc_sch.mux_pdu_type == DEC_MUX_PDU_TYPE_5)
          {
            /* MO 0xf20 (MuxPDU 5) has its own set of counters */
            switch(rxc_sch.sch_rate)
            {
              case DEC_SCH_RATE_1X:
                (void) parm_inc(CAI_FSCH0_1_ID, 1);
                break;
              case DEC_SCH_RATE_2X:
                (void) parm_inc(CAI_FSCH0_2_ID, 1);
                break;
              case DEC_SCH_RATE_4X:
                (void) parm_inc(CAI_FSCH0_3_ID, 1);
                break;
              case DEC_SCH_RATE_8X:
                (void) parm_inc(CAI_FSCH0_4_ID, 1);
                break;
              case DEC_SCH_RATE_16X:
                (void) parm_inc(CAI_FSCH0_5_ID, 1);
                break;
              case DEC_SCH_RATE_32X:
                (void) parm_inc(CAI_FSCH0_6_ID, 1);
                break;
            }
          }
          else  /* The statistics for any MO but 0xF20 */
#endif /* FEATURE_IS2000_REL_A */
          {
          switch(rxc_sch.sch_rate)
          {
            case DEC_SCH_RATE_1X:
                (void) parm_inc(CAI_SCH0_FWD_1X_ID, 1);
              break;
            case DEC_SCH_RATE_2X:
                (void) parm_inc(CAI_SCH0_FWD_2X_ID, 1);
              break;
            case DEC_SCH_RATE_4X:
                (void) parm_inc(CAI_SCH0_FWD_4X_ID, 1);
              break;
            case DEC_SCH_RATE_8X:
                (void) parm_inc(CAI_SCH0_FWD_8X_ID, 1);
              break;
            case DEC_SCH_RATE_16X:
                (void) parm_inc(CAI_SCH0_FWD_16X_ID, 1);
                break;
              default:
                MSG_ERROR("Bad SCH Rate %d",rxc_sch.sch_rate,0,0);
              break;
          }
          }

          rxc_sch_frame_status = RXC_GOOD_FRAME;

          ffpc_update_setpt (rxc_sch_frame_status, FFPC_SCH);
        }
        else if (frame_type == RXC_ERASURE_FRAME)
        {
          // Statistics for power measurement report
          if( rxc_pwr.sch_fer_rep[0] == TRUE )
          {
            rxc_pwr.sch_bad_frames[0]++;
          }
#if defined(FEATURE_RLP_LOGGING)
          /* Indicate that erasure occured on SCH */
          rxc_rlp_logging_chan_info.log_turbo_sch_erasure = TRUE;
#endif
          (void) parm_xtra_inc(CAI_XTRA_SCH0_FWD_SDU_ERR_ID, 1);

          /* Increment erasure count to be printed every RXC_SCH_CRC_MAX_CNT frames */
          sch_crc_err++;

          rxc_sch_frame_status = RXC_ERASURE_FRAME;

          ffpc_update_setpt (rxc_sch_frame_status, FFPC_SCH);
        } else { /* DTX */
          (void) parm_xtra_inc(CAI_XTRA_SCH0_FWD_SDU_DTX_ID, 1);
          /* do nothing for DTX, &rxc_dsm_arr array does not get filled,
          ** we treat DTX like an erasure. */
          rxc_sch_frame_status = RXC_DTX_FRAME;
          sch_dtx_frame_cnt++;

#ifdef FEATURE_RDA_CORRECTION
          if (det_frame_type == RXC_ERASURE_FRAME)
          {
             MSG_HIGH("fixed ERASURE -> DTX frame", 0, 0, 0);
          }
#endif /* FEATURE_RDA_CORRECTION */
        }

        if (++sch_crc_err_dtx_frame_cnt >= RXC_SCH_CRC_MAX_CNT)
        {
          MSG_HIGH("Rcvd %d era and %d DTX in %d SCH frames", 
              sch_crc_err,sch_dtx_frame_cnt,sch_crc_err_dtx_frame_cnt);
          sch_crc_err_dtx_frame_cnt = 0;
          sch_crc_err = 0;
          sch_dtx_frame_cnt = 0;
        }

#endif /* FEATURE_IS2000_F_SCH && !FEATURE_PLT*/

#ifdef FEATURE_IS2000_REL_A
#if defined(FEATURE_IS95B_MAHHO) || defined(FEATURE_GPS_MODE)
        if (rxc_sch_frame_status == RXC_GOOD_FRAME )
        {
          rxc_flag_sch_eib( FALSE );
        }
        else
        {
          rxc_flag_sch_eib( TRUE );
        }
#else
        txc_sch_eib_flag = (rxc_sch_frame_status == RXC_ERASURE_FRAME);
#endif /* FEATURE_IS95B_MAHHO || FEATURE_GPS_MODE */
#endif /* FEATURE_IS2000_REL_A */


#ifdef FEATURE_PLT
#error code not present
#endif /* FEATURE_PLT */
      }
      else
      {
        MSG_ERROR("SCH Int with no TD done",0,0,0);
      }
#endif /* FEATURE_IS2000_F_SCH && MSMHW_TURBO_CODING */
    }  /* End turbo SCH interrupt */

    /* Convolutional SCH interrupt */
    if( (task_id == DEC_SCCH_SCH_TASK) && (rxc_sup_dec_int_enable) &&
            (! rxc_turbo_dec_enabled) )
    {
      /* Based on the Radio Configuration, we check if it is a SCCH
         or SCH interrupt */
      if ( rxc_so.high_rc )  /* SCH Task Interrupt */
      {
#if defined( FEATURE_IS2000_F_SCH ) && !defined( FEATURE_PLT )
        /* Supplemental Channel Interrupt Processing */

#ifdef FEATURE_IS2000_REL_A /* 32X */
        dec_read_sch_status((dec_mux_pdu_type_type) rxc_sch.mux_pdu_type,(dec_rc_type)rxc_sch.rc, rxc_sch.sch_rate,
                            &rxc_sch_status, DEC_CONVOLUTIONAL);
#else
        dec_read_sch_status((dec_rc_type)rxc_sch.rc, rxc_sch.sch_rate,
                            &rxc_sch_status, DEC_CONVOLUTIONAL);
#endif /* 32X */

        if(rxc_sch.code_type == DEC_CONVOLUTIONAL)
        {
#ifdef FEATURE_RDA_CORRECTION
          rxc_frame_type det_frame_type = RXC_ERASURE_FRAME;
#endif /* FEATURE_RDA_CORRECTION */
          rxc_frame_type frame_type = RXC_ERASURE_FRAME;
          // Statistics for power measurement report
          if( rxc_pwr.sch_fer_rep[0] == TRUE )
          {
            rxc_pwr.sch_tot_frames[0]++;
          }
          /* DTX supported for RC3, RC4 and RC5 */
          if ((rxc_sch.rc==DEC_RC3) || (rxc_sch.rc==DEC_RC4) || (rxc_sch.rc==DEC_RC5))
          {
#ifdef FEATURE_RDA_CORRECTION
            det_frame_type = rxc_dtx_det_sch((dec_rc_type)rxc_sch.rc,
                               rxc_sch_status.conv_sch_status.status & 0x1);
            frame_type = rxc_sch_dtx_filter (det_frame_type);
#else
            frame_type = rxc_dtx_det_sch((dec_rc_type)rxc_sch.rc,
                               rxc_sch_status.conv_sch_status.status & 0x1);
#endif /* FEATURE_RDA_CORRECTION */
          }
          else
          {
            frame_type = RXC_DTX_FRAME;
            ERR("BAD RC in RXC_TC_ISR %d",rxc_sch.rc,0,0);
          }

#ifndef FEATURE_PLT
          if (frame_type != RXC_DTX_FRAME)
          {
            /* SCH may be assigned for a voice call.  In this case, don't
            ** read SCH data, otherwise debug printf may be printed every
            ** frame, causing confusion.
            */
            if( 
                (rxc_so.pri == CAI_SO_PPP_PKT_DATA_3G) ||
                (rxc_so.sec == CAI_SO_PPP_PKT_DATA_3G) ||
                (rxc_so.pri == CAI_SO_TDSO)            ||
                (rxc_so.sec == CAI_SO_TDSO)            ||
                (rxc_so.pri == CAI_SO_FULL_TDSO)       ||
                (rxc_so.sec == CAI_SO_FULL_TDSO)
              )
            {
              /*-------------------------------------------------------------
                Read the data from the decoder, even if SDU Erasure, as the
                LTU inner-CRC's may allow recovery of some of the LTU's.
              -------------------------------------------------------------*/
              if (rxc_dsm_arr_cnt == 0)
              {
#ifdef FEATURE_IS2000_REL_A /* 32X */
                if (rxc_sch.mux_pdu_type == DEC_MUX_PDU_TYPE_5)
                {     
                  if (rxc_sch.num_ltu == 0)
                  {
                    /*
                    ** No LTU case. Either 1X or 2X or turbo. Turbo is not
                    ** processed here.
                    */
                    if (frame_type == RXC_GOOD_FRAME)
                    {
                      /*
                      ** If the frame is bad, don't bother to read data,
                      ** because the MuxPDU header might be corrupted.
                      */
                      dec_read_mux_pdu5_sch_data(&rxc_sch,
                                                 &rxc_dsm_arr[0],
                                                 &rxc_dsm_arr_cnt,
                                                 0xFF,
                                                 &good_ltu);
                    }
                  }
                  else /* # of LTU is not 0 */
                  {
                    /* get LTU CRC status first */
                    ltu_crc = rxc_sch_status.conv_sch_status.ltu_crc;

                    dec_read_mux_pdu5_sch_data(&rxc_sch,
                                               &rxc_dsm_arr[0],
                                               &rxc_dsm_arr_cnt,
                                               ltu_crc,
                                               &good_ltu);
             
                    /* Update Statistics */
                    (void) parm_inc(CAI_FSCH0_LTU_ID, rxc_sch.num_ltu);
             
                    if (frame_type == RXC_GOOD_FRAME)
                    {
                      ltu_stats.good_sdus++;
                      (void) parm_inc(CAI_FSCH0_LTUOK_ID, rxc_sch.num_ltu);
             
                      /* Bad LTU within good frame, likely a bad BS */
                      (void) parm_xtra_inc(CAI_XTRA_SCH0_FWD_BAD_MUXPDU5_LTUS_ID,
                                    rxc_sch.num_ltu-good_ltu);
                    }
                    else if (frame_type == RXC_ERASURE_FRAME)
                    {
                      ltu_stats.sdu_erasure++;
                      ltu_stats.recovered_ltus += good_ltu;
                      (void) parm_inc(CAI_FSCH0_LTUOK_ID, good_ltu);
                      (void) parm_xtra_inc(CAI_XTRA_SCH0_FWD_SAVED_MUXPDU5_LTUS_ID,good_ltu);
                    } /* if erasure */
                  } /* if # of LTU is 0 */
                }
                else /* the MuxPDU 1, 2, and 3 case. Do it the old way */
#endif /* FEATURE_IS2000_REL_A */
                {
                  /* Copy SCH data into dsm_item array */
                  dec_read_sch_data( &rxc_sch,
                                     &rxc_dsm_arr[0],
                                     &rxc_dsm_arr_cnt);
                  /*-------------------------------------------------------------
                    Set the app fld field to TRUE, to indicate the frame is good.
                    For <= 2X, if the SDU is an erasure, this has no effect.  For
                    > 2X, then this may allow recovery of LTU's if the inner-
                    -CRC's check out.
              
                    account for single/double size PDU's.
                  -------------------------------------------------------------*/
                  if (rxc_sch.sch_rate <= DEC_SCH_RATE_2X)
                  {
                    if (frame_type == RXC_GOOD_FRAME)
                    {
                      ltu_flag = TRUE;
                    }
                    else
                    {
                      ltu_flag = FALSE;
                    }
                    rxc_dsm_arr[0]->app_field = ltu_flag;
                    if ((rxc_sch.mux_pdu_double == FALSE) &&
                        (rxc_sch.sch_rate == DEC_SCH_RATE_2X))
                    {
                      rxc_dsm_arr[1]->app_field = ltu_flag;
                    }
                  }
                  else /* > 2X */
                  {
                    /*-----------------------------------------------------------
                       quick way to figure out how many ltu's in the frame.
                       Then update the statistics counter.
                    -----------------------------------------------------------*/
                    ltu_cntr = 0x02 << (rxc_sch.sch_rate - DEC_SCH_RATE_4X);
                    (void) parm_inc(CAI_SCH0_FWD_LTU_ID, ltu_cntr);//lint !e734 Loss of precision (arg. no. 2) (16 bits to 8 bits)
#ifdef FEATURE_IS2000_LTU_CRCS
                   
                    /*-----------------------------------------------------------
                      Shift down the ltu crc bits into the least sig bits
                    -----------------------------------------------------------*/
                    ltu_crc = rxc_sch_status.conv_sch_status.ltu_crc;
                   
                    mask = 0x80 >> (8 - ltu_cntr);
                    /*-----------------------------------------------------------
                      Figure out how many of the LTU's were actually good.  If
                      erasure, then mark the bad LTU's.
                    -----------------------------------------------------------*/
#endif /* FEATURE_IS2000_LTU_CRCS */
                  
                    if (frame_type == RXC_GOOD_FRAME)
                    {
                      ltu_stats.good_sdus++;
                      (void) parm_inc(CAI_SCH0_FWD_LTUOK_ID, ltu_cntr);//lint !e734 Loss of precision (arg. no. 2) (16 bits to 8 bits)
                  
#ifdef FEATURE_IS2000_LTU_CRCS
                      ltu_cnt = 0;
                      for (index = 0; index < ltu_cntr; index++)
                      {
                        if ((mask & ltu_crc) == 0)
                        {
                          ltu_stats.incorrect_ltus++;
                          ltu_cnt++;
                        }
                        mask = mask >> 1;
                      }
                      (void) parm_xtra_inc(CAI_XTRA_SCH0_FWD_BAD_LTUS_ID,ltu_cnt);//lint !e734 Loss of precision (arg. no. 2) (16 bits to 8 bits)
#endif /* FEATURE_IS2000_LTU_CRCS */
                    
                      for (index = 0; index < rxc_dsm_arr_cnt; index++)
                      {
                        /*-------------------------------------------------------
                          Go ahead and mark all of these frames as good
                        -------------------------------------------------------*/
                        rxc_dsm_arr[index]->app_field = TRUE;
                      } /* for */
                    } /* full frame */
                    else if (frame_type == RXC_ERASURE_FRAME)
                    {
                      ltu_stats.sdu_erasure++;

#ifdef FEATURE_IS2000_LTU_CRCS
                      ltu_cnt = 0;
                      for (index = 0; index < ltu_cntr; index++)
                      {
                        /*-------------------------------------------------------
                          Check the bits indicating if the LTU CRC passes.  If it
                          does, mark the PDU as good/bad.  Account for single &
                          double size PDU's.
                        -------------------------------------------------------*/
                        if (((mask >> index) & ltu_crc) != 0)
                        {
                          ltu_flag = TRUE;
                          ltu_stats.recovered_ltus++;
                          ltu_cnt++;
                        }
                        else
                        {
                          ltu_flag = FALSE;
                        }
                     
                        if (rxc_sch.mux_pdu_double == TRUE)
                        {
                          rxc_dsm_arr[index]->app_field = ltu_flag;
                        }
                        else
                        {
                          rxc_dsm_arr[index * 2]->app_field       = ltu_flag;
                          rxc_dsm_arr[(index * 2) + 1]->app_field = ltu_flag;
                        }
                      }/* for */
                     
                      (void) parm_inc(CAI_SCH0_FWD_LTUOK_ID, ltu_cnt);//lint !e734 Loss of precision (arg. no. 2) (16 bits to 8 bits)
                      (void) parm_xtra_inc(CAI_XTRA_SCH0_FWD_SAVED_LTUS_ID, (uint8) ltu_cnt);
#else /* FEATURE_IS2000_LTU_CRCS */
                      for (index = 0; index < rxc_dsm_arr_cnt; index++)
                      {
                        /*-------------------------------------------------------
                          Go ahead and mark all of these frames as bad
                        -------------------------------------------------------*/
                        rxc_dsm_arr[index]->app_field = FALSE;
                      } /* for */
#endif /* FEATURE_IS2000_LTU_CRCS */
                    } /* if erasure */
                  } /* > 2X */
                }
              }
              else
              {
                ERR("RX task can't do SCH, arr_cnt=%d",
                      rxc_dsm_arr_cnt, 0, 0);
              }
            } /* pri or sec is 3G */
          } /* != DTX */
#endif /* !FEATURE_PLT */

          if (frame_type == RXC_GOOD_FRAME)
          {
#ifdef FEATURE_PLT
#error code not present
#endif

#ifdef FEATURE_IS2000_REL_A /* 32X */
            if (rxc_sch.mux_pdu_type == DEC_MUX_PDU_TYPE_5)
            {
              /* MO 0xf20 (MuxPDU 5) has its own set of counters */
              switch(rxc_sch.sch_rate)
              {
                case DEC_SCH_RATE_1X:
                  (void) parm_inc(CAI_FSCH0_1_ID, 1);
                  break;
                case DEC_SCH_RATE_2X:
                  (void) parm_inc(CAI_FSCH0_2_ID, 1);
                  break;
                case DEC_SCH_RATE_4X:
                  (void) parm_inc(CAI_FSCH0_3_ID, 1);
                  break;
                case DEC_SCH_RATE_8X:
                  (void) parm_inc(CAI_FSCH0_4_ID, 1);
                  break;
                case DEC_SCH_RATE_16X:
                  (void) parm_inc(CAI_FSCH0_5_ID, 1);
                  break;
                case DEC_SCH_RATE_32X:
                  (void) parm_inc(CAI_FSCH0_6_ID, 1);
                  break;
              }
            }
            else    /* The statistics for any MO but 0xF20 */
#endif /* FEATURE_IS2000_REL_A */
            {
            switch(rxc_sch.sch_rate)
            {
              case DEC_SCH_RATE_1X:
                  (void) parm_inc(CAI_SCH0_FWD_1X_ID, 1);
                break;
              case DEC_SCH_RATE_2X:
                  (void) parm_inc(CAI_SCH0_FWD_2X_ID, 1);
                break;
              case DEC_SCH_RATE_4X:
                  (void) parm_inc(CAI_SCH0_FWD_4X_ID, 1);
                break;
              case DEC_SCH_RATE_8X:
                  (void) parm_inc(CAI_SCH0_FWD_8X_ID, 1);
                break;
              case DEC_SCH_RATE_16X:
                  (void) parm_inc(CAI_SCH0_FWD_16X_ID, 1);
                  break;
                default:
                  MSG_ERROR("Bad SCH rate %d",rxc_sch.sch_rate,0,0);
                break;
            } /* switch */
            }

            rxc_sch_frame_status = RXC_GOOD_FRAME;

            ffpc_update_setpt (rxc_sch_frame_status, FFPC_SCH);

          } //full frame
          else if (frame_type == RXC_ERASURE_FRAME)
          {
            // Statistics for power measurement report
            if( rxc_pwr.sch_fer_rep[0] == TRUE )
            {
              rxc_pwr.sch_bad_frames[0]++;
            }
            (void) parm_xtra_inc(CAI_XTRA_SCH0_FWD_SDU_ERR_ID, 1);

            /* Increment erasure count to be printed every RXC_SCH_CRC_MAX_CNT frames */
            sch_crc_err++;

            rxc_sch_frame_status = RXC_ERASURE_FRAME;

            ffpc_update_setpt (rxc_sch_frame_status, FFPC_SCH);

          }
          else if (frame_type == RXC_DTX_FRAME)
          {
            (void) parm_xtra_inc(CAI_XTRA_SCH0_FWD_SDU_DTX_ID, 1);
            /* do nothing for DTX, &rxc_dsm_arr array does not get filled,
            ** we treat DTX like an erasure. */
            rxc_sch_frame_status = RXC_DTX_FRAME;
            sch_dtx_frame_cnt++;

#ifdef FEATURE_RDA_CORRECTION
            if (det_frame_type == RXC_ERASURE_FRAME)
            {
               MSG_HIGH("fixed ERASURE -> DTX frame", 0, 0, 0);
            }
#endif /* FEATURE_RDA_CORRECTION */
          }

          if (++sch_crc_err_dtx_frame_cnt >= RXC_SCH_CRC_MAX_CNT)
          {
            MSG_HIGH("Rcvd %d era and %d DTX in %d SCH frames", 
              sch_crc_err,sch_dtx_frame_cnt,sch_crc_err_dtx_frame_cnt);
            sch_crc_err_dtx_frame_cnt = 0;
            sch_crc_err = 0;
            sch_dtx_frame_cnt = 0;
          }

        } //code_type==CONV

#endif /* FEATURE_IS2000_F_SCH && !FEATURE_PLT */

#ifdef FEATURE_IS2000_REL_A
#if defined(FEATURE_IS95B_MAHHO) || defined(FEATURE_GPS_MODE)
        if (rxc_sch_frame_status == RXC_GOOD_FRAME)
        {
          rxc_flag_sch_eib( FALSE );
        }
        else
        {
          rxc_flag_sch_eib( TRUE );
        }
#else
        txc_sch_eib_flag = (rxc_sch_frame_status == RXC_ERASURE_FRAME);
#endif /* FEATURE_IS95B_MAHHO || FEATURE_GPS_MODE */
#endif /* FEATURE_IS2000_REL_A */


#ifdef FEATURE_PLT
#error code not present
#endif /* FEATURE_PLT */
      }
      else   /* SCCH Task Interrupt */
      {
#if  defined (FEATURE_IS95B_MDR)
        buf_ptr->buf.tc_buf.scc.num_sup = rxc_scch.num_sup;
        buf_ptr->buf.tc_buf.scc.scc_crc = 0;
        // SCCH, FCH must be included
        dec_read_scch_data((dec_rc_type)rxc_so.for_fch.rc,
                           buf_ptr->buf.tc_buf.scc.bits,
                           buf_ptr->buf.tc_buf.scc.scc_ser,
                           buf_ptr->buf.tc_buf.scc.scc_energy,
                           &buf_ptr->buf.tc_buf.scc.scc_crc,
                           rxc_scch.num_sup);
#ifdef FEATURE_PLT
#error code not present
#endif /* FEATURE_PLT */

#endif
      }
    }                            /* SCH/ SCCH Interrupt */

#ifdef FEATURE_IS2000_SCH
//
// RXC_SCH_SHUTDOWN
//
    if ( rxc_sch_shutdown_state == RXC_SCH_SHUTDOWN_NOW )
    {
      rxc_shutdown_sch();
      rxc_sch_shutdown_state = RXC_SCH_SHUTDOWN_INACTIVE;
      // Report SCH FER if needed to
      if ( rxc_sch_rep )
      {
        send_for_sch_fer_rpt( rxc_sch_id );
        // clear tot_frames and bad_frames
        rxc_pwr.sch_tot_frames[ rxc_sch_id ] = 0;
        rxc_pwr.sch_bad_frames[ rxc_sch_id ] = 0;

        rxc_sch_id = 0;
      }
    }

#endif /* FEATURE_IS2000_SCH */

    buf_ptr->state = RXC_TC_STATE;
      /* mark bits from Traffic Channel */
    qw_equ ( buf_ptr->buf.tc_buf.frame, rxc_dec_frame );

    /* Place item on rxc_dec_q */
    q_put( &rxc_dec_q, &buf_ptr->link );
    (void)rex_set_sigs ( &rx_tcb, RXC_INT_SIG );



  } /* buf_ptr != NULL */

  /* Write the power control data to the encoder in this context to
   * ensure that the next frame's power control bits contain information
   * about the quality of this frame. */
  txc_write_pcbit_data();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* service vocoder encoder for building tx packets */

#ifdef FEATURE_AGC_LOGGING
  /* perform AGC/power control logging */
  srch_mux_log_agc_pctl(); /* Log AGC and Power Control for Diag */
#endif

  /* Previous Description */
  /* In order to minimize the dec/enc isr loading, the decoder isr was
  ** optimized.  Writing the voice data in the vocoder decoder could have
  ** been done in the rxc_tc_isr but to avoid the additional voice delay,
  ** it was decided that it will be done in the RXC task.  Having a
  ** spectra-wide (around 17ms) voc decoder transaction window helps this
  ** purpose.  Hence now the data read in the vit decoder ISR is written
  ** into the voc decoder in the same frame as soon as the RXC task gets
  ** priority. The voc decoder transaction windows have been changed
  ** accordingly.
  */

  /* new description */
  /* A bug was unearthed in the vocoder EVRC code due to which the vocoder
  ** decoder window is no more a spectrawide window of 17ms but has 1 ms long
  ** "invalid windows" after every 4ms. The mobile can not write in these 1ms
  ** windows.  This forces the RXC to not write in the task context as the
  ** writing time are not deterministic in the task context. Hence we have
  ** moved rxc_voc_xchg back to the isr.
  */
  rxc_voc_xchg();      /* Writes voice bits to the vocoder decoder */


  /*-----------------------------------------------------------------------*/


  /* set frame signal for rxtx */
  (void)rex_set_sigs(&rxtx_tcb, RXTX_FRAME_SIG );

} /* rxc_tc_isr */

/*===========================================================================

FUNCTION RXC_LOG_TC_MSG

DESCRIPTION
  This function logs messages received on the Traffic Channel.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void rxc_log_tc_msg
(
  rxc_dsch_type dsch,           /* Physical channel carrying the message */
  cai_fwd_tc_ext_type *msg_ptr  /* Pointer to received message */
)
{
  log_code_type log_code;
#if !defined(FEATURE_DIAG_V7_LOG_IFACE)
  log_f_tc_msg_type  *log_ptr; /* Pointer to log buffer */
#else
  rxc_log_msg_type *log_ptr;   /* pointer to buffer to log msg in */
#endif /* !defined(FEATURE_DIAG_V7_LOG_IFACE) */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (dsch == RXC_FOR_FCH) 
  {
    log_code = LOG_F_TC_MSG_C;
  }
  else if (dsch == RXC_FOR_DCCH)
  {
    log_code = LOG_FOR_DCCH_MSG_C;
  }
  else
  {
    return; // vlechan Currently not supporting L3 message logging on PDCH
  }

#if !defined(FEATURE_DIAG_V7_LOG_IFACE)
  if (msg_ptr && msg_ptr->length != 0)
  {
    log_ptr = (log_f_tc_msg_type *) log_alloc (log_code,
                                               sizeof(log_hdr_type) +
                                               msg_ptr->length);
  }
  else
  {
    log_ptr = NULL;
  }

  if (log_ptr == NULL)
  {
    rxc_log_msg_drop++;  /* no buffer to log message in */
  }
  else
  {
    /* copy message into buffer */
    /* msg_ptr cannot be NULL because of check for log_ptr above. */
    (void)memcpy( (byte *) log_ptr->msg, (byte *) &(msg_ptr->length),
                  msg_ptr->length );//lint !e613 Possible use of null pointer (msg_ptr) in left argument to operator '->'

    log_commit((log_type *) log_ptr);
  }
#else
  /* get a buffer off of the rxc_log_msg_q to log the received message */

  if(( log_ptr = (rxc_log_msg_type *)q_get(&rxc_log_msg_q) ) == NULL )
  {
    rxc_log_msg_drop++;  /* no buffer to log message in */
  }
  else
  {
    /* set up log buffer */
    log_ptr->entry.tc_msg.hdr.len =  msg_ptr->length + sizeof(log_hdr_type);
    log_ptr->entry.tc_msg.hdr.code = log_code;
    (void) ts_get( (unsigned long *)(log_ptr->entry.tc_msg.hdr.ts ));

    /* copy message into buffer */
    /* msg_ptr cannot be NULL because of check for log_ptr above. */
    (void)memcpy((void *) log_ptr->entry.tc_msg.msg,
                 (void *) msg_ptr->length,
                 msg_ptr->length );

    log_put( &log_ptr->desc );  /* log message */
  }
#endif /* !defined(FEATURE_DIAG_V7_LOG_IFACE) */

  /* Event report: Message received */
  if (msg_ptr)
  {
  if ( dsch == RXC_FOR_FCH )
  {
    /* F-FCH */
    mclog_report_event_msg_rxed(CAI_F_FCH, msg_ptr->body);
  }
  else
  {
    /* F-DCCH */
    mclog_report_event_msg_rxed(CAI_F_DCCH, msg_ptr->body);
  }
  }
} /* rxc_log_tc_msg */

#ifdef FEATURE_PLT
#error code not present
#endif
/* <EJECT> */
/*===========================================================================

FUNCTION RXC_TC_PARSE

DESCRIPTION
  This function collects the signaling bits of the Traffic Channel.
  When it determines that an entire Traffic Channel message has been
  received and the message CRC checks, it will place the message on
  a queue for the Layer 2 task and signal the Layer 2 task.

DEPENDENCIES
  None.

RETURN VALUE
  Boolean indicator of whether message data was good.

SIDE EFFECTS
  None.

===========================================================================*/

boolean rxc_tc_parse
(
  qword  frame_num,
  byte   frame[],    /* Address of 1st byte of frame. */
  word   frame_len,   /* Number of bits in frame */
  rxc_dsch_type frame_chan  /* Channel frame received on */
)
{
  word frame_inx;          /* index into traffic channel frame */
  word num_bits;           /* number of bits to copy into message buffer */
  boolean good_data = TRUE;
  /* Boolean indicator of whether message data was good. */
  boolean good_crc;        /* variable for CRC check */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_IS2000_REL_A

  /* Check if the P_REV is in transition and if the action frame has arrived */
  if( rxc_tc_var.in_p_rev_transition )
  {
    /* If action time <= current time, switch P_REV_IN_USE */
    if( qw_cmp( rxc_tc_var.p_rev_action_frame, frame_num ) != 1 )
    {
      /* If yes, it is time to update the P_REV_IN_USE now. */
      rxc_tc_var.p_rev_in_use = rxc_tc_var.temp_p_rev_in_use;
    }
  }

#ifdef FEATURE_TC_EXTENDED_MSG_LENGTH
#error code not present
#endif /* FEATURE_TC_EXTENDED_MSG_LENGTH */
#endif /* FEATURE_IS2000_REL_A */
  {
    if ( frame[0] & 0x80 )
    {
      /* if you find a start of message bit start HUNT state regardless
          of current state (new message has been found ) */
      rxc_tc_var.state = TC_HUNT_STATE;
    }

    frame_inx = 1;  /* Skip over 1st bit (SOM) */
  }


  while( frame_inx <  frame_len )
  {
    switch( rxc_tc_var.state )
    {
      case TC_HUNT_STATE:
      {

#ifdef FEATURE_TC_EXTENDED_MSG_LENGTH
#error code not present
#else
        if( frame[0] & 0x80 )
#endif /* FEATURE_TC_EXTENDED_MSG_LENGTH */
        {
          /* Start of message bit set */
          rxc_tc_var.msg_inx = 0;

#ifdef FEATURE_TC_EXTENDED_MSG_LENGTH
#error code not present
#endif /* FEATURE_TC_EXTENDED_MSG_LENGTH */
          {
              rxc_tc_var.msg_len =
                  (word)(b_unpackw( frame, frame_inx, 8 ) * 8);
          }

          // For release A, additional checking is reqired because we might not
          // support the full 32K byte message length yet. Therefore, the raw msg
          // length ( including CRC and MSG_LEN fields ) minus 16 bits MSG_LEN
          // fields (worst case scenario) and 16 bits CRC should fit in the
          // buffer with as size of CAI_FWD_TC_EXT_SIZE bytes.
          if ( (rxc_tc_var.msg_len > 32)
#ifdef FEATURE_TC_EXTENDED_MSG_LENGTH
#error code not present
#endif /* FEATURE_TC_EXTENDED_MSG_LENGTH */
             )
          {
            if( (rxc_tc_var.msg_ptr == NULL ) &&
                ( (rxc_tc_var.msg_ptr =
                     (rxtx_rx_msg_type *)q_get( &rxtx_rx_free_q) ) == NULL ) )
            {

              /* out of buffers -- wait for next start of message */
              ERR( "0 bufs on rxtx_rx_free_q", 0, 0, 0 );

              rxc_tc_var.state = TC_FLUSH_STATE;

            }
            else
            {
#ifdef FEATURE_TC_EXTENDED_MSG_LENGTH
#error code not present
#endif /* FEATURE_TC_EXTENDED_MSG_LENGTH */
              {
                b_copy ( frame, frame_inx,
                         (void *)&(rxc_tc_var.msg_ptr->msg.tc.length),
                         rxc_tc_var.msg_inx, 8 );    /* copy length byte */

                rxc_tc_var.msg_inx += 8;
                  /* increment index into message buffer */
                frame_inx += 8;
                 /* increment index into frame */

              }

              rxc_tc_var.state = TC_MSG_STATE; /* go to message state */
            }
          }
          else
          { /* message length = 0 */
            rxc_tc_var.state = TC_FLUSH_STATE;
            good_data = FALSE;
          }

        }

        else
        { /* Start of message bit not found */
          rxc_tc_var.state = TC_FLUSH_STATE;
          good_data = FALSE;
        }

        break;
      }


      case TC_MSG_STATE:
      {
        num_bits = MIN ( (rxc_tc_var.msg_len - rxc_tc_var.msg_inx),
                         (frame_len - frame_inx) );
          /* calculate number of bits to copy to message buffer */
#ifdef FEATURE_TC_EXTENDED_MSG_LENGTH
#error code not present
#endif /* FEATURE_TC_EXTENDED_MSG_LENGTH */
        {
            b_copy ( frame, frame_inx, &rxc_tc_var.msg_ptr->msg.tc.length,
                       rxc_tc_var.msg_inx, num_bits );
        }

        frame_inx += num_bits;
          /* increment index into frame */

        rxc_tc_var.msg_inx += num_bits;
          /* increment index into message buffer */

        if ( rxc_tc_var.msg_inx >= rxc_tc_var.msg_len )
        {
          /* entire message collected - check CRC */

          /* If rxc_ignore_good_frame is set to TRUE, that means
           * we're about to tune to the TF/CF or we are already on the
           * TF/CF.  If that's the case, we should ignore any incoming
           * signalling message by failing the CRC.  This is to avoid the
           * race condition when during a CFS, the mobile might receive a
           * valid traffic channel message just before it tunes to
           * the CF and start processing the message after it has actually
           * tuned to the CF.  This will break the current software design
           * because the design assumes that the mobile is always on the
           * SF when processing TC messages.  By ignoring the message, the
           * base station is then forced to re-transmit, which might or
           * might not have an impact on performance
           */
#ifdef FEATURE_TC_EXTENDED_MSG_LENGTH
#error code not present
#endif /* FEATURE_TC_EXTENDED_MSG_LENGTH */
          {
              good_crc = (( crc_16_calc( &rxc_tc_var.msg_ptr->msg.tc.length,
                               rxc_tc_var.msg_len ) == CRC_16_OK )
#if defined(FEATURE_IS95B_MAHHO) || defined (FEATURE_GPS_MODE)
                 && ( ! rxc_ignore_good_frame )
#endif /* FEATURE_IS95B_MAHHO || FEATURE_GPS_MODE */
                 );
          }

          if ( good_crc )
          {

            /* Set up message to be sent to Main Control task */
            if(rxc_tc_var.msg_ptr != NULL )
            {
#ifdef FEATURE_TC_EXTENDED_MSG_LENGTH
#error code not present
#endif /* FEATURE_TC_EXTENDED_MSG_LENGTH */
              {
                rxc_tc_var.msg_ptr->length =
                  rxc_tc_var.msg_len - CAI_TC_CRC_SIZE - CAI_TC_LGTH_SIZE;
              }

              rxc_tc_var.msg_ptr->return_q_ptr = &rxtx_rx_free_q;
              rxc_tc_var.msg_ptr->chn = RXC_CDMA_TC;
              qw_equ( rxc_tc_var.msg_ptr->msg.tc.frame_num, frame_num );
  
              /* log message */
              rxc_log_tc_msg( frame_chan, &rxc_tc_var.msg_ptr->msg.tc );
  
              /* Put message on queue for Layer 2 task */
              q_put( &rxtx_rx_q, &rxc_tc_var.msg_ptr->link );
            }

            /* set signal for Main Control task */
            (void)rex_set_sigs(&rxtx_tcb, RXTX_RX_Q_SIG );
          
            rxc_tc_var.msg_ptr = NULL;
              /* indicate no working buffer */

          }
          else
          {
            MSG_HIGH( "Bad TC MSG crc", 0, 0, 0 );
            good_data = FALSE;

            if(rxc_tc_var.msg_ptr != NULL )
            {
              /* Put message on free queue */
              q_put( &rxtx_rx_free_q, &rxc_tc_var.msg_ptr->link );

              /* indicate no working buffer */
              rxc_tc_var.msg_ptr = NULL;
            }
          }

          rxc_tc_var.state = TC_FLUSH_STATE;

        }

        break;
      }

      case TC_FLUSH_STATE:
      {
        frame_inx = frame_len; /* Indicate all bits used up. */

        rxc_tc_var.state = TC_HUNT_STATE;

        break;
      }

      default:
      {
        ERR_FATAL("Bad TC parse subst",rxc_tc_var.state,0,0);
      }
    } /* switch */
  } /* while */

  return (good_data);
} /* rxc_tc_parse */

/* <EJECT> */
/*===========================================================================

FUNCTION PWR_MSR_CHECK

DESCRIPTION
  This procedure processes the updated rxc_pwr structure to determine if a
  power measurement report should be sent to MC.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  A power measurement report may be sent to MC.

===========================================================================*/

void pwr_msr_check( void )
{
  /* Process Power measurement reports */
  if (rxc_pwr.num_delay_frames > 0)
  {
    rxc_pwr.num_delay_frames--;
  }
  else
  {
#if defined(FEATURE_IS95B_MAHHO) || defined(FEATURE_GPS_MODE)
    /* If Return-If-Fail HHO or CFS is in progress, suspend counting TOT_FRAMES
     * per section 6.6.6.2.8.2 of IS-95B
     */
    if ( rxc_rifhho_cfs_gps_in_progress )
    {
      return;
    }
#endif /* FEATURE_IS95B_MAHHO || FEATURE_GPS_MODE */

#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
    /* For VP2 only, num_frames applies to FCH and dcch_num_frames 
    ** applies to DCCH.  This is done for both "DCCH only" and "FCH & DCCH"
    ** scenarios.
    */
    if (rxc_so.for_fch.included )
    {
      rxc_pwr.num_frames++;
    }
    if (rxc_so.for_dcch.included )
    {
      rxc_pwr.dcch_num_frames++;
    }
#else /* else of FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */
    rxc_pwr.num_frames++;

#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */

    if ((rxc_pwr.threshold_on) &&
        (
         (rxc_pwr.num_bad_frames >= rxc_pwr.pwr_thresh)
#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
         || (rxc_pwr.dcch_num_bad_frames >= rxc_pwr.pwr_thresh)
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */
        )
       )
    {
      /* Send threshold power measurment report to MC */
      send_pwr_rpt();
    }
    else if (
             (rxc_pwr.num_frames == rxc_pwr.pwr_frames) 
#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
             || (rxc_pwr.dcch_num_frames == rxc_pwr.pwr_frames)
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */
            )
    {
      if (rxc_pwr.periodic_on)
      {
      /* Send periodic power measurment report to MC */
        send_pwr_rpt();
      }
      else
      {
        /* Reset counts but don't send a report */
        rxc_pwr.num_frames     = 0;
        rxc_pwr.num_bad_frames = 0;
#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
        rxc_pwr.dcch_num_frames     = 0;
        rxc_pwr.dcch_num_bad_frames = 0;
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */
      }
    }
  }
} /* pwr_msr_check */

/* <EJECT> */
/*===========================================================================

FUNCTION RXC_START_TX

DESCRIPTION
  This procedure restarts the transmitter by sending the RXC_TX_ON_R report
  to MC.

DEPENDENCIES
  rxc_sent_bad_rpt should be TRUE when this procedure is called.

RETURN VALUE
  None.

SIDE EFFECTS
  rxc_sent_bad_rpt is set to FALSE after the RXC_TX_ON_R report is sent


===========================================================================*/

void rxc_start_tx( void )
{
  mccrx_rpt_type *rpt_ptr;
    /* pointer to report to be sent to the Main Control task */
  MSG_HIGH("RXC asking to turn TX on",0,0,0);

  /* send message to mc to restart transmissions */
  if ( (rpt_ptr = (mccrx_rpt_type *)q_get(&mcc_rx_free_q) ) != NULL )
  {
    rxc_sent_bad_rpt = FALSE;
    rpt_ptr->hdr.rpt = RXC_TX_ON_R;
    rxc_mc_rpt(rpt_ptr );
  }
  else              /* will retry when next good frame arrives */
  {
    ERR( "0 bufs on mcc_rx_free_q", 0, 0, 0 );
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION FADE_CHECK_GOOD_FRAME_HHO

DESCRIPTION
  This procedure is called upon the receipt of the first two good frames
  during HHO.

  If this is the first good frame after a successful handoff
  (rxc_sent_bad_rpt = TRUE), RXC shall command MC to restart the transmitter.
  In addition, if this is a successful Return-If-Fail HHO, the fade timer
  shall be reset.

  If this is the second consecutive good frame received during handoff,
  RXC shall command MC to restart forward power control and mark the end
  of the HHO.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void fade_check_good_frame_hho( void )
{
  mccrx_rpt_type *rpt_ptr;
    /* pointer to report to be sent to the Main Control task */

  if ( rxc_good_frames < cai_tmo.n11m )
  {
    rxc_t5m_bad_frames++; /* don't reset fade timer yet */
  }
  else
  {
    MSG_HIGH( "Got good fr", 0, 0, 0 );

    /* Event report: Counter N11m threshold reached */
    mclog_report_event_counter(CAI_COUNTER_N11M);

    if( rxc_sent_bad_rpt ) /* Check if RXC_TX_ON_R has already been sent */
    {
      /* rxc_sent_bad_rpt is set to FALSE when we send RXC_TX_ON_R, so
       * by checking this flag, we avoid sending RXC_TX_ON_R more than once.
       */

#ifdef FEATURE_IS95B_MAHHO
      /* rxc_rifhho_cfs_gps_in_progress is set to FALSE when we return
       * from a failed hard handoff (if the GHDM allowed return-on-failure),
       * so being here while the flag is still TRUE indicates a successful
       * hard handoff (rifhho).
       */
      if ( rxc_rifhho_cfs_gps_in_progress )
      {
        /* If a Return-If-Fail HHO is successful, reset the fade timer */
        rxc_t5m_bad_frames = 0;
        rxc_rifhho_cfs_gps_in_progress = FALSE;
      }

      /* If we return quickly from a rifhho, we send RXC_TX_ON_R immediately,
       * so rxc_sent_bad_rpt is FALSE even when the first good frame is
       * received after returning. In the case where we take a long time to
       * return, we must wait for 2 consecutive good frames, not just one.
       * So the only time we enable the transmitter here is after a successful
       * handoff.
       */
      if (!rxc_delay_after_hho_failure)
      {
        /* We have successfully completed a handoff. Enable the transmitter. */
        rxc_start_tx();
      }
#else
      rxc_start_tx();
#endif /* FEATURE_IS95B_MAHHO */
    }

    if (rxc_good_frames >= cai_tmo.n3m)/* At least 2 consecutive good frames */
    {
      MSG_HIGH( "Got 2 consec good fr", 0, 0, 0 );
      if( !rxc_sent_pwron_rpt )
      {
        /* send message to mc to restart forward power control */
        if ((rpt_ptr = (mccrx_rpt_type *)q_get(&mcc_rx_free_q)) != NULL)
        {
          rxc_sent_pwron_rpt = TRUE;
          rpt_ptr->hdr.rpt = RXC_PWR_ON_R;
          rxc_mc_rpt(rpt_ptr );
        }
        else
        {
          /* We will not re-try to start forward power control reporting. */
          ERR( "0 bufs on mcc_rx_free_q", 0, 0, 0 );
        }
      }

#ifdef FEATURE_IS95B_MAHHO
      if (rxc_delay_after_hho_failure)
      {
        rxc_start_tx();
        rxc_delay_after_hho_failure = FALSE;
      }
#endif /* FEATURE_IS95B_MAHHO */
      rxc_fade = FALSE;  /* fade processing ended */
      rxc_hard_handoff_in_progress = FALSE; /* done with HHO processing */
    }
  }
} /* fade_check_good_frame_hho */

/* <EJECT> */
/*===========================================================================

FUNCTION FADE_CHECK_GOOD_FRAME_NON_HHO

DESCRIPTION
  This procedure is called upon the receipt of a good frame during fade
  processing.  If this is the second consecutive good frame, RXC shall
  restart the transmitter, reset the fade timer, and mark the end of
  fade processing

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void fade_check_good_frame_non_hho( void )
{
  if ( rxc_good_frames < cai_tmo.n3m )
  {
    rxc_t5m_bad_frames++; /* don't reset fade timer yet */
  }
  else /* 2 good frames after bad frames */
  {
    rxc_t5m_bad_frames = 0; /* restart fade timer */
    rxc_fade = FALSE;  /* fade processing ended */
    /* send message to mc to restart transmissions */
    if( rxc_sent_bad_rpt )
    {
      rxc_start_tx();

      /* Event report: Counter N3m threshold reached */
      mclog_report_event_counter(CAI_COUNTER_N3M);
    }
  }
} /* fade_check_good_frame_non_hho */

/* <EJECT> */
/*===========================================================================

FUNCTION FADE_CHECK_GOOD_FRAME

DESCRIPTION
  This procedure is called upon the receipt of a good frame.  It determines
  if a HHO is in progress and calls the appropriate procedure.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void fade_check_good_frame( void )
{
  /* got a good frame */
  rxc_good_frames++;
  rxc_n2m_bad_frames = 0;

#ifdef FEATURE_IS2000_REL_D
#error code not present
#endif /* FEATURE_IS2000_REL_D */

  if ( rxc_hard_handoff_in_progress )
  {
    fade_check_good_frame_hho();
  }
  else /* not in hard handoff */
  {
    fade_check_good_frame_non_hho();
  }
} /* fade_check_good_frame */

/* <EJECT> */
/*===========================================================================

FUNCTION FADE_CHECK_BAD_FRAME

DESCRIPTION
  This procedure is called upon the receipt of a bad frame during fade
  processing.  If this is the N2M-th (12th) consecutive bad frame, RXC
  shall shut down the transmitter

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void fade_check_bad_frame( void )
{
  mccrx_rpt_type *rpt_ptr;
    /* pointer to report to be sent to the Main Control task */

#ifdef FEATURE_IS2000_REL_D
#error code not present
#endif /* FEATURE_IS2000_REL_D */

  if( ++rxc_n2m_bad_frames >= cai_tmo.n2m )
  {
    /* Event report: Counter N2m threshold reached */
    mclog_report_event_counter(CAI_COUNTER_N2M);

    /* generate transmit off report to Main Control task */
    if( !rxc_sent_bad_rpt )
    {
      if ( (rpt_ptr = (mccrx_rpt_type *)q_get(&mcc_rx_free_q) ) != NULL )
      {
        rxc_sent_bad_rpt = TRUE;
        rxc_sent_pwron_rpt = FALSE;
        rpt_ptr->hdr.rpt = RXC_TX_OFF_R;
        rxc_mc_rpt(rpt_ptr );
      }
      else
      {
        ERR( "0 bufs on mcc_rx_free_q", 0, 0, 0 );
      }
    }
  }

  if ( ++rxc_t5m_bad_frames >= RXC_TC_TMO_FRAMES )
  {
    /* Event report: Timer T5m expired */
    mclog_report_event_timer(CAI_TIMER_T5M);

    /* generate Traffic Channel good frame time-out report*/
    if ( (rpt_ptr = (mccrx_rpt_type *)q_get(&mcc_rx_free_q) ) != NULL )
    {
      /* First Free all the TC resources, since in this case we won't
         get the RELEASE_F command from MC */
      rpt_ptr->hdr.rpt = RXC_TC_TMO_R;
      rxc_mc_rpt(rpt_ptr );
    }
    else
    {
      ERR( "0 bufs on mcc_rx_free_q", 0, 0, 0 );
    }
  }
} /* fade_check_bad_frame */



#ifdef FEATURE_IS2000_REL_A_SVD
/*===========================================================================

FUNCTION FADE_CHECK_FCH

DESCRIPTION
  This procedure processes the frame rate decision and updates the fade
  good and bad frame counts accordingly to determine if a fade report
  should be sent to MC.  This procedure is called when the primary power
  control channel is FCH.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  A fade report may be sent to MC.

===========================================================================*/
void fade_check_fch
(
  byte mux
    /* category of received frame */
)
{
  boolean fch_frame_check;

#ifdef FEATURE_IS2000_REL_D
#error code not present
#endif /* FEATURE_IS2000_REL_D */

  fch_frame_check = ( (mux == CAI_MUX1_FOR_10_ID ) || 
                      (mux == CAI_MUX1_FOR_9_ID) ||
                      (mux == CAI_MUX2_FOR_26_ID) );

  if(fch_frame_check) 
  {
    /* Bad frame received, broke consecutive good frame count */
    rxc_good_frames = 0;

#if defined(FEATURE_IS95B_MAHHO) || defined(FEATURE_GPS_MODE)
    /*
    ** If Return-If-Fail HHO or CFS is in progress, suspend the fade timer per
    ** section 6.6.6.2.8.2 of IS-95B
    */
    if ( ! rxc_rifhho_cfs_gps_in_progress )
#endif /* FEATURE_IS95B_MAHHO || FEATURE_GPS_MODE */
    {
      fade_check_bad_frame();
    }
  }
  else
  {
#if defined(FEATURE_IS95B_MAHHO) || defined(FEATURE_GPS_MODE)
    /* Ignore good frames during CFS, at the beginning of a RIF HHO prior to
     * SRCH checking the failure thresholds, and during the period after
     * a RIF HHO times out, before we return to the serving frequency.
     */
    if ( ! rxc_ignore_good_frame )
#endif /* FEATURE_IS95B_MAHHO || FEATURE_GPS_MODE */
    {
      /* good frame */
      fade_check_good_frame();
    }
  }
} /* fade_check_fch */

/*===========================================================================

FUNCTION FADE_CHECK_DCCH

DESCRIPTION
  This procedure processes the frame rate decision and updates the fade
  good and bad frame counts accordingly to determine if a fade report
  should be sent to MC.  This procedure is called when the primary power
  control channel is DCCH.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  A fade report may be sent to MC.

===========================================================================*/
void fade_check_dcch
(
#ifdef FEATURE_DCCH_FRAME_QUALITY_SUPERVISION
  rxc_dcch_quality_type  dcch_chan_quality
#else
  byte mux
#endif /* End of FEATURE_DCCH_FRAME_QUALITY_SUPERVISION */
)
{

#ifdef FEATURE_IS2000_REL_D
#error code not present
#endif /* FEATURE_IS2000_REL_D */

#ifdef FEATURE_DCCH_FRAME_QUALITY_SUPERVISION
  if (dcch_chan_quality == RXC_POOR_CHANNEL)
#else
  if( mux == CAI_MUX1_FOR_DCCH_10_ID) || (mux == CAI_MUX2_FOR_DCCH_26_ID))
#endif /* End of FEATURE_DCCH_FRAME_QUALITY_SUPERVISION */
  {
    /* Bad frame received, broke consecutive good frame count */
    rxc_good_frames = 0;

#if defined(FEATURE_IS95B_MAHHO) || defined(FEATURE_GPS_MODE)
    /*
    ** If Return-If-Fail HHO or CFS is in progress, suspend the fade timer per
    ** section 6.6.6.2.8.2 of IS-95B
    */
    if ( ! rxc_rifhho_cfs_gps_in_progress )
#endif /* FEATURE_IS95B_MAHHO || FEATURE_GPS_MODE */
    {
      fade_check_bad_frame();
    }
  }
  else
  {
#if defined(FEATURE_IS95B_MAHHO) || defined(FEATURE_GPS_MODE)
    /* Ignore good frames during CFS, at the beginning of a RIF HHO prior to
     * SRCH checking the failure thresholds, and during the period after
     * a RIF HHO times out, before we return to the serving frequency.
     */
    if ( ! rxc_ignore_good_frame )
#endif /* FEATURE_IS95B_MAHHO || FEATURE_GPS_MODE */
    {
      /* good frame */
      fade_check_good_frame();
    }
  }
} /* fade_check_dcch */
#else /* else of FEATURE_IS2000_REL_A_SVD */
/*===========================================================================

FUNCTION FADE_CHECK

DESCRIPTION
  This procedure processes the frame rate decision and updates the fade
  good and bad frame counts accordingly to determine if a fade report
  should be sent to MC.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  A fade report may be sent to MC.

===========================================================================*/
void fade_check
(
  byte mux
    /* category of received frame */
#ifdef FEATURE_DCCH_FRAME_QUALITY_SUPERVISION  
       ,rxc_dcch_quality_type  dcch_chan_quality
#endif /* End of FEATURE_DCCH_FRAME_QUALITY_SUPERVISION  */
)
{
#ifdef FEATURE_IS2000_REL_D
#error code not present
#endif /* FEATURE_IS2000_REL_D */

#ifdef FEATURE_DCCH_FRAME_QUALITY_SUPERVISION
  boolean fch_frame_check;
  fch_frame_check = ( (mux == CAI_MUX1_FOR_10_ID ) || 
                      (mux == CAI_MUX1_FOR_9_ID) ||
                      (mux == CAI_MUX2_FOR_26_ID) );

  if( (rxc_so.for_fch.included && fch_frame_check) 
      #ifdef FEATURE_IS2000_P2
      || (rxc_so.for_dcch.included &&
          dcch_chan_quality == RXC_POOR_CHANNEL)
      #endif // FEATURE_IS2000_P2
      )
#else
  if( (mux == CAI_MUX1_FOR_10_ID ) || (mux == CAI_MUX1_FOR_9_ID) ||
      (mux == CAI_MUX2_FOR_26_ID)
      #ifdef FEATURE_IS2000_P2
      || (mux == CAI_MUX1_FOR_DCCH_10_ID) || (mux == CAI_MUX2_FOR_DCCH_26_ID)
      #endif // FEATURE_IS2000_P2
      )
#endif /* End of FEATURE_DCCH_FRAME_QUALITY_SUPERVISION */
  {
    /* Bad frame received, broke consecutive good frame count */
    rxc_good_frames = 0;

#if defined(FEATURE_IS95B_MAHHO) || defined(FEATURE_GPS_MODE)
    /*
    ** If Return-If-Fail HHO or CFS is in progress, suspend the fade timer per
    ** section 6.6.6.2.8.2 of IS-95B
    */
    if ( ! rxc_rifhho_cfs_gps_in_progress )
#endif /* FEATURE_IS95B_MAHHO || FEATURE_GPS_MODE */
    {
      fade_check_bad_frame();
    }
  }
  else
  {
#if defined(FEATURE_IS95B_MAHHO) || defined(FEATURE_GPS_MODE)
    /* Ignore good frames during CFS, at the beginning of a RIF HHO prior to
     * SRCH checking the failure thresholds, and during the period after
     * a RIF HHO times out, before we return to the serving frequency.
     */
    if ( ! rxc_ignore_good_frame )
#endif /* FEATURE_IS95B_MAHHO || FEATURE_GPS_MODE */
    {
      /* good frame */
      fade_check_good_frame();
    }
  }
} /* fade_check */
#endif /* FEATURE_IS2000_REL_A_SVD */



#ifdef FEATURE_IS2000_REL_A_CC
/*===========================================================================

FUNCTION RXC_ATMR_INIT

DESCRIPTION
  This funtion initializes the action timer resource for the traffic 
  channel state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static boolean 
rxc_atmr_init (void)
{
  boolean success = FALSE;

  if (rxc_atmr_handle != ATMR_INVALID_HANDLE) {
    /* Free action timer resource. */
    atmr_set (rxc_atmr_handle, NULL, ATMR_5MS_FRAME(NONE), 
      (atmr_pcg_offset_enum_type) 0, (unsigned long) 0);
  }

  rxc_atmr_handle = atmr_get (ATMR_TIME_2);

  if (rxc_atmr_handle == ATMR_INVALID_HANDLE) {
    MSG_HIGH("rxc can't get atmr hndl", 0, 0, 0);
  } else {
    success = TRUE;
  }

  return success;
}
#endif /* FEATURE_IS2000_REL_A_CC */


/*===========================================================================

FUNCTION RXC_TCI_INIT

DESCRIPTION
  This funtion initializes the Traffic Channel initialization state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

#ifdef FEATURE_IS2000_REL_A_CC
rxc_state_type rxc_tci_init
(
  rxc_state_type curr_state,  /* Current state of RXC state machine */
  rxc_cmd_msg_type *cmd_ptr
)
#else
void rxc_tci_init
(
  word curr_state      /* Current state of RXC state machine */
)
#endif /* FEATURE_IS2000_REL_A_CC */
{
#ifdef FEATURE_IS2000_REL_A_SVD
  word  sr_index;
#endif /* FEATURE_IS2000_REL_A_SVD */
#ifdef FEATURE_IS2000_REL_A_CC
  rxc_state_type next_state = curr_state;
#ifndef FEATURE_IS2000_REL_A_SVD
  cai_radio_config_type cur_rc;
#endif /* !FEATURE_IS2000_REL_A_SVD */
  dword isave;
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_IS2000_REL_A_CC
  ASSERT(cmd_ptr);
#endif

  switch (curr_state)
  {
    case RXC_PC_STATE:
#ifdef FEATURE_IS2000_REL_A_CC
    case RXC_FCCCH_STATE:
#endif
#ifdef FEATURE_IS2000_REL_D
#error code not present
#endif /* FEATURE_IS2000_REL_D */
    {
#ifdef FEATURE_IS2000_REL_A_CC
#ifndef FEATURE_IS2000_REL_A_SVD
      // Current FCH/DCCH RC, initialize to something to silent compiler
      cur_rc = cmd_ptr->tci.for_fch.rc;
#endif /* !FEATURE_IS2000_REL_A_SVD */
  
      rxc_set_frame_offset(cmd_ptr->tci.frame_offset);

      INTLOCK_SAV( isave );
  
      /* free decoder data buffer */
      rxc_clear_decoder_queue ();
  
#ifdef FEATURE_PLT
#error code not present
#endif /* FEATURE_PLT */
#ifdef FEATURE_FACTORY_TESTMODE
      rxc_so.enable_skip_tci = cmd_ptr->tci.enable_skip_tci;
      rxc_set_fch_loopback(FALSE);
#ifdef FEATURE_IS2000_P2
      /* Support for FEATURE_FTM_DCCH */
      rxc_set_dcch_loopback(FALSE);
#endif
      rxc_set_sch_loopback(FALSE);
#endif /* FEATURE_FACTORY_TESTMODE */
    
#ifdef FEATURE_IS2000_REL_A
      rxc_tc_var.p_rev_in_use = cmd_ptr->tci.p_rev_in_use;
  
      // Initialize the P_REV transistion flag to FALSE.
      rxc_tc_var.in_p_rev_transition = FALSE;
#endif
    
#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
     
      (void) rxc_configure_ffpc_channel(cmd_ptr);

#else
#ifdef FEATURE_IS2000_P2   /* DCCH/FCH */
      /* DCCH configuration */
      if ((rxc_so.for_dcch.included = cmd_ptr->tci.for_dcch.included) != FALSE)
      {
        cur_rc = rxc_so.for_dcch.rc = cmd_ptr->tci.for_dcch.rc;
        rxc_so.high_rc = TRUE;
      }
  
      /* FCH configuration */
      if ( (rxc_so.for_fch.included = cmd_ptr->tci.for_fch.included) != FALSE )
#endif /* FEATURE_IS2000_P2 */
      {
        // cur_rc already holds fch rc
        rxc_so.for_fch.rc = cmd_ptr->tci.for_fch.rc;
        rxc_so.high_rc = rxc_so.for_fch.rc > CAI_RC_2;
      }

      rxc_config_demod( cur_rc );
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */
  
      /* Initialize SCH0 RC */
      rxc_sch.rc = (dec_rc_type)DEINT_RC_INVALID;
  
      /* For SRCH logging */
#ifdef FEATURE_IS2000_P2
      srch_mux_update_rc (
        rxc_so.for_fch.included?
        (deint_rc_type)rxc_so.for_fch.rc:DEINT_RC_INVALID, 
        (deint_rc_type)rxc_sch.rc,
        DEINT_RC_INVALID,
        rxc_so.for_dcch.included?
        (deint_rc_type)rxc_so.for_dcch.rc:DEINT_RC_INVALID);
#else
      srch_mux_update_rc ((deint_rc_type)rxc_so.for_fch.rc,
                          (deint_rc_type)rxc_sch.rc,
                          DEINT_RC_INVALID,
                          DEINT_RC_INVALID);
#endif /* FEATURE_IS2000_P2 */
#endif /* FEATURE_IS2000_REL_A_CC */
  
      /* register to MC all RXC action time handlers */
      rxc_register_action_handlers_notifier_interrupt_func();

#ifdef FEATURE_IS2000_REL_D
#error code not present
#endif /* FEATURE_IS2000_REL_D */
      {
        /* send command to interrupt handler to go to TC mode */
        rxc_isr_cmd_buf_ptr = (rxc_isr_cmd_type *) q_get (&rxc_isr_free_q);
        if (rxc_isr_cmd_buf_ptr != NULL)
        {
          rxc_isr_cmd_buf_ptr->cmd = RXC_TC_F;
          rxc_isr_cmd(rxc_isr_cmd_buf_ptr);
        }
        else
        {
          ERR( "0 bufs on isr_free_q", 0, 0, 0 );
        }
      }
  
      /* Default service options to none */
      rxc_so.pri = CAI_SO_NULL;
      rxc_so.sec = CAI_SO_NULL;
#ifdef FEATURE_IS2000_REL_A_SVD
      /* reset the service option reference */
      for (sr_index = 0; sr_index < RXC_SR_ID_MAX; sr_index++)
      {
        rxc_srid_ref[sr_index].so = CAI_SO_NULL;
        rxc_srid_ref[sr_index].traffic_type = CAI_NUL_TRAF;
      }
  
#endif /* FEATURE_IS2000_REL_A_SVD */

      /* Indicate that vocoder is not initialized */
      rxc_voc_init_done   = FALSE;
      rxc_voc_config_done = FALSE;

#if !defined(FEATURE_DIAG_V7_LOG_IFACE)
      /* Re-initialize Markov log buffer */
      rxc_log_mar_buf.count = 0;
      rxc_log_mar_buf.hdr.len = FPOS(log_mar_type, entry);//lint !e734 FPOS

#else
      /* put markov log buffer back on queue if one is leftover from
         last call */
      if( rxc_mar_buf_ptr != NULL )
      {
        /* put item back on free queue */
        q_put( &rxc_log_mar_q, &rxc_mar_buf_ptr->desc.link );
        rxc_mar_buf_ptr = NULL;
      }
#endif

      rxc_lost_voc_pkts = 0;  /* reset number of lost logged vocoder pkts */

#ifdef FEATURE_RDA_CORRECTION
      rxcrda_dcch_dtx_fix_counter = -1;
#endif

      rxc_good_frames = 0;
      rxc_t5m_bad_frames = 0;
      rxc_n2m_bad_frames = 0;

      /* initialize to NOT treat blank and burst to erasures */
      rxc_erase = FALSE;

      /* initialize ser totals */
      rxc_ser_reset();

      /* Turn power control reporting off until told otherwise */
      rxc_pwr.threshold_on = FALSE;
      rxc_pwr.periodic_on = FALSE;

      /* Retry control flag when mcc_rx_free_q is full */
      rxc_sent_valid_rpt = FALSE;

#ifdef FEATURE_IS2000_REL_A_CC
      /* Initialize the action timer */
      if (rxc_atmr_init () == TRUE) {
        rxc_atmr_set (rxc_atmr_isr, RXC_ATMR_5MS_FRAME_2,0);
      } else {
        MSG_LOW("Action timer init failed", 0, 0, 0);
      }

      memset (&ltu_stats, 0, sizeof(ltu_stats_type));

      /* program the delay of TX generated MASK_DATA to RX PCG timing */
      MUXMSM_AGC_CTL_MASK_DELAY( cmd_ptr->tci.rev_pwr_cntl_delay );

      ffpc_tci_cmd (cmd_ptr);
#ifdef FEATURE_DCCH_FRAME_QUALITY_SUPERVISION  
      if (cmd_ptr->tci.for_dcch.included)
      {
        /* call the function to initialize DCCH quality threshole value */
        rxcrda_dcch_init();
      }
#endif /* End of FEATURE_DCCH_FRAME_QUALITY_SUPERVISION   */

      cmd_ptr->hdr.status = RXC_DONE_S;
      next_state = RXC_TCI_STATE;

#if defined (FEATURE_IS2000_P2) && !defined (FEATURE_IS2000_REL_A_SVD) 
      if(hw_version() >= MSM_50_A0)
      {
        if(cmd_ptr->tci.for_dcch.included && cmd_ptr->tci.for_fch.included)
        {
          MSG_ERROR("FCH & DCCH not allwd on MSM5000",0,0,0);
          cmd_ptr->hdr.status = RXC_BADPARM_S;
        }
      }
#endif /* FEATURE_IS2000_P2 && !FEATURE_IS2000_REL_A_SVD */

        INTFREE_SAV( isave );
        rxc_pwr.sch_tot_frames[0] = 0;
        rxc_pwr.sch_bad_frames[0] = 0;
        rxc_pwr.sch_fer_rep[0] = FALSE;
        rxc_pwr.sch_tot_frames[1] = 0;
        rxc_pwr.sch_bad_frames[1] = 0;
        rxc_pwr.sch_fer_rep[1] = FALSE;
#endif /* FEATURE_IS2000_REL_A_CC */

      break;
    }
    
    default:
    {
      ERR_FATAL("Bad state %d", curr_state, 0, 0);
    }
  } /* switch */

#ifdef FEATURE_IS2000_REL_A_CC
  return next_state;
#else
  rxc_atmr_set(rxc_atmr_isr,RXC_ATMR_5MS_FRAME_2, 0 );
  memset(&ltu_stats,0,sizeof(ltu_stats_type));
#endif /* FEATURE_IS2000_REL_A_CC */

} /* rxc_tci_init */


/*===========================================================================

FUNCTION RXC_TC_INIT

DESCRIPTION
  This funtion initializes the Traffic Channel processing state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void rxc_tc_init
(
  word curr_state   /* Current state of RXC state machine */
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( curr_state )
  {

    case RXC_TCI_STATE:
    {
      /* interrupt handler is already in Traffic Channel mode */
      /* set up variable for Traffic Channel message parsing state machine */
      rxc_tc_var.state = TC_HUNT_STATE;
      rxc_tc_var.msg_ptr = NULL;

      /* Initialize frame counts for reception watchdog */
      rxc_t5m_bad_frames = 0;
      rxc_n2m_bad_frames = 0;

#ifdef FEATURE_IS2000_REL_D
#error code not present
#endif /* FEATURE_IS2000_REL_D */
      {
        rxc_good_frames = 0;
      }
      rxc_fade = FALSE;
      rxc_sent_bad_rpt = FALSE;
      rxc_sent_pwron_rpt = FALSE;      /* used in hard handoff */

#ifdef FEATURE_RDA_CORRECTION
      rxcrda_dcch_dtx_fix_counter = -1;
#endif

#if defined(FEATURE_IS95B_MAHHO) || defined(FEATURE_GPS_MODE)
      rxc_eib_flags.previous = FALSE;
      rxc_eib_flags.current = FALSE;
      rxc_rifhho_cfs_gps_in_progress = FALSE;

      rxc_cfs_tuneback_count = 0;

      rxc_delay_after_hho_failure = FALSE;
      rxc_ignore_good_frame = FALSE;
#endif /* FEATURE_IS95B_MAHHO || FEATURE_GPS_MODE */

#ifdef FEATURE_GPSONE_OVERRIDE_RDA
#error code not present
#endif

#ifdef FEATURE_DETECT_SUP_MUTING

      rxc_sup_mute_init();  /* Initialize supplemental muting for MDR */

#endif /*FEATURE_DETECT_SUP_MUTING */

      /* No hard handoff in progress */
      rxc_hard_handoff_in_progress = FALSE;


      rxc_init_averaged_fer(); /* Initialize averaged FER counters */

#if defined(FEATURE_RLP_LOGGING)
      /* Initialize channel information used in RLP 3 payload logging. */
      memset (&rxc_rlp_logging_chan_info, 0, sizeof(rxc_rlp_logging_chan_info));
#endif
      break;
    }

    default:
    {
      ERR_FATAL( "Bad state %d", curr_state,0, 0 );
    }

  }/* switch */

} /* rxc_tc_init */


#ifdef FEATURE_IS2000_REL_A_CC
/*===========================================================================

FUNCTION RXCTRAFFIC_FRAME_BND_INIT

DESCRIPTION
  This funtion initializes the Traffic Channel processing state at the 
  frame boundary.  Note this is called in ISR context.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean rxctraffic_frame_bnd_init (void)
{
  
  /* switch to Traffic Channel and use initial rate set */
#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
  if (rxc_so.for_dcch.included)
  {
    rxc_set_dcch_mode  ((deint_rc_type)rxc_so.for_dcch.rc,
                        (ffpc_channel == CAI_FPC_PRI_CHAN_DCCH)
                       ); //lint !e730 Boolean argument to function
  }
  if (rxc_so.for_fch.included)
  {
    rxc_set_fch_mode   ((deint_rc_type)rxc_so.for_fch.rc,
                        (ffpc_channel == CAI_FPC_PRI_CHAN_FCH)
                       ); //lint !e730 Boolean argument to function
  }
#else
#ifdef FEATURE_IS2000_P2
  if (rxc_so.for_dcch.included)
  {
    // Note: for MSM5000, FCH and DCCH shares the same hardware
    deint_set_fch_mode (rxc_so.for_dcch.rc);
  }
  if (rxc_so.for_fch.included)
#endif // FEATURE_IS2000_P2
  {
    deint_set_fch_mode (rxc_so.for_fch.rc);
  }
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */

  /* get frame number and subtract 1 since the decoder output buffer will 
   * contain curr_frame_time-1 frame data in packet mode. */
  ts_get_20ms_frame_time (rxc_dec_frame);
  qw_dec (rxc_dec_frame, 1L);

  /* Decoder clk is TCXO in traffic state */
  dec_clk_select (DEC_CLK_TCXO);

  INTLOCK ();
  tramp_set_isr (TRAMP_RX_ISR, rxc_tc_isr);
  /* Clear DEC DONE int status to make sure ISR is properly serviced.
  */
  HWIO_OUT(DEC_DONE_INT_STATUS, 0x7);
  INTFREE ();

  return TRUE;
}
#endif /* FEATURE_IS2000_REL_A_CC */


/*===========================================================================

FUNCTION RXC_TCI

DESCRIPTION
  Traffic Channel Initialization processing state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Next state to be processed.

===========================================================================*/

word rxc_tci ( void )
{

  word next_state;
    /* next state to be processed */
  rxc_cmd_msg_type *cmd_ptr;
    /* pointer to command buffer received on rxc_cmd_q */
  rxc_dec_type *buf_ptr;
  word dec_rate;  /* rate returned by rxc_dec_rate_sel */
#ifdef FEATURE_IS2000_REL_D
#error code not present
#endif /* FEATURE_IS2000_REL_D */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - - -*/

  next_state = RXC_TCI_STATE;
  while( next_state == RXC_TCI_STATE )
  {

    (void)rex_clr_sigs(&rx_tcb, RXC_INT_SIG );
    if( (buf_ptr = (rxc_dec_type *)q_get(&rxc_dec_q)) != NULL )
    {
      if( buf_ptr->state == RXC_TC_STATE )
      {
#ifdef FEATURE_DCCH_FRAME_QUALITY_SUPERVISION  
        /* Variable holds result from test for a bad frame */
        boolean bad_frame_test;
        /* the dec_rate had been computed at the rxc_tc_isr() level */
        dec_rate = buf_ptr->dec_rate;
        bad_frame_test = (dec_rate == VOC_0_RATE);

        /* bad_frame_test is (rate == ERASURE) for FCH or
           (dcch_quality == POOR) for DCCH */
        bad_frame_test = (rxc_so.for_fch.included && bad_frame_test) ||
                      (rxc_so.for_dcch.included &&
                        (buf_ptr->dcch_chan_quality == RXC_POOR_CHANNEL));

#ifndef FEATURE_FACTORY_TESTMODE
        if( bad_frame_test )
#else
        if ( bad_frame_test &&
             (( ftm_mode==DMSS_MODE ) || !rxc_so.enable_skip_tci ))
#endif /* End of FEATURE_FACTORY_TESTMODE */

#else

#ifndef FEATURE_FACTORY_TESTMODE
        if( (dec_rate = rxc_dec_rate_sel()) == VOC_0_RATE )
#else
        if ( ((dec_rate = rxc_dec_rate_sel ()) == VOC_0_RATE ) &&
             (  ( ftm_mode==DMSS_MODE ) || !rxc_so.enable_skip_tci )
           )
#endif
#endif /* End of FEATURE_DCCH_FRAME_QUALITY_SUPERVISION  */
        {
          /* category 9 or 10 frame */
          rxc_t5m_bad_frames = 0;
          rxc_n2m_bad_frames = 0;
          rxc_good_frames = 0;

#ifndef FEATURE_IS2000_REL_D
          /* clear the valid rpt flag, get ready for the next rpt */
          rxc_sent_valid_rpt = FALSE;

          /* For FEATURE_IS2000_REL_D (Fast Call Setup Bypass 2 Good Frame feature), 
          ** do not set rxc_sent_valid_rpt to FALSE here.  If Mux already sent report, 
          ** Mux should not send it again when it gets bad frames and MC
          ** has not sent RXC_TC_F.  In this case, Mux will free wheel in rxc_tci
          ** but Mux won't send another valid report even if signal strength is
          ** fluctuating.
          */
#endif /* FEATURE_IS2000_REL_D */
        }
        else
        {
#ifndef FEATURE_FACTORY_TESTMODE
          if( ( ++rxc_good_frames >= cai_tmo.n5m )&&( !rxc_sent_valid_rpt ))
#else
          if (((++rxc_good_frames >= cai_tmo.n5m) ||
               (ftm_mode == FTM_MODE && rxc_so.enable_skip_tci))
              && (!rxc_sent_valid_rpt))
#endif
          {
            MSG_MED ("Send 2 valid fr rpt from rxctci()",0,0,0);
            rxc_send_two_valid_frame_rpt ();
          }
        }
      }
      q_put(&rxc_dec_free_q, &buf_ptr->link );
    }
    else
    {
      /* check for received commands */
      (void)rex_clr_sigs( rex_self(), RXC_CMD_Q_SIG );
      if( (cmd_ptr = (rxc_cmd_msg_type *)q_get(&rxc_cmd_q) ) != NULL )
      {
        switch( cmd_ptr->hdr.command )
        {
          case RXC_CDMA_F:
            MSG_MED ("Got RXC_CDMA_F",0,0,0);

            /* --------------------------------------------------
            ** Transition back to CDMA init state without exiting
            ** rxc_subtask()
            ** -------------------------------------------------- */

            /* --------------------------------------
            ** Clean up before switching to CDMA mode
            ** -------------------------------------- */
            rxc_exit();

            rxc_set_frame_offset(0);

            cmd_ptr->hdr.status = RXC_DONE_S;
            next_state = RXC_CDMA_STATE;
            break;

          case RXC_SC_F:
          {
            MSG_MED ("Got RXC_SC_F",0,0,0);

            /* initialize Sync channel state */
            rxc_sc_init( RXC_PC_STATE);
            cmd_ptr->hdr.status = RXC_DONE_S;
            next_state = RXC_SC_STATE;
            break;
          }/* RXC_SC_F */

          case RXC_EXIT_F:
          {
            MSG_MED ("Got RXC_EXIT_F",0,0,0);

            cmd_ptr->hdr.status = RXC_DONE_S;
            next_state = RXC_EXIT_STATE;
            break;
          }

          case RXC_TC_F:
          {
            MSG_MED ("Got RXC_TC_F",0,0,0);

            /* make sure all items are placed back on the rxtx_rx_free_q */
            if (rxc_tc_var.msg_ptr != NULL )
            {
              q_put(&rxtx_rx_free_q, &rxc_tc_var.msg_ptr->link );
              rxc_tc_var.msg_ptr = NULL;
            }
#ifdef FEATURE_IS2000_REL_D
#error code not present
#endif /* FEATURE_IS2000_REL_D */

            /* initialize for traffic channel */
            rxc_tc_init( RXC_TCI_STATE);
            cmd_ptr->hdr.status = RXC_DONE_S;
            next_state = RXC_TC_STATE;
            break;
          }/* RXC_TC_F */

#ifdef FEATURE_IS2000_REL_D
#error code not present
#endif /* FEATURE_IS2000_REL_D */

#ifdef FEATURE_FACTORY_TESTMODE
          case RXC_TCI_F:
          MSG_MED ("Got RXC_TCI_F",0,0,0);
          if(ftm_mode == FTM_MODE)
          {
            cmd_ptr->hdr.status = RXC_DONE_S;
            break;
          }
          /* For DMSS, fall through */
#endif /* FEATURE_FACTORY_TESTMODE */

          default:
          {
#ifdef FEATURE_FACTORY_TESTMODE
            if(ftm_mode == FTM_MODE)
            {
            MSG_HIGH( "Bad RXC cmd %d", cmd_ptr->hdr.command ,0, 0 );
            } else {
              ERR_FATAL("Bad cmd %d", cmd_ptr->hdr.command, 0, 0 );
            }
#else
            ERR_FATAL("Bad cmd %d", cmd_ptr->hdr.command, 0, 0 );
#endif /* FEATURE_FACTORY_TESTMODE */
          }
        } /* switch */

        /* return item to queue if requested */
        cmd_done( &cmd_ptr->hdr.cmd_hdr );

      }
      else
      {
        /* Wait for a command */
        (void) rxc_wait( RXC_CMD_Q_SIG | RXC_INT_SIG );
      }
    }

  } /* while */

  if (next_state != RXC_TC_STATE) 
  {
#ifdef FEATURE_IS2000_REL_D
#error code not present
#endif /* FEATURE_IS2000_REL_D */
    {
      
      /* Traffic Channel exit clean-up */
      rxc_free_tc_resources();
    }
    
    /* Reset reported vocoder frame rate */
    rxc_voc_rate = VOC_0_RATE;
    
    /* Reset active service options to none active */
    rxc_so.pri = CAI_SO_NULL;
    rxc_so.sec = CAI_SO_NULL;
  }

  return( next_state );

} /* rxc_tci */



/*===========================================================================

FUNCTION RXC_TC

DESCRIPTION
  Traffic Channel processing state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Next state to be processed.

===========================================================================*/

word rxc_tc( void )
{
  word next_state;
    /* Next state to be processed */
  rxc_cmd_msg_type *cmd_ptr;
    /* Pointer to command buffer received on rxc_cmd_q */
  voc_status_type voc_status;
    /* Status returned by voc_reinit procedure */
  voc_capability_type voc_option;
    /* Voice service option to configure vocoder */
  rex_sigs_type sigs = 0;
#if defined (FEATURE_IS95B_MAHHO) && !defined (FEATURE_HHO_ENHANCEMENTS)
  mccrx_rpt_type *rpt_ptr;
    /* pointer to report to send to Main Control task */
#endif /* FEATURE_IS95B_MAHHO && !FEATURE_HHO_ENHANCEMENTS */
#ifdef FEATURE_IS2000_REL_B
  boolean free_tc_resources = FALSE;
    /* Indicate if traffic channel resources have been freed */
#endif /* FEATURE_IS2000_REL_B */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  next_state = RXC_TC_STATE;

  /* Initialize voc_option to avoid compiler warning.
   * It must be set correctly based on rxc_so.pri
   */
  voc_option = VOC_CAP_IS96A;

  while (next_state == RXC_TC_STATE)
  {
    if (sigs & RXC_INT_SIG)
    {
#ifdef FEATURE_GPSONE_OVERRIDE_RDA
#error code not present
#endif /* FEATURE_GPSONE_OVERRIDE_RDA */

      /* Process Interrupt signal */
      rxc_tc_data();

      /* -------------------------------------------------------------
      ** If the primary service option is voice, configure the vocoder
      ** for voice option
      ** ------------------------------------------------------------- */
      if ((!rxc_voc_config_done) && ((rxc_so.pri == CAI_SO_VOICE_IS96A)
                                 ||  (rxc_so.pri == CAI_SO_VOICE_EVRC)
#ifndef FEATURE_NO_13K_VOICE_SUPPORT
                                 ||  (rxc_so.pri == CAI_SO_VOICE_13K)
                                 ||  (rxc_so.pri == CAI_SO_VOICE_13K_IS733)
#endif
                            #ifdef FEATURE_SO_VOICE_SMV
                                 ||  (rxc_so.pri == CAI_SO_VOICE_SMV)
                            #endif
#ifdef FEATURE_VOC_4GV
                                 ||  (rxc_so.pri == CAI_SO_VOICE_4GV_NB)
                                 ||  (rxc_so.pri == CAI_SO_VOICE_4GV_WB)
#endif /* FEATURE_VOC_4GV */

           ))
      {
        if (rxc_so.pri == CAI_SO_VOICE_IS96A)
        {
          voc_option = VOC_CAP_IS96A;
        }
#ifndef FEATURE_NO_13K_VOICE_SUPPORT
        else if (
                 rxc_so.pri == CAI_SO_VOICE_13K ||
                 rxc_so.pri == CAI_SO_VOICE_13K_IS733)
        {
          voc_option = VOC_CAP_IS733;
        }
#endif                 
        else if (rxc_so.pri == CAI_SO_VOICE_EVRC)
        {
          voc_option = VOC_CAP_IS127;
        }
#ifdef FEATURE_SO_VOICE_SMV
        else if (rxc_so.pri == CAI_SO_VOICE_SMV)
        {
          voc_option = VOC_CAP_SMV;
        }
#endif
#ifdef FEATURE_VOC_4GV
        else if (rxc_so.pri == CAI_SO_VOICE_4GV_NB)
        {
          voc_option = VOC_CAP_4GV_NB;
        }
        else if (rxc_so.pri == CAI_SO_VOICE_4GV_WB)
        {
          voc_option = VOC_CAP_4GV_WB;
        }
#endif /* FEATURE_VOC_4GV */

        voc_status = voc_config( VOC_ACQ_CDMA, voc_option );
        if (voc_status != VOC_DONE_S)
        {
          ERR_FATAL( "Bad vocoder status %d", voc_status, 0, 0 );
        }

        rxc_voc_config_done = TRUE;

      } /* end if (!rxc_voc_config_done) */

      /* Count down handoff fade timer */
      if( rxc_ho_cnt > 0 )
      {
        rxc_ho_cnt--;
      }
    }

    /* Check for received commands */
    (void) rex_clr_sigs( rex_self(), RXC_CMD_Q_SIG );
    if ((cmd_ptr = (rxc_cmd_msg_type *)q_get( &rxc_cmd_q )) != NULL)
    {
      switch (cmd_ptr->hdr.command)
      {
        case RXC_CDMA_F:
          MSG_MED ("Got RXC_CDMA_F",0,0,0);

          /* --------------------------------------------------
          ** Transition back to CDMA init state without exiting
          ** rxc_subtask()
          ** -------------------------------------------------- */

          /* -------------------------------------
          ** Log last frame rate entry if required
          ** ------------------------------------- */
#if !defined(FEATURE_DIAG_V7_LOG_IFACE)
          if (rxc_log_mar_buf.count != 0)
          {
            rxc_send_mar_log();
          }
#else
          if (rxc_mar_buf_ptr != NULL)
          {
            rxc_mar_buf_ptr->entry.count = rxc_mar_cnt;

            log_put(&rxc_mar_buf_ptr->desc);

            /* -------------------------------
            ** Indicate new buffer is required
            ** ------------------------------- */
            rxc_mar_buf_ptr = NULL;
          }
#endif

          if (rxc_tc_var.msg_ptr != NULL)
          {
            q_put( &rxtx_rx_free_q, &rxc_tc_var.msg_ptr->link );
            rxc_tc_var.msg_ptr = NULL;
          }

          /* --------------------------------------
          ** Clean up before switching to CDMA mode
          ** -------------------------------------- */
          rxc_exit();

          rxc_set_frame_offset(0);

          cmd_ptr->hdr.status = RXC_DONE_S;
          next_state = RXC_CDMA_STATE;
          break;

        case RXC_SC_F:
        {
          MSG_MED ("Got RXC_SC_F",0,0,0);

          /* --------------------------------------------------
          ** Transition back to SYNC_ACQ state without exiting
          ** rxc_subtask()
          ** -------------------------------------------------- */

          /* -------------------------------------
          ** Log last frame rate entry if required
          ** ------------------------------------- */
#if !defined(FEATURE_DIAG_V7_LOG_IFACE)
          if (rxc_log_mar_buf.count != 0)
          {
            rxc_send_mar_log();
          }
#else
          if (rxc_mar_buf_ptr != NULL)
          {
            rxc_mar_buf_ptr->entry.count = rxc_mar_cnt;

            log_put(&rxc_mar_buf_ptr->desc);

            /* -------------------------------
            ** Indicate new buffer is required
            ** ------------------------------- */
            rxc_mar_buf_ptr = NULL;
          }
#endif

          if (rxc_tc_var.msg_ptr != NULL)
          {
            q_put( &rxtx_rx_free_q, &rxc_tc_var.msg_ptr->link );
            rxc_tc_var.msg_ptr = NULL;
          }

          /* --------------------------------------
          ** Clean up before switching to SYNC mode
          ** -------------------------------------- */
          rxc_exit();

          cmd_ptr->hdr.status = RXC_DONE_S;

          rxc_sc_init( RXC_TC_STATE);

          next_state = RXC_SC_STATE;
          break;
        }/* RXC_SC_F */

        case RXC_EXIT_F:
        {
          MSG_MED ("Got RXC_EXIT_F",0,0,0);

          /* Log last frame rate entry if required */
#if !defined(FEATURE_DIAG_V7_LOG_IFACE)
          if (rxc_log_mar_buf.count != 0)
          {
            rxc_send_mar_log();
          }
#else
          if( rxc_mar_buf_ptr != NULL )
          {
            rxc_mar_buf_ptr->entry.count = rxc_mar_cnt;

            log_put( &rxc_mar_buf_ptr->desc );
            rxc_mar_buf_ptr = NULL;
              /* indicate new buffer is required */
          }
#endif

          if( rxc_tc_var.msg_ptr != NULL )
          {
            q_put(&rxtx_rx_free_q, &rxc_tc_var.msg_ptr->link );
            rxc_tc_var.msg_ptr = NULL;
          }

          cmd_ptr->hdr.status = RXC_DONE_S;
          next_state = RXC_EXIT_STATE;
          break;
        }

        case RXC_PWR_F:
        {
          MSG_MED ("Got RXC_PWR_F",0,0,0);

          /* Reset power control parameters */
          rxc_pwr.threshold_on = cmd_ptr->pwr.pwr_thresh_enable;
          rxc_pwr.periodic_on  = cmd_ptr->pwr.pwr_period_enable;

          if (rxc_pwr.threshold_on || rxc_pwr.periodic_on)
          {
            rxc_pwr.pwr_thresh   = cmd_ptr->pwr.pwr_rep_thresh;
            rxc_pwr.pwr_frames   = cmd_ptr->pwr.pwr_rep_frames;
            rxc_pwr.delay_frames = cmd_ptr->pwr.pwr_rep_delay;
            rxc_pwr.num_bad_frames   = 0;
            rxc_pwr.num_frames       = 0;
#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
            rxc_pwr.dcch_num_frames     = 0;
            rxc_pwr.dcch_num_bad_frames = 0;
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */
            rxc_pwr.num_delay_frames = 0;
            rxc_pwr.sch_tot_frames[0] = 0;
            rxc_pwr.sch_bad_frames[0] = 0;
            rxc_pwr.sch_fer_rep[0] = FALSE;
            rxc_pwr.sch_tot_frames[1] = 0;
            rxc_pwr.sch_bad_frames[1] = 0;
            rxc_pwr.sch_fer_rep[1] = FALSE;
          }
          cmd_ptr->pwr.hdr.status = RXC_DONE_S;
          break;
        } /* RXC_PWR_F */

        case RXC_ERASE_F:
        {
          MSG_MED ("Got RXC_ERASE_F",0,0,0);

          /* treat blank and burst as erasures */
          rxc_erase = TRUE;
          cmd_ptr->erase.hdr.status = RXC_DONE_S;
          break;
        }
        /* Processing of Supplemental Channel (SCH IS95C) update command */
#ifdef FEATURE_IS2000_SCH
        case RXC_SCH_F:
        {
          MSG_MED ("Got RXC_SCH_F",0,0,0);

          rxc_sch.rc = (dec_rc_type) cmd_ptr->sch.rc;
          rxc_sch.sch_rate = cmd_ptr->sch.sch_rate;
          rxc_sch.mux_pdu_double = cmd_ptr->sch.double_size_pdu;
          rxc_sch.code_type = cmd_ptr->sch.code_type;
#ifdef FEATURE_FACTORY_TESTMODE
          rxc_tc_var.sch_loopback = FALSE; /* Initialy off */
#endif
          MSM_LOCK();
          /* initialize the dtx threshold value */
          dec_dtx_thresh_init(rxc_sch.rc, rxc_sch.code_type, rxc_sch.sch_rate);

          DEMOD_ENABLE_CH2_HW_COMBINING( ); /* SCH Demodulation done by MAC */
          MSM_CLEAR( DEM1X_CHANNEL2_FRAME, PDCCH_1X_SEL );
          rxc_set_mac_rnd((deint_rc_type) rxc_sch.rc, rxc_sch.sch_rate,
                          rxc_sch.code_type);

#ifdef FEATURE_MULTIFRAME_INTERLEAVING
#error code not present
#else /* FEATURE_MULTIFRAME_INTERLEAVING */

#ifdef FEATURE_IS2000_REL_A //32X
          deint_set_sch_mode( rxc_sch.num_ltu, rxc_sch.ltu_len, rxc_sch.mux_pdu_type,
                              (deint_rc_type) rxc_sch.rc, rxc_sch.code_type,
                              rxc_sch.sch_rate);
          /* Initialize variables used in 2nd ATMR ISR */
          deint_atmr_isr2_init();

          /* Set 2nd ATMR ISR to go off at the 4th bin */
          rxc_atmr_set (rxc_atmr_isr2, RXC_ATMR_5MS_FRAME_4,0);
#else
          deint_set_sch_mode( (deint_rc_type) rxc_sch.rc, rxc_sch.code_type,
                              rxc_sch.sch_rate);
#endif /* FEATURE_IS2000_REL_A */
#endif /* FEATURE_MULTIFRAME_INTERLEAVING */

          if(rxc_sch.code_type == DEC_CONVOLUTIONAL)
          {
            rxc_sup_dec_int_enable = TRUE;
            rxc_turbo_dec_enabled = FALSE;
          }
          else
          {
            /* Decoding = TURBO */
            rxc_sup_dec_int_enable= TRUE;
            rxc_turbo_dec_enabled = TRUE;
          }
          MSM_UNLOCK()

          #ifdef FEATURE_DS_IS2000
          /* Initialize the MuxPDU dsm array, if it hasn't been initialized */
          if( rxc_dsm_arr_cnt == RXC_DSM_ARR_NOT_ALLOC)
          {
            rxc_init_sch_mux();
          }
          #endif /* FEATURE_DS_IS2000 */

          cmd_ptr->hdr.status = RXC_DONE_S;
          break;
        }

#ifdef FEATURE_IS95B_MDR
        /* Processing of Supplemental Code  Channel
           (SCCH IS95B) update command */
        case RXC_SCCH_F:
        {
          MSG_MED ("Got RXC_SCCH_F",0,0,0);

          MSM_LOCK();

          rxc_scch.num_sup = cmd_ptr->scch.num_sup;
          DEMOD_SET_CH2_SPR(SPR_64_CHIP_V);
          DEMOD_DISABLE_CH2_HW_COMBINING( );
          if(rxc_scch.num_sup == 0)
          {
            deint_delete_supplemental_task_list();
            rxc_sup_dec_int_enable = FALSE;
            rxc_turbo_dec_enabled = FALSE;
          }
          else
          {
            // Since we have SCCH, FCH must be included
            deint_set_scch_mode((deint_rc_type)rxc_so.for_fch.rc, rxc_scch.num_sup);
            rxc_sup_dec_int_enable = TRUE;
            rxc_turbo_dec_enabled = FALSE;
          }
          MSM_UNLOCK();
          cmd_ptr->hdr.status = RXC_DONE_S;
          break;
        }
#endif    /* FEATURE_IS95B_MDR */
#endif /*FEATURE_IS2000_SCH*/
        case RXC_RESET_F:
        {
          MSG_MED ("Got RXC_RESET_F",0,0,0);

          /* reset the fade timer */
          rxc_good_frames = 0;
          rxc_t5m_bad_frames = 0;
          rxc_n2m_bad_frames = 0;
          /* the following two lines are good to have */
          rxc_fade = FALSE;
          rxc_sent_bad_rpt = FALSE;

#ifdef FEATURE_RDA_CORRECTION
          rxcrda_dcch_dtx_fix_counter = -1;
#endif
          cmd_ptr->hdr.status = RXC_DONE_S;
          break;
        }

        case RXC_PWR_ON_F:
        {
          MSG_MED ("Got RXC_PWR_ON_F",0,0,0);

          /* Restart power control reporting */
          rxc_pwr.threshold_on = cmd_ptr->pwr_on.pwr_thresh_enable;
          rxc_pwr.periodic_on  = cmd_ptr->pwr_on.pwr_period_enable;

          rxc_pwr.num_frames     = 0;
          rxc_pwr.num_bad_frames = 0;
#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
          rxc_pwr.dcch_num_frames     = 0;
          rxc_pwr.dcch_num_bad_frames = 0;
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */
          rxc_pwr.sch_tot_frames[0] = 0;
          rxc_pwr.sch_bad_frames[0] = 0;
          rxc_pwr.sch_fer_rep[0] = FALSE;
          rxc_pwr.sch_tot_frames[1] = 0;
          rxc_pwr.sch_bad_frames[1] = 0;
          rxc_pwr.sch_fer_rep[1] = FALSE;

          cmd_ptr->hdr.status = RXC_DONE_S;
          break;
        }

        case RXC_PWR_RESET_F:
        {
          MSG_MED ("Got RXC_PWR_RESET_F",0,0,0);

          /* Initialize power control as specified in IS-95 6.6.4.1.1.1 */
          rxc_pwr.num_frames     = 0;
          rxc_pwr.num_bad_frames = 0;
#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
          rxc_pwr.dcch_num_frames     = 0;
          rxc_pwr.dcch_num_bad_frames = 0;
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */

          cmd_ptr->hdr.status = RXC_DONE_S;
          break;
        }

#ifdef FEATURE_IS2000_REL_A_SVD
        case RXC_RELEASE_SO_F:       /* rxc release service option */
        {
          MSG_MED ("Got RXC_RELEASE_SO_F",0,0,0);

          /* get the service option data from the service option ID */
          /* check if release request for primary service */
          if(rxc_srid_ref[cmd_ptr->release_so.sr_id].traffic_type == CAI_PRI_TRAF)
          {
            rxc_so.pri = CAI_SO_NULL;
          }
          /* check if release request for secondary service */
          if(rxc_srid_ref[cmd_ptr->release_so.sr_id].traffic_type == CAI_SEC_TRAF)
          {
            rxc_so.sec = CAI_SO_NULL;
          }
          cmd_ptr->hdr.status = RXC_DONE_S;
          break;
        }
#endif /* FEATURE_IS2000_REL_A_SVD */
        case RXC_RELEASE_F:
        {
          MSG_MED ("Got RXC_RELEASE_F",0,0,0);

          /* Release all services */
          rxc_so.pri = CAI_SO_NULL;
          rxc_so.sec = CAI_SO_NULL;
          cmd_ptr->hdr.status = RXC_DONE_S;
          break;
        }

        case RXC_PWR_REP_DELAY_F:

          MSG_MED ("Got RXC_PWR_REP_DELAY_F",0,0,0);

          /* --------------------------------------------------------------
          ** After sending a Power Measurement Report Message, the mobile
          ** station shall set TOT_FRAMES and BAD_FRAMES to zero and shall
          ** not increment either counter for a period of PWR_REP_DELAY * 4
          ** frames following the first transmission of the message.
          ** -------------------------------------------------------------- */
          rxc_pwr.num_frames = 0;
          rxc_pwr.num_bad_frames = 0;
#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
          rxc_pwr.dcch_num_frames     = 0;
          rxc_pwr.dcch_num_bad_frames = 0;
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */
          rxc_pwr.num_delay_frames = (word)(rxc_pwr.delay_frames * 4);

          /* Restart power control reporting */
          rxc_pwr.threshold_on = cmd_ptr->pwr_rep_delay.pwr_thresh_enable;
          rxc_pwr.periodic_on  = cmd_ptr->pwr_rep_delay.pwr_period_enable;

          cmd_ptr->hdr.status = RXC_DONE_S;
          break;

          case RXC_TC_F:
          {
            /* We have been told to go to traffic channel state but
             we are already there.  Just ignore the command.      */

            MSG_MED ("Got RXC_TC_F",0,0,0);
            cmd_ptr->hdr.status = RXC_DONE_S;

            break;
          }

#ifndef FEATURE_HHO_ENHANCEMENTS
        case RXC_HHO_BEGIN_F:
        {
          MSG_MED ("Got RXC_HHO_BEGIN_F",0,0,0);

          /* -------------------------------------------
          ** Perform CDMA to CDMA Hard Handoff processing
          ** ------------------------------------------- */
          rxc_hard_handoff_in_progress = TRUE;

#ifdef FEATURE_IS95B_MAHHO
          if ( ! rxc_rifhho_cfs_gps_in_progress )
          {
            /* Reset the fade timer */
            rxc_good_frames = 0;
            rxc_t5m_bad_frames = 0;

            /* Indicate that a bad frames report has been sent to the Main
               Control task in order to prevent RXC from sending one */
            rxc_sent_bad_rpt = TRUE;
          }
#else
          rxc_good_frames = 0;
          rxc_t5m_bad_frames = 0;
          rxc_sent_bad_rpt = TRUE;
#endif /* FEATURE_IS95B_MAHHO */

          /* Indicate that fade processing is in progress */
          rxc_fade = TRUE;

          /* Wait 1 frame before fade timer processing */
          rxc_ho_cnt = 1;

          /* Turn off power control reporting */
          rxc_pwr.threshold_on = FALSE;
          rxc_pwr.periodic_on  = FALSE;

          /* Indicate that RXC_PWR_ON_R hasn't been sent */
          rxc_sent_pwron_rpt = FALSE;

          /* Indicate that vocoder needs to be reinitialized */
          rxc_voc_init_done   = FALSE;
          rxc_voc_config_done = FALSE;

          cmd_ptr->hdr.status = RXC_DONE_S;
          break;
        }

#if defined (FEATURE_IS95B_MAHHO) || defined (FEATURE_GPS_MODE)
        case RXC_TUNE_RF_F:
        {
          MSG_MED ("Got RXC_TUNE_RF_F",0,0,0);

          /* rxc_ignore_good_frame is set to TRUE here to ignore any good
           * frames that are received during CFS or before verifying all the
           * handoff failure conditions for RIF HHO.  For RIF HHO, after
           * we've verified that none of the handoff failure conditions are
           * met, this flag will be set to FALSE (by SRCH) so that we can
           * start looking for that one good frame to declare a successful HHO.
           */
          rxc_ignore_good_frame = TRUE;

          cmd_ptr->hdr.status = RXC_DONE_S;

          break;
        }

        case RXC_RIFHHO_CFS_GPS_F:
        {
          MSG_MED ("Got RXC_RIFHHO_CFS_GPS_F",0,0,0);

          /* -----------------------------------------------------
          ** Perform Return-If-Fail Hard Handoff or CFS processing
          ** ----------------------------------------------------- */
          rxc_rifhho_cfs_gps_in_progress = TRUE;
          rxc_lost_frames = 0;
          rxc_use_stored_eibs = FALSE;
#ifdef FEATURE_IS2000_REL_A
          rxc_use_stored_qibs = FALSE;
          rxc_use_stored_sch_eibs = FALSE;
#endif /* FEATURE_IS2000_REL_A */


          if (cmd_ptr->rif_hho.is_hho)
          {
#ifdef FEATURE_IS95B_MAHHO
            /* Convert the 80ms timeout to a 20ms frame count */
            rxc_rif_hho_timeout_frames = cmd_ptr->rif_hho.timeout * 4;

            /* We use zero as an indication that there is no timeout.
             * If the base station was insane enough to set the timeout
             * to zero, send the timeout report immediately.
             */
            if (rxc_rif_hho_timeout_frames == 0)
            {
              /* send report of handoff timeout to Main Control task */
              if ( (rpt_ptr = (mccrx_rpt_type *)q_get(&mcc_rx_free_q) ) != NULL )
              {
                rpt_ptr->hdr.rpt = RXC_HHO_TIMEOUT_R;
                rxc_mc_rpt(rpt_ptr );
              }
              else
              {
                ERR( "0 bufs on mcc_rx_free_q", 0, 0, 0 );
                rxc_rif_hho_timeout_frames++; /* Try again next frame */
              }
            }
#endif /* FEATURE_IS95B_MAHHO */
          }
          else
          {
            rxc_rif_hho_timeout_frames = 0;
          }


          /* Indicate that a bad frames report has been sent to the Main
             Control task in order to prevent RXC from sending one */
          rxc_sent_bad_rpt = TRUE;

          /* To avoid getting valid signaling after tuning away,
          ** rxc_ignore_good_frame must be set to TRUE before we tune.
          ** For RIF HHO and CFS, RXC_TUNE_RF_F is sent before tuning
          ** to set rxc_ignore_good_frame, and then RXC_RIFHHO_CFS_GPS_F
          ** is sent after we tune.  For GPS we send RXC_RIFHHO_CFS_GPS_F
          ** before we tune and do not send the RXC_TUNE_RF_F at all, so
          ** we need to set rxc_ignore_good_frame to TRUE here for GPS.
          */
          rxc_ignore_good_frame = TRUE;

          cmd_ptr->hdr.status = RXC_DONE_S;
          break;
        }

        case RXC_RETURNED_F:
        {
          MSG_MED ("Got RXC_RETURNED_F",0,0,0);

          MSG_HIGH( "LOST_FRAME %d,rxc_fade: %d",
                    rxc_lost_frames, rxc_fade, 0 );


#ifdef FEATURE_FACTORY_TESTMODE
          if(ftm_mode == DMSS_MODE)
          {
#endif
#ifdef FEATURE_IS2000_REL_A
            rxc_tc_var.p_rev_in_use = cmd_ptr->ret.p_rev_in_use;

            // Clear the transistion flag if we force the P_REV.
            rxc_tc_var.in_p_rev_transition = FALSE;

#endif

            if (cmd_ptr->ret.scr.included)
            {
               rxc_process_scr (&cmd_ptr->ret.scr.data);
            }

            if ( cmd_ptr->ret.rev_pwr_cntl_delay.included )
            {
              /* program the delay of TX generated MASK_DATA to RX PCG timing */
              MUXMSM_AGC_CTL_MASK_DELAY( cmd_ptr->ret.rev_pwr_cntl_delay.value );
            }

            ffpc_returned_failure_cmd (cmd_ptr);

#ifdef FEATURE_FACTORY_TESTMODE
          }
#endif

          /*
           * Mark the end of the Return-If-Fail HHO or CFS.  This will
           * re-enable the fade timer and resume counting TOT_FRAMES and
           * BAD_FRAMES.  Note that fade_check still needs to be TRUE at
           * this point
           */
          rxc_rifhho_cfs_gps_in_progress = FALSE;

          // To delay for one frame
          rxc_cfs_tuneback_count = 2;



          /* When returning to the SF, set rxc_ignore_good_frame to FALSE
           * so that we can immediately start counting good frames to restart
           * the transmitter
           */
          rxc_ignore_good_frame = FALSE;

#ifdef FEATURE_GPSONE_OVERRIDE_RDA
#error code not present
#endif /* FEATURE_GPSONE_OVERRIDE_RDA */
          /*
          ** Need to wait N3M (2) consecutive good frames before re-enabling
          ** the transmitter if the interval between the time that the mobile
          ** station disables its transmitter and the time that it resumes
          ** using the SF >= N2M (12) frames per section 6.6.6.2.8.2.1 of
          ** IS-95B.
          */
          if ( rxc_lost_frames >= cai_tmo.n2m )
          {
            /* No need to use stored EIB's because we will receive 2 frames
             * before transmitting, so their EIB's will be used automatically.
             */
            if (rxc_hard_handoff_in_progress)
            {
              rxc_delay_after_hho_failure = TRUE;
            }
          }
          else
          {
            /* Restore EIBS if Rate Set 2 */
            if ( rxc_so.rate_set == DEC_RATE_14400 )
            {
              txc_flag_next_int();
              txc_eib_flag = rxc_eib_flags.previous;
              rxc_use_stored_eibs = TRUE;
#ifdef FEATURE_IS2000_REL_A
              txc_qib_flag = rxc_qib_flags.previous;
              txc_sch_eib_flag = rxc_sch_eib_flags.previous;
              rxc_use_stored_qibs = TRUE;
              rxc_use_stored_sch_eibs = TRUE;
#endif /* FEATURE_IS2000_REL_A */
            }

            rxc_start_tx();
          }

          cmd_ptr->hdr.status = RXC_DONE_S;
          break;
          }
#endif /* FEATURE_IS95B_MAHHO || FEATURE_GPS_MODE */

#ifdef FEATURE_GPS_MODE
#error code not present
#endif /* FEATURE_GPS_MODE */

#ifdef FEATURE_GPSONE_OVERRIDE_RDA
#error code not present
#endif /* FEATURE_GPSONE_CDMA_DEMOD_STOP */
#endif /* FEATURE_HHO_ENHANCEMENTS */

        case RXC_OLR_F:
        {
          MSG_MED ("Got RXC_OLR_F",0,0,0);

          ffpc_send_olrm ();

          cmd_ptr->hdr.status = RXC_DONE_S;
          break;
        }

#ifdef FEATURE_FACTORY_TESTMODE

        case RXC_FTM_FCH_LOOPBACK:
          MSG_MED ("Got RXC_FTM_FCH_LOOPBACK",0,0,0);
          rxc_tc_var.fch_loopback = cmd_ptr->loopback.loopback_on;
          break;
#ifdef FEATURE_IS2000_P2
                /* Support FEATURE_FTM_DCCH */
        case RXC_FTM_DCCH_LOOPBACK:
          MSG_MED ("Got RXC_FTM_DCCH_LOOPBACK",0,0,0);
          rxc_tc_var.dcch_loopback = cmd_ptr->loopback.loopback_on;
          break;
                /* End of FEATURE_FTM_DCCH */
#endif
        case RXC_FTM_SCH_LOOPBACK:
          MSG_MED ("Got RXC_FTM_SCH_LOOPBACK",0,0,0);
          rxc_tc_var.sch_loopback = cmd_ptr->loopback.loopback_on;
          break;
#endif

#ifdef FEATURE_IS2000_REL_A
        case RXC_GET_SETPT_F:
          MSG_MED ("Got RXC_GET_SETPT_F",0,0,0);
          ffpc_get_setpt
          (
            &(cmd_ptr->setpt.fch_incl),
            &(cmd_ptr->setpt.fpc_fch_curr_setpt),
            &(cmd_ptr->setpt.dcch_incl),
            &(cmd_ptr->setpt.fpc_dcch_curr_setpt),
            &(cmd_ptr->setpt.num_sup),
            &(cmd_ptr->setpt.sch_setpt_info[0].sch_id),
            &(cmd_ptr->setpt.sch_setpt_info[0].fpc_sch_curr_setpt)
          );
          cmd_ptr->hdr.status = RXC_DONE_S;
          break;
#endif //FEATURE_IS2000_REL_A

#ifdef FEATURE_IS2000_REL_B
        case RXC_IDLE_F:
          MSG_MED ("Got RXC_IDLE_F",0,0,0);

          /* Transition back to Idle state without going to Sync state
          ** first.
          */

          /* Log last frame rate entry if required (for Markov.)
          */
#if !defined(FEATURE_DIAG_V7_LOG_IFACE)
          if (rxc_log_mar_buf.count != 0)
          {
            rxc_send_mar_log();
          }
#else
          if (rxc_mar_buf_ptr != NULL)
          {
            rxc_mar_buf_ptr->entry.count = rxc_mar_cnt;
            log_put(&rxc_mar_buf_ptr->desc);
            rxc_mar_buf_ptr = NULL;
          }
#endif
          /* Make sure all items are placed back on the rxtx_rx_free_q 
          */
          if (rxc_tc_var.msg_ptr != NULL)
          {
            q_put( &rxtx_rx_free_q, &rxc_tc_var.msg_ptr->link );
            rxc_tc_var.msg_ptr = NULL;
          }

          /* Clean up before switching to Idle state.
          */
          rxc_exit();

          rxc_set_frame_offset(0);

          /* Call rxc_free_tc_resources() here instead of outside
          ** switch statement.  In case Mobile is configured to go to
          ** BCCH state, do not call rxc_free_tc_resources() after
          ** ATMR is set up for BCCH since rxc_free_tc_resources()
          ** will free up the ATMR. 
          */ 
          rxc_free_tc_resources (); 
          free_tc_resources = TRUE;

          /* Initialize for Idle state.  With new common channel support,
          ** Mobile will transition to either Paging channel or 
          ** BCCH/F-CCCH.
          */
          rxcidle_init ();
#ifdef FEATURE_IS2000_REL_A_CC
// TODO: Review: intentionally ignoring cmd_ptr->idle.frame since no
// channel actually needs the info (they all get frame number in their ISRs).
          next_state = rxcidle_chan_config (&cmd_ptr->idle.chan_info);
          /* this should call after inititialized funtion finishing its 
          execution, so that the atmr won't mess it up */
          rxcidle_set_slotted_status (RXCIDLE_SLOTTED_AWAKE);
#else
          if (cmd_ptr->idle.chan_info.chan_id == CAI_PCH) 
          {
            rxc_pc_init (RXC_SC_STATE, 
                         cmd_ptr->idle.chan_info.config.pch.rate);
            next_state = RXC_PC_STATE;
          }
#endif

          cmd_ptr->hdr.status = RXC_DONE_S;
          if (next_state == RXC_TC_STATE)
          {
            MSG_ERROR ("Failed Traf to Idle trans",0,0,0);
          } 
          break;
#endif /* FEATURE_IS2000_REL_B */

#ifdef FEATURE_IS2000_REL_D
#error code not present
#endif /* FEATURE_IS2000_REL_D */

        default:
        {
#ifdef FEATURE_FACTORY_TESTMODE
          if(ftm_mode == FTM_MODE)
          {
          MSG_HIGH( "Bad RXC cmd %d", cmd_ptr->hdr.command ,0, 0 );
          } else {
            ERR_FATAL("Bad cmd %d", cmd_ptr->hdr.command, 0, 0 );
          }
#else
          ERR_FATAL("Bad cmd %d", cmd_ptr->hdr.command, 0, 0 );
#endif /* FEATURE_FACTORY_TESTMODE */
        }
      } /* switch */

      /* return item to queue if requested */
      cmd_done( &cmd_ptr->hdr.cmd_hdr );

      (void) rex_clr_sigs( rex_self(), RXC_CMD_Q_SIG );
    }
    else
    {
      /* Wait for a command or an interrupt */
      sigs = rxc_wait( RXC_CMD_Q_SIG | RXC_INT_SIG );
    }
  } /* while */

#ifdef FEATURE_IS2000_REL_B
  if (free_tc_resources == FALSE) 
  {
    /* If haven't done it above, do traffic Channel exit clean-up now */
    rxc_free_tc_resources();
  }
#else
  /* Traffic Channel exit clean-up */
  rxc_free_tc_resources();
#endif /* FEATURE_IS2000_REL_B */

  /* Reset reported vocoder frame rate */
  rxc_voc_rate = VOC_0_RATE;

  /* Reset active service options to none active */
  rxc_so.pri = CAI_SO_NULL;
  rxc_so.sec = CAI_SO_NULL;

  return (next_state);

} /* rxc_tc */



/*===========================================================================

FUNCTION RXC_RESET_VOC_INIT

DESCRIPTION
  Theis routine signals a re- voc_init.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void rxc_reset_voc_init ( void )
{
  word isav;

  /* lock out interrupts to update state */
  INTLOCK_SAV(isav);

  /* reset frame down counter */
  rxc_time_cnt = RXC_VOC_FRAME_DELAY;

  /* signal vocoder module to re-initialize */
  (void)voc_init();

  /* release interrupt lock */
  INTFREE_SAV(isav);
}



#if defined (FEATURE_IS95B_MDR)
#ifdef FEATURE_DETECT_SUP_MUTING
/*===========================================================================

FUNCTION RXC_SUP_MUTE

DESCRIPTION
  This function executes a algorithm to detect muting of supplemental channels
  by the basestation. In MDR transmission, IS-95B allows the basestation to not
  to deallocate the unused supplemental channels. Hence typically a basestation
  may allocate supplementals to the mobile when the MDR call is initiated but
  if there is no forward link traffic then it may just stop xmitting on these.
  So the mobile is demodulating uncoded walsh channels which can result in
  random CRC passes. We term this not-transmitting-on-allocated-traffic-channels
  as "muting".

  C0 = [ 0 0 0 0 0 0 0]   #CRC status of last 20ms supplemental channels
  C1 = [ 1 1 0 0 0 1 1]   #CRC status of this 20ms supplemental channels

  Dn = [ 1.0 1.0 1.0 1.0 1.0 1.0 1.0 ]  # Data indicator for prev 20ms frame

  a = 0.1, b = 0.9

  IIR filter equation is:
  Dn = b*(128*C1) + a*Dn_1

  Dn_1 = Dn for next iteration.

  C1 is multiplied by 128 to avoid floating point operations on the non-floating
  ARM processor. This algorithm is executed to obtain Dn for every supplemental
  but the decision is used to alter sup channel status only if the Dn threshold
  exceeds DATA_IND_THRESHOLD which is set typically around 15.0


DEPENDENCIES
  None.

RETURN VALUE
  DEC_RATE_ALL : If the supplemental channel is to be erasued.
  DEC_RATE_FULL : If the supplemental channel is to be kept full rate.

SIDE EFFECTS
  None.

===========================================================================*/
rlp_rate_enum_type rxc_sup_mute(
                   rlp_rate_enum_type sup_status,/* 0->erasure, 1->full sup*/
                   byte si              /* index of sup chanel */
                 )
{
byte cur_crc;

  /* C = 1 if sup channel is full rate else C = 0 */
  cur_crc = ( sup_status == RLP_FULL_RATE ? 1 : 0 );

  /* Run a Dn = 0.1*(128*C) + 0.9Dn-1 IIR filter here */
  cur_di[si]=(int)( ( 128 * cur_crc + 9 * past_di[si] ) / 10 );

  past_di[si] = cur_di[si];    /* assign Dn-1 = Dn */

  /* if this frame is FULL *and* previous frame was an erasure */

  if((sup_status==RLP_FULL_RATE)&&(past_sup_status[si]==RLP_ERASURE))
  {

    /* Aloha ! so here is a 0->1 transition */

    past_sup_status[si] = sup_status;

    if( cur_di[si] < DATA_IND_THRESHOLD )
    {
      /* now check if Yamomato bit is set for this sup */
      if( !(sup_qmet &  (0x1 << si)) )
      {
         /* sorry! yamomato bit says its a bad full rate frame */

         MSG_HIGH("MDR: Muting a Fwd link sup [%d]!",si,0,0);

         return RLP_ERASURE; /* Erase this frame though it passes CRC */
      }

    } /* Data indicator crosses threshold */

  }/* if n is full and n-1 is an erasure */

  past_sup_status[si] = sup_status;
  return sup_status;
}
#endif /* FEATURE_DETECT_SUP_MUTING */
#endif /* FEATURE_IS95B_MDR */

byte rxc_sw_beta_sch[3][6] = {
  { /* RC3 */
    0x80, /* 1x,  Walsh length 64 */
    0x5b, /* 2x,  Walsh length 32 */
    0x80, /* 4x,  Walsh length 16 */
    0x5b, /* 8x,  Walsh length  8 */
    0x80, /* 16x, Walsh length  4 */
    0x00  /* unused for RC3 */
  },
  { /* RC4 */
    0x5b, /* 1x,  Walsh length 128 */
    0x80, /* 2x,  Walsh length  64 */ 
    0x5b, /* 4x,  Walsh length  32 */ 
    0x80, /* 8x,  Walsh length  16 */ 
    0x5b, /* 16x, Walsh length   8 */ 
    0x80  /* 32x, Walsh length   4 */ 
  },
  { /* RC5 */
    0x80, /* 1x,  Walsh length 64 */
    0x5b, /* 2x,  Walsh length 32 */
    0x80, /* 4x,  Walsh length 16 */
    0x5b, /* 8x,  Walsh length  8 */
    0x80, /* 16x, Walsh length  4 */
    0x00  /* unused for RC5 */
  }
};

#ifdef MSMHW_TURBO_CODING
byte rxc_sw_turbo_beta[3][5] = {
  { /* RC3 */
    0x48, /* 2x */
    0x66, /* 4x */
    0x48, /* 8x */
    0x66, /* 16x */
    0x00  /* unused for RC3 */
  },
  { /* RC4 */
    0x90, /* 2x */
    0x66, /* 4x */
    0x90, /* 8x */
    0x66, /* 16x */
    0x90  /* 32x */
  },
  { /* RC5 */
    0x58, /* 2x */
    0x7c, /* 4x */
    0x58, /* 8x */
    0x7c, /* 16x */
    0x00  /* unused for RC5 */
   }
};
#endif /* MSMHW_TURBO_CODING */

/*========================================================================

FUNCTION RXC_SET_MAC_RND

DESCRIPTION This function sets the MAC Engine Rounding factors based on the
             spreading Rate.

DEPENDENCIES MAC Engine should be enabled.

RETURN VALUE None

SIDE EFFECTS None

=========================================================================*/

void rxc_set_mac_rnd
(
  deint_rc_type rc,
  dec_sch_rate_type sch_rate,
  dec_coding_type code_type
)
{
  /* Local variable to capture walsh length */
  uint8 walsh_length = SPR_64_CHIP_V;
  byte sw_sch_scale_value;
  word decoder_type;
  MSM_LOCK();
#ifdef MSMHW_TURBO_CODING
  if(code_type == DEC_CONVOLUTIONAL)
  {
    decoder_type = DMOD_COMB_RND_CONVOLUTIONAL;
    sw_sch_scale_value = rxc_sw_beta_sch[DEINT_SCH_RC_INDEX(rc)][sch_rate];
  }
  else
  {
    /* code_type = TURBO */
    decoder_type = DMOD_COMB_RND_TURBO;
    sw_sch_scale_value = rxc_sw_turbo_beta[DEINT_SCH_RC_INDEX(rc)]
                    [(((sch_rate - 1) > 0) ? (sch_rate - 1) : 0) ];
  }
#else /* turbo codes not supported */

  /* Unused Parameter */
  (void) code_type;
   
    decoder_type = DMOD_COMB_RND_CONVOLUTIONAL;
    sw_sch_scale_value = rxc_sw_beta_sch[DEINT_SCH_RC_INDEX(rc)][sch_rate];
#endif /* MSMHW_TURBO_CODING */

  DEMOD_SET_SW_BETA_CH2(sw_sch_scale_value);
  DEMOD_REP_FACTOR( DMOD_REP_FACTOR_1 );

#ifdef FEATURE_IS2000_REL_A
#ifdef FEATURE_IS2000_REL_A_TD
  DEMOD_SET_MAC_SEQ_REP_1( );
    /* Set the CH2_TD_SIGN register for sequence repetition = 1.
       Special case for RC4 1X is set for that case below. */
#endif /* FEATURE_IS2000_REL_A_TD */
#endif /* FEATURE_IS2000_REL_A */

  switch(rc)
  {
    case DEINT_RC3:
    case DEINT_RC5:
    {
      switch(sch_rate)
      {
        case DEC_SCH_RATE_1X:
        {
          DEMOD_SET_MAC_RND_128_64_CHIPS(decoder_type);
          walsh_length = SPR_64_CHIP_V;
          break;
        }
        case DEC_SCH_RATE_2X:
        {
          DEMOD_SET_MAC_RND_32_16_CHIPS(decoder_type);
          walsh_length = SPR_32_CHIP_V;
          break;
        }
        case DEC_SCH_RATE_4X:
        {
          DEMOD_SET_MAC_RND_32_16_CHIPS(decoder_type);
          walsh_length = SPR_16_CHIP_V;
          break;
        }
        case DEC_SCH_RATE_8X:
        {
          DEMOD_SET_MAC_RND_8_4_CHIPS(decoder_type);
          walsh_length = SPR_8_CHIP_V;
          break;
        }
        case DEC_SCH_RATE_16X:
        {
          DEMOD_SET_MAC_RND_8_4_CHIPS(decoder_type);
          walsh_length = SPR_4_CHIP_V;
          break;
        }
        default:
          MSG_FATAL("Bad SCH rate ", 0,0,0);
          break;
      }
      break;
    }

    case DEINT_RC4:
    {
      switch (sch_rate)
      {
        case DEC_SCH_RATE_1X:
        {
          DEMOD_SET_MAC_RND_128_64_CHIPS(decoder_type);
          walsh_length = SPR_128_CHIP_V;

          break;
        }
        case DEC_SCH_RATE_2X:
        {
          DEMOD_SET_MAC_RND_128_64_CHIPS(decoder_type);
          walsh_length = SPR_64_CHIP_V;
          break;
        }
        case DEC_SCH_RATE_4X:
        {
          DEMOD_SET_MAC_RND_32_16_CHIPS(decoder_type);
          walsh_length = SPR_32_CHIP_V;
          break;
        }
        case DEC_SCH_RATE_8X:
        {
          DEMOD_SET_MAC_RND_32_16_CHIPS(decoder_type);
          walsh_length = SPR_16_CHIP_V;
          break;
        }
        case DEC_SCH_RATE_16X:
        {
          DEMOD_SET_MAC_RND_8_4_CHIPS(decoder_type);
          walsh_length = SPR_8_CHIP_V;
          break;
        }
#ifdef FEATURE_32X_SCH_DATA_RATE
        case DEC_SCH_RATE_32X:
        {
          DEMOD_SET_MAC_RND_8_4_CHIPS(decoder_type);
          walsh_length = SPR_4_CHIP_V;
          break;
        }
#endif /* FEATURE_32X_SCH_DATA_RATE */

        default:
          MSG_FATAL("Bad SCH rate",0,0,0);
          break;
      }
      break;
    }

    default:
      MSG_FATAL("Bad RC", 0,0,0);
      break;
  }

  /* Load the demod spreading rate */
  DEMOD_SET_CH2_SPR(walsh_length);

  MSM_UNLOCK();
}


#ifdef FEATURE_IS2000_REL_A_CC
/*========================================================================

FUNCTION RXC_SET_MAC_RND_CCCH

DESCRIPTION This function sets the MAC Engine Rounding factors based on the
             code rate and data rate.

DEPENDENCIES MAC Engine should be enabled.

RETURN VALUE None

SIDE EFFECTS None

=========================================================================*/

void rxc_set_mac_rnd_ccch
(
  deint_code_rate_type code_rate,
  dec_ccch_data_rate_type data_rate,
  deint_ch_id_type chan_id
)
{
  byte sw_sch_scale_value;
  word decoder_type;

  MSM_LOCK();

  decoder_type = DMOD_COMB_RND_CONVOLUTIONAL;
  /* What should the beta scaling factors be for CCCH ??? */
  sw_sch_scale_value = rxc_sw_beta_sch[code_rate][data_rate];

  if (chan_id == DEINT_CH1)
  {
    DEMOD_SET_SW_BETA_CH1(sw_sch_scale_value);
  }
  else
  {
    DEMOD_SET_SW_BETA_CH2(sw_sch_scale_value);
  }

  DEMOD_REP_FACTOR( DMOD_REP_FACTOR_1 );

#ifdef FEATURE_IS2000_REL_A
#ifdef FEATURE_IS2000_REL_A_TD
  DEMOD_SET_MAC_SEQ_REP_1( );
    /* Set the CH2_TD_SIGN register for sequence repetition = 1.
       Special case for RC4 1X is set for that case below. */
#endif /* FEATURE_IS2000_REL_A_TD */
#endif /* FEATURE_IS2000_REL_A */

  switch(code_rate)
  {
    case DEINT_CODE_RATE_1BY4:
    {
      switch(data_rate)
      {
        case DEC_CCCH_FULL_RATE:
        {
          if (chan_id == DEINT_CH1)
          {
            DEMOD_SET_MAC_RND_128_64_CHIPS_CH1(decoder_type);
            DEMOD_SET_CH1_SPR(SPR_64_CHIP_V);
          }
          else
          {
            DEMOD_SET_MAC_RND_128_64_CHIPS(decoder_type);
            DEMOD_SET_CH2_SPR(SPR_64_CHIP_V);
          }
          break;
        }
        case DEC_CCCH_2X_RATE:
        {
          if (chan_id == DEINT_CH1)
          {
            DEMOD_SET_MAC_RND_32_CHIPS_CH1(decoder_type);
            DEMOD_SET_CH1_SPR(SPR_32_CHIP_V);
          }
          else
          {
            DEMOD_SET_MAC_RND_32_16_CHIPS(decoder_type);
            DEMOD_SET_CH2_SPR(SPR_32_CHIP_V);
          }
          break;
        }
        case DEC_CCCH_4X_RATE:
        {
          if (chan_id == DEINT_CH1)
          {
            MSG_FATAL("Not support FCCCH 16 chips spread on CH2",0,0,0);
          }
          else
          {
            DEMOD_SET_MAC_RND_32_16_CHIPS(decoder_type);
            DEMOD_SET_CH2_SPR(SPR_16_CHIP_V);
          }
          break;
        }
        default:
          MSG_FATAL("Bad CCCH Rate: %d",data_rate,0,0);
          break;
      }
      break;
    }

    case DEINT_CODE_RATE_1BY2:
    {
      switch (data_rate)
      {
        case DEC_CCCH_FULL_RATE:
        {
          if (chan_id == DEINT_CH1)
          {
            DEMOD_SET_MAC_RND_128_64_CHIPS_CH1(decoder_type);
            DEMOD_SET_CH1_SPR(SPR_128_CHIP_V);
          }
          else
          {
            DEMOD_SET_MAC_RND_128_64_CHIPS(decoder_type);
            DEMOD_SET_CH2_SPR(SPR_128_CHIP_V);
          }

          break;
        }
        case DEC_CCCH_2X_RATE:
        {
          if (chan_id == DEINT_CH1)
          {
            DEMOD_SET_MAC_RND_128_64_CHIPS_CH1(decoder_type);
            DEMOD_SET_CH1_SPR(SPR_64_CHIP_V);
          }
          else
          {
            DEMOD_SET_MAC_RND_128_64_CHIPS(decoder_type);
            DEMOD_SET_CH2_SPR(SPR_64_CHIP_V);
          }
          break;
        }
        case DEC_CCCH_4X_RATE:
        {
          if (chan_id == DEINT_CH1)
          {
            DEMOD_SET_MAC_RND_32_CHIPS_CH1(decoder_type);
            DEMOD_SET_CH1_SPR(SPR_32_CHIP_V);
          }
          else
          {
            DEMOD_SET_MAC_RND_32_16_CHIPS(decoder_type);
            DEMOD_SET_CH2_SPR(SPR_32_CHIP_V);
          }
          break;
        }
        default:
          MSG_FATAL("Bad CCCH Rate: %d",data_rate,0,0);
          break;
      }
      break;
    }

    default:
      MSG_FATAL("Bad Code Rate: %d ", code_rate,0,0);
      break;
  }
  MSM_UNLOCK();
}
#endif /* FEATURE_IS2000_REL_A_CC */

/*========================================================================

FUNCTION RXC_PROCESS_SCR

DESCRIPTION This function processes the Service Configuration record.

DEPENDENCIES ??

RETURN VALUE None

SIDE EFFECTS None

=========================================================================*/
void
rxc_process_scr
(
 rxc_scr_info_type  *scr
)
{
  boolean prev_rxc_so_pri = rxc_so.pri;
  boolean prev_rxc_so_sec = rxc_so.sec;
#ifdef FEATURE_IS2000_REL_A
  word so_index;
#endif /* FEATURE_IS2000_REL_A */
#ifdef FEATURE_IS2000_P2
  if( !scr->for_fch.included && !scr->for_dcch.included )
  {  /* Neither is included, turn it off */
    rxc_free_tc_resources();
  }

  /* Is DCCH configured */
  if ( scr->for_dcch.included )
  {
    // If DCCH is newly added or changing RC
    if ( !rxc_so.for_dcch.included ||
         scr->for_dcch.rc != rxc_so.for_dcch.rc )
    {
      rxc_so.for_dcch.included = TRUE;
      rxc_so.for_dcch.rc = scr->for_dcch.rc;
#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
      if (ffpc_channel == CAI_FPC_PRI_CHAN_DCCH)
      {
        rxc_config_demod ( (deint_rc_type)rxc_so.for_dcch.rc );
        rxc_set_dcch_mode( (deint_rc_type)rxc_so.for_dcch.rc, TRUE);
      }
      else
      {
        rxc_config_demod_ch1 ( (deint_rc_type)rxc_so.for_dcch.rc );
        rxc_set_dcch_mode( (deint_rc_type)rxc_so.for_dcch.rc, FALSE);
      }
#else
      rxc_config_demod ( (deint_rc_type)rxc_so.for_dcch.rc );
      deint_set_fch_mode ((deint_rc_type)rxc_so.for_dcch.rc );

#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */
      /* For SRCH logging */
      // Note: we know that FCH is not included since DCCH included
      // We assume only 1 of FCH/DCCH supported here
      srch_mux_update_rc (DEINT_RC_INVALID, 
                          (deint_rc_type)rxc_sch.rc, 
                          DEINT_RC_INVALID,
                          (deint_rc_type)rxc_so.for_dcch.rc);
    }
    rxc_so.rate_set = fwd_rc_to_rate_set[rxc_so.for_dcch.rc];
    rxc_so.high_rc = TRUE;
  }
  else
  {
    rxc_so.for_dcch.included = FALSE;
#ifdef FEATURE_IS2000_REL_A_SVD
    /* Disables the processing of the DCCH.
    */
    rxc_so.for_dcch.data_valid = FALSE;
    rxc_so.for_dcch.data_valid_countdown = 0;
#endif /* FEATURE_IS2000_REL_A_SVD */
  }
#endif /* FEATURE_IS2000_P2 */

  if (scr->for_fch.included)
  {
    // If FCH is newly added or changing RC
    if (
        #ifdef FEATURE_IS2000_P2
        !rxc_so.for_fch.included ||
        #endif // FEATURE_IS2000_P2
        scr->for_fch.rc != rxc_so.for_fch.rc )
    {
      #ifdef FEATURE_IS2000_P2
      rxc_so.for_fch.included = TRUE;
      #endif // FEATURE_IS2000_P2

      rxc_so.for_fch.rc = scr->for_fch.rc;
#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
      if (ffpc_channel == CAI_FPC_PRI_CHAN_FCH)
      {
        rxc_config_demod ((deint_rc_type)rxc_so.for_fch.rc);
        rxc_set_fch_mode ((deint_rc_type)rxc_so.for_fch.rc, TRUE);
      }
      else
      {
        rxc_config_demod_ch1 ((deint_rc_type)rxc_so.for_fch.rc);
        rxc_set_fch_mode ((deint_rc_type)rxc_so.for_fch.rc, FALSE);
      }
#else
      rxc_config_demod ( (deint_rc_type)rxc_so.for_fch.rc );
      deint_set_fch_mode ( (deint_rc_type)rxc_so.for_fch.rc );

#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */

         /* For SRCH logging */
      // Note: we know that DCCH is not included since FCH included
      // We assume only 1 of FCH/DCCH supported here
      srch_mux_update_rc ((deint_rc_type)rxc_so.for_fch.rc, 
                          (deint_rc_type)rxc_sch.rc, 
                          DEINT_RC_INVALID,
                          DEINT_RC_INVALID);
    }
    rxc_so.rate_set = fwd_rc_to_rate_set[rxc_so.for_fch.rc];
    rxc_so.high_rc = rxc_so.for_fch.rc > CAI_RC_2;
  }
#ifdef FEATURE_IS2000_P2
  else
  {
    rxc_so.for_fch.included = FALSE;
#ifdef FEATURE_IS2000_REL_A_SVD
    /* Disables the processing of the FCH.
    */
    rxc_so.for_fch.data_valid = FALSE;
    rxc_so.for_fch.data_valid_countdown = 0;
#endif /* FEATURE_IS2000_REL_A_SVD */
  }
#endif /* FEATURE_IS2000_P2 */

#ifndef FEATURE_IS2000_REL_A
   if ((scr->change_pri) && (scr->pri != rxc_so.pri))
   {
      rxc_so.pri = scr->pri;

      /* For voice service options, indicate that voice option is
         not configured */
      if ( scr->pri == CAI_SO_VOICE_IS96A ||
           scr->pri == CAI_SO_VOICE_EVRC 
#ifndef FEATURE_NO_13K_VOICE_SUPPORT
           || scr->pri == CAI_SO_VOICE_13K  
           || scr->pri == CAI_SO_VOICE_13K_IS733  
#endif           

#ifdef FEATURE_SO_VOICE_SMV
           || scr->pri == CAI_SO_VOICE_SMV
#endif
#ifdef FEATURE_VOC_4GV
           || scr->pri == CAI_SO_VOICE_4GV_NB
           || scr->pri == CAI_SO_VOICE_4GV_WB
#endif /* FEATURE_VOC_4GV */
           )
      {
         rxc_voc_config_done = FALSE;
      }
      else if (scr->pri == CAI_SO_NULL)
      {
         rxc_voc_config_done = FALSE;
         if (rxc_sup_dec_int_enable)
         {
#if  defined (FEATURE_IS95B_MDR)
            /* This was a MDR/HDR call. So delete SCCH/SCH task from
               the deinterleaver Task RAM. */
            rxc_sup_dec_int_enable = FALSE;
            rxc_turbo_dec_enabled = FALSE;
            rxc_scch.num_sup = 0;
            deint_delete_supplemental_task_list();
#endif
         }
      }
   }
#else
   /* ckech if there is new service option record */
  if (scr->num_of_so_recs > 0)
  {
    /* search through all the new service option records */
    for (so_index = 0; so_index < scr->num_of_so_recs; so_index++)
    {
      /* check if this the primary service option */
      if (scr->so_cfgs[so_index].traffic_type == CAI_PRI_TRAF)
      {
        if (scr->so_cfgs[so_index].so != rxc_so.pri)
        {
          rxc_so.pri = scr->so_cfgs[so_index].so;
          /* For voice service options, indicate that voice option is
          not configured */
          if (scr->so_cfgs[so_index].so == CAI_SO_VOICE_IS96A ||
              scr->so_cfgs[so_index].so == CAI_SO_VOICE_EVRC  ||
              scr->so_cfgs[so_index].so == CAI_SO_VOICE_13K_IS733 ||
              scr->so_cfgs[so_index].so == CAI_SO_VOICE_13K  
              #ifdef FEATURE_SO_VOICE_SMV
              || scr->so_cfgs[so_index].so == CAI_SO_VOICE_SMV                
              #endif
#ifdef FEATURE_VOC_4GV
              || scr->so_cfgs[so_index].so == CAI_SO_VOICE_4GV_NB
              || scr->so_cfgs[so_index].so == CAI_SO_VOICE_4GV_WB
#endif /* FEATURE_VOC_4GV */
              
              )
          {
            rxc_voc_config_done = FALSE;
          }
          else if (scr->so_cfgs[so_index].so == CAI_SO_NULL)
          {
            rxc_voc_config_done = FALSE;
            if (rxc_sup_dec_int_enable)
            {
              /* This was a MDR/HDR call. So delete SCCH/SCH task from
              the deinterleaver Task RAM. */
              #if  defined (FEATURE_IS95B_MDR)
              rxc_sup_dec_int_enable = FALSE;
              rxc_scch.num_sup = 0;
              deint_delete_supplemental_task_list();
              #endif
            }
          }
        }
#ifdef FEATURE_IS2000_REL_A_SVD
        /* if PRIMARY SERVICE is the only service, reset the secondary SO */
        if (scr->num_of_so_recs == 1)
        {
          if (rxc_so.sec != CAI_SO_NULL)
          {
            MSG_MED ("Rel Sec SO", 0, 0, 0);
          }
          rxc_so.sec = CAI_SO_NULL;
        }
#endif /* end of FEATURE_IS2000_REL_A_SVD */
      }
#ifdef FEATURE_IS2000_REL_A_SVD
      /* check if this the secondary service option */
      if (scr->so_cfgs[so_index].traffic_type == CAI_SEC_TRAF)
      {
        if (scr->so_cfgs[so_index].so != rxc_so.sec)
        {
          rxc_so.sec = scr->so_cfgs[so_index].so;
          /* For voice service options, indicate that voice option is
          not configured */
          if (scr->so_cfgs[so_index].so == CAI_SO_VOICE_IS96A ||
              scr->so_cfgs[so_index].so == CAI_SO_VOICE_EVRC  ||
              scr->so_cfgs[so_index].so == CAI_SO_VOICE_13K_IS733 ||
              scr->so_cfgs[so_index].so == CAI_SO_VOICE_13K 
              #ifdef FEATURE_SO_VOICE_SMV
              || scr->so_cfgs[so_index].so == CAI_SO_VOICE_SMV                
              #endif
#ifdef FEATURE_VOC_4GV
              || scr->so_cfgs[so_index].so == CAI_SO_VOICE_4GV_NB
              || scr->so_cfgs[so_index].so == CAI_SO_VOICE_4GV_WB
#endif /* FEATURE_VOC_4GV */
              )
          {
            rxc_voc_config_done = FALSE;
          }
          else if (scr->so_cfgs[so_index].so == CAI_SO_NULL)
          {
            rxc_voc_config_done = FALSE;
            if (rxc_sup_dec_int_enable)
            {
              /* This was a MDR/HDR call. So delete SCCH/SCH task from
              the deinterleaver Task RAM. */
              #if  defined (FEATURE_IS95B_MDR)
              rxc_sup_dec_int_enable = FALSE;
              rxc_scch.num_sup = 0;
              deint_delete_supplemental_task_list();
              #endif
            }
          }
        }
        /* if SECONDARY SERVICE is the only service, reset the primary SO */
        if (scr->num_of_so_recs == 1)
        {
          if (rxc_so.pri != CAI_SO_NULL)
          {
            MSG_MED ("Rel Pri SO", 0, 0, 0);
          }
          rxc_so.pri = CAI_SO_NULL;
        }
      }
      /* store the service option for later reference */
      rxc_srid_ref[scr->so_cfgs[so_index].sr_id].so = scr->so_cfgs[so_index].so;
      rxc_srid_ref[scr->so_cfgs[so_index].sr_id].traffic_type = scr->so_cfgs[so_index].traffic_type;
#endif /* FEATURE_IS2000_REL_A_SVD */
    }
  }
#endif /* FEATURE_IS2000_REL_A */

   /* process SCH */

   /* process SCCH */

  /* Report to SRCH if the SO changed */
  if ((prev_rxc_so_pri != rxc_so.pri) || (prev_rxc_so_sec != rxc_so.sec))
  {
    srch_mux_report_data_call(rxc_is_data_call());
  }
}

#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
/*=========================================================================
FUNCTION rxc_delete_fch_dcch_task_list

DESCRIPTION
    This function deletes the FCH and DCCH tasks from the deinterleaver task 
    RAM and clears data_valid flag for both channels.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void rxc_delete_fch_dcch_task_list (void)
{
  deint_delete_fch_dcch_task_list ();

  rxc_so.for_dcch.data_valid_countdown = 0;
  rxc_so.for_dcch.data_valid = FALSE;

  rxc_so.for_fch.data_valid_countdown = 0;
  rxc_so.for_fch.data_valid = FALSE;

} /* end rxc_delete_fch_dcch_task_list */

/*=========================================================================
FUNCTION FFPC_TRANS_CMD

DESCRIPTION
  This function handles MCTRANS_FFPC_ACTION command fro MC.

DEPENDENCIE
  None.

RETURN VALUE

SIDE EFFECTS
  None.
=========================================================================*/
void
rxc_ffpc_trans_cmd
(
  mctrans_action_packet_type *cmd_ptr
)
{
   mctrans_ffpc_action_type  *ffpc_ptr;

   if (cmd_ptr->action_type != MCTRANS_FFPC_ACTION)
   {
      MSG_ERROR("Bad Act Type for FFPC in MCTRANS", 0, 0, 0);
      return;
   }

   ffpc_ptr = &cmd_ptr->action.ffpc;

   /* Check for change in primary power control channel. 
   */
   if (ffpc_ptr->fpc_pri_chan.included)
   {
     if (ffpc_channel != ffpc_ptr->fpc_pri_chan.value) 
     {
       /* Initialize frame counts for reception watchdog */
       rxc_t5m_bad_frames = 0;
       rxc_n2m_bad_frames = 0;
       rxc_good_frames = 0;
       rxc_fade = FALSE;
       rxc_sent_bad_rpt = FALSE;
       rxc_sent_pwron_rpt = FALSE;      /* used in hard handoff */

       /* Delete the current task list then set up the new task
       ** list with the appropriate channels.
       */
       if ((rxc_so.for_fch.included == TRUE) &&
           (rxc_so.for_dcch.included == TRUE))
       {
         rxc_delete_fch_dcch_task_list ();
         ffpc_channel   = (cai_fpc_pri_chan_type) ffpc_ptr->fpc_pri_chan.value;
         if (ffpc_channel == CAI_FPC_PRI_CHAN_FCH) 
         {
           MSG_HIGH ("Switch FFPC ch to FCH",0,0,0);
           rxc_config_demod ( (deint_rc_type)rxc_so.for_fch.rc );
           rxc_config_demod_ch1 ( (deint_rc_type)rxc_so.for_dcch.rc );
         }
         else
         {
           MSG_HIGH ("Switch FFPC ch to DCCH",0,0,0);
           rxc_config_demod_ch1 ( (deint_rc_type)rxc_so.for_fch.rc );
           rxc_config_demod ( (deint_rc_type)rxc_so.for_dcch.rc );
         }
         rxc_set_dcch_mode  ((deint_rc_type)rxc_so.for_dcch.rc,
                             (ffpc_channel == CAI_FPC_PRI_CHAN_DCCH)
                            ); //lint !e730 Boolean argument to function
         rxc_set_fch_mode   ((deint_rc_type)rxc_so.for_fch.rc,
                             (ffpc_channel == CAI_FPC_PRI_CHAN_FCH)
                            ); //lint !e730 Boolean argument to function
       }
       else if ((rxc_so.for_fch.included == TRUE) &&
                (rxc_so.for_dcch.included == FALSE) &&
                (ffpc_ptr->fpc_pri_chan.value == CAI_FPC_PRI_CHAN_FCH))
       {
         rxc_delete_fch_dcch_task_list ();
         ffpc_channel = CAI_FPC_PRI_CHAN_FCH;
         rxc_config_demod ( (deint_rc_type)rxc_so.for_fch.rc );
         rxc_set_fch_mode ((deint_rc_type)rxc_so.for_fch.rc, TRUE);
         MSG_HIGH ("Switch FFPC ch to FCH,no DCCH",0,0,0);
       }
       else if ((rxc_so.for_fch.included == FALSE) &&
                (rxc_so.for_dcch.included == TRUE) &&
                (ffpc_ptr->fpc_pri_chan.value == CAI_FPC_PRI_CHAN_DCCH))
       {
         rxc_delete_fch_dcch_task_list ();
         ffpc_channel = CAI_FPC_PRI_CHAN_DCCH;
         rxc_config_demod ( (deint_rc_type)rxc_so.for_dcch.rc );
         rxc_set_dcch_mode ((deint_rc_type)rxc_so.for_dcch.rc, TRUE);
         MSG_HIGH ("Switch FFPC ch to DCCH,no FCH",0,0,0);
       }
       else
       {
         MSG_ERROR ("Unxpctd FFPC ch switch %d,%d,%d",
                  ffpc_ptr->fpc_pri_chan.value,
                  rxc_so.for_fch.included,
                  rxc_so.for_dcch.included);
       }
     }
   }

   /* Call ffpc_trans_cmd() to process FFPC transaction.
   */
   ffpc_trans_cmd (cmd_ptr);

} /* end rxc_ffpc_trans_cmd */
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */

#ifdef FEATURE_IS2000_F_SCH
/*========================================================================
FUNCTION RXC_CONFIG_SCH_HW

DESCRIPTION
   This function sets up hardware for F-SCH processing.

DEPENDENCIES
  None

SIDE EFFECTS
  None

RETURN VALUE
  None

========================================================================*/
void rxc_config_sch_hw( mctrans_rxc_f_sch_action_type *fwd_sch_packet )
{
   MSM_LOCK();
   DEMOD_ENABLE_CH2_HW_COMBINING( ); /* SCH Demodulation done by MAC */
   MSM_CLEAR( DEM1X_CHANNEL2_FRAME, PDCCH_1X_SEL );

   rxc_set_mac_rnd
   (
     (deint_rc_type) ((dec_rc_type) fwd_sch_packet->rc),
     (dec_sch_rate_type)fwd_sch_packet->sch_rate,
     (dec_coding_type)fwd_sch_packet->code_type
   );
   dec_dtx_thresh_init
   (
     (dec_rc_type) fwd_sch_packet->rc,
     (dec_coding_type)fwd_sch_packet->code_type,
     (dec_sch_rate_type)fwd_sch_packet->sch_rate
   );

#ifdef FEATURE_MULTIFRAME_INTERLEAVING
#error code not present
#else /* else of FEATURE_MULTIFRAME_INTERLEAVING */
#ifdef FEATURE_IS2000_REL_A //32X
   deint_set_sch_mode
   (
     fwd_sch_packet->num_ltu,
     fwd_sch_packet->ltu_len,
     (dec_mux_pdu_type_type) fwd_sch_packet->mux_pdu_type,
     (deint_rc_type) ((dec_rc_type) fwd_sch_packet->rc),
     (dec_coding_type)fwd_sch_packet->code_type,
     (dec_sch_rate_type)fwd_sch_packet->sch_rate
   );
   /* Initialize variables used in 2nd ATMR ISR */
   deint_atmr_isr2_init();

   /* Set 2nd ATMR ISR to go off at the 4th bin */
   rxc_atmr_set (rxc_atmr_isr2, RXC_ATMR_5MS_FRAME_4,0);
#else
     deint_set_sch_mode
     (
       (deint_rc_type) ((dec_rc_type) fwd_sch_packet->rc),
       (dec_coding_type)fwd_sch_packet->code_type,
       (dec_sch_rate_type)fwd_sch_packet->sch_rate
     );
#endif /* FEATURE_IS2000_REL_A */

#endif /* FEATURE_MULTIFRAME_INTERLEAVING */
   MSM_UNLOCK();
}

/*========================================================================
FUNCTION RXC_FWD_SCH_TRANS_CMD

DESCRIPTION
   The fwd. SCH transaction callback

DEPENDENCIES
  None

SIDE EFFECTS
  None

RETURN VALUE
  None

========================================================================*/
void rxc_fwd_sch_trans_cmd( mctrans_action_packet_type *fwd_sch_packet )
{
  if( fwd_sch_packet != NULL )
  {
    if( fwd_sch_packet->action.rxc_f_sch.start )
    {
      rxc_stored_sch_packet = fwd_sch_packet->action.rxc_f_sch;
      if( (deint_rc_type) rxc_sch.rc != DEINT_RC_INVALID )
      // This is one of the 3 scenarios:
      // 1. Overlapping
      // 2  back-to-back
      // 3. Two assignments with 1 frame gap in between
      {
        /* For overlapping SCH assignments defer updating of
         * struct rxc_sch until the last frame of the previous burst
         * is processed by RLP. RLP will start processing the first
         * frame of the new burst 3 SCH interrupts from now.
         *
         * It is assumed that FCH interrupt occurs before SYS_TIME_INT2.
         * SYS_TIME_INT2 is currently programmed to trigger 5ms after the
         * start of a 20ms frame. Also SYS_TIME_INT2 is assumed to happen
         * before SCH interrupt.
         * When this no longer holds the counter value should be adjusted.
         */

        if ( rxc_sch_shutdown_state == RXC_SCH_SHUTDOWN_NOW )
        {
          // This is scenario 3:
          // There is 1 frame gap between bursts.  In that case, we are shutting
          // down SCH and starting in the same frame.  Unfortunately, ATMR ISR
          // happens before the decoder ISR, so we need to defer the F-SCH start
          // until after the shutdown (inside decoder ISR).
          rxc_sch_update_now = TRUE;
          rxc_sch_update_counter = 0;
          rxc_log_ffpc_sch_data = TRUE;
        }
        else
        {
          if ( rxc_sch_shutdown_state == RXC_SCH_SHUTDOWN_ARMED )
          {
            // This is scenario 2:
            // This happens when stop and start time are the same.  The "stop" was just
            // set up and then the "start" is now schedule in the same ATMR interrupt.
            // In this case, we need to cancel the "stop".
            // We also need to do whatever is needed for scenario 1 below.
            rxc_sch_shutdown_state = RXC_SCH_SHUTDOWN_INACTIVE;
            rxc_log_ffpc_sch_data = TRUE;
          }
          // This is scenario 1: overlapping bursts
          // Start sch_update counter for countdown to update SCH...
          rxc_sch_update_counter = 3;
          //... and config HW for SCH
          rxc_config_sch_hw( &fwd_sch_packet->action.rxc_f_sch );
        }
      }
      else
      {
        /* New SCH assignment - not an overlapping burst. Update SCH info.
          */
        rxc_sch_update_counter = 0;
        rxc_log_ffpc_sch_data = TRUE;
        rxc_update_sch_info( &fwd_sch_packet->action.rxc_f_sch );
        rxc_config_sch_hw( &fwd_sch_packet->action.rxc_f_sch );
      }
    }
    else
    {
//
// RXC_SCH_SHUTDOWN
//
      // Shutdown SCH, but after the current frame is decoded
      rxc_sch_shutdown_state = RXC_SCH_SHUTDOWN_ARMED;
      rxc_log_ffpc_sch_data = FALSE;
      if( fwd_sch_packet->action.rxc_f_sch.for_sch_fer_rep )
      {
         rxc_sch_rep = TRUE;
         rxc_sch_id = fwd_sch_packet->action.rxc_f_sch.sch_id;
      }
      else
      {
         rxc_sch_rep = FALSE;
      }
    }
  }
} // rxc_fwd_sch_trans_cmd
#endif // FEATURE_IS2000_F_SCH

#ifdef FEATURE_IS2000_CHS
/*========================================================================
FUNCTION RXC_CONTROL_HOLD_TRANS_CMD

DESCRIPTION
   The control hold transaction callback.

DEPENDENCIES
  None

SIDE EFFECTS
  None

RETURN VALUE
  None

========================================================================*/
void rxc_control_hold_trans_cmd( mctrans_action_packet_type *chs_packet )
{
  uint8 dem_gating_mask = DEM_FULL_RATE_GATING_MASK; // Default no gating

  if ( chs_packet == NULL )
  {
    MSG_ERROR( "Got NULL CHS pkt", 0, 0, 0 );
    return;
  }
  else
  {
    MSG_HIGH( "Got Control Hold act: rate %d, del %d",
              chs_packet->action.rxc_ctrl_hld_gating.gating_rate,
              chs_packet->action.rxc_ctrl_hld_gating.rev_pwr_control_delay,
              0 );
  }

  // Set reverse power control delay
  MUXMSM_AGC_CTL_MASK_DELAY( 
            chs_packet->action.rxc_ctrl_hld_gating.rev_pwr_control_delay );

  // Now set gating mask
  if ( chs_packet->action.rxc_ctrl_hld_gating.gating_rate == CAI_GATING_RATE_QUARTER )
  {
    dem_gating_mask = DEM_QUARTER_RATE_GATING_MASK;
  }
  else if (chs_packet->action.rxc_ctrl_hld_gating.gating_rate == CAI_GATING_RATE_HALF )
  {
    dem_gating_mask = DEM_HALF_RATE_GATING_MASK;
  }

  MUXMSM_SET_PILOT_GATING_MASK( dem_gating_mask );

  /* 
     Notify RDA of the gating rate.  This affects energy accumulation for the PCGs on
     F-power control subchannel.
  */
  rxcrda_update_gating_rate( chs_packet->action.rxc_ctrl_hld_gating.gating_rate );
} /* rxc_control_hold_trans_cmd */
#endif // FEATURE_IS2000_CHS

/*========================================================================
FUNCTION RXC_HO_TRANS_CMD

DESCRIPTION
  Process Handoff transaction request from MC.

DEPENDENCIES
  None

SIDE EFFECTS
  None

RETURN VALUE
  None

========================================================================*/
void rxc_ho_trans_cmd
(
  mctrans_action_packet_type *cmd_ptr
)
{
#ifdef FEATURE_IS2000_REL_A
  qword current_frame_time;
#endif /* FEATURE_IS2000_REL_A */

  mctrans_rxc_ho_action_type *ho = &cmd_ptr->action.rxc_ho;

#ifdef FEATURE_IS2000_REL_A
  /* find current time */
  ts_get_20ms_frame_offset_time (current_frame_time);
  qw_inc (current_frame_time, 1L);
  qw_equ(rxc_tc_var.p_rev_action_frame, current_frame_time);
#endif /* FEATURE_IS2000_REL_A */

#if defined (FEATURE_IS95B_MAHHO) || defined (FEATURE_GPS_MODE)
  /* rxc_ignore_good_frame is set to TRUE here to ignore any good
   * frames that are received during CFS or before verifying all the
   * handoff failure conditions for RIF HHO.  For RIF HHO, after
   * we've verified that none of the handoff failure conditions are
   * met, this flag will be set to FALSE (by SRCH) so that we can
   * start looking for that one good frame to declare a successful HHO.
   */
  if (ho->hho_ind && ho->rif_ind)
  {
#ifdef FEATURE_HHO_ENHANCEMENTS
    /* Prepare for RIFHHO/CFS/GPS , FALSE inidicates this is not for GPS */
    rxc_prep_for_rifhho_cfs_gps( FALSE );
#else
    rxc_ignore_good_frame = TRUE;
#endif
  }
  if (!ho->rif_ind)
  {
#endif /* FEATURE_IS95B_MAHHO || FEATURE_GPS_MODE */
     /* Soft Handoff or Hard Handoff with No Return-If-Failure */
     if (ho->reset_fpc)
     {
       rxc_pwr.num_frames     = 0;
       rxc_pwr.num_bad_frames = 0;
#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
       rxc_pwr.dcch_num_frames     = 0;
       rxc_pwr.dcch_num_bad_frames = 0;
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */
       rxc_pwr.sch_tot_frames[0] = 0;
       rxc_pwr.sch_bad_frames[0] = 0;
       rxc_pwr.sch_fer_rep[0] = FALSE;
       rxc_pwr.sch_tot_frames[1] = 0;
       rxc_pwr.sch_bad_frames[1] = 0;
       rxc_pwr.sch_fer_rep[1] = FALSE;
     }
#if defined (FEATURE_IS95B_MAHHO) || defined (FEATURE_GPS_MODE)
  }
#endif /* FEATURE_IS95B_MAHHO || FEATURE_GPS_MODE */

   /* program the delay of TX generated MASK_DATA to RX PCG timing */
  if (ho->rev_pwr_cntl_delay.included)
  {
    MUXMSM_AGC_CTL_MASK_DELAY( ho->rev_pwr_cntl_delay.value );
  }

  if (ho->hho_ind)
  {
     /* Since we had a hard handoff, terminate FFPC log */
     ffpc_send_log ();
  }

#ifdef FEATURE_RLP_LOGGING
   /* If enabled, terminate the accumulated frame types log. */
  dsrlp_log_rx_send_frames();
#endif

#ifdef FEATURE_IS2000_REL_A
  if( rxc_tc_var.p_rev_in_use != ho->p_rev_in_use )
  {
    // At the action frame, RXC should use the new P_REV to
    // assemble signaling messages. But now it is not the
    // right time. Considering there may be one, or more
    // decoder interrupts depending on SCH is enabled
    // (maybe some other channels in the future), it is easier
    // to allow RXC to check a flag before building a frame.
    // If the flag is set, the RXC should change the P_REV_IN_USE
    // when the action frame is arrived.

    rxc_tc_var.in_p_rev_transition = TRUE;

    // rxc_tc_var.p_rev_action_frame is already set before RXC calls
    // mctrans_process_action, which in turn calls this function.

    rxc_tc_var.temp_p_rev_in_use = ho->p_rev_in_use;

  }
#endif /* FEATURE_IS2000_REL_A */

}

#ifdef FEATURE_PLT
#error code not present
#endif /* FEATURE_PLT */


/*===========================================================================

FUNCTION RXC_FREE_TC_RESOURCES

DESCRIPTION
  This function resets the MAC Engine and resets the deinteleaver/decoder.
  Also frees up the action timer resource and resets the sch mux stats.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rxc_free_tc_resources(void)
{
  MSM_LOCK();
  // Note: there is a race condition between here and processing of RXC_RELEASE_F
  // command from MC so we need to check the handle here.  This handle is used to
  // indicate if shutdown has been performed.
  if ( rxc_atmr_handle != ATMR_INVALID_HANDLE )
  {
    atmr_set( rxc_atmr_handle,
                       NULL,
                       ATMR_5MS_FRAME(NONE),
                       (atmr_pcg_offset_enum_type)0,
                       (unsigned long)0 ) ;
    rxc_atmr_handle= ATMR_INVALID_HANDLE;

#ifdef FEATURE_IS2000_SCH
    // RXC_SCH_SHUTDOWN
    rxc_sch_shutdown_state = RXC_SCH_SHUTDOWN_INACTIVE;
    rxc_log_ffpc_sch_data = FALSE;
    rxc_sch_rep = FALSE;
    rxc_sch_id = 0;
#endif /* FEATURE_IS2000_SCH */

#ifdef FEATURE_FACTORY_TESTMODE
    if(ftm_mode == FTM_MODE)
    {
      enc_set_frame_off(0x00, 0x00);
    }
#endif /* FEATURE_FACTORY_TESTMODE */

#if defined( FEATURE_DS_RLP3 ) && defined( FEATURE_RLP_LOGGING )
    dsrlp_log_rlp_statistics( DSRLP_FIXED_SR_ID_INDEX, TRUE );
#endif /* defined( FEATURE_DS_RLP3 ) && defined( FEATURE_RLP_LOGGING ) */

    rxc_voc_config_done = FALSE;
    rxc_sup_dec_int_enable = FALSE;
    rxc_turbo_dec_enabled = FALSE;
#if  defined (FEATURE_IS95B_MDR)
    rxc_scch.num_sup = 0x00;
#endif
    DEMOD_DISABLE_CH1_HW_COMBINING( );
    DEMOD_DISABLE_CH2_HW_COMBINING( );
    
    deint_delete_supplemental_task_list();
#ifdef FEATURE_DS_IS2000
    rxc_cleanup_sch_mux();
#endif
  }

  MSM_UNLOCK();

  ffpc_disable ();

#ifndef FEATURE_IS2000_REL_A

  /* flush out leftover log records. Note that this is done inspite of the
  ** fact that FORWARD FRAME type and FFPC logging may be turned off at this
  ** time. If the logs are not flushed, the logging info from this call will
  ** appear in the logs of the subsequent call.
  */
  rxc_frame_types_send_log();

#else
  rxc_fdch_frame_info_send_log();
#endif /* FEATURE_IS2000_REL_A */

  ffpc_send_log();
#ifdef FEATURE_LOG_PC_INFO_2
  ffpc_info_send_log();
#endif /* FEATURE_LOG_PC_INFO_2 */

  /* If enabled, send accumulated RDA log. */
  rxc_rda_frame_info_send_log();

  /* Revert gating mask to no gating 
  */
  #ifdef FEATURE_IS2000_CHS
  MUXMSM_SET_PILOT_GATING_MASK( DEM_FULL_RATE_GATING_MASK );
  #endif /*FEATURE_IS2000_CHS */

  #ifdef FEATURE_IS2000_P2
  rxcrda_update_gating_rate( CAI_GATING_RATE_NONE );
  #endif /* FEATURE_IS2000_P2 */

#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)  
  /* Delete the current task list, reset fundicated channel deinterleaver 
  ** state.
  */
  rxc_delete_fch_dcch_task_list ();
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */
}

#ifdef FEATURE_PLT
#error code not present
#endif   /* FEATURE_PLT */

#ifdef FEATURE_FACTORY_TESTMODE
/*===========================================================================

FUNCTION RXC_FCH_LOOPBACK_QUERY

DESCRIPTION
  This procedure returns whether FCH loopback is enabled for Factory
  Test Mode.

DEPENDENCIES
  None.

RETURN VALUE
  Boolean, true if loobpack is on.

SIDE EFFECTS
  None.

===========================================================================*/
boolean rxc_fch_loopback_query( void )
{
  return rxc_tc_var.fch_loopback;
}

#ifdef FEATURE_IS2000_P2
/* Support FEATURE_FTM_DCCH */
/*===========================================================================

FUNCTION RXC_DCCH_LOOPBACK_QUERY

DESCRIPTION
  This procedure returns whether DCCH loopback is enabled for Factory
  Test Mode.

DEPENDENCIES
  None.

RETURN VALUE
  Boolean, true if loobpack is on.

SIDE EFFECTS
  None.

===========================================================================*/
boolean rxc_dcch_loopback_query( void )
{
  return rxc_tc_var.dcch_loopback;
}
/* End of FEATURE_FTM_DCCH */
#endif

/*===========================================================================

FUNCTION RXC_SCH_LOOPBACK_QUERY

DESCRIPTION
  This procedure returns whether SCH loopback is enabled for Factory
  Test Mode.

DEPENDENCIES
  None.

RETURN VALUE
  Boolean, true if loobpack is on.

SIDE EFFECTS
  None.

===========================================================================*/
boolean rxc_sch_loopback_query( void )
{
  return rxc_tc_var.sch_loopback;
}

/*===========================================================================

FUNCTION RXC_SET_FCH_LOOPBACK

DESCRIPTION
  This procedure sets FCH loopback for Factory Test Mode.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Loopback mode is set in rxctraffic.

===========================================================================*/
void rxc_set_fch_loopback( boolean set )
{
  rxc_tc_var.fch_loopback = set;
}

#ifdef FEATURE_IS2000_P2
/* Support FEATURE_FTM_DCCH */
/*===========================================================================

FUNCTION RXC_SET_DCCH_LOOPBACK

DESCRIPTION
  This procedure sets DCCH loopback for Factory Test Mode.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Loopback mode is set in rxctraffic.

===========================================================================*/
void rxc_set_dcch_loopback( boolean set )
{
  rxc_tc_var.dcch_loopback = set;
}
/* End of FEATURE_FTM_DCCH */
#endif

/*===========================================================================

FUNCTION RXC_SET_SCH_LOOPBACK

DESCRIPTION
  This procedure sets SCH loopback for Factory Test Mode.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Loopback mode is set in rxctraffic.

===========================================================================*/
void rxc_set_sch_loopback( boolean set )
{
  rxc_tc_var.sch_loopback = set;
}
#endif // FEATURE_FACTORY_TESTMODE

/*========================================================================
FUNCTION RXC_SCR_TRANS_CMD

DESCRIPTION
  Process SCR transaction request from MC.

DEPENDENCIES
  None

SIDE EFFECTS
  None

RETURN VALUE
  None

========================================================================*/
void rxc_scr_trans_cmd
(
  mctrans_action_packet_type *cmd_ptr
)
{
  mctrans_rxc_scr_action_type *scr = &cmd_ptr->action.rxc_scr;
  boolean prev_rxc_so_pri = rxc_so.pri;
  boolean prev_rxc_so_sec = rxc_so.sec;
#ifdef FEATURE_IS2000_REL_A
  word so_index;
#endif /* FEATURE_IS2000_REL_A */

#ifdef FEATURE_IS2000_P2
  if ( !scr->dcch.included && !scr->fch.included )
  {  /* Neither is included, turn it off */
    rxc_free_tc_resources();
  }

  if (scr->dcch.included)
  {
    // If DCCH is newly added or changing RC
    if ( !rxc_so.for_dcch.included ||
         scr->dcch.rc != rxc_so.for_dcch.rc )
    {
      rxc_so.for_dcch.included = TRUE;
      rxc_so.for_dcch.rc = scr->dcch.rc;
#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
      if (ffpc_channel == CAI_FPC_PRI_CHAN_DCCH)
      {
        rxc_config_demod ( (deint_rc_type)rxc_so.for_dcch.rc );
        rxc_set_dcch_mode( (deint_rc_type)rxc_so.for_dcch.rc, TRUE);
      }
      else
      {
        rxc_config_demod_ch1 ( (deint_rc_type)rxc_so.for_dcch.rc );
        rxc_set_dcch_mode( (deint_rc_type)rxc_so.for_dcch.rc, FALSE);
      }
#else
      rxc_config_demod ( (deint_rc_type)rxc_so.for_dcch.rc );
      deint_set_fch_mode ( (deint_rc_type)rxc_so.for_dcch.rc );

#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */
      /* For SRCH logging */
      // Note: we know that FCH is not included since DCCH included
      // We assume only 1 of FCH/DCCH supported here
      srch_mux_update_rc (DEINT_RC_INVALID, 
                          (deint_rc_type)rxc_sch.rc, 
                          DEINT_RC_INVALID,
                          (deint_rc_type)rxc_so.for_dcch.rc);
    }
    rxc_so.rate_set = fwd_rc_to_rate_set[rxc_so.for_dcch.rc];
    rxc_so.high_rc = TRUE;
  }
  else
  {
    rxc_so.for_dcch.included = FALSE;
#ifdef FEATURE_IS2000_REL_A_SVD
    /* Disables the processing of the DCCH.
    */
    rxc_so.for_dcch.data_valid = FALSE;
    rxc_so.for_dcch.data_valid_countdown = 0;
#endif /* FEATURE_IS2000_REL_A_SVD */
  }
#endif // FEATURE_IS2000_P2
#ifdef FEATURE_IS2000_REL_A
  /* copy the SCH MUX OPTION from the upper layer to the MUX for FDCH logging*/
  if (scr->for_sch0_mux_option.included)
  {
    rxc_so.for_sch_mux_option = scr->for_sch0_mux_option.value;
  }
  else
  {
    rxc_so.for_sch_mux_option = 0;
  }
#endif /* FEATURE_IS2000_REL_A */
  if (scr->fch.included)
  {
    // If FCH is newly added or changing RC
    if (
        #ifdef FEATURE_IS2000_P2
        !rxc_so.for_fch.included ||
        #endif
        scr->fch.rc != rxc_so.for_fch.rc )
    {
      #ifdef FEATURE_IS2000_P2
      rxc_so.for_fch.included = TRUE;
#endif
      rxc_so.for_fch.rc = scr->fch.rc;
#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
      if (ffpc_channel == CAI_FPC_PRI_CHAN_FCH)
      {
        rxc_config_demod ( (deint_rc_type)rxc_so.for_fch.rc );
        rxc_set_fch_mode( (deint_rc_type)rxc_so.for_fch.rc, TRUE);
      }
      else
      {
        rxc_config_demod_ch1 ( (deint_rc_type)rxc_so.for_fch.rc );
        rxc_set_fch_mode( (deint_rc_type)rxc_so.for_fch.rc, FALSE);
      }
#else
      rxc_config_demod ( (deint_rc_type)scr->fch.rc );
      deint_set_fch_mode ( (deint_rc_type)scr->fch.rc );
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */

      /* For SRCH logging */
      // Note: we know that DCCH is not included since FCH included
      // We assume only 1 of FCH/DCCH supported here
      srch_mux_update_rc ((deint_rc_type)scr->fch.rc, 
                          (deint_rc_type)rxc_sch.rc, 
                          DEINT_RC_INVALID,
                          DEINT_RC_INVALID);
    }
    rxc_so.rate_set = fwd_rc_to_rate_set[scr->fch.rc];
    rxc_so.high_rc = rxc_so.for_fch.rc > CAI_RC_2;
  }
#ifdef FEATURE_IS2000_P2
  else
  {
    rxc_so.for_fch.included = FALSE;
#ifdef FEATURE_IS2000_REL_A_SVD
    /* Disables the processing of the FCH.
    */
    rxc_so.for_fch.data_valid = FALSE;
    rxc_so.for_fch.data_valid_countdown = 0;
#endif /* FEATURE_IS2000_REL_A_SVD */

  }
#endif /* FEATURE_IS2000_P2 */

#ifndef FEATURE_IS2000_REL_A
  if ((scr->pri_so.included) && (scr->pri_so.value != rxc_so.pri))
  {
     rxc_so.pri = scr->pri_so.value;

     /* For voice service options, indicate that voice option is
        not configured */
     if ( scr->pri_so.value == CAI_SO_VOICE_IS96A ||
#ifndef FEATURE_NO_13K_VOICE_SUPPORT
          scr->pri_so.value == CAI_SO_VOICE_13K  ||
#endif          
          scr->pri_so.value == CAI_SO_VOICE_EVRC
#ifdef FEATURE_SO_VOICE_SMV
          || scr->pri_so.value == CAI_SO_VOICE_SMV
#endif
#ifdef FEATURE_VOC_4GV
          || scr->pri_so.value == CAI_SO_VOICE_4GV_NB
          || scr->pri_so.value == CAI_SO_VOICE_4GV_WB
#endif /* FEATURE_VOC_4GV */
          )
     {
        rxc_voc_config_done = FALSE;
     }
     else if (scr->pri_so.value == CAI_SO_NULL)
     {
        rxc_voc_config_done = FALSE;
        if (rxc_sup_dec_int_enable)
        {
           /* This was a MDR/HDR call. So delete SCCH/SCH task from
              the deinterleaver Task RAM. */
#if  defined (FEATURE_IS95B_MDR)
           rxc_sup_dec_int_enable = FALSE;
           rxc_scch.num_sup = 0;
           deint_delete_supplemental_task_list();
#endif
        }
     }
  }
#else
  /* ckech if there is new service option record */
  if (scr->num_of_so_recs > 0)
  {
    /* search through all the new service option records */
    for (so_index = 0; so_index < scr->num_of_so_recs; so_index++)
    {
      /* check if this the primary service option */
      if (scr->so_cfgs[so_index].traffic_type == CAI_PRI_TRAF)
      {
        if (scr->so_cfgs[so_index].so != rxc_so.pri)
        {
          rxc_so.pri = scr->so_cfgs[so_index].so;
          /* For voice service options, indicate that voice option is
          not configured */
          if (scr->so_cfgs[so_index].so == CAI_SO_VOICE_IS96A ||
              scr->so_cfgs[so_index].so == CAI_SO_VOICE_EVRC  ||
              scr->so_cfgs[so_index].so == CAI_SO_VOICE_13K  
              #ifdef FEATURE_SO_VOICE_SMV
              || scr->so_cfgs[so_index].so == CAI_SO_VOICE_SMV                
              #endif              
#ifdef FEATURE_VOC_4GV
              || scr->so_cfgs[so_index].so == CAI_SO_VOICE_4GV_NB
              || scr->so_cfgs[so_index].so == CAI_SO_VOICE_4GV_WB
#endif /* FEATURE_VOC_4GV */
              )
          {
            rxc_voc_config_done = FALSE;
          }
          else if (scr->so_cfgs[so_index].so == CAI_SO_NULL)
          {
            rxc_voc_config_done = FALSE;
            if (rxc_sup_dec_int_enable)
            {
              /* This was a MDR/HDR call. So delete SCCH/SCH task from
              the deinterleaver Task RAM. */
              #if  defined (FEATURE_IS95B_MDR)
              rxc_sup_dec_int_enable = FALSE;
              rxc_scch.num_sup = 0;
              deint_delete_supplemental_task_list();
              #endif
            }
          }
        }
#ifdef FEATURE_IS2000_REL_A_SVD
        /* if PRIMARY SERVICE is the only service, reset the secondary SO */
        if (scr->num_of_so_recs == 1)
        {
          if (rxc_so.sec != CAI_SO_NULL)
          {
            MSG_MED ("Rel Sec SO", 0, 0, 0);
          }
          rxc_so.sec = CAI_SO_NULL;
        }
#endif /* end of FEATURE_IS2000_REL_A_SVD */
      }
#ifdef FEATURE_IS2000_REL_A_SVD
      /* check if this the secondary service option */
      if (scr->so_cfgs[so_index].traffic_type == CAI_SEC_TRAF)
      {
        if (scr->so_cfgs[so_index].so != rxc_so.sec)
        {
          rxc_so.sec = scr->so_cfgs[so_index].so;
          /* For voice service options, indicate that voice option is
          not configured */
          if (scr->so_cfgs[so_index].so == CAI_SO_VOICE_IS96A ||
              scr->so_cfgs[so_index].so == CAI_SO_VOICE_EVRC  ||
              scr->so_cfgs[so_index].so == CAI_SO_VOICE_13K  
              #ifdef FEATURE_SO_VOICE_SMV
              || scr->so_cfgs[so_index].so == CAI_SO_VOICE_SMV                
              #endif              
#ifdef FEATURE_VOC_4GV
              || scr->so_cfgs[so_index].so == CAI_SO_VOICE_4GV_NB
              || scr->so_cfgs[so_index].so == CAI_SO_VOICE_4GV_WB
#endif /* FEATURE_VOC_4GV */
              )
          {
            rxc_voc_config_done = FALSE;
          }
          else if (scr->so_cfgs[so_index].so == CAI_SO_NULL)
          {
            rxc_voc_config_done = FALSE;
            if (rxc_sup_dec_int_enable)
            {
              /* This was a MDR/HDR call. So delete SCCH/SCH task from
              the deinterleaver Task RAM. */
              #if  defined (FEATURE_IS95B_MDR)
              rxc_sup_dec_int_enable = FALSE;
              rxc_scch.num_sup = 0;
              deint_delete_supplemental_task_list();
              #endif
            }
          }
        }
        /* if SECONDARY SERVICE is the only service, reset the primary SO */
        if (scr->num_of_so_recs == 1)
        {
          if (rxc_so.pri != CAI_SO_NULL)
          {
            MSG_MED ("Rel Pri SO", 0, 0, 0);
          }
          rxc_so.pri = CAI_SO_NULL;
        }
      }
      /* store the service option for later reference */
      rxc_srid_ref[scr->so_cfgs[so_index].sr_id].so = scr->so_cfgs[so_index].so;
      rxc_srid_ref[scr->so_cfgs[so_index].sr_id].traffic_type = scr->so_cfgs[so_index].traffic_type;
#endif /* FEATURE_IS2000_REL_A_SVD */
    } /* for */
  }

#endif /* FEATURE_IS2000_REL_A */

  /* program the delay of TX generated MASK_DATA to RX PCG timing */
  if (scr->rev_pwr_cntl_delay.included)
  {
    MUXMSM_AGC_CTL_MASK_DELAY( scr->rev_pwr_cntl_delay.value );
  }

  /* process secondary service option */

  /* process SCH */

  /* process SCCH */

  /* Report to SRCH if the SO changed */
  if ((prev_rxc_so_pri != rxc_so.pri) || (prev_rxc_so_sec != rxc_so.sec))
  {
    srch_mux_report_data_call(rxc_is_data_call());
  }
}

#ifdef FEATURE_IS2000_SCH
/*===========================================================================

FUNCTION RXC_SHUTDOWN_SCH

DESCRIPTION
  This procedure shuts down F-SCH.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rxc_shutdown_sch( void )
{
  // Shutdown SCH processing
  MSG_HIGH("SCH shutdown", 0, 0, 0);
  MSM_LOCK();
  DEMOD_DISABLE_CH2_HW_COMBINING( );
  rxc_sup_dec_int_enable = FALSE;
  rxc_turbo_dec_enabled = FALSE;
  deint_delete_supplemental_task_list();

#ifdef FEATURE_IS2000_REL_A
   /* De-install 2nd ISR */
    rxc_atmr_set (NULL, RXC_ATMR_5MS_FRAME_4,0);
#endif /* FEATURE_IS2000_REL_A */

  if (!rxc_sch_update_now) {
     /* disable F-SCH Outer Loop in SW */
     ffpc_disable_chan_olpc (FFPC_SCH);
  } 
  else {
    /* 
     * OLPC was updated by the previous ATMR_ISR (action:MCTRANS_FFPC_ACTION)
     * if a sch traffic is comming at 1 grame gap distance
     * sch channel olpc should not be touched at SCH_ISR in this 1frame gap scenario
     * otherwise the olpc would be disable during the next frame
     */
     MSG_HIGH("SCH 1-frame-gap; untouch the OLPC", 0, 0, 0);
  }

  /* Initialize SCH0 RC */
  rxc_sch.rc = (dec_rc_type)DEINT_RC_INVALID;
  MSM_UNLOCK()

  #ifdef FEATURE_IS2000_P2
  /* For SRCH logging */
  srch_mux_update_rc ( rxc_so.for_fch.included?
                       (deint_rc_type)rxc_so.for_fch.rc:DEINT_RC_INVALID,
                       (deint_rc_type)rxc_sch.rc,
                       DEINT_RC_INVALID,
                       rxc_so.for_dcch.included?
                       (deint_rc_type)rxc_so.for_dcch.rc:DEINT_RC_INVALID );
  #else
  srch_mux_update_rc ( (deint_rc_type)rxc_so.for_fch.rc,
                       (deint_rc_type)rxc_sch.rc,
                       DEINT_RC_INVALID,
                       DEINT_RC_INVALID );
  #endif // FEATURE_IS2000_P2
}
#endif // FEATURE_IS2000_SCH

#ifdef FEATURE_GPS_MODE
#error code not present
#endif /* FEATURE_GPS_MODE */

void rxc_atmr_set
(
  void                   (*callback_func_ptr)(void),
  rxc_atmr_5ms_enum_type which_5ms,
  uint16                 which_pcg_into_slot /* PCG 0, 1, 2, 3 */
)
{
  if (rxc_atmr_handle == ATMR_INVALID_HANDLE)
  {
     return;
  }

  // Protect critical section
  INTLOCK();

  if(callback_func_ptr == NULL)
  {
    rxc_atmr_tab[(4*which_5ms) + which_pcg_into_slot]=NULL;
  }
  else
  {
    rxc_atmr_tab[(4*which_5ms) + which_pcg_into_slot] = callback_func_ptr;
  }
  atmr_set( rxc_atmr_handle,
            rxc_atmr_dispatcher,
            (atmr_5ms_mask_enum_type)(which_5ms + 1),
             (atmr_pcg_offset_enum_type) which_pcg_into_slot,
             0 ) ;

  // Unprotect critical section
  INTFREE();
}

/*============================================================================
FUNCTION RXC_ATMR_DISPATCHER

DESCRIPTION
  This function invokes the callback function that have been registered
  based on the phase(0,1,2,3) of the system timeline interrupt.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void rxc_atmr_dispatcher( void)
{
  uint16 phase, index, old_phase = 0xffff; // This guarantees phase won't equal init_phase the first time
  boolean exit_condition = FALSE;

  do
  {
   /* Since we are using TIMe_INT2, the bits 4:7 give us the
      system 5ms count in the offset 80ms frame at the time of
      interrupt strobe 2 */
    phase = ATMR_GET_INT2_STATUS;
    if (phase != 0)
    {
      /* need to clear the bit */
      ATMR_CLEAR_INT2_STATUS (phase);
      /* convert the phase into index */
      for (index = 0; index < 16; index++)
      {
        if ((phase) & ((0x1 << index)))
        {
          phase = index;
          break;
        }
      }
    }
    else
    {
      phase = old_phase;
    }

    // If this is first in the loop, or if the phase has changed, execute the registered ATMR
    // function; otherwise, we are done.
    if (phase != old_phase)
    {
      /*lint -save -e796 -e797 Conceivable access or creation of out-of-bounds pointer -- probably OK*/      
      /* ANDing phase with 0x000f prevents any possibility of the value of
         phase from exceeding the boundary of the array */
      if (rxc_atmr_tab[(phase & 0x000F)] )
      {
        old_phase = phase;
        rxc_atmr_tab[(phase & 0x000F)]();
      }
      /*lint -restore */
    }
    else
    {
      exit_condition = TRUE;
    }
  } while (!exit_condition);
}

/*=============================================================================
FUNCTION RXC_ATMR_ISR

DESCRIPTION
  This is the Action Timer System Interrupt. In this, we check the Action Item
  Queue to service the commands that need to be serviced before the next frame
  time.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void rxc_atmr_isr(void)
{
  /* Check the Action Item Queue to find out if there
     are any Commands pending that need to be serviced
     before the next frame. */
  qword current_frame_time;
  INTLOCK();

  /* Check if there any read parameters that need to be programmed */
  deint_process_sequencing_cmd();

#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
  /* Since there are 2 frames lag time between when the DCCH/FCH is set up and 
  ** when data is valid, need to count down these 2 frames.  Without this
  ** mechanism, MUX may deliver stale data to its clients.  The time line is
  ** shown below:
  **    Frame 0:  Start Deint programming by calling deint_set_xxch_mode()
  **    Frame 1:  In ATMR ISR, decrement countdown counter
  **    Frame 2:  In rxc_tc_isr, if count down counter is 1, decrement it
  **              and set data_valid flag.
  ** The count down is set to 2 when the channel is set up.
  ** In the next frame, it is decremented in the ATMR ISR.
  ** In the frame after that, it is decremented in the TC ISR and data valid
  ** is set to TRUE only in the TC ISR.  
  ** Reason for this scheme:  in TCI state, there is no TC ISR so we must use
  ** the ATMR ISR to count down.
  ** However, in the 2nd frame, when data is valid, we must make sure data valid
  ** flag is set to TRUE to read it at the TC ISR interrupt.  ATMR interrupt happens
  ** in the next bin, when we already serviced the TC ISR.
  */
  if (
      (rxc_so.for_dcch.included) && 
      (!(rxc_so.for_dcch.data_valid)) &&
      (rxc_so.for_dcch.data_valid_countdown > 0)
     )
  {
    rxc_so.for_dcch.data_valid_countdown--;
  }
  if (
      (rxc_so.for_fch.included) && 
      (!(rxc_so.for_fch.data_valid)) &&
      (rxc_so.for_fch.data_valid_countdown > 0)
     )
  {
    rxc_so.for_fch.data_valid_countdown--;
  }
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */

#ifdef FEATURE_IS2000_SCH
  // If SCH shutdown is pending, need to update shutdown state
  if ( rxc_sch_shutdown_state == RXC_SCH_SHUTDOWN_ARMED )
  {
    rxc_sch_shutdown_state = RXC_SCH_SHUTDOWN_NOW;
  }
#endif /* FEATURE_IS2000_SCH */

  /* Need to read the 64 bit system timestamp in units of 20
  ** millisecond frame time.  Every frame, give this time to 
  ** the transaction engine.  If there is an action for that
  ** frame, the transaction engine will set RXC_ACTION_TIME_SIG
  ** within the interrupt context.
  */
  ts_get_20ms_frame_offset_time(current_frame_time);

  /* Use function pointer to call the transaction engine.
  ** This function must execute or RXC will not process any
  ** action.
  */
  if (rxc_action_time_notifier_func_ptr != NULL)
  {
    /* Reset the signal. */
    (void) rex_clr_sigs (&rx_tcb, RXC_ACTION_TIME_SIG );
    (*rxc_action_time_notifier_func_ptr)(MCTRANS_SERVER_RXC, current_frame_time);
  }

  /* So here, check for the signal.  If it is set, call the transaction
  ** engine dispatcher to handle the action.  Action handlers have been
  ** registered to the transaction engine.
  **
  ** Note that there is a plan to move the action time handling to task
  ** context in the future.
  */
  if ((rex_get_sigs(&rx_tcb)) & RXC_ACTION_TIME_SIG)
  {
    mctrans_process_transactions(MCTRANS_SERVER_RXC);
    /* Reset the signal. */
    (void) rex_clr_sigs (&rx_tcb, RXC_ACTION_TIME_SIG );
  }

  INTFREE();
}

/*=============================================================================
FUNCTION RXC_SET_TX_IN_TRAFFIC_ON

DESCRIPTION
  This function is called to set/reset the rxc_tx_in_traffic_on flag if TX is turned 
  on/off

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void rxc_set_tx_in_traffic_on(boolean tx_status)
{
  rxc_tx_in_traffic_on = tx_status;
}

#ifdef FEATURE_IS2000_REL_A
/*=============================================================================
FUNCTION rxc_atmr_isr2

DESCRIPTION
  This function is invoked once a frame, at the 4th bin.  This function calls
  Deint function deint_atmr_isr2() to support changes for 6100 release A Turbo
  SCH.  These changes are the result of moving the Turbo SCH dummy task to the
  3rd bin to give the Decoder more time to decode 32X turbo data rate. 
  
DEPENDENCIES
  rxc_atmr_* functions were called in the correct order to handle ATMR ISRs.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void rxc_atmr_isr2(void)
{
  deint_atmr_isr2();
}
#endif /* FEATURE_IS2000_REL_A */

#ifdef FEATURE_IS2000_REL_A_CC
/*===========================================================================

FUNCTION RXC_CONFIG_MAC_FOR_CH1

DESCRIPTION
  This routine configures the MAC (CH1).

DEPENDENCIES
  None.

RETURN VALUE
  boolean indicating success.

SIDE EFFECTS
  None.

===========================================================================*/
void rxc_config_mac_for_ch1 (cai_code_rate_type code_rate)
{
  MSG_HIGH ("Config MAC Engine for Ch1",0,0,0);
  
  MSM_LOCK();
  DEMOD_ENABLE_CH1_HW_COMBINING();

  if( code_rate == CAI_CODE_RATE_QTR )
  {
    DEMOD_SET_MAC_RND_32_CHIPS_CH1 (DMOD_COMB_RND_CONVOLUTIONAL);
    /* since we still use the old Tiger values for the COMB_RND3, so we need to derive
    the new beta value for it (0x5B / 4). In the future, we will use the corrected 
    puma/cougar values */ 
    DEMOD_SET_SW_BETA_CH1 (0x5B);
    DEMOD_SET_CH1_SPR (SPR_32_CHIP_V);
  }
  else
  {
    DEMOD_SET_MAC_RND_128_64_CHIPS_CH1 (DMOD_COMB_RND_CONVOLUTIONAL);
    /* since we still use the old Tiger values for the COMB_RND3, so we need to derive
    the new beta value for it (0x80 / 4). In the future, we will use the corrected 
    puma/cougar values */ 
    DEMOD_SET_SW_BETA_CH1 (0x80);
    DEMOD_SET_CH1_SPR (SPR_64_CHIP_V);
  }

  MSM_UNLOCK();
}
#endif /* FEATURE_IS2000_REL_A_CC */

#ifdef FEATURE_HHO_ENHANCEMENTS
/*=========================================================================
FUNCTION RXC_PREP_FOR_RIFHHO_CFS_GPS

DESCRIPTION
  This function prepares for RIFHHO, CFS and GPS operations

DEPENDENCIE
  None.

RETURN VALUE

SIDE EFFECTS
  None.
=========================================================================*/
void rxc_prep_for_rifhho_cfs_gps
(
  boolean is_gps
)
{
  MSG_HIGH( "Prep for RIFHHO_CFS_GPS", 0, 0, 0 );

  /* -----------------------------------------------------
  ** Perform Return-If-Fail Hard Handoff or CFS processing
  ** ----------------------------------------------------- */
  rxc_rifhho_cfs_gps_in_progress = TRUE;
  rxc_lost_frames = 0;
  rxc_use_stored_eibs = FALSE;
#ifdef FEATURE_IS2000_REL_A
  rxc_use_stored_qibs = FALSE;
  rxc_use_stored_sch_eibs = FALSE;
#endif /* FEATURE_IS2000_REL_A */

  /* Indicate that a bad frames report has been sent to the Main
     Control task in order to prevent RXC from sending one */
  rxc_sent_bad_rpt = TRUE;

  /* To avoid getting valid signaling after tuning away,
  ** rxc_ignore_good_frame must be set to TRUE before we tune.
  ** For RIF HHO and CFS, RXC_TUNE_RF_F is sent before tuning
  ** to set rxc_ignore_good_frame, and then RXC_RIFHHO_CFS_GPS_F
  ** is sent after we tune.  For GPS we send RXC_RIFHHO_CFS_GPS_F
  ** before we tune and do not send the RXC_TUNE_RF_F at all, so
  ** we need to set rxc_ignore_good_frame to TRUE here for GPS.
  */
  rxc_ignore_good_frame = TRUE;
  rxc_rif_hho_timeout_frames = 0;

#ifdef FEATURE_GPSONE_OVERRIDE_RDA
#error code not present
#endif

}

/*=========================================================================
FUNCTION RXC_CFS_GPS_PREP_CMD

DESCRIPTION
  This function handles MCTRANS_CFS_GPS_PREP__ACTION command fro MC.

DEPENDENCIE
  None.

RETURN VALUE

SIDE EFFECTS
  None.
=========================================================================*/
void
rxc_cfs_gps_prep_cmd
(
  mctrans_action_packet_type *cmd_ptr
)
{
  rxc_prep_for_rifhho_cfs_gps(cmd_ptr->action.cfs_gps_prep.is_gps);
}

/*=========================================================================
FUNCTION RXC_HHO_BEGIN_CMD

DESCRIPTION
  This function handles MCTRANS_HHO_BEGIN_ACTION command fro MC.

DEPENDENCIE
  None.

RETURN VALUE

SIDE EFFECTS
  None.
=========================================================================*/
void
rxc_hho_begin_cmd
(
  mctrans_action_packet_type *cmd_ptr
)
{
#ifdef FEATURE_IS95B_MAHHO
  /* pointer to report to send to Main Control task */
  mccrx_rpt_type *rpt_ptr;

  if (cmd_ptr->action.rxc_hho_begin.rif_ind == TRUE)
  {
    /* Convert the 80ms timeout to a 20ms frame count */
    rxc_rif_hho_timeout_frames = cmd_ptr->action.rxc_hho_begin.hho_timeout * 4;

    /* We started counting lost frame since receiving the HO action, which may
    ** preceed the HHO begin action.  So the lost frame count may be non zero
    ** at this time.  Since we just got the RIF timer, reset the lost frame 
    ** count so the RIF timer will not expire earlier than expected.
    */
    rxc_lost_frames = 0;

    /* We use zero as an indication that there is no timeout.
     * If the base station was insane enough to set the timeout
     * to zero, send the timeout report immediately.
     */
    if (rxc_rif_hho_timeout_frames == 0)
    {
      /* send report of handoff timeout to Main Control task */
      if ( (rpt_ptr = (mccrx_rpt_type *)q_get(&mcc_rx_free_q) ) != NULL )
      {
        rpt_ptr->hdr.rpt = RXC_HHO_TIMEOUT_R;
        rxc_mc_rpt(rpt_ptr );
      }
      else
      {
        ERR( "0 bufs on mcc_rx_free_q", 0, 0, 0 );
        rxc_rif_hho_timeout_frames++; /* Try again next frame */
      }
    }
  }
  else
#endif /* FEATURE_IS95B_MAHHO */
  {
    rxc_rif_hho_timeout_frames = 0;
  }

  /* -------------------------------------------
   ** Perform CDMA to CDMA Hard Handoff processing
   ** ------------------------------------------- */
  rxc_hard_handoff_in_progress = TRUE;


  /* Indicate that a bad frames report has been sent to the Main
     Control task in order to prevent RXC from sending one */
  rxc_sent_bad_rpt = TRUE;

#ifdef FEATURE_IS95B_MAHHO
  if ( ! rxc_rifhho_cfs_gps_in_progress )
  {
    /* Reset the fade timer */
    rxc_good_frames = 0;
    rxc_t5m_bad_frames = 0;
  }
#else
  rxc_good_frames = 0;
  rxc_t5m_bad_frames = 0;
#endif /* FEATURE_IS95B_MAHHO */

  /* Indicate that fade processing is in progress */
  rxc_fade = TRUE;

  /* Wait 1 frame before fade timer processing */
  rxc_ho_cnt = 1;

  /* Turn off power control reporting */
  rxc_pwr.threshold_on = FALSE;
  rxc_pwr.periodic_on  = FALSE;

  /* Indicate that RXC_PWR_ON_R hasn't been sent */
  rxc_sent_pwron_rpt = FALSE;

  /* Indicate that vocoder needs to be reinitialized */
  rxc_voc_init_done   = FALSE;
  rxc_voc_config_done = FALSE;
}

/*=========================================================================
FUNCTION RXC_RETURN_CMD

DESCRIPTION
  This function handles MCTRANS_RETURN_ACTION command fro MC.

DEPENDENCIE
  None.

RETURN VALUE

SIDE EFFECTS
  None.
=========================================================================*/
void rxc_return_cmd
(
  mctrans_action_packet_type *unused_cmd_ptr
)
{
#if defined (FEATURE_IS95B_MAHHO) || defined (FEATURE_GPS_MODE) 
  MSG_HIGH( "Return action_F,LOST_FRAME %d,rxc_fade: %d",
             rxc_lost_frames, rxc_fade, 0 );

  /*
   * Mark the end of the Return-If-Fail HHO or CFS.  This will
   * re-enable the fade timer and resume counting TOT_FRAMES and
   * BAD_FRAMES.  Note that fade_check still needs to be TRUE at
   * this point
   */
  rxc_rifhho_cfs_gps_in_progress = FALSE;

  /* When returning to the SF, set rxc_ignore_good_frame to FALSE
   * so that we can immediately start counting good frames to restart
   * the transmitter
   */
  rxc_ignore_good_frame = FALSE;

  /* Remember that we do not need to send a TX off rpt to MC */
  rxc_sent_bad_rpt = TRUE;
  /* Indicate that fade processing is in progress */
  rxc_fade = TRUE;

#ifdef FEATURE_GPSONE_OVERRIDE_RDA
#error code not present
#endif /* FEATURE_GPSONE_OVERRIDE_RDA */
  /*
  ** Need to wait N3M (2) consecutive good frames before re-enabling
  ** the transmitter if the interval between the time that the mobile
  ** station disables its transmitter and the time that it resumes
  ** using the SF >= N2M (12) frames per section 6.6.6.2.8.2.1 of
  ** IS-95B.
  */
  if ( rxc_lost_frames >= cai_tmo.n2m )
  {
    /* No need to use stored EIB's because we will receive 2 frames
     * before transmitting, so their EIB's will be used automatically.
     */
    if (rxc_hard_handoff_in_progress)
    {
      MSG_HIGH("Delay after HHO fail", 0, 0, 0);
      rxc_delay_after_hho_failure = TRUE;
    }
  }
  else
  {
    /* Restore EIBS if Rate Set 2 */
    if ( rxc_so.rate_set == DEC_RATE_14400 )
    {
      txc_flag_next_int();
      txc_eib_flag = rxc_eib_flags.previous;
      rxc_use_stored_eibs = TRUE;
#ifdef FEATURE_IS2000_REL_A
      txc_qib_flag = rxc_qib_flags.previous;
      txc_sch_eib_flag = rxc_sch_eib_flags.previous;
      rxc_use_stored_qibs = TRUE;
      rxc_use_stored_sch_eibs = TRUE;
#endif /* FEATURE_IS2000_REL_A */
    }

    rxc_start_tx();
  }

#endif /* FEATURE_IS95B_MAHHO || FEATURE_GPS_MODE */
}

#endif /* FEATURE_HHO_ENHANCEMENTS */

/*===========================================================================

FUNCTION rxc_set_action_time_signal

DESCRIPTION This function sets the RXC_ACTION_TIME_SIG for RXC.  This signal
  indicates that there is an RXC action in this frame which needs to be processed.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rxc_set_action_time_signal(void)
{
  MSG_HIGH("RXC set act time sig",0,0,0);
  (void)rex_set_sigs ( &rx_tcb, RXC_ACTION_TIME_SIG );
} /* end rxc_set_action_time_signal */

/*===========================================================================

FUNCTION rxc_custom_interrupt_reg_func

DESCRIPTION This function is called to obtain a pointer to MC's transaction 
            manager's function which handles action time.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rxc_custom_interrupt_reg_func(
  void (* ticker_func)(mctrans_server_token_type, qword)
)
{
  /* Store ptr to action time function and call it, if it's not NULL, 
  ** every frame to process action time.
  */
  //MSG_HIGH("Before INIT rxc_at_notifier_func_ptr %d",
  //         rxc_action_time_notifier_func_ptr,0,0);

  rxc_action_time_notifier_func_ptr = ticker_func;
  
  //MSG_HIGH("After INIT rxc_at_notifier_func_ptr %d",
  //         rxc_action_time_notifier_func_ptr,0,0);
} /* end rxc_custom_interrupt_reg_func */

/*lint -save -e785 Too few initializers for aggregate -- Difficult to keep
 * track of due to ifdefs, probably OK */
/* Structure to support centralized transaction manager.  This struct contains
** call back functions to set action time signal and to register the timer tick
** used in processing RXC actions, as well as the functions which the MC will 
** call to process RXC actions.
*/
mctrans_function_pointer_initializer_type rxc_func_pointer_initializers =
{
  rxc_set_action_time_signal,
  rxc_custom_interrupt_reg_func,
  {
#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
    /* For VP2 mode, need to check if the primary power control 
    ** has changed and program the deinterleaver if it has.  This is 
    ** separate from the main FFPC action processing.  Function 
    ** rxc_ffpc_trans_cmd() calls ffpc_trans_cmd() when it's done 
    ** with the power control channel switch.
    */
    { MCTRANS_FFPC_ACTION,            rxc_ffpc_trans_cmd},
#else
    { MCTRANS_FFPC_ACTION,            ffpc_trans_cmd},
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */
    { MCTRANS_RLGC_ACTION,            NULL},
#ifdef FEATURE_IS2000_F_SCH
    { MCTRANS_F_SCH_ACTION,           rxc_fwd_sch_trans_cmd},
#else
    { MCTRANS_F_SCH_ACTION,           NULL},
#endif // FEATURE_IS2000_F_SCH
    { MCTRANS_R_SCH_ACTION,           NULL},
    { MCTRANS_HO_ACTION,              rxc_ho_trans_cmd},
    { MCTRANS_SCR_ACTION,             rxc_scr_trans_cmd},
    { MCTRANS_CFS_NSET_ACTION,        NULL},
    { MCTRANS_CFS_CTRL_ACTION,        NULL},
    { MCTRANS_GATING_ACTION,          NULL},
#ifdef FEATURE_IS2000_CHS
    { MCTRANS_CTRL_HLD_GATING_ACTION, rxc_control_hold_trans_cmd},
#else
    { MCTRANS_CTRL_HLD_GATING_ACTION, NULL},
#endif // FEATURE_IS2000_CHS
    { MCTRANS_ASET_ACTION,            NULL},
#ifdef FEATURE_HHO_ENHANCEMENTS
    { MCTRANS_HHO_BEGIN_ACTION,       rxc_hho_begin_cmd},
    { MCTRANS_RETURN_ACTION,          rxc_return_cmd},
    { MCTRANS_CFS_GPS_PREP_ACTION,    rxc_cfs_gps_prep_cmd},
#endif
    { MCTRANS_NULL_ACTION,            NULL},
    { MCTRANS_MAX_ACTION,             NULL}
  }
};
/*lint -restore */

/*===========================================================================

FUNCTION rxc_register_action_handlers_notifier_interrupt_func

DESCRIPTION This function is called at power up to register the following items
            with MC's transaction manager:
      - call back function to set RXC action time signal 
      - call back function to register RXC timer tick
            - call back functions which MC will call to process RXC actions.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rxc_register_action_handlers_notifier_interrupt_func (void)
{
  MSG_HIGH("reg_act_handlers called",0,0,0);
  mctrans_register_action_notifier_handler_interrupt_reg_func(
                      MCTRANS_SERVER_RXC,
                      rxc_func_pointer_initializers);
}

#ifdef FEATURE_IS2000_F_SCH  
/*===========================================================================

FUNCTION rxc_check_for_sch_update

DESCRIPTION
  This function checks for SCH updates in case of 1 frame gap.  This function
  is called AFTER the current frame has been demuxed and is ready for RLP, 
  and BEFORE RLP is called.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rxc_check_for_sch_update (void)
{
  /* If there is a deferred SCH update pending, do it now
  ** This covers the 1-frame gap scenario
  **
  ** This code was moved here from its original location
  ** (towards the end of rxc_tc_isr().)  This code needed to be moved
  ** because the previous location causes the current frame
  ** to be demuxed with new SCH parameters meant for future frames.  
  ** (The current frame is always demuxed AFTER rxc_tc_isr().  The critical 
  ** problem which showed up with the old time line is the last frame
  ** of the previous burst is bad when the rate is changed and
  ** the gap between the 2 bursts is 1 frame, specifically for MUX 
  ** PDU 5.  There may not be NAKs because the frame sequence numbers 
  ** still match, the problem shows up in bad PPP errors, as a portion
  ** of the data payload may have been added (garbage) or deleted 
  ** depending on the rate change (from lower to higer or from higher to 
  ** lower.)
  ** 
  ** A good time to update SCH info is after the frame data has been
  ** demuxed but before the frame data is given to RLP.  Since RLP
  ** runs in RXC context, we don't want to do the update after giving
  ** data to RLP, since RLP may run across frame boundary, and we must
  ** update the SCH within the correct frame to be aligned with burst
  ** assignments.
  */
  if ( rxc_sch_update_now )
  {
    MSG_HIGH("Update SCH in RXC tsk contxt", 0, 0, 0);
    rxc_update_sch_info( &rxc_stored_sch_packet );
    rxc_config_sch_hw( &rxc_stored_sch_packet );
    rxc_sch_update_now = FALSE;
  }

} /* end rxc_check_for_sch_update */
#endif /* FEATURE_IS2000_F_SCH */

#if defined( FEATURE_RLP_LOGGING )
/*===========================================================================

FUNCTION RXC_CLEAR_RLP_STATISTICS

DESCRIPTION
  Clears the current values of the RLP statistics on command from the
  off-target diagnostic tool.
  
  NOTE that this is a wrapper function only.  The real processing has been
  moved to DSRLP module. 

  Input Parameters:
    *req_ptr: Pointer to DIAG request packet
    pkt_len: Length of DIAG request packet

DEPENDENCIES
  Logging mask, rxc_rlp_stats_interval module global

RETURN VALUE
  Pointer to DIAG response packet

SIDE EFFECTS
  None
===========================================================================*/
diag_rsp_type * rxc_clear_rlp_statistics(
  DIAG_RLP_STAT_RESET_F_req_type *req_ptr,
  word pkt_len
)
{

  DIAG_RLP_STAT_RESET_F_rsp_type *rsp_ptr;
  rsp_ptr = (DIAG_RLP_STAT_RESET_F_rsp_type *) dsrlp_clear_rlp_statistics (req_ptr, pkt_len);
  return((diag_rsp_type *) rsp_ptr );

}
#endif /* FEATURE_RLP_LOGGING */

#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
/*===========================================================================

FUNCTION RXC_CONFIGURE_FFPC_CHANNEL

DESCRIPTION
  Used in simultaneous F-FCH and F-DCCH channel set up.  Indicates 
  which channel handles FFPC. Also resets the data valid flag and 
  counters for FCH and DCCH in VP2
  
DEPENDENCIES
  

RETURN VALUE
  Current Radio Configuration

SIDE EFFECTS
  None
===========================================================================*/
cai_radio_config_type rxc_configure_ffpc_channel 
(
  rxc_cmd_msg_type *cmd_ptr
)
{ 
  cai_radio_config_type cur_rc = 0;

  /* need to reset the data valid flag and counters for FCH and DCCH in VP2 */
  rxc_so.for_dcch.included = FALSE;
  rxc_so.for_dcch.data_valid = FALSE;
  rxc_so.for_dcch.data_valid_countdown = 0;
  rxc_so.for_fch.included = FALSE;
  rxc_so.for_fch.data_valid = FALSE;
  rxc_so.for_fch.data_valid_countdown = 0;

  if (cmd_ptr->tci.fpc.included)
  {
    if ((cmd_ptr->tci.fpc.fpc_pri_chan == CAI_FPC_PRI_CHAN_DCCH) ||
        (cmd_ptr->tci.fpc.fpc_pri_chan == CAI_FPC_PRI_CHAN_FCH))
    {
      ffpc_channel = cmd_ptr->tci.fpc.fpc_pri_chan;
    }
    else
    {
      ERR ("Bad ffpc_ch val %d",
           cmd_ptr->tci.fpc.fpc_pri_chan,0,0);
      ffpc_channel = CAI_FPC_PRI_CHAN_FCH;   /* Use FCH as default */
    }
  }
  else
  {
    ERR ("FPC pri cntrl ch not avail",0,0,0);
    ffpc_channel = CAI_FPC_PRI_CHAN_FCH;     /* Use FCH as default */
  }

  /* DCCH configuration */
  if ( (rxc_so.for_dcch.included = cmd_ptr->tci.for_dcch.included) != FALSE )
  {
    cur_rc = rxc_so.for_dcch.rc = cmd_ptr->tci.for_dcch.rc;
    rxc_so.high_rc = TRUE;
    if (ffpc_channel == CAI_FPC_PRI_CHAN_DCCH)
    {
      rxc_config_demod ( (deint_rc_type) rxc_so.for_dcch.rc );
      MSG_HIGH ("Config demod for DCCH",0,0,0);
    }
    else
    {
      rxc_config_demod_ch1 ( (deint_rc_type) rxc_so.for_dcch.rc );
      MSG_HIGH ("Config demod Ch1 for DCCH",0,0,0);
    }
  }
  /* FCH configuration */
  if ( (rxc_so.for_fch.included = cmd_ptr->tci.for_fch.included) != FALSE )
  {
    cur_rc = rxc_so.for_fch.rc = cmd_ptr->tci.for_fch.rc;
    rxc_so.high_rc = rxc_so.for_fch.rc > CAI_RC_2;
    if (ffpc_channel == CAI_FPC_PRI_CHAN_FCH)
    {
      rxc_config_demod ( (deint_rc_type) rxc_so.for_fch.rc );
      MSG_HIGH ("Config demod for FCH",0,0,0);
    }
    else
    {
      rxc_config_demod_ch1 ( (deint_rc_type) rxc_so.for_fch.rc );
      MSG_HIGH ("Config demod Ch1 for FCH",0,0,0);
    }
  }
  return cur_rc;

} /* rxc_configure_ffpc_channel() */
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */


/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            S E A R C H E R    H A R D W A R E    U T I L I T Y    F I L E  
            


GENERAL DESCRIPTION
   This file is part of the breakup of the former file srchdrv.c. This file
   consists of the functions that are involved with programming and retrieving
   searches. (see below edit history for srchdrv.c's original general 
   description)

  Copyright (c) 1990 - 2009
                by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*===*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

Function List
   name                         Description
    srch_range_slew              ranges slew value with one PN circle
    srch_get_tx_adj              gets value of tx adj register
    srch_set_tx_adj              sets value of tx adj register
    srch_set_tx_rate_adj         sets value of tx adj register for data rate
    srch_get_rx_agc              gets value of rx agc register 
    srch_get_rx1_agc             gets value of rx1 agc register
    srch_start_rand_buf_fill     start to fill buffer with random peak data
    srch_peak_fill_rand_buf      loads buffer with random peak data


*====*====*====*====*====*====*====*====*====*====*====*====*====*====*===*/

/*=========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/1x/srch/rel/1h08/src/common/srch_hw.c#3 $

when       who     what, where, why
--------   ---     ------------------------------------------------------
03/10/09   adw     Corrected compiler warning.
02/20/09   bb      Ensure that CDMA TX clock / MDSP clock is on before 
                   reading TX gain adjust value while handling 
                   DIAG_TAGRAPH_F request"
09/04/08   vks     Fixed Klocwork errors
08/18/08   adw     Added SINGLE_ANTENNA to featurize alt chain for ULC.
12/15/06   tjc     Modified traffic to use the sector module
06/08/06   awj     Remove max_inx and max_eng from sector structures
05/31/06   awj     Further sect and srch4 updates
05/12/06   awj     Updated sector structure.
05/01/06   tjc     Stripped FEATURE_SEARCH1 and mainlined FEATURE_SRCH_PC2
10/26/05   awj     Lint compliance
09/23/05   ejv     Deprecate rex timer types/calls.
                   Remove srch_last_search_type.
09/22/05   sst     Corrected srch1 sub-packet to report start of window
09/14/05   ejv     Use static for local vars.
09/08/05   ejv     Use static in place of LOCAL.  Remove MSM5000_IRAM_REV.
08/30/05   ejv     Remove featurization from srch_get_rx1_agc.
07/14/05   sfm     Removed include of srch_tcxo.h
06/22/05   sfm     Moved AGC functions into srch_rx.
06/03/05   sfm     srch_rx naming changes
05/31/05   rng     Merged in from SHDR Sandbox.
05/24/05   ejv     Remove srch_fing_driver.h.
05/20/05   ejv     Added functions & constants from srch_pn.c/h.  Remove
                   srch_pn.h.
05/13/05   ejv     Cast SRCH_SUB_ID_x to uint8.
05/11/05   bt      Remove AGC to PDM dependency.
05/04/05   kwo     Removed include srch_idle_util.h
03/31/05   sst     Changed genlog reporting of srch1 pos, win_siz in Cx2
                   Mask the win_pos and peak_pos to ensure they're in bounds
02/23/05   ejv     Featurize srch1_merge_results.
02/10/05   kwo     Moved linear ecio calc into merge results
02/07/05   ejv     Lint update.  Removed legacy file header description.
12/15/04   kwo     Rework of DH interface
09/29/04   sfm     Changed include of srch_fing.h to srch_fing_driver.h
09/27/04   ejv     Include srchtc.h.
09/08/04   jcm     Rename searcher1 functions srch1
09/08/04   jcm     Added generalized srch1 subpacket
09/03/04   sfm     Finger Driver changes
09/03/04   kwo     Moved linear ecio peak processing to srch_util
08/20/04   kwo     Added support for linear ecio in srch1
07/19/04   ejv     Mainline FEATURE_IS95B_MAHHO.
06/14/04   ejv     Call muxmdsp_wr_tx_rate_adj for srch_set_tx_rate_adj
                   for 6700.
06/02/04   ejv     Remove support for allowing PLT to switch SRCH1 speed.
05/28/04   jcm     Sector module integration
05/04/04   sst     Merge from srch4 3Q dev branch
                   Removed srch4_enable() from srch_select_hw()
04/02/04   ejv     Call srch_tc_get_hho_status() instead of accessing
                   global which has been removed.
03/25/04   ejv     Mainlined FEATURE_MULTI_RX_CHAIN.
03/09/04   ejv     Removed FEATURE_MSM5105_PPM_SRCH_ENGINE.
02/23/04   ddh     Correcting lint errors.
02/20/04   ejv     Added more FEATURE_SEARCH1 checks.
02/04/04   br      Added separate free queue for PPM search request buffers.
01/28/04   ejv     Mainline T_MSM6500.
01/19/04   ejv     Remove unused TD-related drivers that used both 8x
                   and 16x searchers, since 16x does not exist.
01/16/04   ejv     Remove FEATURE_SEARCH2, FEATURE_SEARCH2_ACQ,
                   FEATURE_SRCH_SCHEDULER, and FEATURE_SRCH_SCHEDULER_MONITOR.
01/15/04   ejv     Remove early return in srch_search_reset_ptrs.
01/12/04   ejv     Added FEATURE_SEARCH1.
11/19/03   jcm     No longer resetting sector eng filt length to 1 in ZZ
11/04/03   fu/aaj  Function to read AGC1
11/04/03   bt      Change srch4_1x_en/disable to srch4_enable interface.
08/11/03   va      Use ppm_ptr for storing PPM results.
07/25/03   ejv     Add FEATURE_MULTI_RX_CHAIN.
06/23/03   ejv     Remove FEATURE_EXT_LINEARIZER.
06/11/03   rdh     Streamlined search governor.
06/09/03   ejv     Remove nop functions srch_set_agc_cdma() and
                   srch_freeze_iq() after mainlining FEATURE_RF_HAS_ZIF.
06/06/03   kwo     Added a missing INTFREE
04/28/03   ejv     Mainline 6100 code.
04/14/03   sst     Removed SRCH_ENABLE_SD20
03/25/03   ejv     Added function srch_stop_1x_searches().
03/24/03   ejv     Return if NULL ptr found when checking results.
02/12/03   bt      change sleep_1x_enable to select clk_src.
01/31/03   sst     Moved #endif to remove a warning
01/28/03   sst     Encapsulated srch_win_center to srch_hw.c/h
                   Added srch_(set/get/slew)_win_center()
                   Added calls to dis/enable clks for searcher 4
12/30/02   ejv     Removed FEATURE_MULTI_SRCH_SCHEDULER.
12/23/02   ejv     Removed implied returns.
11/25/02   ejv     Support for TD on NCC.
11/12/02   rm      Use the new HWIO macros to read synbol combiner
                   status.
10/31/02   gs      Merge in MSM_SRCH.08.06.47
10/04/02   sst     Added support for 4 & 6 searcher peaks
09/18/02   gs      Fix FEATURE_DH compile issues
07/30/02   gs      Initial merge into perforce for cougar.
07/16/02   ejv     Added FEATURE_SEC_CALLS_SEARCHER_RANDOM.
06/28/02   bt      Changed max_pos from a single value to an array.
06/14/02   ejv     Lowered message level.
06/10/02   ejv     In srch_sect_clr(), clear the dump signal if it gets
                   set but 16x is still active.
06/07/02   rdh     Fixed nonslotted IHO failures due to filt len maintenance.
05/30/02   ejv     Added missing TD code.
05/21/02   ejv     Mainlined FEATURE_IS2000.
05/02/02   ejv     Removed FEATURE_QUARTER_ROLL_TIMELINE.
04/22/02   ejv     Changed variable name.
04/18/02   ejv     Update trpt.len for ZZ state only.
04/11/02   br      Removed externalized srch_fing_pos_offset.
04/08/02   br      Changed order of execution in srch_req_stop() function.
                   Fixed incorrect usage of srch_sect_cmd_struct_type fields.
04/08/02   rdh     Added HT's corrections to function names inside RelA code.
03/21/02   br      Exchanged FEATURE_GPSONE with
                   FEATURE_MSM5105_PPM_SRCH_ENGINE.
03/01/02   ejv     Removed increment of srch_govern.srch_cnt.
02/27/02   br      Fixed PPM results reading from 16x searcher.
02/26/02   ejv     Added missing #ifdef.
02/25/02   ejv     Removed updating filter length for Nset searchers, as
                   it is already done in PC.
02/25/02   rm      Set the walsh and qof mask to 0 for 16x searcher.
02/21/02   ejv     Removed masking of 1/8 chip slew for 8x searcher.
02/15/02   ejv     Removed unused variable cur_win_pos.
02/14/02   ejv     Removed bogus set of ().  Added missing #include.
02/11/02   rm      Added MSM6050 support.
02/05/02   ejv     Added missing FEATURE_SEC.
01/30/02   gs      Corrected missing comment termination.
01/16/02   ejv     Added T_MSM6000.
01/14/02   SST     Merged into tip to replace srchdrv.c
01/11/02   SST     Completed function name changes.
                        srch_schedule_new_search_req -> srch_req_schedule_new
                        srch_prog_new_search_req -> srch_req_prog_new
                        srch_clear_scheduled_srch_req -> 
                                                    srch_req_clear_scheduled
                        srch_err_clear_requests -> srch_req_err_clear
                        srch_done_srch_req_management -> srch_req_done_mgmt
                        srch_ppm_free -> srch_req_ppm_free
                        srch_get_aborted_ppm_srch_result -> 
                                              srch_req_get_aborted_ppm_result
                        srch_stop_current_ppm_srch -> 
                                                  srch_req_stop_cur_ppm_srch
01/10/02   SST     finished move for srchdrv.c breakup
01/01/02   SST     Created Module - refer below for earlier edit history.

                        The following is the edit history from srchdrv.c
12/02/01   br      Added new function definition srch_cdma_search_active().
11/25/01   pj      Got rid of srch_fing_dump in srch_fing_freeze.
11/21/01   br      Changed updating 16x searcher position after abort and
                   changed status dump setting mechanism for SRCH scheduler.
                   Updated comments and fixed spelling mistake.
11/05/01   ejv     Initialize QOF for CH3.
10/31/01   ejv     More REL_A updates.
10/24/01   pj      Improved srch_fing_(un)freeze to (un)force fingers 
                   into lock. The fingers should be forced into lock during a GPS visit
                   so that the finger combiner offset will be latched.
10/23/01   rm      Added a status flag to indicate if hardware finger
                   merge detection is enabled or not.
10/11/01   ejv     Changed comments.
10/08/01   ejv     Added additional REL_A PLT support.
10/08/01   br      Backed off some search scheduler changes.
09/25/01   rm      For fat path finger assignments, enable finger merge detection 
                   in SYNC, PAGING and TC state.
09/25/01   ejv     Added additional FEATURE_IS2000_REL_A support.  Cleanup.
                   Use common_path for energy/index storage.  Added new
                   function srch_sect_merge_results().  Increased the oldest
                   supported MSM to MSM3300 and newer.
08/07/01   aaj     Added QPCH ULPN mask changes for MSM5500
08/22/01   ejv     Shadow init of DEM_FRAME_OFF.
07/26/01   ajn     SLEEP VU - Remerge with Search Tip
07/18/01   br      Added random number generator support to SRCH scheduler.
                   Fixed PPM search abort problems.
07/12/01   ejv     Update srch_win_center when undoing 1/4 roll timeline.
07/12/01   ejv     Write to AGC_CTL3 as a byte for MSM5105+.
07/06/01   br      Added support for abort_srch_callback in SRCH scheduler.
                   Added support for getting partial PPM search results when
                   PPM search is aborted. Corrected IS2000_P2 featurization.
06/25/01   ejv     Renamed srch_fing_cfg_fch_qof() to srch_fing_set_qof().
                   Added function srch_fing_cfg_all_qof().  Updated comments.
06/21/01   ajn     SLEEP VU: sleep clock management moved to SCLK.*
06/20/01   ejv     Mainlined FEATURE_SRCH_SLEW_IN_DRV.
                   Removed srch_winsize_slew.
                   Initialize srch_fings_common in srch_config_msm().
06/14/01   ejv     Wrapped srch_finger_lock_handler() with FEATURE_FINGER_POLL.
06/13/01   br      srch_fing_pos_offset used externally.
06/12/01   ejv     Changed uint8->byte and uint16->word.
06/12/01   abh     Shadow the master finger, so that it can be used for gps
                   timetransfer.
06/07/01   ejv     Removed some unnecessary SD20 code.
06/05/01   ejv     Created FEATURE_FAST_FINGER_SLEW and replaced some T_MSM5000
                   checks with FEATURE_IS2000.  Also removed T_MSM5000A_SUPPORT.
06/05/01   rdh     Got rid of FEATURE_QUICK_SCAN, mainlining it now.
06/04/01   ajn     Corrected upper nibble code in srch_sec_get_rnd( ).
06/01/01   ajn     Updated srch_sec_get_rnd().
06/01/01   ajn     Added FEATURE_SEC.
06/01/01   ejv     Added #ifdefs to allow FEATURE_IS95B_MAHHO to be turned off.
05/23/01   br      Featurized debugging message.
05/23/01   ejv     Check for QPCH enabled before reloading finger walsh.
05/22/01   br      Removed ERR message.
05/21/01   br      Changes in SRCH scheduler mechanism.
05/21/01   rm      Enable merge protection between finger pairs even when
                   they are not in combiner lock, since the finger RSSI
                   might still be building up and the fingers could go in lock
                   later.
05/20/01   br      Added support for subsignals. Added more SRCH scheduler
                   support. Changed search duration determination for
                   setting lost search dump timer. Code cleanup.
05/17/01   rm      Separated programming of merge detection between finger
                   pairs and finger de-assignment of merged fingers.
05/08/01   ejv     Remove function srch_8x_ena() for non-PLT.
05/08/01   rdh     Only generate epoch on lost dump expiry if 16X is active.
05/08/01   ejv     Replaced access of possibly NULL ptr with init'd variable.
05/03/01   rm      Disable Second order time trakcing loop when hardware merge
                   detection is enabled in TC state. Also de-assign the weaker
                   finger when the two fingers are within 5/8th of a chip as
                   compared to when they are with in 6/8th of a chip.
05/03/01   ejv     Replaced labs with LABS.
05/03/01   rdh     Completely disable finger when it is deassigned.
05/01/01   br      Added SRCH Scheduler support. Moved searcher slewing to
                   SRCH drivers. Added shoulder search mode support.
04/26/01   rdh     Changes for 8X searching in all states, code streamlining.
03/29/01   ejv     Added FEATURE_IS2000_P2.
03/16/01   ejv     Removed ULPN mask changes for MSM5100.
03/14/01   ejv     Added FEATURE_IS2000_REL_A.
03/12/01   dgy     Added FEATURE_IS2000_QPCH_CCI.
02/27/01   ejv     Added ajn's updates to lock/unlock_age in srch_fing_restart.
02/21/01   ejv     Removed #ifdef from a section of the finger QOF config code.
02/15/01   bgc     Changed FTM to a runtime mode.
02/14/01   ejv     Added changes for setting the ULPN mask on MSM5100.
02/14/01   rdh     Added initial support for System Determination 2.0.
02/07/01   ejv     Updated DMOD_R_FRSSI for MSM5100.
02/07/01   ejv     Don't disable finger CH1 if QPCH is enabled and initialize
                   the finger power lock threshold.
01/31/01   rm      Added FEATURE_HW_FING_MERGE_DETECT.
01/23/01   ejv     Added missing macros for MSM5100.
01/18/01   ejv     Enabled clock regimes separately in srch_dmod_reset.
01/18/01   ejv     Added MSM5100 support.
01/12/01   ejv     Added missing "#ifdef FEATURE_IS2000_SCH" check.
01/08/01   ejv     Added new sub-structures for channel parameters.
12/18/00   rm      Revert the changes to adjust the raw position when finger
                   is slewed.
12/19/00   ajn     Changes for disabling sup. channels when not in use.
12/11/00   et      Replaced one call to clk_regime_enable with OR'd parameters
                   with 2 calls to clk_regime_enable with 1 parameter for MSM5105.
12/11/00   rm      Initialization changes for MSM5105.
11/17/00   ajn     Adjust raw_pos when finger is slewed to reflect new posn.
11/09/00   ejv     Add a 2nd abort call for SRCH2.  Always return positive
                   slew values from srch_fast_searcher_slew.
11/07/00   rdh     Generate epoch in srch_trig_dmp_err.
11/06/00   ejv     Added new searcher driver interface.
11/03/00   br      Changed reference pilot determination mechanism for gpsOne.
10/23/00   ejv     Moved interrupt lock in srch_sect_q_cmd to avoid a possible
                   race condition.
10/16/00   ejv     Removed T_SURF.
10/03/00   ejv     Removed dummy IRAM ifdef.
10/02/00   ejv     Removed IRAM ifdef from 2 functions.  Left in a dummy
                   ifdef to avoid compile problems.
09/26/00   dgy/ejv Checked the special searcher dump for QPCH, which we use
                   as the wakeup roll interrupt, in srch_bypass_interface().
09/18/00   ejv     Added new function srch_trans_qpch_to_pc().
09/12/00   abh,pj  Added master_fing_get_pos. Modified freeze/unfreeze fing.
09/11/00   ajn/dgy Added the QPCH timeline that wakes up before the first PI
           /ejv    bit, goes back to sleep immediately if any of the two PI
                   bits is zero, and stays awake for PCH if both PI bits are
                   non-zero.
09/10/00   ejv     Added macros to enable/disable QOF on FCH and SCH.
09/06/00   bc      Added FEATURE_FACTORY_TESTMODE.
08/21/00   ejv     Removed FEATURE_SRCH_DYNAMIC_BAND_CLASS and FEATURE_
                   PILOT_SCANNER.
08/07/00   day     Added FEATURE_IS2000_QPCH around srch_qpch_enabled.
07/31/00   eh      Check for srch_qpch_enabled before setting walsh code.
07/31/00   ejv     Added MSM5000_IRAM_REV.
07/20/00   ejv     Set PDM1/2 initial value to bottom rail.
07/13/00   yll     Added srch_fing_freeze and srch_fing_unfreeze for
                   "tune away without drop" capability in FEATURE_GPSONE.
06/19/00   ejv     Updated the srch_fing_merge_detect function.
06/13/00   ejv     Removed srchint_wakeup_roll from slow clock ifdef.
06/12/00   ejv     Added support for changing finger TTL speed.
06/09/00    eh     Added support for search window offset.
06/08/00   ejv     Added MSM5000_IRAM_FWD.
06/02/00   nxb     Added back FEATURE_GPIO_30
05/23/00    eh     Added checking of srch_qpch_enabled in srch_fing_cmd before
                   disabling SCCH soft combine because QPCH pilot threshold
                   uses channel 1.
                   Use MAX_FING_CNT to set srch_fing_cnt in srch_config_msm.
05/15/00   rdh     Simplified and consolidated fing lock threshold variables.
05/04/00   wli     Removed FEATURE_SEARCH2_IDLE to compile two functions
05/01/00   ejv     Moved wrapping around demod_bypass functions.
04/24/00   dgy     Added some comments for FEATURE_IS2000_SCH.
04/13/00   ejv     Moved srch_get_win_pos with _cmd functions.
04/10/00   eh/ejv  Added FEATURE_RELATIVE_DMOD_POS.
03/21/00   ajn     Fixed TCXO/4 sleep problem caused by disabling TRK_LO_ADJ PDM
02/24/00    eh     Added srch_enable_demod_bypass and
                   srch_disable_demod_bypass function definition.
02/16/00    eh     Enabled support for 7 SCCH for MSM5000B.
02/14/00   rdh     Added FEATURE_FING_MERGE_DETECT.
02/14/00   ejv     Added function srch_set_tx_rate_adj.
02/11/00   ejv     Wrapped srch_bypass_interface in FEATURE_SLEEP_SLOW_CLOCK.
02/02/00   ejv     Added function srch_slew_16x and modified srch_sect_clr to
                   insure we will finish the queue before proceeding.
01/27/00   ejv     Compile SRCH2_CLK_ENA for MSM3100 based targets.
01/13/00   gb/ejv  Added srch_bypass_interface() function and necessary externs
                   and defines for support.
01/10/00   rdh     Removed window segmenting.
01/10/00   ajn     Re-Added FEATURE_ENHANCED_STANDBY_III & QUARTER_ROLL_TIMELINE
                   support.
12/13/99   ejv     Cleanup for FEATURE_SEARCH2 and FEATURE_SEARCH2_IDLE.
                   Changed srch_get_dmod_id to return a word instead of byte.
11/24/99   ajn     Finger "Change Flags" for Supplimental Channel support
                   QPCH Support added, TEMP_HACKS_FOR_TIGER removed
11/10/99   ejv     Added FEATURE_SEARCH2 and FEATURE_SEARCH2_IDLE.
11/08/99   dgy     Handled invalid QOF mask id.
11/03/99   lcc     Removed call to COMBINER_LOCK_RSSI_EXT_RANGE.  This is not
                   needed for the new demod image.
11/02/99   ejv     Removed update of srchtri_delayed_fing_posn from function
                   srch_delay_last_finger().
10/21/99   rm      Fixed a bug in srch_fing_sup_init, where we were disabling
                   all channels for a finger. We need to enable channel 0
                   to demodultae SYNC Channel afetr we unslew.
10/21/99   ejv     Do signal checking on rex_sigs_type instead of words.
10/20/99   dgy     Added support for QOF.
10/07/99   lcc     Merged from PLT2.0:
           ajn/rm  Added in support for SCH.
10/06/99   ejv     Removed unused variable srch_fing_pc_cell_en[].  Removed
                   some of the srch_msm_config parameters and replaced the
                   useage with hard-coded values.
09/30/99   yus     IS95B code review cleanup.
09/07/99   ejv     Added function srch_freeze_iq, which extracts a portion of
                   srch_park_trk_lo_adj.
08/23/99   ejv     Mainlined FEATURE_ENHANCED_STANDBY_II.  Removed function
                   srch_sect_end, since it does not do anything now that
                   FEATURE_ENHANCED_STANDYB_II is mainlined.
08/11/99   rm      Removed function srch_enable_symbols and srch_disable_symbols.
                   New macros have been defined in msm_drv.h to enable/disable
                   I/Q channels.
08/06/99   ejv     ajn: Mainlined T_ARM support.  Mainlined T_MSM3 support.
                   Removed FEATURE_NO_VBB.  Mainlined FEATURE_ENHANCED_
                   STANDBY and FEATURE_VIRTUAL_COMBINER.  Mainlined delayed
                   timeline code.  Added FEATURE_DH code from srch.c.
                   Removed SRCH_FING_DEBUG.  Replaced many of the DMOD
                   read/writes with new macros (which are defined in
                   msm_drv.h).  Added some new low level function calls.
                   Removed pre-MSM3000 support.
                   wjn: Added FEATURE_SRCH_DYNAMIC_BAND_CLASS.
08/06/99   ejv     Added preliminary Pink Panther support.  Added initial
                   Tiger2 support.
07/24/99   jcw     Merged in PLT support
07/23/99   wjn     Added Added function srch_set_dmod_pilot_gain() for run time
                   CDMA band class switches to support dual CDMA band class.
07/19/99   ml      Added support for PLT.
06/28/99   yus     Merged IS95B features to the ASW tip.
06/09/99   rdh     Merged FEATURE_PILOT_SCANNER and FEATURE_SCAN_FING_LOG.
06/04/99   ejv     Unwrapped srch_get_rx_agc from being conditionally compiled.
06/03/99   ejv     srch_get_rx_agc will only update the Rx AGC value if the
                   PDMs have not been frozen.  Otherwise it uses the previous
                   value.
05/25/99   ejv     Added intlock/free in srch_fing_move and added new functions
                   srch_fing_disable and srch_fing_restart.
05/03/99   ejv     Removed the conditional scaling for finger RSSI if logging
                   Gen TA packets.  Instead store necessary results for use
                   in srch_rpt_ta as required.
04/23/99   ejv     Replaced repetitive DMOD_I_OFF_EN_V with DMOD_Q_OFF_EN_V.
04/16/99   ejv     Removed unaccessed variable.
04/08/99   ejv     Removed LOCAL from srch_range_slew since it is externed in
                   srchi.h and used in srchaq.c.
04/05/99   yus     Added function srch_4th_fing_enable for enabling/disabling
                   the fourth finger.
04/01/99   ejv     Removed FEATURE_SBI code, since this is now run from RF.
                   Freeze/unfreeze AGC_REF, I_OFFSET, and Q_OFFSET during
                   srch_park_trk_lo_adj.
02/16/99   ejv     Merged update of new parameter last_win_siz in
                   srch_sect_struct_type.
02/03/99   ejv     yus: Merge updates for MDR features.
02/03/99   ejv     yus: Added IS95B MDR support.
02/02/99   ejv     Changed T_VIRTUAL_COMBINER to FEATURE_VIRTUAL_COMBINER.
02/01/99   ejv     Added numerous changes for rdh's FEATURE_ENHANCED_STANDBY_II
                   including additional functions.  Also added dgy's changes
                   for FEATURE_QUICK_SCAN.
01/25/99   ejv     Must define FEATURE_NO_VBB if virtual boot block is not
                   being used.
01/21/99   ejv     Removed FEATURE_DH code - not necessary for non-DMA
                   implementation.
01/13/99   ejv     ajn: Added knowledge about time reference slamming to
                   srch_comb_slam_timeline & added srch_get_vsc_time( )
01/13/99   ejv     Removed #ifndef T_MSM3 code.
01/08/99   ejv     Made changes to remove ARM compiler warnings.
12/17/98   ejv     Peaks will now be poked into buffer for Panther whether or
                   not 8x is on.  There is no DMA in 1x.
11/25/98   yus     Added P_REV checking.
11/25/98   yus     Changed #ifdef FEATURE_JSTD008 to run time checking.
10/16/98   ejv     Merged changes to support Panther.
10/08/98   ejv     Changed FEATURE_NEW_SLEEP_TIMELINE to
                   FEATURE_ENHANCED_STANDBY.
08/18/98   ejv     Replaced srch_msm versions with versions listed in hw.h,
                   including Lynx support.  Call hw_version directly instead
                   of calling vocm2_version.
08/18/98   ejv     Added dh task support, must define FEATURE_DH.
07/30/98   bgc/mk  Added srch_finger_lock_handler() to monitor the fingers
                   out of power combiner lock state.
07/14/98   dgy     Fixed bug that erroneously updates power control cell
                   assignment register for fourth finger.
07/01/98   ajn     Added FEATURE_NEW_SLEEP_TIMELINE macro.
05/13/98   ajn     Used a delayed timeline for MSM2.3 in paging/idle sleep
                   mode.
04/21/98   mk      Added support for msm2310.
03/30/98   dgy     Enabled fourth finger. Disabled fingers not in use. Used
                   extended RSSI range for general TA packet. Added function
                   to set power combiner threshold.
03/04/98   smo     Added check to verify we had a buffer before poking peaks.
01/26/98   smo     Deleted Time-Tracking Gain adjustment for newly unassigned
                   fingers (msm2300 only).
11/25/97   smo     Changed default initialization to Turbo (8x) mode.
10/28/97   smo     Added support for msm2300, 8x searcher.
10/28/97   smo     Added support for msm2300, A4.
10/01/97   smo     Added MSM2300 support code. Modified functions include:
                   srch_dmod_reset, srch_sect_cmd, srch_sect_dmp,
                   srch_sect_stop, srch_fing_cmd, srch_fing_dmp,
                   srch_comb_ftrk_acq, srch_set_carrier_freq, srch_set_tx_adj.
                   New functions:  srch_dma_ena, srch_8x_ena, srch_config_msm.
06/19/97   rdh     Added T_Q, removed pre-NGP support, updated copyright.
03/14/97   jjn     Added (TG==T_MD) to the appropriate conditional compile
                   statements for the Module
01/22/97   rdh     Added srch_get_stop_slew pulled from ZZ state.
11/22/96   rdh     Added Hutch's T_T target definitions.
08/14/96   gb      srch_fing_cmd now reloads lock thresholds for all fingers.
06/07/96   smo     Added Pilot filter bandwidth initialization code.
03/19/96   rdh     Made lost dump catcher able to catch back-to-back failures.
02/01/96   rdh     Added srch_sect_stop to more quickly respond to idle sleep.
01/06/96   rdh     Moved in modules from SRCH.C to help simplify code review.
11/15/95   jah     Moved interrupt mask/polarity initialization to tramp_init,
                   and changed srch_dmod_reset() to be a re-init, to let
                   tramp_init() specify the values.
09/20/95   rdh     Made PN mask autoswitching compile-time chosen. Saves ROM.
08/23/95   rdh     Pre- code review cleanup checkin to save RAM/ROM.
07/26/95   rdh     Fixed AGC I/Q offset init bug after pctl Punct changes made.
07/17/95   rdh     Added pctl Punct rate set function for MSM 2p.
06/15/95   rdh     Railing TRK_LO_ADJ high now for Gemini. A bit more stable.
06/13/95   rdh     Made T_SLEEP handling explicit and consistent DMSS-wide.
06/01/95   rdh     Collected all DMOD I/O functions across Search task.

===========================================================================*/

#include "customer.h"
#include "comdef.h"
#include "clk.h"
#ifdef FEATURE_SRCH_CLKREGIM_RM
#include "clkrgm_rm.h"
#endif
#ifdef FEATURE_MDSP_SUPPORTS_TX_AGC
#include "clkrgm_modem.h"
#endif
#include "err.h"
#include "msg.h"
#include "msm_drv.h"
#include "queue.h"
#include "rex.h"
#include "srch.h"
#include "srchi.h"
#include "srch4trans.h"
#include "srch_hw.h"
#include "dmod.h"
#include "qw.h"
#include "rfm.h"
#include "srch_comb.h"
#include "srch_conv.h"
#include "srch_genlog.h"
#ifdef FEATURE_DH
#include "dh.h"
#endif

#include "srch_sect.h"
#include "srch_lib_ext.h"

#ifdef T_MSM6700
#include "muxmdsp.h"
#endif /* T_MSM6700 */

#include "srch_rx.h"
#include "timer.h"


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/*-------------------------------------------------------------------------
      Constants
-------------------------------------------------------------------------*/

#define  FULL_PN_CIRCLE    (SHORT_CODE_LEN * (int4) CHIPX8_PER_CHIP)
#define  HALF_PN_CIRCLE    (SHORT_CODE_HALF_LEN * (int4) CHIPX8_PER_CHIP)
#define  QUARTER_PN_CIRCLE (SHORT_CODE_QUARTER_LEN * (int4) CHIPX8_PER_CHIP)
  /* The number of 1/8 chip "chipx8" units in a full, half, and quarter
     PN sequence */


/*XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  X                                                                  X
  X     Temporary placement of security and rf functions...          X
  X     hopefully to be moved soon to their own files/subsystems     X
  X                                                                  X */


#ifdef FEATURE_SEC
/*---------------------------------------------------------------------------
                 Variables pertaining to rand buffer
---------------------------------------------------------------------------*/
srch_sec_type srch_sec;
#endif /* FEATURE_SEC */

#ifdef FEATURE_DH
/*---------------------------------------------------------------------------
                 Variables pertaining to rand buffer fill
---------------------------------------------------------------------------*/

byte srch_rand_buf[ SRCH_MAX_RAND_BUF_SIZE ];
  /* This is the buffer where we will store the data */

static uint32 srch_rand_buf_cnt = 0;
  /* Number of bytes in the random buffer */

static uint32 srch_akey_start_time;
static uint32 srch_akey_end_time;
static uint32 srch_akey_delta_time;
  /* Vars for timing analysis */

#endif

/*X                                                                  X
  X      End of temporary stuff                                      X
  X                                                                  X
  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX*/       


/*===========================================================================
              FUNCTION DEFINITIONS FOR MODULE
===========================================================================*/


/*===========================================================================

FUNCTION SRCH_RANGE_SLEW

DESCRIPTION    This function makes sure a particular slew amount is
      within the allowable + - 32768-chip range.

DEPENDENCIES   None.

RETURN VALUE   The range-justified slew.

SIDE EFFECTS   None.

===========================================================================*/

int4 srch_range_slew
(
  int4 slew
    /* The slew to keep within good range */
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  slew &= PN_CIRCLE_MASK;
    /* Limit slew to the range of a PN circle */

  if ( slew > HALF_PN_CIRCLE )
  {
    slew -= FULL_PN_CIRCLE;
      /* If slew is more than +1/2 PN circle, slew the opposite direction */
  }
  return ( slew );

} /* srch_range_slew */



/*XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  X                                                                  X
  X     Temporary placement of security and rf functions...          X
  X     hopefully to be moved soon to their own files/subsystems     X
  X                                                                  X
  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX*/       


/*===========================================================================

FUNCTION SRCH_GET_TX_ADJ

DESCRIPTION    This function returns the current value of the tx gain
      adjust register.

DEPENDENCIES   srch_dmod_reset should have been previously called.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/

byte  srch_get_tx_adj ( void )
{
  byte tx_gain_adj;
  /* variable used to return the tx gain adjust value */

  #ifdef FEATURE_SRCH_CLKREGIM_RM
  #ifdef FEATURE_MDSP_SUPPORTS_TX_AGC
  boolean mdsp_clk_is_off = FALSE;              /* flag to track MDSP clock status */
  #endif
  #endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MSM_LOCK();
    /* Lock for access to msm registers */

  #ifdef FEATURE_SRCH_CLKREGIM_RM
  /* Make sure that either MDSP clock or CDMA TX clock is on when we try 
   * to access the MDSP/HW register for TX Gain adjustment value
   */
  #ifdef FEATURE_MDSP_SUPPORTS_TX_AGC  
  /* If MDSP clock is off, turn it on, we need it */
  mdsp_clk_is_off = !clk_regime_is_on( CLKRGM_MDSP_CLK );
  if ( mdsp_clk_is_off )
  {
    clk_regime_resource_enable( CLKRGM_CLIENT_1X, CLKRGM_RESOURCE_MDSP );
  }

  /* read contents of tx gain adjust value */
  tx_gain_adj = TX_GET_GAIN_ADJUST();

  /* If MDSP clock was off before, turn it off, we no longer need it */
  if ( mdsp_clk_is_off )
  {
    clk_regime_resource_disable( CLKRGM_CLIENT_1X, CLKRGM_RESOURCE_MDSP );
  }
  #else /* !FEATURE_MDSP_SUPPORTS_TX_AGC */
  if(!clk_regime_resource_required_by_client(CLKRGM_CLIENT_1X, CLKRGM_RESOURCE_CDMA_TX))
  {
    /* enable clocks to access this register */
    clk_regime_resource_enable(CLKRGM_CLIENT_1X, CLKRGM_RESOURCE_CDMA_TX);

    /* Read current contents of tx gain adjust register. */
    tx_gain_adj = TX_GET_GAIN_ADJUST();

    /* disable clocks */
    clk_regime_resource_disable(CLKRGM_CLIENT_1X, CLKRGM_RESOURCE_CDMA_TX);
  }
  else /* TX Clocks are ON */
  {
    /* Read current contents of tx gain adjust register. */
    tx_gain_adj = TX_GET_GAIN_ADJUST();
  }
  #endif /* FEATURE_MDSP_SUPPORTS_TX_AGC */ 
  #else /*  !FEATURE_SRCH_CLKREGIM_RM */ 
  tx_gain_adj = TX_GET_GAIN_ADJUST();
    /* Read current contents of tx gain adjust register. */
  #endif /*  FEATURE_SRCH_CLKREGIM_RM */

  MSM_UNLOCK();
    /* Lock after access to demod registers */

  return tx_gain_adj;

} /* srch_get_tx_adj */



/*===========================================================================

FUNCTION SRCH_SET_TX_ADJ

DESCRIPTION    This function allows external tasks (namely the Transmit
  Task) to have access to the tx gain adjust register of the demod chip.

DEPENDENCIES   srch_dmod_reset should have been previously called.

RETURN VALUE   None.

SIDE EFFECTS   Changes the contents of the tx gain adjust register of the
               demod chip.

===========================================================================*/

void  srch_set_tx_adj
(
  byte  tx_adj
    /* The 8-bit value to be written to the tx gain adjust register.
       Note that this is a signed value. For maximum power, use the
       value -128 (0x80); for minimum power, use +127 (0x7F). */
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSM_LOCK( );
    /* Lock for access to msm registers */

  TX_SET_GAIN_ADJUST( tx_adj );
    /* Write specified value to tx gain adjust register.  Note:  The macro
       takes care to first clear power control enable bit so this write
       takes effect. */

  MSM_UNLOCK( );
    /* Lock after access to demod registers */

} /* srch_set_tx_adj */



/*===========================================================================

FUNCTION SRCH_SET_TX_RATE_ADJ

DESCRIPTION    This function allows external tasks (namely the Transmit
  Task) to have access to the tx gain adjust register for the data rate.

DEPENDENCIES   srch_dmod_reset should have been previously called.

RETURN VALUE   None.

SIDE EFFECTS   Changes the contents of the tx rate adjust register of the
               demod chip.

===========================================================================*/

void  srch_set_tx_rate_adj
(
  int16  tx_adj
    /* The 10-bit value to be written to the tx rate adjust register. */
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef T_MSM6700
  /* EEE 6700 revisit - remove this once 6700 MUX code
     calls this function directly themselves or entire
     srch_set_tx_rate_adj function moves to TXC */
  muxmdsp_wr_tx_rate_adj ( tx_adj );
  #else
  MSM_LOCK( );
    /* Lock for access to msm registers */

  MSM_W_MASKED( TX_RATE_ADJ, DATA, tx_adj );
    /* Write specified value to tx gain adjust register */

  MSM_UNLOCK( );
    /* Lock after access to demod registers */
  #endif /* T_MSM6700 */

} /* srch_set_tx_rate_adj */



/*===========================================================================

FUNCTION SRCH_GET_RX_AGC

DESCRIPTION    This function returns the current value of the rx agc register.
      For targets using MSM2 or higher, this is simply a passthru for RF
      CAGC function calls.

DEPENDENCIES   srch_dmod_reset should have been previously called.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/

byte  srch_get_rx_agc ( void )
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the calibrated AGC from CAGC section */
  return srch_rx_get_rx_agc( DEMOD_CHAIN );

} /* srch_get_rx_agc */


#ifndef FEATURE_SRCH_SINGLE_ANTENNA
/*===========================================================================

FUNCTION SRCH_GET_RX1_AGC

DESCRIPTION    This function returns the current value of the rx1 agc register.
      For targets using diversity receivers, this is simply a passthru for RF
      CAGC function calls.

DEPENDENCIES   srch_dmod_reset should have been previously called.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/

byte  srch_get_rx1_agc ( void )
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the calibrated AGC from CAGC section */
  return srch_rx_get_rx_agc( ALT_CHAIN );

} /* srch_get_rx1_agc */
#endif /* FEATURE_SRCH_SINGLE_ANTENNA */


#ifdef FEATURE_SEC
#ifdef FEATURE_SEC_CALLS_SEARCHER_RANDOM
#error code not present
#endif /* FEATURE_SEC_CALLS_SEARCHER_RANDOM */
#endif /* FEATURE_SEC */


#ifdef FEATURE_DH

/*===========================================================================

FUNCTION SRCH_START_RAND_BUF_FILL

DESCRIPTION    This function sets the appropriate flag to start filling the
      buffer with random bits, necessary for DH task.  It also grabs the
      start time of the process.

DEPENDENCIES   None.

RETURN VALUE   Good status.

SIDE EFFECTS   None.

===========================================================================*/
srch_cmd_status_type srch_start_rand_buf_fill ( void )
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* if the buffer is already full then bail out */
  if (srch_rand_buf_cnt >= SRCH_MAX_RAND_BUF_SIZE)
  {
    MSG_HIGH( "Akey start buf fill", 0, 0, 0 );

    srch_rand_buf_cnt = 0;
      /* reset the buffer counter */

    srch_akey_start_time = timetick_get();
      /* Begin timing the akey buf process for debug purposes */

    return ( SRCH_DONE_S );
      /* Return good status */
  }
  else
  {
    MSG_ERROR("Akey rand fill in progress",0,0,0);
    return ( SRCH_ERR_S );
      /* Return good status */
  }

} /* srch_start_rand_buf_fill */

/*===========================================================================

FUNCTION SRCH_PEAK_FILL_RAND_BUF 

DESCRIPTION     This function takes randomsearch peak data and loads it into a
      buffer for use by DH

DEPENDENCIES    None.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/
void srch_peak_fill_rand_buf
(
  srch_sect_result_type *results,   /* pointer to a sector to take data from */
  uint32                 num_peaks  /* number of peaks to use */
)
{
  uint32 i;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* if the buffer is already full then bail out */
  if (srch_rand_buf_cnt >= SRCH_MAX_RAND_BUF_SIZE)
  {
    return;
  }

  /* loop over the number of peaks until 
     we run out of peaks or the buffer is full */
  for( i = 0;
       ( i < num_peaks ) && ( srch_rand_buf_cnt < SRCH_MAX_RAND_BUF_SIZE - 1 );
       i++)
  {
    srch_rand_buf[srch_rand_buf_cnt++] = (results->max_lecio[i] & 0xFF);
    srch_rand_buf[srch_rand_buf_cnt++] = (results->max_pos[i] & 0xFF);
  }

  if (srch_rand_buf_cnt >= SRCH_MAX_RAND_BUF_SIZE)
  {
    (void)rex_set_sigs( &dh_tcb, DH_SRCH_RAND_SIG );
      /* Set signal for dh task when buffer is full */

    srch_akey_end_time = timetick_get();
    srch_akey_delta_time = timetick_diff(srch_akey_start_time,
                                         srch_akey_end_time,
                                         T_MSEC);

    MSG_HIGH( "Akey buf fill took %ld ms", srch_akey_delta_time, 0, 0 );
      /* Output akey buf time in milliseconds */
  }
} /* srch_peak_fill_rand_buf */

#endif /* FEATURE_DH */

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                S E A R C H   T A S K  --  G E N E R A L

GENERAL DESCRIPTION
   This module contains the externally visible functions of the Search
   task, regional functions and variables used throughout the task, local
   variables and functions used by this module, and the Search task's
   initial startup functions and main state machine loop.

   The srch module manages the acquisition and demodulation of the CDMA
   signal, including pilot set management.  This functionality is divided
   among various states, as follows:
                               _________________________________
                              |           /|\           |       |
                              |            |            |       |
                             \|/           |            |       |
     POWERUP -> INACTIVE -> CDMA -> ACQ -> SC -> SL -> PC ---> TC
                              |                         /|\    /|\
                              |                          |      |
                              |                         \|/     |
                             \|/                      ZZ or PCG |
                              TT________________________________|

   Scoping and encapsulation follows the general hierarchy of files. All
   Search modules have local variables and functions prefixed by the label
   "srch<+state>_*", for example, "srchtc_ref_time".  Each module exports
   regional functions, for example "srch_sc_init()" and "srch_sort_sect()",
   but none exports regional variables except SRCH.C.


EXTERNALIZED FUNCTIONS
   Global Procedures [External to the Search task]:
     srch_task             Entry procedure for the Search Task
     srch_cmd              Place a command onto the Search Task's command queue
     srch_set_ta_parm      Set parameters controlling Temporal Analyzer logging.
     srch_log_agc_pctl     Log receive AGC and power control values.
     srch_get_cdma_rssi    Compute and return CDMA RSSI.
     srch_get_best_rssi    Return CDMA RSSI based on best-pilot Ec/Io.
     srch_get_pilot_set    Get status of Search pilot sets.
     srch_get_pilot_ecio   Get Ec/Io of Search active pilot sets.

   Regional Procedures [Internal to the Search task]:
     srch_get_freq_est     Retrieve the frequency estimate from internal table.
     srch_store_freq_est   Store the frequency estimate into internal table.
     srch_freq_timer_done  Callback timer routine for frequency estimate timer.
     srch_powerdown        Perform powerdown cleanup.
     srch_eng_to_ecio      Convert energy to CAI 6-bit Ec/Io value.
     srch_ecio_to_eng      Convert CAI 6-bit Ec/Io to energy value.
     srch_log_agc_pctl_ena Enable AGC/pctl logging if DIAG calls for it.
     srch_set_parm         Get the over-air parms and fill in regional parms.
     srch_rpt              Send a report to MC.
     srch_wait             Wait on REX sigs, kicking the watchdog.
     srch_get_cmd          Get a command from MC-SRCH command queue.
     srch_post_cmd         Post process an MC-SRCH command and return status.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Yeah, right.

  Copyright (c) 1990 - 2009
                by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/1x/srch/rel/1h08/src/common/srch.c#5 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/29/09   bb      Update all neighbor pilots window size with neighbor
                   widow size value received in the SRCH_PARM_F in the
                   traffic state for neigbor search mode "00" or "01" 
		   cases
07/22/09   adw     Call srch4_1x_deregister() directly when exiting service.
06/27/09   adw     Deregister srch4 when exiting service.
01/30/09   sst     Added srch_rx_release_owned_chains()
10/01/08   aps     Revert previous change
09/15/08   sst     Remove sleepctl_init for qsc60x5 hw rev 1.0
08/18/08   adw     Added SINGLE_ANTENNA to featurize alt chain for ULC.
                   Aadded FEATURE_SRCH_HAS_NO_TRM to remove TRM for ULC.
08/15/08   mca     Added cdma2kdiag_init()
08/13/08   vks     Added FEATURE_SRCH_HAS_NO_IRM for ULC
08/12/08   adw     Enable QPCH for QSC1100.
07/11/08   aps     Temporarily disable sleep for T_MDM7800 w/o using dip switch
07/01/08   sst     Enable sleep for QSC1100 (QPCH still disabled)
03/14/08   sst     Temporarily disable sleep and QPCH for QSC1100
03/11/08   sst     Temporarily disable the fing disable for QSC1100
11/09/07   aps     Disable sleep for T_MDM7800
05/31/07   aps     Enabling QPCH for QSC6055
05/14/07   sst     Corrected QSC60X5 featurization for initializing sleepctl
05/07/07   aps     Additional QSC60X5 featurization for initializing sleepctl.
05/02/07   cab     Moved to new sleep voting
05/01/07   aps     Lint Fixes
04/27/07   aps     Switch off sleep for QSC60X5 only for MPM bug not fixed
04/20/07   tjc     Clean up of traffic 1.5
04/12/07   tjc     Remove GPS's ability to set the PN roll ISR with
                   FEATURE_CGPS_IF
03/15/07   sst     Disable sleep and QPCH for QSC60X5
03/14/07   awj     Correct featurization for CGPS
02/26/07   tjc     Correct featurization
02/15/07   rkc     Featurize GPS and CGPS header file includes.
01/31/07   sst     Change T_QSC6055 to T_QSC60X5 to catch all tiers
01/23/07   rkc     Remove call to defunct srch_srm_init().
01/17/07   sst     Remove XO_TRIM initialization, moved to tcxomgr
12/19/06   sst     Merge the following changes from 11.02.02B branch
12/12/06   sst     Initialize the XO_TRIM to an initial known good value for
                   QSC6055 surf
11/21/06   sst     Featurized out call of sleepctl_init for QSC6055 and
                   !FEATURE_MPM_HW_BUG_FIXED
12/15/06   tjc     Modified traffic to use the sector module
12/14/06   pa      Removed srch_get_freq_est() and srch_get_temp_est()
10/18/06   bn      Added a new TT state machine to support timming transfer from
                   DO to 1x during DO VOIP to 1x handoff
09/22/06   trc     Wrap GPS/AFLT calls with FEATURE_GPS_MODE/FEATURE_GPSONE_PPM
08/04/06   tjc     Remove unused variables
07/11/06   tjc     Implemented searcher peak module
07/03/06   awj     Moved pilot_rec_type to sect parms, removed parm translation
06/26/06   pa      Removed srch_afc.h and srch_srm.h to pack correctly.
06/13/06   pa      Added additional comment for previous checkin.
06/13/06   pa      Call pgi_init() after tcxomgr is initialized.
06/06/06   pa      Initialize srch srm and afc modules
06/01/06   tjc     Removed subsignal functions
05/15/06   kwo     Added message for CDMA rssi
05/12/06   awj     Updated sector structure.
05/01/06   tjc     Stripped FEATURE_SEARCH1 and mainlined FEATURE_SRCH_PC2
02/02/06   awj     Reworked srch_aset_walsh and srch_get_pilot_set to not
                   rebuild dirty lists in sect.
01/16/06   awj     Lint cleanup
12/21/05   pa      Skip over unsupported band/chan in system measurement.
12/09/05   bt      Tell IRM what resources are avail. upon start of srch_task.
11/28/05   awj     In srch_get_pilot_set(), only check active pilots
11/14/05   awj     Lint compliance
11/07/05   kwo     Removed srch_fing_force_deassign_now()
11/02/05   awj     Stripped FEATURE_SRCH_PCG, mainlined RTL and IRM
10/25/05   awj     Moved NV support to srch_nv.[ch]
10/10/05   awj     Moved srch_report_frame to srchtc.c
09/23/05   ejv     Deprecate rex timer types/calls.
                   Remove unused variables, tables, and functions calls.
                   Remove call to srch_util_config_msm - functionality is
                   included in srch_fing_init.
09/22/05   ejv     Use lecio for sys_meas.
09/21/05   ddh     In srch_get_best_rssi return min RSSI if sect_loaded and
                   sect_a is NULL.
09/20/05   bt      Use srch_rx_get_home_rx_agc instead of srch_get_rx_agc.
09/20/05   pa      Lint fixes
09/14/05   ejv     Use static for local vars.
09/12/05   ejv     Removed unused srch_rssi_db_tab.
09/08/05   ejv     Use static in place of LOCAL.
                   Remove unused srch_freq_filt_n.
08/31/05   ejv     Update FEATURE_SRCH_PC2 featurization.
08/09/05   grl     Added SOOS threshold NV read.
08/05/05   ejv     Remove srch_rssi_to_db.
08/03/05   ejv     Mainline FEATURE_GPSONE_PPM.
08/02/05   sst     In srch_exit_service properly cancel requests
                   and release all TRM chains
07/28/05   ejv     Extend lecio support throughout SRCH task.
07/19/05   grl     Moved to simplified tuning interface with built in enable.
07/19/05   ejv     Upgrade check for 6800 r1.0.
07/14/05   sfm     Name changes related to srch_afc/srch_tcxo merge
                   and mainlined FEATURE_1X_SRCH_USES_ROTATOR_AFC and
                   FEATURE_TCXOMGR
07/13/05   ejv     Move functions called by MC only to srchmc.c.
07/11/05   awj     Added message payloads.
07/08/05   ejv     Mainlined FEATURE_GPS_MODE.
07/01/05   ejv     Removed unused ecio_thresh_eng.
                   Added support for 6800B.
06/30/05   bt      Only kick the dog when we have to.
06/29/05   br      Moved PPM init after the task start.
06/24/05   ejv     Moved ref, ctr, and ppsmm timers to srchtc.c.
06/24/05   ejv     Remove srch_tune_rfm and move srch_freq_timer_done to pc.
06/22/05   sfm     Replaced rfm_ function calls with srch_rx function calls
                   and removed srch_acq_timer and srch_rf_tune_timer
06/14/05   ejv     Use higher-layer CONV macros.
06/10/05   ejv     Call srch_fing_get_max_fing_cnt to get MAX_FING_CNT.
06/09/05   ejv     Added void return to srch_rx_tune.
06/06/05   br      Mainline feature GPSONE_PGI2.
06/06/05   bt      Temp. disable QPCH for MSM6700++.
06/03/05   sfm     srch_rx naming changes
06/02/05   grl     Changed srch_tune_rfm to not wait for the tune to complete.
06/01/05   grl     Removed "sam.h" include since it has been replaced by TRM.
06/01/05   grl     Temporarily added srch_tune_rfm for calls from mccidl.c
05/31/05   rng     Merged from SHDR Sandbox.
05/27/05   rng     remove initialization of tcxo pdm during start up.
05/24/05   ejv     Remove srch_fing_driver.h.  Remove unused variables.
                   Moved other variables into appropriate files within the
                   library.  Call library interface functions to access
                   fingers.  Mainline calls to srch_fing_hw_merge_detect.
05/20/05   ejv     Remove srch_pn.h.
05/19/05   ejv     Remove FEATURE_ACP.
05/18/05   kwo     Include updates for library packing
                   Relocated srch_rf in the finger driver
                   Relocated srch_rpt_ta in srch_diag
05/17/05   sst     Changed window size in sys_meas to FULL_CIRCLE
05/16/05   sst     Removed Acq1 and FEATURE_ACQ1_AND_ACQ2
05/04/05   kwo     Cleaned up includes
04/25/05   kwo     Merge in PC2
04/01/05   sfm     Changed usage of srch_fings[].locked to boolean
03/18/05   ejv     Remove call to srch_tc_cfs_timers_init.
03/15/05   ejv     Renamed local "time" variable.
03/07/05   sst     Update VCO<->Rot<->ppm translation factors through srch_afc
                   on each tune
02/17/05   abh/gs  Removed quantization effects on the TrkLo PDM
02/15/05   kwo     Modified filter growing logic in srch_filt_iir()
01/18/05   sfm     Put the body of srch_cmd() into a critical section
12/28/04   bt      Rework of library specific interface.
12/15/04   kwo     Rework of DH interface
12/07/04   sst     Changed srch4_1x_add_task() and srch_sect_srch4_append/_r()
                   to remove offset and add receive diversity field parameters
12/01/04   rng     Removed srch_freq_est_tab and associated functions.
11/29/04   ejv     Move a FEATURE_GPS_MODE check.
11/24/04   ejv     Always include srchgps.h.
11/22/04   aar     Keep the PN roll ISR when GPS session is active.
11/17/04   sfm     Removed change_flags from srch_fings[]
11/17/04   ejv     Added missing FEATURE_SEARCH1.
11/15/04   ejv     Mainlined FEATURE_IS2000_P2.
11/08/04   ejv     Updated srch_get_cdma_rssi - 32 bits for total_aset_eng.
11/04/04   kwo     Removed db_down table
11/03/04   kwo     Removed obsolete fade command support
10/25/04   ejv     Removed srch_gps_handler and srch_update_sch_walsh_len.
10/21/04   sfm     Simplified srch_reset_pilot_info()
10/21/04   rng     Added srch_tcb to tcxomgr_init().
10/15/04   kwo     Removed unslew
10/09/04   dna     Fixed sys_meas SRCH4 FULL_CIRCLE & fatal error on empty list
10/07/04   kwo     Renamed gen log packets to SRCH_LOG_TNG...
09/30/04   br      Inserted PGI function call pgioos_is_ghost_fing() instead of
                   checking internal PGI parameter.
09/30/04   ejv     Added sys meas constants from srch.h.
09/29/04   sfm     Changed include of srch_fing.h to srch_fing_driver.h
09/28/04   jcm     Move sect load/unload responsibility into TC and Old ACQ
09/27/04   ejv     Include srchtc.h.
09/24/04   ajn     Removed srch_get_allow_uP_clock_switch()
09/23/04   kwo     Added support to save off demod data for logging
09/20/04   sfm     Added call to srch_fing_init() in srch_init()
09/16/04   jcm     Featurized AFC with FEATURE_1X_SRCH_USES_ROTATOR_AFC
09/13/04   sst     Integrated with ACQ2.0 dev, featurized as FEATURE_ACQ2
09/13/04   kwo     Added srch_get_channel_estimate() & srch_in_service()
09/09/04   ejv     Removed REL_C/D_DV featurization.
09/08/04   jcm     Rename searcher1 functions srch1_
09/03/04   sfm     Finger Driver changes
09/03/04   kwo     Advanced support for lecio
09/03/04   jcm     Modified genlog to log multiple packet simultaneously
08/31/04   ejv     Update srch_reset_pilot_info with new channels.
08/27/04   dmw     Added support for genlogging in srch_rpt_ta()
08/27/04   ejv     Mainline more features.
08/24/04   br      Added support for GPS OOS search.
08/24/04   ejv     Removed srch_win_pool and srch_win_q.
08/23/04   ejv     Change ts_ function calls to new time_ calls.
08/23/04   rng     Call tcxomgr_init and _powerdown.
08/23/04   kwo     Modified srch_filt_iir() to support uint32
08/18/04   gs      Added srch_get_freq_range function.  srch_tune_rfm now
                   updates AFC gains if the frequency range changes, and
                   uses the updated IF for srch_fing_set_dmod_pilot_gain.
08/17/04   kwo     Added powerup to srch_state_str
08/12/04   dna     Transitions to SRCH_START_STATE now go to SRCH_CDMA_STATE
08/11/04   dna     Merged a sys_meas fix from MSM6100 branch #20.
                   Don't abort SRCH4 if it isn't active (avoids err msg).
08/05/04   sar     Replaced call to rf_get_band_chan() with rfm_get_band_chan()
                   which returns band and channel for the specified device.
08/02/04   ejv     Mainline FEATURE_MULTIMODE_ARCH.  Use new interface to
                   signal MC when a SRCH report is ready.
07/26/04   ejv     Remove srch_msm_version.
07/23/04   ejv     Updates for REL_C_DV.
07/22/04   kwo     Removed finger ecio table
07/19/04   ejv     Mainline FEATURE_IS95B_MAHHO, _SOFT_HO, and SYS_RESELEC.
07/13/04   kwo     Added support for defaulting win_n when using UNLM
07/12/04   sst     Changed prototype for srch4_1x_register() to add reg_cb's
07/08/04   ejv     Remove srch_catnap_ctr_dz.  Replace Deep Sleep in
                   srch_state_str with Inactive.
07/07/04   hxw     Moved GPS MDSP ISR registration into GPSSRCH task.
07/06/04   ejv     LINT cleanup.
06/30/04   dna     Changed SRCH_START_STATE to SRCH_INACTIVE_STATE.
                   Added srch_exit_service(), called when we enter Inactive
                   state or when we return to CDMA state from any state other
                   than inactive.
06/28/04   ejv     Moved srch_lib_check_mdsp_ver to after MDSP download.
06/25/04   jcm     Add run-time check to ensure MDSP image used by Search
                   Library matches, also print Library version during boot
06/24/04   ejv     Add missing comma and update var names for LINT warning.
06/22/04   br      Moved feature PDSM_SRCH_INTERFACE. Removed feature
                   GPSONE_ACQ_MODE.
06/17/04   ejv     Remove srch_report_tc_msg.
06/17/04   ejv     Mainline FEATURE_USE_TIME_VU.
06/18/04   dd      Removed GPS NV support.
06/16/04   jcm     Moved TC Specific functions to srchtc_util.h/.c
06/08/04   kwo     Updated window size error reporting
06/02/04   ejv/rdh Interface functions to update new log packets.
                   Removed unnecessary NULL check (always FALSE).
05/28/04   jcm     Sector module integration
05/27/04   ajn     Switched from sleep1x to sleepctl.
05/27/04   ejv     Mainline FEATURE_MDSP_USES_MODULES, FEATURE_MDSP_TASK &
                   FEATURE_SYSTEM_ARBITRATION.
                   Renamed FEATURE_ENHANCED_TRIAGE as SRCH_ENHANCED_TRIAGE.
05/26/04   ejv     In srch_rf_parms, map diversity pilots and first 2 SCCHs
                   to accumulators for Ant1.
05/14/04   kwo     Changed return type of srch_set_parm()
05/04/04   sst     Merge from srch4 3Q dev branch
                   Added partial results param to srch4_1x_abort_search()
                   Added support for srch4 (de)registering
04/22/04   sfm     More changes for traffic channel manager
04/21/04   sfm     Changes for traffic channel manager
04/09/04   bt      Use new RF API in srch_tune_rfm.
04/08/04   ejv     Mainline SRCH_HAS_TRK_LO_CNTL and SRCH_HAS_PILOT_GAIN_CTL.
                   Removed srch_tune_to_chan.
04/05/04   jcm     Removed unused functions: srch_get_rset_pilot(),
                   srch_is_asleep(), srch_get_ref_pn()
04/02/04   ejv     Removed srch_hho_in_progress.
03/25/04   ejv     Mainlined FEATURE_MULTI_RX_CHAIN.
03/23/04   ejv     Support for parms per antenna by adding srch_rf_parms,
                   srch_rf and function srch_set_ant_status().
03/11/04   bt      Added srch_idle_init.
03/08/04   ejv     Changed wrapping around srch_gps_handler from T_MSM6500
                   to T_QUASAR.
03/04/04   ejv     Added call to srch_tc_cfs_timers_init() and
                   srch_tc_get_off_time_fwd().  Remove srch speed from
                   call to srchtc_cfs_num_srch().
03/01/04   kwo     Correct more lint errors
02/24/04   ejv     Still more FEATURE_SEARCH1.
02/23/04   ddh     Corrected Lint errors
02/20/04   ejv     Added more FEATURE_SEARCH1 checks.
02/20/04   sfm     Removed change_flags from srch_reset_pilot_info()
02/17/04   ejv     Added FEATURE_SRCH_PCG around SRCH_PCG_STATE.
02/17/04   ejv     Added FEATURE_IS2000_REL_C_DV.
02/11/04   ejv     Mainline FEATURE_IS95B.
02/04/04   br      Added separate free queue for PPM search request buffers.
02/02/04   sst     Added synchronous field to srch4_1X_abort_search()
02/02/04   ejv     Removed unused variables and FEATURE_FAST_FINGER_SLEW.
01/30/04   kwo     Changed pc_ho_timer and pc_throt_timer to not use sigs
01/28/04   ejv     Mainline T_MSM6500.
01/21/04   sst     Check for max allowable active set win siz by the standard
01/21/04   jcm     Removed reminents of the delay timer
01/16/04   ejv     Remove FEATURE_SEARCH2, FEATURE_SRCH_SCHEDULER, and
                   FEATURE_SRCH_SCHEDULER_MONITOR.  Changed SRCH2_SW_Q_LEN
                   to SRCH_SW_Q_LEN.
01/12/04   ejv     Added FEATURE_SEARCH1.
01/09/04   gs      Added srch_last_good_win_siz definition and added
                   srch_sect_find_any_freq.
01/06/04   sst     Added parameter age_searches to srch4_1x_add_task calls
                   to facilitate proper use of search sector aging
01/05/04   ejv     Removed unused srch_clr_timer, srch_set_timer,
                   srch_tick_timer and srch_tick_expired.
12/30/03   sm      Added calls to rex_def_timer for srchtc_cfs_period_timer and
                   srchtc_cfs_visit_timer
12/19/03   kwo     Cleaned up some ZZ1 style slot tracking vars
12/12/03   dd      Featureized for GPS Search Scheduler 2.
12/10/03   jcm     Added srch idle util initialization
12/04/03   aar     Removed RTC_ROLL ISR installation; it's currently unused.
12/04/03   sst     Fixed srch_populate_triage_log_fing_entry() to correctly
                   check flags
                   Misc cleanup in triage logging
                   Allow triage logging in ZZ state
11/20/03   sst     Added changes to correct compiler warnings for RCVT
                   Changed magic num to use def NO_BAND/FREQ_TUNE
11/19/03   bmg     srch_tune_rfm changes to support second chain call test
11/05/03   sst     Added calls to wake up trk_lo_adj
11/04/03   sst     Changed prototype for srch_tune_rfm(), added check for
                   recieve diversity through srchrdiv in srch_tune_rfm()
10/28/03   ddh     Added changes to correct compiler warnings for RCVT
10/24/03   sst     Added function srch_tune_rfm() to act as the tuning
                   function for all tasks, this function calls rf tune
                   functions, changed RF tuning calls to use this
10/16/03   kwo     Removed FEATURE_ZZ2_OFREQ_TIMER
10/09/03   sfm     Updated for new state machine paradigm
10/10/09   bt      Moved srch_comb_post_slam_tt_fing into srch_comb.c
10/07/03   kwo     Added FEATURE_ZZ2_OFREQ_TIMER
10/06/03   abh     Proper default values for rf config for GPS in baseband
                   mode.
09/30/03   ejv     Modified part of srch_multimode_sanity_err_fatal.
                   Remove obsolete feature FEATURE_GPS_DISABLE_CHIPX8_SLEEP.
                   Mainlined FEATURE_GPS_SRCH_ENGINE.
09/29/03   ajn     Remove PMIC RTC -> clk_set_ms.  Now performed by TIME VU.
09/26/03   ejv     Remove old/unused variables and functions.  Update function
                   srch_add_nset_pilot to have the same alg for CELL as PCS.
09/24/03   ejv     Remove check of sch_info.num in srch_update_sch_walsh_len.
09/24/03   sst/kwo Fixed srch_fing_v2 packet length
09/16/03   dd      Added support for NV_GPS_RF_MODE (Rx chain and Rx mode).
09/10/03   aaj     Added support for RSSI linear to dB conversion function
09/08/03   aaj     Added interface to MC to report Rx Div capability
09/04/03   aaj     Read CDMA Rx Div NV item in srch_task()
08/20/03   aaj     Support for reading NV item for Rx Diversity
08/18/03   br/gs   Added SRCH_PDSM_GET_GPS_INFO_F to srch_proc_pdsm_cmd
08/13/03   gs      Add function srch_multimode_sanity_err_fatal
08/05/03   ejv     Added FEATURE_IS2000_P2 back in to logging.
07/28/03   ejv     Mainline FEATURE_ZZ2.
07/29/03   abh     Changed MDSP interrupt mapping for GPS for MSM6500.
07/25/03   ejv     Add FEATURE_MULTI_RX_CHAIN.
07/24/03   ejv     Added new function srch_update_sch_walsh_len.
                   Moved srch_ch_rc_type typedef to srchi.h.
07/17/03   kwo     Move srch diag requests into their own module
07/14/03   ejv     Remove FEATURE_SPECIAL_MDR.
07/07/03   ejv     Mainline FEATURE_ANTIREGISTRATION_BIAS.
                   Mainline FEATURE_ENHANCED_STANDBY_III.
                   Mainline FEATURE_TRIAGE_LOGGING.
07/01/03   kwo     Added message to report macro search state transitions
06/23/03   ejv     Remove FEATURE_EXT_LINEARIZER and FEATURE_SEARCH3.
06/19/03   ejv     Remove old log pkts and update interface to srch_rpt_ta().
06/11/03   rdh     Streamlined search governor.
05/29/03   kwo     Moved wakeup/active report messaging to srch_idle_util()
05/20/03   rdh     Added Gen Pilot Sets packet, mainlined JCDMA monitor.
05/02/03   br      Centralized PPM PDSM command handling in PPM module. Added
                   PPM module initialization.
04/28/03   ejv     Code cleanup.
04/28/03   sst     Renamed T_ACP to FEATURE_ACP
04/25/03   sst     Added support for searcher's 1 & 4 for sys_meas
                   Mainlined FEATURE_SD20 and SRCH_ENABLE_SD20
04/22/03   br      Added support for sending A-set and C-set to PDSM.
04/16/03   aar     Installed 1X RTC ROLL ISR.
04/09/03   kwo     srch_chan_init was changed to srch_chan_reset
04/03/03   bt      FEATURE_ZZ2 cleanup and removal of QPCH PLT support.
03/25/03   ejv     Added function srch_stop_1x_searches().
03/26/03   gs      Remove setting of the combiner symbol size in
                   srch_idle_chan_config.
03/20/03   sst     Fixed order of init and definition of queues in srch_init
03/20/03   rm      Handle the case where log ptr is NULL in
                   srch_rpt_triage_results to avoid memory scribble.
03/05/03   kwo     Added support for l1m
02/19/03   zg      Added support for FEATURE_MDSP_USES_MODULES.
02/18/03   va      SRCH4 AFLT support.
02/14/03   kwo     Added Finger RSSI to ecio table
02/11/03   kwo     Mainlined srch_reset_pilot_info
02/05/03   gs/kwo  Add in Rel A support for ZZ2.
02/05/03   gs/br   Added FEATURE_GPSONE_ACQ_MODE to srchgps.h include
02/03/03   gs      Merge in from MSM_SRCH.08.06.60, specifically removing
                   unused GPS code.
01/28/03   sst     Encapsulated srch_win_center to srch_hw.c/h
01/23/03   gs      Fix issue with old log packets trying to stuff 6 fingers
                   worth of info in them.
01/17/03   bt      Merge FEATURE_ZZ2 from SRCH_SLOTTED_DEV.00.02.00.
01/14/03   gs      Merge with MSM_SRCH.08.06.57
01/13/03   kwo     Added initializion of srch stm
01/07/03   sst     Removed freq_incl from sect struct, All pilots now have
                    freq and band class info.
12/05/02   sst     Changed include from srch4drv.h to srch4trans.h, and
                   searcher 4 init call is not to the trans layer
01/03/03   ejv     Fixed typo in Rel A code.
12/26/02   ejv     Cleanup.
12/23/02   ejv     Changed DOG_RPT to SRCH_DOG_RPT.
12/12/02   lad     Fixed triage logging to handle MAX_FING_CNT > 4.
                   Added version 2 of the triage logging packet.
12/03/02   pj      Partial merge with 5500 development (MSM_SRCH.08.06.52).
12/02/02   ejv     Additional EBCSMS changes.
11/26/02   bt      merge FEATURE_ZZ2 from SRCH_SLOTTED_DEV.00.01.03
11/25/02   ejv     Use dynamic BCN for calculating ULPN mask for EBCSMS.
11/25/02   ejv     Mainlined !FEATURE_SRCH_PC_IFACE.
11/22/02   va      Removed code under FEATURE_GPSONE_ACQ_MODE
11/12/02   br      Move 'Start PPM' command processing to
                   srch_ppm_proc_start_ppm_cmd(). Added input parameter to
                   srch_ppm_proc_get_ppm_res_cmd().
11/12/02   gs      Finish MSM_SRCH.08.06.47 merge.
11/05/02   gs      Partial merge up to MSM_SRCH.08.06.47.
10/09/02   va      Removed processing of GPS SRCH trigger signal
10/04/02   sst     Added support for 4 & 6 searcher peaks
10/03/02   bt      Added srch_get_allow_uP_clock_switch
10/02/02   gs      Merge up to MSM_SRCH.08.06.42
09/30/02   sst     Added searcher 4 initialization
09/16/02   va      Registering GPS app for MDSP moved later, to ensure MDSP
                   task is running when the app is registered.
09/11/02   va      Added support for GPS on MDSP. Removed obsolete signal from
                   srch_gps_state
08/05/02   gs      Merged in support for mdsp image download.
07/30/02   gs      Initial merge into perforce for cougar.
06/28/02   ejv     Changed INTx_SAV macros to just INTx.
06/28/02   ajn     Replaced magic numbers with identifiers
06/26/02   ejv     Removed idle task priority setting for DYNAMIC_THROTTLE.
06/25/02   aar     Added function srch_get_srchdz_chipx8_sleep_allowed().
06/03/02   aar     Replaced srch_get_srchzz_chipx8_sleep_allowed by srch_get_
                   srchzz_keep_chipx8_on.
05/31/02   ejv     Removed function srch_get_vc_osc_ctl().
05/31/02   ejv     Allow caller to set ULPN mask for PLT.
05/30/02   aar     Added function srch_get_srchzz_chipx8_sleep_allowed().
05/21/02   ejv     Mainlined FEATURE_IS2000.
05/20/02   nng     Added FEATURE_PMIC_RTC.
05/14/02   gs      Added Candidate Frequency Neighbor Set to initializations
                   in srch_reset_pilot_info.
05/09/02   abh     Added support for Low mode tests in GPS Baseband setup.
05/05/02   ejv     Removed duplicate variable declaration.
05/02/02   ejv     Removed FEATURE_QUARTER_ROLL_TIMELINE.
04/24/02   rdh     Centralized Rx calculations and eliminated PCS fudge factor.
04/24/02   br      Added gpsOne PGI initialization.
03/21/02   br      Removed FEATURE_SRCH_BATCH_LOG. Added proper GPS and PPM
                   feature names. Cleanup.
03/16/02   br      Added support for the new GPS software architecture.
03/15/02   ejv     Don't always print error message for win_n > 15.
03/13/02   ejv     BCN for primary BCCH is changed to 1.
03/05/02   ejv     Check srch_hsd_call_active flag for setting throttle duty
                   cycle.  Added function srch_notify_sch_active().
03/04/02   ejv     Changed srch_set_sym_siz to srch_scomb_set_siz.
03/01/02   aar     Changed srch_get_rf_delays to srch_get_gps_nv_items and
                   added support to retrieve GPS_DOPP_SDEV_I NV item.
02/27/02   ejv     Featurization around "bcch_supported" is changed.
02/26/02   ejv     Added missing comma.
02/24/02   va      Added ifdef for T_MSM6050 around call to rf_is_lna1_on
02/22/02   ejv     FEATURE_IS2000_REL_A_NCC -> FEATURE_IS2000_REL_A_CC.
02/21/02   ejv     Added FEATURE_SRCH_PC_IFACE and FEATURE_IS2000_REL_A_NCC.
02/20/02   ajn     Added rf_complete_warmup after rf_init_digital to ensure
                   rf_init_digital will be fully completed.
02/12/02   abh     Updates in the functionality to perform GPS operation while MS
                   is in Acquisition State (for performing multi-visit operation).
02/12/02   abh     Merged GPS code. Also, added code to be able to go
                   to GPS mode while Searcher is in Acquisition state.
02/11/02   rm      Fixes for loading the long code mask for QPCH (MSM6050).
02/06/02   ajn     Request SAM lock immediately upon task start.
01/30/02   ejv     Removed FEATURE_JSTD008_DEBUG.
01/21/02   ejv     Set nghbr_config for Rset nghbrs moving into Nset.
01/16/02   ejv     Added T_MSM6000.
01/15/02   ejv     Added min_time back into srch_scan_meas_list_member().
11/02/01   sst     Modified to incorporate new srch files from the srchdrv.c
                    breakup
12/18/01   ejv     Added FEATURE_RF_WAIT_CHANGES.
11/30/01   ejv     Added check for NO_SECTOR for fing in Ver2 log.
11/21/01   br      Changed status dump occurance type for SRCH scheduler.
11/15/01   ejv     Changed FEATURE_TRIAGE_DEBUG to FEATURE_TRIAGE_LOGGING.
11/14/01   ejv     Changed a MSG_ERROR to MSG_LOW.
11/12/01   pj      Made changes to srch_gps_get_rf_delays and added
                   srch_get_gps_ant_offset.
11/05/01   gs      Fix CFS throttling bug introduced below.
11/01/01   gs      CFS searches no longer throttle.
10/03/01   ejv     Moved triage debug packet structure to srchlog.h.
10/03/01   ejv     Added new log packet for Rel A.
10/01/01   rm      Fixed compile warnings in srch_rpt_triage_results.
08/17/01   rm      Added new logging for reporting triage decisions and also
                   enabled hardware finger merge detection in SYNC, PAGING and
                   TC state when implementing fat path algorithm.
09/25/01   ejv     Added new function srch_agc_to_dbm().  Added additional
                   FEATURE_IS2000_REL_A support.
09/10/01   aaj     Added QPCH ULPN mask changes for MSM5500
08/21/01   ejv     Changed occurrence of srch_fing_cnt to MAX_FING_CNT.
08/07/01   br      Fixed subsignal timing problem.
07/26/01   ajn     SLEEP VU - Remerge with Search Tip
07/06/01   br      Added support for abort_srch_callback in SRCH scheduler.
06/21/01   ajn     SLEEP VU: 1x sleep controller initialization added.
06/25/01   ejv     Reset the QOF mask for DCCH.
06/21/01   ajn     SLEEP VU: sleep clock management moved to SCLK.*
06/20/01   ejv     Mainlined FEATURE_SRCH_SLEW_ IN_DRV.
                   Removed srch_winsize_slew.
                   Removed FEATURE_DIAG_V7_LOG_IFACE.
                   Moved setting of srch_fings_common to srch_config_msm().
06/14/01   ejv     Comment cleanup to avoid some confusion.
06/13/01   ejv     Pass correct parameter to calculate eng from ecio.
06/07/01   ejv     Replaced magic number with appropriate define.
06/05/01   ejv     Created FEATURE_FAST_FINGER_SLEW and replaced some T_MSM5000
                   checks with FEATURE_IS2000.
06/05/01   rdh     Consolidated and fixed temperature estimation functions.
05/28/01   pj      Added srch_srch3fw_ready_timer.
05/18/01   br      Added support for subsignals. Cease sharing srch_gps_timer
                   between GPS and PPM modes. Changed search duration
                   determination mechanism for setting lost search dump timer.
05/08/01   ejv     Added missing #ifdef T_ACP to avoid compiler warnings.
05/03/01   ejv     Replaced abs with ABS.
05/01/01   br      Added SRCH Scheduler support. Moved searcher slewing to
                   SRCH drivers.
05/01/01   ejv     Moved SLEEP_PRI_HI to task.h.
04/26/01   rdh     Changes for 8X searching in all states, code streamlining.
04/10/01   ejv     Added new function srch_start_rand_buf_fill for FEATURE_DH.
03/29/01   ejv     Added FEATURE_IS2000_P2.
03/16/01   ejv     Removed ULPN mask changes for MSM5100.
03/14/01   ejv     Removed update of a finger parameter.
03/12/01   dgy     Added FEATURE_IS2000_QPCH_CCI.
02/28/01   ejv     Small code cleanup.
02/27/01   ejv     Added FEATURE_IS2000_REL_A.
02/27/01   ejv     Removed srch_reacq_mstr_posn_set.
02/27/01   ejv     Include test_mc.h for FTM.
02/27/01   br      Added FEATURE_SEARCH3 to featurize searcher 3 operation.
02/27/01   rdh     Fixed struct copy and missing sect_free for SD2.0.
02/26/01   sko     Added srch_get_ref_pn().
02/26/01   ejv     Added SCH active channel reporting.
02/23/01   ejv     Moved function srch_set_rssi_timer.
02/15/01   bgc     Changed FTM to a runtime mode.
02/15/01   rdh     Added handling for SD2.0 when ACP not defined, shuffled
                   sys meas command struct members to conform to MC convention.
02/14/01   ejv     Added changes for setting the ULPN mask on MSM5100.
02/14/01   rdh     Added initial support for System Determination 2.0.
02/08/01   ejv     Removed loop check for log codes in srch_rpt_ta().
02/07/01   dgy     Added a variable for PC framing type.
02/02/01   br      Added FEATURE_GPS_MODE to featurize GPS operation.
01/31/01   br      Renamed FEATURE_GPS_MODE into FEATURE_GPS_SRCH_ENGINE.
01/26/01   ych     Merged T-53/JCDMA features for SK.
01/25/00   abh     Changes to read the GPS RF Loss calibration value from NV.
01/22/01   ejv     Mainlined T_SLEEP.
01/08/01   ejv     Added new sub-structures for channel parameters.
12/11/00   pj      Made sure srch_get_moving_psd() and related variables were
                   defined when GPSONE is defined, not just when 95B is defined.
12/01/00   aaj     Removed FEATURE_GPSONE_TAWD_TEST_ONLY code
11/09/00   ejv     Check the freq_incl flag before logging the band class.
11/06/00   ejv     Added new searcher driver interface.
10/23/00   ejv     Moved definition of srch_total_sleep_dur from srchzz.c.
09/19/00   abh     Added support for using the RF Delay calibration of GPS Measurements
                   according to the values stored in NV.
09/11/00   ajn/dgy Added the QPCH timeline that wakes up before the first PI
           /ejv    bit, goes back to sleep immediately if any of the two PI
                   bits is zero, and stays awake for PCH if both PI bits are
                   non-zero.
09/06/00   bc      Added FEATURE_FACTORY_TESTMODE.
08/24/00   pj      Added support for new QDSP services.
08/21/00   ejv     Removed FEATURE_PILOT_SCANNER and SRCH_PRE_IS95B_BUILD.
08/06/00   br      Added support for SRCH3 lost status dump.
08/04/00   ejv     Added srch_get_slot_status() and 2 last slot qwords.
08/04/00   br      Added additional FEATURE_GPSONE support.
07/20/00   day     Modify Gen TA log to display finger info for
                   FEATURE_GPSONE_TAWD_TEST_ONLY
06/29/00   abh/br  Added MSM3300 support - FEATURE_GPSONE and FEATURE_SRCH_BATCH_LOG.
06/12/00   ejv     Added srch_fpc_enabled flag and function srch_set_fpc_mode.
06/09/00    eh     Added support for search window offset.
05/26/00   ejv     Added slot mask and slot counting variables and function
                   srch_set_slot_status.
05/15/00   rdh     Lowered fing lock_lo from -24.6 to -27.6 dB for better FER.
05/15/00   rdh     Simplified and consolidated fing lock threshold variables.
04/24/00   dgy     Fully wrapped comments under FEATURE_IS2000_SCH.
04/21/00   ejv     Added function srch_udpate_rc and type srch_ch_rc_type.
04/20/00   lad     Added RC to IS2000 logging.
04/17/00   lad     Changed 2 variables in IS2000 logging.
04/11/00   lad     Added new log packet for IS2000.
04/10/00   ejv     Added FEATURE_IS2000_QPCH.
03/16/00   ejv     Added FEATURE_RFR3100.
03/07/00   day     Added FEATURE_PM1000_RTC.
02/25/00   ejv     Changed #ifdef FEATURE_DIAG_STREAMING_OUTPUT to
                   #ifndef FEATURE_DIAG_V7_LOG_IFACE.
02/24/00   ejv     Removed FEATURE_PRE_SUP_WALSH_LOG.
01/27/00   ejv     Added FEATURE_KEYPRESS_OPTIMIZATION.
01/21/00   ejv     Default to PCS band if FEATURE_CDMA_800 is not defined.
01/18/00   ejv     Added #ifdef to fix compile warning.
01/10/00   rdh     Removed window segmenting.
01/10/00   ajn     Re-Added QUARTER_ROLL_TIMELINE support.
12/21/99   rdh     Added FEATURE_ENHANCED_STANDBY_III.
12/20/99   ejv     Changed a SEARCH2 variable type.
11/24/99   ajn     Finger "Change Flags" for Supplemental Channel support.
11/10/99   ejv     Added FEATURE_SEARCH2.
11/10/99   ejv     Changed a variable type for PILOT_SCANNER.
11/08/99   wjn     Added the code that steals tcxo_drift field of GEN_TA and
                   GEN_TA_SUP_CH logging packets for band class logging.
10/21/99   ejv     Changed srch_wait to use rex_sigs_type instead of word.
10/20/99   dgy     Added support for QOF.
10/05/99   wjn     Added code to ship the sparse AGC logs to DM before
                   hard handoff to a different band class.
10/04/99   ejv     Merged FEATURE_DIAG_STREAMING_OUTPUT.
09/30/99   yus     IS95B code review cleanup.
09/22/99   ejv     Cleanup and final removal of MSM_23 checking.
08/23/99   ejv     Mainlined FEATURE_ENHANCED_STANDBY_II.
08/20/99   yus     Changed unit of max_win of the function srch_cfs_off_time_fwd
                   form chips to window size index.
08/12/99   ejv     Need to cast parameter in calls to ts_get if the qword that
                   is being passed is a member of a packed structure, since
                   qword itself is no longer packed.
08/12/99   yus     Added srch_cfs_off_time_fwd function
                   (under FEATURE_IS95B_MAHHO.)
08/12/99   yus     Changed FEATURE_IS95B_CFS and FEATURE_IS95B_HARD_HO to
                   FEATURE_IS95B_MAHHO.
08/06/99   ejv     ajn: Moved FEATURE_DH code out of this file.  Mainlined
                   FEATURE_ENHANCED_STANDBY and FEATURE_VIRTUAL_COMBINER.
                   Added new structure srch_fings_struct_type.  Removed
                   pre-MSM3000 support.  yshi: Added FEATURE_IS95B_CFS
                   support.  Added new function srch_reset_moving_psd.
                   Added FEATURE_PRE_SUP_WALSH_LOG.  Code cleanup.
07/26/99   ejv     yus: Fixed a bug on allocating srch_gen_ta_pool. It use
                   srch_gen_ta_sup_log_struct_type for srch_gen_ta_pool since
                   it is larger than srch_gen_ta_log_struct_type.  Added
                   function srch_aset_walsh for displaying walsh codes on DM.
                   Added walsh codes to Gen MDR TA packet.
07/24/99   jcw     Merged in changes for PLT.
07/23/99   wjn     Added the band class information to identify CDMA channels.
07/20/99   yus     Added the codes to save and restore the over-the-air window
                   size of each individual neighbors in a IS95B system. The over
                   -the-air will be saved in the ota_win_n of the sect structure.
07/19/99   dgy/ml  Added support for PLT.
07/02/99   rdh     Merged jah's 6/14/99 changes (with minor tweaks/comments):
                     "Changed frequency error table lookup to extend the
                      endpoints, rather than interpolate using a default 0ppm.
                      Changed frequency error table sizing to be based on the
                      size from NV (NV_FR_TEMP_OFFSET_TABLE_SIZ)."
07/01/99   yus     De-coupling FEATURE_SPECIAL_MDR with FEATURE_IS95B_PILOT_RPT
                   if SRCH_PRE_IS95B_BUILD is defined.
06/28/99   yus     Merged IS95B features to the ASW tip.
06/10/99   yus     Merged IS95B Hard Hardoff ( w/o Candidate Frequency Search.)
06/09/99   rdh     Merged FEATURE_PILOT_SCANNER, FEATURE_SCAN_FING_LOG.
06/03/99   ejv     Removed calls to rf_get_cdma_rx_agc for
                   FEATURE_EXT_LINEARIZER, since srch_get_rx_agc will call
                   rf_get_cdma_rx_agc anyway.
05/26/99   ejv     Removed function srch_labs, which has been replaced by labs.
05/24/99   yus     Added IS95B Periodic PSMM feature.
05/03/99   ejv     Changed the Gen TA packet reporting to use the stored upper
                   byte of the raw RSSI value instead of the converted value.
04/22/99   ejv     Moved definition of WIN_TAB_SIZ to srchi.h.
04/08/99   ejv     Removed unsed variables max_fing and srch_adj_cnt.
03/25/99   rdh     Cleanup/removal of dead code and test ifdefs which
                   either do not work, affect performance if turned on,
                   or are simply no longer needed.
03/10/99   dgy     Removed code in srch_get_cdma_rssi() that prevented searcher
                   from updating CDMA RSSI in DB while it is in ZZ state.
03/22/99   kmp     Merged the changes listed below from the TGP IS-95B baseline.
                   03/10/99   yus     Added access_attempted field.
02/22/99   yus     Merged IS95B features to ARM based code( see below for the
                   features.)
                   02/04/99   yus     Changed srch_get_pilot_ecio() to return
                                      a half db units.
                   02/03/99   yus     Modified ACCESS_HO and ACCESS_ENTRY_HO
                                      #ifdef properly.
                   11/25/98   yus     Added P_REV checking.
                   11/25/98   yus     Added IS95B Neighbor search enhancements.
                   11/25/98   yus     Added IS95B Access handoff and Access
                                      Entry handoff support.
                   11/25/98   yus     Added IS95B Pilot reporting.
                   11/25/98   yus     Changed #ifdef FEATURE_JSTD008 to run time
                                      checking.
                   09/23/98   yus     Added IS95B soft handoff algorithm.
                   09/23/98   yus     Added IS95B system reselection in Idle
                                      feature.
                   09/22/98   yus     Modified srch_get_pilot_ecio to return
                                      half db unit.
02/03/99   ejv     yus: Merge updates for MDR features.
02/03/99   ejv     yus: Added IS95B MDR support.
02/02/99   ejv     Changed T_VIRTUAL_COMBINER to FEATURE_VIRTUAL_COMBINER.
02/01/99   ejv     Merged changes for FEATURE_ENHANCED_STANDBY_II, including
                   srch_get_fing_rssi, srch_labs and avoid using DMA buffers
                   (rdh).  Also added FEATURE_EXT_LINEARIZER.
01/21/99   ejv     Edited FEATURE_DH - no longer uses DMA.
01/13/99   ejv     ajn: Added knowledge about time reference slamming to
                   srch_comb_slam_timeline & added srch_get_vsc_time( )
01/08/99   ejv     Cleaned up other ARM compiler warnings.
01/06/99   ejv     Removed _cdecl and ROM - not necessary for ARM.
10/16/98   ejv     Merged changes to support Panther.
10/09/98   dgy     Used correct finger lock thresholds for backwards
                   compatibility with MSM2.2.
10/08/98   ejv     Changed FEATURE_NEW_SLEEP_TIMELINE to
                   FEATURE_ENHANCED_STANDBY.
09/28/98   ajn     Avoid slew computation during finger slew problem.
09/28/98   ejv     Added _cdecl to force C calling to specific functions
                   for Pascal builds.
09/25/98   ejv     Changed prototype of srch_get_freq_est to handle an int2.
08/24/98   ejv     Removed pilot scanner.
08/18/98   ejv     Replaced srch_msm versions with versions listed in hw.h,
                   including Lynx support.
08/18/98   ejv     Added dh task support, must define FEATURE_DH.
08/18/98   ejv     Added rdh/bgc's FEATURE_DYNAMIC_THROTTLE to the governor.
07/01/98   ajn     Added FEATURE_NEW_SLEEP_TIMELINE macro
05/13/98   ajn     Used a delayed timeline for MSM2.3 in paging/idle sleep
                   mode.
04/13/98   dgy     Added general TA packet and type A1 searcher pathlog packet
                   for MSM2300. Modified finger lock thresholds.
03/16/98   scdb    Merged CLOCK 2.0 changes
03/09/98   smo     Changed TA report so that unassigned fingers report zero
                   RSSI for msm2300, to be consistent with msm22 targets.
03/04/98   yus/smo Added srch_select_8x function, srch_get_pilot_ecio, and ML
                   energy tables for use in Turbo (8x) searcher mode.
11/25/97   dgy/smo Use of new eng_to_ecio table pointer for 1x/8x modes.
11/17/97   dgy/smo Changes to support acquisition enhancements, use of new
                   temperature related algorithm.
10/28/97   smo     Added support for msm2300, 8x searcher.
10/01/97   smo     Added MSM2300 Support, call to srch_config_msm() for MSM2x
                   auto-detect and configuration within search.
06/27/97   rdh     Removed target dependency for CDMA RSSI. Targets desiring
                   unfiltered values must now call the function directly.
                   Improved and mainlined Pilot Scanner feature. Fixed several
                   bugs in search pathlog feature and mainlined entire feature.
06/19/97   rdh     Added T_Q, removed pre-NGP support, updated copyright.
03/21/97   smo     Lint Cleanup.
03/14/97   jjn     Added (TG==T_MD) to the appropriate conditional compile
                   statements for the Module
02/11/97   rdh     Raised interference detection by 2dB in srch_get_best_rssi.
01/15/97   rdh     Added largest-pilot CDMA RSSI for Gemini target calls.
01/09/97   smo     Corrected Rx AGC power calculation for J-STD-008 builds.
12/17/96   smo     Added PC state "Pilot Sniffer" feature.
12/12/96   smo     Modifications to srch_adj_srch_gov for rate of decay.
11/22/96   rdh     Added Hutch's T_T target definitions.
11/05/96   smo     Added CDMA RSSI filter for 800Mhz Gemini phones.
10/23/96   smo     Filter CDMA RSSI for FEATURE_JSTD008 builds.
10/22/96   rdh     More ROM savings to make 800 fit 4M flash.
09/26/96   rdh     More ROM savings to make 800 fit 4M flash.
09/26/96   rdh     Improved SC/PC thrash protection with dual-mode sys det.
09/19/96   rdh     Added unconditional PC state idle handoff throttling.
09/14/96   rdh     Demoted missed search in pathlog to MSG_LOW.
08/20/96   smo     Changes to not update RSSI when in Acquisition State.
08/15/96   rdh     Added idle handoff throttle timers/sigs.
08/12/96   smo     Fixed sector matching functions to allow freq_incl=1
                   when cdma_freq=current channel for J-STD-008 neighbors.
07/11/96   smo     Updated J-STD-008 Debugging information.
06/13/96   smo     Fixed Binary search for converting eng to Ec/Io.
06/06/96   smo     Fixed Rx AGC measurement when low Ec/Io. Lint cleanup.
05/21/96   rdh     Fixed fast search log finger PN assignment logging.
05/20/96   smo     Added JSTD-008 priority scanning debug variables
05/08/96   smo     Added JSTD-008 Features and modifications.  Major changes
                   include a priority mask "srch_pri_mask", used to scan
                   neighbors at different rates, based on priority.  Many
                   changes to general utilites, since for JSTD-008 a pilot is
                   defined by PN, Freq and band-class.
05/04/96   rdh     Removed references to PN autoswitch, simpler sleep_status.
                   Removed fixed handoff threshold poke as MC already has it.
05/01/96   rdh     Added orig_pend, made throttle 120-130 Hz due to loading.
04/15/96   rdh     Fixed bug leaving TA reporting off when not log path pkts.
04/09/96   rdh     Made 160-chip win non-segmented, added higher pre-Can
                   thresholds, lowered Cset size to IS-95's = 5, put catcher
                   in binary search in case of hang, added pathlog packet.
                   Fixed TA win center and timestamp bugs when fing log off.
03/09/96   rdh     Fixed pkt ts, win cen bugs in TA rpt. Added pathlog packet.
03/06/96   smo     Modified CDMA rssi to detect for very low Ec/Io.
02/29/96   rdh     Added poke capability for thresholds after SRCH_ASET_F.
02/28/96   rdh     Regionalized debug window poking for use in TC state.
02/28/96   rdh     Added finer T_TDROP table, PC-state arrival fnct for Jump.
02/27/96   rdh     Added Precan thresholds for T_ADDs > -13dB Ec/Io.
02/01/96   rdh     Added srch_expected_win_pos for idle sleep speedup.
01/16/96   smo     Revised srch_get_cdma_rssi to be consistent accross targets
01/06/96   rdh     Re-arranged modules to help simplify code review.
01/05/96   ras     added support for CSS
12/19/95   rdh     Added Gemini-like RSSI write into database.
11/30/95   rdh     Added internal-Neighbor keep support to handle large N lists.
10/04/95   rdh     Separated SLEEP_ACTIVE from SLEEP_MODE.
09/20/95   rdh     Made PN mask autoswitching compile-time chosen. Saves ROM.
09/20/95   rdh     Added DB value to tell if we are currently sleeping.
09/07/95   rdh     Passing pilot set energies to DIAG in IS-95 format.
09/04/95   rdh     Debug message modifications, more pre-review cleanup.
08/23/95   rdh     Pre- code review cleanup checkin to save RAM/ROM.
08/21/95   rdh     Added INTLOCK to srch_get_pilot_set function.
08/18/95   rdh     Added srch_get_pilot_set DIAG support.
07/21/95   rdh     ROM'd some large tables to save RAM space.
07/19/95   rdh     Now placing CDMA RSSI strength directly into DB for Gemini.
07/05/95   rdh     Made sleep disable less sticky for all targets except ISS1.
06/29/95   rdh     Added ISS2 to the sleep disable via NV spare_2 party.
06/28/95   rdh     Changes to support fast mini/micro acq on Deep Sleep wakeup.
06/25/95   rdh     Fixed sleep disable function for ISS1.
06/22/95   rdh     Added sleep disable function for ISS1.
06/13/95   rdh     Made T_SLEEP handling explicit and consistent DMSS-wide.
06/09/95   rdh     Changed MSG_ERR to MSG_HIGH per Hutch's request.
06/08/95   jca     Changed MCC queue sig for new MC sig structure.
06/01/95   rdh     Mods for search driver break-up, CSS.
05/27/95   rdh     Improved lost dump catcher in srch_sect_clr().
05/16/95   rdh     Better handling of NV_NOTACTIVE status.
05/16/95   rdh     Generalized NV_READ status handling to NV_DONE.
05/16/95   rdh     Fixed CDMA RSSI typecast for Gemini, added prelim table.
05/12/95   rdh     Added parm poke (win sizes, thresholds) for field testing.
05/11/95   rdh     A little touch up of CDMA RSSI for Beta I release.
05/09/95   rdh     Added search speed throttling to allow all window sizes.
05/05/95   rdh     Corrected Gemini RSSI equation yet again - update from Tom.
05/04/95   rdh     Added TA log enable check, other speedups, set min win siz.
04/26/95   rdh     Modified Gemini RSSI equation based on update from Tom.
04/18/95   ras     Added T_I2 for ISS2 operation.
04/14/95   rdh     Added CDMA RSSI for all targets.
04/13/95   gb/rdh  Changes to support Gemini MSM2P, optional broken DMA.
04/06/95   rdh     Added lost dump recovery wait on sig to srch_sect_clr().
03/31/95   rdh     Eliminated win_z variability as ZZ now spirals on reacq.
03/30/95   rdh     Merged in lost dump recovery timer code.
03/30/95   rdh     Elim slam bug catch - separated sym siz change from slam arm.
03/30/95   rdh     Improved PN mask change sequencing/interaction with DZ.
03/23/95   rdh     Added new srch_dz_state for Deep Sleep.
01/24/95   rdh     More PhsIII - Fixed bug in win_z init/assign.
01/10/95   rdh     Bug fixes for PhsIII release - initialized segmented parms,
                   got rid of old "nset_received" getting in way of mask change.
01/09/95   rdh     IS-95 Phase III release:
                    - Added segmented search max calc vars.
                    - Added PC state to center suppression for segm searches.
01/06/95   rdh     IS-95 Phase III upgrades:
                    - Fixed 2x initial winsize bug.
                    - Misc mods for reference time rework.
                    - Made t_tdrop tab[0]=2 to avoid dropping pilot when
                      t_tdrop=0.
                    - Made state and status code types consistent with calls.
                    - Added TC state handoff and slew throttling timers.
                    - Fixed computation of TC state pre-candidate thresholds
                      during param update.
                    - Added TC state DM TA center suppress for non-ref pilots.
12/22/94   ras     cdma_rssi returns db cdma_rssi if in sleep state.
11/15/94   rdh     Now using CDMA_PN_MASK NV item rather than SPARE_2.
                   Added PN mask change hysteresis counter to prevent thrash.
11/14/94   rdh     Added MSM interrupt mask register inits for ISS1.
11/09/94   gb      Changed srch_int3_isr ( INT3 ) priority to 1.
10/27/94   rdh     Added srch_notify_bad_slot, tightened ZZ reacq winsiz range.
10/24/94   rdh     Added slew state MSM hardware problem error detection
                   mods, PN mask autoswitching, etc.
                   Raised min win size to 20 chips to keep from bogging UI.
                   Also included 0 as a trigger for default good win parms.
10/11/94   rdh     Mainlining new CDMA RSSI (AGC+Ec/Io) technique.
09/06/94   rdh     Changed ERR to MSG_ERROR in srch_get_freq_est.
08/04/94   rdh     Lowered min win size to 10 chips, removed annoying tabs.
07/30/94   rdh     Added IS-95 Phase III upgrades, including over-air window
                   parms, segmented srch for large windows, individual pilot
                   centering in TC state, ref time symbol comb prediction,
                   TC state hard handoff processing, compliant idle searching,
                   idle/ZZ integ, SRCH-MC handoff request sequencing, etc..
07/01/94   rdh     Added CDMA RSSI for ISS1.
06/28/94   jah     Added init changes to ISS1
03/28/94   jah     Added srch_find_earliest_finger() from srchzz.c
03/10/94   jah     Changed srch_get_rx_agc() to use the ADC macros for the ADC.
02/09/94   jah     Changed srch_sect_clr() to always clear SRCH_DUMP_ISR_SIG.
01/06/94   jah     Changed watchdog iface to use dog_report().
12/14/93   jah     Changed sleep code to be #ifdef T_SLEEP != 0
10/09/93   jah     Modified srch_post_cmd() to not use sigs/tcb from the
                   command after q_put'ing it back to the free queue.
                   Added finger tracing under SRCH_FING_DEBUG define.
08/09/93   ptw/jah Added sleep state case, and init INT3 for sleep.
07/27/93   ptw     Added support for Pre-Candidate Set searching.
07/15/93   ptw     Added count of how many finger assignments take place.
                   Limit how far an assigned, unlocked finger can wander.
07/15/93   jai     Added srch_powerdown()
06/24/93   ptw     Added srch_cells[] and srch_cell_cnt definitions.
05/25/93   jah     Inverted TX_GAIN_ADJUST and I/Q_OFFSET PDM's for Beta II.
05/14/93   ptw     Switched to T_3CHIP and T_MSM conditionals, enabling new
                   targets (like T_B2) to do the right thing automatically.
04/28/93   jai     Added routines to store and retrieve frequency estimates and
                   added freqeuncy estimate callback routine.
03/24/93   ptw     Change t_tdrop_tab to srch_t_tdrop_tab and made it regional.
03/02/93   ptw     Added srch_get_ref_time() and srch_get_ulpn_state().
02/03/93   jah     Changed srch_dmod_reset() to init the GPIO state from the
                   BIO shadow registers in all cases.
07/16/92   ptw     Various changes for DMSS integration
04/30/92   ptw     Switched to new MSG and ERR macros.
01/22/92   ptw     Hacked out large chunks of code to separate modules.
                   Added lots of comments.
08/28/91   rrd     Ver0428 - Demo - Most protocol stuff, improved finger
                   assignment
07/23/91   rdb     Added power control to finger assignments (srch_triage).
                   Also, made change to ensure the finger used for slamming
                   on the transition from the slew state to traffic is
                   forced into lock.
07/22/91   rdb     Tweaks to time tracking.  Also, new interrupt system
                   deployed.
07/20/91   rdb     Tons and tons of changes.  Basically, rewrote searher
                   task.  Added lots of code to do intelligent (?)
                   finger assignment.  Also, refinements to perform
                   combiner time tracking.  And logging of AGC and power
                   control values.  Etc, etc, etc...
07/02/91   rdb     Lots of changes to support temporal analyzer.  Also
                   added code to enable combiner time tracking.
06/27/91   rdb     Added code to center search window around energy.
06/25/91   rdb     Millions of changes to support jump from sync channel
                   to traffic channel timing.
06/11/91   rdb     Added lots of code to support initial acquisition with
                   real demod emulator, cracking of sync channel messages.
06/02/91   rdb     Revision 1.0 release
05/06/91   rdb     Began cleaning up, making real.
04/09/91   rdb     Added more stuff.
03/19/91   rdb     First cut of module.

===========================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"

#include "bio.h"
#include "cai.h"
#include "clk.h"
#include "comdef.h"
#include "db.h"
#include "dmod.h"
#include "err.h"
#include "event.h"
#include "hw.h"
#include "mc.h"
#include "msg.h"
#include "nv.h"
#include "queue.h"
#include "qw.h"
#include "rex.h"
#include "rfm.h"
#include "sleepctl.h"
#include "stm.h"
#include "sleep.h"
#include "target.h"
#include "task.h"
#include "therm.h"
#include "time_cdma.h"
#include "time_svc.h"
#include "timer.h"
#include "tramp.h"
#include "vbatt.h"

#if defined(FEATURE_GPSONE) && !defined(FEATURE_SRCH_CGPS_IF)
#error code not present
#endif /* FEATURE_GPSONE && !FEATURE_SRCH_CGPS_IF */

#if defined(FEATURE_SRCH_CGPS_IF) && defined(FEATURE_GPSONE_PPM)
#error code not present
#endif /* FEATURE_SRCH_CGPS_IF && FEATURE_GPSONE_PPM */

#ifndef FEATURE_SRCH_HAS_NO_TRM
#include "trm.h"
#endif /* FEATURE_SRCH_HAS_NO_TRM */

#ifdef FEATURE_FACTORY_TESTMODE
#include "test_mc.h"
#endif /* FEATURE_FACTORY_TESTMODE */

/* srch includes */
#include "srch.h"
#include "srch4trans.h"
#include "srch_bm.h"
#include "srch_comb.h"
#include "srch_conv.h"
#include "srch_genlog.h"
#include "srch_hw.h"
#include "srch_irm.h"
#include "srch_lib_ext.h"
#include "srch_lib_int.h"
#include "srch_mdsp.h"
#include "srch_nv.h"
#include "srch_rx.h"
#include "srch_rx_t.h"
#include "srch_sect.h"
#include "srchcom_sm.h"
#include "srchi.h"
#include "srchlog.h"


/*===========================================================================

                        FUNCTION PROTOTYPES FOR MODULE

This section contains function prototypes needed by this module.

===========================================================================*/

#ifdef FEATURE_MODEM_1X_DIAG
#if defined(FEATURE_IS2000) && !defined(FEATURE_GSM_PLT)
extern void cdma2kdiag_init (void);
#endif
#endif /* FEATURE_MODEM_1X_DIAG */


/*===========================================================================

                LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*---------------------------------------------------------------------------
         Variables and Constants pertaining to CDMA RSSI Filtering
---------------------------------------------------------------------------*/

#define CDMA_RSSI_FILT_LEN 5
  /* Value of N for the (N-1)/N Filter Coefficients. */

#define CDMA_ECIO_MIN_THRESHOLD 36
  /* Minimum Ec/Io threshold (-18dB), below wich we peg RSSI to MIN_VALUE.
     This is to prevent Intermods from giving false RSSI readings. */

#define CDMA_RSSI_MIN_VALUE     125
  /* Min value of RSSI, will map to 0 bars for all targets */

#define CDMA_BEST_MIN_THRESHOLD 32
  /* Minimum Ec/Io threshold (-16dB), below wich we peg BEST RSSI to MIN_VALUE.
     This is to prevent low RSSI/hi AGC from from giving false RSSI readings.*/

/*---------------------------------------------------------------------------
                  Variables pertaining to IS95B
---------------------------------------------------------------------------*/
static srch_moving_psd_type srch_moving_psd;
  /* PSD moving average structure */

/*---------------------------------------------------------------------------
        Variables and Constants pertaining to frequency estimation
---------------------------------------------------------------------------*/

#define  FREQ_EST_TAB_SIZE  NV_FR_TEMP_OFFSET_TABLE_SIZ
  /* The size of the freq-vs-temp table is the size of what is stored in NV */

#define  FREQ_EST_RATIO     (256 / ( FREQ_EST_TAB_SIZE ))
  /* The ratio of the temperature range to the table size */

#define  TEMP_EST_AVG        2
  /* The number of temperature samples to average over */


/*===========================================================================

             REGIONAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains regional definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

srch_state_enum_type  srch_state = SRCH_INACTIVE_STATE;
  /* Keeps track of the current state of the Searcher Task state machine. */

srch_parm_struct_type  srch_parms;
  /* Parameters that control the behavior of the searcher task.
     These are settable by the base station via MCC */

srch_ch_rc_type srch_rc;
  /* Store the RC values (updated by MUX layer calling srch_update_rc)
     so that the correct RC values can be logged */

word  srch_temp_est = 0;
  /* Our most recent temperature estimate */

int2  srch_win_siz  = 60 * CHIPX8_PER_CHIP;
  /* Default search window size in 1/8 chip units. Over-air parms will
     supercede this value. */

uint2  srch_ref_pilot = NO_PILOT;
  /* Record of which pilot offset is the time reference */

qword srch_no_slot = { 0, 0 };
  /* Used to for initializing and comparing with other qwords */

#ifdef SRCH_ENHANCED_TRIAGE
boolean srch_disable_fn_merge_detect = FALSE;
  /*  Used to indicate disabling of finger merge detection when
      going to sleep */
#endif /* SRCH_ENHANCED_TRIAGE */



/*===========================================================================

                    MSM2.3 REGIONAL DECLARATIONS

This section contains regional definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

static sleep_okts_handle srch_sleep_handle;
  /* handle used when voting for/against sleep (tcxo shutdown) */

srch_msm_config_struct_type srch_msm_config;
  /* Search msm configuration parameters, to be used by search driver */

srch_sect_struct_type* srch_ref_ptr = NULL;
  /* pointer to reference pilot Active set */

int2  srch_agc_rssi = 0;
  /* Regional variable holding AGC RSSI (appx -105dBm.. scale) */

srch_roll_isr_vars_struct_type  srch_roll_isr_vars;
  /* The 26.667ms PN roll ISR variables */

static srch_agc_pctl_vars_struct_type  srch_agc_pctl_vars;
  /* AGC/Power control logging variables */

uint2  srch_init_jump_pilot = NO_PILOT;
  /* Which pilot offset we used in the initial jump to hyperspace */

uint2  srch_last_good_pilot = NO_PILOT;
  /* The pilot index of the most recent reference pilot. This
     is used as a guess of where to start when re-acquiring. */

int4  srch_last_good_window = 0L;
  /* The position of the end of the search window for the most
     recent reference finger. This is used as a guess of where
     to start when re-acquiring. */

int2  srch_last_good_win_siz = 0;
  /* The size of the search window for the most recent reference finger, used
     in concert with srch_last_good_window to determine where to start when
     re-acquiring. */

boolean srch_rudely_awakened = FALSE;
  /* Flag: nonzero if an event other than the expiration of the
     sleep timer has resulted in a decision to wake up the MSM. */

boolean srch_ho_to_oband = FALSE;
  /* Flag indicating if the mobile hard handed off to a different band class.
     Set during SRCH_TC_ASET_F processing; used and reset in the function
     srch_log_agc_pctl() */

int2 const srch_win_offset_tab[ SRCH_WIN_OFFSET_TAB_MAX ] =
{0, 1, 2, 3, -1, -2, -3};
  /* The search window offset multiplier table */

/*------------------------------------------------------------------------
                      J-STD-008 Definitions
--------------------------------------------------------------------------*/

word srch_pri_mask = SRCH_PRI_LOW;
  /* The priority mask for extended neighbor searching */
                                                     /* 0 = LOW */
                                                     /* 1 = MED */
                                                     /* 2 = HIGH */
                                                     /* 3 = VERY HIGH */

/*---------------------------------------------------------------------------
                    Regionalized Timer Definitions
---------------------------------------------------------------------------*/

timer_type  srch_rpt_timer;
  /* This timer is used during REX waits for an event. It wakes up
     the Search Task in time to signal the watchdog task. It is
     only used in the srch_wait function. */

/*---------------------------------------------------------------------------
                 Regionalized Queue and Buffer Definitions
---------------------------------------------------------------------------*/

q_type srch_cmd_q;
  /* Queue for commands from other tasks */

/*-------------------------------------------------------------------------
             Regionalized storage for a list of Cells

   This count+array list keeps track of the cells in the Active Set.
   For this purpose, a cell is a collection of sectors that all carry
   the same power control subchannel.

   Membership of sectors in a cell is maintained in the sector struct.
   These cell structs just keep some common information for each cell.
-------------------------------------------------------------------------*/

srch_cell_struct_type  srch_cells[ SRCH_ASET_MAX ];
  /* Data about each cell. There is at most one cell per
     sector in the Active Set, so SRCH_ASET_MAX is a safe
     size for this list. */

word  srch_cell_cnt;
  /* Count of the number of cells in the list */


/*---------------------------------------------------------------------------
                Regionalized Sign Extension Lookup Table

   This table is used together with the SIGNX macro to sign extend
   a 18-bit signed two's-complement number stored in the LS bits of
   a 32-bit word to the full 32 bits. This is useful for positions
   as reported by the demod chip.
---------------------------------------------------------------------------*/

word srch_sign_words[ ] = { 0x0000, 0x0001, 0xFFFE, 0xFFFF };


/*---------------------------------------------------------------------------
       Regionalized Energy to Neighbor Set Threshold Lookup Table

   This table is used to convert the T_ADD value sent over the air
   into a threshold for Neighbor Set members to enter the Pre-Candidate
   set. This is computed to match, with optimum probability, the
   Ec/Io values required by IS-95. Note: These energies differ from the
   standard Ec/Io table by some fraction. The difference is that this
   table is used in single-sweep evaluations, whereas the standard table
   is averaged over many sweeps including the effects of early dumps.
---------------------------------------------------------------------------*/

word  const srch_tc_nthresh_tab[ ECIO_TAB_SIZ ] =
{
  0x3e10,   /* - 0.0 dB */    // bogus double-per-3dB values here
  0x3708,   /* - 0.5 dB */
  0x30bc,   /* - 1.0 dB */
  0x2b1c,   /* - 1.5 dB */
  0x2746,   /* - 2.0 dB */
  0x22f0,   /* - 2.5 dB */
  0x1f08,   /* - 3.0 dB */
  0x1b84,   /* - 3.5 dB */
  0x185e,   /* - 4.0 dB */
  0x158e,   /* - 4.5 dB */
  0x13a3,   /* - 5.0 dB */    // real values start here
  0x1178,   /* - 5.5 dB */
  0x0f84,   /* - 6.0 dB */
  0x0dc2,   /* - 6.5 dB */
  0x0c2f,   /* - 7.0 dB */
  0x0ac7,   /* - 7.5 dB */
  0x0986,   /* - 8.0 dB */
  0x0869,   /* - 8.5 dB */
  0x076c,   /* - 9.0 dB */
  0x068c,   /* - 9.5 dB */
  0x05c6,   /* -10.0 dB */
  0x0518,   /* -10.5 dB */
  0x047f,   /* -11.0 dB */
  0x03f9,   /* -11.5 dB */
  0x0383,   /* -12.0 dB */
  0x031c,   /* -12.5 dB */
  0x02c1,   /* -13.0 dB */
  0x0274,   /* -13.5 dB */
  0x022d,   /* -14.0 dB */
  0x01ea,   /* -14.5 dB */
  0x01b5,   /* -15.0 dB */
  0x0184,   /* -15.5 dB */
  0x015a,   /* -16.0 dB */
  0x0132,   /* -16.5 dB */
  0x010e,   /* -17.0 dB */
  0x00ed,   /* -17.5 dB */
  0x00d6,   /* -18.0 dB */    // real values end here
  0x00d6,   /* -18.5 dB */    // bogus values copied into rest of table
  0x00d6,   /* -19.0 dB */
  0x00d6,   /* -19.5 dB */
  0x00d6,   /* -20.0 dB */
  0x00d6,   /* -20.5 dB */
  0x00d6,   /* -21.0 dB */
  0x00d6,   /* -21.5 dB */
  0x00d6,   /* -22.0 dB */
  0x00d6,   /* -22.5 dB */
  0x00d6,   /* -23.0 dB */
  0x00d6,   /* -23.5 dB */
  0x00d6,   /* -24.0 dB */
  0x00d6,   /* -24.5 dB */
  0x00d6,   /* -25.0 dB */
  0x00d6,   /* -25.5 dB */
  0x00d6,   /* -26.0 dB */
  0x00d6,   /* -26.5 dB */
  0x00d6,   /* -27.0 dB */
  0x00d6,   /* -27.5 dB */
  0x00d6,   /* -28.0 dB */
  0x00d6,   /* -28.5 dB */
  0x00d6,   /* -29.0 dB */
  0x00d6,   /* -29.5 dB */
  0x00d6,   /* -30.0 dB */
  0x00d6,   /* -30.5 dB */
  0x00d6,   /* -31.0 dB */
  0x00d6    /* -31.5 dB */
};


/*---------------------------------------------------------------------------
      Regionalized Energy to Pre-Candidate Set Threshold Lookup Table

   This table is used to convert the T_ADD value sent over the air
   into a threshold for Pre-Candidate Set members to advance their
   state variables within the Pre-Candidate Set.  This is computed
   to match, with optimum probability, the Ec/Io values required by
   IS-95.

---------------------------------------------------------------------------*/

word  const srch_tc_pcthresh_tab[ ECIO_TAB_SIZ ] =
{
  0x5e64,   /* - 0.0 dB */    // bogus double-per-3dB values here
  0x5990,   /* - 0.5 dB */
  0x5488,   /* - 1.0 dB */
  0x4f58,   /* - 1.5 dB */
  0x338a,   /* - 2.0 dB */
  0x3176,   /* - 2.5 dB */
  0x2f32,   /* - 3.0 dB */
  0x2cc8,   /* - 3.5 dB */
  0x2a44,   /* - 4.0 dB */
  0x27ac,   /* - 4.5 dB */
  0x19c5,   /* - 5.0 dB */    // real values start here
  0x18bb,   /* - 5.5 dB */
  0x1799,   /* - 6.0 dB */
  0x1664,   /* - 6.5 dB */
  0x1522,   /* - 7.0 dB */
  0x13d6,   /* - 7.5 dB */
  0x1286,   /* - 8.0 dB */
  0x1135,   /* - 8.5 dB */
  0x0fe7,   /* - 9.0 dB */
  0x0ea0,   /* - 9.5 dB */
  0x0d62,   /* -10.0 dB */
  0x0c30,   /* -10.5 dB */
  0x0b0c,   /* -11.0 dB */
  0x09f8,   /* -11.5 dB */
  0x08f6,   /* -12.0 dB */
  0x0806,   /* -12.5 dB */
  0x0726,   /* -13.0 dB */
  0x0663,   /* -13.5 dB */
  0x05a0,   /* -14.0 dB */
  0x0505,   /* -14.5 dB */
  0x0474,   /* -15.0 dB */
  0x03e8,   /* -15.5 dB */
  0x037c,   /* -16.0 dB */
  0x031b,   /* -16.5 dB */
  0x02bc,   /* -17.0 dB */
  0x0264,   /* -17.5 dB */
  0x021c,   /* -18.0 dB */    // real values end here
  0x021c,   /* -18.5 dB */    // bogus values copied into rest of table
  0x021c,   /* -19.0 dB */
  0x021c,   /* -19.5 dB */
  0x021c,   /* -20.0 dB */
  0x021c,   /* -20.5 dB */
  0x021c,   /* -21.0 dB */
  0x021c,   /* -21.5 dB */
  0x021c,   /* -22.0 dB */
  0x021c,   /* -22.5 dB */
  0x021c,   /* -23.0 dB */
  0x021c,   /* -23.5 dB */
  0x021c,   /* -24.0 dB */
  0x021c,   /* -24.5 dB */
  0x021c,   /* -25.0 dB */
  0x021c,   /* -25.5 dB */
  0x021c,   /* -26.0 dB */
  0x021c,   /* -26.5 dB */
  0x021c,   /* -27.0 dB */
  0x021c,   /* -27.5 dB */
  0x021c,   /* -28.0 dB */
  0x021c,   /* -28.5 dB */
  0x021c,   /* -29.0 dB */
  0x021c,   /* -29.5 dB */
  0x021c,   /* -30.0 dB */
  0x021c,   /* -30.5 dB */
  0x021c,   /* -31.0 dB */
  0x021c    /* -31.5 dB */
};


/*---------------------------------------------------------------------------
          Regionalized Window Size Decoding Lookup Table

  This table is used to expand the window size code in the search parameters
  command into an actual window size. This is Table 6.6.6.2.1-1 from CAI 2.0.
---------------------------------------------------------------------------*/

int2  const win_tab[ WIN_TAB_SIZ ] =
{
    4,
    6,
    8,
   10,
   14,
   20,
   28,
   40,
   60,
   80,
  100,
  130,
  160,
  226,
  320,
  452
};

/*---------------------------------------------------------------------------
              Regionalized T_TDROP Decoding Lookup Table

   This table is used to expand the T_TDROP index in the search parameters
   command into an actual duration. This is Table 6.6.6.2.3-1 from CAI 2.0.
   Values appear in units of 26.66..ms. Where the CAI value is not an even
   multiple of 26.66..ms, the value is rounded up.
---------------------------------------------------------------------------*/
#define                  T_TDROP_TAB_SIZE   16
  /* Max length of the t_tdrop mapping table */

word  const srch_t_tdrop_tab [ T_TDROP_TAB_SIZE ] =
{         /* Index  CAI Value */
     2,   /*   0    0 seconds (Up to 100ms is allowed) */
    38,   /*   1    1 second  */
    75,   /*   2    2 seconds */
   150,   /*   3    4 seconds */
   225,   /*   4    6 seconds */
   338,   /*   5    9 seconds */
   488,   /*   6   13 seconds */
   713,   /*   7   19 seconds */
  1013,   /*   8   27 seconds */
  1463,   /*   9   39 seconds */
  2063,   /*  10   55 seconds */
  2962,   /*  11   79 seconds */
  4200,   /*  12  112 seconds */
  5963,   /*  13  159 seconds */
  8438,   /*  14  225 seconds */
 11963,   /*  15  319 seconds */
};


#ifdef FEATURE_PLT
#error code not present
#endif /* FEATURE_PLT */


#ifdef FEATURE_IS95B_MDR
/*===========================================================================

FUNCTION SRCH_ASET_WALSH

DESCRIPTION
  This function is used to provide walsh code information of the Active
  Set.

DEPENDENCIES
  srch_aset[], srch_aset_cnt OR sect_loaded() == TRUE.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void srch_aset_walsh
(
  srch_walsh_struct_type *ptr
    /* Point to store the information */
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* This function uses sect's pilot data interface */
  srch_diag_aset_walsh(ptr);

} /* srch_aset_walsh */
#endif

/*===========================================================================

FUNCTION SRCH_GET_MOVING_PSD

DESCRIPTION    This function writes a moving average power spectral density
         value of serving frequency to the passed variable.

DEPENDENCIES    srch_moving_psd.

RETURN VALUE    TRUE if the moving average is available,
                FALSE if it is not available:
                  1) The calcualtion is not enabled, OR
                  2) The samples is less than SRCH_CAI_N12M (10 ).

SIDE EFFECTS    None.

===========================================================================*/
boolean srch_get_moving_psd
(
  int2 *psd
    /*  Store the moving average of psd in dB */
)
{
  int2 total_power = 0;
    /* Sum of all samples */

  byte i;
    /* Index of loop  */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( !psd )
  {
    MSG_ERROR("Unexpected NULL pointer: psd ptr", 0, 0, 0);
    return ( FALSE );
  }

  if ( (srch_moving_psd.enabled) && (srch_moving_psd.full) )
  {
    for ( i =0; i < SRCH_CAI_N12M; i++ )
    {
      total_power += srch_moving_psd.psd[i];
    }

    total_power /= SRCH_CAI_N12M;   /* Divided by the N12m */

    *psd = srch_agc_to_dbm ( total_power );
      /* Convert AGC to power in dBm */

    return( TRUE );
  }
  else
  {
    return( FALSE );
  }
} /* srch_get_moving_psd */

/*===========================================================================

FUNCTION SRCH_ENABLE_MOVING_PSD

DESCRIPTION    This function enables the power spectral density moving
         average calculation for the serving frequency.

DEPENDENCIES    srch_moving_psd.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/
void srch_enable_moving_psd
(
  boolean enable
  /* TRUE, enable it. FALSE, disable it. */
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( enable )
  {
     srch_moving_psd.enabled = TRUE;
  }
  else
  {
    srch_moving_psd.enabled = FALSE;
  }
} /* srch_enable_moving_psd */

/*===========================================================================

FUNCTION SRCH_RESET_MOVING_PSD

DESCRIPTION    This function will also reset all the previous stored psd
               sample values.

DEPENDENCIES    srch_moving_psd.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/
void srch_reset_moving_psd(void)
{
  byte i;
    /* Loop index */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for ( i = 0; i < SRCH_CAI_N12M; i++ )
  {
     srch_moving_psd.psd[i] =  0;
     srch_moving_psd.full = FALSE;
     srch_moving_psd.index = 0;
  }

} /* srch_reset_moving_psd */

/*===========================================================================

FUNCTION SRCH_SAVE_PSD_SAMPLE

DESCRIPTION    This function saves the most recent received power spectral
         density sample and discards the oldest sample.

DEPENDENCIES    srch_moving_psd.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/
void srch_save_psd_sample( void)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( srch_moving_psd.enabled )
  {
    srch_moving_psd.psd[ srch_moving_psd.index++] = (int2) srch_get_rx_agc();
    /* Get RX AGC value */

    if ( srch_moving_psd.index == SRCH_CAI_N12M )
    {
      srch_moving_psd.index = 0;    /* Circle back to 0 */
      srch_moving_psd.full = TRUE;  /* Has enough samples */
    }
  }
} /* srch_save_psd_sample */


/*===========================================================================

FUNCTION SRCH_GET_PILOT_ECIO

DESCRIPTION    This function returns a byte representing the current
               Page Channel active pilot Ec/Io value.

DEPENDENCIES   srch_aset[0].

RETURN VALUE   The 6-bit Ec/Io value.  If the energy is out of range,
               the return value is clipped to 0 or 63. This is in
               1/2 db unit.

SIDE EFFECTS    None.

===========================================================================*/
byte srch_get_pilot_ecio  (void)
{
  byte cdma_ecio;
    /* CDMA Ec/Io value */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cdma_ecio = CONV_LIN_HALF_DB_UNITS(srch_sect_a()->srch_filter.lecio);
    /* Find the Ec/Io index of the Active set in 0.5dB steps from
       0 (0dB) to 63 (-31.5dB). */

  return ( cdma_ecio );
} /* srch_get_pilot_ecio */

/*===========================================================================

FUNCTION SRCH_GET_CHANNEL_ESTIMATE

DESCRIPTION    This function returns an estimate of the channel in 1/8 db units.
               This function is sleep safe and can be called in another task
               context

DEPENDENCIES   None.

RETURN VALUE   Channel estimate in 1/8 db units

SIDE EFFECTS   None.

===========================================================================*/
uint32 srch_get_channel_estimate (void)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(srch_in_service())
  {
    if(srch_idle_slotted_mode_active())
    {
      /* if we are currently in sleep mode either slotted or qpch get a
       * estimate of the channel from the channel estimator
       */
      return(srchzz_get_qpch_ch_est());
    }
    else
    {
      /* if we are currently in service, but not in sleep mode so
       * just take a look at the filtered searcher energy
       */
      return(srch_conv_linear_to_db(srch_ref_ptr->srch_filter.lecio));
    }
  }
  else
  {
    /* if search is out of service, return the dB value for 0 linear ecio */
    return(srch_conv_linear_to_db(0));
  }

} /* srch_get_channel_estimate */

#ifdef FEATURE_JCDMA
/*===========================================================================

FUNCTION SRCH_GET_CDMA_RAW_RSSI

DESCRIPTION    This function returns a int2 representing the active set
               pilot RSSI.

DEPENDENCIES   None.

RETURN VALUE   negative RSSI value in dBm.

SIDE EFFECTS   None.

===========================================================================*/

word  srch_get_cdma_raw_rssi ( void )
{
  return ( (word) (-srch_agc_rssi) );
} /* srch_get_cdma_raw_rssi */
#endif /* FEATURE_JCDMA */

/*===========================================================================

FUNCTION SRCH_AGC_TO_DBM

DESCRIPTION    This function converts an Rx AGC value to dBm.

DEPENDENCIES   None.

RETURN VALUE   The AGC value in dBm units.

SIDE EFFECTS   None.

===========================================================================*/

int2  srch_agc_to_dbm
(
  byte rx_agc
    /* CDMA RX AGC value */
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return ( (-6325 + ((int1)rx_agc * 33) )/100 );
    /* Convert AGC to power in dBm */

} /* srch_agc_to_dbm */

/*===========================================================================

FUNCTION SRCH_GET_CDMA_RAW_ECIO

DESCRIPTION    This function returns a byte representing the active set
               pilot Ec/IO.

DEPENDENCIES   srch_aset[], srch_aset_cnt.

RETURN VALUE   negtive Ec/IO value in 0.5 dBm steps.

SIDE EFFECTS   None.

===========================================================================*/

byte  srch_get_cdma_raw_ecio ( void )
{
  uint32 max_aset_eng = 0;
    /* Max energy of Aset pilots */

  word i;
    /* Loop index */

  byte cdma_ecio;
    /* CDMA Ec/Io value */

  srch_sect_list_type sectors;
    /* Active sector list */

  uint32 list_cnt;
    /* Number of sectors */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sectors = srch_sect_list_get(SRCH_SECT_ELIST_A,&list_cnt,FALSE);
  /* Take a look at all Active members */
  for ( i = 0; i < list_cnt; i++ )
  {
    if (sectors[i] != NULL)
    {
      /* Check if the energy is bigger than the largest already found */
      if ( sectors[i]->srch_filter.lecio > max_aset_eng )
      {
        max_aset_eng = sectors[i]->srch_filter.lecio;
        /* Store the bigger energy */
      }
    }
  }

  cdma_ecio = CONV_LIN_HALF_DB_UNITS(max_aset_eng);
    /* Find the Ec/Io index of the Active set in 0.5dB steps from
       0 (0dB) to 63 (-31.5dB). */

  return cdma_ecio;
} /* srch_get_cdma_raw_ecio */


/*===========================================================================

FUNCTION SRCH_GET_CDMA_RSSI

DESCRIPTION    This function returns a byte representing the current
      CDMA RSSI value in P(AGC) + Ec/Io form. This byte can then be
      arbitrarily mapped to UI display space using an equation, table,
      or case statement.  RSSI is stored internal to search as a word,
      for better filtering results.

      The RSSI if filtered using an (N-1)/N IIR filter for a smoother signal
      strength display. This CDMA RSSI is written into DB (database)
      for consistency with analog RSSI. This database value is thereby
      updated at a rate determined by the caller.

      The function returns the raw, unfiltered CDMA RSSI. Targets which
      need unfiltered values must call this function explicitly rather
      than using the filtered DB value.

DEPENDENCIES    srch_aset[], srch_aset_cnt.

RETURN VALUE    The sector RSSI in (positive) dBm.

SIDE EFFECTS    Filtered database CDMA_RSSI is updated.

===========================================================================*/

byte  srch_get_cdma_rssi ( void )
{
  word raw_cdma_rssi;
    /* Unfiltered CDMA RSSI to return to caller */

  static word cdma_rssi = 0;
    /* Filtered combined CDMA RSSI */

  uint32 last_cdma_rssi;
    /* Temp storage for filtering */

  byte cdma_ecio;
    /* CDMA Ec/Io value */

  byte rx_agc;
    /* CDMA RX AGC value */

  dword total_aset_eng = 0;
    /* Summation of Aset energy */

  word i;
    /* Summation loop index */

  int2 power;
    /* Computation workspace for power calc */

  static db_items_value_type  db_value;
    /* Data Base structure to place CDMA RSSI into */

  srch_sect_list_type sectors;
    /* Active sector list */

  uint32 list_cnt;
    /* Number of sectors */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  last_cdma_rssi = cdma_rssi;
    /* Save last measured RSSI before we calculate a new value */

  sectors = srch_sect_list_get(SRCH_SECT_ELIST_A,&list_cnt,FALSE);
  /* Take a look at all Active members */
  for ( i = 0; i < list_cnt; i++ )
  {
    if (sectors[i] != NULL)
    {
      /* Check if the energy is bigger than the largest already found */
      if ( sectors[i]->srch_filter.lecio > total_aset_eng )
      {
        total_aset_eng = sectors[i]->srch_filter.lecio;
          /* Store the bigger energy */
      }
    }
  }

  cdma_ecio = CONV_LIN_HALF_DB_UNITS(total_aset_eng);
    /* Load in 1/2 dB units */

  rx_agc = srch_rx_get_home_rx_agc();
    /* Get RX AGC value */

  /* Calculate RX power in dB depending on target type and AGC.  We must
     do this regardless of Ec/Io because srch_agc_rssi is used regionally.*/

  power = srch_agc_to_dbm ( rx_agc );
    /* Convert AGC to power in dBm */

  srch_agc_rssi = power;
    /* Make AGC in dBm power units available regionally. */
#ifdef CUST_EDITION
  // for ec/io
  db_value.cdma_ecio = (srch_get_cdma_raw_ecio())/2;
  db_put(DB_CDMA_ECIO, &db_value); 
#endif
/*-------------------------------------------------------------------------
  Check for a reasonable Ec/Io (above -18dB) and calculate cdma_rssi.
  If Ec/Io is too low, we will peg cdma_rssi to a low value (125), else
  we will calculate, based on Ec/Io and Rx AGC.
--------------------------------------------------------------------------*/
  if( cdma_ecio < CDMA_ECIO_MIN_THRESHOLD )
  {
      power = 2*power - (int2)cdma_ecio;
        /* Convert to 0.5dB units and add in the 6-bit Ec/Io. */

      cdma_rssi = (word) (-(power / 2));
        /* Let CDMA RSSI be power in dB directly.
           However, strip off the sign, making it positive. */

      cdma_rssi *= 16;
        /* Scale RSSI for better filtering results. */

  }
  else /* Ec/Io is not high enough, for a valid RSSI calculation. */
  {
      cdma_rssi = (word) CDMA_RSSI_MIN_VALUE;
      cdma_rssi *=16;
        /* we will peg RSSI to minimum value, to avoid interference */

      MSG_LOW ( "Ec/Io below minimum threshold", 0, 0, 0 );
  }

  raw_cdma_rssi = cdma_rssi;
    /* Remember the raw CDMA RSSI for return to caller */

/*-------------------------------------------------------------------------
  Implement IIR filtering of RSSI for the benefit of targets with a display
--------------------------------------------------------------------------*/

  if(last_cdma_rssi != 0)
  {
    (void) srch_filt_iir(&last_cdma_rssi,cdma_rssi,CDMA_RSSI_FILT_LEN,0);
    cdma_rssi = last_cdma_rssi;
      /* Filter new RSSI into old and take the filtered value */
  }
  else
  {
    MSG_LOW("INIT RSSI, use instantaneous",0,0,0);
  }

/*-----------------------------------------------------------
  Don't update the DB_CDMA_RSSI when in Acquisition Substate.
  This is so we can roam for prefered systems without the
  RSSI reflecting this fact.
-----------------------------------------------------------*/
  if ( srch_state != SRCH_ACQ_STATE )
  {
    db_value.cdma_rssi = cdma_rssi/16;
    db_put(DB_CDMA_RSSI, &db_value);
      /* Place CDMA RSSI in DB item for consistency with analog RSSI */
  }

  MSG_5(MSG_SSID_1X_SRCH,MSG_LEGACY_LOW,
        "Ecio=-%d, RxAGC=%d=%ddBm, raw RSSI=%d, filt RSSI=%d",
        cdma_ecio>>1,rx_agc,srch_agc_rssi,(raw_cdma_rssi/16),(cdma_rssi/16));

  return ( (byte) (raw_cdma_rssi/16) );
    /* Return the final scaled CDMA RSSI to the caller */

} /* srch_get_cdma_rssi */


/*===========================================================================

FUNCTION SRCH_GET_BEST_RSSI

DESCRIPTION    This function returns a byte representing the best-potential
      CDMA RSSI value in P(AGC) + Ec/Io form.

DEPENDENCIES    srch_?set[], srch_?set_cnt.

RETURN VALUE    A 6-bit RSSI index, 0..63, representing RSSI from
                -75 dBm to -106.5 dBm, in 0.5 dBm steps. If the srch
                state is not valid for the query will return 63.

SIDE EFFECTS    None.

===========================================================================*/

byte  srch_get_best_rssi ( void )
{
  word cdma_rssi;
  /* RSSI to return to caller */

  byte cdma_ecio = 0;
    /* CDMA Ec/Io value */

  dword best_eng;
    /* Best-pilot Ec/Io */

  word i;
    /* Summation loop index */

  int2 power;
    /* Computation workspace for power calc */

  srch_sect_list_type sectors;
    /* Active sector list */

  uint32 list_cnt;
    /* Number of sectors */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Compute returned CDMA_RSSI based on largest pilot, either Active
     or Neighbor, to pass back potentially-best CDMA_RSSI to caller */

  /* We could be in TC or START after abnormally exiting from
     TC state. Therefore, we must consider all sets to find
     the strongest pilot. Don't bother with remaining set as
     its energy is likely stale, also, it ends up being
     interference energy which shouldn't be considered good. */

  if (srch_sect_a() == NULL)
  {
    /* In acquisition state srch_sect_loaded is valid but srch_sect_a
     * is not, in this case there is no valid rssi value to return so
     * return the minimum rssi value. */
    MSG_LOW ( "No valid rssi available state %d", srch_state, 0, 0 );
    return ((byte)CDMA_RSSI_MIN_VALUE);
  }

  best_eng = srch_sect_a()->search_results.max_lecio[0];

  sectors = srch_sect_list_get(SRCH_SECT_ELIST_A,&list_cnt,FALSE);
  for (i=1; i<list_cnt; i++)
  {
    if (sectors[i] != NULL)
    {
      if ( sectors[i]->search_results.max_lecio[0] > best_eng )
      {
        best_eng = sectors[i]->search_results.max_lecio[0];
      }
    }
  }
  sectors = srch_sect_list_get(SRCH_SECT_LIST_N,&list_cnt,FALSE);
  for (i=0; i<list_cnt; i++)
  {
    if ( sectors[i]->search_results.max_lecio[0] > best_eng )
    {
      best_eng = sectors[i]->search_results.max_lecio[0];
    }
  }

  cdma_ecio = (byte) CONV_LIN_HALF_DB_UNITS( best_eng );
    /* Find the Ec/Io index of the Active set in 0.5dB steps from
       0 (0dB) to 63 (-31.5dB). */

  power = srch_agc_to_dbm( srch_rx_get_home_rx_agc() );
    /* Get Rx power in dBm */

  /* Now, do interference detection */

  if( cdma_ecio < CDMA_BEST_MIN_THRESHOLD )
  {
    power = 2*power - (int2)cdma_ecio;
      /* Convert to 0.5dB units and add in the 6-bit Ec/Io. */

    cdma_rssi = (word) (-(power / 2));
      /* Let CDMA RSSI be power in dB directly.
         However, strip off the sign, making it positive. */
  }
  else /* Ec/Io is not high enough, for a valid RSSI calculation. */
  {
    cdma_rssi = (word) CDMA_RSSI_MIN_VALUE;
      /* we will peg RSSI to minimum value, to avoid interference */
  }

  MSG_MED ( "CDMA Rssi rssi=%d  (ecio %d)", cdma_rssi, cdma_ecio, 0 );

  return ( (byte) cdma_rssi );
    /* Return the final CDMA RSSI to the caller */

} /* srch_get_best_rssi */


/*===========================================================================

FUNCTION SRCH_SET_PARM

DESCRIPTION    This function is called to implement a command to set
      certain parameters that control the behavior of the searcher task.

DEPENDENCIES   None.

RETURN VALUE   Command status, always SRCH_DONE_S (good status).

SIDE EFFECTS   srch_parms is updated.

===========================================================================*/

srch_cmd_status_type  srch_set_parm
(
  srch_parm_cmd_type  *cmd_ptr
    /* Pointer to the Set Search Parameters command */
)
{
  word n;
    /* Index of the neighbor set */

  srch_sect_list_type sectors;
    /* Active sector list */

  uint32 list_cnt;
    /* Number of sectors */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( !cmd_ptr )
  {
    MSG_ERROR("Unexpected NULL pointer: cmd_ptr", 0, 0, 0);
    return ( SRCH_ERR_S );
  }

  /* If we are on a Rel A system which is using a MC-RR and UNLM, it is
   * possible that a global neighbor window size will not be specified.
   * In this case, the neighbors will each have their own window size;
   * however, this SRHC->MC interface requires some value to be
   * populated in the fields so MC will send one more than max window
   * size indicating that search should just use some default value.
   *
   * We might want to rethink this at some point, but for right now
   * just spit out a message high rather than an error.
   */
  if(cmd_ptr->win_n == WIN_TAB_SIZ)
  {
    MSG_HIGH(
      "Defaulting global win_n (80c), Using nbr specific win sizes",0,0,0);

    cmd_ptr->win_n = 9; /* 80 chips */
  }

  /* Make sure over-air window parms in reasonable range */
  /* active set window size by standard has max size of 226 (index 13) */
  if (cmd_ptr->win_a > 13)
  {
    if (cmd_ptr->win_a >= WIN_TAB_SIZ)
    {
      ERR ( "Bad active win size (%d). Using 8",cmd_ptr->win_a, 0, 0);

      cmd_ptr->win_a = 8;    /* "8" = 60 chips. A reasonable value */
    }
    else   /* win size > 226 */
    {
      ERR ( "Bad active win size (%d). Using 13",cmd_ptr->win_a, 0, 0);

      cmd_ptr->win_a = 13;    /* "13" = 226 chips, Max allowable */
    }

  }

  if (cmd_ptr->win_n >= WIN_TAB_SIZ)
  {
    ERR ( "Bad nbr win size (%d). Using 8",cmd_ptr->win_n, 0, 0);

    cmd_ptr->win_n = 8;    /* "8" = 60 chips. A reasonable value */
  }

  if (cmd_ptr->win_r >= WIN_TAB_SIZ)
  {
    ERR ( "Bad remaining win size (%d). Using 8",cmd_ptr->win_r, 0, 0);

    cmd_ptr->win_r = 8;    /* "8" = 60 chips. A reasonable value */
  }

  /* Disallow (0,0,0), even though its a valid CAI set. This is because
     (0,0,0) sometimes gets shoved down SRCH's throat when things run
     amock. (0,0,0) is absurd. Assume base station is nuts. */

  if ((cmd_ptr->win_a == 0) && (cmd_ptr->win_n == 0) && (cmd_ptr->win_r == 0))
  {
    cmd_ptr->win_a = 8; /* A reasonable value */
    cmd_ptr->win_n = 8; /* A reasonable value */
    cmd_ptr->win_r = 8; /* A reasonable value */

    ERR ( "Bad win parm. Using (%d,%d,%d)",
       cmd_ptr->win_a, cmd_ptr->win_n, cmd_ptr->win_r );
  }

  /* Copy in all the parameters from the over-air message */
  srch_parms.aset_win_siz  = win_tab[ cmd_ptr->win_a ];
  srch_parms.nset_win_siz  = win_tab[ cmd_ptr->win_n ];
  srch_parms.rset_win_siz  = win_tab[ cmd_ptr->win_r ];

  srch_parms.t_add         = (uint32) cmd_ptr->t_add;
  srch_parms.t_drop        = (uint32) cmd_ptr->t_drop;
  srch_parms.t_comp        = cmd_ptr->t_comp;
  srch_parms.t_tdrop       = srch_t_tdrop_tab[cmd_ptr->t_tdrop];
  srch_parms.nghbr_max_age = cmd_ptr->nghbr_max_age;

  srch_parms.t_add_eng  = CONV_HALF_DB_LIN_UNITS( srch_parms.t_add );
  srch_parms.t_drop_eng = CONV_HALF_DB_LIN_UNITS( srch_parms.t_drop );
    /* Keep a copy of these thresholds in energy units too */

  srch_parms.t_nset_eng    = srch_tc_nthresh_tab[ srch_parms.t_add ];
  srch_parms.pc_thresh_eng = srch_tc_pcthresh_tab[ srch_parms.t_add ];
    /* Compute the corresponding Pre-Candidate thresholds */

  srch_parms.t_nset_eng    =
    srch_conv_eng_to_lecio( srch_parms.t_nset_eng, 512, 1, 5 );
  srch_parms.pc_thresh_eng =
    srch_conv_eng_to_lecio( srch_parms.pc_thresh_eng, 800, 1, 5 );
    /* Convert thresholds to lecio */

  if ( P_REV_IS_4_OR_GREATER )
  {
    srch_parms.soft_slope     = (int2)cmd_ptr->soft_slope;
    srch_parms.add_intercept  = (int2)cmd_ptr->add_intercept;
    srch_parms.drop_intercept = (int2)cmd_ptr->drop_intercept;

    MSG_MED("soft_slope =%d, add_int=%d, drop_int=%d ",
            srch_parms.soft_slope,
            srch_parms.add_intercept,
            srch_parms.drop_intercept );
  }
  else
  {
    srch_parms.soft_slope = 0;
  }

  if ( P_REV_IS_4_OR_GREATER )
  {
    if ( cmd_ptr->sys_reselect_included )
    {
      if ( (word) cmd_ptr->ec_thresh > SRCH_MAX_EC_THRESH )
      {
        ERR ( "Bad EC_THRESH. Using ( %d )", (word) cmd_ptr->ec_thresh, 0, 0 );
        /* ec_thresh shall never exceed 115 */
        /* Ignore the command. Just print the error message.
           Do nothing, keep as it is */
      }
      else
      {
        srch_parms.sys_reselect_enabled = TRUE;
        srch_parms.ec_thresh = SRCH_MAX_EC_THRESH - (word) cmd_ptr->ec_thresh;
        srch_parms.ec_io_thresh = (word) cmd_ptr->ec_io_thresh;
        MSG_MED("EC_thr = -%d(dBm), EcIo_thr = -%d(dB)", srch_parms.ec_thresh,
                srch_parms.ec_io_thresh/2, 0);
      }
    }
    else
    {
      srch_parms.sys_reselect_enabled = FALSE;
    }
  }

  /* Limit Aset window size to CAI size = 13. This is IS-95. */
  if (cmd_ptr->win_a > 13)
  {
    cmd_ptr->win_a   = 13;
    srch_parms.aset_win_siz = win_tab[ 13 ];
  }

  srch_parms.win_a = cmd_ptr->win_a;
  srch_parms.win_n = cmd_ptr->win_n;
  srch_parms.win_r = cmd_ptr->win_r;
    /* Remember the CAI window sizes for later use */

  srch_parms.aset_win_siz *= CHIPX8_PER_CHIP;
  srch_parms.nset_win_siz *= CHIPX8_PER_CHIP;
  srch_parms.rset_win_siz *= CHIPX8_PER_CHIP;
    /* Get the window param sizes from table values in
       chips into chipx8 units */

  if ( P_REV_IS_4_OR_GREATER )         /* IS95B */
  {
    /* Set each neighbor window size to same size, later it could be
       changed by ENLM. */
    sectors = srch_sect_list_get(SRCH_SECT_LIST_N,&list_cnt,FALSE);
    for (n=0; n<list_cnt; n++)
    {
      sectors[ n ]->ota_win_n = srch_parms.nset_win_siz;
    }

    /* Set the window size value of all neighbor pilots
     * with neighbor search window size value received in the
     * ITSPM message, if the neighbor search mode value is 
     * either "00" or "01".
     */
    if ( (IS_TRAFFIC_STATE(srch_state)) &&
         ( (srch_parms.nghbr_srch_mode == SRCH_NO_PRI_OR_WIN) ||
	   (srch_parms.nghbr_srch_mode == SRCH_PRI_ONLY)) )
    {
      for (n=0; n<list_cnt; n++)
      {
        sectors[ n ]->search_parms.win_size = srch_parms.nset_win_siz;
      }
    }
  }

  MSG_HIGH("Srch win a,n,r=%d,%d,%d chips", win_tab[ cmd_ptr->win_a ],
        win_tab[ cmd_ptr->win_n ], win_tab[ cmd_ptr->win_r ] );
  MSG_LOW("Srch tadd,tdrop,tcomp=%d,%d,%d", srch_parms.t_add,
        srch_parms.t_drop, srch_parms.t_comp );

  return ( SRCH_DONE_S );
    /* Always return good status */

} /* srch_set_parm */


/*===========================================================================

FUNCTION SRCH_LOG_AGC_PCTL_ENA

DESCRIPTION    This function is invoked to enable logging of AGC and power
      control values.

DEPENDENCIES   srch_agc_pctl_vars

RETURN VALUE   None.

SIDE EFFECTS   Messages can be sent to DIAG task.

===========================================================================*/

void  srch_log_agc_pctl_ena ( void )
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check and note whether AGC / power control logging is enabled */

  if ( log_status(LOG_AGC_PCTL_C) || log_status(LOG_S_AGC_PCTL_C) )
  {
    /* Log mask is enabled. Check to see if we are in a valid state */

    if ( (srch_state == SRCH_ACQ_STATE) || (srch_state == SRCH_SC_STATE)
       || (srch_state == SRCH_PC_STATE) || (srch_state == SRCH_TC_STATE) )
    {
      /* Valid state for AGC/pctl logging. */

      if ( !srch_agc_pctl_vars.enabled )     /* Was logging disabled before? */
      {
        srch_agc_pctl_vars.inx = 0;
          /* Start at the beginning of the hopper */

        srch_agc_pctl_vars.enabled = TRUE;
          /* Enable AGC/pctl logging */

      } /* if logging was disabled before */
    } /* if valid logging state */
  } /* if logging now enabled */

  /* Logging is not enabled or we are in the wrong state */

  else if ( srch_agc_pctl_vars.enabled )     /* Was logging enabled before? */
  {
    srch_agc_pctl_vars.enabled = FALSE;
      /* Turn AGC/pctl logging if off. */
  }

} /* srch_log_agc_pctl_ena */


/*===========================================================================

FUNCTION SRCH_LOG_AGC_PCTL

DESCRIPTION    This function is invoked to log AGC and power control values
      for DIAG.

DEPENDENCIES   srch_agc_pctl_vars

RETURN VALUE   None.

SIDE EFFECTS   Messages can be sent to DIAG task.

===========================================================================*/

void  srch_log_agc_pctl ( void )
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check to see AGC/pctl logging is enabled. If not, just bail out.
     This is the fastest exit path. Saves time when logging not enabled. */

  if ( !srch_agc_pctl_vars.enabled )
  {
    return;
      /* Logging is not enabled. Just return, having done nothing. */
  }

  /* Check to see if we are currently in a valid state for AGC/pctl
     logging. If not, just bail out. This is the next fastest exit
     path, used when we are in a bad state for logging and we have
     not yet gotten around to disabling the logging enable flag. */

  if ( (srch_state == SRCH_ACQ_STATE) || (srch_state == SRCH_SC_STATE)
       || (srch_state == SRCH_PC_STATE) || (srch_state == SRCH_TC_STATE) )
  {
    /* Valid logging states. Continue. */
  }
  else
  {
    return;
      /* Invalid logging state. Just return, having done nothing. */
  }

  /* Now, we passed all the up-front filters. Check to see if we need
     to open a new DIAG log buffer, then log AGC and power control */

  /* If we are pointing to the first bin in the hopper, then we must be
     ready to fill a new buffer.  This gets bumped on every logging call */
  if ( srch_agc_pctl_vars.inx == 0 )
  {
    /* Just starting.  Fill in the params that get updated at the start
       of the log */

    (void) time_get ( (unsigned long *) srch_agc_pctl_vars.log.hdr.ts );
      /* Get a timestamp for the log packet. Notice that it is
         valid stamp of the *beginning* of the data collection. */

    srch_agc_pctl_vars.log.adc_therm = (byte)therm_read();
      /* Thermistor ADC sample value */

    srch_agc_pctl_vars.log.batt_volt = (byte)vbatt_read();
      /* Battery voltage */

    /* CDMA TX power limit */
    srch_agc_pctl_vars.log.tx_pwr_limit =
      (byte)( srch_rx_get_tx_pwr_limit() >> 2 );
  } /* If we are just getting started */

  /* Now fill in the items which should get updated at the sample rate */

  /* RX AGC sample */
  srch_agc_pctl_vars.log.agc_vals[ srch_agc_pctl_vars.inx ]
                                        = srch_get_rx_agc();

  /* TX gain adjust */
  srch_agc_pctl_vars.log.pctl_vals[ srch_agc_pctl_vars.inx ]
                                        = srch_get_tx_adj();

  /* Get top 8 bits of CAGC TX power estimate */
  srch_agc_pctl_vars.log.tx_pwr_vals[ srch_agc_pctl_vars.inx ] =
    (byte)( srch_rx_get_tx_agc() >> 2 );

  if ( ( ++srch_agc_pctl_vars.inx >= LOG_AGC_PCTL_CNT ) ||
       srch_ho_to_oband ) /* Buffer full or HO to a different Band Class? */
  {
    /* We've filled up the logging buffer. */

    if (log_submit((log_type *) &srch_agc_pctl_vars.log) == FALSE)
    {
      srch_agc_pctl_vars.drops++;
    }

    srch_agc_pctl_vars.inx = 0;
      /* Point to first bin in the hopper. This gets bumped
         on every logging call.  It also signals a new buffer. */

    srch_ho_to_oband = FALSE;
      /* Reset srch_ho_to_oband so no more brute force shipment before the
         next hard handoff to o-band */
  }

} /* srch_log_agc_pctl() */


/*===========================================================================

FUNCTION SRCH_SET_TA_PARM

DESCRIPTION    This function is used to set what the searcher task
      reports to the temporal analyzer.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   srch_ta_parms

===========================================================================*/

void srch_set_ta_parm
(
  word  set_mask,
    /* Specifies which type of pilot sets should be reported.
       Bit 15 => report active set pilot scans.
       Bit 14 => report candidate set pilot scans.
       Bit 13 => report neighbor set pilot scans.
       Bit 12 => report remaining set pilot scans. */

  word  ta_cnt
    /* Specifies desired size of each scan in 1/2 chip units. */
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* REVISIT - why is this called from DIAG? */

  NOTUSED( set_mask );
  NOTUSED( ta_cnt );

} /* srch_set_ta_parm */


/*===========================================================================

FUNCTION SRCH_NOTIFY_FPC_MODE

DESCRIPTION    This function tells search that FPC is enabled.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/

void srch_notify_fpc_mode
(
  boolean fpc_enabled,
    /* FPC on-off status */

  uint16 mode
    /* Mode passed as a word for logging */
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Search must be in TC state to use this data */
  if(IS_TRAFFIC_STATE(srch_state))
  {
    srch_tc_save_fpc_info(fpc_enabled,mode);
  }

} /* srch_notify_fpc_mode */


/*===========================================================================

FUNCTION SRCH_UPDATE_RC

DESCRIPTION    This function updates the srch's local copy of the FCH RC
      value which is necessary for logging.  This function is called from
      the MUX layer where the RC value is updated.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/

void srch_update_rc
(
  deint_rc_type  fch_rc_val,
    /* Fundamental channel RC value */

  deint_rc_type  sch0_rc_val,
    /* Supplemental channel 0 RC value */

  deint_rc_type  sch1_rc_val,
    /* Supplemental channel 1 RC value */

  deint_rc_type  dcch_rc_val
    /* Dedicated Control channel RC value */
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  srch_rc.fch = fch_rc_val;
    /* Update the srch FCH RC value */

  srch_rc.sch0 = sch0_rc_val;
    /* Update the srch FCH RC value */

  srch_rc.sch1 = sch1_rc_val;
    /* Update the srch FCH RC value */

  srch_rc.dcch = dcch_rc_val;
    /* Update the srch FCH RC value */

} /* srch_update_rc */


/*===========================================================================

FUNCTION SRCH_GET_PILOT_SET

DESCRIPTION    This function fills up a diagnostic pilot set status
      packet with current set populations and memberships.

DEPENDENCIES   srch_?set, srch_?set_cnt, srch_parms.pilot_inc OR
               srch_sect_loaded() == TRUE

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/

void srch_get_pilot_set
(
  srch_pilot_set_struct_type *pkt_ptr
    /* The DIAG pilot packet to fill in */
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* This function uses sect's pilot data interface */
  srch_diag_get_pilot_set( pkt_ptr );

} /* srch_get_pilot_set */


/*===========================================================================

FUNCTION SRCH_RPT

DESCRIPTION    This function enqueues a specified report from the Search
      Task to the Main Control Task.

DEPENDENCIES   This function assumes that the 'mcc_srch_q' has been properly
      initialized.

RETURN VALUE   None.

SIDE EFFECTS   The MC Task dequeues and processes the specified report.

===========================================================================*/

void  srch_rpt
(
  mccsrch_rpt_type  *rpt_ptr
    /* Pointer to searcher report. */
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( !rpt_ptr )
  {
    MSG_ERROR("Unexpected NULL rpt_ptr, report not queued", 0, 0, 0);
    return;
  }

  MSG_MED("srch->mc id=%d", rpt_ptr->hdr.rpt, 0, 0);

  rpt_ptr->hdr.rpt_hdr.done_q_ptr   = &mcc_srch_free_q;
    /* Tell the buffer where to go when freed */

  rpt_ptr->hdr.rpt_hdr.task_ptr = NULL;
    /* Tell it not to bother alerting any task when freed */

  q_put ( &mcc_srch_q, q_link ( rpt_ptr, &rpt_ptr->hdr.rpt_hdr.link ) );
    /* Enqueue report to Main Control Task. */

  (void) rex_set_sigs ( &mc_tcb, MCC_Q_SIG );
    /* Signal Main Control Task. */

} /* srch_rpt */


/*===========================================================================

FUNCTION SRCH_WAIT

DESCRIPTION    This function just performs a REX wait as specified by the
      caller, except that it automatically kicks the watchdog timer every
      so often while waiting.

DEPENDENCIES   Relies on SRCH_RPT_TIMER_SIG being raised often enough to
      avoid watchdog alarms, so should not be called until srch_init has
      run.

RETURN VALUE   Returns the value returned by rex_wait.

SIDE EFFECTS   The watchdog gets kicked periodically.

===========================================================================*/

rex_sigs_type  srch_wait
(
  rex_sigs_type  sigs
    /* Mask of signals to wait for */
)
{
  rex_sigs_type  ret_val;
    /* Return value from REX: bitmap of signals found raised */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Keep waiting and kicking the watchdog as long as the return
     value has the kick timer signal set and none of the user's
     desired signals are set. */

  do
  {
    ret_val = rex_wait ( sigs | SRCH_RPT_TIMER_SIG );
      /* Wait for the caller's conditions OR our kick timer */

    if ( ret_val & SRCH_RPT_TIMER_SIG )
    {
      SRCH_DOG_RPT ( );
        /* Kick the watchdog */
    }
  }
  while ( (ret_val & sigs ) == 0 );
    /* Keep looping as long as user's conditions are un-met */

  return ( ret_val );
    /* Return the rex_wait return value with user condition(s) set */

} /* srch_wait */


/*===========================================================================

FUNCTION SRCH_GET_CMD

DESCRIPTION    This function gets a command for the searcher task.
      It just gets one off the srch_cmd_q.

DEPENDENCIES   srch_cmd_q

RETURN VALUE   Pointer to the command.

SIDE EFFECTS   srch_cmd_q

===========================================================================*/

srch_cmd_type  *srch_get_cmd ( void )
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  (void) rex_clr_sigs ( &srch_tcb, SRCH_CMD_Q_SIG );
    /* Clear the new-command signal */

  return ( (srch_cmd_type  *) q_get ( &srch_cmd_q ) );
    /* Get the command and return it */

} /* srch_get_cmd */


/*===========================================================================

FUNCTION SRCH_POST_CMD

DESCRIPTION    This function post-processes a command.  It reports
      errors to the diagnostic monitor.  It handles returning the
      command to a queue and/or signalling a task as specified in
      the command itself.  It maintains a count for debug purposes
      of how many times each return value was returned.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   A command is posted back to MC.

===========================================================================*/

void  srch_post_cmd
(
  srch_cmd_type  *cmd_ptr,
    /* Points to the command to perform post processing on. */

  srch_cmd_status_type  status
    /* Status to post for command. */
)
{
  rex_sigs_type sigs;
  rex_tcb_type *tcb_ptr;
    /* Signal and task to notify */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( cmd_ptr == NULL )
  {
    MSG_ERROR("Unexpected NULL pointer: cmd_ptr", 0, 0, 0);
    return;
  }

  /* Check for error status conditions */
  if ( (status != SRCH_DONE_S) && (status != SRCH_TOOLATE_S) &&
       (status != SRCH_SLEEP_S) )
  {
    ERR ( "Bad status %d, cmd %d", status, cmd_ptr->hdr.cmd, 0 );
      /* There was an error. Report it to the DM */
  }

  cmd_ptr->hdr.status = status;
    /* Copy the status into the command */

  tcb_ptr = cmd_ptr->hdr.cmd_hdr.task_ptr;
  sigs    = cmd_ptr->hdr.cmd_hdr.sigs;
    /* Save task pointer and signals for using after the q_put() */

  /* Check to see if the command wants to be put on a queue when finished */
  if ( cmd_ptr->hdr.cmd_hdr.done_q_ptr != NULL )
  {
    q_put ( cmd_ptr->hdr.cmd_hdr.done_q_ptr, &cmd_ptr->hdr.cmd_hdr.link );
      /* Indeed it does. Do as the command asks */
  }

  /* Check to see if some task wants to be signalled when command is done. */
  if ( tcb_ptr != NULL )
  {
    (void) rex_set_sigs ( tcb_ptr, sigs );
      /* Someone does. Notify the task requesting signaling */
  }

} /* srch_post_cmd */


/*===========================================================================

FUNCTION SRCH_CMD

DESCRIPTION    This function places a specified command onto the Search
      Task's command queue for processing by the Search Task. Notice that
      this function is invoked by tasks other than the Search Task.

DEPENDENCIES   This function assumes that the 'srch_cmd_q' has been properly
      initialized.

RETURN VALUE   None.

SIDE EFFECTS   The Search Task dequeues and processes the specified command.
===========================================================================*/

void  srch_cmd
(
  srch_cmd_type  *cmd_ptr
    /* Pointer to searcher command. Upon queueing, the 'status'
       field of the command block is set to 'SRCH_BUSY_S'. */
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( !cmd_ptr )
  {
    MSG_ERROR("Unexpected NULL pointer: cmd_ptr", 0, 0, 0);
    return;
  }

  cmd_ptr->hdr.status = SRCH_BUSY_S;
    /* Mark command status as busy. Search Task will set final
       outcome status after processing command. */

  MSG_MED("mc->srch id=%d", cmd_ptr->hdr.cmd, 0, 0);

  SRCH_ENTER_CRIT_SECT();

  q_put ( &srch_cmd_q, q_link ( cmd_ptr, &cmd_ptr->hdr.cmd_hdr.link ) );
    /* Place command onto the Search task's command queue. */

  (void) rex_set_sigs ( &srch_tcb, SRCH_CMD_Q_SIG );
    /* Signal Search task. */

  SRCH_LEAVE_CRIT_SECT();

} /* srch_cmd */


/*===========================================================================

FUNCTION SRCH_START_PC_STATE

DESCRIPTION    This function returns TRUE the first time it is called
      after Search enters PC state.

DEPENDENCIES   srch_state.

RETURN VALUE   boolean indicating if this is first call (TRUE).

SIDE EFFECTS   none.

===========================================================================*/
boolean srch_start_pc_state(boolean reset_only)
{
  static boolean called_in_pc_state=FALSE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( reset_only )
  {
    called_in_pc_state = FALSE;
  }
  else if ( !called_in_pc_state && (srch_state == SRCH_PC_STATE) )
  {
    called_in_pc_state = TRUE;
    return( TRUE );
  }

  return( FALSE );

} /* srch_start_pc_state */


/*===========================================================================

FUNCTION SRCH_GET_SLEEP_STAT

DESCRIPTION    This function writes sleep status (enabled/disabled) to DB.
      Note, this function used to read from NV SPARE_1 in first-generation
      targetg, but that feature has since been removed.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   Writes to DB.

===========================================================================*/

static void  srch_get_sleep_stat ( void )
{
  db_items_value_type  db_value;
    /* Database structure to place values into */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  db_value.sleep_mode = DB_SLEEP_ENABLED;
  db_put(DB_SLEEP_MODE, &db_value);
    /* Set up default values - Sleep always enabled. */

} /* srch_get_sleep_stat */


/*===========================================================================

FUNCTION SRCH_RESET_PILOT_INFO

DESCRIPTION    This function resets the pilot record type for the all
      pilot sets and the fingers.  It also reset the FCH and DCCH active
      flags.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
void  srch_reset_pilot_info( void )
{
  srch_sect_list_type sectors;
  uint32 i, size;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sectors = srch_sect_list_get( SRCH_SECT_ELIST_A, &size, FALSE );
  for ( i = 0; i < size; i++ )
  {
    if ( sectors[ i ] != NULL )
    {
      sectors[ i ]->fch_info.active  = TRUE;
        /* Default mode is FCH active */

      sectors[ i ]->dcch_info.active = FALSE;
        /* Default mode is DCCH inactive */

      #ifdef FEATURE_IS95B_MDR
      sectors[ i ]->scch_info.num = 0;
        /* Default mode is no SCCH active */
      #endif /* FEATURE_IS95B_MDR */

      sectors[ i ]->sch_info.num = 0;
        /* Default mode is no SCH active */

      sectors[ i ]->dv_pilot_info.pdcch.num = 0;
        /* Default mode is no PDCCH active */

      sectors[ i ]->dv_pilot_info.gch.num = 0;
        /* Default mode is no GCH active */

      sectors[ i ]->dv_pilot_info.ackch.num = 0;
        /* Default mode is no ACKCH active */

      sectors[ i ]->dv_pilot_info.rcch.num = 0;
        /* Default mode is no RCCH active */

      sectors[ i ]->search_parms.pilot_rec.type = CAI_PILOT_1X_COMMON;
        /* Reset active pilot type to common pilot */
    }
  }

  sectors = srch_sect_list_get( SRCH_SECT_ELIST_C, &size, FALSE );
  for ( i = 0; i < size; i++ )
  {
    if ( sectors[ i ] != NULL )
    {
      sectors[ i ]->search_parms.pilot_rec.type = CAI_PILOT_1X_COMMON;
        /* Reset candidate pilot type to common pilot */
    }
  }

  sectors = srch_sect_list_get( SRCH_SECT_LIST_TC_N, &size, FALSE );
  for ( i = 0; i < size; i++ )
  {
    sectors[ i ]->search_parms.pilot_rec.type = CAI_PILOT_1X_COMMON;
      /* Reset neighbour pilot type to common pilot */
  }

  sectors = srch_sect_list_get( SRCH_SECT_LIST_TC_CF_N, &size, FALSE );
  for ( i = 0; i < size; i++ )
  {
    sectors[ i ]->search_parms.pilot_rec.type = CAI_PILOT_1X_COMMON;
      /* Reset candidate frequency set pilot type to common pilot */
  }

  srch_fing_reset_chans( );
    /* Reset finger channels to FCH only */

} /* srch_reset_pilot_info */


/*===========================================================================

FUNCTION SRCH_ENABLE_COMB_DSP_INTS_MANAGER

DESCRIPTION    This function enables Combiner interrupts to the DSP
               if either 1x SRCH, or GPS Search needs them. Or it disables
               Combiner interrupts if both 1x SRCH and GPS Search do not need
               them.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   Combiner DSP ints can be enabled or disabled

===========================================================================*/
void srch_enable_comb_dsp_ints_manager
(
  boolean enable,                      /* enable flag */
  boolean cdma                         /* TRUE if 1x, FALSE if GPS */
)
{
  static boolean cdma_needs_comb_dsp_ints = FALSE;
  static boolean gps_needs_comb_dsp_ints  = FALSE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( cdma == TRUE )
  {
    if( enable )
    {
      /* 1x SRCH needs to enable Combiner */
      srch_comb_enable_dsp_ints( TRUE );

      cdma_needs_comb_dsp_ints = TRUE;
    }
    else
    {
      /* 1x SRCH does not needs Combiner anymore */
      if( gps_needs_comb_dsp_ints == FALSE  )
      {
        /* Since GPS Search does not need Combiner it is okay to disable it */
        srch_comb_enable_dsp_ints( FALSE );
      }

      cdma_needs_comb_dsp_ints = FALSE;
    }
  }
  else
  {
    /* GPS Search has called this function */
    if( enable )
    {
      /* GPS Search needs to enable Combiner */
      srch_comb_enable_dsp_ints( TRUE );

      gps_needs_comb_dsp_ints = TRUE;
    }
    else
    {
      /* GPS Search does not needs Combiner anymore */
      if( cdma_needs_comb_dsp_ints == FALSE )
      {
        /* Since 1x SRCH does not need Combiner it is okay to disable it */
        srch_comb_enable_dsp_ints( FALSE );
      }

      gps_needs_comb_dsp_ints = FALSE;
    }
  } /* cdma == TRUE */

} /* srch_enable_comb_dsp_ints_manager */


/*===========================================================================

FUNCTION SRCH_ENABLE_COMB_DSP_INTS

DESCRIPTION    This function enables/disables Combiner interrupts to the DSP.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   Combiner DSP ints can be enabled or disabled

===========================================================================*/
void srch_enable_comb_dsp_ints
(
  boolean enable                       /* enable flag */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  srch_enable_comb_dsp_ints_manager( enable, TRUE );

} /* srch_enable_comb_dsp_ints */


/*===========================================================================

FUNCTION SRCH_SET_PNROLL_ISR_MANAGER

DESCRIPTION    This function registers PN Roll ISR if either 1x SRCH, or
               GPS Search needs it. Or it deregisters PN Roll ISR if both
               1x SRCH and GPS Search do not need it.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   PN roll ISR can be registered or deregistered

===========================================================================*/
void srch_set_pnroll_isr_manager
(
  void (*isr_ptr)( void ),             /* ISR function */
  boolean cdma                         /* TRUE if 1x, FALSE if GPS */
)
{
  #ifdef FEATURE_SRCH_CGPS_IF
#error code not present
  #else /* !FEATURE_SRCH_CGPS_IF */
  static boolean cdma_needs_pnroll_isr = FALSE;
  static boolean gps_needs_pnroll_isr  = FALSE;
  #endif /* FEATURE_SRCH_CGPS_IF */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_SRCH_CGPS_IF
#error code not present
  #else /* !FEATURE_SRCH_CGPS_IF */

  if( cdma == TRUE )
  {
    /* 1x SRCH has called this function */
    if( isr_ptr != NULL )
    {
      /* 1x SRCH requests PN Roll ISR to be registered */
      tramp_set_isr( TRAMP_PNROLL_ISR, isr_ptr );

      cdma_needs_pnroll_isr = TRUE;
    }
    else
    {
      /* 1x SRCH requests PN Roll ISR to be deregistered */
      if( gps_needs_pnroll_isr == FALSE  )
      {
        /* Since GPS Search does not need RN Roll ISR it is okay to
           deregister it */
        tramp_set_isr( TRAMP_PNROLL_ISR, isr_ptr );
      }

      cdma_needs_pnroll_isr = FALSE;
    }
  }
  else
  {
    /* GPS Search has called this function */
    if( isr_ptr != NULL )
    {
      /* GPS Search requests PN Roll ISR to be registered */
      /* Register PN Roll ISR only if 1x SRCH is not using it */
      if( cdma_needs_pnroll_isr == FALSE )
      {
        tramp_set_isr( TRAMP_PNROLL_ISR, isr_ptr );
      }

      gps_needs_pnroll_isr = TRUE;
    }
    else
    {
      /* GPS Search requests PN Roll ISR to be deregistered */
      if( cdma_needs_pnroll_isr == FALSE  )
      {
        /* Since 1x SRCH does not need RN Roll ISR it is okay to
           deregister it */
        tramp_set_isr( TRAMP_PNROLL_ISR, isr_ptr );
      }

      gps_needs_pnroll_isr = FALSE;
    }
  }

  #endif /* FEATURE_SRCH_CGPS_IF */
} /* srch_set_pnroll_isr_manager */


/*===========================================================================

FUNCTION SRCH_SET_PNROLL_ISR

DESCRIPTION    This function registers/deregisters PN Roll ISR.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
void srch_set_pnroll_isr
(
  void (*isr_ptr)( void )              /* ISR function */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  srch_set_pnroll_isr_manager( isr_ptr, TRUE );

} /* srch_set_pnroll_isr */


/*===========================================================================

FUNCTION SRCH_VOTE_OK_TO_SLEEP

DESCRIPTION    This function tells sleep that tcxo shutdown is ok (i.e.
  ok to sleep)

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
void srch_vote_ok_to_sleep( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sleep_assert_okts( srch_sleep_handle );

} /* srch_vote_ok_to_sleep */


/*===========================================================================

FUNCTION SRCH_VOTE_NOT_OK_TO_SLEEP

DESCRIPTION    This function tells sleep that tcxo shutdown is not ok (i.e.
  not ok to sleep)

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
void srch_vote_not_ok_to_sleep( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sleep_negate_okts( srch_sleep_handle );

} /* srch_vote_not_ok_to_sleep */


/*===========================================================================

FUNCTION SRCH_EXIT_SERVICE

DESCRIPTION    This function does all the common processing needed when
               exiting zz, pc, or tc states, and entering inactive or cdma
               states.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
void srch_exit_service( void )
{
  #ifndef FEATURE_BRINGUP_DISABLE
  /* Get the max finger count */
  uint32 fing_cnt = srch_fing_get_max_fing_cnt();
  #endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* REVISIT: temporarily disable the fing disable for QSC1100 */
  #ifndef FEATURE_BRINGUP_DISABLE
  /* disable all fingers when we exit 1x service. */
  srch_fing_disable( 0, ( fing_cnt - 1 ) );
  #endif

  /* The demodulator position will be frozen soon.  Time services should
     no longer rely on its position for accurate timestamps. */
  time_cdma_set_valid( FALSE );

  /* Turn off the TX clocks in the Interleaver and Encoder.  This
     saves a bit more power, and keeps the out-of-spec CHIPx8 clock
     from screwing up the state of the circuit. Do the same for Decoder. */
  enc_set_mode( ENC_SLEEP_MODE );

  /* Disable the Combiner as it is not needed anymore */
  srch_enable_comb_dsp_ints( FALSE );

  /* Turn off handling of the PN roll interrupt. */
  srch_set_pnroll_isr( NULL );

  /* Release any RX chains owned or requested */
  srch_rx_release_owned_chains();

  /* Deregister 1x with srch4 and abort any search currently running */
  srch4_1x_deregister();

} /* srch_exit_service */

/*===========================================================================

FUNCTION SRCH_FILT_IIR

DESCRIPTION    Gradually grow filter until max filter length, then let it be
               a fixed length filter. This lets the filter quickly ramp and
               be equivalent to the normal IIR filter.  Growing filter has a
               straight linear averaging characteristic while the fixed filter
               has the normal (n-1)/n characteristic

DEPENDENCIES   None

RETURN VALUE   New filter length

SIDE EFFECTS   None

===========================================================================*/
uint32 srch_filt_iir
(
  uint32 *filter,  /* Filter */
  uint32 input,    /* Value to filter in */
  uint32 len,      /* Length of filter */
  uint32 max       /* Maximum filter length, if non-zero then filter length
                      is grown but will not exeed this value */
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( !filter )
  {
    MSG_ERROR("Unexpected NULL pointer: filter", 0, 0, 0);
    return ( len );
  }

  /* Is a max filter length requested? */
  /* haven't hit max so grow the filter */
  if ((max == 0) || (len < max))
  {
    len++;
  }
  else
  {
    /* the filter is too large, cut it back to max length */
    len = max;
  }

  *filter = ((*filter * (len - 1)) + input) / len;

  return len;

} /* srch_filt_iir */


/*===========================================================================

FUNCTION  SRCH_IN_SERVICE

DESCRIPTION    This function returns true if srch is on the system. It returns
     false if search is either inactive or trying to get on the system.

DEPENDENCIES   None.

RETURN VALUE   TRUE - srch in on the system; otherwise, FALSE

SIDE EFFECTS   None.

===========================================================================*/
boolean srch_in_service (void)
{
  /* This is implemented as a case statement with all of the states enumerated
   * so if anyone adds or changes a state a lint warning will flag this code
   * block to be updated.
   */
  switch(srch_state)
  {
    case SRCH_INACTIVE_STATE:
    case SRCH_CDMA_STATE:
    case SRCH_ACQ_STATE:
    case SRCH_SC_STATE:
    case SRCH_SLEW_STATE:
    case SRCH_UNSLEW_STATE:
    case SRCH_PCG_STATE:
    case SRCH_POWERUP_STATE:
    case SRCH_MAX_STATE:
    {
      return(FALSE);
    }

    case SRCH_PC_STATE:
    case SRCH_ZZ_STATE:
    case SRCH_TC_STATE:
    {
      return(TRUE);
    }
  }

  /* shouldn't get here */
  return(FALSE);

} /* srch_in_service */


/*===========================================================================

FUNCTION SRCH_INIT

DESCRIPTION    This function is called at startup to put everything into
       a known, benign state.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   Queues and timers are initialized.
               Regional variables are initialized.

===========================================================================*/

static void  srch_init ( void )
{
  db_items_value_type  db_value;
    /* Data Base structure to place values into */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  srch_diag_init();
  /* Initialize Search diag subsystem command-response logging */

  #ifdef FEATURE_MODEM_1X_DIAG
  #if defined(FEATURE_IS2000) && !defined(FEATURE_GSM_PLT)
  cdma2kdiag_init();
  /* Initialize the c2k diag system */
  #endif
  #endif /* FEATURE_MODEM_1X_DIAG */

  srch_peak_init();
  /* Initialize the Search Peak Module */

  srch_sect_init();
  /* Initialize Search Sector Module and load it */

  srch_fing_init();
    /* Initialize Finger Driver Module */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   /* Set up Free Queues of various resources */

  (void) q_init ( &srch_cmd_q );

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  timer_def(&srch_rpt_timer, NULL, &srch_tcb, SRCH_RPT_TIMER_SIG, NULL, 0);
    /* Set up the timer for kicking the watchdog */

  srch4trans_init();
    /* initialize searcher 4 (timers, etc) */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  srch_update_rc ( DEINT_RC_INVALID, DEINT_RC_INVALID,
                   DEINT_RC_INVALID, DEINT_RC_INVALID );
    /* Initial the RC values, used for logging purposes */

  srch_sleep_handle = sleep_register( "1X_SRCH", FALSE );
    /* Register for sleep voting, initial vote not ok to sleep */

  /* Initialize AGC log buffer.  Since it takes too long to accumulate data,
     we must use our own buffer space, then submit it to LOG services.
     The following never changes. */

  srch_agc_pctl_vars.log.hdr.code = LOG_S_AGC_PCTL_C;
    /* Header for sparse (20ms) AGC and pctl sampling mode */

  srch_agc_pctl_vars.log.hdr.len  = sizeof(log_s_agc_pctl_type);
    /* Fill in log packet length */

  srch_agc_pctl_vars.enabled  = FALSE;
  srch_agc_pctl_vars.inx      = 0;
  srch_agc_pctl_vars.drops    = 0;
    /* AGC logging defaults to disabled, and no AGC log entries
       have been dropped. */

  srch_parms.soft_slope        = 0;
    /* Default to IS95A */

  /* Initialize srch_parms to reasonable values */
  srch_parms.aset_win_siz = srch_win_siz;
  srch_parms.nset_win_siz = srch_win_siz;
  srch_parms.rset_win_siz = srch_win_siz;

  srch_parms.pilot_inc = 0;
    /* Don't do Remaining set searches until we get a real PILOT_INC */

  db_value.acq_mode = DB_ACQ_FULL;
  db_put(DB_ACQ_MODE, &db_value);
    /* Set up default Acquisition mode values - Acq always full. */

  db_value.sleep_active = DB_SLEEP_OFF;
  db_put(DB_SLEEP_ACTIVE, &db_value);
    /* Indicate that Sleep is off - we are fully awake */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  srch_roll_isr_vars.tick26 = 0;
  srch_roll_isr_vars.phase80 = 0;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  srch_idle_init();

} /* srch_init */


/*===========================================================================

FUNCTION SRCH_TASK

DESCRIPTION    This function is the main routine for the Search Task.

      Several main purpose(s) of the Search task are to:

        1.  Initially search for, and find, a CDMA pilot in-channel,

        2.  Measure neighboring cell's pilot energies and call for handoffs,

        3.  Establish and maintain CDMA system time and frequency track,

        4.  Keep multipath RAKE receiver fingers/combiners tracking good pilots,

        5.  Put the MSM chip to sleep and re-establish acquisition after wakeup.

      The Search task's state machine begins in INACTIVE, and under control by
      Main Control, proceeds generally through the following transitions:
                                    _____________________________
                                   |           /|\               |
                                   |            |                |
                                  \|/           |                |
        INACTIVE -> CDMA -> ACQ -> SC -> SL -> PC -> TC -> UN -> +
                                               /|\
                                                |
                                               \|/
                                               ZZ

      Interfacing between Search and other tasks is accomplished by means of
      queues, called interfaces, and REX signals. Types of information passed
      between Search and other tasks include state change commands, reports,
      data tables to store to and read from NV, search, finger, and AGC/power
      control data packets, and mode information.

      Also, Search interacts heavily with the CDMA demodulation hardware.
      This hardware includes a searcher, demodulation fingers, symbol,
      frequency, and power combiners, and AGC reference control circuitry.
      The Search task interfaces with the demodulation hardware by means
      of device drivers and interrupt callback functions.

      The following table summarizes the main interfaces which exist between
      Search and the rest of DMSS:

       Interface       Type of
         With         Interface             Why Information Exchanged
        ------        ---------             -------------------------
        MC Task       REX queues            MC sending commands to Search
                                            Search sending reports to MC

        CDMA Demod    Device drivers        Search controlling demod hardware
        Hardware                            (searcher, fingers, combiner, AGC)

        TRAMP         Interrupt callbacks   Demod hardware events (PN roll,
        Services                            searcher dump, wakeup, keypress)

        DIAG Task     REX queues            Search sending searcher, finger,
                                            AGC, power control data to the DM

        NV Task       Called interfaces     Search storing/retrieving PN mask,
                                            sleep status, freq-temp estimation
                                            table to/from nonvolatile memory

        DB Services   Called interfaces     Search storing/retrieving sleep
                                            status, acquisition mode, RF mode
                                            (PCN -vs- Cellular)

        Sleep Task    Called interfaces     Search programming sleep timer,
                      REX signals           signaling Sleep task for powerdown

        DOG Task      Called interface      Search kicking the watchdog while
                                            waiting on REX signals.

      Upon task creation, the Search task main routine performs initializations
      such as setting up queues, defining timers, and reading parameters from
      NV, then enters an infinite loop which runs the Search main state machine.

DEPENDENCIES   This function assumes that the demodulator as already been
      initialized into a benign state.

RETURN VALUE   None.

SIDE EFFECTS   Everything in the world.

===========================================================================*/

void  srch_task
(
  dword  param
    /* Task startup parameter. Ignored by Search task. */
)
{
  srch_state_enum_type  prev_srch_state = SRCH_INACTIVE_STATE;
    /* Previous search state */

  static const char srch_state_str[ ( ( uint16 ) SRCH_MAX_STATE + 1 ) ][ 13 ] =
  {
    "Reserved",
    "Inactive",
    "CDMA",
    "Acq",
    "Sync",
    "Slew",
    "Paging",
    "Sleep",
    "Traffic",
    "Reserved",
    "PCG",
    "PowerUP",
    "TimeTransfer",
    ""
  };

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  NOTUSED( param );
  
  #if defined(T_MSM6700) && !defined(T_MSM6800B)
  /* Only disable QPCH if this is 6800 r1.0 */
  if( HW_MSM6800_HAS_R1_V0 )
  {
    bio_set_sw(BIO_SW13_M);
      /* Disable QPCH for MSM6700/6800A */
  }
  #endif /* T_MSM6700 && !T_MSM6800B */

  #ifdef T_QSC60X5
  #if !defined(FEATURE_MPM_HW_BUG_FIXED) && defined(T_QSC60X5REV1_0)
  bio_set_sw(BIO_SW9_M);
    /* ensure sleep is disabled */
  #endif
  #endif

  /* Because of the MPM bug, external sleep clock has to be shutdown.
  ** Sleep controller cannot be driven by TCXO/512. This is a temporary
  ** workaround before new HW or a better workaround
  ** This applies only to QSC60X5 HW rev 1.0
  */
  #if ( !defined( T_QSC60X5 ) || \
          defined( FEATURE_MPM_HW_BUG_FIXED ) || \
        !defined( T_QSC60X5REV1_0 ) )
  sleepctl_init();
    /* Perform Sleep Controller initialization */
  #endif /* !T_QSC60X5 || FEATURE_MPM_HW_BUG_FIXED || !T_QSC60X5REV1_2 */

  srch_rx_init();
    /* Initialize Srch RX module */

  srch_init ();
    /* Perform Search task initializations */

  l1m_init();
    /* init the layer 1 manager (eventually this should be its own task) */

  srch_bm_init_cmd_buf_pool();
    /* Initialize the buffer manager */

  srch_chan_reset();
    /* init the srch chan subsystem */

  task_start ( SRCH_RPT_TIMER_SIG, DOG_SRCH_RPT, &srch_rpt_timer );
    /* Start us up as a task */

  SRCH_DOG_RPT ();
    /* Report to watchdog task. */

  #ifdef FEATURE_GPS_MODE
#error code not present
  #endif /* FEATURE_GPS_MODE */

  #ifndef FEATURE_SRCH_HAS_NO_IRM
  srch_irm_resource_update( SRCH_IRM_RXCX8_RESOURCE |     \
                            SRCH_IRM_RTC_RESOURCE |       \
                            SRCH_IRM_RTC_SYNC_RESOURCE |  \
                            SRCH_IRM_SRCH4_SCHED_RESOURCE,
                            SRCH_IRM_RESOURCE_AVAILABLE );
    /* These resources are available at the moment */
  #endif /* FEATURE_SRCH_HAS_NO_IRM */

  #ifndef FEATURE_SRCH_HAS_NO_TRM
  trm_init( &srch_tcb, SRCH_NV_CMD_SIG, void_srch_wait );
    /* Initialize TRM (Temp) */
  #endif /* FEATURE_SRCH_HAS_NO_TRM */

  srchcom_init();
    /* Start the Common State Machine Group */

  srch_afc_init();
    /* Initialize the AFC module */

  srch_get_sleep_stat();
    /* Get the default enabled sleep types from NV */

  srch_nv_read_nv_items();
    /* Read searcher related NV items that are needed at power up. */

  #ifdef FEATURE_GPSONE_PPM
#error code not present
  #endif /* FEATURE_GPSONE_PPM */

  srch_state = SRCH_POWERUP_STATE;
    /* Set initial state for state machine. */

  /* Try to initialize the inactive state */
  if ( srch_inactive_init ( SRCH_POWERUP_STATE ) != SRCH_DONE_S )
  {
    ERR_FATAL ("Can't start", 0, 0, 0 );
      /* This didn't succeed, so we're doomed. */
  }

  srch_state = SRCH_INACTIVE_STATE;

  MSG_SPRINTF_1(MSG_SSID_1X_SRCH,MSG_LEGACY_HIGH,"Using %s",srch_lib_int_ver());
    /* Print the version of the search library being used */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (;;)                    /* Infinite loop */
  {

    MSG_SPRINTF_2(MSG_SSID_1X_SRCH,MSG_LEGACY_HIGH,"Srch State (%s) -> (%s)",
      srch_state_str[prev_srch_state],srch_state_str[srch_state]);

    prev_srch_state = srch_state;

    switch ( srch_state )     /* Run the main Search task state machine */
    {
      case SRCH_INACTIVE_STATE:       /* Release resources, vote OK to sleep */
      {
        srch_state = srch_inactive_state ();
        break;
      }

      case SRCH_CDMA_STATE:             /* Beginning of CDMA - transitional */
      {
        srch_state = srch_cdma_state ( );
        break;
      }

      case SRCH_ACQ_STATE:              /* CDMA initial pilot acquisition */
      {
        #ifdef FEATURE_ACQ2
        srch_state = srch_acq_state ( );
        #endif /* FEATURE_ACQ2 */

        break;
      }

      case SRCH_SC_STATE:               /* Pilot search and finger assignment */
      {                                 /* while in Sync channel state        */
        srch_state = srch_sc_state ( );
        break;
      }

      case SRCH_SLEW_STATE:             /* "Jump to hyperspace"  - searcher & */
      {                                 /* finger slew, combiner time align */
        srch_state = srch_slew_state ( );
        break;
      }

      case SRCH_PC_STATE:               /* Pilot search and finger assignment */
      {                                 /* while in Paging channel state      */
        srch_state = srch_idle_ctl_state ( );
        break;
      }

      case SRCH_TC_STATE:               /* Pilot search and finger assignment */
      {                                 /* while in Traffic channel state     */
        srch_state = srch_tc_state ( );
        break;
      }

      case SRCH_TT_STATE: /* start doing DO time transfering to 1x */
      {
        #ifdef FEATURE_SRCH_VOIP_1X_HANDOFF
#error code not present
        #endif  /* FEATURE_SRCH_VOIP_1X_HANDOFF */
        break;
      }

      default:                           /* No other states are valid */
      {
        ERR_FATAL ( "Bad state %d", srch_state, 0, 0 );
      }

    } /* switch */

  } /* for (;;) */

} /* srch_task */

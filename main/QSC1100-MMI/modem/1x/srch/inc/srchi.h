#ifndef SRCHI_H
#define SRCHI_H
/*===========================================================================

                   S R C H I    H E A D E R    F I L E

DESCRIPTION
   This file contains definitions used internally by the Search task.
   It should NOT be included by any source file outside the Search task!

    Copyright (c) 1991 - 2008
                  by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/1x/srch/rel/1h08/inc/srchi.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/28/08   adw     Included srch_dep.h to verify internal feature dependencies.
08/13/08   adw     Added FEATURE_SRCH_MSG_LITE for ULC.
                   Replaced STM_NULL_PAYLOAD_CHECK w/ SRCH_NULL_PAYLOAD_CHECK.
08/13/08   adw     Featurized SRCH_NULL_PTR_CHK to remove MSG_ERROR for ULC.
08/24/07   sst     Updated init function param to receive srch_cmd_type
                   Removed _pc_ and _zz_ functions that no longer exist
05/02/07   cab     Moved to new sleep voting
12/15/06   tjc     Modified traffic to use the sector module
12/14/06   pa      Removed unused function srch_get_freq_est().
10/18/06   bn      Added new functions to handle the time transfering from
                   DO to 1x
10/13/06   trc     Changes for new STM grammar
08/03/06   tjc     Removed unused signals
07/19/06   trc     Decoupling STM from SRCH
07/11/06   tjc     Modified the traffic state to use the state machine module
07/11/06   tjc     Implemented searcher peak module
06/06/06   pa      Removed unused variable srch_last_good_reacq.
06/01/06   tjc     Removed subsignal functions
05/31/06   tjc     Signal cleanup
05/01/06   tjc     Stripped FEATURE_SEARCH1 and mainlined FEATURE_SRCH_PC2
01/17/06   awj     Lint cleanup; type matching
12/02/05   awj     Added FEATURE_SRCH_RELA_RX0_ONLY
11/04/05   kwo     Remove IDLE_HO_ALLOWED_NEIG() macro
11/02/05   awj     Stripped FEATURE_SRCH_PCG, mainlined RTL and IRM
10/25/05   awj     Removed global NV variables, created srch_nv.[ch]
10/18/05   awj     Lint cleanup
10/17/05   kwo     Added SRCH_NULL_PTR_CHK()
10/10/05   awj     Moved srch_report_frame to srchtc.c
09/23/05   ejv     Deprecate rex timer types/calls.
                   Change sum_power to 32 bits.  Delete unused macros,
                   variables, and function calls.
09/19/05   ejv     Remove unused defines.
09/08/05   ejv     Add functions from srch.h.
08/31/05   ejv     Update FEATURE_SRCH_PC2 featurization.  Remove now unused
                   signal checks.  Move IS-95B enum to TC files.
08/11/05   sst     Make FREQ_TIMER_MS 10 secs
08/05/05   ejv     Remove srch_rssi_to_db.
08/03/05   ejv     Mainline FEATURE_TRANS_MANAGER.
07/28/05   ejv     Extend lecio support throughout SRCH task.
07/27/05   pa      Added CHIPS_PER_ROLL, CHIPX8_PER_ROLL, CHIPX8_ROLL_MASK.
07/01/05   ejv     Removed unused ecio_thresh_eng.
06/24/05   ejv     Moved TC-specific struct types and timers to TC files.
06/24/05   ejv     Delete srch_freq_timer_done.
06/23/05   ejv     Move DB_DOWN to srch_conv.h.
06/22/05   sfm     Removed RF_TUNE_DONE() macro
06/13/05   br      Removed unused PPM macros.
06/10/05   ejv     Remove include of srch_fing_driver.h.
05/31/05   rng     Merged in from SHDR Sandbox.
05/24/05   ejv     Removed unused externs.  Moved other to more appropriate
                   locations inside the library.
05/17/05   kwo     Cleanup for library packing
05/16/05   sst     Removed Acq1 and FEATURE_ACQ1_AND_ACQ2
05/04/05   ejv     Moved Antenna-specific Parms to srch_fing_driver.h.
04/25/05   kwo     Merge in of FEATURE_SRCH_PC2
04/01/05   sfm     Added srch_eng to srch_path_struct_type.  Added #defines for
                   path_type.  Removed srch_fing_earliest_fing().
02/17/05   abh/gs  Removed quantization effects on the TrkLo PDM
12/15/04   kwo     Rework of DH interface
12/06/04   bt      Added SRCH_ENTER/LEAVE_CRIT_SECT defines.
12/01/04   rng     Removed depricated srch_powerdown() and
                   srch_store_freq_est().
11/15/04   ejv     Replaced rf_ with rfm_ equivalents.
11/04/04   kwo     Translated DB_DOWN to use srch_conv_db_adj()
11/03/04   kwo     Removed obsolete fade command support
10/25/04   ejv     Removed srch_update_sch_walsh_len.
10/15/04   kwo     Removed unslew
10/12/04   kwo     Removed srchpc_last_slot_eng
10/11/04   ejv     Unfeaturize pilot walsh values.
10/07/04   sfm     Removed include of srch_fing_driver.h
09/30/04   ejv     Moved SRCH_USE_PREV_TRK_LO and srch_cmd_q from srch.h.
09/29/04   sfm     Changed include of srch_fing.h to srch_fing_driver.h
09/27/04   ejv     Moved TC prototypes to srchtc.h.
09/23/04   kwo     Added state macros, FRAME_DONE(), demod info packet support
09/13/04   sst     Integrated with ACQ2.0 dev, featurized as FEATURE_ACQ2
09/09/04   ejv     Remove REL_C_DV featurization.
09/03/04   kwo     Linear ecio triage cleanup
09/03/04   ejv     Add ficch_lcm and cpcch_info.
08/27/04   ejv     Mainline more features.
08/26/04   kwo     Modified DB_DOWN to return a uint32
08/24/04   ejv     Removed srch_win_pool and srch_win_q.
08/23/04   rng     Add define for void_srch_wait.
08/23/04   kwo     Modified srch_filt_iir() to support uint32
08/18/04   gs      Added prototype for srch_get_freq_range().
08/12/04   dna     Transitions to SRCH_START_STATE now go to SRCH_CDMA_STATE
07/26/04   ejv     Remove srch_msm_version.
07/22/04   kwo     Removed finger ecio table
07/22/04   br      Removed unused GPS macros.
07/20/04   gs      Modify VALID_BAND_CHAN to support band classes other than
                   0 and 1.
07/19/04   ejv     Mainline FEATURE_IS95B_MAHHO, _SYS_RESELEC, PILOT_RPT and
                   _ACCESS_HO.
07/08/04   ejv     Remove srch_catnap_ctr_dz.  Move SRCH_INACTIVE_STATE to
                   replace SRCH_DZ_STATE in state enum.
06/30/04   dna     Changed SRCH_START_STATE to SRCH_INACTIVE_STATE.
                   Added srch_exit_service(), called when we enter Inactive
                   state or when we return to CDMA state from any state other
                   than inactive.
06/22/04   br      Moved support for PDSM_SRCH_INTERFACE. Removed unused
                   variables.
06/18/04   dd      Removed GPS NV items.
06/16/04   jcm     Moved TC Specific functions to srchtc_util.h/.c
06/15/04   kwo     Moved srch_pc_eval_handoff to srch_idle_util
06/02/04   jcm     Convert to using srchi_t types header file
06/01/04   jcm     Added VALID_BAND_CHAN() Macro
05/28/04   jcm     Sector module integration
05/27/04   ejv     Mainline FEATURE_BROADCAST_SMS.
                   Renamed FEATURE_ENHANCED_TRIAGE as SRCH_ENHANCED_TRIAGE.
                   Enable SRCH_ENHANCED_TRIAGE.
05/14/04   kwo     Changed return type of srch_set_parm()
04/21/04   sfm     Changed for traffic channel manager
04/07/04   ejv     Removed unused variable from srch_parm_struct_type.
04/02/04   ejv     Removed srch_hho_in_progress and added
                   srch_tc_get_hho_status().
03/23/04   ejv     Support for parms per antenna, including 5 new types
                   and function srch_set_ant_status().
03/16/04   ejv     Additional FEATURE_TRANS_MANAGER checks.
03/04/04   ejv     Removed old/unused externs.  Added new functions
                   srch_tc_cfs_timers_init, srch_tc_get_off_time_fwd(),
                   srch_tc_get_cfs_state(), and srch_tc_get_cfs_state().
                   Removed srch speed from interface to srchtc_cfs_num_srch().
02/24/04   ddh     Cleaned up lint disable/enable
02/20/04   ejv     Added more FEATURE_SEARCH1 checks.
02/17/04   dd      Added srch_gps_handler() prototype from srchgps.h.
02/11/04   ejv     Mainline FEATURE_IS95B.
02/11/04   ejv     Changed FEATURE_IS2000C_DV to FEATURE_IS2000_REL_C_DV.
02/10/04   jcm     Removed first pass of lint errors
02/10/04   ejv     Added FEATURE_TRANS_MANAGER.
02/09/04   sfm     Added #define for FULL_ACQ_AGC_RSSI_THRESH
02/04/04   br      Added separate free queue for PPM search request buffers.
02/02/04   ejv     Removed unused and unnecessary variables.  Removed
                   macro UPDATE_SLOT_MASK and FEATURE_FAST_FINGER_SLEW.
01/30/04   kwo     Removed PC_HO_TIMER_DONE(), PC_THROT_TIMER_DONE()
01/28/04   ejv     Mainline T_MSM6500.
01/21/04   sfm     Added dv_pilot_info to srch_tmp_aset_struct_type
01/21/04   jcm     Removed reminents of the delay timer
01/19/04   ejv     Remove unused variables.
01/19/04   ejv     Remove unused TD-related drivers that used both 8x
                   and 16x searchers, since 16x does not exist.
01/16/04   ejv     Remove FEATURE_SEARCH2, FEATURE_SRCH_SCHEDULER, and
                   FEATURE_SRCH_SCHEDULER_MONITOR.
01/09/04   bt      Added srchpc_update_aset prototype.
01/09/04   gs      Added srch_last_good_win_siz declaration and prototype
                   for srch_sect_find_any_freq.
01/05/04   ejv     Removed unused srch_clr_timer, srch_set_timer,
                   srch_tick_timer and srch_tick_expired.
12/30/03   sfm     Changed extern for srchtc_cfs_visit_timer to be a rex timer
                   and added an extern for srchtc_period_timer
12/19/03   kwo     Cleaned up some ZZ1 style slot tracking vars
12/12/03   jcm     Retain energy of old neighbors when updating neighbor list
12/04/03   aar     Removed srch_rtc_roll_isr since it's currently unused.
11/19/03   jcm     Added FILT_N_GROW() and FILT_I4_GROW() Macros
11/13/03   ejv     Compile out srchtc_process_sch_transaction for 6700.
10/28/03   ddh     Moved NOTUSED macro here from stm.h
10/16/03   kwo     Removed FEATURE_ZZ2_OFREQ_TIMER
                   Added srch_pc_eval_handoff()
10/10/09   bt      Moved srch_comb_post_slam_tt_fing into srch_comb.c
10/07/03   kwo     Added FEATURE_ZZ2_OFREQ_TIMER
09/30/03   ejv     Remove obsolete feature FEATURE_GPS_DISABLE_CHIPX8_SLEEP.
                   Mainlined FEATURE_GPS_SRCH_ENGINE.
09/26/03   ejv     Remove old/unused macros, variables and functions.  Add
                   srch_wakeup_isr prototype from srch.h.
09/24/03   ejv     Extern srch_update_sch_walsh_len.
09/10/03   aaj     Added prototype for RSSI to dB conversion function
09/10/03   kwo     Modified srch_calc_total_eng() to take a thresh arg
08/11/03   va      Moved definition of PPM relevant structures to srch_hw.h
08/05/03   ejv     Remove FEATURE_FING_MERGE_DETECT.
07/29/03   ejv     Removed srchtri_delayed_fing_posn.
07/28/03   ejv     Mainline FEATURE_ZZ2.
07/24/03   ejv     Add srch_ch_rc_type from srch.c and extern srch_rc.
07/14/03   ejv     Remove FEATURE_SPECIAL_MDR.
07/07/03   ejv     Mainline FEATURE_ANTIREGISTRATION_BIAS.
                   Mainline FEATURE_ENHANCED_STANDBY_III.
                   Mainline FEATURE_TRIAGE_LOGGING.
07/02/03   ejv     Fixed featurization.
06/23/03   ejv     Remove FEATURE_SEARCH3.
06/19/03   ejv     Update interface to srch_rpt_ta().
06/15/03   rdh     Added srch4 dump processing signals.
06/11/03   rdh     Streamlined search governor.
05/29/03   kwo     Added srch_normalize_eng()
04/28/03   sst     Mainlined FEATURE_SD20 and SRCH_ENABLE_SD20
                   Added support for searcher's 1 & 4 for sys_meas
04/16/03   aar     Added RTC related macros/variables.
04/03/04   bt      Removal of QPCH PLT support.
03/24/03   ejv     Moved placement of SRCH_PCG_STATE in enum.
03/24/03   sst     Added defines for ChipX16 conversions
02/18/03   br      Added proper featurization.
02/18/03   va      SRCH4 AFLT support.
02/14/03   kwo     Added Finger RSSI to ecio table
02/11/03   gs      Move SRCH_ENABLE_SD20 definition into custsrch.h.
02/11/03   kwo     Mainlined call to srch_reset_pilot_info
02/05/03   gs/kwo  Add in Rel A support for ZZ2.
02/05/03   gs/br   Add GSC_TIMER_EXPIRED definition.
02/04/03   gs      Add srchint_slam_at_reacq flag.
02/03/03   gs      Merge with MSM_SRCH.08.06.60, specifically adding the
                   reacq parameter definitions shared between ZZ and PCG.
01/28/03   sst     Encapsulated srch_win_center to srch_hw.c/h
01/17/03   bt      Merge FEATURE_ZZ2 from SRCH_SLOTTED_DEV.00.02.00.
01/14/03   gs      Merge with MSM_SRCH.08.06.57
01/13/03   sst     Removed freq_incl from sect struct, All pilots now have
                   freq and band class info.
01/09/03   gs      Brought in consistency check for srchzz_wakeup_state
12/23/02   ejv     Changed DOG_RPT to SRCH_DOG_RPT.
12/03/02   pj      Partial merge with 5500 development (MSM_SRCH.08.06.52).
12/02/02   ejv     Additional EBCSMS changes.
11/26/02   bt      merge FEATURE_ZZ2 from SRCH_SLOTTED_DEV.00.01.03
11/25/02   ejv     Support for TD on NCC.
11/25/02   ejv     Mainlined !FEATURE_SRCH_PC_IFACE.
11/22/02   va      Added srchzz_chipx8_sleep_allowed_int variable
11/19/02   gs      Move SRCH_OOK_SUBSIG back to SRCH_OOK_SIG
11/05/02   gs      Move SRCH_OOK_SIG to SRCH_OOK_SUBSIG
10/31/02   bt      add micro processor dump macro
10/30/02   gs      Merge with MSM_SRCH.08.06.47
10/21/02   rm      Added search4 lost dump timer
10/09/02   va      Removed SRCH4_TRIG_DONE Macro
10/04/02   sst     Added support for 4 & 6 searcher peaks
10/02/02   gs      Merge with MSM_SRCH.08.06.42
07/30/02   gs      Initial merge into perforce for cougar.
06/24/02   aar     Added variable declarations for GPS fake DZ sleep.
06/03/02   aar     Replaced srch_get_srchzz_chipx8_sleep_allowed with srch_get_
                   srchzz_keep_chipx8_on.
05/31/02   ejv     Removed vc_osc_ctl.
05/31/02   ejv     Added lc_mask to srch_idle_info_type for PLT.
05/30/02   aar     Added declaration of variable srchzz_chipx8_sleep_allowed.
05/30/02   ejv     Mainlined T_MSM5000.
05/24/02   ejv     Added FEATURE_IS2000_REL_A_SVD.
05/21/02   ejv     Mainlined FEATURE_IS2000.
05/09/02   br      Moved some subsignal handling function prototypes to srch.h.
05/02/02   ejv     Removed FEATURE_QUARTER_ROLL_TIMELINE.
04/24/02   br      Removed srchzz_enable_and_init_rf_digital() prototype.
04/22/02   ejv     Added defines from srch.h.
04/17/02   ejv     Added section for doing searcher feature sanity checks.
03/21/02   br      Removed FEATURE_SRCH_BATCH_LOG. Added proper GPS and PPM
                   feature names. Rearanged gpsOne definitions.
03/16/02   br      Added support for the new GPS software architecture and
                   removed subsignal definitions.
03/05/02   ejv     Extern new flag srch_hsd_call_active.
03/01/02   aar     Changes related to new NV item: NV_GPS_DOPP_SDEV_I.
02/27/02   ejv     Featurization around "bcch_supported" is changed.
02/26/02   ejv     Removed duplicate definitions.
02/22/02   ejv     FEATURE_IS2000_REL_A_NCC -> FEATURE_IS2000_REL_A_CC.
02/21/02   ejv     Added FEATURE_SRCH_PC_IFACE and FEATURE_IS2000_REL_A_NCC.
02/20/02   va      Support for searcher 4 for MSM 6050 and for going to GPS
                   from ACQ.
02/11/02   rm      Fixes for loading the long code mask for QPCH (MSM6050).
                   Scale the qpch soft decisons and threshold values to
                   be backward compatible with 5100.
02/05/02   ejv     Removed srch_qpch_enabled and added srch_qpch_is_enabled().
01/30/02   ejv     Removed FEATURE_JSTD008_DEBUG.
01/18/02   sst     Moved Subsignal Definitions - from srch.h
01/16/02   ejv     Added T_MSM6000.
01/12/02   sst     Modified to use the new srch_* files that replaced srchdrv.c
12/02/01   br      Added SRCH_RESTART_SEARCH_SUBSIG for GPS MSTO. Rearanged
                   subsignal definitions. Added srch_cdma_search_active()
                   definition.
12/02/01   pj      Added SRCH_GPS_NEED_RX_TX_SUBSIG for GPS MSTO.
11/30/01   gs      Remmoved unneeded srchtc_next_sector prototype.
11/30/01   gs      Changed srchtc_next_sector prototype to no longer
                   depend on passed-in slew.
11/21/01   br      Changed status dump occurrence type and added new function
                   definition srch_collect_srch_sect_requests().
11/15/01   ejv     Changed FEATURE_TRIAGE_DEBUG to FEATURE_TRIAGE_LOGGING.
11/12/01   pj      Added GPS antenna offset #defines.
11/01/01   gs      Changed EXPECTED_MAX_OFF_TIME_FWD to 3 frames from 6 frames.
10/08/01   ejv     Added additional REL_A PLT support.
09/25/01   rm      Added a flag used to indicate disabling of finger merge
                   detection when going to sleep.
09/25/01   ejv     Added new function srch_agc_to_dbm().  Added common_path
                   structure for energy storage.  Added pilot_rec and
                   pilot_rec_type to fingers.  Added FEATURE_IS2000_REL_A_TD.
09/17/01   pj      Changed FEATURE organization of SUBSIG definitions to allow
                   non-GPS_MODE builds to compile.
09/11/01   lcc     1. Modified mc_trans in srch_parm_struct_type to contain
                     queues for transaction management instead just an array of
                     2.
                   2. Changed maximum number of pending transactions from
                      2 to 10.
09/07/01   aaj     Added QPCH ULPN mask changes for MSM5500
08/22/01   ejv     Added CDMA_CHIP_RATE and CDMA_CHIP_RATE_PER_MS.
07/10/01   rm      Removed declaration of srch_fing_reset_frame_cnt.
07/06/01   br      Added support for abort_srch_callback in SRCH scheduler.
                   Added srch_stop_current_ppm_srch() definition.
06/25/01   ejv     Renamed srch_fing_cfg_fch_qof() to srch_fing_set_qof().
                   Added function srch_fing_cfg_all_qof().
06/20/01   ejv     Removed srch_winsize_slew.
                   Mainlined FEATURE_SRCH_SLEW_IN_DRV.
                   Removed FEATURE_DIAG_V7_LOG_IFACE.
06/12/01   abh     Changed prototype for the srch_gps_record_fing_comb_offset
                   function. Added variables to handle the gps time transfer
                   problem.
06/05/01   ejv     Created FEATURE_FAST_FINGER_SLEW and replaced some T_MSM5000
                   checks with FEATURE_IS2000.
06/05/01   rdh     Changed the meaning of temperature estimation constants.
05/30/01   ejv     Removed #ifdef from around 95B handoff params.
05/28/01   pj      Added srch_srch3fw_ready_timer.
05/21/01   br      More SRCH Scheduler support.
05/18/01   br      Added support for subsignals. Cease sharing srch_gps_timer
                   between GPS and PPM modes. Increased SRCH3 lost status dump
                   timeout period. Changed search duration determination for
                   setting lost search dump timer. Corrected certain
                   featurizations.
05/17/01   rm      Added a function prototype for srch_set_finger_pair_merge().
05/03/01   ejv     Added macros ABS and LABS.
05/01/01   br      Added SRCH Scheduler support. Moved searcher slewing to
                   SRCH drivers. Added shoulder search mode support.
04/26/01   rdh     Changes for 8X searching in all states, code streamlining.
04/10/01   ejv     Externed new function srch_start_rand_buf_fill for FEATURE_DH.
03/29/01   ejv     Added FEATURE_IS2000_P2.
03/16/01   ejv     Removed ULPN mask changes for MSM5100.
03/14/01   ejv     Modified comment.
03/14/01   ejv     Removed FEATURE_IS2000_REL_A from finger structure.
03/12/01   dgy     Added a variable for FEATURE_IS2000_QPCH_CCI.
03/05/01   ejv     Removed #ifdef from around srch_rssi_timer.
03/02/01   br      Added initial support for gpsOne PPM on MSM5105
                   (FEATURE_MSM5105_PPM_SRCH_ENGINE).
02/28/01   br      Added FEATURE_SEARCH3 to featurize searcher 3 operation.
02/27/01   ejv     Added FEATURE_IS2000_REL_A.
02/27/01   ejv     Removed srch_reacq_mstr_posn_set.
02/16/01   dgy     Added FEATURE_IS2000_QPCH_CCI, the initial support for CCI.
02/14/01   ejv     Added changes for setting the ULPN mask on MSM5100.
02/14/01   rdh     Added initial support for System Determination 2.0.
02/07/01   dgy     Added a variable for PC framing type.
02/07/01   ejv     Removed temporary changes for MSM5100.
02/02/01   br      Added FEATURE_GPS_MODE to featurize GPS operation.
01/31/01   rm      Added FEATURE_HW_FING_MERGE_DETECT.
01/31/01   br      Renamed FEATURE_GPS_MODE into FEATURE_GPS_SRCH_ENGINE.
01/25/00   abh     Changes to read the GPS RF Loss calibration value from NV.
01/22/01   ejv     Added SEARCH1_DUMP and SEARCH2_DUMP macros.
01/22/01   ejv     Mainlined T_SLEEP.
01/18/01   ejv     Added MSM5100 support.
01/10/01   dgy     For FEATURE_UP_CLOCK_SWITCHING, used TCXO for the uP clock
                   during the catnaps between PI1 and PI2 and between PI2 and
                   the PC slot boundary in QPCH.
01/08/01   ejv     Added new sub-structures for channel parameters.
12/14/00   ejv     Added another parameter to srch_fing_cfg_qof().
12/12/00   ejv     Moved srch_restore_pch() for PLT only.
12/11/00   pj      Put srch_enable_moving_psd code inside FEATURE_GPSONE ifdefs,
                   rather than simply in FEATURE_IS95B* ifdefs.
12/07/00   bgc     Added srch_restore_pch().
11/22/00   dgy     Added support for catnaps between PI2 and PCH slot boundary.
11/21/00   bgc     Updated for PLT support.
11/06/00   ejv     Added new searcher driver interface.
11/03/00   br      Exchanged gpsOne function srch_master_fing_get_pos() with
                   srch_get_master_finger_data().
11/02/00   dgy     Added a variable and moved a definition to srchzz.c for
                   FEATURE_IS2000_QPCH.
10/23/00   ejv     Externed srch_total_sleep_dur.
09/19/00   abh     Added support for using the RF Delay calibration of GPS Measurements
                   according to the values stored in NV.
09/18/00   ejv     Externed new function srch_trans_qpch_to_pc().
09/14/00   day     Added srch_gps_record_fing_comb_offset() prototype.
09/12/00   day     Move srchzz_enable_and_init_rf_digital() prototype outside
                   of FEATURE_IS2000_QPCH.
09/12/00   pj/abh  Added master_fing_get_pos() prototype.
09/11/00   ajn/dgy Added the QPCH timeline that wakes up before the first PI
           /ejv    bit, goes back to sleep immediately if any of the two PI
                   bits is zero, and stays awake for PCH if both PI bits are
                   non-zero.
09/08/00   eh      Declared qpch channel estimator functions.
09/07/00   lc      Added function prototype for srchtc_process_sch_transaction().
09/06/00   eh      Declared srchzz_enable_and_init_rf_digital().
09/01/00   pj      Added new SRCH_GPS_SRCH_DONE macro.
08/21/00   ejv     Removed FEATURE_PILOT_SCANNER and SRCH_PRE_IS95B_BUILD.
08/06/00   abh/br  Added support for SRCH3 lost status dump and memset().
08/04/00   ejv     Externed srch_get_slot_status() and 2 new last slot qwords.
07/28/00   fc      Mainlined FEATURE_CS_ACTION_TIME_TRANS.
07/13/00   yll     Added "Tune Away Without Drop" capability for FEATURE_GPSONE.
06/21/00   abh     Added macro definition for GPS Srch Status Dump.
06/29/00   abh/br  Added MSM3300 support - FEATURE_GPSONE and FEATURE_SRCH_BATCH_LOG.
                   Changed local definitions from srchtc.c to global and externed
                   srchtc.c function variables to support new features.
06/19/00   ejv     Externed srch_qpch_log_data.
06/13/00   ejv     Removed srch_enable/disable_demod_bypass from slow clock
                   ifdef.  Edited memcpy prototype to match memory.h/string.h.
06/12/00   lc/dgy  Added FEATURE_CS_ACTION_TIME_TRANS to streamline the processing of
                   MC transactions and to support soft handoff transactions.
06/12/00   ejv     Externed srch_fpc_enabled and added ttl_slow to fing struct.
06/09/00    eh     Added support for search window offset.
06/06/00   ejv     Changed int8 to int1.
05/26/00   ejv     Doubled ROLLS_PER_MAX_SLOT_CYCLE.  Externed new slot mask
                   and slot counting variables.  Added new macros for slot
                   mask checking/clearing.  Externed some ZZ functions
                   which are need in PC state.
05/23/00    eh     Added srch_qpch_enabled.
05/15/00   rdh     Simplified and consolidated fing lock threshold variables.
05/11/00   rdh/wli Added improvements to power control symbol combiner handling.
04/24/00   dgy     Added full support for SCH, including handoff, Active set
                   updating, and action time processing.
04/10/00   dgy/ejv Added FEATURE_IS2000_QPCH.
04/10/00   ejv     Added CHIPX8_PER_OFFSET and PILOT_PN_MASK.
03/16/00   ejv     Added ROLLS_PER_MAX_SLOT_CYCLE.
02/25/00   ejv     Changed #ifdef FEATURE_DIAG_STREAMING_OUTPUT to
                   #ifndef FEATURE_DIAG_V7_LOG_IFACE.
02/24/00    eh     Declared srch_enable_demod_bypass and
                   srch_disable_demod_bypass.
02/14/00   rdh     Added FEATURE_FING_MERGE_DETECT.
02/11/00   ejv     Wrapped srch_bypass_interface in FEATURE_SLEEP_SLOW_CLOCK.
02/02/00   dgy     Added support for action time processing.
01/27/00   ejv     Added FEATURE_KEYPRESS_OPTIMIZATION.
01/19/00   ejv     Increased SRCH2_SW_Q_LEN to 42.
01/13/00   ejv     Externed srch_bypass_interface().
01/10/00   rdh     Removed window segmenting.
01/10/00   ajn     Re-Added FEATURE_ENHANCED_STANDBY_III & QUARTER_ROLL_TIMELINE
                   support.
12/21/99   rdh     Added FEATURE_ENHANCED_STANDBY_III.
12/13/99   ejv     Changes for FEATURE_SEARCH2.  Removed duplicate extern
                   of srch_freeze_iq() (it is also in srch.h now).
11/24/99   ajn     Finger "Change Flags" for Supplimental Channel support
                   TEMP_HACKS_FOR_TIGER removed
11/10/99   ejv     Added FEATURE_SEARCH2 and FEATURE_SEARCH2_IDLE.
11/08/99   dgy     Added function prototypes for srch_fing_cfg_qof() and
                   srch_reset_qof().
10/27/99   ajn     Added QPCH support
10/21/99   ejv     Changed srch_wait to use rex_sigs_type instead of words.
10/20/99   dgy     Added support for QOF.
10/07/99   lcc     Merged from PLT 2.0:
           ajn/rm  Added in support for SCH (for PLT).
10/06/99   ejv     Removed some parameters from the srch_msm_config_struct_
                   type.  Removed use_max_inx from srch_sort_sect input list.
10/05/99   wjn     Exported variable srch_ho_to_oband for sparse AGC shipment
                   to DM before hard handoff to a different band class.
10/04/99   ejv     Merged FEATURE_DIAG_STREAMING_OUTPUT.
09/30/99   yus     IS95B code review cleanup.
09/07/99   ejv     Added prototype for function srch_freeze_iq.
08/25/99   wjn     Export srchpc_rssi_rpt and srchpc_sys_reselect_check() for
                   Analog System Reselection algorithm to be run in ZZ state.
08/23/99   ejv     Mainlined FEATURE_ENHANCED_STANDBY_II.  Removed prototype
                   for srch_sect_end.
08/13/99   ajn     Fixed the INCW_MAX and INCI_MAX macros.
08/12/99   ejv     Lint cleanup.
08/12/99   rm      Removed functions declarations relating to enabling
                   I/Q Channels. New Macros have been defined in msm_drv.h.
08/12/99   yus     Made several modifications of the structures and command
                   names under FEATURE_IS95B_MAHHO
08/12/99   yus     Changed FEATURE_IS95B_CFS and FEATURE_IS95B_HARD_HO to
                   FEATURE_IS95B_MAHHO.
08/06/99   ejv     yshi: Added FEATURE_IS95B_CFS support.  ajn: Mainlined
                   FEATURE_ENHANCED_STANDBY and FEATURE_VIRTUAL_COMBINER.
                   Removed pre-MSM3000 support.  Added initial Tiger2 support.
07/24/99   jcw     Merged in PLT changes.
07/23/99   wjn     Added the band class information in macro
                   CURRENT_FREQ_NEIG(i) definition and in function prototypes
                   srch_sect_find() and srch_nset_dup().
07/20/99   yus     Added ota_win_n member in the sect structure. It's used for
                   saving the over-the-air window size.
07/20/99   yus     Defined WINDOW_SIZE_14. It's used for enable 8x mode if
                   neighbor window size larger than that (IS95B system only.)
07/19/99   ml      Added support for PLT.
07/01/99   yus     De-coupling FEATURE_SPECIAL_MDR with FEATURE_IS95B_PILOT_RPT
                   if SRCH_PRE_IS95B_BUILD is defined.
06/28/99   yus     Merged IS95B features to the ASW tip.
06/28/99   yus     Added #ifdef SRCH_PRE_IS95B_BUILD for pre-95b backward build.
06/24/99   dgy     Changed PCS AGC threshold to -100 dBm. Added a new define.
06/10/99   yus     Merged IS95B Hard Hardoff ( w/o Candidate Frequency Search.)
06/09/99   rdh     Merged FEATURE_PILOT_SCANNER.
06/08/99   wli     Removed slow clock code due to redesign.
05/26/99   ejv     Removed prototype for srch_labs, which no longer exists.
05/25/99   ejv     Added srch_fing_disable and srch_fing_restart.
05/24/99   yus     Added IS95B Periodic PSMM feature.
05/03/99   ejv     Added raw_rssi to the fings struct to store the energy
                   needed for reporting Gen TA packet data.
03/22/99   kmp     Merged the changes listed below from the TGP IS-95B baseline.
                   03/10/99   yus     Added access_attempted to
                                      srch_sect_struct_type.
04/22/99   ejv     Added the definition of WIN_TAB_SIZ from srch.c.
04/08/99   ejv     Removed redundant variable declarations.
02/26/99   yus     Merged IS95B features to ARM based code( see below for the
                   features.)
                   02/03/99   ks      Fixed the AEHO and AHO ifdef feature
                                      definitions.
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
04/06/99   yus     Changed the value of MAX_NUM_SUP_CH_SUPPORT_IF_FOUR_FINGS
                   to zero.
04/05/99   yus     Added function srch_4th_fing_enable for enabling/disabling
                   the fourth finger.
02/16/99   ejv     Added last_win_siz to srch_sect_struct_type.
02/03/99   ejv     yus: Merge updates for MDR features.
02/03/99   ejv     yus: Added IS95B MDR support.
02/03/99   ejv     Removed debug code, srch_sleep_roll_isr.
02/02/99   ejv     Changed T_VIRTUAL_COMBINER to FEATURE_VIRTUAL_COMBINER.
02/01/99   ejv     Added a number of defines and prototypes for rdh's
                   FEATURE_ENHANCED_STANDBY_II.
01/21/99   ejv     Edited FEATURE_DH - no longer uses DMA.
01/13/99   ejv     ajn: Added knowledge about time reference slamming to
                   srch_comb_arm_slam & added srch_get_vsc_time( )
01/06/99   ejv     Removed _cdecl and ROM - not necessary for ARM.
12/16/98   ejv     Merged code for SLOW_CLOCK.
10/16/98   ejv     Merged changes to support Panther.
10/08/98   ejv     Changed FEATURE_NEW_SLEEP_TIMELINE to
                   FEATURE_ENHANCED_STANDBY.
09/28/98   ajn     Avoid slew computation during finger slew problem.
09/28/98   ejv     Added _cdecl to force C calling to specific functions
                   for Pascal builds.
09/25/98   ejv     Changed prototype of srch_get_freq_est to handle an int2.
09/24/98   ejv     Added const to 2nd parameter in memcpy() to silence
                   compile warnings and match memory.h declaration.
08/24/98   ejv     Removed pilot scanner variables.
08/18/98   ejv     Removed srch_msm versions since hw.h versions are now used.
08/18/98   ejv     Added dh task support, must define FEATURE_DH.
08/18/98   ejv     Added rdh/bgc's FEATURE_DYNAMIC_THROTTLE to the governor.
07/01/98   ajn     Added FEATURE_NEW_SLEEP_TIMELINE macro.
05/13/98   ajn     Used a delayed timeline for MSM2.3 in paging/idle sleep
                   mode.
04/21/98   mk      Added support for msm2310.
04/13/98   dgy     Include customer.h only if CUST_H is defined. Changed
                   FING_CNT and added general TA packet and function to set
                   power combiner threshold for MSM2300.
03/16/98   scdb    Merved CLOCK 2.0 changes. This removes the SRCH_TIMEKEEP_10
                   define.
03/04/98   yus/smo Added new ML eng_to_ecio tables for use in 8x searcher.
02/20/98   smo     Removed new clock support code via ifdef SRCH_TIMEKEEP_10.
                   This represents use of timekeeping VU clock01.00.00.
11/25/97   dgy/smo Changes to use new eng_to_ecio table pointer.
11/17/97   dgy/smo Support for enhanced acquisition algorithm, use of recent
                   temp array.
10/28/97   smo     Added support for msm2300, 8x searcher.
10/28/97   smo     Added support for msm2300, A4.
10/01/97   smo     Added msm2300 support, new srch_msm_version structure.
06/27/97   rdh     Added regional defines for Pilot Scanner.
06/19/97   rdh     Added T_Q, removed pre-NGP support, updated copyright.
05/20/97   rdh     Added Low Power Mode support.
03/21/97   smo     Added customer.h include for FEATUER_JSTD008
03/14/97   jjn     Added (TG==T_MD) to the appropriate conditional compile
                   statements for the Module
01/22/97   rdh     Added srch_get_stop_slew pulled from ZZ state.
12/12/96   smo     Increased SRCH_GOVERN_TIMEOUT to 16ms. Added constant
                   SRCH_GOVERN_ADJ_TIME for governer adjust periods.
11/22/96   rdh     Added Hutch's T_T target definitions.
11/05/96   smo     Increased CDMA_RSSI timeout period to 100ms.
10/22/96   rdh     More ROM savings to make 800 fit 4M flash.
09/26/96   rdh     Improved SC/PC thrash protection with dual-mode sys det.
09/19/96   rdh     Added unconditional PC state idle handoff throttling.
08/15/96   rdh     Added idle handoff delay timers/sigs.
07/11/96   smo     Added more debug information for priority scanning.
06/27/96   rdh     Changed AGC thresh to -90dBm on Systems recommendation.
05/20/96   smo     Added debug variables for JSTD-008 priority scanning
05/20/96   rdh     Added prototypes for sleep-related system time inc functions.
05/09/96   smo     Added JSTD-008 features and definitions.
05/07/96   rdh     Added T_COMP state machine vars in trpt structure.
05/01/96   rdh     Added sleep holdoff time, orig_pend flag for rescan-send.
04/09/96   rdh     Added "fast-revisit" struct members, pathlog proto. consts.
03/06/96   smo     Made RSSI timeout value 100ms for ISS2.
02/29/96   rdh     Added poke capability for thresholds after SRCH_ASET_F.
02/28/96   rdh     Got rid of extra searching Nset, changed over to tick26.
02/01/96   rdh     Added srch_expected_win_pos and srch_sect_stop prototype.
11/30/95   rdh     Added fade report support and internal Nset-keep feature.
11/03/95   rdh     Reduced Micro-acq AGC threshold to -100 from -104 dBm.
09/07/95   rdh     Added FILT_I4 for ZZ slow-open SCI fixes.
09/04/95   rdh     More pre-review cleanup of speed throttling constants.
08/23/95   rdh     Pre- code review cleanup checkin to save RAM/ROM.
07/21/95   rdh     ROM'd some large tables to save RAM space.
06/28/95   rdh     Changes to support fast mini/micro acq on Deep Sleep wakeup.
06/01/95   rdh     Mods for search driver break-up, CSS.
05/10/95   rdh     Added NGP-related interrupt handler declarations.
05/09/95   rdh     Added search speed throttling to allow all window sizes.
05/03/95   rdh     Added agc/pctl enabled flag field.
04/18/95   ras     Added T_I2 for ISS2 operation.
04/14/95   rdh     Added CDMA RSSI processing for all targets.
04/13/95   rdh     Gemini change: Use new sparse AGC/pctl logging packet.
04/06/95   rdh     Made lost dump recovery more responsive (faster).
03/31/95   rdh     Eliminated win_z variability as ZZ now spirals on reacq.
03/30/95   rdh     Merged in lost dump recovery timer code.
03/30/95   rdh     Elim slam bug catch - separated sym siz change from slam arm.
03/23/95   rdh     Added new srch_dz_state definitions for deep sleep.
01/10/95   rdh     Bug fix for PhsIII release - got rid of old "nset_received"
01/09/95   rdh     IS-95 Phase III release:
                    - Added segmented search max calc var.
01/06/95   rdh     IS-95 Phase III upgrades:
                    - Mods for .pos, time reference rework.
                    - Added find_earliest_path, mods to find_earliest_fing.
                    - Made state and status code types consistent with calls.
                    - Added REX timer for handoff/slew throttling.
                    - PC state idle structure streamlining.
10/31/94   rdh     Added PC_ASSIGN needed parameters to idle structure.
10/24/94   rdh     Added PN mask autoswitching capability and slew state
                   MSM hardware error detection declarations.
09/06/94   rdh     Added finger struct referencing var and no path counter
                   in sect struct for individual window pilot centering.
08/04/94   rdh     Removal of annoying tabs/spaces.
07/30/94   rdh     Added IS-95 phase III upgrades.
04/05/94   jah     Added prototype for srch_find_earliest_finger().
03/01/94   jah     Changed cur_state's to srch_state_code_type.
01/06/94   jah     Changed watchdog iface to use dog_report().
12/09/93   jah     Added finger tracing externs under SRCH_FING_DEBUG define.
11/19/93   jah     Added extern for srch_rudely_awakened.
08/05/93   jah     Added prototype for srch_set_sym_siz()
07/26/93   ptw     Added SET_* definitions.
06/18/93   ptw     Added cell_type struct.
06/08/93   ptw     Added support for pwr_comb_ind bit (pci)
04/30/93   ptw     Added sleep stuff.
04/28/93   jai     Added routines to store and retrieve frequency estimates and
                   added freqeuncy estimate callback routine.
10/13/92   ptw     Added srch_cdma_freq variable.
10/12/92   ptw     Moved some regional declarations from srch.h
04/30/92   ptw     Removed FATAL macro (replaced by ERR_FATAL in msg.h)
01/22/92   ptw     Carved this module out of monolithic SRCH.C module.

===========================================================================*/

#include "customer.h"
/* Verify internal feature dependencies, must be included after customer.h */
#include "srch_dep.h"

#include "dog.h"
#include "log.h"
#include "mccsrch.h"
#include "srch.h"
#include "srchi_t.h"
#include "target.h"
#include "cai.h"
#include "sleep.h"

#ifdef FEATURE_PLT
#error code not present
#endif /* FEATURE_PLT */

#ifdef FEATURE_IS2000_QPCH
#include "mccqpch.h"
#endif /* FEATURE_IS2000_QPCH */

#include "mctrans.h"
#include "srch_hw_t.h"
#include "srch_rx_t.h"
#include "hw.h"
#include "srch_conv.h"

#include "rfm.h"
#include "rflib.h"
#include "timer.h"


/*===========================================================================

               SEARCHER FEATURE COMBINATIONS SANITY CHECK

===========================================================================*/

#ifdef FEATURE_IS2000_REL_A_CC_BCSMS
#ifndef FEATURE_IS2000_REL_A_CC
#error FEATURE_IS2000_REL_A_CC_BCSMS requires FEATURE_IS2000_REL_A_CC
#endif /* !FEATURE_IS2000_REL_A_CC */
#endif /* FEATURE_IS2000_REL_A_CC_BCSMS */

#ifndef FEATURE_SRCH_INACTIVE_STATE
#error  FEATURE_SRCH_INACTIVE_STATE required!  Unslew no longer supported!
#endif /* FEATURE_SRCH_INACTIVE_STATE */


/*===========================================================================

                      REGIONAL DATA DECLARATIONS

===========================================================================*/

/*--------------------------------------------------------------------------
                         Search CDMA System Types
--------------------------------------------------------------------------*/

/* Use this type to pass the system type to driver function in
   order to determine how to proceed.  Initial implementation
   uses this to set combiner channels, which is a different
   configuration for IS95B vs. IS2000 */
typedef enum
{
  SRCH_IS95A,
  SRCH_IS95B,
  SRCH_IS2000
}
srch_is2000_system_enum_type;

/*--------------------------------------------------------------------------
                           Search Task States
--------------------------------------------------------------------------*/

/* DO NOT modify the order of this enum, as it will affect the
   post-processing tools - add new states at the end, before
   SRCH_MAX_STATE.  Also check srch.c to update srch_state_str if
   necessary */
typedef enum
{
  SRCH_INACTIVE_STATE = 1,  /* SRCH is not active */
  SRCH_CDMA_STATE     = 2,  /* Initial state for CDMA operation */
  SRCH_ACQ_STATE      = 3,  /* CDMA acquisition of the pilot channel */
  SRCH_SC_STATE       = 4,  /* CDMA reception of the sync channel */
  SRCH_SLEW_STATE     = 5,  /* CDMA transition from sync to paging channel */
  SRCH_PC_STATE       = 6,  /* CDMA operation on the paging channel */
  SRCH_ZZ_STATE       = 7,  /* CDMA slotted mode sleep state */
  SRCH_TC_STATE       = 8,  /* CDMA operation on the traffic channel */
  SRCH_UNSLEW_STATE   = 9,  /* Obsolete! Don't use */
  SRCH_PCG_STATE      = 10, /* Obsolete! Don't use */
  SRCH_POWERUP_STATE  = 11, /* Initial state on power up */
  SRCH_TT_STATE       = 12, /* HDR Time Transfering */
  SRCH_MAX_STATE        /* Must remain last item in enum. */
}
srch_state_enum_type;

#define IS_IDLE_STATE(state)             (((state) == SRCH_PC_STATE) || \
                                          ((state) == SRCH_ZZ_STATE))

#define IS_NON_SLOTTED_IDLE_STATE(state)  ((state) == SRCH_PC_STATE)

#define IS_SLOTTED_IDLE_STATE(state)      ((state) == SRCH_ZZ_STATE)

#define IS_TRAFFIC_STATE(state)           ((state) == SRCH_TC_STATE)

#define IS_ONLINE_STATE(state)           (((state) == SRCH_TC_STATE) || \
                                          ((state) == SRCH_PC_STATE))

/*--------------------------------------------------------------------------
              path types for srch_path_struct_type
--------------------------------------------------------------------------*/
typedef enum
{
  SRCH_NORMAL_PATH,          /* normal everyday path */
  SRCH_VIRTUAL_PATH,         /* created path */
  SRCH_MANIPULATED_PATH      /* path with manipulated energy */
}
srch_path_type;

/*--------------------------------------------------------------------------
                         Constant Definitions
--------------------------------------------------------------------------*/

#define  ASET_WIN_SIZ       srch_parms.aset_win_siz
#define  NSET_WIN_SIZ       srch_parms.nset_win_siz
#define  RSET_WIN_SIZ       srch_parms.rset_win_siz
#define  CFNSET_WIN_SIZ     srch_parms.cf_srch_win_n
  /* Over-air window sizes. Units are chipx8. */

#define  WIN_TAB_SIZ     16
  /* The maximim length of the window size mapping table */

#define  CHIPX8_PER_CHIP    8
  /* The number of 1/8 chip "chipx8" units per chip. Used in slew math. */

#define  CHIPX8_PER_CHIP_SHIFT  3
  /* The number of 1/8 chip "chipx8" units per chip. Used in slew math. */

#define  CHIPX8_PER_HALF_CHIP    4
  /* The number of 1/8 chip "chipx8" units per half-chip. Used in slew math. */

#define  CHIPX16_PER_CHIP         16
  /* The number of 1/16 chip "chipx16" units per chip. Used in slew math */

#define  CHIPX16_PER_HALF_CHIP     8
  /* The number of 1/16 chip "chipx16" units per half-chip. Used in slew math */

#define  CHIPX16_PER_EIGHTH_CHIP   2
  /* The number of 1/16 chip "chipx16" units per Cx8. Used in slew math */

#define  CDMA_CHIP_RATE          1228800L
  /* Chip rate for regular CDMA, ie. IS95A/B, CDMA2000 1X, etc. */

#define  CDMA_CHIP_RATE_PER_MS   (CDMA_CHIP_RATE / 1000)
  /* Chip rate per millisecond */

#define  MS_PER_SEC         1000
  /* The number of milliseconds per second. */

#define  MS_PER_SLOT        80
  /* The number of milliseconds per 80ms superframe. */

#define  NO_PWRINX          ((word)~0)
  /* Indicates that no power control symbol combiner is assigned */

#define  NO_FINGER          ((word)~0)
  /* Value used in a finger index to indicate none of the fingers. */

#define  NO_PATH            ((word)~0)
  /* Value used in a path index to indicate none of the paths. */

#define  NO_SECTOR          ((word)~0)
  /* Value used in a sector index to indicate none of the sectors. */

#define  NO_PILOT           ((word)~0)
  /* Value used in a pilot offset to indicate no pilot offset */

#define  PILOT_SELF         (-1)
  /* Value used in a pilot offset to indicate your own pilot offset */

#define  NEVER_LOCKED       ((int4) 0x7FFFFFFF)
  /* Value used in a finger position to indicate never having been in lock */

#define  ROLLS_PER_MAX_SLOT_CYCLE   12288
  /* Number of 26.666...ms PN rolls in the max slot cycle.  This
     has actually been doubled from 6144, because BC SMS slots are
     longer than 6144. */

#define  MAX_PILOT_OFFSET   511
  /* Highest valid value for a Pilot PN Offset, per IS-95 */

#define  PILOT_PN_MASK      511
  /* Mask for normalizing the PN value */

#define  CHIPS_PER_OFFSET   64
  /* The number of chips between the center of each pilot offset */

#define  CHIPX8_PER_OFFSET  (CHIPS_PER_OFFSET * CHIPX8_PER_CHIP)
  /* The number of chipx8 between the center of each pilot offset */

#define  CHIPS_PER_ROLL     32768
  /* The number of chips for a PN roll */

#define  CHIPX8_PER_ROLL   (CHIPS_PER_ROLL * CHIPX8_PER_CHIP)
  /* The number of chipx8 for a PN roll */

#define  CHIPX8_ROLL_MASK   0x3FFFF
  /* Mask used for and-ing instead of mod */

#define  ECIO_TAB_SIZ       64
  /* Size of translation tables from IS-95 Ec/Io to anything.  This
     is just the number of possible Ec/Io values */

#define  SRCH_RPT_TIME      340
  /* Watchdog report interval, in millisecs. */

#define  SRCH_RSSI_TIMEOUT  100
  /* CDMA RSSI periodic timer timeout, in millisecs. */

#define  SRCH_DUMP_TIMEOUT  150
  /* Status dump timeout for lost dump recovery, in millisecs. */

#define  FREQ_TIMER_MS     10000L
  /* Frequency estimation periodic timer timeout value,  in milliseconds */

#define  GET_TEMP_EST   ((word)~0)
  /* Value indicating that there is no temp estimate available
     and we need to get one */

extern qword srch_no_slot;
  /* Used to for initializing and comparing with other qwords */

#define SRCH_WIN_OFFSET_TAB_MAX   7
  /* The search window offset multiplier table size */

/*--------------------------------------------------------------------------
                          Trk_lo setting declarations
--------------------------------------------------------------------------*/

#define SRCH_USE_PREV_TRK_LO  (~0)
  /* variable to pass in to use the last stored trk lo accum value */


/*--------------------------------------------------------------------------
                          IS95B Constants
----------------------------------------------------------------------------*/

#define SRCH_MAX_EC_THRESH  115
  /* Maximum EC threshold for system re-slection */

#define SRCH_CAI_N12M   10

#define  SRCH_PSD_TIMEOUT 20
  /* CDMA power spectral density periodic timer timeout,
    in millisecs. */

#define MIN_PPSMM_TIME 80
  /* Min periodic PSMM time allowed. */

#define NO_MIN_PILOT_PWR_THRESH_CHK       0x1F
  /* See IS95B 6.6.6.2.12 */

#define NO_MIN_PILOT_EC_IO_THRESH_CHK     0x1F
  /* See IS95B 6.6.6.2.12 */

#define NO_SF_TOTAL_EC_THRESH_CHK         0x1F
  /* See IS95B 6.6.6.2.8.3.2 */

#define NO_SF_TOTAL_EC_IO_THRESH_CHK      0x1F
  /* See IS95B 6.6.6.2.8.3.2 */

#define FRAME_LEN 20
  /* Frame length in ms */

#define FRESHNESS_INTERVAL  (40*FRAME_LEN)
  /* Freshness interval in ms (40 frames) */

#define EXPECTED_MAX_OFF_TIME_FWD  (3*FRAME_LEN)
  /* The expected max forward ch. off time due to one cfs visit.
     It's in ms unit (3 frames)  */

#define EXPECTED_ERASE_RATE 15
  /* The expected erase rate due to CF search, in percentage */

#define CHIP_PERIOD 814
  /* IS95 chip period in 1/1000 us */

#define CFS_SW_LATENCY 5
  /* The estimated software latency of per one CFS visit (ms) */

/*--------------------------------------------------------------------------
                          PCS Constants
----------------------------------------------------------------------------*/

#define  SRCH_NUM_PRI_LEVELS  4
  /* Number of priority levels for neighbor set pilots */

#define  SRCH_PRI_LOW   0
#define  SRCH_PRI_MED   1
#define  SRCH_PRI_HIGH  2
#define  SRCH_PRI_VHIGH 3
  /* Neighbor search priorities */

#define  FREQ_INCL_NO_FREQ      FALSE
#define  FREQ_INCL_FREQ         TRUE
  /* Frequency include flag */

/*--------------------------------------------------------------------------
                          Phase80 Declarations
--------------------------------------------------------------------------*/

#define PHASE_80M 0xC000u
  /* Mask used to determine phase of 80 ms superframe from time reference */

#define PHASE_SUB_80M 0x3fffu
  /* Mask to get range within a 26.667ms sub period of the 80ms period. */

#define PHASE0 0x0000u
  /* Value for masked reference time when in 1st 26.66 ms period of 80 ms */

#define PHASE1 0x4000u
  /* Value for masked reference time when in 2nd 26.66 ms period of 80 ms */

#define PHASE2 0x8000u
  /* Value for masked reference time when in 3rd 26.66 ms period of 80 ms */

#define PHASE_80_SHIFT 14
  /* Value to shift PHASE_80M masked value to get to range 0 .. 2 */

#define FCH_BETA_SCALE 0x20
  /* Value that the SW_BETA_FCH register should be set to */

#define COMBINER_CODE_CH_0 0x01
#define COMBINER_CODE_CH_1 0x02
#define COMBINER_CODE_CH_2 0x04
#define COMBINER_CODE_CH_3 0x08
#define COMBINER_CODE_CH_4 0x10
#define COMBINER_CODE_CH_5 0x20
#define COMBINER_CODE_CH_6 0x40
#define COMBINER_CODE_CH_7 0x80
  /* Bit masks used to access the code channels */


#ifdef FEATURE_PLT
#error code not present
#endif /* FEATURE_PLT */

/*===========================================================================
            Background/Dedicated System Measurement feature
===========================================================================*/
#define FULL_CIRCLE          -1
  /* Special window index which implies the full PN space
     is desired in the search */


/*--------------------------------------------------------------------------
          IS-95B POWER SPECTRAL DENSITY MOVING AVERAGE STRUCTURE TYPE

This structure is used for IS-95B Serving Frequency moving average of rssi.
--------------------------------------------------------------------------*/
typedef struct
{
  boolean enabled;
    /* Set to enable the moving average calculation */

  boolean full;
    /* Set to indicate the circle buffer of the moveing averge is full */

  byte index;
    /* Index of a array to store the most recent received rssi value */

  int2 psd[ SRCH_CAI_N12M ];
   /* A array to store the member of the moving average */
}
srch_moving_psd_type;

/*--------------------------------------------------------------------------
                 SRCH INTERNAL TIMER STRUCTURE TYPE

This structure is used for SRCH internal timer.
--------------------------------------------------------------------------*/
typedef struct
{
  boolean enabled;
    /* Set to enabled the timer */

  word ticks;
    /* The timer ticks  */
}
srch_timer_type;


/*--------------------------------------------------------------------------
               Declaration of Search Parameters Structure
--------------------------------------------------------------------------*/

typedef struct
{
  byte     frssi_scale;
    /* Scale multiplier for MSM23 */

  byte     frssi_low_mask;
    /* Mask out lower byte of frssi */
}
srch_msm_config_struct_type;

typedef struct
{
  int2  pilot_inc;
    /* How far to jump ahead in pilot offset when searching the remaining
       set */

  int2  aset_win_siz;
  int2  nset_win_siz;
  int2  rset_win_siz;
    /* Sweep window sizes for the three sets in 1/8 chip units.
       These are int4 since they are frequently used in slew
       computations. */

  byte  win_a;
  byte  win_n;
  byte  win_r;
    /* Sweep window sizes for the three sets in CAI index units. */

  uint32  t_add;
    /* Ec/Io required for promotion to the candidate set */

  uint32  t_drop;
    /* Ec/Io required for demotion from the candidate set */

  dword  t_add_eng;
  dword  t_drop_eng;
    /* The above two Ec/Io values converted to energies */

  dword  t_nset_eng;
    /* Neighbor threshold for promotion to the Pre-Candidate Set,
       corresponding to the t_add value above */

  dword  pc_thresh_eng;
    /* Threshold for the Pre-Candidate Set state machine */

  byte  t_comp;
    /* Energy ratio by which a rising candidate must exceed an
       active set pilot to trigger a pilot strength measurement
       message.  In 0.5 dB units. */

  word  t_tdrop;
    /* If a pilot's strength stays below t_drop for this duration,
       its dropout timer expires. In units of 80ms. */

  byte  nghbr_max_age;
    /* The number of Neighbor List Update messages that can pass
       without mentioning a pilot before the pilot is dropped from
       the neighbor set. */

  int2 soft_slope;
    /* Threshold slope value */

  int2 add_intercept;
    /* T_ADD threshold intercept value, in 0.5dB units */

  int2 drop_intercept;
    /* T_DROP threshold intercept value, in 0.5dB units */

  boolean  sys_reselect_enabled;
    /* Flag tells if the re-selection thresholds included */

  int32 ec_thresh;
    /* Pilot power threshold */

  word  ec_io_thresh;
    /* Pilot Ec/Io threshold */

  int1  diff_rx_pwr_thresh;
    /* Mini. difference in received power */

  byte  min_tot_pilot_ecio;
    /* Minimum total pilot Ec/Io */

  dword  min_tot_pilot_eng;
    /* Minimum total pilot Ec/Io in linear unit */

  byte  cfs_seq;
    /* The sequence number for the current search period */

  boolean  cf_periodic_srch;
    /* TRUE: CF Periodic search, FALSE, single search. */

  srch_rx_band_type  cf_band_class;
    /* Band class of the CF */

  srch_rx_chan_type  cf_cdma_freq;
    /* CDMA frequency for the CF */

  byte  cf_t_add;
    /* Pilot detection threshold for CDMA on the CF */

  byte  cf_pilot_inc;
    /* pilot PN offset for the CF */

  int2  cf_srch_win_n;
    /* Default search window size for the CF Search Set */

  int2  cf_srch_win_r;
    /* Search window size for the Remaining Set on the CF */

  byte  cf_nghbr_srch_mode;
    /* Search mode for the CF */

  word  cf_max_win;
    /* Maximum window size for CF */

  boolean hho_ind;
    /* Indicates if this is a HHO */

  boolean rif_ind;
    /* Indicates if we need to return to SF on failure */

  boolean srch_incl;
    /* Indicates if search parameters are included in this command */

  boolean lcm_change_ind;
    /* Indicates whether to restore long code mask after RIF-HHO */

  qword lcm;
    /* Long code mask */

  #ifdef FEATURE_IS2000_REL_C_DV
#error code not present
  #endif /* FEATURE_IS2000_REL_C_DV */

  cai_fpc_pri_chan_type fpc_pri_chan;
    /* Which channel has power control */
}
srch_parm_struct_type;


/*--------------------------------------------------------------------------
                            Cell Structure

   For the purposes of the Searcher task, a Cell is a collection of
   one or more Sectors, all of which carry the same power control
   subchannel.  This property means that finger assignment should
   favor cell diversity, and that the power control symbol combiners
   should be programmed to soft-combine power control symbols from all
   the sectors of a cell.

   Membership in a Cell is maintained in the Sector structure.  The
   Cell structure contains only information common to all sectors
   of the Cell.

   The cell table lists the current cells in the Active Set, in the
   order they were received in the last Handoff Direction Message.
   This means that cell numbering can change whenever a Handoff
   Direction Message is processed.

--------------------------------------------------------------------------*/

typedef struct
{
  int2  fing_cnt;
    /* The number of fingers actively assigned in this sector. */

  word  pwr_inx;
    /* The index (0,1,2, or NO_PWRINX) of the power control symbol combiner
       assigned to this cell.  Can also be 3,4, or 5 for MSM6700. */

  dword  sum_power;
    /* Summation of correlated energy contained within this cell */

  int2  pctl_fing_cnt;
    /* The number of fingers actively assigned in this sector for power control
       symbol combiner assignment purposes */
}
srch_cell_struct_type;


/*--------------------------------------------------------------------------
               Declarations of Structures for DIAG reports
--------------------------------------------------------------------------*/

/*------------------------------------------------------------------------*/

typedef struct
{
  log_s_agc_pctl_type log;

  int2  inx;
    /* Index into the AGC log buffer */

  word  drops;
    /* Count of how many times we've dropped the AGC logging operation
       for lack of a buffer */

  boolean  enabled;
    /* Tells whether or not logging is enabled */
}
srch_agc_pctl_vars_struct_type;

typedef struct
{
  deint_rc_type fch;
    /* Fundamental channel RC value */

  deint_rc_type sch0;
    /* Supplemental channel 0 RC value */

  deint_rc_type sch1;
    /* Supplemental channel 1 RC value */

  deint_rc_type dcch;
    /* Dedicated Control channel RC value */
} srch_ch_rc_type;

#define SRCH_COM_TD_WALSH  0x10
  /* The walsh code used for the TD path of a common pilot */

#define SRCH_COM_WALSH     0x00
  /* The walsh code used for the common path of a common pilot */

/*--------------------------------------------------------------------
                     Declaration for FAT PATH finger assignment
--------------------------------------------------------------------*/

#define SRCH_ENHANCED_TRIAGE
  /* Enable the enhanced triage algorithm */

#ifdef SRCH_ENHANCED_TRIAGE
extern boolean srch_disable_fn_merge_detect;
  /* Used to indicate disabling of finger merge detection when
     going to sleep */

#endif /* SRCH_ENHANCED_TRIAGE */


/*--------------------------------------------------------------------------
                         Function-like Macros
--------------------------------------------------------------------------*/

/* Absolute value */
#define  ABS( x ) ( ((x) < 0) ? -(x) : (x) )
#define LABS( x ) ( ((x) < 0) ? -(x) : (x) )
  /* Keep separate, just to realize the difference in types where used */

/* Macro to determine whether the passed in band and channel are
   legal and supported. */
#define VALID_BAND_CHAN( path, band, chan )  \
       rflib_is_band_chan_supported( (path),(band),(chan) )

/* Kick the watchdog */
#define  SRCH_DOG_RPT() \
  dog_report(DOG_SRCH_RPT); \
  (void) rex_clr_sigs( &srch_tcb, SRCH_RPT_TIMER_SIG ); \
  timer_set ( &srch_rpt_timer, SRCH_RPT_TIME, 0, T_MSEC )

/* Clear the OFFLINE signal and return whether it was set */
#define  OFFLINE_DONE() \
  ( rex_clr_sigs ( &srch_tcb, SRCH_OFFLINE_SIG ) & SRCH_OFFLINE_SIG )

#ifdef FEATURE_IS2000_QPCH
/* Sign extend the OOK measurements */
#define OOK_SIGN_EXTEND( x ) \
 x = ((x + 8) >> 4); \
 if ( x >  1023 ) { x =  1023; } \
 if ( x < -1023 ) { x = -1023; }
#endif /* FEATURE_IS2000_QPCH */

/* Decide if the handoff drop timer (T_TDROP) for a given pilot has expired.
   It uses the current value of T_TDROP, which is required by IS-95
   section 6.6.6.2.3. Arguments: "trpt" = The trpt struct for that sector,
   which contains the relevant dropout timer in 26.66..ms units. "time" = The
   current time in 26.66..ms units */
#define DROP_TIMER_EXPIRED(trpt, t_tdrop, time) \
  (trpt.drop_timer + t_tdrop <= time)

/* Maximum filter length used for active set filtering while in ZZ state */
#define ASET_MAX_FILT_LEN 16

/* Maximum filter length used for neighbor set filtering while in ZZ state */
#define NSET_MAX_FILT_LEN ASET_MAX_FILT_LEN

/* Increment an unsigned word, saturating at FFFF */
#define  INCW_MAX( v ) ((v) = ((v) < 65535) ? (v)+1 : (v))

/* Increment a signed int, saturating at 7FFF */
#define  INCI_MAX( i ) ((i) = ((i) < 32767) ? (i)+1 : (i))

#define  SIGNX( val )                                               \
  /*lint -save -e740  Yes, this is an unusual pointer cast */       \
  W_PTR ( val )[ 1 ] = srch_sign_words[ W_PTR ( val )[ 1 ] & 0x3 ]; \
  /*lint -restore */

#ifdef SRCH_VERBOSE
  #define SRCH_DEBUG_MSG(str,arg0,arg1,arg2) MSG_LOW(str,arg0,arg1,arg2)
#else
  #define SRCH_DEBUG_MSG(str,arg0,arg1,arg2)
#endif

/* macro to prevent compiler warnings when the payload parameter is not used
   by state entry, exit, and transition functions, or when a variable is only
   used in MSG_XXXX calls that may be compiled out */
#define NOTUSED(i) if(i){}

/* This Macro should be used to null check interfaces.  If a Void return
   value is desired, then pass in the RETURN_VOID macro */
#ifdef FEATURE_SRCH_MSG_LITE
#define SRCH_NULL_PTR_CHK(ptr, retval)                                   \
  if (ptr == NULL)                                                       \
  {                                                                      \
    return retval;                                                       \
  }
#else /* !FEATURE_SRCH_MSG_LITE */
#define SRCH_NULL_PTR_CHK(ptr, retval)                                   \
  if (ptr == NULL)                                                       \
  {                                                                      \
    MSG_ERROR("Unexpected Null Ptr", 0, 0, 0); { return retval; }        \
  }
#endif /* FEATURE_SRCH_MSG_LITE */

/* This function is for use within a state for NULL payload checking.  
   Returns STM_SAME_STATE if payload == NULL. */
#ifdef FEATURE_SRCH_MSG_LITE
#define SRCH_NULL_PAYLOAD_CHECK( payload )                                   \
  if ( (payload) == NULL )                                                   \
  {                                                                          \
    return STM_SAME_STATE;                                                   \
  }
#else /* !FEATURE_SRCH_MSG_LITE */
#define SRCH_NULL_PAYLOAD_CHECK( payload )                                   \
  if ( (payload) == NULL )                                                   \
  {                                                                          \
    MSG_ERROR("Null payload not expected in transition function", 0, 0, 0 ); \
    return STM_SAME_STATE;                                                   \
  }
#endif /* FEATURE_SRCH_MSG_LITE */


#define RETURN_VOID /* null macro */

/* The following defines create sections where other code is not allowed to
   run. Intlock and tasklocks are called because calls withing an intlocked
   section might cause a task switch which could would likely enable
   interrupts */
#define SRCH_ENTER_CRIT_SECT() \
        {                      \
          INTLOCK();           \
          TASKLOCK();          \
        }

#define SRCH_LEAVE_CRIT_SECT() \
        {                      \
          TASKFREE();          \
          INTFREE();           \
        }

#define SRCH_ENTER_CRIT_SECT_SAV(sav) \
        {                             \
          INTLOCK_SAV(sav);           \
          TASKLOCK_SAV();             \
        }

#define SRCH_LEAVE_CRIT_SECT_SAV(sav) \
        {                             \
          TASKFREE_SAV();             \
          INTFREE_SAV(sav);           \
        }

#define SRCH_ENTER_ATOMIC_SECT         SRCH_ENTER_CRIT_SECT
#define SRCH_LEAVE_ATOMIC_SECT         SRCH_LEAVE_CRIT_SECT
#define SRCH_ENTER_ATOMIC_SECT_SAV     SRCH_ENTER_CRIT_SECT_SAV
#define SRCH_LEAVE_ATOMIC_SECT_SAV     SRCH_LEAVE_CRIT_SECT_SAV

/*--------------------------------------------------------------------------
                           Regional Variables

   These variables are used all over, so it's more convenient to
   declare them here than in each module.
--------------------------------------------------------------------------*/

extern srch_state_enum_type  srch_state;
  /* Keeps track of the current state of the Searcher Task state machine. */

extern srch_parm_struct_type  srch_parms;
  /* Parameters that control the behavior of the searcher task.
     These are settable by the base station via MCC */


/*--------------------------------------------------------------------------
                          Queue Declarations
--------------------------------------------------------------------------*/

extern q_type          srch_cmd_q;
   /* This queue receives commands from MCC to SRCH */

/*===========================================================================

FUNCTION SRCH_GET_PROCESS_TIME

DESCRIPTION    Get the time available to process until the next Broadcast
               or Paging slot.

DEPENDENCIES   None.

RETURN VALUE   The available time in 80ms units.

SIDE EFFECTS   None.

===========================================================================*/
extern int4 srch_get_process_time( void );

/*===========================================================================

FUNCTION SRCH_SCAN_MEAS_LIST_MEMBER

DESCRIPTION    This function scans a system measurement list member
               and may send a report to MC if the results buffer is full.

DEPENDENCIES   None.

RETURN VALUE   Boolean reflecting whether we did or did not process
               an element this time.

SIDE EFFECTS   The RF may be tuned.
               The RF mode may be switched between digital and analog.

===========================================================================*/
extern boolean srch_scan_meas_list_member ( void );

/*---------------------------------------------------------------------------
                       Regional IS-2000 Definition
---------------------------------------------------------------------------*/
extern srch_ch_rc_type srch_rc;
  /* Store the RC values (updated by MUX layer calling srch_update_rc)
     so that the correct RC values can be logged */

extern int2 const srch_win_offset_tab[ ];
  /* The search window offset multiplier table */

/*--------------------------------------------------------------------------*/
/* SRCHTC SPECIFIC TYPE DEFINITIONS                       */
/*--------------------------------------------------------------------------*/
typedef enum          /* Define values for the substate */
{
  SRCHTC_SS0_INIT,         /* Punt */
  SRCHTC_SS1_ASET,         /* Process sweep of Active Set member */
  SRCHTC_SS2_CSET,         /* Process sweep of Candidate Set member */
  SRCHTC_SS3_NSET,         /* Process sweep of Neighbor Set member */
  SRCHTC_SS4_RSET,         /* Process sweep of Remaining Set member */
  SRCHTC_SS5_NREV,         /* Process revisit of interesting Neighbor */
  SRCHTC_SS6_CFNSET        /* Process sweep of Candidate Freq. Set member */
}
srchtc_substate_enum_type;

  typedef enum          /* Define values for the hard handoff substate */
{
  SRCHTC_HHO_DONE,         /* No hard handoff in progress */
  SRCHTC_HHO_ACQ,          /* Acquiring another frequency pilot */
  SRCHTC_HHO_CONFIRM,      /* Waiting hard handoff success signal */
  SRCHTC_HHO_ABORT,        /* Aborting hand hanoff due ec/io < thr */
  SRCHTC_HHO_CFSRCH        /* Waiting CF Nset search to be finished */
}
srchtc_hho_state_enum_type;

typedef enum          /* Define values for the CFS substate */
{
  SRCHTC_CFS_SF,           /* In Serving Frequency Now */
  SRCHTC_CFS_CF,           /* In Candidate Frequency Now */
  SRCHTC_CFS_TUNING        /* It's tuning the channel now */
}
srchtc_cfs_state_enum_type;

/* macro to determine the index into srch_aset[] from a pointer */
/* REVISIT: Remove when callers quit calling this macro */
#define ASET_INDEX(p) (0)

#ifdef FEATURE_IS2000_REL_A_CC
/*===========================================================================

FUNCTION SRCHZZ_CONFIG_RXC

DESCRIPTION
  This function is used to configure RXC for the appropriate channel for
  the next slot to be monitored.

DEPENDENCIES
  srch_idle_info, srch_rxc_msg_buf

RETURN VALUE
  None.

SIDE EFFECTS
  Uses srch_rxc_msg_buf.

===========================================================================*/

extern void srchzz_config_rxc
(
  boolean override_skip
    /* Do not let RXC skip a frame if we are already awake and decoding */
);
#endif /* FEATURE_IS2000_REL_A_CC */

/*===========================================================================

FUNCTION SRCH_ZZ_COMPUTE_NEXT_SLOT

DESCRIPTION
  This function calculates the next assigned slot boundary.

DEPENDENCIES
  Many

RETURN VALUE
  None

SIDE EFFECTS
  srchzz_slot_cycle_length & srchzz_wakeup_time are modified.

===========================================================================*/

extern void srch_zz_compute_next_slot ( void );

/*===========================================================================

FUNCTION SRCH_ZZ_COMPUTE_SCI

DESCRIPTION
  This function calculates the desired SCI based on the mobile's allowed
  value and the BS's desired value.

DEPENDENCIES

RETURN VALUE
  The SCI that will be used.

SIDE EFFECTS

===========================================================================*/

extern byte srch_zz_compute_sci
(
  srch_sleep_cmd_type  *cmd_ptr
    /* Pointer to command which is causing us to enter this state. */
);


/*===========================================================================

FUNCTION SRCHPC_UPDATE_ASET

DESCRIPTION    This function is called to handle an active set
      update command on the Paging Channel.  The command contains a new
      pilot, walsh channel, and PN long code mask for the active paging
      channel.

      If the new active sector pilot is already active or in the neighbor list
      (which should always be the case, except for Channel List Messages),
      it will be used as the active sector with its old trpt values.
      In case an unknown sector is specified as active, it will be used
      with an initialized trpt.

DEPENDENCIES   srchpc_*
               DIP switch 3 enables long code scrambling

RETURN VALUE   Command status - always SRCH_DONE_S (good status *)

SIDE EFFECTS   srch_aset and sometimes srch_nset are updated.
               The PN long code mask is updated in the demod chip.

===========================================================================*/

extern word  srchpc_update_aset
(
  srch_pc_aset_cmd_type  *cmd_ptr
      /* Pointer to the command that got us here */
);



/*--------------------------------------------------------------------------
         PCS specific Regional Variables
---------------------------------------------------------------------------*/

extern word srch_pri_mask;
  /* The priority mask for extended neighbor list searches */

extern word  srch_temp_est;
  /* Our most recent temperature estimate */

extern int2  srch_win_siz;
  /* Default search window size in chipx8 units. Over-air parms will
     supercede this value. */

extern uint2  srch_ref_pilot;
  /* Record of which pilot offset is the time reference */

extern srch_sect_struct_type* srch_ref_ptr;
  /* pointer to reference pilot Active set */

extern int2  srch_agc_rssi;
  /* Regional variable holding AGC RSSI (appx -105dBm.. scale) */

extern srch_roll_isr_vars_struct_type  srch_roll_isr_vars;
  /* The 26.667ms PN roll ISR variables */

extern uint2  srch_init_jump_pilot;
  /* Which pilot offset we used in the initial jump to hyperspace */

extern uint2  srch_last_good_pilot;
  /* The pilot index of the most recent reference pilot. This
     is used as a guess of where to start when re-acquiring. */

extern int4  srch_last_good_window;
  /* The position of the end of the search window for the most
     recent reference finger. This is used as a guess of where
     to start when re-acquiring. */

extern int2  srch_last_good_win_siz;
  /* The size of the search window for the most recent reference finger, used
     in concert with srch_last_good_window to determine where to start when
     re-acquiring. */

extern boolean srch_rudely_awakened;
  /* Flag: nonzero if an event other than the expiration of the
     sleep timer has resulted in a decision to wake up the MSM. */

extern srch_msm_config_struct_type srch_msm_config;
  /* Structure holding MSM version related configuration parameters */

extern boolean srch_ho_to_oband;
  /* Flag indicating if the mobile just handed-off to a different band class.
     Set during SRCH_TC_ASET_F processing, and used and reset in the function
     srch_log_agc_pctl() */


/*---------------------------------------------------------------------------
                    Regionalized Timer Definitions
---------------------------------------------------------------------------*/

extern timer_type  srch_rpt_timer;
  /* This timer is used during REX waits for an event. It wakes up
     the Search Task in time to signal the watchdog task. It is
     only used in the srch_wait function. */


/*-------------------------------------------------------------------------
             Regionalized storage for a list of Cells

   This count+array list keeps track of the cells in the Active Set.
   For this purpose, a cell is a collection of sectors that all carry
   the same power control subchannel.

   Membership of sectors in a cell is maintained in the sector struct.
   These cell structs just keep some common information for each cell.
-------------------------------------------------------------------------*/

extern srch_cell_struct_type  srch_cells[ ];
  /* Data about each cell. There is at most one cell per
     sector in the Active Set, so SRCH_ASET_MAX is a safe
     size for this list. */

extern word  srch_cell_cnt;
  /* Count of the number of cells in the list */

/*---------------------------------------------------------------------------
                Regionalized Sign Extension Lookup Table

   This table is used together with the SIGNX macro to sign extend
   a 18-bit signed two's-complement number stored in the LS bits of
   a 32-bit word to the full 32 bits. This is useful for positions
   as reported by the demod chip.
---------------------------------------------------------------------------*/

extern word srch_sign_words[ ];

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
extern word  const srch_tc_nthresh_tab[ ];


/*---------------------------------------------------------------------------
      Regionalized Energy to Pre-Candidate Set Threshold Lookup Table

   This table is used to convert the T_ADD value sent over the air
   into a threshold for Pre-Candidate Set members to advance their
   state variables within the Pre-Candidate Set.  This is computed
   to match, with optimum probability, the Ec/Io values required by
   IS-95.

---------------------------------------------------------------------------*/

extern word  const srch_tc_pcthresh_tab[ ];

/*---------------------------------------------------------------------------
          Regionalized Window Size Decoding Lookup Table

   This table is used to expand the window size code in the
   search parameters command into an actual window size.
   This is Table 6.6.6.2.1-1 from CAI 2.0.
---------------------------------------------------------------------------*/

extern int2  const win_tab[ ];

/*---------------------------------------------------------------------------
              Regionalized T_TDROP Decoding Lookup Table

   This table is used to expand the T_TDROP index in the search parameters
   command into an actual duration. This is Table 6.6.6.2.3-1 from CAI 2.0.
   Values appear in units of 26.66..ms. Where the CAI value is not an even
   multiple of 26.66..ms, the value is rounded up.
---------------------------------------------------------------------------*/

extern word  const srch_t_tdrop_tab [ ];

/*---------------------------------------------------------------------------
     Regionalized Default Ec/Io to CDMA RSSI display percentage table

   This table is used to convert energy (6-bit unsigned Ec/Io values as
   required by 6.7.2.3.2.5 of IS-95) into a corresponding linearized
   device-independent CDMA RSSI percentage of display to show.
---------------------------------------------------------------------------*/

extern byte  const srch_rssi_index_tab[ ];

/*-------------------------------------------------------------------------*/

#ifdef FEATURE_PLT
#error code not present
#endif /* FEATURE_PLT */


/*===========================================================================

                      REGIONAL FUNCTION DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
               Standard Library Function Declarations

   These functions are used all over.  Rather than include the standard
   headers for these functions (and bring in lots of irrelevant stuff)
   it is convenient to simply declare them here.
---------------------------------------------------------------------------*/

#ifndef _enable
extern void  _enable(void);
#endif
#ifndef _disable
extern void  _disable(void);
#endif
#ifndef __size_t
#define __size_t 1
typedef unsigned int size_t;   /* from <stddef.h> */
#endif
extern void *  memset(void * s, int c, size_t n);
extern void *  memcpy(void *dest, const void *src, unsigned int count);
extern void *  memmove(void *dest, const void *src, unsigned int count);


/*=============================================================================

FUNCTION       SRCH_PEAK_INIT

DESCRIPTION    Initializes the peak module and only needs to be called once.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/
extern void srch_peak_init( void );


/*===========================================================================

FUNCTION SRCH_INACTIVE_INIT

DESCRIPTION
  This function is the entry initialization code for the INACTIVE state. Its
  operation depends on the state from which it is entered.  All Search task
  states (INACTIVE, CDMA, ACQ, SC, SLEW, PC, TC, ZZ, DZ) may successfully
  init the INACTIVE state. If INACTIVE is initialized from any valid DMSS state,
  good status is returned indicating that the state transition to INACTIVE is
  approved. In addition, if INACTIVE is initialized from TC state, closed-loop
  power control and its associated puncturing is also disabled, having been
  in use when the DMSS was in a call.

DEPENDENCIES
  None.

RETURN VALUE
  Status of the requested state transition:
      SRCH_DONE_S if successful.
      SRCH_BADSEQ_S if unsuccessful due to a illegal state transition.

SIDE EFFECTS
  puncturing and closed-loop power control may be disabled.

===========================================================================*/

extern srch_cmd_status_type srch_inactive_init
(
  srch_state_enum_type cur_state    /* current state */
);


/*===========================================================================

FUNCTION SRCH_INACTIVE_STATE

DESCRIPTION
  This function is the state handler for the Inactive state.  The SRCH task
  enters Inactive state as its default state on power up.  When the 1x
  protocol is activated by MMoC, MC commands SRCH to enter CDMA state.  When
  the 1x protocol is deactivated by MMoC, MC commands SRCH to enter CDMA
  state and then to deactivate.  In other words, Inactive state is only
  ever entered from powerup up or from CDMA state.

  The INACTIVE state's main loop waits for, and processes events as follows:

  Event         Origin  Inactive state's action  Results (If successful)
  -----         ------  -----------------------  -----------------------
  DEACTIVATE_F  MC      Ignore                   Stay in Inactive
  AMPS_F        MC      Init AMPS state          Transition to AMPS
  CDMA_F        MC      Init CDMA state          Transition to CDMA
  POWERDOWN_F   MC      Call srch_powerdown()    Stay in Inactive
  STOP sig      TMC/MC  Ack the STOP sig         Stay in Inactive
  OFFLINE sig   TMC/MC  Ack the OFFLINE sig      Stay in Inactive

  Any other MC-SRCH command received causes Inactive state to return a
  BADSEQ_S status. Any other REX signal sent to Search during Inactive state
  is ignored.

  Currently the SRCH_AMPS_F command is not implements since no current
  targets require AMPS.  This command will be implemented only if there
  is a new requirement to support AMPS operation on a current or new target.

DEPENDENCIES
  This function may only be called after a successful call of srch_cdma_init.

RETURN VALUE
  The new state of the searcher task:  CDMA

SIDE EFFECTS
  The PN roll and status dump isr's are unhooked (unless GPS needs them)

===========================================================================*/

extern srch_state_enum_type srch_inactive_state( void );


/*===========================================================================

FUNCTION SRCH_CDMA_INIT

DESCRIPTION
  This function is the entry code for the CDMA state.

DEPENDENCIES
  None.

RETURN VALUE
  SRCH_DONE_S - All states can transition to CDMA state.

SIDE EFFECTS
  None.

===========================================================================*/

extern srch_cmd_status_type  srch_cdma_init
(
  srch_state_enum_type cur_state,                /* current state          */
  srch_cmd_type       *ext_cmd                   /* CDMA_F ext cmd ptr     */
);


/*===========================================================================

FUNCTION SRCH_CDMA_STATE

DESCRIPTION    This function is the state handler for the CDMA state.

      Search CDMA state is responsible for obtaining the RF lock before
      entering ACQ, and also for performing dedicated system measurement.
      When entered from ACQ or SYNC states (acquisition failure), CDMA
      state releases the RF lock.  When entered from other states it holds
      onto the RF to allow for immediate re-acquisition.  When asked to
      perform a system measurement, CDMA state obtains the RF lock,
      performs the measurement, then releases the lock.  It is expected
      that MMoC deactivates the 1x protocol before requesting a dedicated
      measurment.  As a result, dedicated measurements always occur in the
      following sequence: Inactive -> CDMA -> do measurement -> Inactive.


      The CDMA state's main loop waits for, and processes events as follows:

   Event         Origin  CDMA state's action   Results (If successful)
   ------------  ------  --------------------  -----------------------
   DEACTIVATE_F  MC      Init Inactive state   Transition to Inactive state
   ACQ_F         MC      Init ACQ state        Transition to ACQ state
   SYS_MEAS_F    MC      Perform system meas.  Stay in CDMA state

      Any other MC-SRCH command received causes CDMA to return a BADSEQ_S
      status. Any other REX signal sent to Search during CDMA is ignored.

DEPENDENCIES   This function may only be called after a successful call of
               srch_cdma_init.

RETURN VALUE   The new state of the Search task, Inactive state or ACQ state.

SIDE EFFECTS   None.

===========================================================================*/

extern srch_state_enum_type  srch_cdma_state ( void );


#ifdef FEATURE_ACQ2
/*===========================================================================

FUNCTION SRCH_ACQ_INIT

DESCRIPTION    This function is the entry code for the ACQ state.

      Search ACQ state may only be entered from the CDMA state after
      the RF lock has been obtained.

DEPENDENCIES   Freq estimation -vs- temperature table.

RETURN VALUE   Status of the requested state transition:
      SRCH_DONE_S if successful.
      SRCH_BADSEQ_S if unsuccessful due to an illegal state transition.

SIDE EFFECTS   The roll interrupt service routine is hooked up.
               Various demod chip registers are written.

===========================================================================*/

extern srch_cmd_status_type  srch_acq_init
(
  srch_state_enum_type  cur_state,               /* "previous" state       */
  srch_cmd_type        *ext_cmd                  /* ACQ_F ext cmd ptr      */
);
#endif /* FEATURE_ACQ2 */



/*===========================================================================

FUNCTION SRCH_ACQ_STATE

DESCRIPTION    This function is the state handler for the ACQ state.

      In the ACQ state, the Search task is trying to find a good pilot.
      Upon entry, ACQ state initializes and slews the searcher and finger
      hardware, gets Cellular/PCN and Full/Mini/Micro modes from DB services,
      and begins the first search. Then, ACQ enters a loop which continues as
      long as the Search task is in the ACQ state.

      The ACQ state's main loop waits for, and processes events as follows:

    Event       Origin   ACQ state's action       Results (If successful)
    -----       ------   --------------------     -----------------------
   CDMA_F         MC      Init CDMA state          Transition back to CDMA
   SC_F           MC      Init SC state            Transition forward to SC
   DUMP sig     searcher  Process based on SS      Continue acquiring in ACQ
   ACQ timeout    REX     Process based on SS      Continue acquiring in ACQ
   Lost dmp t/o   REX     Trigger status dump      ACQ state machine continues.

      Any other MC-SRCH command received causes ACQ to return a BADSEQ_S
      status. Any other REX signal sent to Search during ACQ is ignored.

      The ACQ state's sends messages and data to other tasks as follows:

   What Sent    Sent To   Why sent                 Expected response
   ---------    -------   --------                 -----------------
   ACQFAIL_R      MC      Micro-acq AGC too low    Command back to CDMA
   ACQFAIL_R      MC      Mini-acq stage switch    Command back to CDMA
   ACQ_R          MC      Acquired, found pilot    Command forward to SC
   TA packets    DIAG     Search scans available   None
   Fing packets  DIAG     Finger status available  None

      During a full-up acquisition, no ACQFAIL_R message is ever sent
      to MC if no pilot is found. ACQ state just continues looking for
      a pilot, relying on MC to timeout and command Search back to CDMA.

      The ACQ state uses four stages of acquisition parameters. Changing
      stages result in searches which look deeper and deeper (lower and
      lower Ec/Io) into the signal, looking for a pilot, as the stage is
      incremented. As higher stages of acquisition are used, lower Ec/Io's
      will be looked for, but it will take longer and longer to traverse
      around the PN circle.

      The ACQ state has four substates: SS1_DETECT, SS2_DWELL, SS3_VERIFY,
      and SS4_PULL_IN. These substates are detailed in the ACQ event processing
      function header pertaining to each substate.

DEPENDENCIES   srchaq_*

RETURN VALUE   The new state of the searcher task: CDMA or SC.

SIDE EFFECTS   srch_aset[0] is used.
               srch_fings is used.
               Various demod chip registers are written.

===========================================================================*/

#ifdef FEATURE_ACQ2
extern srch_state_enum_type  srch_acq_state ( void );
#endif /* FEATURE_ACQ2 */


/*===========================================================================

FUNCTION SRCH_SC_INIT

DESCRIPTION    This function is the entry code for the SC state.

      Search SC state may be initialized from ACQ states. In
      both cases, various initializations are performed such as setting
      up frequency tracking and disabling scrambling for SC state.
      Having performed these initializations, the init function returns
      good status. If SC is initialized from any other state, it returns
      bad status.

DEPENDENCIES   None.

RETURN VALUE   Status of the requested state transition:
      SRCH_DONE_S if successful.
      SRCH_BADSEQ_S if unsuccessful due to an illegal state transition.

SIDE EFFECTS   Various demod chip registers are written.

===========================================================================*/

extern srch_cmd_status_type  srch_sc_init
(
  srch_state_enum_type  cur_state,               /* "previous" state       */
  srch_cmd_type        *ext_cmd                  /* SYNC_F ext cmd ptr     */
);



/*===========================================================================

FUNCTION SRCH_SC_STATE

DESCRIPTION    This function is the state handler for the SC state.
      The purpose of SC state is to set up the demod path for receiving
      sync channel frames, and to keep demodulator fingers assigned
      to the strongest energy paths for highest quality demodulation.

      Upon entry to SC state, initializations take place such as
      setting the combiner and finger symbol size to 256 chips/symbol,
      slamming combiner to locked finger timing, and setting up the
      Active set structure to contain SC-relates search parameters.
      Once these initializations are performed, SC enters a loop which
      continues as long as the Search task is in the SC state.

      The SC state's main loop waits for, and processes events as follows:

    Event       Origin   SC state's action        Results (If successful)
    -----       ------   --------------------     -----------------------
   CDMA_F         MC      Init CDMA state          Transition back to CDMA
   SLEW_F         MC      Init SLEW state          Transition forward to SLEW
   ROLL sig     combiner  Process based on SS      Continue in SC, change SS
   DUMP sig     searcher  Process based on SS      Continue in SC, searching
   Governor sig   REX     Wait for governor t/o    Continue in SC, searching
   RSSI sig       REX     Write CDMA RSSI to DB    Continue in SC, searching
   Lost dmp t/o   REX     Trigger status dump      SC state machine continues.

      Any other MC-SRCH command received causes ACQ to return a BADSEQ_S
      status. Any other REX signal sent to Search during ACQ is ignored.

      The SC state only sends TA / Finger log packets to DIAG. SC state
      sends no *_R messages back to MC. SC just relies upon MC to receive a
      good sync channel message and command Search to transition forward
      to SLEW state. If good messages are not received, SC state relies
      upon MC to time out and send Search back to CDMA.

      SC state has three substates:  SS1_SKIP, SS2_SYNC, and SS3_TRACK.

      In SS1_SKIP, we simply wait for several PN rolls to pass. This
      makes sure the combiner slam has plenty of time to take effect
      before we start moving fingers around. When the roll countdown
      runs out, we move on to SS2_SYNC.

      In SS2_SYNC, we wait for a PN roll to pass, at which time the
      combiner will be aligned to finger (and thus searcher) timing.
      When that occurs, we enable combiner time tracking on the slammed
      finger, originate a searcher scan, and go on to SS3_TRACK.

      In SS3_TRACK, we wait for the searcher to complete sweeps of the
      active window. When a sweep is complete, we run the search "triage"
      operation to get the best finger assignments and begin a new
      search. We stay in this substate forever, until MC commands Search
      to transition to a new state.

DEPENDENCIES   srchsc_*
               Assumes the searcher is still lined up on the signal at entry.

RETURN VALUE   The new state of the searcher task: CDMA or SLEW.

SIDE EFFECTS   srch_aset[0] is used.
               srch_fings is used.
               Various demod registers are written.

===========================================================================*/

extern srch_state_enum_type  srch_sc_state ( void );


#ifdef FEATURE_SRCH_VOIP_1X_HANDOFF
#error code not present
#endif /* FEATURE_SRCH_VOIP_1X_HANDOFF */


/*===========================================================================

FUNCTION SRCH_SLEW_INIT

DESCRIPTION    This function is the entry code for the SLEW state.

      Search SLEW state may only be initialized from the SC state.
      Upon entry from SC state, the initialization function copies the
      information from the slew command (pilot offset, 80ms phase),
      shifts fingers and combiner to using 64 chips/symbol and returns
      good status. If SLEW is initialized from any other state, it returns
      bad status.

DEPENDENCIES   None.

RETURN VALUE   Status of the requested state transition:
      SRCH_DONE_S if successful.
      SRCH_BADSEQ_S if unsuccessful due to an illegal state transition.

SIDE EFFECTS   srch_init_jump_pilot is updated.
               Various demod chip registers are written.

===========================================================================*/

extern srch_cmd_status_type  srch_slew_init
(
  srch_state_enum_type  cur_state,
    /* Specifies current state from which this function is being invoked. */

  srch_cmd_type        *cmd_ptr
    /* Pointer to command which caused us to go to this state. */
);


/*===========================================================================

FUNCTION SRCH_SLEW_STATE

DESCRIPTION    This function is the state handler for the SLEW state.

      In the SLEW state, the Search task performs the manipulations on
      the demod chip to place the fingers, searcher, and symbol combiner
      all on the correct paging channel timing.

      Upon entry to SLEW state, SLEW enters loop which continues as long
      as the Search task is in the SLEW state. The SLEW state's main loop
      waits for, and processes events as follows:

    Event       Origin   SLEW state's action      Results (If successful)
    -----       ------   --------------------     -----------------------
   CDMA_F         MC      Init CDMA state          Transition back to CDMA
   PC_F           MC      Init PC state            Transition forward to PC
   ROLL sig     combiner  Process based on SS      Continue in SLEW, change SS

      Any other MC-SRCH command received causes SLEW to return a BADSEQ_S
      status. Any other REX signal sent to Search during SLEW is ignored.

      The SLEW state only sends the SLEW_R report back to MC on the PN roll
      right before the hyperspace boundary. The receiver of the SLEW_R report
      must act on it quickly. Originally, there was exactly 320ms from the
      start of the sync channel was received to the hyperspace boundary, but
      once SLEW state finshes working, the very next 26ms roll is the boundary.
      MC must cause a PC_F command to be sent to the Search task in time
      for it to act before the next 26ms PN roll, or the jump to hyperspace
      will be missed. The Search task does not currently detect if this happens.

      The SLEW state has five substates: SS1_SLEW, SS2_PHS_ADJ,
      SS3_UNLOCK, SS4_REPORT, and SS5_DONE.

      In SS1_SLEW, we wait for the 26ms PN roll that starts the middle
      26ms period of an 80ms, according to the existing sync channel
      timing in the symbol combiner. At that time, we compute the
      slew value needed to reach system timing by taking out the pilot
      offset per the sync channel message (using a full 26ms slew in
      the case of zero pilot offset). We command the fingers to slew,
      command the symbol combiner to align its timing with that of
      the fingers (forcing F0 into lock so the combiner can align to
      it), and command the searcher to slew to a position that will
      align the search window on the computed signal position on the
      paging channel. We then advance to SS2_PHS_ADJ.

      In SS2_PHS_ADJ, we wait for a 26ms PN roll that starts the first
      26ms period of an 80ms. This will be the next 26ms PN roll, since
      we programmed the combiner to align its timing before leaving
      SS1_SLEW. When it arrives, we program the new 80ms phase to align
      the next 80ms boundary on the right 26ms boundary (given by
      alignment with the start of the sync channel message). We then
      advance to SS3_UNLOCK.

      In SS3_UNLOCK, we wait for the new 80ms boundary to pass. When it
      does, we remove the forced lock from finger 0.  We then go on to
      SS4_REPORT.

      In SS4_REPORT, we wait for a 26ms PN roll that starts the last 26ms
      period of a (new) 80ms. When it comes around, we make a slew done
      report and advance to SS5_DONE.

      In SS5, we do nothing.  We just wait for a state change command.

      So, the net effect is the following sequence:
            1. Wait for PN roll, phase 1, on sync channel timing.
            2. Everything slews by up to 26ms.
            3. At the next PN roll, we are on system timing except for
               80ms phase.
            4. Adjust 80ms phase to get true system time.
            5. 1, 2, or 3 PN rolls later, we're in phase 0 on system time.
            6. Unforce lock on finger 0.
            7. PN roll, phase 1.
            8. PN roll, phase 2.
            9. Make a slew report.
           10. Wait for a state change command.
           11. Next PN roll is phase 0, and time to load the PN state
               for the paging channel (per the sync channel message).
               This operation is accomplished later in srch_pc_init.
               This 80ms boundary is the hyperspace boundary.

DEPENDENCIES   srch_slew_vars
               Assumes that the searcher and all the fingers (especially
               finger 0) are lined up on the sync channel signal at entry.

RETURN VALUE   The new state of the searcher task: CDMA or PC.

SIDE EFFECTS   Various demod registers are written.

===========================================================================*/

extern srch_state_enum_type  srch_slew_state ( void );


/*===========================================================================

FUNCTION SRCH_TC_INIT

DESCRIPTION    If TC state is initialized from PC as normal, it initializes the
      TC parms and copies over the TC command info to the static TC structure.
      If entered from any other state, it returns bad status.

DEPENDENCIES   None

RETURN VALUE   Status of the requested state transition:
      SRCH_DONE_S if successful.
      SRCH_BADSEQ_S if unsuccessful due to an illegal state transition.

SIDE EFFECTS   TC parms are initialized.

===========================================================================*/

extern srch_cmd_status_type  srch_tc_init
(
  srch_state_enum_type  cur_state,
    /* Specifies current state from which this function is being invoked. */

  srch_cmd_type        *ext_ptr
    /* Pointer to command which is causing us to enter this state. */
);


/*===========================================================================

FUNCTION SRCH_TC_STATE

DESCRIPTION    This function is the state handler for the TC state.

      The purpose of TC state is to keep the demodulating fingers in the
      best places to receive traffic channel data. It works with the
      searcher hardware to triage fingers amongst the best sectors
      in the Active set. TC will generate a Pilot Strength Measurement
      Message if any Active, Candidate, or Neighbor's pilot energy or age
      time crosses certain thresholds specified by IS-95. The base station
      then determines what type of handoff (soft/softer/hard) is necessary, and
      will signal the DMSS, thus MC and Search tasks, to complete the handoff.

      Upon entry to TC state, sector, finger, and other initializations are
      performed, and a loop is entered which continues as long as the Search
      task is in the TC state. This loop waits for, and processes events as
      follows:

    Event        Origin     TC state's action       Results (If successful)
    -----        ------     --------------------    -----------------------
   CDMA_F          MC        Init CDMA               Transition back to CDMA
   PARM_F          MC        Load over-air parms     Stay in TC, punt schedule
   TC_ASET_F       MC        Update Active pilot     Stay in TC, punt schedule
   TC_NSET_F       MC        Update Neighbor list    Stay in TC
   MEAS_F          MC        Send TC_MEAS_R rpt      Stay in TC
   FPC_MEAS_F      MC        Send TC_MEAS_R rpt      Stay in TC
   DUMP sig     searcher     Process based on SS     Stay in TC
   Governor sig   REX        Wait for governor t/o   Stay in TC
   Freq timer sig REX        Update freq-temp table  Stay in TC
   RSSI timer sig REX        Write CDMA RSSI to DB   Stay in TC
   PSM timer sig  REX        Allow TS_MEAS reports   Stay in TC
   REF timer sig  REX        Allow time ref adj      Stay in TC
   CTR timer sig  REX        Allow centering slews   Stay in TC
   Lost dmp t/o   REX        Trigger status dump     TC state machine continues

      Any other MC-SRCH command received causes TC to return a BADSEQ_S
      status. Any other REX signal sent to Search during TC is ignored.

      The TC state sends messages and data to other tasks as follows:

   What Sent    Sent To    Why sent                   Expected response
   ---------    -------    --------                   -----------------
   TC_MEAS_R      MC      Pilot Eng/age cross thresh  Handoff Direction Message
                           or Base station requests    or nothing at all.

   TA packets    DIAG     Search scans available      None
   Finger pkts   DIAG     Finger status available     None

      The TC state has five substates: SS0_INIT, SS1_ASET, SS2_CSET, SS3_NSET,
      and SS4_RSET. Generally we start in SS1_ASET and only drop back to
      SS0_INIT if we decide to punt the search schedule and start over.

      In SS0_INIT, we are merely waiting for the current sweep to finish
      so we can discard it and begin again at SS1_ASET. This substate is
      entered when either a PARM_F or TC_ASET_F command arrives. It is
      used to terminate the current activity so we can begin afresh with
      (possibly) new parameters. Punting when a PARM_F is necessary as we
      do not want to update sector window sizes and other parms on the fly.
      Computations and evaluations using the old parameters may be fouled up.
      Punting on a ASET_F command is needed as we may have completely
      reorganized the sector structures and reordered the search schedule.

      In SS1_ASET, we are waiting for an Active Set sweep to finish.  When
      it does, we grab its results, being the next search, and go to the
      substate that corresponds to the type of search selected. If this
      was the last Active Set sector, we run finger assignment on the
      new Active Set results.

      In SS2_CSET, we are waiting for a Candidate or Pre-Candidate Set
      sweep to finish.  When it does, we grab its results, begin the next
      search, and go to the substate that corresponds to the type of
      search selected.  We then evaluate the sweep results.

      In SS3_NSET, we are waiting for a Neighbor Set sweep to finish. When
      it does, we grab its results, begin the next search, and go to the
      corresponding substate.  We then evaluate the sweep results.

      In SS4_RSET, we are waiting for a Remaining Set search to finish.
      When it does, we grab its results, begin the next search, and go to
      the corresponding substate.  We then evaluate the sweep results.

      Explicitly, in TC state the pilot search schedule will be:

         A1  A2  A3 .. Ai    C1  C2  C3 .. Cj    N1
         A1  A2  A3 .. Ai    C1  C2  C3 .. Cj    N2
           <etc>
         A1  A2  A3 .. Ai    C1  C2  C3 .. Cj    Nk

         A1  A2  A3 .. Ai    C1  C2  C3 .. Cj    N1
         A1  A2  A3 .. Ai    C1  C2  C3 .. Cj    N2
           <etc>
         A1  A2  A3 .. Ai    C1  C2  C3 .. Cj    R1     ..etc, etc

      That is, we search all Active pilots, all Candidates and Pre-Candidiates,
      one Neighbor, then repeat. Occasionally we will substitute a Remaining
      pilot search for a Neighbor search.

      is performed. An Rset search is performed once every "rcnt_max" spins
      spins through the A/C/N schedule. The process of sweeping continues
      indefinitely until the calling routine receives a command to change to
      another state.

      To grasp the expected revisit frequency of various pilots, consider
      the following benchmarks of search revisit speed versus window size:

                 Search               Approximate
               Window size           Raw srch rate
                (chips)              (searches/sec)
               ----------            --------------
                  20                     150
                  28                     120
                  40                     100
                  60                      70
                  80                      50

      These numbers assume equal window sizes for all sets and are approximate
      benchmarks taken from MSM1 and early MSM2P-based targets. The TC revisit
      frequencies of unique pilot PN offsets in each set will approximately be:

        Each Active Pilot:  Fa = R / (aset_size + cset_size + 1)

        Each Candidate:     Fc = Fa

        Each Neighbor:      Fn = R / (aset_size + cset_size + nset_size)

        Each Remaining:     Fr = (Fa / rcnt_max)
                     / ((512/pilot_inc) - aset_size-cset_size - nset_size)

      where R is the raw search rate, a/c/nset_size are the sizes of Active,
      Candidate, and Neighbor sets being searched, and rcnt_max is the number
      of schedules per Remaining search.

      The most critical parameters are Fa and Fn. Fa determines how fast finger
      assignments are refreshed, and Fn determines how fast we can react to
      changes in the environment such that soft handoff is needed to continue
      to keep acquisition and demodulate good traffic channel frames.

      For 40-chip windows, 3 Active pilots, 1 Candidate, and 6 Neighbors,
      we can expect Fa = Fc = 20 Hz, Fn = 10 Hz, and thus the finger assignment
      rate also equals 20 Hz.

      Allowable set transitions and conditions for reporting are all according
      to IS-95 and are detailed in the search evaluation functions for each set.
      See them for more details.

      The Active, Neighbor, and Remaining sets may also require different
      search window sizes. This is accounted for by continually remembering
      the latest slew we backed the searcher up by in order to center the
      search scan's window symmetrically about the pilot offset. On each
      search, the previous window-related slew is removed, the new window's
      "back-up" slew is added, the search is sent, and the slew memory is
      refreshed with the new window-related slew.

      When pilots are transitioned between sets, different window sizes require
      special handling of each pilot's nominal offset from center. For example,
      Highly offset Neighbors found in wide Neighbor set windows can be lost
      during narrow Candidate searches unless this is taken into account.

      In addition, in TC state, Active and Candidate searches are recommended
      to each be centered about their earliest usable multipath component. This
      recommendation and its related need for centering slew rate throttling
      and snapback hysteresis are all handled.

      Finally, IS-95 requires that a "time reference" be established, and
      searches and pilot measurement reports be in accordance with this time
      reference.  In TC state, this is taken into account via the POS_MATH
      baseline  (the latest time reference baseline is always set to be the
      POS_MATH  position center relative to the reference sector). This time
      reference  is gradually changed as the reference is changed according to
      maximum slew rates specified by IS-95 and reflected in symbol combiner
      operation.

      Therefore, in a dynamic environment with delayed pilots changing set
      membership, all these slew movements (window sizing, nominal offset,
      centering slew, snapback hysteresis, and time reference baseline
      movements) work together to center search windows and make reports
      according to IS-95 requirements and recommendations.

DEPENDENCIES   srchtc_*
               srch_roll_isr_vars.tick26
               srch_aset, srch_cset, srch_nset, ...

RETURN VALUE   The new state of the Search task:  CDMA.

SIDE EFFECTS   Many srchtc_* local variables are heavily used.
               Regional sector, finger, path, cell structs are heavily used.
               Various demod chip registers are written

===========================================================================*/
extern srch_state_enum_type  srch_tc_state ( void );


/*===========================================================================

FUNCTION SRCH_GET_NOISE_THRESH_LECIO

DESCRIPTION    This function based on window size determines the noise
      threshold  used to reject search peaks.

DEPENDENCIES   None.

RETURN VALUE   The noise theshold in linear ecio

SIDE EFFECTS   None.

===========================================================================*/
extern uint32 srch_get_noise_thresh_lecio
(
  uint32 win_size  /* winsize is cx8 */
);


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
);

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
);

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
);

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
);

/*===========================================================================

FUNCTION SRCH_VOTE_OK_TO_SLEEP

DESCRIPTION    This function tells sleep that tcxo shutdown is ok (i.e.
  ok to sleep)

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
void srch_vote_ok_to_sleep( void );

/*===========================================================================

FUNCTION SRCH_VOTE_NOT_OK_TO_SLEEP

DESCRIPTION    This function tells sleep that tcxo shutdown is not ok (i.e.
  not ok to sleep)

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
void srch_vote_not_ok_to_sleep( void );

/*===========================================================================

FUNCTION SRCH_EXIT_SERVICE

DESCRIPTION    This function does all the common processing needed when
               exiting zz, pc, or tc states, and entering inactive or cdma
               states.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
extern void srch_exit_service( void );

/*===========================================================================

FUNCTION SRCH_AGC_TO_DBM

DESCRIPTION    This function converts an Rx AGC value to dBm.

DEPENDENCIES   None.

RETURN VALUE   The AGC value in dBm units.

SIDE EFFECTS   None.

===========================================================================*/

extern int2  srch_agc_to_dbm
(
  byte rx_agc
    /* CDMA RX AGC value */
);


/*===========================================================================

FUNCTION SRCH_LOG_AGC_PCTL_ENA

DESCRIPTION    This function is invoked to enable logging of AGC and power
      control values.

DEPENDENCIES   srch_agc_pctl_vars

RETURN VALUE   None.

SIDE EFFECTS   Messages can be sent to DIAG task.

===========================================================================*/

extern void  srch_log_agc_pctl_ena ( void );



/*===========================================================================

FUNCTION SRCH_SET_PARM

DESCRIPTION    This function is called to implement a command to set
      certain parameters that control the behavior of the searcher task.

DEPENDENCIES   None.

RETURN VALUE   Command status, always SRCH_DONE_S (good status).

SIDE EFFECTS   srch_parms is updated.

===========================================================================*/

extern srch_cmd_status_type  srch_set_parm
(
  srch_parm_cmd_type  *cmd_ptr
    /* Pointer to the Set Search Parameters command */
);



/*===========================================================================

FUNCTION SRCH_RPT

DESCRIPTION    This function enqueues a specified report from the Search
      Task to the Main Control Task.

DEPENDENCIES   This function assumes that the 'mcc_srch_q' has been properly
      initialized.

RETURN VALUE   None.

SIDE EFFECTS   The MC Task dequeues and processes the specified report.

===========================================================================*/

extern void  srch_rpt
(
  mccsrch_rpt_type  *rpt_ptr
    /* Pointer to searcher report. */
);


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

extern rex_sigs_type  srch_wait
(
  rex_sigs_type  sigs
    /* Mask of signals to wait for */
);

/* Add a define for srch_wait cast to return void */
#define void_srch_wait  ((void (*)(rex_sigs_type))srch_wait)


/*===========================================================================

FUNCTION SRCH_GET_CMD

DESCRIPTION    This function gets a command for the searcher task.
      It just gets one off the srch_cmd_q.

DEPENDENCIES   srch_cmd_q

RETURN VALUE   Pointer to the command.

SIDE EFFECTS   srch_cmd_q

===========================================================================*/

extern srch_cmd_type  *srch_get_cmd ( void );


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

extern void  srch_post_cmd
(
  srch_cmd_type  *cmd_ptr,
    /* Points to the command to perform post processing on. */

  srch_cmd_status_type  status
    /* Status to post for command. */
);


/*===========================================================================

FUNCTION SRCH_ROLL_ISR

DESCRIPTION    This function is invoked to service the 26.6667 millisecond
      interrupt generated by the demodulator chip.

DEPENDENCIES   This function assumes that pre- and post-interrupt processing
      with respect to stacks and rex are taken care of by the caller.

RETURN VALUE   None.

SIDE EFFECTS   timestamp clock is ticked forward.
               SRCH_ROLL and TXC_ROLL signals are set.
               srch_roll_isr_vars fields are updated.
               AGC/pctl logging may be enabled or disabled.

===========================================================================*/

extern void  srch_roll_isr ( void );


/*===========================================================================

FUNCTION SRCH_ROLL_COMMON_ISR

DESCRIPTION    This function is the sub section of the roll isr which
      handles all the common processing.  The design model is that a subsystem
      may install their own roll isr handler to preform sybsystem specific
      needs and then it should call this function to handle common system
      wide services

DEPENDENCIES   To be called inside of the roll isr context

RETURN VALUE   None.

SIDE EFFECTS   timestamp clock is ticked forward.
               TXC_ROLL signals are set.
               srch_roll_isr_vars fields are updated.
               AGC/pctl logging may be enabled or disabled.

===========================================================================*/
extern void srch_roll_common_isr (void);


/*===========================================================================

FUNCTION SRCH_RESET_PILOT_INFO

DESCRIPTION    This function resets the pilot record type for the active and
      neighbour sets and the fingers.  It also reset the FCH and DCCH active
      flags.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void  srch_reset_pilot_info( void );


/*===========================================================================

FUNCTION SRCH_CHAN_SET_IDLE_ULPN_MASK

DESCRIPTION    This function fills in the desired ULPN mask.

DEPENDENCIES

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/
extern void srch_chan_set_idle_ulpn_mask
(
  int2 pilot,
    /* Pilot of this channel */

  boolean load_mask
    /* Should this mask be loaded immediately */
);

/*===========================================================================

FUNCTION SRCH_GET_MOVING_PSD

DESCRIPTION    This function writes a moving average power spectral density
         value of serving frequency to the passed variable.

DEPENDENCIES    srch_moving_psd.

RETURN VALUE    TURE if the moving average is available,
                FALSE if it is not available:
                  1) The calcualtion is not enabled, OR
                  2) The samples is less than SRCH_CAI_N12M (10 ).

SIDE EFFECTS    None.

===========================================================================*/

extern boolean srch_get_moving_psd
(
  int2 *psd
  /*  Store the moving average of rssi in dB */
);

/*===========================================================================

FUNCTION SRCH_RESET_MOVING_PSD

DESCRIPTION    This function will also reset all the previous stored psd
               sample values.

DEPENDENCIES    srch_moving_psd.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/
extern void srch_reset_moving_psd(void);

/*===========================================================================

FUNCTION SRCH_SAVE_PSD_SAMPLE

DESCRIPTION    This function saves the most recent received power spectral
         density sample and discards the oldest sample.

DEPENDENCIES    srch_moving_psd.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

extern void srch_save_psd_sample( void );

/*===========================================================================

FUNCTION SRCH_ENABLE_MOVING_PSD

DESCRIPTION    This function enables the power spectral density moving
         average calculation for the serving frequency. And it will also
         reset all the previous stored psd sample values.

DEPENDENCIES    srch_moving_psd.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

extern void srch_enable_moving_psd
(
  boolean enable
  /* TRUE, enable it. FALSE, disable it. */
);

/*===========================================================================

FUNCTION SRCHPC_SYS_RESELECT_INIT

DESCRIPTION    This function initializes system re-selection process.

DEPENDENCIES   srch_aset.

RETURN VALUE   None.

SIDE EFFECTS   Initilizes srchpc_rssi, srchpc_rssi_ecio, srchpc_rssi_low_cnt,
               srchpc_rssi_samp_time.
===========================================================================*/
void srchpc_sys_reselect_init( void );


/*===========================================================================

FUNCTION SRCHPC_SYS_RESELECT_CHECK

DESCRIPTION    This function checks system re-selection condition.

DEPENDENCIES   srch_aset.

RETURN VALUE   Indicate if a report should have been sent, but was not.

SIDE EFFECTS   Updates srchpc_rssi, srchpc_rssi_ecio, srchpc_rssi_low_cnt,
               srchpc_rssi_samp_time.
===========================================================================*/
boolean srchpc_sys_reselect_check( boolean send_rpt );

#ifdef FEATURE_PLT
#error code not present
#endif /* FEATURE_PLT */

/*===========================================================================

FUNCTION SRCH_ZZ_CTL_INIT

DESCRIPTION    This function initializes variables/state machine etc. before
               SRCH_ZZ_STATE is entered.
DEPENDENCIES   None

RETURN VALUE   SRCH_DONE_S
SIDE EFFECTS   None

===========================================================================*/
extern srch_cmd_status_type  srch_zz_ctl_init
(
  /* Specifies current state from which this function is being invoked. */
  srch_state_enum_type  cur_state,

  /* Pointer to command which is causing us to enter this state. */
  srch_sleep_cmd_type  *cmd_ptr
);

/*===========================================================================

FUNCTION SRCHZZ_CTL_STATE

DESCRIPTION    This function contains the main loop for the SRCH_ZZ_STATE.
DEPENDENCIES   None

RETURN VALUE   The new state of the searcher task:  CDMA or PC.
SIDE EFFECTS   None

===========================================================================*/
extern srch_state_enum_type srch_zz_ctl_state(void);

/*===========================================================================

FUNCTION SRCH_FILT_IIR

DESCRIPTION    Gradually grow filter until max filter length, then let it be
               a fixed length filter. This lets the filter quickly ramp and
               be equivalent to the normal IIR filter.  Growing filter has a
               straight linear averaging characteristic while the fixed filter
               has the normal (n-1)/n characteristic

DEPENDENCIES   None

RETURN VALUE   New filtered value

SIDE EFFECTS   None

===========================================================================*/
extern uint32 srch_filt_iir
(
  uint32* filter,  /* Filter */
  uint32 input,    /* Value to filter in */
  uint32 len,      /* Length of filter */
  uint32 max       /* Maximum filter length, if non-zero then filter length
                      is grown but will not exeed this value */
);

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

RETURN VALUE    A 6-bit RSSI index, 0..63,  representing RSSI from
                -75 dBm to -106.5 dBm, in 0.5 dBm steps.

SIDE EFFECTS    Filtered database CDMA_RSSI is updated.

===========================================================================*/
extern byte  srch_get_cdma_rssi ( void );

/*===========================================================================

FUNCTION SRCH_NOTIFY_FPC_MODE

DESCRIPTION    This function tells search that FPC is enabled.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_notify_fpc_mode
(
  boolean fpc_enabled,
    /* FPC on-off status */

  uint16 mode
    /* Mode passed as a word for logging */
);

/*===========================================================================

FUNCTION SRCH_UPDATE_RC

DESCRIPTION    This function updates the srch's local copy of the FCH RC
      value which is necessary for logging.  This function is called from
      the MUX layer where the RC value is updated.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_update_rc
(
  deint_rc_type  fch_rc_val,
    /* Fundamental channel RC value */

  deint_rc_type  sch0_rc_val,
    /* Supplemental channel 0 RC value */

  deint_rc_type  sch1_rc_val,
    /* Supplemental channel 1 RC value */

  deint_rc_type  dcch_rc_val
   /* Dedicated Control channel RC value */
);

/*===========================================================================

FUNCTION SRCH_LOG_AGC_PCTL

DESCRIPTION    This function is invoked to log AGC and power control values
      for DIAG.

DEPENDENCIES   srch_agc_pctl_vars

RETURN VALUE   None.

SIDE EFFECTS   Messages can be sent to DIAG task.

===========================================================================*/
extern void  srch_log_agc_pctl ( void );

/*===========================================================================

FUNCTION SRCH_START_PC_STATE

DESCRIPTION    This function returns TRUE the first time it is called
      after Search enters PC state.

DEPENDENCIES   srch_state.

RETURN VALUE   boolean indicating if this is first call (TRUE).

SIDE EFFECTS   none.

===========================================================================*/
extern boolean srch_start_pc_state
(
  boolean reset_only
);

#ifdef FEATURE_SRCH_VOIP_1X_HANDOFF
#error code not present
#endif /* FEATURE_SRCH_VOIP_1X_HANDOFF */

#endif /* SRCHI_H */


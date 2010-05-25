/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     M A I N   C O N T R O L   T A S K

GENERAL DESCRIPTION
  This module contains the initialization, startup and shutdown processing
  provided by the MC task.  This task initializes the complete DMSS software
  system and then it calls system determination.  System dfetermination in
  turn calls the analog and digital call processing subtasks.  Control
  returns to this module from system determination when powerdown or
  reset are to be done.

EXTERNALIZED FUNCTIONS
  mc_task
    Entrance procedure to the Main Control task.
  mc_cmd
    Procedure to send a command to Main Control from other tasks
  mc_task_start
    Procedure for tasks to acknowledge start up from MC.
  mc_task_ack
    Procedure for task to acknowledge from MC.
  mc_task_stop
    Procedure for task to do task stop processing from MC.
  mc_task_offline
    Procedure for task to do task offline processing from MC.
  mc_cdma_activate_protocol
    Command the MC task to activate the CDMA protocol.
  mc_cdma_deactivate_protocol
    Command the MC task to deactivate the CDMA protocol.
  mc_cdma_ph_status_chgd_cmd
    Inform the CDMA protocol of the phone status change.
  mc_cdma_generic_prot_cmd
    Command the CDMA protocol to process the generic command.
  mc_amps_activate_protocol
    Command the MC task to activate the AMPS protocol.
  mc_amps_deactivate_protocol
    Command the MC task to deactivate the AMPS protocol.
  mc_amps_ph_status_chgd_cmd
    Inform the AMPS protocol of the phone status change.
  mc_amps_generic_prot_cmd
    Command the AMPS protocol to process the generic command.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  This task is the first task created in the DMSS.  It is created from
  the main module (using the REX task initialization service) after boot
  processing is completed.  No other tasks or drivers have been created
  or started when this task is started.

Copyright (c) 1990-2005 by QUALCOMM Incorporated. All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mc.c_v   1.53   08 Oct 2002 12:21:26   hexinw  $
  $Header: //source/qcom/qct/modem/1x/cp/rel/1h08/src/mc.c#1 $ $DateTime: 2009/05/29 03:54:56 $ $Author: prabhuk $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/05/09   pk      Removing function calls of rfm_init_bbrx_sleep
08/04/08   bb      Code changes to move Service Option initialization after MC task has 
                   started for 8K targets 
06/09/08   ag      Fixed some watermarking issues.
05/14/08   ag      Watermarking for trans_queue.
01/08/08   ag      Mainlined FEATURE_LPM
10/31/07   pg      Set deact_reason_for_sleep to LPM mode for power save mode.
08/10/07   yll     If the deactivation reason is for LPM, don't leave CDMA
                   sleep mode so that MS can go to sleep.
08/03/07   pg      Added support for extended REXL4 signal under 
                   FEATURE_REX_SIGS_EXT.
05/11/07   pg      More Lint Cleanup.
02/23/07   sb      Added support for new exit reason.
02/15/07   sb      Update last_entry for VOIP-1X handoffs.
01/30/07   sb      Fixed compilation error in mc_get_csna_message_type.
01/15/07   sb      VOIP-1X handoff updates.
11/21/06   sb      Added support for VOIP-1X handoffs.
10/25/06   fc      Changes to not use entry reason to indicate TC to Idle
                   transition.
09/21/06   fh      AKA/AES: added a new access state 
                     MCC_ACCESS_STOPPED_MI_AES_RESYNC -- access stopped for 
                     AKA/AES crypto-sync procedure
08/01/06   fc      Merged the following from MSMSHARED:
08/01/06   an      Made klokwork fixes.
07/10/06   fc      Changed mcc_set_digital_acquistion prototype.
07/06/06   fc      Renamed MCC_LOST_RF_RESOURCE to MCC_SRCH_REQUEST_SYS_RESTART.
06/23/06   pg      Added support for RPC.
06/02/06   fc      Cleanup umts interface.
05/19/06   an      Added check to detect if MC is waiting for the search task
04/26/06   an      Fixed Lint Errors
03/03/06   fc      Changed to print debug message instead of error fatal if
                   mmoc sanity timer expired in system access or traffic 
                   state.
01/20/06   fc      Merged the following:
           fh      Added support for AKA and MI.
10/10/05   fh      Removed including of enc.h, and deint.h
10/03/05   an      Fixed lint errors
08/25/05   fc      Changed mc_cdma_prot_activate to send SRCH_CDMA_F only if
                   activate reason is not FTM.
07/15/05   fc      Fixed lint errors.
07/11/05   fh      Cleaned up header file including from MUX:
                     added including of rxcmc.h
05/11/05   fc      Moved tdso_init_so_cfg to mc_init_after_task_start.
04/25/05   fc      Merged the following from MSMSHARED:
04/20/05   fh      In function mc_init_ran(), set the random seed to be 1 when
                    ESN is 0.
04/19/05   fc      Merged the following from MSMSHARED:
04/13/05   an       Mainlined FEATURE_SRCH_INACTIVE_STATE
03/24/05   an       Replaced ds707.h by ds707_extif.h
03/28/05   fc      Added support for DCA.
03/23/05   fc      Merged the following change from MSMSHARED:
03/22/05   an       Mainlined FEATURE_ENHANCED_BAND_CLASS. Eliminated 
                    FEATURE_CDMA_800 and FEATURE_CDMA_1900
03/11/05   fc      Merged the following change from MSMSHARED:
03/10/05   fh       Inside the function mc_cdma_prot_activate, changed to pass
                    PROT_STATE_ONLINE_CDMA or PROT_STATE_ONLINE_DED_MEAS to the
                    function call of mmoc_rpt_prot_auto_deactd_ind according to
                    different act_reasons
03/01/05   fc      Merged the following change from MSMSHARED:
           ka      Subscription info is only reloaded if CM informed there is
                   a subscription change.
02/18/05   fc      Merged the following from MSMSHARED:
02/09/05   fh       changed including of ds.h to be ds707.h, as data team requested
02/08/05   an       Mainlined IS95B features
02/03/05   fc      Fixed lint error.
01/07/05   an      Removed feature FEATURE_SMS and FEATURE_UASMS
12/10/04   sb      Fixed lint errors.
11/04/04   fc      Fixed lint errors.
10/29/04   pg      If 1X subscription is not available, use default values for 
                   subscription items.
10/20/04   an      Replaced inclusion of srch.h with srchmc.h
09/30/04   sb      Fixed lint warning.
09/27/04   va      Merged the following from RELB/C
08/11/04   dna      Get RF lock for mc_meas_ded_state, for MSM6500 4.0P4 only.
08/03/04   ph       Featurized access of NV_SLOT_CYCLE_INDEX_I to the exclusion
                    of FEATURE_UIM_RUIM_SUPPORT_SCI
07/28/04   fc       Fixed lint errors.
07/21/04   sj       Added support for umts to 1x handover.
07/12/04   pg       Perform registration only if subscrition (RUIM card is 
                    inserted for RUIM only build) is available.
06/30/04   dna      Changes for FEATURE_SRCH_INACTIVE_STATE.
                    This feature changes the MC<->SRCH interface for activating
                    and deactivating SRCH, and also delays when
                    mmoc_rpt_prot_auto_deactd_ind() is called until after
                    SRCH has really released the RF and other resources.
08/17/04   az      Release D Negative SCI changes
08/12/04   va      Removed unneccesary include files.
08/09/04   fc      Service negotiation changes for Release C and D.
06/03/04   bkm     Merged the following from IS2000_REL_B_C:
  05/26/04   fc      Included mccsyncid.
05/28/04   bkm     Merged the following from IS2000_REL_B_C
  05/17/04   va      Merged the following from MSMSHARED:
    04/20/04   dyc      Updates to support removal of DFM VU dependency.
    03/11/04   yll      Support the KDDI requirement to ignore async data and 
                        async fax 1x pages during HDR session.
    03/05/04   bkm     Lint cleanup.
03/26/04   bkm     Merged the following from IS2000_REL_B_C
  03/12/04   fc      Added support for Release C fast call setup.
  03/04/04   az      Added support for new transaction engine
  02/27/04   yll/br  Support GPS Out of Service operation, notify Search that
                     MC enters AMPS.
02/10/04   az      Added a funtion call to initialize action handlers for the
                   new transaction engine.
01/08/04   yll     Added the DF init in mc_init_after_task_start(), after the
                   MOB_CAI_REV is init'ed.
01/07/04   ph      Removed spurious #endif
01/05/04   pg      Mainlined FEATURE_MULTIMODE_ARCH.
11/13/03   yll     Moved the resetting of cdma.call_orig outside the function
                   mcc_send_orig_failure().
11/12/03   sj      Added the missing exit reason in mc_online_digital()
                   MCC_ACCESS_FAILED_SYS_LOST.
11/06/03   yll     Enabling the RF receiver chain in mc_meas_ded_state().
11/03/03   yll     Dynamic P_REV: default the P_REV to Rel 0 in Rel A build.
10/23/03   yll     Fixed the problem in non multimode architecture in 
                   mc_meas_ded_state() that a command is left in mc_cmd_q 
                   while not processing Search's meas report.
10/22/03   is      Includes pdsmtask.h if FEATURE_PDAPI enabled.
10/21/03   yll     mc_get_rf_items_from_nv() no longer exists when FEATURE_
                   MULTI_RX_CHAIN is defined.
10/07/03   yll     When MC doesn't responde to MMoC commands, check if MC 
                   exclusively waits on Search's wakeup, let Search know 
                   it's stuck.
09/11/03   yll     Added the featurization for ACP include files.
08/27/03   sj      Changed mc_process_cmd() to buffer unprocessed commands
                   until safe limit is reached.
08/20/03   yll     Changed FEATURE_HDR_HYBRID to FEATURE_HDR.
08/12/03   sj      Updates for placing srch in standby only in power save
                   and lpm. 1x protocol hw init moved out of mc.
08/11/03   bkm     Removed support for FEATURE_SPECIAL_MDR.
08/11/03   az      Added support for FEATURE_MODIFIED_SCI
08/01/03   dyc     Added support for FEATURE_MULTI_RX_CHAIN
07/31/03   yll     Removed TEMP_MC from HDR related feature.
07/30/03   yll     Merged in the following changes:
           dna     Added support for HDR.
07/21/03   sj      Place RF in 1x_mode before activating CDMA or AMPS.
07/01/03   az      Added FEATURE_ENHANCED_BAND_CLASS
06/02/03   bkm     Lint cleanup.
06/05/03   wd      Added case to handle PROT_OPRT_MODE_FTM in
                   mc_process_cdma_ph_stat_cmd.
06/05/03   va      Added trex initialization under the MM architecture path
                   too
04/15/03   jqi     Revised the change for MMRF API for non runtime switching
                   target. (cougar target)
                   Runtime switching target (Leopard)- RF initialization
                   whenever entering CDMA mode.
                   None Runtime switching target (Cougar)- RF initialization
                   only did once during power up.
03/28/03   va      Added trex_init_blocks call to the MM path as well.
03/04/03   atien   Changes to resolve naming conflict with MMGPS defined types
                   in mmgps_types.h
                   1. Replace nf.h, cd.h (pe.lib's internal include files) with pe_task.h
                   2. Replace pb.h with pb_task.h
03/05/03   jqi     Added MMRF API support with Multimode arch.
                   rf_init_delayed() will be called for all multimode target.
02/13/03   jqi     Remove set ftm_mode to DMSS_MODE in mc_init_before_task_start().
01/27/03   sj      Removed err_fatal while handling pref_sys & nam_sel in
                   offline states.
                   When subscripton load from NV/RUIM fails for CDMA or AMPS,
                   transition to offline state only if the oprt_mode is not
                   currently offline.
01/20/03   jqi     Added rf_init_bbrx_sleep() after rf_init_delayed() for MSM6300
                   Target.
                   Updated rf_init_bbrx_sleep() API in mc_online_digital.
11/19/02   ro/bkm  Added initialization for 'leds' for voc, graph, vs, fs tasks.
11/05/02   va      Added support for dynamic p_rev change.
11/04/02   bkm     Corrected featurization around MC_SILENT_REDIAL_F in
                   mc_gps_state_process_mc_command().
10/31/02   az      Added support for HDR Data dedicated transmission mode
                   access prevention.
10/14/02   sb      Merged in changes from MSM6100.
10/08/02   hxw     Changed FEATURE_GPSONE_MSBASED to FEATURE_GPSONE_OOS.
10/07/02   ph      Moved mccreg_init() to after task initialization.
10/04/02   sj      Updates from Multimode arch. FTM & PLT testing.
10/04/02   lh      Lint cleanup, 2nd round.
10/01/02   az      Mainlined FEATURE_IS2000, FEATURE_COMPLETE_SNM, T_MSM2P
09/30/02   sb      Mainlined FEATURE_SD20.
09/26/02   fc      Added exit reason MCC_ACCESS_FAILED_GSRM.
09/19/02   lh      Lint clean up.
09/12/02   sj      Updates from Multimode FTM testing.
09/04/02   sj      Featurized AMPS handling under Multimode architecture.
08/19/02   jqi     Undefined mc_start_xxxx_tasks() when mm arch is defined.
08/12/02   sj      Fixed merge errors from multimode architecture changes.
08/07/02   jqi     Removed a tempoary feature and redundant code.
08/05/02   HQ      Added FEATURE_CDSMS.
07/29/02   jqi     Undefine mc_get_esn_me() when multimode arch is defined.
07/25/02   jqi     Replace FEATURE_TMC_TASK with FEATURE_MULTIMODE_ARCH
07/23/02   sj      Moved SRCH to standby if CDMA/AMPS protocol is not active.
07/18/02   jqi     Undefine mc_init_uim_items() with multimode architecture.
07/08/02   az      Added FEATURE_DBM encapsulating Short Data Burst and
                   Position Determination
07/02/02   jqi     Remove support in mc_device_init
07/01/02   yll     Added suupport for MC Queue Watermark
06/21/02   ph      changed cmmc.h to cmxll.h
06/07/02   sj      Added CP support for multimode architecture.
06/06/02   jqi     Added support for FEATURE_MULTIMODE.
                   1.Added support in mc_device_init()
                   2.Removed HW initialization and task opertion such as creat,
                     start, stop and offline from mc_task to tmc_task.
                   3.Added mc_init(), mc_init_before_task_start(),
                     mc_init_after_task_start() function
                   Added task operation functions,such as mc_task_ack(),
                   mc_task_stop(), mc_task_offline() if TMC task is not defined.
05/23/02   ht      Added chg_sleep_init() under FEATURE_BATTERY_CHARGER.
05/06/02   yll     Changed Broadcast SMS function names.
04/17/02   cah     Added OFFLINE notification for NF and CD tasks.
04/16/02   dd      Feature-ized vectorOne device dependent code.
04/09/02   yll     Changed handling of MC_BCSMS_ENABLE_F to a function call.
04/02/02   dna     Removed "HDR Hybrid" bit from phone state (for logging)
04/02/02   hxw     Changed CM_GPS_MODE_ABORT_F to CM_GPS_MODE_EXIT_F.
03/08/02   hxw     Added FEATURE_GPSONE_MSBASED to support MSBased gpsOne.
03/22/02   fc      Changes to use static buffer to store selected system info.
03/20/02   ht      Removed direct call to sbi driver and replace it with call to rf zif lib.
03/18/02   ht      Added code to support FEATURE_SOFTWARE_PROFILE.
03/14/02   sh      Added processing for MC_ONLINE_ANALOG_F
03/14/02   ht      Featurized I2C driver support for MSM6050.
03/09/02   st      Added support for the Graphics task and I2C driver.
03/11/02   ht      Corrected the target name T_MSM6000 and T_MSM6050.
03/11/02   fc      Changes to make mode controller back to offline state if
                   nam selection failed.
03/09/02   lh      Merged from CP119.
  03/04/02   ht      Include clkrgm_5500.h to fix compiler warnings.
  03/04/02   dna     Fixed mc_state() for HDR hybrid operation.
  03/03/02   sh      Removed MC_STOP_FWD_PREF_SYS_CMD_F
  02/18/02   abh     Modified the demod download function prototype to
                     selectively enable the firmware download verification
                     using test bus.
  02/12/02   abh     Updates in the functionality to perform GPS operation
                     while MS is in Acquisition State (for performing
                     multi-visit operation).
  02/13/02   ts      Removed the initialization of the UIM queues from within the
                    FEATURE_AUTH ifdef.
  02/11/02   mpa     Using cm_get_cur_hybr_status instead of sdss_is_hybrid.
  02/08/02   sq      Externed function Error_Fatal for MSM5500.
03/06/02   jrp     Fixed merge error.
02/13/02   jrp     Merged the following from common archive:
                   02/07/02   sh      Added srch support for HDR only mode
                   02/06/02   abh     Added code to go into GPS state when Phone is in Acquisition state.
                   02/06/02   ssw     Changed enabling SBI to account for ZIF
                   02/06/02   sh      Added hybird operation support
                   01/16/02   ak      Added FEATURE_VECTORONE
                   01/15/02   ht      Added ONC Socket Test Applications task and
                                      ONC RPC SERVICES task under FEATURE_ONCRPC.
                   01/12/02   sst     Modified to use the new srch_* files that replaced srchdrv.c
                                        Includes a function name change:
                                          srch_dmod_reset -> srch_util_dmod_reset
                   01/07/02   ht      Merged changes for MSM6000 and MSM6050.(clang)
                                      Added support for the PM60X0 battery charger code.(rmd)
                   12/06/01   sq      The MSM5500 software will not support old MSM REV before 1.3
                   12/02/01   dna     Merged in changes from a branch being used for MSM5500
                                      development.  The only non-featurized change is moving
                                      mc_init_ran().
                              sh      Added processings for HDRMC_EXIT_RESET
                              dna     Move mc_int_ran() call to before we start HDR app. tasks.
                              st      Merge HDR changes to mc_system_offline, mc_system_powedown
                                      and mc_online_hdr_state.
                   11/13/01   sq      Switch MCLK after demod download
01/09/01   va      Removed FEATURE_IS2000_REL_A_SVD from around cmtask.h
                   include.
12/17/01   va      Conditionally include cmtask.h
12/06/01   lh      Merged from MSM_CP.05.00.108.
  11/13/01   lh      Changed ACP_OFFLINE_ANALOG to MC_OFFLINE_ANALOG_STATE in
                     mc_analog_offline_state().
10/02/01   jrp     Merge from MSM_CP.05.00.96
08/21/01   fc      Added support for exit reasons MCC_ENCRYPTION_FAILED and
                   MCC_ACCT_BLOCKED.
08/16/01   kk      Merge from MSM_CP.05.00.89.
                   08/07/01   fc      Added #ifdef FEATURE_ACP in
                   mc_online_digital_state (SD2.0 only).
08/08/01   yll     Merged the following from the MSM archive:
                   Include timetest.h when TIMETEST is defined.
                   Moved the STORE_ESN_ME command to UI after CHV is verified
                   and added an interface to export the esn in cdma structure.
                   Changed TIMETEST ID's to #def's (from timetest.h)
07/06/01   ph      Added support for Callback Timers
06/26/01   ph      Merged in from MSM_CP.05.00.81
                   06/18/01   ck      Added offline signalling to UIM task.
                   06/13/01   vlc     Added an #ifndef FEATURE_PLT around mcc_subtask calls as
                                      this function is not visible in PLT builds.
                   06/11/01   fc      Added mc_update_last_cdma_entry to support offline for
                                      non-MC (SD20 only).
                   06/11/01   rp      Added TIMETEST support for USB.
                   06/08/01   ck      Added support to vote on the power control of UIM.
                                      Flushed NVRUIM cache before reading items from NV on a NAM
                                      change/ OTASP commit when using SD2.0.
                   06/06/01   bkm     Added TIMETEST support for Bluetooth.
                   05/31/01   ht      (JH) Removed the "static" declaration to save RAM.
                   05/26/01   ry      Added new Bluetooth LPOS task for FEATURE_BT_LPOS
                   05/25/01   ts      Added new UIM server task and API to new UIM server.
                   05/23/01   dna     Fix HDR_OFFLINE mode so we can write NV items while in HDR
05/17/01   ph      Merged in from MSM_CP.05.00.76
                   05/02/01   kss     Include hdrmdspmcr.h.
                   05/01/01   dna     HDRMC_START_HDR_SIG -> TASK_START_SIG
                   04/12/01   dna     Merged in support for HDR
                   04/12/01   fc      Moved events logging data structures/functions to mclog.c.
                                      Added offline signalling to TDSO task.
                   04/10/01   fc      Added support of FTM and PLT for system determination 2.0.
03/27/01   kk      Merged mainline changes
                   03/08/01   va      Merged in some fixed for FEATURE_PLT
                   03/06/01   fc      Added support of events report: Operation mode change.02/16/01   fc      Added support for system determination 2.0. All changes
                   are featurized under FEATURE_SD20.
02/16/01   bgc     Changed FEATURE_FACTORY_TESTMODE to be a runtime mode.
02/08/01   ks      Added TDSO task initialization.
02/05/01   yll     Added Security Services Task.
01/23/01   cah     Rid T_SLEEP define checks and mainline code.
01/19/01   va      Removed clk_regime_set1_enable call for MSM5100.
01/11/01   dwp     Added support for FEATURE_RUNTIME_DEVMAP (RDM).
01/11/01   va      Added support for MSM5100.
12/07/00   mai     Changed feature name for PDSMSCLIENT and removed extraneous
                   #endif.
12/04/00   yll     Added support for task PDSMSCLIENT for gpsOne.
11/15/00   ck      Changed the AUTH_RAND_SEED_F command to AUTH_BS_CHAL_F cmd.
                   Added the function mc_queue_auth_rpt to handle reports from
                   Auth task.
                   Changed the number of Auth Rpt buffers.
                   Added support for STORE_ESN_ME command in the function
                   mc_init_ruim_items().
11/08/00   ts      Added the function mc_init_ruim_items() right after starting the
                   AUTH and NV tasks in mc_start_service_tasks() to avoid deadlock.
11/16/00   bgc     Updated support for PLT and FTM.
11/02/00   lcc     Support for FEATURE_COMPLETE_SNM: initialization of SOs.
10/27/00   hrk     Using a generic function name - msm_device_init() to make
                   MSM specific initializations.
10/18/00   jkl     Moved SND and VS from application to service tasks.
10/13/00   bgc     Added HS task for FTM.
09/20/00   jq      Cleaned all lint errors.
09/14/00   ry      Removed the Bluetooth Audio Gateway Demo task
09/11/00   sr      Added call to vocoder instruction memory test under
                   FEATURE_VOC_IRAM_TEST
09/08/00   ht      Change the Bluetooth task shutdown order.
09/07/00   ry      Added Bluetooth to start_service_tasks and system_powerdown
09/06/00   jq      Implemented FEATURE_FACTORY_TESTMODE based on FEATURE_PLT
09/06/00   ry      Renamed FEATURE_BT_AG_HS_DEMO to FEATURE_BT_HS
08/29/00   yll/dlb Added SFAT task for MMC support.
08/23/00   ry      Renamed BT_TEST_RC_2_SIO to FEATURE_BT_TEST_RC_2_SIO
08/22/00   ro      Added QDSP Services Task and TASK_STOP to Vocoder task.
08/21/00   ry      Added Bluetooth support
08/02/00   yll     Added sending TASK_STOP_SIG for PDSM task.
08/01/00   va      Added support for initializing boothw clk callback.
07/10/00   ks      Added support for Simple TDSO, Calling tdso_int_so_cfg()
07/05/00   ts      Moved the ruim_dev_init call past the msm5000_dev_init call.
06/19/00   yll     Added Support for Creating a new task "PDSM" under
                   FEATURE_GPSONE.
06/22/00   va      Support for caching the pref RC NV item.
06/14/00   ks      Moved the Ifdef checking for FEATURE_PLT so that CM task is
                   used when PLT is defined.
05/26/00   ck      Added the issuing of the command AUTH_RAND_SEED_F to indi-
                   cate to RUIM that the pseudo random number generator has
                   been seeded in the function mc_init_ran().
                   AUTH task is started and powered down if FEATURE_AUTH or
                   FEATURE_RUIM is defined or both.
                   AUTH task is started right after EFS and before NV task in
                   the start_service_tasks routine if FEATURE_RUIM is defined.
                   If RUIM is not defined, the sequence of tasks start up
                   remains the same and AUTH task is started as an application
                   task.
04/24/00   cah     Added USBDC support (rp).  TLMM support. (rp/jc)
                                   Moved EFS task start to end.  (rp)
04/11/00   jwh     Moved mc_sdevmap_from_nv() to mc_start_service_tasks()
                   as it needs to be called AFTER nv task is running, but
                   before diag or ds tasks are started.
04/09/00   jwh     Added mc_sdevmap_from_nv() to support
                   FEATURE_SDEVMAP_MENU_ITEM_NV.
03/12/00   ry      Added the support of non-MSM5000 target
02/03/00   lcc     Removed inclusion of mccap.h.
01/26/00   va      Merged changes from GV40401
11/10/99   kmp     Removed the #ifdef FEATURE_DS around dsm_init() in
                   mc_device_init().
11/04/99   kmp     Merged in the 3 changes listed below:
11/03/99   mk      Not using msm3000_2300_device_init() initialization code
                   for MSM3100.
11/02/99   mk      Moved misc MSM3100 pin initialization.
11/01/99   jc      Added PM1000 charger support.
01/14/99   lcc     Cleaned up TEMP_HACKS_FOR_TIGER
01/10/00   bgc     Added initialization of call manager task for PLT.
12/20/99   va      Removed mccap_initialize
11/15/99   va      IS2000 voice changes.
                   Added support for initializing the new mccap module
11/03/99   lcc     Merged from GD4021:
10/29/99   fc      Vocoder driver feature clean up. FEATURE_VOC_TASK replaced
                   FEATURE_QDSP2 and T_MSM31 replaced
                   FEATURE_QDSP_INTERNAL_CODEC.
09/22/99   lh      Merged following changes.
           lh      Merged PM1000 support. Code clean up for Panther
                   compatibility.
           lh      SMS and CM phase 2 implementation.
           mk      Added more FEATURE_QDSP2 and FEATURE_QDSP_INTERNAL_CODEC
                   support. Deleted TCXODIV2 support.
           jc      Removed support for MSM3100 GPIOs.
           jc/mk   Added support for MSM3100 GPIOs, deleted TCXODIV2 PLL,
                   deleted ITF3000 init, added TLMM init.
           spf     Pink Panther DFM support.  Fixed so acpwb_init() is called.
           spf     - Added 10/20MHz TCXO clock featurization.
                   - Added FEATURE_QDSP2 featurization to support new vocoder
                   interface.
                   - Added Vocoder task support.
           tn      Removed SBI related code, sbi_init is moved to rf_init.
           mk      Added initial (temporary) MSM3100 support.
           vl      Merged support for VS task and EFS task.
           lh      If def changes.
10/28/99   lcc     Corrected #ifdef around SBI (TG!=T_O instead of
                   !defined(T_O)).
10/28/99   lcc     Moved PLT related code to plt_mc.c.
10/07/99   lcc     Merged from PLT 2.0:
           jcw     Removed SIMULATED_RF since it isn't needed
           hrk     Added debug code for FFPC.
09/15/99   lcc     Merged in PLT support from PLT archive:
                   1. Merged in the latest changes for RPC via mini-DM
                   2. Enabled DEC Clock Regime for TIGER2 & call deint_init.
                   3. Merged in PLT changes
                   4. Conditionally compiled out the code that starts UI task
                      for PLT.
                   5. Added RPC task
07/07/99   vl      Merged support for VS task and EFS task.
08/12/99   nmn     Merged from SPSW: Added checking for external power during
                   powering down when in LPM mode.
08/11/99   ak      Added entry points for sockets udp test code, and fixed
                   up #defines for sockets test apps.
08/10/99   rp      Moved start of Embedded File System Task before NV Task
                   start, moved stop of Embedded File System Task after NV
                   Task stop.
07/30/99   lcc     Added some comments around snm_initialize() as a result of
                   code review.  Also removed some unit test code that was left
                   there unintentionally.
07/28/99   lh      CM and two-way SMS support added.
05/06/99   kmp     Merge IS-95B changes below & change T_AUTH & T_ACP with
                   FEATURE_AUTH and FEATURE_ACP respectively
           lh      Function mc_get_band_class, mc_get_p_rev_in_use added for
                   run-time P_REV and BC checking.
03/30/99   ak      Added entry point for sockets test code.
02/12/99   yll     Fix CR8381. call to ran_seed() to ensure that the
                   randomization routines work properly.
02/12/99   nmn     Moved ERR_INIT sooner to allow for logging of startup errors
02/01/99   br      RAM saving changes.
01/22/99   ht      Add structure for Embedded File System Task(for LSpall).
12/09/98   ro      call voc_manager_init and don't call mcc_voc_selftest
                    if FEATURE_VOCODER_MANAGER is defined
                    call voc_adsp_evrc_init if FEATURE_EVRC_ADSP is defined.
12/02/98   br      Keyword 'const' inserted in front of the ROM macro.

03/10/99   ram     Moved the MDR DPT init function to be after the
                   service tasks are started, since this function
                   is now using message services. This applies only to MDR.
02/17/99   ychan   Modified type of variable nv_item_list in function
                   mc_get_rf_items_from_nv as const nv_items_enum_type*.
02/12/99   lcc     Added initialization of CCL and mccsyobj objects.
02/12/99   nmn     Moved err_init earlier to allow for loging of task start up.
01/15/98   ak      Integrated back in old SIO code, under compile time switch.
01/13/99   lcc     Merged in MDR support:
                   - Added function call to initilize MDR DPT Service Options
                     Required for MDR DPT.
                   - Added service negotiation manager (used in MDR) support and
                     merged in some runtime P_REV/band class support functions.
01/12/98   ak      Made changes to support new SIO architecture.  Change to
                   mc_device_init - sio & dm initialization much simpler now.
12/14/98   dna     Didn't merge T_Q mods because using split RF in Panther 3.0
11/23/98   br      Inlude ma.h only if T_AUTO_ICU is defined (this is 186
                   specific feature and doesn't have to be included for ARM).
11/05/98   abh     Serial Bus Interface Initialisation for SURF  & defaulting
                   MSM3000 in MSM2300 mode.
10/07/98   ck      Changed the order of powering down of DH task to maintain
                   the reverse of task startup.
09/04/98   pms     Neared the variables to save ROM space.
08/31/98   pms     Added Tan's Pascal calling convention code for ROM savings
08/25/98   ck      Added DH support for builds with FEATURE_DH defined
08/10/98   bns     Renamed sd_init() to bd_init() and changed to
                   FEATURE_BORSCHT to use generic BORSCHT driver
08/06/98   ces     Add QMIP task support for builds with FEATURE_QMIP defined.
08/04/98   ych     Merged E911, SSPR-800, and Acq. Improvements
07/30/98   lcc     Merged in ARM support.
04/16/98   aks     Made definitions for vbatt, therm, rf and hw configurations
                   valid for all targets EXCEPT for Unit Test (T_PC).
02/04/98   thh     Move Q's RF interface to the new RF architecture.
                   #ifdef out FM RF NV items with FEATURE_ACP for NGP.
02/03/98   kss     Used the new RF architecture for Module.
01/29/98   sk      Added code to take phone off-line if it is not a Qualcomm
                   phone.
01/28/98   ldg     Removed the read of NV_DATA_SRVC_STATE at startup (DS).
01/28/98   ldg     Removed the DS_SLIM switch.
01/28/98   fkm     FEATURE_xxx Updates (via Automatic Scripts)
01/28/98   wjn     Added a UI-MC interface function mc_get_prl_version();
01/17/98   dhh     Added support for IDS task.
11/17/97   jjn     Used the new RF architecture for ISS2 as well as for TGP
10/31/97   thh     Define the new RF architecture for TGP only.  This allows
                   those who are on other targets to work with the latest
                   MC.C.
10/08/97   thh     Fixed the hanging #endif that was left by the previous
                   check-in.
10/08/97   smo/udp Added clkregim enables for MSM2.3 support.
09/26/97   thh     Changed RF interface to support new RF driver architecture.
07/02/97   jjn     Moved Module initialization code into mod_init() and moved
                   Module power-down code into mod_powerdown()
06/17/97   dhh     Added target T_Q wherever applicable for Q phone.
05/27/97   thh     Added a new function mc_cal_charger_from_nv() to help
                   the Charger Utility calibrate the VBATT circuit.  This is
                   currently activated for TGP only.
05/23/97   jjn     Added mod_record_event, which informs the System Unit
                   that a power-down has occurred.  Only applicable for
                   the Module
05/20/97   ych     Added new entry_action MC_LPM_F to system determination.
02/10/97   thh     Added call to initialize the TGP Charger.
01/16/97   jah     Moved HS task definition above
                   NV task to allow a shorter time before the LCD
                   initialization and the associated power-up signs of life.
                   Added MC_HAS_RF/HW/THERM/VBATT_CONFIG.  Added comments
01/14/97   bns     Added calls to initialize the ECFM and SinBaD drivers.
10/03/96   thh     Update the RF nv items table in mc_get_rf_items_from_nv().
                   Remove the obsolte NV_FM_LNA_CTL_I.  This shows up as
                   "00 1434 08 mc" in the error log.  Remove a duplicate
                   (1 of 2) NV_FM_EXP_HDET_VS_TEMP_I.
09/12/96    ro     Changed RF_CODEC_VOC to RF_CODEC_MC in mc_device_init()
                   to avoid potential problem if call to mcc_voc_selftest()
                   would be moved or removed.
05/24/96   jjw     Gemini & ISS2 Data Services release.
04/10/96   jah     Changed RF NV items list for new RF Compensation method
                   Fixed error message in HW_CONFIG read.
03/28/96   jah     Added MSM 2.2/2p ICU auto-detect/configure logic under
                   control of #ifdef T_AUTO_ICU.
03/27/96   rah     Removed T_G related conditional compiles for
                   mcc_voc_selftest call.
03/26/96   jca     Removed T_ACP conditional on call to acpwb_init().
02/08/96   ras     Added CSS support
11/28/95   jah     Added LOCAL void mc_cal_hw_from_nv( void )
11/08/95   jah     Fixed entry_action bug for all non-Gemini targets
11/01/95   jah     Changed rf_init_delayed() to return on/off-line status.
10/31/95   ras     added #if's for initial CSS build from ref. "spooky"
10/12/95   jah     Added new RF NV items, NV_FM_AGC_I and NV_FM_LNA_CTL_I, to
                   mc_get_rf_items_from_nv().
07/21/95   rdh     Made max SCI = 7 now.
07/14/95   jjw     Small changes for IS-99 data services.
06/19/95   rdh     Made NV_NOTACTIVE status handling less severe.
06/12/95   tst     Added NV_PA_RANGE_STEP_CAL, NV_LNA_RANGE_POL,
                   NV_LNA_RANGE_RISE, NV_LNA_RANGE_FALL,
                   and NV_LNA_RANGE_OFFSET to NGP and ISS2 RF calibration
                   tables.
                   Removed NV_CDMA_TX_COMP_VS_VOLT from ISS2 RF cal table.
06/07/95   dna     Added Authentication queues.
06/07/95   tst     Removed NV_CDMA_TX_LIM_VS_VOLT for ISS2 calibration.
05/23/95   jah     Removed THERM from Portable
05/23/95   tst     Modified Gemini RF NV items list to remove xxx_VS_VOLT.
                   Added code to mc_get_rf_items_from_nv() to call
                   rf_init_delayed() after NV items have been loaded for the
                   Gemini target only.
05/19/95   jah     Fixed dangling #elif in HDLC init support for iss1
05/19/95   ras     Added HDLC init support for iss1
05/17/95   jah     Modified mc_device_init for MSM 2p.  Removed therm
                   calibration from CD-7000, and added error case to per
                   target calibration table.
05/12/95   tst     Added latest NV items for RF cal.
05/06/95   tst     Added mc_cal_therm_from_nv() function.
                   Added call to mc_cal_therm_from_nv() to mc_task().
                   Modified mc_get_rf_items_from_nv() to use lookup table.
                   Added #include therm.h.
                   Modified several conditional compiles to include T_G.
04/06/95   jjw     Added and #ifdef DS around dsm_init()
04/05/95   jjw     Added changes to support PS task (for Data service
                   compile-time inclusion only)
10/24/94   ras     Added #LIFETEST define for factory mtbf testing.
08/09/94   jca     mcc_voc_selftest no longer returns status.
08/08/94   jjw     Fixed Portable w/DS compile-time error.
08/02/94   jca     Added T_ACP conditional compiles to support ISS1 release.
04/13/94   jjw     Added updated SIO service initialization to support Data
                   Services
01/06/94   jah     Changed watchdog iface to use dog_report() & dog_report_type.
12/28/93   jah     Added initialization for 'leds' for sleep task w/T_SLEEP.
12/01/93   jah     Changed mc_init_items_to_nv() to allow configuration of SCI
11/08/93   jjw     Changed NV_EXT_PORT_MODE_I to NV_DM_IO_MODE_I to conform
                   to new NV name.
08/05/93   jah/ptw Added init of sleep task.
05/17/93   jca     Added NV_FM_RSSI_I to list of RF NV items.
05/10/93   jca     Added code to load new RF calibration tables from NV.
04/13/93   jjw     Moved sio_init() from mc_device_init() to the mc_task to
                   initialize SIO services to the mode stored in NV.
02/23/93   jah/gb  Added TIMETEST code for task performance analysis using
                   an 8-bit buffered output.
02/61/93   jah/jca Added loading of EEPROM calibration tables for the
                   RF and Vbatt libraries.
01/20/93   jca     Added conditional call to acpwb_init for Portable version.
12/09/92    ip     Added call to initialize error services after
                   service tasks are started (after NV is started).
09/22/92   jah     Added enc_init() prior to rf_init() to allow the Encoder's
                   outputs to be initialized prior to being used by rf_init().
08/29/92    ip     Changed rex_wait to mc_wait; changed order so that
                   subtasks powerdown is called before DMSS powerdown
                   is started; changed to force offline if FALSE
                   status returned from subtasks powerup init.
08/22/92    ip     Fixes after code review and initial integration.
08/13/92    ip     Added configuration checksum temporary update.
08/10/92    ip     First release for integration.
04/30/92    ip     Initial porting from Brassboard to DMSS.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "target.h"
#include "customer.h"
#include "assert.h"

#include "rxcmc.h"
#include "mc.h"
#include "mci.h"
#include "sd.h"
#include "mccreg.h"
#include "mccsrch.h"

#if defined(FEATURE_IS2000_REL_C_FCS) || defined(FEATURE_IS2000_REL_D)
#include "mccsyncid.h"
#endif /* FEATURE_IS2000_REL_C_FCS || FEATURE_IS2000_REL_D */

#include "rex.h"
#include "queue.h"
#include "err.h"

#ifdef FEATURE_MULTIMODE_RFAPI
#include "rfm.h"
#else
#include "rf.h"
#endif

#if ((TG==T_Q) || (TG==T_G) || (TG==T_O))
#include "rfnv.h"
#endif

#include "db.h"
#include "msg.h"
#include "task.h"
#include "mcc.h"

#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */

#include "dog.h"
#include "nv.h"
#include "tx.h"
#include "rx.h"
#include "cmxll.h"
#include "cm.h"
#include "rxtx.h"
#include "srchmc.h"
#include "snm.h"
#include "diag.h"
#include "mobile.h"
#include "mccdma.h"
#include "mccbcsms.h"

#if !defined (_enable)
    void _enable( void );
#endif

#if defined(FEATURE_IS95B_MDR)
#include "mccscm.h"
#endif

#include "mccsyobj.h"
#include "mccccl.h"


#ifdef FEATURE_IS2000_TDSO
#include "tdso.h"
#endif

#ifdef FEATURE_FACTORY_TESTMODE
#include "test_mc.h"

#ifdef FEATURE_PLT
#error code not present
#endif /* FEATURE_PLT */

#endif /* FEATURE_FACTORY_TESTMODE */

#ifdef FEATURE_UIM
#include <memory.h>
#endif /*FEATURE_UIM */

#include "mccso.h"
#include "mccapdf.h"
#include "ran.h"

#ifdef FEATURE_VOC_IRAM_TEST
#error code not present
#endif

#include "event.h"

#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */

#ifdef FEATURE_MC_QUEUE_WATERMARK
#include "mccrx.h"
#endif /* FEATURE_MC_QUEUE_WATERMARK */

#include "mclog.h"

#ifdef TIMETEST
#include "timetest.h"
#endif

#include "mmoc.h"

#ifdef T_MSM5500
#error code not present
#endif

#include "tmc.h"
#include "mctrans.h"

#ifdef FEATURE_IS2000_REL_C_MI
#error code not present
#endif /* FEATURE_IS2000_REL_C_MI */

#ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
#error code not present
#endif /* FEATURE_UMTS_1X_HANDOVER_1XMSM */

#ifdef FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X
#error code not present
#endif

#ifdef FEATURE_REX_SIGS_EXT 
  #ifndef HAVE_REX_SIGS_EXT 
    #error "REX Signal API Extension is not avaibale. Check REX Version" 
  #endif /* HAVE_REX_SIGS_EXT */ 
#endif /* FEATURE_REX_SIGS_EXT */ 

/*lint -e818 */

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                         Data Definitions                                */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/* -------------------------------------------------------------------------
** The following status variable is used to save selected system info which
** is used to provide info to System Determination.
** ------------------------------------------------------------------------- */
LOCAL mc_online_type sel_sys_info;

/* ------------------------------------------------------------------------
** The following status variables are used to save entry reason to CDMA and
** Analog call processing for MC
** ------------------------------------------------------------------------ */
LOCAL mcc_entry_type cdma_entry;   /* CDMA entry reason   */
#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */

/* ------------------------------------------------------------------------
** The following status variable is used to save last entry reason to CDMA
** and Analog call processing which is used to provide info to Diag
** ------------------------------------------------------------------------ */
LOCAL mcc_status_type last_entry;

/* ------------------------------------------------------------------------
** The following variable is used to save the current state of the
** mode controller in static memory so it can be peeked for logging.
** ------------------------------------------------------------------------ */
LOCAL mc_state_type   mc_mode_controller_state;
                                 /*lint -esym(551,mc_mode_controller_state)*/

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                           Local Data                                    */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/* Data base buffer.                                                       */

LOCAL db_items_value_type  mc_db_item;

#ifdef FEATURE_MSM6300_SLEEP
#error code not present
#endif /*   #ifdef FEATURE_MSM6300_SLEEP */

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                              REX Data                                   */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/* Command queue and free queue.  Both must be GLOBAL (not LOCAL).  The    */
/* command queue is shared by the MCC and ACPMC subtasks.  The free queue  */
/* is accessed by external tasks.  The number of buffers in the free       */
/* queue is designed to anticipate the most numbers of commands which      */
/* could be queued onto to MC (and its subtasks) at any given time.        */

#define  MC_CMD_BUFS_NUM   6
LOCAL    mc_msg_type       mc_cmd_bufs[MC_CMD_BUFS_NUM];
         q_type            mc_cmd_free_q;
         q_type            mc_cmd_q;

#define MC_EXPIRED_TIMERS_MAX 10
LOCAL  mc_expired_timer_type mc_expired_timer_buf[MC_EXPIRED_TIMERS_MAX];
       q_type              mc_expired_timers_free_q;
       q_type              mc_expired_timers_q;

#if defined (FEATURE_AUTH) || defined(FEATURE_RUIM)
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Items for Authentication Task Report Interface                          */

q_type          mc_auth_q;
q_type          mc_auth_free_q;

/* Buffers to be placed on the mc_auth_free_q */
#define                               MC_NUM_AUTH_RPT_BUFS 3
LOCAL auth_rpt_type mc_auth_rpt_pool[ MC_NUM_AUTH_RPT_BUFS ];

#endif /* FEATURE_AUTH || FEATURE_RUIM*/

#ifdef FEATURE_UIM_RUIM
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Items for UIM Server Task Report Interface                          */

q_type          mc_uim_q;
q_type          mc_uim_free_q;

/* Buffers to be placed on the mc_uim_free_q */
#define                             MC_NUM_UIM_RPT_BUFS 6
LOCAL uim_rpt_type mc_uim_rpt_pool[ MC_NUM_UIM_RPT_BUFS ];

#endif /* FEATURE_UIM_RUIM */

#if (defined(FEATURE_DH) && defined(FEATURE_DH_EXP))
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Items for DH Task Report Interface                                      */

q_type          mc_dh_q;
q_type          mc_dh_free_q;

/* Buffers to be placed on the mc_dh_free_q */
#define                               MC_NUM_DH_RPT_BUFS 2
LOCAL dh_rpt_type mc_dh_rpt_pool[ MC_NUM_DH_RPT_BUFS ];

#endif /* FEATU  _       FEATURE_DH_EXP*/

/* Rex timer for kicking watchdog.  This timer is used by MCSYS so it is   */
/* public (regional).                                                      */

rex_timer_type    mc_rpt_timer;

#ifdef FFPC_DEBUG
#error code not present
#endif // FFPC_DEBUG

#ifdef FEATURE_MC_QUEUE_WATERMARK
/* Watermark masks when storing into NV. */
#define           MCWM_CMD_FREE_Q_MASK      0xF000
#define           MCWM_RXTX_FREE_Q_MASK     0x0F00
#define           MCWM_RX_FREE_Q_MASK       0x00F0
#define           MCWM_TRANS_FREE_Q_MASK    0x000F

#define           MCWM_CMD_FREE_Q_SHIFT_BITS      12
#define           MCWM_RXTX_FREE_Q_SHIFT_BITS     8
#define           MCWM_RX_FREE_Q_SHIFT_BITS       4
#define           MCWM_TRANS_FREE_Q_SHIFT_BITS    0

mc_watermark_type mc_wm[MCWM_MAX_NUM_WM];
#endif /* FEATURE_MC_QUEUE_WATERMARK */

#ifdef FEATURE_MODIFIED_SCI
#error code not present
#endif /* FEATURE_MODIFIED_SCI */

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                               Macros                                    */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/* Macro to kick the watchdog timer and set a timer to wake it again.      */

#define KICK_WATCHDOG() \
   dog_report(DOG_MC_RPT); \
   (void) rex_set_timer(&mc_rpt_timer, DOG_MC_RPT_TIME)

/* Macro to update the DMSS state in the database (db).                    */

#define SET_DB_DMSS_STATE(state) \
    mc_db_item.dmss_state = (state); \
    db_put(DB_DMSS_STATE, &mc_db_item)

/*===========================================================================

FUNCTION MC_WAIT

DESCRIPTION
  This procedure waits for specified signals to be set.  While it waits,
  this procedure continues to set the Watchdog report signal and reset
  the mc_rpt_timer.

DEPENDENCIES
  None.

RETURN VALUE
  Signal mask from signals that were set after rex_wait call.

SIDE EFFECTS
  None.

===========================================================================*/

LOCAL rex_sigs_type mc_wait
(
  rex_sigs_type   mask        /* Mask of signals to wait for */
)
{

  rex_sigs_type   sigs;       /* Signals returned by rex_get_sigs call */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Loop while waiting for signals specified in mask and   */
  /* for watchdog signal.  If watchdog signal then kick it  */
  /* and loop again until some other signal in mask is set. */

  while (((sigs = rex_get_sigs(rex_self())) & mask) == 0) {
    KICK_WATCHDOG();
    (void) rex_wait(mask | MC_RPT_TIMER_SIG );
  }

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* kick watchdog one last time and return the signals. */

  KICK_WATCHDOG();
  return(sigs);

} /* mc_wait */

/*===========================================================================

FUNCTION MC_TASK_INIT

DESCRIPTION
  This function performs MC task static initialization.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  After return from this funtion task timers can be used.

===========================================================================*/

LOCAL void mc_task_init (void)
{
  word    i;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Clear all signals */

  (void) rex_clr_sigs(&mc_tcb, (rex_sigs_type) ~0);

  /* Initialize timers. */

  rex_def_timer(&mc_rpt_timer,  &mc_tcb, MC_RPT_TIMER_SIG);

  /* Initialize queues. */

  (void) q_init(&mc_cmd_q);
  (void) q_init(&mc_cmd_free_q);

  /* Fill mc_cmd_free_q. */

  for (i = 0; i < MC_CMD_BUFS_NUM; i++) {
    mc_cmd_bufs[i].hdr.cmd_hdr.done_q_ptr = &mc_cmd_free_q;
    q_put(&mc_cmd_free_q,
          q_link(&mc_cmd_bufs[i], &mc_cmd_bufs[i].hdr.cmd_hdr.link));
  }

  /* Initialize Expired Timer Queues. */

  (void) q_init(&mc_expired_timers_q);
  (void) q_init(&mc_expired_timers_free_q);

  /* Fill mc_expired_timers_free_q. */

  for (i = 0; i < MC_EXPIRED_TIMERS_MAX; i++) {
    q_put(&mc_expired_timers_free_q,
          q_link(&mc_expired_timer_buf[i], &mc_expired_timer_buf[i].link));
  }

  #ifdef FEATURE_FACTORY_TESTMODE
  #ifdef FEATURE_PLT
#error code not present
  #else
  /* This function is benign when running in DMSS mode */
  ftm_mc_init();
  #endif
  #endif

  #if defined (FEATURE_AUTH) || defined(FEATURE_RUIM)
  /* initialize queue for Authentication Reports */
  (void) q_init( &mc_auth_q );
  (void) q_init( &mc_auth_free_q );

  /* Fill mc_auth_free_q */
  for (i = 0; i < MC_NUM_AUTH_RPT_BUFS; i++)
  {
    mc_auth_rpt_pool[i].rpt_hdr.done_q_ptr = &mc_auth_free_q;
    q_put( &mc_auth_free_q, q_link( &mc_auth_rpt_pool[i],
           &mc_auth_rpt_pool[i].rpt_hdr.link ) );
  }
  #endif /* FEATURE_AUTH || FEATURE_RUIM */

  #ifdef FEATURE_UIM_RUIM
  /* initialize queue for Authentication Reports */
  (void) q_init( &mc_uim_q );
  (void) q_init( &mc_uim_free_q );

  /* Fill mc_auth_free_q */
  for (i = 0; i < MC_NUM_UIM_RPT_BUFS; i++)
  {
    mc_uim_rpt_pool[i].rpt_hdr.done_q_ptr = &mc_uim_free_q;
    q_put( &mc_uim_free_q, q_link( &mc_uim_rpt_pool[i],
           &mc_uim_rpt_pool[i].rpt_hdr.link ) );
  }
  #endif /* FEATURE_UIM_RUIM */

  #if (defined (FEATURE_DH) && defined (FEATURE_DH_EXP))
  /* initialize queue for DH Reports */
  (void) q_init( &mc_dh_q );
  (void) q_init( &mc_dh_free_q );

  /* Fill mc_dh_free_q */
  for (i = 0; i < MC_NUM_DH_RPT_BUFS; i++)
  {
    mc_dh_rpt_pool[i].rpt_hdr.done_q_ptr = &mc_dh_free_q;
    q_put( &mc_dh_free_q, q_link( &mc_dh_rpt_pool[i],
           &mc_dh_rpt_pool[i].rpt_hdr.link ) );
  }
  #endif /* FEATURE_DH_EXP*/

  #ifdef FEATURE_MC_QUEUE_WATERMARK
  /* Initialize the free transaction queue here to support watermarking.
   * This is also initialized during tc_init.
  */
  mctrans_trans_free_q_init();
  #endif  

} /* mc_task_init */

#if !defined (FEATURE_NEW_SBI) && defined (FEATURE_SBI)
/*===========================================================================

FUNCTION MC_INIT_SBI

DESCRIPTION
  This function performs SBI driver initialization

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void mc_init_sbi()
{
  /* Initialize the SBI*/
  SBI_Init();

  /* Program IFT to be in CDMA mode */
  SBI_Slave_Sel(SBI_CTL_WH__SLAVE_ID_IFT );

  /* We can shut off some of the subsystems in the IFT when PA is
     not ON. This reduces the current consumption. The IFT3000 allows
     us to shut them off through the OOE register. We shut off the
     TX AGC signal path and the TX mixer and quadrature divider.
     Write into the Shadow register and this shadow register is
     transmitted through the SBI to the IFT3000.
  */

  SBI_IFT_ON_OFF_REG_Shadow |= (SBI_IFT_ON_OFF_TXPATH_OFF |
                                 SBI_IFT_ON_OFF_TXMIXER_OFF);
  SBI_Write( SBI_IFT_ON_OFF_REG , SBI_IFT_ON_OFF_REG_Shadow);

  /* Start the transmission */
  SBI_Start();

  /* Check if the machine has written or not */
  SBI_Chk_Busy_Write();

  /* We need to send to another slave. So, wait for the transmission to
     be completed with first slave before initiating communication with
     another slave */
  SBI_Chk_Mchn_Busy();

  /* Program the IFR to be in CDMA Mode and out of IDLE/SLEEP */
  SBI_Slave_Sel(SBI_CTL_WH__SLAVE_ID_IFR );

  /* Clear the shadow initially before we write to it */
  SBI_IFR_BLOCK_CTL_REG_Shadow = 0x0;

  /* Set the Block ctl register to values we need:
     TCXO_DIV1: NOte that this should also be done in CLK_GEN_REGister
     Enable CHIPX8, Disable FM,Idle and Sleep */
  SBI_IFR_BLOCK_CTL_REG_Shadow = SBI_IFR_BLOCK_CTL_TCXO_DIV1   |
                                 SBI_IFR_BLOCK_CTL_CHIP_ON_ENA |
                                 SBI_IFR_BLOCK_CTL_FMB_DIS     |
                                 SBI_IFR_BLOCK_CTL_IDLEB_DIS   |
                                 SBI_IFR_BLOCK_CTL_SLEEPB_DIS  ;

  /* Send the data to the IFR */
  SBI_Write ( SBI_IFR_BLOCK_CTL_REG, SBI_IFR_BLOCK_CTL_REG_Shadow) ;
  SBI_Start();
  SBI_Chk_Busy_Write();

} /* mc_init_sbi */
#endif /* FEATURE_SBI && !FEATURE_NEW_SBI */

/*===========================================================================

FUNCTION MC_PUT_NV_ITEM

DESCRIPTION
  This function puts a specific item to NV.

DEPENDENCIES
  The NV task has been started and is running.

RETURN VALUE
  None.

SIDE EFFECTS
  While this function is running all other MC task activities are
  suspended except for watchdog kicking, until the NV item is written.

===========================================================================*/

#ifdef FEATURE_ID
#error code not present
#else
LOCAL void mc_put_nv_item
#endif
(
  nv_items_enum_type  item_code,       /* Item to put */
  nv_item_type        *item_ptr        /* Pointer where to get the item */
)
{

  static nv_cmd_type  mc_nv_cmd_buf;   /* Command buffer to NV */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Prepare command buffer to NV. */

  mc_nv_cmd_buf.cmd = NV_WRITE_F;               /* Write request */
  mc_nv_cmd_buf.tcb_ptr = &mc_tcb;              /* Notify back to me */
  mc_nv_cmd_buf.sigs = MC_NV_CMD_SIG;           /* With this signal */
  mc_nv_cmd_buf.done_q_ptr = NULL;              /* No buffer to return */
  mc_nv_cmd_buf.item = item_code;               /* Item to put */
  mc_nv_cmd_buf.data_ptr = item_ptr;            /* Where to get it */

  /* Clear the signal, issue the command, wait for the responses. */

  (void) rex_clr_sigs(&mc_tcb, MC_NV_CMD_SIG);  /* Clear signal for NV */
  nv_cmd(&mc_nv_cmd_buf);                       /* Issue the request */
  (void) mc_wait(MC_NV_CMD_SIG);                /* And wait for completion */

  /* If there is an error in the request then this is a fatal fault. */

  if (mc_nv_cmd_buf.status != NV_DONE_S) {
    ERR_FATAL("Failed write item to NV %d", item_code, 0, 0);
  }

} /* mc_put_nv_item */

/*===========================================================================

FUNCTION MC_GET_NV_ITEM

DESCRIPTION
  This function gets a specific item from NV.

DEPENDENCIES
  The NV task has been started and is running.

RETURN VALUE
  Status returned from the NV read request.  Status can be either:
    NV_DONE_S       - request done
    NV_NOTACTIVE_S  - item was not active
  Any other status causes ERR_FATAL and the function does not return.

SIDE EFFECTS
  While this function is running all other MC task activities are
  suspended except for watchdog kicking, and until the NV item is
  read in.  If any of read status other then the one described above
  is returned then an ERR_FATAL call is made.

===========================================================================*/

#ifdef FEATURE_ID
#error code not present
#else
LOCAL nv_stat_enum_type mc_get_nv_item
#endif
(
  nv_items_enum_type  item_code,       /* Item to get */
  nv_item_type        *item_ptr        /* Pointer where to put the item */
)
{

  static nv_cmd_type  mc_nv_cmd_buf;   /* Command buffer to NV */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Prepare command buffer to NV. */

  mc_nv_cmd_buf.cmd = NV_READ_F;                /* Read request         */
  mc_nv_cmd_buf.tcb_ptr = &mc_tcb;              /* Notify back to me    */
  mc_nv_cmd_buf.sigs = MC_NV_CMD_SIG;           /* With this signal     */
  mc_nv_cmd_buf.done_q_ptr = NULL;              /* No buffer to return  */
  mc_nv_cmd_buf.item = item_code;               /* Item to get          */
  mc_nv_cmd_buf.data_ptr = item_ptr;            /* Where to return it   */

  /* Clear signal, issue the command, and wait for the respone. */

  (void) rex_clr_sigs(&mc_tcb, MC_NV_CMD_SIG);  /* Clear signal for NV  */
  nv_cmd(&mc_nv_cmd_buf);                       /* Issue the request    */
  (void) mc_wait(MC_NV_CMD_SIG);                /* Wait for completion  */

  /* Check and return status. */

  if ((mc_nv_cmd_buf.status == NV_DONE_S) ||       /* If DONE           */
      (mc_nv_cmd_buf.status == NV_NOTACTIVE_S)) {  /* or NOTACTIVE      */
  } else {                                         /* else fatal error  */
    ERR_FATAL( "Bad NV read status %d", mc_nv_cmd_buf.status, 0, 0 );
  }

  return (mc_nv_cmd_buf.status);                   /* return status     */

} /* mc_get_nv_item */

/*===========================================================================

FUNCTION MC_INIT_ITEMS_TO_NV

DESCRIPTION
  This function loads configuration items from mobile.c definitions
  into NV.  These are items which reside in NV only for consistency
  with mobile station database items to be reported to the base station
  and to be available to the handset (UI).  In each case the item is
  first read.  It then written only if it is different or if was inactive.
  The items are:
    NV_SCM_I               - Station Class Mark
    NV_SLOT_CYCLE_INDEX_I  - Slot cycle index
    NV_MOB_CAI_REV_I       - Mobile CAI revision number
    NV_MOB_FIRM_REV_I      - Mobile firmware revision number
    NV_MOB_MODEL_I         - Mobile model
    NV_CONFIG_CHKSUM_I     - Configuration checksum

DEPENDENCIES
  The NV task is running.

RETURN VALUE
  None.

SIDE EFFECTS
  This function calls mc_put_nv_item which suspends all MC task
  activities, except for watchdog kicking, while waiting for
  completion of write to NV.

===========================================================================*/

LOCAL void mc_init_items_to_nv (void)
{

  nv_item_type          mc_nv_data_buf;     /* NV data buffer */
  nv_stat_enum_type     read_status;        /* NV read status */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* NV_SCM_I */

  read_status = mc_get_nv_item(NV_SCM_I, &mc_nv_data_buf);
  if ((read_status == NV_NOTACTIVE_S) ||
      (mc_nv_data_buf.scm != station_class_mark)) {
    mc_nv_data_buf.scm = station_class_mark;
    mc_put_nv_item(NV_SCM_I, &mc_nv_data_buf);
  }

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* NV_SLOT_CYCLE_INDEX_I */
  #ifndef FEATURE_UIM_RUIM_SUPPORT_SCI
  read_status = mc_get_nv_item(NV_SLOT_CYCLE_INDEX_I, &mc_nv_data_buf);
  if (read_status == NV_NOTACTIVE_S) 
  {
    mc_nv_data_buf.slot_cycle_index = (int8) slot_cycle_index;  
    mc_put_nv_item(NV_SLOT_CYCLE_INDEX_I, &mc_nv_data_buf);
  }

  /* limit SCI to a max of REL_D_MAX_SCI and a min of REL_D_MIN_SCI
  */
  if (mc_nv_data_buf.slot_cycle_index > MCC_MAX_SCI) 
  {
    mc_nv_data_buf.slot_cycle_index = MCC_MAX_SCI;
    mc_put_nv_item(NV_SLOT_CYCLE_INDEX_I, &mc_nv_data_buf);
  }
  
  if (mc_nv_data_buf.slot_cycle_index < MCC_MIN_SCI) 
  {
    mc_nv_data_buf.slot_cycle_index = MCC_MIN_SCI;
    mc_put_nv_item(NV_SLOT_CYCLE_INDEX_I, &mc_nv_data_buf);
  }
  #endif /* FEATURE_UIM_RUIM_SUPPORT_SCI */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* NV_NEG_SLOT_CYCLE_INDEX_I */

  #ifdef FEATURE_MODIFIED_SCI
#error code not present
  #endif /* FEATURE_MODIFIED_SCI */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* NV_MOB_CAI_REV_I */

  read_status = mc_get_nv_item(NV_MOB_CAI_REV_I, &mc_nv_data_buf);

  #ifndef FEATURE_DYNAMIC_P_REV
  if ((read_status == NV_NOTACTIVE_S) ||
      (mc_nv_data_buf.mob_cai_rev != mob_cai_rev)
     )
  {
    mc_nv_data_buf.mob_cai_rev = mob_cai_rev;
    mc_put_nv_item(NV_MOB_CAI_REV_I, &mc_nv_data_buf);
  }
  #else /* DYNAMIC_P_REV */
  if ((read_status == NV_NOTACTIVE_S) ||
      (mc_nv_data_buf.mob_cai_rev > mob_cai_rev) ||
      (!MOB_P_REV_IS_VALID(mc_nv_data_buf.mob_cai_rev))
     )
  {
    #ifdef FEATURE_DYNAMIC_P_REV_DEFAULT_REL_0
    if (mob_cai_rev == P_REV_IS2000_REL_A)
    {
      /* Default the DYNAMIC_P_REV to 6 */
      mc_nv_data_buf.mob_cai_rev = P_REV_IS2000;
    }
    else
    #endif /* FEATURE_DYNAMIC_P_REV_DEFAULT_REL_0 */
    {
      mc_nv_data_buf.mob_cai_rev = mob_cai_rev;
    }

    mc_put_nv_item(NV_MOB_CAI_REV_I, &mc_nv_data_buf);
    MSG_ERROR("MOB_P_REV is set to %d", 
               mc_nv_data_buf.mob_cai_rev, 0, 0);
  }
  else
  {
    if (mc_nv_data_buf.mob_cai_rev < mob_cai_rev)
    {
      MSG_ERROR("MOB_P_REV is downgraded to %d from %d",
                 mc_nv_data_buf.mob_cai_rev, mob_cai_rev, 0);
    }
  }
  #endif /* FEATURE_DYNAMIC_P_REV */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* NV_MOB_FIRM_REV_I */

  read_status = mc_get_nv_item(NV_MOB_FIRM_REV_I, &mc_nv_data_buf);
  if ((read_status == NV_NOTACTIVE_S) ||
      (mc_nv_data_buf.mob_firm_rev != mob_firm_rev)) {
    mc_nv_data_buf.mob_firm_rev = mob_firm_rev;
    mc_put_nv_item(NV_MOB_FIRM_REV_I, &mc_nv_data_buf);
  }

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* NV_MOB_MODEL_I */

  read_status = mc_get_nv_item(NV_MOB_MODEL_I, &mc_nv_data_buf);
  if ((read_status == NV_NOTACTIVE_S) ||
      (mc_nv_data_buf.mob_model != mob_model)) {
    mc_nv_data_buf.mob_model = mob_model;
    mc_put_nv_item(NV_MOB_MODEL_I, &mc_nv_data_buf);
  }

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* NV_CONFIG_CHKSUM_I */

  read_status = mc_get_nv_item(NV_CONFIG_CHKSUM_I, &mc_nv_data_buf);
  if ((read_status == NV_NOTACTIVE_S) ||
      (mc_nv_data_buf.config_chksum.chksum != 0xFFFF)) {
    mc_nv_data_buf.config_chksum.chksum = 0xFFFF;
    mc_put_nv_item(NV_CONFIG_CHKSUM_I, &mc_nv_data_buf);
  }

} /* mc_init_items_to_nv */

/*===========================================================================

FUNCTION MC_INIT_RAN

DESCRIPTION
  This function initializes the pseudo-random number generator by using MS
  ESN as the seed.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

LOCAL void mc_init_ran (void)
{
  nv_item_type        mc_nv_data_buf; /* NV data buffer */
  nv_stat_enum_type   read_status;    /* NV read status */
  dword               seed;           /* seed to random number generator */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Read MS ESN from NV and use it as seed to random number generator */
  /* Set seed to 1 if NV reading fails                                   */

  read_status = mc_get_nv_item( NV_ESN_I, &mc_nv_data_buf );

  if (read_status == NV_DONE_S) {
    seed = (mc_nv_data_buf.esn.esn)?mc_nv_data_buf.esn.esn : 1;
  }
  else {
    ERR( "Bad NV read status %d on NV_ESN_I", read_status, 0, 0 );
    seed = 1;
  }

  ran_seed (seed);

} /* mc_init_ran */

#ifdef FEATURE_CHARGER
#error code not present
#endif

/*===========================================================================

FUNCTION MC_CACHE_PREF_RC_FROM_NV

DESCRIPTION
  This function retrieves the pref RC from NV and stores
  it in SNM

DEPENDENCIES
  The NV task is running.

RETURN VALUE
  None.

SIDE EFFECTS
  This function calls mc_get_nv_item which suspends all MC task
  activities, except for watchdog kicking, while waiting for
  completion of read from NV.

===========================================================================*/

LOCAL void mc_cache_pref_rc_from_nv( void )
{

  nv_item_type          mc_nv_data_buf;     /* NV data buffer */
  nv_stat_enum_type     read_status;        /* NV read status */
  byte pref_fwd_fundicated_rc;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  read_status = mc_get_nv_item( NV_PREF_FOR_RC_I, &mc_nv_data_buf );
  if (read_status == NV_DONE_S) 
  {
    pref_fwd_fundicated_rc = (byte)mc_nv_data_buf.pref_for_rc;
    snm_set_user_pref_fundicated_rc((cai_radio_config_type) pref_fwd_fundicated_rc );
  }

} /* mc_cache_pref_rc_from_nv */

/*===========================================================================

FUNCTION MC_GET_ENTRY

DESCRIPTION
  The function copies the last CDMA and Analog call processing entry reason
  to specified input structure.

DEPENDENCIES
  None.

RETURN VALUE
  None.


SIDE EFFECTS
  None.
===========================================================================*/

void mc_get_entry
(
  mcc_status_type *entry
)
{
  /*lint -e668 */
  memcpy(entry, &last_entry, sizeof(last_entry));
  /*lint +e668 */

} /* mc_last_entry */

/*===========================================================================

FUNCTION MC_UPDATE_LAST_ENTRY

DESCRIPTION
  This procedure updates the last CDMA and Analog entry reason based on the
  current CDMA and Analog entry reason.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void mc_update_last_entry()
{
  /* Update last CDMA entry reason */
  if (cdma_entry.entry == MCC_ENTER_OFFLINE)
  {
    last_entry.cdma = MCC_OFFLINE;
  }
  else
  {
    last_entry.cdma = MCC_ONLINE;
  }

  #ifdef FEATURE_ACP
#error code not present
  #endif /* FEATURE_ACP */

} /* mc_update_last_entry */

/*===========================================================================

FUNCTION MC_UPDATE_LAST_CDMA_ENTRY

DESCRIPTION
  This procedure updates the last CDMA entry reason based on the input
  parameter.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void mc_update_last_cdma_entry
(
  mcc_entry_reason_type entry_reason
)
{
  if ((entry_reason == MCC_OFFLINE) || (entry_reason == MCC_ONLINE))
  {
    last_entry.cdma = entry_reason;
  }

} /* mc_update_last_cdma_entry */

#if defined(FEATURE_ACP) 
#error code not present
#endif /* FEATURE_ACP */

/*===========================================================================

FUNCTION MC_SET_DIGITAL_ACQUISITION

DESCRIPTION
  This procedure sets up the digital acquisition parameters based on
  suggested SS-Action.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void mc_set_digital_acquisition
(
  mcc_entry_name_type entry_name
)
{
  sd_ss_act_s_type ss_act_param; /* SS-Action parameters */

  /* ----------------------------------
  ** Set up CDMA acquisition parameters
  ** ---------------------------------- */
  (void) sd_ss_act_get(&ss_act_param);
  ASSERT( ss_act_param.act == SD_SS_ACT_ACQ_CDMA);

  cdma_entry.entry = entry_name;
  cdma_entry.gen.block_or_sys = ss_act_param.prm.acq_cdma.blksys;
  cdma_entry.gen.band_class = ss_act_param.prm.acq_cdma.band; /*lint !e732 !e641 */
  cdma_entry.gen.cdma_channel = ss_act_param.prm.acq_cdma.chan;
  cdma_entry.gen.new_mode_or_blksys =
    ss_act_param.prm.acq_cdma.is_new_mode_or_blksys;

  #ifdef FEATURE_IS2000_REL_B
  /* MMOC activates 1x protocol, not enter Idle froM TC. */ 
  cdma_entry.gen.enter_idle_from_tc = FALSE;
  #endif /* FEATURE_IS2000_REL_B */

  /* ----------------------------------------------------------------------
  ** Update last CDMA and Analog entry reason based on the current CDMA and
  ** Analog entry reason.
  ** ---------------------------------------------------------------------- */
  mc_update_last_entry();

} /* mc_set_digital_acquisition */

#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */

/*===========================================================================

FUNCTION MC_INFORM_DIAG_OFFLINE_COMPLETE

DESCRIPTION
  This function updates the last digital/analog entry reason based on the
  current digital/analog entry reason and signals DIAG task that we are
  ready to begin offline operation.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void mc_inform_diag_offline_complete
(
  void
)
{
  /* ----------------------------------------------------------------------
  ** Update last CDMA and Analog entry reason based on the current CDMA and
  ** Analog entry reason.
  ** ---------------------------------------------------------------------- */
  mc_update_last_entry();

} /* mc_inform_diag_offline_complete */

/*===========================================================================

FUNCTION MC_SET_DIGITAL_OFFLINE

DESCRIPTION
  This procedure sets up the digital offline parameters.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void mc_set_digital_offline
(
  void
)
{
  cdma_entry.entry = MCC_ENTER_OFFLINE;

  /* The CDMA channel of the entry structure is zero if we never went online
  ** or it is the last CDMA channel used in online mode. Either way it does
  ** not need to be changed here.
  */
  /* Set the band class to PCS if it is supported, otherwise assume cellular
  ** band is supported. This assumes we support CDMA on at least one band.
  ** For tri-mode, a new offline command is needed.
  */
  /*lint -e506 */

  cdma_entry.gen.band_class = cdma.band_class;
} /* mc_set_digital_offline */

#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */

/*===========================================================================

FUNCTION  mc_map_ss_action_to_deact_reason

DESCRIPTION
   This function will map the SD action to the deactivate reason based on the
   current state of MC.

DEPENDENCIES
  None.

RETURN VALUE
  PROT_DEACT_MAX - do not deactivate the current protocol selection,
  others         - deactivate the protocol.

SIDE EFFECTS
  This function will initialize the acquisition parameters for analog/
  digital if the curr_state is not changed.
===========================================================================*/

static prot_deact_e_type mc_map_ss_action_to_deact_reason
(
  mc_state_type curr_state,
    /* Current state of Mode Controller. */

  sd_ss_act_e_type ss_action
    /* SS-Action.  */
)
{
  prot_deact_e_type  deact_reason = PROT_DEACT_MAX;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Switch on the next sd action.
  */
  switch (ss_action)
  {
    /* Continue in current state */
    case SD_SS_ACT_CONTINUE:
#ifdef FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X
#error code not present
#endif /* FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X */
      break;

    /* Go to CDMA online state */
    case SD_SS_ACT_ACQ_CDMA:
      if ( curr_state != MC_ONLINE_DIGITAL_STATE )
      {
        deact_reason = PROT_DEACT_ACQ_OTHERS;
      }
      else if ( cdma_entry.entry == MCC_PSEUDO_IDLE )
      {
        /* Since MMOC did not request to change idle mode, contine in 
           pseudo-idle mode. */
        mc_set_digital_acquisition( MCC_PSEUDO_IDLE );
      }
      else
      {
        mc_set_digital_acquisition( MCC_ACQUISITION );
      }
      break;

    /* Go to AMPS online state */
    #ifdef FEATURE_ACP
#error code not present
    #endif /* FEATURE_ACP */

    #ifdef FEATURE_GPSONE_OOS
#error code not present
    #endif /* FEATURE_GPSONE_OOS */

    case SD_SS_ACT_ACQ_GW:
    case SD_SS_ACT_ACQ_HDR:
//  case SD_SS_ACT_PWR_SAVE:
    case SD_SS_ACT_GET_NET_GW:
      deact_reason = PROT_DEACT_ACQ_OTHERS;
      break;

    case SD_SS_ACT_MEAS_BACK:
    case SD_SS_ACT_MEAS_DED:
      if ( curr_state != MC_MEAS_DED_STATE )
      {
        deact_reason = PROT_DEACT_ACQ_OTHERS;
      }
      break;

  case SD_SS_ACT_PWR_SAVE:
      deact_reason = PROT_DEACT_ACQ_OTHERS;
#ifdef FEATURE_MSM6300_SLEEP
#error code not present
#endif /* FEATURE_MSM6300_SLEEP */
      break;

    default:
      MSG_MED("Invalid SS-Action", 0, 0, 0);
      break;

  } /* end switch (ss_action) */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_MED("deact_reason %d SS-Action %d", deact_reason, ss_action, 0);
  return deact_reason;

} /* mc_map_ss_action_to_deact_reason */

/*===========================================================================

FUNCTION MC_DOG_REPORT

DESCRIPTION
  This function kicks the watchdog timer and set a timer to wake it again.
  It is used by system determination.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mc_dog_report
(
  void
)
{
  KICK_WATCHDOG();

} /* mc_dog_report */

#ifdef FEATURE_AUTH
/*===========================================================================

FUNCTION MC_INIT_AUTH

DESCRIPTION
  This function sends a command to the authentication task to initialize.
  It should be called on powerup or NAM change.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

static void mc_init_auth
(
  void
)
{
  auth_cmd_type *auth_cmd_ptr; /* Command to AUTH */

  MSG_MED("Begin AUTH Initialization", 0, 0, 0);

  if ((auth_cmd_ptr = (auth_cmd_type*) q_get(&auth_free_q)) != NULL)
  {
    /* Initialize authentication variables
     */
    /*lint -e734 */
    auth_cmd_ptr->hdr.command = AUTH_INIT_F; /*lint !e734 */
    auth_cmd_ptr->init.nam = cm_get_curr_nam(); /*lint !e641 !e732 */
    /*lint +e734 */

    /* Set flag so AUTHR will be recalculated for registrations
     */
    cdma.authr_valid = FALSE;

    /* Fill in necessary fields of command block
     */
    auth_cmd_ptr->hdr.cmd_hdr.done_q_ptr = &auth_free_q;
    auth_cmd_ptr->hdr.cmd_hdr.task_ptr = &mc_tcb;
    auth_cmd_ptr->hdr.cmd_hdr.sigs = MC_CMD_SIG;

    (void) rex_clr_sigs(&mc_tcb, MC_CMD_SIG);
    auth_cmd(auth_cmd_ptr);
    (void) mc_wait(MC_CMD_SIG);
    (void) rex_clr_sigs(&mc_tcb, MC_CMD_SIG);

    if (auth_cmd_ptr->hdr.status == AUTH_RECEIVED_S)
    {
      MSG_MED("AUTH Initialized on powerup", 0, 0, 0);
    }
    else
    {
      ERR("Bad status AUTH_INIT_F %d", auth_cmd_ptr->hdr.status, 0, 0);
    }
  }
  else
  {
    ERR("No free buffers on auth_free_q", 0, 0, 0);
  }
} /* mc_init_auth */
#endif /* FEATURE_AUTH */

/*===========================================================================

FUNCTION mc_process_mc_command.

DESCRIPTION
  The function processes commands received from MC command queue when neither
  CDMA nor AMPS protocol are active.

  Note! It is assumed that reselection is allowed when this function is
  invoked.

DEPENDENCIES
  None.

RETURN VALUE
  PROT_DEACT_MAX - current protocol selection is ok.
  Other -  Current protocol stack should be deactivated.

SIDE EFFECTS
  None.
===========================================================================*/
static prot_deact_e_type  mc_process_mc_command
(
  mc_state_type curr_state,
    /* Current state of MC. */

  prot_trans_type *deact_transid_ptr
    /* Pointer to the mmoc's trans Id. To be filled in with
    ** the MMoC's trans Id if a deactivate protocol command
    ** was received.
    */
)
{
  /* pointer to the MC command. */
  mc_msg_type   *cmd_ptr;

  /* Deactivate reason. */
  prot_deact_e_type  deact_reason = PROT_DEACT_MAX;

  /* SD SS action. */
  sd_ss_act_e_type   sd_act;

  if (deact_transid_ptr == NULL)
  {
    ERR_FATAL("deact_transid_ptr is NULL", 0,0,0);
    /* return deact_reason; */
  }

  /* Initialize the MMoC's transid. */
  *deact_transid_ptr = PROT_TRANS_NONE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if there is any command in the queue.  */
  if ((cmd_ptr = (mc_msg_type*) q_check(&mc_cmd_q)) != NULL)
  {
    switch (cmd_ptr->hdr.cmd)
    {

      #ifdef FEATURE_DBM
      case MC_MO_DBM_F :
        /* If an MO DBM request is received while we are in SD,
           we reject the request */
        MSG_HIGH( "Rejecting MO DBM request in SD", 0, 0, 0 );
        mcc_send_mo_dbm_status( cmd_ptr->mo_dbm.burst_type,
                                CM_DBM_MO_NO_SRV
                              );
        break;
      #endif /* FEATURE_DBM */

      #ifdef FEATURE_BROADCAST_SMS
      case  MC_BCSMS_ENABLE_F:
        MSG_MED( "Rxed BCSMS_ENABLE Cmd %d not in CDMA",
                 cmd_ptr->hdr.cmd, 0, 0
               );
        cmd_ptr = (mc_msg_type *) q_get(&mc_cmd_q);
        mccbcsms_enable(cmd_ptr->bcsms_enable.enable);
        break;
      #endif /* FEATURE_BROADCAST_SMS */

      case MC_CDMA_PROT_GEN_CMD_F:
      case MC_AMPS_PROT_GEN_CMD_F:
        cmd_ptr = (mc_msg_type *) q_get(&mc_cmd_q);
        /* It is assumed that reselection is allowed when this function
        ** is invoked.  */
        sd_act =  mmoc_proc_prot_gen_cmd( TRUE, TRUE, &cmd_ptr->gen_cmd.info );

        deact_reason = mc_map_ss_action_to_deact_reason ( curr_state, sd_act );
        MSG_HIGH( "Rcv Gen Cmd deact_reason=%d", deact_reason,0,0 );
        break;

      case MC_CDMA_PROT_DEACT_CMD_F:
      case MC_AMPS_PROT_DEACT_CMD_F:
        cmd_ptr = (mc_msg_type *) q_get(&mc_cmd_q);
        deact_reason = cmd_ptr->deact_prot.info.reason;
        *deact_transid_ptr = cmd_ptr->deact_prot.info.trans_id;
        MSG_HIGH( "Rcv Gen Cmd deact_reason=%d", deact_reason, 0, 0 );
        break;

      default :
        /* In all the other cases, remove command from the Queue */
        cmd_ptr = (mc_msg_type *) q_get(&mc_cmd_q);
        MSG_ERROR( "Received unexpected MC command %d, ", cmd_ptr->hdr.cmd, 0, 0 );
        break;

     } /* end switch (cmd_ptr->hdr.cmd) */

    if (cmd_ptr != NULL)
    {
      cmd_ptr->hdr.status = MC_DONE_S;
      cmd_done(&cmd_ptr->hdr.cmd_hdr);

    } /* end if (cmd_ptr != NULL) */

  } /* end if ((cmd_ptr = (mc_msg_type*) q_check(&mc_cmd_q)) != NULL) */

  return deact_reason;

} /* mc_process_mc_command */

/*===========================================================================

FUNCTION MC_ONLINE_DIGITAL_STATE

DESCRIPTION
  The entrance procedure to the Online Digital State of MC which determines
  what need to be processed and vectors control to the appropiate procedure.

DEPENDENCIES
  None.

RETURN VALUE
  The reason for leaving CDMA mode, and the MMoC Transaction ID.

SIDE EFFECTS
  None.
===========================================================================*/

static prot_deact_e_type mc_online_digital_state
( 
  prot_trans_type *mmoc_trans_id_ptr /* Return the MMoC transaction ID */
)
{
  prot_deact_e_type deact_reason = PROT_DEACT_MAX;

  mcc_exit_type    cdma_exit_info;    /* CDMA call processing exit parameter */
  #ifdef FEATURE_ACP
#error code not present
  #endif /* FEATURE_ACP */


  MSG_HIGH("Going online digital", 0, 0, 0);

  /* Initialize MMOC trans_id */
  cdma_exit_info.mmoc_trans_id = PROT_TRANS_NONE;

  while ( deact_reason == PROT_DEACT_MAX )
  {
    /* Before entering the MCC subtask, clear the watchdog timer and it's
     * signal and record the last entry reason
     */
    (void) rex_clr_timer (&mc_rpt_timer);
    (void) rex_clr_sigs(&mc_tcb, MC_RPT_TIMER_SIG);

    #ifndef FEATURE_PLT
    cdma_exit_info = *mcc_subtask(&cdma_entry);
    #endif /* !FEATURE_PLT */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    MSG_HIGH("Exit Digital Mode %d", cdma_exit_info.exit, 0, 0);

    /* --------------------------------------------------------------------
     * Based on the exit status, call the function to determine the next
     * state. The called function will prepare the entry parameters if we
     * are to stay in digital, or it will return an exit state if the state
     * has changed to NOT online. In certain cases all we need to do is to
     * change state because we are exiting online all together.
     * -------------------------------------------------------------------*/
    switch (cdma_exit_info.exit)
    {
      case MCC_ACCESS_FAILED_SYS_LOST:
      case MCC_ACCESS_COMPLETED:
      case MCC_ACCESS_DENIED:
      case MCC_ACCESS_FAILED:
      case MCC_ACCESS_FAILED_GSRM:
      case MCC_ACCESS_TIMEOUT:
      case MCC_ACQ_FAILED:
      case MCC_INCOMP_CAI_REV :
      case MCC_INTERCEPT :
      case MCC_LOCK:
      case MCC_MAX_ACCESS_PROBES:
      case MCC_NEW_SYSTEM:
      case MCC_NDSS_OFF:
      case MCC_NOT_PREFERRED:
      case MCC_PREF_SYS_CHANGED :
      case MCC_REDIRECTION :
      case MCC_REG_REJECTED :
      case MCC_RELEASE:
      case MCC_REORDER:
      case MCC_RESCAN:
      case MCC_RESELECTION:
      case MCC_SYSTEM_LOST:
      case MCC_SYSTEM_RESELECTION :
      case MCC_UNLOCK:

      #ifdef FEATURE_IS2000_REL_A
      case MCC_ENCRYPTION_MI_FAILED:
      case MCC_ACCT_BLOCKED:
      #endif /* FEATURE_IS2000_REL_A */

      #if defined(FEATURE_IS2000_REL_C_AKA) || defined(FEATURE_IS2000_REL_A_AES)
#error code not present
      #endif /* FEATURE_IS2000_REL_C_AKA || FEATURE_IS2000_REL_A_AES */

      #ifdef FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X
#error code not present
      #endif

      case MCC_SRCH_REQUEST_SYS_RESTART:
      case MCC_SHUTDOWN:
        deact_reason = mc_map_ss_action_to_deact_reason(MC_ONLINE_DIGITAL_STATE,
                                                        sd_ss_act_get(NULL));
        break;

      #ifdef FEATURE_GPSONE_ACQ_MODE
#error code not present
      #endif /* FEATURE_GPSONE_ACQ_MODE */

      case MCC_NAM_CHANGED:
        deact_reason = PROT_DEACT_SUBSC_CHGD;
        break;

      #ifdef FEATURE_ACP
#error code not present
      #endif /* FEATURE_ACP */

      case MCC_LPM:
        MSG_HIGH("LPM", 0, 0, 0);
        deact_reason = PROT_DEACT_LPM;
#ifdef FEATURE_MSM6300_SLEEP
#error code not present
#endif
        break;

      case MCC_OFFLINE_DIGITAL:
        MSG_HIGH("Offline digital", 0, 0, 0);
        deact_reason = PROT_DEACT_OFFLINE;
        break;

      case MCC_POWERDOWN:
        MSG_HIGH("Powerdown", 0, 0, 0);
        deact_reason = PROT_DEACT_PWR_DOWN;
        break;

      case MCC_RESET:
        MSG_HIGH("Reset", 0, 0, 0);
        deact_reason = PROT_DEACT_RESET;
        break;

      default:
        ERR("Bad exit reason returned from MCC: %d", cdma_exit_info.exit, 0, 0);
        break;

    } /* end switch (mcsyspr_cdma_exit_info.exit) */
  } /* end while (next_state == MC_ONLINE_DIGITAL_STATE) */

  *mmoc_trans_id_ptr = cdma_exit_info.mmoc_trans_id;

  return (deact_reason);
 
} /* mc_online_digital_state */

#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */

/*===========================================================================

FUNCTION MC_OFFLINE_DIGITAL_STATE

DESCRIPTION
  The entrance procedure to the Offline Digital State of MC which determines
  what need to be processed and vectors control to the appropiate procedure.

DEPENDENCIES
  None.

RETURN VALUE
  The reason for leaving CDMA mode, and the MMoC Transaction ID.

SIDE EFFECTS
  None.
===========================================================================*/

static prot_deact_e_type  mc_offline_digital_state
( 
  prot_trans_type *mmoc_trans_id_ptr /* Return the MMoC transaction ID */
)
{
  prot_deact_e_type  deact_reason = PROT_DEACT_MAX;

  mcc_exit_type   cdma_exit_info;  /* CDMA call processing exit parameter */


  MSG_HIGH("Going offline digital", 0, 0, 0);

  /* Initialize MMOC trans_id */
  cdma_exit_info.mmoc_trans_id = PROT_TRANS_NONE;

  /* Initialize next state of MC and CDMA entry parameter */
  mc_set_digital_offline();

  /* No clock dividing in offline */
  CLK_DIVIDE(CLK_DIV_ACP | CLK_DIV_CDMA, FALSE);

  /* Initialize MMOC trans_id */
  cdma_exit_info.mmoc_trans_id = PROT_TRANS_NONE;

  /* ---------------------------------------------------------------------
   * Start an endless loop which first calls MCC with the entry reason.
   * Based on the exit reason we decide if we should loop again with a new
   * entry reason, or exit out. Exit is taken only when the new state is
   * not offline digital.
   * --------------------------------------------------------------------- */
  while (deact_reason == PROT_DEACT_MAX)
  {
    /* ------------------------------------------------------------------
     * Before entering the MCC subtask, clear the watchdog timer and it's
     * signal and record the last entry reason.
     * ------------------------------------------------------------------ */
    (void) rex_clr_timer (&mc_rpt_timer);
    (void) rex_clr_sigs(&mc_tcb, MC_RPT_TIMER_SIG);

    /* ---------------------------------------------------------------
     * Call the MCC subtask with the digital entry reason and copy the
     * exit reason when the function returns.
     * --------------------------------------------------------------- */
    #ifndef FEATURE_PLT
    cdma_exit_info = *mcc_subtask(&cdma_entry);
    #endif /* !FEATURE_PLT */


    /* -------------------------------------------------------------------
     * Check the exit status and call the appropriate function to
     * determine the next state. The called function will prepare the
     * entry parameters if we are to stay in digital, or it will return an
     * exit state if the state has changed to NOT offline.
     * ------------------------------------------------------------------- */
    switch (cdma_exit_info.exit)
    {
      /* -----------------------------------------------------------------
       * If these cases we just go right back into offline digital. These
       * orders arrive from the air interface or are the result of regular
       * channel operation and we continue in offline digital.
       * ----------------------------------------------------------------- */
      case MCC_ACCESS_COMPLETED:
      case MCC_ACCESS_DENIED:
      case MCC_ACCESS_FAILED:
      case MCC_ACCESS_FAILED_GSRM:
      case MCC_ACCESS_TIMEOUT:
      case MCC_ACQ_FAILED:
      case MCC_INCOMP_CAI_REV:
      case MCC_INTERCEPT:
      case MCC_LOCK:
      case MCC_MAX_ACCESS_PROBES:
      case MCC_NEW_SYSTEM:
      case MCC_NDSS_OFF:
      case MCC_NOT_PREFERRED:
      case MCC_REDIRECTION:
      case MCC_REG_REJECTED:
      case MCC_RELEASE:
      case MCC_REORDER:
      case MCC_RESCAN:
      case MCC_RESELECTION:
      case MCC_SYSTEM_LOST:
      case MCC_SYSTEM_RESELECTION:
      case MCC_UNLOCK:

      #ifdef FEATURE_IS2000_REL_A
      case MCC_ENCRYPTION_MI_FAILED:
      case MCC_ACCT_BLOCKED:
      #endif /* FEATURE_IS2000_REL_A */

      #if defined(FEATURE_IS2000_REL_C_AKA) || defined(FEATURE_IS2000_REL_A_AES)
#error code not present
      #endif /* FEATURE_IS2000_REL_C_AKA || FEATURE_IS2000_REL_A_AES */

      #ifdef FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X
#error code not present
      #endif

      case MCC_SRCH_REQUEST_SYS_RESTART:
      case MCC_SHUTDOWN:
        break;

      /* These cases are illegal and should not happen */
      case MCC_PREF_SYS_CHANGED:
      case MCC_NAM_CHANGED :
        deact_reason = PROT_DEACT_SUBSC_CHGD;
        break;

      #ifdef FEATURE_ACP
#error code not present
      #endif /* FEATURE_ACP */

      case MCC_OFFLINE_DIGITAL:
        MSG_HIGH("Offline digital", 0, 0, 0);
        deact_reason = PROT_DEACT_OFFLINE;
        break;

      case MCC_POWERDOWN:
        MSG_HIGH("Powerdown", 0, 0, 0);
        deact_reason = PROT_DEACT_PWR_DOWN;
        break;

      case MCC_RESET:
        MSG_HIGH("Reset", 0, 0, 0);
        deact_reason = PROT_DEACT_RESET;
        break;


      /* Otherwise fatal fault */
      default :
        ERR("Bad exit reason returned from digital offline %d",
          cdma_exit_info.exit, 0, 0);
        break;

    } /* switch (cdma_exit_info.exit) */
  } /* while (deact_reason == PROT_DEACT_MAX) */

  *mmoc_trans_id_ptr = cdma_exit_info.mmoc_trans_id;
  return (deact_reason);

} /* mc_offline_digital_state */

#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */

/*===========================================================================

FUNCTION MC_MEAS_DED_STATE

DESCRIPTION
  The entrance procedure to the Dedicated Mode of Measurement state of MC
  which determines what need to be processed and vectors control to the
  appropiate procedure.

DEPENDENCIES
  None.

RETURN VALUE
  The reason for leaving CDMA mode, and the MMoC Transaction ID.

SIDE EFFECTS
  None.
===========================================================================*/

static prot_deact_e_type mc_meas_ded_state
( 
  prot_trans_type *mmoc_trans_id_ptr /* Return the MMoC transaction ID */
)
{
  prot_deact_e_type  deact_reason = PROT_DEACT_MAX;

  mccsrch_rpt_type *rpt_ptr;   /* Pointer to SRCH report */
  dword            sigs;       /* Signals returned by mc_wait() */
  dword            mc_wait_mask;
   /* This mask is used to set various signals on which Mobile waits */
  boolean          new_sys_meas_req = TRUE;
   /* Flag indicates whether it is a new system measurement request */

  #ifdef FEATURE_MULTI_RX_CHAIN
  uint32           rfm_wait;
  #endif

  MSG_HIGH("Enter MC Meas Ded state", 0, 0, 0);

  /* Clean off queue to start */
  while ((rpt_ptr = (mccsrch_rpt_type*) q_get(&mcc_srch_q)) != NULL)
  {
    cmd_done(&rpt_ptr->hdr.rpt_hdr);
  }

  #ifdef FEATURE_MULTI_RX_CHAIN
  /* Enable the RF Receiver chain if it hasn't done so. */
  if (rfm_get_state() == RF_DISABLED_ST)
  {
    rfm_wait = rfm_enable_rx(RFCOM_TRANSCEIVER_0, NULL, NULL);

    (void) rex_set_timer( &mcc_cmd_timer, rfm_wait );
    (void) mcc_wait( (dword) MCC_CMD_TIMER_SIG );
    (void) rex_clr_sigs( &mc_tcb, MCC_CMD_TIMER_SIG );
  }
  #endif

  /* wait for UI command or system measurement report */
  mc_wait_mask = MCC_SRCH_Q_SIG | MCC_CMD_Q_SIG;
  
  while ( deact_reason == PROT_DEACT_MAX )
  {
    /* Send system measurement command to SRCH */
    if (new_sys_meas_req)
    {
      new_sys_meas_req = FALSE;
      /* Search rejected system measurement request */
      if (!mcc_sys_meas_cmd())
      {
        /* Based on SS-Action, determines the next state of MC */
        if ((deact_reason = mc_map_ss_action_to_deact_reason(MC_MEAS_DED_STATE,
            sd_ss_act_get(NULL))) == PROT_DEACT_MAX)
        {
          /* Another system measurement request */
          new_sys_meas_req = TRUE;
        }
      }
    }
    /* wait for UI command or measurement report */
    else
    {
      sigs = mcc_wait(mc_wait_mask);

      /* Process MC command */
      if (sigs & MCC_CMD_Q_SIG)
      {
        deact_reason = mc_process_mc_command(MC_MEAS_DED_STATE, mmoc_trans_id_ptr);
      }
      /* Process Search report */
      else if ((rpt_ptr = (mccsrch_rpt_type*) q_get(&mcc_srch_q)) != NULL)
      {
        switch (rpt_ptr->hdr.rpt)
        {
          case SRCH_SYS_MEAS_R:
            /* Report system measurements to System Determination */
            mcc_sys_meas_rpt(rpt_ptr);
            /* --------------------------------------------------------
            ** Inform system determination of finished reporting system
            ** measurement. Based on SS-Action, determines the next
            ** state of MC to be processed.
            ** -------------------------------------------------------- */
            if ((deact_reason = mc_map_ss_action_to_deact_reason(MC_MEAS_DED_STATE,
              sd_ss_ind_misc_meas_rprt(rpt_ptr->sys_meas.meas_id, TRUE,
              NULL))) == PROT_DEACT_MAX)
            {
              /* Another system measurement request */
              new_sys_meas_req = TRUE;
            }
            cmd_done(&rpt_ptr->hdr.rpt_hdr);
            break;

         default:
           MSG_MED("Unexpected srch rpt %d", rpt_ptr->hdr.rpt, 0, 0);
           cmd_done(&rpt_ptr->hdr.rpt_hdr);
           break;

        } /* end switch (rpt_ptr->hdr.rpt) */

      } /* end if (sigs & MCC_CMD_Q_SIG) */

    } /* end if (new_sys_meas_req) */

  } /* end while (next_state = MC_MEAS_DED_STATE) */

  return (deact_reason);

} /* mc_meas_ded_state */

#ifdef FEATURE_GPSONE_ACQ_MODE
#error code not present
#endif /* FEATURE_GPSONE_ACQ_MODE */

/*===========================================================================

FUNCTION MC_INIT_BEFORE_TASK_START()

DESCRIPTION
  This function initializes CDMA/AMPS related data structure before task
  starts.

DEPENDENCIES
  Perform MC initialization that needs to be done before tasks have started.

  NOTE! Any initialization that result in MC contacting some other tasks,
  such as NV, must be done in sd_init_after_task_start(), as opposed to
  this function.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

LOCAL void mc_init_before_task_start(void)
{

  /* Init task timers/queues */
  mc_task_init();

  /* Initialize Code Channel List object.*/
  mccccl_initialize();

  /* Initialize service negotiation manager */
  snm_initialize_snm();

  /* Initialize mccsyobj_system object.*/
  mccsyobj_initialize();

  /* Move the SO initalizaton after task has started
   ** for 8K targets. 
   */
#ifndef FEATURE_8K_STUFF_TO_BE_PURGED
  /* Initialize non-data service options
  ** This must be called after mc_device_init for voc_available to be set properly
  */
  mccso_initialize();
#endif

  #if defined(FEATURE_IS2000_REL_C_FCS) || defined(FEATURE_IS2000_REL_D)
  /* Initialize SYNC_ID manager */
  mccsyncid_powerup_init();
  #endif /* FEATURE_IS2000_REL_C_FCS || FEATURE_IS2000_REL_D */

  #ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
#error code not present
  #endif /* FEATURE_UMTS_1X_HANDOVER_1XMSM */
 
  #ifdef FEATURE_MDR_DPT_MARKOV
#error code not present
  #endif /* FEATURE_MDR_DPT_MARKOV */

  /* AMPS/CDMA power up initialization */
  #ifdef FEATURE_ACP
#error code not present
  #endif

  (void) mcc_powerup_init();

}/* mc_init_before_task_start */

/*===========================================================================

FUNCTION MC_INIT_AFTER_TASK_START()

DESCRIPTION
  This function initializes CDMA/AMPS related data structure after task
  starts.

DEPENDENCIES
  mc_init_before_task_start has to be called before this function is called.
  nv_task has started.


RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

LOCAL void mc_init_after_task_start(void)
{
  /* Initialize registration services */
  mccreg_init();

 /* Initialize non-data service options for 8k Targets
  ** This must be called after mc_device_init for voc_available to be set properly
  */
#ifdef FEATURE_8K_STUFF_TO_BE_PURGED
  mccso_initialize();
#endif /* FEATURE_8K_STUFF_TO_BE_PURGED */

  /* Write MC parameter from local variable to NV */
  mc_init_items_to_nv();

  /* Retrieve the preference RC from NV and stores */
  mc_cache_pref_rc_from_nv();

  /* Initialize the pseudo-random number generator. */
  mc_init_ran();

  /* Initialize the band class table */
  mcc_band_class_init();

  #ifdef FEATURE_IS2000_DYNAMIC_FEATURE
  /* Initialize the Dynamic Feature items. */
  mccap_df_init();
  #endif /* FEATURE_IS2000_DYNAMIC_FEATURE */
  
  #ifdef FEATURE_IS2000_TDSO
  /* Initialize the Simple-TDSO service option, This is a one time
  ** Initialization with the SNM */
  tdso_init_so_cfg();
  #endif /* FEATURE_IS2000_TDSO */

  /*  Initialize action_store data structure to NULL. This holds pointers
      to action handler functions registered by RXC, TXC and MUX.
  */
  mctrans_init_action_handlers();

  #ifdef FEATURE_IS2000_REL_C_MI
#error code not present
  #endif /* FEATURE_IS2000_REL_C_MI */

  #ifdef FEATURE_MC_QUEUE_WATERMARK
  mc_init_watermark();
  #endif /* FEATURE_MC_QUEUE_WATERMARK */
 
}/* mc_init_after_task_start */

/*===========================================================================

FUNCTION MC_INIT

DESCRIPTION
  This function initializes MC queues, timers and data structure related to
  CDMA/AMPS sub tasks.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

LOCAL void mc_init ( void )
{

  /* Initialization before receiving start signal */
  mc_init_before_task_start();

  /*
  ** Wait for the start signal from the Task Main Controller
  */
  tmc_task_start();

  (void) rex_clr_sigs( rex_self(), TASK_START_SIG );

  /* Initialization after receiving start signal */
  mc_init_after_task_start();

  /* Set watchdog report timer signal so that we'll reset the watchdog as soon
  ** as we start checking for events and start our first tick timer
  */
  (void) rex_set_sigs(&mc_tcb, MC_RPT_TIMER_SIG);
 
  MSG_LOW("MC Initialization finished",0,0,0);

}/* mc_init */

/*===========================================================================
FUNCTION  mc_cdma_prot_activate

DESCRIPTION
  This function is responsible for activating the CDMA protocol for doing
  online acquisition, offline, ftm etc.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void mc_cdma_prot_activate
(
  prot_act_e_type                  act_reason
    /* Reason for activating the CDMA protocol. */
)
{
  /* Reason for deactivating the protocol */
  prot_deact_e_type deact_reason = PROT_DEACT_MAX;

  /* Trans_id for MMoC request.  MMoC handles only one request at a time. */
  prot_trans_type  mmoc_trans_id = PROT_TRANS_NONE;

  /* Prot state to be passed in function mmoc_rpt_prot_auto_deactd_ind to MMOC */
  prot_state_e_type prot_state = PROT_STATE_ONLINE_CDMA;
  
  mccsrch_rpt_type *srch_ptr;  /* pointer to report received from Searcher */
  boolean srch_deactivate_done_rpt_rxed = FALSE; /* Flag when rpt rxed */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_MSM6300_SLEEP
#error code not present
  #endif

  #ifdef FEATURE_FACTORY_TESTMODE
  if (act_reason != PROT_ACT_FTM)
  #endif /* FEATURE_FACTORY_TESTMODE */
  {
    /* Activate SRCH */
    mcc_srch_buf.hdr.cmd = SRCH_CDMA_F;
    mcc_srch_cmd( &mcc_srch_buf );
  }

  /* Activate CDMA protocol based on act_reason.
  */
  switch ( act_reason )
  {
    case PROT_ACT_ONLINE_ACQ:
      mc_set_digital_acquisition( MCC_ACQUISITION );
      deact_reason = mc_online_digital_state(&mmoc_trans_id);
      break;

#ifdef FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X
#error code not present
#endif /* FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X */

    case PROT_ACT_OFFLINE:
      deact_reason = mc_offline_digital_state(&mmoc_trans_id);
      /* Note: Cannot autonomously deactivate itself. */
      if (mmoc_trans_id == PROT_TRANS_NONE)
      {
        MSG_ERROR("mc_offline_digital exited autonomously",0,0,0);
      }
      break;

    #ifdef FEATURE_FACTORY_TESTMODE
    case PROT_ACT_FTM:
      ftm_mode = FTM_MODE;
      #ifdef FEATURE_PLT
#error code not present
      #else
      MSG_HIGH("Entering FTM", 0, 0, 0);
      (void) ftm_system_determination( MC_ONLINE_F );
      #endif /* #ifdef FEATURE_PLT */
      ftm_mode = DMSS_MODE;
      break;
    #endif /* FEATURE_FACTORY_TESTMODE */

    case PROT_ACT_DED_MEAS_REQUEST:
      deact_reason = mc_meas_ded_state(&mmoc_trans_id);

      /* in this case, we need pass a different prot_state to the function of
       * mmoc_rpt_prot_auto_deactd_ind to MMOC */
      prot_state = PROT_STATE_ONLINE_DED_MEAS;
      break;

//    case PROT_ACT_PSEUDO_IDLE:
//      MSG_HIGH( "Activate 1x to Pseudo-Idle Mode", 0, 0, 0 );
//      mc_set_digital_acquisition( MCC_PSEUDO_IDLE );
//      deact_reason = mc_online_digital_state(&mmoc_trans_id);
//      break;

    default:
      ERR_FATAL("Unknown activate reason on CDMA protocol", 0, 0, 0 );
      break;

  } /* switch ( act_reason ) */


  if ((act_reason != PROT_ACT_FTM ) &&
      ((deact_reason != PROT_DEACT_CDMA_AMPS_HANDOFF) &&
       (deact_reason != PROT_DEACT_MAX)))
  {
    /* Deactivate SRCH and wait for it to really deactivate. */
    mcc_srch_buf.hdr.cmd = SRCH_DEACTIVATE_F;
    mcc_srch_cmd( &mcc_srch_buf );

    while (!srch_deactivate_done_rpt_rxed)
    {
      (void) mcc_wait( (dword) MCC_SRCH_Q_SIG );
      while ( (srch_ptr = (mccsrch_rpt_type*) q_get(&mcc_srch_q)) != NULL )
      {
        if ( srch_ptr->hdr.rpt == SRCH_DEACTIVATE_DONE_R )
        {
          MSG_HIGH( "Rxed SRCH_DEACTIVATE_DONE_R", 0, 0, 0 );
          srch_deactivate_done_rpt_rxed = TRUE;
          cmd_done( &srch_ptr->hdr.rpt_hdr );
          break;
        }
        MSG_MED("DISCARDED %d",  srch_ptr->hdr.rpt,0,0);
        cmd_done( &srch_ptr->hdr.rpt_hdr );
      }
    }
  }

  #ifdef FEATURE_MSM6300_SLEEP
#error code not present
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Send the deactivation report to MMOC. */
  if ( mmoc_trans_id != PROT_TRANS_NONE ) //lint !e771
  {
    mmoc_rpt_prot_deactd_cnf( mmoc_trans_id );
  }
  else
  {
    mmoc_rpt_prot_auto_deactd_ind( deact_reason, prot_state);
  }

  return;

} /* mc_cdma_prot_activate() */

#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */

/*===========================================================================
FUNCTION  mc_process_cdma_ph_stat_cmd

DESCRIPTION
  This function will call the appropriate housekeeping functions for CDMA
  protocol based on the phone status changed command.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void mc_process_cdma_ph_stat_cmd
(
  prot_ph_stat_chgd_s_type *ph_stat_ptr
)
{
  /* Perform housekeeping functions based on the phone status change
  ** type.
  */
  switch ( ph_stat_ptr->chg_type )
  {
    case PROT_PH_STAT_CHG_NAM:
    case PROT_PH_STAT_CHG_SUBSC:
      #ifdef FEATURE_AUTH
      mc_init_auth();
      #endif /* #ifdef FEATURE_AUTH */

      /* Reload the subscription information.
      ** Subscription information is reloaded only if CM informs of a change
      ** in CDMA subscription information.
      ** For NAM change, CM always marks CDMA subscription info as changed.
      */
      if (CHECK_PROTO(ph_stat_ptr->prot_subsc_chg, PROT_SUBSC_CHG_CDMA))
      {
        cdma.is_cdma_subs_avail = ph_stat_ptr->is_cdma_subs_avail;

        if ( !mcc_powerup_load() &&
             ph_stat_ptr->oprt_mode != PROT_OPRT_MODE_OFFLINE &&
             ph_stat_ptr->oprt_mode != PROT_OPRT_MODE_OFFLINE_CDMA &&
             ph_stat_ptr->oprt_mode != PROT_OPRT_MODE_OFFLINE_AMPS
           )
        {
          /* subscription load has failed, move the phone to offline state.
          ** if the phone not already offline.
          */
          (void)cm_ph_cmd_oprt_mode( NULL,
                                     NULL,
                                     CM_CLIENT_ID_ANONYMOUS,
                                     SYS_OPRT_MODE_OFFLINE
                                   );

        }
      }

      #ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
#error code not present
      #endif /* FEATURE_UMTS_1X_HANDOVER_1XMSM */

      break;

    case PROT_PH_STAT_CHG_OPRT_MODE:
    {
      switch ( ph_stat_ptr->oprt_mode )
      {
        case PROT_OPRT_MODE_PWROFF:
        case PROT_OPRT_MODE_RESET:
        case PROT_OPRT_MODE_LPM:
          mcc_powerdown();
          break;

        case PROT_OPRT_MODE_OFFLINE:
          /* set offline in MC.  */
          mc_update_last_cdma_entry(MCC_OFFLINE);
          /*lint -fallthrough */
        case PROT_OPRT_MODE_OFFLINE_CDMA:
        case PROT_OPRT_MODE_OFFLINE_AMPS:
          mcc_offline();
          break;

        case PROT_OPRT_MODE_ONLINE:
          /* Perform registration powerup procedures. */
          mccreg_powerup();
          break;

        case PROT_OPRT_MODE_FTM:
          /* Load nv data for mc. */
          cdma.is_cdma_subs_avail = ph_stat_ptr->is_cdma_subs_avail;

          (void) mcc_powerup_load();
          break;

        default:
          break;

      } /* switch ( ph_stat_ptr->oprt_mode ) */

      break;
    }

    default:
      break;

  } /* switch ( ph_stat_ptr->chg_type ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Send the confirmation to MMoC. */
  mmoc_rpt_ph_stat_chgd_cnf( ph_stat_ptr->trans_id
                             #ifdef FEATURE_MMOC_SUPPORTS_HYBRID
                             ,SD_MODE_CDMA
                             #endif
                           );

  return;

} /* mc_process_cdma_ph_stat_cmd() */

#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */

/*===========================================================================
FUNCTION  mc_process_cmd

DESCRIPTION
  This function gets a message off the MC command queue and processes it.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void mc_process_cmd
(
  void
)
{
  /* Pointer to the command received. */
  mc_msg_type *cmd_ptr;

  int i = 0;
  boolean is_cmd_processed = TRUE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Loop thru' entire command q. */
  for (i = 0, cmd_ptr = (mc_msg_type *)q_check(&mc_cmd_q);
       ( ( i < q_cnt(&mc_cmd_q) ) && ( cmd_ptr != NULL ) );
       is_cmd_processed = TRUE
      )
  {
    /* Process or buffer based on the command recvd. */
    switch ( cmd_ptr->hdr.cmd )
    {
      case MC_CDMA_PROT_ACT_CMD_F:
      {
        mc_msg_type mc_cmd_buf = *cmd_ptr;
        q_delete( &mc_cmd_q, &cmd_ptr->hdr.cmd_hdr.link );
        cmd_done( &cmd_ptr->hdr.cmd_hdr );

        /* Activate the CDMA protocol based on activate reason. */
        mc_cdma_prot_activate( mc_cmd_buf.act_prot.info.reason );

        break;
      }

      #ifdef FEATURE_ACP
#error code not present
      #endif /* FEATURE_ACP */

      case MC_CDMA_PROT_DEACT_CMD_F:
      case MC_AMPS_PROT_DEACT_CMD_F:
        /* Protocol already in deactivated state, send deactivated
        ** confirmation anyways.
        */
        mmoc_rpt_prot_deactd_cnf( cmd_ptr->deact_prot.info.trans_id );
        q_delete( &mc_cmd_q, &cmd_ptr->hdr.cmd_hdr.link );
        cmd_done( &cmd_ptr->hdr.cmd_hdr );
        break;

      case MC_CDMA_PROT_GEN_CMD_F:
      case MC_AMPS_PROT_GEN_CMD_F:
        /* Call protocol generic command proc, with not active flag
        ** set to TRUE.
        */
        (void) mmoc_proc_prot_gen_cmd( TRUE, FALSE, &cmd_ptr->gen_cmd.info );
        q_delete( &mc_cmd_q, &cmd_ptr->hdr.cmd_hdr.link );
        cmd_done( &cmd_ptr->hdr.cmd_hdr );
        break;

      case MC_CDMA_PROT_PH_STAT_CHGD_CMD_F:
        /* Process the CDMA phone status command. */
        mc_process_cdma_ph_stat_cmd( &cmd_ptr->ph_stat_cmd.info );
        q_delete( &mc_cmd_q, &cmd_ptr->hdr.cmd_hdr.link );
        cmd_done( &cmd_ptr->hdr.cmd_hdr );
        break;

      #ifdef FEATURE_ACP
#error code not present
      #endif /* FEATURE_ACP */

      #ifdef FEATURE_DBM
      case MC_MO_DBM_F :
        /* If an MO DBM request is received here, reject the request. */
        MSG_HIGH( "Rejecting MO DBM request in SD", 0, 0, 0 );
        mcc_send_mo_dbm_status( cmd_ptr->mo_dbm.burst_type,
                                CM_DBM_MO_NO_SRV
                              );
        q_delete( &mc_cmd_q, &cmd_ptr->hdr.cmd_hdr.link );
        cmd_done( &cmd_ptr->hdr.cmd_hdr );
        break;
      #endif

      #ifdef FEATURE_BROADCAST_SMS
      case  MC_BCSMS_ENABLE_F:
        MSG_MED( "Rxed BCSMS_ENABLE Cmd %d not in CDMA",
                 cmd_ptr->hdr.cmd, 0, 0
               );
        mccbcsms_enable(cmd_ptr->bcsms_enable.enable);
        q_delete( &mc_cmd_q, &cmd_ptr->hdr.cmd_hdr.link );
        cmd_done( &cmd_ptr->hdr.cmd_hdr );
        break;
      #endif /* FEATURE_BROADCAST_SMS */

      case MC_ORIGINATION_F:
        /* Inform CM origination failed.
        */
        mcc_send_orig_failure( CM_CALL_ORIG_ERR_NO_SRV,
                               cmd_ptr->origination.call_id,
                               CM_CALL_ORIG_ERR_LAYER_OTHER
                             );
        cdma.call_orig = FALSE;
        q_delete( &mc_cmd_q, &cmd_ptr->hdr.cmd_hdr.link );
        cmd_done( &cmd_ptr->hdr.cmd_hdr );
        break;

      default:
        /* Check if we crossed the safe limit for buffering.
        */
        if ( q_cnt( &mc_cmd_q ) >= (MC_CMD_BUFS_NUM -2) )
        {
          /* We crossed, delete the received command.
          */
          MSG_ERROR( "Ignored unexpected command %d",
                     cmd_ptr->hdr.cmd, 0, 0 );
          q_delete( &mc_cmd_q, &cmd_ptr->hdr.cmd_hdr.link );
          cmd_done( &cmd_ptr->hdr.cmd_hdr );

        }
        else
        {
          /* Buffer the unhandled commands to be processed by either the AMPS
          ** or CDMA stack.
          */
          is_cmd_processed = FALSE ;
          i++;
          MSG_ERROR( "Buffer unexpected MC command %d, ",
                     cmd_ptr->hdr.cmd, 0, 0 );
          cmd_ptr = (mc_msg_type *)q_next( &mc_cmd_q, &cmd_ptr->hdr.cmd_hdr.link );
        }
        break;

    } /* switch ( mc_cmd.hdr.cmd ) */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /*  Check if the command was processed. */
    if ( is_cmd_processed )
    {
      /* If command was processed, start with the first in the queue.
      ** since MC command are being handled inside the procesing functions like
      ** mc_cdma_prot_activate().
      */
      cmd_ptr = q_check( &mc_cmd_q );
      i = 0;
    }

  } /* for () */

} /* mc_process_cmd() */

/*===========================================================================

FUNCTION MC_TASK

DESCRIPTION
  This function is the entrance for the Main Control task.  It is the
  task responsible for creating all other software tasks and timers,
  initializing all services, and then starting up all other software
  tasks.  After an aknowledgement has been received from each software
  task indicating that it has started successfully, this function calls
  the ACP and CDMA subtasks power on initialization point so that they
  can perform their initialization.  The function then enters system
  determination where analog, digital call processing or LPM is invoked.
  Control returns from system determination when reset or powerdown
  are to be performed.  In either case DMSS powerdown is invoked,
  followed by either reset or powerdown.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Reset and powerdown processing are special in that they cause the
  complete DMSS software to either restart or to be shutdown.

===========================================================================*/

void mc_task
(
  dword ignored
    /* Parameter received from Rex - ignored */
    /*lint -esym(715,ignored)
    ** Have lint not complain about the ignored parameter 'ignored' which is
    ** specified to make this routine match the template for rex_init().
    */
)
{
/*---------------------------------------------------------------------------
**
**             DEFINE MULTI MODE FEATURE
**
**-------------------------------------------------------------------------*/

  /*
  ** Initialize task
  */
  mc_init();

  #ifdef FEATURE_TREX
#error code not present
  #endif

  /* Add mc task report command processing here */

  while ( TRUE ) //lint !e716
  {
    /* Process the received command.
    */
    mc_process_cmd();

    (void) mc_wait(MC_CMD_Q_SIG); 
    (void) rex_clr_sigs(&mc_tcb, MC_CMD_Q_SIG); 

  } /* while ( true ) */

} /* mc_task */

/*===========================================================================

FUNCTION mc_get_buf_else_err_fatal

DESCRIPTION
  Allocate a MC Task command buffer. Error fatal if allocation is NOT
  successful.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to allocated MC command buffer.

SIDE EFFECTS
  none
===========================================================================*/

mc_msg_type *mc_get_buf_else_err_fatal( void )
{

  mc_msg_type *mc_cmd_ptr = (mc_msg_type*) q_get( &mc_cmd_free_q );
    /* Try allocating a MC Task command buffer */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If allocation failed, error fatal. */
  if( mc_cmd_ptr == NULL )
  {
    ERR_FATAL( "Can't get MC buf",0,0,0 );
    /* return NULL; */
  }

  /* If we got here, allocation is successful,
  ** so indicate queue to place command buffer on when done,
  ** and specify NO task to be signalled when done
  */
  mc_cmd_ptr->hdr.cmd_hdr.done_q_ptr  = &mc_cmd_free_q;
  mc_cmd_ptr->hdr.cmd_hdr.task_ptr    = NULL;

  return mc_cmd_ptr;

} /* mc_get_buf_else_err_fatal() */

/*===========================================================================

FUNCTION MC_CMD

DESCRIPTION
  This is the outside world's interface to the MC task and subtasks.
  this function takes an already filled out mc_msg_type and places it
  on the mc command queue.  The function returns to the caller after
  setting the status to busy indicaton.  This does _not_ mean that the
  command has been executed yet, just that it is waiting to be executed.

DEPENDENCIES
  The MC task must have been started up already.  All of the items in
  the mc_cmd_type must be already set.

RETURN VALUE
  None directly.  The status variable of the mc_cmd_type will be updated
  to reflect the current status of this command as it is processed.

SIDE EFFECTS
  The mc_cmd_type is placed on the MC command queue.  It must not be
  modified until the command has been processed.

===========================================================================*/

void mc_cmd
(
  mc_msg_type  *msg_ptr   /* Pointer to MC message buffer */
)
{
  switch (msg_ptr->hdr.cmd)
  {
    #ifdef FEATURE_HDR
#error code not present
    #endif /* FEATURE_HDR */

    default:
    {

      #ifdef FEATURE_MC_QUEUE_WATERMARK
      mc_update_watermark(MCWM_CMD_FREE_Q);
      #endif /* FEATURE_MC_QUEUE_WATERMARK */

      /* Set the request status to busy. initialize and link the command  */
      /* onto the MC command queue, set a signal to the MC task and exit. */

      msg_ptr->hdr.status = MC_BUSY_S;
      (void) q_link(msg_ptr, &msg_ptr->hdr.cmd_hdr.link);
      q_put(&mc_cmd_q, &msg_ptr->hdr.cmd_hdr.link);
      (void) rex_set_sigs(&mc_tcb, MC_CMD_Q_SIG);

      break;
    }
  }

} /* mc_cmd */


/*===========================================================================

FUNCTION MC_QUEUE_CMD

DESCRIPTION
  This is the outside world's interface to the MC task and subtasks.
  This function will unqueue an MC command from the mc_cmd_free_q and 
  fill it out and places it on the mc command queue.  This does _not_ 
  mean that the command has been executed yet, just that it is waiting 
  to be executed.

DEPENDENCIES
  The MC task must have been started up already.  

RETURN VALUE
  mc_queue_cmd_status_type  
  
SIDE EFFECTS
  None.

===========================================================================*/

mc_queue_cmd_status_type  mc_queue_cmd
(
  mc_name_type cmd_type
)
{
  mc_msg_type *mc_cmd_ptr;
  /*------------------------------------------------------------------------
    Get an MC command link, load up the REX-related fields, and return the
    pointer
  -------------------------------------------------------------------------*/
  if(( mc_cmd_ptr = (mc_msg_type *)q_get( &mc_cmd_free_q)) == NULL)
  {
    return MC_FREE_QUEUE_EMPTY;
  }
  else /* load up the fields */
  {
    mc_cmd_ptr->hdr.cmd                = cmd_type;
    mc_cmd_ptr->hdr.cmd_hdr.task_ptr   = rex_self();

    /* Put NULL for the signal return for now, figure out what signal
       need to put here later. */
    mc_cmd_ptr->hdr.cmd_hdr.sigs       = NULL;
    mc_cmd_ptr->hdr.cmd_hdr.done_q_ptr = &mc_cmd_free_q;
  }

  mc_cmd (mc_cmd_ptr);
  return MC_CMD_QUEUED;

} /* mc_queue_cmd */

/*===========================================================================

FUNCTION MC_STATE

DESCRIPTION
  This function returns the current state of MC.

DEPENDENCIES
  None.

RETURN VALUE
  Current state of MC.

SIDE EFFECTS
  None.

===========================================================================*/

byte mc_state( void )
{
  byte state;
    /* Current state of the phone */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_SD20_LOGGING
  switch( mc_mode_controller_state )
  {
    case MC_ONLINE_DIGITAL_STATE:
      state = mcc_get_state();
      break;

    case MC_OFFLINE_DIGITAL_STATE:
      state = DIAG_M_OFFLINE_DIGITAL_S;
      break;

    #ifdef FEATURE_HDR
#error code not present
    #endif /* FEATURE_HDR */

    #ifdef FEATURE_ACP
#error code not present
    #endif /* FEATURE_ACP */

    case MC_RESET_STATE:
      state = DIAG_M_RESET_S;
      break;

    case MC_POWERDOWN_STATE:
      state = DIAG_M_POWERDOWN_S;
      break;

    case MC_POWERSAVE_STATE:
      state = DIAG_M_POWERSAVE_S;
      break;

    case MC_POWERUP_STATE:
      state = DIAG_M_POWERUP_S;
      break;

    case MC_LPM_STATE:
      state = DIAG_M_LPM_S;
      break;

    case MC_MEAS_DED_STATE:
      state = DIAG_M_MEAS_DED_S;
      break;

    case MC_EXIT_STATE:
    default:
      state = DIAG_NO_SVC_S;
      break;
  }
  #else /* not FEATURE_SD20_LOGGING */
  db_get(DB_DMSS_STATE, &mc_db_item);
  switch (mc_db_item.dmss_state)
  {
    case DB_DMSS_POWERUP_S:
    case DB_DMSS_RESET_S:
    case DB_DMSS_POWERDOWN_S:
      state = DIAG_NO_SVC_S;
      break;

    #ifdef FEATURE_ACP
#error code not present
    #endif /* FEATURE_ACP */

    case DB_DMSS_ONLINE_DIGITAL_S:
    case DB_DMSS_OFFLINE_DIGITAL_S:
      state = mcc_get_state();
      break;

    default:
      state = DIAG_NO_SVC_S;
      break;
  }
  #endif /* FEATURE_SD20_LOGGING */

  return (state);

} /* mc_state */

/*===========================================================================

FUNCTION MC_GET_BAND_CLASS

DESCRIPTION
  This function returns the current operating band class of the mobile.

DEPENDENCIES
  None.

RETURN VALUE
  Current band class.

SIDE EFFECTS
  None.

===========================================================================*/

byte mc_get_band_class
(
  void
)
{
  return cdma.band_class;

} /* mc_get_band_class */

/*===========================================================================

FUNCTION MC_GET_P_REV_IN_USE

DESCRIPTION
  This function returns the current P_REV_IN_USE of the mobile.

DEPENDENCIES
  None.

RETURN VALUE
  Current P_REV_IN_USE.

SIDE EFFECTS
  None.

===========================================================================*/

byte mc_get_p_rev_in_use
(
  void
)
{
  return cdma.p_rev_in_use;

} /* mc_get_p_rev_in_use */

/*===========================================================================

FUNCTION MC_GET_MOB_CAI_REV

DESCRIPTION
  This function returns the protocol revision number of the mobile station.

DEPENDENCIES
  None.

RETURN VALUE
  The mobile's protocol revision number.

SIDE EFFECTS
  None.

===========================================================================*/

byte mc_get_mob_cai_rev
(
  void
)
{
  return cdma.mob_cai_rev;

} /* mc_get_mob_cai_rev */

/*===========================================================================
FUNCTION mc_cdma_activate_protocol

DESCRIPTION
  This function commands the MC task to activate the CDMA protocol to do
  either  online acquisition, offline operation, FTM etc specified in the
  act_reason.

DEPENDENCIES
  None.

RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/

void mc_cdma_activate_protocol
(
  prot_act_e_type act_reason
    /* Reason for activating the CDMA protocol. */
)
{
  /* MC command pointer. */
  mc_msg_type *cmd_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the command buffer. */
  cmd_ptr = mc_get_buf_else_err_fatal();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Fill in the fields. */
  cmd_ptr->hdr.cmd                     = MC_CDMA_PROT_ACT_CMD_F;
  cmd_ptr->act_prot.info.reason        = act_reason;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Post the command. */
  mc_cmd( cmd_ptr );

  return;

} /* mc_cdma_activate_protocol() */

/*===========================================================================
FUNCTION mc_cdma_deactivate_protocol

DESCRIPTION
  This function commands the MC task to deactivate the CDMA protocol.

DEPENDENCIES
  None.

RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/

void mc_cdma_deactivate_protocol
(
  prot_deact_e_type deact_reason,
    /* Reason for deactivating the CDMA protocol. example, PWR_DOWN,
    ** RESET etc.
    */

  prot_trans_type trans_id
    /* Transaction Id for this request. */
)
{
  /* MC command pointer. */
  mc_msg_type *cmd_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_MSM6300_SLEEP
#error code not present
#endif /* FEATURE_MSM6300_SLEEP */

  /* Get the command buffer. */
  cmd_ptr = mc_get_buf_else_err_fatal();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Fill in the fields. */
  cmd_ptr->hdr.cmd                     = MC_CDMA_PROT_DEACT_CMD_F;
  cmd_ptr->deact_prot.info.reason      = deact_reason;
  cmd_ptr->deact_prot.info.trans_id    = trans_id;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Post the command. */
  mc_cmd( cmd_ptr );

  return;

} /* mc_cdma_deactivate_protocol() */

/*===========================================================================
FUNCTION mc_cdma_ph_status_chgd_cmd

DESCRIPTION
  This function should be called to inform the CDMA protocol of the change
  in phone status, the protocol will initiate the housekeeping tasks if
  necessary.

DEPENDENCIES
  None.

RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/

void mc_cdma_ph_status_chgd_cmd
(
  prot_ph_stat_chg_e_type chg_type,
    /* Type of phone status change( NAM change, operating mode change etc.) */

  byte nam,
    /* Current NAM selected by the user. */

  boolean is_cdma_subs_avail,
    /* Current CDMA subscription availability status. */

  prot_oprt_mode_e_type oprt_mode,
    /* Current operating mode of the phone. */

  prot_trans_type trans_id,
    /* Transaction Id for this request. */

  prot_subsc_chg_e_type prot_subsc_chg
    /* Enumeration of protocols that have a change in subscription 
       available status */
)
{
  /* MC command pointer. */
  mc_msg_type *cmd_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the command buffer.
  */
  cmd_ptr = mc_get_buf_else_err_fatal();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Fill in the fields. */
  cmd_ptr->hdr.cmd                     = MC_CDMA_PROT_PH_STAT_CHGD_CMD_F;
  cmd_ptr->ph_stat_cmd.info.chg_type   = chg_type;
  cmd_ptr->ph_stat_cmd.info.nam        = nam;
  cmd_ptr->ph_stat_cmd.info.is_cdma_subs_avail =
                                         is_cdma_subs_avail;
  cmd_ptr->ph_stat_cmd.info.oprt_mode  = oprt_mode;
  cmd_ptr->ph_stat_cmd.info.trans_id   = trans_id;
  cmd_ptr->ph_stat_cmd.info.prot_subsc_chg = prot_subsc_chg;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Post the command. */
  mc_cmd( cmd_ptr );

  return;

} /* mc_cdma_ph_status_chgd_cmd() */

/*===========================================================================
FUNCTION mc_cdma_generic_prot_cmd

DESCRIPTION
  This function should be used to command the CDMA protocol stack to process
  the generic command from MMoC in its MC's task context.

DEPENDENCIES
  None.

RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/
void mc_cdma_generic_prot_cmd
(
  prot_gen_cmd_s_type *gen_cmd_ptr
    /* Pointer to the generic command information. */
)
{
  /* MC command pointer.
  */
  mc_msg_type                    *cmd_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the command buffer.
  */
  cmd_ptr = mc_get_buf_else_err_fatal();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Fill in the fields.
  */
  cmd_ptr->hdr.cmd                     = MC_CDMA_PROT_GEN_CMD_F;
  cmd_ptr->gen_cmd.info                = *gen_cmd_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Post the command.
  */
  mc_cmd( cmd_ptr );

  return;

} /* mc_cdma_generic_prot_cmd() */

/*===========================================================================
FUNCTION mc_amps_activate_protocol

DESCRIPTION
  This function commands the MC task to activate the AMPS protocol to do
  either  online acquisition, offline operation etc specified in the
  act_reason.

DEPENDENCIES
  None.

RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/

void mc_amps_activate_protocol
(
  prot_act_e_type act_reason
  /* Reason for activating the AMPS protocol. */
)
{
  /* MC command pointer.
  */
  mc_msg_type                    *cmd_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the command buffer.
  */
  cmd_ptr = mc_get_buf_else_err_fatal();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Fill in the fields.
  */
  cmd_ptr->hdr.cmd                     = MC_AMPS_PROT_ACT_CMD_F;
  cmd_ptr->act_prot.info.reason        = act_reason;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Post the command.
  */
  mc_cmd( cmd_ptr );

  return;

} /* mc_amps_activate_protocol() */

/*===========================================================================
FUNCTION mc_amps_deactivate_protocol

DESCRIPTION
  This function commands the MC task to deactivate the AMPS protocol.

DEPENDENCIES
  None.

RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/

void mc_amps_deactivate_protocol
(
  prot_deact_e_type deact_reason,
    /* Reason for deactivating the AMPS protocol. example, PWR_DOWN,
    ** RESET etc. */

  prot_trans_type trans_id
    /* Transaction Id for this request.  */
)
{
  /* MC command pointer.
  */
  mc_msg_type                    *cmd_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the command buffer.
  */
  cmd_ptr = mc_get_buf_else_err_fatal();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Fill in the fields.
  */
  cmd_ptr->hdr.cmd                     = MC_AMPS_PROT_DEACT_CMD_F;
  cmd_ptr->deact_prot.info.reason      = deact_reason;
  cmd_ptr->deact_prot.info.trans_id    = trans_id;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Post the command.
  */
  mc_cmd( cmd_ptr );

  return;

} /* mc_amps_deactivate_protocol() */

/*===========================================================================
FUNCTION mc_amps_ph_status_chgd_cmd

DESCRIPTION
  This function should be called to inform the AMPS protocol of the change
  in phone status, the protocol will initiate the housekeeping tasks if
  necessary.

DEPENDENCIES
  None.

RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/

void mc_amps_ph_status_chgd_cmd
(
  prot_ph_stat_chg_e_type chg_type,
    /* Type of phone status change( NAM change, operating mode change etc.) */

  byte nam,
    /* Current NAM selected by the user. */

  boolean is_cdma_subs_avail,
    /* Current CDMA subscription availability status. */

  prot_oprt_mode_e_type oprt_mode,
    /* Current operating mode of the phone. */

  prot_trans_type trans_id,
    /* Transaction Id for this request. */

  prot_subsc_chg_e_type prot_subsc_chg
    /* Enumeration of protocols that have a change in subscription available
       status */
)
{
  /* MC command pointer.
  */
  mc_msg_type                    *cmd_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the command buffer.
  */
  cmd_ptr = mc_get_buf_else_err_fatal();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Fill in the fields.
  */
  cmd_ptr->hdr.cmd                     = MC_AMPS_PROT_PH_STAT_CHGD_CMD_F;
  cmd_ptr->ph_stat_cmd.info.chg_type   = chg_type;
  cmd_ptr->ph_stat_cmd.info.nam        = nam;
  cmd_ptr->ph_stat_cmd.info.is_cdma_subs_avail =
                                         is_cdma_subs_avail;
  cmd_ptr->ph_stat_cmd.info.oprt_mode  = oprt_mode;
  cmd_ptr->ph_stat_cmd.info.trans_id   = trans_id;
  cmd_ptr->ph_stat_cmd.info.prot_subsc_chg = prot_subsc_chg;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Post the command. */
  mc_cmd( cmd_ptr );

  return;

} /* mc_amps_ph_status_chgd_cmd() */

/*===========================================================================
FUNCTION mc_amps_generic_prot_cmd

DESCRIPTION
  This function should be used to command the AMPS protocol stack to process
  the generic command from MMoC in MC's task context.

DEPENDENCIES
  None.

RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/

void mc_amps_generic_prot_cmd
(
  prot_gen_cmd_s_type              *gen_cmd_ptr
    /* Pointer to the generic command information.  */
)
{
  /* MC command pointer.
  */
  mc_msg_type                    *cmd_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the command buffer.
  */
  cmd_ptr = mc_get_buf_else_err_fatal();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Fill in the fields.
  */
  cmd_ptr->hdr.cmd                     = MC_AMPS_PROT_GEN_CMD_F;
  cmd_ptr->gen_cmd.info                = *gen_cmd_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Post the command.
  */
  mc_cmd( cmd_ptr );

  return;

} /* mc_amps_generic_prot_cmd() */

#ifdef FEATURE_MC_QUEUE_WATERMARK
/*===========================================================================
FUNCTION MC_INIT_WATERMARK

DESCRIPTION
  This function retrieves the MC queue watermark history values from NV,
  and initializes the queue watermark variable.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mc_init_watermark
(
  void
)
{
  nv_item_type          mc_nv_data_buf;     /* NV data buffer */
  nv_stat_enum_type     read_status;        /* NV read status */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the watermark info from NV. */
  read_status = mc_get_nv_item( NV_SPARE_2_I, &mc_nv_data_buf );
  if (read_status == NV_DONE_S)
  {
    mc_wm[MCWM_CMD_FREE_Q].nv_value =
    mc_wm[MCWM_CMD_FREE_Q].max_use =
      ((mc_nv_data_buf.spare_2 & MCWM_CMD_FREE_Q_MASK) >>
       MCWM_CMD_FREE_Q_SHIFT_BITS);
    mc_wm[MCWM_RXTX_FREE_Q].nv_value =
    mc_wm[MCWM_RXTX_FREE_Q].max_use =
      ((mc_nv_data_buf.spare_2 & MCWM_RXTX_FREE_Q_MASK) >>
       MCWM_RXTX_FREE_Q_SHIFT_BITS);
    mc_wm[MCWM_RX_FREE_Q].nv_value =
    mc_wm[MCWM_RX_FREE_Q].max_use =
      ((mc_nv_data_buf.spare_2 & MCWM_RX_FREE_Q_MASK) >>
       MCWM_RX_FREE_Q_SHIFT_BITS);
    mc_wm[MCWM_TRANS_FREE_Q].nv_value =
    mc_wm[MCWM_TRANS_FREE_Q].max_use =
      ((mc_nv_data_buf.spare_2 & MCWM_TRANS_FREE_Q_MASK) >>
       MCWM_TRANS_FREE_Q_SHIFT_BITS);
  }
  else
  {
    mc_wm[MCWM_CMD_FREE_Q].nv_value  = mc_wm[MCWM_CMD_FREE_Q].max_use  = 0;
    mc_wm[MCWM_RXTX_FREE_Q].nv_value = mc_wm[MCWM_RXTX_FREE_Q].max_use = 0;
    mc_wm[MCWM_RX_FREE_Q].nv_value   = mc_wm[MCWM_RX_FREE_Q].max_use   = 0;
    mc_wm[MCWM_TRANS_FREE_Q].nv_value   = mc_wm[MCWM_TRANS_FREE_Q].max_use   = 0;
  }

  mc_wm[MCWM_CMD_FREE_Q].wm_q_name = MCWM_CMD_FREE_Q;
  mc_wm[MCWM_CMD_FREE_Q].q_ptr     = &mc_cmd_free_q;
  mc_wm[MCWM_CMD_FREE_Q].max_alloc = MC_CMD_BUFS_NUM;

  mc_wm[MCWM_RXTX_FREE_Q].wm_q_name = MCWM_RXTX_FREE_Q;
  mc_wm[MCWM_RXTX_FREE_Q].q_ptr     = &mcc_rxtx_free_q;
  mc_wm[MCWM_RXTX_FREE_Q].max_alloc = 5; /* MCCRXTX_NUM_BUFS */

  mc_wm[MCWM_RX_FREE_Q].wm_q_name  = MCWM_RX_FREE_Q;
  mc_wm[MCWM_RX_FREE_Q].q_ptr      = &mcc_rx_free_q;
  mc_wm[MCWM_RX_FREE_Q].max_alloc  = 3; /* MCCRX_NUM_RPT_BUFS */

  mc_wm[MCWM_TRANS_FREE_Q].wm_q_name  = MCWM_TRANS_FREE_Q;
  mc_wm[MCWM_TRANS_FREE_Q].q_ptr      = &trans_free_q;
  mc_wm[MCWM_TRANS_FREE_Q].max_alloc  = MCTRANS_MAX_TRANSACTIONS;

} /* mc_init_watermark */

/*===========================================================================

FUNCTION MC_UPDATE_WATERMARK

DESCRIPTION
  This function checks and updates the MC's queue watermark against the
  history value. Note it does not write to NV.

DEPENDENCIES
  MC's queue watermark has to be retrieved from NV upon powerup.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mc_update_watermark
(
  mc_wm_q_type  q_name
)
{
  byte  cur_cnt;
  uint16 wm_value;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch(q_name)
  {
    case MCWM_CMD_FREE_Q:
    case MCWM_RXTX_FREE_Q:
    case MCWM_RX_FREE_Q:
    case MCWM_TRANS_FREE_Q:
      cur_cnt = q_cnt(mc_wm[q_name].q_ptr);
      if (mc_wm[q_name].max_use < (mc_wm[q_name].max_alloc - cur_cnt))
      {
        mc_wm[q_name].max_use = mc_wm[q_name].max_alloc - cur_cnt;
      }
      wm_value = (((mc_wm[MCWM_CMD_FREE_Q].max_use <<
                 MCWM_CMD_FREE_Q_SHIFT_BITS) & MCWM_CMD_FREE_Q_MASK) |
                ((mc_wm[MCWM_RXTX_FREE_Q].max_use <<
                 MCWM_RXTX_FREE_Q_SHIFT_BITS) & MCWM_RXTX_FREE_Q_MASK) |
                ((mc_wm[MCWM_RX_FREE_Q].max_use <<
                 MCWM_RX_FREE_Q_SHIFT_BITS) & MCWM_RX_FREE_Q_MASK)    |
                ((mc_wm[MCWM_TRANS_FREE_Q].max_use <<
                 MCWM_TRANS_FREE_Q_SHIFT_BITS) & MCWM_TRANS_FREE_Q_MASK));

      MSG_MED ("MC queue %d max use updated to:%d WM=0x%x", q_name, mc_wm[q_name].max_use, wm_value);

      break;

    default:
      MSG_ERROR("Unsupported wm q type %d", q_name, 0, 0);
      break;
  }

} /* mc_update_watermark */

/*===========================================================================

FUNCTION MC_STORE_WATERMARK

DESCRIPTION
  This function does a final check of the MC's queue watermark against the
  NV history value, and updates it if necessary. Note it does write to NV.

DEPENDENCIES
  MC's queue watermark has to be retrieved from NV upon powerup.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mc_store_watermark
(
  void
)
{
  byte      cur_cnt;
  int       i;
  boolean   nv_update = FALSE;
  uint16    wm_value;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (i = 0; i < MCWM_MAX_NUM_WM; i++)
  {
    cur_cnt = q_cnt(mc_wm[i].q_ptr);

    if (mc_wm[i].max_use < (mc_wm[i].max_alloc - cur_cnt))
    {
      mc_wm[i].max_use = mc_wm[i].max_alloc - cur_cnt;
    }
    if (mc_wm[i].nv_value < mc_wm[i].max_use)
    {
      mc_wm[i].nv_value = mc_wm[i].max_use;
      nv_update = TRUE;
    }
  }

  wm_value = (((mc_wm[MCWM_CMD_FREE_Q].nv_value <<
                 MCWM_CMD_FREE_Q_SHIFT_BITS) & MCWM_CMD_FREE_Q_MASK) |
                ((mc_wm[MCWM_RXTX_FREE_Q].nv_value <<
                 MCWM_RXTX_FREE_Q_SHIFT_BITS) & MCWM_RXTX_FREE_Q_MASK) |
                ((mc_wm[MCWM_RX_FREE_Q].nv_value <<
                 MCWM_RX_FREE_Q_SHIFT_BITS) & MCWM_RX_FREE_Q_MASK)    |
                ((mc_wm[MCWM_TRANS_FREE_Q].nv_value <<
                 MCWM_TRANS_FREE_Q_SHIFT_BITS) & MCWM_TRANS_FREE_Q_MASK));

  MSG_HIGH("MC queue watermark : 0x%x", wm_value, 0, 0);

  if (nv_update)
  {
    MSG_HIGH("MC queue watermark NV update to 0x%x", wm_value, 0, 0);
    mc_put_nv_item(NV_SPARE_2_I, (nv_item_type*)&wm_value);
  }

} /* mc_store_watermark */
#endif /* FEATURE_MC_QUEUE_WATERMARK */

/*===========================================================================

FUNCTION MC_CDMA_MULTIMODE_SANITY_ERR_FATAL

DESCRIPTION
  The function to call the err_fatal because of the reason passed in.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void mc_cdma_multimode_sanity_err_fatal
(
  mmoc_sanity_err_e_type reason
)
{
  /* Check the mc_tcb wait signal. If MC is exclusively waiting
  ** on MCC_SRCH_Q_SIG, ie, MC timeout because Search doesn't
  ** wakeup MC properly, let Search know that it's stuck.
  ** MCC_Q_SIG is a signal when an item is placed on any MCC queue.
  */
  #ifdef FEATURE_REX_SIGS_EXT
  if ( (mc_tcb.wait[0] == (MCC_RPT_TIMER_SIG | MCC_SRCH_Q_SIG | MCC_Q_SIG)) &&
       (mc_tcb.wait[1] == 0) ) 
  #else
  if (mc_tcb.wait == (MCC_RPT_TIMER_SIG | MCC_SRCH_Q_SIG | MCC_Q_SIG))
  #endif /* FEATURE_REX_SIGS_EXT */
  {
    MSG_ERROR("MC exclusively waits on Search", 0, 0, 0);
    
    /* No good srch_cmd_code to pass in, just use SRCH_MAX_F */
    srch_multimode_sanity_err_fatal(SRCH_MAX_F);
    return;
    
  }

  /* Check if MC is waiting on SRCH */ 
  #ifdef FEATURE_REX_SIGS_EXT
  if ( (mc_tcb.wait[0] == (MCC_CMD_SIG | MCC_RPT_TIMER_SIG)) &&
       (mc_tcb.wait[1] == 0) ) 
  #else
  if (mc_tcb.wait == (MCC_CMD_SIG | MCC_RPT_TIMER_SIG))
  #endif /* FEATURE_REX_SIGS_EXT */
  {
    if (( mcc_srch_buf.hdr.status != SRCH_DONE_S ) &&
        ( mcc_srch_buf.hdr.status != SRCH_TOOLATE_S ) &&
        ( mcc_srch_buf.hdr.status != SRCH_SLEEP_S)
       )
    {
      /* No good srch_cmd_code to pass in, just use SRCH_MAX_F */
      srch_multimode_sanity_err_fatal(mcc_srch_buf.hdr.cmd);  
      return;
    }
  }

  if ( MAIN_STATE ( cdma.curr_state ) != CDMA_IDLE )
  {
    MSG_MED("MMoc sanity timer expired: cmd %d, state %d", reason, 
      cdma.curr_state, 0);
  }
  else
  {
    switch ( reason )
    {
      case MMOC_SANITY_ERR_DEACT_CMD_CNF:
        /* The CDMA protocol did not process/respond to the MC_CDMA_PROT_DEACT_CMD_F
        ** The CDMA protocol should respond within 15 seconds, in case of
        ** deactivating because of power down it is 30 seconds.
        */
        ERR_FATAL( "CDMA did not respond to  deactivate command", 0, 0, 0 );
        break;

      case MMOC_SANITY_ERR_PH_STAT_CMD_CNF:
        /* The CDMA protocol did not process/respond to the MC_CDMA_PROT_PH_STAT_CHGD_CMD_F
        ** The CDMA protocol should respond within 15 seconds.
        */
        ERR_FATAL( "CDMA did not respond to phone status chgd command", 0, 0, 0 );
        break;

      case MMOC_SANITY_ERR_GENERIC_CMD_CNF:
        /* The CDMA protocol did not process/respond to the MC_CDMA_PROT_GEN_CMD_F( because
        ** of pref_sys changed or SD reselection timeout.
        ** The CDMA protocol should respond within 15 seconds.
        */
        ERR_FATAL( "CDMA did not respond to generic command", 0, 0, 0 );
        break;

      default:
       ERR_FATAL( "Unknown reason", 0, 0, 0 );
       break;

    } /* switch ( reason ) */
  }

} /* mc_cdma_multimode_sanity_err_fatal */

/*===========================================================================

FUNCTION MC_AMPS_MULTIMODE_SANITY_ERR_FATAL

DESCRIPTION
  The function to call the err_fatal because of the reason passed in.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void mc_amps_multimode_sanity_err_fatal
(
  mmoc_sanity_err_e_type reason
)
{
  switch ( reason )
  {
    case MMOC_SANITY_ERR_DEACT_CMD_CNF:
      /* The AMPS protocol did not process/respond  to the MC_AMPS_PROT_DEACT_CMD_F
      ** The AMPS protocol should respond within 15 seconds, in case of
      ** deactivating because of power down it is 30 seconds.
      */
      ERR_FATAL( "AMPS did not respond to deactivate command",
                 0, 0, 0
                );
      break;

    case MMOC_SANITY_ERR_PH_STAT_CMD_CNF:
      /* The AMPS protocol did not process/respond to the MC_AMPS_PROT_PH_STAT_CHGD_CMD_F
      ** The AMPS protocol should respond within 15 seconds.
      */
      ERR_FATAL( "AMPS did not respond to phone status chgd command",
                 0, 0, 0
                );
      break;

    case MMOC_SANITY_ERR_GENERIC_CMD_CNF:
      /* The AMPS protocol did not process/respond to the MC_AMPS_PROT_GEN_CMD_F( because
      ** of pref_sys changed or SD reselection timeout.
      ** The AMPS protocol should respond within 15 seconds.
      */
      ERR_FATAL( "AMPS did not respond to generic command",
                 0, 0, 0
                );
      break;

    default:
     ERR_FATAL( "Unknown reason", 0, 0, 0 );
     break;

  } /* switch ( reason ) */

} /* mc_amps_multimode_sanity_err_fatal */

#ifdef FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X
#error code not present
#endif /* FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X */

/*lint +e818 */

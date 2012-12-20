 /*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            TASK MAIN CONTROLLER

GENERAL DESCRIPTION

  This module provides none protocol specific power up initialization, and
  task related operations such as task define start, offline and stop.


EXTERNALIZED FUNCTIONS

  Task Entrance
    tmc_task

  TMC Tasks-Interface
    tmc_task_start    Procedure for tasks to acknowledge start up from TMC.
    tmc_task_ack      Procedure for task to acknowledge from TMC.
    tmc_task_stop     Procedure for task to do task stop processing from TMC.
    tmc_task_offline  Procedure for task to do task offline processing from
                      MC.
    tmc_cmd           Procedure to send a command to TMC from other tasks.
    tmc_ack           Procedure to send acknowledge signal to TMC from other
                      tasks.

INITIALIZATION AND SEQUENCING REQUIREMENTS

  This task is the first task created in the DMSS.  It is created from
  the main module (using the REX task initialization service) after boot
  processing is completed.  No other tasks or drivers have been created
  or started when this task is started.


Copyright (c) 2002,2003,2004,2005 by QUALCOMM INCORPORATED.
All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc1100/services/tmc/tmc.c#33 $ $DateTime: 2009/05/21 01:41:55 $ $Author: ovyas $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/07/09   pk      Moved support for storing MEID_ME out of TMC
03/30/09   pk      Added support to store MEID_ME for FEATUE_HWID_SUPPORT
02/24/09   hvo     Removed worked around for VREG MSMA
12/04/08   hvo     Increased S2 to 2.3 V due to RXADC issue
09/16/08   rp      Added FDI Task
06/27/08   apu	   Added support for FEATURE_VBATT_TASK	
09/08/08   hvo     Turned off MSMC, MSMA, RFA1 for PG2
08/27/08   hvo     Enable/Disable DCVS via NV, Kept MSME1 on for PG2
08/20/08   hvo     Supported KIP A1 Bring up, configured MSMC 1.3 V for MSM
07/30/08   ejh     Added  QSC11X0_PG2_POWER_GRID support
07/02/08   hs      Added support for FEATURE_CHG_TASK.
11/30/07   rjm     FEATURE_FM switch now independent of ODM builds
06/18/07   va      Added TCXO Manager Task
06/05/07   jd      minor of TMC wait count for CD task to support
06/05/07   jd      saving of GPS assistance data at shutdown.
05/23/07   cr      TLB lockdown of SDRAMC registers and CHIP_BASE.
01/16/07   fs      Added parb_init
08/30/06   pc      Removed support for boot_init_abnormal_hw_reset_detection
                   and boot_reset_early_error_detection.
08/30/06   pc      Provided support for FEATURE_TIME_TOD_AS_RTC_OFFSET.
08/29/06   pc      Removed support for FEATURE_BT_HS, FEATURE_BT_TEST_RC_2_SIO,
                   FEATURE_BT_LPOS and FEATURE_BT_EXTPF_TEST.
08/24/06   pc      Added feature FEATURE_SEC_PROCESS_INIT around certain SEC 
                   related tasks.
08/24/06   pc      Added ifndef FEATURE_MULTIPROCESSOR around QIPCALL.
08/22/06   pc      Added calls to boot_reset_early_error_detection() and 
                   boot_init_abnormal_hw_reset_detection() and included 
                   boot_hw_reset.h
08/14/06   pc      Added feature FEATURE_QTV_STATIC_TASKS.
08/09/06   pc      Added support for QTV_VIDEO_RENDERER and QTV_AUDIO
08/01/06   pc      Added support for FEATURE_IXDESCRAMBLERSVC
07/25/06   pc      Added support for FEATURE_CPU_BASED_FLOW_CONTROL
07/24/06   pc      Added inclusion of mao.h and invocation of mao_init()
07/17/06   pc      Added support for FEATURE_DMTASK.
07/14/06   pc      Added featurized code to turn on/off handling misaligned 
                   memory access in L4.
06/19/06   pc      Added support for FEATURE_SEC_USFS, 
                   FEATURE_SEC_CONTENT_AGENT, FEATURE_IXSQLSVC and SECDiag.
05/06/06   anb     Added code support to detect abnormal HW reset.
05/05/06   ac      Added support for FEATURE_QDJTEST
04/19/06   bt      Added UBM L1 task.
03/29/06   ptm     Added init calls for spinlocks and smem logging for 7K.
03/24/06   pc      Added support for SECRND task
03/23/06   cl      Added PDSM_COMM_TASK
03/10/06   xz      Changed QVPHONE related code for 7500 support
01/25/06   ddh     Added DEM Task
01/24/06   ajn     Enabled GRAPHICS Clock regime
01/23/06   rlp     Added HID Support.
01/02/06   dle     Added FEATURE_CAMERA_DRV_TASK.
12/14/05   taw     Pmem is now initialized with the real modem heap in SMI.
11/10/05   pc      Added support for AVS_SBCENC_TASK
11/08/05   pc      Added support for GSM_PLT task
11/02/05   gw      Added support for GameMode.
10/26/05   ck      Fixed a compiler issue.
10/19/05   as      Fixed compiler warnings.
10/03/05   dd      Added L4 call to enable timetest.
08/01/05   vs      Added usbhost_api_init based on feature FEATURe_USB_HOST
08/26/05   pc      Added support for DISP_TASK.
08/20/05   pc      Renamed FEATURE_PROF_DIAG to FEATURE_PROFDIAG
08/15/05   hn      Replaced use of FS_FEATURE_RMTEFS_SERVER with FS_FEATURE_IG_IPC_SERVER
08/10/05   taw     Added a call to flash_nand_init() to init the flash driver
                   and HW.  Added a call to pmem_init() to allow pmem use
                   by the flash driver.
08/10/05   pc      Increased the wait_cnt for the UI task in tmc_stop_tasks()
                   to 5s, for Sirius.
08/09/05   fl      Replaced file qvp_aud_eng.h with extern void qvp_ae_task()
08/05/05   jhs     Removed SMEM references.  Added SMD include file.
08/05/05   jhs     Removed SMEM references.  Added SMD include file.
08/03/05   pc      Replaced certain FEATURE_L4 with FEATURE_L4LINUX, added
                   support for FEATURE_USB_HOST_API, included
                   FEATURE_TCXOMGR_TMC.
08/01/05   vs      Added usbhost_api_init based on feature FEATURe_USB_HOST
07/19/05   taw     Added includes for flash functions and removed NANDC MPU
                   usage for modem only builds.
07/18/05   et      added PARK MODE support
07/15/05   adm     Merged from mainline ver #23
07/13/05   adm     Merged in changes related to supporting BootLoader compilation
                   for L4 builds
07/12/05   jz      Featureized QVP APP Task for Codec Only
07/12/05   grl     Added initialization of TRM for 1x targets.
06/20/05   fl      Replaced file sipt_task.h with extern void sipt_task()
06/17/05   fl      Replaced file qvp_encvfe.h with extern void qvp_vfe_task()
06/13/05   pc      Added support for WLAN sec task
06/13/05   sl      Sirius changes: Task Reordering; Secure clock API.
06/17/05   jhs     Removed time_tod_task.
06/07/05   jhs     Added pm_pwr_key_init for power key support.
06/01/05   grl     Removed srm/xrm initialization since they aren't used.
05/23/05   cep     Added aps_init for tmc_libraries_init_before_tasks_start
05/12/05   pc      Added call to time_set_from_pmic() to get the time from
                   PMC RTC for Time Services.
05/02/05   ibm     Moved UI initialization past DS and GSDI for Sirius.
04/21/05   pc      Added support for FEATURE_MFLO
04/20/05   pc      Added support for FEATURE_QIPCALL
04/09/05   pc      Added support for CNV task
04/14/05   sr      Remove the featurization for adie_init call
05/19/05   lp      Removed HWIO PMIC_SSBI call that enables MDDI
05/18/05   ~SN     Removed clk_regime_msm_init() & clk_regime_msm_enable (CLKRGM_PRPH_CLK).
05/18/05   jhs     Added time_tod_task.
04/05/05   ddh     Replace OKTS MASK with OKTS SIG
03/28/05   sv      More Update Abnormal Exit count - Sirius specific change
03/21/05   sv      Update Abnormal Exit count - Sirius specific change
03/15/05   wal     Added APS task for L4/Linux under FEATURE_APS
03/15/05   adg     Added QDIAG task for L4/Linux under FEATURE_IG_QDIAG
03/15/05   jqi     Added support for FS_FEATURE_RMTEFS_SERVER, FEATURE_L4
                   FEATURE_PM_SERVER,FEATURE_IG_UI_CLIENT_SELECT
03/07/05   lyr     Moved WLAN task startup before DS task startup
02/24/05 t_jarmis  Added FEATURE_FM for FM-RDS Radio.
02/14/05   jqi     Updated for FEATURE_BATTERY_CHARGER.
02/04/05   jqi     Fix the bug in the tmc processing stop/offline tasks.
                   Revised the change for TMC vote for sleep.
                   Send stop/offline signal to sleep task.
02/05/05   aku     Added support for WLAN Adapter initialization in TMC.
02/04/05   dwp     Added QVP support for SIP.
02/03/05   jqi     Added support for TMC vote for sleep.
02/03/05 eprice    Addition of Sirius specific tasks using define SIRIUS_PLATFORM
01/31/05   jqi     Added support for FEATURE_CC_MONITOR, FEATURE_FOTA_FS,
                   FEATURE_ENABLE_FLOATING_POINT_ERRORS.
05/06/05   ptm     Moved dmov_init to before time_init to ensure it completes
                   before apps dmov_init starts.
04/29/05   jc      Added PMIC support to enable MDDI power on SURF.
04/22/05   ptm     Moved smem and oncrpc tasks to top of task def routine.
                   Moved timer sync code to the timer subsystem.
04/24/05   ~SN     Added FEATURE_BRINGUP_DIAG_ONLY
04/14/05   adm     Added smd task under FEATURE_SMD
03/24/05   jhs     Moved i2c_init above sio_init becuase USB uses I2C.
03/21/05   smkg    Update the function call to clk_regime_msm_init.
03/14/05   jhs     Added support for multiprocessor slow clock sync.  Again
                   moved rdm_get_prev_ports and rdm_issue_opens as per
                   comment 12/06/04, ptm.
01/28/04   ptm     Remove references to sta_remote_task and add feature
                   protection around sta references.
01/21/05   ~sn     Added DMOV INIT
12/27/04    ro     Add offline and stop sigs for VOC, QDSP, QDSP_RTOS.
12/06/04   ptm     Moved the start up ONCRPC of to tmc_start_service_tasks and
                   rdm_get_prev_ports and rdm_issue_opens up to
                   tmc_libraries_init_after_service_tasks_start so that
                   ONCRPC is ready to handle RPC calls before any of the
                   application tasks are started.
                   Moved #ifndef T_RUMI2_DIAG_ONLY around call to
                   tmc_libraries_init_after_service_tasks_start into the
                   routine so that only part of the routine is protected.
12/03/04    lp     Enabled QDSP task by removing T_RUMI_QDSP.  Fixed compile
                   error when TIMETEST is defined.
11/24/04    cr     Add smem_boot_sync to release aARM to begin executing
                   Reset_Handler.
11/08/04    ro     Added FEATURE_QDSP_RTOS - start ADSP RTOS Task
10/13/04   adm     Added smem task
01/25/05   jqi     Fixed constantly high sleep current issue.
01/21/05   ldaita  Added support MGPCC task (needed for GPS).
01/19/05   aar     Fixed compilation problem due to rex_def_task2 I/F mismatch.
01/14/05   rp      Added background loading in Progressive boot task support
           jh      Added support for enabling profiling through diag interface.
01/12/05   jqi     Lint fixes.
01/06/05   aar     Added support for GPSSRCH demod task.
12/23/04   ty      Changed ran_seed() use to ran_init() to initialize seed
11/22/04   gfr     Suppress intlock profiling during init.
11/10/04   jz      Added QVP VFE Task
11/18/04   pa      Added support for PBM task.
11/02/04   jqi     Fixed compilation warning and featurization issue.
                   updated comments.
10/22/04   jqi     Featurization fixes.
10/20/04   ajn     Added support for TIMER task.
10/15/04   rp      Added support for page and interrupt tasks.
10/08/04   jqi     Added support for FEATURE_HSDPA
09/29/04   ss      Added support for DTV (Digital TV) task
09/29/04   jqi     Fixed ADS1.2 and RVCT compilation warnings.
09/24/04   gfr     Rename feature
09/16/04   gfr     Added support for tcxo manager init and powerdown.
09/16/04   jqi     Added ftm_task.h file.
09/10/04   hl      Added support for joystick feature
08/25/04   jqi     Fixed compilation errors/warnings.
08/23/04   vr      Replaced include dsbcdb_api.h by dsbcdb_task.h
08/11/04   efy     Unified adie_init() interface.
08/05/04   ap      Moved UI task to stop last.
07/26/04   jqi     Added support to read GPS nv items during power up.
07/22/04   jqi     Added support for WPLT and GPLT.
07/13/04   ajn     Added initialization for SRM/XRM
07/09/04   jqi     Corrected the parameters for tmc_realloc() API.
                   Added support for FEATURE_ADC_CALIBRATION.
06/24/04   jm      Used prototype directly for DRM task.
06/21/04   dc      Wrapped#include "acpwb.h" with FEATURE_ACP.
06/16/04   bd      Unconditional include "adie.h" regardless of
                   FEATURE_WCDMA_PLT.
06/10/04   jqi     Updated the history comments.
06/09/04   jl      Featurized CLK_RGM_DFM_M regime enable function with
                   FEATURE_ACP.
05/25/04   jqi     Fixed mode preference reset issue after power up.
           et      Unified adie init interface for all the targets.
05/19/04   vr      Added support for BCDB and HDRBC tasks
           jqi     TMC HEAP API.
                   Bump up tlmm_init() before rflib_init().
           ml      Added FEATURE_UI_CORE_REMOVED.
04/27/04   jqi     Back up the changes to rename time.h to time_svc.h.
04/23/04   jqi     Read Vbatt, therm and HW config during power up regardless
                   of multimode RF feature.
04/07/04   tg      Added support for FEATURE_ERR_EXTENDED_STORE.
                   Removed support for FEATURE_LOG_EXCEPTIONS_TO_NV.
04/06/04   jqi     Added FEATURE_BREW_PWRUP_INIT.
04/01/04   sg      removed support for LP task as task no longer exists.
03/15/04   tg      Added support for FEATURE_LOG_EXCEPTIONS_TO_NV.
03/12/04   st      Removed MSM6500 work around when creating MDSP task.
02/23/04   jqi     Fixed some typo in FEATURE_SEC_DRM.
           bs      Added support for Bluetooth External Profiles feature support .
02/19/04   jqi     Fixed Lint error
           ak      Added support for Brew 3.0.
           Jm      Added support for FEATURE_SEC_DRM.
02/12/04   vd      Added support for FEATURE_GSM_CB.
02/09/04   ajn     Renamed time.h to time_svc.h to avoid OS conflict.
                   Moved time_init() below tramp_init(), since it uses tramp.
02/06/04   et      Added support for DRM feature.
02/02/04   dd      Called adie_init after rflib_init and before voc task is
                   started for 1x target.
01/19/04   jqi     Fixed compile warninigs.
12/03/03   bt      Move srch4 registration into
                   tmc_libraries_init_after_service_tasks_start.
11/26/03   jqi     Remove update rf init status based on ftm nv flag.
11/20/03   jqi     Added support for SYS_OPRT_MODE_OFFLINE_IF_NOT_FTM.
11/19/03   jqi     Added support for FEATURE_ENHANCED_REX_TASK.
11/14/03   dd      Added support for GPS Search task.
11/12/03   dwp     Additional QVPHONE updates.
10/31/02    sr     QVPHONE changes merged for Saber. SBI API's rolled into RF
                   VU's.
10/29/03   jqi     Consolidate MSM6200 target related features.
10/22/03   ts      re-organization in PDSM files.
10/17/03   jm      Fixed typo.
10/13/03   jqi     Fixed compile warning.
10/07/03   sa      CR30885-Add diag request to check the size of TMC heap
10/06/03   jqi     Fixed multimode RF enter mode issue.
09/29/03   dna     Get T_QUASAR to compile and work again for MSM6500
09/21/03   bt      Download SRCH4 App after all tasks have been started.
08/14/03   ht      Merged MSM6500 changes and QUASAR support.
07/21/03   xfg     Changed for Thin UI build and set lock to memory heap.
07/14/03   jqi     Swapped the tlmm_init() with sio_init().
07/09/03   cd      CR32650-Replace FEATURE_UIM_USIM withe FEATURE_UIM.
07/01/03   jqi     Added support for FEATURE_RESET_COUNTER.
06/19/03   ev      Added support for MSM6500 target for power up
                   initialization.
06/12/03   rtp     Changed the FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING to
                   FEATURE_AUTOMATIC_CLOCK_FREQUENCY_CHANGING
05/30/03   jqi     Renamed TIMETEST_SECIPS_TASK_ID to TIMETEST_SECIPSEC_TASK_ID.
05/29/03   jqi     CR28002
                   Show task name which does not responds offline cmd in LCD.
           Om      Added support for ARM_CRYPTO and IPSEC tasks,
05/28/03   TMR/dgy Added changes for WCDMA single-mode build.
05/14/03    ld     Added support for MGP related tasks
04/30/03   jqi     Added !FEATURE_MDSP_TASK around included file dmddown.h
04/26/03   jqi     Delete enable all clk regimes for leopard target.
04/25/03   ts      Swapped the tlmm_init with uim_dev_init().
04/23/03   rtp     Modified the code under
                   FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING
04/22/03   jqi     Removed TMC heap to tmc_nzi.c.
                   Added include file virtuim.h under FEATURE_VIRTUAL_SIM.
04/17/03   dgy     Used dsgcsd_task.h instead of dsgcsd.h.
04/16/03   jqi     Removed test_mc.h and used ftm.h for cougar,leopard,
                   cheetah and saber targets.
                   Removed T_RF_ZRF6200 feature around sbi_init().
04/14/03   jqi     Revised MM RF driver initialization for cougar.
                   Only did once during power up in tmc.c and removed codes
                   in mc.c
04/04/03   jqi     Moved stack profiling inside of tmc_define_tasks().
           bd      Added WPLT support for PMIC
           cd      Added uim_power_control() in power off and reset.
04/01/03   jqi     Rename TIMETEST_GCDS_TASK_ID to TIMETEST_GCSD_TASK_ID.
03/25/03   jqi     Increased GSTK,UIM,GSDI stop time out to 5 seconds.
                   Added definition of tmc_get_mem_heap_size().
03/21/03   jqi     Removed technology features around adie_init().
                   Moved stop UIM task aster GSDI and GSTK tasks.
03/14/03   jqi     Added FEATURE_MULTIMODE_ARCH around mmoc task.
                   Added FEATURE_CM around sending cm offline command.
03/12/03   jqi     CR28464 - added tasks' name.
03/11/03   jqi     CR28438 - moved err_init() after nv_task starts.
03/05/03   jqi     If RF CAL is invalid and FTM flag is invalid,
                   set ftm flag to true.
                   Clearn up MMRF interface for Cougar, Leopard and Cheetah.
03/04/03   at      Changes to resolve naming conflict with MMGPS defined types
                   in mmgps_types.h
                   1. Replace nf.h, cd.h (pe.lib's internal include files) with pe_task.h
                   2. Replace pb.h with pb_task.h
03/04/03    lz     Removed FEATURE_MOBILE_SUITE.
02/28/03   jqi     Set ftm mode default value amss_mode for cheetah (temporary fix)
02/27/03   jqi     Set ftm mode default value ftm_mode.
02/26/03   jqi     Without feature FEATURE_DIAG_COUPLED_ARCHITECTURE, diag no
                   longer wait for complete signal during offline process.
02/21/03   jay     Added support for FEATURE_SEC_SSL.
                   Clear task start signal in tmc_start_task().
02/19/03   sj      Added support for LPM.
02/19/03   jqi     Added support for FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING.
02/18/03   jqi/jar Added Generic SIM Tookit Task (FEATURE_GSTK) Support and
                   Virtual Sim Application Toolkit (FEATURE_VIRTUAL_SAT) Task.
02/14/03   jqi     Added support for FTM task
                   If nv ftm flag is set true, do not send offline command.
                   Remove unfinished LPM support.
                   Added support for FEATURE_MULTIMODE_APIRF.
02/07/03   jqi     Added support for LPM.(not finished yet)
                   Added support for stack profiling.
                   Expanded FEATUREs associated with GSDI task.
02/06/03   jqi     FTM support with all tasks start for UMTS target.
                   Use chg_init(TRUE) for UMTS target.
01/29/03   jqi     Fixed a typo.
01/28/03   jqi     Called ftm_tmc_init() on behalf of FEATURE_FACTORY_TEST_MOE.
01/27/03   sj      Moved starting of UI/CM/MMOC etc tasks after initializing
                   rf so that offline is the first command in the CM queue.
01/22/03    lz     Added support for FEATURE_MOBILE_SUITE.
01/21/03   jqi     Featurize the UMTS FTM change with T_RF_ZRF_6200
                   Rename MC_RPT_TIMER_SIG with TMC_RPT_TIMER_SIG.
                   Remove mc.h
01/20/03   jqi     Removed feature around rf_init() and rf_init_delayed().
01/15/03   jqi     Merged with MSM6200 TMC #68.
                   1.added "clkrtc.h"
                   2.New feature support
                     FEATURE_VDSP_AT_48MHZ
                   3.A temporary support for UMTS FTM: do not start all tasks
                   Featurized the clk_regime_enable() for only 6300 target.
12/31/02   jqi     Merged with MSM6200 TMC #62.
                   1.added tmc_heap_small.
                   2.New feature support
                     FEATURE_GSM_GPRS
                     FEATURE_RF
                     FEATURE_MVS
                     FEATURE_UIM_USIM
                     FEATURE_HS_DETECT_BASEBAND_TYPE
                     FEATURE_PDAPI
                     DEBUG_TEST_RF
                   3.added support for FEATURE_WCDMA_PLT, FEATURE_GSM_PLT
                   4.added support for FEATURE T_RF_ZRF6200 during power up HW initialication
                   5.remove ftm_tmc_init to where ftm_mode is read.
12/09/02   jqi     Remove test_mc.h out of FEATURE_GSM.
12/05/02   jqi     Fixed compile errors.
12/04/02   jqi     Added support for multimode FTM.
                   -Start all tasks in FTM mode.
                   -use test_mc.h instead of ftm.h
                   -add ftm_init in tmc_libraries_init_before_tasks_start();
11/06/02   jqi     Featurized the temporary fixes for MM combined builds.
11/05/02   jai     A temporary fixes for MM combined builds.
                   - Removed adie_init(), rf_init, rf_init_delayed,
                   - Added enable all clk regimes during Power up.
11/01/02   jqi     Added and featurized pm1000_init().
10/24/02   dle     Removed grp_init. It should only be called from disp_init.
10/21/02   jqi     Added support for FTM mode.
10/17/02   jqi     Remove bio_init on behalf of GSM feature.
                   Added support for GSM FTM mode support.
10/15/02   jqi     Added support for GSM FTM mode support.
10/02/02   jqi     Use TIMETEST_TMC_TASK_ID for TMC task.
                   Change TIMETEST_RR_TASK_ID to TIMETEST_GSM_RR_TASK_ID.
                   Updated timetest.h and hp_stats.ini with missing TIMETEST task IDs.
09/27/02   jkl     Fixed featurization in ZIF libraries.
           jqi     Added GSDI task in offline/stop handler.
                   Featurized dog report code.
                   Included WCDMA task header files.
09/26/02   dle     camera_init moved to graph_task.
09/26/02   jqi     Included target.h header file.
09/25/02   rmd     Moved pm init functions before adie init.
           jqi     Used timed_wait for ack signal when sending offline/stop
                   signals.Removed reset work around for cougar.
09/24/02   rmd     Added chg_init() with FEATURE_BATTERY_CHARGER.
09/18/02   jqi     Removed the redundant enc_init().
09/17/02   jqi     Fixed error when creating GCSD task.
09/17/02   hr      Added reset work around for cougar.
09/17/02   cyran   Complete the HWIO port such that MSM6000, MSM6050, and MSM6100
09/14/02   jqi     Removed custcmx.h.
09/13/02   jqi     Removed support for FTM mode.
09/10/02   jqi     Featurized HW related header files for unit test environment.
                   Added support for camera and graph.
09/06/02   jqi     Change tmc_wait() return type to void to support MM RF.
09/06/02   jqi     Change gsdi_task_stack to gsdi_stack.
08/27/02   jqi     Added support for powerup init with FEATURE_MDSP_TASK.
                   Require RF changes.
                   Replaced GSDI feature with  FEATURE_MMGSDI.
                   Replaced TMC_WMS with FEATURE_CDSMS || FEATURE_GWSMS.
                   Moved read ESN ME to start service tasks.
                   started wms task after ui task started.
08/21/02   jqi     Reverse the sequence of srch_util_demod_reset and
                   demod_down_image.
08/19/02   jqi     Replace tempore uim feature with FEATURE_UIM.
                   Treated GSM/WCDMA task as application tasks.
                   Added support for timetest profile.
                   Treated tlmm and I2C as generic HW.
                   Reverse the sequence of srch_util_demod_reset and
                   demod_download_image.
                   Remove clkrgm_6050.h header file.
07/26/02   jqi     Featurize header files
                   Used FEATURE_DATA_PS for ps task.
                   Remove support for cdma.esn_me
07/25/02   jqi     Added temporary support for cdma.esn_me.
                   Added support for MDSP task.
07/24/02   jqi     TMC reorganization.
07/09/02   jqi     Changes for code review.
07/02/02   jqi     Changes per 4th week of I&T
06/19/02   jqi     Added sending DIAG_OFFLINE_COMPLETE_SIG in the end of
                   offline procedure.
06/14/02   jqi     Changes per 1st week of I&T.
06/06/02   jqi     Initial Release

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"                                  /* Target header file */

#include "tmcnv.h"                         /* TMC NV-Interface header file */
#include "tmc.h"                       /* Task Main Controller header file */
#include "tmci.h"              /* Task Main Controller interal header file */
#include "task.h"                                      /* Task header file */
#include "AEEstd.h"
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
#if (TG!=T_PC)                /* Define for all targets except Unit Test  */
#include "tramp.h"                      /* Interrupt trampline header file */
#include "hw.h"                                    /* Hardware header file */
#include "gpio_int.h"                             /* GPIO Init header file */
#include "clk.h"                                      /* Clock header file */
#include "bio.h"                                     /* Bit IO header file */
#include "clkregim.h"                             /* CLK Regim header file */
#include "parb.h"                            /* Pacing Arbiter header file */
#if defined(FEATURE_CACHE_WT)
#include "nvim_items.h"                     /*For cache_wt*/
#endif
#include "BREWVersion.h"

#if ((defined(FEATURE_QVPHONE)) && (!defined(T_MSM7500)) && (!defined(T_MSM7200)))
#include "qvp_codec.h"
#include "qvpapp_task.h"
#ifdef FEATURE_QVPHONE_SIP
#error code not present
#endif
#endif

#include "rflib.h"


extern int cacheWT;




#ifdef FEATURE_I2C
  #include "i2c.h"                                      /* I2C header file */
#endif /* FEATURE_I2C */

  #include "ran.h"                                   /* random header file */

#ifdef FEATURE_PMIC_RTC
  #include "clkrtc.h"    /* clk */
#endif

#endif /* TG!=T_PC */

#include "sio.h"                                        /* SIO header file */

#if ( defined(RF_MULTIMODE) || defined (FEATURE_MULTIMODE_RFAPI) )
  #include "rfm.h"
#else /* ! RF_MULTIMODE && ! FEATURE_MULTIMODE_RFAPI */
  #include "rf.h"
#endif /* RF_MULTIMODE || FEATURE_MULTIMODE_RFAPI */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#ifdef FEATURE_DMOV
  #include "dmov.h"
#endif

#ifdef TMC_SLEEP
  #include "sleep.h"                                  /* Sleep header file */
#endif /* TMC_SLEEP */

#ifdef FEATURE_APPS_TCXO_SLEEP
  #include "dem.h"                                      /* DEM Header file */
#endif /* FEATURE_APPS_TCXO_SLEEP */

#ifdef FEATURE_DOG
  #include "dog.h"                           /* Watch Dog Task header file */
#endif /* FEATURE_DOG */

#if !defined(FEATURE_THIN_UI) && !defined(FEATURE_UI_CORE_REMOVED)
#ifdef FEATURE_HS
  #include "hs.h"                                   /* Handset header file */
#endif /* FEATURE_HS */
#endif /* !defined(FEATURE_THIN_UI) && !defined(FEATURE_UI_CORE_REMOVED) */

#ifdef FEATURE_IS2000_TDSO                             /* TDSO header file */
  #include "tdso.h"
#endif /* FEATURE_IS2000_TDSO */

#ifdef FEATURE_EFS                                      /* EFS header file */
  #include "fs.h"
  #if (defined( FEATURE_EFS2 ) && !defined(FEATURE_WCDMA_PLT) && !defined(FEATURE_GSM_PLT))
    #include "keypad.h"
    extern int fs_fresh_start;
  #endif
#endif /* FEATURE_EFS */

#ifdef FEATURE_EFS_FDI                                      /* EDI header file */
  #include "fs_fdi.h"
#endif /* FEATURE_EFS_FDI */

#ifdef FEATURE_EFS_SFAT                                /* SFAT header file */
  #include "fs_sfat.h"
#endif /* FEATURE_EFS_SFAT */

#ifdef FEATURE_NV
  #include "nv.h"                                        /* NV header file */
#endif /* FEATURE_NV */

#ifdef FEATURE_UI
  #include "ui.h"                                        /* UI header file */
#endif /* FEATURE_UI */

#ifdef FEATURE_CM
  #include "cm.h"                                        /* CM header file */
  #include "cmtask.h"
#endif /* FEATURE_CM */

#include "mmoc.h"                                      /* MMOC header file */

#ifdef FEATURE_QDSP_TASK
  #include "qdsp.h"                                    /* QDSP header file */
#endif /* FEATURE_QDSP_TASK */

#ifdef FEATURE_QDSP_RTOS
  #include "adsprtossvc.h"
#endif /* FEATURE_QDSP_RTOS */

#ifdef FEATURE_VOC_TASK
  #include "voc.h"                            /* QDSP2 vocoder header task */
#endif /* FEATURE_VOC_TASK */

#ifdef FEATURE_SND_TASK
  #include "snd.h"                                    /* Sound header file */
#endif /* FEATURE_SND_TASK */

#ifdef FEATURE_AVS_SBCENC_TASK 
  #include "audiosbc.h"
#endif

#ifdef FEATURE_HDR                                     /* HDR header files */
#error code not present
#endif /* FEATURE_HDR */

#ifdef FEATURE_HDR_BCMCS
#error code not present
#endif /* FEATURE_HDR_BCMCS */

#if (defined(FEATURE_GPSONE) && !defined(FEATURE_CGPS))  /* GPSONE related header files */
#error code not present
#endif /* FEATURE_GPSONE && !FEATURE_CGPS */

#ifdef FEATURE_BT                        /* Bluetooth related header files */
  #include "bt.h"
  #include "bti.h"
  #ifdef FEATURE_BT_EXTPF
    #include "btpfi.h"
  #endif /* FEATURE_BT_EXTPF */
#endif /* FEATURE_BT */

#ifdef FEATURE_XO
#include "tcxomgr_task.h"
#endif /* FEATURE_XO */

#ifdef SIRIUS_PLATFORM
#error code not present
#endif /* SIRIUS_PLATFORM */

#ifdef FEATURE_FM                            /* FM-RDS related header files */
  #include "fmrds_api.h"
#endif // FEATURE_FM

#ifdef FEATURE_ATS
#include "ats_task.h"
#endif

#ifdef FEATURE_DIAG_TASK
  #include "diag.h"                                    /* Diag header file */
  #include "event.h"                                  /* Event header file */

  #ifndef FEATURE_DIAG_COUPLED_ARCHITECTURE
    #include "diagi.h"                        /* Diag internal header file */
  #endif /* FEATURE_DIAG_COUPLED_ARCHITECTURE */
#endif /* FEATURE_DIAG_TASK */

#ifdef FEATURE_BCMCS
#error code not present
#endif /* FEATURE_BCMCS */

#ifdef FEATURE_WLAN
#error code not present
#endif  /* FEATURE_WLAN */

#ifdef FEATURE_DATA
  #include "dstask.h"
  #ifdef FEATURE_COMP_TASK
    #include "comptask.h"
  #endif /* FEATURE_COMP_TASK */

  #ifdef FEATURE_WCDMA /* && ( defined FEATURE_DATA_WCDMA_CS ) ) */
#error code not present
  #endif /* FEATURE_WCDMA */

  #if ( (defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS)) && !defined(FEATURE_WCDMA_PLT) && !defined(FEATURE_GSM_PLT))
    #include "dsgcsd_task.h"
  #endif /* (FEATURE_DATA_GCSD || FEATURE_DATA_WCDMA_CS) && !FEATURE_WCDMA_PLT */

/* Need verify if G/W need FEATURE_DATA_PS && FEATURE_DATA to include ps.h
*/
  #ifdef FEATURE_DATA_PS
  #include "ps.h"
  #endif
#endif /* FEATURE_DATA */

#ifdef FEATURE_QMIP
#error code not present
#endif /* FEATURE_QMIP */

#ifdef FEATURE_REX_IPC                                /* IPC header file */
  #include "ipcmem.h"
  #include "tsvc.h"
  #include "rteprocess.h"
#endif /* FEATURE_REX_IPC */


#ifdef FEATURE_ONCRPC                                /* ONCRPC header file */
#error code not present
#endif /* FEATURE_ONCRPC */

#ifdef FEATURE_IPC_TEST                                /* IPC header file */
#error code not present
#endif /* FEATURE_IPC_TEST */

#ifdef FEATURE_DS_SOCKETS_TEST_TCP                  /* TCPTEST header file */
#error code not present
#endif /* FEATURE_DS_SOCKETS_TEST_TCP */

#ifdef FEATURE_DS_SOCKETS_TEST_UDP                  /* UDPTEST header file */
#error code not present
#endif /* FEATURE_DS_SOCKETS_TEST_UDP */

#ifdef FEATURE_SEC                                  /* SECTASK header file */
  #include "sectask.h"
  
  #ifdef FEATURE_SECRND
    #include "secrndtask.h"
  #endif

  #ifdef FEATURE_SEC_USFS
    #include "sfs_svc.h"
  #endif

  #ifndef FEATURE_SEC_PROCESS_INIT
    #ifdef FEATURE_SEC_CONTENT_AGENT
      extern void SECDiagServer (dword dummy );
    #endif

  #ifdef FEATURE_SEC_ARM_CRYPTO
    extern void seccryptarm_task( dword dummy );
  #endif
  #endif /* FEATURE_SEC_PROCESS_INIT */

  #ifdef FEATURE_SEC_IPSEC
    #include "secipstask.h"
  #endif

  #ifdef FEATURE_SEC_SSL                              /* SSLTASK header file */
    #include "secssltask.h"
  #endif /* FEATURE_SEC_SSL */

  #ifdef FEATURE_SEC_DRM
#error code not present
  #endif /* FEATURE_SEC_DRM */

#endif /* FEATURE_SEC */

#if (defined(FEATURE_SEC_SSL_TESTAPP) || defined(FEATURE_SEC_CRYPT_TESTAPP))
#error code not present
#endif /* FEATURE_SEC_SSL_TESTAPP || FEATURE_SEC_CRYPT_TESTAPP */

#if defined (FEATURE_AUTH) || defined (FEATURE_RUIM)
  #include "auth.h"                                    /* AUTH header file */
#endif /*(FEATURE_AUTH) || (FEATURE_RUIM)*/

#ifdef FEATURE_UIM
  #include "uim.h"                                     /* UIM header files */
#endif /* FEATURE_UIM */

#ifdef FEATURE_UIM_RUIM
  #include <memory.h>
  #include "nvruimi.h"                            /* UIM RUIM header files */
#endif /* FEATURE_UIM_RUIM */

#ifdef FEATURE_NAS
#error code not present
#endif /* FEATURE_NAS */

#ifdef FEATURE_DH                                        /* DH header file */
  #include "dh.h"
#endif /* FEATURE_DH */

#ifdef FEATURE_GRAPH_TASK
  #include "graph.h"                                  /* GRAPH header file */
#endif /* FEATURE_GRAPH_TASK*/

#ifdef FEATURE_CAMERA_DRV_TASK
  #include "camera_drv.h"
#endif /* FEATURE_CAMERA_DRV_TASK*/

#ifdef FEATURE_VS_TASK
  #include "vs.h"                                        /* VS hezder file */
#endif

#ifdef FEATURE_USB                                    /* USBDC header file */
  #include "usbdc.h"
  #ifdef FEATURE_USB_HOST
    #include "usbhost_api.h"
  #endif
  #ifdef FEATURE_USB_HOST_ENHANCED
    #include "usbhc_urb_task.h"
    #include "usbhc_dev_task.h"
  #if defined(FEATURE_USB_HOST_TEST_ENHANCED) && \
      (defined(FEATURE_USB_HOST_MASS_STORAGE_TEST_ENHANCED) || \
       defined(FEATURE_USB_HOST_HID_TEST_ENHANCED))
    #include "usbhc_test_task.h"
  #endif
  #endif
#endif /* FEATURE_USB */

#if defined(FEATURE_HS_USB)
  #if (defined(FEATURE_STANDALONE_MODEM) || !defined(FEATURE_NO_USB_HW_ON_PROCESSOR))
    #include "usbhost_api.h"
    #ifdef FEATURE_HS_USB_MS_FD	
      #include "hsu_al_ms.h" /* Mass storage adaptation layer header file */
    #endif
    #include "hsu_al_task.h"
    #include "hsu_config_selector.h"
  #endif /* FEATURE_STANDALONE_MODEM || !FEATURE_NO_USB_HW_ON_PROCESSOR */
#endif /* FEATURE_HS_USB */

#ifdef FEATURE_SMD
/*Include SMD's main interface header file*/
#include "smd_task.h"
#endif /* FEATURE_SMD */

#ifdef FEATURE_TC
  #include "tctask.h"
#endif /* FEATURE_TC */

#ifdef FEATURE_RUNTIME_DEVMAP
  #include "rdevmap.h"               /* Run Time Device Mapper header file */
#endif /* FEATURE_RUNTIME_DEVMAP */

#ifdef FEATURE_TC
  #include "tctask.h"
#endif /* FEATURE_TC */

#ifdef FEATURE_MMGSDI
  #include "gsdi_exp.h"
#endif /* FEATURE_MMGSDI */

#if ( defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS) )
  #include "wmsi.h"
#endif /* FEATURE_CDSMS || FEATURE_GWSMS */

#ifdef TIMETEST
  #include "timetest.h"
  #ifdef FEATURE_L4
#error code not present
  #endif
#endif /* TIMETEST */

#ifdef FEATURE_MDSP_TASK
#include "mdsp.h"
#endif /* FEATURE_MDSP_TASK */

#ifdef FEATURE_MMGPS
#error code not present
#endif /* FEATURE_MMGPS */

#ifdef FEATURE_MGP
#ifdef FEATURE_MGPCC_TASK
#include "cc_task.h"
#endif
#include "mc_task.h"
#include "nf_task.h"
#include "cd_task.h"
#include "ai_task.h"
#endif /* FEATURE_MGP */

#ifdef FEATURE_CGPS
#error code not present
#endif /* FEATURE_CGPS */

#ifdef FEATURE_BATTERY_CHARGER
  #include "charger.h"
#endif

#ifdef FEATURE_CHG_TASK
  #include "chg_task.h"
#endif /* FEATURE_CHG_TASK */

#ifdef FEATURE_FACTORY_TESTMODE
  #include "ftm.h"
#endif /* FEATURE_FACTORY_TESTMODE */
#ifdef FEATURE_FTM_TASK
  #include "ftm_task.h"
#endif /* FEATURE_FTM_TASK */

#ifdef FEATURE_DTV
#error code not present
#endif /* FEATURE_DTV */

#ifdef FEATURE_USE_TIME_VU
#include "time.h"

#ifdef FEATURE_TIME_TOD_AS_RTC_OFFSET
#include "time_tod.h"     
#endif /* FEATURE_TIME_TOD_AS_RTC_OFFSET */

#endif /* FEATURE_USE_TIME_VU */

#if defined(T_MSM7500) && !defined(FEATURE_STANDALONE_MODEM)
#error code not present
#endif /* T_MSM7500 && ! FEATURE_STANDALONE_MODEM */

#if !MIN_BREW_VERSIONEx(3,1,4)   /* Brew 3.1.4 removed brew_pwrup_init() */
#if defined(FEATURE_BREW_3_0) || defined(FEATURE_BREW_PWRUP_INIT)
extern void  brew_pwrup_init(void);
#endif /* FEATURE_BREW_3_0 || FEATURE_BREW_PWRUP_INIT */
#endif /* !MIN_BREW_VERSIONEx(3,1,4) */

#ifdef FEATURE_JOYST_TASK
  #include "joyst.h"                    /* Joystick header file */
#endif /* FEATURE_JOYST_TASK */

#ifdef FEATURE_PBM_TASK
  #include "pbmtask.h"                  /* PBM task header file */
#endif /* FEATURE_PBM_TASK */

#ifdef FEATURE_NV_CNV
  #include "nv_cnv.h"
#endif /* FEATURE_NV_CNV */

#ifdef FEATURE_DMTASK
  #include "dmtask.h"
#endif /* FEATURE_DMTASK */

#if defined(FEATURE_SEC_TIME) && !defined(FEATURE_SEC_PROCESS_INIT)
  #include "sectime.h"                  /* Secure time header file */
#endif /* FEATURE_SEC_TIME && !FEATURE_SEC_PROCESS_INIT */

#if (defined(FEATURE_DISP_TASK)) && (!defined(T_MSM7500) || \
      (defined(T_MSM7500) && defined(IMAGE_APPS_PROC)))
#include "disp_task.h"
#endif /* FEATURE_DISP_TASK */

#ifdef FEATURE_QTV_STATIC_TASKS
  #include "qtv_task_if_task.h"                  /* QTV task services */
#endif /* FEATURE_QTV_STATIC_TASKS */

#ifdef FEATURE_VBATT_TASK
	#include "vbatt_task.h"
#endif /* VBATT Task */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/* Add mode specific header file here */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#ifdef TMC_1X
  #include "rflib.h"                                  /* RFLIB header file */
  #include "rfnv.h"                           /* RF NV Manager header file */
  #include "pm.h"                                        /* PM header file */
  #ifdef FEATURE_ACP
#error code not present
  #endif
  #include "msm_init.h"                            /* msm_init header file */
  #ifndef FEATURE_MDSP_TASK
  #include "dmddown.h"                  /* Demodulator service header file */
  #endif
  #include "mccdma.h"                                  /* CDMA header file */
  #include "tx.h"                                        /* TX header file */
  #include "rx.h"                                        /* RX header file */
  #include "srch.h"                                    /* Srch header file */
  #include "rxtx.h"                                    /* RXTX header file */
  #ifdef FEATURE_TMC_TRM
    #include "trm.h"              /* Tranceiver Resource Manager header file */
  #endif
  #if (TG!=T_PC)               /* Define for all targets except Unit Test  */
    #include "therm.h"                             /* Thermister header file */
    #include "vbatt.h"                    /* Battery level utility header file */
    #define TMC_HAS_VBATT_CONFIG
    #define TMC_HAS_THERM_CONFIG
    #define TMC_HAS_RF_CONFIG
    #define TMC_HAS_HW_CONFIG
  #endif /* TG!=T_PC */

#endif /* TMC_1X */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#if (defined( FEATURE_GSM ) && !defined( FEATURE_WCDMA_PLT) && !defined(FEATURE_GSM_PLT))
#error code not present
#endif /* FEATURE_GSM && ! FEATURE_WCDMA_PLT */

#ifdef FEATURE_GSM_PLT
#error code not present
#endif
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#ifdef FEATURE_WCDMA
#error code not present
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_MMGSDI
  #include "gsdi_exp.h"
#endif /* FEATURE_MMGSDI */

#ifdef FEATURE_GSTK
  #include "gstki.h"
#endif /* FEATURE_GSTK */

#if defined (FEATURE_GSTK) && defined (FEATURE_VIRTUAL_SAT)
  #include "virtsat.h"
#endif /* FEATURE_GSTK && FEATURE_VIRTUAL_SAT */

#include "adie.h"                                      /* ADIE header file */

#ifdef FEATURE_VIRTUAL_SIM
  #include "virtuim.h"
#endif

#ifdef FEATURE_ERR_EXTENDED_STORE
  #include "err.h"
#endif

#include "sd.h"

#if defined (FEATURE_TMC_TCXOMGR) || defined (FEATURE_TCXOMGR_TMC)
  #include "tcxomgr.h"
#endif

#ifdef FEATURE_HSDPA
#error code not present
#endif

#ifdef FEATURE_ENABLE_FLOATING_POINT_ERRORS
#include "boot_trap.h"
#endif

#ifdef FEATURE_PROGRESSIVE_BOOT

  #ifndef FEATURE_L4
  #ifndef FEATURE_TIMER_TASK
   #include "interrupt_task.h"
  #endif
  #endif

  #include "boot_pager.h"
#endif

#if defined(SIRIUS_PLATFORM) && defined(WOLF_5)
#error code not present
#endif /* defined(SIRIUS_PLATFORM) && defined(WOLF_5) */

#if defined(FEATURE_SPINLOCK) && defined(FEATURE_ARM_ASM_SPINLOCK)
#include "smem_data_decl.h"
#include "spinlockarm.h"
#endif /* FEATURE_SPINLOCK && FEATURE_ARM_ASM_SPINLOCK */

#ifdef FEATURE_SMSM
#include "smem_data_decl.h"
#include "smsm.h"
#endif /* FEATURE_SMSM */

#ifdef FEATURE_SMEM_LOG
#include "smem_data_decl.h"
#include "smem_log.h"
#endif /* FEATURE_SMEM_LOG */

#ifdef FEATURE_PROFDIAG
  #include "profdiag.h"
#endif

#ifdef FEATURE_FOTA_FS
#include "fota_fs_update.h"
#endif /* #ifdef FEATURE_FOTA_FS */

#ifdef FEATURE_CC_MONITOR
#error code not present
#endif /* FEATURE_CC_MONITOR */

#ifdef FEATURE_L4
#error code not present
#endif

#ifdef FEATURE_CPU_BASED_FLOW_CONTROL
#include "fctask.h"
#endif

#ifdef FEATURE_L4LINUX
#error code not present
#endif

#ifdef FEATURE_IG_QDIAG
#include "qdiag.h"
#endif

#ifdef FEATURE_APS
#include "aps.h"
#endif

#ifdef FEATURE_PM_SERVER
  #include "pmserv.h"
#endif /* FEATURE_PM_SERVER */

#if (defined(FS_FEATURE_IG_IPC_SERVER) || defined(FS_FEATURE_RMTEFS_SERVER)) \
      && !defined(FEATURE_IG_EFS_EXT_SERVER)
  #include  "fs_iguana_server.h"
#endif /* FS_FEATURE_RMTEFS_SERVER && !FEATURE_IG_EFS_EXT_SERVER */

#ifdef FEATURE_QIPCALL
#ifndef FEATURE_MULTIPROCESSOR
#include "qipcall.h"
#endif
#endif /* FEATURE_QIPCALL */

#ifdef FEATURE_QDJTEST
  #include "qdjtest.h"
#endif /* FEATURE_QDJTEST */

#ifdef FEATURE_UBM_L1
#include "ubm_l1.h"
#endif /* FEATURE_UBM_L1 */

#ifdef FEATURE_CRASH_DEBUG_DISABLED_CHECK
#include "hw_crash_debug.h"
#endif

#if defined (FEATURE_USE_MPM_TCXO_CONTROL) || defined (FEATURE_POWER_COLLAPSE)
#error code not present
#endif

#if defined (FEATURE_EARLY_ERROR_RESET) || defined(FEATURE_DLOAD_HW_RESET_DETECT)
#include "boot_hw_reset.h"
#endif /* FEATURE_EARLY_ERROR_RESET or FEATURE_DLOAD_HW_RESET_DETECT*/



/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


/* <EJECT> */
/*===========================================================================

            GLOBAL DEFINITIONS AND DECLARATIONS

This section contains global definitions for constants, macros, types,
variables

===========================================================================*/
/*
 ** Add comment here
 */
#if !defined (_enable)
    void _enable( void );
#endif

/* Have to verify if feature T_MSM5500 is included or not
**
*/
#ifdef T_MSM5500
#error code not present
#endif /* T_MSM5500 */

/* Have to verify if feature T_AUTO_ICU is included or not
*/
#ifdef T_AUTO_ICU
  #include "ma.h"
#endif /* T_AUTO_ICU */

#ifdef FEATURE_OFFLINE_DEBUG
#error code not present
#endif

#include "memheap.h"                            /* Memory heap header file */

#ifdef FEATURE_PMEM
#include "pmem.h"
#endif

/* PUBLIC REGIONAL */
/*---------------------------------------------------------------------------
                                  REX TIMER
---------------------------------------------------------------------------*/

LOCAL rex_timer_type    tmc_rpt_timer,       /* Timer for kicking watchdog */
                        tmc_wait_timer;      /* Timer for timer wait       */

/*---------------------------------------------------------------------------
                                    QUEUE
---------------------------------------------------------------------------*/
#define TMC_CMD_POOL_SIZE 10
                         /* Maximun number of command waiting in the queue */

static tmc_cmd_type  tmc_cmd_arr[TMC_CMD_POOL_SIZE]; /* RAM command pool */

static q_type tmc_free_cmd_q;             /* Queue to manage commands pool */

static q_type tmc_cmd_q;

/*---------------------------------------------------------------------------
                                  STATE
---------------------------------------------------------------------------*/
/* TMC data structure is defined to hold all internal used data.
*/
typedef struct {

    /* TMC task state indicator - record current TMC task state.
    */
    tmc_state_e_type    state;                         /* TMC task state */

    #ifdef FEATURE_UIM_RUIM
    /* ESN ME - read ESN ME from TMC-NV interface and store it here.
    */
      dword               esn_me;
    #endif /* FEATURE_UIM_RUIM */
#ifdef CUST_EDITION
#ifdef FEATURE_UIM_EUIMID
      qword             meid_me;
      int               meid_st;
#endif
#endif
    boolean             is_init_done;
    /* Inidcate tmc initialization is done or not.
    */
    rex_tcb_type        *ack_task_ptr;
    /* The task tcb who responses the ack signal to TMC. */
} tmc_data_s_type;

LOCAL tmc_data_s_type tmc_data;

/* Be ware of the risk in case the max number of task is over 100.
*/
#define TMC_MAX_NUM_OF_TASK  100
/* wait counter in milliseconds for waiting for acknowledge for
** offline/stop signal from each tasks.
*/
#define TMC_DEFAULT_WAIT_CNT 50
#define TMC_WAIT_CNT_5SEC    5000
/* Maximum number of task for offline and stop processing
*/
#ifdef FEATURE_TMC_STACK_PROFILING
#define TMC_STACK_PROFILING_DWORD    0xabab
#endif

#ifdef FEATURE_CC_MONITOR
#error code not present
#endif  /* FEATURE_CC_MONITOR */

/* --------------------------------------------------------------------------
                       Reset Counter File
---------------------------------------------------------------------------*/

#ifdef FEATURE_RESET_COUNTER

/* EFS file name to store reset counter.
*/
LOCAL const char tmc_reset_cnt_file[] = "reset_cntr.bin";

/* TMC file name max length.
*/
#define TMC_FILE_NAME_MAX_LEN 20

/* Internal reset counter data structure.
*/
typedef struct {

  /* indicate if the value of counter is valid.
  ** Invalid indicates that there is problem with EFS system,
  */
  boolean is_valid;

  /* reset counter. reset counter is increased by one after each power up.
  ** UI can reset the counter to 0.
  */
  int num;

  /* file name to store reset counter in EFS.
  */
  char filename[TMC_FILE_NAME_MAX_LEN];

} tmc_reset_counter_s_type;

LOCAL tmc_reset_counter_s_type tmc_reset_counter;
#endif

#if (defined(FEATURE_QVPHONE) && (!defined(T_MSM7500)) && (!defined(T_MSM7200)))
extern void qvp_ae_task (dword dummy);
extern void qvp_vfe_task(dword param);
extern void qvp_mpeg4_task(dword param);
#ifdef FEATURE_QVPHONE_SIP
#error code not present
#endif /* FEATURE_QVPHONE_SIP */
#endif /* FEATURE_QVPHONE */

#ifdef FEATURE_EFS_PROBE_NAND
#include "flash_nand.h"
#endif

#if (defined(FEATURE_QVPHONE_SIP) && (!defined(T_MSM7500)) && (!defined(T_MSM7200)))
#error code not present
#endif

#ifdef FEATURE_MFLO
#error code not present
#endif /* FEATURE_MFLO */

#ifdef FEATURE_SEC_PROCESS_INIT
  extern void IxContentMain(dword param);
#else
#ifdef FEATURE_IXFILESVC
extern void IxFile_IPC_Server(dword param);
#endif

#ifdef FEATURE_IXHTTPSVC
extern void IxHTTPSvcMainFunc(dword param);
#endif

#ifdef FEATURE_IXTIMESVC
extern void IxTimeOfDay_IPC_Server(dword param);
#endif

#ifdef FEATURE_IXCONTENTSVC
  extern void IxContentClientHandler_IPC_Server(dword param);
extern void IxContent_IPC_Server(dword param);
#endif

#ifdef FEATURE_IXSQLSVC
extern void IxSQL_IPC_Server(dword param);
extern void IxSQLClientHandler_IPC_Server(dword param);
#endif
#endif /* FEATURE_SEC_PROCESS_INIT */


#ifdef PLATFORM_LTK
#error code not present
#endif

#ifdef FEATURE_QTV_STATIC_DLAGENT_TASK
//extern void mp4dsptask(dword param);
extern void DL_Agent_Task(dword param);
#endif

#ifdef FEATURE_QTV_STATIC_DLDSPSVC_TASK
extern void DL_Dsp_Svc_Task(dword param);
#endif

#ifdef FEATURE_IXDESCRAMBLERSVC
extern void IxDescrambler_IPC_Server(dword param);
#endif

#ifdef FEATURE_L4
#error code not present
#endif

/* <EJECT> */
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
/*===========================================================================
=============================================================================
============================= MISCELLANEOUS BLOCK ===========================
================================= FUNCTIONS =================================
=============================================================================
===========================================================================*/

#ifdef FEATURE_RESET_COUNTER
/* <EJECT> */
/*===========================================================================

FUNCTION tmc_reset_counter_init

DESCRIPTION
  Read reset counter from flash memeory and update the tmc reset counter data
  structure. If the file does not exist,create a file and set counter to 0.

DEPENDENCIES
  FS task has already been started.

RETURN VALUE
  TRUE init operation succeed.
  FALSE int operation fail.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL   boolean                    tmc_reset_counter_init( void )
{
  fs_rsp_msg_type fs_rsp;           /* fs generic response msg type */

  fs_open_xparms_type open_params;
  fs_handle_type file_handle;       /* file handler */

  /* Initialize tmc reset counter data structure.
  */
  tmc_reset_counter.is_valid = FALSE;
  tmc_reset_counter.num = 0;
  (void) std_strlcpy(tmc_reset_counter.filename, tmc_reset_cnt_file , sizeof(tmc_reset_counter.filename));

  /* Check if file exists or not.
  */
  fs_nametest ( tmc_reset_counter.filename, FS_TEST_FILE, NULL, &fs_rsp);

  if( fs_rsp.nametest.status != FS_OKAY_S )
  {
    TMC_MSG_ERROR("Unable to check the reset counter file.", 0, 0, 0);
    return FALSE;
  }

  if( !fs_rsp.nametest.name_found )
  {
    /* Creat the file and set counter to zero.
    */
    open_params.create.cleanup_option = FS_OC_CLOSE;
    open_params.create.buffering_option = FS_OB_ALLOW;
    open_params.create.attribute_mask = FS_FA_UNRESTRICTED;

    fs_open(tmc_reset_counter.filename, FS_OA_CREATE, &open_params, NULL,
            &fs_rsp);

    if ( fs_rsp.open.status != FS_OKAY_S )
    {
      TMC_MSG_ERROR( "Unable to create reset counter file.", 0, 0, 0 );
      return FALSE;
    }

    file_handle = fs_rsp.open.handle;

    /* Write reset counter.
    */
    fs_write(file_handle, &tmc_reset_counter.num,
             sizeof(tmc_reset_counter.num), NULL, &fs_rsp );

    if( fs_rsp.write.status != FS_OKAY_S )
    {
      TMC_MSG_ERROR( "Unable to write the reset counter file.", 0, 0, 0 );
      return FALSE;
    }

    if( fs_rsp.write.count != sizeof(tmc_reset_counter.num) )
    {
      TMC_MSG_ERROR( "The writer counter does not match the size.", 0, 0, 0);
      return FALSE;
    }

    fs_close(file_handle, NULL, &fs_rsp);

    if ( fs_rsp.close.status != FS_OKAY_S )
    {
      TMC_MSG_ERROR("Unable to close the reset counter file.", 0, 0, 0);
      return FALSE;
    }
  }
  else
  {
    /* Open file and read reset counter.
    */
    fs_open(tmc_reset_counter.filename, FS_OA_READONLY, NULL, NULL, &fs_rsp);

    if( fs_rsp.open.status != FS_OKAY_S )
    {
      TMC_MSG_ERROR("Unable to open the reset counter file.", 0, 0, 0);
      return FALSE;
    }

    file_handle = fs_rsp.open.handle;

    /* read reset counter */
    fs_read(file_handle, &tmc_reset_counter.num,
            sizeof(tmc_reset_counter.num), NULL, &fs_rsp);

    if( fs_rsp.read.status != FS_OKAY_S )
    {
      TMC_MSG_ERROR("Unable to read the reset counter file.", 0, 0, 0);
      return FALSE;
    }

    if( fs_rsp.read.count != sizeof(tmc_reset_counter.num) )
    {
      TMC_MSG_ERROR("Read counter does not match the size.", 0, 0, 0);
      return FALSE;
    }

    fs_close(file_handle, NULL, &fs_rsp);

    if ( fs_rsp.close.status != FS_OKAY_S )
    {
      TMC_MSG_ERROR("Unable to close the reset counter file.", 0, 0, 0);
      return FALSE;
    }

  }

  /* Update tmc reset counter data structure.
  */
  tmc_reset_counter.is_valid = TRUE;

  return TRUE;

} /* tmc_reset_counter_init */


/* <EJECT> */
/*===========================================================================

FUNCTION tmc_reset_counter_write

DESCRIPTION
  Write reset counter from tmc reset counter data structure to flash memeory.

DEPENDENCIES
  FS task has already been started.
  tmc_reset_counter_init() has already been called.

RETURN VALUE
  TRUE write operation succeed.
  FALSE write operation fail.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL   boolean                    tmc_reset_counter_write()
{
  fs_rsp_msg_type fs_rsp;           /* fs generic response msg type */

  fs_handle_type file_handle;       /* file handler */

  /* If couner is invalid, return false;
  */
  if( ! tmc_reset_counter.is_valid )
    return FALSE;

  /* Open file */
  fs_open(tmc_reset_counter.filename, FS_OA_READWRITE, NULL, NULL, &fs_rsp);

  if( fs_rsp.open.status != FS_OKAY_S )
  {
    TMC_MSG_ERROR("Unable to open the reset counter file.", 0, 0, 0);
    return FALSE;
  }

  file_handle = fs_rsp.open.handle;

  /* Write counter to file.*/
  fs_write( file_handle, &tmc_reset_counter.num,
            sizeof(tmc_reset_counter.num), NULL, &fs_rsp );

  if( fs_rsp.write.status != FS_OKAY_S )
  {
    TMC_MSG_ERROR( "Unable to write the reset counter file.", 0, 0, 0 );
    return FALSE;
  }

  if( fs_rsp.write.count != sizeof(tmc_reset_counter.num) )
  {
    TMC_MSG_ERROR( "The write counter does not match the size.", 0, 0, 0);
    return FALSE;
  }

  fs_close(file_handle, NULL, &fs_rsp);

  if ( fs_rsp.close.status != FS_OKAY_S )
  {
    TMC_MSG_ERROR("Unable to close the reset counter file.", 0, 0, 0);
    return FALSE;
  }

  return TRUE;

} /* tmc_reset_counter_write */


/* <EJECT> */
/*===========================================================================

FUNCTION tmc_reset_counter_increment

DESCRIPTION
  Increment reset counter from flash memeory. If the file does not exist,
  create a file and set counter to 0.

DEPENDENCIES
  FS task has already been started.

RETURN VALUE
  TRUE increment operation succeed.
  FALSE increment operation fail.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL   boolean                    tmc_reset_counter_increment( void )
{
  if( tmc_reset_counter_init() )
  {
    tmc_reset_counter.num++;
    return ( tmc_reset_counter_write() );
  }

  return FALSE;
}
#endif /* FEATURE_RESET_COUNTER */


/* <EJECT> */
/*===========================================================================

FUNCTION tmc_set_ack_task

DESCRIPTION
  Set task tcb pointer in tmc_data.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL   void                       tmc_set_ack_task(
        rex_tcb_type               *tcb_ptr
)
{
  tmc_data.ack_task_ptr = tcb_ptr;
} /* tmc_set_ack_task */


/* <EJECT> */
/*===========================================================================

FUNCTION tmc_is_ack_task

DESCRIPTION
  If the specified task tcb pointer is the same as that in tmc_data.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL   boolean                    tmc_is_ack_task(
        rex_tcb_type               *tcb_ptr
)
{
  return (tmc_data.ack_task_ptr == tcb_ptr);
} /* tmc_is_ack_task */


/* <EJECT> */
/*===========================================================================

FUNCTION tmc_module_init

DESCRIPTION
  Initialize timers and queues of Task Main Controller

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
LOCAL   void                       tmc_module_init( void )
{
  unsigned int i;

  /* Initialize timers */
  rex_def_timer(&tmc_rpt_timer,&tmc_tcb,TMC_RPT_TIMER_SIG);
  /* Add other timers intialize here */
  rex_def_timer(&tmc_wait_timer,&tmc_tcb,TMC_WAIT_SIG);

  /* Initialize queues */
  (void) q_init(&tmc_cmd_q);
  (void) q_init(&tmc_free_cmd_q);

  /* Link and place each block in command pool into the free command queue.
  ** Initalize command array.The done_q_ptr is for free command buffer back
  ** to free command queue. If the task pointer is non-NULL, signal the task
  ** that the tone is done.
  */
  for ( i=0; i< ARR_SIZE(tmc_cmd_arr); i++)
  {
    tmc_cmd_arr[i].cmd_id = TMC_CMD_TYPE_NONE;
    tmc_cmd_arr[i].hdr.done_q_ptr = &tmc_free_cmd_q;
    tmc_cmd_arr[i].hdr.task_ptr = NULL;
    tmc_cmd_arr[i].hdr.sigs = 0;
    (void) q_link(&tmc_cmd_arr[i],&tmc_cmd_arr[i].hdr.link);
    q_put(&tmc_free_cmd_q, &tmc_cmd_arr[i].hdr.link);
  }

} /* tmc_module_init */


/* <EJECT> */
/*===========================================================================

FUNCTION tmc_dog_report

DESCRIPTION
  Kick watch dog and set watch dog timer

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
LOCAL   void                       tmc_dog_report( void )
{
#ifdef FEATURE_DOG
  dog_report(DOG_TMC_RPT);
  (void) rex_set_timer(&tmc_rpt_timer, DOG_TMC_RPT_TIME);
#endif /* FEATURE_DOG */
} /* tmc_dog_report */


/* <EJECT> */
/*===========================================================================

FUNCTION tmc_wait

DESCRIPTION
  Suspend TMC from executing until at least one of the specified signals
  in the input mask is set.

  Note that this function keep reporting to the watchdog while waiting
  for the signal(s) to be set.

DEPENDENCIES
  tmc_rpt_timer must have been defined with rex_def_timer().

RETURN VALUE
  rex_sigs_type

SIDE EFFECTS
  None

===========================================================================*/
LOCAL   rex_sigs_type              tmc_wait(

        rex_sigs_type              mask
            /* Mask of signals to wait on */
)
{

  rex_sigs_type    sigs;
      /* Signals returned by rex_wait */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Loop while waiting for one of the specified signals
  ** to be set.
  */
  for( ;; )
  {
    /* Wait on specified signals, as well as,
    ** on the watchdog signal.
    */
    sigs = rex_wait( mask | TMC_RPT_TIMER_SIG );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If watchdog signal is set, report back to the watchdog
    */
    if( sigs & TMC_RPT_TIMER_SIG )
    {
      tmc_dog_report();
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If one or more of the specified signals is set
    ** return now.
    */
    if( sigs & mask )
    {
      return sigs;
    }

  } /* for ( ;; ) */

} /* tmc_wait */


/* <EJECT> */
/*===========================================================================

FUNCTION tmc_get_cmd

DESCRIPTION
  This function dequeue a command from command queue and queue the memory
  back to the free command queue

DEPENDENCIES
  Command queue has to be initialized.

RETURN VALUE
  A buffer of type tmc_cmd_type.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL   tmc_cmd_type               *tmc_get_cmd( void )
{
  /* Dequeue a command from command queue */
  return (tmc_cmd_type *) q_get(&tmc_cmd_q);

} /* tmc_get_cmd */


/* <EJECT> */
#ifdef FEATURE_CC_MONITOR
#error code not present
#endif /* FEATURE_CC_MONITOR */

/* <EJECT> */
/*===========================================================================

FUNCTION tmc_free_cmd

DESCRIPTION
  This function returns a command buffer to the free queue.

DEPENDENCIES
  Free queue has to be initialized.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL   void                       tmc_free_cmd(

        tmc_cmd_type               *cmd_ptr
)
{
  /* Enqueue a command into the free queue
  */
  if ( cmd_ptr != NULL )
  {
    /* Enqueue item onto done queue if requested (done_q_ptr != NULL)
    ** Do this before we signal the task so that it has its buffer back
    ** before we call its attention to the fact that the operation has
    ** completed.
    */
    if ( cmd_ptr->hdr.done_q_ptr != NULL ) {
      q_put( cmd_ptr->hdr.done_q_ptr, &cmd_ptr->hdr.link );
    }

    /* If the task pointer is non-NULL, signal the task that the tone is done.
    ** Note: The pointer was either passed as NULL, or was set to NULL because
    ** the signal was sent elsewhere.
    */
    if ( cmd_ptr->hdr.task_ptr != NULL ) {
      (void) rex_set_sigs( cmd_ptr->hdr.task_ptr, cmd_ptr->hdr.sigs );
    }
  }

} /* tmc_free_cmd */


/* <EJECT> */
/*===========================================================================

FUNCTION tmc_vote_to_sleep

DESCRIPTION
  TMC would vote to sleep or not.

DEPENDENCIES
  SLEEP task has be started before this function can be called.

RETURN VALUE
  None

SIDE EFFECTS
  If TMC vote to sleep, it means that TMC task agrees entering sleep. But
  until all voters agree, the phone would not go to sleep.
  If TMC vote not to sleep, it means the phone would never go to sleep.


===========================================================================*/
//LOCAL   void                      tmc_vote_to_sleep(
//        boolean                   is_sleep
//        /* Indicate if it is sleep or not. */
//)
//{
//  if (is_sleep)
//  {
//    #ifdef FEATURE_TMC_VOTE_SLEEP
//    SLEEP_ASSERT_OKTS( SLEEP_TMC_OKTS_SIG );
//    #endif /* FEATURE_TMC_VOTE_SLEEP */
//    return;
//  }

//  #ifdef FEATURE_TMC_VOTE_SLEEP
//  SLEEP_NEGATE_OKTS( SLEEP_TMC_OKTS_SIG );
//  #endif /* FEATURE_TMC_VOTE_SLEEP */

//}/* tmc_vote_to_sleep */


/* <EJECT> */
/*===========================================================================

FUNCTION tmc_offline_tasks

DESCRIPTION
  The function sends the offline signal to tasks that need to be notified
  of offline. For each task it waits for the task to acknowledge the signal.

DEPENDENCIES
  Task Main Controller has start.

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
LOCAL   void                       tmc_offline_tasks( void )
{

  rex_sigs_type        received_sigs;
  rex_tcb_type         *tcb_ptr[TMC_MAX_NUM_OF_TASK];
  int                  i=0,num_of_task;          /* number of task counter */
                       /* the wait time counter in milliseconds */
  rex_timer_cnt_type   wait_cnt[TMC_MAX_NUM_OF_TASK];

  /* Clear ACK signal first.
  */
  (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

  /* Send offline signal to the tasks listed at tcb_ptr table.
  ** Set a watchdog timer while waiting for ACK signal. If timer out,
  ** sent out error message to indicate the failure to offline the task.
  */
  /* Sleep task should received the offline signal as earlier as possible.
  ** Once it receives the signal, it will not shut down the TCXO clock.
  */
  #ifdef TMC_SLEEP
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &sleep_tcb;
  #endif /* TMC_SLEEP */

  #ifdef FEATURE_NV
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &nv_tcb;
  #endif /* FEATURE_NV */

#if !defined(FEATURE_THIN_UI) && !defined(FEATURE_UI_CORE_REMOVED)
  #ifdef FEATURE_HS
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &hs_tcb;
  #endif /* FEATURE_HS */
#endif /* !defined(FEATURE_THIN_UI) && !defined(FEATURE_UI_CORE_REMOVED) */

  #ifdef FEATURE_DIAG_TASK
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &diag_tcb;
  #endif /* FEATURE_DIAG_TASK */

  #ifdef FEATURE_IG_QDIAG
  wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
  tcb_ptr[i++] = &qdiag_tcb;
  #endif /* FEATURE_IG_QDIAG */

  #ifdef FEATURE_UI
  #ifdef FEATURE_IG_UI_CLIENT_SELECT
  if( !hw_ig_ui_client_is_wombat() )
    /* Wombat (Linux) is running the keypad and display */
  #endif /* FEATURE_IG_UI_CLIENT_SELECT */
  {
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &ui_tcb;
  }
  #endif /* FEATURE_UI */

  #ifdef FEATURE_CM
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &cm_tcb;
  #endif /* FEATURE_CM */

  #if (defined(FEATURE_GPSONE) && !defined(FEATURE_CGPS))
#error code not present
  #endif /* FEATURE_GPSONE && !FEATURE_CGPS */

  #ifdef FEATURE_SND_TASK
   wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
   tcb_ptr[i++] = &snd_tcb;
  #endif /* FEATURE_SND_TASK */

  #ifdef FEATURE_VOC_TASK
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &voc_tcb;
  #endif /* FEATURE_VOC_TASK */

  #ifdef FEATURE_QDSP_TASK
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &qdsp_tcb;
  #endif /* FEATURE_QDSP_TASK */

  #ifdef FEATURE_QDSP_RTOS
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &adsp_rtos_tcb;
  #endif /* FEATURE_QDSP_RTOS */

  #ifdef FEATURE_IS2000_TDSO
   wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
   tcb_ptr[i++] = &tdso_tcb;
  #endif /* FEATURE_IS2000_TDSO */

  #ifdef FEATURE_HDR
#error code not present
  #endif /* FEATURE_HDR */

  #ifdef FEATURE_HDR_BCMCS
#error code not present
  #endif /* FEATURE_HDR_BCMCS */

  #ifdef FEATURE_VBATT_TASK
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT ;
    tcb_ptr[i++] = &vbatt_tcb ;
  #endif /* FEATURE_VBATT_TASK */

  #ifdef TMC_1X
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &rxtx_tcb;
  #endif /* TMC_1X */

  #if defined (FEATURE_AUTH) || defined (FEATURE_RUIM)
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &auth_tcb;
  #endif /* FEATURE_AUTH || FEATURE_RUIM */

  #ifdef FEATURE_DH
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &dh_tcb;
  #endif /* FEATURE_DH */

  #ifdef FEATURE_DOG
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &dog_tcb;
  #endif /* FEATURE_DOG */

  #if ( defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS) )
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &wms_tcb;
  #endif /* FEATURE_CDSMS || FEATURE_GWSMS */

  #ifdef FEATURE_MDSP_TASK
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &mdsp_tcb;
  #endif /* FEATURE_MDSP_TASK */

  #ifdef FEATURE_MMGPS
#error code not present
  #endif /* FEATURE_MMGPS */

  #ifdef FEATURE_MGP

    #ifdef FEATURE_MGPCC_TASK
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &mgpcc_tcb;
    #endif

    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &mgpmc_tcb;

    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &mgpnf_tcb;

    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &mgpcd_tcb;

    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &mgpai_tcb;

  #endif /* FEATURE_MGP */

  #ifdef FEATURE_CGPS
#error code not present
  #endif /* FEATURE_CGPS */

  #ifdef FEATURE_XO
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &tcxomgr_tcb;
  #endif /* FEATURE_XO */

  #ifdef FEATURE_MMGSDI
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &gsdi_task_tcb;
  #endif /* FEATURE_MMGSDI */

  #ifdef FEATURE_FTM_TASK
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &ftm_tcb;
  #endif /* FEATURE_FTM_TASK */

  #ifdef FEATURE_CHG_TASK
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &chg_tcb;
  #endif /* FEATURE_CHG_TASK */

  #ifdef FEATURE_GSTK
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &gstk_tcb;
  #endif /* FEATURE_GSTK */

  #ifdef FEATURE_UIM
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &uim_tcb;
  #endif /* FEATURE_UIM */

  #ifdef FEATURE_BCMCS
#error code not present
  #endif /* FEATURE_BCMCS */

  #ifdef FEATURE_JOYST_TASK
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &joyst_tcb;
  #endif  /* FEATURE_JOSYT_TASK */

  #ifdef FEATURE_DTV
#error code not present
  #endif /* FEATURE_DTV */

  #ifdef FEATURE_PBM_TASK
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &pbm_tcb;
  #endif /* FEATURE_PBM_TASK */

  #ifdef FEATURE_APS
  wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
  tcb_ptr[i++] = &aps_tcb;
  #endif /* FEATURE_APS */

  #ifdef FEATURE_ATS
  wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
  tcb_ptr[i++] = &ats_tcb;
  #endif /* FEATURE_ATS */

  #ifdef FEATURE_PM_SERVER
  wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
  tcb_ptr[i++] = &pms_tcb;
  #endif /* FEATURE_PM_SERVER */

  #ifdef FEATURE_QIPCALL
  #ifndef FEATURE_MULTIPROCESSOR
  wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
  tcb_ptr[i++] = &qipcall_tcb;
  #endif
  #endif  /* FEATURE_QIPCALL */

  #ifdef FEATURE_MFLO
#error code not present
  #endif  /* FEATURE_MFLO */

  #if (defined(FEATURE_DISP_TASK)) && (!defined(T_MSM7500) || \
       (defined(T_MSM7500) && defined(IMAGE_APPS_PROC)))
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &disp_tcb;
  #endif /* FEATURE_DISP_TASK */

  #ifdef FEATURE_AVS_SBCENC_TASK 
   wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
   tcb_ptr[i++] = &sbcenc_tcb;
  #endif 
  /* Add other tasks here.
  */

  #ifdef FEATURE_UBM_L1
  wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
  tcb_ptr[i++] = &ubm_l1_tcb;
  #endif /* FEATURE_UBM_L1 */

  /* If total num of offline task are greater than TMC_MAX_NUM_TASKS,
  ** you have to increase TMC_MAX_NUM_TASKS to match the number.
  */
  num_of_task = i ;

 /* Sending offline signale and timer wait for ACK signale.*/

 for (i = 0; i < num_of_task; i++ ) {

   TMC_MSG_MED("Offline task : %c%c%c.",tcb_ptr[i]->task_name[0],
                                         tcb_ptr[i]->task_name[1],
                                         tcb_ptr[i]->task_name[2]);

   (void) rex_set_sigs(tcb_ptr[i], TASK_OFFLINE_SIG);

   /* Report dog first so that it would not timer out immediately after we are
   ** waiting for ack signal from other tasks.
   */
   tmc_dog_report();

   /* Set the Sanity Timer to avoid having the possibility of
   ** losing the time when the wrong Task Acks back after
   ** it had timed out
   */
   (void) rex_set_timer (&tmc_wait_timer, wait_cnt[i]);

   do
   {
     /* Wait for either the Sanity Timer to Expire or
     ** the TMC_ACK_SIG To occur
     */
     received_sigs = rex_wait(TMC_ACK_SIG | TMC_WAIT_SIG);

     (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
     /* Get out only, if the task that acks back
     ** is the task that we are waiting for
     */
   } while( !tmc_is_ack_task( tcb_ptr[i] ) &&
            !(received_sigs & TMC_WAIT_SIG) );

   (void) rex_clr_timer(&tmc_wait_timer);

   (void) rex_clr_sigs(rex_self(), TMC_WAIT_SIG);

   if (received_sigs & TMC_WAIT_SIG)
   {
     TMC_MSG_ERROR("Offline timer out task : %c%c%c.",
                   tcb_ptr[i]->task_name[0],
                   tcb_ptr[i]->task_name[1],
                   tcb_ptr[i]->task_name[2]);

     #ifdef FEATURE_OFFLINE_DEBUG
#error code not present
     #endif
   }
 }
} /* tmc_offline_tasks */


/* <EJECT> */
/*===========================================================================

FUNCTION tmc_stop_tasks
  The function sends the stop signal to each task then waits for
  the task to acknowledge the signal.

DEPENDENCIES
  Task Main Controller has start.

RETURN VALUE
  void

SIDE EFFECTS
  void

===========================================================================*/
LOCAL   void                       tmc_stop_tasks( void )
{
  rex_sigs_type        received_sigs;                   /* Received signal */
  rex_tcb_type         *tcb_ptr[TMC_MAX_NUM_OF_TASK];   /* Stop task array */
  int                  i=0,num_of_task;          /* number of task counter */
                       /* the wait time counter in milliseconds */
  rex_timer_cnt_type   wait_cnt[TMC_MAX_NUM_OF_TASK];

  /* Clear ack signal. */
  (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

  /* Vote not to sleep.
  ** There is a RACE condition where all the other tasks vote to
  ** sleep priory to finish the hw reset. Have TMC task not vote to sleep
  ** to avoid this race condition.
  */
  //tmc_vote_to_sleep( FALSE );

  /* Store required task to tcb array.*/

  #ifdef FEATURE_DOG
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &dog_tcb;
  #endif /* FEATURE_DOG */

  /* Sleep task should received the stop signal as earlier as possible.
  ** Once it receives the signal, it will not shut down the TCXO clock.
  */
  #ifdef TMC_SLEEP
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &sleep_tcb;
  #endif /* TMC_SLEEP */

  #ifdef FEATURE_DH
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &dh_tcb;
  #endif /* FEATURE_DH */

  #ifdef FEATURE_ONCRPC
#error code not present
  #endif /* FEATURE_ONCRPC */

  #ifdef FEATURE_WLAN
#error code not present
  #endif  /* FEATURE_WLAN */

  #ifdef FEATURE_DATA
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &ds_tcb;

    #ifdef FEATURE_COMP_TASK
      wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
      tcb_ptr[i++] = &comp_tcb;
    #endif /* FEATURE_COMP_TASK */

    #ifdef FEATURE_DATA_PS
      wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
      tcb_ptr[i++] = &ps_tcb;
    #endif /* FEATURE_DATA_PS */

  #endif /* FEATURE_DATA */

  #ifndef FEATURE_SEC_PROCESS_INIT
  #ifdef FEATURE_IXHTTPSVC
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &ix_httpsvc_tcb;
  #endif

  #ifdef FEATURE_IXTIMESVC
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &ix_timesvc_tcb;
  #endif

  #ifdef FEATURE_IXCONTENTSVC
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &ix_contentclnt_tcb;
    #endif
  #endif /* !FEATURE_SEC_PROCESS_INIT */

  #ifdef FEATURE_IXCONTENTSVC
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &ix_contentsvc_tcb;
  #endif

  #ifdef FEATURE_IXDESCRAMBLERSVC
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &ix_descramblersvc_tcb;
  #endif

  #ifndef FEATURE_SEC_PROCESS_INIT
  #ifdef FEATURE_SEC
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &sec_tcb;
    #ifdef FEATURE_SEC_CONTENT_AGENT
      wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
      tcb_ptr[i++] = &contentagentdiag_tcb;
    #endif
      #ifdef FEATURE_SEC_ARM_CRYPTO
        wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
        tcb_ptr[i++] = &seccryptarm_tcb;
      #endif
    #endif /* FEATURE_SEC */
  #endif /* !FEATURE_SEC_PROCESS_INIT */

  #ifdef FEATURE_SEC
    #ifdef FEATURE_SECRND
      wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
      tcb_ptr[i++] = &secrnd_tcb;
    #endif
    #ifdef FEATURE_SEC_USFS
      wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
      tcb_ptr[i++] = &sfs_tcb;
    #endif
    #ifdef FEATURE_SEC_IPSEC
      wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
      tcb_ptr[i++] = &secips_tcb;
    #endif
    #ifdef FEATURE_SEC_SSL
      wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
      tcb_ptr[i++] = &secssl_tcb;
    #endif /* FEATURE_SEC_SSL */
    #ifdef FEATURE_SEC_DRM
#error code not present
    #endif /* FEATURE_SEC_SSL */
    #if (defined(FEATURE_SEC_SSL_TESTAPP) || defined(FEATURE_SEC_CRYPT_TESTAPP))
#error code not present
    #endif /* FEATURE_SEC_SSL_TESTAPP || FEATURE_SEC_CRYPT_TESTAPP */
  #endif /* FEATURE_SEC */

  #ifdef FEATURE_DS_SOCKETS_TEST_TCP
#error code not present
  #endif /* FEATURE_DS_SOCKETS_TEST_TCP */

  #ifdef FEATURE_DS_SOCKETS_TEST_UDP
#error code not present
  #endif /* FEATURE_DS_SOCKETS_TEST_UDP */

  #ifdef FEATURE_QMIP
#error code not present
  #endif /* FEATURE_QMIP */

  #ifdef FEATURE_IS2000_TDSO
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &tdso_tcb;
  #endif /* FEATURE_IS2000_TDSO */

  #if defined (FEATURE_AUTH) || defined (FEATURE_RUIM)
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &auth_tcb;
  #endif /* FEATURE_AUTH || FEATURE_RUIM */

  #ifdef FEATURE_HDR
#error code not present
  #endif /* FEATURE_HDR */

  #ifdef FEATURE_HDR_BCMCS
#error code not present
  #endif /* FEATURE_HDR_BCMCS */

  #ifdef TMC_1X
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &rxtx_tcb;

    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &srch_tcb;

    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &rx_tcb;

    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &tx_tcb;
  #endif /* TMC_1X */

  #ifdef FEATURE_CM
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &cm_tcb;
  #endif /* FEATURE_CM */

  #ifdef FEATURE_BT
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &bt_tcb;
  #endif /* FEATURE_BT */

  #if (defined(FEATURE_GPSONE) && !defined(FEATURE_CGPS))
#error code not present
  #endif /* FEATURE_GPSONE && !FEATURE_CGPS */

  #ifdef FEATURE_IG_QDIAG
  wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
  tcb_ptr[i++] = &qdiag_tcb;
  #endif /* FEATURE_IG_QDIAG */

  #ifdef FEATURE_DIAG_TASK
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &diag_tcb;
  #endif /* FEATURE_DIAG_TASK */

#if !defined(FEATURE_THIN_UI) && !defined(FEATURE_UI_CORE_REMOVED)
  #ifdef FEATURE_HS
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &hs_tcb;
  #endif /* FEATURE_HS */
#endif /* !defined(FEATURE_THIN_UI) && !defined(FEATURE_UI_CORE_REMOVED) */

  #ifdef FEATURE_IDS
#error code not present
  #endif /* FEATURE_IDS */

  #if defined (FEATURE_USB) && !defined (FEATURE_NO_USB_HW_ON_PROCESSOR)
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &usbdc_tcb;
    #ifdef FEATURE_USB_HOST_ENHANCED
      wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
      tcb_ptr[i++] = &usbhc_urb_tcb;

      wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
      tcb_ptr[i++] = &usbhc_dev_tcb;
    #if defined(FEATURE_USB_HOST_TEST_ENHANCED) && \
        (defined(FEATURE_USB_HOST_MASS_STORAGE_TEST_ENHANCED) || \
         defined(FEATURE_USB_HOST_HID_TEST_ENHANCED))
      wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
      tcb_ptr[i++] = &usbhc_test_tcb;
    #endif
    #endif

  #endif /* FEATURE_USB */

  #if defined(FEATURE_HS_USB) && (defined(FEATURE_STANDALONE_MODEM) || \
          !defined(FEATURE_NO_USB_HW_ON_PROCESSOR))
      #ifdef FEATURE_HS_USB_MS_FD
        wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
        tcb_ptr[i++] = &hsu_al_ms_tcb;
      #endif

        wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
        tcb_ptr[i++] = &hsu_al_task_tcb;
  #endif /* FEATURE_HS_USB && (FEATURE_STANDALONE_MODEM || !FEATURE_NO_USB_HW_ON_PROCESSOR) */

  #ifdef FEATURE_SMD
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &smd_tcb;
  #endif /*FEATURE_SMD*/

  #ifdef FEATURE_NV
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &nv_tcb;
  #endif /* FEATURE_NV */

  #ifdef FEATURE_VS_TASK
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &vs_tcb;
  #endif /* FEATURE_VS_TASK */

  #ifdef FEATURE_VBATT_TASK
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT ;
    tcb_ptr[i++] = &vbatt_tcb ;
   #endif /* FEATURE_VBATT_TASK */

  #ifdef FEATURE_GRAPH_TASK
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &graph_tcb;
  #endif /* FEATURE_GRAPH_TASK */

  #ifdef FEATURE_CAMERA_DRV_TASK
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &camera_drv_tcb;
  #endif /* FEATURE_CAMERA_DRV_TASK */

  #ifdef FEATURE_SND_TASK
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &snd_tcb;
  #endif /* FEATURE_SND_TASK */

  #ifdef FEATURE_VOC_TASK
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &voc_tcb;
  #endif /* FEATURE_VOC_TASK */

  #ifdef FEATURE_QDSP_TASK
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &qdsp_tcb;
  #endif /* FEATURE_QDSP_TASK */

  #ifdef FEATURE_QDSP_RTOS
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &adsp_rtos_tcb;
  #endif /* FEATURE_QDSP_RTOS */

  #ifndef FEATURE_SEC_PROCESS_INIT
  #ifdef FEATURE_IXFILESVC
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &ix_filesvc_tcb;
  #endif

  #ifdef FEATURE_IXSQLSVC
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &ix_sqlsvc_tcb;

    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &ix_sqlclnt_tcb;
  #endif
  #endif /* !FEATURE_SEC_PROCESS_INIT */

  #ifdef FEATURE_EFS_SFAT
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &sfat_tcb;
  #endif /* FEATURE_EFS_SFAT */

  #ifdef FEATURE_EFS
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &fs_tcb;
  #endif /* FEATURE_EFS */

  #if ( defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS) )
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &wms_tcb;
  #endif /* FEATURE_CDSMS || FEATURE_GWSMS */

  #ifdef FEATURE_MDSP_TASK
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &mdsp_tcb;
  #endif /* FEATURE_MDSP_TASK */

  #ifdef FEATURE_MMGPS
#error code not present
  #endif /* FEATURE_MMGPS */

  #ifdef FEATURE_MGP

    #ifdef FEATURE_MGPCC_TASK
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &mgpcc_tcb;
    #endif

    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &mgpmc_tcb;

    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &mgpnf_tcb;

    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &mgpcd_tcb;

    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &mgpai_tcb;

  #endif /* FEATURE_MGP */

  #ifdef FEATURE_CGPS
#error code not present
  #endif /* FEATURE_CGPS */

  #ifdef FEATURE_MMGSDI
    wait_cnt[i] = TMC_WAIT_CNT_5SEC;
    tcb_ptr[i++] = &gsdi_task_tcb;
  #endif /* FEATURE_MMGSDI */

  #ifdef FEATURE_FTM_TASK
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &ftm_tcb;
  #endif /* FEATURE_FTM_TASK */

  #ifdef FEATURE_CHG_TASK
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &chg_tcb;
  #endif /* FEATURE_CHG_TASK */

  #ifdef FEATURE_GSTK
    wait_cnt[i] = TMC_WAIT_CNT_5SEC;
    tcb_ptr[i++] = &gstk_tcb;
  #endif /* FEATURE_GSTK */

  #ifdef FEATURE_UIM
    wait_cnt[i] = TMC_WAIT_CNT_5SEC;
    tcb_ptr[i++] = &uim_tcb;
  #endif /* FEATURE_UIM */

  #ifdef FEATURE_BT_EXTPF
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &bt_pf_tcb;
  #endif /* FEATURE_BT_EXTPF */

  /* Add other required tasks here. */
  #ifdef FEATURE_BCMCS
#error code not present
  #endif /* FEATURE_BCMCS */

  #ifdef FEATURE_DTV
#error code not present
  #endif /* FEATURE_DTV */

  #ifdef FEATURE_JOYST
     wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
     tcp_ptr[i++] = &joyst_tcb;
  #endif  /* FEATURE_JOYST */

  #ifdef FEATURE_PBM_TASK
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &pbm_tcb;
  #endif /* FEATURE_PBM_TASK */

  #ifdef FEATURE_APS
  wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
  tcb_ptr[i++] = &aps_tcb;
  #endif /* FEATURE_APS */

  #ifdef FEATURE_ATS
  wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
  tcb_ptr[i++] = &ats_tcb;
  #endif /* FEATURE_ATS */

  #ifdef FEATURE_PM_SERVER
  wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
  tcb_ptr[i++] = &pms_tcb;
  #endif /* FEATURE_PM_SERVER */

  #ifdef FEATURE_QIPCALL
  #ifndef FEATURE_MULTIPROCESSOR
  wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
  tcb_ptr[i++] = &qipcall_tcb;
  #endif
  #endif  /* FEATURE_QIPCALL */

  #ifdef FEATURE_MFLO
#error code not present
  #endif  /* FEATURE_MFLO */

  #ifdef FEATURE_AVS_SBCENC_TASK 
   wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
   tcb_ptr[i++] = &sbcenc_tcb;
  #endif 

  #ifdef FEATURE_UBM_L1
  wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
  tcb_ptr[i++] = &ubm_l1_tcb;
  #endif /* FEATURE_UBM_L1 */

  #ifdef FEATURE_QTV_STATIC_TASKS
  wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
  tcb_ptr[i++] = &qtv_video_renderer_tcb;
  wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
  tcb_ptr[i++] = &qtv_audio_tcb;
  #endif /* FEATURE_QTV_STATIC_TASKS */

  #ifdef FEATURE_EFS_FDI
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &fdi_bg_tcb;
  #endif /* FEATURE_EFS_FDI */

  /* Add other required tasks here.
  */


  /* UI has to be last task to stop
  */
  #ifdef FEATURE_UI
  #ifdef FEATURE_IG_UI_CLIENT_SELECT
  if( !hw_ig_ui_client_is_wombat() )
    /* Wombat (Linux) is running the keypad and display */
  #endif /* FEATURE_IG_UI_CLIENT_SELECT */
  {
    #if defined(FEATURE_SIRIUS) || defined(T_MSM7500) 
#error code not present
    #else
    wait_cnt[i] = 4*TMC_DEFAULT_WAIT_CNT;
    #endif /* FEATURE_SIRIUS */
    tcb_ptr[i++] = &ui_tcb;
  }
  #endif /* FEATURE_UI */
  #ifdef FEATURE_QTV_STATIC_DLAGENT_TASK
  wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
  tcb_ptr[i++] = &qtv_dec_dlAgenttask_tcb;

  #endif 
  #ifdef FEATURE_QTV_STATIC_DLDSPSVC_TASK
  wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
  tcb_ptr[i++] = &qtv_dec_dlDspSvctask_tcb;

  #endif 

  /* If total num of stop task are greater than TMC_MAX_NUM_TASKS,
  ** you have to increase TMC_MAX_NUM_TASKS to match the number.
  */
  num_of_task = i ;

  /* Sending stop signale and timed wait for ACK signale.*/

  for (i = 0; i < num_of_task; i++ ) {

    TMC_MSG_MED("Stop task : %c%c%c.",tcb_ptr[i]->task_name[0],
                                      tcb_ptr[i]->task_name[1],
                                      tcb_ptr[i]->task_name[2]);

    (void) rex_set_sigs(tcb_ptr[i], TASK_STOP_SIG);
    /* Report dog first so that it would not timer out immediately after we are
    ** waiting for ack signal from other tasks.
    */
    tmc_dog_report();

    /* Set the Sanity Timer to avoid having the possibility of
    ** losing the time when the wrong Task Acks back after
    ** it had timed out
    */
    (void) rex_set_timer (&tmc_wait_timer, wait_cnt[i]);

    do
    {
      /* Wait for either the Sanity Timer to Expire or
      ** the TMC_ACK_SIG To occur
      */
      received_sigs = rex_wait(TMC_ACK_SIG | TMC_WAIT_SIG);

      (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
      /* Get out only, if the task that acks back
      ** is the task that we are waiting for
      */
    } while( !tmc_is_ack_task( tcb_ptr[i] ) &&
             !(received_sigs & TMC_WAIT_SIG) );

    (void) rex_clr_timer(&tmc_wait_timer);

    (void) rex_clr_sigs(rex_self(), TMC_WAIT_SIG);

    if(received_sigs & TMC_WAIT_SIG)
    {
      TMC_MSG_ERROR("Stop timer out task : %c%c%c.",
                    tcb_ptr[i]->task_name[0],
                    tcb_ptr[i]->task_name[1],
                    tcb_ptr[i]->task_name[2]);
    }
  }
} /* tmc_stop_tasks */

/* ^L<EJECT> */
/*===========================================================================

FUNCTION tmc_online_handler

DESCRIPTION
  This function is called when the MS is to transition to the  online
  state. The function processes the reset procedures.

DEPENDENCIES
  Task Main Controller has start.

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
LOCAL   void                       tmc_online_handler( void )
{
 if ( tmc_get_state() == TMC_LPM )
 {
   #ifdef TMC_HW_IMPLEMENT_LPM
    hw_exit_low_power_mode();
   #endif /* TMC_HW_IMPLEMENT_LPM */
 }


}/* tmc_online__handler */


/* <EJECT> */
/*===========================================================================

FUNCTION tmc_offline_handler

DESCRIPTION
  This function is called when the MS is to transition to the OFFLINE
  state. The function processes the offline command.

DEPENDENCIES
  Task Main Controller has start.

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
LOCAL   void                       tmc_offline_handler( void )
{
  /* send task offline signals */
  tmc_offline_tasks();

  /* Add other offline procedure here.
  */

  #if (defined(FEATURE_DIAG_TASK) && defined(FEATURE_DIAG_COUPLED_ARCHITECTURE))
    /* Send Offline complete signal to diag task in the end.
     */
    TMC_MSG_MED("Offline complete to DIAG task...",0,0,0);
    (void) rex_set_sigs(&diag_tcb, DIAG_OFFLINE_COMPLETE_SIG);
  #endif /* FEATURE_DIAG_TASK && FEATURE_DIAG_COUPLED_ARCHITECTURE */

  #ifdef FEATURE_UIM_RUIM
    /* Indicate to power down the RUIM
    */
    TMC_MSG_MED("Vote to Power down RUIM...", 0, 0, 0);
    (void) uim_power_control(UIM_MC, FALSE);
  #endif /* FEATURE_UIM_RUIM */

}/* tmc_offline_handler */


/* <EJECT> */
/*===========================================================================

FUNCTION tmc_reset_handler

DESCRIPTION
  This function is called when the MS is to transition to the reset
  state. The function processes the reset procedures.

DEPENDENCIES
  Task Main Controller has start.

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
LOCAL   void                       tmc_reset_handler( void )
{

   #ifdef FEATURE_UIM_RUIM
     /* Indicate to power down the RUIM
     */
     TMC_MSG_MED("Vote to Power down RUIM...", 0, 0, 0);
     (void) uim_power_control(UIM_MC, FALSE);
   #endif /* FEATURE_UIM_RUIM */

   #if defined (FEATURE_TMC_TCXOMGR) || defined (FEATURE_TCXOMGR_TMC)
    /* Power off the TCXO manager */
    tcxomgr_powerdown(rex_self(), TMC_NV_CMD_SIG, (void (*)( rex_sigs_type ))tmc_wait);
   #endif

   /* Send stop signal to various tasks. */
   tmc_stop_tasks();

   /* Add other reset procedure here. */

   /* Finish the reset process */
   hw_reset();

}/* tmc_reset_handler */


/* <EJECT> */
/*===========================================================================

FUNCTION tmc_powerdown_handler

DESCRIPTION
  This function is called when the MS is to transition to the power down
  state. The function processes the powerdown command.

DEPENDENCIES
  Task Main Controller has start.

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
LOCAL   void                       tmc_powerdown_handler( void )
{

   #ifdef FEATURE_UIM_RUIM
     /* Indicate to power down the RUIM
     */
     TMC_MSG_MED("Vote to power down RUIM...", 0, 0, 0);
     (void) uim_power_control(UIM_MC, FALSE);
   #endif /* FEATURE_UIM_RUIM */

   #if defined (FEATURE_TMC_TCXOMGR) || defined (FEATURE_TCXOMGR_TMC)
   /* Power off the TCXO manager */
   tcxomgr_powerdown(rex_self(), TMC_NV_CMD_SIG, (void (*)( rex_sigs_type ))tmc_wait);
   #endif

   #ifdef FEATURE_CC_MONITOR
#error code not present
   #endif /* FEATURE_CC_MONITOR */

   /* Send stop signal to various tasks. */
   tmc_stop_tasks();

   /* Add other powerdown procedure here. */

   /* Finish the power down process */
   hw_power_off();

}/* tmc_powerdown_handler */


/*===========================================================================

LOCAL FUNCTION TMC_LPM_HANDLER

DESCRIPTION

  This function handles the low power mode signal

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/

LOCAL   void                       tmc_lpm_handler( void )
{
  TMC_MSG_HIGH("TMC LPM handler", 0, 0, 0);
  #ifdef TMC_HW_IMPLEMENT_LPM
  hw_enter_low_power_mode();
  #endif /* TMC_HW_IMPLEMENT_LPM */
  
  #ifdef FEATURE_GSM_L1_IMPLEMENT_LPM
#error code not present
  #endif
}


/* <EJECT> */
/*===========================================================================

FUNCTION tmc_set_state

DESCRIPTION
  Set tmc operation state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL   void                       tmc_set_state(
        tmc_state_e_type           state
)
{
  tmc_data.state = state;
} /* tmc_set_state */


/* <EJECT> */
#ifdef FEATURE_UIM_RUIM
/*===========================================================================

FUNCTION tmc_store_esn_me

DESCRIPTION
  This function store esn_me to TMC internal data structure.


DEPENDENCIES
  None

RETURN
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL   void                       tmc_store_esn_me (
        dword                      esn_me
)
{
  tmc_data.esn_me = esn_me;
} /* tmc_store_esn_me */
#endif /* FEATURE_UIM_RUIM */

#ifdef CUST_EDITION
#ifdef FEATURE_UIM_EUIMID
EXTERN void tmc_store_meid_me (qword meid_me, int status)
{
  qw_equ(tmc_data.meid_me, meid_me);
  tmc_data.meid_st = status;
} /* tmc_store_meid_me */

EXTERN  int tmc_get_stored_meid_me (qword *pmeid)
{
  qw_equ(*pmeid, tmc_data.meid_me);
  return (tmc_data.meid_st);
} /* tmc_get_stored_meid_me */

#endif
#endif

/* <EJECT> */
/*===========================================================================

FUNCTION tmc_set_init_state

DESCRIPTION
  Set tmc initialization state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL   void                       tmc_set_init_state(
        boolean                    init_state
            /* indicate TMC initialization state */
)
{
  tmc_data.is_init_done = init_state;
} /* tmc_set_state */


/* <EJECT> */
/*===========================================================================

FUNCTION tmc_data_reset

DESCRIPTION
  Reset TMC data.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL   void                       tmc_data_reset( void )
{
#ifdef FEATURE_UIM_RUIM
  tmc_data.esn_me = 0 ;
#endif
  tmc_data.is_init_done = FALSE ;
  tmc_data.state = TMC_PWROFF ;
} /* tmc_set_state */


/* <EJECT> */
/*===========================================================================

FUNCTION tmc_dispatch_commands

DESCRIPTION
  This function handles the commands received in TMC's command queue.
  This function dequeues each command in the command queue
  until the command queue is empty. For each command, it looks at
  the command ID and handles it appropriately. Then it frees the command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL   void                       tmc_dispatch_commands( void )
{
        tmc_cmd_type               *cmd_ptr;    /* To hold received command */

  /* We need to dequeue items from the internal
  ** command queue and process them till the queue is
  ** empty
  */
  TMC_MSG_LOW("Dispatching TMC Commands",0,0,0);

  while( (cmd_ptr = tmc_get_cmd()) != NULL)
  {
    /* Switch on the command id of the received command, and
    ** call the appropriate function to process each command.
    */
    switch( cmd_ptr->cmd_id )
    {
      case TMC_CMD_TYPE_ONLINE:
        tmc_online_handler();
        tmc_set_state(TMC_ONLINE);
        break;

      case TMC_CMD_TYPE_OFFLINE:
        tmc_offline_handler();
        tmc_set_state(TMC_OFFLINE);
        break;

      case TMC_CMD_TYPE_RESET:
        tmc_reset_handler();
        tmc_set_state(TMC_RESET);
        break;

      case TMC_CMD_TYPE_POWERDOWN:
        tmc_powerdown_handler();
        tmc_set_state(TMC_PWROFF);
        break;

      case TMC_CMD_TYPE_LPM:
        tmc_lpm_handler();
        tmc_set_state(TMC_LPM);
        break;

      default:
          /* receive a invalid command*/
          TMC_ERR("A invalid command",0,0,0);
          break;
      } /* switch */

      /* Free the command.
      */
      tmc_free_cmd( cmd_ptr );

    } /* end while */

} /* tmc_dispatch_command */


/* <EJECT> */
/*===========================================================================

FUNCTION tmc_libraries_init_before_tasks_start

DESCRIPTION
  This function does first part of power up initialization. The
  initialization is required before any tasks is defined or started.

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
void config_gpio_uarts(void)
{
  HWIO_OUT(GPIO_PAGE, 9);         // Configure GPIO#9 for UART_DM_RFR_N
  HWIO_OUT(GPIO_CFG, (1<<2));
  
  HWIO_OUT(GPIO_PAGE, 8);         // Configure GPIO#8 for UART_DM_CTS_N
  HWIO_OUT(GPIO_CFG, (1<<2));
  
  HWIO_OUT(GPIO_PAGE, 10);          // Configure GPIO#10 for UART2_DP_RX_DATA
  HWIO_OUT(GPIO_CFG, (1<<2));
  
  HWIO_OUT(GPIO_PAGE, 55);          // Configure GPIO#55 for UART2_TX_DATA
  HWIO_OUT(GPIO_CFG, (4<<2)); 
  
  HWIO_OUT(GPIO_PAGE, 54);          // Configure GPIO#54 for UART2_RFR_n
  HWIO_OUT(GPIO_CFG, (4<<2));
  
  HWIO_OUT(GPIO_PAGE, 53);          // Configure GPIO#53 for UART2_CTS_N
  HWIO_OUT(GPIO_CFG, (3<<2));
}

LOCAL   void                       tmc_libraries_init_before_tasks_start(
        void
)
{
  #ifdef FEATURE_TMC_STACK_PROFILING
    unsigned int i;                        /* counter for number of bytes of stack. */
    i = 0;   /* initializing i to supress compiler warnings */
  #endif
  /*                    Generic Initialization                             */

  #ifdef FEATURE_L4
#error code not present
  #endif

#ifdef FEATURE_PMEM
  /* Make sure to initialize the pmem system */
/* Milan   pmem_6k_init(); */
#endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize TMC NV-Interface
  */
  tmcnv_init(TMC_NV_CMD_SIG, TMC_RPT_TIMER_SIG,tmc_dog_report);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_ENABLE_FLOATING_POINT_ERRORS
  boot_enable_fp_errors();
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  msg_init();                                        /* MSG services       */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dsm_init();                                        /* Data services      */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_REX_IPC                                /* IPC header file */
  ipcmem_pool_init();
  tsvc_init();
#endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifdef T_MSM5500
#error code not present
  #endif /* T_MSM5500 */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef T_AUTO_ICU

    /* Perform MSM auto-detect initialization to determine CPU type
    ** and do related initialization.
    */
    ma_init();

  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifdef FEATURE_REX_DYNA_MEM

    /* Initialize task command heap
    */
    mem_init_heap (&tmc_heap,
                   tmc_heap_mem_buffer,
                   TMC_HEAP_MEM_BUFFER_SIZE,
                   NULL);
    
    #ifndef FEATURE_LOW_MEMORY_USAGE
      mem_init_heap (&tmc_heap_small,
                     tmc_heap_small_mem_buffer,
                     TMC_HEAP_SMALL_MEM_BUFFER_SIZE,
                     NULL);
    #endif
  #endif /* FEATURE_REX_DYNA_MEM */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef TMC_BIO_INIT
    bio_init();                                     /* BIO initialization. */
  #endif /* TMC_BIO_INIT */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* for L4 targets, this is already being done as part of kernel initialization.
  */
  clk_regime_init();            /* Init the clock regime to a known state. */

  #ifdef FEATURE_SURF_CPU_41MHZ
#error code not present
  #endif /* FEATURE_SURF_CPU_41MHZ */

  #ifdef FEATURE_VDSP_AT_48MHZ
#error code not present
  #endif


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  tramp_init();                                /* Interrupt initialization */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  parb_init();                            /* Pacing Arbiter initialization */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if defined (FEATURE_USE_MPM_TCXO_CONTROL) || defined (FEATURE_POWER_COLLAPSE)
#error code not present
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#if defined(FEATURE_SPINLOCK) && defined(FEATURE_ARM_ASM_SPINLOCK)
  {
    /* spin lock init */
    static int32 spinlock_int_array[SPINLOCK_NUM_LOCKS];
    spin_lock_init( smem_data.spinlock_lock_array,
                    spinlock_int_array,
                    SPINLOCK_NUM_LOCKS );
  }
#endif /* FEATURE_SPINLOCK && FEATURE_ARM_ASM_SPINLOCK */

#ifdef FEATURE_SMSM
  /* shared memory state machine init */
  smsm_init( smem_data.smsm_shared_state,
             &smem_data.smsm_int_info,
             &smem_data.smsm_sleep_delay );
#endif /* FEATURE_SMSM */

#ifdef FEATURE_SMEM_LOG
  /* shared memory event log init */
  smem_log_init( smem_data.smem_log_events, /*  */
                 &smem_data.smem_log_idx,
                 SMEM_LOG_NUM_ENTRIES );
#endif /* FEATURE_SMEM_LOG */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_DMOV
  /* Initialize DMOV data mover driver */
  /* The DMOV init on the modem process must happen before the DMOV init
     on the apps processor. So by placing the init on the modem processor
     before time_init. We know the the apps processor has not passed
     time_init and therefore has not reached DMOV init. */
  (void)dmov_init(NULL);
#endif

#if (defined(FEATURE_EFS_PROBE_NAND) && defined (T_MSM7500))
#error code not present
#endif /*FEATURE_EFS_PROBE_NAND && T_MSM7500*/

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifdef FEATURE_USE_TIME_VU
  time_init();                                       /* Time services      */


  /* If FEATURE_TIMER_TASK is defined, then the time tick interrupt is
     serviced in the task context.  The timer task should be started
     before enabling the interrupt so that the task is running when an
     interrupt occurs.
  */

   #ifdef FEATURE_TIMER_TASK

    #ifdef TIMETEST
      timer_tcb.leds = TIMETEST_TIMER_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < TIMER_STACK_SIZ; i++) {
        timer_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(timer_tcb.task_name, "TIMER" , sizeof(timer_tcb.task_name));

    /* Create and Start TIMER task.
    */
    TMC_MSG_MED("Creating and Starting TIMER task", 0, 0, 0);
    rex_def_task(&timer_tcb,
                 (rex_stack_word_type*) timer_stack,
                 TIMER_STACK_SIZ,
                 (rex_priority_type) TIMER_PRI,
                 timer_task,
                 0L);

   #endif /* FEATURE_TIMER_TASK */

  #endif /* FEATURE_USE_TIME_VU */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If FEATURE_PROGRESSIVE_BOOT is defined, then the time tick interrupt is
   serviced in the task context. The interrupt task should be started
   before enabling the interrupt so that the task is running when an
   interrupt accurrs
  */


  #if defined(FEATURE_PROGRESSIVE_BOOT) && !defined(FEATURE_TIMER_TASK) && !defined(FEATURE_L4)

    #ifdef TIMETEST
      interrupt_tcb.leds = TIMETEST_INTERRUPT_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < INTERRUPT_STACK_SIZ; i++) {
        interrupt_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(interrupt_tcb.task_name, "INT " , sizeof(interrupt_tcb.task_name));

    /* Create and start INTERRUPT task .
    */
    TMC_MSG_MED("Creating and Starting INTERRUPT task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&interrupt_tcb,
                        (unsigned char *) interrupt_stack,
                        INTERRUPT_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) INTERRUPT_PRI,
                        interrupt_task,
                        0L,
                        "INT",
                        FALSE,
                        DOG_INTERRUPT_RPT );
    #else
      rex_def_task(&interrupt_tcb,
                   (rex_stack_word_type*) interrupt_stack,
                   INTERRUPT_STACK_SIZ,
                   (rex_priority_type) INTERRUPT_PRI,
                   interrupt_task,
                   0L);
    #endif
  #endif /* FEATURE_PROGRESSIVE_BOOT && !FEATURE_TIMER_TASK */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  gpio_int_init();                     /* Initialize GPIO interrupt handler*/

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  _enable();                    /* Enable interrupts and start processing. */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_L4
  clk_init();                                           /* Initialize Clock*/
  #endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef TMC_HW_INIT
  {
    uint32 config_value = 0;
    /* IQ Mux, AGC, and miscellaneous hardware initialization */
    hw_init(config_value);
  }
  #endif /* TMC_HW_INIT */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the TLMM. */
  #if !defined(FEATURE_L4) || defined(T_MSM6800)
  #ifdef T_MSM31

  tlmm_init ();

  #endif /* T_MSM31 */
  #endif /* !FEATURE_L4 || T_MSM6800 */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize RF */
  rflib_init();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Initialize Power Manager */

  #if defined (FEATURE_PM1000) || defined (FEATURE_PM1000_LDO_ONLY)
#error code not present
  #endif

  #ifdef FEATURE_PMIC

    #ifdef FEATURE_WCDMA_PLT
#error code not present
    #endif /* FEATURE_WCDMA_PLT */
    {
      pm_init();            /* Please look at dependency notes inside the
                            ** function before move around those code
                            */
      pm_init_delayed();
    }

  /* Issue with cold start for RX ADC voltage level shifting at 1.2 V
  ** For Pedro version 1, it needs to set MSMC to 1.3 V in order for
  ** RX ADC to work.
  */
  hw_init_qsc11x0_hw_rev();

#ifndef QSC11X0_PG2_POWER_GRID
  if( qsc11x0_hw_rev.msm_hw_version == 0 )
  {
     pm_vreg_set_level(PM_VREG_MSMC_ID, 1300);
  }
   else if (qsc11x0_hw_rev.msm_hw_version == 1 && qsc11x0_hw_rev.pg == 3 )
  {
     // S2 needs to be 2.3 V for cold temperature issue with RXADC
     pm_vreg_set_level(PM_VREG_S2_ID, 2300);
    // Turn off MSMC as it does not use for PG3 as it use Preston 2.0 Silicon
    pm_vreg_control(PM_OFF_CMD,PM_VREG_MSMC_M);
  }
#endif

  // Work around for turning on MSMA Vreg with KIP B0 on PG1 and PG3
  // Can't not enable MSMA Vreg from its master
  pm_vreg_control(PM_ON_CMD, PM_VREG_MSMA_M); 

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef QSC11X0_PG2_POWER_GRID
   /* 
   ** This is a temporary fix for the new power grid design, the final fix would
   ** be handle by the pmic driver team!!!
   */

   /*
   Register Writes (Address, Data, Comment)
   0x6F     0xAC    Set RUIM to low voltage mode
   0x13     0x98     Enable RUIM, 1.35V
   0x18     0xFB     Disable MPLL
   0x04     0x32     Set S1 to 1.3V
   0x05     0x05     Set S2 to 2.2V
   0x0E     0x06     Set RFA2 to 1.8V
   Turn off MSMC, MSMA, RFA1
   RFA2 uses to simulate noise to RF, but can turn off for power measurement
   */

  // Step 1, configure RUIM to 1.35 V
  outpdw(0x80018008,0x006f00a0);
  outpdw(0x80018008,0x006F00AC);  // Reg addr = 0x6f, write, bank 2,data 0xC
  // Program RUIM to 2.7 V, but it would be 1.35 V because of prev step
  pm_vreg_set_level(PM_VREG_RUIM_ID, 2700);

  // Step 2, enable RUIM
  pm_vreg_control(PM_ON_CMD,PM_VREG_RUIM_M); // turn on VREG_RUIM

  // Step 3, disable MPLL
  pm_vreg_control(PM_OFF_CMD,PM_VREG_MPLL_M); // Disable VREG_MPLL

  // Step 4, set S1 to 1.3 V
  pm_vreg_set_level(PM_VREG_S1_ID, 1300);

  // Step 5, set S2 to 2.2 V
  pm_vreg_set_level(PM_VREG_S2_ID, 2200);

  // Step 6, set RFA2 to 1.8 V, we can turn off RFA2 for power measurement build
  pm_vreg_set_level(PM_VREG_RFA2_ID, 1800);

  // Turn off MSMC as it does not use
  pm_vreg_control(PM_OFF_CMD,PM_VREG_MSMC_M);
  // Turn off RFA1 as it does not use
  pm_vreg_control(PM_OFF_CMD,PM_VREG_RFA1_M);
  // Turn off MSMA as it does not use
  pm_vreg_control(PM_OFF_CMD,PM_VREG_MSMA_M);

#endif /* QSC11X0_PG2_POWER_GRID */

    #if defined(SIRIUS_PLATFORM) && defined(WOLF_5)
#error code not present
    #endif /* defined(SIRIUS_PLATFORM) && defined(WOLF_5) */

    #if 0 // Gemsea Remove def FEATURE_USE_TIME_VU
      /* The PMIC has been initialized, get time-of-day from the PMIC's RTC */
      time_set_from_pmic();
    #endif /* FEATURE_USE_TIME_VU */

    #endif /* FEATURE_PMIC */

  #ifdef FEATURE_HS_DETECT_BASEBAND_TYPE
#error code not present
    #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  #ifdef FEATURE_PMIC_RTC
    /* Initialization for RTC */
  #ifndef FEATURE_BRINGUP_DIAG_ONLY
    clk_rtc_init();
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */
  #endif /* FEATURE_PMIC_RTC */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  /* For UMTS target, the mode is ignored. For GSM, it is done in gl1_hw_init
  ** again.
  */
   adie_init(ADIE_CDMA_MODE);
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef TMC_I2C_INIT
  i2c_init();                                        /* I2C driver     */
  #endif
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize SIO */

   sio_init();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize USB host API */

  #if defined (FEATURE_USB) && !defined (FEATURE_NO_USB_HW_ON_PROCESSOR)
    #ifdef FEATURE_USB_HOST
      usbhost_api_init();
    #endif
   #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_APS
   /* Initialize APS */
   aps_init();
  #endif /* FEATURE_APS */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* UIM driver */
  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #if (defined ( FEATURE_UIM_DRIVER ) && !defined (FEATURE_GSM_PLT))
  {
    //Gemsea Remove extern int uim_dev_init( void );
    uim_dev_init();
  }
  #endif /* FEATURE_UIM_DRIVER && !FEATURE_GSM_PLT*/
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef TMC_I2C_INIT
    i2c_init();                                        /* I2C driver     */
  #endif
#endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
     /* Add charger here. */
  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_BATTERY_CHARGER
    tmc_chg_init();
  #endif
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*                   Mode Specific Initialization                        */

  /*                         1x specific                                 */

#ifdef TMC_1X
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  db_init();                                         /* Database services  */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Initialize the Encoder driver so that the RF driver can */
  /* use it's outputs to initialize the RF synthesizers.     */

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  enc_init();
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_ACP
#error code not present
  #endif /* FEATURE_ACP */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  /*
  ** RF one time power up initialization consists of two parts.
  **   First part is done before any task is created and start.
  **   Second part is done after NV task is started for reading
  **   NV calibration data.
  **
  ** When multimode RF driver is ready, NV calibration data will
  ** be read inside RF driver. RF API will be updated.
  ** Move rf_init() to where protocal is activated when 1x and GSM are enabled.
  ** Move rf_init() and rf_init_delayed() back so that if rf init fail, tmc
  ** will send mode offline command.
  */

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifndef FEATURE_MULTIMODE_RFAPI
  rf_init();                                         /* RF driver          */
  #endif /*FEATURE_MULTIMODE_RFAPI*/
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the CDMA ASIC(s) prior to other device drivers, as they    */
  /* contain GPIO's which are used to control other parts of the system.   */
  /* Re-initialize the Encoder, now that we have a good CHIPx8 clock.      */

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  clk_regime_enable( CLK_RGM_CHIPX8_M );/* Enable all chipx8 clock regimes.*/
  #endif
  
  #ifndef T_BRINGUP
  #ifdef FEATURE_DUAL_CLK_RGM
  clk_regime_msm_enable( CLK_RGM_GEN_M );
  #else
  clk_regime_enable( CLK_RGM_GEN_M );      /* Enable General clock regime. */
  #endif
  #endif

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef T_MSM5000
   clk_regime_enable (CLK_RGM_DEC_M);
  #endif
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef T_MSM5000                                   /* Decoder driver     */
    deint_init();
  #else
#error code not present
  #endif
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_ATI_GRAPHICS
    clk_regime_msm_enable( CLKRGM_GRP_CLK );
  #endif
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef T_MSM6500
  /* Enable rxchipx8 clock regime */
  clk_regime_enable( CLK_RGM_RXCHIPX8_M );

  /* Enable offline clock regime */
  clk_regime_enable( CLK_RGM_OFFLINE_M );

  /* Initialize demod registers - this function does a global reset of the
     demod registers, without regard to 1x-specific init.  The regular
     1x srch init function, srch_util_dmod_reset() should still be called
     when entering 1x */
  srch_util_dmod_powerup_reset();
  #endif /* T_MSM6500 */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  /* If MDSP task is used, srch_util_dmod_reset and demod_download_image
  ** are not used during power up. The regular 1x srch init function,
  ** srch_util_dmod_reset() should still be called when entering 1x.
  **
  */
  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifndef FEATURE_MDSP_TASK
    TMC_MSG_MED("Initializing DMOD driver...", 0, 0, 0);
    srch_util_dmod_reset();
  #endif
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_ACP
#error code not present
  #endif /* FEATURE_ACP */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*Initialize anything else that is left to be initialize in the msm*/

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  msm_device_init ();
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef T_MSM5000

    #ifdef T_MSM4500
#error code not present
    #else /* T_MSM4500 */

        #ifndef FEATURE_DMOD_IMAGE_IN_ROM

          #ifdef T_MSM6050

            /* For T_MSM6300 target, demod... is done by rf_init_delayed()
            ** until Aaron checks in his changes
            */
            #ifndef FEATURE_MDSP_TASK
              demod_download_image(TRUE);
            #endif

          #else
#error code not present
          #endif

        #endif /* FEATURE_DMOD_IMAGE)_IN_ROM */

      #ifdef T_MSM5500
#error code not present
      #endif /* T_MSM5500 */

    #endif /* T_MSM4500 */

  #endif /* T_MSM5000 */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_HDR
#error code not present
  #endif /* FEATURE_HDR */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_VECTORONE_DEVICE_IF
#error code not present
  #endif
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */
  /*  The following code need to clean up                                  */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef LIFETEST
#error code not present
  #endif
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

#endif /* TMC_1X */

  /*                         GSM specific                                  */

#ifdef FEATURE_GSM
#error code not present
#endif /* FEATURE_GSM */

#ifdef FEATURE_WCDMA
#error code not present
#endif /* FEATURE_WCDMA */


#ifdef FEATURE_WLAN
#error code not present
#endif

#ifndef FEATURE_BRINGUP_DIAG_ONLY
#if (defined(FEATURE_FACTORY_TESTMODE) && !defined(FEATURE_FTM_TASK))
  ftm_tmc_init();     /* ftm initializaiton that has to be done in TMC task. */
#endif /* FEATURE_FACTORY_TESTMODE && ! FEATURE_FTM_TASK */
#endif /* FEATURE_BRINGUP_DIAG_ONLY */

  /* Register TMC dispatch table through diag interface */
  tmcdiag_init();

#ifdef FEATURE_PROFDIAG
  /* Register Profiling dispatch table through diag interface */
  profdiag_init();
#endif

  /* Brew Initialization*/
#ifndef FEATURE_BRINGUP_DIAG_ONLY
#if !MIN_BREW_VERSIONEx(3,1,4)   /* Brew 3.1.4 removed brew_pwrup_init() */
#if defined(FEATURE_BREW_3_0) || defined(FEATURE_BREW_PWRUP_INIT)
  brew_pwrup_init();
#endif /* FEATURE_BREW_3_0 || FEATURE_BREW_PWRUP_INIT */
#endif /* !MIN_BREW_VERSIONEx(3,1,4) */
#endif /* FEATURE_BRINGUP_DIAG_ONLY */

#ifdef FEATURE_L4LINUX
#error code not present
#endif

} /* tmc_libraries_init_before_tasks_start */


/* <EJECT> */
/*===========================================================================

FUNCTION tmc_define_tasks
  This function creates all the software tasks with the rex_def_task
  procedure.  After a task has been created it sets the TMC_ACK_SIG to
  communicate to the Task Main Control task that it has completed static
  initialization.  This procedure waits to receive the TMC_ACK_SIG before
  creating the next software task.

DEPENDENCIES
  The watchdog task resetting the Watchdog timer while attempting to
  start all other tasks. If the unable to create other tasks in the time
  allotted by the Watchdog task, the Watchdog task will log the error and
  reset the microprocessor.

RETURN VALUE
  void

SIDE EFFECTS
  After this procedure all the tasks in the Software system will have
  performed preliminary initialization and will be waiting for the
  startup signal from the Main Control task.

===========================================================================*/
LOCAL   void                       tmc_define_tasks( void )
{

  #ifdef FEATURE_TMC_STACK_PROFILING
    unsigned int i;                        /* counter for number of bytes of stack. */
  #endif

  #ifdef FEATURE_ENHANCED_REX_TASK
    /* Enable and disable autodog for MC as a way of getting the dog report
    ** id into the TCB.
    */
    (void) rex_autodog_enable( DOG_TMC_RPT );
    (void) rex_autodog_disable( );
  #endif

  #ifdef FEATURE_SOFTWARE_PROFILE
    #ifdef FEATURE_MSMHWIO
       rex_init_profile( HWIO_ADDR(TIME_TICK2_COUNT) );
    #else
       rex_init_profile( TIME_TICK2_COUNT_WH );
    #endif /* FEATURE_MSMHWIO */
    /* numticks are set to 0 for other tasks in rex_def_task_internal()
    */
    tmc_tcb.numticks = 0;
  #endif

  #ifdef TIMETEST

    #ifdef TIMETEST_PROF
     /* Support timing tests via the LED output buffer.
     */
    timetest_prof_init();
    #endif /* TIMETEST_PROF */

    tmc_tcb.leds = TIMETEST_TMC_TASK_ID;

  #endif /* TIMETEST*/

  /* Clear ACK signal.
  */
  (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 /* If FEATURE_PROGRESSIVE_BOOT is defined, then the pager task should be
    started  as early as possible to reduce the number of page faults
    handled in abort context
  */
  #if defined(FEATURE_PROGRESSIVE_BOOT) && !defined(FEATURE_TIMER_TASK) && !defined(FEATURE_L4)

    #ifdef TIMETEST
      pager_tcb.leds = TIMETEST_PAGER_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < PAGER_STACK_SIZ; i++) {
        pager_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(pager_tcb.task_name, "PAGR" , sizeof(pager_tcb.task_name));

    /* Create PAGER task and start it.
    */
    TMC_MSG_MED("Creating and Starting PAGER task", 0, 0, 0);
    rex_def_task(&pager_tcb,
                 (rex_stack_word_type*) pager_stack,
                 PAGER_STACK_SIZ,
                 (rex_priority_type) PAGER_PRI,
                 pager_task,
                 0L);
  #endif /* FEATURE_PROGRESSIVE_BOOT */

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef TMC_SLEEP

    #ifdef TIMETEST
      sleep_tcb.leds = TIMETEST_SLEEP_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < SLEEP_STACK_SIZ; i++) {
        sleep_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(sleep_tcb.task_name, "SLEEP" , sizeof(sleep_tcb.task_name));

    /* Other tasks signal the sleep task when they are defined, so we should
    ** define the sleep task first so it exists when it is signaled.
    */
    TMC_MSG_MED("Creating Sleep task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&sleep_tcb,
                        (unsigned char *) sleep_stack,
                        SLEEP_STACK_SIZ* sizeof(rex_stack_word_type),
                        (rex_priority_type) SLEEP_PRI,
                        sleep_task,
                        0L,
                        "SLEEP",
                        FALSE,
                        DOG_SLEEP_RPT);
    #else
      rex_def_task(&sleep_tcb,
                   (rex_stack_word_type*) sleep_stack,
                   SLEEP_STACK_SIZ,
                   (rex_priority_type) SLEEP_PRI,
                   sleep_task,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

  #endif /* TMC_SLEEP */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_APPS_TCXO_SLEEP
 
    #ifdef TIMETEST
      dem_tcb.leds = TIMETEST_DEM_TASK_ID;
    #endif /* TIMETEST */
 
    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < DEM_STACK_SIZ; i++) {
        dem_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif
    (void) std_strlcpy(dem_tcb.task_name, "DEM" , sizeof(dem_tcb.task_name));
    TMC_MSG_MED("Creating DEM task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&dem_tcb,
                        (unsigned char *) dem_stack,
                        DEM_STACK_SIZ* sizeof(rex_stack_word_type),
                        (rex_priority_type) DEM_PRI,
                        dem_task,
                        0L,
                        "DEM",
                        FALSE,
                        0L);
    #else
      rex_def_task(&dem_tcb,
                   (rex_stack_word_type*) dem_stack,
                   DEM_STACK_SIZ,
                   (rex_priority_type) DEM_PRI,
                   dem_task,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
 
  #endif /* FEATURE_APPS_TCXO_SLEEP */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */
 
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_SMD

    /* Create SMD task */
    #ifdef TIMETEST
      smd_tcb.leds = TIMETEST_SMD_TASK_ID;
    #endif

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < SMD_PKT_LLW_STACK_SIZ; i++) {
        smd_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy( &smd_tcb.task_name[0], "SHARED_MEMORY_DRIVER"  , sizeof( &smd_tcb.task_name[0]));

    TMC_MSG_MED( "Creating SMD task thread", 0, 0, 0 );
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&smd_tcb,
                        (unsigned char *) smd_stack,
                        SMD_STACK_SIZ * sizeof(rex_stack_word_type),
                        SMD_PRI,
                        smd_task,
                        0L,
                        "SMD",
                        FALSE,
                        0 );
    #else
      rex_def_task(&smd_tcb,
                   ( rex_stack_word_type* ) smd_stack,
                   SMD_STACK_SIZ,
                   SMD_PRI,
                   smd_task,
                   0L );
    #endif
    (void) tmc_wait( TMC_ACK_SIG );
    (void) rex_clr_sigs( rex_self(), TMC_ACK_SIG );

  #endif /*FEATURE_SMD*/

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_ONCRPC
#error code not present
  #endif /* FEATURE_ONCRPC */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_DOG

    #ifdef TIMETEST
      dog_tcb.leds = TIMETEST_DOG_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < DOG_STACK_SIZ; i++) {
        dog_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(dog_tcb.task_name, "DOG " , sizeof(dog_tcb.task_name));

    /* Create DOG task and wait for ack back from it.
    */
    TMC_MSG_MED("Creating DOG task", 0, 0, 0);
    rex_def_task(&dog_tcb,
                 (rex_stack_word_type*) dog_stack,
                 DOG_STACK_SIZ,
                 (rex_priority_type) DOG_PRI,
                 dog_task,
                 0L);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

  #endif /* FEATURE_DOG */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef SIRIUS_PLATFORM
#error code not present
#endif /* SIRIUS_PLATFORM */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifndef FEATURE_SC2X_QDSP_NOT_PORT
  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_QDSP_TASK

    #ifdef TIMETEST
      qdsp_tcb.leds = TIMETEST_QDSP_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < QDSP_STACK_SIZ; i++) {
        qdsp_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(qdsp_tcb.task_name, "QDSP" , sizeof(qdsp_tcb.task_name));

    /* Create QDSP Services task and wait for ack back from it.
    */
    TMC_MSG_MED("Creating QDSP Services task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&qdsp_tcb,
                        (unsigned char *) qdsp_stack,
                        QDSP_STACK_SIZ* sizeof(rex_stack_word_type),
                        (rex_priority_type) QDSP_PRI,
                        qdsp_task,
                        0L,
                        "QDSP",
                        FALSE,
                        DOG_QDSP_RPT);
    #else
      rex_def_task(&qdsp_tcb,
                   (rex_stack_word_type*) qdsp_stack,
                   QDSP_STACK_SIZ,
                   (rex_priority_type) QDSP_PRI,
                   qdsp_task,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

  #endif /* FEATURE_QDSP_TASK */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */
#endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_QDSP_RTOS

    #ifdef TIMETEST
      adsp_rtos_tcb.leds = TIMETEST_QDSP_RTOS_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < ADSP_RTOS_STACK_SIZ; i++) {
        adsp_rtos_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(adsp_rtos_tcb.task_name, "ADSPOS" , sizeof(adsp_rtos_tcb.task_name));

    /* Create ADSP RTOS Services task and wait for ack back from it.
    */
    TMC_MSG_MED("Creating ADSP RTOS Services task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&adsp_rtos_tcb,
                        (unsigned char *) adsp_rtos_stack,
                        ADSP_RTOS_STACK_SIZ* sizeof(rex_stack_word_type),
                        (rex_priority_type) ADSP_RTOS_PRI,
                        adsp_rtos_task,
                        0L,
                        "ADSPOS",
                        FALSE,
                        DOG_ADSP_RTOS_RPT);
    #else
      rex_def_task(&adsp_rtos_tcb,
                   (rex_stack_word_type*) adsp_rtos_stack,
                   ADSP_RTOS_STACK_SIZ,
                   (rex_priority_type) ADSP_RTOS_PRI,
                   adsp_rtos_task,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

  #endif /* FEATURE_ADSP_RTOS_TASK */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_VOC_TASK

    #ifdef TIMETEST
      voc_tcb.leds = TIMETEST_VOC_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < VOC_STACK_SIZ; i++) {
        voc_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(voc_tcb.task_name, "VOC " , sizeof(voc_tcb.task_name));

    /* Create VOC task and wait for ack back from it.
    */
    TMC_MSG_MED("Creating VOC task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&voc_tcb,
                        (unsigned char *) voc_stack,
                        VOC_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) VOC_PRI,
                        voc_task,
                        0L,
                        "VOC",
                        FALSE,
                        DOG_VOC_RPT);
    #else
      rex_def_task(&voc_tcb,
                   (rex_stack_word_type*) voc_stack,
                   VOC_STACK_SIZ,
                   (rex_priority_type) VOC_PRI,
                   voc_task,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

  #endif /* FEATURE_VOC_TASK */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_SND_TASK

    #ifdef TIMETEST
      snd_tcb.leds = TIMETEST_SND_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < SND_STACK_SIZ; i++) {
        snd_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(snd_tcb.task_name, "SND " , sizeof(snd_tcb.task_name));

    /* Create SND task and wait for ack back from it.
    */
    TMC_MSG_MED("Creating SND task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
    rex_def_task_ext2(&snd_tcb,
                      (unsigned char *) snd_stack,
                      SND_STACK_SIZ* sizeof(rex_stack_word_type),
                      (rex_priority_type) SND_PRI,
                      snd_task,
                      0L,
                      "SND",
                      FALSE,
                      DOG_SND_RPT);
    #else
      rex_def_task(&snd_tcb,
                   (rex_stack_word_type*) snd_stack,
                   SND_STACK_SIZ,
                   (rex_priority_type) SND_PRI,
                   snd_task,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG) ;
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

  #endif /* FEATURE_SND_TASK */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_AVS_SBCENC_TASK

    #ifdef TIMETEST
      sbcenc_tcb.leds = TIMETEST_SND_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < SBCENC_STACK_SIZ; i++) {
        sbcenc_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(sbcenc_tcb.task_name, "SBCENC " , sizeof(sbcenc_tcb.task_name));

    /* Create SBCENC task and wait for ack back from it.
    */
    TMC_MSG_MED("Creating SBCENC task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
    rex_def_task_ext2(&sbcenc_tcb,
                      (unsigned char *) sbcenc_stack,
                      SBCENC_STACK_SIZ* sizeof(rex_stack_word_type),
                      (rex_priority_type) SBCENC_PRI,
                      audiosbcenc_task,
                      0L,
                      "SBCENC",
                      FALSE,
                      DOG_SBCENC_RPT);
    #else
      rex_def_task(&sbcenc_tcb,
                   (rex_stack_word_type*) sbcenc_stack,
                   SBCENC_STACK_SIZ,
                   (rex_priority_type) SBCENC_PRI,
                   audiobcenc_task,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG) ;
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

  #endif /* FEATURE_AVS_SBCENC_TASK */
#endif /* FEATURE_BRINGUP_DIAG_ONLY */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifndef FEATURE_BRINGUP_DIAG_ONLY

#if !defined(FEATURE_THIN_UI) && !defined(FEATURE_UI_CORE_REMOVED)
  #ifdef FEATURE_HS

    #ifdef TIMETEST
      hs_tcb.leds = TIMETEST_HS_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < HS_STACK_SIZ; i++) {
        hs_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(hs_tcb.task_name, "HS  " , sizeof(hs_tcb.task_name));

    /* Create HS task and wait for ack back from it.
    */
    TMC_MSG_MED("Creating HS task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&hs_tcb,
                        (unsigned char *) hs_stack,
                        HS_STACK_SIZ* sizeof(rex_stack_word_type),
                        (rex_priority_type) HS_PRI,
                        hs_task,
                        0L,
                        "HS",
                        FALSE,
                        DOG_HS_RPT);
    #else
      rex_def_task(&hs_tcb,
                   (rex_stack_word_type*) hs_stack,
                   HS_STACK_SIZ,
                   (rex_priority_type) HS_PRI,
                   hs_task,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

  #endif /* FEATURE_HS */
#else /* !defined(FEATURE_THIN_UI) && !defined(FEATURE_UI_CORE_REMOVED) */
  /* In a dual proc system there will be no HS Service on the Modem,
   * so we must explicitly tell the PMIC to set up the Power key.
   */
  #if defined(FEATURE_PMIC) && !defined (FEATURE_KEYPAD_USES_GPIO_FOR_PWR_KEY)
    /* Initialize the Power Key */
    pm_pwr_key_init();
  #endif
#endif /* !defined(FEATURE_THIN_UI) && !defined(FEATURE_UI_CORE_REMOVED) */
#endif /* FEATURE_BRINGUP_DIAG_ONLY */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    
  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_IS2000_TDSO

    #ifdef TIMETEST
      tdso_tcb.leds = TIMETEST_TDSO_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < TDSO_STACK_SIZ; i++) {
        tdso_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(tdso_tcb.task_name, "TDSO" , sizeof(tdso_tcb.task_name));

    /* Create TDSO task and wait for ack back from it.
    */
    TMC_MSG_MED("Creating TDSO task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&tdso_tcb,
                        (unsigned char *) tdso_stack,
                        TDSO_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) TDSO_PRI,
                        tdso_task,
                        0L,
                        "TDSO",
                        FALSE,
                        DOG_TDSO_RPT);
    #else
      rex_def_task(&tdso_tcb,
                   (rex_stack_word_type*) tdso_stack,
                   TDSO_STACK_SIZ,
                   (rex_priority_type) TDSO_PRI,
                   tdso_task,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

  #endif /* FEATURE_IS2000_TDSO */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_IDS
#error code not present
  #endif /* FEATURE_IDS */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_EFS

  #ifndef T_QUASAR
    #if (defined(FEATURE_EFS2) && !defined(FEATURE_WCDMA_PLT) && !defined(FEATURE_GSM_PLT))
      /* if the panic keypad combination is pressed, set fs_fresh_start */
    if(keypad_is_panic_chord_pressed())
        fs_fresh_start=1;
    #endif /* FEATURE_EFS2 && ! FEATURE_WCDMA_PLT */
  #endif /* T_QUASAR */

    #ifdef TIMETEST
      fs_tcb.leds = TIMETEST_FS_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < FS_STACK_SIZ; i++) {
        fs_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(fs_tcb.task_name, "FS  " , sizeof(fs_tcb.task_name));

    /* Create Embedded File System task and wait for ack back from it.
    */
    TMC_MSG_MED("Creating FS task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&fs_tcb,
                        (unsigned char *)fs_stack,
                        FS_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) FS_PRI,
                        fs_task,
                        0L,
                        "FS",
                        FALSE,
                        DOG_FS_RPT);
    #else
      rex_def_task(&fs_tcb,
                   (rex_stack_word_type*)fs_stack,
                   FS_STACK_SIZ,
                   (rex_priority_type) FS_PRI,
                   fs_task,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

    #if (defined(FS_FEATURE_RMTEFS_SERVER)||defined(FS_FEATURE_IG_IPC_SERVER)) \
            && !defined(FEATURE_IG_EFS_EXT_SERVER)

      #ifdef TIMETEST
        efs_srvr_tcb.leds = TIMETEST_EFS_SRVR_TASK_ID;
      #endif /* TIMETEST */

      #ifdef FEATURE_TMC_STACK_PROFILING
        for (i = 0; i < EFS_SRVR_STACK_SIZ; i++) {
          efs_srvr_stack[i] = TMC_STACK_PROFILING_DWORD;
        }
      #endif
      (void) std_strlcpy(efs_srvr_tcb.task_name, "EFS Srvr" , sizeof(efs_srvr_tcb.task_name));

      /* Create EFS server task and wait for ack back from it.
      */
      TMC_MSG_MED("Creating EFS Srvr task", 0, 0, 0);
      #ifdef FEATURE_ENHANCED_REX_TASK
        rex_def_task_ext2(&efs_srvr_tcb,
                          (unsigned char *)efs_srvr_stack,
                          EFS_SRVR_STACK_SIZ * sizeof(rex_stack_word_type),
                          (rex_priority_type) EFS_SRVR_PRI,
                          efs2_server_task,
                          0L,
                          "EFS_SRVR",
                          FALSE,
                          DOG_FS_RPT);
      #else
        rex_def_task(&efs_srvr_tcb,
                     (rex_stack_word_type*)efs_srvr_stack,
                     EFS_SRVR_STACK_SIZ,
                     (rex_priority_type) EFS_SRVR_PRI,
                     efs2_server_task,
                     0L);
      #endif

      (void) tmc_wait(TMC_ACK_SIG);
      (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

    #endif /* FS_FEATURE_RMTEFS_SERVER && !FEATURE_IG_EFS_EXT_SERVER */
  #endif /* FEATURE_EFS */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_EFS_FDI
    #ifdef TIMETEST
      fdi_bg_tcb.leds = TIMETEST_FID_BG_TASK_ID;
    #endif /* TIMETEST */


    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < FDI_BG_STACK_SIZ; i++) {
        fdi_bg_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(fdi_bg_tcb.task_name, "FDI  " , sizeof(fdi_bg_tcb.task_name));

    /* Create Intel FDI File System task.
    */
    TMC_MSG_MED("Creating FDI task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&fdi_bg_tcb,
                        (unsigned char *)fdi_bg_stack,
                        FDI_BG_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) FDI_BG_PRI,
                        fdi_bg_task,
                        0L,
                        "FDI",
                        FALSE,
                        DOG_FDI_BG_RPT);
    #else
      rex_def_task(&fdi_bg_tcb,
                   (rex_stack_word_type*)fdi_bg_stack,
                   FDI_BG_STACK_SIZ,
                   (rex_priority_type) FDI_BG_PRI,
                   fdi_bg_task,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_EFS_FDI */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_EFS_SFAT

    #ifdef TIMETEST
      sfat_tcb.leds = TIMETEST_SFAT_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < SFAT_STACK_SIZ; i++) {
        sfat_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(sfat_tcb.task_name, "SFAT" , sizeof(sfat_tcb.task_name));

    /* Create SFAT task and wait for ack back from it.
    */
    TMC_MSG_MED("Creating SFAT task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&sfat_tcb,
                        (unsigned char *)sfat_stack,
                        SFAT_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) SFAT_PRI,
                        sfat_task,
                        0L,
                        "SFAT",
                        FALSE,
                        DOG_SFAT_RPT);
    #else
      rex_def_task(&sfat_tcb,
                   (rex_stack_word_type*)sfat_stack,
                   SFAT_STACK_SIZ,
                   (rex_priority_type) SFAT_PRI,
                   sfat_task,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

  #endif /* FEATURE_EFS_SFAT */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_SEC_PROCESS_INIT
  #ifdef FEATURE_IXFILESVC

    #ifdef TIMETEST
      ix_filesvc_tcb.leds = 0;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < IXFILESVC_STACK_SIZ; i++) {
        ix_filesvc_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(ix_filesvc_tcb.task_name, "IXFILE " , sizeof(ix_filesvc_tcb.task_name));

    /* Create IxFile Server task and wait for ack back from it.
    */
    TMC_MSG_MED("Creating IxFile Server task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&ix_filesvc_tcb,
                        (unsigned char *) ix_filesvc_stack,
                        IXFILESVC_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) IXFILESVC_PRI,
                        IxFile_IPC_Server,
                        0L,
                        "IXFILE",
                        FALSE,
                        -1);
    #else
      rex_def_task(&ix_filesvc_tcb,
                   (rex_stack_word_type*) ix_filesvc_stack,
                   IXFILESVC_STACK_SIZ,
                   (rex_priority_type) IXFILESVC_PRI,
                   IxFile_IPC_Server,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

  #endif /* FEATURE_IXFILESVC */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_IXSQLSVC

    #ifdef TIMETEST
      ix_sqlclnt_tcb.leds = 0;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < IXSQLCLNT_STACK_SIZ; i++) {
        ix_sqlclnt_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(ix_sqlclnt_tcb.task_name, "IXSQL CLNT" , sizeof(ix_sqlclnt_tcb.task_name));

    /* Create IxSQL Client task and wait for ack back from it.
    */
    TMC_MSG_MED("Creating IxSQL Client task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&ix_sqlclnt_tcb,
                        (unsigned char *) ix_sqlclnt_stack,
                        IXSQLCLNT_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) IXSQLCLNT_PRI,
                        IxSQLClientHandler_IPC_Server,
                        0L,
                        "IXSQLCLNT",
                        FALSE,
                        -1);
    #else
      rex_def_task(&ix_sqlclnt_tcb,
                   (rex_stack_word_type*) ix_sqlclnt_stack,
                   IXSQLCLNT_STACK_SIZ,
                   (rex_priority_type) IXSQLCLNT_PRI,
                   IxSQLClientHandler_IPC_Server,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

    #ifdef TIMETEST
      ix_sqlsvc_tcb.leds = 0;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < IXSQLSVC_STACK_SIZ; i++) {
        ix_sqlsvc_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(ix_sqlsvc_tcb.task_name, "IXSQL " , sizeof(ix_sqlsvc_tcb.task_name));

    /* Create IxSQL Server task and wait for ack back from it.
    */
    TMC_MSG_MED("Creating IxSQL Server task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&ix_sqlsvc_tcb,
                        (unsigned char *) ix_sqlsvc_stack,
                        IXSQLSVC_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) IXSQLSVC_PRI,
                        IxSQL_IPC_Server,
                        0L,
                        "IXSQL",
                        FALSE,
                        0);
    #else
      rex_def_task(&ix_sqlsvc_tcb,
                   (rex_stack_word_type*) ix_sqlsvc_stack,
                   IXSQLSVC_STACK_SIZ,
                   (rex_priority_type) IXSQLSVC_PRI,
                   IxSQL_IPC_Server,
                   0L);
  #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);


  #endif /* FEATURE_IXSQLSVC */
  #endif /* !FEATURE_SEC_PROCESS_INIT */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_NV

    #ifdef TIMETEST
      nv_tcb.leds = TIMETEST_NV_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < NV_STACK_SIZ; i++) {
        nv_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(nv_tcb.task_name, "NV  " , sizeof(nv_tcb.task_name));

    /* Create NV task and wait for ack back from it.
    */
    TMC_MSG_MED("Creating NV task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&nv_tcb,
                        (unsigned char *) nv_stack,
                        NV_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) NV_PRI,
                        nv_task,
                        0L,
                        "NV",
                        FALSE,
                        DOG_NV_RPT);
    #else
      rex_def_task(&nv_tcb,
                   (rex_stack_word_type*) nv_stack,
                   NV_STACK_SIZ,
                   (rex_priority_type) NV_PRI,
                   nv_task,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

  #endif /* FEATURE_NV */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef TMC_1X

    #ifdef TIMETEST
      tx_tcb.leds = TIMETEST_TX_TASK_ID;
      rx_tcb.leds = TIMETEST_RX_TASK_ID;
      rxtx_tcb.leds       = TIMETEST_RXTX_TASK_ID;
      srch_tcb.leds = TIMETEST_SRCH_TASK_ID;
      mc_tcb.leds = TIMETEST_MC_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < TX_STACK_SIZ; i++) {
        tx_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
      for (i = 0; i < RX_STACK_SIZ; i++) {
        rx_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
      for (i = 0; i < RXTX_STACK_SIZ; i++) {
        rxtx_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
      for (i = 0; i < SRCH_STACK_SIZ; i++) {
        srch_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
      for (i = 0; i < MC_STACK_SIZ; i++) {
        mc_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(tx_tcb.task_name, "TX  " , sizeof(tx_tcb.task_name));
    (void) std_strlcpy(rx_tcb.task_name, "RX  " , sizeof(rx_tcb.task_name));
    (void) std_strlcpy(srch_tcb.task_name, "SRCH" , sizeof(srch_tcb.task_name));
    (void) std_strlcpy(rxtx_tcb.task_name, "RXTX" , sizeof(rxtx_tcb.task_name));
    (void) std_strlcpy(mc_tcb.task_name, "MC  " , sizeof(mc_tcb.task_name));


    /* Create TX task and wait for ack back from it.
    */
    TMC_MSG_MED("Creating TX task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&tx_tcb,
                        (unsigned char *) tx_stack,
                        TX_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) TX_PRI,
                        tx_task,
                        0L,
                        "TX",
                        FALSE,
                        DOG_TX_RPT);
    #else
      rex_def_task(&tx_tcb,
                   (rex_stack_word_type*) tx_stack,
                   TX_STACK_SIZ,
                   (rex_priority_type) TX_PRI,
                   tx_task,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

    /* Create RX task and wait for ack back from it.
    */
    TMC_MSG_MED("Creating RX task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&rx_tcb,
                        (unsigned char *) rx_stack,
                        RX_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) RX_PRI,
                        rx_task ,
                        0L,
                        "RX",
                        FALSE,
                        DOG_RX_RPT);
    #else
      rex_def_task(&rx_tcb,
                   (rex_stack_word_type*) rx_stack,
                   RX_STACK_SIZ,
                   (rex_priority_type) RX_PRI,
                   rx_task ,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

    /* Create SRCH task and wait for ack back from it.
    */
    TMC_MSG_MED("Creating SRCH task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&srch_tcb,
                        (unsigned char *) srch_stack,
                        SRCH_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) SRCH_PRI,
                        srch_task,
                        0L,
                        "SRCH",
                        FALSE,
                        DOG_SRCH_RPT);
    #else
      rex_def_task(&srch_tcb,
                   (rex_stack_word_type*) srch_stack,
                   SRCH_STACK_SIZ,
                   (rex_priority_type) SRCH_PRI,
                   srch_task,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs (rex_self(), TMC_ACK_SIG);

    /* Create RXTX task and wait for ack back from it.
    */
    TMC_MSG_MED("Creating RXTX task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&rxtx_tcb,
                        (unsigned char *) rxtx_stack,
                        RXTX_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) RXTX_PRI,
                        rxtx_task,
                        0L,
                        "RXTX",
                        FALSE,
                        DOG_RXTX_RPT);
    #else
      rex_def_task(&rxtx_tcb,
                   (rex_stack_word_type*) rxtx_stack,
                   RXTX_STACK_SIZ,
                   (rex_priority_type) RXTX_PRI,
                   rxtx_task,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

    /* Create MC task and wait for ack back from it.
    */
    TMC_MSG_MED("Creating MC task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
    rex_def_task_ext2(&mc_tcb,
                      (unsigned char *) mc_stack,
                      MC_STACK_SIZ * sizeof(rex_stack_word_type),
                      (rex_priority_type) MC_PRI,
                      mc_task,
                      0L,
                      "MC",
                      FALSE,
                      DOG_MC_RPT);
    #else
      rex_def_task(&mc_tcb,
                   (rex_stack_word_type*) mc_stack,
                   MC_STACK_SIZ,
                   (rex_priority_type) MC_PRI,
                   mc_task,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

  #endif /* TMC_1X */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_CM

    #ifdef TIMETEST
      cm_tcb.leds = TIMETEST_CM_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < CM_STACK_SIZ; i++) {
        cm_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(cm_tcb.task_name, "CM  " , sizeof(cm_tcb.task_name));

    /* Create CM task and wait for ack back from it.
    */
    TMC_MSG_MED("Creating CM task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&cm_tcb,
                        (unsigned char *) cm_stack,
                        CM_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) CM_PRI,
                        cm_task,
                        0L,
                        "CM",
                        FALSE,
                        DOG_CM_RPT);
    #else
      rex_def_task(&cm_tcb,
                   (rex_stack_word_type*) cm_stack,
                   CM_STACK_SIZ,
                   (rex_priority_type) CM_PRI,
                   cm_task,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

  #endif /* FEATURE_CM */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_MULTIMODE_ARCH

    #ifdef TIMETEST
      mmoc_tcb.leds = TIMETEST_MMOC_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < MMOC_STACK_SIZ; i++) {
        mmoc_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(mmoc_tcb.task_name, "MMOC" , sizeof(mmoc_tcb.task_name));

    /* Create MMOC task and wait for ack back from it.
    */
    TMC_MSG_MED("Creating MMOC task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&mmoc_tcb,
                        (unsigned char *)mmoc_stack,
                        MMOC_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) MMOC_PRI,
                        mmoc_task,
                        0L,
                        "MMOC",
                        FALSE,
                        DOG_MMOC_RPT);
    #else
      rex_def_task(&mmoc_tcb,
                   (rex_stack_word_type*) mmoc_stack,
                   MMOC_STACK_SIZ,
                   (rex_priority_type) MMOC_PRI,
                   mmoc_task,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

  #endif /* FEATURE_MULTIMODE_ARCH */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_HDR
#error code not present
  #endif /* FEATURE_HDR */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_HDR_BCMCS
#error code not present
  #endif /* FEATURE_HDR_BCMCS */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/




  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_VBATT_TASK
    #ifdef TIMETEST
      vbatt_tcb.leds = TIMETEST_VBATT_TASK_ID;
    #endif

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < VBATT_STACK_SIZ; i++) {
        vbatt_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) strcpy( vbatt_tcb.task_name, "VBATT" );
    /* Create VBATT task
    */
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&vbatt_tcb,
                        (unsigned char *) vbatt_stack,
                        VBATT_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) VBATT_PRI,
                        vbatt_task,
                        0L,
                        "VBATT",
                        FALSE,
                        DOG_VBATT_RPT );
    #else
      rex_def_task(&vbatt_tcb,
                   ( rex_stack_word_type* ) vbatt_stack,
                   VBATT_STACK_SIZ,
                   (rex_priority_type) VBATT_PRI,
                   vbatt_task,
                   0L );
    #endif
    (void) tmc_wait( TMC_ACK_SIG );
    (void) rex_clr_sigs( rex_self(), TMC_ACK_SIG );

  #endif /* FEATURE_VBATT_TASK */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/




  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_BT

    #ifdef TIMETEST
      bt_tcb.leds        = TIMETEST_BT_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < BT_STACK_SIZ; i++) {
        bt_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(bt_tcb.task_name, "BT  " , sizeof(bt_tcb.task_name));

    /* Create Bluetooth task and wait for ack.
    */
    TMC_MSG_MED("Creating Bluetooth task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&bt_tcb,
                        (unsigned char *)bt_stack,
                        BT_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) BT_PRI,
                        bt_task,
                        0L,
                        "BT",
                        FALSE,
                        DOG_BT_RPT);
    #else
      rex_def_task(&bt_tcb,
                   (rex_stack_word_type*) bt_stack,
                   BT_STACK_SIZ,
                   (rex_priority_type) BT_PRI,
                   bt_task,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);


    #ifdef FEATURE_BT_EXTPF

      #ifdef TIMETEST
        bt_pf_tcb.leds = TIMETEST_BT_EXTPF_TASK_ID;
      #endif /* TIMETEST */

      #ifdef FEATURE_TMC_STACK_PROFILING
        for (i = 0; i < BT_PF_STACK_SIZ; i++) {
          bt_pf_stack[i] = TMC_STACK_PROFILING_DWORD;
        }
      #endif

      (void) std_strlcpy(bt_pf_tcb.task_name, "BTPF" , sizeof(bt_pf_tcb.task_name));

      /* Create Bluetooth external profile task and wait for ack.
      */
      TMC_MSG_MED("Creating Bluetooth external profile task", 0, 0, 0);
      #ifdef FEATURE_ENHANCED_REX_TASK
        rex_def_task_ext2(&bt_pf_tcb,
                          (unsigned char *)bt_pf_stack,
                          BT_PF_STACK_SIZ * sizeof(rex_stack_word_type),
                          (rex_priority_type) BT_PF_PRI,
                          bt_pf_task,
                          0L,
                          "BTPF",
                          FALSE,
                          DOG_BT_PF_RPT);
      #else
        rex_def_task(&bt_pf_tcb,
                     (rex_stack_word_type*) bt_pf_stack,
                     BT_PF_STACK_SIZ,
                     (rex_priority_type) BT_PF_PRI,
                     bt_pf_task,
                     0L);
      #endif
      (void) tmc_wait(TMC_ACK_SIG);
      (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

    #endif /* FEATURE_BT_EXTPF */

  #endif /* FEATURE_BT */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_UI

  #ifdef FEATURE_IG_UI_CLIENT_SELECT
  if( !hw_ig_ui_client_is_wombat() )
    /* Wombat (Linux) is running the keypad and display */
  #endif /* FEATURE_IG_UI_CLIENT_SELECT */
  {

    #ifdef TIMETEST
      ui_tcb.leds         = TIMETEST_UI_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < UI_STACK_SIZ; i++) {
        ui_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(ui_tcb.task_name, "UI  " , sizeof(ui_tcb.task_name));

    /* Create UI task and wait for ack.
    */
    TMC_MSG_MED("Creating UI task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&ui_tcb,
                        (unsigned char *)ui_stack,
                        UI_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) UI_PRI,
                        ui_task,
                        0L,
                        "UI",
                        FALSE,
                        DOG_UI_RPT);
    #else
      rex_def_task(&ui_tcb,
                   (rex_stack_word_type*) ui_stack,
                   UI_STACK_SIZ,
                   (rex_priority_type) UI_PRI,
                   ui_task,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

  }/* FEATURE_IG_UI_CLIENT_SELECT */

  #endif /* FEATURE_UI */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_NV_CNV

    #ifdef TIMETEST
      cnv_tcb.leds         = TIMETEST_CNV_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < CNV_STACK_SIZ; i++) {
        cnv_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(cnv_tcb.task_name, "CNV " , sizeof(cnv_tcb.task_name));

    /* Create CNV task and wait for ack.*/

    TMC_MSG_MED("Creating CNV task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&cnv_tcb,
                        (unsigned char *)cnv_stack,
                        CNV_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) CNV_PRI,
                        cnv_task,
                        0L,
                        "CNV",
                        FALSE,
                        DOG_CNV_RPT);
    #else
      rex_def_task(&cnv_tcb,
                   (rex_stack_word_type*) cnv_stack,
                   CNV_STACK_SIZ,
                   (rex_priority_type) CNV_PRI,
                   cnv_task,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_NV_CNV */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_DMTASK

    #ifdef TIMETEST
      dmtask_tcb.leds         = TIMETEST_DMTASK_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < DMTASK_STACK_SIZ; i++) {
        dmtask_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(dmtask_tcb.task_name, "DM Task " , sizeof(dmtask_tcb.task_name));

    /* Create DM task and wait for ack.*/

    TMC_MSG_MED("Creating DM task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&dmtask_tcb,
                        (unsigned char *)dmtask_stack,
                        DMTASK_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) DMTASK_PRI,
                        dm_task,
                        0L,
                        "DM Task",
                        FALSE,
                        DOG_DMTASK_RPT);
    #else
      rex_def_task(&dm_tcb,
                   (rex_stack_word_type*) dm_stack,
                   DMTASK_STACK_SIZ,
                   (rex_priority_type) DMTASK_PRI,
                   dm_task,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_DMTASK */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #if (defined(FEATURE_GPSONE) && !defined(FEATURE_CGPS))
#error code not present
  #endif /* FEATURE_GPSONE && !FEATURE_CGPS */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_DIAG_TASK

    #ifdef TIMETEST
      diag_tcb.leds = TIMETEST_DIAG_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < DIAG_STACK_SIZ; i++) {
        diag_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(diag_tcb.task_name, "DIAG" , sizeof(diag_tcb.task_name));

    /* Create DIAG task and wait for ack
    */
    TMC_MSG_MED("Creating DIAG task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&diag_tcb,
                        (unsigned char *)diag_stack,
                        DIAG_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) DIAG_PRI,
                        diag_task,
                        0L,
                        "DIAG",
                        FALSE,
                        DOG_DIAG_RPT);
    #else
      rex_def_task(&diag_tcb,
                   (rex_stack_word_type*) diag_stack,
                   DIAG_STACK_SIZ,
                   (rex_priority_type) DIAG_PRI,
                   diag_task,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

    #ifdef FEATURE_GSM
#error code not present
    #endif /* FEATURE_GSM */

  #endif /* FEATURE_DIAG_TASK */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_IG_QDIAG

    #ifdef TIMETEST
      qdiag_tcb.leds = TIMETEST_QDIAG_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for( i = 0; i < QDIAG_STACK_SIZ; i++ )
      {
        qdiag_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(qdiag_tcb.task_name, "QDIAG" , sizeof(qdiag_tcb.task_name));

    /* Create QDIAG task and wait for ack
    */
    TMC_MSG_MED("Creating QDIAG task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&qdiag_tcb,
                       (unsigned char *)qdiag_stack,
                       QDIAG_STACK_SIZ * sizeof(rex_stack_word_type),
                       (rex_priority_type) QDIAG_PRI,
                       qdiag_task,
                       0L,
                       "QDIAG",
                       FALSE,
                       DOG_QDIAG_RPT);
    #else
      rex_def_task(&qdiag_tcb,
                   (rex_stack_word_type*) qdiag_stack,
                   QDIAG_STACK_SIZ,
                   (rex_priority_type) QDIAG_PRI,
                   qdiag_task,
                   0L);
    #endif

    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

  #endif /* FEATURE_IG_QDIAG */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_IPC_TEST
#error code not present
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifdef FEATURE_SEC_PROCESS_INIT

    #ifdef TIMETEST
      ix_contentsvc_tcb.leds = 0;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < IXCONTENTSVC_STACK_SIZ; i++) {
        ix_contentsvc_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(ix_contentsvc_tcb.task_name, "IXCONTENT " , sizeof(ix_contentsvc_tcb.task_name));

    /* Create IxCONTENT Server task and wait for ack back from it.
    */
    TMC_MSG_MED("Creating IxContent Server task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&ix_contentsvc_tcb,
                        (unsigned char *) ix_contentsvc_stack,
                        IXCONTENTSVC_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) IXCONTENTSVC_PRI,
                        IxContentMain,
                        0L,
                        "IXCONTENT",
                        FALSE,
                        0);
    #else
      rex_def_task(&ix_contentsvc_tcb,
                  (rex_stack_word_type*) ix_contentsvc_stack,
                  IXCONTENTSVC_STACK_SIZ,
                  (rex_priority_type) IXCONTENTSVC_PRI,
                  IxContentMain,
                  0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

  #else

  #ifdef FEATURE_IXHTTPSVC

    #ifdef TIMETEST
      ix_httpsvc_tcb.leds        = TIMETEST_IXHTTP_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < IXHTTPSVC_STACK_SIZ; i++) {
        ix_httpsvc_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(ix_httpsvc_tcb.task_name, "IXHTTP" , sizeof(ix_httpsvc_tcb.task_name));

    /* Create IxHTTP Server task and wait for ack back from it.
    */
    TMC_MSG_MED("Creating IxHTTP task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
    rex_def_task_ext2(&ix_httpsvc_tcb,
                      (unsigned char *)ix_httpsvc_stack,
                      IXHTTPSVC_STACK_SIZ * sizeof(rex_stack_word_type),
                      (rex_priority_type) IXHTTPSVC_PRI,
                      IxHTTPSvcMainFunc,
                      0L,
                      "IxHTTP",
                      FALSE,
                      DOG_IXHTTPSVC_RPT);
    #else
      rex_def_task(&ix_httpsvc_tcb,
                   (rex_stack_word_type*) ix_httpsvc_stack,
                   IXHTTPSVC_STACK_SIZ,
                   (rex_priority_type) IXHTTPSVC_PRI,
                   IxHTTPSvcMainFunc,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_IXHTTPSVC */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #ifdef FEATURE_IXTIMESVC

    #ifdef TIMETEST
      ix_timesvc_tcb.leds = 0;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < IXTIMESVC_STACK_SIZ; i++) {
        ix_timesvc_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(ix_timesvc_tcb.task_name, "IXTIME " , sizeof(ix_timesvc_tcb.task_name));

    /* Create IxTIME Server task and wait for ack back from it.
    */
    TMC_MSG_MED("Creating IxTIME Server task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&ix_timesvc_tcb,
                        (unsigned char *) ix_timesvc_stack,
                        IXTIMESVC_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) IXTIMESVC_PRI,
                        IxTimeOfDay_IPC_Server,
                        0L,
                        "IXTIME",
                        FALSE,
                        -1);
    #else
      rex_def_task(&ix_timesvc_tcb,
                   (rex_stack_word_type*) ix_timesvc_stack,
                   IXTIMESVC_STACK_SIZ,
                   (rex_priority_type) IXTIMESVC_PRI,
                   IxTimeOfDay_IPC_Server,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

  #endif /* FEATURE_IXTIMESVC */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_IXCONTENTSVC

    #ifdef TIMETEST
      ix_contentclnt_tcb.leds = 0;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < IXCONTENTCLNT_STACK_SIZ; i++) {
        ix_contentclnt_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(ix_contentclnt_tcb.task_name, "IXCONTENTCLNT" , sizeof(ix_contentclnt_tcb.task_name));

    /* Create IxContent handler task and wait for ack back from it.
    */
    TMC_MSG_MED("Creating IxContent Clnt Handler task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&ix_contentclnt_tcb,
                        (unsigned char *) ix_contentclnt_stack,
                        IXCONTENTCLNT_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) IXCONTENTCLNT_PRI,
                        IxContentClientHandler_IPC_Server,
                        0L,
                        "IXCONTENTCLNT",
                        FALSE,
                        -1);
    #else
      rex_def_task(&ix_contentclnt_tcb,
                   (rex_stack_word_type*) ix_contentclnt_stack,
                   IXCONTENTCLNT_STACK_SIZ,
                   (rex_priority_type) IXCONTENTCLNT_PRI,
                   IxContentClientHandler_IPC_Server,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

    #ifdef TIMETEST
      ix_contentsvc_tcb.leds = 0;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < IXCONTENTSVC_STACK_SIZ; i++) {
        ix_contentsvc_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(ix_contentsvc_tcb.task_name, "IXCONTENT " , sizeof(ix_contentsvc_tcb.task_name));

    /* Create IxCONTENT Server task and wait for ack back from it.
    */
    TMC_MSG_MED("Creating IxContent Server task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&ix_contentsvc_tcb,
                        (unsigned char *) ix_contentsvc_stack,
                        IXCONTENTSVC_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) IXCONTENTSVC_PRI,
                        IxContent_IPC_Server,
                        0L,
                        "IXCONTENT",
                        FALSE,
                        0);
    #else
      rex_def_task(&ix_contentsvc_tcb,
                   (rex_stack_word_type*) ix_contentsvc_stack,
                   IXCONTENTSVC_STACK_SIZ,
                   (rex_priority_type) IXCONTENTSVC_PRI,
                   IxContent_IPC_Server,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

    #endif /* FEATURE_IXCONTENTSVC */
  #endif /* FEATURE_SEC_PROCESS_INIT */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_IXDESCRAMBLERSVC

    #ifdef TIMETEST
      ix_descramblersvc_tcb.leds = 0;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < IXDESCRAMBLERSVC_STACK_SIZ; i++) {
        ix_descramblersvc_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(ix_descramblersvc_tcb.task_name, "IXSCRMBLSVR" , sizeof(ix_descramblersvc_tcb.task_name));

    /* Create IxDescrambler Server task and wait for ack back from it.
    */
    TMC_MSG_MED("Creating IxDescrambler Server task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&ix_descramblersvc_tcb,
                        (unsigned char *) ix_descramblersvc_stack,
                        IXDESCRAMBLERSVC_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) IXDESCRAMBLERSVC_PRI,
                        IxDescrambler_IPC_Server,
                        0L,
                        "IXSCRMBLSVR",
                        FALSE,
                        0);
    #else
      rex_def_task(&ix_descramblersvc_tcb,
                   (rex_stack_word_type*) ix_descramblersvc_stack,
                   IXDESCRAMBLERSVC_STACK_SIZ,
                   (rex_priority_type) IXDESCRAMBLERSVC_PRI,
                   IxDescrambler_IPC_Server,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

  #endif /* FEATURE_IXDESCRAMBLERSVC */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_BRINGUP_DIAG_ONLY

  #ifdef FEATURE_SEC

    #ifndef FEATURE_SEC_PROCESS_INIT

    #ifdef TIMETEST
      sec_tcb.leds        = TIMETEST_SEC_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < SEC_STACK_SIZ; i++) {
        sec_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(sec_tcb.task_name, "SEC " , sizeof(sec_tcb.task_name));

    /* Create Security services task and wait for ack back from it.
    */
    TMC_MSG_MED("Creating SEC task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
    rex_def_task_ext2(&sec_tcb,
                      (unsigned char *)sec_stack,
                      SEC_STACK_SIZ * sizeof(rex_stack_word_type),
                      (rex_priority_type) SEC_PRI,
                      sec_task,
                      0L,
                      "SEC",
                      FALSE,
                      DOG_SEC_RPT);
    #else
      rex_def_task(&sec_tcb,
                   (rex_stack_word_type*) sec_stack,
                   SEC_STACK_SIZ,
                   (rex_priority_type) SEC_PRI,
                   sec_task,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

    #ifdef FEATURE_SEC_CONTENT_AGENT
    #ifdef TIMETEST
      contentagentdiag_tcb.leds = TIMETEST_CONTENTAGENTDIAG_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < CONTENTAGENTDIAG_STACK_SIZ; i++) {
        contentagentdiag_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(contentagentdiag_tcb.task_name, "CONTENTDIAG " , sizeof(contentagentdiag_tcb.task_name));

    /* Create Security services task and wait for ack back from it.
    */
    TMC_MSG_MED("Creating CONTENTAGENTDIAG task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&contentagentdiag_tcb,
                        (unsigned char *)contentagentdiag_stack,
                        CONTENTAGENTDIAG_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) CONTENTAGENTDIAG_PRI,
                        SECDiagServer,
                        0L,
                        "CONTENTDIAG",
                        FALSE,
                        -1);
    #else
      rex_def_task(&contentagentdiag_tcb,
                   (rex_stack_word_type*) contentagentdiag_stack,
                   CONTENTAGENTDIAG_STACK_SIZ,
                   (rex_priority_type) CONTENTAGENTDIAG_PRI,
                   contentagentdiag_task,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
    #endif /* FEATURE_SEC_CONTENT_AGENT */

    #endif /* !FEATURE_SEC_PROCESS_INIT */

    #ifdef FEATURE_SECRND
    #ifdef TIMETEST
      secrnd_tcb.leds        = TIMETEST_SECRND_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < SEC_STACK_SIZ; i++) {
        secrnd_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(secrnd_tcb.task_name, "SECRND " , sizeof(secrnd_tcb.task_name));

    /* Create Security services RND task and wait for ack back from it.
    */
    TMC_MSG_MED("Creating SECRND task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
    rex_def_task_ext2(&secrnd_tcb,
                      (unsigned char *)secrnd_stack,
                      SECRND_STACK_SIZ * sizeof(rex_stack_word_type),
                      (rex_priority_type) SECRND_PRI,
                      secrnd_task,
                      0L,
                      "SECRND",
                      FALSE,
                      DOG_SECRND_RPT);
    #else
      rex_def_task(&secrnd_tcb,
                   (rex_stack_word_type*) secrnd_stack,
                   SECRND_STACK_SIZ,
                   (rex_priority_type) SECRND_PRI,
                   secrnd_task,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
    #endif /* FEATURE_SECRND */

    #ifndef FEATURE_SEC_PROCESS_INIT

    #ifdef FEATURE_SEC_ARM_CRYPTO

    #ifdef TIMETEST
    seccryptarm_tcb.leds        = TIMETEST_SECCRYPTARM_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < SECCRYPTARM_STACK_SIZ; i++) {
        seccryptarm_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(seccryptarm_tcb.task_name, "SECCRYPTARM" , sizeof(seccryptarm_tcb.task_name));

    /* Create Security services ARM task and wait for ack back from it.
    */
    TMC_MSG_MED("Creating SECCRYPTARM task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&seccryptarm_tcb,
                        (unsigned char *)seccryptarm_stack,
                        SECCRYPTARM_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) SECCRYPTARM_PRI,
                        seccryptarm_task,
                        0L,
                        "SECCRYPTARM",
                        FALSE,
                        DOG_SECCRYPTARM_RPT);
    #else
      rex_def_task(&seccryptarm_tcb,
                   (rex_stack_word_type*) seccryptarm_stack,
                   SECCRYPTARM_STACK_SIZ,
                   (rex_priority_type) SECCRYPTARM_PRI,
                   seccryptarm_task,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
    #endif /* FEATURE_SEC_ARM_CRYPTO */

    #endif /* FEATURE_SEC_PROCESS_INIT */

    #ifdef FEATURE_SEC_USFS
    #ifdef TIMETEST
      sfs_tcb.leds        = TIMETEST_SFS_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < SEC_STACK_SIZ; i++) {
        sfs_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(sfs_tcb.task_name, "SFSsvc" , sizeof(sfs_tcb.task_name));

    /* Create Security services RND task and wait for ack back from it.
    */
    TMC_MSG_MED("Creating SFS svc task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
    rex_def_task_ext2(&sfs_tcb,
                      (unsigned char *)sfs_stack,
                      SFS_STACK_SIZ * sizeof(rex_stack_word_type),
                      (rex_priority_type) SFS_PRI,
                      sfs_task,
                      0L,
                      "SFSsvc",
                      FALSE,
                      DOG_SFS_RPT);
    #else
      rex_def_task(&sfs_tcb,
                   (rex_stack_word_type*) sfs_stack,
                   SFS_STACK_SIZ,
                   (rex_priority_type) SFS_PRI,
                   sfs_task,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
    #endif /* FEATURE_SEC_USFS */

    #ifdef FEATURE_SEC_IPSEC

    #ifdef TIMETEST
    secips_tcb.leds        = TIMETEST_SECIPSEC_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < SECIPS_STACK_SIZ; i++) {
        secips_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(secips_tcb.task_name, "IPSEC" , sizeof(secips_tcb.task_name));

    /* Create IPSec IKE task and wait for ack back from it.
    */
    TMC_MSG_MED("Creating IPSEC task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&secips_tcb,
                        (unsigned char *)secips_stack,
                        SECIPS_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) SECIPS_PRI,
                        secips_task,
                        0L,
                        "SEC IPSEC",
                        FALSE,
                        DOG_SECIPS_RPT);
    #else
      rex_def_task(&secips_tcb,
                   (rex_stack_word_type*) secips_stack,
                   SECIPS_STACK_SIZ,
                   (rex_priority_type) SECIPS_PRI,
                   secips_task,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

    #endif /* FEATURE_SEC_IPSEC */

  #ifdef FEATURE_SEC_SSL

    #ifdef TIMETEST
      secssl_tcb.leds        = TIMETEST_SECSSL_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < SECSSL_STACK_SIZ; i++) {
        secssl_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(secssl_tcb.task_name, "SECSSL " , sizeof(secssl_tcb.task_name));

    /* Create Security services task and wait for ack back from it.
    */
    TMC_MSG_MED("Creating SSL task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&secssl_tcb,
                        (unsigned char *)secssl_stack,
                        SECSSL_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) SECSSL_PRI,
                        secssl_task,
                        0L,
                        "SECSSL",
                        FALSE,
                        DOG_SECSSL_RPT);
    #else
      rex_def_task(&secssl_tcb,
                   (rex_stack_word_type*) secssl_stack,
                   SECSSL_STACK_SIZ,
                   (rex_priority_type) SECSSL_PRI,
                   secssl_task,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

  #endif /* FEATURE_SEC_SSL */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifndef T_RUMI
  #ifdef FEATURE_SEC_DRM
#error code not present
  #endif /* FEATURE_SEC_DRM */
#endif /* ! T_RUMI */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if defined(FEATURE_SEC_SSL_TESTAPP) || defined(FEATURE_SEC_CRYPT_TESTAPP)
#error code not present
  #endif /* FEATURE_SEC_SSL_TESTAPP || FEATURE_SEC_CRYPT_TESTAPP */

  #endif /* FEATURE_SEC */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_DS_SOCKETS_TEST_TCP
#error code not present
  #endif /* FEATURE_DS_SOCKETS_TEST_TCP */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_DS_SOCKETS_TEST_UDP
#error code not present
  #endif /* FEATURE_DS_SOCKETS_TEST_UDP */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_QMIP
#error code not present
  #endif /* FEATURE_QMIP */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #if defined (FEATURE_AUTH) || defined (FEATURE_RUIM)

    #ifdef TIMETEST
      auth_tcb.leds       = TIMETEST_AUTH_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < AUTH_STACK_SIZ; i++) {
        auth_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(auth_tcb.task_name, "AUTH" , sizeof(auth_tcb.task_name));

    /* Create Authentication task and wait for ack back from it.
    */
    TMC_MSG_MED("Creating AUTH task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&auth_tcb,
                        (unsigned char *)auth_stack,
                        AUTH_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) AUTH_PRI,
                        auth_task,
                        0L,
                        "AUTH",
                        FALSE,
                        DOG_AUTH_RPT);
    #else
      rex_def_task(&auth_tcb,
                   (rex_stack_word_type*) auth_stack,
                   AUTH_STACK_SIZ,
                   (rex_priority_type) AUTH_PRI,
                   auth_task,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

  #endif /*FEATURE_AUTH || FEATURE_RUIM */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #if (defined ( FEATURE_UIM ) && ! defined (FEATURE_GSM_PLT) )

    #ifdef TIMETEST
      uim_tcb.leds = TIMETEST_UIM_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < UIM_STACK_SIZ; i++) {
        uim_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(uim_tcb.task_name, "UIM " , sizeof(uim_tcb.task_name));

    /* Create UIM task and wait for ack back from it.
    */
    TMC_MSG_MED("Creating UIM task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&uim_tcb,
                        (unsigned char *)uim_stack,
                        UIM_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) UIM_PRI,
                        uim_task,
                        0L,
                        "UIM",
                        FALSE,
                        DOG_UIM_RPT);
    #else
      rex_def_task(&uim_tcb,
                   (rex_stack_word_type*) uim_stack,
                   UIM_STACK_SIZ,
                   (rex_priority_type) UIM_PRI,
                   uim_task,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

  #endif /* FEATURE_UIM */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_MMGSDI

    #ifdef TIMETEST
      gsdi_task_tcb.leds = TIMETEST_GSDI_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < GSDI_STACK_SIZ; i++) {
        gsdi_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(gsdi_task_tcb.task_name, "GSDI" , sizeof(gsdi_task_tcb.task_name));

    /* Create GSDI task and wait for ack back from it.
    */
    TMC_MSG_MED("Creatign GSDI task.", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&gsdi_task_tcb,
                        (unsigned char *)gsdi_stack,
                        GSDI_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) GSDI_PRI,
                        gsdimain,
                        0L,
                        "GSDI",
                        FALSE,
                        DOG_GSDI_RPT);
    #else
      rex_def_task(&gsdi_task_tcb,
                   (rex_stack_word_type*) gsdi_stack,
                   GSDI_STACK_SIZ,
                   (rex_priority_type) GSDI_PRI,
                   gsdimain,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

  #endif /* FEATURE_MMGSDI */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #if defined (FEATURE_GSTK) && defined(FEATURE_VIRTUAL_SAT)

    #ifdef TIMETEST
      virtsat_tcb.leds = TIMETEST_VIRTSAT_TASK_ID;
    #endif

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < VIRTSAT_STACK_SIZ; i++) {
        virtsat_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(virtsat_tcb.task_name,"VIRT" , sizeof(virtsat_tcb.task_name));

    /* Create Test Driver task */
    TMC_MSG_MED( "Creating Virtual SAT APP task thread", 0, 0, 0 );
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&virtsat_tcb,
                        (unsigned char *)virtsat_stack,
                        VIRTSAT_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) VIRTSAT_PRI,
                        virtsat_main,
                        0L,
                        "VIRT SAT",
                        FALSE,
                        DOG_VIRTSAT_RPT);
    #else
      rex_def_task( &virtsat_tcb,
                    (rex_stack_word_type*) virtsat_stack,
                    VIRTSAT_STACK_SIZ,
                    (rex_priority_type) VIRTSAT_PRI,
                    virtsat_main,
                    0L);
    #endif
    /* Wait for an Ack from the Virtual SAT task before going further.
    */
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* #if defined (FEATURE_GSTK) && defined(FEATURE_VIRTUAL_SAT) */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_GSTK

    #ifdef TIMETEST
      gstk_tcb.leds = TIMETEST_GSTK_TASK_ID;
    #endif

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < GSTK_STACK_SIZ; i++) {
        gstk_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(gstk_tcb.task_name,"GSTK" , sizeof(gstk_tcb.task_name));

    /* Create GSTK task */
    TMC_MSG_MED( "Creating GSTK task thread", 0, 0, 0 );
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&gstk_tcb,
                        (unsigned char *)gstk_stack,
                        GSTK_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) GSTK_PRI,
                        gstk_task,
                        0L,
                        "GSTK",
                        FALSE,
                        DOG_GSTK_RPT);
    #else
      rex_def_task( &gstk_tcb,
                    (rex_stack_word_type*) gstk_stack,
                    GSTK_STACK_SIZ,
                    (rex_priority_type) GSTK_PRI,
                    gstk_task,
                    0L);
    #endif
    /* Wait for an Ack from GSTK task before going further.
    */
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_GSTK */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_DH

    #ifdef TIMETEST
      dh_tcb.leds = TIMETEST_DH_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < DH_STACK_SIZ; i++) {
        dh_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(dh_tcb.task_name, "DH  " , sizeof(dh_tcb.task_name));

    /* Create DH task and wait for ack back from it.
    */
    TMC_MSG_MED("Creating DH task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&dh_tcb,
                        (unsigned char *)dh_stack,
                        DH_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) DH_PRI,
                        dh_task,
                        0L,
                        "DH",
                        FALSE,
                        DOG_DH_RPT);
    #else
      rex_def_task(&dh_tcb,
                   (rex_stack_word_type*) dh_stack,
                   DH_STACK_SIZ,
                   (rex_priority_type) DH_PRI,
                   dh_task,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

  #endif /* FEATURE_DH */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_GRAPH_TASK

    #ifdef TIMETEST
      graph_tcb.leds = TIMETEST_GRAPH_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < GRAPH_STACK_SIZ; i++) {
        graph_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(graph_tcb.task_name, "GRPH" , sizeof(graph_tcb.task_name));

    /* Create Graphics task and wait for ack back from it.
    */
    TMC_MSG_MED("Creating Graphics task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&graph_tcb,
                        (unsigned char *)graph_stack,
                        GRAPH_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) GRAPH_PRI,
                        graph_task,
                        0L,
                        "GRAPH",
                        FALSE,
                        DOG_GRAPH_RPT);
    #else
      rex_def_task(&graph_tcb,
                   (rex_stack_word_type*) graph_stack,
                   GRAPH_STACK_SIZ,
                   (rex_priority_type) GRAPH_PRI,
                   graph_task,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG) ;
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

  #endif /* FEATURE_GRAPH_TASK */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_CAMERA_DRV_TASK

    #ifdef TIMETEST
      camera_drv_tcb.leds = TIMETEST_CAMERA_DRV_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < CAMERA_DRV_STACK_SIZ; i++) {
        camera_drv_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(camera_drv_tcb.task_name, "CAMD" , sizeof(camera_drv_tcb.task_name));

    /* Create Camera Driver task and wait for ack back from it.
    */
    TMC_MSG_MED("Creating Camera Driver task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&camera_drv_tcb,
                        (unsigned char *)camera_drv_stack,
                        CAMERA_DRV_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) CAMERA_DRV_PRI,
                        camera_drv_task,
                        0L,
                        "CAMERA_DRV",
                        FALSE,
                        DOG_CAMERA_DRV_RPT);
    #else
      rex_def_task(&camera_drv_tcb,
                   (rex_stack_word_type*) camera_drv_stack,
                   CAMERA_DRV_STACK_SIZ,
                   (rex_priority_type) CAMERA_DRV_PRI,
                   camera_drv_task,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG) ;
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

  #endif /* FEATURE_CAMERA_DRV_TASK */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_VS_TASK

    #ifdef TIMETEST
      vs_tcb.leds = TIMETEST_VS_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < VS_STACK_SIZ; i++) {
        vs_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(vs_tcb.task_name, "VS  " , sizeof(vs_tcb.task_name));

    /* Create VS task and wait for ack back from it.
    */
    TMC_MSG_MED("Creating VS task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&vs_tcb,
                        (unsigned char *)vs_stack,
                        VS_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) VS_PRI,
                        vs_task,
                        0L,
                        "VS",
                        FALSE,
                        DOG_VS_RPT);
    #else
      rex_def_task(&vs_tcb,
                   (rex_stack_word_type*) vs_stack,
                   VS_STACK_SIZ,
                   (rex_priority_type) VS_PRI,
                   vs_task,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG) ;
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

  #endif /* FEATURE_VS_TASK */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if defined (FEATURE_USB) && !defined (FEATURE_NO_USB_HW_ON_PROCESSOR)

    #ifdef TIMETEST
      usbdc_tcb.leds = TIMETEST_USBDC_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < USBDC_STACK_SIZ; i++) {
        usbdc_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(usbdc_tcb.task_name, "USBSDC" , sizeof(usbdc_tcb.task_name));

    /* Create USBDC task and wait for ack back from it.
    */
    TMC_MSG_MED("Creating USBDC task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&usbdc_tcb,
                        (unsigned char *)usbdc_stack,
                        USBDC_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) USBDC_PRI,
                        usbdc_task,
                        0L,
                        "USBDC",
                        FALSE,
                        DOG_USBDC_RPT);
    #else
      rex_def_task(&usbdc_tcb,
                   (rex_stack_word_type*) usbdc_stack,
                   USBDC_STACK_SIZ,
                   (rex_priority_type) USBDC_PRI,
                   usbdc_task,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

    #ifdef FEATURE_USB_HOST_ENHANCED

      #ifdef FEATURE_TMC_STACK_PROFILING
        for (i = 0; i < USBHC_URB_STACK_SIZ; i++) {
          usbhc_urb_stack[i] = TMC_STACK_PROFILING_DWORD;
        }
      #endif

      (void) std_strlcpy(usbhc_urb_tcb.task_name, "USBHC_URB" , sizeof(usbhc_urb_tcb.task_name));

      /* Create USBDC task and wait for ack back from it.
      */
      TMC_MSG_MED("Creating USBHC_URB task", 0, 0, 0);
      #ifdef FEATURE_ENHANCED_REX_TASK
        rex_def_task_ext2(&usbhc_urb_tcb,
                          (unsigned char *)usbhc_urb_stack,
                          USBHC_URB_STACK_SIZ * sizeof(rex_stack_word_type),
                          (rex_priority_type) USBHC_URB_PRI,
                          usbhc_urb_task,
                          0L,
                          "USBHC_URB",
                          FALSE,
                          DOG_USBHC_URB_RPT);
      #else
        rex_def_task(&usbhc_urb_tcb,
                     (rex_stack_word_type*) usbhc_urb_stack,
                     USBHC_URB_STACK_SIZ,
                     (rex_priority_type) USBHC_URB_PRI,
                     usbhc_urb_task,
                     0L);
      #endif
      (void) tmc_wait(TMC_ACK_SIG);
      (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);


      #ifdef FEATURE_TMC_STACK_PROFILING
        for (i = 0; i < USBHC_DEV_STACK_SIZ; i++) {
          usbhc_dev_stack[i] = TMC_STACK_PROFILING_DWORD;
        }
      #endif

      (void) std_strlcpy(usbhc_dev_tcb.task_name, "USBHC_DEV" , sizeof(usbhc_dev_tcb.task_name));

      /* Create USBDC task and wait for ack back from it.
      */
      TMC_MSG_MED("Creating USBHC_DEV task", 0, 0, 0);
      #ifdef FEATURE_ENHANCED_REX_TASK
        rex_def_task_ext2(&usbhc_dev_tcb,
                          (unsigned char *)usbhc_dev_stack,
                          USBHC_DEV_STACK_SIZ * sizeof(rex_stack_word_type),
                          (rex_priority_type) USBHC_DEV_PRI,
                          usbhc_dev_task,
                          0L,
                          "USBHC_DEV",
                          FALSE,
                          DOG_USBHC_DEV_RPT);
      #else
        rex_def_task(&usbhc_dev_tcb,
                     (rex_stack_word_type*) usbhc_dev_stack,
                     USBHC_DEV_STACK_SIZ,
                     (rex_priority_type) USBHC_DEV_PRI,
                     usbhc_dev_task,
                     0L);
      #endif
      (void) tmc_wait(TMC_ACK_SIG);
      (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

      #if defined(FEATURE_USB_HOST_TEST_ENHANCED) && \
          (defined(FEATURE_USB_HOST_MASS_STORAGE_TEST_ENHANCED) || \
           defined(FEATURE_USB_HOST_HID_TEST_ENHANCED))

      #ifdef FEATURE_TMC_STACK_PROFILING
        for (i = 0; i < USBHC_TEST_STACK_SIZ; i++) {
          usbhc_test_stack[i] = TMC_STACK_PROFILING_DWORD;
        }
      #endif

      (void) std_strlcpy(usbhc_test_tcb.task_name, "USBHC_TEST" , sizeof(usbhc_test_tcb.task_name));

      /* Create USBHC task and wait for ack back from it.
      */
      TMC_MSG_MED("Creating USBHC_TEST task", 0, 0, 0);
      #ifdef FEATURE_ENHANCED_REX_TASK
        rex_def_task_ext2(&usbhc_test_tcb,
                          (unsigned char *)usbhc_test_stack,
                          USBHC_TEST_STACK_SIZ * sizeof(rex_stack_word_type),
                          (rex_priority_type) USBHC_TEST_PRI,
                          usbhc_test_task,
                          0L,
                          "USBHC_TEST",
                          FALSE,
                          DOG_USBHC_TEST_RPT);
      #else
        rex_def_task(&usbhc_test_tcb,
                     (rex_stack_word_type*) usbhc_test_stack,
                     USBHC_TEST_STACK_SIZ,
                     (rex_priority_type) USBHC_TEST_PRI,
                     usbhc_test_task,
                     0L);
      #endif
      (void) tmc_wait(TMC_ACK_SIG);
      (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);


      #endif


    #endif /* FEATURE_USB_HOST_ENHANCED */

  #endif /* FEATURE_USB */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #if (defined(FEATURE_GSM) && !defined(FEATURE_WCDMA_PLT) && !defined(FEATURE_GSM_PLT))
#error code not present
  #endif /* FEATURE_GSM && !FEATURE_WCDMA_PLT */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_NAS
#error code not present
  #endif /* FEATURE_NAS */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #if ( defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS) )

    #ifdef TIMETEST
      wms_tcb.leds = TIMETEST_WMS_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < WMS_STACK_SIZ; i++) {
        wms_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(wms_tcb.task_name, "WMS" , sizeof(wms_tcb.task_name));

    /* Create WMS task and wait for ack back from it.
    */
    TMC_MSG_MED("Creating WMS task.", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&wms_tcb,
                        (unsigned char *)wms_stack,
                        WMS_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) WMS_PRI,
                        wms_task,
                        0L,
                        "WMS",
                        FALSE,
                        DOG_WMS_RPT);
    #else
      rex_def_task(&wms_tcb,
                   (rex_stack_word_type*) wms_stack,
                   WMS_STACK_SIZ,
                   (rex_priority_type) WMS_PRI,
                   wms_task,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

  #endif /* FEATURE_CDSMS || FEATURE_GWSMS */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_MDSP_TASK

    #ifdef TIMETEST
      mdsp_tcb.leds = TIMETEST_MDSP_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < MDSP_STACK_SIZ; i++) {
        mdsp_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(mdsp_tcb.task_name, "MDSP" , sizeof(mdsp_tcb.task_name));

    /* Create MDSP task and wait for ack back from it.
    */
    TMC_MSG_MED("Creating MDSP task.", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&mdsp_tcb,
                        (unsigned char *)mdsp_stack,
                        MDSP_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) MDSP_PRI,
                        mdsp_task,
                        0L,
                        "MDSP",
                        FALSE,
                        DOG_MDSP_RPT);
    #else
      rex_def_task(&mdsp_tcb,
                   (rex_stack_word_type*) mdsp_stack,
                   MDSP_STACK_SIZ,
                   (rex_priority_type) MDSP_PRI,
                   mdsp_task,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

  #endif /* FEATURE_MDSP_TASK */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_VIRTUAL_SIM

    #ifdef TIMETEST
      gsdi_uim_server_task_tcb.leds = TIMETEST_GSDI_UIM_TASK_ID;
    #endif

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < GSDI_UIM_SERVER_STACK_SIZ; i++) {
        gsdi_uim_server_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(gsdi_uim_server_task_tcb.task_name, "GSDIUIMSRV" , sizeof(gsdi_uim_server_task_tcb.task_name));

    /* Create Test Driver task */
    TMC_MSG_MED( "Creating GSDI UIM Server task thread", 0, 0, 0 );
    rex_def_task(&gsdi_uim_server_task_tcb,
                 (rex_stack_word_type*) gsdi_uim_server_stack,
                 GSDI_UIM_SERVER_STACK_SIZ,
                 (rex_priority_type) GSDI_UIM_SERVER_PRI,
                 test_sim_server_task,
                 0L);
    /* Wait for an Ack from L1 task before going further.
     */
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* ifdef FEATURE_VIRTUAL_SIM */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_WCDMA
#error code not present
  #endif /* FEATURE_WCDMA */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #if ( defined(FEATURE_WCDMA_VST_FULLSTACK) || defined(FEATURE_WCDMA_VST_AS))

    #ifdef TIMETEST
      al1_tcb.leds = TIMETEST_AL1_TASK_ID;
      menu_tcb.leds = TIMETEST_MENU_UT_TASK_ID;
    #endif

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < AL1_STACK_SIZ; i++) {
        al1_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
      for (i = 0; i < MENU_STACK_SIZ; i++) {
        menu_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(al1_tcb.task_name, "AL1" , sizeof(al1_tcb.task_name));
    (void) std_strlcpy(menu_tcb.task_name, "MENU" , sizeof(menu_tcb.task_name));

    /* Create AL1 Task */
    TMC_MSG_MED( "Creating AL1 task thread", 0, 0, 0 );
    rex_def_task(&al1_tcb,
                 (rex_stack_word_type*) al1_stack,
                 AL1_STACK_SIZ,
                 (rex_priority_type) AL1_PRI,
                 al1_task,
                 0L);

    /* Wait for an Ack from AL1 task before going further.
     */
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

    /* Create MENU Unit Test Task */
    TMC_MSG_LOW( "Creating MENU Unit Test task thread", 0, 0, 0 );
    rex_def_task(&menu_tcb,
                 (rex_stack_word_type*) menu_stack,
                 MENU_STACK_SIZ,
                 (rex_priority_type) MENU_PRI,
                 menu_task,
                 0L);

    /* Wait for an Ack from RRC task before going further.
     */
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

  #endif /* FEATURE_WCDMA_VST_FULLSTACK || FEATURE_WCDMA_VST_AS */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_TC

    #ifdef TIMETEST
      tc_tcb.leds = TIMETEST_TC_TASK_ID;
    #endif

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < TC_STACK_SIZ; i++) {
        tc_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(tc_tcb.task_name, "TC" , sizeof(tc_tcb.task_name));

    /* Create TC task */
    TMC_MSG_MED( "Creating TC task thread", 0, 0, 0 );
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&tc_tcb,
                        (unsigned char *)tc_stack,
                        TC_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) TC_PRI,
                        tc_task,
                        0L,
                        "TC",
                        FALSE,
                        DOG_TC_RPT);
    #else
      rex_def_task(&tc_tcb,
                   (rex_stack_word_type*) tc_stack,
                   TC_STACK_SIZ,
                   (rex_priority_type) TC_PRI,
                   tc_task,
                   0L);
    #endif
    /* Wait for an Ack from TC task before going further.
     */
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

  #endif /* FEATURE_TC */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_WCDMA_VST_AS
#error code not present
  #endif /*FEATURE_WCDMA_VST_AS*/
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_WCDMA_PLT /* FEATURE_WPLT_TASK */
#error code not present
  #endif /*FEATURE_WCDMA_PLT*/
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_GSM_PLT /* FEATURE_GSM_PLT_TASK */
#error code not present
  #endif /*FEATURE_GSM_PLT*/
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_WLAN
#error code not present
  #endif /* FEATURE_WLAN  */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_DATA

    #ifdef FEATURE_DATA_PS
      #ifdef TIMETEST
        ps_tcb.leds = TIMETEST_PS_TASK_ID;
      #endif /* TIMETEST */

      #ifdef FEATURE_TMC_STACK_PROFILING
        for (i = 0; i < PS_STACK_SIZ; i++) {
          ps_stack[i] = TMC_STACK_PROFILING_DWORD;
        }
      #endif

      (void) std_strlcpy(ps_tcb.task_name, "PS  " , sizeof(ps_tcb.task_name));

      /* Create Protocol Services task and wait for ack back from it.
      */
      TMC_MSG_MED("Creating Protocol Services task", 0, 0, 0);
      #ifdef FEATURE_ENHANCED_REX_TASK
        rex_def_task_ext2(&ps_tcb,
                          (unsigned char *)ps_stack,
                          PS_STACK_SIZ * sizeof(rex_stack_word_type),
                          (rex_priority_type) PS_PRI,
                          ps_task,
                          0L,
                          "PS",
                          FALSE,
                          DOG_PS_RPT);
      #else
        rex_def_task(&ps_tcb,
                     (rex_stack_word_type*) ps_stack,
                     PS_STACK_SIZ,
                     (rex_priority_type) PS_PRI,
                     ps_task,
                     0L);
      #endif
      (void) tmc_wait(TMC_ACK_SIG);
      (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
    #endif /* FEATURE_DATA_PS */

    #ifdef TIMETEST
      ds_tcb.leds = TIMETEST_DS_TASK_ID;
      #ifdef FEATURE_COMP_TASK
        comp_tcb.leds = TIMETEST_COMP_TASK_ID;
      #endif /* FEATURE_COMP_TASK */
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < DS_STACK_SIZ; i++) {
        ds_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
      #ifdef FEATURE_COMP_TASK
        for (i = 0; i < COMP_STACK_SIZ; i++) {
          comp_stack[i] = TMC_STACK_PROFILING_DWORD;
        }
      #endif /* FEATURE_COMP_TASK */
    #endif

    (void) std_strlcpy(ds_tcb.task_name, "DS  " , sizeof(ds_tcb.task_name));
    #ifdef FEATURE_COMP_TASK
      (void) std_strlcpy(comp_tcb.task_name, "COMP  " , sizeof(comp_tcb.task_name));
    #endif /* FEATURE_COMP_TASK */

    /* Create Data Service task and wait for ack back from it.
    */
    TMC_MSG_MED("Creating DATA Service task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&ds_tcb,
                        (unsigned char *)ds_stack,
                        DS_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) DS_PRI,
                        ds_task,
                        0L,
                        "DS",
                        FALSE,
                        DOG_DS_RPT);
    #else
      rex_def_task(&ds_tcb,
                   (rex_stack_word_type*) ds_stack,
                   DS_STACK_SIZ,
                   (rex_priority_type) DS_PRI,
                   ds_task,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

    #ifdef FEATURE_COMP_TASK
    /* Create Data Service task and wait for ack back from it.
    */
    TMC_MSG_MED("Creating COMP task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&comp_tcb,
                        (unsigned char *)comp_stack,
                        COMP_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) COMP_PRI,
                        comp_task,
                        0L,
                        "COMP",
                        FALSE,
                        DOG_COMP_RPT);
    #else
      rex_def_task(&comp_tcb,
                   (rex_stack_word_type*) comp_stack,
                   COMP_STACK_SIZ,
                   (rex_priority_type) COMP_PRI,
                   comp_task,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
    #endif /* FEATURE_COMP_TASK */

    #if ( (defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS)) && !defined(FEATURE_WCDMA_PLT) && !defined(FEATURE_GSM_PLT) )
       #ifdef TIMETEST
         Gcsd_tcb.leds = TIMETEST_GCSD_TASK_ID;
       #endif

      #ifdef FEATURE_TMC_STACK_PROFILING
        for (i = 0; i < DS_GCSD_STACK_SIZ; i++) {
          Gcsd_stack[i] = TMC_STACK_PROFILING_DWORD;
        }
      #endif

       (void) std_strlcpy(Gcsd_tcb.task_name, "GCSD" , sizeof(Gcsd_tcb.task_name));

       /* Create GCSD Task */
       TMC_MSG_LOW( "Creating GCSD task thread", 0, 0, 0 );
       #ifdef FEATURE_ENHANCED_REX_TASK
         rex_def_task_ext2( &Gcsd_tcb,
                            (unsigned char *) Gcsd_stack,
                            DS_GCSD_STACK_SIZ * sizeof(rex_stack_word_type),
                            (rex_priority_type) DS_GCSD_PRI,
                            ds_gcsd_task,
                            0L,
                            "DS GCSD",
                            FALSE,
                            DOG_GCSD_RPT);
       #else
         rex_def_task(&Gcsd_tcb,
                      (rex_stack_word_type*) Gcsd_stack,
                      DS_GCSD_STACK_SIZ,
                      (rex_priority_type) DS_GCSD_PRI,
                      ds_gcsd_task,
                      0L);
       #endif
       (void) tmc_wait(TMC_ACK_SIG);
       (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
    #endif /* (FEATURE_DATA_GCSD || FEATURE_DATA_WCDMA_CS) && !FEATURE_WCDMA_PLT */

    #ifdef FEATURE_WCDMA /* ) && ( defined FEATURE_DATA_WCDMA_CS ) ) */
#error code not present
    #endif /* FEATURE_WCDMA */

  #endif /* FEATURE_DATA */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_WLAN
#error code not present
  #endif /* FEATURE_WLAN */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  #ifdef FEATURE_WLAN
#error code not present
  #endif /* FEATURE_WLAN */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_MMGPS
#error code not present
  #endif /* FEATURE_MMGPS */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_MGP

    #ifdef TIMETEST
      #ifdef FEATURE_MGPCC_TASK
      mgpcc_tcb.leds = TIMETEST_MGPCC_TASK_ID;
      #endif
      mgpmc_tcb.leds = TIMETEST_MGPMC_TASK_ID;
      mgpnf_tcb.leds = TIMETEST_MGPNF_TASK_ID;
      mgpcd_tcb.leds = TIMETEST_MGPCD_TASK_ID;
      mgpai_tcb.leds = TIMETEST_MGPAI_TASK_ID;
    #endif

    #ifdef FEATURE_TMC_STACK_PROFILING
      #ifdef FEATURE_MGPCC_TASK
      for (i = 0; i < MGPCC_STACK_SIZ; i++) {
        mgpcc_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
      #endif
      for (i = 0; i < MGPMC_STACK_SIZ; i++) {
        mgpmc_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
      for (i = 0; i < MGPNF_STACK_SIZ; i++) {
        mgpnf_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
      for (i = 0; i < MGPCD_STACK_SIZ; i++) {
        mgpcd_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
      for (i = 0; i < MGPAI_STACK_SIZ; i++) {
        mgpai_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    #ifdef FEATURE_MGPCC_TASK
    (void) std_strlcpy( mgpcc_tcb.task_name, "MGPCC"  , sizeof( mgpcc_tcb.task_name));
    #endif
    (void) std_strlcpy( mgpmc_tcb.task_name, "MGPMC"  , sizeof( mgpmc_tcb.task_name));
    (void) std_strlcpy( mgpnf_tcb.task_name, "MGPNF"  , sizeof( mgpnf_tcb.task_name));
    (void) std_strlcpy( mgpcd_tcb.task_name, "MGPCD"  , sizeof( mgpcd_tcb.task_name));
    (void) std_strlcpy( mgpai_tcb.task_name, "MGPAI"  , sizeof( mgpai_tcb.task_name));

    #ifdef FEATURE_MGPCC_TASK
    /* Create Channel Controller task
    */
    TMC_MSG_MED( "Creating MGPCC task thread", 0, 0, 0 );
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&mgpcc_tcb,
                        ( unsigned char * ) mgpcc_stack,
                        MGPCC_STACK_SIZ * sizeof(rex_stack_word_type) ,
                        (rex_priority_type) MGPCC_PRI,
                        mgpcc_task,
                        0L,
                        "MGPCC",
                        FALSE,
                        DOG_MGPCC_RPT );
    #else
      rex_def_task( &mgpcc_tcb,
                    ( rex_stack_word_type* ) mgpcc_stack,
                    MGPCC_STACK_SIZ,
                    (rex_priority_type) MGPCC_PRI,
                    mgpcc_task,
                    0L );
    #endif
    (void) tmc_wait( TMC_ACK_SIG );
    (void) rex_clr_sigs( rex_self(), TMC_ACK_SIG );
    #endif /* FEATURE_MGPCC_TASK */

    /* Create Measurement Controller task
    */
    TMC_MSG_MED( "Creating MGPMC task thread", 0, 0, 0 );
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&mgpmc_tcb,
                        ( unsigned char * ) mgpmc_stack,
                        MGPMC_STACK_SIZ * sizeof(rex_stack_word_type) ,
                        (rex_priority_type) MGPMC_PRI,
                        mgpmc_task,
                        0L,
                        "MGPMC",
                        FALSE,
                        DOG_MGPMC_RPT );
    #else
      rex_def_task( &mgpmc_tcb,
                    ( rex_stack_word_type* ) mgpmc_stack,
                    MGPMC_STACK_SIZ,
                    (rex_priority_type) MGPMC_PRI,
                    mgpmc_task,
                    0L );
    #endif
    (void) tmc_wait( TMC_ACK_SIG );
    (void) rex_clr_sigs( rex_self(), TMC_ACK_SIG );

    /* Create Navigational Fix task
    */
    TMC_MSG_MED( "Creating MGPNF task thread", 0, 0, 0 );
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&mgpnf_tcb,
                        ( unsigned char * ) mgpnf_stack,
                        MGPNF_STACK_SIZ *sizeof(rex_stack_word_type),
                        (rex_priority_type) MGPNF_PRI,
                        mgpnf_task,
                        0L,
                        "MGPNF",
                        FALSE,
                        DOG_MGPNF_RPT );
    #else
      rex_def_task( &mgpnf_tcb,
                    ( rex_stack_word_type* ) mgpnf_stack,
                    MGPNF_STACK_SIZ,
                    (rex_priority_type) MGPNF_PRI,
                    mgpnf_task,
                    0L );
    #endif
    (void) tmc_wait( TMC_ACK_SIG );
    (void) rex_clr_sigs( rex_self(), TMC_ACK_SIG );

    /* Create Constellation data task
    */
    TMC_MSG_MED( "Creating MGPCD task thread", 0, 0, 0 );
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&mgpcd_tcb,
                        ( unsigned char * ) mgpcd_stack,
                        MGPCD_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) MGPCD_PRI,
                        mgpcd_task,
                        0L,
                        "MGPCD",
                        FALSE,
                        DOG_MGPCD_RPT );
    #else
      rex_def_task( &mgpcd_tcb,
                    ( rex_stack_word_type* ) mgpcd_stack,
                    MGPCD_STACK_SIZ,
                    (rex_priority_type) MGPCD_PRI,
                    mgpcd_task,
                    0L );
    #endif
    (void) tmc_wait( TMC_ACK_SIG );
    (void) rex_clr_sigs( rex_self(), TMC_ACK_SIG );

    /* Create Air Interface Independent task
    */
    TMC_MSG_MED( "Creating MGPAI task thread", 0, 0, 0 );
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&mgpai_tcb,
                        ( unsigned char * ) mgpai_stack,
                        MGPAI_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) MGPAI_PRI,
                        mgpai_task,
                        0L,
                        "MGPAI",
                        FALSE,
                        DOG_MGPAI_RPT );
    #else
      rex_def_task( &mgpai_tcb,
                    ( rex_stack_word_type* ) mgpai_stack,
                    MGPAI_STACK_SIZ,
                    (rex_priority_type) MGPAI_PRI,
                    mgpai_task,
                    0L );
    #endif
    (void) tmc_wait( TMC_ACK_SIG );
    (void) rex_clr_sigs( rex_self(), TMC_ACK_SIG );

  #endif /* FEATURE_MGP */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
#ifdef FEATURE_CGPS
#error code not present
  #endif /* FEATURE_CGPS */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_FTM_TASK
    #ifdef TIMETEST
      ftm_tcb.leds = TIMETEST_FTM_TASK_ID;
    #endif

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < FTM_STACK_SIZ; i++) {
        ftm_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy( ftm_tcb.task_name, "FTM"  , sizeof( ftm_tcb.task_name));
    /* Create Factory Test Mode task
    */
    TMC_MSG_MED( "Creating FTM task thread", 0, 0, 0 );
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&ftm_tcb,
                        (unsigned char *) ftm_stack,
                        FTM_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) FTM_PRI,
                        ftm_task,
                        0L,
                        "FTM",
                        FALSE,
                        DOG_FTM_RPT );
    #else
      rex_def_task(&ftm_tcb,
                   ( rex_stack_word_type* ) ftm_stack,
                   FTM_STACK_SIZ,
                   (rex_priority_type) FTM_PRI,
                   ftm_task,
                   0L );
    #endif
    (void) tmc_wait( TMC_ACK_SIG );
    (void) rex_clr_sigs( rex_self(), TMC_ACK_SIG );

  #endif /* FEATURE_FTM_TASK */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_CHG_TASK
    #ifdef TIMETEST
      chg_tcb.leds = TIMETEST_CHG_TASK_ID;
    #endif

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < CHG_STACK_SIZ; i++) {
        chg_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy( chg_tcb.task_name, "CHG", sizeof(chg_tcb.task_name) );
    /* Create CHG task
    */
    MSG_FATAL( "Creating CHG task thread", 0, 0, 0 );
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&chg_tcb,
                        (unsigned char *) chg_stack,
                        CHG_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) CHG_PRI,
                        chg_task,
                        0L,
                        "CHG",
                        FALSE,
                        DOG_CHG_RPT );
    #else
      rex_def_task(&chg_tcb,
                   ( rex_stack_word_type* ) chg_stack,
                   CHG_STACK_SIZ,
                   (rex_priority_type) CHG_PRI,
                   chg_task,
                   0L );
    #endif
    (void) tmc_wait( TMC_ACK_SIG );
    (void) rex_clr_sigs( rex_self(), TMC_ACK_SIG );

  #endif /* FEATURE_CHG_TASK */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #if (defined(FEATURE_QVPHONE) && (!defined(T_MSM7500)) && (!defined(T_MSM7200)))
    #ifdef TIMETEST
      qvp_ae_tcb.leds = TIMETEST_QVP_AUDIO_TASK_ID;
    #endif

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < QVP_AE_STACK_SIZ; i++) {
        qvp_ae_task_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy( qvp_ae_tcb.task_name, "VPAE"  , sizeof( qvp_ae_tcb.task_name));
    TMC_MSG_MED( "Creating QVPHONE audio engine task thread", 0, 0, 0 );
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2( &qvp_ae_tcb,
                         (unsigned char *) qvp_ae_task_stack,
                         QVP_AE_STACK_SIZ * sizeof(rex_stack_word_type),
                         (rex_priority_type) QVP_AE_PRI,
                         qvp_ae_task,
                         0L,
                         "QVP AUDIO",
                         FALSE,
                         DOG_QVP_AE_RPT);
    #else
      rex_def_task(&qvp_ae_tcb,
                   (rex_stack_word_type *) qvp_ae_task_stack,
                   QVP_AE_STACK_SIZ,
                   (rex_priority_type) QVP_AE_PRI,
                   qvp_ae_task,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

    /* Start QVP mpeg4 task
    */
    #ifdef TIMETEST
      qvp_mpeg4_tcb.leds = TIMETEST_QVP_MPEG4_TASK_ID;
    #endif

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < QVP_MPEG4_STACK_SIZ; i++) {
        qvp_mpeg4_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy( qvp_mpeg4_tcb.task_name, "VPMP"  , sizeof( qvp_mpeg4_tcb.task_name));
    TMC_MSG_MED("Starting  QVP mpeg4 task...", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2( &qvp_mpeg4_tcb,
                         (unsigned char *) qvp_mpeg4_stack,
                         QVP_MPEG4_STACK_SIZ * sizeof(rex_stack_word_type),
                         (rex_priority_type) QVP_MPEG4_PRI,
                         qvp_mpeg4_task,
                         0L,
                         "QVP MPEG4",
                         FALSE,
                         DOG_QVP_MPEG4_RPT);
    #else
      rex_def_task(&qvp_mpeg4_tcb,
                   (rex_stack_word_type *) qvp_mpeg4_stack,
                   QVP_MPEG4_STACK_SIZ,
                   (rex_priority_type) QVP_MPEG4_PRI,
                   qvp_mpeg4_task,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

    #ifdef FEATURE_QVP_APP_INCLUDE
#error code not present
    #endif /* FEATURE_QVP_APP_INCLUDE */

    #ifdef FEATURE_QVPHONE_SIP
#error code not present
    #endif /* FEATURE_QVPHONE_SIP */

    /* Start QVP IO task
    */
    #ifdef TIMETEST
      qvpio_tcb.leds = TIMETEST_QVP_IO_TASK_ID;
    #endif

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < QVPIO_STACK_SIZ; i++) {
        qvpio_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy( qvpio_tcb.task_name, "VPIO"  , sizeof( qvpio_tcb.task_name));
    TMC_MSG_MED("Creating QVPIO task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2( &qvpio_tcb,
                         ( unsigned char *) qvpio_stack,
                         QVPIO_STACK_SIZ * sizeof(rex_stack_word_type),
                         (rex_priority_type) QVPIO_PRI,
                         qvpio_task,
                         0L,
                         "QVPIO",
                         FALSE,
                         DOG_QVPIO_RPT);
    #else
      rex_def_task(&qvpio_tcb,
                   (rex_stack_word_type*) qvpio_stack,
                   QVPIO_STACK_SIZ,
                   (rex_priority_type) QVPIO_PRI,
                   qvpio_task,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

    /* Create QVPPL task and wait for ack back from it. */
    #ifdef TIMETEST
      qvppl_tcb.leds = TIMETEST_QVP_PLAYER_TASK_ID;
    #endif

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < QVPPL_STACK_SIZ; i++) {
        qvppl_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy( qvppl_tcb.task_name, "VPPL"  , sizeof( qvppl_tcb.task_name));
    TMC_MSG_MED("Creating QVPPL task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2( &qvppl_tcb,
                         (unsigned char *) qvppl_stack,
                         QVPPL_STACK_SIZ * sizeof(rex_stack_word_type),
                         (rex_priority_type) QVPPL_PRI,
                         qvppl_task,
                         0L,
                         "QVPPL",
                         FALSE,
                         DOG_QVPPL_RPT);
    #else
      rex_def_task(&qvppl_tcb,
                   (rex_stack_word_type*) qvppl_stack,
                   QVPPL_STACK_SIZ,
                   (rex_priority_type) QVPPL_PRI,
                   qvppl_task,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG) ;
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

    /* Start QVP VFE task
    */
    #ifdef TIMETEST
      qvp_vfe_tcb.leds = TIMETEST_QVP_VFE_TASK_ID;
    #endif

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < QVP_VFE_STACK_SIZ; i++) {
        qvp_vfe_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy( qvp_vfe_tcb.task_name, "VPVFE"  , sizeof( qvp_vfe_tcb.task_name));
    TMC_MSG_MED("Starting  QVP VFE task...", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2( &qvp_vfe_tcb,
                         (unsigned char *) qvp_vfe_stack,
                         QVP_VFE_STACK_SIZ * sizeof(rex_stack_word_type),
                         (rex_priority_type) QVP_VFE_PRI,
                         qvp_vfe_task,
                         0L,
                         "VPVFE",
                         FALSE,
                         DOG_QVP_VFE_RPT);
    #else
      rex_def_task(&qvp_vfe_tcb,
                   (rex_stack_word_type*) qvp_vfe_stack,
                   QVP_VFE_STACK_SIZ,
                   (rex_priority_type) QVP_VFE_PRI,
                   qvp_vfe_task,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

  #endif /* FEATURE_QVPHONE */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_BCMCS
#error code not present
  #endif /* FEATURE_BCMCS */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_DTV
#error code not present
  #endif /* FEATURE_DTV */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_JOYST_TASK

    #ifdef TIMETEST
      joyst_tcb.leds = TIMETEST_JOYST_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < JOYST_STACK_SIZ; i++) {
        joyst_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif  /* FEATURE_TMC_STACK_PROFILING */

    (void) std_strlcpy(joyst_tcb.task_name, "JOYST" , sizeof(joyst_tcb.task_name));

    /* Create Graphics task and wait for ack back from it.
    */
    TMC_MSG_MED("Creating Joystick task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&joyst_tcb,
                        (unsigned char *)joyst_stack,
                        JOYST_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) JOYST_PRI,
                        joyst_task,
                        0L,
                        "JOYST",
                        FALSE,
                        DOG_JOYST_RPT);
    #else
      rex_def_task(&joyst_tcb,
                   (rex_stack_word_type*) joyst_stack,
                   JOYST_STACK_SIZ,
                   (rex_priority_type) JOYST_PRI,
                   joyst_task,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG) ;
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

  #endif /* FEATURE_JOYST_TASK */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_PBM_TASK

    #ifdef TIMETEST
      pbm_tcb.leds = TIMETEST_PBM_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < PBM_STACK_SIZ; i++) {
        pbm_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(pbm_tcb.task_name, "PBM " , sizeof(pbm_tcb.task_name));

    /* Create PBM task and wait for ack back from it.
    */
    TMC_MSG_MED("Creating PBM task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&pbm_tcb,
                        (unsigned char *) pbm_stack,
                        PBM_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) PBM_PRI,
                        pbm_task,
                        0L,
                        "PBM",
                        FALSE,
                        DOG_PBM_RPT);
    #else
      rex_def_task(&pbm_tcb,
                   (rex_stack_word_type*) pbm_stack,
                   PBM_STACK_SIZ,
                   (rex_priority_type) PBM_PRI,
                   pbm_task,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

  #endif /* FEATURE_PBM_TASK */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  #ifdef FEATURE_CPU_BASED_FLOW_CONTROL

    #ifdef TIMETEST
      fc_tcb.leds = TIMETEST_FC_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < FC_STACK_SIZ; i++) {
        fc_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(fc_tcb.task_name, "FC" , sizeof(fc_tcb.task_name));

    /* Create FC task and wait for ack back from it.
    */
    TMC_MSG_MED("Creating FC task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&fc_tcb,
                        (unsigned char *) fc_stack,
                        FC_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) FC_PRI,
                        fc_task,
                        0L,
                        "FC",
                        FALSE,
                        DOG_FC_RPT);
    #else
      rex_def_task(&fc_tcb,
                   (rex_stack_word_type*) fc_stack,
                   FC_STACK_SIZ,
                   (rex_priority_type) FC_PRI,
                   fc_task,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

  #endif /* FEATURE_CPU_BASED_FLOW_CONTROL_TASK */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_FM     // fm radio/rds support task

    // legacy
    // #ifdef TIMETEST
    //   fmrds_tcb.leds        = TIMETEST_FMRDS_TASK_ID;
    // #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < FMRDS_STACK_SIZ; i++) {
        fmrds_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(fmrds_tcb.task_name, "FM-RDS  " , sizeof(fmrds_tcb.task_name));

    // create fm-rds task and wait for ack...
    TMC_MSG_MED("Creating FM-RDS task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&fmrds_tcb,
                        (unsigned char *)fmrds_stack,
                        FMRDS_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) FMRDS_PRI,
                        fmrds_task,
                        0L,
                        "FMRDS",
                        FALSE,
                        DOG_FMRDS_RPT);
    #else
      rex_def_task(&fmrds_tcb,
                   (rex_stack_word_type*) fmrds_stack,
                   FMRDS_STACK_SIZ,
                   (rex_priority_type) FMRDS_PRI,
                   fmrds_task,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG) ;
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

  #endif    // FEATURE_FM


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
// sirius platform-related tasks

#ifdef SIRIUS_PLATFORM
#error code not present
#endif /* SIRIUS_PLATFORM */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef  FEATURE_PM_SERVER

    #ifdef TIMETEST
      pms_tcb.leds = TIMETEST_PMS_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for( i = 0; i < PMSERV_STACK_SIZ; i++ )
      {
        pms_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(pms_tcb.task_name, "PMS" , sizeof(pms_tcb.task_name));

    TMC_MSG_MED(" Creating PM Server ... ", 0, 0, 0);

    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&pms_tcb,
                        (unsigned char *) pms_stack,
                        PMSERV_STACK_SIZ* sizeof(rex_stack_word_type),
                        (rex_priority_type) PM_SERVER_PRI,
                        pm_server_task,
                        0L,
                        "PM",
                        FALSE,
                        DOG_PMS_RPT);
    #else
      rex_def_task(&pms_tcb,
                   (rex_stack_word_type*) pms_stack,
                   PMSERV_STACK_SIZ,
                   (rex_priority_type) PM_SERVER_PRI,
                   pm_server_task,
                   0L);
    #endif

    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

  #endif /* FEATURE_PM_SERVER */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_APS

    #ifdef TIMETEST
      aps_tcb.leds = TIMETEST_APS_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for( i = 0; i < APS_STACK_SIZ; i++ )
      {
        aps_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(aps_tcb.task_name, "APS" , sizeof(aps_tcb.task_name));

    /* Create APS task and wait for ack
    */
    TMC_MSG_MED("Creating APS task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&aps_tcb,
                        (unsigned char *) aps_stack,
                        APS_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) APS_PRI,
                        aps_task,
                        0L,
                        "APS",
                        FALSE,
                        DOG_APS_RPT);
    #else
      rex_def_task(&aps_tcb,
                   (rex_stack_word_type*) aps_stack,
                   APS_STACK_SIZ,
                   (rex_priority_type) APS_PRI,
                   aps_task,
                   0L);
    #endif

    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

  #endif /* FEATURE_APS */

  #ifdef FEATURE_ATS

    #ifdef TIMETEST
      ats_tcb.leds = TIMETEST_ATS_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for( i = 0; i < ATS_STACK_SIZ; i++ )
      {
        ats_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(ats_tcb.task_name, "ATS" , sizeof(ats_tcb.task_name));

    /* Create ATS task and wait for ack
    */
    TMC_MSG_MED("Creating ATS task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&ats_tcb,
                        (unsigned char *) ats_stack,
                        ATS_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) ATS_PRI,
                        ats_task,
                        0L,
                        "ATS",
                        FALSE,
                        DOG_ATS_RPT);
    #else
      rex_def_task(&ats_tcb,
                   (rex_stack_word_type*) ats_stack,
                   ATS_STACK_SIZ,
                   (rex_priority_type) ATS_PRI,
                   ats_task,
                   0L);
    #endif

    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

  #endif /* FEATURE_ATS */



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_QIPCALL

    #ifndef FEATURE_MULTIPROCESSOR

    #ifdef TIMETEST
      qipcall_tcb.leds = TIMETEST_QIPCALL_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < QIPCALL_STACK_SIZ; i++) {
        qipcall_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(qipcall_tcb.task_name, "QIPCALL  " , sizeof(qipcall_tcb.task_name));

    /* Create QIPCALL task and wait for ack back from it.
    */
    TMC_MSG_MED("Creating QIPCALL task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&qipcall_tcb,
                        (unsigned char *) qipcall_stack,
                        QIPCALL_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) QIPCALL_PRI,
                        qipcall_task,
                        0L,
                        "QIPCALL",
                        FALSE,
                        DOG_QIPCALL_RPT);
    #else
      rex_def_task(&qipcall_tcb,
                   (rex_stack_word_type*) qipcall_stack,
                   QIPCALL_STACK_SIZ,
                   (rex_priority_type) QIPCALL_PRI,
                   qipcall_task,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

    #endif /* FEATURE_MULTIPROCESSOR */

  #endif /* FEATURE_QIPCALL */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_MFLO
#error code not present
  #endif /* FEATURE_MFLO */

  #ifdef FEATURE_UBM_L1
  
    #ifdef TIMETEST
    /* No LED value assigned yet */
    ubm_l1_tcb.leds = 0x0;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
    for (i = 0; i < UBM_L1_STACK_SIZ; i++) 
    {
      ubm_l1_stack[i] = TMC_STACK_PROFILING_DWORD;
    }
    #endif

    TMC_MSG_MED("Creating UBM L1 task.", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
    rex_def_task_ext2(&ubm_l1_tcb,
                      (unsigned char *) ubm_l1_stack,
                      UBM_L1_STACK_SIZ * sizeof(rex_stack_word_type),
                      (rex_priority_type) UBM_L1_PRI,
                      ubm_l1_task,
                      0L,
                      "UBML1",
                      FALSE,
                      DOG_UBM_L1_RPT);
    #else
    (void) std_strlcpy(&ubm_l1_tcb.task_name[0], "UBML1" , sizeof(&ubm_l1_tcb.task_name[0]));
    rex_def_task(&ubm_l1_tcb,
                 (rex_stack_word_type*) ubm_l1_stack,
                 UBM_L1_STACK_SIZ,
                 (rex_priority_type) UBM_L1_PRI,
                 ubm_l1_task,
                 0L);
    #endif

  (void) tmc_wait(TMC_ACK_SIG); 
  (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_UBM_L1 */

  #if (defined(FEATURE_DISP_TASK)) && (!defined(T_MSM7500) || \
      (defined(T_MSM7500) && defined(IMAGE_APPS_PROC)))

    #ifdef TIMETEST
      disp_tcb.leds = TIMETEST_DISP_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < DISP_STACK_SIZ; i++) {
        disp_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(disp_tcb.task_name, "DISP" , sizeof(disp_tcb.task_name));

    /* Create display task and wait for ack back from it.
    */
    TMC_MSG_MED("Creating DISP task.", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&disp_tcb,
                        (unsigned char *)disp_stack,
                        DISP_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) DISP_PRI,
                        disp_task,
                        0L,
                        "DISP",
                        FALSE,
                        DOG_DISP_RPT);
    #else
      rex_def_task(&disp_tcb,
                   (rex_stack_word_type*) disp_stack,
                   DISP_STACK_SIZ,
                   (rex_priority_type) DISP_PRI,
                   disp_task,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

  #endif /* FEATURE_DISP_TASK */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#if defined(FEATURE_HS_USB) && (defined(FEATURE_STANDALONE_MODEM) || !defined(FEATURE_NO_USB_HW_ON_PROCESSOR))
#ifdef TIMETEST
      hsu_al_task_tcb.leds = TIMETEST_HSU_AL_TASK_ID;
#endif /* TIMETEST */
  
    /* Create HSU AL task */
  #ifdef FEATURE_ENHANCED_REX_TASK
    rex_def_task_ext2(&hsu_al_task_tcb,
      hsu_al_task_stack,
      AL_TASK_STACK_SIZE_IN_BYTES,
      (rex_priority_type)HSU_AL_TASK_PRI,
      hsu_al_task_main,
      0L,
      HSU_AL_TASK_NAME,
      FALSE,
      DOG_HSU_AL_TASK_RPT);
  #else
    rex_def_task(&hsu_al_task_tcb,
      (rex_stack_word_type*) hsu_al_task_stack,
  
      (AL_TASK_STACK_SIZE_IN_BYTES \
      sizeof(rex_stack_word_type)) * STACK_SIZ_FACTOR,
  
      (rex_priority_type)HSU_AL_TASK_PRI,
      hsu_al_task_main,
      0L);
  #endif  /* FEATURE_ENHANCED_REX_TASK */
  
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
#endif  /* FEATURE_HS_USB && (FEATURE_STANDALONE_MODEM || !FEATURE_NO_USB_HW_ON_PROCESSOR) */
  
#if defined(FEATURE_HS_USB_MS_FD) && (defined(FEATURE_STANDALONE_MODEM) || !defined(FEATURE_NO_USB_HW_ON_PROCESSOR))
#ifdef TIMETEST
    hsu_al_ms_tcb.leds = TIMETEST_HSU_AL_MS_TASK_ID;
#endif /* TIMETEST */
  
    /* Create HSU_AL_MS task */
#ifdef FEATURE_ENHANCED_REX_TASK
    rex_def_task_ext2(&hsu_al_ms_tcb,
      hsu_al_ms_stack,
      HSU_AL_MS_STACK_SIZE * sizeof(rex_stack_word_type),
      (rex_priority_type)HSU_AL_MS_PRI,
      hsu_al_ms_task,
      0L,
      HSU_AL_MS_TASK_NAME,
      FALSE,
      DOG_HSU_AL_MS_RPT);
#else
    rex_def_task(&hsu_al_ms_tcb,
      (rex_stack_word_type*) hsu_al_ms_stack,
      HSU_AL_MS_STACK_SIZE,
      (rex_priority_type) HSU_AL_MS_PRI,
      hsu_al_ms_task,
      0L);
#endif /* FEATURE_ENHANCED_REX_TASK */
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
#endif /* FEATURE_HS_USB_MS_FD && (FEATURE_STANDALONE_MODEM || !FEATURE_NO_USB_HW_ON_PROCESSOR) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifdef FEATURE_QDJTEST

    #ifdef TIMETEST
      qdjtest_tcb.leds = TIMETEST_QDJTEST_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < QDJTEST_STACK_SIZ; i++) {
        qdjtest_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(qdjtest_tcb.task_name, "QDJTEST" , sizeof(qdjtest_tcb.task_name));

    /* Create QDJTEST task and wait for ack back from it.
    */
    TMC_MSG_MED("Creating QDJTEST task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&qdjtest_tcb,
                        (unsigned char *) qdjtest_stack,
                        QDJTEST_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) QDJTEST_PRI,
                        qdjtest_task,
                        0L,
                        "QDJTEST",
                        FALSE,
                        DOG_QDJTEST_RPT);
    #else
      rex_def_task(&qdjtest_tcb,
                   (rex_stack_word_type*) qdjtest_stack,
                   QDJTEST_STACK_SIZ,
                   (rex_priority_type) QDJTEST_PRI,
                   qdjtest_task,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

  #endif /* FEATURE_QDJTEST */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifdef FEATURE_QTV_STATIC_TASKS

    /* QTV VIDEO RENDERER */

    #ifdef TIMETEST
      qtv_video_renderer_tcb.leds = TIMETEST_MP4_PV_RENDERER_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < QTV_VIDEO_RENDERER_STACK_SIZ; i++) {
        qtv_video_renderer_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(qtv_video_renderer_tcb.task_name, "QTVvrend" , sizeof(qtv_video_renderer_tcb.task_name));

    /* Create QTV renderer task and wait for ack back from it.
    */
    TMC_MSG_MED("Creating QTV renderer task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&qtv_video_renderer_tcb,
                        (unsigned char *) qtv_video_renderer_stack,
                        QTV_VIDEO_RENDERER_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) QTV_VIDEO_RENDERER_PRI,
                        qtv_task_fn,
                        ( dword )&qtv_task_video_renderer_info,
                        "QTVvrend",
                        FALSE,
                        DOG_QTV_VIDEO_RENDERER_RPT);
    #else
      rex_def_task(&qtv_video_renderer_tcb,
                   (rex_stack_word_type*) qtv_video_renderer_stack,
                   QTV_VIDEO_RENDERER_STACK_SIZ,
                   (rex_priority_type) QTV_VIDEO_RENDERER_PRI,
                   qtv_task_fn,
                   ( dword )&qtv_task_video_renderer_info);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

    /* QTV AUDIO */

    #ifdef TIMETEST
      qtv_video_renderer_tcb.leds = TIMETEST_MP4_PV_AUDIO_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < QTV_AUDIO_STACK_SIZ; i++) {
        qtv_audio_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(qtv_audio_tcb.task_name, "QTVaudio" , sizeof(qtv_audio_tcb.task_name));

    /* Create QTV audio task and wait for ack back from it.
    */
    TMC_MSG_MED("Creating QTV audio task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&qtv_audio_tcb,
                        (unsigned char *) qtv_audio_stack,
                        QTV_AUDIO_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) QTV_AUDIO_PRI,
                        qtv_task_fn,
                        ( dword )&qtv_task_audio_info,
                        "QTVaudio",
                        FALSE,
                        DOG_QTV_AUDIO_RPT);
    #else
      rex_def_task(&qtv_audio_tcb,
                   (rex_stack_word_type*) qtv_audio_stack,
                   QTV_AUDIO_STACK_SIZ,
                   (rex_priority_type) QTV_AUDIO_PRI,
                   qtv_task_fn,
                   ( dword )&qtv_task_audio_info);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

  #endif /* FEATURE_QTV_STATIC_TASKS */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifdef FEATURE_SENSORS
    #ifdef TIMETEST
      sns_tcb.leds = TIMETEST_SNS_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < SNS_STACK_SIZ; i++) {
        sns_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(sns_tcb.task_name, "SNS", sizeof(sns_tcb.task_name));

    TMC_MSG_MED("Creating SNS task", 0, 0, 0);
    rex_def_task( &sns_tcb,
                  (rex_stack_word_type*) sns_stack,
                  SNS_STACK_SIZ,
                  (rex_priority_type) SNS_PRI,
                  sns_task,
                  0L);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_SENSORS */

  #ifdef FEATURE_QTV_STATIC_DLAGENT_TASK

    /* QTV DLAGENT Task */

    #ifdef TIMETEST
	  qtv_dec_dlAgenttask_tcb.leds = TIMETEST_MP4_PV_DSPTASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < QTV_DEC_DLAGENTTASK_STACK_SIZ ; i++) {
        qtv_dec_dlAgenttask_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif


	(void) std_strlcpy(qtv_dec_dlAgenttask_tcb.task_name,"QTV_dlAgent", sizeof(qtv_dec_dlAgenttask_tcb.task_name));


    /* Create QTV renderer task and wait for ack back from it.
    */
    TMC_MSG_MED("Creating QTV DLAGENT Task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&qtv_dec_dlAgenttask_tcb,
                        (unsigned char *) qtv_dec_dlAgenttask_stack,
                        QTV_DEC_DLAGENTTASK_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) QTV_DEC_DLAGENTTASK_PRI,
                        DL_Agent_Task,
                        0L,
                        "QTV_dlAgent",
                        FALSE,
                        DOG_QTV_DLAGENTTASK_RPT);
    #else
      rex_def_task(&qtv_dec_dlAgenttask_tcb,
                   (rex_stack_word_type*) qtv_dec_dlAgenttask_stack,
                   QTV_DEC_DLAGENTTASK_STACK_SIZ,
                   (rex_priority_type) QTV_DEC_DLAGENTTASK_PRI,
                   DL_Agent_Task,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

#endif
#ifdef FEATURE_QTV_STATIC_DLDSPSVC_TASK
      /* QTV DSP Svc Task */

    #ifdef TIMETEST
	  qtv_dec_dlDspSvctask_tcb.leds = 0;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < QTV_DEC_DLDSPSVCTASK_STACK_SIZ ; i++) {
        qtv_dec_dlDspSvctask_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif


	(void) std_strlcpy(qtv_dec_dlDspSvctask_tcb.task_name, "QTV_dlDspSvc", sizeof(qtv_dec_dlDspSvctask_tcb.task_name));

    /* Create QTV renderer task and wait for ack back from it.
    */
    TMC_MSG_MED("Creating QTV DSP Services Task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&qtv_dec_dlDspSvctask_tcb,
                        (unsigned char *) qtv_dec_dlDspSvctask_stack,
                        QTV_DEC_DLDSPSVCTASK_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) QTV_DEC_DLDSPSVCTASK_PRI,
                        DL_Dsp_Svc_Task,
                        0L,
                        "QTV_dlDspSvc",
                        FALSE,
                        DOG_QTV_DLDSPSVCTASK_RPT);
    #else
      rex_def_task(&qtv_dec_dlDspSvctask_tcb,
                   (rex_stack_word_type*) qtv_dec_dlDspSvctask_stack,
                   QTV_DEC_DLDSPSVCTASK_STACK_SIZ,
                   (rex_priority_type) QTV_DEC_DLDSPSVCTASK_PRI,
                   DL_Dsp_Svc_Task,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /*FEATURE_QTV_STATIC_DLDSPSVC_TASK*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
} /* tmc_define_tasks(void) */


/* <EJECT> */
/*===========================================================================

FUNCTION tmc_libraries_init_before_service_tasks_start

DESCRIPTION
  Perform any initializations that must be done after the tasks are defined,
  but before any tasks (including the service tasks) are started.

DEPENDENCIES
  The tasks must have been defined, but not started.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void               tmc_libraries_init_before_service_tasks_start( void )
{

#if defined(T_MSM7500) && !defined(FEATURE_STANDALONE_MODEM)
#error code not present
#endif /* T_MSM7500 && ! FEATURE_STANDALONE_MODEM */

} /* tmc_libraries_init_before_service_tasks_start */


/* <EJECT> */
/*===========================================================================

FUNCTION tmc_libraries_init_after_service_tasks_start

DESCRIPTION
  Continue on initialization after service tasks start. This initialization
  requires NV task has been started. The initialization consists of generic
  initialization and mode specific initilaization. Generic refers to any HW
  regardless any specific mode. Specific refers to mode related ones.

DEPENDENCIES
  TMC must have already been initialized with tmc_init_before_task_start, and
  service tasks have already started.

RETURN VALUE
  boolean - Initialization success return TRUE othewise return FALSE.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL   boolean          tmc_libraries_init_after_service_tasks_start( void )
{

  boolean rf_initial_status = TRUE;
  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifndef FEATURE_RF_PARK_MODE
  #ifdef TMC_RFM_ENTER_MODE
  rf_init_status_type rf_ret_stat = RF_INIT_STATUS_MAX;
  #endif /* TMC_RFM_ENTER_MODE */
  #endif /* FEATURE_RF_PARK_MODE */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*                                                                       */
  /*                                                                       */
  /*                    Generic Initialization                             */
  /*                                                                       */
  /*                                                                       */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #if defined(T_MSM6500) && !defined(T_MSM7200)
  {
    /* This code downloads SRCH4_APP, the MDSP service task needs to be
       running at this point */
    extern void srch4_register_mdsp_app(rex_tcb_type *task_tcb,
                                          rex_sigs_type task_sig);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
    srch4_register_mdsp_app(rex_self(),TMC_ACK_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  }
  #endif /* T_MSM6500 && ! T_MSM7200 */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*                                                                       */
  /*                                                                       */
  /*                    HS USB Initialization                              */
  /*                                                                       */
  /*                                                                       */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#if defined(FEATURE_HS_USB) && (defined(FEATURE_STANDALONE_MODEM) || !defined(FEATURE_NO_USB_HW_ON_PROCESSOR))
  hsu_config_selector_init();
#endif



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*                                                                       */
  /*                                                                       */
  /*                   Mode Specific Initialization                        */
  /*                                                                       */
  /*                                                                       */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*                         1X specific                                   */

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef TMC_1X

    /* Load calibration items from NV on behalf of Vbatt library */

    #ifdef TMC_HAS_VBATT_CONFIG
    {
      nv_item_type          nv_data_buf;                   /* NV data buffer */
      nv_stat_enum_type     nv_read_status;                /* NV read status */
      /* NV_VBATT_I */

      /* Get nv read*/
      nv_read_status = tmcnv_read( NV_VBATT_I, &nv_data_buf );

      if (nv_read_status == NV_DONE_S) {

        vbatt_calibrate( nv_data_buf.vbatt.min, nv_data_buf.vbatt.max );

      }
      else {

        TMC_ERR( "Bad NV read status %d on vbatt calibration",
                 nv_read_status, 0, 0 );
      }
    }
    #endif

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Load calibration items from NV on behalf of Charger Utility */

    #ifdef FEATURE_CHARGER
#error code not present
    #endif

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Load calibration items from NV on behalf of Thermistor library */

    #ifdef TMC_HAS_THERM_CONFIG
    {
      nv_stat_enum_type nv_read_status;
      nv_item_type      nv_data_buf; /* NV data buffer */
      /* NV_THERM_I */

      nv_read_status = tmcnv_read( NV_THERM_I, &nv_data_buf );
      if (nv_read_status == NV_DONE_S) {
        therm_calibrate( nv_data_buf.therm.min, nv_data_buf.therm.max );
      }
      else {
        TMC_ERR( "Bad NV read status %d on therm calibration", nv_read_status, 0, 0 );
      }
    }
    #endif

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Load calibration items from NV on behalf of HW library */

    #ifdef TMC_HAS_HW_CONFIG
    {
      nv_stat_enum_type nv_read_status;
      nv_item_type      nv_data_buf; /* NV data buffer */

      /* NV_HW_CONFIG_I */

      nv_read_status = tmcnv_read( NV_HW_CONFIG_I, &nv_data_buf );
      if (nv_read_status == NV_DONE_S) {
        hw_init( nv_data_buf.hw_config );
      }
      else {
        TMC_ERR( "Bad NV read status %d on HW_CONFIG calibration", nv_read_status, 0, 0 );
      }
    }
    #endif

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Load items from NV on behalf of RF unit */

    #if defined(TMC_HAS_RF_CONFIG) && (!defined(FEATURE_MULTIMODE_RFAPI))
    {
        nv_stat_enum_type nv_read_status;
        nv_item_type      nv_data_buf; /* NV data buffer */
        byte                      rf_item_ctr;               /* loop counter*/
        const nv_items_enum_type *rf_item_list;
        /* Ptr to the list of RF NV items */
        word rf_list_siz;                        /* Number of items in RF NV list */

        /* Read RF calibration data from NV
        */

        rf_item_list = rfnv_get_nv_lst_addr();
        rf_list_siz = rfnv_list_size();

        for (rf_item_ctr=0; rf_item_ctr < rf_list_siz; rf_item_ctr++) {

          /* Get nv item */
          nv_read_status = tmcnv_read( rf_item_list[rf_item_ctr], &nv_data_buf );

          if (nv_read_status == NV_DONE_S) {

            /* Store nv data to data sructure */
            (void) rfnv_read_nv_item( rf_item_list[rf_item_ctr], &nv_data_buf );

          } else if (nv_read_status == NV_NOTACTIVE_S) {

            TMC_ERR( "NV RF cal item %d never written",
              rf_item_list[rf_item_ctr], 0 ,0 );

          } else {

            TMC_ERR( "Bad NV read status %d on RF cal item %d",
              nv_read_status, rf_item_list[rf_item_ctr], 0 );
          }
        }

    }
    #endif /* TMC_HAS_RF_CONFIG && ! ! FEATURE_MULTIMODE_RFAPI*/

  #endif /* TMC_1X */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

    /* MM RF driver initialization
    ** Leopard/Cheetah:
    ** 1. Initializes common RF hardware and RF NV variables.
    ** 2. Other remaining RF initialization is done where protocols are activated.
    ** Cougar
    ** 1. All Initialization will done here and only once.
    ** Notes: If phone is in FTM mode, it also does RF initialization.
    */

    #ifndef FEATURE_BRINGUP_DIAG_ONLY
    #ifndef FEATURE_RF_PARK_MODE
    #if defined(RF_MULTIMODE) || defined(FEATURE_MULTIMODE_RFAPI)
    rf_initial_status = rfm_init(
                                  #ifdef FEATURE_MULTI_RX_CHAIN
                                  RFCOM_TRANSCEIVER_0,
                                  #endif
                                  rex_self(),
                                  TMC_NV_CMD_SIG,
                                  (void (*)( rex_sigs_type )) tmc_wait );
    #ifdef TMC_RFM_ENTER_MODE
    (void) rfm_enter_mode(
                          #ifdef FEATURE_MULTI_RX_CHAIN
                          RFCOM_TRANSCEIVER_0,
                          #endif
                          RFCOM_1X_MODE,
                          (void *) &rf_ret_stat,
                          NULL );/* RF 1X mode */
    #endif /* TMC_RFM_ENTER_MODE */

    #else
    #ifdef TMC_1X
      if( rf_init_delayed() != MC_ONLINE_F )
        rf_initial_status = FALSE;
    #else
       rf_initial_status = rf_init( rex_self(),
                                    TMC_NV_CMD_SIG,
                                    (void (*)( rex_sigs_type )) tmc_wait );
    #endif /* TMC_1X */
    #endif /* RF_MULTIMODE || FEATURE_MULTIMODE_RFAPI */
    #else  /* FEATURE_RF_PARK_MODE */
       #if defined(RF_MULTIMODE) || defined(FEATURE_MULTIMODE_RFAPI)
       rf_initial_status =   rfm_init(
                                       #ifdef FEATURE_MULTI_RX_CHAIN
                                          RFCOM_TRANSCEIVER_0,
                                       #endif
                                       rex_self(),
                                       TMC_NV_CMD_SIG,
                                       (void (*)( rex_sigs_type )) tmc_wait) ;
        (void) rfm_enter_mode(
                            #ifdef FEATURE_MULTI_RX_CHAIN
                               RFCOM_TRANSCEIVER_0,
                            #endif
                            RFCOM_1X_MODE,
                            NULL,
                            NULL );/* RF 1X mode */
       #else
          #ifdef TMC_1X
          if( rf_init_delayed() != MC_ONLINE_F )
              rf_initial_status = RF_INIT_HW_FAIL;
          #else
          rf_initial_status = rf_init( rex_self(),
                                       TMC_NV_CMD_SIG,
                                       (void (*)( rex_sigs_type )) tmc_wait );
          #endif /* TMC_1X */
       #endif /* RF_MULTIMODE || FEATURE_MULTIMODE_RFAPI */
    #endif /* FEATURE_RF_PARK_MODE */

    #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  /* If RF initialization is fail,
  ** case I nv ftm flag is true:
  **   do nothing.
  ** case II nv ftm flag is invalid:
  **  set nv ftm flag to TRUE, ftm_mode.
  **
  ** Otherwise do nothing.
  */
  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_FACTORY_TESTMODE
  {
    nv_stat_enum_type nv_read_status;
    ftm_mode_type     nv_data_buf; /* NV data buffer */

    if (!rf_initial_status)
    {

      nv_read_status = tmcnv_read( NV_FTM_MODE_I, (nv_item_type *)&nv_data_buf );

      /* If NV read failed, substitute a default value.*/
      if( nv_read_status != NV_DONE_S )
      {
        TMC_MSG_ERROR( "NV read failed, item=%d, status=%d",
                       NV_FTM_MODE_I, nv_read_status, 0 );

        if (!rf_initial_status)
        {
          nv_data_buf = FTM_MODE;
          (void) tmcnv_write( NV_FTM_MODE_I,(nv_item_type *)&nv_data_buf);
        }
      }
    }
  }
  #endif /* FEATURE_FACTORY_TESTMODE */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  /* Update status if rf fails in enter mode. */
  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifndef FEATURE_RF_PARK_MODE
  #ifdef TMC_RFM_ENTER_MODE
  if( rf_ret_stat != RF_INIT_SUCCESS )
  {
    rf_initial_status = FALSE;
    TMC_MSG_MED("RF enter mode fail, error code :", rf_ret_stat, 0, 0);
  }
  #endif /* TMC_RFM_ENTER_MODE */
  #endif /* FEATURE_RF_PARK_MODE */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  #ifdef FEATURE_CC_MONITOR
#error code not present
  #endif

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_CRASH_DEBUG_DISABLED_CHECK
  /* Check whether the NV item is set */
  {
    nv_stat_enum_type     nv_read_status;                /* NV read status */
    boolean               nv_crash_dump_disallowed;
    boolean               crash_dump__disallowed_read_status =FALSE;
    /* Get nv read*/
    nv_read_status = tmcnv_read( NV_CRASH_DEBUG_DISALLOWED_I, (nv_item_type *)&nv_crash_dump_disallowed );
    if ( (nv_read_status == NV_DONE_S) && (nv_crash_dump_disallowed == TRUE) )
      crash_dump__disallowed_read_status = TRUE;
    hw_crash_debug_init(crash_dump__disallowed_read_status);
  }
  #endif /* FEATURE_CRASH_DEBUG_DISABLED_CHECK */
  #ifdef FEATURE_ERR_EXTENDED_STORE
    err_store_info_to_flash();
  #endif
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  /* Initial SD before application tasks.
  **
  ** The band capability provided by SD need to be ready before CM/UI task
  ** get started.
  */

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  sd_init_before_app_task_start();
  #endif /*  FEATURE_BRINGUP_DIAG_ONLY */

  #ifdef FEATURE_ADC_CALIBRATION

  adc_init();

  #endif /* FEATURE_ADC_CALIBRATION */

#ifndef FEATURE_BRINGUP_DIAG_ONLY
#if defined (FEATURE_TMC_TCXOMGR) || defined (FEATURE_TCXOMGR_TMC)
  (void) tcxomgr_init(rex_self(), TMC_NV_CMD_SIG,
                      (void (*)( rex_sigs_type ))tmc_wait);
  #endif

  #ifdef TMC_1X
     #ifdef FEATURE_TMC_TRM
     /* Initialize TRM.  This call must be made after rf_init which is also
        done in tmc_libraries_init_after_service_tasks_start. Note that no
        TRM clients can call TRM until after they first call task_start. */
     trm_init(rex_self(), TMC_NV_CMD_SIG, (trm_wait_callback_t)tmc_wait);
     #endif
  #endif
#endif

  #ifdef FEATURE_MMGPS
#error code not present
  #endif /* FEATURE_MMGPS */

  
  #if defined(FEATURE_RUNTIME_DEVMAP) && defined(FEATURE_EFS) && defined(IMAGE_MODEM_PROC)
    /* Get any previous port mappings on behalf of the Runtime Device Mapper
    */
    rdm_get_prev_ports();
  #endif

  #if defined(FEATURE_RUNTIME_DEVMAP) && defined(IMAGE_MODEM_PROC)
    /* Have RDM issue any open commands. If there were none retrieved from
    ** EFS, compile time defaults will be issued
    */
    rdm_issue_opens();
  #endif

  #ifdef FEATURE_BATTERY_CHARGER
    chg_init_delayed( rex_self(),
                      TMC_NV_CMD_SIG,
                      tmc_wait );
  #endif

  #ifdef FEATURE_CC_MONITOR
#error code not present
  #endif /* FEATURE_CC_MONITOR */


  #ifdef FEATURE_TIME_TOD_AS_RTC_OFFSET
  time_tod_add_rtc_offset();     
  #endif /* FEATURE_TIME_TOD_AS_RTC_OFFSET */

  #if defined(FEATURE_SEC_TIME) && !defined(FEATURE_SEC_PROCESS_INIT)
  sectime_init( rex_self(),
                TMC_NV_CMD_SIG,
                (void (*)( rex_sigs_type )) tmc_wait );
  #endif /* FEATURE_SEC_TIME && !FEATURE_SEC_PROCESS_INIT */

  //tmc_vote_to_sleep( TRUE );

  return rf_initial_status;

} /* tmc_libraries_init_after_service_tasks_start */


/* <EJECT> */
/*===========================================================================

FUNCTION tmc_start_service_tasks()

DESCRIPTION
  This function starts service tasks tasks. Service tasks are  defined as
  those that are required to be started so that the system can determine if
  it is to continue powerup or if it should return to powerdown. The NV
  task is started first, to allow all other tasks to read any needed
  variables from NVM.

  It sends START signals to all tasks, one at a time. After sending a START
  signal to a task, it expects an Ack back from the task. Once Ack is
  received, it sends a START Signal to the next task. This handshaking
  ensures a controlled starting of all tasks.


DEPENDENCIES
  All tasks must already be created through tmc_define_tasks().

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
LOCAL   void                       tmc_start_service_tasks( void )
{

  /* Clear ack signal. */
  (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_ONCRPC
#error code not present
  #endif /* FEATURE_ONCRPC */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_EFS
    /* Start Embedded File System task and wait for response.
    */
    TMC_MSG_MED("Starting FS task...", 0, 0, 0);
    (void) rex_set_sigs(&fs_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_EFS */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if (defined(FS_FEATURE_RMTEFS_SERVER) || defined(FS_FEATURE_IG_IPC_SERVER) ) \
         && !defined(FEATURE_IG_EFS_EXT_SERVER)
  /* Start Embedded File System task and wait for response.
  */
  TMC_MSG_MED("Starting EFS Server task...", 0, 0, 0);
  (void) rex_set_sigs(&efs_srvr_tcb, TASK_START_SIG);
  (void) tmc_wait(TMC_ACK_SIG);
  (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FS_FEATURE_RMTEFS_SERVER && !FEATURE_IG_EFS_EXT_SERVER */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_EFS_SFAT
    /* Start SFAT task and wait for response.
    */
    TMC_MSG_MED("Starting SFAT task...", 0, 0, 0);
    (void) rex_set_sigs(&sfat_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_EFS_SFAT */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  #ifndef FEATURE_SEC_PROCESS_INIT
  #ifdef FEATURE_IXFILESVC
    /* Start IxFile Server task and wait for response.
    */
    TMC_MSG_MED("Starting IxFile Server task...", 0, 0, 0);
    (void) rex_set_sigs(&ix_filesvc_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_IXFILESVC */

  #ifdef FEATURE_IXSQLSVC
    /* Start IxSQL Server task and wait for response.
    */
    TMC_MSG_MED("Starting IxSQL Server task...", 0, 0, 0);
    (void) rex_set_sigs(&ix_sqlsvc_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

    /* Start IxSQL Client task and wait for response.
    */
    TMC_MSG_MED("Starting IxSQL Client task...", 0, 0, 0);
    (void) rex_set_sigs(&ix_sqlclnt_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_IXSQLSVC */
  #endif /* !FEATURE_SEC_PROCESS_INIT */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_FOTA_FS
  fota_update_init_after_fs_task_started();
  #endif /* FEATURE_FOTA_FS */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_NV
    /* Start Non-Volatile Database task and wait for response.
    */
    TMC_MSG_MED("Starting NV task...", 0, 0, 0);
    (void) rex_set_sigs(&nv_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_NV */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* It has dependency on nv task. so it need to be called after nv task is
  ** started.
  */
  err_init();                                     /* Start error services. */

#ifdef FEATURE_DLOAD_HW_RESET_DETECT
  {
    nv_stat_enum_type     nv_read_status;         /* NV read status */
    boolean               nv_detect_hw_reset;

    nv_read_status = tmcnv_read (NV_DETECT_HW_RESET_I,
                                 (nv_item_type *)&nv_detect_hw_reset);

    /* Disable HW Reset detection if NV item is not active or if NV item
    ** was set to zero.
    */
    if ((nv_read_status == NV_NOTACTIVE_S) ||
        ((nv_read_status == NV_DONE_S) && (nv_detect_hw_reset == FALSE)))
    {
      /* Disable HW Reset detection. */
      boot_hw_reset_shared_info_reset();
    }
  }
  #endif /* FEATURE_DLOAD_HW_RESET_DETECT */

  /* It has dependency on fs task. so it need to be called after fs task is
  ** started.
  */
  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_RESET_COUNTER
  (void) tmc_reset_counter_increment();            /* increase reset counter by 1 */
  #endif
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_UIM_RUIM
  {
    nv_item_type          nv_data_buf;                   /* NV data buffer */
    nv_stat_enum_type     nv_read_status;                /* NV read status */
    /*
    ** ESN-ME could be esn number in stored in NV or a RUIM id.
    ** This function will read ESN-ME from NV,cached to tmc data. The ESN
    ** usage is set to Zero. From that on any entity reads ESN-ME from now on
    ** will get zero until the ESN usage is changed.
    ** When ui task is started, it uses cached ESN-ME to initial RUIM and
    ** reset the ESN usage indicator to NV or RUIM Id based on certain
    ** conditions.After that any entity reads ESN-ME, it will get a number
    ** decided by ESN usage indicator.
    **
    ** Note: ui task is started priori to esn_me cached here. But ui reads
    ** stored esn_me after esn_me is cached right now. If anyhow ui read
    ** stored esn_me before esn_me is cached. We have to move the code before
    ** ui task is started.
    **
    */

    /* Read the ESN ESN from NV */

    nv_read_status = tmcnv_read( NV_ESN_I, &nv_data_buf );

    if (nv_read_status != NV_DONE_S)
      nv_data_buf.esn.esn = 0;

    /* Store esn me to TMC internal data buffer.
    */
    tmc_store_esn_me(nv_data_buf.esn.esn);

    /* Set the esn to return Zero until after store esn-me is finished
    */
    nvruim_set_esn_usage( NV_RUIM_ZERO_ESN );

  }
  #endif /* FEATURE_UIM_RUIM */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */
#ifdef CUST_EDITION
#ifdef FEATURE_UIM_EUIMID
  {
    nv_item_type          nv_data_buf;                   /* NV data buffer */
    nv_stat_enum_type     nv_read_status;                /* NV read status */
    qword                 meid_me;
    
    nv_read_status = tmcnv_read( NV_MEID_ME_I, &nv_data_buf );

    if (nv_read_status != NV_DONE_S)
    {
      qw_equ(meid_me, 0);
    }
    else
    {
      qw_equ(meid_me, nv_data_buf.meid);
    }

    /* Store esn me to TMC internal data buffer.
    */
    tmc_store_meid_me(meid_me, nv_read_status);
  }

#endif
#endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #if (defined ( FEATURE_UIM ) && ! defined(FEATURE_GSM_PLT) )
    /* Start UIM task and wait for response.
    */
    TMC_MSG_MED("Starting UIM task...", 0, 0, 0);
    (void) rex_set_sigs(&uim_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize UIM related NV items - specifically FIRST instruction class */
    uim_nv_init( rex_self(),
                 TMC_NV_CMD_SIG,
                 (void (*)(rex_sigs_type)) tmc_wait);
  #endif
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_CHG_TASK
    /* Start CHG Task and wait for response.
    */
    TMC_MSG_MED("Starting CHG task...", 0, 0, 0);
    (void) rex_set_sigs(&chg_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_CHG_TASK */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  #if defined (FEATURE_USB) && !defined (FEATURE_NO_USB_HW_ON_PROCESSOR)
    /* Start USB task and wait for response.
    */
    TMC_MSG_MED("Starting USBDC task...", 0, 0, 0);
    (void) rex_set_sigs(&usbdc_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

    #ifdef FEATURE_USB_HOST_ENHANCED
      /* Start USBHC_URB task and wait for response.
      */
      TMC_MSG_MED("Starting USBHC_URB task...", 0, 0, 0);
      (void) rex_set_sigs(&usbhc_urb_tcb, TASK_START_SIG);
      (void) tmc_wait(TMC_ACK_SIG);
      (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

      /* Start USBHC_DEV task and wait for response.
      */
      TMC_MSG_MED("Starting USBHC_DEV task...", 0, 0, 0);
      (void) rex_set_sigs(&usbhc_dev_tcb, TASK_START_SIG);
      (void) tmc_wait(TMC_ACK_SIG);
      (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
    #if defined(FEATURE_USB_HOST_TEST_ENHANCED) && \
        (defined(FEATURE_USB_HOST_MASS_STORAGE_TEST_ENHANCED) || \
         defined(FEATURE_USB_HOST_HID_TEST_ENHANCED))
       /* Start USBHC_TEST task and wait for response.
      */
      TMC_MSG_MED("Starting USBHC_TEST task...", 0, 0, 0);
      (void) rex_set_sigs(&usbhc_test_tcb, TASK_START_SIG);
      (void) tmc_wait(TMC_ACK_SIG);
      (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

    #endif

    #endif


  #endif /* FEATURE_USB */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef TMC_SLEEP
    /* Start Sleep task and wait for response.
    */
    TMC_MSG_MED("Starting Sleep task...", 0, 0, 0);
    (void) rex_set_sigs(&sleep_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* TMC_SLEEP */

  /* Load dynamic clock/voltage scaling flags before DSP services are active */
  #if defined (T_MSM6800) || defined(T_QSC1100) || defined(T_QSC1110)
    #if defined (FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING) || defined (FEATURE_CLKREGIM_DYNAMIC_VOLTAGE)
    {
      nv_item_type          nv_data_buf;                   /* NV data buffer */
      nv_stat_enum_type     nv_read_status;                /* NV read status */
      /* NV_DYN_CLOCK_VOLTAGE_I */
      /* Get nv read*/
      nv_read_status = tmcnv_read( NV_DYN_CLOCK_VOLTAGE_I, &nv_data_buf );
      if (nv_read_status == NV_DONE_S) {
        clk_regime_dyn_clock_on = nv_data_buf.dyn_clock_voltage.dyn_clock_on;
        clk_regime_dyn_voltage_on = nv_data_buf.dyn_clock_voltage.dyn_voltage_on;
      }
      else {
        TMC_ERR( "Bad NV read status %d on dynamic clock/voltage",
                 nv_read_status, 0, 0 );
      }
    }
    #endif
  #endif /* T_MSM6800 */

  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

//#ifndef FEATURE_BRINGUP_DISABLE
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifndef FEATURE_SC2X_QDSP_NOT_PORT
  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_QDSP_TASK
    /* Start QDSP Services task and wait for response.
    */
    TMC_MSG_MED("Starting QDSP Services task...", 0, 0, 0);
    (void) rex_set_sigs(&qdsp_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_QDSP_TASK */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */
#endif
//#endif /* FEATURE_BRINGUP_DISABLE */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

 #ifndef FEATURE_BRINGUP_DIAG_ONLY
 #ifdef FEATURE_QDSP_RTOS
    /* Start ADSP RTOS Services task and wait for response.
    */
    TMC_MSG_MED("Starting ADSP RTOS Services task...", 0, 0, 0);
    (void) rex_set_sigs(&adsp_rtos_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_QDSP_RTOS */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_MDSP_TASK
    /* Signal MDSP task and wait for response.
    */
    TMC_MSG_MED( "Starting MDSP task thread", 0, 0, 0 );
    (void) rex_set_sigs(&mdsp_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_MDSP_TASK */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_VOC_TASK
    /* Start Vocoder task and wait for response.
    */
    TMC_MSG_MED("Starting VOC task...", 0, 0, 0);
    (void) rex_set_sigs(&voc_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_VOC_TASK */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_SND_TASK
    /* Start Sound task and wait for response.
    */
    TMC_MSG_MED("Starting SND task...", 0, 0, 0);
    (void) rex_set_sigs(&snd_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_SND_TASK */

  #ifdef FEATURE_AVS_SBCENC_TASK 
    /* Start SBC encoder task and wait for response.
    */
    TMC_MSG_MED("Starting SBC encoder task...", 0, 0, 0);
    (void) rex_set_sigs(&sbcenc_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_AVS_SBCENC_TASK */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_GRAPH_TASK
    /* Start Graphics task and wait for response.
    */
    TMC_MSG_MED("Starting Graphics task...", 0, 0, 0);
    (void) rex_set_sigs(&graph_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_GRAPH_TASK */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_CAMERA_DRV_TASK
    /* Start Camera Driver task and wait for response.
    */
    TMC_MSG_MED("Starting Camera Driver task...", 0, 0, 0);
    (void) rex_set_sigs(&camera_drv_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_CAMERA_DRV_TASK */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_VS_TASK
    /* Start VS task and wait for response.
    */
    TMC_MSG_MED("Starting VOICE SERVICES task...", 0, 0, 0);
    (void) rex_set_sigs(&vs_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_VS_TASK */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_IS2000_TDSO
    /* Start TDSO task and wait for response.
    */
    TMC_MSG_MED("Starting TDSO task...", 0, 0, 0);
    (void) rex_set_sigs(&tdso_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_IS2000_TDSO */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifndef FEATURE_BRINGUP_DIAG_ONLY
#if !defined(FEATURE_THIN_UI) && !defined(FEATURE_UI_CORE_REMOVED)
  #ifdef FEATURE_HS
    /* Start Handset task and wait for response.
    */
    TMC_MSG_MED("Starting HS task...", 0, 0, 0);
    (void) rex_set_sigs(&hs_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_HS */
#endif /* !defined(FEATURE_THIN_UI) && !defined(FEATURE_UI_CORE_REMOVED) */
#endif /* FEATURE_BRINGUP_DIAG_ONLY */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_IDS
#error code not present
  #endif /* FEATURE_IDS */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_DIAG_TASK
    /* Start Diagnostic task and wait for response.
    */
    TMC_MSG_MED("Starting DIAG task...", 0, 0, 0);
    (void) rex_set_sigs(&diag_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_DIAG_TASK */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_IG_QDIAG
  /* Start QDIAG task and wait for response.
  */
  TMC_MSG_MED("Starting QDIAG task...", 0, 0, 0);
  (void) rex_set_sigs(&qdiag_tcb, TASK_START_SIG);
  (void) tmc_wait(TMC_ACK_SIG);
  (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_IG_QDIAG */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_NV_CNV
    /* Start User Interface task and wait for response.
    */
    TMC_MSG_MED("Starting CNV task...", 0, 0, 0);
    (void) rex_set_sigs(&cnv_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_NV_CNV */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_DMTASK
    /* Start User Interface task and wait for response.
    */
    TMC_MSG_MED("Starting DM task...", 0, 0, 0);
    (void) rex_set_sigs(&dmtask_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_DMTASK */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if (defined(FEATURE_DISP_TASK)) && (!defined(T_MSM7500) || \
      (defined(T_MSM7500) && defined(IMAGE_APPS_PROC)))
    /* Start disp task and wait for response.
    */
    TMC_MSG_MED("Starting DISP task...", 0, 0, 0);
    (void) rex_set_sigs(&disp_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_DISP_TASK */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  #ifdef FEATURE_APPS_TCXO_SLEEP
    /* Start DEM Task
    */
    TMC_MSG_MED("Starting DEM task...", 0, 0, 0);
    (void) rex_set_sigs(&dem_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_APPS_TCXO_SLEEP */
 
   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  #ifdef FEATURE_XO
    /* Start TCXOMGR Task
    */
	#ifdef FEATURE_CGPS
#error code not present
	#endif /* FEATURE_CGPS */
  #endif /* FEATURE_XO */
 
   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
#if defined(FEATURE_HS_USB) && (defined(FEATURE_STANDALONE_MODEM) || !defined(FEATURE_NO_USB_HW_ON_PROCESSOR))
    TMC_MSG_MED("Starting HSU AL task...", 0, 0, 0);
    (void) rex_set_sigs(&hsu_al_task_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
#endif /* FEATURE_HS_USB && (FEATURE_STANDALONE_MODEM || !FEATURE_NO_USB_HW_ON_PROCESSOR) */
  
#if defined(FEATURE_HS_USB_MS_FD) && (defined(FEATURE_STANDALONE_MODEM) || !defined(FEATURE_NO_USB_HW_ON_PROCESSOR))
    /* Start HSU mass storage adaptation layer task and wait for response.*/
    TMC_MSG_MED("Starting HSU_AL_MS task...", 0, 0, 0);
    (void) rex_set_sigs(&hsu_al_ms_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
#endif /* FEATURE_HS_USB_MS_FD && (FEATURE_STANDALONE_MODEM || !FEATURE_NO_USB_HW_ON_PROCESSOR) */

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifdef FEATURE_EFS_FDI
    /* Start Embedded Intel FDI File System task and wait for response.
    */
    TMC_MSG_MED("Starting FDI task...", 0, 0, 0);
    (void) rex_set_sigs(&fdi_bg_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_EFS_FDI */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
} /* tmc_start_service_tasks */


/* <EJECT> */
/*===========================================================================

FUNCTION tmc_start_application_tasks()

DESCRIPTION
  This function starts application tasks.It sends START signals to all tasks,
  one at a time. After sending a START signal to a task, it expects an Ack
  back from the task. Once Ack is received, it sends a START Signal to the
  next task. This handshaking ensures a controlled starting of all tasks.


DEPENDENCIES
  All tasks must already be created through tmc_define_tasks().

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
LOCAL   void                       tmc_start_application_tasks( void )
{
    unsigned int i;

    i = 0;             /* initializing i to supress compiler warnings */

  /* Clear ACK signal */

  (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

  #ifndef FEATURE_BRINGUP_DIAG_ONLY

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_WLAN
#error code not present
  #endif /* FEATURE_WLAN */

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if (defined(FEATURE_DATA) && defined(FEATURE_DATA_PS))
    /* Start Data Protocol Services task and wait for response.
    */
    TMC_MSG_MED("Starting PROTOCOL SERVICES task...", 0, 0, 0);
    (void) rex_set_sigs(&ps_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_DATA && FEATURE_DATA_PS */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Warning: WLAN SEC has to be started after PS task. */

  #ifdef FEATURE_WLAN
#error code not present
  #endif /* FEATURE_WLAN */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifdef FEATURE_IPC_TEST
#error code not present
  #endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_SEC_PROCESS_INIT
  #ifdef FEATURE_IXHTTPSVC
    /* Start IxHTTP Server task and wait for response.
    */
    TMC_MSG_MED("Starting IxHTTP Server task...", 0, 0, 0);
    (void) rex_set_sigs(&ix_httpsvc_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_IXHTTPSVC */

  #ifdef FEATURE_IXTIMESVC
    /* Start IxTIME Server task and wait for response.
    */
    TMC_MSG_MED("Starting IxTIME Server task...", 0, 0, 0);
    (void) rex_set_sigs(&ix_timesvc_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_IXTIMESVC */

  #ifdef FEATURE_IXCONTENTSVC
        /* Start IxContent Client Handler task and wait for response.
    */
    TMC_MSG_MED("Starting IxContent Clnt Handler task...", 0, 0, 0);
    (void) rex_set_sigs(&ix_contentclnt_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
    #endif /* FEATURE_IXCONTENTSVC */
  
  #endif /* !FEATURE_SEC_PROCESS_INIT */

  #ifdef FEATURE_IXCONTENTSVC
    /* Start IxContent Server task and wait for response.
    */
    TMC_MSG_MED("Starting IxContent Server task...", 0, 0, 0);
    (void) rex_set_sigs(&ix_contentsvc_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_IXCONTENTSVC */

  #ifdef FEATURE_IXDESCRAMBLERSVC
    /* Start Descrambler Service task and wait for response.
    */
    TMC_MSG_MED("Starting Descrambler tasks...", 0, 0, 0);
    (void) rex_set_sigs(&ix_descramblersvc_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif

  #ifdef FEATURE_SEC

    #ifndef FEATURE_SEC_PROCESS_INIT

    /* Start Security Services task and wait for response.
    */
    TMC_MSG_MED("Starting SEC task...", 0, 0, 0);
    (void) rex_set_sigs(&sec_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

  #ifdef FEATURE_SEC_ARM_CRYPTO
    /* Start Security Services ARM task and wait for response.
    */
    TMC_MSG_MED( "Starting SECCRYPTARM task thread", 0, 0, 0 );
    (void) rex_set_sigs( &seccryptarm_tcb, TASK_START_SIG );
    (void) tmc_wait( TMC_ACK_SIG );
    (void) rex_clr_sigs( rex_self(), TMC_ACK_SIG );
  #endif /* FEATURE_SEC_ARM_CRYPTO */

      #ifdef FEATURE_SEC_CONTENT_AGENT
        /* Start Content Agent Diag task and wait for response.
        */
        TMC_MSG_MED( "Starting ContentAgentDiag task thread", 0, 0, 0 );
        (void) rex_set_sigs( &contentagentdiag_tcb, TASK_START_SIG );
        (void) tmc_wait( TMC_ACK_SIG );
        (void) rex_clr_sigs( rex_self(), TMC_ACK_SIG );
      #endif /* FEATURE_SEC_CONTENT_AGENT */

  #endif /* !FEATURE_SEC_PROCESS_INIT */

  #ifdef FEATURE_SECRND
    /* Start Security Services RND task and wait for response.
    */
    TMC_MSG_MED("Starting SECRND task...", 0, 0, 0);
    (void) rex_set_sigs(&secrnd_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_SECRND */

  #ifdef FEATURE_SEC_USFS
    /* Start Security Services RND task and wait for response.
    */
    TMC_MSG_MED("Starting sfs task...", 0, 0, 0);
    (void) rex_set_sigs(&sfs_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_SEC_USFS */

  #ifdef FEATURE_SEC_IPSEC
    /* Start IPSec IKE task and wait for response.
    */
    TMC_MSG_MED( "Starting SECIPS task thread", 0, 0, 0 );
    (void) rex_set_sigs( &secips_tcb, TASK_START_SIG );
    (void) tmc_wait( TMC_ACK_SIG );
    (void) rex_clr_sigs( rex_self(), TMC_ACK_SIG );
  #endif /* FEATURE_SEC_IPSEC */

  #ifdef FEATURE_SEC_SSL
    /* Start Security Services SSL task and wait for response.
    */
    TMC_MSG_MED("Starting SSSL task...", 0, 0, 0);
    (void) rex_set_sigs(&secssl_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_SEC_SSL */

#ifndef T_RUMI
  #ifdef FEATURE_SEC_DRM
#error code not present
  #endif /* FEATURE_SEC_DRM */
#endif /* ! T_RUMI */


  #endif /* FEATURE_SEC */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_CM
    /* Start Call Manager task and wait for response.
    */
    TMC_MSG_MED("Starting CM task...", 0, 0, 0);
    (void) rex_set_sigs(&cm_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_CM */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_MULTIMODE_ARCH
    /* Start Multi Mode Controller task and wait for response.
    */
    TMC_MSG_MED("Starting MMOC task...", 0, 0, 0);
    (void) rex_set_sigs(&mmoc_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_MULTIMODE_ARCH */

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef TMC_1X
    /* Start MC task and wait for response.
    */
    TMC_MSG_MED("Starting MC task...", 0, 0, 0);
    (void) rex_set_sigs(&mc_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* TMC_1X */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifndef SIRIUS_PLATFORM
  #ifndef T_QUASAR
  #ifdef FEATURE_UI
  #ifdef FEATURE_IG_UI_CLIENT_SELECT
  if( !hw_ig_ui_client_is_wombat() )
    /* Wombat (Linux) is running the keypad and display */
  #endif /* FEATURE_IG_UI_CLIENT_SELECT */
  {
    /* Start User Interface task and wait for response.
    */
    TMC_MSG_MED("Starting UI task...", 0, 0, 0);
    (void) rex_set_sigs(&ui_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  }
  #endif /* FEATURE_UI */
  #endif /* T_QUASAR */
  #endif /* SIRIUS_PLATFORM */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if ( defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS) )
    /* Signal WMS task and wait for response.
    */
    TMC_MSG_MED( "Starting WMS task thread", 0, 0, 0 );
    (void) rex_set_sigs(&wms_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_CDSMS || FEATURE_GWSMS */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef T_QUASAR
  #if (defined(FEATURE_GPSONE) && !defined(FEATURE_CGPS))
#error code not present
  #endif /* FEATURE_GPSONE && !FEATURE_CGPS */
  #endif /* !T_QUASAR */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef T_QUASAR
  #ifdef FEATURE_BT
    /* Signal BT task and wait for response.
    */
    TMC_MSG_MED( "Starting BT task...", 0, 0, 0 );
    (void) rex_set_sigs( &bt_tcb, TASK_START_SIG );
    (void) tmc_wait( TMC_ACK_SIG );
    (void) rex_clr_sigs( rex_self(), TMC_ACK_SIG );

    #ifdef FEATURE_BT_EXTPF
      /* Signal BT external profile task and wait for response.
      */
      TMC_MSG_MED( "Starting BT external profile task...", 0, 0, 0 );
      (void) rex_set_sigs( &bt_pf_tcb, TASK_START_SIG );
      (void) tmc_wait( TMC_ACK_SIG );
      (void) rex_clr_sigs( rex_self(), TMC_ACK_SIG );
    #endif /* FEATURE_BT_EXTPF */
  #endif /* FEATURE_BT */
  #endif /* T_QUASAR */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef TMC_1X
    /* Start Transmit task and wait for response.
    */
    TMC_MSG_MED("Starting TX task...", 0, 0, 0);
    (void) rex_set_sigs(&tx_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

    /* Start Receive task and wait for response.
    */
    TMC_MSG_MED("Starting RX task...", 0, 0, 0);
    (void) rex_set_sigs(&rx_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

    /* Start Searcher task and wait for response.
    */
    TMC_MSG_MED("Starting SRCH task...", 0, 0, 0);
    (void) rex_set_sigs(&srch_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

    /* Start Layer 2 task and wait for response.
    */
    TMC_MSG_MED("Starting RXTX task...", 0, 0, 0);
    (void) rex_set_sigs(&rxtx_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* TMC_1X */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_HDR
#error code not present
  #endif /* FEATURE_HDR */


  #ifdef FEATURE_HDR_BCMCS
#error code not present
  #endif /* FEATURE_HDR_BCMCS */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_VBATT_TASK
    /* Start VBATT Task and wait for response.
    */
    TMC_MSG_MED("Starting VBATT task...", 0, 0, 0);
    (void) rex_set_sigs(&vbatt_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif
  #endif
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef T_QUASAR
  #ifdef FEATURE_QMIP
#error code not present
  #endif /* FEATURE_QMIP */
  #endif /* T_QUASAR */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_DATA

  #ifndef T_QUASAR
    /* Start Data Services task and wait for response.
    */
    TMC_MSG_MED("Starting DATA SERVICES task...", 0, 0, 0);
    (void) rex_set_sigs(&ds_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

    #ifdef FEATURE_COMP_TASK
    /* Start Data Services task and wait for response.
    */
    TMC_MSG_MED("Starting COMP task...", 0, 0, 0);
    (void) rex_set_sigs(&comp_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
    #endif /* FEATURE_COMP_TASK */
    
    #ifdef FEATURE_WCDMA /* ) && ( defined FEATURE_DATA_WCDMA_CS ) ) */
#error code not present
    #endif /* FEATURE_WCDMA */

    #if ( (defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS)) && !defined(FEATURE_WCDMA_PLT) && !defined(FEATURE_GSM_PLT))
      TMC_MSG_MED( "Starting GCSD task thread", 0, 0, 0 );
      (void) rex_set_sigs(&Gcsd_tcb, TASK_START_SIG);
      (void) tmc_wait(TMC_ACK_SIG);
      (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
    #endif /* (FEATURE_DATA_GCSD || FEATURE_DATA_WCDMA_CS) && !FEATURE_WCDMA_PLT */
  #endif /* T_QUASAR */

  #endif /* FEATURE_DATA */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_DS_SOCKETS_TEST_TCP
#error code not present
  #endif /* FEATURE)DS_SOCKETS_TEST_TCP */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_DS_SOCKETS_TEST_UDP
#error code not present
  #endif /* FEATURE_DS_SOCKETS_TEST_UDP */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if defined( FEATURE_AUTH ) || defined ( FEATURE_RUIM )
    /* Start Authentication task and wait for response.
    */
    TMC_MSG_MED("Starting AUTHENTICATION task...", 0, 0, 0);
    (void) rex_set_sigs(&auth_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_AUTH || FEATURE_RUIM */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifndef T_QUASAR
  #ifdef FEATURE_DH
    /* Start DH task and wait for response.
    */
    TMC_MSG_MED("Starting DH task...", 0, 0, 0);
    (void) rex_set_sigs(&dh_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_DH */
  #endif /* T_QUASAR */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_MMGSDI
    /* Signal GSDI task and wait for response.
    */
    TMC_MSG_MED("Starting GSDI task...", 0, 0, 0);
    (void) rex_set_sigs(&gsdi_task_tcb, TASK_START_SIG);
    (void)tmc_wait( TMC_ACK_SIG );
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_MMGSDI */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef SIRIUS_PLATFORM
#error code not present
  #endif /* SIRIUS_PLATFORM */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_VIRTUAL_SIM
    /* Signal GSDI UIM Server task and wait for response.
    */
    TMC_MSG_MED( "Starting GSDI UIM Server task thread", 0, 0, 0 );
    (void) rex_set_sigs(&gsdi_uim_server_task_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if defined(FEATURE_GSTK)
    /* Start GSTK Task and wait for response.
    */
    TMC_MSG_LOW( "Starting GSTK task thread", 0, 0, 0 );
    (void) rex_set_sigs(&gstk_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_GSTK */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if defined ( FEATURE_VIRTUAL_SAT ) && defined ( FEATURE_GSTK )
      /* ---------- Start SAT Test Server Task ------------ */
    TMC_MSG_LOW( "Starting GSDI SAT Server task thread", 0, 0, 0 );
    (void) rex_set_sigs(&virtsat_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_VIRTUAL_SAT && FEATURE_GSIK */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_WCDMA
#error code not present
  #endif /* FEATURE_WCDMA */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if (defined(FEATURE_GSM) && !defined(FEATURE_WCDMA_PLT) && !defined(FEATURE_GSM_PLT) ) 
#error code not present
  #endif /* FEATURE_GSM && !FEATURE_WCDMA_PLT */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_NAS
#error code not present
  #endif /* FEATURE_NAS */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_TC
    /* Signal TC task and wait for response.
    */
    TMC_MSG_MED( "Starting TC task thread", 0, 0, 0 );
    (void) rex_set_sigs(&tc_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_TC */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if ( defined(FEATURE_WCDMA_VST_FULLSTACK) || defined(FEATURE_WCDMA_VST_AS))
    /* #ifdef FEATURE_MENU_UT */
    /* Signal MENU unit test task and wait for response.
    */
    TMC_MSG_LOW( "Starting MENU unit test task thread", 0, 0, 0 );
    (void) rex_set_sigs(&menu_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_MMGPS
#error code not present
  #endif /* FEATURE_MMGPS */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_MGP

    #ifdef FEATURE_MGPCC_TASK
    /* Signal MGPCC task and wait for response.
    */
    TMC_MSG_MED( "Starting MGPCC task thread", 0, 0, 0 );
    (void) rex_set_sigs( &mgpcc_tcb, TASK_START_SIG );
    (void) tmc_wait( TMC_ACK_SIG );
    (void) rex_clr_sigs( rex_self(), TMC_ACK_SIG );
    #endif

    /* Signal MGPMC task and wait for response.
    */
    TMC_MSG_MED( "Starting MGPMC task thread", 0, 0, 0 );
    (void) rex_set_sigs( &mgpmc_tcb, TASK_START_SIG );
    (void) tmc_wait( TMC_ACK_SIG );
    (void) rex_clr_sigs( rex_self(), TMC_ACK_SIG );

    /* Signal MGPNF task and wait for response.
    */
    TMC_MSG_MED( "Starting MGPNF task thread", 0, 0, 0 );
    (void) rex_set_sigs( &mgpnf_tcb, TASK_START_SIG );
    (void) tmc_wait( TMC_ACK_SIG );
    (void) rex_clr_sigs( rex_self(), TMC_ACK_SIG );

    /* Signal MGPCD task and wait for response.
    */
    TMC_MSG_MED( "Starting MGPCD task thread", 0, 0, 0 );
    (void) rex_set_sigs( &mgpcd_tcb, TASK_START_SIG );
    (void) tmc_wait( TMC_ACK_SIG );
    (void) rex_clr_sigs( rex_self(), TMC_ACK_SIG );

    /* Signal MGPAI task and wait for response.
    */
    TMC_MSG_MED( "Starting MGPAI task thread", 0, 0, 0 );
    (void) rex_set_sigs( &mgpai_tcb, TASK_START_SIG );
    (void) tmc_wait( TMC_ACK_SIG );
    (void) rex_clr_sigs( rex_self(), TMC_ACK_SIG );

  #endif /* FEATURE_MGP */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_CGPS
#error code not present
  #endif /* FEATURE_CGPS */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if ( defined(FEATURE_WCDMA_VST_FULLSTACK) || defined(FEATURE_WCDMA_VST_AS))
    /* Signal AL1 task and wait for response.
    */
    TMC_MSG_MED( "Starting AL1 task thread", 0, 0, 0 );
    (void) rex_set_sigs(&al1_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_WCDMA_VST_AS
#error code not present
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_FTM_TASK
    /* Start the FTM task and wait for response.
    */
    TMC_MSG_MED("Starting FTM task...", 0, 0, 0);
    (void) rex_set_sigs(&ftm_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_FTM_TASK */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if ((defined(FEATURE_QVPHONE)) && (!defined(T_MSM7500)) && (!defined(T_MSM7200)))
    TMC_MSG_MED("Starting  QVPHONE audio engine task...", 0, 0, 0);
    (void) rex_set_sigs(&qvp_ae_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

    TMC_MSG_MED("Starting  QVPHONE mpeg4 decoder task...", 0, 0, 0);
    (void) rex_set_sigs(&qvp_mpeg4_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

    #ifdef FEATURE_QVP_APP_INCLUDE
#error code not present
    #endif

    TMC_MSG_MED("Starting  QVPIO task...", 0, 0, 0);
    (void) rex_set_sigs(&qvpio_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

    TMC_MSG_MED("Starting  QVPPL task...", 0, 0, 0);
    (void) rex_set_sigs(&qvppl_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

    TMC_MSG_MED("Starting  QVP VFE task...", 0, 0, 0);
    (void) rex_set_sigs(&qvp_vfe_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

  #ifdef FEATURE_QVPHONE_SIP
#error code not present
  #endif

  #endif /*FEATURE_QVPHONE*/

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifdef FEATURE_QIPCALL
  #ifndef FEATURE_MULTIPROCESSOR
    /* Start QIPCALL task and wait for response.
    */
    TMC_MSG_MED("Starting QIPCALL task...", 0, 0, 0);
    (void) rex_set_sigs(&qipcall_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif
  #endif /* FEATURE_QIPCALL */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_QDJTEST
    /* Start QDJTEST task and wait for response.
    */
    TMC_MSG_MED("Starting QDJTEST task...", 0, 0, 0);
    (void) rex_set_sigs(&qdjtest_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_QDJTEST */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_BCMCS
#error code not present
  #endif /* FEATURE_BCMCS */

  #ifdef FEATURE_DTV
#error code not present
  #endif /* FEATURE_DTV */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_JOYST_TASK
    /* Start Joystick task and wait for response.
    */
    TMC_MSG_MED("Starting Joystick task...", 0, 0, 0);
    (void) rex_set_sigs(&joyst_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_JOYST_TASK */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if defined(FEATURE_PBM_TASK)
    /* Start PBM Task and wait for response.
    */
    TMC_MSG_LOW( "Starting PBM task thread", 0, 0, 0 );
    (void) rex_set_sigs(&pbm_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_PBM_TASK */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  #if defined(FEATURE_CPU_BASED_FLOW_CONTROL)
    /* Start FC Task and wait for response.
    */
    TMC_MSG_LOW( "Starting FC task thread", 0, 0, 0 );
    (void) rex_set_sigs(&fc_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_CPU_BASED_FLOW_CONTROL */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef SIRIUS_PLATFORM
#error code not present
#endif /* SIRIUS_PLATFORM */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_FM
    TMC_MSG_MED("Starting FMRDS task...", 0, 0, 0);
    (void) rex_set_sigs(&fmrds_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif // FEATURE_FM

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_PM_SERVER
  /* Start PM Server task and wait for response.
  */
  TMC_MSG_MED("Starting PM Server task...", 0, 0, 0);
  (void) rex_set_sigs(&pms_tcb, TASK_START_SIG);
  (void) tmc_wait(TMC_ACK_SIG);
  (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_PM_SERVER */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_APS
  /* Start APS task and wait for response.
  */
  TMC_MSG_MED("Starting APS task...", 0, 0, 0);
  (void) rex_set_sigs(&aps_tcb, TASK_START_SIG);
  (void) tmc_wait(TMC_ACK_SIG);
  (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_APS */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_ATS
  /* Start ATS task and wait for response.
  */
  TMC_MSG_MED("Starting ATS task...", 0, 0, 0);
  (void) rex_set_sigs(&ats_tcb, TASK_START_SIG);
  (void) tmc_wait(TMC_ACK_SIG);
  (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_ATS */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_GSM_PLT
#error code not present
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_DOG
    /* Start Watchdog task and wait for response.
    */
    TMC_MSG_MED("Starting DOG task...", 0, 0, 0);
    (void) rex_set_sigs(&dog_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_DOG */

  /* Loads the pages from NAND in the background.
  ** It exist only during boot up and once the pages are loaded, it suspends.
  ** This task could be started any time. it is put as the last task to
  ** eliminates any time wasted in background loading during boot.
  ** Ideally the background loading should be start after the initialization
  ** is over. But that is not a hard requirement.
  */
  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #if defined(FEATURE_PROGRESSIVE_BOOT) && !defined(FEATURE_TIMER_TASK) && !defined(FEATURE_L4)
  /* Create and Start background loading task */
    #ifdef TIMETEST
      background_loader_tcb.leds = TIMETEST_BCKGROUND_LOADER_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < BACKGROUND_LOADER_STACK_SIZ; i++) {
        background_loader_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) std_strlcpy(background_loader_tcb.task_name, "LOADR" , sizeof(background_loader_tcb.task_name));

    /* Create background loading task and start it.
    */
    TMC_MSG_MED("Creating and Starting BCKGROUND LOADER task", 0, 0, 0);
    rex_def_task(&background_loader_tcb,
                 (rex_stack_word_type*) background_loader_stack,
                 BACKGROUND_LOADER_STACK_SIZ,
                 (rex_priority_type) BACKGROUND_LOADER_PRI,
                 background_loader_task,
                 0L);
  #endif /* FEATURE_PROGRESSIVE_BOOT */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_MFLO
#error code not present
  #endif/* FEATURE_MFLO */

  #ifdef FEATURE_UBM_L1
  
  TMC_MSG_MED("Starting UBM L1 task...", 0, 0, 0);
  (void) rex_set_sigs(&ubm_l1_tcb, TASK_START_SIG);
  (void) tmc_wait(TMC_ACK_SIG);
  (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

  #endif /* FEATURE_UBM_L1 */

  #ifdef FEATURE_QTV_STATIC_TASKS
  
  TMC_MSG_MED("Starting QTV video renderer task...", 0, 0, 0);
  (void) rex_set_sigs(&qtv_video_renderer_tcb, TASK_START_SIG);
  (void) tmc_wait(TMC_ACK_SIG);
  (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

  TMC_MSG_MED("Starting QTV audio task...", 0, 0, 0);
  (void) rex_set_sigs(&qtv_audio_tcb, TASK_START_SIG);
  (void) tmc_wait(TMC_ACK_SIG);
  (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

  #endif /* FEATURE_QTV_STATIC_TASKS */

#ifdef FEATURE_QTV_STATIC_DLAGENT_TASK
  TMC_MSG_MED("Starting QTV DLAGENTTask...", 0, 0, 0);
  (void) rex_set_sigs(&qtv_dec_dlAgenttask_tcb, TASK_START_SIG);
  (void) tmc_wait(TMC_ACK_SIG);
  (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
#endif

#ifdef FEATURE_QTV_STATIC_DLDSPSVC_TASK
  TMC_MSG_MED("Starting QTV DLDSP SVC Task...", 0, 0, 0);
  (void) rex_set_sigs(&qtv_dec_dlDspSvctask_tcb, TASK_START_SIG);
  (void) tmc_wait(TMC_ACK_SIG);
  (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);


  #endif /* FEATURE_QTV_STATIC_DLDSPSVC_TASK */

} /* tmc_start_application_tasks */


/* <EJECT> */
/*===========================================================================

FUNCTION tmc_libraries_init_after_tasks_start

DESCRIPTION
  Initialize on behalf of runtime device mapper

DEPENDENCIES
  It has be called after tmc_libraries_init_before_tasks_start and
  tmc_start_tasks.

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
LOCAL   void                       tmc_libraries_init_after_tasks_start(void)
{

  #if defined(FEATURE_RUNTIME_DEVMAP) && defined(FEATURE_EFS) && !defined (T_MSM7500)
    /* Get any previous port mappings on behalf of the Runtime Device Mapper
    */
    rdm_get_prev_ports();
  #endif

  #if defined(FEATURE_RUNTIME_DEVMAP) && !defined (T_MSM7500)
    /* Have RDM issue any open commands. If there were none retrieved from
    ** EFS, compile time defaults will be issued
    */
    rdm_issue_opens();
  #endif

  #ifdef TMC_SET_RAN_SEED
  #ifdef T_MSM7500
#error code not present
  #else
    ran_init( rex_self(),
              TMC_NV_CMD_SIG,
              (void (*)( rex_sigs_type )) tmc_wait );
  #endif
  #endif /* TMC_SET_RAN_SEED */

  #ifdef FEATURE_AUTOMATIC_CLOCK_FREQUENCY_CHANGING
    clk_switch_enable();
  #endif /* FEATURE_AUTOMATIC_CLOCK_FREQUENCY_CHANGING */

  #ifdef FEATURE_MP4_DECODER
  /* MP4 - okay to sleep. We do this here because MP4 task init is not
  ** carried out until a media clip is played.
  */
  SLEEP_ASSERT_OKTS( SLEEP_MP4_OKTS_SIG );
  #endif /* FEATURE_MP4_DECODER */


} /* tmc_libraries_init_after_tasks_start */


/* <EJECT> */
/*===========================================================================

FUNCTION tmc_init

DESCRIPTION
  Initialize Task Main Controller - Called immediately after rex_init is
  called. It does DMSS none protocol specific power up initialization,
  queue initialization, defines timers, defines tasks and starts tasks.

  Initialization sequence:
  1. Set state of TMC task.
  2. Initial timer and command queue of TMC task.
  3. Initialize before any tasks start.
  4. Create tasks.
  5. Start service tasks
  6. Continue on initialization after serice tasks start.
  7. Start application tasks
  8. Continue on initialization after all tasks start.
  9. Update state of TMC task.
  10.if initializaion is fail, send offline cmda command to CM.


DEPENDENCIES
  It has be called before any other functions is used.

RETURN VALUE
  void

SIDE EFFECTS
  Called is stopped until it finishes.

===========================================================================*/
LOCAL   void                       tmc_init( void )
{
  boolean status;                        /* Keep record of initial status. */

#if defined(FEATURE_CACHE_WT)

uint8 cache_wt_enable;
nv_stat_enum_type    nv_read_status;

#endif

/* The following addresses are locked down in the TLB to avoid a TLB miss
   during SDRAMC stall and TCXO shutdown. Placing these addresses in the TLB
   will prevent access to the SDRAM during SDRAMC stall and TCXO shutdown when
   the SDRAM is not accessible.
   Any access to the SDRAM when the SDRAM is in self refresh will cause a
   RESET if WDOG is enabled and will hang if WDOG is not enabled.

   IMPORTANT NOTE:
   The following addresses used for the TLB lock down should not be modified
   under any circumstances. Modification to these addresses will cause 
   unpredictable behavior
*/

  
#if defined( FEATURE_L4)
#error code not present
#endif
  /* Disable intlock profiling during startup */
  REX_STAT_DISABLE();

#if defined( FEATURE_L4)
#error code not present
#endif
#if defined (TIMETEST) && defined (FEATURE_L4)
#error code not present
#endif

#ifdef FEATURE_DISABLE_MISALIGNED_ACCESS_CHECKING
  /* Disable processor alignment exceptions */
  L4_MSM_EXCEPTION_ALIGN_ENABLE(FALSE);
#elif defined (FEATURE_ENABLE_MISALIGNED_ACCESS_HANDLING)
   /* enable misaligned access handling */
    L4_MSM_EXCEPTION_FIX_MISALIGN(TRUE);
#endif

  /* Reset TMC data.
  */
  tmc_data_reset();

  /* Initialize queues and timers.
  */
  tmc_module_init();

#ifdef FEATURE_L4
#error code not present
#endif

  /* Initialize libraries before task start
  */
  tmc_libraries_init_before_tasks_start();

#ifdef FEATURE_L4
#error code not present
#endif

  /* Create tasks
  */
  tmc_define_tasks();

  /* Initialize libraries before service tasks start
   */
  tmc_libraries_init_before_service_tasks_start();

  /* Start service tasks. After nv task is started, read ESN ME number from
  ** NV and stored to tmc data buffer.
  */
  tmc_start_service_tasks();

#if defined(FEATURE_CACHE_WT)

nv_read_status = tmcnv_read(NV_CACHE_WT_I, (nv_item_type *) &cache_wt_enable);


       if (nv_read_status == NV_DONE_S && (cache_wt_enable == 1))
       {
       	cacheWT = 1;
		boot_cache_mmu_init();
       }  
#endif


  /* Initialize libraries after service tasks start.
  */
  status = tmc_libraries_init_after_service_tasks_start();

  /* Initialize the QIDS RPC application
  */
#ifdef FEATURE_L4LINUX
#error code not present
#endif

#ifdef T_QUASAR
#error code not present
#endif
#ifdef FEATURE_BRINGUP_NO_SLEEP
  bio_set_sw(0x100);  /* Force to not sleep for Rumi */
#endif /* FEATURE_BRINGUP_NO_SLEEP */

  if (status == FALSE) {

   /* Set TMC State to OFFLINE.
   */
   tmc_set_state( TMC_OFFLINE_IF_NOT_FTM);


   #ifdef FEATURE_CM

   /* Send offline digital cmd to CM.Need to update with different offline
   ** cmd once the interface is defined.
   */
   (void) cm_ph_cmd_oprt_mode(NULL,
                              NULL,
                              CM_CLIENT_ID_ANONYMOUS,
                              SYS_OPRT_MODE_OFFLINE_IF_NOT_FTM
                              );

   #endif /* FEAURE_CM */
  }
  else {
    /* Set TMC State to ONLINE.
    */
    tmc_set_state( TMC_ONLINE);

    /* TMC does not send online command to CM. Online command is send by MMOC
    ** after MMOC is started.
    */

  }

  /* Start application tasks.
  */
  tmc_start_application_tasks();

  /* Initialize libraries after all tasks start.
  */
  tmc_libraries_init_after_tasks_start();


  /* Inidicate that tmc initializaiton is done. */
  tmc_set_init_state( TRUE );

  /* Enable intlock profiling */
  REX_STAT_ENABLE();

} /* tmc_init */


/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
=================================== TMC CMD =================================
=============================================================================
=============================================================================
===========================================================================*/

/*===========================================================================

FUNCTION tmc_cmd

DESCRIPTION
  This function is called by other tasks to queue a command and send a
  command signal to tmc task.Then function returns to the caller. This does
  not mean that the command has been executed yet, just that it is waiting to
  be executed.

DEPENDENCIES
  Free command queue and command queue have be initialized with tmc_init.

RETURN VALUE
  boolean  If command queue is full, FALSE is returned.

SIDE EFFECTS
  None

===========================================================================*/
EXTERN  boolean                    tmc_cmd(
        tmc_cmd_e_type             cmd
)
{
  tmc_cmd_type  *cmd_ptr;

  /* Dequeue a command buffer from the free command queue.
  ** Point done_q_ptr to the free command queue. Set task pointer and signal
  ** to NULL for now.
  **
  ** The done_q_ptr is for free command buffer back to free command queue.
  ** If the task pointer is non-NULL, signal the task that the tone is done.
  */
  cmd_ptr = (tmc_cmd_type *) q_get(&tmc_free_cmd_q);

  TMC_ASSERT( cmd_ptr != NULL );

  if (cmd_ptr == NULL) {
    return FALSE;
  }

  cmd_ptr->hdr.done_q_ptr = &tmc_free_cmd_q;
  cmd_ptr->hdr.task_ptr = NULL;
  cmd_ptr->cmd_id = cmd;

  /* Queue the command. */
  q_put(&tmc_cmd_q, &cmd_ptr->hdr.link);

  /* Set the command signal to notify TMC */
  (void) rex_set_sigs(&tmc_tcb, TMC_CMD_Q_SIG);

  return TRUE;
} /* tmc_cmd */


/* <EJECT> */
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /* FEATURE_GSM || FEATURE_WCDMA */


/* <EJECT> */
/*===========================================================================

FUNCTION  TMC_GET_STATE

DESCRIPTION

  This function returns the current TMC state

DEPENDENCIES

  None.

RETURN VALUE

  Current TMC state.

SIDE EFFECTS

  None.

===========================================================================*/
EXTERN  tmc_state_e_type           tmc_get_state( void )
{
  return tmc_data.state;
}


/* <EJECT> */
#ifdef FEATURE_UIM_RUIM
/*===========================================================================

FUNCTION tmc_get_stored_esn_me

DESCRIPTION
  This function returns the stored esn_me .


DEPENDENCIES
  None

RETURN
  dword

SIDE EFFECTS
  None

===========================================================================*/
EXTERN  dword                      tmc_get_stored_esn_me (void)
{
  return (tmc_data.esn_me);
} /* tmc_get_stored_esn_me */
#endif /* FEATURE_UIM_RUIM */


/* <EJECT> */
/*===========================================================================

FUNCTION  TMC_IS_INIT_DONE

DESCRIPTION

  This function returns if the tmc initialization is done.

DEPENDENCIES

  None.

RETURN VALUE
  TRUE if the tmc initialization is done.Otherwise return FALSE.


SIDE EFFECTS

  None.

===========================================================================*/
EXTERN  boolean           tmc_is_init_done( void )
{
  return tmc_data.is_init_done;
}


/* <EJECT> */
#ifdef FEATURE_REX_DYNA_MEM
/*===========================================================================

FUNCTION tmc_get_mem_heap_size

DESCRIPTION
  This function returns the size of requested memory heap.


DEPENDENCIES
  None

RETURN
  dword

SIDE EFFECTS
  None

===========================================================================*/

EXTERN  dword                      tmc_get_mem_heap_size (
      tmc_mem_heap_e_type p_type)
{
  dword heap_size = 0;

  switch (p_type) {
    case TMC_MEM_HEAP_TYPE_THRD:
      heap_size = MEM_HEAP_THRESHOLD;
      break;
    case TMC_MEM_HEAP_TYPE_BUF:
      heap_size = TMC_HEAP_MEM_BUFFER_SIZE;
      break;
      
    #ifndef FEATURE_LOW_MEMORY_USAGE
      case TMC_MEM_HEAP_TYPE_SMALL_BUF:
        heap_size = TMC_HEAP_SMALL_MEM_BUFFER_SIZE;
        break;
    #endif
    default:
      /* invalid command mem heap enum type.*/
      TMC_ERR("A invalid mem heap enum type ",0,0,0);
      break;
  }
  return heap_size;
}
#endif /* FEATURE_REX_DYNA_MEM */


/* <EJECT> */
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /* FEATURE_GSM || FEATURE_WCDMA */


/* <EJECT> */
/*===========================================================================

FUNCTION tmc_task_start

DESCRIPTION
  This function is called by each task when it is created.  The function
  then performs the startup processing for the calling task.  The function
  first sets the acknowledge signal back to TMC to acknowledge that is had
  been created.  It then waits for the start signal back to it.  While
  waiting for the start signal and keeps kicking the watchdog timer
  everytime its interval timer has expired.  When it gets the start
  signal it acknowledges it back to TMC and it then returns to the caller.
  The caller task is now formally started and it can start normal application
  processing.


DEPENDENCIES
  TMC task has to be started.

RETURN VALUE
  void

SIDE EFFECTS
  The calling task stays here until it receives the START signal.

===========================================================================*/
EXTERN  void                       tmc_task_start( void )
{
  /* Acknowledge task creation.
  */
  (void) rex_set_sigs( &tmc_tcb, TMC_ACK_SIG );

  /* Wait for the start signal
  */
  (void) rex_wait( TASK_START_SIG );

  /* Clear task start signal.
  */
  (void) rex_clr_sigs(rex_self(), TASK_START_SIG);

  /* If start signal is set then signal back to TMC and return to caller.
  */
  (void) rex_set_sigs(&tmc_tcb, TMC_ACK_SIG);

} /* tmc_task_start*/


/* <EJECT> */
/*===========================================================================

FUNCTION tmc_task_ack

DESCRIPTION
  This function sends acknowledge signal to Task Main Control task by caller

DEPENDENCIES
  Task Main Control task is running.

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/
EXTERN  void                       tmc_task_ack( void )
{
  tmc_set_ack_task(rex_self());

  (void) rex_set_sigs(&tmc_tcb, TMC_ACK_SIG);
} /* tmc_ack */


/* <EJECT> */
/*===========================================================================

FUNCTION tmc_task_stop

DESCRIPTION
  This function is called by each task when it stops.  The function then
  performs the stop processing for the calling task.  The function
  sets the acknowledge signal back to TMC to acknowledge that is had
  been stopped.

DEPENDENCIES
  Task Main Control task is running.

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/
EXTERN  void                       tmc_task_stop( void )
{
  tmc_set_ack_task(rex_self());

  (void) rex_set_sigs(&tmc_tcb, TMC_ACK_SIG);
} /* tmc_task_stop */


/* <EJECT> */
/*===========================================================================

FUNCTION tmc_task_offline

DESCRIPTION
  This function is called by each task when it goes offline.  The function
  then performs the offline processing for the calling task.  The function
  sets the acknowledge signal back to TMC to acknowledge that is had
  been offline.

DEPENDENCIES
  Task Main Control task is running.

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/
EXTERN  void                       tmc_task_offline( void )
{
  (void) rex_set_sigs(&tmc_tcb, TMC_ACK_SIG);
} /* tmc_task_offline */


#ifdef FEATURE_RESET_COUNTER
/* <EJECT> */
/*===========================================================================

FUNCTION tmc_reset_counter_read

DESCRIPTION
  Read reset counter from flash memeory.

DEPENDENCIES
  FS task has already been started.

RETURN VALUE
  TRUE read operation succeed.
  FALSE read operation fail.

SIDE EFFECTS
  None

===========================================================================*/
EXTERN  boolean                    tmc_reset_counter_read(
        int *cntr_ptr
)
{
  TMC_ASSERT( cntr_ptr != NULL );

  if( tmc_reset_counter.is_valid ) {
    *cntr_ptr = tmc_reset_counter.num;
  }

  return tmc_reset_counter.is_valid;

} /* tmc_reset_counter_read */


/* <EJECT> */
/*===========================================================================

FUNCTION tmc_reset_counter_reset

DESCRIPTION
  Reset reset counter from flash memeory.

DEPENDENCIES
  FS task has already been started.

RETURN VALUE
  TRUE reset operation succeed.
  FALSE reset operation fail.

SIDE EFFECTS
  None

===========================================================================*/
EXTERN  boolean                    tmc_reset_counter_reset(void)
{
  if( tmc_reset_counter.is_valid )
  {
    /* Reset counter.
    */
    tmc_reset_counter.num = 0;
    return( tmc_reset_counter_write() );
  }

  /* If invalide, EFS operation is wrong return false.
  */
  return FALSE;

} /* tmc_reset_counter_write */
#endif /* FEATURE_RESET_COUNTER */


/* <EJECT> */
/*===========================================================================

FUNCTION tmc_malloc

DESCRIPTION
  This function allocate the memmory from tmc heap.
  If the number of bytes to allocate is less than the memory threshold, use
  tmc_heap_small_size otherwise use the tmc_heap_size.

DEPENDENCIES
  None

RETURN VALUE
  the pointer to the tmc heap.

SIDE EFFECTS

===========================================================================*/
#ifdef FEATURE_MEM_DEBUG
#error code not present
#else
EXTERN  void                       *tmc_malloc(
        size_t                     size
                                   /* Number of bytes to allocate
                                   */
)
#endif
{
  
  #ifndef FEATURE_LOW_MEMORY_USAGE
    if(size <= tmc_get_mem_heap_size(TMC_MEM_HEAP_TYPE_THRD) )
    {
      #ifdef FEATURE_MEM_DEBUG
#error code not present
      #else
      return( mem_malloc(&tmc_heap_small,size) );
      #endif
    }
    else
  #endif
  {
    #ifdef FEATURE_MEM_DEBUG
#error code not present
    #else
    return( mem_malloc(&tmc_heap, size) );
    #endif
  }
} /* tmc_malloc */


/* <EJECT> */
/*===========================================================================

FUNCTION tmc_free

DESCRIPTION
  This function free the memmory allocated tmc heap.
  If the memory is in the range of tmc heap or tmc small heap, free per
  request.
  Otherwise, return the error message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
#ifdef FEATURE_MEM_DEBUG
#error code not present
#else
EXTERN  void                       tmc_free(
        void                       *ptr
                                   /* pointer to the memory to free.
                                   */
)
#endif
{
  
  #ifndef FEATURE_LOW_MEMORY_USAGE
    if((ptr >= (void *) tmc_heap_small_mem_buffer) &&
       (ptr <  (void *)(tmc_heap_small_mem_buffer +
                             tmc_get_mem_heap_size(TMC_MEM_HEAP_TYPE_SMALL_BUF)) ) )
    { /*lint -e424 *//* inappropriate free for static data */
      #ifdef FEATURE_MEM_DEBUG
#error code not present
      #else
      mem_free( &tmc_heap_small, ptr );
      #endif/*lint +e424 */
    }
    else 
  #endif
  if((ptr >= (void *) tmc_heap_mem_buffer) &&
          (ptr <  (void *)(tmc_heap_mem_buffer +
                      tmc_get_mem_heap_size(TMC_MEM_HEAP_TYPE_BUF)) ) )
  { /*lint -e424 *//* inappropriate free for static data */
    #ifdef FEATURE_MEM_DEBUG
#error code not present
    #else
    mem_free( &tmc_heap, ptr );
    #endif/*lint +e424 */
  }
  else
  {
    if (!ptr) return;
    TMC_ERR_FATAL("Mem Free error : Invalid Heap enountered",0,0,0);
  }
}/* tmc_free */


/* <EJECT> */
/*===========================================================================

FUNCTION tmc_realloc

DESCRIPTION
  This function allocate the memmory from tmc heap.
  If the memory is in the range of tmc heap or tmc small heap, reallocate the
  memory per request.
  Otherwise, return the error message.

DEPENDENCIES
  None

RETURN VALUE
  the pointer to the tmc heap.

SIDE EFFECTS

===========================================================================*/
#ifdef FEATURE_MEM_DEBUG
#error code not present
#else
EXTERN  void                       *tmc_realloc(
        void                       *ptr,
                                   /* pointer to the memory for realloc.
                                   */
        size_t                     size
                                   /* Number of bytes to allocate
                                   */
)
#endif
{
  
#ifndef FEATURE_LOW_MEMORY_USAGE
  if((ptr >= (void *) tmc_heap_small_mem_buffer) &&
     (ptr <  (void *)(tmc_heap_small_mem_buffer +
                      tmc_get_mem_heap_size(TMC_MEM_HEAP_TYPE_SMALL_BUF))) )
  {
    #ifdef FEATURE_MEM_DEBUG
#error code not present
    #else
    return (mem_realloc(&tmc_heap_small, ptr, size));
    #endif
  }
  else 
#endif
  if((ptr >= (void *) tmc_heap_mem_buffer) &&
     (ptr <  (void *)(tmc_heap_mem_buffer +
             tmc_get_mem_heap_size(TMC_MEM_HEAP_TYPE_BUF))) )
  {
    #ifdef FEATURE_MEM_DEBUG
#error code not present
    #else
    return (mem_realloc(&tmc_heap, ptr, size));
    #endif
  }

  /* If coming here, error fatal. */
  TMC_ERR_FATAL("Memory Reallocate Error: Invalid Heap enountered",0,0,0);
  /*lint -e527 */ /* Unreachable */
  return(NULL);/*lint +e527 */
} /* tmc_realloc */


/* <EJECT> */
/*===========================================================================

FUNCTION tmc_task

DESCRIPTION
  This function is entrance of the Task Main Controller. It is responsible
  for none protocol specific power up initialization and task related
  operation, such as define, start, offline, power down, stop, and LPM.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  Reset and powerdown processing are special in that they cause the
  complete DMSS software to either restart or to be shutdown.

===========================================================================*/
EXTERN  void                       tmc_task(
        dword                      ignored
      /* Parameter received from Rex - ignored */
      /*lint -esym(715,ignored)
      ** Have lint not complain about the ignored parameter 'ignored' which is
      ** specified to make this routine match the template for rex_init().
      */
)
{
  rex_sigs_type requested_sigs;
  rex_sigs_type received_sigs;

  /* None protocol specific power up initialization, TMC timer and queue
  ** initialization, and define and start all other tasks.
  */
  tmc_init();

#ifdef FEATURE_DLOAD_HW_RESET_DETECT
  boot_init_abnormal_hw_reset_detection();
#endif /* FEATURE_DLOAD_HW_RESET_DETECT */

  /* Set the watchdog report timer signal
  ** reset the watchdog as soon as we start checking for events
  ** and start our first tick timer
  */
  (void) rex_set_sigs( rex_self(), TMC_RPT_TIMER_SIG );

  /* Define requested sigs */
  requested_sigs =

      /* Signal for TMC commands */
      TMC_CMD_Q_SIG

      /* Signal for watchdog report */
    | TMC_RPT_TIMER_SIG

      /* Add other signals here */
      ;

  /* Loop forever.
  */
  for( ; ;)
  {

    #ifdef FEATURE_BRINGUP_DIAG_ONLY
    TMC_MSG_ERROR("Event loop",0, 0, 0 );
    #endif /* FEATURE_BRINGUP_DIAG_ONLY */
    /* Perform the rex wait.
    */
    received_sigs = rex_wait( requested_sigs );

    /* Process report timer signal.
    */
    if (received_sigs & TMC_RPT_TIMER_SIG) {
      tmc_dog_report();
    }

    /* Process command signal and update state of TMC task.
    */
    if (received_sigs & TMC_CMD_Q_SIG) {
      (void) rex_clr_sigs( rex_self(), TMC_CMD_Q_SIG );
      tmc_dispatch_commands();
    }

    /* Add processing other signals here */


  } /* for ( ; ;) */

} /* tmc_task */

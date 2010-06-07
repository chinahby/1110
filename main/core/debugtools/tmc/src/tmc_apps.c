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


Copyright (c) 2004 - 2008 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc1100/services/tmc/tmc_apps.c#2 $ $DateTime: 2008/07/29 17:18:15 $ $Author: ehong $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/02/08   hs      Added support for FEATURE_CHG_TASK.
06/12/08   vph     Disable tmc_heap_small by added #ifndef FEATURE_LOW_MEMORY_USAGE 
08/24/06   pc      Added support for FEATURE_QIPCALL. 
03/29/06   ptm     Added init calls for spinlocks and smem logging for 7K.
03/24/06   pc      Added support for SECRND task
03/23/06   cl      Added PDSM_COMM_TASK
03/10/06   xz      Added the QVPHONE related tasks for 7500 support
01/06/06   ddh     Added SLEEP Task, and moved DEM task creation and start
12/22/05   ~SN     Added FEATURE_APPS_KEEPALIVE.
10/31/05   hn      Took out DEM task from the apps standalone builds.
09/26/05   jhs     Added call to tlmm_init to setup output enable masks.
08/15/05   hn      Added L4/Iguana and Linux/Wombat support.
08/10/05   taw     Added a call to flash_nand_init() to init the flash driver
                   and HW.
08/09/05   jc      Function brew_pwrup_init() is not called for BREW 3.1.4.
08/05/05   jhs     Removed SMEM references.  Added SMD include file.
08/04/05   jhs     Added display task.
07/14/05   bhd     Added ATI Graphics task support
07/11/05    aw     Added support for snd task.
06/21/05   jhs     Added support for timer task.
05/13/05   jhs     Added SW workaround for VIC HW.
05/12/05   jc      Removed watchdog task from apps build.
05/11/05   ptm     Restore correct featurization around RDM init code.
04/29/05   adm     Merged in SURF bringup related changes made to put together
                   a light DIAG/RPC/UI/EFS/NV only dual processor build using
                   FEATURE_BRINGUP_DIAG_ONLY.
04/22/05   ptm     Moved smem and oncrpc tasks to top of task def routine.
04/14/05   adm     Added smd task under FEATURE_SMD
04/14/05   tmr     Changed featurization of RDM to not be included for apps.
01/28/04   ptm     Remove references to sta_remote_task and add feature
                   protection around sta references.
01/21/05   ~sn     Commented for Apps for now. Will be enabled later.
01/21/05   ~sn     Added DMOV INIT
12/27/04    ro     Added FEATURE_QDSP_RTOS - start ADSP RTOS Task
12/08/04   jhs     Allowed gpio_int_init to run since protected by GPIO
                   ownership by modem.
12/03/04   ptm     Moved the start up of ONCRPC to tmc_start_service_tasks and
                   rdm_get_prev_ports and rdm_issue_opens up to
                   tmc_libraries_init_after_service_tasks_start so that
                   ONCRPC is ready to handle RPC calls before any of the
                   application tasks are started.
11/30/04   TMR     Added PS task
10/29/04   jhs     Put undef of NV back in, removed undef of HS
10/13/04   adm     Added smem task
07/06/04   ptm     Initial Release - Apps processor version of tmc, based on
                   the combined version of tmc.c.

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

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
#if (TG!=T_PC)                /* Define for all targets except Unit Test  */
#include "tramp.h"                      /* Interrupt trampline header file */
#include "hw.h"                                    /* Hardware header file */
#include "gpio_int.h"                             /* GPIO Init header file */
#include "clk.h"                                      /* Clock header file */
#include "bio.h"                                     /* Bit IO header file */
#include "clkregim.h"                             /* CLK Regim header file */

#ifdef FEATURE_QVPHONE
#include "qvp_codec.h"
#include "qvpapp_task.h"
#endif

#if !defined (FEATURE_RF_ZIF)
#error code not present
#else
#include "rflib.h"
#endif

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

#ifndef FEATURE_STANDALONE_APPS
#include "dem.h"                                        /* DEM Header file */
#endif

#ifndef FEATURE_THIN_UI
#ifdef FEATURE_HS
  #include "hs.h"                                   /* Handset header file */
#endif /* FEATURE_HS */
#endif /* !FEATURE_THIN_UI */

#ifdef FEATURE_IS2000_TDSO                             /* TDSO header file */
  #include "tdso.h"
#endif /* FEATURE_IS2000_TDSO */

#ifdef FEATURE_EFS                                      /* EFS header file */
  #include "fs.h"
  #ifdef FEATURE_EFS2
    #include "keypad.h"
    extern int fs_fresh_start;
  #endif
#endif /* FEATURE_EFS */

#ifdef FEATURE_EFS_SFAT                                /* SFAT header file */
  #include "fs_sfat.h"
#endif /* FEATURE_EFS_SFAT */

#if defined(FEATURE_NV) && ! defined(FEATURE_EXPORT_NV)
  #include "nv.h"                                        /* NV header file */
#endif /* FEATURE_NV && ! FEATURE_EXPORT_NV */

#ifdef FEATURE_UI
#include "ui.h"
#endif /* FEATURE_UI */

#include "BREWVersion.h"
#if !MIN_BREW_VERSIONEx(3,1,4)   /* Brew 3.1.4 removed brew_pwrup_init() */
#if MIN_BREW_VERSION(3,0) || defined(FEATURE_BREW_PWRUP_INIT)
extern void  brew_pwrup_init(void);
#endif /* MIN_BREW_VERSION(3,0) || FEATURE_BREW_PWRUP_INIT */
#endif /* !MIN_BREW_VERSIONEx(3,1,4) */

#ifdef FEATURE_QIPCALL
#include "qipcall.h"
#endif /* FEATURE_QIPCALL */


#ifdef FEATURE_CM
  #include "cm.h"                                        /* CM header file */
  #include "cmtask.h"
#endif /* FEATURE_CM */

#include "mmoc.h"                                      /* MMOC header file */

#ifdef FEATURE_QDSP_RTOS
  #include "adsprtossvc.h"
#endif /* FEATURE_QDSP_RTOS */

#ifdef FEATURE_SND_TASK
  #include "snd.h"                                    /* Sound header file */
#endif /* FEATURE_SND_TASK */

#ifdef FEATURE_HDR                                     /* HDR header files */
#error code not present
#endif /* FEATURE_HDR */

#if (defined FEATURE_GPSONE) && (!defined FEATURE_CGPS) /* GPSONE related header files */
#error code not present
#endif  /* FEATURE_GPSONE && !FEATURE_CGPS */

#ifdef FEATURE_GPSONE_PE
#error code not present
#endif  /* FEATURE_GPSONE_PE */

#ifdef FEATURE_BT                        /* Bluetooth related header files */
  #include "bt.h"
  #include "bti.h"
#endif /* FEATURE_BT */

#ifdef FEATURE_DIAG_TASK
  #include "diag.h"                                    /* Diag header file */
  #include "event.h"                                  /* Event header file */

  #ifndef FEATURE_DIAG_COUPLED_ARCHITECTURE
    #include "diagi.h"                        /* Diag internal header file */
  #endif /* FEATURE_DIAG_COUPLED_ARCHITECTURE */
#endif /* FEATURE_DIAG_TASK */

#ifdef FEATURE_DATA
#include "dstask.h"

#ifdef FEATURE_WCDMA /* && ( defined FEATURE_DATA_WCDMA_CS ) ) */
#error code not present
#endif /* FEATURE_WCDMA */

#if ( (defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS)) && !defined(FEATURE_WCDMA_PLT) )
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
  //#include "siosrvr.h"
#endif /* FEATURE_REX_IPC */


#ifdef FEATURE_ONCRPC                                /* ONCRPC header file */
#error code not present
#endif /* FEATURE_ONCRPC */

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

  #ifdef FEATURE_SEC_ARM_CRYPTO
    extern void seccryptarm_task( dword dummy );
  #endif

  #ifdef FEATURE_SEC_IPSEC
    #include "secipstask.h"
  #endif

#ifdef FEATURE_SEC_SSL                              /* SSLTASK header file */
  #include "secssltask.h"
#endif /* FEATURE_SEC_SSL */
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

#ifdef FEATURE_ATI_GRAPHICS
  #include "ahi_apps_drv.h"                        /* GRAPHICS header file */
#endif /* FEATURE_ATI_GRAPHICS*/

#ifdef FEATURE_GRAPH_TASK
  #include "graph.h"                                  /* GRAPH header file */
#endif /* FEATURE_GRAPH_TASK*/

#ifdef FEATURE_VS_TASK
  #include "vs.h"                                        /* VS hezder file */
#endif

#ifdef FEATURE_USB                                    /* USBDC header file */
  #include "usbdc.h"
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

#ifdef FEATURE_APPS_KEEPALIVE
  #include "dog_keepalive.h"
#endif /* FEATURE_APPS_KEEPALIVE */

#ifdef TIMETEST
  #include "timetest.h"
#endif /* TIMETEST */

#ifdef FEATURE_MDSP_TASK
#include "mdsp.h"
#endif /* FEATURE_MDSP_TASK */

#ifdef FEATURE_MMGPS
#error code not present
#endif /* FEATURE_MMGPS */

#ifdef FEATURE_MGP
#include "mc_task.h"
#include "nf_task.h"
#include "cd_task.h"
#include "ai_task.h"
#include "lp_task.h"
#endif /* FEATURE_MGP */

#if defined( FEATURE_BATTERY_CHARGER) || defined(FEATURE_CHG_TASK)
  #include "charger.h"
#endif

#ifdef FEATURE_FACTORY_TESTMODE
  #include "ftm.h"
#endif /* FEATURE_FACTORY_TESTMODE */


#ifdef FEATURE_USE_TIME_VU
#include "time.h"
#endif /* FEATURE_USE_TIME_VU */

#ifdef FEATURE_SMD
/*Include SMD's main interface header file*/
#include "smd_task.h"
#endif
#if defined(T_MSM7500) && !defined(FEATURE_STANDALONE_APPS)
#error code not present
#endif /* T_MSM7500 && ! FEATURE_STANDALONE_APPS */

#ifdef FEATURE_PMEM
#include "pmem.h"
#endif

#ifdef FEATURE_BUSPERF
#include "bus_perf.h"
#endif

#if defined(FEATURE_SPINLOCK) && defined(FEATURE_ARM_ASM_SPINLOCK)
#include "smem_data_decl.h"
#include "spinlockarm.h"
#endif /* FEATURE_SPINLOCK && FEATURE_ARM_ASM_SPINLOCK */

#ifdef FEATURE_SMSM
#include "smem_data_decl.h"
#include "smsm.h"
#endif

#ifdef FEATURE_SMEM_LOG
#include "smem_data_decl.h"
#include "smem_log.h"
#endif /* FEATURE_SMEM_LOG */

#ifdef FEATURE_DISP_TASK
#include "disp_task.h"
#endif /* FEATURE_DISP_TASK */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/* Add mode specific header file here */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#ifdef TMC_1X
  #include "rflib.h"                                  /* RFLIB header file */
  #include "rfnv.h"                           /* RF NV Manager header file */
  #include "pm.h"                                        /* PM header file */
  #include "msm_init.h"                            /* msm_init header file */
  #ifndef FEATURE_MDSP_TASK
  #include "dmddown.h"                  /* Demodulator service header file */
  #endif
  #include "mccdma.h"                                  /* CDMA header file */
  #include "tx.h"                                        /* TX header file */
  #include "rx.h"                                        /* RX header file */
  #include "srch.h"                                    /* Srch header file */
  #include "rxtx.h"                                    /* RXTX header file */
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

#if (defined( FEATURE_GSM ) && !defined( FEATURE_WCDMA_PLT))
#error code not present
#endif /* FEATURE_GSM && ! FEATURE_WCDMA_PLT */

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

#if (!defined(RF_HAS_MULTIMODE_BBRX) && !defined(FEATURE_WCDMA_PLT))
  #include "adie.h"                                  /* ADIE header file */
#endif /* !RF_HAS_MULTIMODE_BBRX && !FEATURE_WCDMA_PLT */

#ifdef FEATURE_VIRTUAL_SIM
  #include "virtuim.h"
#endif

#ifdef FEATURE_L4
#error code not present
#endif /* FEATURE_L4 */

#ifdef FEATURE_QIDS_RPC
#include "qids_smms.h"
#include "qids_rpc_app.h"
#endif /* FEATURE_QIDS_RPC */

#ifdef FEATURE_IG_QDIAG
#include "qdiag.h"
#endif

#ifdef FEATURE_APS
#include "aps.h"
#endif

#ifdef FEATURE_PM_SERVER
  #include "pmserv.h"
#endif /* FEATURE_PM_SERVER */

#if defined(FS_FEATURE_IG_IPC_SERVER) && !defined(FEATURE_IG_EFS_EXT_SERVER)
  #include  "fs_iguana_server.h"
#endif /* FS_FEATURE_IG_IPC_SERVER && !FEATURE_IG_EFS_EXT_SERVER */

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

/* PUBLIC REGIONAL */
/*---------------------------------------------------------------------------
                                  REX TIMER
---------------------------------------------------------------------------*/

LOCAL rex_timer_type    tmc_wait_timer;      /* Timer for timer wait       */

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

#ifdef FEATURE_QVPHONE
extern void qvp_ae_task (dword dummy);
extern void qvp_vfe_task(dword param);
extern void qvp_mpeg4_task(dword param);
#ifdef FEATURE_QVPHONE_SIP
#error code not present
#endif /* FEATURE_QVPHONE_SIP */
#endif /* FEATURE_QVPHONE */

#ifdef FEATURE_QVPHONE_SIP
#error code not present
#endif

#ifdef FEATURE_EFS_PROBE_NAND
#include "flash_nand.h"
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
LOCAL boolean                     tmc_reset_counter_init( void )
{
  fs_rsp_msg_type fs_rsp;           /* fs generic response msg type */

  fs_open_xparms_type open_params;
  fs_handle_type file_handle;       /* file handler */

  /* Initialize tmc reset counter data structure.
  */
  tmc_reset_counter.is_valid = FALSE;
  tmc_reset_counter.num = 0;
  strcpy(tmc_reset_counter.filename, tmc_reset_cnt_file);

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
LOCAL boolean                     tmc_reset_counter_write()
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
LOCAL boolean                     tmc_reset_counter_increment( void )
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
LOCAL void                        tmc_module_init( void )
{
  int i;

  /* Initialize timers */
  /* Add other timers intialize here */
  rex_def_timer(&tmc_wait_timer,&tmc_tcb,TMC_WAIT_SIG);

  /* Initialize queues */
  q_init(&tmc_cmd_q);
  q_init(&tmc_free_cmd_q);

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

FUNCTION tmc_wait

DESCRIPTION
  Suspend TMC from executing until at least one of the specified signals
  in the input mask is set.

DEPENDENCIES
  None

RETURN VALUE
  rex_sigs_type

SIDE EFFECTS
  None

===========================================================================*/
LOCAL   rex_sigs_type                      tmc_wait(

        rex_sigs_type             mask
            /* Mask of signals to wait on */
)
{
  return rex_wait( mask);
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
LOCAL tmc_cmd_type              *tmc_get_cmd( void )
{
  /* Dequeue a command from command queue */
  return (tmc_cmd_type *) q_get(&tmc_cmd_q);

} /* tmc_get_cmd */


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
LOCAL void               tmc_free_cmd(

      tmc_cmd_type    *cmd_ptr
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
LOCAL void                        tmc_offline_tasks( void )
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
  */

#if !defined(FEATURE_THIN_UI) || defined(FEATURE_UI_CORE_REMOVED)
  #ifdef FEATURE_HS
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &hs_tcb;
  #endif /* FEATURE_HS */
#endif /* !FEATURE_THIN_UI || FEATURE_UI_CORE_REMOVED */

  #if defined(FEATURE_NV) && ! defined(FEATURE_EXPORT_NV)
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &nv_tcb;
  #endif /* FEATURE_NV && ! FEATURE_EXPORT_NV */

  #ifdef FEATURE_DIAG_TASK
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &diag_tcb;
  #endif /* FEATURE_DIAG_TASK */

  #ifdef FEATURE_IG_QDIAG
  wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
  tcb_ptr[i++] = &qdiag_tcb;
  #endif /* FEATURE_IG_QDIAG */

  #ifdef FEATURE_UI
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &ui_tcb;
  #endif /* FEATURE_UI */

  #ifdef FEATURE_VS_TASK
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &vs_tcb;
  #endif /* FEATURE_VS_TASK */

  #ifdef FEATURE_ATI_GRAPHICS
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &graphics_tcb;
  #endif /* FEATURE_ATI_GRAPHICS */

  #ifdef FEATURE_GRAPH_TASK
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &graph_tcb;
  #endif /* FEATURE_GRAPH_TASK */

  #ifdef FEATURE_QDSP_RTOS
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &adsp_rtos_tcb;
  #endif /* FEATURE_QDSP_RTOS */

  #ifdef FEATURE_SND_TASK
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &snd_tcb;
  #endif /* FEATURE_SND_TASK */

  #ifdef FEATURE_DISP_TASK
  wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
  tcb_ptr[i++] = &disp_tcb;
  #endif /* FEATURE_DISP_TASK */

  #ifdef FEATURE_APS
  wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
  tcb_ptr[i++] = &aps_tcb;
  #endif /* FEATURE_APS */

  #ifdef FEATURE_PM_SERVER
  wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
  tcb_ptr[i++] = &pms_tcb;
  #endif /* FEATURE_PM_SERVER */

  #ifdef FEATURE_QIPCALL
  wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
  tcb_ptr[i++] = &qipcall_tcb;
  #endif  /* FEATURE_QIPCALL */

  /* Add other tasks here.
  */

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

   received_sigs = rex_timed_wait(TMC_ACK_SIG | TMC_WAIT_SIG,
                                  &tmc_wait_timer,
                                  wait_cnt[i] );

   if (received_sigs & TMC_ACK_SIG) {
     (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
   }
   else{
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
LOCAL void                        tmc_stop_tasks( void )
{
  rex_sigs_type        received_sigs;                   /* Received signal */
  rex_tcb_type         *tcb_ptr[TMC_MAX_NUM_OF_TASK];   /* Stop task array */
  int                  i=0,num_of_task;          /* number of task counter */
                       /* the wait time counter in milliseconds */
  rex_timer_cnt_type   wait_cnt[TMC_MAX_NUM_OF_TASK];

  /* Clear ack signal. */
  (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

  /* Store required task to tcb array.*/

  #ifdef FEATURE_UI
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &ui_tcb;
  #endif /* FEATURE_UI */

  #ifdef FEATURE_IG_QDIAG
  wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
  tcb_ptr[i++] = &qdiag_tcb;
  #endif /* FEATURE_IG_QDIAG */

  #ifdef FEATURE_DIAG_TASK
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &diag_tcb;
  #endif /* FEATURE_DIAG_TASK */

#if !defined(FEATURE_THIN_UI) || defined(FEATURE_UI_CORE_REMOVED)
  #ifdef FEATURE_HS
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &hs_tcb;
  #endif /* FEATURE_HS */
#endif /* !FEATURE_THIN_UI || FEATURE_UI_CORE_REMOVED */

  #ifdef FEATURE_ONCRPC
#error code not present
  #endif /* FEATURE_ONCRPC */

  #if defined(FEATURE_NV) && ! defined(FEATURE_EXPORT_NV)
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &nv_tcb;
  #endif /* FEATURE_NV && ! FEATURE_EXPORT_NV */

  #ifdef FEATURE_EFS_SFAT
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &sfat_tcb;
  #endif /* FEATURE_EFS_SFAT */

  #ifdef FEATURE_EFS
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &fs_tcb;
  #endif /* FEATURE_EFS */

  #ifdef FEATURE_VS_TASK
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &vs_tcb;
  #endif /* FEATURE_VS_TASK */

  #ifdef FEATURE_ATI_GRAPHICS
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &graphics_tcb;
  #endif /* FEATURE_ATI_GRAPHICS */

  #if (defined FEATURE_GPSONE) && (!defined FEATURE_CGPS)
#error code not present
  #endif

  #ifdef FEATURE_GRAPH_TASK
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &graph_tcb;
  #endif /* FEATURE_GRAPH_TASK */

  #ifdef FEATURE_QDSP_RTOS
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &adsp_rtos_tcb;
  #endif /* FEATURE_QDSP_RTOS */

  #ifdef FEATURE_SND_TASK
    wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
    tcb_ptr[i++] = &snd_tcb;
  #endif /* FEATURE_SND_TASK */

  #ifdef FEATURE_APS
  wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
  tcb_ptr[i++] = &aps_tcb;
  #endif /* FEATURE_APS */

  #ifdef FEATURE_PM_SERVER
  wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
  tcb_ptr[i++] = &pms_tcb;
  #endif /* FEATURE_PM_SERVER */

  #ifdef FEATURE_QIPCALL
  wait_cnt[i] = TMC_DEFAULT_WAIT_CNT;
  tcb_ptr[i++] = &qipcall_tcb;
  #endif  /* FEATURE_QIPCALL */

  #ifdef FEATURE_USB
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
  /* Add other required tasks here. */

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

    received_sigs = rex_timed_wait(TMC_ACK_SIG | TMC_WAIT_SIG,
                                   &tmc_wait_timer,
                                   wait_cnt[i] );

    if (received_sigs & TMC_ACK_SIG) {
      (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
    }
    else{
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
LOCAL void                        tmc_online_handler( void )
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
LOCAL void                        tmc_offline_handler( void )
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
LOCAL void                        tmc_reset_handler( void )
{

   #ifdef FEATURE_UIM_RUIM
     /* Indicate to power down the RUIM
     */
     TMC_MSG_MED("Vote to Power down RUIM...", 0, 0, 0);
     uim_power_control(UIM_MC, FALSE);
   #endif /* FEATURE_UIM_RUIM */

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
LOCAL void                        tmc_powerdown_handler( void )
{

   #ifdef FEATURE_UIM_RUIM
     /* Indicate to power down the RUIM
     */
     TMC_MSG_MED("Vote to power down RUIM...", 0, 0, 0);
     uim_power_control(UIM_MC, FALSE);
   #endif /* FEATURE_UIM_RUIM */

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

LOCAL void                        tmc_lpm_handler( void )
{
  TMC_MSG_HIGH("TMC LPM handler", 0, 0, 0);
  #ifdef TMC_HW_IMPLEMENT_LPM
  hw_enter_low_power_mode();
  #endif /* TMC_HW_IMPLEMENT_LPM */
}


/* <EJECT> */
/*===========================================================================

FUNCTION tmc_set_state

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
LOCAL void                        tmc_set_state(
      tmc_state_e_type            state
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
LOCAL void                       tmc_store_esn_me (
        dword                    esn_me
  )
{
  tmc_data.esn_me = esn_me;
} /* tmc_store_esn_me */
#endif /* FEATURE_UIM_RUIM */


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
LOCAL void                        tmc_dispatch_commands( void )
{
  tmc_cmd_type *cmd_ptr;                     /* To hold received command */

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
LOCAL void                      tmc_libraries_init_before_tasks_start( void )
{

  /*                    Generic Initialization                             */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_PMEM
    /* Make sure to initialize the pmem system */
    pmem_module_init();

  /* We are not checking the return value since we know that our static */
  /* arguments cause no harm. Besides, what are we going to do in the   */
  /* case of a failure?                                                 */
  #ifdef FEATURE_SMI
  (void) pmem_init((void *) SCL_MM_HEAP1_BASE, SCL_MM_HEAP1_SIZE, PMEM_FLAG_SMI);
  #endif

  (void) pmem_init((void *) SCL_MM_HEAP2_BASE, SCL_MM_HEAP2_SIZE, PMEM_FLAG_EBI1);
  //DDH(void) pmem_init((void *) TCM_BASE, TCM_RAM_SIZE, PMEM_FLAG_TCM);

#endif /* FEATURE_PMEM */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_BUSPERF
  /* Initialize the bus performance subsystem */
  bus_init();
#endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize TMC NV-Interface
  */
  tmcnv_init(TMC_NV_CMD_SIG, 0, NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  tramp_init();                                /* Interrupt initialization */

#ifdef FEATURE_VIC_PRIO_0_WORKAROUND
  tramp_set_isr( TRAMP_UART1_DM_RX_ISR, tramp_handle_sw_workaround_isr );
  tramp_set_interrupt(TRAMP_UART1_DM_RX_ISR);
#endif /* FEATURE_VIC_PRIO_0_WORKAROUND */

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
  smem_log_init( smem_data.smem_log_events,
                 &smem_data.smem_log_idx,
                 SMEM_LOG_NUM_ENTRIES );
#endif /* FEATURE_SMEM_LOG */


#ifdef FEATURE_APPS_KEEPALIVE
  dog_keepalive_init();
#endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifdef FEATURE_USE_TIME_VU
  /* Initialize the TIME subsystem before MSG, since MSG's uses TIME
  */
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

    (void) strcpy(timer_tcb.task_name, "TIMER");

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

  dsm_init();                                        /* Data services      */

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
#endif /* FEATURE_LOW_MEMORY_USAGE */
  #endif /* FEATURE_REX_DYNA_MEM */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef TMC_BIO_INIT
    bio_init();                                     /* BIO initialization. */
  #endif /* TMC_BIO_INIT */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the clocks */
  clk_regime_apps_init();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  gpio_int_init();                     /* Initialize GPIO interrupt handler*/

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#if 0
  _enable();                    /* Enable interrupts and start processing. */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  clk_init();                                           /* Initialize Clock*/

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#endif /* 0 */

  /* Initialize the TLMM. */
  #ifdef T_MSM31

    tlmm_init ();

  #endif /* T_MSM31 */

  /* Initialize SIO */
   sio_init();


  /* Initialize USB host API */
  #ifdef FEATURE_USB
    #ifdef FEATURE_USB_HOST
      usbhost_api_init();
    #endif
  #endif

  /* Register TMC dispatch table through diag interface */
  tmcdiag_init();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_DMOV
  /* Initialize DMOV data mover driver */
  /* Must be after time_init so that we know DMOV init is complete on the
     modem procesor */
  (void)dmov_init(NULL);
#endif

#ifdef FEATURE_EFS_PROBE_NAND
  /* Intialize the flash driver and HW */
  /* Must be done after time_init so we know flash init is complete on
   * the modem processor. */
  flash_nand_init();
#endif  

#if !MIN_BREW_VERSIONEx(3,1,4)   /* Brew 3.1.4 removed brew_pwrup_init() */
#if MIN_BREW_VERSION(3,0) || defined(FEATURE_BREW_PWRUP_INIT)
  brew_pwrup_init();
#endif /* MIN_BREW_VERSION(3,0) || FEATURE_BREW_PWRUP_INIT */
#endif /* !MIN_BREW_VERSIONEx(3,1,4) */

#ifdef FEATURE_QIDS_RPC
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Init SMMS Library */
  if( qids_smms_init(SMMS_DEAFULT_SERVER_PRIORITY) == SMMS_STATUS_SUCCESS )
  {
    /* Start a loopback server in backgroung */
    qids_smms_start_loopback_server(SMMS_LOOPBACK_SERVER_PRIORITY,
                                    SMMS_SERVICE_ECHO1);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#endif /* FEATURE_QIDS_RPC */

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
  None

RETURN VALUE
  void

SIDE EFFECTS
  After this procedure all the tasks in the Software system will have
  performed preliminary initialization and will be waiting for the
  startup signal from the Main Control task.

===========================================================================*/
LOCAL void                        tmc_define_tasks( void )
{

  #ifdef FEATURE_TMC_STACK_PROFILING
    int i;                        /* counter for number of bytes of stack. */
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

    (void) strcpy(sleep_tcb.task_name, "ASLEEP");

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
                        0);
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

    /* Create SMD task  */
  #ifdef FEATURE_SMD

    #ifdef TIMETEST
      smd_tcb.leds = TIMETEST_SMD_TASK_ID;
    #endif

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < SMD_PKT_LLW_STACK_SIZ; i++) {
        smd_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    strcpy( &smd_tcb.task_name[0], "SHARED_MEMORY_DRIVER" );

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

  #if (defined FEATURE_GPSONE) && (!defined FEATURE_CGPS)
#error code not present
  #endif /* FEATURE_GPSONE && !FEATURE_CGPS */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_ONCRPC
#error code not present
  #endif /* FEATURE_ONCRPC */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifndef FEATURE_STANDALONE_APPS

    #ifdef TIMETEST
      dem_tcb.leds = TIMETEST_DEM_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < DEM_STACK_SIZ; i++) {
        dem_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) strcpy(dem_tcb.task_name, "DEM");

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

  #endif /* FEATURE_STANDALONE_APPS */
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

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

    (void) strcpy(adsp_rtos_tcb.task_name, "ADSPOS");

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

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_ATI_GRAPHICS

    #ifdef TIMETEST
      graphics_tcb.leds = TIMETEST_GRAPH_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < GRAPHICS_STACK_SIZ; i++) {
        graphics_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) strcpy(graphics_tcb.task_name, "GRFATI");

    /* Create Graphics task and wait for ack back from it.
    */
    TMC_MSG_MED("Creating ATI Graphics task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&graphics_tcb,
                        (unsigned char *)graphics_stack,
                        GRAPHICS_STACK_SIZ * sizeof(rex_stack_word_type),
                        (rex_priority_type) GRAPHICS_PRI,
                        graphics_task,
                        0L,
                        "GRAPHICS",
                        FALSE,
                        0L);
    #else
      rex_def_task(&graphics_tcb,
                   (rex_stack_word_type*) graphics_stack,
                   GRAPHICS_STACK_SIZ,
                   (rex_priority_type) GRAPHICS_PRI,
                   graphics_task,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG) ;
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

  #endif /* FEATURE_ATI_GRAPHICS */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_GRAPH_TASK

    #ifdef TIMETEST
      graph_tcb.leds = TIMETEST_GRAPH_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < GRAPH_STACK_SIZ; i++) {
        graph_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) strcpy(graph_tcb.task_name, "GRPH");

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

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_VS_TASK

    #ifdef TIMETEST
      vs_tcb.leds = TIMETEST_VS_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < VS_STACK_SIZ; i++) {
        vs_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) strcpy(vs_tcb.task_name, "VS  ");

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

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_SND_TASK

    #ifdef TIMETEST
      snd_tcb.leds = TIMETEST_SND_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < SND_STACK_SIZ; i++) {
        snd_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif


    (void) strcpy(snd_tcb.task_name, "SND ");

    /* Create Sound Services task and wait for ack back from it.
    */
    TMC_MSG_MED("Creating Sound Services task", 0, 0, 0);
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
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

  #endif /* FEATURE_SND_TASK */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_QVPHONE

    #ifdef TIMETEST
      qvp_ae_tcb.leds = TIMETEST_QVP_AUDIO_TASK_ID;
    #endif

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < QVP_AE_STACK_SIZ; i++) {
        qvp_ae_task_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) strcpy( qvp_ae_tcb.task_name, "VPAE" );
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

    (void) strcpy( qvp_mpeg4_tcb.task_name, "VPMP" );
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

    (void) strcpy( qvpio_tcb.task_name, "VPIO" );
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

    (void) strcpy( qvppl_tcb.task_name, "VPPL" );
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

    (void) strcpy( qvp_vfe_tcb.task_name, "VPVFE" );
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

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_EFS

    #if (defined(FEATURE_EFS2) && !defined(FEATURE_WCDMA_PLT))
      /* if the panic keypad combination is pressed, set fs_fresh_start */
      if(keypad_is_panic_chord_pressed())
        fs_fresh_start=1;
    #endif /* FEATURE_EFS2 && ! FEATURE_WCDMA_PLT */

    #ifdef TIMETEST
      fs_tcb.leds = TIMETEST_FS_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < FS_STACK_SIZ; i++) {
        fs_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif


    strcpy(&fs_tcb.task_name[0], "FS  ");

    /* Create Embedded File System task and wait for ack back from it.
    */
    TMC_MSG_MED("Creating FS task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&fs_tcb,
                        (unsigned char *)fs_stack,
                        FS_STACK_SIZ * sizeof(rex_stack_word_type),
                        FS_PRI,
                        fs_task,
                        0L,
                        "FS",
                        FALSE,
                        DOG_FS_RPT);
    #else
      rex_def_task(&fs_tcb,
                   (rex_stack_word_type*)fs_stack,
                   FS_STACK_SIZ,
                   FS_PRI,
                   fs_task,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

    #if defined(FS_FEATURE_IG_IPC_SERVER) && !defined(FEATURE_IG_EFS_EXT_SERVER)

      #ifdef TIMETEST
        efs_srvr_tcb.leds = TIMETEST_EFS_SRVR_TASK_ID;
      #endif /* TIMETEST */

      #ifdef FEATURE_TMC_STACK_PROFILING
        for (i = 0; i < FS_STACK_SIZ; i++) {
          efs_srvr_stack[i] = TMC_STACK_PROFILING_DWORD;
        }
      #endif
      (void) strcpy(efs_srvr_tcb.task_name, "EFS Srvr");

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

    #endif /* FS_FEATURE_IG_IPC_SERVER && !FEATURE_IG_EFS_EXT_SERVER */
  #endif /* FEATURE_EFS */

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

    strcpy(&sfat_tcb.task_name[0], "SFAT");

    /* Create SFAT task and wait for ack back from it.
    */
    TMC_MSG_MED("Creating SFAT task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&sfat_tcb,
                        (unsigned char *)sfat_stack,
                        SFAT_STACK_SIZ * sizeof(rex_stack_word_type),
                        SFAT_PRI,
                        sfat_task,
                        0L,
                        "SFAT",
                        FALSE,
                        DOG_SFAT_RPT);
    #else
      rex_def_task(&sfat_tcb,
                   (rex_stack_word_type*)sfat_stack,
                   SFAT_STACK_SIZ,
                   SFAT_PRI,
                   sfat_task,
                   0L);
    #endif
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

  #endif /* FEATURE_EFS_SFAT */
#endif /* FEATURE_BRINGUP_DIAG_ONLY */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if defined(FEATURE_NV) && ! defined(FEATURE_EXPORT_NV)

    #ifdef TIMETEST
      nv_tcb.leds = TIMETEST_NV_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < NV_STACK_SIZ; i++) {
        nv_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) strcpy(nv_tcb.task_name, "NV  ");

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

  #endif /* FEATURE_NV && ! FEATURE_EXPORT_NV  */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_UI

    #ifdef TIMETEST
      ui_tcb.leds         = TIMETEST_UI_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < UI_STACK_SIZ; i++) {
        ui_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) strcpy(ui_tcb.task_name, "UI  ");

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

  #endif /* FEATURE_UI */

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

    strcpy(&diag_tcb.task_name[0], "DIAG");

    /* Create DIAG task and wait for ack
    */
    TMC_MSG_MED("Creating DIAG task", 0, 0, 0);
    #ifdef FEATURE_ENHANCED_REX_TASK
      rex_def_task_ext2(&diag_tcb,
                        (unsigned char *)diag_stack,
                        DIAG_STACK_SIZ * sizeof(rex_stack_word_type),
                        DIAG_PRI,
                        diag_task,
                        0L,
                        "DIAG",
                        FALSE,
                        DOG_DIAG_RPT);
    #else
      rex_def_task(&diag_tcb,
                   (rex_stack_word_type*) diag_stack,
                   DIAG_STACK_SIZ,
                   DIAG_PRI,
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

    (void) strcpy(qdiag_tcb.task_name, "QDIAG");

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

#if !defined(FEATURE_THIN_UI) || defined(FEATURE_UI_CORE_REMOVED)
  #ifdef FEATURE_HS

    #ifdef TIMETEST
      hs_tcb.leds = TIMETEST_HS_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < HS_STACK_SIZ; i++) {
        hs_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) strcpy(hs_tcb.task_name, "HS  ");

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
#endif /* !FEATURE_THIN_UI || FEATURE_UI_CORE_REMOVED */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifndef FEATURE_BRINGUP_DIAG_ONLY
   #if defined(FEATURE_DATA) && defined(FEATURE_DATA_PS)

    #ifdef TIMETEST
      ps_tcb.leds = TIMETEST_PS_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < PS_STACK_SIZ; i++) {
        ps_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) strcpy(ps_tcb.task_name, "PS  ");

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
  #endif /* defined(FEATURE_DATA) && defined(FEATURE_DATA_PS) */
#endif /* FEATURE_BRINGUP_DIAG_ONLY */

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_DISP_TASK

    #ifdef TIMETEST
      disp_tcb.leds = TIMETEST_DISP_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < DISP_STACK_SIZ; i++) {
        disp_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) strcpy(disp_tcb.task_name, "DISP");

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
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

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

    (void) strcpy(pms_tcb.task_name, "PMS");

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

    (void) strcpy(aps_tcb.task_name, "APS");

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

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_QIPCALL

    #ifdef TIMETEST
      qipcall_tcb.leds = TIMETEST_QIPCALL_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < QIPCALL_STACK_SIZ; i++) {
        qipcall_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) strcpy(qipcall_tcb.task_name, "QIPCALL  ");

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

  #endif /* FEATURE_QIPCALL */


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

#if defined(T_MSM7500) && !defined(FEATURE_STANDALONE_APPS)
#error code not present
#endif /* T_MSM7500 && ! FEATURE_STANDALONE_APPS */

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
LOCAL boolean            tmc_libraries_init_after_service_tasks_start( void )
{

  boolean status = TRUE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*                                                                       */
  /*                                                                       */
  /*                    Generic Initialization                             */
  /*                                                                       */
  /*                                                                       */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if defined (FEATURE_RUNTIME_DEVMAP) && (!defined (IMAGE_APPS_PROC) || defined (FEATURE_STANDALONE))
    #if defined(FEATURE_EFS)
    /* Get any previous port mappings on behalf of the Runtime Device Mapper
    */
    rdm_get_prev_ports();
    #endif
    /* Have RDM issue any open commands. If there were none retrieved from
    ** EFS, compile time defaults will be issued
    */
    rdm_issue_opens();
  #endif /*defined (FEATURE_RUNTIME_DEVMAP) && (!defined (IMAGE_APPS_PROC) || defined (FEATURE_STANDALONE)) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*                                                                       */
  /*                                                                       */
  /*                   Mode Specific Initialization                        */
  /*                                                                       */
  /*                                                                       */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return status;

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
LOCAL void                        tmc_start_service_tasks( void )
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

  #if defined(FS_FEATURE_IG_IPC_SERVER) && !defined(FEATURE_IG_EFS_EXT_SERVER)
  /* Start Embedded File System task and wait for response.
  */
  TMC_MSG_MED("Starting EFS Server task...", 0, 0, 0);
  (void) rex_set_sigs(&efs_srvr_tcb, TASK_START_SIG);
  (void) tmc_wait(TMC_ACK_SIG);
  (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FS_FEATURE_IG_IPC_SERVER && !FEATURE_IG_EFS_EXT_SERVER */

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
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#if !defined(FEATURE_THIN_UI) || defined(FEATURE_UI_CORE_REMOVED)
  #ifdef FEATURE_HS
    /* Start Handset task and wait for response.
    */
    TMC_MSG_MED("Starting HS task...", 0, 0, 0);
    (void) rex_set_sigs(&hs_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_HS */
#endif /* !FEATURE_THIN_UI || FEATURE_UI_CORE_REMOVED */

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

  #if defined(FEATURE_NV) && ! defined(FEATURE_EXPORT_NV)
    /* Start Non-Volatile Database task and wait for response.
    */
    TMC_MSG_MED("Starting NV task...", 0, 0, 0);
    (void) rex_set_sigs(&nv_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_NV && ! FEATURE_EXPORT_NV */

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

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_ATI_GRAPHICS
    /* ATI Graphics Start Graphics task and wait for response.
    */
    TMC_MSG_MED("Starting ATI Graphics task...", 0, 0, 0);
    (void) rex_set_sigs(&graphics_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_ATI_GRAPHICS */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  #ifdef FEATURE_GRAPH_TASK
    /* Start Graphics task and wait for response.
    */
    TMC_MSG_MED("Starting Graphics task...", 0, 0, 0);
    (void) rex_set_sigs(&graph_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_GRAPH_TASK */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_VS_TASK
    /* Start VS task and wait for response.
    */
    TMC_MSG_MED("Starting VOICE SERVICES task...", 0, 0, 0);
    (void) rex_set_sigs(&vs_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_VS_TASK */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_SND_TASK
    /* Start Sound Services task and wait for response.
    */
    TMC_MSG_MED("Starting Sound Services task...", 0, 0, 0);
    (void) rex_set_sigs(&snd_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_SND_TASK */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_QVPHONE
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

  #ifdef FEATURE_DISP_TASK
    /* Start disp task and wait for response.
    */
    TMC_MSG_MED("Starting DISP task...", 0, 0, 0);
    (void) rex_set_sigs(&disp_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_DISP_TASK */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_STANDALONE_APPS
    /* Start DEM Task
    */
    TMC_MSG_MED("Starting DEM task...", 0, 0, 0);
    (void) rex_set_sigs(&dem_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_STANDALONE_APPS */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#endif /* FEATURE_BRINGUP_DIAG_ONLY */
  /* Add other required tasks here. */

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
LOCAL void                        tmc_start_application_tasks( void )
{

  /* Clear ACK signal */

  (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_UI
    /* Start User Interface task and wait for response.
    */
    TMC_MSG_MED("Starting UI task...", 0, 0, 0);
    (void) rex_set_sigs(&ui_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_UI */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifndef FEATURE_BRINGUP_DIAG_ONLY
  #if defined(FEATURE_DATA) && defined(FEATURE_DATA_PS)
    /* Start Data Protocol Services task and wait for response.
    */
    TMC_MSG_MED("Starting PROTOCOL SERVICES task...", 0, 0, 0);
    (void) rex_set_sigs(&ps_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_DATA && FEATURE_DATA_PS */
#endif /* FEATURE_BRINGUP_DIAG_ONLY */

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

  #if (defined FEATURE_GPSONE) && (!defined FEATURE_CGPS)
#error code not present
  #endif /* FEATURE_GPSONE && !FEATURE_CGPS */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifdef FEATURE_QIPCALL
    /* Start QIPCALL task and wait for response.
    */
    TMC_MSG_MED("Starting QIPCALL task...", 0, 0, 0);
    (void) rex_set_sigs(&qipcall_tcb, TASK_START_SIG);
    (void) tmc_wait(TMC_ACK_SIG);
    (void) rex_clr_sigs(rex_self(), TMC_ACK_SIG);
  #endif /* FEATURE_QIPCALL */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/



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
LOCAL void                       tmc_libraries_init_after_tasks_start( void )
{
  #ifdef FEATURE_MP4_DECODER

    /* MP4 - okay to sleep. We do this here since the MP4 task init is not
    ** carried out until a media clip is played. 
    */
    SLEEP_ASSERT_OKTS ( SLEEP_MP4_OKTS_SIG );
  
  #endif /* FEATURE_MP4_DECODER */

  #ifdef FEATURE_MDDI
#error code not present
  #endif
  #ifdef FEATURE_CAMERA
    SLEEP_ASSERT_OKTS ( SLEEP_CAMERA_OKTS_SIG );
  #endif

 
  #if defined (FEATURE_RUNTIME_DEVMAP) && (!defined (IMAGE_APPS_PROC) || defined (FEATURE_STANDALONE))
    #if defined(FEATURE_EFS)
    /* Get any previous port mappings on behalf of the Runtime Device Mapper
    */
    rdm_get_prev_ports();
    #endif
  #endif /*defined (FEATURE_RUNTIME_DEVMAP) && (!defined (IMAGE_APPS_PROC) || defined (FEATURE_STANDALONE)) */

  #if defined (FEATURE_RUNTIME_DEVMAP) && (defined (FEATURE_DATA_ON_APPS) || defined (FEATURE_USB_ON_APPS_PROC))
    /* Have RDM issue any open commands. If there were none retrieved from
    ** EFS, compile time defaults will be issued
    */
    rdm_issue_opens();
  #endif /*defined (FEATURE_RUNTIME_DEVMAP) && (!defined (IMAGE_APPS_PROC) || defined (FEATURE_STANDALONE)) */

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
LOCAL void                        tmc_init( void )
{
  boolean status;                        /* Keep record of initial status. */

  /* Set TMC State to POWER OFF.
  */
  tmc_set_state( TMC_PWROFF);

  /* Initialize queues and timers.
  */
  tmc_module_init();

  /* Initialize libraries before task start
  */
  tmc_libraries_init_before_tasks_start();

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

  /* Initialize libraries after service tasks start.
  */
  status = tmc_libraries_init_after_service_tasks_start();

#ifdef FEATURE_QIDS_RPC
  qids_rpc_app_init();
#endif /* FEATURE_QIDS_RPC */

#if defined (FEATURE_USB_CDC_ACM) || \
    defined (FEATURE_USB_DIAG)    || \
    defined (FEATURE_USB_DIAG_NMEA) || \
    defined (FEATURE_USB) && defined(FEATURE_MMC)
  // Currently done in MC.
  sio_usb_dev_init();
#endif 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_USB

    #ifdef TIMETEST
      usbdc_tcb.leds = TIMETEST_USBDC_TASK_ID;
    #endif /* TIMETEST */

    #ifdef FEATURE_TMC_STACK_PROFILING
      for (i = 0; i < USBDC_STACK_SIZ; i++) {
        usbdc_stack[i] = TMC_STACK_PROFILING_DWORD;
      }
    #endif

    (void) strcpy(usbdc_tcb.task_name, "USBSDC");

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

      (void) strcpy(usbhc_urb_tcb.task_name, "USBHC_URB");

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

      (void) strcpy(usbhc_dev_tcb.task_name, "USBHC_DEV");

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

      (void) strcpy(usbhc_test_tcb.task_name, "USBHC_TEST");

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
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  #ifndef FEATURE_BRINGUP_DIAG_ONLY
  #ifdef FEATURE_USB
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
  #endif /* FEATURE_BRINGUP_DIAG_ONLY */

  if (status == FALSE) {

   /* Set TMC State to OFFLINE.
   */
   tmc_set_state( TMC_OFFLINE_IF_NOT_FTM);
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
boolean                           tmc_cmd(
        tmc_cmd_e_type        cmd
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
tmc_state_e_type                  tmc_get_state( void )
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
dword                            tmc_get_stored_esn_me (void)
{
  return (tmc_data.esn_me);
} /* tmc_get_stored_esn_me */
#endif /* FEATURE_UIM_RUIM */


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

dword                            tmc_get_mem_heap_size (
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
#ifndef FEATURE_RRC_SIB_HEAP
    case TMC_MEM_HEAP_TYPE_SMALL_BUF:
      heap_size = TMC_HEAP_SMALL_MEM_BUFFER_SIZE;
      break;
#endif /* FEATURE_RRC_SIB_HEAP */
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
  been created.  It then waits for the start signal back to it. When it gets
  the start signal it acknowledges it back to TMC and it then returns to the
  caller. The caller task is now formally started and it can start normal
  application processing.


DEPENDENCIES
  TMC task has to be started.

RETURN VALUE
  void

SIDE EFFECTS
  The calling task stays here until it receives the START signal.

===========================================================================*/
void                              tmc_task_start( void )
{
  /* Acknowledge task creation.
  */
  (void) rex_set_sigs( &tmc_tcb, TMC_ACK_SIG );

  /* Wait for the start signal
  */
  rex_wait( TASK_START_SIG );

  /* Clear task start signal.
  */
  rex_clr_sigs(rex_self(), TASK_START_SIG);

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
void                              tmc_task_ack( void )
{
  rex_set_sigs(&tmc_tcb, TMC_ACK_SIG);
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
void                              tmc_task_stop( void )
{
  rex_set_sigs(&tmc_tcb, TMC_ACK_SIG);
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
void                              tmc_task_offline( void )
{
  rex_set_sigs(&tmc_tcb, TMC_ACK_SIG);
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
boolean                           tmc_reset_counter_read(
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
boolean                           tmc_reset_counter_reset(void)
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
void                              tmc_task(
    dword ignored
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

  /* Define requested sigs */
  requested_sigs =

      /* Signal for TMC commands */
      TMC_CMD_Q_SIG

      /* Add other signals here */
      ;

  /* Loop forever.
  */
  while( TRUE )
  {
    /* Perform the rex wait.
    */
    received_sigs = rex_wait( requested_sigs );

    /* Process command signal and update state of TMC task.
    */
    if (received_sigs & TMC_CMD_Q_SIG) {
      (void) rex_clr_sigs( rex_self(), TMC_CMD_Q_SIG );
      tmc_dispatch_commands();
    }

    /* Add processing other signals here */


  }

} /* tmc_task */

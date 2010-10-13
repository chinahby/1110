
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                    W A T C H D O G     T A S K

GENERAL DESCRIPTION
  The Watchdog task monitors the other software tasks to ensure that they
  execute properly and prevents the watchdog timer from resetting the
  microprocessor.

EXTERNALIZED FUNCTIONS
  dog_report
    Report to the Watchdog task, so that it can tell that the monitored
    task is still functioning properly.
  dog_autokick
    This routine enables/disables the hardware watchdog auto-kick function.
  dog_kick
    This routine kicks the hardware watchdog.  This is not to be used except
    in unusual start-up/shutdown conditions.
  dog_task
    This is the entry procedure for the Watchdog task called by rex_def_task.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Dog_task must be defined (rex_def_task) prior to all other tasks which
  are to be monitored, and signaled to start after the last task has started.
  The reason for doing this is to allow the Watchdog task to start a timer
  when the task is initialized.  If the timer expires before the task is
  started, the Watchdog task initiates fatal error processing.

Copyright (c) 1990, 1992 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1993, 1994 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1995, 1996 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1997, 1998 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1999, 2000 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2001, 2002 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2003, 2004 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2005, 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc1100/services/task/dog.c#11 $ $DateTime: 2009/05/11 10:35:07 $ $Author: ehong $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/27/08   APU	   Added support for FEATURE_VBATT_TASK.	
07/02/08   hs      Added support for FEATURE_CHG_TASK.
11/30/07   rjm     FEATURE_FM switch now independent of ODM builds
06/18/07   va      Added TCXO Manager Task
09/25/06   pc      Increased timeout value for sleep task.
09/05/06   jn      Disable monitoring of the Camera Driver task.
08/29/06   pc      Added support for FEATURE_EXT_USB_P1
08/14/06   pc      Added feature FEATURE_QTV_STATIC_TASKS.
08/09/06   pc      Added support for QTV_VIDEO_RENDERER and QTV_AUDIO
07/25/06   pc      Added support for FEATURE_CPU_BASED_FLOW_CONTROL
07/17/06   pc      Added support for FEATURE_DMTASK.
06/19/06   pc      Added support for FEATURE_SEC_USFS.
05/05/06   ac      Added support for FEATURE_QDJTEST
04/24/06   bt      Added support for UBM L1 task.
03/24/06   ptm     Included bio_led.h for 7K.
03/27/06   ddh     Modified dog_autokick to FREEZE the WDOG for 7500.
03/24/06   pc      Added support for SECRND task
03/23/06   cl      Added PDSM_COMM_TASK
03/23/06   xz      Fixed the dog timeout issue for QVP in 7500 branch
01/23/06   rlp     Added HID Support.
01/02/06   dle     Added FEATURE_CAMERA_DRV_TASK.
12/22/05   ~SN     Added feature DOG KEEPALIVE
12/10/05   pc      Introduced changes under FEATURE_L4_OPT_DOG
11/28/05   pc      Doubled the dog time-out period for Sleep task for 6275.
11/10/05   pc      Added support for AVS_SBCENC_TASK
11/08/05   pc      Added support for GSM_PLT task
10/13/05   ck      Increased the size of DOG_MAX_TASK_NAME
09/12/05   pc      Added support for FEATURE_USB_HOST_ENHANCED.
08/26/05   pc      Added support for DISP_TASK.
07/14/05   bhd     Added ATI graphics task support
07/13/05   adm     Merged in changes related to supporting BootLoader compilation
                   for L4 builds
06/12/06   hba     Added support for WLAN Sec task.
06/02/05   pc      Removed inclusion of dmod.h
05/28/05   grl     Added support for run-time sw watchdog disabling.
05/26/05   hcg     Extended timeouts for lower priorty tasks for FEATURE_L4 build.
04/29/05   ibm     Added support for heap tracker.
04/26/05   sr      Added a delay of 120us between kicking of watchdog and write
                   to autokick register for MSM6275
04/21/05   pc      Added support for FEATURE_MFLO
04/09/05   pc      Added support for CNV task
04/12/05   rp      Added support for MSM6800
03/15/05   wal     Added APS task for L4/Linux under FEATURE_APS
03/15/05   adg     Added QDIAG task for L4/Linux under FEATURE_IG_QDIAG
03/15/05   jqi     Added support for FS_FEATURE_RMTEFS_SERVER, FEATURE_L4
                   FEATURE_PM_SERVER,FEATURE_IG_UI_CLIENT_SELECT
02/29/05   jqi      Lint fix.
02/24/05 t_jarmis  Added FEATURE_FM for FM-RDS Radio.
           jqi     fixed compilation error.
02/22/05   jqi     Lint fix.
02/10/05   sr      Featurized correctly for WPLT
02/05/05   aku     Added support for WLAN tasks.
02/04/05   dwp     Added QVP support for SIP.
02/03/05   epr     Increase the batery manager timeout as it is a low priority task
02/03/05   epr     Addition of Sirius specific tasks using define SIRIUS_PLATFORM
01/31/05   jqi     Revised FEATURE_MGPCC_TASK report ID
                   Mainline MSMHW_GSM_BONDING for 6500 target.
01/21/05   ld/sr   Added support MGPCC task (needed for GPS). Added support for
                   MSM6275
01/12/05   jqi     Lint fixes.
01/06/05   aar     Added support for GPSSRCH demod task.
01/05/05   pa      Increased PBM watchdog timeout.
12/13/04   tbg     Merged in changes for FEATURE_SOFTWARE_PROFILE_CPU_STARV_DET
11/19/04   jz      Added QVP VFE Task Dog
11/19/04   jqi     Fixed compilation warnings.
11/18/04   pa      Added support for PBM task.
10/22/04   jqi     Featurization fixes.
10/15/04   rp      Added support for page and interupt tasks.
           jqi     Increased CB dog time out counter.
10/11/04   ck      Turned off autokicker for dog_autokick(FALSE) also
10/08/04   jqi     Added support for FEATURE_HSDPA.
09/29/04   ss      Added support for DTV (Digital TV)
09/28/04   jqi     Fixed ADS1.2 and RVCT2.1 compilation error.
07/06/04    hl     Added support for FEATURE_JOYST_TASK
08/10/04   sr      RF API called to turn off PA when dog_init fails needs to
                   assume that RF Multimode is not initialised and also needs
                   to ensure that MDSP clocks are turned on and the correct
                   MDSP image is loaded. rf_disable_wcdma_tx is replaced with
                   rf_turn_off_pa in all places where dog does an err_Fatal.
06/04/04   bt      Changed 50us wait before starting autokick to 120us.
05/21/04   vr      Added BCDB and HDRBC tasks.
           mykevin Added FEATURE_UI_CORE_REMOVED.
04/05/04   sr      Made writes to SLEEP_CTL register - to kick hw watchdog -
                   atomic by enclosing within INTLOCK/INTFREE
04/01/04   sg      removed support for LP task as task no longer exists.
02/23/04   balas   Added support for Bluetooth External Profiles feature support .
02/19/04   Jayanth Added support for FEATURE_SEC_DRM.
02/13/04   jqi     Back out some changes related to turn off PA.
02/12/04   vdrapk  Added support for FEATURE_GSM_CB.
02/10/04   jqi     Call rf PAI to turn off PA when there is a dog startup
                   timeout for UMTS target.Unified enc API.
02/06/04   jqi     Removed turn off PA when there is a dog startup timeout
                   for UMTS target.
12/05/03    sr     Lock interrupts when writing to NV.
11/07/03    dd     Added support for GPS Search task.
10/03/03   ajn     Removed MSM5500's HDR autokicker & related code
10/13/03   dwp     Added initial QVPHONE support.
10/13/03   jqi     Fixed compile warning.
09/08/03    sr     Added feature T_SLEEP in addition to FEATURE_DOG_IGNORE_SLEEP_TASK
08/18/03    gr     Fixed a bug in the asynchronous calls to the filesystem
                   functions.
08/14/03    sr     Switch FS task priority upon err_fatal to one below DOG task
08/11/03    sr     WCDMA sleep timer when doing cell reselection causes
                   DOG Monitor to expire. Added more checks to catch this
                   condition.
07/23/03    sr     Disable GPTimer when dog times out on a task.
07/23/03    bs     Merged ThinUI changes from 6200 archive. This is featurized
                   as FEATURE_THIN_UI.
07/21/03   mk/drl  Added dog_gsm_autokick.
07/11/03    sr     Added feature FEATURE_DOG_IGNORE_SLEEP_TASK to ignore if
                   Sleep task does not pet the watchdog.
07/11/03    gr     Made the filesystem calls that are part of
                   FEATURE_DOG_FATAL_NV_REPORT asynchronous to avoid the
                   possibility of a timeout causing a hardware watchdog reset.
07/08/03   drl/mk  Added 6300 sleep support
07/08/03    sr     Added ability to monitor by way of an interrupt whether dog
                   task is getting time to run or not. If not then err_fatal
                   is called. FEATURE_MONITOR_DOG needs to be used. Currently
                   to be used only on MSM6200
06/20/03   bgc     Added autokicker for GSM to dog_autokick().
06/17/03    aw     Increased UI dog timeouts to 40secs form 20secs.
05/29/03   Om      Added support for ARM_CRYPTO and IPSEC tasks,
05/28/03   TMR/dgy Added changes for WCDMA single-mode build.
05/14/03    ld     Added support for MGP related tasks
04/30/03    sr     Added feature FEATURE_DOG_FATAL_NV_REPORT to write out the
                   dog task information to the flash when a task times out.
04/18/03    sr     Removed inclusion of ftm.h and also reference to T_RF_ZRF6200
                   as the differentiating condition to include 6200 specific
                   code.
03/31/03   jqi     Increased UI, CM, WMS dog timeouts to 20secs from 10seconds.
03/24/03   jqi     Increased GSTK dog time out.
03/18/03   jqi     Reverse back TMC MMOC entry, added feature around UI and
                   HS task in dog_task_info.
03/14/03   jqi     Added GSM/WCDMA feature around TMC task.
03/12/03   djd     Added support for GSM watchdog.
03/07/03    gr     Added support for changing the initial startup time allowed
                   before the dog becomes active. This is a way to handle
                   situations with long initialization times (ex: EFS startup
                   on an empty filesystem).
03/04/03    lz     Removed FEATURE_MOBILE_SUITE.
02/21/03   jay     Added support for SSL task and virtual SAT task.
02/18/03   jqi/jar Added Support for the Generic SIM Toolkit Task (FEATURE_GSTK)
02/14/03   jqi     Added support for FTM task.
02/06/03   jqi     FTM support with all tasks start for UMTS target.
02/04/03   jqi     Fixed WCDMA call drops by merging dog_autokick().
01/15/03   jqi     Merged with msmshared/umts/services/task/dog.c#48.
                   1. do not check dog_auto_kicking for 6200 target in dog_kick().
                   2. added a temporary support, do not check task timeout
                   for UMTS ftm unless we can start all tasks in ftm mode.
12/31/02   jqi     Merged with msmshared/umts/services/task/dog.c#47
                   1.Added support for FEATURE_ID_FATAL_TASK.
                   2.Added support for 6200 target.
12/10/02    lz     Added dog_task_info[] entry for Mobile Suite task under
                   FEATURE_MOBILE_SUITE.
12/04/02   lam     Added support for HWTC.
11/19/02   jqi     Remove FTM support for GSM feature.
10/25/02   jqi     Featurized HWIO_OUTM
10/24/02   jqi     Clean up dog_kick and dog_autokick to support various
                   Targets.
10/18/02   jqi     Added FTM support for GSM feature.
09/23/02   jqi     Change the misspelt feature name FEATURE_WCDMA.
09/19/02   ck      Changed the misspelt feature name FEATURE_CDAM_1900
09/18/02   jqi     Moved to MSMSAHRED/SERVICES/TASK directory.
09/16/02   jqi     Merged COMMON with MSMSAHRED/UMTS archive.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "target.h"             /* Target specific definitions          */
#ifdef CUST_H
   #include "customer.h"        /* Customer Specific Definitions        */
#endif
#include "comdef.h"             /* Definitions for byte, word, etc.     */
#include "rex.h"                /* definition of REX data types         */
#include "task.h"               /* Definition for mc_tcb                */
#include "bio.h"                /* BIO defines for error LEDs           */
#ifdef T_MSM7500
#error code not present
#endif
#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900) || defined(FEATURE_ACP) || defined(FEATURE_HDR))
#include "dmod.h"               /* Dmod macro/define to pacify watchdog */
#endif
#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900) || defined(FEATURE_ACP) || defined(FEATURE_WCDMA))
#include "enc.h"                /* Prototype for enc_kill_pa()          */
#endif
#include "err.h"                /* Error logging macros                 */
#include "assert.h"             /* Definition of the ASSERT macro       */
#include "dog.h"                /* Watchdog typedefs and prototypes     */

#ifdef FEATURE_APPS_KEEPALIVE
#include "dog_keepalive.h"
#endif

#ifdef FEATURE_DOG_FATAL_NV_REPORT
#include "fs.h"
#include <string.h>
#endif

#if (defined(FEATURE_DOG_FATAL_NV_REPORT) || defined(FEATURE_ID_FATAL_TASK))
#include <stdio.h>
#endif

#ifdef FEATURE_MONITOR_DOG
   #include "sleepdrv.h"
#endif

#ifdef FEATURE_WCDMA
#error code not present
#endif

#ifdef FEATURE_IG_UI_CLIENT_SELECT
#include "hw.h"
#endif

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/* #define NO_DOG to disable DOG function */

/* This specifies, in milliseconds, the maximum length of the grace period
** the Watchdog Task allows during system initialization and startup.
*/
#define DOG_MAX_STARTUP_TIME    800000

/* This specifies, in milliseconds, the default length of the grace period
** the Watchdog Task allows during system initialization and startup.
*/

#if (defined (FEATURE_L4) || defined (FEATURE_BREW_HEAP_TRACKER))
  #define DOG_DEFAULT_STARTUP_TIME    800000
#else
  #define DOG_DEFAULT_STARTUP_TIME    40000
#endif

/* Number of milliseconds between each wakeup of the Watchdog Task.
** This value is dependent on how often we must reset the watchdog
** timer circuit.  It has nothing to do with how often tasks report.
*/
#define DOG_WAKEUP_TIME         100

/* Number of milliseconds between each check of the all the task counters.
** Should be an integer multiple of DOG_WAKEUP_TIME.
*/
#define DOG_CHECK_TIME          1000

/* Number of DOG_WAKEUP_TIME intervals between each check of the task
** counters.
*/
#define CHECK_CNT               (DOG_CHECK_TIME / DOG_WAKEUP_TIME)

/* Convert time in milliseconds to appropriate number of counter ticks
*/
#ifdef FEATURE_L4_OPT_DOG
#error code not present
#else
#define DOG_MS_TO_COUNTS(t)     ((( t ) / DOG_CHECK_TIME ) + 1 )
#endif

/* Maximum length of a task name in the dog_task_info array.
*/
#define DOG_MAX_TASK_NAME REX_TASK_NAME_LEN

#ifdef FEATURE_DOG_FATAL_NV_REPORT
  /* Time in milliseconds that the Dog task will wait for the FS task to
  ** complete an operation.
  */
  #define DOG_FS_TMO 5000
#endif
/*lint ++flb */ /* turn on library flag */
/* Structure for storing task name and timeout interval */
typedef struct {
  char              task[DOG_MAX_TASK_NAME+1];
#ifdef FEATURE_L4_OPT_DOG
#error code not present
#else
  byte              timeout;
#endif /*FEATURE_L4_OPT_DOG*/
  rex_priority_type priority;
} dog_task_info_type;

/*lint --flb */
/* Structure for storing the current state of a task */
typedef struct {
#ifdef FEATURE_L4_OPT_DOG
#error code not present
#else
  byte    count;
#endif /*FEATURE_L4_OPT_DOG*/
  boolean is_blocked;
} dog_task_state_type;

#ifdef FEATURE_MONITOR_DOG
/* 300MS is the dog monitor timeout value. If dog task does not run
   for 300ms then this timer will alert the AMSS code
*/
LOCAL uint32 dog_monitor_timeout = 300;
#endif

/* Timer for waking up and resetting the watchdog circuit
*/
LOCAL rex_timer_type dog_wakeup_timer;

/* Whether or not the watchdog is being auto-kicked by the MSM.
** Avoid kicking while the MSM is auto-kicking, as this causes
** the MSM to stop auto-kicking AND does not kick (MSM 1.0 bug).
*/
LOCAL boolean dog_auto_kicking = FALSE;

/* This specifies, in milliseconds, the length of the grace period the Watchdog
** Task allows during system initialization and startup. After initialization
** is complete, the main control task signals the Watchdog Task which then
** begins begins its normal checking to make sure all tasks are reporting in at
** regular intervals. If this grace period expires before a signal is received
** from the Main Control Task, then the watchdog resets the system.
*/
static uint32 dog_startup_time = DOG_DEFAULT_STARTUP_TIME;

/* Number of DOG_WAKEUP_TIME intervals allowed between defining the
** Watchdog Task with rex_def_task, and sending the startup signal.
** Since MC defines this task first and starts it last, this is the
** timeout for the system getting the tasks initialized.
*/
static uint32 dog_startup_cnt = DOG_DEFAULT_STARTUP_TIME / DOG_WAKEUP_TIME;

/* This will always be true unless turned off from the debugger.
** This refers to the software watchdog.  This variable has no
** impact on the HW watchdog.
*/
static boolean dog_enabled = TRUE;

/* Timeout times for each task, and a string to associate with each task.
** The string will be dislayed by err_fatal if its task times out.
** Each timeout item contains the total number of counts the associated
** task has to report in. The index corresponding to each task in this
** array must equal the value of the DOG_XXX_RPT constant in dog.h that
** corresponds to the task. For instance, DOG_RX_RPT has the value 2, so
** the entry for the RX task should be at array index 2.
**
** Note that the descriptive string can be at most DOG_MAX_TASK_NAME
** characters long.
*/

/* crude scale factor for L4 timeout extension.  Needs tuning! */
#ifdef FEATURE_L4
#error code not present
#else
  #define CNT_SCALE  1
#endif

LOCAL const dog_task_info_type dog_task_info[] = {

/* Configure in/out monitoring of these tasks,
** based on whether FEATURE_HWTC it is there or not.
*/
#ifndef FEATURE_HWTC
  #if ((!defined FEATURE_MULTIPROCESSOR) || (defined IMAGE_MODEM_PROC)) && !(defined FEATURE_BRINGUP_DIAG_ONLY) && !(defined FEATURE_GSM_PLT) && !(defined FEATURE_WCDMA_PLT)
  { "SND",  DOG_MS_TO_COUNTS(  2000 ), SND_PRI },       /* SND_COUNTS    */
  #else
  { "", 0, 0 },                                   /* Dummy Entry   */
  #endif
  #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900) || defined(FEATURE_ACP)) && \
      ((!defined FEATURE_MULTIPROCESSOR) || (defined IMAGE_MODEM_PROC)) && \
      (!defined FEATURE_BRINGUP_DIAG_ONLY)
  { "TX",   DOG_MS_TO_COUNTS(  1000 ), TX_PRI  },       /* TX_COUNTS     */
  { "RX",   DOG_MS_TO_COUNTS(  1000 ), RX_PRI  },       /* RX_COUNTS     */
  { "SRCH", DOG_MS_TO_COUNTS( 10000 ), SRCH_PRI },       /* SRCH_COUNTS   */
  { "RXTX", DOG_MS_TO_COUNTS(  1000 ), RXTX_PRI },       /* RXTX_COUNTS   */
  { "MC",   DOG_MS_TO_COUNTS(  1000 ), MC_PRI },       /* MC_COUNTS     */
  #else
  { "", 0, 0 },                                   /* Dummy Entry   */
  { "", 0, 0 },                                   /* Dummy Entry   */
  { "", 0, 0 },                                   /* Dummy Entry   */
  { "", 0, 0 },                                   /* Dummy Entry   */
  { "", 0, 0 },                                   /* Dummy Entry   */
  #endif
  #if defined(FEATURE_THIN_UI ) || defined(FEATURE_UI_CORE_REMOVED)
  { "", 0, 0 },                                   /* Dummy Entry because HS is removed */
  #else
     #ifdef FEATURE_HS
     { "HS",   DOG_MS_TO_COUNTS( 10000 ), HS_PRI },  /* HS_COUNTS     */
     #else
     { "", 0, 0 },                                   /* Dummy Entry   */
     #endif
  #endif /* FEATURE_THIN_UI || FEATURE_UI_CORE_REMOVED */
  { "DIAG", DOG_MS_TO_COUNTS( 10000 ), DIAG_PRI },       /* DIAG_COUNTS   */
  #ifdef FEATURE_UI
  #ifdef FEATURE_BREW_HEAP_TRACKER
  { "UI",   DOG_MS_TO_COUNTS( 120000 ), UI_PRI },       /* UI_COUNTS     */
  #else
  { "UI",   DOG_MS_TO_COUNTS( 40000 ), UI_PRI },       /* UI_COUNTS     */
  #endif /* FEATURE_BREW_HEAP_TRACKER */
  #else
  { "", 0, 0 },                                   /* Dummy Entry   */
  #endif
  { "NV",   DOG_MS_TO_COUNTS( 59000 ), NV_PRI },       /* NV_COUNTS     */
#else
#error code not present
#endif /* FEATURE_HWTC */


/* Entry for the IDS task, which is now obsolete.
*/
  { "", 0, 0 },                                   /* Dummy Entry   */

/* Configure in/out monitoring of the Authentication task,
** based on whether it is there or not.
*/
#ifdef FEATURE_AUTH
  { "AUTH", DOG_MS_TO_COUNTS( 59000*CNT_SCALE ), AUTH_PRI },       /* AUTH_COUNTS   */
#else
  { "", 0, 0 },                                   /* Dummy Entry   */
#endif

/* Configure in/out monitoring of the Data Services tasks,
** based on whether they are there or not.
*/
#if (defined(FEATURE_DS) || defined(FEATURE_DATA))
  #if ((!defined FEATURE_MULTIPROCESSOR) || (defined IMAGE_MODEM_PROC))
  { "DS",    DOG_MS_TO_COUNTS( 59000*CNT_SCALE ), DS_PRI },       /* DS_COUNTS    */
  #else
#error code not present
  #endif 
  #ifdef FEATURE_DATA_PS
    { "PS",    DOG_MS_TO_COUNTS( 59000*CNT_SCALE ), PS_PRI },       /* PS_COUNTS    */
  #else
    { "", 0, 0 },                                  /* Dummy Entry     */
  #endif

  #ifdef FEATURE_DS_SOCKETS_TEST_TCP
#error code not present
  #else
    { "", 0, 0 },                                  /* Dummy Entry     */
  #endif
#else
  { "", 0, 0 },                                    /* Dummy Entry   */
  { "", 0, 0 },                                    /* Dummy Entry   */
  { "", 0, 0 },                                    /* Dummy Entry   */
#endif

/* Configure in/out monitoring of the sleep task,
** based on whether it is there or not.
*/
#ifdef T_SLEEP
#if (defined(T_MSM6275))
#error code not present
#else
  { "SLEEP", DOG_MS_TO_COUNTS(59000*CNT_SCALE ), SLEEP_PRI },       /* SLEEP_COUNTS */
#endif
#else
  { "", 0, 0 },                                    /* Dummy Entry   */
#endif

/* Configure in/out monitoring of the QMIP task,
** based on whether it is there or not.
*/
#ifdef FEATURE_QMIP
#error code not present
#else
  { "", 0, 0 },                                   /* Dummy Entry   */
#endif

/* Configure in/out monitoring of the Diffie-Helman task,
** based on whether it is there or not.
*/
#if defined FEATURE_DH && \
            ((!defined FEATURE_MULTIPROCESSOR) || (defined IMAGE_MODEM_PROC))
  { "DH",   DOG_MS_TO_COUNTS( 59000*CNT_SCALE ), DH_PRI },       /* DH_COUNTS     */
#else
  { "", 0, 0 },                                   /* Dummy Entry   */
#endif

/* Configure in/out monitoring of the VR task,
** based on whether it is there or not.
*/
#if defined FEATURE_VS_TASK && \
            ((!defined FEATURE_MULTIPROCESSOR) || (defined IMAGE_MODEM_PROC))
  { "VS",   DOG_MS_TO_COUNTS( 59000*CNT_SCALE ), VS_PRI },       /* VS_COUNTS     */
#else
  { "", 0, 0 },                                   /* Dummy Entry   */
#endif

/* Configure in/out monitoring of the Graphics task,
** based on whether it is there or not.
*/
#if defined FEATURE_GRAPH_TASK && \
            ((!defined FEATURE_MULTIPROCESSOR) || (defined IMAGE_APPS_PROC))

  { "GRAPH",   DOG_MS_TO_COUNTS( 59000*CNT_SCALE ), GRAPH_PRI },    /* GRAPH_COUNTS  */
#else
  { "", 0, 0 },                                   /* Dummy Entry   */
#endif

/* Configure in/out monitoring of the EFS task,
** based on whether it is there or not.
*/
#ifdef FEATURE_EFS
  { "EFS",  DOG_MS_TO_COUNTS( 59000 ), FS_PRI },       /* FS_COUNTS     */
#else
  { "", 0, 0 },                                   /* Dummy Entry   */
#endif

/* Configure in/out monitoring of the Call Manager task,
** based on whether it is there or not.
*/
#if defined FEATURE_CM && \
            ((!defined FEATURE_MULTIPROCESSOR) || (defined IMAGE_MODEM_PROC))
  { "CM",   DOG_MS_TO_COUNTS( 20000 ), CM_PRI },       /* CM_COUNTS     */
#else
  { "", 0, 0 },                                   /* Dummy Entry   */
#endif

/* Configure in/out monitoring of the Vocoder task,
** based on whether it is there or not.
*/
#if defined FEATURE_VOC_TASK && \
            ((!defined FEATURE_MULTIPROCESSOR) || (defined IMAGE_MODEM_PROC))
  { "VOC",  DOG_MS_TO_COUNTS( 10000 ), VOC_PRI },       /* VOC_COUNTS    */
#else
#error code not present
#endif

/* Configure in/out monitoring of the GPS tasks,
** based on whether they are there or not.
*/
#if (defined(FEATURE_GPSONE) && !defined(FEATURE_CGPS)) && \
            ((!defined FEATURE_MULTIPROCESSOR) || (defined IMAGE_MODEM_PROC))
#error code not present
#else
  { "", 0, 0 },                                    /* Dummy Entry   */
  { "", 0, 0 },                                    /* Dummy Entry   */
  { "", 0, 0 },                                    /* Dummy Entry   */
  { "", 0, 0 },                                    /* Dummy Entry   */
  { "", 0, 0 },                                    /* Dummy Entry   */
#endif

/* Configure in/out monitoring of the QDSP task,
** based on whether it is there or not.
*/
#ifdef FEATURE_QDSP_TASK
  { "QDSP",  DOG_MS_TO_COUNTS( 10000 ), QDSP_PRI },       /* QDSP_COUNTS  */
#else
  { "", 0, 0 },                                    /* Dummy Entry   */
#endif

/* Configure in/out monitoring of the USB task,
** based on whether it is there or not.
*/
#if defined (FEATURE_USB) && !defined (FEATURE_NO_USB_HW_ON_PROCESSOR)
  { "USB",  DOG_MS_TO_COUNTS( 59000 ), USBDC_PRI },       /* USB_COUNTS    */
#else
  { "", 0, 0 },                                   /* Dummy Entry   */
#endif

/* Configure in/out monitoring of the SFAT task,
** based on whether it is there or not.
*/
#ifdef FEATURE_EFS_SFAT
  { "SFAT",  DOG_MS_TO_COUNTS( 59000 ), SFAT_PRI },      /* SFAT_COUNTS   */
#else
  { "", 0, 0 },                                   /* Dummy Entry   */
#endif

/* Configure in/out monitoring of the Bluetooth task,
** based on whether it is there or not.
*/
#if defined FEATURE_BT && \
            ((!defined FEATURE_MULTIPROCESSOR) || (defined IMAGE_MODEM_PROC))
  { "BT",  DOG_MS_TO_COUNTS( 10000 ), BT_PRI },        /* BT_COUNTS     */
#else
  { "", 0, 0 },                                   /* Dummy Entry   */
#endif

/* Configure in/out monitoring of the IS2000 TDSO task,
** based on whether it is there or not.
*/
#if defined FEATURE_IS2000_TDSO && \
            ((!defined FEATURE_MULTIPROCESSOR) || (defined IMAGE_MODEM_PROC))
  { "TDSO",  DOG_MS_TO_COUNTS( 10000 ), TDSO_PRI },      /* TDSO_COUNTS   */
#else
  { "", 0, 0 },                                   /* Dummy Entry   */
#endif

/* Configure in/out monitoring of the Security Services task,
** based on whether it is there or not.
*/
#ifdef FEATURE_SEC
  { "SEC",  DOG_MS_TO_COUNTS( 59000*CNT_SCALE ), SEC_PRI },       /* SEC_COUNTS    */
#else
  { "", 0, 0 },                                   /* Dummy Entry   */
#endif

/* Configure in/out monitoring of the UIM task,
** based on whether it is there or not.
*/
#if defined(FEATURE_UIM) && !defined(FEATURE_HWTC) && !defined(FEATURE_GSM_PLT)
  { "UIM",  DOG_MS_TO_COUNTS( 59000 ), UIM_PRI },       /* UIM_COUNTS    */
#else
  { "", 0, 0 },                                   /* Dummy Entry   */
#endif

/* Configure in/out monitoring of the HDR tasks,
** based on whether they are there or not.
*/
#if defined FEATURE_HDR && \
            ((!defined FEATURE_MULTIPROCESSOR) || (defined IMAGE_MODEM_PROC))
#error code not present
#else
  { "", 0, 0 },                                     /* Dummy Entry     */
  { "", 0, 0 },                                     /* Dummy Entry     */
  { "", 0, 0 },                                     /* Dummy Entry     */
  { "", 0, 0 },                                     /* Dummy Entry     */
#endif

/* Configure in/out monitoring of the HWTC task,
** based on whether it is there or not.
*/
#ifdef FEATURE_HWTC
#error code not present
#else
  { "", 0, 0 },                                   /* Dummy Entry   */
#endif /* FEATURE_HWTC */

/* Configure in/out monitoring of the WMS task,
** based on whether it is there or not.
*/
#if (defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS)) && \
    ((!defined FEATURE_MULTIPROCESSOR) || (defined IMAGE_MODEM_PROC))
  { "WMS",   DOG_MS_TO_COUNTS( 20000 ), WMS_PRI },       /* WMS_COUNTS     */
#else
  { "", 0, 0 },                                   /* Dummy Entry   */
#endif

/* Configure in/out monitoring of the TMC tasks,
** based on whether they are there or not.
*/
#if (defined(FEATURE_MULTIMODE_ARCH) || defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
  { "TMC",   DOG_MS_TO_COUNTS( 10000 ), TMC_PRI },          /* TMC_COUNTS     */
#else
  { "", 0, 0 },                                      /* Dummy Entry    */
#endif

/* Configure in/out monitoring of the MULTIMODE ARCH tasks,
** based on whether they are there or not.
*/
#if (defined(FEATURE_MULTIMODE_ARCH) && !defined(FEATURE_HWTC)) && \
    ((!defined FEATURE_MULTIPROCESSOR) || (defined IMAGE_MODEM_PROC))
  { "MMoC",  DOG_MS_TO_COUNTS( 10000 ), MMOC_PRI },          /* MMoC_COUNTS    */
#else
  { "", 0, 0 },                                      /* Dummy Entry    */
#endif

/* Configure in/out monitoring of the GSDI task,
** based on whether it is there or not.
*/
#if (defined(FEATURE_MMGSDI) || defined(FEATURE_NAS) || defined(FEATURE_VIRTUAL_SIM)) && \
    ((!defined FEATURE_MULTIPROCESSOR) || (defined IMAGE_MODEM_PROC))
  { "GSDI",  DOG_MS_TO_COUNTS( 59000 ), GSDI_PRI },       /* GSDI_COUNTS    */
#else
  { "", 0, 0 },                                    /* Dummy Entry   */
#endif

/* Configure in/out monitoring of the GSDI task,
** based on whether it is there or not.
*/
#if defined FEATURE_MDSP_TASK && \
            ((!defined FEATURE_MULTIPROCESSOR) || (defined IMAGE_MODEM_PROC))
  { "MDSP",  DOG_MS_TO_COUNTS( 59000 ), MDSP_PRI },       /* MDSP_COUNTS    */
#else
  { "", 0, 0 },                                    /* Dummy Entry   */
#endif

#if ( (defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS)) && !defined(FEATURE_WCDMA_PLT) && !defined (FEATURE_GSM_PLT) )
  { "GCSD",  DOG_MS_TO_COUNTS( 10000 ), DS_GCSD_PRI },      /* GCSD_COUNTS    */
#else
  { "", 0, 0 },
#endif

#if (defined (FEATURE_GSM) && !defined (FEATURE_WCDMA_PLT) && !defined(FEATURE_HWTC) && !defined (FEATURE_GSM_PLT))
#error code not present
#else
  { "", 0, 0 },
  { "", 0, 0 },
#endif

#if (defined (FEATURE_GSM_GPRS) && !defined (FEATURE_WCDMA_PLT) && !defined (FEATURE_GSM_PLT))
#error code not present
#else
  { "", 0, 0 },
  { "", 0, 0 },
  { "", 0, 0 },
  { "", 0, 0 },
#endif /* FEATURE_GSM_GPRS */

#ifdef FEATURE_WCDMA
#error code not present
#else
  { "", 0, 0 },
  { "", 0, 0 },
  { "", 0, 0 },
#endif

#ifdef FEATURE_MMGPS
#error code not present
#else
  { "", 0, 0 },
#endif

#ifdef FEATURE_NAS
#error code not present
#else
  { "", 0, 0 },
  { "", 0, 0 },
#endif

#if ( defined(FEATURE_WCDMA_PLT) || defined(FEATURE_GSM_PLT) )
#error code not present
#else
  { "", 0, 0 },
  #if defined(FEATURE_GSM_TEST_TASK)
#error code not present
  #else
    { "", 0, 0 },
  #endif
#endif

#ifdef FEATURE_NAS
#error code not present
#else
  { "", 0, 0 },
#endif

#if ( defined(FEATURE_GSM) && !defined(FEATURE_GSM_PLT) && !defined (FEATURE_WCDMA_PLT) && !defined(FEATURE_HWTC))
#error code not present
#else
  { "", 0, 0 },
#endif

#if ( defined(FEATURE_WCDMA) && !defined(FEATURE_WCDMA_PLT) && !defined (FEATURE_GSM_PLT))
#error code not present
#else
  { "", 0, 0 },
#endif

#ifdef FEATURE_NAS
#error code not present
#else
  { "", 0, 0 },
#endif

#ifdef FEATURE_TC
  { "TC",   DOG_MS_TO_COUNTS( 10000 ), TC_PRI },       /* TC_COUNTS     */
#else
  { "", 0, 0 },
#endif

#if defined FEATURE_GPSONE_STANDALONE && \
            ((!defined FEATURE_MULTIPROCESSOR) || (defined IMAGE_MODEM_PROC))
#error code not present
#else /* FEATURE_GPSONE_STANDALONE */
  {  "", 0, 0 },
#endif /* FEATURE_GPSONE_STANDALONE */

#if defined FEATURE_FTM_TASK && \
            ((!defined FEATURE_MULTIPROCESSOR) || (defined IMAGE_MODEM_PROC))
  {  "FTM",  DOG_MS_TO_COUNTS( 90000 ), FTM_PRI },      /* MS_COUNTS    */
#else
  {  "", 0, 0 },
#endif /* FEATURE_FTM_TASK */

#if defined FEATURE_GSTK && \
            ((!defined FEATURE_MULTIPROCESSOR) || (defined IMAGE_MODEM_PROC))
  { "GSTK", DOG_MS_TO_COUNTS( 60000 ), GSTK_PRI },        /* GSTK_COUNTS   */
#else
  {  "", 0, 0 },
#endif

#if defined(FEATURE_VIRTUAL_SAT) && defined(FEATURE_VIRTUAL_SIM)
  { "VSAT", DOG_MS_TO_COUNTS( 15000 ), VIRTSAT_PRI },        /* VSAT_COUNTS   */
#else
  {  "", 0, 0 },
#endif

/* Configure in/out monitoring of the SSSL protocol Services task,
** based on whether it is there or not.
*/

#ifdef FEATURE_SEC_SSL
  { "SSL",  DOG_MS_TO_COUNTS(59000*CNT_SCALE), SECSSL_PRI },       /* SECSSL_COUNTS    */
#else
  { "", 0, 0 },                                 /* Dummy Entry   */
#endif

#ifdef FEATURE_MGP
  { "MGPMC",  DOG_MS_TO_COUNTS( 10000 ), MGPMC_PRI },    /* MGPMC_COUNTS    */
  { "MGPNF",  DOG_MS_TO_COUNTS( 10000 ), MGPNF_PRI },    /* MGPNF_COUNTS    */
  { "MGPCD",  DOG_MS_TO_COUNTS( 10000 ), MGPCD_PRI },    /* MGPCD_COUNTS    */
  { "MGPAI",  DOG_MS_TO_COUNTS( 10000 ), MGPAI_PRI },    /* MGPAI_COUNTS    */
#else
  { "", 0, 0 },
  { "", 0, 0 },
  { "", 0, 0 },
  { "", 0, 0 },
#endif

#if( defined(FEATURE_PROGRESSIVE_BOOT) && !defined(FEATURE_TIMER_TASK) && !defined(FEATURE_L4))
  { "INTR", DOG_MS_TO_COUNTS( 10000 ), INTERRUPT_PRI },      /* INTERRUPT_COUNTS */
#else
  { "", 0, 0 },                                   /* Dummy Entry   */
#endif

#ifdef FEATURE_SEC_ARM_CRYPTO
  { "SECAR",  DOG_MS_TO_COUNTS(59000*CNT_SCALE), SECCRYPTARM_PRI },    /* SEC_ARM_CRYPTO_COUNTS    */

#else
  { "", 0, 0 },                                 /* Dummy Entry   */
#endif

#ifdef FEATURE_SEC_IPSEC
  { "IPSEC",  DOG_MS_TO_COUNTS(59000*CNT_SCALE), SECIPS_PRI },     /* IPSEC_COUNTS    */
#else
  { "", 0, 0 },                                 /* Dummy Entry   */
#endif

#if defined FEATURE_QVPHONE && \
            ((!defined FEATURE_MULTIPROCESSOR) || (defined IMAGE_APPS_PROC))

  { "QVPAE", DOG_MS_TO_COUNTS(60000), QVP_AE_PRI },
  { "QVPIO", DOG_MS_TO_COUNTS(59000), QVPIO_PRI  }, /* QVPIO_COUNTS changed from 59000 */
  { "QVPPL", DOG_MS_TO_COUNTS(59000), QVPPL_PRI  }, /* QVPPL_COUNTS changed from 59000 */
  { "", 0, 0 },                                     /* Dummy Entry   */
  { "", 0, 0 },                                     /* Dummy Entry   */

#else
  { "", 0, 0 },                                 /* Dummy Entry   */
  { "", 0, 0 },                                 /* Dummy Entry   */
  { "", 0, 0 },                                 /* Dummy Entry   */
  { "", 0, 0 },                                 /* Dummy Entry   */
  { "", 0, 0 },                                 /* Dummy Entry   */

#endif /* FEATURE_QVPHONE */

#if defined FEATURE_GPSSRCH_TASK && \
    ((!defined FEATURE_MULTIPROCESSOR) || (defined IMAGE_MODEM_PROC))
#error code not present
#else
  { "", 0, 0 },                                 /* Dummy Entry   */
#endif

#ifdef FEATURE_GSM_CB
#error code not present
#else
  { "", 0, 0 },
#endif

#ifdef FEATURE_SEC_DRM
#error code not present
#else
  { "", 0, 0 },                                 /* Dummy Entry   */
#endif

#ifdef FEATURE_BT_EXTPF
  { "BTPF",  DOG_MS_TO_COUNTS( 10000 ), BT_PF_PRI },   /* BT_PF_COUNTS  */
#else
  { "", 0, 0 },                                        /* Dummy Entry   */
#endif

#if defined FEATURE_HDR_BCMCS && \
            ((!defined FEATURE_MULTIPROCESSOR) || (defined IMAGE_MODEM_PROC))
#error code not present
#else
  {"",0,0},
#endif

#if defined FEATURE_BCMCS && \
            ((!defined FEATURE_MULTIPROCESSOR) || (defined IMAGE_MODEM_PROC))
#error code not present
#else
  { "", 0, 0 },                                /* Dummy Entry   */
#endif

#ifdef FEATURE_JOYST_TASK
  { "JOYST",  DOG_MS_TO_COUNTS( 40000 ), JOYST_PRI },   /* JOYST_COUNTS  */
#else
  { "", 0, 0 },                                         /* Dummy Entry   */
#endif

#ifdef FEATURE_DTV
#error code not present
#else
  { "", 0, 0 },                                /* Dummy Entry   */
#endif

#if ((!defined (FEATURE_WCDMA_PLT)) && (!defined (FEATURE_GSM_PLT)) && (defined (FEATURE_HSDPA)))
#error code not present
#else
  { "", 0, 0 },
#endif

/* Configure in/out monitoring of the PBM task,
** based on whether it is there or not.
*/
#ifdef FEATURE_PBM_TASK
  { "PBM", DOG_MS_TO_COUNTS( 90000 ), PBM_PRI },  /* PBM_COUNTS   */
#else
  { "", 0, 0 },                                   /* Dummy Entry   */
#endif

#if defined FEATURE_QVPHONE && \
            ((!defined FEATURE_MULTIPROCESSOR) || (defined IMAGE_APPS_PROC))
  { "VPVFE",   DOG_MS_TO_COUNTS( 59000 ), QVP_VFE_PRI },
#else
  { "", 0, 0 },                                /* Dummy Entry   */
#endif

#ifdef FEATURE_MGP
  #ifdef FEATURE_MGPCC_TASK
  { "MGPCC",  DOG_MS_TO_COUNTS( 10000 ), MGPCC_PRI },    /* MGPCC_COUNTS    */
  #else
  { "", 0, 0 },
  #endif
#else
  { "", 0, 0 },
#endif

#if defined(SIRIUS_PLATFORM) && defined(FEATURE_IRDA)
#error code not present
#else
  { "", 0, 0 },                                   /* Dummy Entry   */
#endif

#if defined(SIRIUS_PLATFORM) && defined(FEATURE_SIRIUS_TEST)
#error code not present
#else
  { "", 0, 0 },                                   /* Dummy Entry   */
#endif

#if defined(SIRIUS_PLATFORM) && defined(FEATURE_ACCESSORY_ATTACHMENT_MANAGER)
#error code not present
#else
  { "", 0, 0 },                                   /* Dummy Entry   */
#endif

#if defined(SIRIUS_PLATFORM) && defined(FEATURE_BATMAN_TASK)
#error code not present
#else
  { "", 0, 0 },                                   /* Dummy Entry   */
#endif

#if defined FEATURE_QVPHONE && defined(FEATURE_QVPHONE_SIP) && \
            ((!defined FEATURE_MULTIPROCESSOR) || (defined IMAGE_APPS_PROC))
#error code not present
#else
  { "", 0, 0 },
#endif

#if defined(FEATURE_QVPHONE) && defined(FEATURE_QVPHONE_SIP) && \
     ((!defined FEATURE_MULTIPROCESSOR) || (defined IMAGE_APPS_PROC))
#error code not present
#else
  { "", 0, 0 },
#endif

#ifdef FEATURE_WLAN
#error code not present
#else
  { "", 0, 0 },                                 /* Dummy Entry  */
  { "", 0, 0 },                                 /* Dummy Entry  */
#endif  /* FEATURE_WLAN */

#ifdef FEATURE_FM
  { "FMRDS",  DOG_MS_TO_COUNTS( 10000 ), FMRDS_PRI },        /* FMRDS_COUNTS  */
#else
  { "", 0, 0 },                                   /* Dummy Entry   */
#endif

#ifdef FEATURE_IG_QDIAG                               /* L4/Linux QDIAG task */
 { "QDIAG", DOG_MS_TO_COUNTS( 59000*CNT_SCALE ), QDIAG_PRI },  /* QDIAG_COUNTS   */
#else
  { "", 0, 0 },                                       /* Dummy Entry   */
#endif

#ifdef FEATURE_APS                                   /* L4/Linux APS task */
 { "APS", DOG_MS_TO_COUNTS( 59000*CNT_SCALE ), APS_PRI },  /* APS_COUNTS        */
#else
  { "", 0, 0 },                                      /* Dummy Entry       */
#endif

#if defined(FS_FEATURE_RMTEFS_SERVER) && !defined(FEATURE_IG_EFS_EXT_SERVER)
  { "EFSSVR", DOG_MS_TO_COUNTS( 59000*CNT_SCALE ), EFS_SRVR_PRI },/* EFS_SRVR_COUNTS */
#else
  { "", 0, 0 },                                      /* Dummy Entry       */
#endif

#if defined(FEATURE_PM_SERVER)          /* L4/Linux EFS Server task */
  { "PMS", DOG_MS_TO_COUNTS( 59000*CNT_SCALE ), PM_SERVER_PRI },/* EFS_SRVR_COUNTS */
#else
  { "", 0, 0 },                                      /* Dummy Entry       */
#endif

/* Configure in/out monitoring of the CNV task,
** based on whether it is there or not.
*/
#ifdef FEATURE_NV_CNV
  { "CNV",   DOG_MS_TO_COUNTS( 59000 ), CNV_PRI },      /* CNV_COUNTS     */
#else
  { "", 0, 0 },                                   /* Dummy Entry   */
#endif

#ifdef FEATURE_QIPCALL
  { "QIPC",   DOG_MS_TO_COUNTS( 59000 ), QIPCALL_PRI },      /* QIPCALL_COUNTS     */
#else
  { "", 0, 0 },                                   /* Dummy Entry   */
#endif

#ifdef FEATURE_MFLO
#error code not present
#else
  { "", 0, 0 },
  { "", 0, 0 },
  { "", 0, 0 },
#endif

#ifdef FEATURE_WLAN
#error code not present
#else
  { "", 0, 0 },                                 /* Dummy Entry  */
#endif  /* FEATURE_WLAN */

#if defined FEATURE_DISP_TASK && \
            ((!defined FEATURE_MULTIPROCESSOR) || (defined IMAGE_APPS_PROC))

  { "DISP",   DOG_MS_TO_COUNTS( 59000 ), DISP_PRI },    /* GRAPH_COUNTS  */
#else
  { "", 0, 0 },                                   /* Dummy Entry   */
#endif

#if defined (FEATURE_USB) && !defined (FEATURE_NO_USB_HW_ON_PROCESSOR)
#ifdef FEATURE_USB_HOST_ENHANCED
    { "USBHU",  DOG_MS_TO_COUNTS( 59000 ), USBHC_URB_PRI }, /* USBHC_URB_COUNTS    */
    { "USBHD",  DOG_MS_TO_COUNTS( 59000 ), USBHC_DEV_PRI }, /* USBHC_DEV_COUNTS    */
#if defined(FEATURE_USB_HOST_TEST_ENHANCED) && \
    (defined(FEATURE_USB_HOST_MASS_STORAGE_TEST_ENHANCED) || \
     defined(FEATURE_USB_HOST_HID_TEST_ENHANCED))
    { "USBTS",  DOG_MS_TO_COUNTS( 59000 ), USBHC_TEST_PRI }, /* USBHC_TEST_COUNTS    */
#else
    { "", 0, 0 },                                   /* Dummy Entry   */
#endif
#else
    { "", 0, 0 },                                   /* Dummy Entry   */
    { "", 0, 0 },                                   /* Dummy Entry   */
    { "", 0, 0 },                                   /* Dummy Entry   */
#endif
#else
    { "", 0, 0 },                                   /* Dummy Entry   */
    { "", 0, 0 },                                   /* Dummy Entry   */
    { "", 0, 0 },                                   /* Dummy Entry   */
#endif
#ifdef FEATURE_AVS_SBCENC_TASK
    { "SBCEN",  DOG_MS_TO_COUNTS( 10000 ), SBCENC_PRI }, /* SBCENC    */
#else
    { "", 0, 0 },                                   /* Dummy Entry   */
#endif

/* Disable monitoring of the Camera Driver task */
  { "", 0, 0 },                                   /* Dummy Entry   */

#ifdef FEATURE_QDSP_RTOS
  { "DSPOS",  DOG_MS_TO_COUNTS( 10000 ), ADSP_RTOS_PRI }, /* ADSP_RTOS_COUNTS  */
#else
  { "", 0, 0 },                                   /* Dummy Entry   */
#endif

/* Configure in/out monitoring of the ATI Graphics task,
** based on whether it is there or not.
*/
#if defined FEATURE_ATI_GRAPHICS && \
            ((!defined FEATURE_MULTIPROCESSOR) || (defined IMAGE_APPS_PROC))
  { "GRAFX",   DOG_MS_TO_COUNTS( 59000 ), GRAPHICS_PRI },    /* GRAPHICS_COUNTS  */
#else
  { "", 0, 0 },                                   /* Dummy Entry   */
#endif

#ifdef FEATURE_IXHTTPSVC
/* Configure in/out monitoring of the HTTP service task,
** based on whether they are there or not.
*/
  { "IXHTTPSVC",    DOG_MS_TO_COUNTS( 59000*CNT_SCALE ), IXHTTPSVC_PRI }, /* HTTP_COUNTS    */
#else
  { "", 0, 0 },                                  /* Dummy Entry     */
#endif

#ifdef FEATURE_ATS                                   /* L4/Linux ATS task */
  { "ATS", DOG_MS_TO_COUNTS( 59000*CNT_SCALE ), ATS_PRI },  /* ATS_COUNTS        */
#else
  { "", 0, 0 },                                      /* Dummy Entry       */
#endif

#ifdef FEATURE_COMP_TASK
  {"COMP", DOG_MS_TO_COUNTS(20000), COMP_PRI},
#else
  { "", 0, 0 },                                      /* Dummy Entry       */
#endif /* FEATURE_COMP_TASK */

#if defined(FEATURE_MODEM_PDCOMM)
    { "PDCOMM",  DOG_MS_TO_COUNTS( 10000*CNT_SCALE ), PDSM_COMM_PRI },   /* PDSMS_COUNTS  */
#else
    { "", 0, 0 },                                 /* Dummy Entry   */
#endif

#ifdef FEATURE_SECRND
  { "SECRND",  DOG_MS_TO_COUNTS( 59000*CNT_SCALE ), SECRND_PRI },/* SECRND    */
#else
  { "", 0, 0 },                                   /* Dummy Entry   */
#endif

#ifdef FEATURE_UBM_L1
  { "UBML1", DOG_MS_TO_COUNTS(10000), UBM_L1_PRI },
#else
  { "", 0, 0 },
#endif /* FEATURE_UBM_L1 */

#ifdef FEATURE_QDJTEST
  { "QDJTEST", DOG_MS_TO_COUNTS( 59000 ), QDJTEST_PRI}, /* QDJTEST */
#else
  { "", 0, 0 },                                       /* Dummy Entry  */
#endif  /* FEATURE_QDJTEST */

#ifdef FEATURE_SEC_USFS
  { "SFSsvc",  DOG_MS_TO_COUNTS( 59000*CNT_SCALE ), SFS_PRI }, /* SFS    */
#else
  { "", 0, 0 },                                   /* Dummy Entry   */
#endif

#ifdef FEATURE_DMTASK
  { "DMTASK",   DOG_MS_TO_COUNTS( 59000 ), DMTASK_PRI },      /* DMTASK     */
#else
  { "", 0, 0 },                                   /* Dummy Entry   */
#endif /* FEATURE_DMTASK */

#ifdef FEATURE_CPU_BASED_FLOW_CONTROL
  { "FC",  DOG_MS_TO_COUNTS( 10000 ), FC_PRI },
                                               /* FC_COUNTS    */
#else
  { "", 0, 0 },
#endif

#ifdef FEATURE_QTV_STATIC_TASKS
  { "QTVvrend", DOG_MS_TO_COUNTS( 59000 ), QTV_VIDEO_RENDERER_PRI },
#else
  { "", 0, 0 },                                   /* Dummy Entry   */
#endif /* FEATURE_QTV_STATIC_TASKS */

#ifdef FEATURE_QTV_STATIC_TASKS
  { "QTVaudio", DOG_MS_TO_COUNTS( 59000 ), QTV_AUDIO_PRI },
#else
  { "", 0, 0 },                                   /* Dummy Entry   */
#endif /* FEATURE_QTV_STATIC_TASKS */

#ifdef FEATURE_EXT_USB_P1
  { "USBE2", DOG_MS_TO_COUNTS( 59000 ), USBEBI2_PRI },
  { "USBAP", DOG_MS_TO_COUNTS( 59000 ), USBAPI_PRI },
  { "USBDE", DOG_MS_TO_COUNTS( 59000 ), USBD_PRI },
#else
  { "", 0, 0 },                                 /* Dummy Entry  */
  { "", 0, 0 },                                 /* Dummy Entry  */
  { "", 0, 0 },                                 /* Dummy Entry  */
#endif

#ifdef FEATURE_CGPS
#error code not present
#else
  { "", 0, 0 },
  { "", 0, 0 },
  { "", 0, 0 },
  { "", 0, 0 },
  { "", 0, 0 },
  { "", 0, 0 },
  { "", 0, 0 },                                    /* Dummy Entry   */
  { "", 0, 0 },                                    /* Dummy Entry   */
  { "", 0, 0 },                                    /* Dummy Entry   */
  { "", 0, 0 },                                    /* Dummy Entry   */
#endif /* FEATURE_CGPS */
#ifdef FEATURE_XO_TASK
  { "TCXOMGR",  DOG_MS_TO_COUNTS( 10000 ), TCXOMGR_PRI },    /* TCXO_COUNTS    */
#else
  { "", 0, 0 },
#endif /* FEATURE_XO_TASK */
#if defined(FEATURE_HS_USB) && defined(FEATURE_HS_USB_MS_FD) && (defined(FEATURE_STANDALONE_MODEM) || \
     !defined(FEATURE_NO_USB_HW_ON_PROCESSOR) || !defined(FEATURE_MULTIPROCESSOR))
  { "HSU_AL_MS",  DOG_MS_TO_COUNTS( 120000 ), HSU_AL_MS_PRI }, /* HSU_AL_MS TASK  */
#else
  {"", 0, 0},
#endif

#if defined(FEATURE_HS_USB) && (defined(FEATURE_STANDALONE_MODEM) || \
     !defined(FEATURE_NO_USB_HW_ON_PROCESSOR) || !defined(FEATURE_MULTIPROCESSOR))
  { "HSU_AL_TASK",  DOG_MS_TO_COUNTS( 10000 ), HSU_AL_TASK_PRI }, /* HSU_AL TASK  */
#else
  {"", 0, 0},
#endif

#ifdef FEATURE_EFS_FDI
  { "FDI_BG",  DOG_MS_TO_COUNTS( 59000 ), FDI_BG_PRI }, /* FDI_BG_COUNTS */
#else
  { "", 0, 0 },                                    /* Dummy Entry   */
#endif

#ifdef FEATURE_QTV_STATIC_DLAGENT_TASK
  { "QTV_DLAgent", DOG_MS_TO_COUNTS( 59000 ), QTV_DEC_DLAGENTTASK_PRI },
#else
  { "", 0, 0 },                                   /* Dummy Entry   */
#endif /* FEATURE_QTV_STATIC_DLAGENT_TASK */

#ifdef FEATURE_QTV_STATIC_DLDSPSVC_TASK
  { "QTV_DLDspS", DOG_MS_TO_COUNTS( 59000 ), QTV_DEC_DLDSPSVCTASK_PRI },
#else
  { "", 0, 0 },                                   /* Dummy Entry   */
#endif /* FEATURE_QTV_STATIC_DLDSPSVC_TASK */

#ifdef FEATURE_CHG_TASK
  { "CHG", DOG_MS_TO_COUNTS( 59000 ), CHG_PRI },
#else
  { "", 0, 0 },                                   /* Dummy Entry   */
#endif /* FEATURE_CHG_TASK */

#ifdef FEATURE_VBATT_TASK
  { "VBATT", DOG_MS_TO_COUNTS(59000), VBATT_PRI },
#else
  { "", 0, 0 },                                /* Dummy Entry */
#endif /* FEATURE_VBATT_TASK */

}; /* dog_task_info */

#define DOG_NUM_TASKS (sizeof(dog_task_info) / sizeof(dog_task_info[0]))

/* Stores the current state of each task in the system. Indicates the
** number of counts remaining for a task to check in and whether or not
** the task is blocked on a REX call. Initialized in dog_init().
*/
LOCAL dog_task_state_type dog_task_state[DOG_NUM_TASKS];

#ifdef DOG_DEBUG
  /* Debug macro for outputting report values to debug port */
  #define DOG_DEBUG_REPORT(report)  outpw(0x2800000,report);
#endif

#ifdef FEATURE_SOFTWARE_PROFILE_CPU_STARV_DET
#error code not present
#endif


/*===========================================================================

FUNCTION DOG_REPORT

DESCRIPTION
  Report to the Watchdog task, so that it can tell that the monitored
  task is still functioning properly.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dog_report
(
  dog_report_type report
    /* Tell the Watchdog task which task is reporting */
)
{
  /*lint -e730 */ /* Boolean argument to function */
  ASSERT( report < DOG_NUM_TASKS );
  /*line +e730 */
  if ( report < DOG_NUM_TASKS )
  {

    #ifdef DOG_DEBUG
      /* Output report index to debug port */
      DOG_DEBUG_REPORT(report);
    #endif

    /* Reset counter to maximum */
    dog_task_state[report].count = dog_task_info[report].timeout;

  }

} /* end of dog_report */


/*===========================================================================

FUNCTION DOG_SET_AUTOKICKING

DESCRIPTION
  This procedure sets the autokicking state for the dog task.

DEPENDENCIES
  Only call when in an INTLOCK.

RETURN VALUE
  None

SIDE EFFECTS
  Enables or disables the dog_wakeup_timer, depending on autokick state.

===========================================================================*/
LOCAL void dog_set_autokicking
(
  boolean enable
)
{
  static rex_timer_cnt_type dog_save_timer = DOG_WAKEUP_TIME;
    /* Saved timer value */

  /* Check if we are switching the dog_auto_kicking state
  */
  if ( dog_auto_kicking != enable ) {

    if ( enable ) {

      /* Now auto-kicking.  Pause Watchdog's wakeup timer
      */
      dog_save_timer = rex_clr_timer( &dog_wakeup_timer );

    } else {

      /* No longer auto-kicking.  Resume watchdog wakeup timer
      */
      (void) rex_set_timer( &dog_wakeup_timer, dog_save_timer );

    }

    /* Record the new state
      */
    dog_auto_kicking = enable;
  }

} /* end of dog_set_autokicking */


/*===========================================================================

FUNCTION DOG_GSM_AUTOKICK

DESCRIPTION
  This procedure enables/disables the auto-kick feature for GSM only.
  Auto-kick is used when the microprocessor is expected to be asleep.

DEPENDENCIES
  This should only be called when the sleep timer has been programmed
  or when the warm-up timer has expired.

RETURN VALUE
  None

SIDE EFFECTS
  When enabled, pauses the watchdog's wakeup timer.  The timer resumes
  when autokick is disabled via this function.

===========================================================================*/
void dog_gsm_autokick
( /*lint -esym(715, enable) *//* Symbol enable is not used */
  boolean enable
)
{
#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900) || defined(FEATURE_ACP) || defined(FEATURE_HDR))

  INTLOCK( );

  /* Reset the watchdog timer by setting and inverting the trigger
  */
  if ( enable ) {

    /* Kick to give auto-kick max time to enable.
    */
    dog_kick();

    /* Enable auto-kick, atomically per MSM spec.
    */
    #ifdef MSMHW_GSM_BONDING
#error code not present
    #endif
  }
  else
  {
    #ifdef MSMHW_GSM_BONDING
#error code not present
    #endif
  }

  /* Update global auto-kicking flag, and start/stop dog_wakeup_timer
  */
  dog_set_autokicking( enable );


  INTFREE( );

#endif /* (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900) || defined(FEATURE_ACP) || defined(FEATURE_HDR)) */

} /* end of dog_gsm_autokick *//*lint +esym(715, enable) */


#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900) || defined(FEATURE_ACP) || defined(FEATURE_HDR))
/*===========================================================================

FUNCTION DOG_AUTOKICK

DESCRIPTION
  This procedure enables/disables the auto-kick feature.  Auto-kick
  is used when the microprocessor is expected to be asleep.

DEPENDENCIES
  This should only be called when the sleep timer has been programmed
  or when the warm-up timer has expired.

RETURN VALUE
  None

SIDE EFFECTS
  When enabled, pauses the watchdog's wakeup timer.  The timer resumes
  when autokick is disabled via this function.

===========================================================================*/
void dog_autokick
(
  boolean enable
)
{

  INTLOCK( );

  /* Update global auto-kicking flag, and start/stop dog_wakeup_timer
  */
  dog_set_autokicking( enable );

  /* Disable the autokicker */
  dog_kick();

  /* Reset the watchdog timer by setting and inverting the trigger
  */
  if ( enable )
  {
    /* Enable auto-kick, atomically per MSM spec.
    */
    #ifdef T_MSM6500

    /* if dog_kick signal and autokicker enable signal reach the dog circuitry in
       the same sleep cycle, dog_kick precedes autokicker enable. So 50us (>33us)
       delay is added to make sure they come in different sleep cycles.         */

    /* A 50us wait turns out to be not enough so this value is
       increased to 120us */
      clk_busy_wait(120);
    #if (!defined(T_MSM7500))
      HWIO_OUT(AUTOKICK_START, 1 );
    #else
#error code not present
    #endif

    #else
#error code not present
    #endif
  }

  #ifdef MSMHW_GSM_BONDING
#error code not present
  #endif /* MSMHW_GSM_BONDING */

  INTFREE( );

} /* end of dog_autokick */


  #else /*!CDMA_800 && !CDMA_1900 && !ACP && !HDR*/
/*===========================================================================

FUNCTION DOG_AUTOKICK

DESCRIPTION
  This procedure enables/disables the auto-kick feature.  Auto-kick
  is used when the microprocessor is expected to be asleep.

DEPENDENCIES
  This should only be called when the sleep timer has been programmed
  or when the warm-up timer has expired.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dog_autokick
(
  boolean enable
)
{

  INTLOCK( );

  /* Kick to give auto-kick max time to enable.
  */
  dog_kick();

  /* to disable the auto kicking circuit, just write 0;
     to enable, write 1 followed by 0 */
  if ( enable )
  {
    #if (defined(T_MSM6275) || defined ( T_MSM6800))
    /* If dog_kick signal and autokicker enable signal reach the dog circuitry
     * in the same sleep cycle, dog_kick precedes autokicker enable. So 120us
     * (>33us) delay is added to make sure they come in different sleep cycles.
     */
    clk_busy_wait(120);
    HWIO_OUT(AUTOKICK_START, HWIO_FMSK(AUTOKICK_START, AUTO_KICK_START));
    #else/* Enable auto-kick, atomicly per MSM spec.*/
    /*lint -esym(578,sav) *//*Declare 'sav' hide 'sav '*/
    MSM_OUTM(SLEEP_CTL, SLEEP_CTL_SLEEP_CTLR_AUTOKICK_ENA_BMSK,
              SLEEP_CTL_SLEEP_CTLR_AUTOKICK_ENA_V);
    MSM_OUTM(SLEEP_CTL, SLEEP_CTL_SLEEP_CTLR_AUTOKICK_ENA_BMSK, 0 );
    #endif/*lint +esym(578,sav) */

    #ifdef FEATURE_MONITOR_DOG
    dog_setup_monitor_timer( DOG_TURN_OFF_MONITOR );
    #endif
  }
  else
  {
    #if (!defined( T_MSM6275) && !defined( T_MSM6800))/*lint -esym(578,sav) *//*Declare 'sav hide 'sav' */
    MSM_OUTM(SLEEP_CTL, SLEEP_CTL_SLEEP_CTLR_AUTOKICK_ENA_BMSK, 0 );
    #endif/*lint +esym(578,sav) */

    #ifdef FEATURE_MONITOR_DOG
    dog_setup_monitor_timer( DOG_RESET_MONITOR );
    #endif
  }


  INTFREE( );

} /* end of dog_autokick */

#endif /*CDMA_800 || CDMA_1900 || ACP || HDR*/



/*===========================================================================

FUNCTION DOG_KICK

DESCRIPTION
  This procedure resets the watchdog timer circuit.  This is
  not to be used except in unusual start-up/shutdown conditions.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Resets the circuit so that we have another N milliseconds before
  the circuit will reset the system.  Arms auto-kick.

===========================================================================*/
void dog_kick( void )
{

#ifdef FEATURE_L4
#error code not present
#else

  INTLOCK( );

  /* Reset the watchdog timer by setting and inverting the trigger
  */
#if ((!defined FEATURE_MULTIPROCESSOR) || (defined IMAGE_MODEM_PROC))
#if (defined(T_MSM6500) || defined(T_MSM6275) || defined(T_MSM6800))
#if !(defined(T_MSM7500) && defined(FEATURE_STANDALONE) && defined(IMAGE_APPS_PROC))
  /*lint -esym(578,sav) *//*Declare 'sav' hide 'sav '*/
    HWIO_OUTM(WDOG_RESET,HWIO_FMSK(WDOG_RESET,WATCH_DOG),1);
  /*lint +esym(578,sav) */
#endif
#else
#error code not present
#endif /* T_MSM6500 */
#endif /* ! FEATURE_MULTIPROCESSOR || IMAGE_MODEM_PROC */

  INTFREE( );

#endif /* FEATURE_L4 */

} /* end of dog_kick */


/*===========================================================================

FUNCTION DOG_SHUTDOWN

DESCRIPTION
  This procedure performs Watchdog task shutdown processing. It processes
  task stop procedure and then returns.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void dog_shutdown( void )
{
  task_stop();

  (void) rex_clr_sigs (rex_self(), TASK_STOP_SIG );

} /* end of dog_shutdown */


#ifdef FEATURE_ID_FATAL_TASK
/*===========================================================================

FUNCTION dog_fatal_error

DESCRIPTION
  Handle fatal error for dog and identify the failing cause befoer DIAG is
  capable of doing this.

DEPENDENCIES
  None

RETURN VALUE
  This function does not return

SIDE EFFECTS
  Equivalent to ERR_FATAL macro

===========================================================================*/
LOCAL void dog_fatal_error
(
  const char* task_name
)
{
  char dog_fatal_err_msg[20];

  if (task_name)
  {
    (void) snprintf(dog_fatal_err_msg, sizeof(dog_fatal_err_msg), "dog: %s", task_name);
  }
  else
  {
    (void) snprintf(dog_fatal_err_msg, sizeof(dog_fatal_err_msg), "dog: startup");
  }

  /* The following is equivalent to ERR_FATAL macro */
  boot_clear_sl();
  #ifdef FEATURE_ERR_EXTENDED_STORE
  (void)err_add_to_mem_table("dog_task_state[]",
                             (void *)&dog_task_state[0],
                             sizeof(dog_task_state));

#ifdef FEATURE_SOFTWARE_PROFILE_CPU_STARV_DET
#error code not present
  #endif

#endif
  err_fatal_core_dump(0, dog_fatal_err_msg, NULL);
}
#endif /* FEATURE_ID_FATAL_TASK */

#ifdef FEATURE_DOG_FATAL_NV_REPORT

/*===========================================================================

FUNCTION dog_fs_cb

DESCRIPTION
  Callback function used for asynchronous access to the filesystem. This
  function will be called when the filesystem operation is complete.
===========================================================================*/
void dog_fs_cb (fs_rsp_msg_type *rsp_msg)
{
  (void) rsp_msg;
  (void) rex_set_sigs (&dog_tcb, DOG_FS_COMPLETE_SIG);
}

/*===========================================================================

FUNCTION dog_wait

DESCRIPTION
  Wait function for use by the Dog task. Calls rex_wait with the signal mask
  passed in, but kicks the hardware dog periodically in addition. The function
  also takes a timeout parameter that determines the maximum duration of the
  wait in milliseconds. If this a parameter is 0, then there is no timeout.

===========================================================================*/
rex_sigs_type dog_wait (rex_sigs_type sigs, uint32 tmo)
{
  rex_sigs_type set_sigs = 0;
  uint32 elapsed_time = 0;

  for( ; ; ){
    (void) rex_set_timer (&dog_wakeup_timer, DOG_WAKEUP_TIME);
    dog_kick ();
    set_sigs = rex_wait (sigs | DOG_WAKEUP_TIMER_SIG);
    if (set_sigs & sigs){
      break;}
    else if (tmo > 0) {
      elapsed_time += DOG_WAKEUP_TIME;
      if (elapsed_time >= tmo){
        break;}
    }
  }

  return set_sigs;
}

/*===========================================================================

FUNCTION dog_fatal_error_nv_report

DESCRIPTION
  Writes the dog task info to flash memory. Since dog task information is
  written to flash using the FS task, this write to flash occurs
  only when the timed out task is not the FS task. Priority written to
  the report file is the rex priority which is higher the priority, higher
  is the number. Also note that a few tasks can change priorities dynamically.
  This does not happen normally or frequently but potentially could affect
  the data in the report file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void dog_fatal_error_nv_report (void)
{
  /* Directory and file name to store dog task information
  ** on the flash
  */
  const char dog_report_dir [] = "watchdog";
  const char dog_report_file[] = "watchdog/report.txt";

  fs_rsp_msg_type fs_rsp;
  fs_open_xparms_type open_params;
  fs_handle_type file_handle;
  unsigned int cntr;
  char buffer[100];
  rex_sigs_type sigs;

  /* Kick the hardware watchdog to ensure that we have time enough
  ** write the dog task information to flash
  */
  dog_kick ();

  /* Create watchdog folder if it doesn't exist */
  (void) rex_clr_sigs (&dog_tcb, DOG_FS_COMPLETE_SIG);
  fs_nametest (dog_report_dir, FS_TEST_DIR, dog_fs_cb, &fs_rsp);
  sigs = dog_wait (DOG_FS_COMPLETE_SIG, DOG_FS_TMO);
  if (!(sigs & DOG_FS_COMPLETE_SIG)) {
     return;
  }
  if (fs_rsp.nametest.status != FS_OKAY_S) {
     return;
  }

  if (fs_rsp.nametest.name_found == FALSE) {
    (void) rex_clr_sigs (&dog_tcb, DOG_FS_COMPLETE_SIG);
    fs_mkdir (dog_report_dir, dog_fs_cb, &fs_rsp);
    sigs = dog_wait (DOG_FS_COMPLETE_SIG, DOG_FS_TMO);
    if ((sigs & DOG_FS_COMPLETE_SIG) == 0) {
       return;
    }
    if (fs_rsp.mkdir.status != FS_OKAY_S) {
       return;
    }
  }
  else {
    /* watchdog folder exists. So remove any old version of dog report
    ** file
    */
    (void) rex_clr_sigs (&dog_tcb, DOG_FS_COMPLETE_SIG);
    fs_nametest (dog_report_file, FS_TEST_FILE, dog_fs_cb, &fs_rsp);
    sigs = dog_wait (DOG_FS_COMPLETE_SIG, DOG_FS_TMO);
    if ((sigs & DOG_FS_COMPLETE_SIG) == 0) {
       return;
    }
    if (fs_rsp.nametest.status != FS_OKAY_S) {
       return;
    }
    if (fs_rsp.nametest.name_found != FALSE) {
      /* remove it */
      (void) rex_clr_sigs (&dog_tcb, DOG_FS_COMPLETE_SIG);
      fs_remove (dog_report_file, dog_fs_cb, &fs_rsp);
      sigs = dog_wait (DOG_FS_COMPLETE_SIG, DOG_FS_TMO);
      if ((sigs & DOG_FS_COMPLETE_SIG) == 0) {
         return;
      }
      if (fs_rsp.rmfile.status != FS_OKAY_S) {
         return;
      }
    }
  }

  /* Create file */
  (void) rex_clr_sigs (&dog_tcb, DOG_FS_COMPLETE_SIG);
  open_params.create.cleanup_option = FS_OC_CLOSE;
  open_params.create.buffering_option = FS_OB_ALLOW;
  open_params.create.attribute_mask = FS_FA_UNRESTRICTED;
  fs_open(dog_report_file, FS_OA_CREATE, &open_params, dog_fs_cb, &fs_rsp);
  sigs = dog_wait (DOG_FS_COMPLETE_SIG, DOG_FS_TMO);
  if ((sigs & DOG_FS_COMPLETE_SIG) == 0) {
     return;
  }
  if (fs_rsp.open.status != FS_OKAY_S) {
    return;
  }

  file_handle = fs_rsp.open.handle;
  for(cntr = 0 ; cntr <= DOG_NUM_TASKS ; cntr++) {
    /* Kick the hardware watchdog to ensure that we have time enough
    ** to write the dog task information to flash
    */
    dog_kick ();

    if (cntr == 0) {
      (void) sprintf (buffer, "Task Name, Timeout, Current Timer, Priority\n");
    }
    else {
      (void) sprintf (buffer,"%9s, %7d, %13d, %8lu\n",
        dog_task_info [cntr-1].task,
        dog_task_info [cntr-1].timeout,
        dog_task_state[cntr-1].count,
        dog_task_info [cntr-1].priority);

      if (dog_task_info[cntr-1].timeout != 0) {
        (void) rex_clr_sigs (&dog_tcb, DOG_FS_COMPLETE_SIG);
        fs_write (file_handle, buffer, strlen(buffer), dog_fs_cb, &fs_rsp);
        sigs = dog_wait (DOG_FS_COMPLETE_SIG, DOG_FS_TMO);
        if ((sigs & DOG_FS_COMPLETE_SIG) == 0) {
          break;
        }

        if (fs_rsp.write.status != FS_OKAY_S) {
          break;
        }

        if (fs_rsp.write.count != strlen(buffer)) {
          break;
        }
      }
    }
  }

  (void) rex_clr_sigs (&dog_tcb, DOG_FS_COMPLETE_SIG);
  fs_close (file_handle, dog_fs_cb, &fs_rsp);
  sigs = dog_wait (DOG_FS_COMPLETE_SIG, DOG_FS_TMO);
  if ((sigs & DOG_FS_COMPLETE_SIG) == 0) {
    return;
  }
  if (fs_rsp.close.status != FS_OKAY_S) {
    return;
  }
}
#endif /* FEATURE_DOG_FATAL_NV_REPORT */


/*===========================================================================

FUNCTION DOG_INIT

DESCRIPTION
  This procedure performs the initialization for the Watchdog task.
  While waiting for a startup signal from the Main Control task, this
  routine repeatedly resets the watchdog.  If the startup signal from
  the Main Control task is not received before a time-out period, a
  fault is declared.

DEPENDENCIES
  The Watchdog task main processing loop should be started directly after
  this procedure.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void dog_init( void )
{
  rex_sigs_type sigs;   /* signals returned from rex wait procedures */
  word timer_cnt;       /* counter for Watchdog task wakeups */
  word i;               /* index for setting up countdown array */

  /* Define watchdog timer
  */
  rex_def_timer( &dog_wakeup_timer, &dog_tcb, DOG_WAKEUP_TIMER_SIG );

  /* Process task acknowledge procedure.
  */
  task_ack();

  /* Set up task counts. */
  for (i = 0; i < DOG_NUM_TASKS; i++) {
    dog_task_state[i].count = ( dog_task_info[i].timeout == 0 ) ?
                              0xff : dog_task_info[i].timeout;
    dog_task_state[i].is_blocked = FALSE;
  }

  /* Wait for startup/shutdown signal, or a timeout.  The task has been
  ** defined, and will be started after all the other tasks have been started.
  ** If some task hangs during initialization, we'll catch it here.
  */
  (void) rex_set_timer( &dog_wakeup_timer, DOG_WAKEUP_TIME );
  dog_kick();
#if defined (FEATURE_EFS_ROOT_RAMDISK) && defined (T_RUMI)
#error code not present
#else
  for (timer_cnt = 0; timer_cnt < dog_startup_cnt; ) {
#endif /* T_RUMI2 */

    sigs = rex_wait( DOG_WAKEUP_TIMER_SIG | TASK_START_SIG | TASK_STOP_SIG );

    /* Reset the watchdog time while waiting for a startup signal
    ** from the Main Control task
    */
    if ( sigs & DOG_WAKEUP_TIMER_SIG ) {
      dog_kick();
      (void) rex_set_timer( &dog_wakeup_timer, DOG_WAKEUP_TIME );
      timer_cnt++;
    }

    /* We've been sent the start signal, process task acknowledge procedure
    ** and return.
    */
    if ( sigs & TASK_START_SIG) {
      (void) rex_clr_sigs ( rex_self(), TASK_START_SIG );

      task_ack();

      return;
    }

    /* Acknowledge the signal and continue
    */
    if (sigs & TASK_STOP_SIG ) {
      dog_shutdown();
    }

  } /* end of wait loop */

  /* The time-out period, between the Watchdog Task activating and the
  ** Main Control Task sending a start-up signal, has expired with no
  ** startup signal being received.  This is a fatal error.  ERR_FATAL
  ** does not return, and the watchdog circuit will reset the system.
  */
#if (!defined(TIMETEST) && defined(T_MSM7500))|| \
    (!defined(T_MSM7500))
  #ifdef FEATURE_SURF_5200
#error code not present
  #else
  BIO_SET_LED( BIO_LED8_M );    /* Turn on the error LED   */
  #endif
#endif
  #ifndef NO_DOG

  #if defined( FEATURE_SURF_5200 ) && defined(FEATURE_WCDMA)
#error code not present
  #else
    enc_kill_pa();                    /* Turn off the PA         */
  #endif

  #ifdef FEATURE_ID_FATAL_TASK
    dog_fatal_error(NULL);
  #else
    ERR_FATAL( "Watchdog startup timeout", 0, 0, 0 );
  #endif

  #endif /* ! NO_DOG */

} /* end of dog_init */


/*===========================================================================

FUNCTION DOG_TASK

DESCRIPTION
  This procedure contains the main processing loop for the Watchdog task.
  It checks that each task has reported in at least once during its task
  report interval.  If a task has not reported within its timeout period,
  the ERR_FATAL macro is used to log the error and reset the
  microprocessor.

DEPENDENCIES
  This procedure should be called by rex_def_task.  Each task must report
  in at least once each task report interval.  (Intervals are defined in
  the dog_task_info[] array.)

RETURN VALUE
  None, this procedure does not return.

SIDE EFFECTS
  The microprocessor may be reset during this task.

===========================================================================*/
void dog_task
(
  dword ignored
    /* Parameter received from Main Control task - ignored */
    /*lint -esym(715,ignored)
    ** Have lint not complain about the ignored parameter 'ignored' which is
    ** specified to make this routine match the template for rex_def_task().
    */
)
{
  rex_sigs_type sigs;
    /* signals returned from rex wait procedures */

  unsigned long i;
    /* loop index for Watchdog task wakeups      */

  unsigned long loop_index;
    /* loop index for checking each task         */

  dog_init();                   /* Initialize task */

#ifdef FEATURE_APPS_KEEPALIVE
  /* Initialize dog_keepalive */
  dog_keepalive_init();
#endif /* FEATURE_APPS_KEEPALIVE */

  /* Must have received the startup signal, loop forever
  */
  for (;;) {
    for (i = 0; i < CHECK_CNT; i++ ) {

      sigs = rex_timed_wait( DOG_WAKEUP_TIMER_SIG |
                             TASK_STOP_SIG |
                             TASK_OFFLINE_SIG,
                             &dog_wakeup_timer,
                             DOG_WAKEUP_TIME );

      dog_kick();       /* kick the watchdog timer */

#ifdef FEATURE_APPS_KEEPALIVE
      /* Check for 100ms timeout */
      if (sigs & DOG_WAKEUP_TIMER_SIG) {
      /* Trigger Keepalive State Machine */
          dog_keepalive_sm();
      }
#endif /* FEATURE_APPS_KEEPALIVE */

#ifdef FEATURE_MONITOR_DOG
      if ( ! is_sleep_drv_in_sleep() )
      {
         dog_setup_monitor_timer( DOG_RESET_MONITOR );
      }
#endif

      /* Time to shutdown,
      ** acknowledge the signal and suspend until TASK_START_SIG
      */
      if ( sigs & TASK_STOP_SIG ) {

#ifdef FEATURE_MONITOR_DOG
        dog_setup_monitor_timer( DOG_TURN_OFF_MONITOR );
#endif
        dog_shutdown();

      }

      /* If we're supposed to go offline, let's do it
      */
      if ( sigs & TASK_OFFLINE_SIG ) {

#ifdef FEATURE_MONITOR_DOG
        dog_setup_monitor_timer( DOG_TURN_OFF_MONITOR );
#endif

        /* Process task offline procedure.
        */
         task_offline();

        /* Clear the offline signal, and return
        */
        (void) rex_clr_sigs( &dog_tcb, TASK_OFFLINE_SIG );
      }

#ifdef FEATURE_SOFTWARE_PROFILE_CPU_STARV_DET
#error code not present
#endif

    }

#ifdef DOG_DEBUG
    /* If debug, indicate when we are about to check reports
    */
    DOG_DEBUG_REPORT(DOG_NUM_TASKS);
#endif

    /* Loop to check if any task's counter has expired.
    */
    loop_index = 0;
    do {

#if ( defined(T_SLEEP) && defined(FEATURE_DOG_IGNORE_SLEEP_TASK) )
      if ( loop_index == DOG_SLEEP_RPT )
      {
         loop_index++;
         continue;
      }
#endif
#ifdef FEATURE_IG_UI_CLIENT_SELECT
      if (hw_ig_ui_client_is_wombat())
         /* Wombat (Linux) is running the keypad and display, so we didn't start up UI */
      {
        if ( loop_index == DOG_UI_RPT )
        {
           loop_index++;
           continue;
        }
      }
#endif

#ifndef FEATURE_BRINGUP_DIAG_ONLY
      if (--(dog_task_state[loop_index].count) == 0) {

        /* A value of 0 for the count does not indicate a DOG timeout
        ** if the task is not being monitored.
        */
        if ((dog_enabled == TRUE) &&
            (dog_task_info[loop_index].timeout > 0) &&
            !(dog_task_state[loop_index].is_blocked)) {

#ifdef FEATURE_MONITOR_DOG
          dog_setup_monitor_timer( DOG_TURN_OFF_MONITOR );
#endif

#if (!defined(TIMETEST) && defined(T_MSM7500))|| \
    (!defined(T_MSM7500))
          #ifdef FEATURE_SURF_5200
#error code not present
          #else
             BIO_SET_LED( BIO_LED8_M );    /* Turn on the error LED   */
          #endif
#endif

#ifndef NO_DOG

        #ifdef FEATURE_SURF_5200
#error code not present
        #else
          enc_kill_pa();                    /* Turn off the PA         */
        #endif

        #ifdef FEATURE_EFS
          /* Bump up the priority of the FS task so that writes to flash
          ** have a better chance of success.
          */
          (void)rex_task_pri( &fs_tcb, rex_self( )->pri - 1);
        #endif

        #ifdef FEATURE_DOG_FATAL_NV_REPORT
          /* Write the dog task info table to the NV. This write happens
          ** only if the task timed out is not the FS task.
          */
          if ( DOG_FS_RPT != loop_index )
          {
            INTLOCK();
            dog_fatal_error_nv_report();
            INTFREE();
          }
        #endif

        #ifdef FEATURE_ID_FATAL_TASK
          dog_fatal_error(dog_task_info[loop_index].task);
        #endif

          /* ERR_FATAL - Watchdog Timeout */
          /* Here we muck with the ERR_FATAL macro slightly so that we can
          ** give it a line number of 0000 (indicating dog timeout) and the
          ** string from dog_task_info to indicate the task that failed to
          ** report.  This way the offending task can easily be determined.
          */
          err_fatal_put_log ( 0,(char*)(dog_task_info[loop_index].task));
          /* err_fatal_put_log does not return! */

          /* The compiler doesn't know that ERR_FATAL doesn't return, so 'break'
          ** tells the compiler that we bail out on the loop in this case,
          ** which causes it to optimize the loop for *not* ERR_FATALing.
          *//*lint -e527 */  /* unreachable */
          break;/*lint +e527 */

#endif /* ! NO_DOG */

        } /* end if (dog_task_info[loop_index].timeout > 0) */
        else
        {
          dog_task_state[loop_index].count = 0xff;
        }

      } /* end if (--dog_task_state[loop_index].count == 0) */
#endif /* ! FEATURE_BRINGUP_DIAG_ONLY */

      loop_index++;
    } while ( loop_index < DOG_NUM_TASKS );

#ifdef FEATURE_SOFTWARE_PROFILE_CPU_STARV_DET
#error code not present
#endif

  } /* end of infinite loop */

} /* end of dog_task */


/*===========================================================================

FUNCTION DOG_MONITOR_PAUSE

DESCRIPTION
   Pauses the monitoring of a specified task.

===========================================================================*/
void dog_monitor_pause(
   unsigned int report_id
)
{  /*lint -e730 */ /* Boolean argument to function */
   ASSERT( report_id < DOG_NUM_TASKS );/*lint +e730 */
   if( report_id < DOG_NUM_TASKS )
   {
     INTLOCK( );
     dog_task_state[report_id].is_blocked = TRUE;
     INTFREE( );
   }
} /* END dog_monitor_pause */


/*===========================================================================

FUNCTION DOG_MONITOR_RESUME

DESCRIPTION
   Resumes the monitoring of a specified task.

===========================================================================*/
void dog_monitor_resume(
   unsigned int report_id
)
{  /*lint -e730 */
   ASSERT( report_id < DOG_NUM_TASKS ); /*lint +e730 */
   if( report_id < DOG_NUM_TASKS )
   {
     INTLOCK( );
     dog_task_state[report_id].is_blocked = FALSE;
     dog_task_state[report_id].count =
       ( dog_task_info[report_id].timeout == 0 ) ?
       0xff : dog_task_info[report_id].timeout;
     INTFREE( );
   }
} /* END dog_monitor_resume */


/*===========================================================================

FUNCTION DOG_SET_STARTUP_TIME

DESCRIPTION
   Tells Dog how long to wait in milliseconds before starting to monitor tasks.
   Use with care: calling this function to set the startup time to a large
   number will mean that malfunctions may get caught later than usual.
===========================================================================*/
void dog_set_startup_time(
   uint32 startup_time
)
{
   if (startup_time <= DOG_MAX_STARTUP_TIME)
   {
      INTLOCK( );
      dog_startup_time = startup_time;
      dog_startup_cnt  = dog_startup_time / DOG_WAKEUP_TIME;
      INTFREE( );
   }
   else
      ERR( "Dog startup time value too large", 0, 0, 0);

   return;
} /* END dog_set_startup_time */

#ifdef FEATURE_MONITOR_DOG

/*===========================================================================

FUNCTION DOG_MONITOR_REPORT_ISR

DESCRIPTION
   This function is called only when the dog task has not got a chance to
   run and the hardware watchdog is about to expire. This function calls
   err_fatal and execution stops.
===========================================================================*/
void dog_monitor_report_isr()
{
  char dog_mesg[20];

  // Err fatal here
  #ifndef NO_DOG

     dog_kick();

     sprintf(dog_mesg, "Late Dog");
     #ifdef FEATURE_SURF_5200
#error code not present
     #else
        enc_kill_pa();                    /* Turn off the PA         */
     #endif

     dog_kick();

     /* ERR_FATAL - Watchdog Timeout */
     /* Here we muck with the ERR_FATAL macro slightly so that we can
     ** give it a line number of 0000 (indicating dog timeout) and the
     ** string from dog_task_info to indicate the task that failed to
     ** report.  This way the offending task can easily be determined.
     */
     err_fatal_put_log ( 0,dog_mesg);
     /* err_fatal_put_log does not return! */

  #endif /* ! NO_DOG */
}

/*===========================================================================

FUNCTION DOG_SETUP_MONITOR_TIMER

DESCRIPTION
   Sets up a timer to monitor whether the dog task is getting a chance to
   run or not. This timer is reset everytime the dog task runs and when
   the dog task does not get to run for a fixed period of time, an error
   fatal is called.
===========================================================================*/
void dog_setup_monitor_timer(dog_monitor_state state)
{
  uint32 periph_clk_ctl_val;

  INTLOCK( );
  periph_clk_ctl_val = MSM_IN( PERIPH_CLK_CTL );

  if ( state == DOG_RESET_MONITOR )
  {
     /* Do this only if peripheral clock is running at TCXO
     ** CLK_SRC_SEL will be 0 and TCXO is 19.2 Mhz
     */
     if ( (periph_clk_ctl_val & PERIPH_CLK_CTL_CLK_SRC_SEL_BMSK) == 0  )
     {
       MSM_OUT( GPTIMER_CTL, 0 );
       MSM_OUT( GPTIMER_CLOCK, 9600 );
       MSM_OUT( GPTIMER_COUNT, (dog_monitor_timeout*2));
       MSM_OUT( GPTIMER_CTL, GPTIMER_CTL_GPTIMER_ENA_BMSK );
       tramp_set_isr( GP_TIMER_INT, dog_monitor_report_isr );
     }
     else
     {
       MSG_ERROR("GPTimer cannot be setup",0, 0, 0);
     }
  }
  else if ( state == DOG_TURN_OFF_MONITOR )
  {
     tramp_set_isr( GP_TIMER_INT, NULL );
     MSM_OUT( GPTIMER_CTL, 0 );
     MSM_OUT( GPTIMER_CLOCK, 9600 );
     MSM_OUT( GPTIMER_COUNT, (dog_monitor_timeout*2));
  }

  INTFREE( );
}

#endif


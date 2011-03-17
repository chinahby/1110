/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                 T A S K    R E S O U R C E    F I L E

GENERAL DESCRIPTION
  This file contains declarations for task resources, and the task_start
  function.

EXTERNALIZED FUNCTIONS
  task_start
    This function performs task startup processing.
  task_ack
    This function sends back acknowledge signal.
  task_offline
    This function performs task offline processing.
  task_stop
    This function performs task stop processing.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 1990 - 2002 by QUALCOMM Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/services/task/task.c#7 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/27/08   APU	   Added support for FEATURE_VBATT_TASK.	
07/02/08   hs      Added support for FEATURE_CHG_TASK.
11/30/07   rjm     FEATURE_FM switch now independent of ODM builds
06/18/07   va      Added TCXO Manager Task
06/05/07   jd      creation of new GPS_FS task parameters.
11/15/06   ry      Added support for FEATURE_SENSORS
10/03/06   pc      Added support for QTV_TASK10
09/25/06   pc      Added support for FEATURE_DBGR
08/29/06   pc      Removed support for FEATURE_BT_HS, FEATURE_BT_TEST_RC_2_SIO,
                   FEATURE_BT_LPOS and FEATURE_BT_EXTPF_TEST.
08/14/06   pc      Added feature FEATURE_QTV_STATIC_TASKS.
08/09/06   pc      Added support for QTV_VIDEO_RENDERER and QTV_AUDIO
08/01/06   pc      Added support for FEATURE_IXDESCRAMBLERSVC
07/25/06   pc      Added support for FEATURE_CPU_BASED_FLOW_CONTROL
07/17/06   pc      Added support for FEATURE_DMTASK.
06/19/06   pc      Added support for FEATURE_SEC_USFS, 
                   FEATURE_SEC_CONTENT_AGENT and FEATURE_IXSQLSVC.
05/05/06   ac      Added support for FEATURE_QDJTEST
04/19/06   bt      Added UBM L1 Support.
03/24/06   pc      Added support for SECRND task
03/23/06   cl      Added PDSM_COMM_TASK
01/23/06   rlp     Added HID Support.
01/02/06   dle     Added FEATURE_CAMERA_DRV_TASK.
11/10/05   pc      Added support for AVS_SBCENC_TASK
11/08/05   pc      Added support for GSM_PLT task
09/12/05   pc      Added support for FEATURE_USB_HOST_ENHANCED.
08/26/05   pc      Added support for DISP_TASK.
07/22/05   ddh     Added dem task stack and tcb.
07/12/05   jz      Featureized QVP APP Task for Codec Only
06/12/05   pc      Added support for WLAN sec task
04/21/05   pc      Added support for FEATURE_MFLO
04/20/05   pc      Added support for FEATURE_QIPCALL
04/09/05   pc      Added support for CNV task
03/15/05   wal     Added APS task for L4/Linux under FEATURE_APS
03/15/05   adg     Added QDIAG task for L4/Linux under FEATURE_IG_QDIAG
03/15/05   jqi     Added support for FS_FEATURE_RMTEFS_SERVER, FEATURE_L4
                   FEATURE_PM_SERVER,FEATURE_IG_UI_CLIENT_SELECT
02/24/05 t_jarmis  Added FEATURE_FM for FM-RDS Radio.
01/28/05   ptm     Remove support for STA_REMOTE_TASK - it is replaced by
                   ONCRPC proxy tasks and featurize STA separately from ONCRPC
02/05/05   aku     Added support for WLAN_CP and WLAN_TRP tasks.
02/04/05   dwp     Added QVP support for SIP.
02/03/05 eprice    Addition of Sirius specific tasks using define SIRIUS_PLATFORM
01/21/05   ldaita  Added support MGPCC task (needed for GPS).
01/14/05   rp      Added background loading in Progressive boot task support
           jh      Added support for enabling profiling through diag interface.
01/06/05   aar     Added support for GPSSRCH demod task.
11/19/04   jz      Added QVP VFE Task
11/18/04   pa      Added support for PBM task.
11/08/04   ro      Add support for ADSP RTOS Services
10/20/04   ajn     Added support for TIMER task.
10/15/04   rp      Added support for page and interrupt tasks.
10/08/04   jqi     Added support for FEATURE_HSDPA
09/29/04   ss      Added support for DTV (Digital TV) feature.
07/06/04   hl      Added support for joystick feature
05/19/04   vr      Added support for BCDB and HDRBC tasks
04/01/04   sg      removed support for LP task as task no longer exists.
02/23/04   balas   Added support for Bluetooth External Profiles feature support .
02/19/04   Jayanth Added support for FEATURE_SEC_DRM.
           jqi     Fixed lint error.
02/12/04   vdrapk  Added support for FEATURE_GSM_CB.
11/07/03   dd      Added support for GPS Search task.
10/13/03   dwp     Added initial QVPHONE support.
05/29/03   Om      Added support for ARM_CRYPTO and IPSEC tasks.
05/28/03   TMR/dgy Added changes for WCDMA single-mode build.
05/14/03    ld     Added MGP related tasks
03/17/03   jqi     Moved declaration of tasks' stack to task_iram.c
                   wcdma_l1_stack, wcdma_ul_stack, wcdma_dl_stack,ps_stack,
                   usbdc_stack.
03/14/03   jqi     Added GSM/WCDMA feature around TMC task.
03/04/03    lz     Removed FEATURE_MOBILE_SUITE.
02/21/03   jay     Added SSL task
02/18/03   jqi/jar Added Generic SIM Tookit Task (FEATURE_GSTK) Support and
                   Virtual Sim Application Toolkit (FEATURE_VIRTUAL_SAT) Task.
02/14/03   jqi     Added support for FTM task.
12/31/02   jqi     Merged with msm6200 task.c#14
                   1.Added support for FEATURE_GSM_GPRS.
10/09/02   lz      Added ms_task under FEATURE_MOBILE_SUITE.
09/19/02   ck      Changed the misspelt feature name FEATURE_CDAM_1900
09/12/02   jqi     Merged COMMON with MSM6200 archive.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "rex.h"
#include "task.h"
#include "mc.h"


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/*---------------------------------------------------------------------------

                    WATCHDOG TASK DECLARATIONS

---------------------------------------------------------------------------*/

rex_tcb_type          dog_tcb;                     /* TCB for the DOG task */
rex_stack_word_type   dog_stack [DOG_STACK_SIZ];   /* Stack for DOG task   */


#ifdef FEATURE_PROGRESSIVE_BOOT
#ifndef FEATURE_TIMER_TASK
/*---------------------------------------------------------------------------

                       INTERRUPT TASK DECLARATIONS

---------------------------------------------------------------------------*/

rex_tcb_type           interrupt_tcb;
rex_stack_word_type    interrupt_stack[INTERRUPT_STACK_SIZ ];

#endif /* FEATURE_TIMER_TASK */

/*---------------------------------------------------------------------------

                       PAGER TASK DECLARATIONS

---------------------------------------------------------------------------*/

rex_tcb_type           pager_tcb;
rex_stack_word_type    pager_stack[PAGER_STACK_SIZ ];


/*---------------------------------------------------------------------------

                       BACKGROUND LOADER TASK DECLARATIONS

---------------------------------------------------------------------------*/

rex_tcb_type           background_loader_tcb;
rex_stack_word_type    background_loader_stack[BACKGROUND_LOADER_STACK_SIZ];


#endif /* FEATURE_PROGRESSIVE_BOOT */

#ifdef FEATURE_MDSP_TASK
/*---------------------------------------------------------------------------

                  MDSP SERVICES TASK DECLARATIONS

---------------------------------------------------------------------------*/
rex_tcb_type        mdsp_tcb;                    /* TCB for MDSP task.     */
rex_stack_word_type mdsp_stack [MDSP_STACK_SIZ]; /* Stack for MDSP task.   */
#endif

#ifdef FEATURE_QDSP_TASK
/*---------------------------------------------------------------------------

                  QDSP SERVICES TASK DECLARATIONS

---------------------------------------------------------------------------*/

rex_tcb_type        qdsp_tcb;                    /* TCB for QDSP task.     */
rex_stack_word_type qdsp_stack [QDSP_STACK_SIZ]; /* Stack for QDSP task.   */
#endif

#ifdef FEATURE_QDSP_RTOS
/*---------------------------------------------------------------------------

                  ADSP RTOS SERVICES TASK DECLARATIONS

---------------------------------------------------------------------------*/

rex_tcb_type        adsp_rtos_tcb;          /* TCB for ADSP RTOS task.     */
                                            /* Stack for ADSP RTOS task.   */
rex_stack_word_type adsp_rtos_stack [ADSP_RTOS_STACK_SIZ];
#endif

/*---------------------------------------------------------------------------

                     VOCODER TASK DECLARATIONS

---------------------------------------------------------------------------*/

rex_tcb_type        voc_tcb;                     /* TCB for VOC task.      */
rex_stack_word_type voc_stack [VOC_STACK_SIZ];   /* Stack for VOC task.    */

/*---------------------------------------------------------------------------

                     SOUND TASK DECLARATIONS

---------------------------------------------------------------------------*/

rex_tcb_type        snd_tcb;                     /* TCB for SND task.      */
rex_stack_word_type snd_stack [SND_STACK_SIZ];   /* Stack for SND task.    */

#ifdef FEATURE_AVS_SBCENC_TASK 
/*---------------------------------------------------------------------------

                     SBC ENCODER TASK DECLARATIONS

---------------------------------------------------------------------------*/

rex_tcb_type        sbcenc_tcb;                     /* TCB for   SBCENC task. */
rex_stack_word_type sbcenc_stack [SBCENC_STACK_SIZ];/* Stack for SBCENC task. */

#endif

/*---------------------------------------------------------------------------

                NON-VOLATILE DATABASE TASK DECLARATIONS

---------------------------------------------------------------------------*/

rex_tcb_type        nv_tcb;                      /* TCB for NV task.       */
#ifndef CUST_EDITION
rex_stack_word_type nv_stack [NV_STACK_SIZ];     /* Stack for NV task.     */
#endif

/*---------------------------------------------------------------------------

                EMBEDDED FILE SYSTEM TASK DECLARATIONS

---------------------------------------------------------------------------*/

#if defined FEATURE_EFS
rex_tcb_type        fs_tcb;                  /* TCB for File System task.  */
rex_stack_word_type fs_stack [FS_STACK_SIZ]; /* Stack for File System task.*/

#if defined(FS_FEATURE_RMTEFS_SERVER) && !defined(FEATURE_IG_EFS_EXT_SERVER)
rex_tcb_type        efs_srvr_tcb;
/* Actually, stack size of 2048 bytes is sufficient but we use 4096 here */
rex_stack_word_type efs_srvr_stack[EFS_SRVR_STACK_SIZ];
#endif /* FS_FEATURE_RMTEFS_SERVER && !FEATURE_IG_EFS_EXT_SERVER */

#endif /* FEATURE_EFS */

#ifdef FEATURE_EFS_SFAT
rex_tcb_type        sfat_tcb;                    /* TCB for SFAT task.  */
rex_stack_word_type sfat_stack [SFAT_STACK_SIZ]; /* Stack for SFAT task.*/
#endif /* FEATURE_EFS_SFAT */

#ifdef FEATURE_EFS_FDI
rex_tcb_type        fdi_bg_tcb;                  /* TCB for FDI Background task.  */ 
rex_stack_word_type fdi_bg_stack [FDI_BG_STACK_SIZ]; /* Stack for FDI Background task.*/
#endif


#if ( defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900) || defined(FEATURE_ACP) )
/*---------------------------------------------------------------------------

                      TRANSMIT TASK DECLARATIONS

---------------------------------------------------------------------------*/

#ifndef FEATURE_HWTC
rex_tcb_type        tx_tcb;                    /* TCB for TX task.         */
rex_stack_word_type tx_stack [TX_STACK_SIZ];   /* Stack for TX task.       */
#endif /* !FEATURE_HWTC */

/*---------------------------------------------------------------------------

                   RECEIVE TASK DECLARATIONS

---------------------------------------------------------------------------*/

rex_tcb_type        rx_tcb;                      /* TCB for RX task.       */
rex_stack_word_type rx_stack [RX_STACK_SIZ];     /* Stack for RX task.     */

/*---------------------------------------------------------------------------

                   SEARCHER TASK DECLARATIONS

---------------------------------------------------------------------------*/

#ifndef FEATURE_HWTC
rex_tcb_type        srch_tcb;                    /* TCB for SRCH task.     */
rex_stack_word_type srch_stack [SRCH_STACK_SIZ]; /* Stack for SRCH task.   */
#endif /* !FEATURE_HWTC */

/*---------------------------------------------------------------------------

                   LAYER2(RXTX) TASK DECLARATIONS

---------------------------------------------------------------------------*/

#ifndef FEATURE_HWTC
rex_tcb_type        rxtx_tcb;                    /* TCB for RXTX task.     */
rex_stack_word_type rxtx_stack [RXTX_STACK_SIZ]; /* Stack for RXTX task.   */
#endif /* !FEATURE_HWTC */


/*---------------------------------------------------------------------------

                   MAIN CONTROL TASK DECLARATIONS

---------------------------------------------------------------------------*/

rex_tcb_type        mc_tcb;                      /* TCB for MC task.       */
rex_stack_word_type mc_stack [MC_STACK_SIZ];     /* Stack for MC task.     */

#endif/* FEATURE_CDMA_800 || FEATURE_CDMA_1900 || FEATURE_ACP */

#ifdef FEATURE_DIAG_RPC
#error code not present
#endif /* #ifdef FEATURE_DIAG_RPC */

/*---------------------------------------------------------------------------

                     HANDSET TASK DECLARATIONS

---------------------------------------------------------------------------*/

rex_tcb_type        hs_tcb;                      /* TCB for the HS task.   */
rex_stack_word_type hs_stack [HS_STACK_SIZ];     /* Stack for HS task.     */

/*---------------------------------------------------------------------------

                     DIAGNOSTIC TASK DECLARATIONS

---------------------------------------------------------------------------*/

rex_tcb_type        diag_tcb;                    /* TCB for DIAG task.     */
rex_stack_word_type diag_stack [DIAG_STACK_SIZ]; /* Stack for DIAG Task.   */

#if defined(FEATURE_IG_QDIAG)
rex_tcb_type        qdiag_tcb;                     /* TCB for QDIAG task.   */
rex_stack_word_type qdiag_stack [QDIAG_STACK_SIZ]; /* Stack for QDIAG Task. */
#endif  /* defined(FEATURE_IG_QDIAG) */


/*---------------------------------------------------------------------------

                     CALL MANAGER TASK DECLARATIONS

---------------------------------------------------------------------------*/

#ifdef FEATURE_CM
rex_tcb_type        cm_tcb;                      /* TCB for CM task.       */
rex_stack_word_type cm_stack [CM_STACK_SIZ];     /* Stack for CM Task.     */
#endif /* FEATURE_CM */

#ifdef FEATURE_BT
/*---------------------------------------------------------------------------

                    BLUETOOTH TASK DECLARATIONS

---------------------------------------------------------------------------*/

rex_tcb_type        bt_tcb;                      /* TCB for the BT task.   */
rex_stack_word_type bt_stack [BT_STACK_SIZ];     /* Stack for BT task.     */

#ifdef FEATURE_BT_EXTPF
/*---------------------------------------------------------------------------

              BLUETOOTH EXTERNAL PROFILE TASK DECLARATIONS

---------------------------------------------------------------------------*/

rex_tcb_type        bt_pf_tcb;
rex_stack_word_type bt_pf_stack [BT_PF_STACK_SIZ];

#endif /* FEATURE_BT_EXTPF */

#endif /* FEATURE_BT */

/*--------------------------------------------------------------------------

                   POSITION DETERMINATION SESSION MANAGER DECLARATIONS

---------------------------------------------------------------------------*/

#if defined(FEATURE_GPSONE) && !defined(FEATURE_CGPS)
#error code not present
#endif

/*--------------------------------------------------------------------------

                   POSITION DETERMINATION SMS CLIENT DECLARATIONS

---------------------------------------------------------------------------*/

#ifdef FEATURE_GPSONE_SMS_CLIENT
#error code not present
#endif

/*--------------------------------------------------------------------------

                   POSITION DETERMINATION COMMUNICATION MANAGER DECLARATIONS

---------------------------------------------------------------------------*/

#if defined(FEATURE_GPSONE) && !defined(FEATURE_CGPS)
#error code not present
#endif


#ifdef FEATURE_GPSONE_PE
#error code not present
#endif /* FEATURE_GPSONE_PE */

#ifdef FEATURE_GPSSRCH_TASK
#error code not present
#endif /* FEATURE_GPSSRCH_TASK */


#ifdef FEATURE_GPSONE_STANDALONE
#error code not present
#endif /* FEATURE_GPSONE_STANDALONE */



#ifdef FEATURE_DBGR
/*---------------------------------------------------------------------------

                     DEBUGGER TASK DECLARATIONS

---------------------------------------------------------------------------*/
rex_tcb_type        dbgr_tcb;                    /* TCB for DBGR task.     */
rex_stack_word_type dbgr_stack [DBGR_STACK_SIZ]; /* Stack for DBGR Task.   */
#endif /*FEATURE_DBGR*/

/*---------------------------------------------------------------------------

                     USER INTERFACE TASK DECLARATIONS

---------------------------------------------------------------------------*/

rex_tcb_type        ui_tcb;                      /* TCB for UI task.       */
#ifndef CUST_EDITION
rex_stack_word_type ui_stack [UI_STACK_SIZ];     /* Stack for UI Task.     */
#endif
/*---------------------------------------------------------------------------

                     WMS TASK DECLARATIONS

---------------------------------------------------------------------------*/

#if defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS)
rex_tcb_type        wms_tcb;                     /* TCB for WMS task.  */
#ifndef CUST_EDITION
rex_stack_word_type wms_stack [WMS_STACK_SIZ]; /* Stack for WMS Task.*/
#endif
#endif /* FEATURE_CDSMS || FEATURE_GWSMS */

/*---------------------------------------------------------------------------

                       TIMER TASK DECLARATIONS

---------------------------------------------------------------------------*/

#if defined(FEATURE_TIMER_TASK)
rex_tcb_type        timer_tcb;                    /* TCB for Timer task.   */
rex_stack_word_type timer_stack [TIMER_STACK_SIZ];/* Stack for Timer Task. */
#endif


/*---------------------------------------------------------------------------

                       SLEEP TASK DECLARATIONS

---------------------------------------------------------------------------*/

#if defined(T_SLEEP) || defined(FEATURE_SLEEP)
rex_tcb_type        sleep_tcb;                    /* TCB for Sleep task.   */
rex_stack_word_type sleep_stack [SLEEP_STACK_SIZ];/* Stack for Sleep Task. */
#endif


/*---------------------------------------------------------------------------

                       DEM TASK DECLARATIONS

---------------------------------------------------------------------------*/

#if ((defined( IMAGE_APPS_PROC ) && defined(T_MSM7500)) || \
     (defined ( FEATURE_APPS_TCXO_SLEEP )))
rex_tcb_type        dem_tcb;                      /* TCB for DEM task.     */
rex_stack_word_type dem_stack [DEM_STACK_SIZ];    /* Stack for DEM Task.   */
#endif /* (IMAGE_APPS_PROC  && defined (T_MSM7500)) ||
          (FEATURE_APPS_TCXO_SLEEP) */

#ifdef FEATURE_HWTC
#error code not present
#endif /* FEATURE_HWTC */

#ifdef FEATURE_DATA

/*---------------------------------------------------------------------------

                     DATA PROTOCOL TASK DECLARATIONS

---------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_PS
/* Stacks for PS Task is declared in task_iram.c. So that it can be loaded to
** IRAM or not based on target's need.
*/
rex_tcb_type        ps_tcb;                   /* TCB for DATA task.        */
#endif /* FEATURE_DATA_PS */

#ifdef FEATURE_WLAN
#error code not present
#endif /* FEATURE_WLAN */

#ifdef FEATURE_COMP_TASK
/*---------------------------------------------------------------------------

                     COMP DECLARATIONS

---------------------------------------------------------------------------*/

rex_tcb_type        comp_tcb;                      /* TCB for DATA task.     */
rex_stack_word_type comp_stack [COMP_STACK_SIZ];     /* Stack for DATA Task.   */

#endif /* FEATURE_COMP_TASK */

/*---------------------------------------------------------------------------

                     DATA SERVICES TASK DECLARATIONS

---------------------------------------------------------------------------*/

rex_tcb_type        ds_tcb;                      /* TCB for DATA task.     */
rex_stack_word_type ds_stack [DS_STACK_SIZ];     /* Stack for DATA Task.   */

#ifdef FEATURE_BCMCS
#error code not present
#endif

#ifdef FEATURE_WCDMA
#error code not present
#endif /* FEATURE_WCDMA */

#if ( (defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS)) && !defined(FEATURE_WCDMA_PLT) && !defined(FEATURE_GSM_PLT) )
/*---------------------------------------------------------------------------

                   GCSD PROTOCOL STACK TASK DECLARATIONS

---------------------------------------------------------------------------*/

/* TCB for GCSD Protocol stack task */
rex_tcb_type        Gcsd_tcb;

/* Stack for GCSD Protocol stack task */
rex_stack_word_type Gcsd_stack[DS_GCSD_STACK_SIZ];

#endif /* ( (defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS)) && !defined(FEATURE_WCDMA_PLT) && !defined(FEATURE_GSM_PLT)) */

#endif /* FEATURE_DATA */

#ifdef FEATURE_ONCRPC
#error code not present
#endif /* FEATURE_ONCRPC */

#ifdef FEATURE_SEC

/*---------------------------------------------------------------------------

                     SECURITY SERVICES TASK DECLARATIONS

---------------------------------------------------------------------------*/

rex_tcb_type        sec_tcb;                     /* TCB for SEC task.      */
rex_stack_word_type sec_stack [SEC_STACK_SIZ];   /* Stack for SEC Task.    */

#ifdef FEATURE_SECRND
rex_tcb_type        secrnd_tcb;                  /* TCB for SECRND task.   */
rex_stack_word_type secrnd_stack [SECRND_STACK_SIZ];/* Stack for SECRND Task. */
#endif

#ifdef FEATURE_SEC_USFS
rex_tcb_type        sfs_tcb;                  /* TCB for SFS task.   */
rex_stack_word_type sfs_stack [SFS_STACK_SIZ];/* Stack for SFS Task. */
#endif

#ifdef FEATURE_SEC_ARM_CRYPTO
/*---------------------------------------------------------------------------

                     CRYPTO ARM TASK DECLARATIONS

---------------------------------------------------------------------------*/
rex_tcb_type         seccryptarm_tcb;
rex_stack_word_type  seccryptarm_stack[ SECCRYPTARM_STACK_SIZ ];

#endif /* FEATURE_SEC_ARM_CRYPTO */

#ifdef FEATURE_SEC_IPSEC
/*---------------------------------------------------------------------------

                       IPSEC TASK DECLARATIONS

---------------------------------------------------------------------------*/
rex_tcb_type         secips_tcb;                       /* Stack for task */
rex_stack_word_type  secips_stack[ SECIPS_STACK_SIZ ]; /* TCB for task   */

#endif /* FEATURE_SEC_IPSEC */

#ifdef FEATURE_SEC_SSL
/*---------------------------------------------------------------------------

                     SSL TASK DECLARATIONS

---------------------------------------------------------------------------*/

rex_tcb_type        secssl_tcb;                      /* TCB for SEC task.  */
rex_stack_word_type secssl_stack [SECSSL_STACK_SIZ]; /* Stack for SEC Task.*/

#endif /* FEATURE_SEC_SSL */

#ifdef FEATURE_SEC_DRM
#error code not present
#endif /* FEATURE_SEC_DRM */

#ifdef FEATURE_SEC_CONTENT_AGENT
/*---------------------------------------------------------------------------

                     Content Agent Diag TASK DECLARATIONS

---------------------------------------------------------------------------*/

rex_tcb_type        contentagentdiag_tcb;                       /* TCB for CONTENTAGENTDIAG task.  */
rex_stack_word_type contentagentdiag_stack [CONTENTAGENTDIAG_STACK_SIZ]; /* Stack for ContentAgentDiag. */

#endif /* FEATURE_SEC_CONTENT_AGENT */

#if (defined(FEATURE_SEC_SSL_TESTAPP) || defined(FEATURE_SEC_CRYPT_TESTAPP))
#error code not present
#endif /* FEATURE_SEC_SSL_TESTAPP || FEATURE_SEC_CRYPT_TESTAPP */

#endif /* FEATURE_SEC */

#ifdef FEATURE_DS_SOCKETS_TEST_TCP
#error code not present
#endif

#ifdef FEATURE_DS_SOCKETS_TEST_UDP
#error code not present
#endif



#ifdef FEATURE_QMIP
#error code not present
#endif

/*---------------------------------------------------------------------------

                     AUTHENTICATION TASK DECLARATIONS

---------------------------------------------------------------------------*/

#if defined (FEATURE_AUTH) || defined (FEATURE_RUIM)
rex_tcb_type         auth_tcb;                    /* TCB for Auth task.    */
rex_stack_word_type  auth_stack [AUTH_STACK_SIZ]; /* Stack for Auth Task.  */

#endif /* FEATURE_AUTH || FEATURE_RUIM */

/*---------------------------------------------------------------------------

                     UIM TASK DECLARATIONS

---------------------------------------------------------------------------*/

#ifdef FEATURE_UIM
rex_tcb_type        uim_tcb;                       /* TCB for UIM task.    */
rex_stack_word_type uim_stack [UIM_STACK_SIZ];     /* Stack for UIM Task.  */
#endif /* FEATURE_UIM */

/*---------------------------------------------------------------------------

                            DH TASK DECLARATIONS

---------------------------------------------------------------------------*/

#ifdef FEATURE_DH
rex_tcb_type        dh_tcb;                    /* TCB for DH task.         */
rex_stack_word_type dh_stack [DH_STACK_SIZ];   /* Stack for DH Task.       */

#endif /*FEATURE_DH */

/*---------------------------------------------------------------------------

                     VOICE SERVICES TASK DECLARATIONS

---------------------------------------------------------------------------*/

#ifdef FEATURE_VS_TASK
rex_tcb_type        vs_tcb;                        /* TCB for VS task.     */
rex_stack_word_type vs_stack [VS_STACK_SIZ];       /* Stack for VS Task.   */

#endif /* FEATURE_VS_TASK */


/*---------------------------------------------------------------------------

                         ATI GRAPHICS TASK DECLARATIONS

---------------------------------------------------------------------------*/

#ifdef FEATURE_ATI_GRAPHICS
rex_tcb_type        graphics_tcb;                        /* TCB for Graphics task   */
rex_stack_word_type graphics_stack [GRAPHICS_STACK_SIZ]; /* Stack for Graphics Task */

#endif /* FEATURE_ATI_GRAPHICS */

/*---------------------------------------------------------------------------

                         GRAPHICS TASK DECLARATIONS

---------------------------------------------------------------------------*/

#ifdef FEATURE_GRAPH_TASK
rex_tcb_type        graph_tcb;                     /* TCB for Graphics task   */
rex_stack_word_type graph_stack [GRAPH_STACK_SIZ]; /* Stack for Graphics Task */

#endif /* FEATURE_GRAPH_TASK */


/*---------------------------------------------------------------------------

                         CAMERA DRIVER TASK DECLARATIONS

---------------------------------------------------------------------------*/

#ifdef FEATURE_CAMERA_DRV_TASK
rex_tcb_type        camera_drv_tcb;
rex_stack_word_type camera_drv_stack [CAMERA_DRV_STACK_SIZ];
#endif /* FEATURE_CAMERA_DRV_TASK */


/*---------------------------------------------------------------------------

                USBDC TASK DECLARATIONS

---------------------------------------------------------------------------*/
/* The stack of USB task is moved to task_iram.c. So that it can be loaded to
** IRAM or not based on target's need.
*/
#if defined (FEATURE_USB) && !defined (FEATURE_NO_USB_HW_ON_PROCESSOR)
rex_tcb_type        usbdc_tcb;                      /* TCB for USBDC task.  */


/*---------------------------------------------------------------------------

                USBHC TASK DECLARATIONS

---------------------------------------------------------------------------*/
#ifdef FEATURE_USB_HOST_ENHANCED
rex_tcb_type        usbhc_urb_tcb;             /* TCB for USBHC_URB task.  */
rex_stack_word_type usbhc_urb_stack [USBHC_URB_STACK_SIZ]; /* Stack for USBHC_URB Task */

rex_tcb_type        usbhc_dev_tcb;             /* TCB for USBHC_DEV task.  */
rex_stack_word_type usbhc_dev_stack [USBHC_DEV_STACK_SIZ]; /* Stack for USBHC_DEV Task. */

#if defined(FEATURE_USB_HOST_TEST_ENHANCED) && \
    (defined(FEATURE_USB_HOST_MASS_STORAGE_TEST_ENHANCED) || \
     defined(FEATURE_USB_HOST_HID_TEST_ENHANCED))
rex_tcb_type        usbhc_test_tcb;             /* TCB for USBHC_TEST task.  */
rex_stack_word_type usbhc_test_stack [USBHC_TEST_STACK_SIZ]; /* Stack for USBHC_TEST Task. */

#endif
#endif /* FEATURE_USB_HOST_ENHANCED */

#endif /* (FEATURE_USB) && !defined (FEATURE_NO_USB_HW_ON_PROCESSOR) */
/*---------------------------------------------------------------------------

                            TDSO TASK DECLARATIONS

---------------------------------------------------------------------------*/

#ifdef FEATURE_IS2000_TDSO
rex_tcb_type        tdso_tcb;                      /* TCB for TDSO task.   */
rex_stack_word_type tdso_stack [TDSO_STACK_SIZ];   /* Stack for TDSO Task. */
#endif /* FEATURE_IS2000_TDSO */


#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */

#ifdef FEATURE_MULTIMODE_ARCH
/*---------------------------------------------------------------------------

                   MMOC TASK DECLARATIONS

---------------------------------------------------------------------------*/
rex_tcb_type        mmoc_tcb;                      /* TCB for MMOC task.   */
rex_stack_word_type mmoc_stack [MMOC_STACK_SIZ];   /* Stack for MMOC task. */

#endif /* FEATURE_MULTIMODE_ARCH */

#if (defined(FEATURE_MULTIMODE_ARCH) || defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
/*---------------------------------------------------------------------------

                   MAIN CONTROL TASK DECLARATIONS

---------------------------------------------------------------------------*/

rex_tcb_type        tmc_tcb;                    /* TCB for TMC task.       */
rex_stack_word_type tmc_stack [TMC_STACK_SIZ];   /* Stack for TMC task.    */

#endif /* FEATURE_MULTIMODE_ARCH || FEATURE_GSM || FEATURE_WCDMA */

/*---------------------------------------------------------------------------

                     GSDI TASK DECLARATIONS

---------------------------------------------------------------------------*/

#if (defined(FEATURE_MMGSDI) || defined(FEATURE_NAS) || defined(FEATURE_VIRTUAL_SIM))
rex_tcb_type        gsdi_task_tcb;               /* TCB for GSDI  task.    */
rex_stack_word_type gsdi_stack [GSDI_STACK_SIZ];  /* Stack for GSDI Task.  */
#endif /* FEATURE_MMGSDI || FEATURE_NAS || FEATURE_VIRTAUL_SIM */

/*---------------------------------------------------------------------------

                   GSDI UIM VIRTUAL SERVER TASK DECLARATIONS

---------------------------------------------------------------------------*/

#ifdef FEATURE_VIRTUAL_SIM
rex_tcb_type      gsdi_uim_server_task_tcb;                      /* Stack for TMC Task. */
rex_stack_word_type   gsdi_uim_server_stack[ GSDI_UIM_SERVER_STACK_SIZ ];   /* TCB for TMC task.   */
#endif  /* FEATURE_VIRTUAL_SIM */

/*---------------------------------------------------------------------------

                   GSTK TASK DECLARATIONS

---------------------------------------------------------------------------*/

#if defined (FEATURE_GSTK)
rex_tcb_type          gstk_tcb;                      /* Stack for GSTK Task. */
rex_stack_word_type   gstk_stack[ GSTK_STACK_SIZ ];   /* TCB for GSTK task.   */
#endif /* FEATURE_GSTK */

/*---------------------------------------------------------------------------

                   GSDI VIRTUAL SAT APPLICATION TASK  DECLARATIONS

---------------------------------------------------------------------------*/
#if defined(FEATURE_GSTK) && defined(FEATURE_VIRTUAL_SAT)
rex_tcb_type          virtsat_tcb;                      /* Stack for VIRTSAT Task. */
rex_stack_word_type   virtsat_stack[ VIRTSAT_STACK_SIZ ];  /* TCB for VIRTSAT task.   */
#endif /*  defined(FEATURE_GSTK) && defined(FEATURE_VIRTUAL_SAT) */

/*---------------------------------------------------------------------------

                   WCDMA LAYER 1, 2, RRC TASK DECLARATIONS

---------------------------------------------------------------------------*/
#ifdef FEATURE_WCDMA
#error code not present
#endif /* FEATURE_WCDMA */

/*------------------------------------------------------------------------

                   GSM LAYER 1, 2, RR TASK DECLARATIONS

------------------------------------------------------------------------*/
#ifdef FEATURE_GSM
#error code not present
#endif /* FEATURE_GSM */


/*---------------------------------------------------------------------------

                   AL1 TASK DECLARATIONS  (For WCDMA VST)

---------------------------------------------------------------------------*/
#if ( defined(FEATURE_WCDMA_VST_FULLSTACK) || defined(FEATURE_WCDMA_VST_AS))
rex_tcb_type    al1_tcb;                       /* TCB for AL1 task.   */
rex_stack_word_type al1_stack [AL1_STACK_SIZ];     /* Stack for AL1 Task. */
#endif /* FEATURE_WCDMA_VST_FULLSTACK ... */

/*---------------------------------------------------------------------------

                   RRCUT TASK DECLARATIONS  (For WCDMA VST Access-Stratum)

---------------------------------------------------------------------------*/
#ifdef FEATURE_WCDMA_VST_AS
#error code not present
#endif /* FEATURE_WCDMA_VST_AS */

/*---------------------------------------------------------------------------

                   TC TASK DECLARATIONS

---------------------------------------------------------------------------*/
#ifdef FEATURE_TC
rex_tcb_type      tc_tcb;                      /* Stack for TC Task. */
rex_stack_word_type   tc_stack[ TC_STACK_SIZ ];    /* TCB for TC task.   */
#endif /* FEATURE_TC */


#ifdef FEATURE_NAS
#error code not present
#endif /* FEATURE_NAS */

/*---------------------------------------------------------------------------

                   MENU TASK DECLARATIONS

---------------------------------------------------------------------------*/
#if ( defined(FEATURE_WCDMA_VST_FULLSTACK) || defined(FEATURE_WCDMA_VST_AS))
/* #ifdef FEATURE_MENU_UT */
rex_tcb_type    menu_tcb;                       /* TCB for REG task.   */
rex_stack_word_type menu_stack [MENU_STACK_SIZ];     /* Stack for REG Task. */
#endif /* FEATURE_WCDMA_VST_FULLSTACK ... */

/*---------------------------------------------------------------------------

                   WPLT TASK DECLARATIONS

---------------------------------------------------------------------------*/
#ifdef FEATURE_WCDMA_PLT
#error code not present
#endif /* FEATURE_WCDMA_PLT */


/*---------------------------------------------------------------------------

                   GPLT TASK DECLARATIONS

---------------------------------------------------------------------------*/
#ifdef FEATURE_GSM_PLT
#error code not present
#endif /* FEATURE_GSM_PLT */

/*------------------------------------------------------------------------

                   LSM TASK DECLARATIONS

------------------------------------------------------------------------*/
#ifdef FEATURE_MMGPS
#error code not present
#endif /* FEATURE_MMGPS */

/*------------------------------------------------------------------------

                   MGP TASK DECLARATIONS

------------------------------------------------------------------------*/
#ifdef FEATURE_MGP
  rex_tcb_type         mgpmc_tcb;                      /* TCB for MGPMC task */
  rex_stack_word_type  mgpmc_stack[ MGPMC_STACK_SIZ ]; /* Stack for MGPMC task */

  rex_tcb_type         mgpai_tcb;                      /* TCB for MGPAI task */
  rex_stack_word_type  mgpai_stack[ MGPAI_STACK_SIZ ]; /* Stack for MGPAI task */

  rex_tcb_type         mgpnf_tcb;                      /* TCB for MGPNF task */
  rex_stack_word_type  mgpnf_stack[ MGPNF_STACK_SIZ ]; /* Stack for MGPNF task */

  rex_tcb_type         mgpcd_tcb;                      /* TCB for MGPCD task */
  rex_stack_word_type  mgpcd_stack[ MGPCD_STACK_SIZ ]; /* Stack for MGPCD task */

  #ifdef FEATURE_MGPCC_TASK
  rex_tcb_type         mgpcc_tcb;                      /* TCB for MGPCC task */
  rex_stack_word_type  mgpcc_stack[ MGPCC_STACK_SIZ ]; /* Stack for MGPCC task */
  #endif

#endif /* FEATURE_MGP */

#ifdef FEATURE_CGPS
#error code not present
#endif /* FEATURE_CGPS */

/*------------------------------------------------------------------------

                   FTM TASK DECLARATIONS

------------------------------------------------------------------------*/
#ifdef FEATURE_FTM_TASK
  rex_tcb_type         ftm_tcb;                    /* Stack for ftm Task. */
  rex_stack_word_type  ftm_stack[ FTM_STACK_SIZ ]; /* TCB for ftm task.   */
#endif /* FEATURE_FTM_TASK */

/*---------------------------------------------------------------------------

                     QVPHONE TASK DECLARATIONS

---------------------------------------------------------------------------*/
#ifdef FEATURE_QVPHONE

  /* QVP Audio Engine task and TCB */
  rex_tcb_type        qvp_ae_tcb;
  rex_stack_word_type qvp_ae_task_stack[QVP_AE_STACK_SIZ];

  /* QVP MPEG4 task and TCB */
  rex_tcb_type        qvp_mpeg4_tcb;
  rex_stack_word_type qvp_mpeg4_stack[QVP_MPEG4_STACK_SIZ];

  #ifdef FEATURE_QVP_APP_INCLUDE
#error code not present
  #endif

  /* QVP IO task and TCB */
  rex_tcb_type        qvpio_tcb;
  rex_stack_word_type qvpio_stack [QVPIO_STACK_SIZ];

  /* QVP Player task and TCB */
  rex_tcb_type        qvppl_tcb;
  rex_stack_word_type qvppl_stack [QVPPL_STACK_SIZ];

  /* QVP VFE task and TCB */
  rex_tcb_type        qvp_vfe_tcb;
  rex_stack_word_type qvp_vfe_stack[QVP_VFE_STACK_SIZ];

#ifdef FEATURE_QVPHONE_SIP
#error code not present
#endif /* FEATURE_QVPHONE_SIP */
#endif /* FEATURE_QVPHONE */

/*------------------------------------------------------------------------

                   DTV TASK DECLARATIONS

------------------------------------------------------------------------*/
#ifdef FEATURE_DTV
#error code not present
#endif /* FEATURE_DTV */

/*--------------------------------------------------------------------------
                            JOYSTICK TASK DECLARATIONS

---------------------------------------------------------------------------*/
#ifdef FEATURE_JOYST_TASK

   rex_tcb_type        joyst_tcb;                     /* TCB for Joystick Task.  */
   rex_stack_word_type joyst_stack [JOYST_STACK_SIZ]; /* Stack for Joystick Task.*/

#endif  /* FEATURE_JOYST_TASK */

/*---------------------------------------------------------------------------

                     PHONE BOOK MANAGER TASK DECLARATIONS

---------------------------------------------------------------------------*/

#ifdef FEATURE_PBM_TASK
rex_tcb_type        pbm_tcb;                      /* TCB for PBM task.       */
rex_stack_word_type pbm_stack [PBM_STACK_SIZ];    /* Stack for PBM Task.     */
#endif /* FEATURE_PBM_TASK */

#ifdef SIRIUS_PLATFORM
#error code not present
#endif /* SIRIUS_PLATFORM */

#ifdef FEATURE_FM
/*---------------------------------------------------------------------------

                    FM-RDS TASK DECLARATIONS

---------------------------------------------------------------------------*/

rex_tcb_type        fmrds_tcb;                        /* TCB for the FM-RDS task. */
rex_stack_word_type fmrds_stack [FMRDS_STACK_SIZ];    /* Stack for FM-RDS task.   */

#endif // FEATURE_FM

/*---------------------------------------------------------------------

                     PM SERVER DECLARATIONS

 ------------------------------------------------------------------*/

#ifdef FEATURE_PM_SERVER
 rex_tcb_type        pms_tcb;
 rex_stack_word_type pms_stack[PMSERV_STACK_SIZ];
#endif /* FEATURE_PM_SERVER */

/*---------------------------------------------------------------------------

                 AMSS PROXY SERVICE TASK DECLARATIONS

---------------------------------------------------------------------------*/

#if defined(FEATURE_APS)
rex_tcb_type        aps_tcb;                     /* TCB for APS task.   */
rex_stack_word_type aps_stack [APS_STACK_SIZ];   /* Stack for APS Task. */
#endif  /* defined(FEATURE_APS) */

/*---------------------------------------------------------------------------

                 AMSS TIMER SERVER TASK DECLARATIONS

---------------------------------------------------------------------------*/

#if defined(FEATURE_ATS)
rex_tcb_type        ats_tcb;                     /* TCB for ATS task.   */
rex_stack_word_type ats_stack [ATS_STACK_SIZ];   /* Stack for ATS Task. */
#endif  /* defined(FEATURE_ATS) */

#ifdef FEATURE_DISP_TASK
/*---------------------------------------------------------------------------

                    DISPLAY TASK DECLARATIONS

---------------------------------------------------------------------------*/
/* TCB for Display Task.   */
rex_tcb_type        disp_tcb;
/* Stack for Display Task.   */
rex_stack_word_type disp_stack [DISP_STACK_SIZ];

#endif /* FEATURE_DISP_TASK */
/*---------------------------------------------------------------------------

                     COMPACT NV TASK DECLARATIONS

---------------------------------------------------------------------------*/

#ifdef FEATURE_NV_CNV
rex_tcb_type        cnv_tcb;                      /* TCB for CNV task.     */
rex_stack_word_type cnv_stack [CNV_STACK_SIZ];     /* Stack for CNV Task.  */
#endif /* FEATURE_NV_CNV */

/*---------------------------------------------------------------------------

                     DM TASK DECLARATIONS

---------------------------------------------------------------------------*/

#ifdef FEATURE_DMTASK
rex_tcb_type        dmtask_tcb;                      /* TCB for DM task.     */
rex_stack_word_type dmtask_stack [DMTASK_STACK_SIZ];    /* Stack for DM Task.  */
#endif /* FEATURE_DMTASK */


/*---------------------------------------------------------------------------

                 QIPCALL TASK DECLARATIONS

---------------------------------------------------------------------------*/

#if defined(FEATURE_QIPCALL)
rex_tcb_type        qipcall_tcb;                  /* TCB for QIPCALL task. */
rex_stack_word_type qipcall_stack [QIPCALL_STACK_SIZ];  /* Stack for QIPCALL Task*/
#endif  /* defined(FEATURE_QIPCALL) */

/*---------------------------------------------------------------------------

                 FLOW CONTROLLER TASK DECLARATIONS

---------------------------------------------------------------------------*/

#if defined(FEATURE_CPU_BASED_FLOW_CONTROL)
rex_tcb_type        fc_tcb;                     /* TCB for FC  task.   */
rex_stack_word_type fc_stack [FC_STACK_SIZ];   /* Stack for FC Task. */
#endif  /* defined(FEATURE_CPU_BASED_FLOW_CONTROL) */

/*---------------------------------------------------------------------------

                            MediaFLO TASK DECLARATIONS

---------------------------------------------------------------------------*/
#ifdef FEATURE_MFLO
#error code not present
#endif /* FEATURE_MFLO */

/*---------------------------------------------------------------------------

                          IxFileSvc TASK DECLARATIONS

---------------------------------------------------------------------------*/
#ifdef FEATURE_IXFILESVC

rex_tcb_type        ix_filesvc_tcb;
rex_stack_word_type ix_filesvc_stack[IXFILESVC_STACK_SIZ];

#endif /* FEATURE_IXFILESVC */

/*---------------------------------------------------------------------------

                          IxHTTPSvc TASK DECLARATIONS

---------------------------------------------------------------------------*/
#ifdef FEATURE_IXHTTPSVC

rex_tcb_type        ix_httpsvc_tcb;
rex_stack_word_type ix_httpsvc_stack[IXHTTPSVC_STACK_SIZ];

#endif /* FEATURE_IXHTTPSVC */

/*---------------------------------------------------------------------------

                          IxTimeSvc TASK DECLARATIONS

---------------------------------------------------------------------------*/
#ifdef FEATURE_IXTIMESVC

rex_tcb_type        ix_timesvc_tcb;
rex_stack_word_type ix_timesvc_stack[IXTIMESVC_STACK_SIZ];

#endif /* FEATURE_IXTIMESVC */

/*---------------------------------------------------------------------------

                          IxContentSvc TASK DECLARATIONS

---------------------------------------------------------------------------*/
#ifdef FEATURE_IXCONTENTSVC

rex_tcb_type        ix_contentsvc_tcb;
rex_stack_word_type ix_contentsvc_stack[IXCONTENTSVC_STACK_SIZ];

rex_tcb_type        ix_contentclnt_tcb;
rex_stack_word_type ix_contentclnt_stack[IXCONTENTCLNT_STACK_SIZ];

#endif /* FEATURE_IXCONTENTSVC */

/*---------------------------------------------------------------------------

                          IxSQLSvc TASK DECLARATIONS

---------------------------------------------------------------------------*/
#ifdef FEATURE_IXSQLSVC

rex_tcb_type        ix_sqlsvc_tcb;
rex_stack_word_type ix_sqlsvc_stack[IXSQLSVC_STACK_SIZ];

rex_tcb_type        ix_sqlclnt_tcb;
rex_stack_word_type ix_sqlclnt_stack[IXSQLCLNT_STACK_SIZ];

rex_tcb_type        ix_sqltest_tcb;
rex_stack_word_type ix_sqltest_stack[IXSQLTEST_STACK_SIZ];

rex_tcb_type        i_sqltest_tcb;
rex_stack_word_type i_sqltest_stack[ISQLTEST_STACK_SIZ];

#endif /* FEATURE_IXSQLSVC */
/*---------------------------------------------------------------------------

              Universal Broadcast Modem TASK DECLARATIONS

---------------------------------------------------------------------------*/
#ifdef FEATURE_UBM_L1

rex_tcb_type        ubm_l1_tcb;                     /* TCB for UBM L1 task   */
rex_stack_word_type ubm_l1_stack[UBM_L1_STACK_SIZ]; /* Stack for UBM L1 task */

#endif /* FEATURE_UBM_L1 */

#ifdef FEATURE_QDJTEST
/*---------------------------------------------------------------------------

                       QDJTEST TASK DECLARATIONS

---------------------------------------------------------------------------*/
rex_tcb_type        qdjtest_tcb;
rex_stack_word_type qdjtest_stack [QDJTEST_STACK_SIZ];
#endif  /* FEATURE_QDJTEST */

/*---------------------------------------------------------------------------

                          IxDescramblerSvc TASK DECLARATIONS

---------------------------------------------------------------------------*/
#ifdef FEATURE_IXDESCRAMBLERSVC

rex_stack_word_type ix_descramblersvc_stack[IXDESCRAMBLERSVC_STACK_SIZ];
rex_tcb_type        ix_descramblersvc_tcb;

#endif /* FEATURE_IXDESCRAMBLERSVC */

#ifdef FEATURE_QTV_STATIC_TASKS
/*---------------------------------------------------------------------------

        QTV_VIDEO_RENDERER, QTV_AUDIO and QTV_TASK10 TASK DECLARATIONS

---------------------------------------------------------------------------*/
rex_tcb_type        qtv_video_renderer_tcb;
rex_stack_word_type qtv_video_renderer_stack[ QTV_VIDEO_RENDERER_STACK_SIZ ];

rex_tcb_type        qtv_audio_tcb;
rex_stack_word_type qtv_audio_stack[ QTV_AUDIO_STACK_SIZ ];

rex_tcb_type        qtv_task10_tcb;
rex_stack_word_type qtv_task10_stack[ QTV_TASK10_STACK_SIZ ];

#endif  /* FEATURE_QTV_STATIC_TASKS */

#ifdef FEATURE_SENSORS
/*---------------------------------------------------------------------------

                         SENSORS TASK DECLARATIONS

---------------------------------------------------------------------------*/
rex_stack_word_type   sns_stack[ SNS_STACK_SIZ ];
rex_tcb_type          sns_tcb;
#endif /* FEATURE_SENSORS */

#ifdef FEATURE_QTV_STATIC_DLAGENT_TASK
/*---------------------------------------------------------------------------

                    QTV STATIC DLAGENT TASK DECLARATIONS

---------------------------------------------------------------------------*/
rex_tcb_type        qtv_dec_dlAgenttask_tcb;
rex_stack_word_type qtv_dec_dlAgenttask_stack[ QTV_DEC_DLAGENTTASK_STACK_SIZ ];
#endif

#ifdef FEATURE_QTV_STATIC_DLDSPSVC_TASK
/*---------------------------------------------------------------------------

                    QTV STATIC DL DSP SVC TASK DECLARATIONS

---------------------------------------------------------------------------*/
rex_tcb_type        qtv_dec_dlDspSvctask_tcb;
rex_stack_word_type qtv_dec_dlDspSvctask_stack[ QTV_DEC_DLDSPSVCTASK_STACK_SIZ ];
#endif

#ifdef FEATURE_XO
/*---------------------------------------------------------------------------

                         SENSORS TASK DECLARATIONS

---------------------------------------------------------------------------*/
rex_stack_word_type tcxomgr_stack[TCXOMGR_STACK_SIZ];
rex_tcb_type        tcxomgr_tcb;

#endif /* FEATURE_XO */

/*------------------------------------------------------------------------

                   CHG TASK DECLARATIONS

------------------------------------------------------------------------*/
#ifdef FEATURE_CHG_TASK
  rex_stack_word_type  chg_stack[ CHG_STACK_SIZ ]; /* TCB for chg task.   */
  rex_tcb_type         chg_tcb;                    /* Stack for chg Task. */
#endif /* FEATURE_CHG_TASK */


/*------------------------------------------------------------------------

                   VBATT TASK DECLARATIONS

------------------------------------------------------------------------*/
#ifdef FEATURE_VBATT_TASK
  rex_stack_word_type  vbatt_stack[ VBATT_STACK_SIZ ] ; /* TCB for vbatt task.   */
  rex_tcb_type         vbatt_tcb ;                      /* Stack for vbatt Task. */
#endif /* FEATURE_VBATT_TASK */

#ifdef FEATURE_PWRDB
#error code not present
#endif /* FEATURE_PWRDB */

/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
================================== FUNCTIONS ================================
=============================================================================
=============================================================================
===========================================================================*/

/*===========================================================================

FUNCTION task_start

DESCRIPTION
  This function is called by each task when it is created.  The function
  then performs the startup processing for the calling task.  The function
  first sets the acknowledge signal back to TMC/MC to acknowledge that it has
  been created.  It then waits for the start signal back to it.  While
  waiting for the start signal the watchdog timer is kicked everytime the
  interval timer expires.  When it gets the start signal it acknowledges
  it back to TMC/MC and it then returns to the caller.  The caller task is
  now formally started and it can start normal application processing.


DEPENDENCIES
  TMC or MC task has to be started.

RETURN VALUE
  void

SIDE EFFECTS
  The calling task stays here until it receives the START signal.

===========================================================================*/
void task_start(

        rex_sigs_type    rpt_sig,        /* report timer signal for task */
        int              dog_rpt,        /* Watchdog report for the task */
        rex_timer_type   *rpt_timer_ptr  /* pointer to report timer */
)    /*lint -esym(715, rpt_sig, dog_rpt, rpt_timer_ptr) */
{
  /* If multimode feature or GSM or WCDMA feature is defined,
  ** TMC does start procedure. Otherwise MC does start procedure.
  */
  #if defined(FEATURE_MULTIMODE_ARCH) || \
    defined(FEATURE_GSM) || \
    defined(FEATURE_WCDMA)
    tmc_task_start();
  #else /* !FEATURE_MULTIMODE_ARCH !FEATURE_GSM |FEATURE_WCDMA */
    mc_task_start(rpt_sig, (dog_report_type)dog_rpt, rpt_timer_ptr);
  #endif /* FEATURE_MULTIMODE_ARCH FEATURE_GSM FEATURE_WCDMA */

} /* END task_start */  /*lint +esym(715, rpt_sig, dog_rpt, rpt_timer_ptr) */

/* <EJECT> */
/*===========================================================================

FUNCTION task_ack

DESCRIPTION
  This function sends an acknowledge signal to either TMC or MC task.

DEPENDENCIES
  TMC or MC task is running.

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/
void task_ack(void)
{
  /* If multimode feature or GSM or WCDMA feature is defined,
  ** TMC does acknowledge procedure. Otherwise MC does acknowledge
  ** procedure.
  */
  #if defined(FEATURE_MULTIMODE_ARCH) || \
      defined(FEATURE_GSM) || \
      defined(FEATURE_WCDMA)
    tmc_task_ack();
  #else /* !FEATURE_MULTIMODE_ARCH !FEATURE_GSM |FEATURE_WCDMA */
    mc_task_ack();
  #endif /* FEATURE_MULTIMODE_ARCH FEATURE_GSM FEATURE_WCDMA */
} /* END task_ack */

/* <EJECT> */
/*===========================================================================

FUNCTION task_stop

DESCRIPTION
  This function is called by each task when it stops.  The function then
  performs the stop processing for the calling task.  The function
  sets the acknowledge signal back to either TMC or MC to acknowledge that
  it has been stopped.

DEPENDENCIES
  TMC or MC task is running.

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/
void task_stop(void)
{
  /* If multimode feature or GSM or WCDMA feature is defined,
  ** TMC does stop procedure. Otherwise MC does stop procedure.
  */
  #if defined(FEATURE_MULTIMODE_ARCH) || \
      defined(FEATURE_GSM) || \
      defined(FEATURE_WCDMA)
    tmc_task_stop();
  #else /* !FEATURE_MULTIMODE_ARCH !FEATURE_GSM |FEATURE_WCDMA */
    mc_task_stop();
  #endif /* FEATURE_MULTIMODE_ARCH FEATURE_GSM FEATURE_WCDMA */
} /* END task_stop */

/* <EJECT> */
/*===========================================================================

FUNCTION task_offline

DESCRIPTION
  This function is called by each task when it goes offline.  The function
  then performs the offline processing for the calling task.  The function
  sets the acknowledge signal back to either MC or TMC to acknowledge that
  it is offline.

DEPENDENCIES
  TMC task or MC task is running.

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/
void task_offline(void)
{
  /* If multimode feature or GSM or WCDMA feature is defined,
  ** TMC does offline procedure. Otherwise MC does offline procedure.
  */
  #if defined(FEATURE_MULTIMODE_ARCH) || \
      defined(FEATURE_GSM) || \
      defined(FEATURE_WCDMA)
    tmc_task_offline();
  #else /* !FEATURE_MULTIMODE_ARCH !FEATURE_GSM |FEATURE_WCDMA */
    mc_task_offline();
  #endif /* FEATURE_MULTIMODE_ARCH FEATURE_GSM FEATURE_WCDMA */
} /* END task_offline */


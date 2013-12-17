#ifndef TASK_H
#define TASK_H
/*===========================================================================

            T A S K    R E S O U R C E S    H E A D E R    F I L E

DESCRIPTION
  This file contains declarations defining global task resources.

Copyright (c) 1990-2005 by QUALCOMM Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/services/task/task.h#16 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/27/08   APU	   Added support for FEATURE_VBATT_TASK.	
07/02/08   hs      Added support for FEATURE_CHG_TASK.
02/08/08   rjm     FMRDS_PRI_ORDER now duplicated in main 1x enum.
11/30/07   rjm     FEATURE_FM switch now independent of ODM builds
06/18/07   va      Added TCXO Manager Task
06/05/07   jd      creation of new GPS_FS task parameters.
11/15/06   ry      Added support for FEATURE_SENSORS
11/13/06   pc      Moved DUMMY_PRI_ORDER to be below DOG_PRI_ORDER
11/01/06   pc      Added support for FEATURE_HS_USB
10/26/06   pc      Removed support for FEATURE_FS_TESTAPP
10/24/06   pc      Added support for FEATURE_DTV_DVBH_OSF
10/03/06   pc      Added support for QTV_TASK10
09/25/06   pc      Added support for FEATURE_DBGR
08/29/06   pc      Removed support for FEATURE_BT_HS, FEATURE_BT_TEST_RC_2_SIO,
                   FEATURE_BT_LPOS and FEATURE_BT_EXTPF_TEST.
08/29/06   pc      Added support for FEATURE_EXT_USB_P1
08/24/06   pc      Moved up the priority of the DM Task above the QTV tasks.
08/24/06   pc      Added priority order for QIPCALL task.
08/14/06   pc      Added feature FEATURE_QTV_STATIC_TASKS.
08/09/06   pc      Added support for QTV_VIDEO_RENDERER and QTV_AUDIO
08/01/06   pc      Added support for FEATURE_IXDESCRAMBLERSVC
07/25/06   pc      Added support for FEATURE_CPU_BASED_FLOW_CONTROL
07/17/06   pc      Added support for FEATURE_DMTASK.
06/20/06   pc      Removed VIDEO_ENCODER_SIG from global scope.  Moved it to
                   camcorder specific (non-global) scope.
06/19/06   pc      Added support for FEATURE_SEC_USFS, 
                   FEATURE_SEC_CONTENT_AGENT, FEATURE_IXSQLSVC and SECDiag.
05/05/06   ac      Added support for FEATURE_QDJTEST
05/03/06   bt      Added UBM L1 task.
05/02/06   pc      Moved WLAN_CP_PRI_ORDER above MMOC_PRI_ORDER.
04/18/06   pc      Doubled TIMER_STACK_SIZ
04/03/06   ptm     Added Restart thread priority and increased TMC stack size.
04/01/06   pc      Added support for FS_HOTPLUG
03/30/06   jn      Moved up CAMERA_DRV_TASK priority above MPEG4_PV_TASK1.
03/24/06   pc      Added support for SECRND task
03/23/06   cl      Added PDSM_COMM_TASK
03/13/06   pc      Increased REG_STACK_SIZ to 8192
03/03/06   fl      Move QVPPL task priority just above GRAPH for 1x target.
02/21/06   bfc     Changed the name of the new remote API Proxy task 
                   variable from *_MVS_* to *_AUDIO_*
01/25/06   ddh     Added DEM task info for the Modem.
01/23/06   rlp     Added HID Support.
01/23/06   nhk     Moved up the priority of QVPIO task.
01/02/06   dle     Added FEATURE_CAMERA_DRV_TASK.
11/10/05   pc      Added support for AVS_SBCENC_TASK
11/08/05   pc      Added support for GSM_PLT task
11/04/05   jz      Increased QVP MEPG4 Task Stack to 4096
11/02/05   pc      Increased stack size for DISP_TASK from 1024 to 4096
11/02/05   jhs     Changed TRAMP_SERVER_PRI for FIQ support.
10/31/05   adm     Added QDIAG priority order for Apps and changed the order
                   for Modem
10/17/05   jk      Added video async task priority for 7500 Apps proc.
10/12/05   ddh     Added DEM Task for Apps side only
09/12/05   pc      Added support for FEATURE_USB_HOST_ENHANCED.
09/05/05   jyw     Increased PDSM task stack size from 8k to 10k.
08/26/05   pc      Added support for DISP_TASK.
08/23/05   pc      Doubled the size of HDRMC stack to 4200.
08/05/05   jhs     Removed SMEM references.
08/03/05   pc      Doubled the stack size of BT External Profile Task.
07/12/05   jz      Featureized QVP APP Task for Codec Only
06/15/05   mpk     Increased L4 APC thread priority to just below IST.
06/14/05   pc      Increased WLAN_CP_STACK_SIZ and
                   WLAN_TRP_STACK_SIZ to 10K.
06/12/05   hba     Added suport for WLAN Sec task.
05/22/05   ali     Added VIDEO_ENCODER_SIG for QCamcorder Subsystem.
05/24/05   mpk     Changed SLEEP task priority under feature
                   FEATURE_L4 only.
04/27/05   jyw     Increased PDSM task stack size
04/20/05   pc      Added support for FEATURE_QIPCALL
04/09/05   pc      Added support for CNV task
04/11/05   jyw     Increased NF task stack size
03/29/05   lyr     Moved WLAN CP task priority above CM and MMoC tasks
03/16/05   jkl     Increased GRAPH stack.
03/15/05   wal     Added APS task for L4/Linux under FEATURE_APS
03/15/05   adg     Added QDIAG task for L4/Linux under FEATURE_IG_QDIAG
03/15/05   jqi     Added support for FS_FEATURE_RMTEFS_SERVER, FEATURE_L4
                   FEATURE_PM_SERVER,FEATURE_IG_UI_CLIENT_SELECT
03/09/05   srk     Changed sipt and rtp priorities below ps
                   Properly conditionalized sip and RTP tasks.
02/25/05   sfm     Doubled SRCH_STACK_SIZ
02/22/05   fl      Switched the task priority for UI and QVP_MPEG4 task.
02/14/05   jqi     Increased GRAPH_STACK_SIZE if FEATURE_SVG is defined.
02/05/05   aku     Added WLAN_CP and WLAN_TRP tasks.
02/01/05   dwp     Added QVP support for SIP.
02/03/05   epr     Addition of Sirius specific tasks using define SIRIUS_PLATFORM
01/21/05   ldaita  Added support MGPCC task (needed for GPS).
01/14/05   rp      Added background loading in Progressive boot task support
           jh      Added support for enabling profiling through diag interface.
01/06/05   aar     Added support for GPSSRCH demod task.
11/23/04   vj      Increased SM stack size.
           jt      Increased diag stack size.
11/19/04   jz      Added QVP VFE Task
11/18/04   pa      Added support for PBM task.
10/20/04   ajn     Added support for Timer task
10/15/04   rp      Added support for page and interrupt tasks.
10/13/04   ss      Increased the priority of DTV task to be above QTV tasks.
10/08/04   jqi     Added support for FEATURE_HSDPA
09/29/04   ss      Added support for DTV (Digital TV) feature.
07/06/04   hl      Added support for Joystick feature.
09/01/04   ss      Increase the stack size of MGPAI and MGPMC.
08/05/04   anandr  Added a new Priority for QTV subsystem to be used for rendering
                   video.
07/07/04   dd      Lowered GPSSRCH task pri. to minimize impact to 1x PC timeline.
06/15/04   ap      Added SIG for MDP.
06/04/04   gfr     Added support for the REX STAT task.
04/30/04    vr     Added BCDB and HDRBC tasks
04/06/04   ldaita  Pumped up the LSM task priority to close to AI2 task.
04/05/04   rashok  Increased QVPIO_STACK_SIZ to 4096.
04/01/04   sg      removed support for LP task as task no longer exists.
03/17/04   sr      Increased the size of DOG task.
02/23/04   balas   Added support for Bluetooth External Profiles feature support .
02/19/04   Jayanth Added support for FEATURE_SEC_DRM.
           yll     CR38530-Raised the AUTH priority to MC-2.
02/12/04   vdrapk  Added support for FEATURE_GSM_CB.
02/12/04    gr     Added priority definition for the video async task.
02/06/04   jayanth Adjusted the security task priorities.
           etallet Added support for DRM feature.
01/29/03   dpand   Moved diag task priority down below voc task priority.
01/19/04   jqi     Added Macros for MMC testing support.
01/13/04   jqi     Replaced T_MSM6200.
12/08/03   ss      Increased SECCRYPTARM's priority to be higher than MPEG4
11/19/03   waiw    Added support for FEATURE_GRAPHICS_OPENGLES_FIRMWARE.
11/14/03   dd      Added support for GPS Search Task.
11/13/03   dwp     Move QVP_MPEG task below UI in priority.
11/07/03   dle     Increase GRAPH task stack size.
10/30/03   jqi     Replaced T_RF_ZRF6200 with T_MSM6200.
10/23/03   sa      Increased VS task size.
10/20/03   dwp     Added initial QVPHONE support.
10/17/03   jaym    Fixed typo.
10/08/03   jqi     CR35649 - sleep stack overrun issue
                   Double the sleep task stack size.
08/14/03    sr     Added a dummy task entry below DOG task. Under err_fatal
                   condition, FS task is assigned a priority one below DOG.
                   Also added in support for MSM6250 by adding EFS2 related
                   task priorities and also MPEG 4.
07/23/03   tml     Increase GSTK stack size
07/11/03   jqi     Increase SM/WCDMA L1 stack size.
06/17/03   dle     Added CAMERA_SIG.
05/29/03   cr      Featured in EFS2 on-target testapp task priorities.
05/29/03   Om      Added support for ARM_CRYPTO and IPSEC tasks.
05/28/03   TMR/dgy Added changes for WCDMA single-mode build.
05/15/03    aw     Featurize graphic task stack size to 1280 when
                   ARM_3D_GRAPHICS_TASK_ONLY is defined.
05/15/03   npr     Swapped task priorities of GRLC_UL and GRLC_DL tasks.
05/15/03    aw     Increase graphic task stack size to 4096.
05/14/03    ld     Added defines for MGP related tasks (PRI_ORDER, PRI, STACK_SIZ, tcb, stack)
04/22/03    cr     Fix delayed trunc task pri name (backward compatible).
04/15/03    cr     Add EFS2 delayed truncation task priority order.
04/08/03   drl     Added GPRS related tasks for non ZRF6200 targets ... in this
                   case MSM6300
03/21/03   jqi     Increased stack size of MC task.
                   Changed stack size of MN_CNM task back.
03/20/03   jqi     Increased stack size of MN_CNM task.
03/18/03   jqi     Added GSTK_PRI_ORDER for targets other than MSM6200.
03/07/03   jqi     Removed MS_PRI_ORDER.
03/04/03    lz     Removed FEATURE_MOBILE_SUITE.
02/26/03   jqi     Added FTM task priority for cheetah target.
02/21/03   jay     Added SSL task
02/20/03   jqi     Fix a typo.
02/18/03  jqi/jar  Added Generic SIM Tookit Task (FEATURE_GSTK) Support and
                   Virtual Sim Application Toolkit (FEATURE_VIRTUAL_SAT) Task.
02/14/03   jqi     Added suport for FTM task.
02/19/03   cah     Increase SLEEP stack to 512.
01/15/03   enj     Added MPEG4 PacketVideo task priorities
12/31/02   jqi     Merged with msm6200 task.c#21
                   1.Added support for FEATURE_GSM_GPRS.
                   2.Increase stack size of the following tasks
                     SND, CM, MMOC, UI, DIAG and MN_CNM
12/10/02    lz     Added MS_PRI_ORDER and MS task declarations.
12/09/02    gr     Added support for the EFS Compatibility Task.
11/20/02   sun     Increased UI Stack size
11/13/02   jqi     Updated DS_STACK_SIZ on behalf of FEATURE_GSM or FEATURE_WCDMA.
10/17/02   dle     Added support for GRP.
09/17/02   jqi     Added SFAT_PRI_ORDER
                   Made the priority order of UIM task higher than that of NV task.
09/12/02   jqi     Merged COMMON with MSM6200 archive.

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "comdef.h"
#include "rex.h"
#include "customer.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------

                     COMMON SIGNALS DEFINITIONS

---------------------------------------------------------------------------*/

#define  TASK_START_SIG       0x8000
#define  TASK_STOP_SIG        0x4000
#define  TASK_OFFLINE_SIG     0x2000

#if defined FEATURE_EFS
#define  FS_OP_COMPLETE_SIG   0x40000000
#endif

#ifdef FEATURE_VS_TASK
#define VS_OP_COMPLETE_SIG    0x20000000
#endif /* FEATURE_VS_TASK */

#ifdef FEATURE_SEC
#define SEC_OP_COMPLETE_SIG   0x10000000
#endif /* FEATURE_SEC */

#ifdef FEATURE_GRP
#error code not present
#endif /* FEATURE_GRP */

#ifdef FEATURE_CAMERA
#define CAMERA_SIG            0x04000000
#endif /* FEATURE_CAMERA */

#if (defined FEATURE_MDP) || (defined FEATURE_MDP2)
#define MDP_COMPLETE_SIG      0x02000000
#endif /* FEATURE_MDP or FEATURE_MDP2 */

#ifdef FEATURE_ONCRPC
#error code not present
#endif /* FEATURE_ONCRPC */

#define  REX_SLEEP_SIG   0x80000000

/*---------------------------------------------------------------------------

                    INTERRUPT STACK DECLARATIONS

---------------------------------------------------------------------------*/

extern byte  irq_stack[ ];

/*---------------------------------------------------------------------------

                     TASK PRIORITY DEFINITIONS

---------------------------------------------------------------------------*/
#ifdef FEATURE_L4
#error code not present
#endif

/* -----------------------------------------------------------------------
** This enumerator defines the order of task priorities starting from the
** highest priority and going to lower priorities down the list.
** The task on the top of this order list has the highest priority.
** NOTE: THE POSITION OF ANY NEW TASK IN THIS LIST HAS TO BE BASED ON THE
** NEW TASK'S PRIORITY RELATIVE TO THE OTHER TASKS IN THIS LIST.
** ----------------------------------------------------------------------- */
#if !(defined(T_MSM7500) && defined(IMAGE_APPS_PROC))

typedef enum
{
  /* for GSM/WCDMA only target */
  #if( !defined( FEATURE_CDMA_800 ) && \
       !defined( FEATURE_CDMA_1900 ) && \
       !defined( FEATURE_ACP ) && \
       !defined( FEATURE_HDR ) )
  #ifdef FEATURE_ATS
    ATS_PRI_ORDER,    /* Order of REX priority for the ATS Task */
  #endif
    DOG_PRI_ORDER,    /* Order of REX priority for the Watchdog Task */
    DUMMY_PRI_ORDER,  /* Dummy priority. Err_fatal condition - FS priority */
    TIMER_PRI_ORDER,  /* Order of REX priority for the Timer Task */
  #ifdef FEATURE_PROGRESSIVE_BOOT
    INTERRUPT_PRI_ORDER,    /* Order of REX priority for the interrupt Task */
    PAGER_PRI_ORDER,    /* Order of REX priority for the pager Task*/
  #endif
    GPLT_PRI_ORDER,    /* Order of REX priority for the GSM PLT Task */
    DISP_PRI_ORDER,   /* REX priority for the Display Task */        
    SND_PRI_ORDER,    /* Order of REX priority for the Sound Task */
    MDSP_PRI_ORDER,   /* Order of REX priority for the MDSP Task */
#ifdef FEATURE_CPU_BASED_FLOW_CONTROL
    FC_PRI_ORDER,
#endif
    WCDMA_L1_PRI_ORDER,  /* Order of REX priority for the WCDMA L1 Task */
    GSM_L1_PRI_ORDER,    /* Order of REX priority for the GSM L1 Task */
    #ifdef FEATURE_MGPCC_TASK
    MGPCC_PRI_ORDER,  /* Order of REX priority for the Channel controller task */
    #endif
    RPC_PRI_ORDER,    /* Order of REX priority for the RPC Task */
    WCDMA_L2_UL_PRI_ORDER,  /* Order of REX priority for the L2 Uplink Task */
#ifdef FEATURE_HSDPA
#error code not present
#endif
    WCDMA_L2_DL_PRI_ORDER,  /* Order of REX priority for the L2 Downlink Task */
    QVPIO_PRI_ORDER,  /* Order of REX priority for the QVPIO Task  */ //hkn
    QVP_VFE_PRI_ORDER,/* Order of REX priority for the QVP VFE Task */
    QVP_AE_PRI_ORDER,   /* REXpriority for QVP AE task */
    QVP_H223_DEMUX_PRI_ORDER, /* Order of REX priority for the QVP H223 Demux Read*/
    QVP_H324_SM_PRI_ORDER, /* Order of REX priority for the QVP H324 State Machine Thread*/
    GSM_L2_PRI_ORDER,     /* Order of REX priority for the GSM L2 Task */
    GMAC_PRI_ORDER,            /* Order of REX priority for the GPRS MAC Task */
#ifdef FEATURE_UBM_L1
    UBM_L1_PRI_ORDER,          /* Order of UBM Layer 1 task */
#endif /* FEATURE_UBM_L1 */
    GRLC_DL_PRI_ORDER,         /* Order of REX priority for the GPRS RLC DL Task */
    GRLC_UL_PRI_ORDER,         /* Order of REX priority for the GPRS RLC UL Task */
#ifdef FEATURE_AVS_SBCENC_TASK 
    SBCENC_PRI_ORDER,      /* Order of REX priority for the SBC encoder task */
#endif
#ifdef FEATURE_MFLO
#error code not present
#endif
    BT_PRI_ORDER,     /* Bluetooth (BT) Task */
    BT_PF_PRI_ORDER,  /* Bluetooth (BT) External Profile Task */
    RRC_PRI_ORDER,    /* Order of REX priority for the WCDMA RRC Task */
    RR_PRI_ORDER,     /* Order of REX priority for the GSM RR Task */
    DSWCSD_UL_PRI_ORDER, /* Order of REX priority for the WCDMA CS Data UL Task */
    DSWCSD_DL_PRI_ORDER, /* Order of REX priority for the WCDMA CS Data DL Task */
    DS_GCSD_PRI_ORDER,/* Order of REX priority for the GSM CSData Task */
    GLLC_PRI_ORDER,            /* Order of REX priority for the GPRS LLC Task */
#ifdef FEATURE_COMP_TASK
    COMP_PRI_ORDER,   /* Order of REX priority for the COMP Task */
#endif
    MM_PRI_ORDER,     /* Order of REX priority for the MM Task */
    REG_PRI_ORDER,    /* Order of REX priority for the REG Task */
    TC_PRI_ORDER,     /* Order of REX priority for the WCDMA TC Task */
    GSM_TEST_PRI_ORDER, /* Order of REX priority for the GSM TEST Task */
    SM_PRI_ORDER,     /* Order of REX priority for the SM Task */
    MN_CNM_PRI_ORDER, /* Order of REX priority for the CNM Task */

#ifdef SIRIUS_PLATFORM
#error code not present
#endif /* SIRIUS_PLATFORM */

#ifdef FEATURE_FM
    FMRDS_PRI_ORDER,  /* FM-RDS Radio */
#endif

    HS_PRI_ORDER,     /* Order of REX priority for the Handset Task */
#ifdef FEATURE_MFLO
#error code not present
#endif
    VS_PRI_HI_ORDER,  /* Order of REX priority for the VS Hi Prio Task */
    MGPMC_PRI_ORDER,  /* Order of REX priority for the Measurement controller task */
    MGPAI_PRI_ORDER,  /* Order of REX priority for the AI2 task */
    SNS_PRI_ORDER,    /* Order of REX priority for the Sensors Task */
    LSM_PRI_ORDER,    /* Order of REX priority for the LCS Manager Task */
    MGPNF_PRI_ORDER,  /* Order of REX priority for the NF task */
    MGPCD_PRI_ORDER,  /* Order of REX priority for the CD task */
    TMC_PRI_ORDER,    /* Order of REX priority for the TMC Task -- If needed, move this above Handset Task*/
    QDSP_PRI_ORDER,   /* Order of REX priority for the QDSP Task */
    VOC_PRI_ORDER,    /* Order of REX priority for the Vocoder Task */
// L4 pending: need provide correct priority order for PM server.
//#ifdef FEATURE_PM_SERVER
//    PM_SERVER_PRI_ORDER, /* Order of REX priority for the PM SERVER Task */
//#endif
    SMD_PRI_ORDER,    /* Order of REX priority for the Shared Memory Driver Task */
    ONCRPC_REPLY_PRI_ORDER,     /* REX priority for ONCRPC REPLY task */
    ONCRPC_PRI_ORDER,                        /* REX priority for ONCRPC task.*/
#ifdef FEATURE_APPS_TCXO_SLEEP
    DEM_PRI_ORDER,                          /* REX priority for the DEM task */
#endif /* FEATURE_APPS_TCXO_SLEEP */
#ifdef FEATURE_HS_USB
    /* High-Speed USB stack thread priorities. */
    HSU_CONTROLLER_PRI_ORDER,
    HSU_CORE_PRI_ORDER,
    HSU_DRIVER_PRI_ORDER,
    HSU_OTHER_PRI_ORDER,
    /* High-Speed USB adaptation layer task priorities. */
    HSU_AL_TASK_PRI_ORDER,
    HSU_AL_MS_PRI_ORDER,     /* REX priority for HSU_AL_MS task.*/
#endif /* FEATURE_HS_USB */
#ifdef FEATURE_PWRDB
#error code not present
#endif /* FEATURE_PWRDB */
    DIAG_PRI_ORDER,   /* Order of REX priority for the Diag Task */
#ifdef FEATURE_IG_QDIAG
    QDIAG_PRI_ORDER,    /* Order of REX priority for the QDIAG Task */
#endif
#ifdef FEATURE_QDJTEST
    QDJTEST_PRI_ORDER,
#endif /* FEATURE_QDJTEST*/
#ifdef FEATURE_WLAN
#error code not present
#endif /*FEATURE_WLAN */
    CM_PRI_ORDER,     /* Order of REX priority for the CM Task */
    MMOC_PRI_ORDER,   /* Order of REX priority for the MMOC Task */
    GRAPHICS_PRI_ORDER,  /* Order of REX priority for the GRAPHICS Task */
    FTM_PRI_ORDER,    /* REX priority for FTM task.*/
    CHG_PRI_ORDER,    /* REX priority for CHG task */
#ifdef FEATURE_VBATT_TASK
    VBATT_PRI_ORDER,  /* Order of VBATT task */
#endif
    WMS_PRI_ORDER,    /* Order of REX priority */
    RPC_EX_PRI_ORDER, /* Order of RPC Exec task */
    RPC_TX_PRI_ORDER, /* Order of RPC Tx task */
   /* Test Apps should be higher priority than the services they are testing.*/
#ifdef FEATURE_ONCRPC_PROXY
  #ifdef FEATURE_EXPORT_ONCRPCTEST
    #if defined(FEATURE_ONCRPCTEST_MODEM_IS_SERVER)
    ONCRPC_ONCRPCTEST_PROXY_PRI_ORDER,     /* REX priority for the oncrpctest callbacks proxy task */
    #endif
  #endif /* FEATURE_EXPORT_ONCRPCTEST */
    ONCRPC_M_GENERAL_PROXY_PRI_ORDER,              /* REX priority for the general purpose modem proxy */
  #ifdef FEATURE_EXPORT_NV
    ONCRPC_NV_PROXY_PRI_ORDER,               /* REX priority for the nv proxy task */
  #endif
  #ifdef FEATURE_ONCRPC_SM
    ONCRPC_FORWARD_PROXY_PRI_ORDER,
  #endif /* FEATURE_ONCRPC_SM */
#endif /* FEATURE_ONCRPC_PROXY */
    STA_PRI_ORDER,                           /* REX priority for the STA task.*/
#if ( defined FEATURE_ONCRPC_PROXY && defined FEATURE_STA_ONCRPCTEST )
    ONCRPC_STA_ONCRPCTEST_PROXY_PRI_ORDER,   /* REX priority for the sta oncrpctest proxy task */
#endif    
    QVP_MPEG4_PRI_ORDER,         /* REX priority for QVP MPEG task.*/
#ifdef FEATURE_DBGR
    DBGR_PRI_ORDER,
#endif
    UI_PRI_ORDER,     /* Order of REX priority for the UI Task */
    WPLT_PRI_ORDER,   /* Order of REX priority for the PLT Task */
    VS_PRI_ORDER,     /* Order of REX priority for the VS Task */
    UIM_PRI_ORDER,    /* Order of REX priority for the UIM Task */
    NV_PRI_ORDER,     /* Order of REX priority for the NV Task */
#if defined(FS_FEATURE_RMTEFS_SERVER) && !defined(FEATURE_IG_EFS_EXT_SERVER)
    EFS_SRVR_PRI_ORDER,   /* Order of REX priority for the EFS server task */
#endif
#ifdef FEATURE_IXFILESVC
    IXFILESVC_PRI_ORDER, /* Order of REX Priority for IxFile Svc task */
#endif
    FS_PRI_ORDER,     /* Order of REX priority for the FS Task */
#ifdef FEATURE_QVP_APP_INCLUDE
#error code not present
#endif /* FEATURE_QVP_APP_INCLUDE */
#ifdef FEATURE_QIPCALL
    QIPCALL_PRI_ORDER,
#endif /* FEATURE_QIPCALL*/
    SFAT_PRI_ORDER,   /* Order of REX priority for the SFAT Task */
#ifdef FEATURE_JOYST_TASK
    JOYST_PRI_ORDER,                   /* REX priority for the joystick task */
#endif
#ifdef FEATURE_FS_TESTAPP
#error code not present
#endif
#ifdef FEATURE_FS_BENCHMARK
    FS_CPU_HOG_PRI_ORDER,       /* Must be higher priority than benchmark task */
    FS_BENCHMARK_PRI_ORDER,     /* Lower priority than FS tasks */
#endif
    
    MMC_TEST_PRI_ORDER,   /* REX priority for the MMC TEST.*/
    FS_COMPAT_PRI_ORDER,  /* REX priority for the EFS Compatibility task.*/
    PBM_PRI_ORDER,    /* Order of REX priority for the PBM Task */
    GSDI_PRI_ORDER,   /* Order of REX priority for the GSDI (SIM) Task */
    GSTK_PRI_ORDER,   /* Order of REX priority for the Generic SIM Toolkit Task (GSTK) */
    GSDI_UIM_SERVER_PRI_ORDER,  /* Order of REX priority for the UIM Server Task (in place of real UIM) */
    VIRTSAT_PRI_ORDER, /* Order of REX Priority for the Virtual SAT Application */
    DS_PRI_ORDER,     /* Order of REX priority for the Data Services (DS) Task */

#ifdef FEATURE_NV_CNV
    CNV_PRI_ORDER,                          /* REX priority for the CNV task */
#endif

    USBDC_PRI_ORDER,  /* Order of REX priority for the USBDC Task */

#if defined (FEATURE_USB) && defined (FEATURE_USB_HOST_ENHANCED)
    USBHC_URB_PRI_ORDER,  /* Order of REX priority for the USBHC_URB Task */
    USBHC_DEV_PRI_ORDER,  /* Order of REX priority for the USBHC_DEV Task */

#if defined(FEATURE_USB_HOST_TEST_ENHANCED) && \
    (defined(FEATURE_USB_HOST_MASS_STORAGE_TEST_ENHANCED) || \
     defined(FEATURE_USB_HOST_HID_TEST_ENHANCED))
    USBHC_TEST_PRI_ORDER,  /* Order of REX priority for the USBHC_TEST Task */
#endif
#endif
#ifdef FEATURE_WLAN
#error code not present
#endif /* FEATURE_WLAN */
#ifdef FEATURE_IXTIMESVC
   IXTIMESVC_PRI_ORDER,
#endif
#ifdef FEATURE_IXHTTPSVC
    IXHTTPSVC_PRI_ORDER,
#endif
    PS_PRI_ORDER,     /* Order of REX priority for the Protocol Services (PS) Task */
#ifdef FEATURE_WLAN
#error code not present
#endif
#ifdef FEATURE_QVPHONE_SIP
#error code not present
#endif
#ifdef FEATURE_IXSQLSVC
    IXSQLSVC_PRI_ORDER,   /* Order of REX Priority for IxSQL Svc task */
    IXSQLCLNT_PRI_ORDER,  /* Order of REX Priority for IxSQL Clnt task */
    IXSQLTEST_PRI_ORDER,  /* Order of REX Priority for IxSQL Clnt task */
    ISQLTEST_PRI_ORDER,  /* Order of REX Priority for IxSQL Clnt task */
#endif
#ifdef FEATURE_IXCONTENTSVC
    IXCONTENTSVC_PRI_ORDER,
    IXCONTENTCLNT_PRI_ORDER,
#endif
    SECIPS_PRI_ORDER,            /* Order of REX priority for the IPSec task */
#ifdef FEATURE_IXDESCRAMBLERSVC
    IXDESCRAMBLERSVC_PRI_ORDER,
#endif

#ifdef FEATURE_IPC_TEST
#error code not present
#endif
    AL1_PRI_ORDER,    /* Order of REX priority for the AL1 Task */
    RRCUT_PRI_ORDER,  /* Order of REX priority for the RRCUT Task */
    SECTESTAPP_PRI_ORDER,                /* REX priority for SECTESTAPP task.*/

#ifdef FEATURE_SECRND
    /* security services rnd task */
    SECRND_PRI_ORDER,
#endif

#ifdef FEATURE_SEC_USFS
    /* security services sfs task */
    SFS_PRI_ORDER,
#endif
    /* DRM priority should be higher than SSL and Crypto */
    SECDRM_PRI_ORDER,

      /* SSL should always be higher priority to the crypto task since it is
      ** using crypto services, and in general any task using crypto services
      ** should be higher priority compared to crypto
      */
    SECSSL_PRI_ORDER,                           /* REX priority for SSL task.*/
    SEC_PRI_ORDER,                              /* REX priority for SEC task.*/
#ifdef FEATURE_SEC_CONTENT_AGENT
    CONTENTAGENTDIAG_PRI_ORDER,                      /* REX priority for ContentAgentDiag task.*/
#endif
    MENU_PRI_ORDER,   /* Order of REX priority for the Menu Task */
    QVPPL_PRI_ORDER,  /* Order of REX priority for the QVPPL Task */
    GRAPH_PRI_ORDER,  /* Order of REX priority for the GRAPH Task */
    VIDEO_ASYNC_PRI_ORDER, /* Asynchronous video I/O task */
#ifdef FEATURE_GSM_CB
#error code not present
#endif /* #ifdef FEATURE_GSM_CB */

    DTV_PRI_ORDER,                       /* REX priority for DTV task */

    /* SECCRYPTARM, MPEG4 task should be kept as low as possible, since they
    ** take up all the available CPU.
    */
    SECCRYPTARM_PRI_ORDER,                /* SEC services ARM crunching task */

  #ifdef FEATURE_CAMERA_DRV_TASK
    CAMERA_DRV_PRI_ORDER, /* Order of REX priority for CAMERA_DRV Task */
  #endif /* FEATURE_CAMERA_DRV_TASK */ 
  #ifdef FEATURE_DMTASK
    DMTASK_PRI_ORDER,                    /* REX priority for the DM Task */
  #endif 
    MPEG4_PV_TASK1_PRI_ORDER,            /* REX priority for MPEG PV task #1.*/
    MPEG4_PV_TASK2_PRI_ORDER,            /* REX priority for MPEG PV task #2.*/
    MPEG4_PV_TASK3_PRI_ORDER,            /* REX priority for MPEG PV task #3.*/
    MPEG4_PV_TASK4_PRI_ORDER,            /* REX priority for MPEG PV task #4.*/
    MPEG4_PV_TASK5_PRI_ORDER,            /* REX priority for MPEG PV task #5.*/
    MPEG4_PV_TASK6_PRI_ORDER,            /* REX priority for MPEG PV task #6.*/
    MPEG4_PV_TASK7_PRI_ORDER,            /* REX priority for MPEG PV task #7.*/
    MPEG4_PV_TASK8_PRI_ORDER,            /* REX priority for MPEG PV task #8.*/
    MPEG4_PV_TASK9_PRI_ORDER,            /* REX priority for MPEG PV task #9.*/
    MPEG4_PV_TASK10_PRI_ORDER,           /* REX priority for MPEG PV task #10.*/
    EFS_TRUNC_PRI_ORDER,          /* REX priority of truncation task */
    NSRW_PRI_ORDER,      /* REX priority for NON SRW task */
    REX_STAT_PRI_ORDER,  /* Order of REX priority for the Stat Task */
  #ifdef FEATURE_PROGRESSIVE_BOOT
    BACKGROUND_LOADER_PRI_ORDER, /* Order of REX priority for the loader Task*/
  #endif
  #ifdef FEATURE_APS
    APS_PRI_ORDER,                    /* Order of REX priority for the APS Task */
  #endif

  #ifdef FEATURE_EFS_HOTPLUG
    FS_HOTPLUG_PRI_ORDER,
  #endif

  #ifdef FEATURE_EXT_USB_P1
    USBEBI2_PRI_ORDER,     /* Order of REX priority for the USBD Task */
    USBAPI_PRI_ORDER,   /* Order of REX priority for the USBAPI Task */
    USBD_PRI_ORDER,  /* Order of REX priority for the USBEBI2 Task(very weak) */
  #endif

  #ifdef FEATURE_DTV_DVBH_OSF_DAL
    DTV_OSFDAL_VERY_HIGH_PRI_ORDER,
    DTV_OSFDAL_HIGH_PRI_ORDER,
    DTV_OSFDAL_NORMAL_PRI_ORDER,
    DTV_OSFDAL_LOW_PRI_ORDER,
    DTV_OSFDAL_VERY_LOW_PRI_ORDER,
  #endif

  #ifdef FEATURE_EFS_FDI
    /* FDI background task priority MUST be lower than any callers to the Filesystem */
    FDI_BG_TASK_PRI_ORDER, 
  #endif

  #ifdef FEATURE_L4
#error code not present
  #else
    SLEEP_PRI_ORDER,  /* Order of REX priority for the Sleep Task */
  #endif
    MAX_PRI_ORDER_NUM     /* Maximum number of Priority ordering */

  #else

// not GSM/WCDMA target
  #ifdef FEATURE_ATS
    ATS_PRI_ORDER,    /* Order of REX priority for the ATS Task */
  #endif
    DOG_PRI_ORDER,                     /* REX priority for the Watchdog task.*/
    DUMMY_PRI_ORDER,  /* Dummy priority. Err_fatal condition - FS priority */
    TIMER_PRI_ORDER,             /* Order of REX priority for the Timer Task */
  #ifdef FEATURE_PROGRESSIVE_BOOT
    INTERRUPT_PRI_ORDER,    /* Order of REX priority for the interrupt Task */
    PAGER_PRI_ORDER,    /* Order of REX priority for the pager Task*/
  #endif
    SRCH_MAX_PRI_ORDER,/* Max REX priority for srch task after watchdog task.*/
    GPLT_PRI_ORDER,                     /* REX priority for the GSM PLT task.*/
    DISP_PRI_ORDER,                     /* REX priority for the Display Task */        
    SND_PRI_ORDER,                        /* REX priority for the Sound task.*/
    ADSP_RTOS_PRI_ORDER,     /* REX priority for the ADSP RTOS Services task.*/
    QTV_VDL_DSP_SVC_TASK_PRI_ORDER, /* priority for QTV static VDL DSP Services Task.*/
    MDSP_PRI_ORDER,                        /* REX priority for the MDSP task.*/
    TX_PRI_ORDER,                                /* REX priority for TX task.*/
#ifdef FEATURE_CPU_BASED_FLOW_CONTROL
    FC_PRI_ORDER,
#endif
    WCDMA_L1_PRI_ORDER,                /* REX priority for the WCDMA L1 task.*/
    GSM_L1_PRI_ORDER,                    /* REX priority for the GSM L1 task.*/
    HDRTX_PRI_ORDER,                          /* REX priority for HDRTX task.*/
    QDSP_PRI_ORDER,                        /* REX priority for the QDSP Task */
    MC_PRI_HI_ORDER,         /* High REX priority for MC task before TX task.*/
    SRCH_HI_PRI_ORDER,     /* High REX priority for srch task before TX task.*/
    RX_PRI_ORDER,                                /* REX priority for RX task.*/
    HDRRX_PRI_ORDER,                          /* REX priority for HDRRX task.*/

#ifdef FEATURE_CGPS
#error code not present
#endif /* FEATURE_CGPS */

    NV_HI_PRI_ORDER,       /* High REX priority for NV task before srch task.*/
    MC_PRI_TC_ORDER,/* REX priority for MC task while on TC before srch task.*/
    SRCH_PRI_ORDER,                            /* REX priority for srch task.*/
    HDRSRCH_PRI_ORDER,                      /* REX priority for HDRSRCH task.*/
    HDRBC_PRI_ORDER,              /* REX priority for HDRBCAST PROTOCOL task.*/
    RPC_PRI_ORDER,                          /* REX priority for the RPC task.*/
    RXTX_PRI_ORDER,                            /* REX priority for RXTX task.*/
    QVPIO_PRI_ORDER,                     /* REX priority for the QVPIO task. */    
    QVP_VFE_PRI_ORDER,                  /* REX priority for the QVP VFE task */
    QVP_AE_PRI_ORDER,                        /* REX priority for QVP AE task */
    TMC_PRI_ORDER,                          /* REX priority for the TMC task.*/

#ifdef FEATURE_CGPS
#error code not present
#endif /* FEATURE_CGPS */

    WCDMA_L2_UL_PRI_ORDER,            /* REX priority for the L2 Uplink task.*/
#ifdef FEATURE_HSDPA
#error code not present
#endif
    WCDMA_L2_DL_PRI_ORDER,          /* REX priority for the L2 Downlink task.*/
    GSM_L2_PRI_ORDER,                    /* REX priority for the GSM L2 task.*/
    GMAC_PRI_ORDER,           /* Order of REX priority for the GPRS MAC Task */
#ifdef FEATURE_UBM_L1
    UBM_L1_PRI_ORDER,          /* Order of UBM Layer 1 task */
#endif /* FEATURE_UBM_L1 */
    GRLC_DL_PRI_ORDER,     /* Order of REX priority for the GPRS RLC DL Task */
    GRLC_UL_PRI_ORDER,     /* Order of REX priority for the GPRS RLC UL Task */
#ifdef FEATURE_AVS_SBCENC_TASK 
    SBCENC_PRI_ORDER,      /* Order of REX priority for the SBC encoder task */
#endif
    BT_PRI_ORDER,                                /* REX priority for BT task.*/
    BT_PF_PRI_ORDER,  /* Bluetooth (BT) External Profile Task */
    RRC_PRI_ORDER,                    /* REX priority for the WCDMA RRC task.*/
    RR_PRI_ORDER,                        /* REX priority for the GSM RR task.*/
    DSWCSD_UL_PRI_ORDER,       /* REX priority for the WCDMA CS Data UL task.*/
    DSWCSD_DL_PRI_ORDER,       /* REX priority for the WCDMA CS Data DL Task.*/
    DS_GCSD_PRI_ORDER,               /* REX priority for the GSM CSData task.*/
    GLLC_PRI_ORDER,           /* Order of REX priority for the GPRS LLC Task */
#ifdef FEATURE_COMP_TASK
    COMP_PRI_ORDER,   /* Order of REX priority for the COMP Task */
#endif
    MM_PRI_ORDER,                            /* REX priority for the MM task.*/
    REG_PRI_ORDER,                          /* REX priority for the REG task.*/
    TC_PRI_ORDER,                      /* REX priority for the WCDMA TC task */
    GSM_TEST_PRI_ORDER,                /* REX priority for the GSM TEST task.*/
    SM_PRI_ORDER,                            /* REX priority for the SM task.*/
    MN_CNM_PRI_ORDER,                       /* REX priority for the CNM task.*/
    MC_PRI_ORDER,                                /* REX priority for MC task.*/
    SRCH_MD_PRI_ORDER, /* Med REX priority for srch task right after MC task.*/
    HDRMC_PRI_ORDER,                          /* REX priority for HDRMC task.*/
    AUTH_PRI_ORDER,                            /* REX priority for AUTH task.*/
#ifdef FEATURE_WLAN
#error code not present
#endif    
    MMOC_PRI_ORDER,                            /* REX priority for MMOC task.*/

#ifdef FEATURE_FM
    FMRDS_PRI_ORDER,                    /* FM-RDS Radio */
#endif

    HS_PRI_ORDER,                       /* REX priority for the Handset task.*/
#ifdef FEATURE_MFLO
#error code not present
#endif
    TDSO_PRI_ORDER,                            /* REX priority for TDSO task.*/
    QMIP_PRI_ORDER,                            /* REX priority for QMIP task.*/
    USBDC_PRI_ORDER,                      /* REX priority for the USBDC task.*/
#if defined (FEATURE_USB) && defined (FEATURE_USB_HOST_ENHANCED)
    USBHC_URB_PRI_ORDER,                 /* REX priority for USBHC_URB task. */
    USBHC_DEV_PRI_ORDER,                 /* REX priority for USBHC_DEV task. */
#if defined(FEATURE_USB_HOST_TEST_ENHANCED)
    USBHC_TEST_PRI_ORDER,                 /* REX priority for USBHC_TEST task. */
#endif
#endif
    VS_PRI_HI_ORDER,/* REX priority for the VS Hi Prio Task before diag task.*/
    SNS_PRI_ORDER,                       /* REX priority of the Sensors task.*/
#ifdef FEATURE_CGPS
#error code not present
#endif /* FEATURE_CGPS */
    LSM_PRI_ORDER,                  /* REX priority for the LCS Manager task.*/
    VOC_PRI_ORDER,                      /* REX priority for the Vocoder task.*/
    GPSSRCH_PRI_ORDER,          /* REX priority of GPS search scheduler task.*/
    GPSSRCH_DEMOD_PRI_ORDER,       /* REX priority for GPS demodulation task.*/
// L4 pending: need find a correct PM server priority
//#ifdef FEATURE_PM_SERVER
//    PM_SERVER_PRI_ORDER, /* Order of REX priority for the PM SERVER Task */
//#endif
  #ifdef FEATURE_APPS_TCXO_SLEEP
    DEM_PRI_ORDER,                          /* REX priority for the DEM task */
  #endif /* FEATURE_APPS_TCXO_SLEEP */
    SMD_PRI_ORDER,              /* REX priority of Shared Memory Driver task.*/
    ONCRPC_REPLY_PRI_ORDER,     /* REX priority for ONCRPC REPLY task */
    ONCRPC_PRI_ORDER,                        /* REX priority for ONCRPC task.*/
#ifdef FEATURE_HS_USB
    /* High-Speed USB stack thread priorities. */
    HSU_CONTROLLER_PRI_ORDER,
    HSU_CORE_PRI_ORDER,
    HSU_DRIVER_PRI_ORDER,
    HSU_OTHER_PRI_ORDER,
    /* High-Speed USB adaptation layer task priorities. */
    HSU_AL_TASK_PRI_ORDER,
    HSU_AL_MS_PRI_ORDER,     /* REX priority for HSU_AL_MS task.*/
#endif /* FEATURE_HS_USB */
#ifdef FEATURE_PWRDB
#error code not present
#endif /* FEATURE_PWRDB */
    DIAG_PRI_ORDER,                        /* REX priority for the Diag task.*/
#ifdef FEATURE_IG_QDIAG
    QDIAG_PRI_ORDER,    /* Order of REX priority for the QDIAG Task */
#endif                       /* REX priority for the Diag task.*/
    CM_PRI_ORDER,                            /* REX priority for the CM task.*/
    FTM_PRI_ORDER,                              /* REX priority for FTM task.*/
    CHG_PRI_ORDER,                              /* REX priority for CHG task.*/
#ifdef FEATURE_VBATT_TASK
    VBATT_PRI_ORDER,  							/* Order of VBATT task */
#endif    
    TEST_PRI_ORDER,                            /* REX priority for TEST task.*/
    WMS_PRI_ORDER,                              /* REX priority for WMS task.*/
    RPC_EX_PRI_ORDER,                      /* REX priority for RPC Exec task.*/
    RPC_TX_PRI_ORDER,                        /* REX priority for RPC Tx task.*/
    PDSM_PRI_ORDER,                              /* REX priority for NF task.*/
#if defined(FEATURE_MODEM_PDCOMM)
    PDSM_COMM_PRI_ORDER,
#endif
    NF_PRI_ORDER,                                /* REX priority for NF task.*/
#ifndef FEATURE_CGPS
    CD_PRI_ORDER,                                /* REX priority for CD task.*/
#endif /* ! FEATURE_CGPS */

#ifdef FEATURE_XO
    TCXOMGR_PRI_ORDER,                /* TCXOMGR Task Priority */
#endif /* FEATURE_XO */


   /* Test Apps should be higher priority than the services they are testing.*/
#ifdef FEATURE_ONCRPC_PROXY
  #ifdef FEATURE_EXPORT_ONCRPCTEST
    #if defined(FEATURE_ONCRPCTEST_MODEM_IS_SERVER)
    ONCRPC_ONCRPCTEST_PROXY_PRI_ORDER,     /* REX priority for the oncrpctest callbacks proxy task */
    #endif
  #endif /* FEATURE_EXPORT_ONCRPCTEST */
    ONCRPC_M_GENERAL_PROXY_PRI_ORDER,              /* REX priority for the general purpose modem proxy */
  #ifdef FEATURE_EXPORT_NV
    ONCRPC_NV_PROXY_PRI_ORDER,               /* REX priority for the nv proxy task */
  #endif
  #ifdef FEATURE_ONCRPC_SM
    ONCRPC_FORWARD_PROXY_PRI_ORDER,
  #endif /* FEATURE_ONCRPC_SM */
#endif /* FEATURE_ONCRPC_PROXY */
    STA_PRI_ORDER,                           /* REX priority for the STA task.*/
#if ( defined FEATURE_ONCRPC_PROXY && defined FEATURE_STA_ONCRPCTEST )
    ONCRPC_STA_ONCRPCTEST_PROXY_PRI_ORDER,   /* REX priority for the sta oncrpctest proxy task */
#endif    
    QVP_MPEG4_PRI_ORDER,                   /* REX priority for QVP MPEG task.*/
#ifdef FEATURE_DBGR
    DBGR_PRI_ORDER,
#endif
    UI_PRI_ORDER,                            /* REX priority for the UI task.*/

#ifdef FEATURE_CGPS
#error code not present
#endif /* FEATURE_CGPS */

    SRCH_LO_PRI_ORDER,       /* low REX priority for SRCH task after ui task.*/
#ifdef FEATURE_MFLO
#error code not present
#endif
    WPLT_PRI_ORDER,                         /* REX priority for the PLT task.*/
    VS_PRI_ORDER,                            /* REX priority for the VS task.*/
    UIM_PRI_ORDER,                          /* REX priority for the UIM task.*/
    NV_PRI_ORDER,                            /* REX priority for the NV task.*/
    QVPPL_PRI_ORDER,                      /* REX priority for the QVPPL task */
    GRAPH_PRI_ORDER,                          /* REX priority for GRAPH task.*/
    VIDEO_ASYNC_PRI_ORDER, /* Asynchronous video I/O task */
#if defined(FS_FEATURE_RMTEFS_SERVER) && !defined(FEATURE_IG_EFS_EXT_SERVER)
    EFS_SRVR_PRI_ORDER,                /* REX priority for the EFS server task.*/
#endif
#ifdef FEATURE_IXFILESVC
    IXFILESVC_PRI_ORDER, /* Order of REX Priority for IxFile Svc task */
#endif
    FS_PRI_ORDER,                            /* REX priority for the FS task.*/
#ifdef FEATURE_QVP_APP_INCLUDE
#error code not present
#endif /* FEATURE_QVP_APP_INCLUDE */
#ifdef FEATURE_QIPCALL
    QIPCALL_PRI_ORDER,
#endif /* FEATURE_QIPCALL*/
#ifdef FEATURE_QDJTEST
    QDJTEST_PRI_ORDER,
#endif /* FEATURE_QDJTEST*/
    SFAT_PRI_ORDER,                        /* REX priority for the SFAT task.*/
#ifdef FEATURE_JOYST_TASK
    JOYST_PRI_ORDER,                   /* REX priority for the joystick task */
#endif
#ifdef FEATURE_FS_TESTAPP
#error code not present
#endif
#ifdef FEATURE_FS_BENCHMARK
    FS_CPU_HOG_PRI_ORDER,       /* Must be higher priority than benchmark task */
    FS_BENCHMARK_PRI_ORDER,     /* Lower priority than FS tasks */
#endif
    MMC_TEST_PRI_ORDER,       /* REX priority for the MMC TEST.*/
    FS_COMPAT_PRI_ORDER,      /* REX priority for the EFS Compatibility task.*/
    SLEEP_PRI_HI_ORDER,         /* REX priority for SLEEP task after NV task.*/
    PBM_PRI_ORDER,                          /* REX priority for the PBM task.*/
    GSDI_PRI_ORDER,                  /* REX priority for the GSDI (SIM) task.*/
    GSTK_PRI_ORDER,  /* REX priority for the Generic SIM Toolkit Task (GSTK) */
    GSDI_UIM_SERVER_PRI_ORDER,
              /* REX priority for the UIM Server Task (in place of real UIM) */
    DS_PRI_ORDER,            /* REX priority for the Data Services (DS) task.*/
    DH_PRI_ORDER,                                /* REX priority for DH task.*/

#ifdef FEATURE_NV_CNV
    CNV_PRI_ORDER,                          /* REX priority for the CNV task */
#endif

#ifdef FEATURE_WLAN
#error code not present
#endif
#ifdef FEATURE_IXTIMESVC
   IXTIMESVC_PRI_ORDER,
#endif
#ifdef FEATURE_IXHTTPSVC
    IXHTTPSVC_PRI_ORDER,
#endif
    PS_PRI_ORDER,        /* REX priority for the Protocol Services (PS) task.*/
#ifdef FEATURE_WLAN
#error code not present
#endif
#ifdef FEATURE_QVPHONE_SIP
#error code not present
#endif
#ifdef FEATURE_IXSQLSVC
    IXSQLSVC_PRI_ORDER,  /* Order of REX Priority for IxSQL Svc task */
    IXSQLCLNT_PRI_ORDER,  /* Order of REX Priority for IxSQL Clnt task */
    IXSQLTEST_PRI_ORDER,
    ISQLTEST_PRI_ORDER,
#endif
#ifdef FEATURE_IXCONTENTSVC
    IXCONTENTSVC_PRI_ORDER,
    IXCONTENTCLNT_PRI_ORDER,
#endif
#ifdef FEATURE_IXDESCRAMBLERSVC
    IXDESCRAMBLERSVC_PRI_ORDER,
#endif
    SECIPS_PRI_ORDER,            /* Order of REX priority for the IPSec task */
#ifdef FEATURE_IPC_TEST
#error code not present
#endif
    AL1_PRI_ORDER,                          /* REX priority for the AL1 task.*/
    RRCUT_PRI_ORDER,                      /* REX priority for the RRCUT task.*/
    SECTESTAPP_PRI_ORDER,                /* REX priority for SECTESTAPP task.*/

#ifdef FEATURE_SECRND
    /* security services rnd task */
    SECRND_PRI_ORDER,
#endif

#ifdef FEATURE_SEC_USFS
    /* security services sfs task */
    SFS_PRI_ORDER,
#endif

    /* DRM priority should be higher than SSL and Crypto */
    SECDRM_PRI_ORDER,

      /* SSL should always be higher priority to the crypto task since it is
      ** using crypto services, and in general any task using crypto services
      ** should be higher priority compared to crypto
      */
    SECSSL_PRI_ORDER,                           /* REX priority for SSL task.*/
    SEC_PRI_ORDER,                              /* REX priority for SEC task.*/
#ifdef FEATURE_SEC_CONTENT_AGENT
    CONTENTAGENTDIAG_PRI_ORDER,                 /* REX priority for ContentAgentDiag task.*/
#endif
    MENU_PRI_ORDER,                        /* REX priority for the Menu task.*/
    PDSMCLIENT_PRI_ORDER,                /* REX priority for PDSMCLIENT task.*/
    TCPTEST_PRI_ORDER,                      /* REX priority for TCPTEST task.*/
    UDPTEST_PRI_ORDER,                      /* REX priority for UDPTEST task.*/
    PB_PRI_ORDER,                                /* REX priority for PB task.*/
#ifdef FEATURE_GSM_CB
#error code not present
#endif /* #ifdef FEATURE_GSM_CB */

    DTV_PRI_ORDER,                       /* REX priority for DTV task */

  /* SECCRYPTARM, MPEG4 task should be kept as low as possible, since they
  ** takes up all the available CPU.
  */
    SECCRYPTARM_PRI_ORDER,                /* SEC services ARM crunching task */
    BCDB_PRI_ORDER,                      /* REX priority of BCDBAPP task */
  #ifdef FEATURE_CAMERA_DRV_TASK
    CAMERA_DRV_PRI_ORDER, /* Order of REX priority for CAMERA_DRV Task */
  #endif /* FEATURE_CAMERA_DRV_TASK */
  #ifdef FEATURE_DMTASK
    DMTASK_PRI_ORDER,                /* Order of REX priority for the DM Task */
  #endif
    MPEG4_PV_TASK1_PRI_ORDER,            /* REX priority for MPEG PV task #1.*/
    MPEG4_PV_TASK2_PRI_ORDER,            /* REX priority for MPEG PV task #2.*/
    MPEG4_PV_TASK3_PRI_ORDER,            /* REX priority for MPEG PV task #3.*/
    MPEG4_PV_TASK4_PRI_ORDER,            /* REX priority for MPEG PV task #4.*/
    MPEG4_PV_TASK5_PRI_ORDER,            /* REX priority for MPEG PV task #5.*/
    MPEG4_PV_TASK6_PRI_ORDER,            /* REX priority for MPEG PV task #6.*/
    MPEG4_PV_TASK7_PRI_ORDER,            /* REX priority for MPEG PV task #7.*/
    MPEG4_PV_TASK8_PRI_ORDER,            /* REX priority for MPEG PV task #8.*/
    MPEG4_PV_TASK9_PRI_ORDER,            /* REX priority for MPEG PV task #9.*/
    MPEG4_PV_TASK10_PRI_ORDER,           /* REX priority for MPEG PV task #10.*/
    EFS_TRUNC_PRI_ORDER,                 /* REX priority of truncation task */
    NSRW_PRI_ORDER,                    /* REX priority for NON SRW task */
    REX_STAT_PRI_ORDER,
  #ifdef FEATURE_PROGRESSIVE_BOOT
    BACKGROUND_LOADER_PRI_ORDER, /* Order of REX priority for the loader Task*/
  #endif
  #ifdef FEATURE_APS
    APS_PRI_ORDER,                    /* Order of REX priority for the APS Task */
  #endif


  #ifdef FEATURE_EFS_HOTPLUG
    FS_HOTPLUG_PRI_ORDER,
  #endif

  #ifdef FEATURE_EXT_USB_P1
    USBEBI2_PRI_ORDER,     /* Order of REX priority for the USBD Task */
    USBAPI_PRI_ORDER,   /* Order of REX priority for the USBAPI Task */
    USBD_PRI_ORDER,  /* Order of REX priority for the USBEBI2 Task(very weak) */
  #endif

  #ifdef FEATURE_DTV_DVBH_OSF_DAL
    DTV_OSFDAL_VERY_HIGH_PRI_ORDER,
    DTV_OSFDAL_HIGH_PRI_ORDER,
    DTV_OSFDAL_NORMAL_PRI_ORDER,
    DTV_OSFDAL_LOW_PRI_ORDER,
    DTV_OSFDAL_VERY_LOW_PRI_ORDER,
  #endif
 #ifdef FEATURE_CGPS
#error code not present
  #endif /* FEATURE_CGPS */
  #ifdef FEATURE_CGPS
#error code not present
  #endif /* FEATURE_CGPS */

  #ifdef FEATURE_EFS_FDI
    /* FDI background task priority MUST be lower than any callers to the Filesystem */
    FDI_BG_TASK_PRI_ORDER, 
  #endif

  #ifdef FEATURE_L4
#error code not present
  #else
    SLEEP_PRI_ORDER,                      /* REX priority for the Sleep task.*/
  #endif
    MAX_PRI_ORDER_NUM                 /* Maximum number of Priority ordering.*/
  #endif /* !FEATURE_CDMA_800 && ... */
}task_priority_order_e_type;

#else
#error code not present
#endif      // #if !(defined(T_MSM7500) && defined(IMAGE_APPS_PROC))


#ifdef FEATURE_L4
#error code not present
#else /* FEATURE_L4 */


/* -----------------------------------------------------------------------
** This macro calculates the priority from the passed Priority Order number.
** If the passed priority order number is invalid, a priority of 0 (lowest
** priority) is returned.
** This is needed since REX treats a higher number as a higher priority
** whereas the priority order enumerator above puts a higher priority task
** at a higher level in the list, thereby assigning it a lower value in
** the priority order.
** ----------------------------------------------------------------------- */
#define TASK_PRIORITY(x) /*lint -save -e506 -e641 -e656 */ \
                         (((x) < MAX_PRI_ORDER_NUM) ?      \
                         (MAX_PRI_ORDER_NUM - (x)) : 0)    \
                         /*lint -restore */

#endif

#ifdef L4LINUX_TODO
#error code not present
#endif

#ifdef FEATURE_L4
#error code not present
#else
  #define STACK_SIZ_FACTOR       1
#endif
/*---------------------------------------------------------------------------

                    WATCHDOG TASK DECLARATIONS

---------------------------------------------------------------------------*/

/* REX priority for the Watchdog Task                                      */
#define  DOG_PRI        TASK_PRIORITY(DOG_PRI_ORDER)

/* Size (in units of rex_stack_word_type) of Watchdog Task stack           */
#define  DOG_STACK_SIZ  (2048*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

extern rex_stack_word_type    dog_stack[ ];
extern rex_tcb_type           dog_tcb;

#ifdef FEATURE_PROGRESSIVE_BOOT
#ifndef FEATURE_TIMER_TASK
/*---------------------------------------------------------------------------

                       INTERRUPT TASK DECLARATIONS

---------------------------------------------------------------------------*/

/* REX priority for the interrupt Task                                         */
#define  INTERRUPT_PRI      TASK_PRIORITY(INTERRUPT_PRI_ORDER)
/* Size (in units of rex_stack_word_type) of interrupt stack                   */
#define  INTERRUPT_STACK_SIZ (1024*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

extern rex_stack_word_type    interrupt_stack[ ];
extern rex_tcb_type           interrupt_tcb;

#endif /* FEATURE_TIMER_TASK */

/*---------------------------------------------------------------------------

                       PAGER TASK DECLARATIONS

---------------------------------------------------------------------------*/

/* REX priority for the pager Task                                         */
#define  PAGER_PRI                  TASK_PRIORITY(PAGER_PRI_ORDER)
/* Size (in units of rex_stack_word_type) of pager stack                   */
#define  PAGER_STACK_SIZ (1024*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

extern rex_stack_word_type    pager_stack[ ];
extern rex_tcb_type           pager_tcb;


/*---------------------------------------------------------------------------

                       BACKGROUND TASK DECLARATIONS

---------------------------------------------------------------------------*/
/* REX priority for the background Task                                         */
#define  BACKGROUND_LOADER_PRI      TASK_PRIORITY(BACKGROUND_LOADER_PRI_ORDER)
/* Size (in units of rex_stack_word_type) of pager stack                   */
#define  BACKGROUND_LOADER_STACK_SIZ (1024*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

extern rex_stack_word_type    background_loader_stack[ ];
extern rex_tcb_type           background_loader_tcb;

#endif /* FEATURE_PROGRESSIVE_BOOT */



/*---------------------------------------------------------------------------

                     SOUND TASK DECLARATIONS

---------------------------------------------------------------------------*/

/* REX priority for the Sound Task                                         */
#define  SND_PRI        TASK_PRIORITY(SND_PRI_ORDER)

/* Size (in units of rex_stack_word_type) of Sound Task stack              */
#define  SND_STACK_SIZ  (2048*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

extern rex_stack_word_type    snd_stack[ ];
extern rex_tcb_type           snd_tcb;

#ifdef FEATURE_AVS_SBCENC_TASK 
/*---------------------------------------------------------------------------

                     SBC ENCODER TASK DECLARATIONS

---------------------------------------------------------------------------*/

/* REX priority for the SBC Encoder Task                                      */
#define  SBCENC_PRI        TASK_PRIORITY(SBCENC_PRI_ORDER)

/* Size (in units of rex_stack_word_type) of Sound Task stack              */
#define  SBCENC_STACK_SIZ  (2048*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

extern rex_stack_word_type    sbcenc_stack[ ];
extern rex_tcb_type           sbcenc_tcb;

#endif

/*---------------------------------------------------------------------------

                NON-VOLATILE DATABASE TASK DECLARATIONS

---------------------------------------------------------------------------*/

/* REX priority for the Non-Volatile Memory Task                           */
#define  NV_PRI         TASK_PRIORITY(NV_PRI_ORDER)

/* NV priority for OTASP commit                                            */
#define  NV_PRI_HI      TASK_PRIORITY(NV_HI_PRI_ORDER)

/* Size (in units of rex_stack_word_type) of Non-Volatile Memory Task stack */
#ifdef FEATURE_EFS_FDI
  #define  NV_STACK_SIZ   (3072*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))
#else
#define  NV_STACK_SIZ   (2048*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))
#endif

extern rex_stack_word_type    nv_stack[ ];
extern rex_tcb_type           nv_tcb;

#define SMD_STACK_SIZ         ((2048 * 2 * STACK_SIZ_FACTOR)/sizeof(rex_stack_word_type))
#define SMD_TEST_STACK_SIZ    ((2048 * 2 * STACK_SIZ_FACTOR)/sizeof(rex_stack_word_type))

#define SMD_PRI               TASK_PRIORITY(SMD_PRI_ORDER)
#define SMD_TEST_PRI          TASK_PRIORITY(SMD_TEST_PRI_ORDER)

/*---------------------------------------------------------------------------

                EMBEDDED FILE SYSTEM TASK DECLARATIONS

---------------------------------------------------------------------------*/

#ifdef FEATURE_EFS
/* REX priority for the File System Task                                   */
#define  FS_PRI         TASK_PRIORITY(FS_PRI_ORDER)
#ifdef FEATURE_FS_TESTAPP
#error code not present
#endif

/* Size (in units of rex_stack_word_type) of File System Task stack        */
#define  FS_STACK_SIZ   (2048*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

extern rex_stack_word_type    fs_stack[ ];
extern rex_tcb_type           fs_tcb;
#endif

/* FDI Background Reclaim Task Stack and TCB */
#ifdef FEATURE_EFS_FDI
  #define  FDI_BG_PRI         TASK_PRIORITY(FDI_BG_TASK_PRI_ORDER)
  #define  FDI_BG_STACK_SIZ   (2048*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

extern rex_stack_word_type    fdi_bg_stack[ ];
extern rex_tcb_type           fdi_bg_tcb;
#endif


#ifdef FEATURE_EFS_SFAT
/* REX priority for the SFat Task                                          */
#define SFAT_PRI        TASK_PRIORITY(SFAT_PRI_ORDER)

/* Size (in units of rex_stack_word_type) of SFat Task stack               */
#define SFAT_STACK_SIZ  (2048*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

extern rex_stack_word_type    sfat_stack[ ];
extern rex_tcb_type           sfat_tcb;
#endif

#ifdef FEATURE_EFS_COMPATIBILITY
/* The priority of the EFS compatibility task                              */
#define FS_COMPAT_PRI    TASK_PRIORITY(FS_COMPAT_PRI_ORDER)
#endif

#ifdef FEATURE_EFS2
/* The priority of the EFS2 delayed truncate                               */
#define EFS_TRUNC_PRI             TASK_PRIORITY(EFS_TRUNC_PRI_ORDER)
#endif /* FEATURE_EFS2 */

#ifdef FEATURE_FS_BENCHMARK
#define FS_BENCHMARK_PRI  TASK_PRIORITY(FS_BENCHMARK_PRI_ORDER)
#define FS_CPU_HOG_PRI    TASK_PRIORITY(FS_CPU_HOG_PRI_ORDER)
#endif

#if defined(FS_FEATURE_RMTEFS_SERVER) && !defined(FEATURE_IG_EFS_EXT_SERVER)
#define EFS_SRVR_PRI     TASK_PRIORITY(EFS_SRVR_PRI_ORDER)

#define EFS_SRVR_STACK_SIZ  (2048*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

extern rex_stack_word_type efs_srvr_stack[];
extern rex_tcb_type        efs_srvr_tcb;
#endif /* FS_FEATURE_RMTEFS_SERVER && !FEATURE_IG_EFS_EXT_SERVER */

/* MMC Test Task Priority */
#define  MMC_TEST_PRI  TASK_PRIORITY(MMC_TEST_PRI_ORDER)

#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900) || defined(FEATURE_ACP)
/*---------------------------------------------------------------------------

                      TRANSMIT TASK DECLARATIONS

---------------------------------------------------------------------------*/

/* REX priority for the Transmit Task                                      */
#define  TX_PRI         TASK_PRIORITY(TX_PRI_ORDER)

/* Size (in units of rex_stack_word_type) of Transmit Task stack           */
#define  TX_STACK_SIZ   (2048*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

extern rex_tcb_type           tx_tcb;
extern rex_stack_word_type    tx_stack[ ];


/*---------------------------------------------------------------------------

                       RECEIVE TASK DECLARATIONS

---------------------------------------------------------------------------*/

/* REX priority for the Receive Task                                       */
#define  RX_PRI         TASK_PRIORITY(RX_PRI_ORDER)

/* Size (in units of rex_stack_word_type) of Receive Task stack            */
#define  RX_STACK_SIZ   (1144*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

extern rex_stack_word_type    rx_stack[ ];
extern rex_tcb_type           rx_tcb;


/*---------------------------------------------------------------------------

                       SEARCHER TASK DECLARATIONS

---------------------------------------------------------------------------*/

/* REX priority for the Searcher Task                                      */
#define  SRCH_PRI       TASK_PRIORITY(SRCH_PRI_ORDER)

/* SRCH priority for near instant response                                 */
#define  SRCH_PRI_MAX   TASK_PRIORITY(SRCH_MAX_PRI_ORDER)

/* SRCH priority for fast MC response                                      */
#define  SRCH_PRI_HI    TASK_PRIORITY(SRCH_HI_PRI_ORDER)

/* SRCH priority for unthrottled ACQ srch                                  */
#define  SRCH_PRI_LO    TASK_PRIORITY(SRCH_LO_PRI_ORDER)

/* SRCH priority for 8X mode                                               */
#define  SRCH_PRI_MD    TASK_PRIORITY(SRCH_MD_PRI_ORDER)

#ifdef FEATURE_GPSONE
#error code not present
#else
/* Size (in units of rex_stack_word_type) of Searcher Task stack           */
#define  SRCH_STACK_SIZ  (2800*STACK_SIZ_FACTOR/ sizeof(rex_stack_word_type))
#endif

extern rex_stack_word_type    srch_stack[ ];
extern rex_tcb_type           srch_tcb;

/*---------------------------------------------------------------------------

                   LAYER 2 (RXTX) TASK DECLARATIONS

---------------------------------------------------------------------------*/

/* REX priority for the RXTX Task                                          */
#define  RXTX_PRI       TASK_PRIORITY(RXTX_PRI_ORDER)

/* Size (in units of rex_stack_word_type) of RXTX Task stack               */
#define  RXTX_STACK_SIZ (2048*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

extern rex_stack_word_type    rxtx_stack[ ];
extern rex_tcb_type           rxtx_tcb;

#endif /* FEATURE_CDMA_800 || FEATURE_CDMA_1900 || FEATURE_ACP */

/*---------------------------------------------------------------------------

                   MAIN CONTROL TASK DECLARATIONS

---------------------------------------------------------------------------*/

/* REX priority for the Main Control Task                                  */
#define  MC_PRI         TASK_PRIORITY(MC_PRI_ORDER)

/* MC priority for fast RX responsiveness                                  */
#define  MC_PRI_HI      TASK_PRIORITY(MC_PRI_HI_ORDER)

/* MC priority while on TC to improve Hard Handoff performance             */
#define  MC_PRI_TC      TASK_PRIORITY(MC_PRI_TC_ORDER)

/* Size (in units of rex_stack_word_type) of Main Control Task stack       */
#define  MC_STACK_SIZ   (4096*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

extern rex_stack_word_type    mc_stack[ ];
extern rex_tcb_type           mc_tcb;



#ifdef FEATURE_DIAG_RPC
#error code not present
#endif /* #ifdef FEATURE_DIAG_RPC */

/*---------------------------------------------------------------------------

                      HANDSET TASK DECLARATIONS

---------------------------------------------------------------------------*/

/* REX priority for the Handset Task                                       */
#define  HS_PRI         TASK_PRIORITY(HS_PRI_ORDER)
#ifdef FEATURE_BITMAP
/* Size (in units of rex_stack_word_type) of Handset Task stack            */
#define  HS_STACK_SIZ   (2048*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))
#else
/* Size (in units of rex_stack_word_type) of Handset Task stack            */
#define  HS_STACK_SIZ   (744*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))
#endif /* FEATURE_BITMAP */

extern rex_stack_word_type    hs_stack[ ];
extern rex_tcb_type           hs_tcb;

#ifdef FEATURE_QMIP
#error code not present
#endif /* FEATURE_QMIP */


/*---------------------------------------------------------------------------

                     DIAGNOSTIC TASK DECLARATIONS

---------------------------------------------------------------------------*/

/* REX priority for the Diagnostic Task                                    */
#define  DIAG_PRI       TASK_PRIORITY(DIAG_PRI_ORDER)

/* Size (in units of rex_stack_word_type) of Diagnostic Task stack         */
#ifdef FEATURE_STACK_REDUCTION
#ifdef FEATURE_LOW_MEM_BIGFONT
#define  DIAG_STACK_SIZ (2048*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))
#else
#define  DIAG_STACK_SIZ (4096*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))
#endif
#else
#define  DIAG_STACK_SIZ (8192*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))
#endif
extern rex_stack_word_type    diag_stack[ ];
extern rex_tcb_type           diag_tcb;


#ifdef FEATURE_IG_QDIAG
/*---------------------------------------------------------------------------

                     DIAGNOSTIC TASK DECLARATIONS

---------------------------------------------------------------------------*/

#define  QDIAG_PRI            TASK_PRIORITY(QDIAG_PRI_ORDER)

/* Size (in units of rex_stack_word_type) of QDIAG Task stack         */
#if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
#else
  #define  QDIAG_STACK_SIZ (1280*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))
#endif /* FEATURE_GSM && FEATURE_WCDMA */
extern rex_stack_word_type    qdiag_stack[ ];
extern rex_tcb_type           qdiag_tcb;

#endif  /* FEATURE_IG_QDIAG */


#ifdef FEATURE_MDSP_TASK
/*---------------------------------------------------------------------------

                     MDSP SERVICES TASK DECLARATIONS

---------------------------------------------------------------------------*/

/* REX priority for the MDSP Services Task                                 */
  #define  MDSP_PRI     TASK_PRIORITY(MDSP_PRI_ORDER)

/* Size (in units of rex_stack_word_type) of MDSP Services Task stack      */
  #define  MDSP_STACK_SIZ  (1024*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

  extern rex_stack_word_type mdsp_stack[ ];
  extern rex_tcb_type    mdsp_tcb;
#endif /* FEATURE_MDSP_TASK */


#ifdef FEATURE_QDSP_TASK
/*---------------------------------------------------------------------------

                     QDSP SERVICES TASK DECLARATIONS

---------------------------------------------------------------------------*/

/* REX priority for the QDSP Services Task                                 */
#define  QDSP_PRI       TASK_PRIORITY(QDSP_PRI_ORDER)

/* Size (in units of rex_stack_word_type) of QDSP Services Task stack      */
#define  QDSP_STACK_SIZ  (1024*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

extern rex_stack_word_type    qdsp_stack[ ];
extern rex_tcb_type           qdsp_tcb;

#endif /* FEATURE_QDSP_TASK */


#ifdef FEATURE_QDSP_RTOS
/*---------------------------------------------------------------------------

                     ADSP RTOS SERVICES TASK DECLARATIONS

---------------------------------------------------------------------------*/

/* REX priority for the ADSP RTOS Services Task                                 */
#define  ADSP_RTOS_PRI       TASK_PRIORITY(ADSP_RTOS_PRI_ORDER)

/* Size (in units of rex_stack_word_type) of QDSP Services Task stack      */
#define  ADSP_RTOS_STACK_SIZ  (1024*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

extern rex_stack_word_type    adsp_rtos_stack[ ];
extern rex_tcb_type           adsp_rtos_tcb;

#endif /* FEATURE_QDSP_RTOS */


/*---------------------------------------------------------------------------

                     VOCODER TASK DECLARATIONS

---------------------------------------------------------------------------*/

/* REX priority for the Vocoder Task                                       */
#define  VOC_PRI        TASK_PRIORITY(VOC_PRI_ORDER)

/* Size (in units of rex_stack_word_type) of Vocoder Task stack            */
#define  VOC_STACK_SIZ  (2048*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

extern rex_stack_word_type    voc_stack[ ];
extern rex_tcb_type           voc_tcb;


/*---------------------------------------------------------------------------

                     WMS TASK DECLARATIONS

---------------------------------------------------------------------------*/
#if (defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS))

/* REX priority for the WMS Task      */
#define  WMS_PRI        TASK_PRIORITY(WMS_PRI_ORDER)
/* Size (words) of WMS Task stack     */
#define  WMS_STACK_SIZ   (4608*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))
extern rex_stack_word_type  wms_stack[ ];
extern rex_tcb_type         wms_tcb;

#endif /* FEATURE_CDSMS || FEATURE_GWSMS */


/*---------------------------------------------------------------------------

                     CALL MANAGER TASK DECLARATIONS

---------------------------------------------------------------------------*/
#ifdef FEATURE_CM
/* REX priority for the Call Manager Task                                  */
#define  CM_PRI         TASK_PRIORITY(CM_PRI_ORDER)

/* Size (in units of rex_stack_word_type) of Call Manager Task stack       */
#define  CM_STACK_SIZ   (4096*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

extern rex_stack_word_type    cm_stack[ ];
extern rex_tcb_type           cm_tcb;
#endif /* FEATURE_CM */


#ifdef FEATURE_BT
/*---------------------------------------------------------------------------

                       BLUETOOTH TASK DECLARATIONS

---------------------------------------------------------------------------*/

/* REX priority for the Bluetooth Task                                     */
#define  BT_PRI         TASK_PRIORITY(BT_PRI_ORDER)

/* Size (in units of rex_stack_word_type) of Bluetooth Task stack          */
#ifdef FEATURE_STACK_REDUCTION
#ifdef FEATURE_LOW_MEM_BIGFONT
#define  BT_STACK_SIZ   (1024*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))
#else
#define  BT_STACK_SIZ   (2048*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))
#endif
#else
#define  BT_STACK_SIZ   (4096*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))
#endif
extern rex_stack_word_type    bt_stack[ ];
extern rex_tcb_type           bt_tcb;

#ifdef FEATURE_BT_EXTPF
/*---------------------------------------------------------------------------

           BLUETOOTH EXTERNAL PROFILE TASK DECLARATIONS

---------------------------------------------------------------------------*/

/* REX priority for the Bluetooth External Profile Task                    */
#define  BT_PF_PRI  TASK_PRIORITY(BT_PF_PRI_ORDER)

/* Size (in units of rex_stack_word_type) of BT External Profile Task stack*/
#ifdef FEATURE_STACK_REDUCTION
#ifdef FEATURE_LOW_MEM_BIGFONT
#define  BT_PF_STACK_SIZ  (1024*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))
#else
#define  BT_PF_STACK_SIZ  (2048*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))
#endif
#else
#define  BT_PF_STACK_SIZ  (4096*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))
#endif
extern rex_stack_word_type    bt_pf_stack[ ];
extern rex_tcb_type           bt_pf_tcb;

#endif /* FEATURE_BT_EXTPF */

#endif /* FEATURE_BT */

/*---------------------------------------------------------------------------

                    POSITION DETERMINATION SESSION MANAGER DECLARATIONS

---------------------------------------------------------------------------*/

#if defined(FEATURE_GPSONE) && !defined(FEATURE_CGPS)
#error code not present
#endif  /* FEATURE_GPSONE && !FEATURE_CGPS */

/*---------------------------------------------------------------------------

         POSITION DETERMINATION COMMUNICATION MANAGER DECLARATIONS
                    
---------------------------------------------------------------------------*/

#if defined(FEATURE_GPSONE) && !defined(FEATURE_CGPS)
#error code not present
#endif  /* FEATURE_GPSONE && !FEATURE_CGPS */

/*---------------------------------------------------------------------------

                    POSITION DETERMINATION SMS CLIENT DECLARATIONS

---------------------------------------------------------------------------*/

#ifdef FEATURE_GPSONE_SMS_CLIENT
#error code not present
#endif  /* FEATURE_GPSONE_SMS_CLIENT */

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

/* REX priority for the Dbgr Task                                    */
#define  DBGR_PRI       TASK_PRIORITY(DBGR_PRI_ORDER)

/* Size (in units of rex_stack_word_type) of Diagnostic Task stack         */
  #define  DBGR_STACK_SIZ ((1024 * 64)/sizeof(rex_stack_word_type))

extern rex_stack_word_type    dbgr_stack[ ];
extern rex_tcb_type           dbgr_tcb;

#endif /* FEATURE_DBGR */


/*---------------------------------------------------------------------------

                    USER INTERFACE TASK DECLARATIONS

---------------------------------------------------------------------------*/

/* REX priority for the User Interface Task                                */
#define  UI_PRI         TASK_PRIORITY(UI_PRI_ORDER)

/* Size (in units of rex_stack_word_type) of User Interface stack          */

#ifdef FEATURE_ODM_UI
/* Increasing the size of the UI stack from 8K to 64K */
 #define  UI_STACK_SIZ   ((1024 * 64)/sizeof(rex_stack_word_type))
#else
#ifdef CUST_EDITION
 #define  UI_STACK_SIZ   (8192*4*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))
#else
 #define  UI_STACK_SIZ   (8192*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))
#endif
#endif

extern rex_stack_word_type    ui_stack[ ];
extern rex_tcb_type           ui_tcb;


#if defined(FEATURE_TIMER_TASK)
/*---------------------------------------------------------------------------

                       TIMER TASK DECLARATIONS

---------------------------------------------------------------------------*/

/* REX priority for the Timer Task                                         */
#define  TIMER_PRI      TASK_PRIORITY(TIMER_PRI_ORDER)

/* Size (in units of rex_stack_word_type) of Timer stack                   */
#define  TIMER_STACK_SIZ (2048*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

extern rex_stack_word_type    timer_stack[ ];
extern rex_tcb_type           timer_tcb;

#endif /* FEATURE_TIMER_TASK */

#if defined(FEATURE_NSRW_FLASH)

#define  NSRW_PRI      TASK_PRIORITY(NSRW_PRI_ORDER)

#endif


#if defined(T_SLEEP) || defined(FEATURE_SLEEP)
/*---------------------------------------------------------------------------

                       SLEEP TASK DECLARATIONS

---------------------------------------------------------------------------*/

/* REX priority for the Sleep Task                                         */
#define  SLEEP_PRI      TASK_PRIORITY(SLEEP_PRI_ORDER)

/* Level to which searcher will bump Sleep task when drastically starved
** of CPU. This is just above AUTH, PS, DS,..etc, tasks which are possibly
** CPU-bound (and also are not always defined).                            */
#define  SLEEP_PRI_HI   TASK_PRIORITY(SLEEP_PRI_HI_ORDER)

/* Size (in units of rex_stack_word_type) of Sleep stack                   */
#define  SLEEP_STACK_SIZ (1024*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

extern rex_stack_word_type    sleep_stack[ ];
extern rex_tcb_type           sleep_tcb;

#endif /* T_SLEEP || FEATURE_SLEEP */

/*---------------------------------------------------------------------------

                       HOTPLUG TASK DECLARATIONS

---------------------------------------------------------------------------*/

#ifdef FEATURE_EFS_HOTPLUG
/* REX priority for the Hotplug Task                                         */
#define  FS_HOTPLUG_PRI TASK_PRIORITY(FS_HOTPLUG_PRI_ORDER)
#endif

#ifdef T_MSM7500
#error code not present
#endif /* T_MSM7500 */



#if (defined(FEATURE_DS) || defined(FEATURE_DATA))

#if defined(FEATURE_WLAN)
#error code not present
#endif /* FEATURE_WLAN  */

#ifdef FEATURE_COMP_TASK
/*---------------------------------------------------------------------------

                      COMP TASK DECLARATIONS

---------------------------------------------------------------------------*/

/* REX priority for the Data Services Task                                 */
#define  COMP_PRI         TASK_PRIORITY(COMP_PRI_ORDER)

/* Size (in units of rex_stack_word_type) of Data Services stack           */
#define  COMP_STACK_SIZ  ((4096 * 2)*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

extern rex_stack_word_type    comp_stack[ ];
extern rex_tcb_type           comp_tcb;

#endif /* FEATURE_COMP_TASK */

/*---------------------------------------------------------------------------

                    DATA SERVICES TASK DECLARATIONS

---------------------------------------------------------------------------*/

/* REX priority for the Data Services Task                                 */
#define  DS_PRI         TASK_PRIORITY(DS_PRI_ORDER)

/* Size (in units of rex_stack_word_type) of Data Services stack           */
#define  DS_STACK_SIZ  ((2048 * 2)*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

extern rex_stack_word_type    ds_stack[ ];
extern rex_tcb_type           ds_tcb;

/*---------------------------------------------------------------------------

                    DATA PROTOCOL TASK DECLARATIONS

---------------------------------------------------------------------------*/

/* REX priority for the Protocol Services Task                             */
#define  PS_PRI         TASK_PRIORITY(PS_PRI_ORDER)

/* Size (in units of rex_stack_word_type) of Protocol Services stack       */
#define  PS_STACK_SIZ  (4096*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

extern rex_stack_word_type    ps_stack[ ];
extern rex_tcb_type           ps_tcb;

#ifdef FEATURE_WCDMA
#error code not present
#endif /* FEATURE_WCDMA */

#if ( (defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS)) && !defined(FEATURE_WCDMA_PLT) && !defined(FEATURE_GSM_PLT))
/*---------------------------------------------------------------------------

                   GCSD PROTOCOL STACK TASK DECLARATIONS

---------------------------------------------------------------------------*/
   #define  DS_GCSD_PRI    TASK_PRIORITY(DS_GCSD_PRI_ORDER)
   #define  DS_GCSD_STACK_SIZ    (2048*2*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))
   extern rex_stack_word_type Gcsd_stack[];
   extern rex_tcb_type        Gcsd_tcb;

#endif /* ( FEATURE_DATA_GCSD || FEATURE_DATA_WCDMA_CS ) && !FEATURE_WCDMA_PLT && !FEATURE_GSM_PLT */

#endif /* FEATURE_DS || FEATURE_DATA */



#ifdef FEATURE_BCMCS
#error code not present
#endif /* FEATURE_BCMCS */


#ifdef FEATURE_ONCRPC
#error code not present
#endif /* FEATURE_ONCRPC */


#ifdef FEATURE_SEC
/*---------------------------------------------------------------------------

                    SECURITY SERVICES TASK DECLARATIONS

---------------------------------------------------------------------------*/

/* REX priority for the Security Services Task                             */
/* This is slightly below that of the PS task.                             */
#define  SEC_PRI        TASK_PRIORITY(SEC_PRI_ORDER)

/* Size (in units of rex_stack_word_type) of Security Services stack       */
#ifdef FEATURE_STACK_REDUCTION
#ifdef FEATURE_LOW_MEM_BIGFONT
#define  SEC_STACK_SIZ  (1024*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))
#else
#define  SEC_STACK_SIZ  (2048*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))
#endif
#else
#define  SEC_STACK_SIZ  (4096*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))
#endif
extern rex_stack_word_type    sec_stack[ ];
extern rex_tcb_type           sec_tcb;

/*---------------------------------------------------------------------------

                  SECURITY SERVICES DIAG SERVER DECLARATIONS

---------------------------------------------------------------------------*/
#ifdef FEATURE_SEC_CONTENT_AGENT
#define  CONTENTAGENTDIAG_PRI        TASK_PRIORITY(CONTENTAGENTDIAG_PRI_ORDER)

/* Size (in units of rex_stack_word_type) of the SEC Diag Server stack     */
#define  CONTENTAGENTDIAG_STACK_SIZ (2048*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

extern rex_stack_word_type    contentagentdiag_stack[ ];
extern rex_tcb_type           contentagentdiag_tcb;
#endif

#ifdef FEATURE_SECRND
/* REX priority for the Security Services RND Task                         */
#define  SECRND_PRI        TASK_PRIORITY(SECRND_PRI_ORDER)

/* Size (in units of rex_stack_word_type) of Security Services stack       */
#ifdef FEATURE_STACK_REDUCTION
#ifdef FEATURE_LOW_MEM_BIGFONT
#define  SECRND_STACK_SIZ  (1024*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))
#else
#define  SECRND_STACK_SIZ  (2048*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))
#endif
#else
#define  SECRND_STACK_SIZ  (4096*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))
#endif
extern rex_stack_word_type    secrnd_stack[ ];
extern rex_tcb_type           secrnd_tcb;
#endif

#ifdef FEATURE_SEC_USFS
/* REX priority for the Security Services RND Task                         */
#define  SFS_PRI        TASK_PRIORITY(SFS_PRI_ORDER)

/* Size (in units of rex_stack_word_type) of Security Services stack       */
#ifdef FEATURE_STACK_REDUCTION
#ifdef FEATURE_LOW_MEM_BIGFONT
#define  SFS_STACK_SIZ  (1024*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))
#else
#define  SFS_STACK_SIZ  (2048*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))
#endif
#else
#define  SFS_STACK_SIZ  (4096*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))
#endif
extern rex_stack_word_type    sfs_stack[ ];
extern rex_tcb_type           sfs_tcb;
#endif

#ifdef FEATURE_SEC_ARM_CRYPTO
/*---------------------------------------------------------------------------

                        SEC TASK DECLARATIONS

---------------------------------------------------------------------------*/
/* REX priority for the Sec Srvcs ARM crunching task  */
#define  SECCRYPTARM_PRI         TASK_PRIORITY(SECCRYPTARM_PRI_ORDER)
#ifdef FEATURE_STACK_REDUCTION
#ifdef FEATURE_LOW_MEM_BIGFONT
#define  SECCRYPTARM_STACK_SIZ   (1024*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))
#else
#define  SECCRYPTARM_STACK_SIZ   (1024*2*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))
#endif
#else
#define  SECCRYPTARM_STACK_SIZ   (1024*4*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))
#endif
extern   rex_stack_word_type     seccryptarm_stack[ ];
extern   rex_tcb_type            seccryptarm_tcb;
#endif

#ifdef FEATURE_SEC_IPSEC
/*---------------------------------------------------------------------------

                       IPSEC TASK DECLARATIONS

---------------------------------------------------------------------------*/
/* REX priority for the IPSec (IKE) task  */
#define  SECIPS_PRI              TASK_PRIORITY(SECIPS_PRI_ORDER)
#ifdef FEATURE_STACK_REDUCTION
#ifdef FEATURE_LOW_MEM_BIGFONT
#define  SECIPS_STACK_SIZ        (1024*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))
#else
#define  SECIPS_STACK_SIZ        (1024*2*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))
#endif
#else
#define  SECIPS_STACK_SIZ        (1024*4*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))
#endif
extern   rex_stack_word_type     secips_stack[ ];
extern   rex_tcb_type            secips_tcb;
#endif /* FEATURE_SEC_IPSEC */

#ifdef FEATURE_SEC_SSL
/*---------------------------------------------------------------------------

                    SSL Protocol TASK DECLARATIONS

---------------------------------------------------------------------------*/
/* REX priority for the SSL Task                             */

#define  SECSSL_PRI          TASK_PRIORITY(SECSSL_PRI_ORDER)

/* Size (in units of rex_stack_word_type) of Security Services stack       */
#ifdef FEATURE_STACK_REDUCTION
#ifdef FEATURE_LOW_MEM_BIGFONT
#define  SECSSL_STACK_SIZ  (1024*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))
#else
#define  SECSSL_STACK_SIZ  (2048*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))
#endif
#else
#define  SECSSL_STACK_SIZ  (4096*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))
#endif
extern rex_stack_word_type    secssl_stack[ ];
extern rex_tcb_type           secssl_tcb;

#endif /* FEATURE_SEC_SSL */


#ifdef FEATURE_SEC_DRM
#error code not present
#endif /* FEATURE_SEC_DRM */


#endif /* FEATURE_SEC */


#if (defined(FEATURE_SEC_SSL_TESTAPP) || defined(FEATURE_SEC_CRYPT_TESTAPP))
#error code not present
#endif /* FEATURE_SEC_SSL_TESTAPP || FEATURE_SEC_CRYPT_TESTAPP */



#ifdef FEATURE_DS_SOCKETS_TEST_TCP
#error code not present
#endif

#ifdef FEATURE_DS_SOCKETS_TEST_UDP
#error code not present
#endif



#if defined (FEATURE_AUTH) || defined (FEATURE_RUIM)
/*---------------------------------------------------------------------------

                    AUTHENTICATION TASK DECLARATIONS

---------------------------------------------------------------------------*/

/* REX priority for the Authentication Task                                */
#define  AUTH_PRI       TASK_PRIORITY(AUTH_PRI_ORDER)

/* Size (in units of rex_stack_word_type) of Authentication stack          */
#define  AUTH_STACK_SIZ (2048*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

extern rex_stack_word_type    auth_stack[ ];
extern rex_tcb_type           auth_tcb;
#endif /* FEATURE_AUTH or FEATURE_RUIM*/


#ifdef FEATURE_UIM
/*---------------------------------------------------------------------------

                    UIM TASK DECLARATIONS

---------------------------------------------------------------------------*/

/* REX priority for the UIM Task                                           */
#define  UIM_PRI        TASK_PRIORITY(UIM_PRI_ORDER)

/* Size (in units of rex_stack_word_type) of UIM stack                     */
#define  UIM_STACK_SIZ (2048*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

extern rex_stack_word_type    uim_stack[ ];
extern rex_tcb_type           uim_tcb;
#endif /* FEATURE_UIM*/

#ifdef FEATURE_DH
/*---------------------------------------------------------------------------

                         DH TASK DECLARATIONS

---------------------------------------------------------------------------*/

/* REX priority for the DH Task                                            */
#define  DH_PRI         TASK_PRIORITY(DH_PRI_ORDER)

/* Size (in units of rex_stack_word_type) of DH stack                      */
#define  DH_STACK_SIZ  (2048*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

extern rex_stack_word_type    dh_stack[ ];
extern rex_tcb_type           dh_tcb;
#endif /* FEATURE_DH */


#ifdef FEATURE_VS_TASK
/*---------------------------------------------------------------------------

                     VOICE SERVICES TASK DECLARATIONS

---------------------------------------------------------------------------*/

/* REX priority for the VS Task                                            */
#define  VS_PRI         TASK_PRIORITY(VS_PRI_ORDER)

/* REX high priority for the VS Task                                       */
#define  VS_PRI_HI      TASK_PRIORITY(VS_PRI_HI_ORDER)

/* Size (in units of rex_stack_word_type) of VS stack                      */
#define  VS_STACK_SIZ   (3072*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

extern rex_stack_word_type    vs_stack[ ];
extern rex_tcb_type           vs_tcb;
#endif /* FEATURE_VS_TASK */


/*---------------------------------------------------------------------------

                       ATI GRAPHICS TASK DECLARATIONS

---------------------------------------------------------------------------*/

#ifdef FEATURE_ATI_GRAPHICS
/* REX priority for the Graphics Task                                      */
#define  GRAPHICS_PRI      TASK_PRIORITY(GRAPHICS_PRI_ORDER)

/* Size (in uints of rex_stack_word_type) of Graphics Task stack           */
#define  GRAPHICS_STACK_SIZ   (1536/sizeof(rex_stack_word_type))

extern rex_stack_word_type graphics_stack[ ];
extern rex_tcb_type        graphics_tcb;

#endif /* FEATURE_ATI_GRAPHICS */


#ifdef FEATURE_GRAPH_TASK
/*---------------------------------------------------------------------------

                       GRAPHICS TASK DECLARATIONS

---------------------------------------------------------------------------*/

/* REX priority for the Graphics Task                                      */
#define  GRAPH_PRI      TASK_PRIORITY(GRAPH_PRI_ORDER)

/* Size (in uints of rex_stack_word_type) of Graphics Task stack           */
#if defined(FEATURE_SVG)
#define GRAPH_STACK_SIZ   (3288*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))
#elif defined(ARM_3D_GRAPHICS_TASK_ONLY) || defined(FEATURE_GRAPHICS_OPENGLES_FIRMWARE)
#error code not present
#else
#define GRAPH_STACK_SIZ   (2304*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))
#endif

extern rex_stack_word_type graph_stack[ ];
extern rex_tcb_type        graph_tcb;

/* REX priority for the video asynchronous I/O task.
 * See video_async.[ch] for stack and tcb definitions.
 */
#define VIDEO_ASYNC_PRI  TASK_PRIORITY(VIDEO_ASYNC_PRI_ORDER)

#endif /* FEATURE_GRAPH_TASK */

#ifdef FEATURE_CAMERA_DRV_TASK
/*---------------------------------------------------------------------------

                       CAMERA DRIVER TASK DECLARATIONS

---------------------------------------------------------------------------*/

/* REX priority for the Camera Driver Task                                 */
#define  CAMERA_DRV_PRI      TASK_PRIORITY(CAMERA_DRV_PRI_ORDER)

/* Size (in uints of rex_stack_word_type) of Camera Driver Task stack      */
#define CAMERA_DRV_STACK_SIZ   (2304*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

extern rex_stack_word_type camera_drv_stack[ ];
extern rex_tcb_type        camera_drv_tcb;

#endif /* FEATURE_CAMERA_DRV_TASK */

#ifdef FEATURE_HWTC
#error code not present
#endif /* FEATURE_HWTC */

/*---------------------------------------------------------------------------

                USB TASK DECLARATIONS

---------------------------------------------------------------------------*/

#if defined (FEATURE_USB) && !defined (FEATURE_NO_USB_HW_ON_PROCESSOR)
/* REX priority for the USB Task                                           */
#define  USBDC_PRI      TASK_PRIORITY(USBDC_PRI_ORDER)

/* Size (in units of rex_stack_word_type) of USB stack                     */
#define  USBDC_STACK_SIZ  (2048*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

extern rex_stack_word_type    usbdc_stack[ ];
extern rex_tcb_type           usbdc_tcb;

/*---------------------------------------------------------------------------

                USBHC TASK DECLARATIONS

---------------------------------------------------------------------------*/

#if defined (FEATURE_USB) && defined (FEATURE_USB_HOST_ENHANCED)
/* REX priority for the USBHC_URB Task                                     */
#define  USBHC_URB_PRI      TASK_PRIORITY(USBHC_URB_PRI_ORDER)

/* Size (in units of rex_stack_word_type) of USBHC_URB stack               */
#define  USBHC_URB_STACK_SIZ  (2048*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

extern rex_stack_word_type    usbhc_urb_stack[ ];
extern rex_tcb_type           usbhc_urb_tcb;

/* REX priority for the USBHC_DEV Task                                     */
#define  USBHC_DEV_PRI      TASK_PRIORITY(USBHC_DEV_PRI_ORDER)

/* Size (in units of rex_stack_word_type) of USBHC_DEV stack               */
#define  USBHC_DEV_STACK_SIZ  (2048*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

extern rex_stack_word_type    usbhc_dev_stack[ ];
extern rex_tcb_type           usbhc_dev_tcb;

#if defined(FEATURE_USB_HOST_TEST_ENHANCED) && \
    (defined(FEATURE_USB_HOST_MASS_STORAGE_TEST_ENHANCED) || \
     defined(FEATURE_USB_HOST_HID_TEST_ENHANCED))
  /* REX priority for the USBHC_TEST Task                                     */
#define  USBHC_TEST_PRI      TASK_PRIORITY(USBHC_TEST_PRI_ORDER)

/* Size (in units of rex_stack_word_type) of USBHC_TEST stack               */
#define  USBHC_TEST_STACK_SIZ  (2048*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

extern rex_stack_word_type    usbhc_test_stack[ ];
extern rex_tcb_type           usbhc_test_tcb;
#endif
#endif /* FEATURE_USB_HOST_ENHANCED*/

#endif /* defined (FEATURE_USB) && !defined (FEATURE_NO_USB_HW_ON_PROCESSOR) */

/*---------------------------------------------------------------------------

  HSU_AL_MS TASK DECLARATIONS
  
---------------------------------------------------------------------------*/
#ifdef FEATURE_HS_USB

#define HSU_AL_MS_PRI    TASK_PRIORITY(HSU_AL_MS_PRI_ORDER)
#define HSU_AL_TASK_PRI  TASK_PRIORITY(HSU_AL_TASK_PRI_ORDER)
/* All other task declarations are in hsu_al_ms.h . */

#endif /* FEATURE_HS_USB */

#ifdef FEATURE_IS2000_TDSO
/*---------------------------------------------------------------------------

                     TDSO TASK DECLARATIONS

---------------------------------------------------------------------------*/

/* REX priority for the TDSO Task                                          */
#define  TDSO_PRI       TASK_PRIORITY(TDSO_PRI_ORDER)

/* Size (in units of rex_stack_word_type) of TDSO stack                    */
#define  TDSO_STACK_SIZ   (1024*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

extern rex_stack_word_type    tdso_stack[ ];
extern rex_tcb_type           tdso_tcb;
#endif /* FEATURE_IS2000_TDSO */

#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */

#ifdef FEATURE_MULTIMODE_ARCH

/*---------------------------------------------------------------------------

                   Multi-mode Controller TASK DECLARATIONS

---------------------------------------------------------------------------*/

/* REX priority for the Multi Mode controller Task                         */
#define  MMOC_PRI       TASK_PRIORITY(MMOC_PRI_ORDER)

/* Size (in units of rex_stack_word_type) of MMOC stack        */
#define  MMOC_STACK_SIZ   (4096*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

extern rex_stack_word_type mmoc_stack[ ];
extern rex_tcb_type        mmoc_tcb;

#endif /* FEATURE_MULTIMODE_ARCH */


#if (defined(FEATURE_MULTIMODE_ARCH) || defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
/*---------------------------------------------------------------------------

                   TASK MAIN CONTROL TASK DECLARATIONS

---------------------------------------------------------------------------*/

/* REX priority for the Task Main Control Task                             */
#define  TMC_PRI        TASK_PRIORITY(TMC_PRI_ORDER)

/* Size (in units of rex_stack_word_type) of Main Control Task stack       */
#define  TMC_STACK_SIZ   (3072*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

extern rex_stack_word_type    tmc_stack[ ];
extern rex_tcb_type           tmc_tcb;

#endif /* FEATURE_MULTIMODE_ARCH || FEATURE_GSM || FEATURE_WCDMA */

/*---------------------------------------------------------------------------

                    GSDI TASK DECLARATIONS

---------------------------------------------------------------------------*/
#if (defined(FEATURE_MMGSDI) || defined(FEATURE_NAS) || defined(FEATURE_VIRTUAL_SIM))
/* REX priority for the GSDI Task                                          */
#define  GSDI_PRI       TASK_PRIORITY(GSDI_PRI_ORDER)

/* Size (in units of rex_stack_word_type) of GSDI stack                    */
#define  GSDI_STACK_SIZ (2048*2*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))


extern rex_stack_word_type    gsdi_stack[ ];
extern rex_tcb_type           gsdi_task_tcb;
#endif /* FEATURE_MMGSDI || FEATURE_NAS FEATURE_VIRTUAL_SIM*/

/*---------------------------------------------------------------------------

                   TEST SIM SERVER TASK DECLARATIONS

---------------------------------------------------------------------------*/
#ifdef FEATURE_VIRTUAL_SIM
/* REX priority for the GSDI_UIM_SERVER task                               */
  #define  GSDI_UIM_SERVER_PRI        TASK_PRIORITY(GSDI_UIM_SERVER_PRI_ORDER)

/* Size (in units of rex_stack_word_type) of GSDI_UIM_SERVER stack         */
  #define  GSDI_UIM_SERVER_STACK_SIZ  (1024*2*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

  extern   rex_tcb_type             gsdi_uim_server_task_tcb;
  extern   rex_stack_word_type      gsdi_uim_server_stack[ ];
#endif /* FEATURE_VIRTUAL_SIM */


/*---------------------------------------------------------------------------

                   GSTK TASK DECLARATIONS

---------------------------------------------------------------------------*/
#ifdef FEATURE_GSTK
/* REX priority for the GSTK task                                          */
  #define  GSTK_PRI        TASK_PRIORITY(GSTK_PRI_ORDER)
/* Size (in units of rex_stack_word_type) of GSTK stack                    */
  #define  GSTK_STACK_SIZ  (2048*2*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

  extern   rex_tcb_type         gstk_tcb;
  extern   rex_stack_word_type  gstk_stack[ ];
#endif /* FEATURE_GSTK */

/*---------------------------------------------------------------------------

                   TEST SAT APPLICATION TASK DECLARATIONS

---------------------------------------------------------------------------*/
#if defined(FEATURE_GSTK) && defined(FEATURE_VIRTUAL_SAT)

  #define  VIRTSAT_PRI          TASK_PRIORITY(VIRTSAT_PRI_ORDER)
  #define  VIRTSAT_STACK_SIZ    (2048*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

  extern   rex_tcb_type         virtsat_tcb;
  extern   rex_stack_word_type  virtsat_stack[ ];


#endif /* defined(FEATURE_GSTK) && defined(FEATURE_VIRUAL_SAT) */

/*---------------------------------------------------------------------------

                   WCDMA LAYER 1, 2, RRC TASK DECLARATIONS

---------------------------------------------------------------------------*/
/*******************************************************************
 * Note: L1's priority MUST be higher than everything except for TMC
 * and DOG due to the tight timing constraints that it has to deal
 * with.
 ******************************************************************* */

#ifdef FEATURE_WCDMA
#error code not present
#endif /* FEATURE_WCDMA */

/*---------------------------------------------------------------------------

                   GSM LAYER 1, 2, RR TASK DECLARATIONS

---------------------------------------------------------------------------*/
#ifdef FEATURE_GSM
#error code not present
#endif /* FEATURE_GSM */

/*---------------------------------------------------------------------------

                   PLT TASK DECLARATIONS

---------------------------------------------------------------------------*/
#ifdef FEATURE_WCDMA_PLT
#error code not present
#endif /* FEATURE_WCDMA_PLT */

#ifdef FEATURE_GSM_PLT
#error code not present
#endif /* FEATURE_GSM_PLT */

/*---------------------------------------------------------------------------

                   TC TASK DECLARATIONS

---------------------------------------------------------------------------*/
#ifdef FEATURE_TC
  /* REX priority for TC task      */
  #define  TC_PRI       TASK_PRIORITY(TC_PRI_ORDER)
  #define  TC_STACK_SIZ  (512*2*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

  extern   rex_tcb_type             tc_tcb;
  extern   rex_stack_word_type      tc_stack[ ];
#endif /* FEATURE_TC */

/*---------------------------------------------------------------------------

                     GSM TEST TASK DECLARATIONS

---------------------------------------------------------------------------*/
#ifdef FEATURE_GSM
#error code not present
#endif /* FEATURE_GSM */


#ifdef FEATURE_NAS
#error code not present
#endif /* FEATURE_NAS */

/*---------------------------------------------------------------------------

                   AL1 TASK DECLARATIONS  (For WCDMA VST)

---------------------------------------------------------------------------*/
#if ( defined(FEATURE_WCDMA_VST_FULLSTACK) || defined(FEATURE_WCDMA_VST_AS))
/* REX priority for the AL1 task */
  #define  AL1_PRI         TASK_PRIORITY(AL1_PRI_ORDER)
  #define  AL1_STACK_SIZ   (512*2*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

  extern  rex_stack_word_type al1_stack[ ];
  extern  rex_tcb_type    al1_tcb;
#endif /* FEATURE_WCDMA_VST_FULLSTACK ... */

/*---------------------------------------------------------------------------

                   RRC UT TASK DECLARATIONS  (For WCDMA VST Access-Stratum)

---------------------------------------------------------------------------*/
#ifdef FEATURE_WCDMA_VST_AS
#error code not present
#endif /* FEATURE_WCDMA_VST_AS */

/*---------------------------------------------------------------------------

                   MENU TASK DECLARATIONS

---------------------------------------------------------------------------*/
#if ( defined(FEATURE_WCDMA_VST_FULLSTACK) || defined(FEATURE_WCDMA_VST_AS))
/* FEATURE_MENU_UT  */
/* REX priority for the MENU Task */
  #define  MENU_PRI         TASK_PRIORITY(MENU_PRI_ORDER)
  #define  MENU_STACK_SIZ   (512*2*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

  extern  rex_stack_word_type menu_stack[ ];
  extern  rex_tcb_type    menu_tcb;
#endif /* FEATURE_WCDMA_VST_FULLSTACK ... */

/*---------------------------------------------------------------------------

                   LSM TASK DECLARATIONS

---------------------------------------------------------------------------*/
#ifdef FEATURE_MMGPS
#error code not present
#endif /* FEATURE_MMGPS */


/*---------------------------------------------------------------------------

                   MGP Related task DECLARATIONS

---------------------------------------------------------------------------*/
#ifdef FEATURE_MGP

  #define  MGPMC_PRI        TASK_PRIORITY(MGPMC_PRI_ORDER)
  #define  MGPMC_STACK_SIZ  (4096*2*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

  extern   rex_stack_word_type  mgpmc_stack[ ];
  extern   rex_tcb_type         mgpmc_tcb;

  #define  MGPNF_PRI        TASK_PRIORITY(MGPNF_PRI_ORDER)
  #define  MGPNF_STACK_SIZ  (6144*2*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

  extern   rex_stack_word_type  mgpnf_stack[ ];
  extern   rex_tcb_type         mgpnf_tcb;

  #define  MGPCD_PRI        TASK_PRIORITY(MGPCD_PRI_ORDER)
  #define  MGPCD_STACK_SIZ  (2048*2*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

  extern   rex_stack_word_type  mgpcd_stack[ ];
  extern   rex_tcb_type         mgpcd_tcb;

  #define  MGPAI_PRI        TASK_PRIORITY(MGPAI_PRI_ORDER)
  #define  MGPAI_STACK_SIZ  (4096*2*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

  extern   rex_stack_word_type  mgpai_stack[ ];
  extern   rex_tcb_type         mgpai_tcb;

  #ifdef FEATURE_MGPCC_TASK
  #define  MGPCC_PRI        TASK_PRIORITY(MGPCC_PRI_ORDER)
  #define  MGPCC_STACK_SIZ  (2048*2*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

  extern   rex_stack_word_type  mgpcc_stack[ ];
  extern   rex_tcb_type         mgpcc_tcb;
  #endif

#endif /* FEATURE_MGP */

#ifdef FEATURE_CGPS
#error code not present
#endif /* FEATURE_CGPS */

/*---------------------------------------------------------------------------

                   FTM TASK DECLARATIONS

---------------------------------------------------------------------------*/
#ifdef FEATURE_FTM_TASK
/* REX priority for the Location Services Manager task */
  #define  FTM_PRI        TASK_PRIORITY(FTM_PRI_ORDER)
  #define  FTM_STACK_SIZ  (3072*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

  extern   rex_tcb_type         ftm_tcb;
  extern   rex_stack_word_type  ftm_stack[ ];
#endif /* FEATURE_FTM_TASK*/

/*---------------------------------------------------------------------------

                   CHG TASK DECLARATIONS

---------------------------------------------------------------------------*/
#ifdef FEATURE_CHG_TASK
/* REX priority for the Location Services Manager task */
  #define  CHG_PRI        TASK_PRIORITY(CHG_PRI_ORDER)
#ifdef FEATURE_STACK_REDUCTION
  #define  CHG_STACK_SIZ  (1024*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))
#else
  #define  CHG_STACK_SIZ  (2048*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))
#endif

  extern   rex_tcb_type         chg_tcb;
  extern   rex_stack_word_type  chg_stack[ ];
#endif /* FEATURE_CHG_TASK*/

/*---------------------------------------------------------------------------

                   VBATT TASK DECLARATIONS

---------------------------------------------------------------------------*/
#ifdef FEATURE_VBATT_TASK
/* REX priority for the Location Services Manager task */
  #define  VBATT_PRI        TASK_PRIORITY(VBATT_PRI_ORDER)
  #define  VBATT_STACK_SIZ  (2048*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))
  
  extern   rex_stack_word_type  vbatt_stack[];
  extern   rex_tcb_type         vbatt_tcb ;
#endif /* FEATURE_VBATT_TASK*/

/*---------------------------------------------------------------------------

                   VIDEO TELEPHONY TASK DECLARATIONS

---------------------------------------------------------------------------*/
#ifdef FEATURE_QVPHONE

  /*-------------------- QVP Audio Engine ------------------------------*/
  #define QVP_AE_PRI            TASK_PRIORITY(QVP_AE_PRI_ORDER)
  #define QVP_AE_STACK_SIZ      (4096*2*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

  extern rex_stack_word_type    qvp_ae_task_stack[];
  extern rex_tcb_type           qvp_ae_tcb;

#ifdef FEATURE_QVP_APP_INCLUDE
#error code not present
#endif /* FEATURE_QVP_APP_INCLUDE */

  /*--------------------- QVP IO ---------------------------------------*/
  #define  QVPIO_PRI            TASK_PRIORITY(QVPIO_PRI_ORDER)
  #define  QVPIO_STACK_SIZ      (4096*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

  extern rex_stack_word_type    qvpio_stack[ ];
  extern rex_tcb_type           qvpio_tcb;

  /*--------------------- QVP PLayer -----------------------------------*/
  #define  QVPPL_PRI            TASK_PRIORITY(QVPPL_PRI_ORDER)
  #define  QVPPL_STACK_SIZ      (3072*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

  extern rex_stack_word_type    qvppl_stack[ ];
  extern rex_tcb_type           qvppl_tcb;

  /*--------------------- QVP MPEG4 ------------------------------------*/
  #define  QVP_MPEG4_PRI        TASK_PRIORITY(QVP_MPEG4_PRI_ORDER)
  #define  QVP_MPEG4_STACK_SIZ  (4096*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

  extern rex_stack_word_type    qvp_mpeg4_stack[QVP_MPEG4_STACK_SIZ ];
  extern rex_tcb_type           qvp_mpeg4_tcb;

  /*--------------------- QVP VFE --------------------------------------*/
  #define  QVP_VFE_PRI          TASK_PRIORITY(QVP_VFE_PRI_ORDER)
  #define  QVP_VFE_STACK_SIZ    (2048*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

  extern rex_stack_word_type    qvp_vfe_stack[];
  extern rex_tcb_type           qvp_vfe_tcb;

#ifdef FEATURE_QVPHONE_SIP
#error code not present
#endif /* FEATURE_QVPHONE_SIP */

  /*---------- QVP H223 Demux (created dynamically) --------------------*/
  #define QVP_H223_DEMUX_PRI    TASK_PRIORITY(QVP_H223_DEMUX_PRI_ORDER)

  /*-------- QVP H324 State Machine (created dynamically) --------------*/
  #define QVP_H324_SM_PRI       TASK_PRIORITY(QVP_H324_SM_PRI_ORDER)

#endif /* FEATURE_QVPHONE */

/*---------------------------------------------------------------------------

                   DTV (Digital TV) TASK DECLARATIONS

---------------------------------------------------------------------------*/
#ifdef FEATURE_DTV
#error code not present
#endif /* FEATURE_DTV*/


/*---------------------------------------------------------------------------

                    JOYSTICK TASK DECLARATIONS

---------------------------------------------------------------------------*/
#ifdef FEATURE_JOYST_TASK

   /* REX priority for the Joystick Task                                      */
   #define  JOYST_PRI        TASK_PRIORITY(JOYST_PRI_ORDER)

   /* Size (in units of rex_stack_word_type) of Joystick Task stack           */
   #define  JOYST_STACK_SIZ  (2048*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

   extern rex_stack_word_type    joyst_stack[ ];
   extern rex_tcb_type           joyst_tcb;

#endif  /* FEATURE_JOYST_TASK */


/*---------------------------------------------------------------------------

                              PBM TASK DECLARATIONS

---------------------------------------------------------------------------*/
#ifdef FEATURE_PBM_TASK
/* REX priority for the PBM Task                                           */
#define  PBM_PRI         TASK_PRIORITY(PBM_PRI_ORDER)

/* Size (in units of rex_stack_word_type) of PBM Task stack       */
#define  PBM_STACK_SIZ   (4096*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

extern rex_stack_word_type    pbm_stack[ ];
extern rex_tcb_type           pbm_tcb;
#endif /* FEATURE_PBM_TASK */

#ifdef SIRIUS_PLATFORM
#error code not present
#endif /* SIRIUS_PLATFORM */


#ifdef FEATURE_FM
/*---------------------------------------------------------------------------

                        FM-RDS TASK DECLARATIONS

---------------------------------------------------------------------------*/

// REX priority for the FM-RDS Task
#define  FMRDS_PRI         TASK_PRIORITY(FMRDS_PRI_ORDER)

// Size (in units of rex_stack_word_type) of FM-RDS Task stack
#define  FMRDS_STACK_SIZ   (4096*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

extern rex_stack_word_type    fmrds_stack[ ];
extern rex_tcb_type           fmrds_tcb;

#endif // FEATURE_FM

/*---------------------------------------------------------------------------

                       PM SERVER TASK DECLARATIONS

---------------------------------------------------------------------------*/

#ifdef FEATURE_PM_SERVER

/* Size (in units of rex_stack_word_type) of pm server stack */
  #define  PMSERV_STACK_SIZ (1024*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

/* pm server priority, should put the priority into the task pri
  table later */
 #define PM_SERVER_PRI  150

extern rex_stack_word_type    pms_stack[ ];
extern rex_tcb_type           pms_tcb;

#endif /* FEATURE_PM_SERVER */

#ifdef FEATURE_APS
/*---------------------------------------------------------------------------

                        APS TASK DECLARATIONS

---------------------------------------------------------------------------*/

#define  APS_PRI     TASK_PRIORITY(APS_PRI_ORDER)
#define  APS_STACK_SIZ (1024*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

extern rex_stack_word_type    aps_stack[ ];
extern rex_tcb_type           aps_tcb;

#endif  /* FEATURE_APS */

#ifdef FEATURE_CPU_BASED_FLOW_CONTROL
/* REX priority for the Flow Controller Task      */
  #define  FC_PRI         TASK_PRIORITY(FC_PRI_ORDER)
  #define  FC_STACK_SIZ   (1024*2*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

  extern  rex_stack_word_type fc_stack[ ];
  extern  rex_tcb_type    fc_tcb;
#endif /* FEATURE_CPU_BASED_FLOW_CONTROL */

#ifdef FEATURE_ATS
/*---------------------------------------------------------------------------

                        ATS TASK DECLARATIONS

---------------------------------------------------------------------------*/

#define  ATS_PRI     TASK_PRIORITY(ATS_PRI_ORDER)
#define  ATS_STACK_SIZ (1024*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

extern rex_stack_word_type    ats_stack[ ];
extern rex_tcb_type           ats_tcb;

#endif  /* FEATURE_ATS */

#ifdef FEATURE_DISP_TASK
/*---------------------------------------------------------------------------

                    DISPLAY TASK DECLARATIONS

---------------------------------------------------------------------------*/
/* REX priority for Display Task */
#define  DISP_PRI     TASK_PRIORITY(DISP_PRI_ORDER)

/* Size of Display Task stack               */
#ifndef  DISP_STACK_SIZ
#define  DISP_STACK_SIZ  (4096*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))
#endif /* DISP_STACK_SIZ */

extern rex_stack_word_type disp_stack[ ];
extern rex_tcb_type        disp_tcb;

#endif /* FEATURE_DISP_TASK */

#ifdef FEATURE_NV_CNV

/*---------------------------------------------------------------------------

                    COMPACT NV TASK DECLARATIONS

---------------------------------------------------------------------------*/

/* REX priority for the Compact NV Task                                    */
#define  CNV_PRI         TASK_PRIORITY(CNV_PRI_ORDER)

/* Size (in units of rex_stack_word_type) of User Interface stack          */
#define  CNV_STACK_SIZ   (8192/sizeof(rex_stack_word_type))

extern rex_stack_word_type    cnv_stack[ ];
extern rex_tcb_type           cnv_tcb;

#endif /* FEATURE_NV_CNV */

#ifdef FEATURE_DMTASK
/*---------------------------------------------------------------------------

                    DM TASK DECLARATIONS

---------------------------------------------------------------------------*/

/* REX priority for the DM Task                                            */
#define  DMTASK_PRI         TASK_PRIORITY(DMTASK_PRI_ORDER)

/* Size (in units of rex_stack_word_type) of DM stack                      */
#define  DMTASK_STACK_SIZ   (8192/sizeof(rex_stack_word_type))

extern rex_stack_word_type    dmtask_stack[ ];
extern rex_tcb_type           dmtask_tcb;

#endif /* FEATURE_DMTASK */

#ifdef FEATURE_QIPCALL
/*---------------------------------------------------------------------------

                        QIPCALL TASK DECLARATIONS

---------------------------------------------------------------------------*/

#define  QIPCALL_PRI     TASK_PRIORITY(QIPCALL_PRI_ORDER)
#define  QIPCALL_STACK_SIZ (4096*2*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

extern rex_stack_word_type    qipcall_stack[ ];
extern rex_tcb_type           qipcall_tcb;

#endif  /* FEATURE_QIPCALL */

/*---------------------------------------------------------------------------

                     MediaFLO TASK DECLARATIONS

---------------------------------------------------------------------------*/

#ifdef FEATURE_MFLO
#error code not present
#endif /* FEATURE_MFLO */

/*---------------------------------------------------------------------------
          Epson USB TASK DECLARATIONS

---------------------------------------------------------------------------*/
#ifdef FEATURE_EXT_USB_P1

#define  USBD_PRI        TASK_PRIORITY(USBD_PRI_ORDER)
#define  USBAPI_PRI      TASK_PRIORITY(USBAPI_PRI_ORDER)
#define  USBEBI2_PRI     TASK_PRIORITY(USBEBI2_PRI_ORDER)

#endif

/*---------------------------------------------------------------------------

                     IxFile Server TASK DECLARATIONS

---------------------------------------------------------------------------*/

#ifdef FEATURE_IXFILESVC

extern rex_stack_word_type ix_filesvc_stack[ ];
extern rex_tcb_type        ix_filesvc_tcb;

#define IXFILESVC_PRI       TASK_PRIORITY(IXFILESVC_PRI_ORDER)

#define IXFILESVC_STACK_SIZ (1024*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

#endif /* FEATURE_IXFILESVC */

/*---------------------------------------------------------------------------

                     IxTIME Server TASK DECLARATIONS

---------------------------------------------------------------------------*/

#ifdef FEATURE_IXTIMESVC

extern rex_stack_word_type ix_timesvc_stack[ ];
extern rex_tcb_type        ix_timesvc_tcb;

#define IXTIMESVC_PRI       TASK_PRIORITY(IXTIMESVC_PRI_ORDER)

#define IXTIMESVC_STACK_SIZ (4096*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))
 
#endif

/*---------------------------------------------------------------------------

                     IxHTTP Server TASK DECLARATIONS

---------------------------------------------------------------------------*/

#ifdef FEATURE_IXHTTPSVC

extern rex_stack_word_type ix_httpsvc_stack[ ];
extern rex_tcb_type        ix_httpsvc_tcb;

#define IXHTTPSVC_PRI       TASK_PRIORITY(IXHTTPSVC_PRI_ORDER)

#define IXHTTPSVC_STACK_SIZ (4096*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

#endif /* FEATURE_IXHTTPSVC */

/*---------------------------------------------------------------------------

        IxContent Server and  & Client Handler TASK DECLARATIONS

---------------------------------------------------------------------------*/

#ifdef FEATURE_IXCONTENTSVC

extern rex_stack_word_type ix_contentsvc_stack[ ];
extern rex_tcb_type        ix_contentsvc_tcb;

#define IXCONTENTSVC_PRI       TASK_PRIORITY(IXCONTENTSVC_PRI_ORDER)

#define IXCONTENTSVC_STACK_SIZ (4096*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

extern rex_stack_word_type ix_contentclnt_stack[ ];
extern rex_tcb_type        ix_contentclnt_tcb;

#define IXCONTENTCLNT_PRI       TASK_PRIORITY(IXCONTENTCLNT_PRI_ORDER)

#define IXCONTENTCLNT_STACK_SIZ (4096*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

#endif /* FEATURE_IXCONTENTSVC */


/*---------------------------------------------------------------------------

        IxSQL Server and  & Client Handler TASK DECLARATIONS

---------------------------------------------------------------------------*/

#ifdef FEATURE_IXSQLSVC

extern rex_stack_word_type ix_sqlsvc_stack[ ];
extern rex_tcb_type        ix_sqlsvc_tcb;

#define IXSQLSVC_PRI       TASK_PRIORITY(IXSQLSVC_PRI_ORDER)

#define IXSQLSVC_STACK_SIZ (4096*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

extern rex_stack_word_type ix_sqlclnt_stack[ ];
extern rex_tcb_type        ix_sqlclnt_tcb;

#define IXSQLCLNT_PRI       TASK_PRIORITY(IXSQLCLNT_PRI_ORDER)

#define IXSQLCLNT_STACK_SIZ (4096*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

extern rex_stack_word_type ix_sqltest_stack[ ];
extern rex_tcb_type        ix_sqltest_tcb;

#define IXSQLTEST_PRI      TASK_PRIORITY(IXSQLTEST_PRI_ORDER)

#define IXSQLTEST_STACK_SIZ (4096*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

#ifdef FEATURE_IXDESCRAMBLERSVC

extern rex_stack_word_type ix_descramblersvc_stack[ ];
extern rex_tcb_type        ix_descramblersvc_tcb;

#define IXDESCRAMBLERSVC_PRI       TASK_PRIORITY(IXDESCRAMBLERSVC_PRI_ORDER)

#define IXDESCRAMBLERSVC_STACK_SIZ (4096*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

#endif /* FEATURE_IXDESCRAMBLERSVC */

extern rex_stack_word_type i_sqltest_stack[ ];
extern rex_tcb_type        i_sqltest_tcb;

#define ISQLTEST_PRI      TASK_PRIORITY(ISQLTEST_PRI_ORDER)

#define ISQLTEST_STACK_SIZ (4096*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

#endif /* FEATURE_IXSQLSVC */

/*---------------------------------------------------------------------------

                     Universal Broadcast Modem TASK DECLARATIONS

---------------------------------------------------------------------------*/

#ifdef FEATURE_UBM_L1

#define UBM_L1_PRI        TASK_PRIORITY(UBM_L1_PRI_ORDER)
#define UBM_L1_STACK_SIZ  (2100/sizeof(rex_stack_word_type))

extern rex_stack_word_type ubm_l1_stack[ ];
extern rex_tcb_type        ubm_l1_tcb;

#endif /* FEATURE_UBM_L1 */

#ifdef FEATURE_QDJTEST
/*---------------------------------------------------------------------------

                        QDJTEST TASK DECLARATIONS

---------------------------------------------------------------------------*/
#define  QDJTEST_PRI       TASK_PRIORITY(QDJTEST_PRI_ORDER)
#define  QDJTEST_STACK_SIZ (4096*2*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

extern rex_stack_word_type qdjtest_stack[ ];
extern rex_tcb_type        qdjtest_tcb;

#endif /* FEATURE_QDJTEST */

#ifdef FEATURE_QTV_STATIC_TASKS
/*---------------------------------------------------------------------------

                        QTV TASK DECLARATIONS

---------------------------------------------------------------------------*/
#define QTV_VIDEO_RENDERER_PRI \
  TASK_PRIORITY(MPEG4_PV_TASK4_PRI_ORDER)
#ifdef FEATURE_STACK_REDUCTION
#ifdef FEATURE_LOW_MEM_BIGFONT
#define QTV_VIDEO_RENDERER_STACK_SIZ \
  (2*1024*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))
#else
#define QTV_VIDEO_RENDERER_STACK_SIZ \
  (4*1024*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))
#endif
#else
#define QTV_VIDEO_RENDERER_STACK_SIZ \
  (8*1024*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))
#endif
extern rex_stack_word_type qtv_video_renderer_stack[ ];
extern rex_tcb_type        qtv_video_renderer_tcb;

#define QTV_AUDIO_PRI \
  TASK_PRIORITY(MPEG4_PV_TASK5_PRI_ORDER)
#ifdef FEATURE_STACK_REDUCTION
#ifdef FEATURE_LOW_MEM_BIGFONT
#define QTV_AUDIO_STACK_SIZ \
  (2*1024*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))
#else
 #define QTV_AUDIO_STACK_SIZ \
  (4*1024*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))
#endif
#else
#define QTV_AUDIO_STACK_SIZ \
  (8*1024*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))
#endif
extern rex_stack_word_type qtv_audio_stack[ ];
extern rex_tcb_type        qtv_audio_tcb;

#define QTV_TASK10_PRI \
  TASK_PRIORITY(MPEG4_PV_TASK10_PRI_ORDER)
#ifdef FEATURE_STACK_REDUCTION
#ifdef FEATURE_LOW_MEM_BIGFONT
#define QTV_TASK10_STACK_SIZ \
  (2*1024*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))
#else
#define QTV_TASK10_STACK_SIZ \
  (4*1024*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))
#endif
#else
#define QTV_TASK10_STACK_SIZ \
  (8*1024*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))
#endif
extern rex_stack_word_type qtv_task10_stack[ ];
extern rex_tcb_type        qtv_task10_tcb;

#endif /* FEATURE_QTV_STATIC_TASKS */

#ifdef FEATURE_QTV_STATIC_DLAGENT_TASK
#define QTV_DEC_DLAGENTTASK_PRI \
  TASK_PRIORITY(MPEG4_PV_TASK1_PRI_ORDER)
#ifdef FEATURE_STACK_REDUCTION
#define QTV_DEC_DLAGENTTASK_STACK_SIZ  \
  (1024*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))
#else
#define QTV_DEC_DLAGENTTASK_STACK_SIZ  \
  (8*1024*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))
#endif
extern rex_stack_word_type qtv_dec_dlAgenttask_stack[ ];
extern rex_tcb_type        qtv_dec_dlAgenttask_tcb;
#endif

#ifdef FEATURE_QTV_STATIC_DLDSPSVC_TASK
#define QTV_DEC_DLDSPSVCTASK_PRI \
  TASK_PRIORITY(QTV_VDL_DSP_SVC_TASK_PRI_ORDER)
#ifdef FEATURE_STACK_REDUCTION
#define QTV_DEC_DLDSPSVCTASK_STACK_SIZ  \
  (1024*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))
#else
#define QTV_DEC_DLDSPSVCTASK_STACK_SIZ  \
  (8*1024*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))
#endif
extern rex_stack_word_type qtv_dec_dlDspSvctask_stack[ ];
extern rex_tcb_type        qtv_dec_dlDspSvctask_tcb;
#endif

#ifdef FEATURE_SENSORS
/*---------------------------------------------------------------------------

                         SENSORS TASK DECLARATIONS

---------------------------------------------------------------------------*/
/* Rex priority for the Sensors task                                       */
#define SNS_PRI            TASK_PRIORITY(SNS_PRI_ORDER)

/* Size of Sensors Stack                                                   */
#define SNS_STACK_SIZ      (1024*6*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

extern rex_stack_word_type sns_stack[];
extern rex_tcb_type        sns_tcb;

#endif /* FEATURE_SENSORS */


#ifdef FEATURE_XO
/*---------------------------------------------------------------------------

                         TCXOMGR TASK DECLARATIONS

---------------------------------------------------------------------------*/
/* Rex priority for the TcxoMgr task                                       */
#define TCXOMGR_PRI            TASK_PRIORITY(TCXOMGR_PRI_ORDER)

/* Size of TcxoMgr Stack                                                   */
#define TCXOMGR_STACK_SIZ    (1024*4*STACK_SIZ_FACTOR/sizeof(rex_stack_word_type))

extern rex_stack_word_type tcxomgr_stack[];
extern rex_tcb_type        tcxomgr_tcb;

#endif /* FEATURE_XO */

#ifdef FEATURE_PWRDB
#error code not present
#endif /* FEATURE_PWRDB */

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
  TMC or MC task is running.

RETURN VALUE
  void

SIDE EFFECTS
  The calling task stays here until it receives the START signal.

===========================================================================*/
void task_start(

    rex_sigs_type    rpt_sig,        /* report timer signal for task */
    int              dog_rpt,        /* Watchdog report for the task */
    rex_timer_type   *rpt_timer_ptr  /* pointer to report timer */
);

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
void task_ack( void );

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
void task_stop( void );

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
void task_offline( void );

#endif /* TASK_H */


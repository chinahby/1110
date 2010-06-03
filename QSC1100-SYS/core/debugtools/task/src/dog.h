#ifndef DOG_H
#define DOG_H
/*===========================================================================

            W A T C H D O G    T A S K    H E A D E R    F I L E

DESCRIPTION
  This file contains external procedure declarations necessary for
  accessing procedures contained within the watchdog task file.

Copyright (c) 1990, 1992 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1993, 1994 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1995, 1998 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1999, 2000 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2001, 2002 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2003       by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc1100/services/task/dog.h#8 $ $DateTime: 2008/10/06 17:26:03 $ $Author: bcalder $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
06/27/08   APU	   Added support for FEATURE_VBATT_TASK.	
07/02/08   hs      Added support for FEATURE_CHG_TASK.
11/30/07   rjm     FEATURE_FM switch now independent of ODM builds
06/18/07   va      Added TCXO Manager Task
08/29/06   pc      Added support for FEATURE_EXT_USB_P1
08/14/06   pc      Added feature FEATURE_QTV_STATIC_TASKS.
08/09/06   pc      Added support for QTV_VIDEO_RENDERER and QTV_AUDIO
07/25/06   pc      Added support for FEATURE_CPU_BASED_FLOW_CONTROL
07/17/06   pc      Added support for FEATURE_DMTASK.
06/19/06   pc      Added support for FEATURE_SEC_USFS.
05/05/06   ac      Added support for FEATURE_QDJTEST
04/24/06   bt      Added support for UBM L1 task.
03/24/06   pc      Added support for SECRND task
03/23/06   cl      Added PDSM_COMM_TASK
01/23/06   rlp     Added HID Support.
01/02/06   dle     Added FEATURE_CAMERA_DRV_TASK.
11/28/05   wd      Added L4 dog opt.
11/10/05   pc      Added support for AVS_SBCENC_TASK
11/08/05   pc      Added support for GSM_PLT task
09/12/05   pc      Added support for FEATURE_USB_HOST_ENHANCED.
08/26/05   pc      Added support for DISP_TASK.
06/12/05   hba     Added support for WLAN Sc task.
04/21/05   pc      Added support for FEATURE_MFLO
04/20/05   pc      Added support for FEATURE_QIPCALL
04/09/05   pc      Added support for CNV task
03/15/05   wal     Added APS task for L4/Linux under FEATURE_APS
03/15/05   adg     Added QDIAG task for L4/Linux under FEATURE_IG_QDIAG
03/15/05   jqi     Added support for FS_FEATURE_RMTEFS_SERVER, FEATURE_L4
                   FEATURE_PM_SERVER,FEATURE_IG_UI_CLIENT_SELECT
02/24/05 t_jarmis  Added FEATURE_FM for FM-RDS Radio.
02/05/05   aku     Added support for WLAN_CP and WLAN_TRP tasks.
02/04/05   dwp     Added QVP support for SIP.
02/03/05   epr     Addition of Sirius specific tasks using define SIRIUS_PLATFORM
01/31/05   jqi     Revised FEATURE_MGPCC_TASK report ID
01/21/05   ldaita  Added support MGPCC task (needed for GPS).
01/06/05   aar     Added support for GPSSRCH demod task.
11/01/04   jz      Added QVP VFE Task
11/18/04   pa      Added support for PBM task.
10/22/04   jqi     Featurization fixes.
10/15/04   rp      Added support for page and interrupt tasks.
10/08/04   jqi     Added support for FEATURE_HSDPA
09/29/04   ss      Added support for DTV (Digital TV) feature
07/06/04   hl      Added support for joystick feature
05/19/04   vr      Added BCDB and HDRBC tasks
           jqi     Removed FEATURE_THIN_UI.
04/01/04   sg      removed support for LP task as task no longer exists.
02/23/04   balas   Added support for Bluetooth External Profiles feature support .
02/19/04   Jayanth Added support for FEATURE_SEC_DRM.
02/12/04   vdrapk  Added support for FEATURE_GSM_CB.
11/07/03   dd      Added GPS Search Task support.
10/13/03   dwp     Added initial QVPHONE support.
10/03/03   ajn     Removed MSM5500's HDR autokicker & related code
07/23/03    bs     Merged ThinUI changes from 6200 archive. This is featurized
                   as FEATURE_THIN_UI.
07/21/03   mk/dlr  Added dog_gsm_autokick
07/11/03    gr     Added a new signal to enable asynchronous filesystem access
                   from the Dog task.
05/29/03   Om      Added support for ARM_CRYPTO and IPSEC tasks.
05/28/03   TMR/dgy Added changes for WCDMA single-mode build.
05/14/03    ld     Added support for MGP related tasks
03/14/03   jqi     Added GSM/WCDMA feature around TMC task.
03/07/03    gr     Corrected a potential problem with the removal of the MS
                   task.
03/04/03    lz     Removed FEATURE_MOBILE_SUITE
02/21/03   jay     Added suuport for SSL task
02/18/03   jqi/jar Added support for the Generic SIM Toolkit Task (FEATURE_GSTK)
12/31/02   jqi     Merged with msmshared/umts/services/task/dog.h#39
                   1.Added support for FEATURE_GPRS.
11/29/02   lz      Added DOG_MS_RPT under FEATURE_MOBILE_SUITE.
10/18/02   ma, ao  Added FEATURE_GSM_PLT for Vocoder Dog report
08/18/02   jqi     Moved to MSMSHARED/SERVICES/TASK directory.
09/16/02   jqi     Merged with COMMON and MSMSHARED UMTS archive.

===========================================================================*/

#include "comdef.h"     /* Definitions for byte, word, etc. */
#ifdef CUST_H
   #include "customer.h"   /* Customer-specific definitions    */
#endif
#include "clk.h"        /* Definitions for CLK_MS_PER_TICK  */
#include "task.h"       /* Task definitions                 */

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* This signal is set when the dog_wakeup_timer expires.
*/
#define DOG_WAKEUP_TIMER_SIG    0x0800

/* Used to indicate to the Dog task that a filesystem operation is complete.
*/
#define DOG_FS_COMPLETE_SIG 0x400

/* Reserved signals
*/
#define DOG_RSVD_0200_SIG       0x0200
#define DOG_RSVD_0100_SIG       0x0100
#define DOG_RSVD_0080_SIG       0x0080
#define DOG_RSVD_0040_SIG       0x0040
#define DOG_RSVD_0020_SIG       0x0020
#define DOG_RSVD_0010_SIG       0x0010
#define DOG_RSVD_0008_SIG       0x0008
#define DOG_RSVD_0004_SIG       0x0004
#define DOG_RSVD_0002_SIG       0x0002
#define DOG_RSVD_0001_SIG       0x0001

/*----------------------------------------------------------------------------
    Bits to be set in Sleep Control Register (SLEEP_CTL)
----------------------------------------------------------------------------*/

/* To enable auto kick of watch dog
*/
#define SLEEP_CTL_SLEEP_CTLR_AUTOKICK_ENA_V     0x00000002

/* To kick watch dog
*/
#define SLEEP_CTL_WATCH_DOG_V                   0x00000001

/*------------------------------------------------------------------------*/

/* The following are periodically set by the other software tasks
** in order to prove that they are executing regularly.
** The value of each DOG_XXX_RPT constant must equal the index of the
** entry corresponding to that task in the dog_task_info array.
** For instance, DOG_RX_RPT should have the value 2, since the entry for
** the RX task is at index 2 in the dog_task_info array.
*/
#define DOG_SND_RPT     0         /* Sound Task                     */
#define DOG_TX_RPT      1         /* Transmit Task                  */
#define DOG_RX_RPT      2         /* Receive Task                   */
#define DOG_SRCH_RPT    3         /* Searcher Task                  */
#define DOG_RXTX_RPT    4         /* Layer 2 task                   */
#define DOG_MC_RPT      5         /* Main Control Task              */
#define DOG_HS_RPT      6         /* Handset Task                   */
#define DOG_DIAG_RPT    7         /* Diagnostic Task                */
#define DOG_UI_RPT      8         /* User Interface Task            */
#define DOG_NV_RPT      9         /* Non-volatile Database Task     */

/* IDS Task used to be here. It is obsolete. */

#ifdef FEATURE_AUTH
  #define DOG_AUTH_RPT 11        /* Authentication Task             */
#endif

#if (defined(FEATURE_DS) || defined(FEATURE_DATA))
  #define DOG_DS_RPT   12           /* Data Services Task           */
  #define DOG_PS_RPT   13           /* Data Protocol Services       */
  #ifdef FEATURE_DS_SOCKETS_TEST_TCP
#error code not present
  #endif
#endif

#ifdef T_SLEEP
  #define DOG_SLEEP_RPT 15          /* Sleep Task                   */
#endif

#ifdef FEATURE_QMIP
#error code not present
#endif

#ifdef FEATURE_DH
  #define DOG_DH_RPT 17             /* Diffe-Helman Task            */
#endif

#ifdef FEATURE_VS_TASK
  #define DOG_VS_RPT 18             /* Voice Services Task          */
#endif

#ifdef FEATURE_GRAPH_TASK
  #define DOG_GRAPH_RPT 19          /* Graphics Task                */
#endif

#ifdef FEATURE_EFS
  #define DOG_FS_RPT 20             /* Embedded File System Task    */
#endif

#ifdef FEATURE_CM
  #define DOG_CM_RPT 21             /* Call Manager Task            */
#endif

#if defined(FEATURE_VOC_TASK) || defined(FEATURE_HWTC) || defined(FEATURE_GSM_PLT)
  #define DOG_VOC_RPT 22            /* Vocoder Task                 */
#endif

#if defined(FEATURE_GPSONE) && !defined(FEATURE_CGPS)
#error code not present
#endif

#ifdef FEATURE_QDSP_TASK
  #define DOG_QDSP_RPT 28          /* QDSP Task                     */
#endif

#if defined (FEATURE_USB) && !defined (FEATURE_NO_USB_HW_ON_PROCESSOR)
  #define DOG_USBDC_RPT 29         /* USB Task                      */
#endif

#ifdef FEATURE_EFS_SFAT
  #define DOG_SFAT_RPT 30          /* SFAT Task                     */
#endif

#ifdef FEATURE_BT
  #define DOG_BT_RPT 31            /* Bluetooth Task                */
#endif

#ifdef FEATURE_IS2000_TDSO
  #define DOG_TDSO_RPT 32          /* IS2000 TDSO Task              */
#endif

#ifdef FEATURE_SEC
  #define DOG_SEC_RPT 33           /* Security Services Task        */
#endif

#ifdef FEATURE_UIM
  #define DOG_UIM_RPT 34           /* UIM task                      */
#endif

#ifdef FEATURE_HDR
#error code not present
#endif

#ifdef FEATURE_HWTC
#error code not present
#endif /* FEATURE_HWTC */

#if (defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS))
  #define DOG_WMS_RPT 40           /* WMS Task                      */
#endif

#if (defined(FEATURE_MULTIMODE_ARCH) || defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
  #define DOG_TMC_RPT     41       /* Task Main Control task        */
#endif

#ifdef FEATURE_MULTIMODE_ARCH
  #define DOG_MMOC_RPT    42       /* MMOC task                     */
#endif

#if (defined(FEATURE_MMGSDI) || defined(FEATURE_NAS) || defined(FEATURE_VIRTUAL_SIM))
  #define DOG_GSDI_RPT 43
#endif /* FEATURE_MMGSDI || FEATURE_NAS || FEATURE_SIM */

#ifdef FEATURE_MDSP_TASK
  #define DOG_MDSP_RPT 44           /* MDSP Services Task           */
#endif

#if ( (defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS)) && !defined(FEATURE_WCDMA_PLT) && !defined (FEATURE_GSM_PLT))
  #define DOG_GCSD_RPT 45             /* GSM GCSD task              */
#endif/* ((defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS)) && !defined(FEATURE_WCDMA_PLT)*/

#if (defined (FEATURE_GSM) && !defined (FEATURE_WCDMA_PLT) && !defined (FEATURE_GSM_PLT))
#error code not present
#endif /* FEATURE_GSM && FEATURE_WCDMA_PLT */

#ifdef FEATURE_WCDMA
#error code not present
#endif

#ifdef FEATURE_MMGPS
#error code not present
#endif

#ifdef FEATURE_NAS
#error code not present
#endif

#if ( defined(FEATURE_WCDMA_PLT) || defined(FEATURE_GSM_PLT) )
#error code not present
#elif defined(FEATURE_GSM_TEST_TASK)
#error code not present
#endif

#ifdef FEATURE_NAS
#error code not present
#endif

#if (defined (FEATURE_GSM) && !defined (FEATURE_WCDMA_PLT) && !defined (FEATURE_GSM_PLT))
#error code not present
#endif

#if (defined (FEATURE_WCDMA) && !defined (FEATURE_WCDMA_PLT) && !defined (FEATURE_GSM_PLT))
#error code not present
#endif

#ifdef FEATURE_NAS
#error code not present
#endif

#ifdef FEATURE_TC
  #define DOG_TC_RPT 64               /* Test Control Task          */
#endif

#ifdef FEATURE_GPSONE_STANDALONE
#error code not present
#endif /* FEATURE_GPSONE_STANDALONE */

#ifdef FEATURE_FTM_TASK
  #define DOG_FTM_RPT 66            /* Factory test mode task */
#endif

#ifdef FEATURE_GSTK
  #define DOG_GSTK_RPT 67           /* Generic SIM Toolkit task   */
#endif

#if defined(FEATURE_VIRTUAL_SAT) && defined(FEATURE_VIRTUAL_SIM)
  #define DOG_VIRTSAT_RPT 68
#endif

#ifdef FEATURE_SEC_SSL
  #define DOG_SECSSL_RPT 69         /* SSL protocol Task        */
#endif

#ifdef FEATURE_MGP
  #define DOG_MGPMC_RPT 70          /* MGPMC task               */
  #define DOG_MGPNF_RPT 71          /* MGPNF task               */
  #define DOG_MGPCD_RPT 72          /* MGPCD task               */
  #define DOG_MGPAI_RPT 73          /* MGPAI task               */
#endif

#ifdef FEATURE_PROGRESSIVE_BOOT
  #define DOG_INTERRUPT_RPT 74                /* Interrupt task               */
#endif

#ifdef FEATURE_SEC_ARM_CRYPTO
  #define DOG_SECCRYPTARM_RPT 75         /* Sec Svcs ARM crunching     */
#endif

#ifdef FEATURE_SEC_IPSEC
  #define DOG_SECIPS_RPT 76              /* IPSec (IKE)                */
#endif

#ifdef FEATURE_QVPHONE
  #define DOG_QVP_AE_RPT    77      /* QVP Audio Engine task              */
  #define DOG_QVPIO_RPT     78      /* QVP IO task                        */
  #define DOG_QVPPL_RPT     79      /* QVP Player task                    */
  #define DOG_QVP_APP_RPT   80      /* QVP App task                       */
  #define DOG_QVP_MPEG4_RPT 81      /* QVP MPEG4 task                     */
#endif /* FEATURE_QVPHONE */

#ifdef FEATURE_GPSSRCH_TASK
#error code not present
#endif

#ifdef FEATURE_GSM_CB
#error code not present
#endif /* #ifdef FEATURE_GSM_CB */

#ifdef FEATURE_SEC_DRM
#error code not present
#endif /* FEATURE_SEC_DRM */

#ifdef FEATURE_BT_EXTPF
  #define DOG_BT_PF_RPT 85          /* Bluetooth External Profile Task    */
#endif

#ifdef FEATURE_HDR_BCMCS
#error code not present
#endif

#ifdef FEATURE_BCMCS
#error code not present
#endif

#ifdef FEATURE_JOYST_TASK           /* Joystick Task */
   #define DOG_JOYST_RPT 88
#endif

#ifdef FEATURE_DTV
#error code not present
#endif /* FEATURE_DTV */

#if (defined (FEATURE_WCDMA) && defined (FEATURE_HSDPA))
#error code not present
#endif

#ifdef FEATURE_PBM_TASK
  #define DOG_PBM_RPT 91            /* PBM task */
#endif

#ifdef FEATURE_QVPHONE
  #define DOG_QVP_VFE_RPT 92       /* QVP VFE Task */
#endif

#ifdef FEATURE_MGP
  #ifdef FEATURE_MGPCC_TASK
  #define DOG_MGPCC_RPT 93          /* MGPCC task               */
  #endif
#endif

#ifdef SIRIUS_PLATFORM
#error code not present
#endif /* SIRIUS_PLATFORM */

#if (defined(FEATURE_QVPHONE) && defined(FEATURE_QVPHONE_SIP))
#error code not present
#endif

#ifdef FEATURE_WLAN
#error code not present
#endif /* FEATURE_WLAN */

#ifdef FEATURE_FM
  #define DOG_FMRDS_RPT 102         /* FM-RDS Task                    */
#endif

#ifdef FEATURE_IG_QDIAG             /* L4/Linux QDIAG task */
   #define DOG_QDIAG_RPT 103
#endif

#ifdef FEATURE_APS                  /* L4/Linux APS task */
   #define DOG_APS_RPT 104
#endif

#if defined(FS_FEATURE_RMTEFS_SERVER) && !defined(FEATURE_IG_EFS_EXT_SERVER)
   #define DOG_EFS_SRVR_RPT 105
#endif

#ifdef FEATURE_PM_SERVER
   #define DOG_PMS_RPT 106
#endif

#ifdef FEATURE_NV_CNV
  #define DOG_CNV_RPT   107       /* CNV Task */
#endif

#ifdef FEATURE_QIPCALL
#define DOG_QIPCALL_RPT  108
#endif

#ifdef FEATURE_MFLO
#error code not present
#endif

#ifdef FEATURE_WLAN
#error code not present
#endif

#ifdef FEATURE_DISP_TASK
  #define DOG_DISP_RPT 113       /* DISP Task */
#endif

#if defined(FEATURE_USB) && defined (FEATURE_USB_HOST_ENHANCED) 
  #define DOG_USBHC_URB_RPT 114       /* USBHC URB Task                   */
  #define DOG_USBHC_DEV_RPT 115       /* USBHC DEV Task                   */
#if defined(FEATURE_USB_HOST_TEST_ENHANCED) && \
    (defined(FEATURE_USB_HOST_MASS_STORAGE_TEST_ENHANCED) || \
     defined(FEATURE_USB_HOST_HID_TEST_ENHANCED))
  #define DOG_USBHC_TEST_RPT 116       /* USBHC TEST Task                   */
#endif
#endif

#ifdef FEATURE_AVS_SBCENC_TASK
  #define DOG_SBCENC_RPT   117
#endif

#ifdef FEATURE_CAMERA_DRV_TASK
  #define DOG_CAMERA_DRV_RPT 118      /* Camera Driver Task                */
#endif

#ifdef FEATURE_QDSP_RTOS
  #define DOG_ADSP_RTOS_RPT 119          /* ADSP RTOS Task                */
#endif

#ifdef FEATURE_ATI_GRAPHICS
  #define DOG_GRAPHICS_RPT 120      /* ATI Graphics Task            */
#endif

#ifdef FEATURE_IXHTTPSVC
  #define DOG_IXHTTPSVC_RPT 121
#endif

#ifdef FEATURE_ATS
  #define DOG_ATS_RPT 122
#endif

#ifdef FEATURE_COMP_TASK
  #define DOG_COMP_RPT   123
#endif

#if defined(FEATURE_GPSONE) && !defined(FEATURE_CGPS)
#error code not present
#endif                           /* PDSM Communication Task       */

#ifdef FEATURE_SECRND
  #define DOG_SECRND_RPT 125     /* SECRND task */
#endif

#ifdef FEATURE_UBM_L1
  #define DOG_UBM_L1_RPT 126
#endif /* FEATURE_UBM_L1 */

#ifdef FEATURE_QDJTEST
  #define DOG_QDJTEST_RPT   127
#endif

#ifdef FEATURE_SEC_USFS
  #define DOG_SFS_RPT 128     /* SFS task */
#endif

#ifdef FEATURE_DMTASK
   #define DOG_DMTASK_RPT 129  /* Device Management Task */
#endif

#ifdef FEATURE_CPU_BASED_FLOW_CONTROL
  #define DOG_FC_RPT    130
#endif

#ifdef FEATURE_QTV_STATIC_TASKS
  #define DOG_QTV_VIDEO_RENDERER_RPT 131  /* QTV video renderer task */
  #define DOG_QTV_AUDIO_RPT          132  /* QTV audio task */
#endif

#ifdef FEATURE_EXT_USB_P1
  #define DOG_USBEBI2_RPT   133     /* USBEBI2 Task */
  #define DOG_USBAPI_RPT    134     /* USBAPI Task */
  #define DOG_USBD_RPT      135     /* USBD Task */
#endif
#ifdef FEATURE_CGPS
#error code not present
#endif /* FEATURE_CGPS */

#ifdef FEATURE_XO
  #define DOG_TCXOMGR_RPT   146       /* TCXOMGR task     */
#endif

#ifdef FEATURE_HS_USB
  #define DOG_HSU_AL_MS_RPT   147
  #define DOG_HSU_AL_TASK_RPT 148
#endif

#ifdef FEATURE_EFS_FDI
  #define DOG_FDI_BG_RPT      149      /* FDI Background task          */
#endif

#ifdef FEATURE_QTV_STATIC_DLAGENT_TASK
  #define DOG_QTV_DLAGENTTASK_RPT 150  /* QTV Decoder task */
#endif

#ifdef FEATURE_QTV_STATIC_DLDSPSVC_TASK
  #define DOG_QTV_DLDSPSVCTASK_RPT 151  /* QTV general task */
#endif

#ifdef FEATURE_CHG_TASK
  #define DOG_CHG_RPT 152    /* chg task */
#endif

#ifdef FEATURE_VBATT_TASK
  #define DOG_VBATT_RPT 153    /* vbatt task */
#endif

/* The next task should have an ID = 154 */
/*------------------------------------------------------------------------*/

/* Convert 'dog time', which is clock intervals, to REX ticks,
** which are milliseconds.
*/
#ifdef  FEATURE_L4_OPT_DOG
#error code not present
#else
#define DOG_TO_REX_TIME(t)      ((t) * CLK_MS_PER_TICK)
#endif /* FEATURE_L4_OPT_DOG */

/* Some report times which overlap infrequently due to being prime.  The
** purpose of not overlapping is to minimize the number of tasks which
** become runnable at the same time due to their report timers expiring.
** Keeping this event infrequent helps to maintain minimum task execution
** delays.  Some randomization (1-2 clock intervals) as a result of the
** tasks starting sequentially.
**
** Note that for a safety margin, none of the primes selected for report
** times should be greater than approximately DOG_CHECK_TIME / 1.5
** Next primes are

                                                            
   709     719
   743     751    757     769    773    787

*/
#ifdef FEATURE_AVS_SBCENC_TASK 
  #define DOG_SBCENC_RPT_TIME     DOG_TO_REX_TIME( 463 )
#endif
#ifdef FEATURE_QVPHONE
  #define DOG_QVP_AE_RPT_TIME     DOG_TO_REX_TIME( 947 )
  #define DOG_QVPIO_RPT_TIME      DOG_TO_REX_TIME( 439 )
  #define DOG_QVPPL_RPT_TIME      DOG_TO_REX_TIME( 443 )
  #define DOG_QVP_MPEG4_RPT_TIME  DOG_TO_REX_TIME( 449 )
  #define DOG_QVP_APP_RPT_TIME    DOG_TO_REX_TIME( 829 )
  #define DOG_QVP_VFE_RPT_TIME    DOG_TO_REX_TIME( 587 )
#ifdef FEATURE_QVPHONE_SIP
#error code not present
#endif /* FEATURE_QVPHONE_SIP */
#endif /* FEATURE_QVPHONE */

#if (defined(FEATURE_MMGSDI) || defined(FEATURE_NAS) || defined(FEATURE_VIRTUAL_SIM))
#define DOG_GSDI_RPT_TIME      DOG_TO_REX_TIME( 349 )
#endif /* FEATURE_MMGSDI || FEATURE_NAS || FEATURE_VIRTUAL_SIM */

#ifdef FEATURE_ATI_GRAPHICS
#define DOG_GRAPHICS_RPT_TIME    DOG_TO_REX_TIME( 617 )
#endif /* FEATURE_ATI_GRAPHICS */

#ifdef FEATURE_GRAPH_TASK
#define DOG_GRAPH_RPT_TIME      DOG_TO_REX_TIME( 163 )
#endif /* FEATURE_GRAPH_TASK */


#ifdef FEATURE_CAMERA_DRV_TASK
#define DOG_CAMERA_DRV_RPT_TIME DOG_TO_REX_TIME( 881 )
#endif /* FEATURE_CAMERA_DRV_TASK */

#ifdef FEATURE_HDR
#error code not present
#endif

#ifdef FEATURE_MULTIMODE_ARCH
  #define DOG_MMOC_RPT_TIME       DOG_TO_REX_TIME( 241 )
#endif

#if (defined(FEATURE_MULTIMODE_ARCH) || defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
  #define DOG_TMC_RPT_TIME        DOG_TO_REX_TIME( 251 )
#endif

#ifdef FEATURE_UIM
  #define DOG_UIM_RPT_TIME        DOG_TO_REX_TIME( 223 )
#endif
#ifdef FEATURE_SEC
  #define DOG_SEC_RPT_TIME        DOG_TO_REX_TIME( 919 )
#ifdef FEATURE_SECRND
  #define DOG_SECRND_RPT_TIME     DOG_TO_REX_TIME( 857 )
#endif
#ifdef FEATURE_SEC_USFS
  #define DOG_SFS_RPT_TIME        DOG_TO_REX_TIME( 797 )
#endif
  #define DOG_SECCRYPTARM_RPT_TIME  DOG_TO_REX_TIME( 977 )
  #define DOG_SECIPS_RPT_TIME     DOG_TO_REX_TIME( 491 )
  #define DOG_SECSSL_RPT_TIME     DOG_TO_REX_TIME( 643 )
  #define DOG_SECDRM_RPT_TIME     DOG_TO_REX_TIME( 761 )
#endif
  #define DOG_SFAT_RPT_TIME       DOG_TO_REX_TIME( 211 )
#ifdef FEATURE_BT
  #define DOG_BT_RPT_TIME         DOG_TO_REX_TIME( 199 )
#endif
#ifdef FEATURE_BT_EXTPF
  #define DOG_BT_PF_RPT_TIME      DOG_TO_REX_TIME( 457 )
#endif
#ifdef FEATURE_QDSP_TASK
  #define DOG_QDSP_RPT_TIME       DOG_TO_REX_TIME( 281 )
#endif
#ifdef FEATURE_CM
  #define DOG_CM_RPT_TIME         DOG_TO_REX_TIME( 997 ) //HDTEMPO was 181
#endif
#if (defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS))
  #define DOG_WMS_RPT_TIME        DOG_TO_REX_TIME( 991 )
#endif
#if defined (FEATURE_USB) && !defined (FEATURE_NO_USB_HW_ON_PROCESSOR)
  #define DOG_USBDC_RPT_TIME      DOG_TO_REX_TIME( 193 )

  #ifdef FEATURE_USB_HOST_ENHANCED
    #define DOG_USBHC_URB_RPT_TIME DOG_TO_REX_TIME( 613 )
    #define DOG_USBHC_DEV_RPT_TIME DOG_TO_REX_TIME( 877 )
  #endif

  #if defined(FEATURE_USB_HOST_TEST_ENHANCED) && \
      (defined(FEATURE_USB_HOST_MASS_STORAGE_TEST_ENHANCED) || \
       defined(FEATURE_USB_HOST_HID_TEST_ENHANCED))
    #define DOG_USBHC_TEST_RPT_TIME DOG_TO_REX_TIME( 863 )
  #endif
#endif

#ifdef FEATURE_HWTC
#error code not present
#endif  /* FEATURE_HWTC */

#if defined(FEATURE_GPSONE) && !defined(FEATURE_CGPS)
#error code not present
#endif

#ifdef FEATURE_EFS
  #define DOG_FS_RPT_TIME         DOG_TO_REX_TIME( 191 )
#endif

#ifdef FEATURE_EFS_FDI
  #define DOG_FDI_BG_RPT_TIME         DOG_TO_REX_TIME( 709 )
#endif

#define DOG_VOC_RPT_TIME        DOG_TO_REX_TIME( 183 )
#define DOG_TCPTEST_RPT_TIME    DOG_TO_REX_TIME( 929 )
#define DOG_DH_RPT_TIME         DOG_TO_REX_TIME( 179 )
#define DOG_VS_RPT_TIME         DOG_TO_REX_TIME( 173 )
#define DOG_QMIP_RPT_TIME       DOG_TO_REX_TIME( 167 )
#define DOG_PD_RPT_TIME         DOG_TO_REX_TIME( 157 )
#define DOG_PS_RPT_TIME         DOG_TO_REX_TIME( 151 )

#ifdef FEATURE_WLAN
#error code not present
#endif  /* FEATURE_WLAN */

#define DOG_AUTH_RPT_TIME       DOG_TO_REX_TIME( 149 )
#define DOG_DS_RPT_TIME         DOG_TO_REX_TIME( 139 )
#ifdef FEATURE_COMP_TASK
  #define DOG_COMP_RPT_TIME       DOG_TO_REX_TIME( 659 )
#endif /* FEATURE_COMP_TASK */
#define DOG_DIAG_RPT_TIME       DOG_TO_REX_TIME( 137 )
#define DOG_HS_RPT_TIME         DOG_TO_REX_TIME( 131 )
#define DOG_MC_RPT_TIME         DOG_TO_REX_TIME( 127 )
#define DOG_NV_RPT_TIME         DOG_TO_REX_TIME( 113 )
#define DOG_RX_RPT_TIME         DOG_TO_REX_TIME( 109 )
#define DOG_RXTX_RPT_TIME       DOG_TO_REX_TIME( 107 )
#define DOG_SND_RPT_TIME        DOG_TO_REX_TIME( 103 )
#define DOG_SRCH_RPT_TIME       DOG_TO_REX_TIME( 101 )
#define DOG_TX_RPT_TIME         DOG_TO_REX_TIME(  97 )
#define DOG_UI_RPT_TIME         DOG_TO_REX_TIME( 983 )  // HDTEMPO was 89
#define DOG_SLEEP_RPT_TIME      DOG_TO_REX_TIME(  83 )
#define DOG_TDSO_RPT_TIME       DOG_TO_REX_TIME( 197 )

#ifdef FEATURE_MDSP_TASK
  #define DOG_MDSP_RPT_TIME     DOG_TO_REX_TIME( 499 )
#endif

#define DOG_GCSD_RPT_TIME       DOG_TO_REX_TIME( 257 ) /* GSM GCSD Task */
#define DOG_GSM_L1_RPT_TIME     DOG_TO_REX_TIME( 269 )
#define DOG_GSM_L2_RPT_TIME     DOG_TO_REX_TIME( 271 )
#define DOG_L1_RPT_TIME         DOG_TO_REX_TIME( 277 )
#define DOG_L2DL_RPT_TIME       DOG_TO_REX_TIME( 283 )
#define DOG_L2UL_RPT_TIME       DOG_TO_REX_TIME( 293 )
#define DOG_LSM_RPT_TIME        DOG_TO_REX_TIME( 307 )
#define DOG_MM_RPT_TIME         DOG_TO_REX_TIME( 311 )
#define DOG_MN_CNM_RPT_TIME     DOG_TO_REX_TIME( 313 )
#define DOG_PLT_RPT_TIME        DOG_TO_REX_TIME( 317 )
#define DOG_REG_RPT_TIME        DOG_TO_REX_TIME( 331 )
#define DOG_RR_RPT_TIME         DOG_TO_REX_TIME( 337 )
#define DOG_RRC_RPT_TIME        DOG_TO_REX_TIME( 347 )
#define DOG_SM_RPT_TIME         DOG_TO_REX_TIME( 353 )
#define DOG_TC_RPT_TIME         DOG_TO_REX_TIME( 359 )
#define DOG_TEST_RPT_TIME       DOG_TO_REX_TIME( 367 ) /* GSM TEST task */
#define DOG_GLLC_RPT_TIME       DOG_TO_REX_TIME( 263 )
#define DOG_GRLC_UL_RPT_TIME    DOG_TO_REX_TIME( 373 )
#define DOG_GRLC_DL_RPT_TIME    DOG_TO_REX_TIME( 379 )
#define DOG_GMAC_RPT_TIME       DOG_TO_REX_TIME( 383 )
#define DOG_FTM_RPT_TIME        DOG_TO_REX_TIME( 389 )
#define DOG_GSTK_RPT_TIME       DOG_TO_REX_TIME( 397 )
#define DOG_VIRTSAT_RPT_TIME    DOG_TO_REX_TIME( 401 )

#ifdef FEATURE_MGP
#define DOG_MGPMC_RPT_TIME      DOG_TO_REX_TIME( 409 )
#define DOG_MGPNF_RPT_TIME      DOG_TO_REX_TIME( 419 )
#define DOG_MGPCD_RPT_TIME      DOG_TO_REX_TIME( 421 )
#define DOG_MGPAI_RPT_TIME      DOG_TO_REX_TIME( 431 )
#ifdef FEATURE_MGPCC_TASK
#define DOG_MGPCC_RPT_TIME      DOG_TO_REX_TIME( 461 )
#endif
#endif

#ifdef FEATURE_CGPS
#error code not present
#endif /* FEATURE_CGPS */

#ifdef FEATURE_GSM_CB
#error code not present
#endif /* #ifdef FEATURE_GSM_CB */

#ifdef FEATURE_HDR_BCMCS
#error code not present
#endif

#ifdef FEATURE_BCMCS
#error code not present
#endif

#ifdef FEATURE_DTV
#error code not present
#endif /* FEATURE_DTV */

#ifdef FEATURE_JOYST_TASK
#define DOG_JOYST_RPT_TIME       DOG_TO_REX_TIME( 971 )
#endif


#ifdef FEATURE_HSDPA
#error code not present
#endif

#if( defined(FEATURE_PROGRESSIVE_BOOT) && !defined(FEATURE_TIMER_TASK) )
#define DOG_INTERRUPT_RPT_TIME  DOG_TO_REX_TIME(  433 )
#endif

#ifdef FEATURE_PBM_TASK
#define DOG_PBM_RPT_TIME        DOG_TO_REX_TIME( 911 )
#endif

#ifdef SIRIUS_PLATFORM
#error code not present
#endif /* SIRIUS_PLATFORM */

#ifdef FEATURE_FM
#define FMRDS_DOG_RPT_TIME     DOG_TO_REX_TIME( 569)
#endif

#ifdef FEATURE_IG_QDIAG             /* L4/Linux QDIAG task */
#define DOG_QDIAG_RPT_TIME      DOG_TO_REX_TIME( 593 )
#endif

#ifdef FEATURE_APS                  /* L4/Linux APS task */
#define DOG_APS_RPT_TIME        DOG_TO_REX_TIME( 599 )
#endif

#ifdef FEATURE_ATS                  /* L4/Linux ATS task */
#define DOG_ATS_RPT_TIME        DOG_TO_REX_TIME( 733 )
#endif

#if defined(FS_FEATURE_RMTEFS_SERVER) && !defined(FEATURE_IG_EFS_EXT_SERVER)
#define DOG_EFS_SRVR_RPT_TIME   DOG_TO_REX_TIME( 601 )
#endif

#ifdef FEATURE_PM_SERVER            /* L4/Linux PM Server task */
#define DOG_PMS_RPT_TIME        DOG_TO_REX_TIME( 607 )
#endif

#ifdef FEATURE_NV_CNV
#define DOG_CNV_RPT_TIME        DOG_TO_REX_TIME( 577 )
#endif

#ifdef FEATURE_DMTASK
#define DOG_DMTASK_RPT_TIME     DOG_TO_REX_TIME( 631 )
#endif

#ifdef FEATURE_QIPCALL
#define DOG_QIPCALL_RPT_TIME  DOG_TO_REX_TIME(571)
#endif

#ifdef FEATURE_DISP_TASK
#define DOG_DISP_RPT_TIME        DOG_TO_REX_TIME( 887 )
#endif

#ifdef FEATURE_MFLO
#error code not present
#endif

#ifdef FEATURE_CPU_BASED_FLOW_CONTROL
#define DOG_FC_RPT_TIME  DOG_TO_REX_TIME( 727 )
#endif

#ifdef FEATURE_QDSP_RTOS
#define DOG_ADSP_RTOS_RPT_TIME  DOG_TO_REX_TIME( 839 )
#endif

#ifdef FEATURE_IXHTTPSVC
#define DOG_IXHTTPSVC_RPT_TIME DOG_TO_REX_TIME( 859 )
#endif

#ifdef FEATURE_UBM_L1
  #define DOG_UBM_L1_RPT_TIME   DOG_TO_REX_TIME( 739 )
#endif /* FEATURE_UBM_L1 */

#ifdef FEATURE_QDJTEST
  #define DOG_QDJTEST_RPT_TIME   DOG_TO_REX_TIME(619)
#endif

#ifdef FEATURE_EXT_USB_P1
  #define  DOG_USBEBI2_RPT_TIME  DOG_TO_REX_TIME(653)
  #define  DOG_USBAPI_RPT_TIME   DOG_TO_REX_TIME(673)
  #define  DOG_USBD_RPT_TIME     DOG_TO_REX_TIME(677)
#endif

#ifdef FEATURE_QTV_STATIC_TASKS
  #define DOG_QTV_VIDEO_RENDERER_RPT_TIME DOG_TO_REX_TIME(641)
  #define DOG_QTV_AUDIO_RPT_TIME          DOG_TO_REX_TIME(647)
#endif

#ifdef FEATURE_XO
  #define DOG_TCXOMGR_RPT_TIME          DOG_TO_REX_TIME(647)
#endif

#ifdef FEATURE_QTV_STATIC_DLAGENT_TASK
  #define DOG_QTV_DLAGENTTASK_RPT_TIME       DOG_TO_REX_TIME(719)
#endif

#ifdef FEATURE_QTV_STATIC_DLDSPSVC_TASK
  #define   DOG_QTV_DLDSPSVCTASK_RPT_TIME        DOG_TO_REX_TIME(709)
#endif

#ifdef FEATURE_HS_USB
  #define DOG_HSU_AL_MS_RPT_TIME   DOG_TO_REX_TIME( 773 )
  #define DOG_HSU_AL_TASK_RPT_TIME  DOG_TO_REX_TIME( 787 )
#endif

#ifdef FEATURE_CHG_TASK
  #define DOG_CHG_RPT_TIME       DOG_TO_REX_TIME(500) 
#endif

/* Note: 997 is a prime number */
#ifdef FEATURE_VBATT_TASK
  #define DOG_VBATT_RPT_TIME       DOG_TO_REX_TIME( 997 )
#endif
/*------------------------------------------------------------------------*/

/* Typedef added to preserve the signature of dog_report(), which is an
** exported function.
*/
typedef unsigned int dog_report_type;

#ifdef FEATURE_MONITOR_DOG
typedef enum {
  DOG_RESET_MONITOR,
  DOG_TURN_OFF_MONITOR
} dog_monitor_state;
#endif

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

FUNCTION DOG_REPORT

DESCRIPTION
  Report to the watchdog task, so that it can tell that the monitored
  task is still functioning properly.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void dog_report
(
  dog_report_type report
    /* Tell the watchdog task which task is reporting */
);


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
(
  boolean enable
);


/*===========================================================================

FUNCTION DOG_AUTOKICK

DESCRIPTION
  This procedure enables/disables the auto-kick feature.  Auto-kick
  is used when the microprocessor is expected to be asleep.

DEPENDENCIES
  This should only be called when the sleep timer has been programmed
  or when the processor wakes up after the sleep timer expires.

RETURN VALUE
  None

SIDE EFFECTS
  When enabled, pauses the watchdog's wakeup timer.  The timer resumes
  when autokick is disabled via this functions.

===========================================================================*/
extern void dog_autokick
(
  boolean enable
);



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
  the circuit will reset the system.

===========================================================================*/
extern void dog_kick( void );


/*===========================================================================

FUNCTION DOG_TASK

DESCRIPTION
  This procedure contains the main processing loop for the watchdog task.
  It checks that each task has reported in during that task's report
  interval. If a task has not reported, dog_task calls the ERR_FATAL macro
  to log the error and reset the microprocessor.

DEPENDENCIES
  This procedure should be called by rex_def_task.  Each task must report
  in at least once each task report interval, which is defined in
  dog_task_info[].

RETURN VALUE
  None, this procedure does not return.

SIDE EFFECTS
  The microprocessor may be reset during this task.

===========================================================================*/
extern void dog_task
(
  dword param
    /* The least significant rex_sigs_type portion of this dword is
    ** a mask of task report signals to be monitored.
    */
);

/*===========================================================================

FUNCTION DOG_MONITOR_PAUSE

DESCRIPTION
   Pauses the monitoring of a specified task.

===========================================================================*/
extern void dog_monitor_pause(
   unsigned int report_id
);

/*===========================================================================

FUNCTION DOG_MONITOR_RESUME

DESCRIPTION
   Resumes the monitoring of a specified task.

===========================================================================*/
extern void dog_monitor_resume(
   unsigned int report_id
);

/*===========================================================================

FUNCTION DOG_SET_STARTUP_TIME

DESCRIPTION
   Tells Dog how long to wait in milliseconds before starting to monitor
   tasks.  Use with care: calling this function to set the startup time to
   a large number will mean that malfunctions may get caught later than usual.
===========================================================================*/
void dog_set_startup_time(
   uint32 startup_time
);

#ifdef FEATURE_MONITOR_DOG
/*===========================================================================

FUNCTION DOG_SETUP_MONITOR_TIMER

DESCRIPTION
   Sets up a timer to monitor whether the dog task is getting a chance to
   run or not. This timer is reset everytime the dog task runs and when
   the dog task does not get to run for a fixed period of time, an error
   fatal is called.
===========================================================================*/
void dog_setup_monitor_timer(
   dog_monitor_state state
);
#endif

#endif  /* DOG_H */


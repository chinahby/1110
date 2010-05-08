#ifndef TIMETEST_H
#define TIMETEST_H
/*===========================================================================

           T I M E T E S T   H E A D E R    F I L E

DESCRIPTION
  This header file contains all the definitions necessary for supporting
  timing tests via the LED output buffer.

Copyright (c) 1993-2006 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/





/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc6055/services/utils/timetest.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/16/06   pc      Added TIMETEST_PDSM_COMM_TASK_ID
05/16/06   cc      Added  TIMETEST_TRAMP_BM_ISR and TIMETEST_TRAMP_TSIF_ISR
03/27/06   pc      Added TIMETEST_SECRND_TASK_ID
01/02/06   dle     Added CAMERA_DRV_TASK ID same as GRAPH_TASK ID.
02/04/05   lyr     Added timetest ID for WLAN CP task
           hba     Added timetest ID for WLAN Transport task.
           dw      Added timetest ID for QVP_RTP_TASK, QVP_SIPT TASK.
01/06/05   aar     Added TIMETEST_GPSSRCH_DEMOD_TASK_ID for GPS demod task.
04-12-09   bwo     L4 modifications
01/05/05   rp      Added support for background loader task for Progressive
                   Boot.
11/22/04   pa      Added TIMETEST_PBM_TASK_ID for PBM task.
09/29/04   ss      Added support for TIMETEST_DTV_TASK_ID
09/21/04   rp      Added Task IDS Progressive boot tasks
05/19/04   jqi     Merged timetest task id from msmshared archive.
12/10/03    ss     Added timetest IDs for MPEG4 tasks
08/08/03   kvd     Modified TIMETEST_PORT and TIMETEST_DSRLP_BYTES macro to
                   support CPU and RLP throughput profiling on Jaguar
04/04/02    vr     Added profiling support for ONCRPC test app
08/14/01    om     Changed TIMETEST_DSRLP_ macro to distinguish 1x and HDR.
08/02/01    om     Added PPS value for 510x.
07/25/01    om     Added support for RLP Rx/Tx statistics.
07/16/01    om     Added task specific profiling support.
03/07/01   rmd     Updated file to support MSM510x surf.
02/13/99    ms     Removed pre-MSM2 definitions. Added defs for MSM3.
11/25/97   jjn     Instead of defining TIMETEST_PORT as 0x03E just for
                   Gemini, defined TIMETEST_PORT as 0x03E for all targets
                   that use MSM 2 family CDMA chip
04/04/95   jah     Added Gemini support
12/09/93   jah     Added comments
05/27/93   jah     Added Beta II support
02/16/93   jah     Initial revision.

===========================================================================*/

/*===========================================================================

                      TIMETEST CONFIGURATION

===========================================================================*/
#include "customer.h"
#ifdef FEATURE_L4_TIMETEST
#error code not present
#else /* FEATURE_L4_TIMETEST */

#ifdef TIMETEST

  /* Multiple task / ISR profiling */
  #define TIMETEST_PROF

  /* Function level profiling */
  #undef TIMETEST_FUNCS

  /* Function level profiling only (no tasks / ISRs) */
  #undef TIMETEST_NO_TASKS

  /* Only count total ISR time, not individual ISR routines */
  #define TIMETEST_NO_ISRS

  #define TIMETEST_DSRLP_STATS

  #define TIMETEST_EXPLICIT_TRIGGER

#endif /* TIMETEST */
#endif /* FEATURE_L4_TIMETEST */


/*===========================================================================

                           INCLUDES

===========================================================================*/

#include "target.h"

#if (defined TIMETEST_FUNCS) && (! defined REXARM_S)
  #include "msg.h"
  #include "err.h"
  #include "assert.h"
#endif


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* Define the address of the buffer which drives the outputs
** hooked to the profiling mux.
*/

#ifdef FEATURE_L4_TIMETEST
#error code not present
#else /* FEATURE_L4_TIMETEST */

#ifdef CPLD_HAS_LED_DBG

  /* TIMETEST Port address
  */
  #define TIMETEST_PORT  CPLD_LED_DEBUG_WH

#elif defined(T_MSM2) && !defined (T_MSM3)
  #ifdef T_MSM5200
#error code not present
  #else
    /* Gemini Portable*/
    #define TIMETEST_PORT           0x03E
  #endif

#else

  /* MSM3000 and up targets */

  /* TIMETEST Port address - this is specific to 6500 hardware */
  #define TIMETEST_PORT             0x30000000
  #define TIMETEST_DSRLP_STATS
#endif /*CPLD_HAS_LED_DEBUG IS DEFINED*/
#endif /* FEATURE_L4_TIMETEST           */

/* 16-bit value used as a unique parse token for the postprocessor */
#define TT_PARSE_TOKEN            0xffff
#define TT_PARSE_TOKEN_ISR_CONT   0x8000
#define TT_PARSE_TOKEN_ISR_LEVEL  0x0f00
#define TT_PARSE_TOKEN_IS856      0x4000
#define TT_PARSE_TOKEN_IS2K       0x2000

/* Values used to distinguish between 1x and HDR data rates */
#define TIMETEST_DSRLP_IS856   TT_PARSE_TOKEN_IS856
#define TIMETEST_DSRLP_IS2K    TT_PARSE_TOKEN_IS2K

/* Task number definitions for timetest */


/* Unique task id definitions for timetest
 *
 * Note: A task id of zero should not be used.
 *       This will prevent task tcbs which have not had
 *       their tcb.leds field properly initialized from
 *       being profiled as task 0.
 */

#define TIMETEST_AUTH_TASK_ID                0x01
#define TIMETEST_BT_TASK_ID                  0x02
#define TIMETEST_CM_TASK_ID                  0x03
#define TIMETEST_DH_TASK_ID                  0x04
#define TIMETEST_DIAG_TASK_ID                0x05
#define TIMETEST_DOG_TASK_ID                 0x06
#define TIMETEST_DS_TASK_ID                  0x07
#define TIMETEST_FS_TASK_ID                  0x08
#define TIMETEST_HS_TASK_ID                  0x09
#define TIMETEST_MC_TASK_ID                  0x0a
#define TIMETEST_NV_TASK_ID                  0x0b
#define TIMETEST_PS_TASK_ID                  0x0c
#define TIMETEST_QDSP_TASK_ID                0x0d
#define TIMETEST_QMIP_TASK_ID                0x0e
#define TIMETEST_RXTX_TASK_ID                0x0f
#define TIMETEST_RX_TASK_ID                  0x10
#define TIMETEST_SEC_TASK_ID                 0x11
#define TIMETEST_SECTESTAPP_TASK_ID          0x12
#define TIMETEST_SFAT_TASK_ID                0x13
#define TIMETEST_SLEEP_TASK_ID               0x14
#define TIMETEST_SND_TASK_ID                 0x15
#define TIMETEST_SRCH_TASK_ID                0x16
#define TIMETEST_TCPTEST_TASK_ID             0x17
#define TIMETEST_TX_TASK_ID                  0x18
#define TIMETEST_UDPTEST_TASK_ID             0x19
#define TIMETEST_UI_TASK_ID                  0x1a
#define TIMETEST_USBDC_TASK_ID               0x1b
#define TIMETEST_VOC_TASK_ID                 0x1c
#define TIMETEST_VS_TASK_ID                  0x1d
#define TIMETEST_HDR_MC_TASK_ID              0x1e
#define TIMETEST_HDR_SRCH_TASK_ID            0x1f
#define TIMETEST_HDR_RX_TASK_ID              0x20
#define TIMETEST_HDR_TX_TASK_ID              0x21
#define TIMETEST_EFS_TASK_ID                 0x22
#define TIMETEST_VST_TASK_ID                 0x23
#define TIMETEST_UIM_TASK_ID                 0x24
#define TIMETEST_GSDI_UIM_TASK_ID            0x25

/* #define TIMETEST_L4_SYSCALL_msm  		     0x04E0 
 * just pick up where l4 left off... 
*/
#define TIMETEST_FS_COMPAT_TASK_ID           0x0500
#define TIMETEST_FS_HOTPLUG_TASK_ID          0x0501


/* We should rename the WCDMA Timetest port IDs to the following names:

  NAS_MN_CNM
  NAS_GSDI
  NAS_MM
  NAS_REG
  NAS_SM

  WCDMA ONLY TASKS
  WCDMA_RRC
  WCDMA_RLC
  WCDMA_MAC
  WCDMA_L1
  WCDMA_L2_UL
  WCDMA_L2_DL
*/

#define TIMETEST_L1_TASK_ID                  0x26
#define TIMETEST_L2_UL_TASK_ID               0x27
#define TIMETEST_L2_DL_TASK_ID               0x28
#define TIMETEST_RRC_TASK_ID                 0x29
#define TIMETEST_AL1_TASK_ID                 0x2a
#define TIMETEST_MM_TASK_ID                  0x2b
#define TIMETEST_REG_TASK_ID                 0x2c
#define TIMETEST_MN_CNM_TASK_ID              0x2d
#define TIMETEST_SM_TASK_ID                  0x2e
#define TIMETEST_GSDI_TASK_ID                0x2f
#define TIMETEST_MENU_UT_TASK_ID             0x30
#define TIMETEST_RRC_UT_TASK_ID              0x31
#define TIMETEST_WPLT_TASK_ID                0x32
#define TIMETEST_DS_CS_UL_TASK_ID            0x33
#define TIMETEST_DS_CS_DL_TASK_ID            0x34
#define TIMETEST_GCSD_TASK_ID                0x35

#define TIMETEST_GSM_L1_TASK_ID              0x36
#define TIMETEST_GSM_L2_TASK_ID              0x37
#define TIMETEST_GSM_RR_TASK_ID              0x38

#define TIMETEST_INTLOCK                     0x39
#define TIMETEST_INTFREE                     0x3a

#define TIMETEST_WMS_TASK_ID                 0x3b
#define TIMETEST_MDSP_TASK_ID                0x3c
#define TIMETEST_ONCRPC_TASK_ID              0x3d
#define TIMETEST_STA_TASK_ID                 0x3e
#define TIMETEST_TC_TASK_ID                  0x3f

#define TIMETEST_BT_HSDEMO_TASK_ID           0x40
#define TIMETEST_BT_RC2SIO_TASK_ID           0x41
#define TIMETEST_BT_LPOS_TASK_ID             0x42

/*
  The ONCRPC STA creates new REX tasks in the
  phone at runtime. These are assigned task ids
  in sta_sched_server.c when they are created.
  Since the STA can create upto 4 tasks during
  runtime, reserve 4 ids for them here.

  The task id dynamically assigned to the STA
  tasks in sta_sched_server.c will be
  TIMETEST_STA_FORK_TASK_ID + task_num
  where task num can be 0, 1, 2 or 3.

  Make sure that the hp_stats.ini file used for
  profiling has these defined correctly for
  accurate profiling results.
*/
#define TIMETEST_STA_FORK_TASK_ID            0x43
#define TIMETEST_STA_RESERVED_0              TIMETEST_STA_FORK_TASK_ID
#define TIMETEST_STA_RESERVED_1              0x44
#define TIMETEST_STA_RESERVED_2              0x45
#define TIMETEST_STA_RESERVED_3              0x46

#define TIMETEST_TMC_TASK_ID                 0x47
#define TIMETEST_TDSO_TASK_ID                0x48
#define TIMETEST_IDS_TASK_ID                 0x49
#define TIMETEST_MMOC_TASK_ID                0x4a
#define TIMETEST_PDSM_TASK_ID                0x4b
#define TIMETEST_PDSMSCLIENT_TASK_ID         0x4c
#define TIMETEST_PB_TASK_ID                  0x4d
#define TIMETEST_CD_TASK_ID                  0x4e
#define TIMETEST_NF_TASK_ID                  0x4f

#define TIMETEST_BCKGROUND_LOADER_TASK_ID    0x50
#define TIMETEST_GRAPH_TASK_ID               0x51
#define TIMETEST_CAMERA_DRV_TASK_ID          TIMETEST_GRAPH_TASK_ID
#define TIMETEST_LSM_TASK_ID                 0x52

#define TIMETEST_GSM_GPRS_MAC_TASK_ID        0x53
#define TIMETEST_GSM_GPRS_RLC_UL_TASK_ID     0x54
#define TIMETEST_GSM_GPRS_RLC_DL_TASK_ID     0x55
#define TIMETEST_GSM_GPRS_LLC_TASK_ID        0x56

#define TIMETEST_GSTK_TASK_ID                0x57
#define TIMETEST_FTM_TASK_ID                 0x58
#define TIMETEST_SECSSL_TASK_ID              0x59

/* MGP related tasks */
#define TIMETEST_MGPMC_TASK_ID               0x5a
#define TIMETEST_MGPNF_TASK_ID               0x5b
#define TIMETEST_MGPCD_TASK_ID               0x5c
#define TIMETEST_MGPAI_TASK_ID               0x5d

#define TIMETEST_VIRTSAT_TASK_ID             0x5e
#define TIMETEST_SECCRYPTARM_TASK_ID         0x5f
#define TIMETEST_SECIPSEC_TASK_ID            0x60

/* QVPHONE related tasks */
#define TIMETEST_QVP_AUDIO_TASK_ID           0x61
#define TIMETEST_QVP_MPEG4_TASK_ID           0x62
#define TIMETEST_QVP_APP_TASK_ID             0x63
#define TIMETEST_QVP_IO_TASK_ID              0x64
#define TIMETEST_QVP_PLAYER_TASK_ID          0x65
#define TIMETEST_GPSSRCH_TASK_ID             0x66
/*GSM Cell broadcasting task */
#define TIMETEST_GSM_CB_TASK_ID              0x67
/* DRM task */
#define TIMETEST_SECDRM_TASK_ID              0x68
/* Bluetooth External Profiles feature support */
#define TIMETEST_BT_EXTPF_TASK_ID            0x69
#define TIMETEST_BT_EXTPF_TEST_TASK_ID       0x6a

/* BCMCS tasks */
#define TIMETEST_HDR_BCASTPROT_TASK_ID       0x6b
#define TIMETEST_BCDB_TASK_ID                0x6c
#define TIMETEST_JOYST_TASK_ID               0x6d
/* Digital TV task */
#define TIMETEST_DTV_TASK_ID                 0x6e

/* WLAN related tasks */
#define TIMETEST_WLAN_TRP_TASK_ID            0x6F
#ifdef FEATURE_IG_QDIAG
#define TIMETEST_QDIAG_TASK_ID               0x70
#endif /* FEATURE_IG_QDIAG */

#define TIMETEST_WLAN_CP_TASK_ID             0x71
/* Timer task */
#define TIMETEST_TIMER_TASK_ID               0x72
/* PBM task */
#define TIMETEST_PBM_TASK_ID                 0x73
#define TIMETEST_QVP_RTP_TASK_ID             0x74
#define TIMETEST_QVP_SIPT_TASK_ID            0x75
/* QVP VFE */
#define TIMETEST_QVP_VFE_TASK_ID             0x76
/* GPSSRCH demod task */
#define TIMETEST_GPSSRCH_DEMOD_TASK_ID       0x77

#define TIMETEST_PMS_TASK_ID                 0x78
#define TIMETEST_EFS_SRVR_TASK_ID            0x79
#define TIMETEST_APS_TASK_ID                 0x7a

#define TIMETEST_MAC_HS_DL_TASK_ID           0x7b
#define TIMETEST_MGPCC_TASK_ID               0x7c

/* The following are rex tasks */
#define TIMETEST_DPC_TASK_ID                 0x7d
#define TIMETEST_REX_TASK_ID                 0x7e
#define TIMETEST_MAX_TASK_ID                 0x7f

#define TIMETEST_INT_TASK_ID                 TIMETEST_MAX_TASK_ID

#define TIMETEST_TRAMP_AD_TSHK_ISR             0x80
#define TIMETEST_TRAMP_SEC_ISR                 0x81
#define TIMETEST_TRAMP_MPM_WAKEUP_ISR          0x82
#define TIMETEST_TRAMP_GPIO2_GROUP_ISR         0x83
#define TIMETEST_TRAMP_MPM_GPIO_GROUP_ISR      0x84
#define TIMETEST_TRAMP_CHIP_SBI_ISR            0x85
#define TIMETEST_TRAMP_MDDI_HOST_ISR           0x86
#define TIMETEST_TRAMP_MDDI_CLIENT_ISR         0x87
#define TIMETEST_TRAMP_USB_ISR                 0x88
#define TIMETEST_TRAMP_ADSP_ISR                0x89
#ifndef T_QSC6055
#define TIMETEST_TRAMP_GRAPHICS_ISR            0x8a
#else
#define TIMETEST_TRAMP_PM_ISR                  0x8a
#endif
#define TIMETEST_TRAMP_MDP_ISR                 0x8b
#define TIMETEST_TRAMP_NAND_ISR                0x8c
#define TIMETEST_TRAMP_MPM_KEYSENSE_GROUP_ISR  0x8d
#define TIMETEST_TRAMP_I2C_ISR                 0x8e
#define TIMETEST_TRAMP_UART_ALL_ISR            0x8f
#define TIMETEST_TRAMP_AUX_CODEC_ONES_ISR      0x90
#define TIMETEST_TRAMP_SDCC_MMCC_ISR           0x91
#define TIMETEST_TRAMP_TIME_TICK_ISR           0x92
#define TIMETEST_TRAMP_SLEEP_TICK_ISR          0x93
#define TIMETEST_TRAMP_SLEEP_TIMER0_ISR        0x94
#define TIMETEST_TRAMP_SLEP_TICK2_ISR          0x95
#define TIMETEST_TRAMP_SYS_ONLINE_1X_ISR       0x96
#define TIMETEST_TRAMP_SLEEP_END_HDR_ISR       0x97
#define TIMETEST_TRAMP_GSM_SLEEP_ISR           0x98
#define TIMETEST_TRAMP_SBI_SSBI_ISR            0x99
#define TIMETEST_TRAMP_BT_WAKEUP_ISR           0x9a
#define TIMETEST_TRAMP_GSM_MICRO_ISR           0x9b
#define TIMETEST_TRAMP_GSM_GSAC1_ISR           0x9c
#define TIMETEST_TRAMP_GSM_GSAC0_ISR           0x9d
#define TIMETEST_TRAMP_MDM_MARM_ISR            0x9e
#define TIMETEST_TRAMP_MDSP_ISR                0x9f
#define TIMETEST_TRAMP_SLEEP_FEE_ISR           0xa0
#define TIMETEST_TRAMP_SLEEP_INT_HDR_ISR       0xa1
#define TIMETEST_TRAMP_SLEEP_INT_1X_ISR        0xa2
#define TIMETEST_TRAMP_RTC_ROLL_ISR            0xa3
#define TIMETEST_TRAMP_OFFLINE_DONE_ISR        0xa4
#define TIMETEST_TRAMP_RX_QP_ISR               0xa5
#define TIMETEST_TRAMP_RX_ISR                  0xa6
#define TIMETEST_TRAMP_MOD_TX_FR_ISR           0xa7
#define TIMETEST_TRAMP_MMGPS_HOST_ISR          0xa8
#define TIMETEST_TRAMP_BACKUP_ISR              0xa9
#define TIMETEST_TRAMP_MOD_TX_SLOT_ISR         0xaa
#define TIMETEST_TRAMP_SYS_TIME_INT2_ISR       0xab
#define TIMETEST_TRAMP_SYS_TIME_INT1_ISR       0xac
#define TIMETEST_TRAMP_PNROLL_ISR              0xad
#define TIMETEST_TRAMP_HMOD_EVEN_SEC_ISR       0xae
#define TIMETEST_TRAMP_RTC_COMPARE_1_ISR       0xaf
#define TIMETEST_TRAMP_RTC_COMPARE_2_ISR       0xb0
#define TIMETEST_TRAMP_SYS_TIME_GP_COMPARE_ISR 0xb1
#define TIMETEST_TRAMP_PN_ROLLOVER_1X_ISR      0xb2
#define TIMETEST_TRAMP_PN_ROLLOVER_HDR_ISR     0xb3
#define TIMETEST_TRAMP_125_MS_ISR              0xb4
#define TIMETEST_TRAMP_PER_SLOT_ISR            0xb5
#define TIMETEST_TRAMP_HDR_FRAME_ISR           0xb6
#define TIMETEST_TRAMP_1X_TX_FR_ISR            0xb7
#define TIMETEST_TRAMP_SVD_DONE_ISR            0xb8
#define TIMETEST_TRAMP_TD_DONE_ISR             0xb9
#define TIMETEST_TRAMP_ADSP_0_ISR              0xba
#define TIMETEST_TRAMP_ADSP_1_ISR              0xbb
#define TIMETEST_TRAMP_ADSP_2_ISR              0xbc
#define TIMETEST_TRAMP_UART1_ISR               0xbd
#define TIMETEST_TRAMP_UART2_ISR               0xbe
#define TIMETEST_TRAMP_UART3_ISR               0xbf
#define TIMETEST_TRAMP_DP_RX_DATA1_ISR         0xc0
#define TIMETEST_TRAMP_DP_RX_DATA2_ISR         0xc1
#define TIMETEST_TRAMP_DP_RX_DATA3_ISR         0xc2
#define TIMETEST_TRAMP_SBI_0_ISR               0xc3
#define TIMETEST_TRAMP_SBI_1_ISR               0xc4
#define TIMETEST_TRAMP_SSBI_0_ISR              0xc5
#define TIMETEST_TRAMP_SSBI_1_ISR              0xc6
#define TIMETEST_TRAMP_SSBI_2_ISR              0xc7
#define TIMETEST_TRAMP_CHIP_SBI_0_ISR          0xc8
#define TIMETEST_TRAMP_CHIP_SSBI5_ISR          0xc9
#define TIMETEST_TRAMP_CHIP_SSBI4_ISR          0xca
#define TIMETEST_TRAMP_CHIP_SSBI2_ISR          0xcb
#define TIMETEST_TRAMP_HSSD_ISR                0xcc
#define TIMETEST_TRAMP_PENIRQB_ISR             0xcd
#define TIMETEST_TRAMP_EOC_ISR                 0xce
#define TIMETEST_TRAMP_NAND_WR_ERR_DONE_ISR    0xcf
#define TIMETEST_TRAMP_NAND_OP_DONE_ISR        0xd0
#define TIMETEST_TRAMP_MMCC_ERR_ISR            0xd1
#define TIMETEST_TRAMP_SDCC_1_ISR              0xd2
#define TIMETEST_TRAMP_SDCC_0_ISR              0xd3
#define TIMETEST_TRAMP_KEY_SENSE_ISR           0xd4
#define TIMETEST_TRAMP_WAKEUP_ISR              0xd5
#define TIMETEST_QIPCALL_TASK_ID               0xd6
#define TIMETEST_TRAMP_BM_ISR                  0xd7
#define TIMETEST_TRAMP_TSIF_ISR                0xd8
#define TIMETEST_CC_TASK_ID                    0xd9
#define TIMETEST_PP_TASK_ID                    0xda
#define TIMETEST_GPSFFT_TASK_ID                0xdb
#define TIMETEST_LM_TASK_ID                    0xdc
#define TIMETEST_SM_TM_TASK_ID                 0xdd



#define TIMETEST_SECRND_TASK_ID                0xe5

#define TIMETEST_PDSM_COMM_TASK_ID             0xf0
#define TIMETEST_DISP_TASK_ID                  0xf1
/* WLAN TASK ID */
#define TIMETEST_WLAN_SEC_TASK_ID              0xf2


/* MPEG4 timetest defines */
#define TIMETEST_MP4_TASK_ID                 0xf3

#define TIMETEST_MP4_PV_FIRST_TASK_ID        0xf4  /* base PV task     */
#define TIMETEST_MP4_PV_PARSER_ID            0xf4  /* ensure sync with */
#define TIMETEST_MP4_PV_CPVTEXT_ID           0xf5  /* zrex2rex.c and   */
#define TIMETEST_MP4_PV_VIDEO_ID             0xf6  /* qtvplayer.h      */
#define TIMETEST_MP4_PV_RENDERER_ID          0xf7
#define TIMETEST_MP4_PV_STREAMER_ID          0xf8
#define TIMETEST_MP4_PV_AUDIO_ID             0xf9
#define TIMETEST_MP4_PV_PLAYER_ID            0xfa
#define TIMETEST_MP4_PV_DSPTASK_ID           0xfb
#define TIMETEST_MP4_PV_LAST_TASK_ID         0xfb /* last of PV tasks */


#define TIMETEST_INTERRUPT_TASK_ID           0xfc /* interrpt task*/
#define TIMETEST_PAGER_TASK_ID               0xfd /* pager task*/
#define TIMETEST_BACKGROUND_LOADER_TASK_ID   0xfe /* background loader task*/

#define TIMETEST_ISR_NESTED_ID               0xff

/*** New timetest for function and ISR profiling ***/
#ifdef TIMETEST_PROF

  /* No foward declarations in assembly code ! */
  #ifndef REXARM_S
    extern unsigned char timetest_isr_markers[32];
    extern unsigned char timetest_isr_level;
    extern unsigned long timetest_parse_token;
    extern void timetest_prof_init( void );
  #endif

  /* Macro to write an ISR's ID to the LED port */
  #ifndef TIMETEST_NO_ISRS
    #ifdef TIMETEST_EXPLICIT_TRIGGER
      #define TIMETEST_ISR_ID( id )                                        \
        INTLOCK();                                                         \
        timetest_isr_markers[timetest_isr_level] = id;                     \
        outpw (TIMETEST_PORT, (uint16)(0x0000) );                          \
        outpw( TIMETEST_PORT,                                              \
               (uint16)(((timetest_isr_level & 0x0f) << 8) | id) );        \
        INTFREE();
    #else
        #define TIMETEST_ISR_ID( id )                                      \
        INTLOCK();                                                         \
        timetest_isr_markers[timetest_isr_level] = id;                     \
        outpw( TIMETEST_PORT,                                              \
               (uint16)(((timetest_isr_level & 0x0f) << 8) | id) );        \
        INTFREE();
    #endif
  #else
    #define TIMETEST_ISR_ID( id )
  #endif

  /* Value on the buffer output when in the Interrupt Handler */
  #define TIMETEST_INT   TIMETEST_MAX_TASK_ID

  /* Macro to write RLP rx and tx statistics out */
  /* This macro has been modified specific for 6500 */
  #ifdef TIMETEST_EXPLICIT_TRIGGER
    #define TIMETEST_DSRLP_BYTES( rlp_type, rxb, txb )                      \
      INTLOCK();                                                            \
      outpw( TIMETEST_PORT, (uint16)(0x0000) );                             \
      outpw( TIMETEST_PORT, (uint16)(rlp_type) );                           \
      outpw( TIMETEST_PORT, (uint16)(0x0000) );                             \
      outpw( TIMETEST_PORT, ((((uint16)(rxb)) << 8) | txb ));               \
      INTFREE();
  #else
    #define TIMETEST_DSRLP_BYTES( rlp_type, rxb, txb )                      \
      INTLOCK();                                                            \
      outpw( TIMETEST_PORT, (uint16)(rlp_type) );                           \
      outpw( TIMETEST_PORT, ((((uint16)(rxb)) << 8) | txb ));               \
      INTFREE();
  #endif

  /* Macros to write the function address upon beginning and   */
  /* end of a function body.                                   */
  #ifdef TIMETEST_FUNCS

  /* This is dependent on the assembly code in rexarm.s  */
  /* Don't change it !!                                  */
  #define TIMETEST_MAX_FUNC_LEVEL     31

  #ifndef TIMETEST_NO_TASKS
    #define TT_OUT_FADDR( tid, f_addr )   \
      outpw( TIMETEST_PORT, (uint16)(0x0000) );                               \
      outpw( TIMETEST_PORT, (uint16)(TT_PARSE_TOKEN) );                       \
      outpw( TIMETEST_PORT, (uint16)(0x0000) );                               \
      outpw( TIMETEST_PORT, (uint16)(tid) );                                  \
      outpw( TIMETEST_PORT, (uint16)(0x0000) );                               \
      outpw( TIMETEST_PORT, (uint16)((((uint32)f_addr) & 0xffff0000) >> 16) );\
      outpw( TIMETEST_PORT, (uint16)(0x0000) ); \
      outpw( TIMETEST_PORT, (uint16)(((uint32)f_addr) & 0x0000ffff) );
  #else /* TIMETEST_NO_TASKS */
    #define TT_OUT_FADDR( tid, f_addr )   \
      outpw( TIMETEST_PORT, (uint16)(0x0000) );                               \
      outpw( TIMETEST_PORT, (uint16)(TT_PARSE_TOKEN) );                       \
      outpw( TIMETEST_PORT, (uint16)(0x0000) );                               \
      outpw( TIMETEST_PORT, (uint16)((((uint32)f_addr) & 0xffff0000) >> 16) );\
      outpw( TIMETEST_PORT, (uint16)(0x0000) ); \
      outpw( TIMETEST_PORT, (uint16)(((uint32)f_addr) & 0x0000ffff) );
  #endif /* TIMETEST_NO_TASKS */

  #define TT_BEGIN_FUNC( f_addr )  \
    { INTLOCK();                                                              \
    if ( ! rex_self()->leds ) ERR("LEDs not set for task %x",rex_self(),0,0); \
    else {                                                                    \
    ASSERT( timetest_markers[rex_self()->leds][0] < 31 );                     \
    timetest_markers[rex_self()->leds][0]++;                                  \
    timetest_markers[rex_self()->leds][timetest_markers[rex_self()->leds][0]] \
      = (uint32) f_addr;                                                      \
    TT_OUT_FADDR( rex_self()->leds, f_addr );                                 \
    } INTFREE(); }

  #define TT_END_FUNC( f_addr )    \
    { INTLOCK();                                                              \
    if ( rex_self()->leds && timetest_markers[rex_self()->leds][0] ){         \
    timetest_markers[rex_self()->leds][timetest_markers[rex_self()->leds][0]] \
      = 0x00;                                                                 \
    timetest_markers[rex_self()->leds][0]--;                                  \
    TT_OUT_FADDR( rex_self()->leds,                                           \
                  timetest_markers[rex_self()->leds]                          \
                                  [timetest_markers[rex_self()->leds][0]] );  \
    } INTFREE(); }

  #ifndef REXARM_S
    /* Array to store markers during context switches */
    extern unsigned long timetest_markers[TIMETEST_MAX_TASK_ID]
                                         [TIMETEST_MAX_FUNC_LEVEL+1];

    extern void timetest_prof_init( void );
  #endif

  #endif /* (defined TIMETEST_FUNCS) */

#else  /* (defined TIMETEST_PROF) */

  /*** Legacy support for the old timetest ***/
  #define TIMETEST_ISR_ID( id )
  #define TIMETEST_INT    0x1f
  #if defined(T_MB_SURF_510X)
    #define TIMETEST_1PPS   0x20
  #else
    #define TIMETEST_1PPS   0x1E
  #endif

#endif /* TIMETEST_PROF */

#endif  /* TIMETEST_H */




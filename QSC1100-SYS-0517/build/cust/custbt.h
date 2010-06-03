#ifndef CUSTBT_H
#define CUSTBT_H
/*===========================================================================

            " C u s t - B T "   H E A D E R   F I L E

DESCRIPTION
  Configuration for Bluetooth Feature.

Copyright (c) 2001-2008 by QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order. Please
  use ISO format for dates.

  $Header: //source/qcom/qct/wconnect/bthost/brew/target/rel/00.00.26/build/cust/custbt.h#5 $
  $DateTime: 2009/04/28 13:42:34 $
  $Author: c_vikask $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
2009-27-04   vk    Enabling QSoC Powerdown Feature on 76XX.
2009-02-04   vk    Enabling PSVT call support on SC2X
2009-01-13   vk    Adding Flags for FTM.
2009-01-12   sp    Remove FEATURE_BT_AUTOMATIC_SWITCHER feature for all targets
2008-12-29   sa    Enabling proper clk regime client when FEATURE_BT_DCVS feature 
                   is defined on a target.
2008-12-03   vk    Defining FEATURE_BT_ENABLE_SLEEP_VOTING for 76xx & 7601 3.2 targets.
2008-12-04   ap    Remove IBT_DYNAMIC from Q6270
2008-11-25   gb    Removed feature flag for BMP(FEATURE_IBT_DYNAMIC_BMP_1_0)
2008-11-17   vk    Added feature to support BMP1.0.
2008-11-12   sa    Added feature to support NVM automation.
2008-10-13   sa    Remove IBT_DYNAMIC from QSC1110
2008-09-01   ap    Enable DCVS for 6270
2008-09-25   jn    Added support for MSM7601 3.2 baseline.
2008-09-01   gs    Added BT SIO escape code detection feature for 7500 JCDMA.
2008-08-20   jn    Added 19P2 and clock sharing features for QSC1110. Add
                   460 feature for MSM6800.
2008-08-20   rb    Enable DCVS for 6290
2008-08-18   jn    Added support for MSM6800. Enabled WLAN Coex for MSM76XX,
                   SC2X, MSM7501_90, MSM6290/46 and MSM6800.
2008-07-30   NM    Enabled IBT_DYNAMIC feature for 76XX targets
2008-07-25   sa    Added support for voting against shutting BT 32K sleep clk.
2008-07-16  sjp    Added support for MSM7501_90. Disable QSOC_POWERDOWN for
                   MSM6246/90 SURF builds.
2008-06-27  dsn    Enabling IBT_DYNAMIC for 6246/90
2008-06-20   sa    Enable sleep and UART baud of 460.8k for  ULC
2008-06-17   rb    Enable BT_QSOC_POWERDOWN for 6290/46
2008-06-02   rs    Added support for MSM7601.
2008-05-06   rs    Enable FEATURE_BT_ALIGN_SIO_DATA,
                   FEATURE_BT_SOC_BITRATE_115200 and
                   FEATURE_BT_USES_SIO_GENERIC_MODE for QSC1110
2008-04-02  dsn    Enable QSOC_POWERDOWN and disable AUTOMATIC_SWITCHER for
                   QSC6270.
2008-04-02  dsn    Adding support for QSC1110.
2008-03-13  dsn    Disable QSOC_POWERDOWN for 6246/90. Enabled QSOC_SLEEP for
                   QSC6270.
2008-03-10  dsn     Target specific tags to conditionally undefine features..
02/08/08    ap     - Enable BT 2.1 (Lisbon) feature set
                   - Enable Double buffering and Reduce memory features for 3.3
01/21/08    ap     Disable Double buffering and Reduce memory features for 3.3.
                   The fix has still not been pulled in latest SU
01/21/08    ap     Disable FEATURE_BT_QSOC_GPIO_WAKE when in-band sleep is enabled.
01/21/08    ap     Move Inband sleep feature to .builds file
01/17/08    ap     Enable Inband sleep and DBL_BUF_WRITE and MEMORY_USAGE
                   for rel 3.3
12/18/07    ap     Disabling DBL_BUF_WRITE and MEMORY_USAGE CR 134869
10/25/07    ap     Featurized Host HCI Flow control under rel 3.2
10/19/07   ssk     Enable Ctlr to Host HCI Flow control feature for rel 3.0
10/01/07    ap     Enable Scatternet, Throughput, and Memory optimization
                   functionality for rel 3.2
09/12/07    bh     Rolled back 3.x features and added QSOC powerdown.
07/20/07    ap     Enable memory optimization feature in rel 3.0
07/09/07    ap     Enable Throughput optimization functionality in rel 3.0
07/07/07    ap     Enable WLAN coex and scatternet functionality in rel 3.0
06/01/07    ap     Disable scatternet support on SC2X.
04/09/07    bh     Added sleep support for BTS4020.
03/08/07    jn     Disabled feature SOC1 and enabled feature QSOC.
02/06/07   dsn     Adding SIO sleep feature, removing superfluous BT features.
02/05/07   dsn     Features for SOC1 support on QSC6055.
01/15/07    pg     Enabling features for SOC1 ROM, Flash parts support.
11/17/06   dsn     Enabling H4DS and BCCMDs for SURF
11/03/06   dsn     Enabling feature for SOC1 support for FFA
08/25/06   dsn     Enabling features for SOC solution.
12/14/05   dsn     Disabling FEATURE_BT_NA, FEATURE_BT_AG temporarily.
12/02/05   dsn     Adding featurization for APPS processor.
06/28/05   ptm     Remove // comments for L4 merge.
09/11/01   nxb     Define FEATURE_BT_NEW_DSMS
03/15/01   brm     Created initial version.
===========================================================================*/


/**************************************************************************
 *                         Bluetooth
 *  All featurizations supported by bluetooth are added here. Features that
 * are not applicable to a target or features that are specific only to a
 * target should be added in the target specific sections that follow this
 * section.
***************************************************************************/

#ifdef FEATURE_BT
  /* define FTM have BT */
  #define FTM_HAS_BT

  /* Enable the use of Bluetooth spec 1.2*/
  #define FEATURE_BT_1_2

  /* BT enable usage of NEW DSM items */
  #define FEATURE_BT_NEW_DSMS

  /*  Enable the Bluetooth Network Access Feature */
  #define FEATURE_BT_NA

  /*  Enable the Bluetooth Audio Gateway Application.  */
  #define FEATURE_BT_AG

  /*  Enable the Bluetooth Hands-Free 1.5 Application.  */
  #define FEATURE_BT_HFP_1_5

  /*  Enable "SCO rework" feature */
  #define FEATURE_AVS_BT_SCO_REWORK

  /*  Enable the Bluetooth Hands-Free Application.  */
  #define FEATURE_BT_HF

  /*  Enable the new Bluetooth SPP interface for RFCOMM.  */
  #define FEATURE_BT_SPP

  /*  Enable the Bluetooth Driver debug MSG macros.  */
  #define FEATURE_BT_MSG_DEBUG

  /*  Disable the Bluetooth Headset Application. (QC only)  */
  #undef FEATURE_BT_HS

  /*  Disable BT debug features. (QC only)  */
  #undef FEATURE_BT_ICON

  /*  Disable setting the local BD address through the UI. (QC only)  */
  #undef FEATURE_BT_MY_BD_ADDR

  /*  Disable the SDP test feature (removes test SDP service records).  */
  #undef FEATURE_BT_TEST_SDP

  /*  Disable the RC 2 SIO test application. (QC only)  */
  #undef FEATURE_BT_TEST_RC_2_SIO

  /*  Disable the Bluetooth API test. (QC only)  */
  /*  defined in msm6100.builds for STA builds */
  /* #define FEATURE_BT_TEST_API */

  /*  Disable BT debug features. (QC only)  */
  #undef FEATURE_BT_DEBUG

  /* Bluetooth Device Discovery */
  #define FEATURE_BT_DEVICE_DISCOVERY

  #ifdef FEATURE_BT_TEST_API
#error code not present
  #endif

  #ifdef FEATURE_BREW
    #define FEATURE_IBT
    #define FEATURE_IBT_DYNAMIC    
  #endif

    /* BT FTP/OPP Double Buffer Throughput Enhancement */
    #define FEATURE_BT_OBEX_DBL_BUF_WRITE

    /* Reduce memory used by BT profiles */
    #define FEATURE_BT_REDUCE_MEMORY_USAGE

  #ifdef FEATURE_QVPHONE
    #define FEATURE_BT_VT
  #endif

    /* Feature to disable BT Scatternet support */
    #undef FEATURE_BT_HOST_DIS_SCATTERNET

  /* Feature to support BT Host Controller on external SOC */
  #define FEATURE_BT_SOC

  #ifdef FEATURE_BT_SOC
    /* Feature to support UART HCI Transport layer with SOC */
    #define FEATURE_BT_SOC_UART_TL

    /* Feature for HCI Controller to Host flow control support */
    #define FEATURE_BT_HCI_HOST_FCTL

    /* Support for DCVS */
    #define FEATURE_BT_DCVS

    /* Feature to support BTS */
    #define FEATURE_BT_QSOC

    #ifdef FEATURE_BT_QSOC
      /* QSoC Sleep Switch */
      #define FEATURE_BT_QSOC_SLEEP

      #ifdef FEATURE_BT_QSOC_SLEEP
        #ifndef FEATURE_BT_QSOC_INBAND_SLEEP
          /* For BTS4020 wake-sleep signaling */
          #define FEATURE_BT_QSOC_GPIO_WAKE
        #endif /* FEATURE_BT_QSOC_INBAND_SLEEP */
          /* For debuging only */
          #define FEATURE_BT_QSOC_SLEEP_TRACE
      #endif /* FEATURE_BT_QSOC_SLEEP */

      /* Enable BT QSOC Powerdown */
      #define FEATURE_BT_QSOC_POWERDOWN

      /* BT 2.1 (Lisbon) feature set */
      #define FEATURE_BT_2_1

      /* WLAN Coexistence feature */
      #define FEATURE_BT_WLAN_COEXISTENCE

    #endif /* FEATURE_BT_QSOC */

    /* Feature to control UART sleep from BT */
    #define FEATURE_SIO_CLIENT_SLEEP_CONTROL

  #endif /* FEATURE_BT_SOC */


/****************************************************************
 * TARGET SPECIFIC FEATURIZATIONS
 * Featurizations below are specific to each target. Please add
 * or remove features specific to a target.
****************************************************************/
/****************
 * SC2X
****************/
#ifdef BT_SC2X
#error code not present
#endif /* BT_SC2X */

/****************
 * 6246 AND 6290
****************/
#if defined( BT_MSM6246 ) || defined( BT_MSM6290 )
#error code not present
#endif /* BT_MSM6246 || BT_MSM6290 */

/*************
 * QSC 6270
 * QSC6240 and QSC6270 are same targets.
*************/
#ifdef BT_QSC6270
#error code not present
#endif /* BT_QSC6270*/

/*************
 * QSC 1110
*************/
#ifdef BT_QSC1110
    #ifdef FEATURE_BT_DCVS
      #define FEATURE_BT_USE_CLKRGM_BT_SERVICES
    #endif
    #define FEATURE_BT_SOC_BITRATE_460800
    #define FEATURE_BT_19P2_SYSTEM_CLOCK
    #define FEATURE_BT_SYSTEM_CLOCK_SHARING

    /* Feature to define use of SIO DM Lite */
    #define FEATURE_BT_USES_SIO_DM_LITE

    #ifdef FEATURE_BT_USES_SIO_DM_LITE

      /* Enables memory alignement of DSM data prior to SIO Tx */
      #define FEATURE_BT_ALIGN_SIO_DATA

      /* Changes SIO mode to Generic Mode */
      #define FEATURE_BT_USES_SIO_GENERIC_MODE

    #endif /* FEATURE_BT_USES_SIO_DM_LITE */

    /* Changes to vote ON/OFF 32K sleep clk */
    #define FEATURE_BT_VOTING_32K_SLEEP_CLOCK

    #ifdef FEATURE_BT_WLAN_COEXISTENCE
      #undef FEATURE_BT_WLAN_COEXISTENCE
    #endif /* FEATURE_BT_WLAN_COEXISTENCE */

    #undef FEATURE_IBT_DYNAMIC

#endif /* BT_QSC1110 */

/*****************************************************************************
 * MSM7501A and MSM7501_90 are two different targets, hence two separate
 * macros. There are differences in HW blocks as well as process technology,
 * one is 65nm other is 90nm.
 *****************************************************************************/
/*************
 * MSM7601, MSM7501 & MSM7201
*************/
#if ( defined( BT_MSM7601 ) || defined( BT_MSM7501 ) ||  defined( BT_MSM7201 ) )
#error code not present
#endif /* BT_MSM7601 || BT_MSM7501 || BT_MSM7201 */

/*************
 * MSM7601 3.2 baseline
*************/
#ifdef BT_MSM7601_3_2
#error code not present
#endif /* BT_MSM7601_3_2  */

/*****************************************************************************
 * MSM7501A and MSM7501_90 are two different targets, hence two separate
 * macros. There are differences in HW blocks as well as process technology,
 * one is 65nm other is 90nm.
 *****************************************************************************/
/*************
 * MSM7501_90
*************/
#ifdef BT_MSM7501_90
#error code not present
#endif /* BT_MSM7501_90 */

/*************
 * MSM6800
*************/
#ifdef BT_MSM6800
#error code not present
#endif /* MSM6800 */

#endif /* FEATURE_BT */

#endif /* CUSTBT_H */


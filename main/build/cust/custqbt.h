#ifndef CUSTQBT_H
#define CUSTQBT_H
/*===========================================================================

            " C u s t - Q B T "   H E A D E R   F I L E

DESCRIPTION
  Configuration for Bluetooth Value-Added Features.

Copyright (c) 2006-2008, QUALCOMM Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary


===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/brew/target/rel/00.00.26/build/cust/custqbt.h#1 $ $DateTime: 2009/01/13 17:39:17 $ $Author: deepikas $

when        who     what, where, why
--------    ---     ----------------------------------------------------------
2008-09-25   jn     Added support for MSM7601 3.2 baseline
2008-08-23   sa     Remove FEATURE_IOBEX for QSC1110.
2008-08-18   jn     Adding support for MSM6800.
2008-07-16  sjp     Disabling SAP and HID for MSM7501_90
2008-04-02  dsn     Disabling SAP for 7601 and 7501.
2008-04-02  dsn     Adding support for QSC1110.
2008-03-10  dsn     Unified file for SC2X,6246,6290 support.
2007-02-05  dsn     Initial version for QSC6055.
2006-12-18  dsn     Enabling FEATURE_BT_L2FLUSH for AV-Sync.
2006-11-17  dsn     Enabling features for AV-Sync
2006-11-03  dsn     Enabling A2DP
2006-08-25  dsn     Initial version for SOC solution.

===========================================================================*/


/*---------------------------------------------------------------------------
                          Bluetooth
---------------------------------------------------------------------------*/

#ifdef FEATURE_BT

  /* Bluetooth Value-Added Profiles */
  #define FEATURE_BT_EXTPF
  #define FEATURE_BT_EXTPF_GOEP
  //Gemsea Remove #define FEATURE_BT_EXTPF_BIP
  //Gemsea Remove #define FEATURE_BT_EXTPF_BPP
  #define FEATURE_BT_EXTPF_FTP
  #define FEATURE_BT_EXTPF_OPP
  //Gemsea Remove #define FEATURE_BT_EXTPF_SML
  //Gemsea Remove #define FEATURE_BT_EXTPF_CTP
  //Gemsea Remove #define FEATURE_BT_EXTPF_PBAP
  #define FEATURE_BT_EXTPF_AV
  //Gemsea Remove #define FEATURE_BT_EXTPF_SAP
  //Gemsea Remove #define FEATURE_BT_EXTPF_HID_HOST
 

  #ifdef FEATURE_BT_EXTPF_GOEP
    #define FEATURE_MT_OBEX
    #ifdef FEATURE_BREW
      #define FEATURE_IOBEX
    #endif /* FEATURE_BREW */
  #endif /* FEATURE_BT_EXTPF_GOEP */
  
  #ifdef FEATURE_BT_EXTPF_AV
    #define FEATURE_BT_AVSYNC
    #define FEATURE_BT_L2FLUSH
    #define FEATURE_SBC_DSP_CODEC
    #define FEATURE_ARM_POST_PROCESSING
    #define FEATURE_BT_AVSYNC_SOC
    #define FEATURE_AVS_A2DP_AVSYNC
    #define FEATURE_BT_AVRCP_13 
  #endif /* FEATURE_BT_EXTPF_AV */

/*****************************************************************************
 * MSM7501A and MSM7501_90 are two different targets, hence two separate 
 * macros. There are differences in HW blocks as well as process technology, 
 * one is 65nm other is 90nm.
 *****************************************************************************/
/*************
 * MSM7601 & MSM7501
*************/
  #if defined( BT_MSM7601 ) || defined( BT_MSM7501 )
#error code not present
  #endif /* if defined( BT_MSM7601 ) || defined( BT_MSM7501 ) */

/*************
 * MSM7601 3.2 baseline
*************/
  #ifdef BT_MSM7601_3_2
#error code not present
  #endif /* BT_MSM7601_3_2 */

/*************
 * QSC 60x5
*************/
  #ifdef BT_SC2X
#error code not present
  #endif /* BT_SC2X */

/*************
 * QSC 1110
*************/
  #ifdef BT_QSC1110
    #undef FEATURE_BT_EXTPF_SAP
    #undef FEATURE_BT_EXTPF_HID_HOST
    #undef FEATURE_IOBEX
  #endif /* BT_QSC1110 */

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
  #endif /* BT_MSM6800 */

#endif /* FEATURE_BT */

#endif /* CUSTQBT_H */


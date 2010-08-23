#ifndef CUSTCDMA2000_H
#define CUSTCDMA2000_H
/*===========================================================================

            " C D M A 2 0 0 0 "   H E A D E R   F I L E

DESCRIPTION
  Configuration for IS-856 support on MSM6800 targets.

  Copyright (c) 2001, 2002, 2003
                2004, 2005, 2006 by QUALCOMM Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/plfs/cdma2000/2H07/build/cust/custcdma2000.h#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/12/08   hrk     Undefined FEATURE_MULTI_RX_CHAIN for QSC1100/1110
05/23/06   dnn     Created the file. Added FEATURE_IS683_OTASP for MM 

===========================================================================*/

/*===========================================================================

                         PUBLIC DATA DECLARATIONS

===========================================================================*/
/**************************************************/
/*                      CDMA                      */
/**************************************************/

#define FEATURE_MULTIMODE_RFAPI

#if defined (T_QSC1100) || defined (T_QSC1110)
  #undef FEATURE_MULTI_RX_CHAIN
#else
  #define FEATURE_MULTI_RX_CHAIN
#endif

#define FEATURE_MMOC_SUPPORTS_HYBRID
/* Radio resource arbitration.
*/
#define FEATURE_SYSTEM_ARBITRATION
#define FEATURE_PLCM_ENHANCEMENTS
#define FEATURE_HHO_ENHANCEMENTS
#define FEATURE_BUILD_CDMA_TRIMODE 
#define FEATURE_TRANS_MANAGER 
/**************************************************/
/*                  SRCH / SLEEP                  */
/**************************************************/

#define FEATURE_MSM6500_DEC_DYNAMIC_CLK_OFF
/**************************************************/
/*                      HDR                       */
/**************************************************/
/* Minimal UATI Mode feature */
#define FEATURE_HDR_MIN_UATI_MODE
#ifdef FEATURE_HDR_MIN_UATI_MODE
  /* When maintain EVDO session is FALSE, whether to explicitly
   * close session (ECS) by sending SessionClose message to AN */
  #undef FEATURE_HDR_MIN_UATI_MODE_ECS
#endif

/* Control of FEATURE_HDR_CUSTOM_CONFIG has been moved to the build file */


/**************************************************/
/*                  Multimode                     */
/**************************************************/
#ifdef FEATURE_IS683C_PRL
#define FEATURE_IS683C_OTASP
#endif
/**************************************************/
/*                  MDSP                          */
/**************************************************/

/**************************************************/
/*                      RF                        */
/**************************************************/

/**************************************************/
/*                      GPSONE                    */
/**************************************************/

/**************************************************/
/*                      DATA                      */
/**************************************************/
#define FEATURE_SNM_ENHANCEMENTS
#define FEATURE_DATA_SERIALIZER
// #define FEATURE_DATA_PS_DATA_LOGGING_PARTIAL_PPP


/* FEATURE_CDMA feature should be defined only in C2k 
 * and multimode (UMTS + CDMA) builds. 
 * support for IS-95/IS2000/1X Data Services   */
#define FEATURE_CDMA


/*************************************************/
/*             C2K INTEGRATION ONLY              */
/*************************************************/
#ifdef FEATURE_C2K_INTEGRATION_ONLY
#define FEATURE_HDR_AT_TEST_ONLY
#define FEATURE_HDR_PS_GAUP
#define FEATURE_HDR_PN_DELAY_SUPPORT
#endif /* FEATURE_C2K_INTEGRATION_ONLY */

#endif /* CUSTCDMA2000_H */


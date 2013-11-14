#ifndef CUSTDATA_H
#define CUSTDATA_H
/*===========================================================================

DESCRIPTION
  Configuration for DATA.

  Copyright (c) 2002 - 2011 by QUALCOMM Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/data/build/main/lite/custdata.h#13 $ $DateTime: 2011/02/24 23:31:53 $ $Author: msankar $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
02/25/11   ms      Enabled Mobile IP Deregistration feature.
04/19/10   sn      Merged support for PAP internal auth.
05/30/09   ms      Added EPZID Hysteresis Feature definitions.
09/29/08   sn      Added FEATURE_DMU check to define FEATURE_DS_MOBILE_IP_DMU.
07/11/08   sn      Changed FEATURE_DS_SIP_MULTIPLE_PROFILE to 
                   FEATURE_DS_MULTIPLE_PROFILES.
05/26/08   sn      Added multiple profiles feature definitions.
06/13/08   sn      Updated to enable FEATURE_DS_MOBILE_IP_DMU based on 
                   FEATURE_SEC.
02/08/08   sn      Removed feature definitions that are obsolete/not needed for
                   data lite.
11/23/07   sn      Enabled Mobile IP feature.
10/18/07   sn      Disabled FEATURE_DATA_SERIALIZER as serializer support is 
                   not available. And defined FEATURE_USE_OLD_PS_RAND_IFACE.
11/11/02   cah     Added GSM features.
05/01/02   jct     Created
===========================================================================*/

/* -----------------------------------------------------------------------
** Data Services
** ----------------------------------------------------------------------- */
#if !defined(FEATURE_USES_LOWMEM)&&!defined(FEATURE_LOWER_MEM)
#define FEATURE_DS
#endif

#ifdef FEATURE_DS
   /* Turn on old data api */
   #define FEATURE_DSS_DNS_DEPRECATED_API

   #define FEATURE_DSAT_BREW_SUPPORT

#ifdef FEATURE_BUILD_JCDMA
     /* disable support for Mobile IP */
     #undef FEATURE_DS_MOBILE_IP
     
     /* disable Mobile IP performance metrics */
     #undef FEATURE_DS_MOBILE_IP_PERF

     /* disable dynamic MIP key update */
     #undef FEATURE_DS_MOBILE_IP_DMU

     /* disable TCP delayed ACK feature */
     #undef FEATURE_DS_TCP_NO_DELAY

     /* disable FEATURE_DS_QNC in JCDMA 
      * Quick Net Connect originations */
     #undef FEATURE_DS_QNC

#else

     /* Code hooks to support Mobile IP */
     #define FEATURE_DS_MOBILE_IP
   
     /* Enable Mobile IP performance metrics */
     #define FEATURE_DS_MOBILE_IP_PERF
    
     /* Enable Mobile IP Deregistration feature */
     #define FEATURE_DS_MOBILE_IP_DEREG

     /* FEATURE_DMU needs to be defined/enabled in .builds file of the target 
      * depending on which tier (low/mid/high) requires MIP DMU feature.
     */
     #ifdef FEATURE_DMU
        /* Support for dynamic Mobile IP Key update */
        #define FEATURE_DS_MOBILE_IP_DMU
     #endif /* FEATURE_DMU */

     /* TCP delayed ACK feature */
     #define FEATURE_DS_TCP_NO_DELAY

     /* Enable FEATURE_DS_QNC in non JCDMA  
      * Quick Net Connect originations */
     #define FEATURE_DS_QNC

#endif /* FEATURE_BUILD_JCDMA */

   /*enable ppp logging*/
   #define FEATURE_DATA_PS_PPP_LOGGING

   /* New Multi-Mode Data Features */
   /* Support for generic Data Services*/
   #define FEATURE_DATA

   /* Support for new multimode data architecture.
    * Used by code which supports both the old and new data architectures.
   */
   #define FEATURE_DATA_MM

   /* Support for IS-707 Data Services (Async & Packet) */
   #define FEATURE_DATA_IS707

   /* Support for packet-switched data services and IS-707 Async Data */
   #define FEATURE_DATA_PS

   /* Support for IS-95/IS2000/1X Data Services */
   #define FEATURE_CDMA

   /* IS 707 PZID Hysterisis feature*/
   #define FEATURE_DS_PZID_HYSTERESIS

   /* IS 707B EPZID Hysterisis feature*/
   #ifdef FEATURE_DATA_EPZID
     #define FEATURE_DS_EPZID_HYSTERESIS
     #undef FEATURE_DS_PZID_HYSTERESIS
   #endif /* FEATURE_DATA_EPZID */

   /* Replaces FEATURE_DS_PZID_RECON from old data architecture.
    * Enables data reconnections on PZID change.
   */
   #define FEATURE_DATA_IS707_PZID_RECON

   /* Replaces FEATURE_DS_SID_CHNG_RECON from old data architecture.
    * Enables data reconnections on SID change.
   */
   #define FEATURE_DATA_IS707_SID_CHNG_RECON

   /* Replaces FEATURE_DS_NID_CHNG_RECON from old data architecture.
    * Enables data reconnections on NID change.
   */
   #define FEATURE_DATA_IS707_NID_CHNG_RECON

   /* Enables features used for test */
   #define FEATURE_DS_AT_TEST_ONLY

   /* For old PS VUs */
   #define FEATURE_USE_OLD_PS_RAND_IFACE

   /* Data on the Go AMPS data */
   #undef FEATURE_DS_DOTG_DATA

   /* Enable Data Services memory checks*/
   #define FEATURE_DSM_MEM_CHK

   /* Enable large items in DSM */
   #define FEATURE_DSM_LARGE_ITEMS

   /* Enable duplicate items in DSM */
   #define FEATURE_DSM_DUP_ITEMS

   /* Disables packet based Browser Interface*/
   #undef FEATURE_DS_BROWSER_INTERFACE

   /* Enables Network Model */
   #define FEATURE_DS_NET_MODEL

   /* Enables packet based sockets interface*/
   #define FEATURE_DS_SOCKETS

   /* Enables Protocol statistics for Data Services */
   #undef FEATURE_DS_PSTATS

   /* Enables Debug UI for Data Services */
   #undef FEATURE_DS_DEBUG_UI

   /* Enable Mobile to Mobile calls for IS-99 and IS-657 */
   #define FEATURE_DS_MTOM

   /*********************************************************************
     Exactly ONE of FEATURE_DS_ORIG_XXX_SO must be defined.
   *********************************************************************/
   /* Support choosing the set of service options used for originations.*/
   #define FEATURE_DS_CHOOSE_SO

#if defined(T_QSC6055) || defined(T_QSC6085) || defined(T_MSM7500) || defined(T_QSC1100)
#else
   /* Enable Autobaud detection for rates between 1200 and 19200   */
   #define FEATURE_AUTOBAUD
#endif

   /* MDR specific features. These are included only
   ** if FEATURE_IS95B_MDR is included
   */
   #ifdef FEATURE_IS95B_MDR
      /* MDR Data Pattern Test feature. This is an internal test feature  */
      #undef FEATURE_MDR_DPT_MARKOV

      /* Support for the IS-707-A format of the AT+CMUX command. This command
      ** sets the maximum forward & reverse multiplex options.
      */
      #define FEATURE_DS_IS707A_CMUX

   #endif /* FEATURE_IS95B_MDR */

   /* RF SCRM algorithm.
    * Lets mobile send a SCRM based on RF channel conditions; otherwise,
    * mobile sends a SCRM based only on CPU.
   */
   #define FEATURE_SCRM_ON_RF

   #undef FEATURE_DATA_SERIALIZER

#ifdef FEATURE_UIM_3GPD_MULTIPLE_SIP_PROFILES
   /* Enable multiple profiles functionality */
   #define FEATURE_DS_MULTIPLE_PROFILES
#endif /* FEATURE_UIM_3GPD_MULTIPLE_SIP_PROFILES */

#ifdef FEATURE_DS_MULTIPLE_PROFILES
   #define FEATURE_DATA_PS_ARBITRATION_MGR
#endif /* FEATURE_DS_MULTIPLE_PROFILES */

#endif /* FEATURE_DS */

/* Please leave the definition below as is at the end of the file 
   as it depends on another feature being defined */                                               
#if defined(FEATURE_DS_MOBILE_IP_DMU)
  #define FEATURE_DATA_PS_INTERNAL_AUTH
#endif /* FEATURE_DS_MOBILE_IP_DMU */

#if defined(FEATURE_DATA_PS_INTERNAL_AUTH)
  #define FEATURE_DATA_PS_PAP_INTERNAL_AUTH
#endif /* FEATURE_DATA_PS_INTERNAL_AUTH */

#endif /* CUSTDATA_H */

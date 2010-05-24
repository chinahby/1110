#ifndef MSGTGT_H
#define MSGTGT_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
         
     DIAGNOSTIC MESSAGE SERVICE 2.0 TARGET-SPECIFIC CONFIGURATION FILE

GENERAL DESCRIPTION

  MSG 2.0 offers the ability to utilize more build-time and run-time 
  filtering of debug messages.  
  
  This file is used to customize MSG 2.0 for a specific build.  All 
  definitions here override their counterparts in msgcfg.h.
  
  This file may contain, but is not limited to any or all of the following:
  build masks for known SSIDs
  default build mask
  max string length (used to avoid endless strcpy).

  Build masks are 32 bits, each specifying a category within a SSID.  The 
  fist 5 LSBs are reserved for legacy (Low, Med, High, Error, Fatal).  These
  can be reused with the tradeoff of any legacy tool interpreting those bits
  as legacy message levels.
  
  The 27 MSBs in the mask are defined at the discretion of the technology 
  area associated with the SSID.  These are to be published, so changing the
  meaning is not allowed.
                      
Copyright (c) 2002-2007 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR FILE

  $Header: //depot/asic/qsc1100/services/diag/msgtgt.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/14/04   as      Reallocated SSID's used by WinCE to L4LINUX.
03/07/03   lad     First SSID deployment.
12/03/02   lad     Created file.

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#include "msg.h"

/* Default build mask for all uncategorizerd (default) messages. */
#define MSG_BUILD_MASK_DFLT MSG_LVL_ERROR

#if defined ( FEATURE_MSG_LOW_MEMORY_USAGE)
#define MSG_LOW_MSM_USAGE                   (MSG_LVL_FATAL)
#endif

/* General SSIDs */
#if defined ( FEATURE_MSG_LOW_MEMORY_USAGE)
 #define MSG_BUILD_MASK_MSG_SSID_DFLT      (MSG_LOW_MSM_USAGE)
#elif defined( T_MSM6200 ) || defined( T_MSM6250 )
#error code not present
#else
 #define MSG_BUILD_MASK_MSG_SSID_DFLT      (MSG_LVL_MED)
#endif

#if defined ( FEATURE_MSG_LOW_MEMORY_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_AUDFMT    (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_AVS       (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_BOOT      (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_BT        (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_CM        (MSG_LOW_MSM_USAGE) //AST req
#define MSG_BUILD_MASK_MSG_SSID_CMX       (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_DIAG      (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_DSM       (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_FTM       (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_FS        (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_HS        (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_GPS       (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_MDSP      (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_MMOC      (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_QDSP      (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_REX       (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_RF        (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_RUIM      (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_SIO       (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_SD        (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_SRM       (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_TMC       (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_VS        (MSG_LOW_MSM_USAGE)     
#define MSG_BUILD_MASK_MSG_SSID_WMS       (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_MMGPS     (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_SLEEP     (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_SAM       (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_SFAT      (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_JOYST     (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_MFLO      (MSG_LOW_MSM_USAGE|MSG_MASK_7 | \
               MSG_MASK_8|MSG_MASK_9|MSG_MASK_10|MSG_MASK_11|MSG_MASK_12 | \
               MSG_MASK_13|MSG_MASK_14|MSG_MASK_15|MSG_MASK_16 | \
               MSG_MASK_17|MSG_MASK_18|MSG_MASK_19|MSG_MASK_20|MSG_MASK_21)
#define MSG_BUILD_MASK_MSG_SSID_DTV       (MSG_LOW_MSM_USAGE|MSG_MASK_5 | \
             MSG_MASK_6|MSG_MASK_7|MSG_MASK_8|MSG_MASK_9|MSG_MASK_10| \
             MSG_MASK_11|MSG_MASK_12|MSG_MASK_13|MSG_MASK_14| \
             MSG_MASK_15|MSG_MASK_16)
#define MSG_BUILD_MASK_MSG_SSID_TCXOMGR   (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_EFS       (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_IRDA      (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_FM_RADIO  (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_AAM       (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_BM        (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_PE        (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_QIPCALL   (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_FLUTE     (MSG_LOW_MSM_USAGE|MSG_MASK_5 | \
             MSG_MASK_6|MSG_MASK_7|MSG_MASK_8)
#define MSG_BUILD_MASK_MSG_SSID_CAMERA    (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_HSUSB     (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_FC        (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_USBHOST   (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_PROFILER  (MSG_LOW_MSM_USAGE)
#ifndef MSG_BUILD_MASK_MSG_SSID_MGP
  #define MSG_BUILD_MASK_MSG_SSID_MGP       (MSG_LOW_MSM_USAGE)
#endif
#ifndef MSG_BUILD_MASK_MSG_SSID_MGPME
  #define MSG_BUILD_MASK_MSG_SSID_MGPME     (MSG_LOW_MSM_USAGE|MSG_MASK_5 | \
             MSG_MASK_6|MSG_MASK_7|MSG_MASK_8|MSG_MASK_9|MSG_MASK_10| \
             MSG_MASK_11|MSG_MASK_12|MSG_MASK_13|MSG_MASK_14|MSG_MASK_15| \
             MSG_MASK_16|MSG_MASK_17|MSG_MASK_18|MSG_MASK_19|MSG_MASK_20| \
             MSG_MASK_21|MSG_MASK_22|MSG_MASK_23|MSG_MASK_24|MSG_MASK_25)
#endif
#ifndef MSG_BUILD_MASK_MSG_SSID_MGPPE
  #define MSG_BUILD_MASK_MSG_SSID_MGPPE     (MSG_LOW_MSM_USAGE|MSG_MASK_5 | \
             MSG_MASK_6|MSG_MASK_7|MSG_MASK_8|MSG_MASK_9|MSG_MASK_10)
#endif
#ifndef MSG_BUILD_MASK_MSG_SSID_GPSOS
  #define MSG_BUILD_MASK_MSG_SSID_GPSOS     (MSG_LOW_MSM_USAGE)
#endif
#ifndef MSG_BUILD_MASK_MSG_SSID_GPSSM
  #define MSG_BUILD_MASK_MSG_SSID_GPSSM     (MSG_LOW_MSM_USAGE)
#endif
#ifndef MSG_BUILD_MASK_MSG_SSID_IMS
  #define MSG_BUILD_MASK_MSG_SSID_IMS       (MSG_LOW_MSM_USAGE)
#endif
#ifndef MSG_BUILD_MASK_MSG_SSID_MBP_RF
  #define MSG_BUILD_MASK_MSG_SSID_MBP_RF    (MSG_LOW_MSM_USAGE)
#endif
#ifndef MSG_BUILD_MASK_MSG_SSID_PWRDB
  #define MSG_BUILD_MASK_MSG_SSID_PWRDB     (MSG_LVL_HIGH)
#endif

/* ONCRPC related SSIDs */
#define MSG_BUILD_MASK_MSG_SSID_ONCRPC            (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_ONCRPC_MISC_MODEM (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_ONCRPC_MISC_APPS  (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_ONCRPC_CM_MODEM   (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_ONCRPC_CM_APPS    (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_ONCRPC_DB         (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_ONCRPC_SND        (MSG_LOW_MSM_USAGE)

#else  /* FEATURE_MSG_LOW_MEMORY_USAGE */

#define MSG_BUILD_MASK_MSG_SSID_AUDFMT    (MSG_LVL_MED)
#define MSG_BUILD_MASK_MSG_SSID_AVS       (MSG_LVL_MED)
#define MSG_BUILD_MASK_MSG_SSID_BOOT      (MSG_LVL_ERROR)
#define MSG_BUILD_MASK_MSG_SSID_BT        (MSG_LVL_LOW)
#define MSG_BUILD_MASK_MSG_SSID_CM        (MSG_LVL_MED) //AST req
#define MSG_BUILD_MASK_MSG_SSID_CMX       (MSG_LVL_MED)
#define MSG_BUILD_MASK_MSG_SSID_DIAG      (MSG_LVL_HIGH)
#define MSG_BUILD_MASK_MSG_SSID_DSM       (MSG_LVL_ERROR)
#define MSG_BUILD_MASK_MSG_SSID_FTM       (MSG_LVL_LOW)
#define MSG_BUILD_MASK_MSG_SSID_FS        (MSG_LVL_ERROR)
#define MSG_BUILD_MASK_MSG_SSID_HS        (MSG_LVL_ERROR)
#define MSG_BUILD_MASK_MSG_SSID_GPS       (MSG_LVL_MED)
#define MSG_BUILD_MASK_MSG_SSID_MDSP      (MSG_LVL_MED)
#define MSG_BUILD_MASK_MSG_SSID_MMOC      (MSG_LVL_MED)
#define MSG_BUILD_MASK_MSG_SSID_QDSP      (MSG_LVL_HIGH)
#define MSG_BUILD_MASK_MSG_SSID_REX       (MSG_LVL_HIGH)
#define MSG_BUILD_MASK_MSG_SSID_RF        (MSG_LVL_HIGH)
#define MSG_BUILD_MASK_MSG_SSID_RUIM      (MSG_LVL_LOW)
#define MSG_BUILD_MASK_MSG_SSID_SIO       (MSG_LVL_ERROR)
#define MSG_BUILD_MASK_MSG_SSID_SD        (MSG_LVL_LOW)
#define MSG_BUILD_MASK_MSG_SSID_SRM       (MSG_LVL_MED)
#define MSG_BUILD_MASK_MSG_SSID_TMC       (MSG_LVL_MED)
#define MSG_BUILD_MASK_MSG_SSID_VS        (MSG_LVL_MED)     
#define MSG_BUILD_MASK_MSG_SSID_WMS       (MSG_LVL_MED)
#define MSG_BUILD_MASK_MSG_SSID_MMGPS     (MSG_LVL_LOW)
#define MSG_BUILD_MASK_MSG_SSID_SLEEP     (MSG_LVL_MED)
#define MSG_BUILD_MASK_MSG_SSID_SAM       (MSG_LVL_MED)
#define MSG_BUILD_MASK_MSG_SSID_SFAT      (MSG_LVL_MED)
#define MSG_BUILD_MASK_MSG_SSID_JOYST     (MSG_LVL_MED)
#define MSG_BUILD_MASK_MSG_SSID_MFLO      (MSG_LVL_MED|MSG_MASK_7 | \
               MSG_MASK_8|MSG_MASK_9|MSG_MASK_10|MSG_MASK_11|MSG_MASK_12 | \
               MSG_MASK_13|MSG_MASK_14|MSG_MASK_15|MSG_MASK_16 | \
               MSG_MASK_17|MSG_MASK_18|MSG_MASK_19|MSG_MASK_20|MSG_MASK_21)
#define MSG_BUILD_MASK_MSG_SSID_DTV       (MSG_LVL_MED|MSG_MASK_5 | \
             MSG_MASK_6|MSG_MASK_7|MSG_MASK_8|MSG_MASK_9|MSG_MASK_10| \
             MSG_MASK_11|MSG_MASK_12|MSG_MASK_13|MSG_MASK_14| \
             MSG_MASK_15|MSG_MASK_16)
#define MSG_BUILD_MASK_MSG_SSID_TCXOMGR   (MSG_LVL_MED)
#define MSG_BUILD_MASK_MSG_SSID_EFS       (MSG_LVL_MED)
#define MSG_BUILD_MASK_MSG_SSID_IRDA      (MSG_LVL_HIGH)
#define MSG_BUILD_MASK_MSG_SSID_FM_RADIO  (MSG_LVL_HIGH)
#define MSG_BUILD_MASK_MSG_SSID_AAM       (MSG_LVL_HIGH)
#define MSG_BUILD_MASK_MSG_SSID_BM        (MSG_LVL_HIGH)
#define MSG_BUILD_MASK_MSG_SSID_PE        (MSG_LVL_MED)
#define MSG_BUILD_MASK_MSG_SSID_QIPCALL   (MSG_LVL_MED)
#define MSG_BUILD_MASK_MSG_SSID_FLUTE     (MSG_LVL_MED|MSG_MASK_5 | \
             MSG_MASK_6|MSG_MASK_7|MSG_MASK_8)
#define MSG_BUILD_MASK_MSG_SSID_CAMERA    (MSG_LVL_HIGH)
#define MSG_BUILD_MASK_MSG_SSID_HSUSB     (MSG_LVL_LOW)
#define MSG_BUILD_MASK_MSG_SSID_FC        (MSG_LVL_HIGH)
#define MSG_BUILD_MASK_MSG_SSID_USBHOST   (MSG_LVL_HIGH)
#define MSG_BUILD_MASK_MSG_SSID_PROFILER  (MSG_LVL_HIGH)
#ifndef MSG_BUILD_MASK_MSG_SSID_MGP
  #define MSG_BUILD_MASK_MSG_SSID_MGP       (MSG_LVL_MED)
#endif
#ifndef MSG_BUILD_MASK_MSG_SSID_MGPME
  #define MSG_BUILD_MASK_MSG_SSID_MGPME     (MSG_LVL_MED|MSG_MASK_5 | \
             MSG_MASK_6|MSG_MASK_7|MSG_MASK_8|MSG_MASK_9|MSG_MASK_10| \
             MSG_MASK_11|MSG_MASK_12|MSG_MASK_13|MSG_MASK_14|MSG_MASK_15| \
             MSG_MASK_16|MSG_MASK_17|MSG_MASK_18|MSG_MASK_19|MSG_MASK_20| \
             MSG_MASK_21|MSG_MASK_22|MSG_MASK_23|MSG_MASK_24|MSG_MASK_25)
#endif
#ifndef MSG_BUILD_MASK_MSG_SSID_MGPPE
  #define MSG_BUILD_MASK_MSG_SSID_MGPPE     (MSG_LVL_MED|MSG_MASK_5 | \
             MSG_MASK_6|MSG_MASK_7|MSG_MASK_8|MSG_MASK_9|MSG_MASK_10)
#endif
#ifndef MSG_BUILD_MASK_MSG_SSID_GPSOS
  #define MSG_BUILD_MASK_MSG_SSID_GPSOS     (MSG_LVL_MED)
#endif
#ifndef MSG_BUILD_MASK_MSG_SSID_GPSSM
  #define MSG_BUILD_MASK_MSG_SSID_GPSSM     (MSG_LVL_MED)
#endif
#ifndef MSG_BUILD_MASK_MSG_SSID_IMS
  #define MSG_BUILD_MASK_MSG_SSID_IMS       (MSG_LVL_HIGH)
#endif
#ifndef MSG_BUILD_MASK_MSG_SSID_MBP_RF
  #define MSG_BUILD_MASK_MSG_SSID_MBP_RF    (MSG_LVL_MED)
#endif


/* ONCRPC related SSIDs */
#define MSG_BUILD_MASK_MSG_SSID_ONCRPC            (MSG_LVL_MED)
#define MSG_BUILD_MASK_MSG_SSID_ONCRPC_MISC_MODEM (MSG_LVL_HIGH)
#define MSG_BUILD_MASK_MSG_SSID_ONCRPC_MISC_APPS  (MSG_LVL_HIGH)
#define MSG_BUILD_MASK_MSG_SSID_ONCRPC_CM_MODEM   (MSG_LVL_LOW)
#define MSG_BUILD_MASK_MSG_SSID_ONCRPC_CM_APPS    (MSG_LVL_HIGH)
#define MSG_BUILD_MASK_MSG_SSID_ONCRPC_DB         (MSG_LVL_HIGH)
#define MSG_BUILD_MASK_MSG_SSID_ONCRPC_SND        (MSG_LVL_HIGH)

#endif  /* FEATURE_MSG_LOW_MEMORY_USAGE */ 
/* 1X related SSIDs */
#ifdef FEATURE_IS2000

#ifdef FEATURE_MSG_LOW_MEMORY_USAGE

#define MSG_BUILD_MASK_MSG_SSID_1X          (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_1X_ACP      (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_1X_DCP      (MSG_LOW_MSM_USAGE)  //AST req
#define MSG_BUILD_MASK_MSG_SSID_1X_DEC      (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_1X_ENC      (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_1X_GPSSRCH  (MSG_LOW_MSM_USAGE|MSG_MASK_5)
#define MSG_BUILD_MASK_MSG_SSID_1X_MUX      (MSG_LOW_MSM_USAGE)  //AST req
#ifndef MSG_BUILD_MASK_MSG_SSID_1X_SRCH
 #define MSG_BUILD_MASK_MSG_SSID_1X_SRCH     (MSG_LOW_MSM_USAGE)
#endif

#else /* FEATURE_MSG_LOW_MEMORY_USAGE */

#define MSG_BUILD_MASK_MSG_SSID_1X          (MSG_LVL_MED)
#define MSG_BUILD_MASK_MSG_SSID_1X_ACP      (MSG_LVL_MED)
#define MSG_BUILD_MASK_MSG_SSID_1X_DCP      (MSG_LVL_MED)  //AST req
#define MSG_BUILD_MASK_MSG_SSID_1X_DEC      (MSG_LVL_MED)
#define MSG_BUILD_MASK_MSG_SSID_1X_ENC      (MSG_LVL_MED)
#define MSG_BUILD_MASK_MSG_SSID_1X_GPSSRCH  (MSG_LVL_MED|MSG_MASK_5)
#define MSG_BUILD_MASK_MSG_SSID_1X_MUX      (MSG_LVL_MED)  //AST req
#ifndef MSG_BUILD_MASK_MSG_SSID_1X_SRCH
 #define MSG_BUILD_MASK_MSG_SSID_1X_SRCH     (MSG_LVL_MED)
#endif

#endif /* FEATURE_MSG_LOW_MEMORY_USAGE */

#endif /* FEATURE_IS2000 */

/* HDR related SSIDs */
#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */


/* HIT related SSIDs */
#ifdef FEATURE_HIT

#ifndef MSG_BUILD_MASK_MSG_SSID_HIT
#define MSG_BUILD_MASK_MSG_SSID_HIT         (MSG_LVL_MED)
#endif

#endif /* FEATURE_HIT */


/* UMTS related SSIDs */
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
#error code not present
#endif /* FEATURE_WCDMA */


/* GSM related SSIDs */
#ifdef FEATURE_GSM
#error code not present
#endif /* FEATURE_GSM */

#ifdef FEATURE_WLAN
#error code not present
#endif /* FEATURE_WLAN */

/* DS related SSIDs */
#if defined (FEATURE_DS) || defined (FEATURE_DATA)

#ifdef FEATURE_MSG_LOW_MEMORY_USAGE

#define MSG_BUILD_MASK_MSG_SSID_DS          (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_DS_RLP      (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_DS_PPP      (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_DS_TCPIP    (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_DS_IS707    (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_DS_3GMGR    (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_DS_PS       (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_DS_MIP      (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_DS_UMTS     (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_DS_GPRS     (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_DS_GSM      (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_DS_SOCKETS  (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_DS_ATCOP    (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_DS_SIO      (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_DS_BCMCS    (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_DS_MLRLP    (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_DS_RTP      (MSG_LOW_MSM_USAGE)
#define MSG_BUILD_MASK_MSG_SSID_DS_SIPSTACK (MSG_LOW_MSM_USAGE)
#else /* FEATURE_MSG_LOW_MEMORY_USAGE */
#define MSG_BUILD_MASK_MSG_SSID_DS          (MSG_LVL_MED)
#define MSG_BUILD_MASK_MSG_SSID_DS_RLP      (MSG_LVL_HIGH)
#define MSG_BUILD_MASK_MSG_SSID_DS_PPP      (MSG_LVL_HIGH)
#define MSG_BUILD_MASK_MSG_SSID_DS_TCPIP    (MSG_LVL_HIGH)
#define MSG_BUILD_MASK_MSG_SSID_DS_IS707    (MSG_LVL_HIGH)
#define MSG_BUILD_MASK_MSG_SSID_DS_3GMGR    (MSG_LVL_HIGH)
#define MSG_BUILD_MASK_MSG_SSID_DS_PS       (MSG_LVL_HIGH)
#define MSG_BUILD_MASK_MSG_SSID_DS_MIP      (MSG_LVL_HIGH)
#define MSG_BUILD_MASK_MSG_SSID_DS_UMTS     (MSG_LVL_HIGH)
#define MSG_BUILD_MASK_MSG_SSID_DS_GPRS     (MSG_LVL_HIGH)
#define MSG_BUILD_MASK_MSG_SSID_DS_GSM      (MSG_LVL_HIGH)
#define MSG_BUILD_MASK_MSG_SSID_DS_SOCKETS  (MSG_LVL_HIGH)
#define MSG_BUILD_MASK_MSG_SSID_DS_ATCOP    (MSG_LVL_HIGH)
#define MSG_BUILD_MASK_MSG_SSID_DS_SIO      (MSG_LVL_HIGH)
#define MSG_BUILD_MASK_MSG_SSID_DS_BCMCS    (MSG_LVL_MED)
#define MSG_BUILD_MASK_MSG_SSID_DS_MLRLP    (MSG_LVL_MED)
#define MSG_BUILD_MASK_MSG_SSID_DS_RTP      (MSG_LVL_LOW)
#define MSG_BUILD_MASK_MSG_SSID_DS_SIPSTACK (MSG_LVL_LOW)

#endif /* FEATURE_MSG_LOW_MEMORY_USAGE */
#endif /* defined (FEATURE_DS) || defined (FEATURE_DATA) */

#ifdef FEATURE_MSG_LOW_MEMORY_USAGE
#define MSG_BUILD_MASK_MSG_SSID_SEC            (MSG_LOW_MSM_USAGE|MSG_MASK_5)
#define MSG_BUILD_MASK_MSG_SSID_SEC_CRYPTO     (MSG_LOW_MSM_USAGE|MSG_MASK_5)
#define MSG_BUILD_MASK_MSG_SSID_SEC_SSL        (MSG_LOW_MSM_USAGE|MSG_MASK_5)
#define MSG_BUILD_MASK_MSG_SSID_SEC_IPSEC      (MSG_LOW_MSM_USAGE|MSG_MASK_5)
#define MSG_BUILD_MASK_MSG_SSID_SEC_SFS        (MSG_LOW_MSM_USAGE|MSG_MASK_5)
#define MSG_BUILD_MASK_MSG_SSID_SEC_TEST       (MSG_LOW_MSM_USAGE|MSG_MASK_5)
#define MSG_BUILD_MASK_MSG_SSID_SEC_CNTAGENT   (MSG_LOW_MSM_USAGE|MSG_MASK_5)
#define MSG_BUILD_MASK_MSG_SSID_SEC_RIGHTSMGR  (MSG_LOW_MSM_USAGE|MSG_MASK_5)
#define MSG_BUILD_MASK_MSG_SSID_SEC_ROAP       (MSG_LOW_MSM_USAGE|MSG_MASK_5)
#define MSG_BUILD_MASK_MSG_SSID_SEC_MEDIAMGR   (MSG_LOW_MSM_USAGE|MSG_MASK_5)
#define MSG_BUILD_MASK_MSG_SSID_SEC_IDSTORE    (MSG_LOW_MSM_USAGE|MSG_MASK_5)
#define MSG_BUILD_MASK_MSG_SSID_SEC_IXFILE     (MSG_LOW_MSM_USAGE|MSG_MASK_5)
#define MSG_BUILD_MASK_MSG_SSID_SEC_IXSQL      (MSG_LOW_MSM_USAGE|MSG_MASK_5)
#define MSG_BUILD_MASK_MSG_SSID_SEC_IXCOMMON   (MSG_LOW_MSM_USAGE|MSG_MASK_5)
#define MSG_BUILD_MASK_MSG_SSID_SEC_BCASTCNTAGENT (MSG_LOW_MSM_USAGE|MSG_MASK_5)

#else /* FEATURE_MSG_LOW_MEMORY_USAGE */

#define MSG_BUILD_MASK_MSG_SSID_SEC            (MSG_LVL_MED|MSG_MASK_5)
#define MSG_BUILD_MASK_MSG_SSID_SEC_CRYPTO     (MSG_LVL_MED|MSG_MASK_5)
#define MSG_BUILD_MASK_MSG_SSID_SEC_SSL        (MSG_LVL_MED|MSG_MASK_5)
#define MSG_BUILD_MASK_MSG_SSID_SEC_IPSEC      (MSG_LVL_MED|MSG_MASK_5)
#define MSG_BUILD_MASK_MSG_SSID_SEC_SFS        (MSG_LVL_MED|MSG_MASK_5)
#define MSG_BUILD_MASK_MSG_SSID_SEC_TEST       (MSG_LVL_MED|MSG_MASK_5)
#define MSG_BUILD_MASK_MSG_SSID_SEC_CNTAGENT   (MSG_LVL_MED|MSG_MASK_5)
#define MSG_BUILD_MASK_MSG_SSID_SEC_RIGHTSMGR  (MSG_LVL_MED|MSG_MASK_5)
#define MSG_BUILD_MASK_MSG_SSID_SEC_ROAP       (MSG_LVL_MED|MSG_MASK_5)
#define MSG_BUILD_MASK_MSG_SSID_SEC_MEDIAMGR   (MSG_LVL_MED|MSG_MASK_5)
#define MSG_BUILD_MASK_MSG_SSID_SEC_IDSTORE    (MSG_LVL_MED|MSG_MASK_5)
#define MSG_BUILD_MASK_MSG_SSID_SEC_IXFILE     (MSG_LVL_MED|MSG_MASK_5)
#define MSG_BUILD_MASK_MSG_SSID_SEC_IXSQL      (MSG_LVL_MED|MSG_MASK_5)
#define MSG_BUILD_MASK_MSG_SSID_SEC_IXCOMMON   (MSG_LVL_MED|MSG_MASK_5)
#define MSG_BUILD_MASK_MSG_SSID_SEC_BCASTCNTAGENT (MSG_LVL_MED|MSG_MASK_5)

#endif /* FEATURE_MSG_LOW_MEMORY_USAGE */

/* APPS related SSIDs */
#define MSG_BUILD_MASK_MSG_SSID_APPS        (MSG_LVL_MED)
#define MSG_BUILD_MASK_MSG_SSID_APPS_APPMGR (MSG_LVL_ERROR)
#define MSG_BUILD_MASK_MSG_SSID_APPS_UI     (MSG_LVL_LOW)      
#define MSG_BUILD_MASK_MSG_SSID_APPS_QTV    (MSG_LVL_LOW|MSG_MASK_5 | \
             MSG_MASK_6|MSG_MASK_7|MSG_MASK_8|MSG_MASK_9|MSG_MASK_10| \
             MSG_MASK_11|MSG_MASK_12|MSG_MASK_13|MSG_MASK_14|MSG_MASK_15| \
             MSG_MASK_16|MSG_MASK_17|MSG_MASK_18|MSG_MASK_19|MSG_MASK_20| \
             MSG_MASK_21|MSG_MASK_22)
#define MSG_BUILD_MASK_MSG_SSID_APPS_QVP    (MSG_LVL_LOW|MSG_MASK_5) 
#define MSG_BUILD_MASK_MSG_SSID_APPS_QVP_STATISTICS  (MSG_LVL_LOW)
#define MSG_BUILD_MASK_MSG_SSID_APPS_QVP_VENCODER    (MSG_LVL_LOW)
#define MSG_BUILD_MASK_MSG_SSID_APPS_QVP_MODEM       (MSG_LVL_LOW)
#define MSG_BUILD_MASK_MSG_SSID_APPS_QVP_UI          (MSG_LVL_LOW)
#define MSG_BUILD_MASK_MSG_SSID_APPS_QVP_STACK       (MSG_LVL_LOW)
#define MSG_BUILD_MASK_MSG_SSID_APPS_QVP_VDECODER    (MSG_LVL_LOW)
#define MSG_BUILD_MASK_MSG_SSID_APPS_ACM             (MSG_LVL_LOW)
#define MSG_BUILD_MASK_MSG_SSID_APPS_HEAP_PROFILE    (MSG_LVL_LOW)
#define MSG_BUILD_MASK_MSG_SSID_APPS_QTV_GENERAL     (MSG_LVL_LOW|MSG_MASK_5)
#define MSG_BUILD_MASK_MSG_SSID_APPS_QTV_DEBUG       (MSG_LVL_LOW|MSG_MASK_5)
#define MSG_BUILD_MASK_MSG_SSID_APPS_QTV_STATISTICS  (MSG_LVL_LOW|MSG_MASK_5)
#define MSG_BUILD_MASK_MSG_SSID_APPS_QTV_UI_TASK     (MSG_LVL_LOW|MSG_MASK_5)
#define MSG_BUILD_MASK_MSG_SSID_APPS_QTV_MP4_PLAYER  (MSG_LVL_LOW|MSG_MASK_5)
#define MSG_BUILD_MASK_MSG_SSID_APPS_QTV_AUDIO_TASK  (MSG_LVL_LOW|MSG_MASK_5)
#define MSG_BUILD_MASK_MSG_SSID_APPS_QTV_VIDEO_TASK  (MSG_LVL_LOW|MSG_MASK_5)
#define MSG_BUILD_MASK_MSG_SSID_APPS_QTV_STREAMING   (MSG_LVL_LOW|MSG_MASK_5)
#define MSG_BUILD_MASK_MSG_SSID_APPS_QTV_MPEG4_TASK  (MSG_LVL_LOW|MSG_MASK_5)
#define MSG_BUILD_MASK_MSG_SSID_APPS_QTV_FILE_OPS    (MSG_LVL_LOW|MSG_MASK_5)
#define MSG_BUILD_MASK_MSG_SSID_APPS_QTV_RTP         (MSG_LVL_LOW|MSG_MASK_5)
#define MSG_BUILD_MASK_MSG_SSID_APPS_QTV_RTCP        (MSG_LVL_LOW|MSG_MASK_5)
#define MSG_BUILD_MASK_MSG_SSID_APPS_QTV_RTSP        (MSG_LVL_LOW|MSG_MASK_5)
#define MSG_BUILD_MASK_MSG_SSID_APPS_QTV_SDP_PARSE   (MSG_LVL_LOW|MSG_MASK_5)
#define MSG_BUILD_MASK_MSG_SSID_APPS_QTV_ATOM_PARSE  (MSG_LVL_LOW|MSG_MASK_5)
#define MSG_BUILD_MASK_MSG_SSID_APPS_QTV_TEXT_TASK   (MSG_LVL_LOW|MSG_MASK_5)
#define MSG_BUILD_MASK_MSG_SSID_APPS_QTV_DEC_DSP_IF  (MSG_LVL_LOW|MSG_MASK_5)
#define MSG_BUILD_MASK_MSG_SSID_APPS_QTV_STREAM_RECORDING  (MSG_LVL_LOW|MSG_MASK_5)
#define MSG_BUILD_MASK_MSG_SSID_APPS_QTV_CONFIGURATION     (MSG_LVL_LOW|MSG_MASK_5)
#define MSG_BUILD_MASK_MSG_SSID_APPS_QCAMERA               (MSG_LVL_MED)
#define MSG_BUILD_MASK_MSG_SSID_APPS_QCAMCORDER            (MSG_LVL_MED)
#define MSG_BUILD_MASK_MSG_SSID_APPS_BREW                  (MSG_LVL_HIGH)
#define MSG_BUILD_MASK_MSG_SSID_APPS_QDJ                   (MSG_LVL_LOW)
#define MSG_BUILD_MASK_MSG_SSID_APPS_QDTX                  (MSG_LVL_LOW)
#define MSG_BUILD_MASK_MSG_SSID_APPS_QTV_BCAST_FLO   (MSG_LVL_LOW|MSG_MASK_5)
#define MSG_BUILD_MASK_MSG_SSID_APPS_MDP_GENERAL     (MSG_LVL_LOW|MSG_MASK_0| \
          MSG_MASK_1|MSG_MASK_2|MSG_MASK_3|MSG_MASK_4|MSG_MASK_5|MSG_MASK_6)
#define MSG_BUILD_MASK_MSG_SSID_APPS_PBM                   (MSG_LVL_HIGH)
#define MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_GENERAL      (MSG_LVL_LOW)
#define MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_EGL          (MSG_LVL_LOW)
#define MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_OPENGL       (MSG_LVL_LOW)
#define MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_DIRECT3D     (MSG_LVL_LOW)
#define MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_SVG          (MSG_LVL_LOW)
#define MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_OPENVG       (MSG_LVL_LOW)
#define MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_2D           (MSG_LVL_LOW)
#define MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_QXPROFILER   (MSG_LVL_LOW)
#define MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_DSP          (MSG_LVL_LOW)
#define MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_GRP          (MSG_LVL_LOW)
#define MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_MDP          (MSG_LVL_LOW)

/* ADSP tasks related SSIDs */
#define MSG_BUILD_MASK_MSG_SSID_ADSPTASKS                 (MSG_LVL_LOW)
#define MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_KERNEL          (MSG_LVL_LOW)
#define MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_AFETASK         (MSG_LVL_LOW)
#define MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_VOICEPROCTASK   (MSG_LVL_LOW)
#define MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_VOCDECTASK      (MSG_LVL_LOW)
#define MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_VOCENCTASK      (MSG_LVL_LOW)
#define MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_VIDEOTASK       (MSG_LVL_LOW)
#define MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_VFETASK         (MSG_LVL_LOW)
#define MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_VIDEOENCTASK    (MSG_LVL_LOW)
#define MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_JPEGTASK        (MSG_LVL_LOW)
#define MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_AUDPPTASK       (MSG_LVL_LOW)
#define MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_AUDPLAY0TASK    (MSG_LVL_LOW)
#define MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_AUDPLAY1TASK    (MSG_LVL_LOW)
#define MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_AUDPLAY2TASK    (MSG_LVL_LOW)
#define MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_AUDPLAY3TASK    (MSG_LVL_LOW)
#define MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_AUDPLAY4TASK    (MSG_LVL_LOW) 
#define MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_LPMTASK         (MSG_LVL_LOW)
#define MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_DIAGTASK        (MSG_LVL_LOW)
#define MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_AUDRECTASK      (MSG_LVL_LOW)
#define MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_AUDPREPROCTASK  (MSG_LVL_LOW) 
#define MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_MODMATHTASK     (MSG_LVL_LOW)
#define MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_GRAPHICSTASK    (MSG_LVL_LOW)

/* L4 LINUX KERNEL related SSIDs */
#define MSG_BUILD_MASK_MSG_SSID_L4LINUX_KERNEL       (MSG_LVL_MED)
#define MSG_BUILD_MASK_MSG_SSID_L4LINUX_KEYPAD       (MSG_LVL_MED)
#define MSG_BUILD_MASK_MSG_SSID_L4LINUX_APPS         (MSG_LVL_MED)
#define MSG_BUILD_MASK_MSG_SSID_L4LINUX_QDDAEMON     (MSG_LVL_MED)

/* L4 IGUANA related SSIDs */
#define MSG_BUILD_MASK_MSG_SSID_L4IGUANA_IGUANASERVER  (MSG_LVL_MED)
#define MSG_BUILD_MASK_MSG_SSID_L4IGUANA_EFS2          (MSG_LVL_MED)
#define MSG_BUILD_MASK_MSG_SSID_L4IGUANA_QDMS          (MSG_LVL_MED)
#define MSG_BUILD_MASK_MSG_SSID_L4IGUANA_REX           (MSG_LVL_MED)
#define MSG_BUILD_MASK_MSG_SSID_L4IGUANA_SMMS          (MSG_LVL_MED)
#define MSG_BUILD_MASK_MSG_SSID_L4IGUANA_FRAMEBUFFER   (MSG_LVL_MED)
#define MSG_BUILD_MASK_MSG_SSID_L4IGUANA_KEYPAD        (MSG_LVL_MED)
#define MSG_BUILD_MASK_MSG_SSID_L4IGUANA_NAMING        (MSG_LVL_MED)
#define MSG_BUILD_MASK_MSG_SSID_L4IGUANA_SDIO          (MSG_LVL_MED)
#define MSG_BUILD_MASK_MSG_SSID_L4IGUANA_SERIAL        (MSG_LVL_MED)
#define MSG_BUILD_MASK_MSG_SSID_L4IGUANA_TIMER         (MSG_LVL_MED)
#define MSG_BUILD_MASK_MSG_SSID_L4IGUANA_TRAMP         (MSG_LVL_MED)

/* L4 AMSS related SSIDs */
#define MSG_BUILD_MASK_MSG_SSID_L4AMSS_QDIAG           (MSG_LVL_MED)
#define MSG_BUILD_MASK_MSG_SSID_L4AMSS_APS             (MSG_LVL_MED)


/* Example: specify a build mask for SSID_FOO */
#if 0
#define MSG_BUILD_MASK_MSG_SSID_FOO       (MSG_LVL_MED | 0x00ff00)
#endif


#endif /* MSGTGT_H */


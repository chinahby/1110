// Local QTV Feature Definitions

#ifndef _QTVINTERNALDEF_
#define _QTVINTERNALDEF_

/* =======================================================================
                              qtvInternalDefs.h
DESCRIPTION
 This file should include #defines (aka FEATURES) which are not customer
 configurable and meant for internal use to QTV. For example during develoment
 we can use some #define to guard a piece of code which will be main lined
 after development.

Copyright(c) 2006 by QUALCOMM, Incorporated. All Rights Reserved.
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/player/playerif/main/latest/inc/qtvinternaldefs.h#9 $
$DateTime: 2010/11/26 00:05:45 $
$Change: 1532566 $


 ========================================================================== */
/* Make sure that customer.h is included already */
#include "customer.h"


//mcast internal features
#ifdef FEATURE_QTV_MCAST
#define FEATURE_QTV_MCAST_IOCTL
#endif

#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES

/* Byte based AV Sync support for EVRC codec */
#if ((defined T_MSM6800 || defined T_QSC6085 || defined T_QSC6695))
  #define FEATURE_QTV_CMX_AV_SYNC_BYTES_EVRC
#endif

/* Byte based AV Sync support for QCELP codec */
#if(defined T_QSC6085 || defined T_QSC6695)
  #define FEATURE_QTV_CMX_AV_SYNC_BYTES_QCELP
#endif

/* Byte based AV Sync support for QCELP codec */
#if(defined T_MSM7200)
  #define FEATURE_QTV_CMX_AV_SYNC_BYTES_QCELP
#endif

#if(defined T_MSM7200)
  #define FEATURE_QTV_CMX_AV_SYNC_BYTES_MP3
#endif

  /* byte-based AV Sync */
  #if(defined FEATURE_AAC)
    #define FEATURE_QTV_CMX_AV_SYNC_BYTES_AAC
  #endif
#if((defined FEATURE_QTV_3GPP_AMR_WB || defined FEATURE_QTV_3GPP_AMR_WB_PLUS))
#error code not present
#endif /* (defined FEATURE_QTV_3GPP_AMR_WB || defined FEATURE_QTV_3GPP_AMR_WB_PLUS) && defined T_MSM6280) */

#define FEATURE_QTV_CMX_AV_SYNC_BYTES_AMR

#ifdef FEATURE_QTV_3GPP_EVRC_NB
#error code not present
#endif /* FEATURE_QTV_3GPP_EVRC_NB */

#ifdef FEATURE_QTV_3GPP_EVRC_WB
#error code not present
#endif /* FEATURE_QTV_3GPP_EVRC_WB */

#ifdef FEATURE_QTV_AVI_AC3
#error code not present
#endif /* FEATURE_QTV_AVI_AC3 */

#ifdef FEATURE_QTV_PCM
#error code not present
#endif /* FEATURE_QTV_PCM */

#endif /*FEATURE_QTV_CMX_AV_SYNC_BYTES */

#ifndef PLATFORM_LTK
#define FEATURE_QTV_GENERIC_AUDIO_FORMAT
#else
#error code not present
#endif

/* When this feature is defined, pause is mapped to a player suspend. OEMs can
   change this behavior by modifying the config item QTVCONFIG_PAUSE_IS_SUSPEND. */
#define FEATURE_ENABLE_PAUSE_IS_SUSPEND


/* This feature is only used in Bcast FLO code. When defined, it means that
 * the Media implementation is responsible for selecting tracks instead of the
 * player, and other changes as a result of Alt-Tracks are present. It is so
 * that we can use mainline Bcast FLO code with the 7500/5.1 QTV branch.
 */
#define FEATURE_QTV_MEDIA_TRACK_SELECTION

#ifdef FEATURE_VIDEO_PLAYER_INTERFACE_REV_2A_XSCALE
#define FEATURE_QTV_DECODER_XSCALE_VIDEO
#endif /* FEATURE_VIDEO_PLAYER_INTERFACE_REV_2A_XSCALE */

/* The location of the STL std:: namespace changes with compiler version,
 * and the manner in which it must be referenced changes.  This define
 * allows code to consistently use STD:: to get there. */
#if ( 120000 <= __ARMCC_VERSION && __ARMCC_VERSION < 200000 ) /* ADS 12 */
#define STD
#elif ( 210000 <= __ARMCC_VERSION && __ARMCC_VERSION < 230000 ) /* RVCT 2.1 or 2.2 */
#define STD std
#elif (defined PLATFORM_LTK || defined FEATURE_WINCE)
#define STD std
#else
#error unknown target, can not find STL!
#endif

#endif /* _QTVINTERNALDEF_ */

#ifndef CUSTMMODE_H
#define CUSTMMODE_H
/*========================================================================

DESCRIPTION

  Configuration CUST file for all of Multi Mode code

  Copyright (c) 2009  by QUALCOMM Incorporated. All Rights Reserved.
========================================================================*/


/*========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/mmode/build/rel/08H1_QSC1110_3.X/cust/inc/custmmode.h#2 $

when       who   what, where, why
--------   ---   ---------------------------------------------------------
03/03/11   sg    Enable FEATURE_MMOC_TMC_HEAP, for all targets.
06/01/09   sv    Enable HICPS feature to Sync GSM and HDR protocol ACQ's
                 when UOOS is not enabled
04/09/09   aj    Add FEATURE_CM_SS_MEAS
04/02/09   aj    Add FEATURE_CM_GAN
03/20/09   ks    Removing feature flag FEATURE_CM_DELAY_SETUPRES because tear 
                 down of sec pdp due to MT voice call is no more valid
12/08/08   sv    Corrected FEATURE_EGPRS_IND
12/04/08   sv    Added FEATURE_EGPRS_IND 
11/18/08   am    Added FEATURE_CM_MMGSDI
10/27/08   rk    Added feature FEATURE_PS_DORMANT_PWR_SAVE
10/17/08   sv    Added FEATURE_MMODE_LOW_MEM_TARGET to consolidate all the 
                 Low Mem features in CM/SD/MMOC.
09/30/08   fj    Removed FEATURE_SD_DETECT_HDR_HW.
09/02/08   ak    Added feature FEATURE_UNIFORM_SCAN_OOS_HDR_ENH.
08/15/08   sv    Added internal feature FEATURE_CM_MVS_API
05/09/08   sv    Added feature FEATURE_CM_DETECT_HW under FEATURE_MSM_SUPPORT
04/24/08   rm    Removing feature flag FEATURE_CM_NAS_SERVICE_REQ_TYPE
04/10/08   cl    Added FEATURE_CM_NAS_SERVICE_REQ_TYPE and
                 FEATURE_CM_NAS_PH_STAT_CHGD features
03/19/08   rm    Removed FEATURE_CM_NAS_SERVICE_REQ_TYPE for CFG_GW
03/14/08   rm    Added FEATURE_CM_NAS_SERVICE_REQ_TYPE for CFG_GW
02/20/08   ic    Renamed FEATURE_MSMSUPPORT to FEATURE_MSM_SUPPORT.
                 Added FEATURE_MMOC_SELECTIVE_ACTIVATION under
                 FEATURE_MSM_SUPPORT
12/19/07   ic    Turn on FEATURE_MM_SUPERSET for targets that use RPCs
12/11/07   jqi   Remove FEATURE_IMS_VCC. It shall not be enabled in
                 CUSTMM.H file
12/11/07   rn    Added required #ifdef around FEATURE_IMS_VCC
11/20/07   ic    Added FEATURE_LIMITED_MANUAL_ACQ
11/20/07   ic    Added FEATURE_LIMITED_MANUAL_ACQ
11/15/07   ic    Put FEATURE_SD_DETECT_HDR_HW under FEATURE_MSMSUPPORT
11/13/07   ic    Put FEATURE_GW_EMERG_SRV_CATEG under WCDMA and GSM flags
11/01/07   ic    Defined FEATURE_ADD_GW_IF_IS683C_PRL for UMTS builds
10/31/07   ic    Defined FEATURE_ADD_GW_IF_IS683C_PRL for 1x+GSM builds
10/03/07   bh    Merged changes associated with new API in SD
08/16/07   ic    Initial revision.
                 Current custcm.h converted into custmm.h
========================================================================*/

/* -----------------------------------------------------------------------
** Multi Mode sanity
** ---------------------------------------------------------------------*/

  /*
  ** Make sure custmmode.h is used
  ** MMODE code checks for this flag and throws an error if not defined
  */
  #define FEATURE_CUSTMMODE_H

  /* Make sure custcm.h is no longer used
  ** Got to check for
  ** - build local copy (defines CUSTCM_H only)
  ** - obsolete custcm.h file from CM VU (defines both CUSTCM_H and
  **   FEATURE_CM_CUSTCM_H)
  */
  #if (defined CUSTCM_H) || (defined FEATURE_CM_CUSTCM_H)
  #error custcm.h is obsolete, use custmmode.h instead !!!
  #endif

/* -----------------------------------------------------------------------
** Multi Mode general features
** ---------------------------------------------------------------------*/

  /* Enable FEATURE_MM_SUPERSET for targets that use RPCs and thus
  ** need remote API files.
  ** This flag being on results in uniform remote API files for MMODE
  ** as even 1X-only targets will then pull in NAS files that CM uses in
  ** its API
  */
  #ifdef FEATURE_MULTIPROCESSOR
#error code not present
  #endif

  /* Enable UOOS for 1X targets
  ** UOOS for GW targets is controlled via FEATURE_UMTS_UNIFORM_OOS_HANDLING
  ** in custusurf.h of respective targets
  */
  #ifdef FEATURE_CDMA1X
  #define FEATURE_UNIFORM_SCAN_OOS
  #define FEATURE_UNIFORM_SCAN_OOS_HDR_ENH
  #endif

  #if defined(FEATURE_MVS_MIGRATE) || defined(FEATURE_8K_STUFF_TO_BE_PURGED)
  #define FEATURE_CM_MVS_API
  #endif

  /* Enable this feature to reduce memory in all the modules
  ** CM/SD and MMOC
  */
  #ifdef FEATURE_MMODE_LOW_MEM_TARGET
  #define FEATURE_CM_LOW_MEM_TARGET
  #define FEATURE_SD_LOW_MEM_TARGET
  #define FEATURE_MMOC_LOW_MEM_TARGET
  #endif

  /* Enable HICPS feature to Sync GSM and HDR protocol ACQ's
  ** when UOOS is not enabled.
  */
  #if !(defined(FEATURE_UNIFORM_SCAN_OOS) || defined(FEATURE_UMTS_UNIFORM_OOS_HANDLING)) && \
       (defined(FEATURE_HDR) && ((defined FEATURE_WCDMA) || (defined FEATURE_GSM)))
    #define FEATURE_HICPS_STACK_SYNC_UP
  #endif
     

/* -----------------------------------------------------------------------
** Call Manager
** ---------------------------------------------------------------------*/

  /* Call Manager is used.
  ** Feature flag has to remain as REX and TMC code uses it.
  */
  #define FEATURE_CM

  /* Legacy feature - OEMPhone.c etc uses it to distinguish what CM API to
  ** call (gotta clean up)
  */
  #define FEATURE_NEWCM

  /* Enables CM clients to interface with CM3.0.
  ** (this should be always turned on going forward as we will be using
  ** only CM3.0 and shim layer will not be supported)
  ** Required for OEM
  */
  #define FEATURE_CM_MULTIMODE

  /* Turn on/off misc CM_ASSERT() assertions and extra debug info
  */
  #ifdef  CM_UNIT_TEST
  #define CM_DEBUG
  #else
  #undef  CM_DEBUG
  #endif

  /* Enable CM Powerup-hold-origination feature to hold origination
  ** right after powerup giving service a chance to appear.
  */
  #define FEATURE_CM_PWRUP_HOLD_ORIG

  /* Enable CM Preferred-service-hold-origination feature to hold
  ** origination after powerup or mode preference change giving preferred
  ** service a chance to appears once sufficient service is available.
  */
  #define FEATURE_CM_PREF_SRV_HOLD_ORIG

  /* Enable CM Silent redial feature to silently redial failed calls or
  ** to ride through limited periods of no service.
  */
  #define FEATURE_CM_SILENT_REDIAL

  /* Features that need to be defined on build to enable HDR meas */
  #ifdef FEATURE_IRAT_PILOT_MEAS
#error code not present
  #endif 

  #define FEATURE_MMOC_TMC_HEAP

  /*
  ** Multi Mode features that are WCDMA or GSM specific
  */
  #if (defined FEATURE_WCDMA) || (defined FEATURE_GSM)
#error code not present
  #endif /* (defined FEATURE_WCDMA) || (defined FEATURE_GSM) */

  /*
  ** Multi Mode features that are WCDMA and GSM specific
  */
  #if (defined FEATURE_WCDMA) && (defined FEATURE_GSM)
#error code not present
  #endif /* (defined FEATURE_WCDMA) && (defined FEATURE_GSM) */

  /* Feature to support HW detection.
  */
  #ifdef FEATURE_MSM_SUPPORT

    #define FEATURE_CM_DETECT_HW

  #endif /* FEATURE_MSM_SUPPORT */

  #if defined(FEATURE_MMGSDI)

  /*
  ** Feature switch on changes done to replace gsdi to MMGSDI
  */
  #define FEATURE_CM_MMGSDI

  #endif /* FEATURE_MMGSDI */


/* --------------------------------------------------------------------------
** System Determination
** ------------------------------------------------------------------------*/

  /* Feature to enable loading of the PRL
  */
  #define FEATURE_PREFERRED_ROAMING

  /* Add GSM/UMTS acquistion rec when IS683C formate PRL is used in MS
  */
  #if (defined FEATURE_WCDMA) || (defined FEATURE_GSM)
#error code not present
  #endif

  #if (defined FEATURE_WCDMA) || (defined FEATURE_GSM)
#error code not present
  #endif

/* --------------------------------------------------------------------------
** Multi Mode Controller
** ------------------------------------------------------------------------*/

  /*
  ** Multimode Controller run time hardware detection to support GSM-UMTS
  ** only targets with multimode image (1x+EVDO+GSM+UMTS)
  */
  #ifdef FEATURE_MSM_SUPPORT
  #define FEATURE_MMOC_SELECTIVE_ACTIVATION
  #endif

  #ifndef FEATURE_BSR2
  #define FEATURE_BSR2
  #endif

  #ifndef FEATURE_SD_GW_SINGLE_BAND_GROUP
  #define FEATURE_SD_GW_SINGLE_BAND_GROUP
  #endif

#endif /* CUSTMMODE_H */


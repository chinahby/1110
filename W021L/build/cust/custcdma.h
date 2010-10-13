#ifndef CUSTCDMA_H
#define CUSTCDMA_H
/*===========================================================================

DESCRIPTION
  Configuration for CDMA and AMPS Operation

  Copyright (c) 2002 through 2009  by QUALCOMM Incorporated. All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/1x/build/rel/ulc/custcdma.h#7 $ $DateTime: 2009/11/08 22:56:49 $ $Author: ssaranu $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/09/09   ss      Defined FEATURE_DH_EXP
06/03/09   ag      Defined FEATURE_OTASP_STATUS
03/30/09   pk      Defined FEATURE_HWID_SUPPORT
06/18/08   vlc     To save memory, undefined FEATURE_RL_JUMP_ENHANCEMENT and
                   defined FEATURE_MEM_REDUCE_PRL, FEATURE_CP_MEM_REDUCTION
                   and FEATURE_MC_QUEUE_WATERMARK.
01/23/08   vlc     Removed FEATURE_IS2000_P2 and FEATURE_IS2000_CHS.
11/06/07   vlc     Removed FEATURE_IS2000_REL_A for QSC1100 target.
                   Added FEATURE_PRE_DV_RLP_INTERFACE for QSC1100 target.
09/27/07   pg      Defined FEATURE_AUTH
08/21/07   pm      Defined FEATURE_AUTH_DIGITS
11/02/05   fc      Added MOB_P_REV support for FEATURE_IS2000_REL_C_MI.
09/07/05   fc      Added MOB_P_REV support for FEATURE_IS2000_REL_D.
07/19/05   vlc     Defined FEATURE_RL_JUMP_ENHANCEMENT.
05/31/05   grl     Defined FEATURE_MC_SUPPORTS_SHDR.
07/02/04   st      Defined feature FEATURE_RXTX_TC_MSG_NOTIFY.
03/01/04   yll     Added support for Band Class 3 and 6.
09/08/03   aaj     Defined feature FEATURE_CDMA_RX_DIVERSITY 
08/20/03   aaj     Included feature FEATURE_CDMA_RX_DIVERSITY
11/11/02   nxb     Added FEATURE_BUILD_CDMA_CELL_PCS.
05/01/02   jct     Created
===========================================================================*/

/* -----------------------------------------------------------------------
** Top Level CDMA Call Processing Mode Feature Selection
** ----------------------------------------------------------------------- */
#if (defined (FEATURE_BUILD_CDMA_CELL) && !defined (FEATURE_BUILD_JCDMA))

   /* Supports CDMA at 800 MHz
   */
   #define FEATURE_CDMA_800

#elif defined FEATURE_BUILD_CDMA_CELL_AMPS

   /* Supports CDMA at 800 MHz with AMPS
   */
   #define FEATURE_CDMA_800
   /* Enable Analog Call Processing.
   */
#ifndef T_MSM6500
#error code not present
#endif
   
#elif (defined (FEATURE_BUILD_JCDMA) && !defined (FEATURE_BUILD_CDMA_PCS))

   /* Supports CDMA at 800 MHz with JCDMA
   */
   #define  FEATURE_CDMA_800
   #include "custjcdma.h"
   
#elif (defined (FEATURE_BUILD_CDMA_PCS) && !defined (FEATURE_BUILD_JCDMA))

   /* Supports CDMA at 1900 MHz
   */
   #define FEATURE_CDMA_1900

#elif (defined (FEATURE_BUILD_CDMA_PCS) && defined (FEATURE_BUILD_JCDMA))

   /* Supports CDMA at 800 MHz with JCDMA
   */
   #define  FEATURE_CDMA_800
   #include "custjcdma.h"

   /* Supports CDMA at 1900 MHz
   */
   #define FEATURE_CDMA_1900
   
#elif defined FEATURE_BUILD_CDMA_TRIMODE

   /* Supports CDMA at 800  MHz
   **          CDMA at 1900 Mhz
   **          AMPS
   */
   #define FEATURE_CDMA_800
   #define FEATURE_CDMA_1900
   /* Enable Analog Call Processing.
   */
#ifndef T_MSM6500
#error code not present
#endif
   
#elif defined FEATURE_BUILD_CDMA_CELL_PCS

   /* Supports CDMA at 800  MHz
   **          CDMA at 1900 Mhz
   */
   #define FEATURE_CDMA_800
   #define FEATURE_CDMA_1900
   
#else
   #error Must specify a FEATURE_BUILD_CDMA_xxx option
#endif


/* -----------------------------------------------------------------------
** General CDMA Call Processing
** ----------------------------------------------------------------------- */

/* Feature to enable some test code to demonstrate how Packet Zone Id change
** handler is used.  This feature should be turned off by the customers.
*/
#undef FEATURE_PZID_CHANGE_EXAMPLE

/* Rate determination adjustments for SCH DTX detection
*/
#define FEATURE_RDA_CORRECTION

/* Define Power Class III SCM presentation
*/
#define FEATURE_POWER_CLASS_III

/* Preferred Roaming
*/
#define  FEATURE_PREFERRED_ROAMING

/* These features need to be turned on to enable all the functionality needed for
** FEATURE_OTASP.
*/

/* Enables code to process a roaming list with the format specified in 
** IS 683A. Otherwise, the code will process a roaming list with the format 
** specified in IS 683.
*/
#define  FEATURE_IS683A_PRL
/* Allows a mode that restricts the phone to home systems only.
*/
#define  FEATURE_HOME_ONLY

/* This feature is required to ensure the proper functionality of the preferred roaming
** code, for both PCS and SSPR 800 builds.
** Implements special processing in the event of MAX ACCESS PROBES on the 
** Access channel to prevent the phone from returning immediately to the
** system, because it is probably not usable.  
*/
#define  FEATURE_MAX_ACCESS_FALLBACK

/* GSRM Service Indicator
*/
#define  FEATURE_GSRM_SVC_IND

/* EVRC SO Management
*/
#define  FEATURE_EVRC_SO_MGMT

/* EVRC Rate Determination Improvement
*/
#define  FEATURE_EVRC_RDA

/*  Class 1 IMSI support
*/
#define FEATURE_CLASS_1_IMSI

/*  Emergency 911
*/
#define FEATURE_E911

/*  Use faster algorithm to scan through the PRL when a system is acquired
*/
#define FEATURE_FASTPRL

/*  Wildcard System ID Management
*/
#undef FEATURE_WCARD_SID_MGMT


/* -----------------------------------------------------------------------
** IS-95B
** ----------------------------------------------------------------------- */
#define FEATURE_IS95B

#ifdef FEATURE_IS95B
  /*---------------------------------------*/
  /* Enable the individual IS-95B features */
  /*---------------------------------------*/

  /* IS95B compliant version of True IMSI
  ** This is included only if FEATURE_IS95B is included
  */
  #define FEATURE_IS95B_TRUE_IMSI

  /* IS95B compliant version of System Reselection
  ** This is included only if FEATURE_IS95B is included
  */
  #define FEATURE_IS95B_SYS_RESELEC

  /* IS95B compliant version of New Soft Handoff Algorithm
  ** This is included only if FEATURE_IS95B is included
  */
  #define FEATURE_IS95B_SOFT_HO

  /* IS95B compliant version of Calling NAme Presentation (CNAP)
  ** This is included only if FEATURE_IS95B is included
  */
  #define FEATURE_IS95B_EXT_DISP

  /* IS95B compliant version of Access Handoff
  ** This is included only if FEATURE_IS95B is included
  */
  #define FEATURE_IS95B_ACCESS_HO

  /* IS95B compliant version of Pilot Reporting
  ** This is included only if FEATURE_IS95B is included
  */
  #define FEATURE_IS95B_PILOT_RPT

  /* IS95B compliant version of Neighbor Search Enhancements
  ** This is included only if FEATURE_IS95B is included
  */
  #define FEATURE_IS95B_NGHBR_SRCH_ENH

  /* IS95B compliant version of the Release Order on Access channel
  ** This is included only if FEATURE_IS95B is included
  */
  #define FEATURE_IS95B_RELEASE_ON_AC

  /* IS95B compliant version of the Periodic Pilot Strength Measurement Msg
  ** This is included only if FEATURE_IS95B is included
  */
  #define FEATURE_IS95B_PPSMM

  /* IS95B compliant version of Network Directed System Selection (NDSS)
  ** This is included only if FEATURE_IS95B is included
  */
  #define FEATURE_IS95B_NDSS

  /* IS95B compliant version of the Incoming Call Forwarding Enhancement
  ** This is included only if FEATURE_IS95B is included
  */
  #define FEATURE_IS95B_INC_CALL_FWD_ENH

  /* IS95B compliant version of the Alert Answer Bypass Feature
  ** This is included only if FEATURE_IS95B is included
  */
  #define FEATURE_IS95B_ALERT_ANS_BYPASS

  /*-----------------------------------------------------------*/
  /* Enable the individual IS-95B Protocol Revision 5 features */
  /* These are optional for Revsion 4                          */
  /*-----------------------------------------------------------*/

  /* IS95B compliant version of Mobile Assisted Hard Handoff feature.
  ** This is included only if FEATURE_IS95B is included
  */
  #define FEATURE_IS95B_MAHHO

  /*-----------------------------------------------------------*/
  /* Enable the optional IS-95B Protocol Revision features     */
  /*-----------------------------------------------------------*/

  /* IS95B compliant version of Access Entry Handoff
  ** This is included only if FEATURE_IS95B is included
  */
  #define FEATURE_IS95B_ACCESS_ENTRY_HO

  /* IS95B compliant version of Access Probe Handoff
  ** This is included only if FEATURE_IS95B is included
  */
  #define FEATURE_IS95B_ACCESS_PROBE_HO
  
  /* TMSI support is required for IS-95B Band Class 0
  ** This is included only if FEATURE_IS95B is included
  */
  #define FEATURE_TMSI

  /*-----------------------------------------------------------*/
  /* End of IS-95B Feature Definitions                         */
  /*-----------------------------------------------------------*/

#endif /* FEATURE_IS95B */

/* -----------------------------------------------------------------------
** IS-2000
** ----------------------------------------------------------------------- */
#define FEATURE_IS2000

#ifdef FEATURE_IS2000

  /* Fast Forward Power Control
  */
  #define FEATURE_FFPC

  /* Quick Paging Channel
  */
  #define FEATURE_IS2000_QPCH

  /* Quick Paging Channel - Configuration Change Indicators
  */
  #define FEATURE_IS2000_QPCH_CCI

  /* Reverse Link Gain Control
  */
  #define FEATURE_RLGC

  /* LTU CRC checking for conv. coded SCH.  Can heavily load CPU
  */
  #define FEATURE_IS2000_LTU_CRCS

  /* Data related features
  */
  #ifdef FEATURE_DS
   /*
    * Featurizes data code for IS-2000. 
    * Data Services support for IS-2000 data, such as RLP3.
   */
    #define FEATURE_DS_IS2000
    /* This is the new RLP for IS-2000
    */
    #define FEATURE_DS_RLP3

    #ifdef FEATURE_DS_IS2000

      //Supplemental Channel
      #define FEATURE_IS2000_SCH

      #ifdef FEATURE_IS2000_SCH

        // Forward supplemental channel
        #define FEATURE_IS2000_F_SCH

        // Reverse supplemental channel
        #define FEATURE_IS2000_R_SCH

      #endif

      // Supplemental Channel MUX statistics
      #define FEATURE_IS2000_SCH_STATS
    #endif

    /* Enables logging of RLP3 frames. 
     * Supports logging of RLP Rx and Tx frames for IS-2000.
    */
    #define FEATURE_RLP_LOGGING
  #endif

  #ifdef FEATURE_IS2000_P2
    /* DCCH DTX using DCCH Frame Quality Indicator */
    #define FEATURE_DCCH_FRAME_QUALITY_SUPERVISION
    /* The enhanced DCCH DTX logic is based on the Power Control Bit
     * energy(PC_ACC), CRC, SER, Frame Energy for frame classification.
     * It declares a GOOD frame when CRC passes; otherwise the ratio
     * between the PC_ACC value and the FRAME_ENERGY is used to detect
     * if the data bits are present and based on the comparison between
     * the ratio and SER value, the frame is declared as DTX or ERASURE.
    */
    #define FEATURE_ENHANCE_DCCH_DTX_DETECTION
  #endif
  /* Enables the reduction of the mobile's maximum total reverse 
   * power as a function of a state vector.
  */
  #define FEATURE_TX_POWER_BACK_OFF

  #ifdef FEATURE_BUILD_JCDMA
     #undef FEATURE_BUILD_DS_MIP
  #endif /* FEATURE_BUILD_JCDMA */
  
  #ifdef FEATURE_BUILD_DS_MIP
     /* Mobile IP
     */
     #define FEATURE_DS_MOBILE_IP

     /* Mobile IP Performance Metrics
     */
     #define FEATURE_DS_MOBILE_IP_PERF
  #endif

  /* RS2 Packet Data support - IS-2000 only
  */
  #define FEATURE_DS_IS2000_RS2

  /* Test Data Service Option Support
  */
  #define FEATURE_IS2000_TDSO

  /* Bias triage towards pilots with supplemental channels
  */
  #define FEATURE_SCH_TRIAGE

  /* Max Power Protection on the SCH
  */
  #define FEATURE_MAX_PWR_PROTECTION

  /* Handle cases of SO/P_REV mismatch caused by P_REV 5 in Sync
  ** Channel for 1x systems.
  */
  #define FEATURE_VERIFY_DS_SO_ON_ORIG
  /*
  * Causes update of the P_REV information in use during the paging
  * channel state.
  */
  #define FEATURE_UPDATE_P_REV_IN_PCH

  /*turn on for backward compatability*/
  #define FEATURE_IS2000_SLOTTED_TIMER
  /* Enable the IS-2000 Release 0 Reverse FCH 1/8 rate gating feature.
  */
  #define FEATURE_IS2000_REV_FCH_GATING

  /* Enable RL jump to hyperspace enhancement to save call set up time.
  */
  #undef  FEATURE_RL_JUMP_ENHANCEMENT

#endif /* FEATURE_IS2000 */

/* Enables management of all service options by SNM
*/
#define FEATURE_COMPLETE_SNM

/* This feature changes the rxc/txc task's way of processing traffic channel
** bit streams. The changes are centered around the viterbi decoder ISR and
** convolutional encoder isr.
*/
#define FEATURE_MINIMIZE_ISR_LOADING

/* Perform AGC/power control logging
*/
#define FEATURE_AGC_LOGGING

#ifndef T_QSC60X5
#error code not present
#endif // #ifndef T_QSC60X5

/* ----------------------------------------------------------------------
** P_REV definition
** ----------------------------------------------------------------------- */
#ifdef FEATURE_IS2000_REL_D
#error code not present
#elif defined ( FEATURE_IS2000_REL_C_MI )
#error code not present
#elif defined ( FEATURE_IS2000_REL_C )

   /* Pull in Release A specific definitions
   */
   #include "custrela.h"

   #define MOB_P_REV 9

#elif defined ( FEATURE_IS2000_REL_B )
   
   /* Pull in Release A specific definitions
   */
   #include "custrela.h"

   #define MOB_P_REV 8

#elif defined ( FEATURE_IS2000_REL_A )

   /* Pull in Release A specific definitions
   */
   #include "custrela.h"

   /* MOB_P_REV definition needed for run-time P_REV checking
   ** It is defined as 7 for IS-2000 Rel A compliant mobiles
   */
   #define MOB_P_REV 7

#elif defined (FEATURE_IS2000)
   
   /* MOB_P_REV definition needed for run-time P_REV checking
   ** It is defined as 6 for IS-2000 Rel 0 compliant mobiles
   */
   #define MOB_P_REV 6
   
#elif defined (FEATURE_IS95B)
  
   /* MOB_P_REV definition needed for run-time P_REV checking
   ** It is defined as 4 for IS-95B compliant mobiles
   */
   #define MOB_P_REV 4
  
#else
   /* MOB_P_REV definition needed for run-time P_REV checking
   ** It is defined as 3 to assist the P_REV1+ solution for MDR
   */
   #define MOB_P_REV 3
   
#endif

/* -----------------------------------------------------------------------
** AMPS
** ----------------------------------------------------------------------- */
#ifdef FEATURE_ACP
#error code not present
#endif

/* -----------------------------------------------------------------------
** CDMA at 800MHz Specific
** ----------------------------------------------------------------------- */
#ifdef FEATURE_CDMA_800
   /* This feature allows the use of a roaming list as defined in IS-683A.
   ** SSPR_800 can operate without a roaming list, in which case it behaves similar
   ** to the old algorithm, but not exactly the same.
   */
   #define FEATURE_SSPR_800
   
   /* These features need to be turned on to enable all the functionality needed for
   ** FEATURE_OTASP.
   ** Allows the phone to be forced into a specific mode 
   ** (for example, "DIGITAL_ONLY"). 
   */
   #define  FEATURE_PRL_FORCE_MODE
   
   /* Needs to be turned on to enable all the functionality needed for FEATURE_SSPR_800.
   */
   #define  FEATURE_SSPR_ENHANCEMENTS

#endif

#if  !defined (FEATURE_BUILD_CDMA_PCS)
#if defined FEATURE_BUILD_CDMA_CELL     || \
    defined FEATURE_BUILD_JCDMA || \
    defined FEATURE_BUILD_CDMA_CELL_AMPS
      /* Feature to prevent roam side available systems from being acquired.
       * For 800 MHz band carriers in markets where A and B systems 
       * are present. If the carrier uses A band, this feature prevents
       * the phone from acquiring B band systems, unless they are
       * explicitly listed in the PRL or CDMA SID/NID list.
      */
      #define FEATURE_AVAIL_HOME_SIDE_PREF
#endif   
#endif /*  !defined (FEATURE_BUILD_CDMA_PCS) */
#if defined FEATURE_BUILD_CDMA_TRIMODE
   /* Enable MC silent redial in AMPS
   */
   #define FEATURE_MC_SILENT_REDIAL_AMPS
#endif

/* -----------------------------------------------------------------------
** OTASP/OTAPA 
** ----------------------------------------------------------------------- */

/* Phone will do OTASP Protocol
** This feature includes full compliance with IS-683A except for OTAPA.
*/
#define  FEATURE_OTASP

/* Unselect non-standard OTASP implementation.
*/
#undef FEATURE_NSOTASP

/* OTASP_OTAPA
*/
#if defined(FEATURE_OTASP) && !defined(FEATURE_NSOTASP)
   /* Support for Over the Air Authentication
    * OTAPA is the network-initiated portion of IS-683A OTASP.
   */
   #define FEATURE_OTASP_OTAPA

   /* Hold the ending command while an OTASP commit is in progress.
    * CM keeps user's call-end and call-origination commands from
    * being sent to CP while an OTASP commit is in process.
   */
   #define FEATURE_OTASP_HOLD_CMD_IN_COMMIT

   /* Support for sending OTASP STATUS to CM
   */
   #define FEATURE_OTASP_STATUS
#endif

/* Enable the Diffie-Helman task.
 * Used by OTASP to perform AKey exchange. Enables the task and code
 * to send commands to and receive reports from the task.
*/
#define FEATURE_DH

/* FEATURE_DH_EXP functionality is used when phone operates in NV mode
 *
 * Phone can operate in NV mode in following cases:
 *        1) When RUIM feature is not defined.
 *		  2) When RUIM feature is defined but RTRE setting is possible
 *
 * It is confirmed from RUIM team that FEATURE_UIM_RUN_TIME_ENABLE feature is always
 * defined whenever FEATURE_UIM_RUIM feature is defined.
 * FEATURE_DH_EXP used to be defined by target .builds file when that target 
 * is hitting the above two cases (1 & 2 above).
 * Now that it is established RTRE capability is by default turned ON, FEATURE_DH_EXP
 * is being defined by default for all cases.
 */
#define FEATURE_DH_EXP

/* CDMA Rx diversity feature */
#define FEATURE_CDMA_RX_DIVERSITY

/* New support for TC message notification from CP to SRCH. */
#define FEATURE_RXTX_TC_MSG_NOTIFY 

/* Supports revised searcher API that among other things supports SHDR */
#define FEATURE_MC_SUPPORTS_SHDR

#define FEATURE_AUTH
#define FEATURE_AUTH_DIGITS

#define FEATURE_PRE_DV_RLP_INTERFACE

/* Features related to support memory reduction for ULC */
#define FEATURE_MEM_REDUCE_PRL
#define FEATURE_CP_MEM_REDUCTION
#define FEATURE_MC_QUEUE_WATERMARK

/* Feature to support Hardware Id retrieval feature for ULC */
#define FEATURE_HWID_SUPPORT

#endif /* CUSTCDMA_H */


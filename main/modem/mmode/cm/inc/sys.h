#ifndef SYS_H
#define SYS_H

/**===========================================================================

              S Y S T E M   H E A D E R   F I L E

@DESCRIPTION
  This header file contains definitions that are shared between Call Manager,
  Call Manager clients and the protocol stacks.


  Organization of the file:

    The file is divided into multiple sections. Each section contains
    type definitions for a particular entity.  The following entities
    are defined in this file:

        Service Indicators
        System Identifiers
        Network Lists
        Mobility Management Information
        Menu Selection
        BCMCS

    Each section is further subdivided to represent different types of
    information.  The types of information include:

        #defines
        enumerated types
        data types
        functions

  NOTE:
    Please do not change the order of the enumerations.  If a new
    enumeration value is needed, add to the end of the enumerated
    type.  Other code may be dependent on the existing order.

Copyright (c) 2009 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/
#ifndef AEE_SIMULATOR
#include "customer.h"  /* Customer configuration file                   */
#include "comdef.h"    /* Common definitions such as byte, uint16, etc. */
#endif




/**===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/cm/rel/08H1_QSC1110_3.X/inc/sys.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/19/09   rm      Adding new feature defintions to have consistent value in 
                   the RPC files  
05/27/09   aj      Adding support for updating clients with cdma local time
05/20/09   ks      Adding more fields into camped cell info structures
01/26/09   fj      Added FEATURE_MM_SUPERSET for data structures.
04/09/09   aj      Added support for IRAT measurements
03/16/09   aj      HDR Rev B support added
03/09/09   rm      Reverting the fix -
                   Adding FEATURE_MM_SUPERSET for some features
01/23/09   rm      Adding FEATURE_MM_SUPERSET for some GW features
01/13/09   ks      Add spare_bits field in the mm_information to indicate 
                   no. of spare bits in end octet of n/w names(full/short)
12/08/08   fj      Added SYS_BAND_MASK_ANY
10/02/08   aj      Add support for BC 17,18,19
09/24/08   jd      Removed duplicate entry of uint32 sys_bcmcs_flow_id_type
09/17/08   ks      When OOS in CELL_FACH, UE needs to follow UOOS scanning 
                   pattern to re-acquire service
09/12/08   fj/aj   Support Android RIL for ARM9 Reset
09/04/08   ks      Remove info for Neighbors Cell IDs
08/19/08   sv      Increased SYS_NETWORK_LIST_INFO_MAX to 25
                   Added security info as part of sys_wlan_bss_info_s_type
07/18/08   ks      Added new info for Current Operator's Cell ID and 
                   Neighbors Cell IDs
07/01/08   rm      Mainlining of MBMS changes
06/18/08   sn      Adding new enum - sys_gw_conn_status_e_type
06/16/08   rk      Adding FEATURE_MM_SUPERSET to remaining feature flags
05/06/08   ak      BCMCS 2.0 changes
04/30/08   cl      Adding support for API sd_misc_sys_avoid_list, which list all  
                   the avoided systems. HDR/SCHR use this info to update their 
                   scan list when doing OFS.
03/27/08   rn/ic   Added support for new command CM_PH_STATUS_CHGD_REQ
03/14/08   ks      Updating UI with service status when UE is OOS in Connected Mode
12/10/07   sju     Renamed HDR system type enum
11/13/07   ak/sju  Added support for HDR revB
08/08/07   cl      Add OTASP Support for ASW Band
06/22/07   sk      Added bit manipulation macros.
06/07/07   rp      Added support for EGPRS indication
06/06/07   pk/cl   Added support for LN status notification from HDR
05/22/07   rp      Changing size of SYS_USER_PPLMN_LIST_MAX_LENGTH
03/09/07   sk      RAM usage reduction.
03/07/07   jqi     Added support for FEATURE_SMS_OVER_IP
01/03/07   pk      Added support for DO VOIP -> 1X Handoff
08/14/06   ka      Adding persistent enum for band pref.
12/13/06   rp      Adding sys_service_search_e_type
11/17/06   jqi     Added support for HDR active protocol revision information.
11/16/06   pk      Corrected meta comment
11/13/06   ic      Lint fixes
11/07/06   pk      Added support for HYBR_GW and UMTS -> 1X handover
11/06/06   hn/ic   Added missing meta-comments.
10/11/06   jqi     Added CDMA 450M(BC5) block L support.
09/18/06   rp      Added BC15 and BC16 support.
09/17/06   sk      Added band field to sys_wlan_manual_sys_info_s_type.
08/28/06   ka      Adding pdp header comp and data comp enums.
08/21/06   ka/rp   Changing SYS_FORBIDDEN_PLMN_LIST_MAX_LENGTH to 20 on 
                   request from NAS team.
07/20/06   mpa     Fixed meta comments for sys_ip_address_s_type.
04/06/06   ka      Adding enums for HSDPA/DTM indications.
04/19/06   pk      Added meta comments for 7200
04/06/06   sk      Added new bcmcs flow status 
                   - SYS_BCMCS_FLOW_STATUS_SYS_UNAVAILABLE
03/10/06   pk      Added support for high PSIST
03/10/06   pk      Added mcc and imsi info. to sys_is95_sys_id_s_type
03/05/06   jqi     Merging the code for WCDMA BC3,4,8,9 support.
02/23/06   ka      Merging code
                   01/31/06  up  SYS_PRIORITY_PLMN_LIST_MAX_LENGTH(256)
                   is added
02/03/06   mpa     Reworked sys_bcmcs_flow_status_e_type().
12/15/05   ka      Name change to SYS_PLMN_LIST_NONE
01/09/06   ic      Under FEATURE_HTORPC_METACOMMENTS, merged in meta comments
                   from //depot/asic/msm7500/orphans/00.04/services/cm/sys.h#6
                   (delta from 5 to 6)
01/06/06   ic      Lint cleanup  
01/05/06   pk      Added numbering for enums
12/22/05   jqi     Merged background PLMN search support.
11/30/05   ka      Increased number of secondaries to 2.
11/17/05   pk      Added support for BC 5 and BC 11 sub blocks I,J,K
10/14/05   jqi     Added BC14 support.
09/20/05   pk      Removed FEATURE_DATA_PS_UNSUPPORTED from "uint64 v6[2]"
                   addr union in sys_ip_address_s_type
08/29/05   ic      Integration of the BCMCS DS to CM to HDRMC path
06/26/05   ic      Lint and compiler warnings cleanup
05/20/05   ic      Mainlined FEATURE_IS683C_PRL
05/12/05   ic      Removed bcmcs_srv_status_is_srv()
05/11/05   ic      Added metacomments under FEATURE_HTORPC_METACOMMENTS
04/13/05   ic      Put RPC comments under FEATURE_HTORPC_METACOMMENTS
04/05/05   pk      Corrected comment for SYS_MSM_ID_SECONDARY in 
                   sys_msm_id_e_type
04/04/05   ic      Added SYS_ARG_NOT_USED() 
03/22/05   up      Function sys_mcc_same_country is added to extend MCC
                   matching for NA MCC range 310-316.
03/09/05   kwa     Removed sys_hplmn_match.
02/02/05   ka      Added band class 12 enums
01/10/05   dk      Added sys_plmn_lac_id_s_type and sys_lac_match().
01/06/05   dk      Increased SYS_USER_PPLMN_LIST_MAX_LENGTH to 50 and
                   set SYS_PLMN_LIST_MAX_LENGTH to 40. Removed
                   SYS_OPERATOR_PPLMN_LIST_MAX_LENGTH.
11/19/04   ic      Corrected typo in sys_sys_mode_e_type
09/08/04   jqi     More ps dormant optimization.
08/24/04   ws      Merge with std.h
08/20/04   sj      Added support for PRL Preference.
08/10/04   sj      Added sys_hplmn_match.
07/30/04   sj      Added new enum for srv_domain_camped.
07/08/04   ws      Added RPC meta comments.
06/03/04   ka      Fixed Lint warning
05/17/04   ic      Added type field in sys_ip_address_s_type and
                   sys_ip_addr_enum_type enum (FEATURE_BCMCS)
05/06/04   jqi     Renamed T32, KPCS, BC5,6 with band mapping features.
                   Increased the size of sys_band_mask_type to 64 bits.
05/06/04   ic      Added commas in sys_band_mask_e_type after UMTS 1700.850,
                   800 band support enums
05/03/04   jqi     Added UMTS 1700.850,800 band support.
04/12/04   ws      Fixed typo.
03/17/04   sj      Added new DDTM related typedefs.
03/02/04   ic      Added BCMCS support (FEATURE_BCMCS)
02/06/04   ws      Initial jaguar/mainline merge.
01/28/04   ka      Added multiple PDP context constants
11/24/03   jqi     Added support for SYS_OPRT_MODE_OFFLINE_IF_NOT_FTM.
01/22/04   prk     Removed extra comma in sys_band_mask_e_type.
10/24/03   ws      Added jqi's changes for band preference enum.
10/24/03   ic      Modified to support common sys.h across targets
10/20/03   ws      Added new limited_regional service status type.
09/30/03   vt      Modified to support common cm.h across all targets.
07/10/03   jqi     Added Extended PRL support
07/01/03   az      Added FEATURE_ENHANCED_BAND_CLASS
05/28/03   vt      Defined sys bitmask macro and sband enum.
05/05/03   kwa     Updated the logic in sys_mm_information_is_valid for
                   determining if an sys_mm_information_s_type is valid to
                   account for optional elements within the structure.
                   Modified several functions to pass pointers rather than
                   structures to minimize stack utilization.
02/14/03   vt      Assigned fix numbers to operating mode enums.
02/03/02   prk     Removed references to SYS_ACT_UNKNOWN_MASK.
01/21/03   ws      Updated copyright information for 2003
12/17/02   vt      Defined fixed numbers to an enums for event logging.
11/27/02   vt      Changed sys_jcdma_m513_e_type back to what it was before.
11/22/02   MKS     Defined data type sys_network_name_coding_scheme_type_e_type
                   for indicating the network name coding scheme. Added
                   coding_scheme & add_country_initials as members of
                   sys_network_short_name_s_type & sys_network_full_name_s_type
11/04/02   ic      Changes to sys_jcdma_m513_e_type
10/22/02   ic      Added enumerations for M51 mode settings
09/03/02   PRK     Added NOTE section in funtion header. Removed enumerator
                   tags to prevent lint warnings.
06/11/02   vt      Restored the original order to operating mode enum. All
                   the new additions to the enums were added at the end.
05/16/02   PRK     Changed constants SYS_USER_PPLMN_LIST_MAX_LENGTH,
                   SYS_OPERATOR_PPLMN_LIST_MAX_LENGTH and
                   SYS_FORBIDDEN_PLMN_LIST_MAX_LENGTH from 64 to 10.
05/02/02   PRK     Added SYS_SRV_STATUS_LIMITED to sys_srv_status_e_type.
                   Removed SYS_SRV_DOMAIN_CS_LIMITED from sys_srv_domain_e_type.
04/01/02   PRK     Initial version
===========================================================================*/


/*------------------------------------------------------------------------------
   General macros
------------------------------------------------------------------------------*/

#define SYS_WLAN_PROFILE_MAX_SIZE  120

/** Macro to convert bit number to bit mask
*/
#define SYS_BM( val ) (1<<(int)(val))

#define SYS_BM_64BIT( val )   ((uint64)1<<(int)(val))

/** Macro to convert an expression to a boolean.
*/
#ifndef BOOLEAN
#define BOOLEAN( val )  ( (val) ? TRUE : FALSE )
#endif

/** Macro to check if bit-mask e1 contains at least a part of e0.
*/
#define SYS_IS_BIT_CONTAIN( e1, e0 ) ( (BOOLEAN ((word) e1 & (word) e0)) )

/** Macro to return bits that are set either in e1 or in e0.
*/
#define SYS_BIT_ADD( e1, e0 )        ( (word) e1 | (word) e0 )

/** Macro to get common bits in both e1 and e0.
*/
#define SYS_BIT_COMMON( e1, e0 )     ( (word) e1 & (word) e0 )

/** Macro to take the bits set in e0 out of e1.
*/
#define SYS_BIT_DELETE( e1, e0 )     ( (word) e1 & ~(word) e0 )


/** 
** Macro to prevent lint warning 715 'Symbol not referenced' 
** (cleaner than lint suppressing comments)
** Have the macro expand with Lint suppression 
** -e774 Boolean within 'if' always evaluates to False/True 
*/
#define SYS_ARG_NOT_USED(arg) /*lint -save -e774*/if(arg){}/*lint -restore*/

/**
** Macro to prevent lint warning 818 'Symbol could be declared as pointing to
** const'.
*/
#define SYS_ARG_NOT_CONST(arg) /*lint -e{717,506,774} */ do{if(0){(*arg)=(*arg);}}while(0)


/** @}
*
*/

/*------------------------------------------------------------------------------
   DDTM related define.
------------------------------------------------------------------------------*/

/** @defgroup DRD DDTM related define
**  @{
*/

#define SYS_DDTM_MAX_SO_LIST_SIZE            10

typedef uint16                        sys_srv_opt_type;


/** Enumeration of DDTM actions to enhance to HDR data thru'put.
*/
typedef enum
{
  SYS_DDTM_ACT_SUPPRESS_L2ACK   = 0,
    /**< Do not send L2 ACK on 1x.
    */

  SYS_DDTM_ACT_SUPPRESS_REG     = 1,
    /**< Suppress 1x registrations.
    */

  SYS_DDTM_ACT_IGN_SO_PAGES     = 2,
    /**< Ignore 1x pages with specified service options.
    */

  SYS_DDTM_ACT_SUPPRESS_MO_DBM  = 3,
    /**< Block MO SMS and DBM
    */  

  SYS_DDTM_ACT_MAX
    /**< FOR INTERNAL USE OF CM ONLY!
    */

} sys_ddtm_act_e_type;

/**
** Enumeration of DDTM action mask.
** It converts bit position of sys_ddtm_act_e_type to a bit mask.
*/
typedef enum {

  SYS_DDTM_ACT_MASK_EMPTY           = 0,
    /**< Empty action mask.
    */

  SYS_DDTM_ACT_MASK_SUPPRESS_L2ACK  = SYS_BM(SYS_DDTM_ACT_SUPPRESS_L2ACK),
    /**< Mask for suppressing L2 acks in 1x.
    */

  SYS_DDTM_ACT_MASK_SUPPRESS_REG    = SYS_BM(SYS_DDTM_ACT_SUPPRESS_REG),
    /**< Mask for suppressing 1x registrations.
    */

  SYS_DDTM_ACT_MASK_IGN_SO_PAGES    = SYS_BM(SYS_DDTM_ACT_IGN_SO_PAGES),
    /**< Mask for ignoring pages with specified service options.
    */

  SYS_DDTM_ACT_MASK_SUPPRESS_MO_DBM = SYS_BM(SYS_DDTM_ACT_SUPPRESS_MO_DBM),
    /**< Mask for blocking MO sms and DBM
    */

  SYS_DDTM_ACT_MASK_ALL =  0x3fff,
    /**< Mask for enabling all DDTM actions.
    */

  SYS_DDTM_ACT_MASK_NO_CHANGE,
    /**< Reserved value for not changing current DDTM action settings.
    */

  SYS_DDTM_ACT_MASK_MAX
    /**< For internal use.
    */

} sys_ddtm_act_mask_e_type;

/** @}
*/

/*------------------------------------------------------------------------------
   Multiple PDP Contexts Defines
------------------------------------------------------------------------------*/

/** @defgroup MPCD Multiple PDP Contexts Defines
*@{
*/

/** Maximum number of primary PDP contexts
*/
#if defined(FEATURE_MULTIPLE_PRIMARY_PDP)
#error code not present
#else
#define SYS_MAX_PRIMARY_PDP_CONTEXTS          1
#endif

/** Maximum number of secondary PDP contexts per primary PDP context
*/
#if defined(FEATURE_SECONDARY_PDP)
#error code not present
#else
#define SYS_MAX_SEC_PDP_CONTEXT_PER_PRIMARY   0
#endif

/** Maximum number of PDP contexts
*/
#define SYS_MAX_PDP_CONTEXTS                                                 \
  ( SYS_MAX_PRIMARY_PDP_CONTEXTS +                                           \
    ( SYS_MAX_PRIMARY_PDP_CONTEXTS * SYS_MAX_SEC_PDP_CONTEXT_PER_PRIMARY ) )

#define SYS_MBMS_MAX_PDP_CONTEXTS_PER_IFACE 5 

#define SYS_MBMS_MAX_IFACE 2 // We support IPV6 and IPV4 Ifaces for MBMS

#define SYS_MBMS_MAX_PDP_CONTEXTS (SYS_MBMS_MAX_IFACE * \
                                   SYS_MBMS_MAX_PDP_CONTEXTS_PER_IFACE)


/* Maximum size of the SSID.
*/
#ifdef FEATURE_WLAN
#error code not present
#else
#define SYS_WLAN_SSID_MAX_SIZE         (int)(1)
#endif

#define SYS_MAX_NW_NAME_SPARE_BITS    7

/**==============================================================================

                     S E R V I C E   I N D I C A T O R S

==============================================================================*/

/*------------------------------------------------------------------------------
   Service Indicators Defines
------------------------------------------------------------------------------*/

/** @defgroup SI Service Indicators
*@{
*/

/** typedef for set window search size.
*/
typedef byte                      sys_srch_win_type;

/** Typedef for base station latitude.
*/
typedef int32                     sys_base_lat_type;

/** Typedef for base station longitude.
*/
typedef int32                     sys_base_long_type;

/** Typedef for base station ID.
*/
typedef word                     sys_base_id_type;

/* Type for WLAN BSS id.
*/
typedef uint64                   sys_wlan_bssid_type;

/*------------------------------------------------------------------------------
   Service Indicators Enums
------------------------------------------------------------------------------*/
/**
** Enumeration of phone measurement modes.
** Currently used only for HDR CP
*/
typedef enum
{
  SYS_MEAS_MODE_NONE = -1,
    /**< FOR INTERNAL USE OF CM ONLY!                     */

  SYS_MEAS_MODE_HDR_OFF = 0,
    /**< HDR enables network access                       */

  SYS_MEAS_MODE_HDR_ON = 1,
    /**< HDR disables network access                      */

  SYS_MEAS_MODE_MAX
    /**< FOR INTERNAL USE OF CM ONLY!                     */

} sys_meas_mode_e_type;


/**
** Enumeration of phone operating modes.
*/
/** For compatibility with QXDM database, please do not change the following
** fix assigned values. If new values are added please also update the QXDM
** database
*/
typedef enum
{
  SYS_OPRT_MODE_NONE = -1,
    /**< FOR INTERNAL USE OF CM ONLY!                     */

  SYS_OPRT_MODE_PWROFF = 0,
    /**< Phone is powering off                            */

  SYS_OPRT_MODE_FTM = 1,
    /**< Phone is in factory test mode                    */

  SYS_OPRT_MODE_OFFLINE = 2,
    /**< Phone is offline                                 */

  SYS_OPRT_MODE_OFFLINE_AMPS = 3,
    /**< Phone is offline analog                          */

  SYS_OPRT_MODE_OFFLINE_CDMA = 4,
    /**< Phone is offline cdma                            */

  SYS_OPRT_MODE_ONLINE = 5,
    /**< Phone is online                                  */

  SYS_OPRT_MODE_LPM = 6,
    /**< Phone is in LPM - Low Power Mode                 */

  SYS_OPRT_MODE_RESET = 7,
    /**< Phone is resetting - i.e. power-cycling          */

  SYS_OPRT_MODE_NET_TEST_GW = 8,
    /**< Phone is conducting network test for GSM/WCDMA.  */
    /**< This mode can NOT be set by the clients. It can  */
    /**< only be set by the lower layers of the stack.    */

  SYS_OPRT_MODE_OFFLINE_IF_NOT_FTM = 9,
    /**< This mode is only set when there is offline      */
    /**< request during powerup.                          */
    /**< This mode can not be set by the clients. It can  */
    /**< only be set by task mode controller.             */

  SYS_OPRT_MODE_PSEUDO_ONLINE = 10,
    /**< Phone is pseudo online, tx disabled              */

  SYS_OPRT_MODE_RESET_MODEM = 11,
    /**< Phone is resetting the modem processor.          */

  SYS_OPRT_MODE_MAX
    /**< FOR INTERNAL USE OF CM ONLY!                     */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} sys_oprt_mode_e_type;


/**
** Enumeration of system modes.
*/
/** For compatibility with QXDM database, please do not change the following
** fix assigned values. If new values are added please also update the QXDM
** database
*/
typedef enum
{
  SYS_SYS_MODE_NONE = -1,
    /**< FOR INTERNAL USE ONLY!                             */

  SYS_SYS_MODE_NO_SRV=0,
    /**< No service = NV_MODE_INACTIVE                      */

  SYS_SYS_MODE_AMPS=1,
    /**< Analog Mobile Phone System (AMPS) mode             */

  SYS_SYS_MODE_CDMA=2,
    /**< Code Division Multiple Access (CDMA) mode          */

  SYS_SYS_MODE_GSM=3,
    /**< Global System for Mobile communications (GSM) mode */

  SYS_SYS_MODE_HDR=4,
    /**< High Data Rate (HDR) mode                          */

  SYS_SYS_MODE_WCDMA=5,
    /**< Wideband CDMA (WCDMA) mode                         */

  SYS_SYS_MODE_GPS=6,
    /**< Global Positioning System (GPS) mode               */

  SYS_SYS_MODE_GW=7,
    /**< GSM and WCDMA mode                                 */

  SYS_SYS_MODE_WLAN=8,
  /**< WLAN mode                                          */

  SYS_SYS_MODE_MAX
    /**< FOR INTERNAL USE ONLY!                             */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */
} sys_sys_mode_e_type;


/** Macro for creating a Bit Mask by shifting 1 left by a specified number -
** useful for defining a bit-mask enum values.
*/
#define BM( val ) ( 1<< ((int)(val)) )


/** Bit Masks of system modes 
** Combination of system modes can be created
** by using the macro ADD_SYSMODE
*/
typedef enum
{

  SYS_SYS_MODE_MASK_NONE        = 0,
                                /**< Acquire no system */

  SYS_SYS_MODE_MASK_NO_SRV      = BM( SYS_SYS_MODE_NO_SRV ),
                                /**< No service */

  SYS_SYS_MODE_MASK_CDMA        = BM( SYS_SYS_MODE_CDMA ),
                                /**< CDMA mode systems only */

  SYS_SYS_MODE_MASK_HDR         = BM( SYS_SYS_MODE_HDR ),
                                /**< Acquire HDR mode systems only */

  SYS_SYS_MODE_MASK_GSM         = BM( SYS_SYS_MODE_GSM ),
                                /**< Acquire GSM systems only */

  SYS_SYS_MODE_MASK_WCDMA       = BM( SYS_SYS_MODE_WCDMA ),
                                /**< Acquire WCDMA systems only */

  SYS_SYS_MODE_MASK_WLAN        = BM( SYS_SYS_MODE_WLAN ),
                                /**< Acquire WLAN systems only */

  SYS_SYS_MODE_MASK_MAX

} sys_sys_mode_mask_e_type;


/**
** Enumeration of system band classes.
*/
typedef enum
{
  SYS_BAND_CLASS_NONE        = -1,
    /**< FOR INTERNAL USE OF CM ONLY!                    */

  SYS_BAND_BC0               = 0,
    /**< Band Class 0 - US Cellular band (800 MHz)       */

  SYS_BAND_BC1               = 1,
    /**< Band Class 1 - US */

  SYS_BAND_BC3               = 3,
    /**< Band Class 3 - Japanese Cellular band (800 MHz) */

  SYS_BAND_BC4               = 4,
    /**< Band Class 4 - Korean PCS band (1800 MHz)       */

  SYS_BAND_BC5               = 5,
    /**< Band Class 5 (450 MHz)                          */

  SYS_BAND_BC6               = 6,
    /**< Band Class 6 (2 GHz)                            */

  SYS_BAND_BC7               = 7,
    /**< Band Class 7 (Upper 700 MHz)                          */

  SYS_BAND_BC8               = 8,
    /**< Band Class 8 (1800 MHz)                         */

  SYS_BAND_BC9               = 9,
    /**< Band Class 9 (900 MHz)                          */

  SYS_BAND_BC10              = 10,
    /**< Band Class 10 (Second 800 MHz)                  */

  SYS_BAND_BC11              = 11,
    /**< Band Class 11 - European PAMR Band (400 MHz)    */

  SYS_BAND_BC12              = 12,
  /**< Band Class 12 - PAMR Band (800 MHz)               */

  SYS_BAND_BC14              = 14,
  /**< Band Class 14 - Band Class 14 (US PCS 1.9GHz Band)*/

  SYS_BAND_BC15              = 15,
  /**< Band Class 15 - Band Class 15 (1700-2100 MHz -AWS)*/

  SYS_BAND_BC16              = 16,
  /**< Band Class 16 - Band Class 16 (US 2.5GHz)         */

  SYS_BAND_BC17              = 17,
  /**< Band Class 17 - Band Class 17 (US 2.5GHz Forward Link only band)         */

  SYS_BAND_BC18              = 18,
  /**< Band Class 18 - Band Class 18 (700 MHz Public Safety Broadband)         */

  SYS_BAND_BC19              = 19,
  /**< Band Class 19 - Band Class 19 (Lower 700 MHz band)         */


    /**< Reserved 20-39 for CDMA band classes. */



  SYS_BAND_GSM_450           = 40,
    /**< GSM 450 band (450 MHz)                          */

  SYS_BAND_GSM_480           = 41,
    /**< GSM 480 band (480 MHz)                          */

  SYS_BAND_GSM_750           = 42,
    /**< GSM 750 band (750 MHz)                          */

  SYS_BAND_GSM_850           = 43,
    /**< GSM 850 band (850 MHz)                          */

  SYS_BAND_GSM_EGSM_900      = 44,
    /**< GSM Extended GSM (E-GSM) 900 band (900 MHz)     */

  SYS_BAND_GSM_PGSM_900      = 45,
    /**< GSM Primary GSM ( P-GSM) 900 band (900 MHz)     */

  SYS_BAND_GSM_RGSM_900      = 46,
    /**< GSM Railways GSM ( R-GSM) 900 band (900 MHz)    */

  SYS_BAND_GSM_DCS_1800      = 47,
    /**< GSM DCS band (1800 MHz)                         */

  SYS_BAND_GSM_PCS_1900      = 48,
    /**< GSM PCS band (1900 Mhz)                         */

    /**< Reserved 49-79 for GSM band classes. */

  SYS_BAND_WCDMA_I_IMT_2000  = 80,
    /**< WCDMA EUROPE JAPAN & CHINA IMT 2100 band        */

  SYS_BAND_WCDMA_II_PCS_1900 = 81,
    /**< WCDMA US PCS 1900 band                          */

  SYS_BAND_WCDMA_III_1700    = 82,
    /**< WCDMA EUROPE&CHINA DCS 1800 band                */

  SYS_BAND_WCDMA_IV_1700     = 83,
    /**< WCDMA US 1700 band                              */

  SYS_BAND_WCDMA_V_850       = 84,
    /**< WCDMA US 850 band                               */

  SYS_BAND_WCDMA_VI_800      = 85,
    /**< WCDMA JAPAN 800 band                            */

  SYS_BAND_WCDMA_VII_2600    = 86,
    /**< WCDMA EUROPE 2600 band                          */

  SYS_BAND_WCDMA_VIII_900    = 87,
    /**< WCDMA EUROPE & CHINA 900 band.                  */

  SYS_BAND_WCDMA_IX_1700     = 88,
    /**< WCDMA JAPAN 1700 band                           */

  /**< Reserved 85-109 for WCDMA band classes. 
  */

  SYS_BAND_WLAN_US_2400    = 110,
    /**< WLAN US 2400 Mhz band                           */

  SYS_BAND_WLAN_JAPAN_2400 = 111,
    /**< WLAN JAPAN 2400 Mhz band                        */

  SYS_BAND_WLAN_EUROPE_2400 = 112,
    /**< WLAN EUROPEAN 2400 Mhz band                     */

  SYS_BAND_WLAN_FRANCE_2400 = 113,
    /**< WLAN FRANCE 2400 Mhz band                       */

  SYS_BAND_WLAN_SPAIN_2400 = 114,
    /**< WLAN SPAIN 2400 Mhz band                        */


  SYS_BAND_WLAN_US_5000    = 115,
    /**< WLAN US 5000 Mhz band                           */

  SYS_BAND_WLAN_JAPAN_5000 = 116,
    /**< WLAN JAPAN 5000 Mhz band                        */

  SYS_BAND_WLAN_EUROPE_5000 = 117,
    /**< WLAN EUROPEAN 5000 Mhz band                     */

  SYS_BAND_WLAN_FRANCE_5000 = 118,
    /**< WLAN FRANCE 5000 Mhz band                       */

  SYS_BAND_WLAN_SPAIN_5000 = 119,
    /**< WLAN SPAIN 5000 Mhz band                        */

  SYS_BAND_CLASS_MAX
    /**< FOR INTERNAL USE ONLY!                          */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */
} sys_band_class_e_type;


/**
** Cellular and PCS bands to band-class mapping.
**
** Note that IS-683-A PRL only provide support for one cellular band and one
** PCS band. Therefore we define a Cellular and PCS bands and map them to
** different band-classes depanding on the market, as follows:
**
** Cellular band is mapped to band-class 3 for the Japanese market or band-
** class 0 for any other market.
**
** PCS band is mapped to band-class 4 for the Korean market or band-class 1
** for any other market.
*/

  #define SYS_BAND_CLASS_CELL  SYS_BAND_BC0  /**< US Cellular band-class       */

  #define SYS_BAND_CLASS_PCS   SYS_BAND_BC1  /**< US PCS band-class            */


typedef uint16 sys_pilot_pn_type;      /**< PILOT PN: Valid values: 0 - 511 */

typedef uint16 sys_channel_num_type;   /**< CDMA Channel Number */

/**
** Enumeration of system band classes and band classes' sub-bands.
** If there is a new band class, append to the end of list.
*/
typedef enum {
  SYS_SBAND_BC0_A = 0,
    /**< Band Class 0, A-System */

  SYS_SBAND_BC0_B = 1,
    /**< Band Class 0, B-System */

    /**< Band Class 0 AB , GSM 850 Band*/

  SYS_SBAND_BC1 = 2,
    /**< Band Class 1, all blocks */

  SYS_SBAND_BC2 = 3,
    /**< Band Class 2 place holder */

  SYS_SBAND_BC3 = 4,
    /**< Band Class 3, A-System */

  SYS_SBAND_BC4 = 5,
    /**< Band Class 4, all blocks */

  SYS_SBAND_BC5 = 6,
    /**< Band Class 5, all blocks */

  SYS_SBAND_GSM_DCS_1800 = 7,
    /**< GSM DCS band */

  SYS_SBAND_GSM_EGSM_900 = 8,
    /**< GSM Extended GSM (E-GSM) band */

  SYS_SBAND_GSM_PGSM_900 = 9,
    /**< GSM Primary GSM (P-GSM) band */

  SYS_SBAND_BC6 = 10,
    /**< Band Class 6 */

  SYS_SBAND_BC7 = 11,
    /**< Band Class 7 */

  SYS_SBAND_BC8 = 12,
    /**< Band Class 8 */

  SYS_SBAND_BC9 = 13,
    /**< Band Class 9*/

  SYS_SBAND_BC10 = 14,
    /**< Band Class 10 */

  SYS_SBAND_BC11 = 15,
    /**< Band Class 11 */

  SYS_SBAND_GSM_450 = 16,
    /**< GSM 450 band */

  SYS_SBAND_GSM_480 = 17,
    /**< GSM 480 band */

  SYS_SBAND_GSM_750 = 18,
    /**< GSM 750 band */

  SYS_SBAND_GSM_850 = 19,
    /**< GSM 850 band */

  SYS_SBAND_GSM_RGSM_900 = 20,
    /**< GSM Railways GSM Band */

  SYS_SBAND_GSM_PCS_1900 = 21,
    /**< GSM PCS band */

  SYS_SBAND_WCDMA_I_IMT_2000 = 22,
    /**< WCDMA EUROPE JAPAN & CHINA IMT 2100 band */

  SYS_SBAND_WCDMA_II_PCS_1900 = 23,
    /**< WCDMA US PCS 1900 band */

  SYS_SBAND_WCDMA_III_1700 = 24,
    /**< WCDMA EUROPE & CHINA DCS 1800 band */

  SYS_SBAND_WCDMA_IV_1700 = 25,
    /**< WCDMA US 1700 band */

  SYS_SBAND_WCDMA_V_850 = 26,
    /**< WCDMA US850 band */

  SYS_SBAND_WCDMA_VI_800 = 27,
    /**< WCDMA JAPAN 800 band */

  SYS_SBAND_BC12 = 28,
    /**< Band Class 12 */


  SYS_SBAND_BC14 = 29,
   /**< Band Class 14 */

  SYS_SBAND_RESERVED_2 = 30,
   /**< Reserved 2 */

  SYS_SBAND_BC15 = 31,
   /**< Band Class 15 */


  SYS_SBAND_WLAN_US_2400 = 32,
    /**< WLAN US 2400 band */

  SYS_SBAND_WLAN_EUROPE_2400 = 33,
    /**< WLAN ETSI 2400 band */

  SYS_SBAND_WLAN_FRANCE_2400 = 34,
    /**< WLAN FRANCE 2400 band */

  SYS_SBAND_WLAN_SPAIN_2400 = 35,
    /**< WLAN SPAIN 2400 band */

  SYS_SBAND_WLAN_JAPAN_2400 = 36,
    /**< WLAN JAPAN 2400 band */

  SYS_SBAND_WLAN_US_5000 = 37,
    /**< WLAN US 2400 band */

  SYS_SBAND_WLAN_EUROPE_5000 = 38,
    /**< WLAN EUROPE 5000 band */

  SYS_SBAND_WLAN_FRANCE_5000 = 39,
    /**< WLAN FRANCE 5000 band */

  SYS_SBAND_WLAN_SPAIN_5000 = 40,
    /**< WLAN SPAIN 5000 band */

  SYS_SBAND_WLAN_JAPAN_5000 = 41,
    /**< WLAN JAPAN 5000 band */

  SYS_SBAND_WCDMA_VII_2600 = 48,
    /**< WCDMA EUROPE 2600 band*/

  SYS_SBAND_WCDMA_VIII_900 = 49,
    /**< WCDMA EUROPE & JAPAN 900 band */

  SYS_SBAND_WCDMA_IX_1700 = 50,
    /**< WCDMA JAPAN 1700 band */

  /** 51-55 reserved for WLAN */
  SYS_SBAND_BC16 = 56,
   /**< Band Class 16 */

  SYS_SBAND_BC17 = 57,
   /**< Band Class 17 */

  SYS_SBAND_BC18 = 58,
   /**< Band Class 18 */

  SYS_SBAND_BC19 = 59,
   /**< Band Class 19 */

  SYS_SBAND_PERSISTENT = 62,
    /**< Persistent value from NV */

  SYS_SBAND_MAX = 63
    /**< FOR INTERNAL USE OF CM ONLY! */


} sys_sband_e_type;


/**
** Enumeration of system band class mask.
** It converts bit position of sys_sband_e_type to a bit mask.
** To keep numbering scheme consistent,
** use sys_sband_e_type instead of numbers directly.
**
*/
/**
** Band class bit mask data type. It is a combination of enumeration of
** system band class mask.
*/
typedef uint64                sys_band_mask_type;
typedef sys_band_mask_type    sys_band_mask_e_type;

#define SYS_BAND_MASK_EMPTY             0
     /**< No band selected */

#define SYS_BAND_MASK_BC0_A             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC0_A))
    /**< Acquire band class 0, A-Side systems only */

#define SYS_BAND_MASK_BC0_B             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC0_B))
    /**< Acquire band class 0, B-Side systems only */

#define SYS_BAND_MASK_BC0               ((sys_band_mask_type) (SYS_BM_64BIT(SYS_SBAND_BC0_A) | \
                                        SYS_BM_64BIT(SYS_SBAND_BC0_B)))
    /**< Acquire band class 0 systems only */

#define SYS_BAND_MASK_BC1               ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC1))
    /**< Acquire band class 1 systems only */

#define SYS_BAND_MASK_BC2               ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC2))
    /**< Acquire band class 2 place holder */

#define SYS_BAND_MASK_BC3               ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC3))
    /**< Acquire band class 3 systems only */

#define SYS_BAND_MASK_BC4               ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC4))
    /**< Acquire band class 4 systems only */

#define SYS_BAND_MASK_BC5               ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC5))
    /**< Acquire band class 5 systems only */

#define SYS_BAND_MASK_GSM_DCS_1800      ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_GSM_DCS_1800))
    /**< Acquire band class 8 systems only */

#define SYS_BAND_MASK_GSM_EGSM_900      ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_GSM_EGSM_900))
    /**< Acquire GSM Extended GSM systems only */

#define SYS_BAND_MASK_GSM_PGSM_900      ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_GSM_PGSM_900))
    /**< Acquire GSM Primary GSM systems only */

#define SYS_BAND_MASK_BC6               ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC6))
    /**< Acquire band class 6 systems only */

#define SYS_BAND_MASK_BC7               ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC7))
    /**< Acquire band class 7 systems only */

#define SYS_BAND_MASK_BC8               ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC8))
    /**< Acquire band class 8 systems only */

#define SYS_BAND_MASK_BC9               ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC9))
    /**< Acquire band class 9 systems only */

#define SYS_BAND_MASK_BC10              ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC10))
    /**< Acquire band class 10 systems only */

#define SYS_BAND_MASK_BC11              ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC11))
    /**< Acquire band class 11 systems only */

#define SYS_BAND_MASK_BC12              ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC12))
    /**< Acquire band class 12 systems only */

#define SYS_BAND_MASK_BC14              ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC14))
    /**< Acquire band class 14 systems only */

#define SYS_BAND_MASK_BC15              ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC15))
    /**< Acquire band class 15 systems only */

#define SYS_BAND_MASK_BC16              ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC16))
    /**< Acquire band class 16 systems only */

#define SYS_BAND_MASK_BC17              ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC17))
    /**< Acquire band class 17 systems only */

#define SYS_BAND_MASK_BC18              ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC18))
    /**< Acquire band class 18 systems only */

#define SYS_BAND_MASK_BC19              ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC19))
    /**< Acquire band class 19 systems only */

#define SYS_BAND_MASK_GSM_450           ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_GSM_450))
    /**< Acquire GSM 450 systems only */

#define SYS_BAND_MASK_GSM_480           ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_GSM_480))
    /**< Acquire GSM 480 systems only */

#define SYS_BAND_MASK_GSM_750           ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_GSM_750 ))
    /**< Acquire GSM 750 systems only */

#define SYS_BAND_MASK_GSM_850           ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_GSM_850 ))
    /**< Acquire GSM 850 systems only */

#define SYS_BAND_MASK_GSM_RGSM_900      ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_GSM_RGSM_900))
    /**< Acquire GSM Railways GSM systems only */

#define SYS_BAND_MASK_GSM_PCS_1900      ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_GSM_PCS_1900))
    /**< Acquire GSM PCS systems only */

#define SYS_BAND_MASK_WCDMA_I_IMT_2000  ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_WCDMA_I_IMT_2000))
    /**< Acquire WCDMA EUROPE JAPAN & CHINA IMT 2100 systems only */

#define SYS_BAND_MASK_WCDMA_II_PCS_1900 ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_WCDMA_II_PCS_1900))
    /**< Acquire WCDMA US PCS 1900 system only */

#define SYS_BAND_MASK_WCDMA_III_1700    ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_WCDMA_III_1700))
    /**< Acquire WCDMA EUROPE & CHINA 1800 systems only */

#define SYS_BAND_MASK_WCDMA_IV_1700     ((sys_band_mask_type) SYS_BM_64BIT( SYS_SBAND_WCDMA_IV_1700 ))
    /**< Acquire WCDMA US 1700 systems only */

#define SYS_BAND_MASK_WCDMA_V_850       ((sys_band_mask_type) SYS_BM_64BIT( SYS_SBAND_WCDMA_V_850 ))
    /**< Acquire WCDMA US 850 systems only */

#define SYS_BAND_MASK_WCDMA_VI_800      ((sys_band_mask_type) SYS_BM_64BIT( SYS_SBAND_WCDMA_VI_800 ))
    /**< Acquire WCDMA JAPAN 800 systems only */

#define SYS_BAND_MASK_WCDMA_VII_2600      SYS_BM_64BIT( SYS_SBAND_WCDMA_VII_2600 )
    /**< Acquire WCDMA EUROPE 2600 systems only */

#define SYS_BAND_MASK_WCDMA_VIII_900      SYS_BM_64BIT( SYS_SBAND_WCDMA_VIII_900 )
    /**< Acquire WCDMA EUROPE & CHINA 900 systems only */

#define SYS_BAND_MASK_WCDMA_IX_1700      SYS_BM_64BIT( SYS_SBAND_WCDMA_IX_1700 )
    /**< Acquire WCDMA JAPAN 1700 systems only */

#define SYS_BAND_MASK_WLAN_US_2400      ((sys_band_mask_type) SYS_BM_64BIT( SYS_SBAND_WLAN_US_2400 ))
    /**< Acquire WLAN US 2400 systems only */

#define SYS_BAND_MASK_WLAN_EUROPE_2400  ((sys_band_mask_type) SYS_BM_64BIT( SYS_SBAND_WLAN_EUROPE_2400 ))
    /**< Acquire WLAN EUROPE 2400 systems only */

#define SYS_BAND_MASK_WLAN_FRANCE_2400  ((sys_band_mask_type) SYS_BM_64BIT( SYS_SBAND_WLAN_FRANCE_2400 ))
    /**< Acquire WLAN FRANCE 2400 systems only */

#define SYS_BAND_MASK_WLAN_SPAIN_2400   ((sys_band_mask_type) SYS_BM_64BIT( SYS_SBAND_WLAN_SPAIN_2400 ))
    /**< Acquire WLAN SPAIN 2400 systems only */

#define SYS_BAND_MASK_WLAN_JAPAN_2400   ((sys_band_mask_type) SYS_BM_64BIT( SYS_SBAND_WLAN_JAPAN_2400 ))
    /**< Acquire WLAN JAPAN 2400 systems only */

#define SYS_BAND_MASK_WLAN_US_5000      ((sys_band_mask_type) SYS_BM_64BIT( SYS_SBAND_WLAN_US_5000 ))
    /**< Acquire WLAN US 5000 systems only */

#define SYS_BAND_MASK_WLAN_EUROPE_5000  ((sys_band_mask_type) SYS_BM_64BIT( SYS_SBAND_WLAN_EUROPE_5000 ))
    /**< Acquire WLAN EUROPE 5000 systems only */

#define SYS_BAND_MASK_WLAN_FRANCE_5000  ((sys_band_mask_type) SYS_BM_64BIT( SYS_SBAND_WLAN_FRANCE_5000 ))
    /**< Acquire WLAN FRANCE 5000 systems only */

#define SYS_BAND_MASK_WLAN_SPAIN_5000   ((sys_band_mask_type) SYS_BM_64BIT( SYS_SBAND_WLAN_SPAIN_5000 ))
    /**< Acquire WLAN SPAIN 5000 systems only */

#define SYS_BAND_MASK_WLAN_JAPAN_5000   ((sys_band_mask_type) SYS_BM_64BIT( SYS_SBAND_WLAN_JAPAN_5000 ))
    /**< Acquire WLAN JAPAN 5000 systems only */


#define SYS_BAND_MASK_WLAN_2400         (SYS_BAND_MASK_WLAN_US_2400      | \
                                         SYS_BAND_MASK_WLAN_JAPAN_2400   | \
                                         SYS_BAND_MASK_WLAN_EUROPE_2400  | \
                                         SYS_BAND_MASK_WLAN_FRANCE_2400  | \
                                         SYS_BAND_MASK_WLAN_SPAIN_2400)

#define SYS_BAND_MASK_WLAN_5000         (SYS_BAND_MASK_WLAN_US_5000      | \
                                         SYS_BAND_MASK_WLAN_JAPAN_5000   | \
                                         SYS_BAND_MASK_WLAN_FRANCE_5000  | \
                                         SYS_BAND_MASK_WLAN_EUROPE_5000  | \
                                         SYS_BAND_MASK_WLAN_SPAIN_5000)

#define SYS_BAND_MASK_WLAN_ANY          (SYS_BAND_MASK_WLAN_2400 | \
                                         SYS_BAND_MASK_WLAN_5000)

#define SYS_BAND_MASK_ANY               ((sys_band_mask_e_type)0x7FFFFFFFFFFFFFFFULL)
                                         /* No, this doesn't say "FULL". 
                                         ** "ULL" designates 
                                         **"unsigned long long" 
                                         */
                                     

/**
** Channel data type.
*/
typedef word  sys_chan_type;


/** Invalid channel.
*/
#define SYS_INVALID_CHANNEL  (sys_chan_type)(-1)


/**
** Enumeration of PCS frequency blocks/Cellular systems.
*/
typedef enum
{
  SYS_BLKSYS_NONE = -1,
    /**< FOR INTERNAL USE OF CM ONLY!           */

  /* 0 */
  SYS_BLKSYS_PCS_A,
    /**< PCS block A                            */

  SYS_BLKSYS_PCS_B,
    /**< PCS block B                            */

  SYS_BLKSYS_PCS_C,
    /**< PCS block C                            */

  SYS_BLKSYS_PCS_D,
    /**< PCS block D                            */

  /* 4 */
  SYS_BLKSYS_PCS_E,
    /**< PCS block E                            */

  SYS_BLKSYS_PCS_F,
    /**< PCS block F                            */

  SYS_BLKSYS_PCS_CELL_A,
    /**< Cellular system A                      */

  SYS_BLKSYS_PCS_CELL_B,
    /**< Cellular system B                      */

  /* 8 */
  SYS_BLKSYS_PCS_G,
    /**< BC5/11 G-Block                         */

  SYS_BLKSYS_PCS_H,
    /**< BC5/11 H-Block                         */

  SYS_BLKSYS_PCS_I,
    /**< BC5/11 I-Block                         */

  SYS_BLKSYS_PCS_J,
    /**< BC5/11 J-Block                         */

  /* 12 */
  SYS_BLKSYS_PCS_K,
    /**< BC5/11 K-Block                         */

  SYS_BLKSYS_PCS_L,
    /**< BC5 L-BLOCK                            */

  SYS_BLKSYS_MAX
    /**< FOR INTERNAL USE OF CM ONLY!           */

  #ifdef FEATURE_RPC
#error code not present
  #endif /**< FEATURE_RPC */

} sys_blksys_e_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** Enumeration of prl system types.
*/
typedef enum {

  SYS_PRL_AVAIL_BC0_A = 0,         /**< available systems on Band Class 0,
                                   ** A-System only
                                   */

  SYS_PRL_AVAIL_BC0_B = 1,         /**< available systems on
                                   ** Band Class 0, B-System only
                                   */

  SYS_PRL_MAX

} sys_prl_e_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/**
** Enumeration of service status.  Indicates whether service is
** available or not available.
*/
typedef enum
{
  SYS_SRV_STATUS_NONE = -1,
    /**< FOR INTERNAL USE ONLY!                 */

  /* 0 */
  SYS_SRV_STATUS_NO_SRV,
    /**< No service                             */

  SYS_SRV_STATUS_LIMITED,
    /**< Limited service                        */

  SYS_SRV_STATUS_SRV,
    /**< Service available                      */

  SYS_SRV_STATUS_LIMITED_REGIONAL,
    /**< Limited regional service               */

  /* 4 */
  SYS_SRV_STATUS_PWR_SAVE,
    /**< MS is in power save or deep sleep      */

  SYS_SRV_STATUS_MAX
    /**< FOR INTERNAL USE OF CM ONLY!           */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} sys_srv_status_e_type;


/**
** The following enumerated type defines type of service.
*/
typedef enum
{
  SYS_SRV_DOMAIN_NONE = -1,
    /**< FOR INTERNAL USE ONLY!                 */

  /* 0 */
  SYS_SRV_DOMAIN_NO_SRV,
    /**< No service                             */

  SYS_SRV_DOMAIN_CS_ONLY,
    /**< Circuit Switched Only Capable          */

  SYS_SRV_DOMAIN_PS_ONLY,
    /**< Packet Switched Only Capable           */

  SYS_SRV_DOMAIN_CS_PS,
    /**< Circuit and Packet Switched Capable    */

  /* 4 */
  SYS_SRV_DOMAIN_CAMPED,
    /**< MS found the right system but not yet
    ** registered/attached.                  */

  SYS_SRV_DOMAIN_MAX
    /**< FOR INTERNAL USE ONLY!                 */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */
} sys_srv_domain_e_type;


/**
** The following enumerated type defines the service capabilities
** of a network.
*/
typedef enum
{
  SYS_SRV_CAPABILITY_NONE = -1,
    /**< FOR INTERNAL USE ONLY!                                            */

  SYS_SRV_CAPABILITY_NOT_AVAILABLE,
    /**< Network can not provide service                                   */

  SYS_SRV_CAPABILITY_CS_SERVICE_ONLY,
    /**< Network is capable of providing Circuit Switched service          */

  SYS_SRV_CAPABILITY_PS_SERVICE_ONLY,
    /**< Network is capable of providing Packet Switched service           */

  SYS_SRV_CAPABILITY_CS_PS_SERVICE,
    /**< Network is capable of providing Circuit & Packet Switched service */

  SYS_SRV_CAPABILITY_MAX
    /**< FOR INTERNAL USE ONLY!                                            */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */
} sys_srv_capability_e_type;


/**
** Enumeration of roaming statuses.  The values for these enumerates
** were derived from the roaming display indicators described in
** 3GPP2 C.R1001-A.  Please note that the actual value of the enumerate
** is not consistent with the value listed in the standard (e.g. the
** value of SYS_ROAM_STATUS_OFF and SYS_ROAM_STATUS_ON are reversed from
** the standard.
*/
typedef enum
{

  SYS_ROAM_STATUS_NONE = -1,
    /**< FOR INTERNAL USE ONLY!                      */

  /* 0 */
  SYS_ROAM_STATUS_OFF,
    /**< Roaming Indicator off                       */

  SYS_ROAM_STATUS_ON,
    /**< Roaming Indicator on                        */

  SYS_ROAM_STATUS_BLINK,
    /**< Roaming Indicator flashing                  */

  SYS_ROAM_STATUS_OUT_OF_NEIGHBORHOOD,
    /**< Out of neighborhood                         */
  
  /* 4 */
  SYS_ROAM_STATUS_OUT_OF_BLDG,
    /**< Out of building                             */

  SYS_ROAM_STATUS_PREF_SYS,
    /**< Roaming - preferred system                  */

  SYS_ROAM_STATUS_AVAIL_SYS,
    /**< Roaming - available system                  */

  SYS_ROAM_STATUS_ALLIANCE_PARTNER,
    /**< Roaming - alliance partner                  */

  /* 8 */
  SYS_ROAM_STATUS_PREMIUM_PARTNER,
    /**< Roaming - premium partner                   */

  SYS_ROAM_STATUS_FULL_SVC,
    /**< Roaming - full service functionality        */

  SYS_ROAM_STATUS_PARTIAL_SVC,
    /**< Roaming - partial service functionality     */

  SYS_ROAM_STATUS_BANNER_ON,
    /**< Roaming banner on                           */

  /* 12 */
  SYS_ROAM_STATUS_BANNER_OFF,
    /**< Roaming banner off                          */

  SYS_ROAM_STATUS_MAX  = 0xFF+1
    /**< FOR INTERNAL USE ONLY!                      */
    /**< Values 0x00 - 0xFF are used in the standard */

} sys_roam_status_e_type;

/**
** The following enumerated type defines the radio access technologies.
*/
typedef enum
{
  SYS_RAT_NONE = -1,
    /**< FOR INTERNAL USE ONLY!                 */

  SYS_RAT_GSM_RADIO_ACCESS,
    /**< GSM radio access technology            */

  SYS_RAT_UMTS_RADIO_ACCESS,
    /**< UMTS radio access technology           */

  SYS_RAT_MAX
    /**< FOR INTERNAL USE ONLY!                 */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} sys_radio_access_tech_e_type;


/**
** The following enumerated type defines the state of the
** Subscriber Identfication Module (SIM) card.
*/
typedef enum
{
  SYS_SIM_STATE_NONE = -1,
    /**< FOR INTERNAL USE ONLY!                 */

  /* 0 */
  SYS_SIM_STATE_NOT_AVAILABLE,
    /**< The SIM is not available               */

  SYS_SIM_STATE_AVAILABLE,
    /**< The SIM is available                   */

  SYS_SIM_STATE_CS_INVALID,
    /**< The SIM has been marked by the network */
    /**< as invalid for CS services             */

  SYS_SIM_STATE_PS_INVALID,
    /**< The SIM has been marked by the network */
    /**< as invalid for PS services             */

  /* 4 */
  SYS_SIM_STATE_CS_PS_INVALID,
    /**< The SIM has been marked by the network */
    /**< as invalid for CS and PS services      */

  SYS_SIM_STATE_MAX
    /**< FOR INTERNAL USE ONLY!                 */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} sys_sim_state_e_type;


#if (defined(FEATURE_UMTS_1X_HANDOVER_1XMSM) || \
    defined(FEATURE_UMTS_1X_HANDOVER_UMTSMSM) || \
    defined(FEATURE_MM_SUPERSET))

/** Enumeration to identify the MSM.
*/
typedef enum
{
  SYS_MSM_ID_NONE = -1,
    /**< used only for bounds checking.           */

  /* 0 */
  SYS_MSM_ID_6500,
    /**< MSM Id is 6500.                          */

  SYS_MSM_ID_6250,
    /**< MSM Id is 6250.                          */

  SYS_MSM_ID_MAIN,
    /**< MSM which has control of DPRAM.          */

  SYS_MSM_ID_SECONDARY,
    /**< MSM which does not has control of DPRAM. */

  /* 4 */
  SYS_MSM_ID_MAX
    /**< Used for arrays & bounds checking        */

} sys_msm_id_e_type;
#endif


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*---------------------------------------------------------------------------
                         Simultaneous mode type
---------------------------------------------------------------------------*/

/** Enumeration for simultaneous modes, needed to 7600 to differentiate
** between WX and non-WX modes of operation
*/

typedef enum {

  SYS_SIMULTANEOUS_MODE_NONE = -1,    /**< For Internal SD use              */

  SYS_SIMULTANEOUS_MODE_NON_WX,       /**< Not in WX mode                   */

  SYS_SIMULTANEOUS_MODE_WX ,          /**< Simultaneous CDMA and WCDMA (WX) */

  SYS_SIMULTANEOUS_MODE_MAX           /**< For Internal SD use              */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} sys_simultaneous_mode_e_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** Enumeration for hsdpa, hsupa indication types
** High speed downlink packet access and high speed
** uplink packet access can be available within WCDMA
** service only
*/
typedef enum
{

  SYS_HS_IND_NONE = -1,
    /**< FOR INTERNAL USE OF CM ONLY!
    */

  SYS_HS_IND_HSDPA_HSUPA_UNSUPP_CELL,
    /**< Cell does not support either HSDPA or HSUPA
    */

  SYS_HS_IND_HSDPA_SUPP_CELL,
    /**< Cell supports HSDPA
    */

  SYS_HS_IND_HSUPA_SUPP_CELL,
    /**< Cell supports HSUPA
    */

  SYS_HS_IND_HSDPA_HSUPA_SUPP_CELL,
    /**< Cell supports HSDPA and HSUPA
    */  

  SYS_HS_IND_MAX
    /**< FOR INTERNAL USE OF CM ONLY!
    */

} sys_hs_ind_e_type;


/** Dual transfer mode allows simultaneous
** CS and ps calls during GSM only service
*/
typedef enum
{

  SYS_DTM_SUPPORT_NONE = -1,
    /**< FOR INTERNAL USE OF CM ONLY!
    */  

  SYS_DTM_SUPPORT_NOT_AVAIL,
    /**< DTM support is not available
    */

  SYS_DTM_SUPPORT_AVAIL,
    /**< DTM support is available
    */

  SYS_DTM_SUPPORT_MAX
    /**< FOR INTERNAL USE OF CM ONLY!
    */

} sys_dtm_support_e_type;

typedef enum

{

  SYS_EGPRS_SUPPORT_NONE = -1,
    /**< FOR INTERNAL USE OF CM ONLY!
    */  
  
  SYS_EGPRS_SUPPORT_NOT_AVAIL,
    /**< EGPRS support is not available
    */

  SYS_EGPRS_SUPPORT_AVAIL,
    /**< EGPRS support is available
    */

  SYS_EGPRS_SUPPORT_MAX
    /**< FOR INTERNAL USE OF CM ONLY!
    */

} sys_egprs_support_e_type;

 


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/**
** structure to hold user zone name information.
*/

/** Maximum user zone name length.
*/
#define  SYS_PUZL_MAX_UZ_NAME_LEN          32

typedef struct
{
  byte     uz_name_encoding;

  byte     uz_name_n_octets;

  byte     uz_name[SYS_PUZL_MAX_UZ_NAME_LEN];

} sys_puzl_uz_name_s_type;

/*------------------------------------------------------------------------------
   HDR related Defines^M
------------------------------------------------------------------------------*/

/** Enumeration of HDR acquire modes.
*/
typedef enum {

  SYS_HDR_ACQ_MEASURE_RF,            /**< Only acq if raw power is high enough */

  SYS_HDR_ACQ_MODE_MICRO,            /**< Micro acquisitions */

  SYS_HDR_ACQ_MODE_SHALLOW,          /**< Shallow acquisitions only */

  SYS_HDR_ACQ_MODE_DEEP,             /**< Deep acquisitions only */

  SYS_HDR_ACQ_MODE_FULL,             /**< Shallow and deep acquisitions */

  SYS_HDR_ACQ_MODE_MAX               /**< For internal use only */

} sys_hdr_acq_mode_e_type;


/** Enumeration of active protocol revision for various technolgies.
*/
typedef enum {

  
  SYS_ACTIVE_PROT_NONE,               /**< The protocol revision is unknown.
                                      ** When HDR sys ID is first acquired 
                                      ** and no protocol revision is 
                                      ** available at this point.
                                      */
  SYS_ACTIVE_PROT_HDR_START,          /**< The protocol revision is unknown.
                                      ** When HDR sys ID is first acquired 
                                      ** and no protocol revision is 
                                      ** available at this point.
                                      */
  SYS_ACTIVE_PROT_HDR_REL0,           /**< HDR release 0 
                                      */
  SYS_ACTIVE_PROT_HDR_RELA,           /**< HDR release A (physical layer 
                                      ** subtype 2 is active in current 
                                      ** personality) 
                                      */
                                       
  SYS_ACTIVE_PROT_HDR_RELB,           /**< HDR release B 
                                      */

  SYS_ACTIVE_PROT_HDR_END,            /**< Internal use for HDR range check  
                                      */

                                      /**< If adding other technologies, pls
                                      ** reserve some entries for HDR.
                                      */

  SYS_ACTIVE_PROT_MAX                 /**< Internal use for enum range check.
                                      */

} sys_active_prot_e_type;


/** Structure to define a CDMA/HDR channel
*/
typedef struct
{
  sys_band_class_e_type    band;
    /**< Band class - 0, 1, etc */

  sys_channel_num_type     chan_num;
    /**< CDMA Channel Number */

} sys_channel_type;

/** Structure to define a CDMA/HDR channel as well as mode of operation.
*/
typedef struct
{

  sys_sys_mode_e_type       mode;
    /**< Mode - AMPS, CDMA or HDR, etc */

  sys_channel_type          channel;
    /**< Channel */

} sys_channel_record_type;

/** Structure to define a CDMA/HDR avoided channels and mode */
typedef struct
{
  sys_sys_mode_e_type       mode;
    /**< Mode - AMPS, CDMA or HDR, etc */

  sys_channel_type          channel;
    /**< Channel */

  dword                     uptime;
    /**< Clock Uptime */

  dword                     avoided_time;
    /**< Avoided Time */

} sys_avoided_channel_record_type;


/* Structure to define a HDR system type.
*/
typedef enum
{
  /* High Data Rate (HDR) mode, ChannelNumber field specifies
   * forward and Reverse CDMA channels that are FDD-paired 
   */ 
  SYS_HDR_CHAN_REC_MODE_IS856 = 0, 

  /* A CDMA 2000 system */
  SYS_HDR_CHAN_REC_MODE_IS2000 = 1,

  /* High Data Rate (HDR) mode, ChannelNumber Field 
   *  specifies only the forward CDMA channel 
   */
  SYS_HDR_CHAN_REC_MODE_IS856_FWD_CH = 2, 

  /* For internal use only*/
  SYS_HDR_CHAN_REC_MODE_MAX 

} sys_hdr_chan_rec_mode_e_type;

/* Structure to define a HDR channel record.
*/
typedef struct
{

  sys_hdr_chan_rec_mode_e_type       mode;

  sys_channel_type          channel;
    /* Channel */

} sys_hdr_channel_record_s_type;

/* Structure to define entire linke : Channel and Pilot PN
*/
typedef struct
{
  sys_hdr_chan_rec_mode_e_type    mode;

  sys_channel_type          channel;
    /* Channel */

  sys_pilot_pn_type         pilot_pn;
    /* Pilot PN */

} sys_link_type;

/* Structure to define pilot strength measurement 
** for a channel
*/
typedef struct sys_pilot_info_s
{
  sys_pilot_pn_type         pilot_pn;
    /* Pilot PN */

  word                      pilot_pn_phase;
    /* Pilot PN phase */

  boolean                   is_chan_incl;
    /* channel included */

  sys_channel_type          channel;
    /* channel - only valid if is_chan_incl = TRUE */

  byte                      pilot_strength;
    /* pilot strength */

} sys_pilot_info_s_type;

#define SYS_CHAN_CMP_EQUAL(c1, c2) (((c1).band == (c2).band) && \
                                    ((c1).chan_num == (c2).chan_num))

/*--------------------------------------------------------------------------
   PDP Header and Data compression enums
--------------------------------------------------------------------------*/

/** PDP header compression types */
typedef enum
{
  SYS_PDP_HEADER_COMP_NONE = -1,
    /**< FOR INTERNAL USE OF CM ONLY!
    */

  SYS_PDP_HEADER_COMP_OFF = 0,
    /**< PDP header compression is OFF.
    ** Preserves earlier value of h_comp=FALSE 
    ** for no header compression.
    */

  SYS_PDP_HEADER_COMP_ON  = 1,
    /**< PDP header compression is ON.
    ** Preserves earlier value of h_comp=TRUE
    ** for default header compression.
    */


  SYS_PDP_HEADER_COMP_RFC1144,
    /**< PDP header compression based on rfc 1144.    
    ** 3GPP TS 44.065 (sect 6.5.1.1.4)
    */

  SYS_PDP_HEADER_COMP_RFC2507,
    /**< PDP header compression based on rfc 2507.    
    ** 3GPP TS 44.065 (sect 6.5.1.1.4)
    */


  SYS_PDP_HEADER_COMP_RFC3095,
    /**< PDP header compression based on rfc 3095.    
    ** 3GPP TS 44.065 release-6 version-6.5.0 
    ** (sect 6.5.4 and 6.5.5)
    */

  SYS_PDP_HEADER_COMP_MAX
    /**< FOR INTERNAL USE OF CM ONLY!
    */

} sys_pdp_header_comp_e_type;


/** PDP Data compression types */
typedef enum
{
  SYS_PDP_DATA_COMP_NONE = -1,
    /**< FOR INTERNAL USE OF CM ONLY!
    */

  SYS_PDP_DATA_COMP_OFF = 0,
    /**< PDP Data compression is OFF.
    ** Preserves earlier value of d_comp=FALSE 
    ** for no data compression.
    */

  SYS_PDP_DATA_COMP_ON  = 1,
    /**< Default PDP Data compression is enabled.
    ** Preserves earlier value of d_comp=TRUE 
    ** for default data compression.
    */


  SYS_PDP_DATA_COMP_V42_BIS,
    /**< Data compression standard specified in
    ** 3GPP TS 44.065 (sect 6.6.1.1.4).
    */


  SYS_PDP_DATA_COMP_V44,
    /**< Data compression standard specified in
    ** 3GPP TS 44.065 (sect 6.6.1.1.4).
    */

  SYS_PDP_DATA_COMP_MAX
    /**< FOR INTERNAL USE OF CM ONLY!
    */

} sys_pdp_data_comp_e_type;



/*------------------------------------------------------------------------------
   WLAN related defines
------------------------------------------------------------------------------*/

/** Enumeration of WLAN scan types.
*/
typedef enum {

  /** FOR INTERNAL USE ONLY!.
  */
  SYS_WLAN_SCAN_TYPE_NONE = -1,

  /** Active scanning.
  */
  SYS_WLAN_SCAN_TYPE_ACTIVE,

  /** Passive scanning.
  */
  SYS_WLAN_SCAN_TYPE_PASSIVE,

  /** FOR INTERNAL USE ONLY!.
  */
  SYS_WLAN_SCAN_TYPE_MAX

} sys_wlan_scan_type_e_type;

/* Enumeration of WLAN BSS types.
*/
typedef enum {

  /* FOR INTERNAL USE ONLY!.
  */
  SYS_WLAN_BSS_TYPE_NONE = -1,

  /* Ad-Hoc Type BSS.
  */
  SYS_WLAN_BSS_TYPE_ADHOC,

  /* Infrastructure Mode BSS.
  */
  SYS_WLAN_BSS_TYPE_INFRA,

  /* Any BSS Mode Type.
  */
  SYS_WLAN_BSS_TYPE_ANY,

  /* Ad-Hoc Type BSS created by MS.
  */
  SYS_WLAN_BSS_TYPE_ADHOC_START,

  /* FOR INTERNAL USE ONLY!.
  */
  SYS_WLAN_BSS_TYPE_MAX

} sys_wlan_bss_e_type;

/* Type that defines the SSID of WLAN system.
*/
typedef struct sys_wlan_ssid_s
{
  /* Length of the SSID, if == 0, then SSID = broadcast SSID.
  */
  uint8                                len;

  /* SSID of the wlan system.
  */
  char                                 ssid[SYS_WLAN_SSID_MAX_SIZE];

} sys_wlan_ssid_s_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FIELD sys_wlan_ssid_s.ssid VARRAY SYS_WLAN_SSID_MAX_SIZE
    LENGTH sys_wlan_ssid_s.len */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/* Enumeration of Technology type/version.
*/
typedef enum
{

  /* FOR INTERNAL USE OF CM ONLY!.
  */
  SYS_TECH_NONE                        = -1,

  /* WLAN 802.11a technology.
  */
  SYS_TECH_WLAN_80211A                 = 0,

  /* WLAN 802.11b technology.
  */
  SYS_TECH_WLAN_80211B                 = 1,

  /* WLAN 802.11g technology.
  */
  SYS_TECH_WLAN_80211G                 = 2,


  /* Reserved values for CM use.
  */
  SYS_TECH_RESERVED                    = 30,

  /* FOR INTERNAL USE OF CM ONLY!.
  */
  SYS_TECH_MAX

} sys_tech_e_type;

/*------------------------------------------------------------------------------
   Service Indicator Data Types
------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
   Service Indicator Functions
------------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_oprt_mode_is_valid

@DESCRIPTION

  Function that verifies that the input value is within its valid range.

@PARAMETERS

  sys_oprt_mode_e_type  value          Value that is to be checked
                                       for validity.

@RETURN VALUE

  boolean  Flag indicating whether the sys_oprt_mode_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
extern boolean sys_oprt_mode_is_valid
(
  sys_oprt_mode_e_type  value
);


/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_sys_mode_is_valid

@DESCRIPTION

  Function that verifies that the input value is within its valid range.

@PARAMETERS

  sys_sys_mode_e_type  value          Value that is to be checked
                                      for validity.

@RETURN VALUE

  boolean  Flag indicating whether the sys_sys_mode_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
extern boolean sys_sys_mode_is_valid
(
  sys_sys_mode_e_type  value
);


/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_band_class_is_valid

@DESCRIPTION

  Function that verifies that the input value is within its valid range.

@PARAMETERS

  sys_band_class_e_type  value        Value that is to be checked
                                      for validity.

@RETURN VALUE

  boolean  Flag indicating whether the sys_band_class_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
extern boolean sys_band_class_is_valid
(
  sys_band_class_e_type  value
);


/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_block_or_system_is_valid

@DESCRIPTION

  Function that verifies that the input value is within its valid range.

@PARAMETERS

  sys_blksys_e_type  value            Value that is to be checked
                                      for validity.

@RETURN VALUE

  boolean  Flag indicating whether the sys_blksys_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
extern boolean sys_block_or_system_is_valid
(
  sys_blksys_e_type  value
);


/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_srv_status_is_valid

@DESCRIPTION

  Function that verifies that the input value is within its valid range.

@PARAMETERS

  sys_srv_status_e_type  value            Value that is to be checked
                                          for validity.

@RETURN VALUE

  boolean  Flag indicating whether the sys_srv_status_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
extern boolean sys_srv_status_is_valid
(
  sys_srv_status_e_type  value
);



/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_srv_status_is_srv

@DESCRIPTION

  Function that verifies that the input value is indicates any service.

@PARAMETERS

  sys_srv_status_e_type  value            Value that is to be checked
                                          for validity.

@RETURN VALUE

  boolean  Flag indicating whether the value indicates service( TRUE)
           or no service (FALSE).

==============================================================================*/
boolean sys_srv_status_is_srv
(
  sys_srv_status_e_type  value
);



/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_srv_status_is_full_srv

@DESCRIPTION

  Function that verifies that the input value is indicates full service.

@PARAMETERS

  sys_srv_status_e_type  value            Value that is to be checked
                                          for validity.

@RETURN VALUE

  boolean  Flag indicating whether the value indicates full service( TRUE)
           or no full service (FALSE).

==============================================================================*/
extern boolean sys_srv_status_is_full_srv
(
  sys_srv_status_e_type  value
);

/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_srv_domain_is_valid

@DESCRIPTION

  Function that verifies that the input value is within its valid range.

@PARAMETERS

  sys_srv_domain_e_type  value            Value that is to be checked
                                          for validity.

@RETURN VALUE

  boolean  Flag indicating whether the sys_service_domain_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
extern boolean sys_srv_domain_is_valid
(
  sys_srv_domain_e_type  value
);


/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_srv_capability_is_valid

@DESCRIPTION

  Function that verifies that the input value is within its valid range.

@PARAMETERS

  sys_srv_capability_e_type   value       Value that is to be checked
                                          for validity.

@RETURN VALUE

  boolean  Flag indicating whether the sys_srv_capability_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
extern boolean sys_srv_capability_is_valid
(
  sys_srv_capability_e_type value
);


/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_roam_status_is_valid

@DESCRIPTION

  Function that verifies that the input value is within its valid range.

@PARAMETERS

  sys_roam_status_e_type value                   Value that is to be checked
                                                 for validity.

@RETURN VALUE

  boolean  Flag indicating whether the sys_roam_status_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
extern boolean sys_roam_status_is_valid
(
  sys_roam_status_e_type  value
);


/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_radio_access_tech_is_valid

@DESCRIPTION

  Function that verifies that the input value is within its valid range.

@PARAMETERS

  sys_radio_access_tech_e_type value             Value that is to be checked
                                                 for validity.

@RETURN VALUE

  boolean  Flag indicating whether the sys_radio_access_tech_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
extern boolean sys_radio_access_tech_is_valid
(
  sys_radio_access_tech_e_type  value
);


/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_sim_state_is_valid

@DESCRIPTION

  Function that verifies that the input value is within its valid range.

@PARAMETERS

  sys_sim_state_e_type value                     Value that is to be checked
                                                 for validity.

@RETURN VALUE

  boolean  Flag indicating whether the sys_sim_state_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
extern boolean sys_sim_state_is_valid
(
  sys_sim_state_e_type  value
);

#ifdef __cplusplus
}
#endif

/* @}
**
*/

/**==============================================================================

                     S Y S T E M   I D E N T I F I E R

==============================================================================*/

/* @defgroup SI System Identifier
**
*/

/*------------------------------------------------------------------------------
   System Identifier Defines
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
   System Identifier Enums
------------------------------------------------------------------------------*/

/**
** Define a enumerated type that indicates the type of system ID.
*/
typedef enum
{

  SYS_SYS_ID_TYPE_UNDEFINED,
    /**< System ID is undefined     */

  SYS_SYS_ID_TYPE_UMTS,
    /**< UMTS (WCDMA/GSM) system ID */

  SYS_SYS_ID_TYPE_IS95,
    /**< IS-95 system ID            */

  SYS_SYS_ID_TYPE_IS856,
    /**< IS-856 (HDR) system ID     */

  SYS_SYS_ID_TYPE_WLAN
    /**< WLAN System Id                 */

} sys_sys_id_type_e_type;


/*------------------------------------------------------------------------------
   System Identifier Data Types
------------------------------------------------------------------------------*/
/**
** Define a structure that identifies an IS-95 system.
**/
typedef struct sys_is95_sys_id_s
{
  uint16         sid;
       /**< The system ID */

  uint16         nid;
       /**< The network ID */

  word           mcc;       
       /**< MCC - Mobile County Code */

  byte           imsi_11_12;
       /**< IMSI_11_12 */

} sys_is95_sys_id_s_type;


/**
** Define a structure that identifies a Public Land Mobile Network (PLMN).
** A PLMN ID is defined within a GSM network to be the combination of a Mobile
** Country Code (MCC) and Mobile Network Code (MNC).  A PLMN ID is stored in 3
** octets as specified in 3G TS 24.008 Section 10.5.1.3 and shown below:
**                                        Bit
**                 |  8  |  7  |  6  |  5  |  4  |  3  |  2  |  1  |
**                 |===============================================|
**         octet 1 |      MCC Digit 2      |      MCC Digit 1      |
**                 |-----------------------------------------------|
**         octet 2 |      MNC Digit 3      |      MCC Digit 3      |
**                 |-----------------------------------------------|
**         octet 3 |      MNC Digit 2      |      MNC Digit 1      |
**                 |===============================================|
**
** The criteria for comparing one PLMN against another is specified in
** 3G TS 23.122 Annex A.
**/
typedef struct sys_plmn_id_s
{
  byte                                identity[3];
} sys_plmn_id_s_type;


/**
**        LAC      local area code.
**/
typedef word                          sys_lac_type;


/**
**        PLMN + LAC.
**/
typedef struct sys_plmn_lac_id_s
{
  sys_plmn_id_s_type   plmn;
  sys_lac_type         lac;
}sys_plmn_lac_id_s_type;



/**
** Type for system ID.
*/
typedef union sys_sys_id_u
{
  /** IS-95 system ID.
  */
  sys_is95_sys_id_s_type               is95;

  /** IS-856 system ID.
  */
  byte                                 is856[16];

  /** UMTS (WCDMA/GSM) system ID.
  */
  sys_plmn_id_s_type                   plmn;

  /** WLAN system Id.
  */
  sys_wlan_ssid_s_type                 ssid;

  sys_plmn_lac_id_s_type  plmn_lac;
    /**< UMTS (WCDMA/GSM) PLMN and LAC ID  */

} sys_sys_id_u_type;

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ PARTIAL sys_sys_id_u */
/*~ CASE SYS_SYS_ID_TYPE_UNDEFINED sys_sys_id_u.void */
/*~ CASE SYS_SYS_ID_TYPE_IS95      sys_sys_id_u.is95 */
/*~ CASE SYS_SYS_ID_TYPE_IS856     sys_sys_id_u.is856 */
/*~ CASE SYS_SYS_ID_TYPE_UMTS      sys_sys_id_u.plmn_lac */
/*~ CASE SYS_SYS_ID_TYPE_WLAN      sys_sys_id_u.ssid */
#endif

typedef struct sys_sys_id_s
{
  /** Type of the system id.
  */
  sys_sys_id_type_e_type              id_type;

  /** The system identifier.
  */
  sys_sys_id_u_type                   id;

} sys_sys_id_s_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FIELD sys_sys_id_s.id DISC sys_sys_id_s.id_type */
#endif 

/**
**        CELL ID
**/
typedef uint32                        sys_cell_id_type;

/**
**   Camped Cell Info 
**/
typedef struct sys_cell_info_s
{
  sys_cell_id_type         cell_id;
                           /* Cell_id */

  sys_plmn_id_s_type       plmn_id;
                           /*  PLMN ID of serving system */

  sys_lac_type             lac_id;
                           /*  LAC of serving system */

  uint16                   arfcn;
                           /* ARFCN of serving system - applicable only in GSM mode, 
                           ** range 0..1023 
                           */

  uint8                    bsic;
                           /* BSIC  of serving system  - applicable only in GSM mode
                           ** range 0…63, refer 24.008 10.5.1.3
                           */

  uint16                   psc;
                           /* Indicates the serving PSC */

  uint16                   uarfcn_dl;
                           /*  down link UARFCN of serving system  */

  uint16                   uarfcn_ul;         
                           /*  uplink UARFCN of serving system  */

  uint16                   refpn;
                           /* PN Number of the Base Station.Applicable only in CDMA mode */
						   
}sys_cell_info_s_type;


/** Mobile Country Code type.
*/
typedef uint32                        sys_mcc_type;

/** Mobile Network Code type.
*/
typedef uint32                        sys_mnc_type;


/** Type for profile Id.
*/
typedef byte                          sys_profile_id_type;


#define SYS_NW_PROFILE_ID_INVALID     (sys_profile_id_type)(-1)

/*------------------------------------------------------------------------------
   System Identifier Functions
------------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_plmn_get_digits

@DESCRIPTION

  Function that returns the each of the digits in the MCC and MNC contained in
  a PLMN ID as integer values.

@PARAMETERS

  sys_plmn_id_s_type                   plmn provided as input from which
            plmn                       to read the MCC and MNC.
  uint32*   mcc_digit_1_ptr            First digit of the MCC.
  uint32*   mcc_digit_2_ptr            Second digit of the MCC.
  uint32*   mcc_digit_3_ptr            Third digit of the MCC.
  uint32*   mnc_digit_1_ptr            First digit of the MNC.
  uint32*   mnc_digit_2_ptr            Second digit of the MNC.
  uint32*   mnc_digit_3_ptr            Third digit of the MNC.

@RETURN VALUE

  void

==============================================================================*/
extern void sys_plmn_get_digits
(
  sys_plmn_id_s_type plmn_id,
  uint32*            mcc_digit_1_ptr,
  uint32*            mcc_digit_2_ptr,
  uint32*            mcc_digit_3_ptr,
  uint32*            mnc_digit_1_ptr,
  uint32*            mnc_digit_2_ptr,
  uint32*            mnc_digit_3_ptr
);


/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_plmn_get_mcc_mnc

@DESCRIPTION

  Function that returns the MCC and MNC contained in a PLMN ID as integer
  values.

@PARAMETERS

  sys_plmn_id_s_type                   plmn provided as input from which
            plmn                       to read the MCC and MNC.
  boolean*  plmn_is_undefined_ptr      Flag indicating that the input plmn
                                       contains a defined MCC and MNC (FALSE)
                                       or was set to 0xFFFFFF (TRUE).
  boolean*  mnc_includes_pcs_digit_ptr Flag indicating whether the MNC included
                                       the third PCS digit (TRUE) or not (FALSE)
  uint32*   mcc_ptr                    MCC converted from Binary Coded Decimal
                                       Digits in the plmn to an integer.
  uint32*   mnc_ptr                    MNC converted from Binary Coded Decimal
                                       Digits in the plmn to an integer.

@RETURN VALUE

  void

==============================================================================*/
extern void sys_plmn_get_mcc_mnc
(
  sys_plmn_id_s_type  plmn,
  boolean*            plmn_is_undefined_ptr,
  boolean*            mnc_includes_pcs_digit_ptr,
  sys_mcc_type*       mcc_ptr,
  sys_mnc_type*       mnc_ptr
);


/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_plmn_set_mcc_mnc

@DESCRIPTION

  Function that fills a PLMN ID with the MCC and MNC provided as integer values.

@PARAMETERS

  boolean           mnc_includes_pcs_digit  Flag indicating whether the MNC
                                            includes the third PCS digit (TRUE)
                                            or not (FALSE)
  uint32            mcc                     MCC as an integer value (0 - 999).
  uint32            mnc                     MCC as an integer value (0 - 99 or
                                            0 - 999 if the MCS digit is
                                            included).
  sys_plmn_id_type* plmn_ptr                Pointer to the plmn to populate
                                            input from which to read the MCC and
                                            MNC.

@RETURN VALUE

  boolean  Flag indicating whether or not the plmn_ptr was successfully filled
           with values provided for the MCC and MNC.  If TRUE then *plmn_ptr
           was successfully filled.  If FALSE then the input parameters were
           determined to be out of range and *plmn_ptr was set to 0xFFFFFF.

==============================================================================*/
extern boolean sys_plmn_set_mcc_mnc
(
  boolean               mnc_includes_pcs_digit,
  sys_mcc_type          mcc,
  sys_mnc_type          mnc,
  sys_plmn_id_s_type*   plmn_ptr
);


/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_plmn_id_is_valid

@DESCRIPTION

  Function that checks if each of the digits in the input plmn are between 0
  and 9 or equal to 0xF.

@PARAMETERS

  sys_plmn_id_s_type  plmn                PLMN ID to validate.

@RETURN VALUE

  boolean  Flag indicating if the plmn is valid (TRUE) or not (FALSE).

==============================================================================*/
extern boolean sys_plmn_id_is_valid
(
  sys_plmn_id_s_type  plmn
);


/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_plmn_id_is_undefined

@DESCRIPTION

  Function that checks if each of the digits in the input plmn is equal to
  0xF.  In other words, an undefined plmn contains 0xFFFFFF.

@PARAMETERS

  sys_plmn_id_s_type  plmn                PLMN ID to check.

@RETURN VALUE

  boolean  Flag indicating if the plmn is undefined (TRUE) or not (FALSE).

==============================================================================*/
extern boolean sys_plmn_id_is_undefined
(
  sys_plmn_id_s_type  plmn
);


/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_plmn_undefine_plmn_id

@DESCRIPTION

  Function that sets each of the digits in the plmn_id to 0xF.  The resulting
  undefined *plmn_ptr contains 0xFFFFFF.

@PARAMETERS

  sys_plmn_id_s_type*  plmn_ptr           Pointer to the PLMN ID to undefine.

@RETURN VALUE

  void

==============================================================================*/
extern void sys_plmn_undefine_plmn_id
(
  sys_plmn_id_s_type*  plmn_ptr
);


/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_plmn_match

@DESCRIPTION

  Function that compares two PLMN IDs and determines if they are equal.
  plmn_2 is the ID of the PLMN broadcast by the network and plmn_1 is the
  ID of the PLMN that it is being compared against (e.g., the HPLMN stored
  on the SIM).  If the third MNC digit in the plmn_2 is 0xF, then the plmn_2
  is assumed to contain a two digit MNC so the third MNC digit is not compared.
  This function differs from sys_plmn_match in that it implements a further
  requirement defined in 3GPP TS23.122 Annex A for comparing the HPLMN in
  North American PCS networks.

@PARAMETERS

  sys_plmn_id_s_type  plmn_1                  The first PLMN ID.
  sys_plmn_id_s_type  plmn_2                  The second PLMN ID.

@RETURN VALUE

  boolean  Flag indicating whether the two PLMN IDs are equal (TRUE) or
  not (FALSE).

==============================================================================*/
extern boolean sys_plmn_match
(
  sys_plmn_id_s_type  plmn_1,
  sys_plmn_id_s_type  plmn_2
);

/**==============================================================================

@FUNCTION NAME

  sys_mcc_same_country

@DESCRIPTION

  Function to check if the MCCs are of same country. MCCs from NA with
  different values (in range 310-316) are considered as MCCs of same country.

@RETURN VALUE

  TRUE if MCCs are of same country, FALSE otherwise

==============================================================================*/

extern boolean sys_mcc_same_country
(
  uint32  mcc1,
  uint32  mcc2
);

/**===========================================================================

@FUNCTION sys_undefine_sys_id

@DESCRIPTION
  This function undefines or initializes a system identifier.

@DEPENDENCIES
  None

@RETURN VALUE
  None

@SIDE EFFECTS
  None

===========================================================================*/
extern void sys_undefine_sys_id(

  sys_sys_id_s_type     *sys_id_ptr
);


/**===========================================================================

@FUNCTION sys_sys_id_match

@DESCRIPTION
  This function compares two system identifiers.

@DEPENDENCIES
  None

@RETURN VALUE
  TRUE  = if the two system identifiers are equal
  FALSE = if the two system identifiers are not equal

  Note: TRUE is returned if both system identifiers are undefined.

@SIDE EFFECTS
  None

===========================================================================*/
extern boolean sys_sys_id_match(

  sys_sys_id_s_type     sys_id_1,
  sys_sys_id_s_type     sys_id_2
);

/**===========================================================================

@FUNCTION sys_lac_match

@DESCRIPTION
  This function compares two LAC system identifiers of a UMTS SYS ID.

@DEPENDENCIES
  None

@RETURN VALUE
  TRUE  = if the two LAC are equal
  FALSE = if the two LAC are not equal

  Note: TRUE is returned if both system identifiers are not UMTS type.

@SIDE EFFECTS
  None

===========================================================================*/
extern boolean sys_lac_match(

  sys_sys_id_s_type     sys_id_1,
  sys_sys_id_s_type     sys_id_2
);

/** @}
**
*/

#ifdef __cplusplus
}
#endif

/**==============================================================================

                            N E T W O R K   L I S T S

==============================================================================*/

/** @defgroup NL Network lists
*  @{
*/

/*------------------------------------------------------------------------------
   Network Lists Defines
------------------------------------------------------------------------------*/

#define SYS_FORBIDDEN_PLMN_LIST_MAX_LENGTH  20
  /**< Maximum number of forbidden PLMNs */

#define SYS_PRIORITY_PLMN_LIST_MAX_LENGTH   256

#if (defined(FEATURE_MM_SUPERSET) || defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_WLAN))
#define SYS_NETWORK_LIST_INFO_MAX          (int)(25)
#else
#define SYS_NETWORK_LIST_INFO_MAX          (int)(1)
#endif

/* apitrim -include */
#if (defined(FEATURE_MM_SUPERSET) || defined(FEATURE_GSM) || defined(FEATURE_WCDMA))

#define SYS_USER_PPLMN_LIST_MAX_LENGTH      85
  /**< Maximum number of user preferred/controlled PLMNs */
#define SYS_PLMN_LIST_MAX_LENGTH            40

#else

#define SYS_USER_PPLMN_LIST_MAX_LENGTH      1
  /**< Maximum number of user preferred/controlled PLMNs */
#define SYS_PLMN_LIST_MAX_LENGTH            1

#endif
/* apitrim -end-include */

/*------------------------------------------------------------------------------
   Network Lists Enums
------------------------------------------------------------------------------*/
/**
** The following enumerated types defines whether the PLMN in the PLMN list
** is a
**
**     Home PLMN
**     User Controlled PLMN (preferred) stored on the SIM
**     Operator Controlled PLMN stored on the SIM
**     Other PLMN
**     Forbidden PLMN
*/

typedef enum
{
  SYS_DETAILED_PLMN_LIST_CATEGORY_NONE = -1,     /* FOR INTERNAL USE ONLY! */
  SYS_DETAILED_PLMN_LIST_CATEGORY_HPLMN,
  SYS_DETAILED_PLMN_LIST_CATEGORY_PREFERRED,
  SYS_DETAILED_PLMN_LIST_CATEGORY_USER_PREFERRED,
  SYS_DETAILED_PLMN_LIST_CATEGORY_OPERATOR_PREFERRED,
  SYS_DETAILED_PLMN_LIST_CATEGORY_OTHER,
  SYS_DETAILED_PLMN_LIST_CATEGORY_MAX            /* FOR INTERNAL USE ONLY! */
} sys_detailed_plmn_list_category_e_type;


/**
** The following enumerated type defines the PLMN's signal quality.
*/
typedef enum
{
  SYS_SIGNAL_QUALITY_NONE = -1,                  /* FOR INTERNAL USE ONLY! */
  SYS_SIGNAL_QUALITY_HIGH,
  SYS_SIGNAL_QUALITY_LOW,
  SYS_SIGNAL_QUALITY_MAX                         /* FOR INTERNAL USE ONLY! */
} sys_signal_quality_e_type;


/**
** The following enumerated type defines the access technologies that are
** defined for the elementary file, EFPLMNwAcT (User controlled PLMN selector
** with Access Technology).  The #defines are to be used in a bit mask.
** Refer to TS 11.11.
*/
typedef uint16 sys_access_tech_e_type;

/** GSM access technology.
*/
#define  SYS_ACT_GSM_MASK         ( (sys_access_tech_e_type) (1<<0) )

/** GSM compact access technology.
*/
#define  SYS_ACT_GSM_COMPACT_MASK ( (sys_access_tech_e_type) (1<<1) )

/** UMTS radio access technology.
*/
#define  SYS_ACT_UMTS_MASK        ( (sys_access_tech_e_type) (1<<2) )


/**
** The following enumerated type used to specify the status of Manual PLMN 
** Search.
**
*/

typedef enum
{
  SYS_PLMN_LIST_NONE = -1,       /* FOR INTERNAL USE ONLY */
  SYS_PLMN_LIST_SUCCESS,
  SYS_PLMN_LIST_AS_ABORT,
  SYS_PLMN_LIST_MAX              /* FOR INTERNAL USE ONLY */
} sys_plmn_list_status_e_type;

/*------------------------------------------------------------------------------
   Network Lists Data Types
------------------------------------------------------------------------------*/
/**
** The following type defines the PLMN's the signal strength in
** units that allow direct comparison of the signal strength between
** GSM and WCDMA cells.
**
*/
typedef int32 sys_signal_strength_type;


typedef struct sys_detailed_plmn_list_info_s
{
  /** The ID of the PLMN.
  */
  sys_plmn_id_s_type                        plmn;

  /** The radio access technology of the PLMN.
  */
  sys_radio_access_tech_e_type              rat;

  /** Indicates the type of service domain the PLMN can provide.
  */
  sys_srv_capability_e_type                 plmn_service_capability;

  /** Indicates if the PLMN is forbidden.
  */
  boolean                                   plmn_forbidden;

  /** Indicates the type of PLMN.
  */
  sys_detailed_plmn_list_category_e_type    list_category;

  /** The signal quality of the PLMN.
  */
  sys_signal_quality_e_type                 signal_quality;

  /** The signal strength of the PLMN.
  */
  sys_signal_strength_type                  signal_strength;

} sys_detailed_plmn_list_info_s_type;


/**
** Define a structure for the detailed PLMN list.
*/
typedef struct sys_detailed_plmn_list_s
{
  /** The number of PLMNs in the list.
  */
  uint32                                    length;

  /** PLMN info.
  */
  sys_detailed_plmn_list_info_s_type        info[SYS_PLMN_LIST_MAX_LENGTH];

} sys_detailed_plmn_list_s_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FIELD sys_detailed_plmn_list_s.info VARRAY SYS_PLMN_LIST_MAX_LENGTH
    LENGTH sys_detailed_plmn_list_s.length */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/**
** Define a structure for simple PLMN lists.
*/
typedef struct sys_plmn_list_s
{
  /** The number of PLMNs in the list.
  */
  uint32                                    length;

  sys_plmn_id_s_type                        plmn[SYS_PLMN_LIST_MAX_LENGTH];
} sys_plmn_list_s_type;


/**
** Define a structure that contains information regarding the user
** preferred/controlled network.  The access_tech_mask is a bit mask
** indicated by the sys_access_tech_e_type enumerates.
*/
typedef struct sys_user_pref_plmn_list_info_s
{
  sys_plmn_id_s_type                        plmn;
  sys_access_tech_e_type                    access_tech;
} sys_user_pref_plmn_list_info_s_type;


/**
** Define a structure for the list of user preferred/controlled networks.
*/
typedef struct sys_user_pref_plmn_list_s
{
  /** The number of PLMNs in the list.
  */
  byte                                      length;

  sys_user_pref_plmn_list_info_s_type       info[SYS_USER_PPLMN_LIST_MAX_LENGTH];
} sys_user_pref_plmn_list_s_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FIELD sys_user_pref_plmn_list_s.info VARRAY SYS_USER_PPLMN_LIST_MAX_LENGTH
    LENGTH sys_user_pref_plmn_list_s.length */
#endif /* FEATURE_HTORPC_METACOMMENTS */

#define WPA_RSN_MAX_PAIRWISE_CIPHER_SUITE  5
#define WPA_RSN_MAX_AKM_SUITE              2

/** Type of WLAN Security.
*/
typedef enum
{
  SYS_TECH_WLAN_SEC_NONE = 0,
  SYS_TECH_WLAN_SEC_WEP  = 1,
  SYS_TECH_WLAN_SEC_WPA  = 2,
  SYS_TECH_WLAN_SEC_WPA2 = 3
} sys_wlan_security_e_type;

/** Type of Cipher Suite.
*/
typedef enum
{
  SYS_WLAN_SEC_CIPHER_SUITE_WEP_40  = 1,
  SYS_WLAN_SEC_CIPHER_SUITE_TKIP    = 2,
  SYS_WLAN_SEC_CIPHER_SUITE_RSVD    = 3,
  SYS_WLAN_SEC_CIPHER_SUITE_CCMP    = 4,
  SYS_WLAN_SEC_CIPHER_SUITE_WEP_104 = 5,
  SYS_WLAN_SEC_CIPHER_SUITE_VENDOR_SPECIFIC = 6,
  SYS_WLAN_SEC_CIPHER_SUITE_INVALID = 7
} sys_wlan_cipher_suite_e_type;

/** Type of AKM Suite.
*/
typedef enum
{
  SYS_WLAN_SEC_AKM_SUITE_RSVD    = 0,
  SYS_WLAN_SEC_AKM_SUITE_8021X   = 1,
  SYS_WLAN_SEC_AKM_SUITE_PSK     = 2,
  SYS_WLAN_SEC_AKM_SUITE_VENDOR_SPECIFIC = 3,
  SYS_WLAN_SEC_AKM_SUITE_INVALID = 4     
} sys_wlan_akm_suite_e_type;

/** Type that holds the WLAN WPA security Info.
*/
typedef struct sys_wlan_security_wpa_rsn_info_type
{
  sys_wlan_cipher_suite_e_type  grp_cipher_suite;
 
  uint16                        pairwise_cipher_count;
  sys_wlan_cipher_suite_e_type  pairwise_cipher_suites[WPA_RSN_MAX_PAIRWISE_CIPHER_SUITE];
 
  uint16                        akm_count;
  sys_wlan_akm_suite_e_type     akm_suites[WPA_RSN_MAX_AKM_SUITE];

} sys_wlan_security_wpa_rsn_info_s_type;
 
/** Type that holds the WLAN security Info.
*/
typedef struct sys_wlan_security_info_s
{
 
  /** Type of WLAN security.
  */
  sys_wlan_security_e_type                 sec_type;

  /** WLAN WPA security Info.
  */
  sys_wlan_security_wpa_rsn_info_s_type    wpa_rsn_info;
  
} sys_wlan_security_info_s_type;

/** Type that holds the BSS information in WLAN.
*/
typedef struct sys_wlan_bss_info_s
{
  /** Channel on which this BSS provides service.
  */
  sys_chan_type                             chan;

  /** Band in which this BSS provides service.
  */
  sys_band_class_e_type                     band;

  /** Type of BSS - Infrastructure or independent.
  */
  sys_wlan_bss_e_type                       bss_type;

  /** BSS Id.
  */
  sys_wlan_bssid_type                       bssid;

  /** SS Id.
  */
  sys_wlan_ssid_s_type                      ssid;

  /** Measured RSSI of the BSS.
  */
  uint16                                    rssi;

  /** WLAN technology in use.
  */
  sys_tech_e_type                           tech;

  /** WLAN Security in use.
  */
  sys_wlan_security_info_s_type             security_info;

} sys_wlan_bss_info_s_type;

typedef struct sys_wlan_bss_info_list_s
{
    /** Number of BSS in this list.
    */
    int                            num_bss;

    /** BSS List
    */
    sys_wlan_bss_info_s_type       bss_info[SYS_NETWORK_LIST_INFO_MAX];
}sys_wlan_bss_info_list_s_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FIELD sys_wlan_bss_info_list_s.bss_info VARRAY SYS_NETWORK_LIST_INFO_MAX
    LENGTH sys_wlan_bss_info_list_s.num_bss */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/** Type that captures the network list info after power scan/survey.
*/
typedef struct sys_network_info_s
{
  /** Type of network - WLAN, GW PLMN etc.
  */
  sys_sys_mode_e_type                      type;

  /** Network info.
  */
  union
  {
    sys_wlan_bss_info_s_type               wlan_bss;

    sys_detailed_plmn_list_info_s_type     plmn;

  } info;

} sys_network_info_s_type;


/** List type for Network information.
*/
typedef struct sys_network_list_info_s
{
  /** Number of valid system Info defined in this list.
  */
  int                                       num;

  /** Network info list.
  */
  sys_network_info_s_type                   list[SYS_NETWORK_LIST_INFO_MAX];

} sys_network_list_info_s_type;


/*------------------------------------------------------------------------------
   Network Lists Functions
------------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_signal_quality_is_valid

@DESCRIPTION

  Function that verifies that the input value is within its valid range.

@PARAMETERS

  sys_signal_quality_e_type value                Value that is to be checked
                                                 for validity.

@RETURN VALUE

  boolean  Flag indicating whether the sys_signal_quality_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
extern boolean sys_signal_quality_is_valid
(
  sys_signal_quality_e_type value
);


/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_detailed_plmn_list_category_is_valid

@DESCRIPTION

  Function that verifies that the input value is within its valid range.

@PARAMETERS

  sys_detailed_plmn_list_category_e_type value   Value that is to be checked
                                                 for validity.

@RETURN VALUE

  boolean  Flag indicating whether the sys_detailed_plmn_list_category_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
extern boolean sys_detailed_plmn_list_category_is_valid
(
  sys_detailed_plmn_list_category_e_type  value
);



/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_detailed_plmn_list_found_plmn_rat

@DESCRIPTION

  Function that verifies that the given PLMN and RAT is in the PLMN list.

@PARAMETERS

  sys_detailed_plmn_list_s_type  plmn_list_ptr    The PLMN list to be searched
  sys_plmn_id_s_type             plmn             The PLMN being searched
  sys_radio_access_tech_e_type   rat              The RAT being searched

@RETURN VALUE

  boolean  Flag indicating whether the PLMN and RAT was found in the PLMN list.

==============================================================================*/
extern boolean sys_detailed_plmn_list_found_plmn_rat
(
  const sys_detailed_plmn_list_s_type*      plmn_list_ptr,
  sys_plmn_id_s_type                        plmn,
  sys_radio_access_tech_e_type              rat
);


/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_plmn_list_found_plmn_rat

@DESCRIPTION

  Function that verifies that the given PLMN is in the PLMN list.

@PARAMETERS

  sys_plmn_list_s_type           plmn_list_ptr    The PLMN list to be searched
  sys_plmn_id_s_type             plmn             The PLMN being searched

@RETURN VALUE

  boolean  Flag indicating whether the PLMN and RAT was found in the PLMN list.

==============================================================================*/
extern boolean sys_plmn_list_found_plmn
(
  const sys_plmn_list_s_type*               plmn_list_ptr,
  sys_plmn_id_s_type                        plmn
);


/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_detailed_plmn_list_info_is_valid

@DESCRIPTION

  Function that verifies that the input value is within its valid range.

@PARAMETERS

  sys_detailed_plmn_list_info_s_type value       Value that is to be checked
                                                 for validity.

@RETURN VALUE

  boolean  Flag indicating whether the sys_detailed_PLMN_list_info_s_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
extern boolean sys_detailed_plmn_list_info_is_valid
(
  sys_detailed_plmn_list_info_s_type value
);


/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_detailed_plmn_list_is_valid

@DESCRIPTION

  Function that verifies that the input value is within its valid range.

@PARAMETERS

  sys_detailed_plmn_list_s_type value            Value that is to be checked
                                                 for validity.

@RETURN VALUE

  boolean  Flag indicating whether the sys_detailed_plmn_list_s_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
extern boolean sys_detailed_plmn_list_is_valid
(
  const sys_detailed_plmn_list_s_type* value_ptr
);


/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_plmn_list_is_valid

@DESCRIPTION

  Function that verifies that the input value is within its valid range.

@PARAMETERS

  sys_PLMN_list_s_type value                     Value that is to be checked
                                                 for validity.

@RETURN VALUE

  boolean  Flag indicating whether the sys_plmn_list_s_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
extern boolean sys_plmn_list_is_valid
(
  const sys_plmn_list_s_type* value_ptr
);



/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_user_pref_plmn_list_info_is_valid

@DESCRIPTION

  Function that verifies that the input value is within its valid range.

@PARAMETERS

  sys_user_pref_plmn_list_info_s_type  value     Value that is to be checked
                                                 for validity.

@RETURN VALUE

  boolean  Flag indicating whether the sys_user_pref_plmn_list_info_s_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
extern boolean sys_user_pref_plmn_list_info_is_valid
(
  sys_user_pref_plmn_list_info_s_type  value
);


/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_user_pref_plmn_list_is_valid

@DESCRIPTION

  Function that verifies that the input value is within its valid range.

@PARAMETERS

  sys_user_pref_plmn_list_s_type  value          Value that is to be checked
                                                 for validity.

@RETURN VALUE

  boolean  Flag indicating whether the sys_user_pref_plmn_list_s_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
extern boolean sys_user_pref_plmn_list_is_valid
(
  const sys_user_pref_plmn_list_s_type*  value_ptr
);

#ifdef __cplusplus
}
#endif

/* @}
**
*/

/**==============================================================================

                M O B I L I T Y   M A N A G E M E N T   I N F O

==============================================================================*/

/* @defgroup MMI Mobility Management Info
** @{
*/

/*------------------------------------------------------------------------------
   Mobility Management Info Defines
------------------------------------------------------------------------------*/
/**
** Define constants used by for mobility management information.
** According to the standard, 3GPP TS 24.0008, the short name and
** full name can be from 3 to N characters in length.
*/
#if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
#else
#define SYS_MAX_SHORT_NAME_CHARS      1
#define SYS_MAX_FULL_NAME_CHARS       1
#endif

/* To have consistent value in RPC files for interface files,
** these feature defintions are used
*/ 
#define SYS_INTERFACE_MAX_SHORT_NAME_CHARS      255
#define SYS_INTERFACE_MAX_FULL_NAME_CHARS       255


/*------------------------------------------------------------------------------
   Mobility Management Enums
------------------------------------------------------------------------------*/

/**
** Define a type for the Network Name Coding Scheme.  The format is
** specified by 3GPP TS 24.008 and 03.38.
*/
typedef enum
{
  SYS_NW_NAME_CODING_SCHEME_NONE                             = -1,
    /**< FOR INTERNAL USE ONLY */

  SYS_NW_NAME_CODING_SCHEME_CELL_BROADCAST_DEFAULT_ALPHABET  =  0,
    /**< Cell Broadcast data coding, default alphabet, language unspecified */

  SYS_NW_NAME_CODING_SCHEME_UCS2                             =  1,
    /**< UCS2 coding scheme */

  SYS_NW_NAME_CODING_SCHEME_RESERVED                         =  2,
    /**< Reserved */

  SYS_NW_NAME_CODING_SCHEME_MAX
    /**< FOR INTERNAL USE ONLY */
} sys_network_name_coding_scheme_type_e_type;

/**
** Define a type for the Localized Service Area identity.  The format
** is specified by 3GPP TS 24.008 and 23.003.
*/
typedef enum
{

  SYS_LSA_ID_TYPE_NONE         = -1,             /**< FOR INTERNAL USE ONLY! */
  SYS_LSA_ID_TYPE_PLMN_SIG_NUM =  0,             /**< The LSA is a PLMN      */
                                                 /**< significant number     */
  SYS_LSA_ID_TYPE_UNIVERSAL    =  1,             /**< The LSA is a universal */
                                                 /**< LSA                    */
  SYS_LSA_ID_TYPE_MAX                            /**< FOR INTERNAL USE ONLY! */
} sys_lsa_identity_type_e_type;


/**
** Define a enumerated type for the daylight savings adjustment.
** The values for the enumerated constants are dictated by 3GPP
** TS 24.008.
**/
typedef enum
{
  SYS_DAY_SAV_ADJ_NONE          = -1,            /* FOR INTERNAL USE ONLY! */
  SYS_DAY_SAV_ADJ_NO_ADJUSTMENT =  0,
  SYS_DAY_SAV_ADJ_PLUS_ONE_HOUR =  1,
  SYS_DAY_SAV_ADJ_PLUS_TWO_HOUR =  2,
  SYS_DAY_SAV_ADJ_MAX                            /* FOR INTERNAL USE ONLY! */
} sys_daylight_saving_adj_e_type;


/*------------------------------------------------------------------------------
   Mobility Management Info Data Types
------------------------------------------------------------------------------*/
/**
** Define types for the network's short name and full name.
**/
typedef struct sys_network_short_name_s
{
  uint8                                       length;
    /**< Number of characters in the short name */

  sys_network_name_coding_scheme_type_e_type  coding_scheme;
    /**< The coding scheme used to encode the short name of the network */

  boolean                                     add_country_initials;
    /**< Flag that indicates whether MS should add the country initials to
       the short name of the network */

  uint8                                       name[SYS_INTERFACE_MAX_SHORT_NAME_CHARS];
    /**< The short name of the network          */

  byte                                         spare_bits;
    /**< Number of spare bits in the end octet of n/w short name */

} sys_network_short_name_s_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FIELD sys_network_short_name_s.name VARRAY SYS_INTERFACE_MAX_SHORT_NAME_CHARS
    LENGTH sys_network_short_name_s.length */
#endif /* FEATURE_HTORPC_METACOMMENTS */

typedef struct sys_network_full_name_s
{
  uint8                                        length;
    /**< Number of characters in the full name */

  sys_network_name_coding_scheme_type_e_type   coding_scheme;
  /**< The coding scheme used to encode the full name of the network */

  boolean                                      add_country_initials;
    /**< Flag that indicates whether MS should add the country initials to
       the full name of the network */

  uint8                                        name[SYS_INTERFACE_MAX_FULL_NAME_CHARS];
    /**< The full name of the network          */
  
  byte                                         spare_bits;
    /**< Number of spare bits in the end octet of n/w full name */
  
} sys_network_full_name_s_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FIELD sys_network_full_name_s.name VARRAY SYS_INTERFACE_MAX_FULL_NAME_CHARS
    LENGTH sys_network_full_name_s.length */
#endif /* FEATURE_HTORPC_METACOMMENTS */


/* No information on LTM is present
** in 3.7.2.3.2.26 Sync Channel Message
** or HDR sector parameter message
** Could have also be named as NO_INFO.
*/
#define SYS_LTM_OFFSET_INVALID     0xFF

/* No information on Leap seconds is available
** at present from  Sync Channel Message
** or sector parameter message
*/
#define SYS_LEAP_SECONDS_NO_INFO   0xFF

/* Daylight savings information
** 3.7.2.3.2.26 Sync Channel Message only
** This is not present in HDR sector parameter msg
*/
typedef enum {

    SYS_DAYLT_SAVINGS_NONE    = -1,
      /* INTERNAL USE */

    SYS_DAYLT_SAVINGS_OFF = 0,
      /* daylight savings not in effect */

    SYS_DAYLT_SAVINGS_ON = 1,
      /* daylight savings in effect */

    SYS_DAYLT_SAVINGS_MAX
      /* INTERNAL USE */

} sys_daylt_savings_e_type;

#define   SYS_DAYLT_SAVINGS_NO_INFO SYS_DAYLT_SAVINGS_OFF
#define   SYS_DAYLT_SAVINGS_INVALID SYS_DAYLT_SAVINGS_OFF

/* Define a type for CDMA time related info obtained from the sync
** channel message
*/
typedef struct sys_cdma_time_s { 

  byte                     lp_sec;             
    /**< Number of leap seconds that have occurred since
    ** the start of System Time 
    */
  
  byte                     ltm_offset; 
    /**< Offset of local time from System Time 
    **  It is a two's complement value.
    */

  sys_daylt_savings_e_type daylt_savings;   
    /**< Daylight savings indicator
    */
 
} sys_cdma_time_s_type;   

/*
** Define a type for HDR system time obtained from the sector parameter
**  message
*/
typedef struct sys_hdr_time_s  
{
  byte lp_sec;   
    /**< Number of leap seconds that have occurred since
    ** the start of System Time */
          
  int16 ltm_offset;                      
    /**< Offset of local time from System Time 
    */

}sys_hdr_time_s_type;

/**
** Union to hold  CDMA and HDR time related information
*/
typedef union sys_time_info_u {

  sys_cdma_time_s_type    cdma_time;
    /**< Time obtained from CDMA sync channel message 
    */

  sys_hdr_time_s_type     hdr_time;
    /**< Time obtained from HDR sector parameter message 
    */

} sys_time_info_u_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ IF (_DISC_ == SYS_SYS_MODE_CDMA ) sys_time_info_u.cdma_time */
/*~ IF (_DISC_ == SYS_SYS_MODE_HDR ) sys_time_info_u.hdr_time */
/*~ DEFAULT sys_time_info_u.void */
#endif

/**
** Type to hold information for CDMA/HDR time related information
** The sys_mode parameter acts as discriminator to see which time info is
** valid. If sys_mode is SYS_SYS_MODE_NO_SRV, time info is invalid.
*/
typedef struct sys_time_info_s { 

  sys_sys_mode_e_type    sys_mode;
    /**< Mode (CDMA or HDR ) which has last updated the time 
    */

  sys_time_info_u_type    time;
    /**< Union of cdma and hdr time info structures. 
    ** Both are not the same since hdr does not provide daylight saving info
    */ 
  
} sys_time_info_s_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FIELD sys_time_info_s.time DISC sys_time_info_s.sys_mode */
#endif

/**
** Define a type for time zone.  The time zone is an offset from
** Universal time (i.e. difference between local time and Universal
** time) in increments of 15 minutes.  The format for this type is
** specified in 3G TS 24.008 and 23.040.
*/
typedef int sys_time_zone_type;

/**
** Define a structure for the Time Zone and Universal Time mobility
** management information.  Each field is received as Binary Coded
** Digits as specified in 3GPP TS 24.008 and 23.040.
*/
typedef struct sys_time_and_time_zone_s
{
  uint8                                     year;
  uint8                                     month;
  uint8                                     day;
  uint8                                     hour;
  uint8                                     minute;
  uint8                                     second;
  sys_time_zone_type                        time_zone;
} sys_time_and_time_zone_s_type;

/**
** Define a type for the Localized Service Area identity.  The format
** is specified by 3GPP TS 24.008 and 23.003.
*/
typedef struct sys_lsa_identity_s
{
  sys_lsa_identity_type_e_type              type;
  uint8                                     identity[3];
} sys_lsa_identity_s_type;

/**
** Define a structure that contains the currently registered network's
** mobility management information (i.e. network's name, date, time and
** time zone).  All fields in this structure are optional.  If the
** "available" flag is true, then the corresponding data field contains
** valid information.  If the "available" flag is false, then the
** corresponding data field should be ignored.
*/
typedef struct sys_mm_information_s
{
  /**
  ** Flags that indicate whether the particular mm information is
  ** available.
  **/
  boolean                                   plmn_avail;
  boolean                                   full_name_avail;
  boolean                                   short_name_avail;
  boolean                                   univ_time_and_time_zone_avail;
  boolean                                   time_zone_avail;
  boolean                                   lsa_identity_avail;
  boolean                                   daylight_saving_adj_avail;

  sys_plmn_id_s_type                        plmn;
  sys_network_full_name_s_type              full_name;
  sys_network_short_name_s_type             short_name;
  sys_time_and_time_zone_s_type             univ_time_and_time_zone;
  sys_time_zone_type                        time_zone;
  sys_lsa_identity_s_type                   lsa_identity;
  sys_daylight_saving_adj_e_type            daylight_saving_adj;
} sys_mm_information_s_type;



/*------------------------------------------------------------------------------
   Mobility Management Information Functions
------------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_network_name_coding_scheme_is_valid

@DESCRIPTION

  Function that verifies that the input value is within its valid range.

@PARAMETERS

   sys_network_name_coding_scheme_type_e_type value
                             Value that is to be checked for validity.

@RETURN VALUE

  boolean  Flag indicating whether the sys_network_name_coding_scheme_type_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
boolean sys_network_name_coding_scheme_is_valid
(
  sys_network_name_coding_scheme_type_e_type value
);

/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_network_short_name_is_valid

@DESCRIPTION

  Function that verifies that the input value is within its valid range.

@PARAMETERS

  const sys_network_short_name_s_type* value_ptr Pointer to the value that is
                                                 to be checked for validity.

@RETURN VALUE

  boolean  Flag indicating whether the sys_network_short_name_s_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
extern boolean sys_network_short_name_is_valid
(
  const sys_network_short_name_s_type* value_ptr
);


/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_network_full_name_is_valid

@DESCRIPTION

  Function that verifies that the input value is within its valid range.

@PARAMETERS

  const sys_network_full_name_s_type* value_ptr  Pointer to the value that is
                                                 to be checked for validity.

@RETURN VALUE

  boolean  Flag indicating whether the sys_network_full_name_s_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
extern boolean sys_network_full_name_is_valid
(
  const sys_network_full_name_s_type* value_ptr
);


/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_time_zone_is_valid

@DESCRIPTION

  Function that verifies that the input value is within its valid range.

@PARAMETERS

  sys_time_zone_type value                       Value that is to be checked
                                                 for validity.

@RETURN VALUE

  boolean  Flag indicating whether the sys_time_zone_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
extern boolean sys_time_zone_is_valid
(
  sys_time_zone_type value
);


/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_time_and_time_zone_is_valid

@DESCRIPTION

  Function that verifies that the input value is within its valid range.

@PARAMETERS

  const sys_time_and_time_zone_s_type* value_ptr Pointer to the value that is
                                                 to be checked for validity.

@RETURN VALUE

  boolean  Flag indicating whether the sys_time_and_time_zone_s_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
extern boolean sys_time_and_time_zone_is_valid
(
  const sys_time_and_time_zone_s_type* value_ptr
);


/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_lsa_id_type_is_valid

@DESCRIPTION

  Function that verifies that the input value is within its valid range.

@PARAMETERS

  sys_LSA_identity_type_e_type value             Value that is to be checked
                                                 for validity.

@RETURN VALUE

  boolean  Flag indicating whether the sys_LSA_identity_type_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
extern boolean sys_lsa_id_type_is_valid
(
  sys_lsa_identity_type_e_type value
);


/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_lsa_id_is_valid

@DESCRIPTION

  Function that verifies that the input value is within its valid range.

@PARAMETERS

  sys_LSA_identity_s_type value                  Value that is to be checked
                                                 for validity.

@RETURN VALUE

  boolean  Flag indicating whether the sys_LSA_identity_s_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
extern boolean sys_lsa_id_is_valid
(
  sys_lsa_identity_s_type value
);


/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_day_sav_adj_is_valid

@DESCRIPTION

  Function that verifies that the input value is within its valid range.

@PARAMETERS

  sys_daylight_saving_adj_e_type value           Value that is to be checked
                                                 for validity.

@RETURN VALUE

  boolean  Flag indicating whether the sys_daylight_saving_adj_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
extern boolean sys_day_sav_adj_is_valid
(
  sys_daylight_saving_adj_e_type value
);


/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_mm_information_is_valid

@DESCRIPTION

  Function that verifies that the input value is within its valid range.

@PARAMETERS

  const sys_mm_information_s_type* value_ptr     Pointer to the value that is
                                                 to be checked for validity.

@RETURN VALUE

  boolean  Flag indicating whether the sys_mm_information_s_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
extern boolean sys_mm_information_is_valid
(
  const sys_mm_information_s_type* value_ptr
);

#ifdef __cplusplus
}
#endif

/* @}
**
*/

/**==============================================================================

                     M E N U  S E L E C T I O N

==============================================================================*/

/* @defgroup MS Menu Selection
** @{
*/

/*------------------------------------------------------------------------------
   Menu Selection Enums
------------------------------------------------------------------------------*/

/* Enumerations for M51 mode settings - FEATURE_JCDMA_1X
*/

/**---------------------------------------------------------------------------
  M511 Table values.
---------------------------------------------------------------------------*/
typedef enum
{
  SYS_JCDMA_M511_MIN = -1,        /**< used only for bounds checking       */
  SYS_JCDMA_M511_PKT,             /**< User picked packet from M511 menu   */
  SYS_JCDMA_M511_ASYNC_FAX,       /**< User picked async/fax from M511 menu*/
  SYS_JCDMA_M511_MAX              /**< Used for arrays & bounds checking   */
} sys_jcdma_m511_e_type;

/**---------------------------------------------------------------------------
  M512 Table values.
---------------------------------------------------------------------------*/
typedef enum
{
  SYS_JCDMA_M512_MIN = -1,        /**< used only for bounds checking       */
  SYS_JCDMA_M512_STD,             /**< User picked standard data rate      */
  SYS_JCDMA_M512_HIGH,            /**< User picked high data rate          */
  SYS_JCDMA_M512_MAX              /**< Used for arrays & bounds checking   */
} sys_jcdma_m512_e_type;

/**---------------------------------------------------------------------------
  M513 Table values.
---------------------------------------------------------------------------*/
typedef enum
{
  SYS_JCDMA_M513_MIN = -1,        /**< used only for bounds checking       */
  SYS_JCDMA_M513_AUTO,            /**< AUTO setting                        */
  SYS_JCDMA_M513_19200,           /**< 19200 bps                           */
  SYS_JCDMA_M513_115200,          /**< 115200 bps                          */
  SYS_JCDMA_M513_230400,          /**< 230400                              */
  SYS_JCDMA_M513_MAX              /**< Used for arrays & bounds checking   */
} sys_jcdma_m513_e_type;



/**==============================================================================

                     UMTS to 1X handover types

==============================================================================*/

/** Maximum size(bytes) of the IS2000 Message.
*/
#define    SYS_MAX_IS2000_MSG_SIZE     (256)


/** Invalid is2000 msg type.
*/
#define    SYS_INVALID_IS2000_MSG      (uint8)(-1)

/** UMTS->CDMA Handover status causes.
*/
typedef enum
{
  SYS_UMTS_CDMA_HANDOVER_NONE,
  SYS_UMTS_CDMA_HANDOVER_SUCCESS,
  SYS_UMTS_CDMA_HANDOVER_ERR_NO_SRV,
  SYS_UMTS_CDMA_HANDOVER_ERR_ACC_IN_PROG,
  SYS_UMTS_CDMA_HANDOVER_ERR_IN_TC,
  SYS_UMTS_CDMA_HANDOVER_ERR_UHDM_PARSE,
  SYS_UMTS_CDMA_HANDOVER_ERR_CDMA_LOCK,
  SYS_UMTS_CDMA_HANDOVER_ERR_OTHERS,
  SYS_UMTS_CDMA_HANDOVER_ERR_BAD_MSG_TYPE,
  SYS_UMTS_CDMA_HANDOVER_MAX

}sys_umts_cdma_handover_e_type;



typedef enum
{
  SYS_HO_STATUS_NONE,
  SYS_HDR_CDMA_HO_SUCCESS,
  SYS_HDR_CDMA_HO_ERR_NO_SRV,
  SYS_HDR_CDMA_HO_ERR_ACC_IN_PROG,
  SYS_HDR_CDMA_HO_ERR_IN_TC,
  SYS_HDR_CDMA_HO_ERR_UHDM_PARSE,
  SYS_HDR_CDMA_HO_ERR_CDMA_LOCK,
  SYS_HDR_CDMA_HO_ERR_OTHERS,

  SYS_HO_STATUS_MAX
} sys_ho_status_e_type;

/** From and To technology in hand over operation
*/
typedef enum
{
  SYS_HO_TYPE_NONE = -1,

  SYS_HO_TYPE_VOIP_HDR_1X,

  SYS_HO_TYPE_UMTS_1X,

  SYS_HO_TYPE_MAX

}sys_ho_type_e_type;


/** Typedef which holds a IS2000 Message.
*/
typedef struct {

  /** Message type.
  */
  uint8                                type;

  /** Message payload length in bits max is SYS_MAX_IS2000_MSG_SIZE * 8
  */
  uint32                               payload_len;

  /** Message payload.
  */
  byte                                 payload[SYS_MAX_IS2000_MSG_SIZE];

} sys_is2000_msg_s_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FIELD sys_is2000_msg_s_type.payload VARRAY SYS_MAX_IS2000_MSG_SIZE
    LENGTH ( sys_is2000_msg_s_type.payload_len / 8 ) */
#endif /* FEATURE_HTORPC_METACOMMENTS */



/**==============================================================================

  BCMCS - FEATURE_BCMCS

  BCMCS stands for "High rate BroadCast-MultiCast packet data air interface
  Specification"


==============================================================================*/

/*------------------------------------------------------------------------------
   BCMCS Defines
------------------------------------------------------------------------------*/
#ifdef FEATURE_BCMCS_20 
#error code not present
#elif defined( FEATURE_BCMCS )
#error code not present
#else
#define SYS_BCMCS_MAX_FLOWS     1
#define SYS_BCMCS_MAX_MON_FLOWS 1
#endif
/**------------------------------------------------------------------------------
   BCMCS Enums
------------------------------------------------------------------------------*/

/** Enumeration for BCMCS data service availability
*/
typedef enum {

  SYS_BCMCS_SRV_STATUS_NONE = -1,
    /**< FOR INTERNAL USE ONLY!
    */

  SYS_BCMCS_SRV_STATUS_NO_SRV,
    /**< No BCMCS service is available
    */

  SYS_BCMCS_SRV_STATUS_SRV,
    /**< BCMCS service is available
    */

  SYS_BCMCS_SRV_STATUS_MAX
    /**< FOR INTERNAL USE OF CM ONLY!
    */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */


} sys_bcmcs_srv_status_e_type;

/** Enumeration for BCMCS flow status values
*/
typedef enum
{

  SYS_BCMCS_FLOW_STATUS_MIN = -1,
    /**< For bounds checking only
    */

  SYS_BCMCS_FLOW_STATUS_NONE = 0,
    /**< Used for initialization or to indicate that
    ** there is no flow status info sent with the
    ** event / report
    */

  /*---------------------------------------------------------------
    No-op
  ---------------------------------------------------------------*/
  SYS_BCMCS_FLOW_STATUS_CANCELLED = 100,
    /**< A new Flow Request received before the previous one is done processing.
    ** Cancel the remaining lookup and start processing the new Request.
    */

  /*---------------------------------------------------------------
    Success - Flow is monitored  (DESIRED_MONITORED)
  ---------------------------------------------------------------*/
  SYS_BCMCS_FLOW_STATUS_MONITORED = 200,
    /**< Flow is active and included in broadcast overhead msg
    */

  /*---------------------------------------------------------------
    Successful request and db lookup, but the flow cannot be 
    monitored yet.

    Considered a temporary Failure - start a deregistration timer.
  ---------------------------------------------------------------*/
  /*-------------------------------------
    DESIRED_UNMONITORED
  -------------------------------------*/
  SYS_BCMCS_FLOW_STATUS_UNABLE_TO_MONITOR = 300,
    /**< Flow is available in overhead but can not be monitored 
     * because its channel assignment conflicts with other flows
     */

  /*-------------------------------------
    DESIRED_UNAVAILABLE
  -------------------------------------*/
  SYS_BCMCS_FLOW_STATUS_REQUESTED,
    /**< Registered but not included in broadcast overhead msg
    */

  SYS_BCMCS_FLOW_STATUS_TIMEOUT,
    /**< Broadcast Supervision Timeout.  The AT is on a system where
    ** broadcast is available. The AT waited for 3 x BroadcastOverheadPeriod, 
    ** and the flow did not appear in the bcovhd message.
    */

  SYS_BCMCS_FLOW_STATUS_LOST,
    /**< The AT lost acquisition and temporarily disabled its monitored flows.
    ** It is as if the flows weren't in the BC OVHD msg as with
    ** SYS_BCMCS_FLOW_STATUS_REQUESTED. Another flow status will be sent 
    ** when the AT reacquires. 
    */

  SYS_BCMCS_FLOW_STATUS_SYS_UNAVAILABLE,
    /**< The AT cannot process the flow request because BCMCS is in
    ** CLOSED state (HDR not acquired)
    */

  SYS_BCMCS_FLOW_STATUS_REACHED_MAX_MON_FLOWS,
    /**< 
    ** The flow is available in ovhd, but the AT cannot monitor it 
    ** because it has reached the maximum number of flows it can 
    ** concurrently monitor.  
    */

  /*---------------------------------------------------------------
    Failure (permanent) - flow should be deregistered.
  ---------------------------------------------------------------*/
  /*-------------------------------------
    AN is explicitely rejecting the flow
  -------------------------------------*/
  SYS_BCMCS_FLOW_STATUS_REJECTED_NOT_AVAILABLE = 400,
    /**< Flow was rejected explicitly by the network
    ** - BCMCS Flow ID / BCMCS Program ID not available
    */

  SYS_BCMCS_FLOW_STATUS_REJECTED_NOT_TRANSMITTED,
    /**< Flow was rejected explicitly by the network
    ** - BCMCS Flow ID / BCMCS Program ID not transmitted
    */

  SYS_BCMCS_FLOW_STATUS_REJECTED_INVALID_AUTH_SIG,
    /**< Flow was rejected explicitly by the network 
     * - Invalid authorization signature
    */

  /*-------------------------------------
    DB Lookup Failure
  -------------------------------------*/
  SYS_BCMCS_FLOW_STATUS_DB_UNAVAILABLE = 500,
    /**< A lookup in the XML file couldn't be completed because the database
    ** is in an invalid state 
    */  

  SYS_BCMCS_FLOW_STATUS_NO_MAPPING,
    /**< The XML file does not contain any mapping for the current subnet.
    */

  SYS_BCMCS_FLOW_STATUS_ID_NOT_FOUND_FOR_GIVEN_MULTICAST_IP,
    /**< A lookup in the XML file can't find a FlowID for
    ** the given Multicast IP in the current subnet
    */

  SYS_BCMCS_FLOW_STATUS_MAX
    /**< For bounds checking only
    */

} sys_bcmcs_flow_status_e_type;

/* 
** Enumeration for BCMCS flow status values  
*/
typedef enum
{
  SYS_BCMCS_REG_STATUS_MIN = -1,
    /**< For bounds checking only */
 
  SYS_BCMCS_REG_STATUS_NONE = 0,
    /**< Used for initialization or to indicate that there is no 
    ** registration status info sent with the event / report */
  
  SYS_BCMCS_REG_STATUS_SUCCESS,
    /**< FlowRegistration message was sent successfully for this flow */

  SYS_BCMCS_REG_STATUS_NOT_ALLOWED,
    /**< Could not send a FlowRegistration message - not allowed */

  SYS_BCMCS_REG_STATUS_GEN_FAILURE,
    /**< Could not send a FlowRegistration message – generic failure */
 
  SYS_BCMCS_REG_STATUS_MAX
    /**< For bounds checking only */
 
} sys_bcmcs_reg_status_e_type;

/* Identifies different flow service types 
*/
typedef enum 
{
  SYS_FLOW_SERVICE_NONE = -1,
    /* For bounds checking only
    */

  SYS_FLOW_SERVICE_BCMCS = 0,
    /* Flow serv type is BCMCS
    */

  SYS_FLOW_SERVICE_MBMS = 1,
    /* Flow serv type is MBMS
    */

  SYS_FLOW_SERVICE_MAX
    /* For bounds checking only
    */

} sys_flow_service_e_type;


/*------------------------------------------------------------------------------
   BCMCS Data Types
------------------------------------------------------------------------------*/

/**
** An enum that defines all of the address types supported - used to
** discriminate the union below
** (same as ip_addr_type from ps_ip_addr.h)
*/
typedef enum
{
  SYS_IP_ANY_ADDR     = 0,
  SYS_IPV4_ADDR       = 4,
  SYS_IPV6_ADDR       = 6,
  SYS_IP_ADDR_INVALID = 255

} sys_ip_addr_enum_type;

/** IP address
*/
typedef struct sys_ip_address_s
{

  sys_ip_addr_enum_type type;

  union {

    uint32 v4;
    uint64 v6[2];
  } addr;

} sys_ip_address_s_type;

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ IF ( (_DISC_ == SYS_IP_ANY_ADDR) ||
         (_DISC_ == SYS_IPV6_ADDR) ||
         (_DISC_ == SYS_IP_ADDR_INVALID) ) 
         sys_ip_address_s.addr.v6 */
/*~ IF ( _DISC_ == SYS_IPV4_ADDR )
         sys_ip_address_s.addr.v4 */
/*~ FIELD sys_ip_address_s.addr DISC sys_ip_address_s.type */
#endif

/*---------------------------------------------------------------------------
                                MBMS
----------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
   MBMS (Multimedia broadcast multicast service) MACRO definitions.   
   MBMS is specific to WCDMA.
--------------------------------------------------------------------------*/

#define SYS_MBMS_NUM_MAX_SRVS_PER_REQ 2

/* TMGI is a parameter used in 
** MBMS service act req. Gives
** the max length of TMGI.
** TMGI stands for ???
*/
#define SYS_MAX_MBMS_TMGI_LENGTH 15

/* Maximum number of MBMS services
** that can simultaneously exist.
*/
#define SYS_MAX_MBMS_SERVICES 16


/*---------------------------------------------------------------------------
** MBMS enum definitions
---------------------------------------------------------------------------*/
typedef enum
{

  SYS_MBMS_SRV_STATUS_NONE = -1,
    /* For internal use only */

  SYS_MBMS_SRV_STATUS_UNAVAILABLE_NETWORK_FAILURE,
    /* MBMS service in unavailable due to network failure */

  SYS_MBMS_SRV_STATUS_UNAVAILABLE_SETUP_FAILURE,
    /* MBMS service in unavailable due to setup failure on UE side */

  SYS_MBMS_SRV_STATUS_UNAVAILABLE_FAILURE_OTHER,
    /* MBMS service in unavailable due to some other failure */

  SYS_MBMS_SRV_STATUS_AVAILABLE,
    /*MBMS service in available */

  SYS_MBMS_SRV_STATUS_MONITORED,
    /* MBMS service monitored */

  SYS_MBMS_SRV_STATUS_OTHER,
    /* Reserved for future use */
    
  SYS_MBMS_SRV_STATUS_RESERVED1,
    /* Reserved for future use */
    
  SYS_MBMS_SRV_STATUS_MAX
    /* For internal use only */

} sys_mbms_srv_status_e_type;


/*---------------------------------------------------------------------------
** MBMS structure definitions
---------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------
   MBMS (Multimedia broadcast multicast service) Data Types.   
--------------------------------------------------------------------------*/

/* To be discarded for phase 2a */
typedef struct
{ 

  uint32  service_id;
  boolean plmn_id_present;
  uint32  plmn_id;

} sys_mbms_tmgi_uint_old_s_type;


/* MBMS logical channel id 
*/
typedef struct
{
  /* TRUE  -  Log chan id is present
  ** FALSE -  Log chan id is not present
  */
  boolean present;

  /* Logical channel id. 
  ** Used between DS and RLC on traffic plane.
  */
  uint32  log_chan_id;

}sys_mbms_log_chan_id_s_type;


/* TMGI structure in int format */
typedef struct
{ 

  /* TRUE  -  TMGI is present
  ** FALSE -  TMGI is not present
  */
  boolean present;

  /* TMGI in uint64 format */
  uint64  tmgi;

}sys_mbms_tmgi_uint_s_type;

/* TMGI structure in array format */
typedef struct
{
  /* TRUE  -  TMGI is present
  ** FALSE -  TMGI is not present
  */
  boolean present;

  /* Length of TMGI */
  uint8   length;

  /* Actual TMGI value */
  uint8   value[SYS_MAX_MBMS_TMGI_LENGTH];

}sys_mbms_tmgi_s_type;


/* MBMS service priority */
typedef struct
{
  /* TRUE  -  Priority is present
  ** FALSE -  Priority is not present
  */
  boolean present;

  /* Priority of MBMS service request */
  uint32   priority;

}sys_mbms_srv_priority_s_type;

/* Structure to hold mbms information
*/
typedef struct
{
  /* TMGI required for MBMS service
  */
  sys_mbms_tmgi_s_type              tmgi;

  /* Converted TMGI used by MBMS mgr
  */
  sys_mbms_tmgi_uint_old_s_type     tmgi_conv;

  /* Priority reqd for MBMS 
  */
  sys_mbms_srv_priority_s_type      srv_priority;

  /* Gives conf on MBMS srv activ or deact
  ** request
  */
  sys_mbms_srv_status_e_type        srv_status;

  /* Logical channel id. Reported from MBMS mgr
  ** to CM
  */
  sys_mbms_log_chan_id_s_type       srv_log_chan_id;

}sys_mbms_srv_info_s_type;



/* Flow ID structure
**
** Overloading bcmcs flow id structure to hold MBMS
** specific information. Has been done to resue 
** cm_bcmcs_cmd_flow_request().
*/
typedef struct
{

  /* IP address for flow (MBMS and BCMCS) 
  */
  sys_ip_address_s_type ip_address;

  /* Port address for flow (MBMS and BCMCS)
  */
  uint16                port_number;

  /* Identifies the type of flow (MBMS, BCMCS...)
  */ 
  sys_flow_service_e_type  flow_service;

  /* Information specific to MBMS service request
  */
  sys_mbms_srv_info_s_type mbms_srv_info;

} sys_bcmcs_flow_addr_s_type;


/* Flow status element type
*/
typedef struct
{
  /* Address of flow depending on BCMCS or MBMS
  */
  sys_bcmcs_flow_addr_s_type   flow_address;


  sys_bcmcs_flow_status_e_type flow_status;

} sys_bcmcs_flow_status_element_s_type;

typedef uint32 sys_bcmcs_flow_id_type;


/* MBMS session id */
typedef struct
{
  /* TRUE  -  Session id is present
  ** FALSE -  Session id is not present
  */
  boolean present;

  /* Session id_type */
  uint32  session_id;

}sys_mbms_session_id_s_type;

typedef struct {

  sys_bcmcs_flow_addr_s_type   flow_address;
  sys_bcmcs_reg_status_e_type  reg_status;

} sys_bcmcs_reg_status_element_s_type;


typedef enum{

  SYS_BCMCS_REG_SETUP_MIN = -1,
  /**< For bounds checking only */

  SYS_BCMCS_REG_SETUP_ALLOWED,
  /**< Allow Registration setup */

  SYS_BCMCS_REG_SETUP_NOT_ALLOWED,
  /**< Registration setup not allowed */

  SYS_BCMCS_REG_SETUP_MAX
  /**< For bounds checking only */

}sys_bcmcs_reg_setup_e_type;

typedef enum{

  SYS_BCMCS_BOM_CACHING_MIN = -1,
  /**< For bounds checking only */

  SYS_BCMCS_BOM_CACHING_STRICT_ENABLE,
  /**< Enable Strict caching */

  SYS_BCMCS_BOM_CACHING_STRICT_DISABLE,
  /**< Disable Strict caching */

  SYS_BCMCS_BOM_CACHING_MAX
  /**< For bounds checking only */

}sys_bcmcs_bom_caching_e_type;
/**==============================================================================

                     Manual selection types

==============================================================================*/

#define  SYS_MAX_MANUAL_CHANNELS        5


/**
** Define a enumerated type that indicates the type of GW manual selection.
*/
typedef enum
{

  /* FOR INTERNAL USE ONLY!
  */
  SYS_MANUAL_SEL_TYPE_NONE = -1,

  /** System ID should be defined in manual selection.
  */
  SYS_MANUAL_SEL_TYPE_USER_SPECIFIED,

  /** Select only the HPLMN.
  */
  SYS_MANUAL_SEL_TYPE_HPLMN,

  /** Search for RPLMN only.
  */
  SYS_MANUAL_SEL_TYPE_RPLMN,

  /** FOR INTERNAL USE ONLY!
  */
  SYS_MANUAL_SEL_TYPE_MAX

} sys_manual_sel_type_e_type;

typedef struct sys_manual_sys_profile_s{

  /** Manual System Profile is stored in packed format in profile_buf. This needs to be
  ** unpacked using net_unpack_profile() to "net_profile_s_type" before
  ** using.
  */
  word                                   profile_size;
  byte                                   profile_buf[SYS_WLAN_PROFILE_MAX_SIZE];

} sys_manual_sys_profile_s_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FIELD sys_manual_sys_profile_s.profile_buf
    VARRAY SYS_WLAN_PROFILE_MAX_SIZE LENGTH sys_manual_sys_profile_s.profile_size */
#endif /*~ FEATURE_HTORPC_METACOMMENTS */

/** Type that associates the parameters that define the WLAN manual system.
*/
typedef struct sys_wlan_manual_sys_info_s
{
  /** BSSid for wlan manual selection. Can be 0.
  */
  sys_wlan_bssid_type                    bssid;

  /** Channel to search for bssid/sysid.
  ** if channel = 0, use any channel, otherwise specifies
  ** absolute channel for WLAN.
  */
  sys_chan_type                          channel;

  /** Band to use for manual system selection
  */
  sys_band_mask_type                     band;

  /** profile id to use.
  */
  sys_profile_id_type                    profile_id;

  /** Bss type.
  */
  sys_wlan_bss_e_type                    bss_type;


  /** Profile Infor for Manaul System.
  */
  sys_manual_sys_profile_s_type          profile;


} sys_wlan_manual_sys_info_s_type;


/** Type that associates the parameters that define the GSM/WCDMA manual system.
*/
typedef struct sys_gw_manual_sys_info_s
{

  /** Manual selection type.
  */
  sys_manual_sel_type_e_type             sel_type;

} sys_gw_manual_sys_info_s_type;


/** Type that associates the parameters that define the CDMA manual system.
*/
typedef struct sys_cdma_manual_sys_info_s
{

  /** Number of channels, if num_chans = 0, then use any applicable channel.
  */
  int                                    num_chans;

  /** Channels to search for bssid/sysid.
  */
  sys_chan_type                          chans[SYS_MAX_MANUAL_CHANNELS];

} sys_cdma_manual_sys_info_s_type;


/** Type that associates the parameters that define the manual system.
*/
typedef struct sys_manual_sys_info_s
{
  /** System id for manual selection. Can be wildcards also.
  */
  sys_sys_id_s_type                    sys_id;

  union
  {
    /** Additional parameters for WLAN manual system selection.
    ** Applicable only if sys_id is WLAN.
    */
    sys_wlan_manual_sys_info_s_type    wlan;

    /** Additional parameters for GSM/WCDMA manual system selection.
    ** Applicable only if sys_id is PLMN.
    */
    sys_gw_manual_sys_info_s_type      gw;

    /** Additional parameters for CDMA/HDR/AMPS manual system selection.
    ** Applicable only if sys_id is is95/is856.
    */
    sys_cdma_manual_sys_info_s_type    cdma;

  } prm;

} sys_manual_sys_info_s_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FIELD sys_manual_sys_info_s.prm DISC _OBJ_.sys_id.id_type */
/*~ CASE SYS_SYS_ID_TYPE_WLAN sys_manual_sys_info_s.prm.wlan */
/*~ CASE SYS_SYS_ID_TYPE_UMTS sys_manual_sys_info_s.prm.gw */
/*~ CASE SYS_SYS_ID_TYPE_IS95 sys_manual_sys_info_s.prm.cdma */
/*~ DEFAULT sys_manual_sys_info_s.prm.void */
#endif

/**  Dummy.
*/
typedef struct {

  int a;

} sys_wlan_acq_fail_list_info_s_type;

typedef enum 
{
  SYS_SERVICE_SEARCH_NONE = -1,
  SYS_SERVICE_SEARCH_BACKGROUND_AUTOMATIC,
  SYS_SERVICE_SEARCH_BACKGROUND_MANUAL,
  SYS_SERVICE_SEARCH_MAX
}sys_service_search_e_type;

/** Enumeration for LN notification status
*/
typedef enum
{

  SYS_HDR_LOCATION_NOTIFICATION_STATUS_NONE = -1,      /** Internal CM use */
  SYS_HDR_LOCATION_NOTIFICATION_STATUS_SUCCESSFUL,     /** LN Successful */
  SYS_HDR_LOCATION_NOTIFICATION_STATUS_FAILED,         /** LN Failed     */
  SYS_HDR_LOCATION_NOTIFICATION_STATUS_MAX             /** Internal CM use */

} sys_hdr_location_notification_status_e_type;

/*
** Enumeration of phone status change status. Indicates the status of phone
** status change command.
*/
typedef enum
{
  SYS_PH_STATUS_CHGD_NONE = -1,
    /* FOR INTERNAL USE OF CM ONLY!
    */

  /* 0 */
  SYS_PH_STATUS_CHGD_SUCCESS,
    /* Powered down success
    */

  /* 1 */
  SYS_PH_STATUS_CHGD_FAILURE,
    /* Powered down failure
    */

  SYS_PH_STATUS_CHGD_MAX
    /* FOR INTERNAL USE OF CM ONLY!
    */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} sys_ph_status_chgd_status_e_type;

/* Type to update UI with service status info
** when UE is OOS in connected mode 
*/
typedef struct sys_conn_mode_ss_ind_s
{
  /* Service status indicates service availibility */
  sys_srv_status_e_type           service_status;

  /* PLMN */
  sys_plmn_id_s_type              selected_plmn;

  /* RAT */
  sys_radio_access_tech_e_type    rat;

  /* Signal quality */
  sys_signal_quality_e_type       signal_quality;

  /* Signaling strength of the strongest cell in PLMN */
  sys_signal_strength_type        signal_strength;

  /* To store the current status of oos connected mode */
  boolean                         is_connected_mode_oos;

} sys_conn_mode_ss_ind_s_type;

typedef enum
{
    SYS_GW_CONN_STATUS_NONE = -1,
      /* FOR INTERNAL USE ONLY */
    
    SYS_GW_CONN_STATUS_CONN,   
      /* Connection (being) established */
    
    SYS_GW_CONN_STATUS_BPLMN_SRCH,   
      /* REG initiated HPLMN search is on-going. */
    
    SYS_GW_CONN_STATUS_IDLE,
      /* NAS in idle state */

    SYS_GW_CONN_STATUS_MAX
      /* FOR INTERNAL USE ONLY */

}sys_gw_conn_status_e_type;

#endif /* #ifndef SYS_H */

/* @}
** 
*/

#ifndef AEECM_H
#define AEECM_H

/*============================================================================
FILE:  AEECM.h

SERVICES:  AEE Call Manager Interface

GENERAL DESCRIPTION:
        Interface definitions, data structures, etc. for BREW Call Manager

        Copyright © 2003-2008 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/StaticExtensions/Inc/AEECM.h#94 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/15/08   sg      Add ICM_OriginateCall_SkipFDNChk to skip FDN checks.
07/09/08   sg      Add AEECM_EVENT_SS_INFO corresponding to CM_SS_EVENT_INFO
04/14/08   cvs     Fix compile warning
03/10/08   sg      Update the RSSI bars using new UI signal strength indicator 
                   algorithm
01/09/08   kac     Added enum of mode preferences for GSM_WCDMA_ONLY 
11/28/07   sg      Added AEECM_IS_MODE_PREF_WLAN to check if mode pref is WLAN
11/02/07   alb     Added MBMS API support.
10/05/07   ypmw    Fix the CNAP alpha string display
08/06/07   alb     API for editing PRL records added.
08/10/07   cvs     Add number of call ID's to ICM
07/13/07   alb     Fixed AEECM_GSMEND_SERVICE_OR_OPTION_NOT_IMPLEMENTED value
04/19/07   ypmw    Increased the password size to 63
04/10/07   ypmw    Fix the day light saving time adjustment
03/26/07   ypmw    Add EAP authentication scheme for automatic wlan mode
03/20/07   jas/ak  Adding support for persistent preference values.
01/10/07   jks     Added support for VoIP ringback (remote alert)
11/22/06   ak      Release the call if Release record is sent from n/w
11/17/06   ak      Display Protocol Revision for HDR
11/14/06   ypmw    Fix wlan compiler warnings
11/08/06   alb     Added band class 15/16 support.
09/26/06   ak      Remove featurization from ICM interface for Binary Compatibility
09/19/06   jas     Adding band value to AEECMWLANPref structure.
08/23/06   jks     Added support for WLAN Adhoc networks
05/19/06   alb     Accomodate NULL terminator in dial string buffers.
04/24/06   alb     Change to handle 80 character dial strings plus the '+'
04/18/06   alb     BCMCS annunciator support.
04/17/06   alb     Removed AEECM_EVENT_SS_CELL_SUPP_SERVICES_IND.
03/28/06   jas     Adding typecasts to macros to fix compile warnings
03/21/06   alb     Added AEECMSysHsdpaHsupa in AEECMSSInfo.
03/21/06   jas     Stripping out CM dependencies
03/21/06   jas     Modifying band pref values to match changes in CM
03/14/06   jks     Removed WLAN profile_id from AEECMWLANPref (obsolete)
03/03/06   jks     Added call event AEECM_EVENT_CALL_ALL_CALLS_ENDED
01/24/06   JKS     Added AEECM_WLAN_TECH_TO_TECH_PREF macro
01/05/06   JAS     Added support for new WCDMA band classes
11/07/05   RI      Added new mode preference for HPLMN search.
10/27/05   jks     Increased AEECM_MAX_SSID_STR_SIZE by one to allow for NULL 
                   termination of SSID strings in OEMCM_CopyInBSSList
08/01/05   jas     Fixing WEP key sizes
07/20/05   AT      Added new API to set the subscription_status as available
                   not available for all technologies in one CM commands.
07/15/05   AT      Added API to Reset Origination Throttling.
07/11/05   aps     Added PI field to AEECMCallInfo structure.
06/27/05   sun     Added new call state
06/21/05   jas     Adding TLS Certificate structure for WLAN
06/07/05   sun     Added support for CCBS
06/02/05   jas     Adding more WLAN fields to AEECMPhInfo and AEECMSSInfo
05/13/05   jas     Updating 8021X id/pw structures to match CM
04/20/05   jas     Tech value in BSS list should be tech type, not tech pref
04/19/05   jas     Band value in BSS list should be band class, not band pref
04/13/05   SUN     Added call_mode_info to CallInfo structure
04/06/05   RI      Support for TTY.
03/29/05   jas     Updated the WLAN tech values to match CM
03/24/05   jas     Added "no change" value for scan preference
03/18/05   jas     Adding support for WLAN stats
03/18/05   cvs     Add interface to SINR values
03/15/05   ak      Change in Signature since an arg is output
03/10/05   ak      Added CPHS EONS Support
03/07/05   jas     Making AEECMBandPref a uint64 to match CM
03/02/05   JAS     Added WLAN value to AEECMSysMode
02/18/05   sun     Added Handler for CM_CALL_EVENT_LINE_CTRL
01/26/05   SUN     Fixed ALS Enums
12/03/04   AT      Fill in WLAN preferences
11/23/04   ak      Added a mode correponding to HDR+CDMA only
09/27/04   SUN     Removed the Extra PH Event
09/03/04   SUN     Added Support for ALS
06/03/04   PA      Added error for invalid SIM state.
05/13/04   SUN     Added new Band Pref Changes
04/21/04   PA      Added RFU TON and NPI
04/14/04   PA      Added support for data call statistics.
02/23/04   SUN     Added AEECM_MAX_DIAL_STRING
12/19/03   PA      Added Signalling enums
11/20/03   PA      Support for DDTM
11/18/03   PA      Added AEECM_SRV_STATUS_PWR_SAVE
11/13/03   PA      Support for ICallOpts
11/03/03   PA      Removed support for ICM_SetUIMSlotPref().
11/03/03   PA      Band pref changes to reflect CM/SD changes
10/22/03   PA      Support for AEECallback, added size for all info.
10/16/03   PA      Combined EXT_DISPLAY with DISPLAY event, added sim_state,
                   Support for HDR, Changed call start/duration to uptime
10/08/03   PA      AOC and Dual Slot SIM functionality, added ICM_IsMyCall().
10/07/03   PA      Added GetSysMode(), missed call flag, removed RejectCall()
09/30/03   PA      Simplified ICM error values, changed call_type
                   and call state to bit mask. Added helper macros and
                   IsValidModeBandPref().
09/26/03   PA      Changed enum values to reflect CM changes
09/25/03   PA      Function name changes, 32-bit enums, Band Pref changes
                   Change in call related sups handling
09/21/03   tml     Added invalid User zone #define
09/19/03   PA      Fixed EndAllCalls typo in macro definition
09/08/03   PA      Changed Call Sups request enum types
09/04/03   SUN     Changed AeeSuppSys.h to AEESuppSvc.h
08/22/03   PA      Initial version.
=============================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "AEE.h"
#include "AEESuppSvc.h"
#include "AEEWebOpts.h"

/* Do NOT include CM.h.  This interface file needs to be independent of
 * the service used to implement it.  Unfortunately, that means that
 * consistency between ICM enumerated values and CM enumerated values
 * is a manual process.
 */

#include "CALLMANAGER.BID"   /* AEECLSID_CM */
#include "CM_NOTIFIER.BID"   /* AEECLSID_CM_NOTIFIER */
#include "CM_MODEL.BID"      /* AEECLSID_CM_MODEL */
#include "CALLOPTS.BID"      /* AEECLSID_CALLOPTS */

#define AEECLSID_CALLMANAGER AEECLSID_CM

/*===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/


/* Event Masks for Notification */
#define NMASK_CM_VOICE_CALL    0x0001   /* includes emergency calls */
#define NMASK_CM_DATA_CALL     0x0002
#define NMASK_CM_TEST_CALL     0x0004
#define NMASK_CM_OTHER_CALL    0x0008
#define NMASK_CM_PHONE         0x0010
#define NMASK_CM_SS            0x0020
#define NMASK_CM_INBAND        0x0040
#define NMASK_CM_MBMS          0x0080

/* Following Macros can be used to determine the type of event */
#define AEECM_EVENT_GROUP(x) ((uint16)(((x) & 0xFF00) >> 8))
#define AEECM_CALL_GROUP   0x01
#define AEECM_PH_GROUP     0x02
#define AEECM_SS_GROUP     0x03
#define AEECM_INBAND_GROUP 0x04
#define AEECM_MBMS_GROUP 0x07

#define AEECM_CALLID_NONE 0

/* Maximum number of forwarded info stored */
#define AEECM_MAX_NUM_FWDING 5

/* Maximum number of call IDs */
#define AEECM_MAX_NUM_CALLS  16

/*===========================================================================

                    ICallOpts Definitions

===========================================================================*/

/* IWebOpts ID space used by ICallOpts */
#define CALLOPT_OFFSET         0x1100
#define CALLOPT_SIZE           0x00FF

/* NULL Terminated AECHAR strings */
/* [0x00011100, 0x000111FF] */
#define CALLOPT_SZ             (WEBOPT_SZ + CALLOPT_OFFSET)
#define CALLOPT_SZ_LAST        (CALLOPT_SZ + CALLOPT_SIZE)

/* 32-bit quantities */
/* [0x00021100, 0x000211FF] */
#define CALLOPT_32BIT          (WEBOPT_32BIT + CALLOPT_OFFSET)
#define CALLOPT_32BIT_LAST     (CALLOPT_32BIT + CALLOPT_SIZE)

/* Interface pointers */
/* [0x00031100, 0x000311FF] */
#define CALLOPT_INTERFACE      (WEBOPT_INTERFACE + CALLOPT_OFFSET)
#define CALLOPT_INTERFACE_LAST (CALLOPT_INTERFACE + CALLOPT_SIZE)

/* Variable length buffer. First two bytes give the length of the following
   data. The length is little-endian, low byte first. */
/* [0x00041100, 0x000411FF] */
#define CALLOPT_VARBUFFER      (WEBOPT_VARBUFFER + CALLOPT_OFFSET)
#define CALLOPT_VARBUFFER_LAST (CALLOPT_VARBUFFER + CALLOPT_SIZE)

/* ASN.1 data */
/* [0x00051100, 0x000511FF] */
#define CALLOPT_ASN1           (WEBOPT_ASN1 + CALLOPT_OFFSET)
#define CALLOPT_ASN1_LAST      (CALLOPT_ASN1 + CALLOPT_SIZE)

/* low 12 bits of the id specify length of the option, in bytes */
/* [0x11100000, 0x111FF000] */
#define CALLOPT_VARIABLE       (WEBOPT_VARIABLE + (CALLOPT_OFFSET << 3))
#define CALLOPT_VARIABLE_LAST  (CALLOPT_VARIABLE + (CALLOPT_SIZE << 3))


/* Call options supported by ICM */

#define AEECM_CALLOPT_SERVICE_TYPE       (CALLOPT_32BIT)
  /* AEECMServiceType: automatic, amps etc. */

#define AEECM_CALLOPT_SRV_OPT            (CALLOPT_32BIT+1)
  /* AEECMServiceOption:  Specify service options */

#define AEECM_CALLOPT_OTASP_ACT_CODE     (CALLOPT_32BIT+2)
  /* AEECMActivateCode: OTASP activation code */

#define AEECM_CALLOPT_CLIR_INFO          (CALLOPT_32BIT+3)
  /* AEECMCLIR: CLIR suppression / invocation etc. */

#define AEECM_CALLOPT_CALL_CTRL_COMPLETE (CALLOPT_32BIT+4)
  /* Boolean: Denotes whether calling task has already applied call control */

#define AEECM_CALLOPT_CALLED_SUBADDR     (CALLOPT_SZ)
  /* NULL terminated AECHAR string for Called party subaddress */

#define AEECM_CALLOPT_FWD_CUG_INFO       (CALLOPT_VARIABLE + \
                                         (sizeof(AEECMFwdCugInfo) & 0x00000FFF))
  /* AEECMFwdCugInfo: Closed user Group information */

/*===========================================================================

                             HELPER MACROS

===========================================================================*/

/* Helper Macros to search calls */
#define AEECM_IS_VOICECALL_PRESENT(po) (ICM_GetActiveCallIDs((po),            \
                                (AEECMCallType)(AEECM_CALL_TYPE_VOICE |       \
                                AEECM_CALL_TYPE_EMERGENCY |                   \
                                AEECM_CALL_TYPE_STD_OTASP |                   \
                                AEECM_CALL_TYPE_NON_STD_OTASP),               \
                                AEECM_CALL_STATE_NONE,                        \
                                NULL, 0) > 0 ? TRUE: FALSE)

#define AEECM_IS_ANYCALL_PRESENT(po) (ICM_GetActiveCallIDs((po),              \
                                AEECM_CALL_TYPE_NONE,                         \
                                AEECM_CALL_STATE_NONE,                        \
                                NULL, 0) > 0 ? TRUE: FALSE)

#define AEECM_IS_XCALL_PRESENT(po, call_type)  (ICM_GetActiveCallIDs((po),    \
                                (call_type) ,                                 \
                                AEECM_CALL_STATE_NONE,                        \
                                NULL, 0) > 0 ? TRUE: FALSE)

#define AEECM_IS_VOICECALL_CONNECTED(po)  (ICM_GetActiveCallIDs((po),         \
                                (AEECMCallType)(AEECM_CALL_TYPE_VOICE |       \
                                AEECM_CALL_TYPE_EMERGENCY |                   \
                                AEECM_CALL_TYPE_STD_OTASP |                   \
                                AEECM_CALL_TYPE_NON_STD_OTASP),               \
                                (AEECMCallState)(AEECM_CALL_STATE_CONV |      \
                                AEECM_CALL_STATE_ONHOLD |                     \
                                AEECM_CALL_STATE_DORMANT),                    \
                                NULL, 0) > 0 ? TRUE: FALSE)

#define AEECM_IS_ANYCALL_CONNECTED(po)  (ICM_GetActiveCallIDs((po),           \
                                AEECM_CALL_TYPE_NONE,                         \
                                (AEECMCallState)(AEECM_CALL_STATE_CONV |      \
                                AEECM_CALL_STATE_ONHOLD |                     \
                                AEECM_CALL_STATE_DORMANT),                    \
                                NULL, 0) > 0 ? TRUE: FALSE)

#define AEECM_IS_XCALL_CONNECTED(po, call_type)  (ICM_GetActiveCallIDs((po),  \
                                (call_type),                                  \
                                (AEECMCallState)(AEECM_CALL_STATE_CONV |      \
                                AEECM_CALL_STATE_ONHOLD |                     \
                                AEECM_CALL_STATE_DORMANT),                    \
                                NULL, 0) > 0 ? TRUE: FALSE)

#define AEECM_IS_CALLSTATE_CONNECTED(call_state)                              \
                           ( ((call_state) == AEECM_CALL_STATE_CONV   ||      \
                             (call_state) == AEECM_CALL_STATE_ONHOLD  ||      \
                             (call_state) == AEECM_CALL_STATE_DORMANT)        \
                             ? TRUE : FALSE )

#define AEECM_IS_CALLTYPE_VOICE(call_type) \
                           ( ((call_type) == AEECM_CALL_TYPE_VOICE ||         \
                              (call_type) == AEECM_CALL_TYPE_EMERGENCY ||     \
                              (call_type) == AEECM_CALL_TYPE_STD_OTASP ||     \
                              (call_type) == AEECM_CALL_TYPE_NON_STD_OTASP)   \
                              ? TRUE : FALSE )

#define AEECM_IS_MODE_GW(mode) \
                           ( ((mode) == AEECM_SYS_MODE_GSM ||                 \
                              (mode) == AEECM_SYS_MODE_WCDMA)                 \
                              ? TRUE : FALSE )

#define AEECM_IS_CALL_MODE_GW(mode) \
                           ( ((mode) == AEECM_CALL_MODE_INFO_GW_CS ||                 \
                              (mode) == AEECM_CALL_MODE_INFO_GW_PS ||                 \
                              (mode) == AEECM_CALL_MODE_INFO_IP)                      \
                              ? TRUE : FALSE )

#define AEECM_IS_CALL_MODE_CDMA(mode) \
                           ( ((mode) == AEECM_CALL_MODE_INFO_CDMA)                 \
                              ? TRUE : FALSE )

#define AEECM_IS_MODE_CDMA(mode) \
                           ( ((mode) == AEECM_SYS_MODE_CDMA ||                 \
                              (mode) == AEECM_SYS_MODE_HDR)                 \
                              ? TRUE : FALSE )

#define AEECM_IS_MODE_PREF_WLAN(mode_pref)                                        \
                           ( ((mode_pref) == AEECM_MODE_PREF_AUTOMATIC ||         \
                              (mode_pref) == AEECM_MODE_PREF_WLAN_ONLY ||         \
                              (mode_pref) == AEECM_MODE_PREF_CDMA_WLAN ||         \
                              (mode_pref) == AEECM_MODE_PREF_HDR_WLAN  ||         \
                              (mode_pref) == AEECM_MODE_PREF_CDMA_HDR_WLAN ||     \
                              (mode_pref) == AEECM_MODE_PREF_GSM_WLAN ||          \
                              (mode_pref) == AEECM_MODE_PREF_WCDMA_WLAN ||        \
                              (mode_pref) == AEECM_MODE_PREF_GW_WLAN ||           \
                              (mode_pref) == AEECM_MODE_PREF_CURRENT_PLUS_WLAN || \
                              (mode_pref) == AEECM_MODE_PREF_CDMA_AMPS_HDR_WLAN_ONLY || \
                              (mode_pref) == AEECM_MODE_PREF_CDMA_AMPS_WLAN_ONLY) \
                              ? TRUE : FALSE )

/*===========================================================================

                    TYPE DECLARATIONS

===========================================================================*/

/*=============================================================================

                COMMON DATA TYPES WITH cm.h/sys.h

==============================================================================*/

/* Maximum length of DTMF digits */
#define AEECM_INBAND_MAX_DIGITS_LENGTH  257

/* Maximum length of alphanumeric tag */
#define AEECM_MAX_ALPHA_TAG_LENGTH  65

/* Maximum length of subaddress digits */
#define AEECM_MAX_SUBADDRESS_DIGITS_LENGTH   22

//AEECM_MAX_DIAL_STRING corresponds to CM_MAX_NUMBER_CHARS defined in cm.h
//Includes the NULL terminator, actual max dial string length is 81
#define AEECM_MAX_DIAL_STRING 82
#define AEECM_MAX_DIGITS_LENGTH AEECM_MAX_DIAL_STRING

#define AEECM_INVALID_UZID         -1
#define AEECM_MAX_CCBS_REQ         5

/*
** Constant for the maximum number of PLMNS in the PLMN list.  The maximum
** length for the sys_plmn_list_s_type is twice the sum of the maximum lengths
** for the user and operator preferred PLMN lists since sys_plmn_list_s_type
** must contain a separate entry for each radio access technology.
*/
#define AEECM_USER_PPLMN_LIST_MAX_LENGTH      10
  /* Maximum number of user preferred/controlled PLMNs */

#define AEECM_OPERATOR_PPLMN_LIST_MAX_LENGTH  10
  /* Maximum number of operator preferred/controlled PLMNs */

#define AEECM_PLMN_LIST_MAX_LENGTH  ( 2 * ( AEECM_USER_PPLMN_LIST_MAX_LENGTH + \
                                          AEECM_OPERATOR_PPLMN_LIST_MAX_LENGTH ) )

#define AEECM_MAX_BSS_LIST                 10
/*
** Define constants used by for mobility management information.
** According to the standard, 3GPP TS 24.0008, the short name and
** full name can be from 3 to N characters in length.
*/
#define AEECM_MAX_NW_NAME_CHARS       255


/* Maximum user zone name length.
*/
#define  AEECM_PUZL_MAX_UZ_NAME_LEN          32

/* Return value for ICM_CallOpsStringCheck() */
typedef enum
{
  AEECM_NOT_CALLOPS,
    /* Dialed string not a call operation */

  AEECM_IS_CALLOPS,
    /* Dialed string is a call operations */

  AEECM_IS_CALLOPS_NEED_CALLID,
    /* Dialed string is a call operation and ICM_CallOpsRequest()
     * require the call id associated with call number returned */

  AEECM_IS_CALLOPS_BIG = 0x10000000
   /* Forcing all enums to be 32 bit */

} AEECMIsCallOps;

/*
** Enumeration that defines the type of Apps and SIM Slot.
*/
typedef enum
{
  AEECM_UIM_APP_NONE = -1,

  AEECM_UIM_APP_CDMA,
   /* Application is CDMA */

  AEECM_UIM_APP_GSM,
   /* Application is GSM */

  AEECM_UIM_APP_BIG = 0x10000000
   /* Forcing all enums to be 32 bit */

} AEECMUIMApp;

/* Enumeration of service types.
**
** To specify a service type while making a call ICallOpts should
** be used. Default service type is AEECM_SRV_TYPE_AUTOMATIC.
**
*/
/* Note: Different from DMSS, require translation */
typedef enum
{

  AEECM_SRV_TYPE_NONE = 0,

  AEECM_SRV_TYPE_AMPS=1,
    /* Call is to be connected over analog service.
    **
    ** Note that the client does NOT need to specify service option when
    ** originating with this call type.
    **
    ** NOTE! When originating with this service type, the mode preference
    ** might be forced to ANALOG ONLY before origination and returned to
    ** its original setting at the end of the call */

  AEECM_SRV_TYPE_HDR=2,
    /* Call is to be connected over HDR service.
    ** Valid for FEATURE_HDR */

  AEECM_SRV_TYPE_CDMA_SPECIFIC=3,
    /* Call is to be connected over CDMA service with specific client
    ** designated CDMA service option.
    **
    ** Note that the client need to specify service option when
    ** originating with this call type.
    **
    ** NOTE! When originating with this service type, the mode preference
    ** might be forced to DIGITAL ONLY before origination and returned to
    ** its original setting at the end of the call */

  AEECM_SRV_TYPE_CDMA_AUTOMATIC=4,
    /* Call is to be connected over CDMA service where CM automatically
    ** originate with the best available service option for the specified
    ** call type.
    **
    ** Note that the client does NOT need to specify service option when
    ** originating with this call type */

  AEECM_SRV_TYPE_AUTOMATIC=5,
    /* Call is to be connected over available service - AMPS or CDMA,
    ** or GSM or WCDMA or HDR.
    **
    ** Note that the client does NOT need to specify service option when
    ** originating with this srv type except for the packet data call*/

  AEECM_SRV_TYPE_BIG = 0x10000000
    /* Forcing all enums to be 32 bit */

} AEECMServiceType;

/* Privacy mode during a call. CDMA only.
** See Also: ICM_ChangePrivacyPref().
*/
/* Note: Different from DMSS, require translation */
typedef enum
{

  AEECM_PRIVACY_PREF_NONE = 0,

  AEECM_PRIVACY_PREF_STANDARD,
    /* Standard privacy */

  AEECM_PRIVACY_PREF_ENHANCED,
    /* Enhanced privacy */

  AEECM_PRIVACY_PREF_BIG = 0x10000000
    /* Force 32-bit enums */

} AEECMPrivacyPref;

/* FEATURE_HDR - Enumeration of system selection hybrid preferences.
** See Also: ICM_SetSystemPreference().
*/
/* Note: Different from DMSS, require translation */
typedef enum
{

  AEECM_HYBR_PREF_NONE = 0,

  AEECM_HYBR_PREF_OFF,
    /* hybrid operation is not allowed - SD_SS_HYBR_PREF_NONE */

  AEECM_HYBR_PREF_ON,
    /* hybrid operation is allowed - SD_SS_HYBR_PREF_CDMA_HDR */

  AEECM_HYBR_PREF_NO_CHANGE,
    /* Do not change the hybr preference */

  AEECM_HYBR_PREF_PERSISTENT,
    /* Use the NV value */

  AEECM_HYBR_PREF_BIG = 0x10000000
    /* Force 32-bit enums */

} AEECMHybrPref;


/* Enumeration of HDR active protocol revision */
typedef enum {

  AEECM_SYS_ACTIVE_PROT_NONE,      /* The protocol revision is unknown */

  AEECM_SYS_ACTIVE_PROT_HDR_START,   /** When HDR sys ID is first acquired 
                                     ** and no protocol revision is 
                                     ** available at this point.
                                      */
  AEECM_SYS_ACTIVE_PROT_HDR_REL0,      /* HDR release 0 */
                                       
  AEECM_SYS_ACTIVE_PROT_HDR_RELA,      /* HDR release A */
  AEECM_SYS_ACTIVE_PROT_HDR_END,

          
  AEECM_SYS_ACTIVE_PROT_MAX        /* For internal use only */
                                      
}AEECMActiveProtType;

/* Enumeration of Dedicated Data Transmission
   Mode preferences.
*/
typedef enum
{
  AEECM_DDTM_PREF_NONE        = -1,

  AEECM_DDTM_PREF_OFF,
        /* disable data dedicated transmission mode */

  AEECM_DDTM_PREF_ON,
        /* enable data dedicated transmission mode */

  AEECM_DDTM_PREF_NO_CHANGE,
        /* Do not change the DDTM preference */

  AEECM_DDTM_PREF_BIG=0x10000000
      /* To force enum to 32 bit */

} AEECMDDTMPref;

/* Current RTRE control.
** Meaningful in CDMA only
*/
typedef enum
{

  AEECM_RTRE_CONTROL_NONE,

  AEECM_RTRE_CONTROL_RUIM,
    /* RTRE control: use R-UIM */

  AEECM_RTRE_CONTROL_NV,
    /* RTRE control: use NV */

  AEECM_RTRE_CONTROL_BIG = 0x10000000
    /* Force 32-bit enums */

} AEECMRTREControl;


/* Current RTRE confiuration
** Meaningful in CDMA only
*/
typedef enum
{

  AEECM_RTRE_CONFIG_NONE,

  AEECM_RTRE_CONFIG_RUIM_ONLY,
    /* RTRE configuration: use R-UIM only */

  AEECM_RTRE_CONFIG_NV_ONLY,
    /* RTRE configuration: use NV only    */

  AEECM_RTRE_CONFIG_RUIM_OR_DROP_BACK,
    /* RTRE use R-UIM if available or NV  */

  AEECM_RTRE_CONFIG_BIG = 0x10000000
    /* Force 32-bit enums */

} AEECMRTREConfig;

/* Enumeration of phone CDMA lock modes.
** Meaningful in CDMA only
*/
typedef enum
{

  AEECM_CDMA_LOCK_MODE_NONE=-1,

  AEECM_CDMA_LOCK_MODE_OFF,
    /* phone is NOT CDMA locked */

  AEECM_CDMA_LOCK_MODE_ON,
    /* phone is CDMA locked until power-cycle */

  AEECM_CDMA_LOCK_MODE_BIG = 0x10000000
    /* Force 32-bit enums */

} AEECMCDMALockMode;

/*
** Enumeration that defines the type of network list to be requested.  The
** requested list will either contain the preferred networks or all available
** networks.
** See Also: ICM_GetNetworks().
*/
typedef enum
{

  AEECM_NETWORK_LIST_NONE=-1,

  AEECM_NETWORK_LIST_AVAILABLE,
    /* The available network list. */

  AEECM_NETWORK_LIST_USER_PREFERRED,
    /* The preferred network list from SIM/USIM */

  AEECM_NETWORK_LIST_BSS,
    /* The list of available WiFi networks. */

  AEECM_NETWORK_LIST_BIG = 0x10000000
    /* Force 32-bit enums */

} AEECMNetworkType;

/* Enumeration of NAM selections
*/
typedef enum
{

  AEECM_NAM_NONE=-1,

  AEECM_NAM_1,
    /* NAM 1*/

  AEECM_NAM_2,
    /* NAM 2 - for targets with more than 1 NAM */

  AEECM_NAM_3,
    /* NAM 3 - for targets with more than 2 NAM's */

  AEECM_NAM_4,
    /* NAM 4 - for targets with more than 3 NAM's */

  AEECM_NAM_AUTO,
    /* Auto NAM - for targets with more than 1 NAM */

  AEECM_NAM_BIG = 0x10000000
    /* Force 32-bit enums */

} AEECMNAM;


/*
** Enumeration that defines whether the network should be selected
** automatically, manually or for limited service. GSM/WCDMA Only.
*/
typedef enum
{

  AEECM_NETWORK_SEL_MODE_PREF_NONE=-1,

  AEECM_NETWORK_SEL_MODE_PREF_AUTOMATIC=0,
    /* Automatic network selection */

  AEECM_NETWORK_SEL_MODE_PREF_MANUAL=1,
    /* Manual network selection */

  AEECM_NETWORK_SEL_MODE_PREF_LIMITED_SRV=2,
    /* Select a network for limited service */

  AEECM_NETWORK_SEL_MODE_PREF_NO_CHANGE=3,
  /* No change */

  
  AEECM_NETWORK_SEL_MODE_PREF_PERSISTENT=4,
   /* Return to persistent preference. */

  AEECM_NETWORK_SEL_MODE_PREF_HPLMN_SRCH=5,  
   /* Search and Camp on HPLMN */
  
  AEECM_NETWORK_SEL_MODE_PREF_BIG = 0x10000000
    /* Force 32-bit enums */

} AEECMNwSelModePref;

/*
** Enumeration that defines the service domain being requested by
** the client (the preferred service domain).
*/
typedef enum
{

  AEECM_SRV_DOMAIN_PREF_NONE=-1,

  AEECM_SRV_DOMAIN_PREF_CS_ONLY=0,
    /* Prefer Circuit Switched Only */

  AEECM_SRV_DOMAIN_PREF_PS_ONLY,
    /* Prefer Packet Switched Only */

  AEECM_SRV_DOMAIN_PREF_CS_PS,
    /* Prefer Circuit and Packet Switched */

  AEECM_SRV_DOMAIN_PREF_ANY,
    /* Any domain will do. No preference */

  AEECM_SRV_DOMAIN_PREF_NO_CHANGE,
    /* To be used by clients who do not want to change the service domain */

  AEECM_SRV_DOMAIN_PREF_PS_ATTACH,
    /*  PS attach on demand.
        Note: This value of service domain preference is not saved in NV  */

  AEECM_SRV_DOMAIN_PREF_PS_DETACH,
  /* PS detach on demand
       Note: This value of service domain preference is not saved in NV   */

  AEECM_SRV_DOMAIN_PREF_PERSISTENT,
  /* Revert to NV value */

  AEECM_SRV_DOMAIN_PREF_BIG = 0x10000000
    /* Force 32-bit enums */

} AEECMServDomPref;


/*
** Enumeration of roaming statuses.  The values for these enumerates
** were derived from the roaming display indicators described in
** 3GPP2 C.R1001-A.  Please note that the actual value of the enumerate
** is not consistent with the value listed in the standard (e.g. the
** value of AEECM_ROAM_STATUS_OFF and AEECM_ROAM_STATUS_ON are reversed from
** the standard.
*/
typedef enum
{

  AEECM_ROAM_STATUS_NONE = -1,

  AEECM_ROAM_STATUS_OFF,
    /* Roaming Indicator off                       */

  AEECM_ROAM_STATUS_ON,
    /* Roaming Indicator on                        */

  AEECM_ROAM_STATUS_BLINK,
    /* Roaming Indicator flashing                  */

  AEECM_ROAM_STATUS_OUT_OF_NEIGHBORHOOD,
    /* Out of neighborhood                         */

  AEECM_ROAM_STATUS_OUT_OF_BLDG,
    /* Out of building                             */

  AEECM_ROAM_STATUS_PREF_SYS,
    /* Roaming - preferred system                  */

  AEECM_ROAM_STATUS_AVAIL_SYS,
    /* Roaming - available system                  */

  AEECM_ROAM_STATUS_ALLIANCE_PARTNER,
    /* Roaming - alliance partner                  */

  AEECM_ROAM_STATUS_PREMIUM_PARTNER,
    /* Roaming - premium partner                   */

  AEECM_ROAM_STATUS_FULL_SVC,
    /* Roaming - full service functionality        */

  AEECM_ROAM_STATUS_PARTIAL_SVC,
    /* Roaming - partial service functionality     */

  AEECM_ROAM_STATUS_BANNER_ON,
    /* Roaming banner on                           */

  AEECM_ROAM_STATUS_BANNER_OFF,
    /* Roaming banner off                          */

  AEECM_ROAM_STATUS_BIG = 0x10000000
    /* Force 32-bit enums */

} AEECMRoamStatus;

#define AEECM_BM(val) (1 << (val))
#define AEECM_BM64(val) ((uint64)1 << (val))

/* Enumeration of system selection roam preferences.
*/
typedef enum
{

  AEECM_ROAM_PREF_NONE=0,

  AEECM_ROAM_PREF_HOME=AEECM_BM(AEECM_ROAM_STATUS_OFF),
    /* Acquire only systems for which the roaming indication is off
    ** SD_SS_ROAM_PREF_HOME */

  AEECM_ROAM_PREF_AFFIL=AEECM_BM(AEECM_ROAM_STATUS_OFF)|AEECM_BM(AEECM_ROAM_STATUS_ON),
    /* Acquire only systems for which the roaming indication is off or
    ** solid on - i.e. not flashing. CDMA Only
    ** SD_SS_ROAM_PREF_AFFIL */

  AEECM_ROAM_PREF_ANY=0xFF,
    /* Acquire systems regardless of their roaming indication
    ** SD_SS_ROAM_PREF_ANY */

  AEECM_ROAM_PREF_NO_CHANGE,
    /* Do not change the roam preference */

  AEECM_ROAM_PREF_PERSISTENT,
    /* Revert to NV value */

  AEECM_ROAM_PREF_BIG = 0x10000000
    /* Force 32-bit enums */

} AEECMRoamPref;

/*
** Enumeration of system band classes.
*/
typedef enum
{
  AEECM_SYS_BAND_CLASS_NONE        = -1,

  AEECM_SYS_BAND_BC0               = 0,
    /* Band Class 0 - US Cellular band (800 MHz)       */

  AEECM_SYS_BAND_BC1               = 1,
    /* Band Class 1 - US */

  AEECM_SYS_BAND_BC3               = 3,
    /* Band Class 3 - Japanese Cellular band (800 MHz) */

  AEECM_SYS_BAND_BC4               = 4,
    /* Band Class 4 - Korean PCS band (1800 MHz)       */

  AEECM_SYS_BAND_BC5               = 5,
    /* Band Class 5 (450 MHz)                          */

  AEECM_SYS_BAND_BC6               = 6,
    /* Band Class 6 (2 GHz)                            */

  AEECM_SYS_BAND_BC7               = 7,
    /* Band Class 7 (700 MHz)                          */

  AEECM_SYS_BAND_BC8               = 8,
    /* Band Class 8 (1800 MHz)                         */

  AEECM_SYS_BAND_BC9               = 9,
    /* Band Class 9 (900 MHz)                          */

  AEECM_SYS_BAND_BC10              = 10,
    /* Band Class 10 (Second 800 MHz)                  */

  AEECM_SYS_BAND_BC11              = 11,
    /* Band Class 11 - European PAMR Band (400 MHz)    */

    /* Reserved 12-39 for CDMA band classes. */
  AEECM_SYS_BAND_BC12              = 12,
    /* Band Class 12 - PAMR Band (800 MHz)             */

  AEECM_SYS_BAND_BC15              = 15,
    /* Band Class 15 (1700-2100 MHz -AWS)              */

  AEECM_SYS_BAND_BC16              = 16,
    /* Band Class 16 - Band Class 16 (US 2.5GHz)       */

  AEECM_SYS_BAND_GSM_450           = 40,
    /* GSM 450 band (450 MHz)                          */

  AEECM_SYS_BAND_GSM_480           = 41,
    /* GSM 480 band (480 MHz)                          */

  AEECM_SYS_BAND_GSM_750           = 42,
    /* GSM 750 band (750 MHz)                          */

  AEECM_SYS_BAND_GSM_850           = 43,
    /* GSM 850 band (850 MHz)                          */

  AEECM_SYS_BAND_GSM_EGSM_900      = 44,
    /* GSM Extended GSM (E-GSM) 900 band (900 MHz)         */

  AEECM_SYS_BAND_GSM_PGSM_900      = 45,
    /* GSM Primary GSM ( P-GSM) 900 band (900 MHz)         */

  AEECM_SYS_BAND_GSM_RGSM_900      = 46,
    /* GSM Railways GSM ( R-GSM) 900 band (900 MHz)        */

  AEECM_SYS_BAND_GSM_DCS_1800      = 47,
    /* GSM DCS band (1800 MHz)                         */

  AEECM_SYS_BAND_GSM_PCS_1900      = 48,
    /* GSM PCS band (1900 Mhz)                         */

    /* Reserved 49-79 for GSM band classes. */

  AEECM_SYS_BAND_WCDMA_I_IMT_2000  = 80,
    /* WCDMA IMT 2000 (2000 MHz)                       */

  AEECM_SYS_BAND_WCDMA_II_PCS_1900 = 81,
    /* WCDMA PCS band (1900 MHz)                       */

  AEECM_SYS_BAND_WCDMA_III_1700    = 82,
    /* WCDMA 1700 (1700 MHz)                           */

  AEECM_SYS_BAND_WCDMA_IV_1700     = 83,
    /* WCDMA 1700 (1700 MHz)                           */

  AEECM_SYS_BAND_WCDMA_V_850       = 84,
    /* WCDMA 850 (850 MHz)                             */

  AEECM_SYS_BAND_WCDMA_VI_800      = 85,
    /* WCDMA 800 (800 MHz)                             */

  AEECM_SYS_BAND_WCDMA_VII_2600    = 86,
    /* WCDMA 2600 (2600 MHz)                           */

  AEECM_SYS_BAND_WCDMA_VIII_900    = 87,
    /* WCDMA 900 (900 MHz)                             */

  AEECM_SYS_BAND_WCDMA_IX_1700     = 88,
    /* WCDMA 1700 (1700 MHz)                           */

  AEECM_SYS_BAND_WLAN_US_2400      = 110,
    /* WLAN US 2400 Mhz band                           */

  AEECM_SYS_BAND_WLAN_JAPAN_2400 = 111,
    /* WLAN JAPAN 2400 Mhz band                        */

  AEECM_SYS_BAND_WLAN_EUROPE_2400 = 112,
    /* WLAN EUROPEAN 2400 Mhz band                     */

  AEECM_SYS_BAND_WLAN_FRANCE_2400 = 113,
    /* WLAN FRANCE 2400 Mhz band                       */

  AEECM_SYS_BAND_WLAN_SPAIN_2400 = 114,
    /* WLAN SPAIN 2400 Mhz band                        */

  AEECM_SYS_BAND_WLAN_US_5000    = 115,
    /* WLAN US 5000 Mhz band                           */

  AEECM_SYS_BAND_WLAN_JAPAN_5000 = 116,
    /* WLAN JAPAN 5000 Mhz band                        */

  AEECM_SYS_BAND_WLAN_EUROPE_5000 = 117,
    /* WLAN EUROPEAN 5000 Mhz band                     */

  AEECM_SYS_BAND_WLAN_FRANCE_5000 = 118,
    /* WLAN FRANCE 5000 Mhz band                       */

  AEECM_SYS_BAND_WLAN_SPAIN_5000 = 119,
    /* WLAN SPAIN 5000 Mhz band                        */

  AEECM_SYS_BAND_BIG = 0x10000000
    /* Force 32-bit enums */

} AEECMSysBandClass;

/*
** Enumeration of system band classes and band classes' sub-bands.
** If there is a new band class, append to the end of list.
*/
typedef enum {
  AEECM_SBAND_BC0_A = 0,
    /* Band Class 0, A-System */

  AEECM_SBAND_BC0_B = 1,
    /* Band Class 0, B-System */

    /* Band Class 0 AB , GSM 850 Band*/

  AEECM_SBAND_BC1 = 2,
    /* Band Class 1, all blocks */

  AEECM_SBAND_BC2 = 3,
    /* Band Class 2 place holder */

  AEECM_SBAND_BC3 = 4,
    /* Band Class 3, A-System */

  AEECM_SBAND_BC4 = 5,
    /* Band Class 4, all blocks */

  AEECM_SBAND_BC5 = 6,
    /* Band Class 5, all blocks */

  AEECM_SBAND_GSM_DCS_1800 = 7,
    /* GSM DCS band */

  AEECM_SBAND_GSM_EGSM_900 = 8,
    /* GSM Extended GSM (E-GSM) band */

  AEECM_SBAND_GSM_PGSM_900 = 9,
    /* GSM Primary GSM (P-GSM) band */

  AEECM_SBAND_BC6 = 10,
    /* Band Class 6 */

  AEECM_SBAND_BC7 = 11,
    /* Band Class 7 */

  AEECM_SBAND_BC8 = 12,
    /* Band Class 8 */

  AEECM_SBAND_BC9 = 13,
    /* Band Class 9*/

  AEECM_SBAND_BC10 = 14,
    /* Band Class 10 */

  AEECM_SBAND_BC11 = 15,
    /* Band Class 11 */

  AEECM_SBAND_GSM_450 = 16,
    /* GSM 450 band */

  AEECM_SBAND_GSM_480 = 17,
    /* GSM 480 band */

  AEECM_SBAND_GSM_750 = 18,
    /* GSM 750 band */

  AEECM_SBAND_GSM_850 = 19,
    /* GSM 850 band */

  AEECM_SBAND_GSM_RGSM_900 = 20,
    /* GSM Railways GSM Band */

  AEECM_SBAND_GSM_PCS_1900 = 21,
    /* GSM PCS band */

  AEECM_SBAND_WCDMA_I_IMT_2000 = 22,
    /* WCDMA IMT 2000 band */

  AEECM_SBAND_WCDMA_II_PCS_1900 = 23,
    /* WCDMA PCS band */

  AEECM_SBAND_WCDMA_III_1700 = 24,
    /* WCDMA 1700 band */

  AEECM_SBAND_WCDMA_IV_1700 = 25,
    /* WCDMA 1700 band */

  AEECM_SBAND_WCDMA_V_850 = 26,
    /* WCDMA 850 band */

  AEECM_SBAND_WCDMA_VI_800 = 27,
    /* WCDMA 800 band */

  /* WCDMA band classes continue below */

  AEECM_SBAND_BC12 = 28,
    /* Band class 12 */

  AEECM_SBAND_RESERVED_1 = 29,
    /* For compatibility with SYS_SBAND_ enum values */

  AEECM_SBAND_RESERVED_2 = 30,
    /* For compatibility with SYS_SBAND_ enum values */

  AEECM_SBAND_BC15 = 31,
    /* Band Class 15 */

  AEECM_SBAND_WLAN_US_2400 = 32,
    /* WLAN US 2.4Mhz band */

  AEECM_SBAND_WLAN_EUROPE_2400 = 33,
    /* WLAN Europe 2.4Mhz band */

  AEECM_SBAND_WLAN_FRANCE_2400 = 34,
    /* Acquire WLAN in France 2.4 GHz */

  AEECM_SBAND_WLAN_SPAIN_2400 = 35,
    /* Acquire WLAN in Spain 2.4 GHz */

  AEECM_SBAND_WLAN_JAPAN_2400 = 36,
    /* WLAN Japan 2.4Mhz band */

  AEECM_SBAND_WLAN_US_5000 = 37,
    /* Acquire WLAN in US 5 GHz */

  AEECM_SBAND_WLAN_EUROPE_5000 = 38,
    /* Acquire WLAN in Europe 5 GHz */

  AEECM_SBAND_WLAN_FRANCE_5000 = 39,
    /* Acquire WLAN in France 5 GHz */

  AEECM_SBAND_WLAN_SPAIN_5000 = 40,
    /* Acquire WLAN in SPAIN 5 GHz */

  AEECM_SBAND_WLAN_JAPAN_5000 = 41,
    /* Acquire WLAN in Japan 5 GHz */

  AEECM_SBAND_WCDMA_VII_2600  = 48,
    /* WCDMA 2600 band             */

  AEECM_SBAND_WCDMA_VIII_900  = 49,
    /* WCDMA 900 band              */

  AEECM_SBAND_WCDMA_IX_1700   = 50,
    /* WCDMA 1700 band             */

    /* 51-55 reserved for WLAN */  
  AEECM_SBAND_BC16 = 56,
    /* Band Class 16 */

  AEECM_SBAND_PERSISTENT      = 62,
    /* Revert to NV value          */

  AEECM_SBAND_BIG = 0x10000000
    /* Force 32-bit enums */

} AEECMSysSubBandClass;

/* Type definition for band preferences.  Use the #defines below */
typedef uint64 AEECMBandPref;

#define AEECM_BAND_PREF_NONE 0

#define AEECM_BAND_PREF_BC0_A AEECM_BM64(AEECM_SBAND_BC0_A)
    /* Acquire band class 0, A-Side systems only */

#define AEECM_BAND_PREF_BC0_B AEECM_BM64(AEECM_SBAND_BC0_B)
    /* Acquire band class 0, B-Side systems only */

#define AEECM_BAND_PREF_BC0 AEECM_BM64(AEECM_SBAND_BC0_A)| AEECM_BM64(AEECM_SBAND_BC0_B)
    /* Acquire band class 0 systems only */

#define AEECM_BAND_PREF_BC1 AEECM_BM64(AEECM_SBAND_BC1)
    /* Acquire band class 1 systems only */

#define AEECM_BAND_PREF_BC3 AEECM_BM64(AEECM_SBAND_BC3)
    /* Acquire band class 3 systems only */

#define AEECM_BAND_PREF_BC4 AEECM_BM64(AEECM_SBAND_BC4)
    /* Acquire band class 4 systems only */

#define AEECM_BAND_PREF_BC5 AEECM_BM64(AEECM_SBAND_BC5)
    /* Acquire band class 5 systems only */

#define AEECM_BAND_PREF_GSM_DCS_1800 AEECM_BM64(AEECM_SBAND_GSM_DCS_1800)
    /* Acquire GSM DCS systems only */

#define AEECM_BAND_PREF_GSM_EGSM_900 AEECM_BM64(AEECM_SBAND_GSM_EGSM_900)
    /* Acquire GSM Extended GSM 900 systems only */

#define AEECM_BAND_PREF_GSM_PGSM_900 AEECM_BM64(AEECM_SBAND_GSM_PGSM_900)
    /* Acquire GSM Primary GSM 900 systems only */

#define AEECM_BAND_PREF_BC6 AEECM_BM64(AEECM_SBAND_BC6)
    /* Acquire band class 6 systems only */

#define AEECM_BAND_PREF_BC7 AEECM_BM64(AEECM_SBAND_BC7)
    /* Acquire band class 7 systems only */

#define AEECM_BAND_PREF_BC8 AEECM_BM64(AEECM_SBAND_BC8)
    /* Acquire band class 8 systems only */

#define AEECM_BAND_PREF_BC9 AEECM_BM64(AEECM_SBAND_BC9)
    /* Acquire band class 9 systems only */

#define AEECM_BAND_PREF_BC10 AEECM_BM64(AEECM_SBAND_BC10)
    /* Acquire band class 10 systems only */

#define AEECM_BAND_PREF_BC11 AEECM_BM64(AEECM_SBAND_BC11)
    /* Acquire band class 11 systems only */

#define AEECM_BAND_PREF_BC15 AEECM_BM64(AEECM_SBAND_BC15)
    /* Acquire band class 15 systems only */

#define AEECM_BAND_PREF_BC16 AEECM_BM64(AEECM_SBAND_BC16)
    /* Acquire band class 16 systems only */

#define AEECM_BAND_PREF_GSM_450 AEECM_BM64( AEECM_SBAND_GSM_450 )
    /* Acquire GSM 450 systems only */

#define AEECM_BAND_PREF_GSM_480 AEECM_BM64( AEECM_SBAND_GSM_480 )
    /* Acquire GSM 480 systems only */

#define AEECM_BAND_PREF_GSM_750 AEECM_BM64( AEECM_SBAND_GSM_750 )
    /* Acquire GSM 750 systems only */

#define AEECM_BAND_PREF_GSM_850 AEECM_BM64( AEECM_SBAND_GSM_850 )
    /* Acquire GSM 850 systems only */

#define AEECM_BAND_PREF_GSM_RGSM_900 AEECM_BM64(AEECM_SBAND_GSM_RGSM_900)
    /* Acquire GSM Railway GSM 900 systems only */

#define AEECM_BAND_PREF_GSM_PCS_1900 AEECM_BM64( AEECM_SBAND_GSM_PCS_1900 )
    /* Acquire GSM PCS systems only */

#define AEECM_BAND_PREF_WCDMA_I_IMT_2000 AEECM_BM64( AEECM_SBAND_WCDMA_I_IMT_2000 )
    /* Acquire WCDMA IMT 2000 systems only */

#define AEECM_BAND_PREF_WCDMA_II_PCS_1900 AEECM_BM64( AEECM_SBAND_WCDMA_II_PCS_1900 )
    /* Acquire WCDMA PCS systems only */

#define AEECM_BAND_PREF_WCDMA_III_1700 AEECM_BM64( AEECM_SBAND_WCDMA_III_1700 )
    /* Acquire WCDMA 1700 systems only */

#define AEECM_BAND_PREF_WCDMA_IV_1700 AEECM_BM64( AEECM_SBAND_WCDMA_IV_1700 )
    /* Acquire WCDMA 1700 systems only */

#define AEECM_BAND_PREF_WCDMA_V_850 AEECM_BM64( AEECM_SBAND_WCDMA_V_850 )
    /* Acquire WCDMA 850 systems only */

#define AEECM_BAND_PREF_WCDMA_VI_800 AEECM_BM64( AEECM_SBAND_WCDMA_VI_800 )
   /* Acquire WCDMA 800 systems only */

#define AEECM_BAND_PREF_WCDMA_VII_2600 AEECM_BM64( AEECM_SBAND_WCDMA_VII_2600 )
   /* Acquire WCDMA 2600 systems only */

#define AEECM_BAND_PREF_WCDMA_VIII_900 AEECM_BM64( AEECM_SBAND_WCDMA_VIII_900 )
   /* Acquire WCDMA 900 systems only */

#define AEECM_BAND_PREF_WCDMA_IX_1700 AEECM_BM64( AEECM_SBAND_WCDMA_IX_1700 )
   /* Acquire WCDMA 1700 systems only */

#define AEECM_BAND_PREF_WLAN_2400_US AEECM_BM64( AEECM_SBAND_WLAN_US_2400)
    /* Acquire WLAN in US 2.4 GHz */

#define AEECM_BAND_PREF_WLAN_2400_JP AEECM_BM64( AEECM_SBAND_WLAN_JAPAN_2400)
    /* Acquire WLAN in JP 2.4 GHz */

#define AEECM_BAND_PREF_WLAN_2400_ETSI AEECM_BM64( AEECM_SBAND_WLAN_EUROPE_2400)
    /* Acquire WLAN in Europe 2.4 GHz */

#define AEECM_BAND_PREF_WLAN_2400_SP AEECM_BM64( AEECM_SBAND_WLAN_SPAIN_2400)
    /* Acquire WLAN in SPAIN 2.4 GHz */

#define AEECM_BAND_PREF_WLAN_2400_FR AEECM_BM64( AEECM_SBAND_WLAN_FRANCE_2400)
    /* Acquire WLAN in France 2.4 GHz */

#define AEECM_BAND_PREF_WLAN_5000_US  AEECM_BM64( AEECM_SBAND_WLAN_US_5000)
    /* Acquire WLAN in US 5 GHz */

#define AEECM_BAND_PREF_WLAN_5000_ETSI AEECM_BM64( AEECM_SBAND_WLAN_EUROPE_5000 )
    /* Acquire WLAN in Europe 5 GHz */

#define AEECM_BAND_PREF_WLAN_5000_FR   AEECM_BM64( AEECM_SBAND_WLAN_FRANCE_5000 )
    /* Acquire WLAN in France 5 GHz */

#define AEECM_BAND_PREF_WLAN_5000_SP   AEECM_BM64( AEECM_SBAND_WLAN_SPAIN_5000 )
    /* Acquire WLAN in SPAIN 5 GHz */

#define AEECM_BAND_PREF_WLAN_5000_JP  AEECM_BM64( AEECM_SBAND_WLAN_JAPAN_5000)
    /* Acquire WLAN in JP 5 GHz */

#define AEECM_BAND_PREF_PERSISTENT     AEECM_BM64( AEECM_SBAND_PERSISTENT )
    /* Restore the NV value */

/* "ULL" designates "unsigned long long" */
#define AEECM_BAND_PREF_WLAN_ANY (AEECMBandPref)0x000003FF00000000ULL
    /* Acquire any WLAN band */

/* No, this doesn't say "FULL". "ULL" designates "unsigned long long" */
#define AEECM_BAND_PREF_ANY            (AEECMBandPref)0xFFFFFFFFBFFFFFFFULL
    /* Acquire any band class systems */

#define AEECM_BAND_PREF_NO_CHANGE (0x40000000)
    /* Do not change the band preference */

typedef enum {

  /* FOR INTERNAL USE ONLY!.*/
  AEECM_WLAN_BSS_TYPE_PREF_NONE = -1,

  /* Ad-Hoc Type pref BSS.*/
  AEECM_WLAN_BSS_TYPE_PREF_ADHOC,

  /* Infrastructure Mode pref BSS.*/
  AEECM_WLAN_BSS_TYPE_PREF_INFRA,

  /* Any BSS Mode Type.*/
  AEECM_WLAN_BSS_TYPE_PREF_ANY,

  /* Do not change the WLAN BSS type preference */
  AEECM_WLAN_BSS_TYPE_PREF_NO_CHANGE

} AEECMWLAN_BSS_TYPE_PREF; 

typedef enum {

  /* FOR INTERNAL USE ONLY!.*/
  AEECM_WLAN_BSS_TYPE_NONE = -1,

  /* Ad-Hoc Type BSS.*/
  AEECM_WLAN_BSS_TYPE_ADHOC,

  /* Infrastructure Mode BSS.*/
  AEECM_WLAN_BSS_TYPE_INFRA,

  /* Any BSS Mode Type.*/
  AEECM_WLAN_BSS_TYPE_ANY,

  /* Create an adhoc network */
  AEECM_WLAN_BSS_TYPE_ADHOC_START

} AEECMWLAN_BSS_TYPE; 

typedef uint64 AEECM_WLAN_BSSID_TYPE;

/* Cellular and PCS band preference.
**
** Note that IS-683-A PRL only provide support for one cellular band and
** one PCS band. Therefore we define a Cellular and PCS band preferences
** that are mapped into different band-classes preferences depending on
** the market, as follows:
**
** Cellular band preference is mapped to band-class 3 preference for the
** Japanese market or band-class 0 preference for any other market.
**
** PCS band preference is mapped to band-class 4 preference for the
** Korean market or band-class 1 preference for any other market.
*/

#define AEECM_BAND_PREF_CELL_A   AEECM_BAND_PREF_BC0_A
    /* Acquire cellular A-Side systems only - SD_SS_BAND_PREF_CELL_A */

#define AEECM_BAND_PREF_CELL_B   AEECM_BAND_PREF_BC0_B
    /* Acquire cellular B-Side systems only - SD_SS_BAND_PREF_CELL_B */

#define AEECM_BAND_PREF_CELL     AEECM_BAND_PREF_BC0
    /* Acquire cellular (A or B) systems only - SD_SS_BAND_PREF_CELL */

#define AEECM_BAND_PREF_PCS      AEECM_BAND_PREF_BC1
    /* Acquire PCS systems only - SD_SS_BAND_PREF_PCS */


/* Enumeration of GSM/WCDMA acquisition order
*/
typedef enum
{

  AEECM_GW_ACQ_ORDER_PREF_NONE = -1,

  AEECM_GW_ACQ_ORDER_PREF_AUTOMATIC=0,
    /* Determine mode automatically from the PRL order */

  AEECM_GW_ACQ_ORDER_PREF_GSM_WCDMA=1,
    /* Acqisition order is first GSM followed by WCDMA */

  AEECM_GW_ACQ_ORDER_PREF_WCDMA_GSM=2,
    /* Acqisition order is first WCDMA followed by GSM */

  AEECM_GW_ACQ_ORDER_PREF_NO_CHANGE=3,
    /* Do not change GSM/WCDMA acquisition order */

  AEECM_GW_ACQ_ORDER_PREF_PERSISTENT=4,
    /* Revert to NV value                        */

  AEECM_GW_ACQ_ORDER_PREF_BIG = 0x10000000
    /* Force 32-bit enums */

} AEECMAcqOrderPref;


/* Enumeration of mode terms - i.e designate the mode span.
*/
typedef enum
{

  AEECM_PREF_TERM_NONE=-1,

  AEECM_PREF_TERM_PERMANENT,
    /* Permanent mode change */

  AEECM_PREF_TERM_PWR_CYCLE,
    /* Until power cycle */

  AEECM_PREF_TERM_1_CALL,
    /* Until end of next call or power-cycle */

  AEECM_PREF_TERM_1_CALL_OR_TIME,
    /* Until end of next call, specified timeout or power-cycle */

  AEECM_PREF_TERM_NET_SEL_MANUAL,
    /* Preference term to be used when AEECM_NETWORK_SEL_MODE_PREF_MANUAL
     * is used.  Certain preferences will be written to NV, while others
     * will not.
     */

  AEECM_PREF_TERM_BIG = 0x10000000
    /* Force 32-bit enums */

} AEECMPrefTerm;

typedef enum {
  AEECM_TECH_NONE = -1,
  AEECM_TECH_WLAN_80211A = 0,
  AEECM_TECH_WLAN_80211B,
  AEECM_TECH_WLAN_80211G
} AEECMTechWLAN;
/* Enumeration of system selection WLAN Technology  preferences.
*/
/* For compatibility with QXDM database, please do not change the following
** fix assigned values. If new values are added please also update the QXDM
** database
*/
typedef enum {

  AEECM_WLAN_TECH_PREF_NONE=0,
    /* FOR INTERNAL USE OF CM ONLY! */

  AEECM_WLAN_TECH_PREF_A       =          AEECM_BM(AEECM_TECH_WLAN_80211A),
    /* Acquire 802.11a only systems */

  AEECM_WLAN_TECH_PREF_B       =          AEECM_BM(AEECM_TECH_WLAN_80211B),
    /* Acquire 802.11b only systems */

  AEECM_WLAN_TECH_PREF_G_ONLY  =          AEECM_BM(AEECM_TECH_WLAN_80211G),
    /* Acquire 802.11g only systems */

  AEECM_WLAN_TECH_PREF_G       =          AEECM_BM(AEECM_TECH_WLAN_80211B) | \
                                       AEECM_BM(AEECM_TECH_WLAN_80211G),
    /* Acquire Any 802.11g systems */

  AEECM_WLAN_TECH_PREF_ANY     =          AEECM_BM(AEECM_TECH_WLAN_80211A) | \
                                       AEECM_BM(AEECM_TECH_WLAN_80211B) | \
                                       AEECM_BM(AEECM_TECH_WLAN_80211G),
    /* Acquire Any 802.11 systems */

  AEECM_WLAN_TECH_PREF_NO_CHANGE,
    /* Do not change the WLAN Technology preference */

  AEECM_WLAN_TECH_PREF_MAX,
    /* FOR INTERNAL USE OF CM ONLY! */

  AEECM_WLAN_TECH_PREF_BIG=0x10000000
    /* To force enum to 32 bit for windows NT */

} AEECMWLANTechPref;



#define AEECM_WLAN_TECH_TO_TECH_PREF(tech, techpref)  \
        { \
           switch((tech)) {  \
             case AEECM_TECH_NONE: \
               (techpref) = AEECM_WLAN_TECH_PREF_NONE; \
               break; \
             case AEECM_TECH_WLAN_80211A: \
               (techpref) = AEECM_WLAN_TECH_PREF_A; \
               break; \
             case AEECM_TECH_WLAN_80211B: \
               (techpref) = AEECM_WLAN_TECH_PREF_B; \
               break; \
             case AEECM_TECH_WLAN_80211G: \
               (techpref) = AEECM_WLAN_TECH_PREF_G_ONLY; \
               break; \
             default: \
               (techpref) = AEECM_WLAN_TECH_PREF_ANY; \
               break; \
           } \
         }

/* Enumeration of mode preferences.
*/
typedef enum
{

  AEECM_MODE_PREF_NONE=-1,

  AEECM_MODE_PREF_AMPS_ONLY=0,
    /* = NV_MODE_ANALOG_ONLY, Service is limited to analog only */

  AEECM_MODE_PREF_DIGITAL_ONLY=1,
    /* = NV_MODE_DIGITAL_ONLY, Service is limited to digital
    ** (CDMA, HDR, GW) only */

  AEECM_MODE_PREF_AUTOMATIC=2,
    /* = NV_MODE_AUTOMATIC, Determine the mode automatically */

  AEECM_MODE_PREF_EMERGENCY=3,
    /* = NV_MODE_EMERGENCY, Emergency mode */

  /* For compatibility with QPST, do not change values or
  ** order. We start with NV_MODE_CELL_CDMA_ONLY+1 (i.e. 9).
  */
  AEECM_MODE_PREF_CDMA_ONLY = 9,
    /* Service is limited to CDMA only */

  AEECM_MODE_PREF_HDR_ONLY=10,
    /* Service is limited to HDR only */

  AEECM_MODE_PREF_CDMA_AMPS_ONLY=11,
    /* Service is limited to CDMA and AMPS only */

  AEECM_MODE_PREF_GPS_ONLY=12,
    /* Service is limited to GPS only */

  /*
  ** The following enums are the radio access technologies for
  ** WCDMA and GSM.
  */
  AEECM_MODE_PREF_GSM_ONLY=13,
    /* Service is limited to GSM only */

  AEECM_MODE_PREF_WCDMA_ONLY=14,
    /* Service is limited to WCMDA only */

  /*
  ** Others
  */
  AEECM_MODE_PREF_PERSISTENT=15,
    /* Return to persistent mode preference */

  AEECM_MODE_PREF_NO_CHANGE=16,
    /* Do not change the mode preference */

  AEECM_MODE_PREF_GSM_WCDMA_ONLY= 19,
    /* Service is limited to GSM and WCDMA only. */

  AEECM_MODE_PREF_CDMA_HDR_ONLY=22,
  /* CDMA and HDR modes are supported only */

  AEECM_MODE_PREF_WLAN_ONLY=25,
    /*
    ** Acquire WLAN systems only
    */

  AEECM_MODE_PREF_CDMA_WLAN=26,
    /*
    ** Acquire CDMA and WLAN systems only
    */

  AEECM_MODE_PREF_HDR_WLAN=27,
    /*
    ** Acquire HDR and WLAN systems only
    */

  AEECM_MODE_PREF_CDMA_HDR_WLAN=28,
    /*
    ** Acquire CDMA, HDR and WLAN systems only
    */

  AEECM_MODE_PREF_GSM_WLAN=29,
    /*
    ** Acquire GSM and WLAN systems only
    */

  AEECM_MODE_PREF_WCDMA_WLAN=30,
    /*
    ** Acquire WCDMA and WLAN systems only
    */

  AEECM_MODE_PREF_GW_WLAN=31,
    /*
    ** Acquire GSM/WCDMA and WLAN systems only
    */

  AEECM_MODE_PREF_CURRENT_PLUS_WLAN=32,
    /*
    ** Acquire WLAN systems in addition to current system
    */

  AEECM_MODE_PREF_CURRENT_LESS_WLAN=33,
    /*
    ** Remove WLAN systems from the current system
    */

  AEECM_MODE_PREF_CDMA_AMPS_HDR_WLAN_ONLY = 34,
    /*
    ** Acquire CDMA, AMPS, HDR and WLAN systems.
    */

  AEECM_MODE_PREF_CDMA_AMPS_WLAN_ONLY = 35,
    /*
    ** Acquire CDMA, AMPS and WLAN systems.
    */

  AEECM_MODE_PREF_BIG = 0x10000000
    /* Force 32-bit enums */

} AEECMModePref;

/* Enumeration to indicate the type of system to be avoided
*/
typedef enum {

  AEECM_SS_AVOID_SYS_NONE        = -1,

  AEECM_SS_AVOID_SYS_SRV_IF_UZ,
    /* Avoid serving system only if MS has currently selected user zone */

  AEECM_SS_AVOID_SYS_SRV,
    /* Avoid serving system */

  AEECM_SS_AVOID_BIG = 0x10000000
    /* Force 32-bit enums */

} AEECMAvoidSSPref;


/* Enumeration of answer voice as data settings. CDMA Only.
*/
typedef enum
{

  AEECM_ANSWER_VOICE_NONE=-1,

  AEECM_ANSWER_VOICE_AS_VOICE,
    /* = DB_VOICE_AS_DATA_NEVER, Answer voice as voice */

  AEECM_ANSWER_VOICE_AS_FAX_ONCE,
    /* = DB_VOICE_AS_FAX_ONCE, Answer voice as fax once (for the next
    ** 10 minutes) */

  AEECM_ANSWER_VOICE_AS_FAX_ALWAYS,
     /* = DB_VOICE_AS_FAX_ALWAYS, Answer voice as fax always */

  AEECM_ANSWER_VOICE_AS_MODEM_ONCE,
    /* = DB_VOICE_AS_MODEM_ONCE, Answer voice as modem once (for the
    ** next 10 minutes) */

  AEECM_ANSWER_VOICE_AS_MODEM_ALWAYS,
   /* = DB_VOICE_AS_MODEM_ALWAYS, Answer voice as modem always */

  AEECM_ANSWER_VOICE_BIG = 0x10000000
    /* Force 32-bit enums */

} AEECMAnsVoicePref;

/* Direction of Call */
typedef enum
{
  AEECM_CALL_DIRECTION_NONE,
    /* Undefined Direction */

  AEECM_CALL_DIRECTION_MO,
    /* Mobile Originated Call */

  AEECM_CALL_DIRECTION_MT,
    /* Mobile Terminated Call */

  AEECM_CALL_DIRECTION_BIG = 0x10000000
    /* Mobile Terminated Call */

} AEECMCallDirection;

/* OTASP activation code. For CDMA only.
*/
typedef enum
{
  AEECM_ABAND,
  /* A band */

  AEECM_BBAND,
  /* B band */

  AEECM_ABLOCK,
  /* A block */

  AEECM_BBLOCK,
  /* B block */

  AEECM_CBLOCK,
  /* C block */

  AEECM_DBLOCK,
  /* D block */

  AEECM_EBLOCK,
  /* E block */

  AEECM_FBLOCK,
  /* F block */

  AEECM_PREFERRED,
  /* Preferred OTASP */

  AEECM_ACTIVATECODE_BIG = 0x10000000
    /* Force 32-bit enums */

} AEECMActivateCode;

/* Enumeration of call states. At all times a CM call must be in one
** of the states below.
*/
typedef enum
{

  AEECM_CALL_STATE_NONE=0,

  AEECM_CALL_STATE_IDLE= AEECM_BM(0),
    /* Call is in idle state - i.e. no call */

  AEECM_CALL_STATE_ORIG = AEECM_BM(1),
    /* Call is in origination state */

  AEECM_CALL_STATE_INCOM = AEECM_BM(2),
    /* Call is in alerting state */

  AEECM_CALL_STATE_CONV = AEECM_BM(3),
    /* Call is in conversation state */

  AEECM_CALL_STATE_ENDED = AEECM_BM(4),
   /* Call has already ended */

  AEECM_CALL_STATE_ONHOLD = AEECM_BM(5),
   /* Call is on hold */

  AEECM_CALL_STATE_DORMANT = AEECM_BM(6),
  /* Call is in dormant mode */

  AEECM_CALL_STATE_SETUP = AEECM_BM(7),
  /* Call is in the setup state */

  AEECM_CALL_STATE_BIG = 0x10000000
    /* Force 32-bit enums */

} AEECMCallState;

/* Enumeration of CM call types.
*/
typedef enum
{

  AEECM_CALL_TYPE_NONE=0,

  AEECM_CALL_TYPE_VOICE=AEECM_BM(0),
    /* Voice call type.
    **
    ** NOTE! Answering with this type is only successful if the
    ** incoming call is of type VOICE */

  AEECM_CALL_TYPE_CS_DATA=AEECM_BM(1),
    /* CS Data call type - (for modem, fax, etc. calls).
    **
    ** NOTE! Answering with this call is only successful if the
    ** incoming call is of type DATA or VOICE (with AMPS service) */

  AEECM_CALL_TYPE_PS_DATA=AEECM_BM(2),
    /* Packet Switched Data call type */

  AEECM_CALL_TYPE_SMS=AEECM_BM(3),
    /* SMS call type (SMS SO-6 and SO-14). CDMA only
    **
    ** NOTE! Answering with this call type is only successful
    ** if incoming call is of type SMS */

  AEECM_CALL_TYPE_PD=AEECM_BM(4),
    /* Position Determination call type (SO-35 and 36). CDMA only
    ** This call type used to exchange IS-801 messages.
    ** Valid for FEATURE_GPSONE_DBM */

  AEECM_CALL_TYPE_TEST=AEECM_BM(5),
    /* Test call type (for Markov, Loopback, etc.). CDMA only
    **
    ** NOTE! Answering with this call type is only successful
    ** if incoming call is of type TEST */

  AEECM_CALL_TYPE_OTAPA=AEECM_BM(6),
    /* OTAPA call type (OTAPA SO-18 and SO-19). CDMA only
    **
    ** NOTE! Answering with this call type is only successful
    ** if incoming call is of type OTAPA */

  /* Call types that are automatically assigned by call manager when
  ** detecting an origination dial-string that matches one of the
  ** special numbers that are stored in the phone-book.
  **
  ** NOTE! This call types should not be used to originate
  ** or answer calls.
  */

  AEECM_CALL_TYPE_STD_OTASP=AEECM_BM(7),
    /* Standard OTASP call type. CDMA only
    **
    ** This call type is automatically assigned by call manager when
    ** detecting an origination dial-string that matches one of the
    ** standard OTASP numbers that are stored in the phone-book.
    **
    ** NOTE! This call type should not be used to originate
    ** or answer calls */

  AEECM_CALL_TYPE_NON_STD_OTASP=AEECM_BM(8),
    /* None standard OTASP call type. CDMA only
    **
    ** This call type is automatically assigned by call manager when
    ** detecting an origination dial-string that matches one of the none
    ** standard OTASP numbers that are stored in the phone-book.
    **
    ** NOTE! This call type should not be used to originate
    ** or answer calls */

  AEECM_CALL_TYPE_EMERGENCY=AEECM_BM(9),
    /* Emergency call type.
    **
    ** This call type is automatically assigned by call manager when
    ** detecting an origination dial-string that matches one of the
    ** EMERGENCY numbers that are stored in the phone-book.
    **
    ** This call type can be used by Clients also to originate calls.
    **
    ** NOTE! If call manager assigned this call type, the mode
    ** preference is forced to EMERGENCY before origination. If at the
    ** end of the call the phone is not in EMERGENCY callback mode,
    ** the mode is automatically restored to its original selection.
    ** Else the EMERGENCY mode stays in effect until changed by a client
    ** or power-cycle */

  AEECM_CALL_TYPE_BIG = 0x10000000
    /* Force 32-bit enums */

} AEECMCallType;

/* Enumeration of call end statuses
** - i.e. the reason for ending a call.
*/
typedef enum
{

  AEECM_CALL_END_NONE=-1,

  /* Common
  */
  AEECM_CALL_END_OFFLINE=0,
    /* phone is offline */

  /* CDMA
  */
  AEECM_CALL_END_CDMA_LOCK=20,
    /* phone is CDMA locked until power cycle. CDMA only */

  AEECM_CALL_END_NO_SRV=21,
    /* phone has no service. CDMA only */

  AEECM_CALL_END_FADE=22,
    /* Call has ended abnormally. CDMA only */

  AEECM_CALL_END_INTERCEPT=23,
    /* received intercept from BS - orig only. CDMA only */

  AEECM_CALL_END_REORDER=24,
    /* received reorder from BS - orig only. CDMA only */

  AEECM_CALL_END_REL_NORMAL=25,
    /* received release from BS - no reason given. CDMA only */

  AEECM_CALL_END_REL_SO_REJ=26,
    /* received release from BS - SO reject. CDMA only */

  AEECM_CALL_END_INCOM_CALL=27,
    /* received incoming call from BS */

  AEECM_CALL_END_ALERT_STOP=28,
    /* received alert stop from BS - incom only. CDMA only */

  AEECM_CALL_END_CLIENT_END=29,
    /* client ended the call */

  AEECM_CALL_END_ACTIVATION=30,
    /* received end activation - OTASP call only. CDMA only */

  AEECM_CALL_END_MC_ABORT=31,
    /* MC aborts the origination/conversation. CDMA only */

  AEECM_CALL_END_MAX_ACCESS_PROBE=32,
    /* Max access probes transmitted. CDMA only */

  AEECM_CALL_END_PSIST_NG=33,
    /* Persistence test failure. FEATURE_JCDMA only. CDMA only */

  AEECM_CALL_END_UIM_NOT_PRESENT=34,
    /* RUIM is not present */

  AEECM_CALL_END_ACC_IN_PROG=35,
    /* Access attempt already in progress. */

  AEECM_CALL_END_ACC_FAIL=36,
    /* Access failure for reason other than the above */

  AEECM_CALL_END_RETRY_ORDER=37,
    /* Received retry order - orig only. IS-2000. CDMA only */

  AEECM_CALL_END_CCS_NOT_SUPPORTED_BY_BS=38,
    /* Concurrent servive is not supported by base station */

  AEECM_CALL_END_NO_RESPONSE_FROM_BS=39,
    /* No response recived from base station */

  AEECM_CALL_END_REJECTED_BY_BS=40,
    /* Call rejected by the base station. CDMA only */

  AEECM_CALL_END_INCOMPATIBLE=41,
    /* Concurrent services requested were not compatible. CDMA only */

  AEECM_CALL_END_ACCESS_BLOCK=42,
    /* Access is blocked by base station. CDMA only */

  AEECM_CALL_END_ALREADY_IN_TC=43,
    /* Corresponds to AEECM_CALL_ORIG_ERR_ALREADY_IN_TC */

  AEECM_CALL_END_EMERGENCY_FLASHED=44,
    /* Call is ended due to Emergency call is flashed over this call. CDMA only */

  AEECM_CALL_END_USER_CALL_ORIG_DURING_GPS=45,
    /* Used if CM is ending a GPS call in favor of a user call */

  AEECM_CALL_END_USER_CALL_ORIG_DURING_SMS=46,
    /* Used if CM is ending a SMS call in favor of a user call */

  /* GSM/WCDMA
  */
  AEECM_CALL_END_LL_CAUSE=100,
    /* rxd a reason from lower layer,look in cc_cause. WCDMA/GSM only*/

  AEECM_CALL_END_CONF_FAILED,
    /* call orig request failed. WCDMA/GSM only */

  AEECM_CALL_END_INCOM_REJ,
    /* client rejected the incoming call. WCDMA/GSM only */

  AEECM_CALL_END_SETUP_REJ,
    /* client rejected the setup_ind. WCDMA/GSM only */

  AEECM_CALL_END_NETWORK_END,
    /* network ended the call,look in cc_cause. WCDMA/GSM only*/

  AEECM_CALL_END_NO_FUNDS,
    /* GSM/WCDMA only */

  AEECM_CALL_END_NO_GW_SRV,
    /* GWM/WCDMA only.  Phone has no service */

  AEECM_CALL_END_NO_CDMA_SRV,
    /* 1x only.  Phone has no service */


  /* HDR
  */
  AEECM_CALL_END_CD_GEN_OR_BUSY=150,
    /* abort connection setup due to the reception of a ConnectionDeny msg
    ** with deny code = general or network busy */

  AEECM_CALL_END_CD_BILL_OR_AUTH=151,
    /* abort connection setup due to the reception of a ConnectionDeny msg
    ** with deny code = billing or authentication failure */

  AEECM_CALL_END_CHG_HDR=152,
    /* change HDR system due to redirection or PRL not preferred */

  AEECM_CALL_END_EXIT_HDR=153,
    /* exit HDR  due to redirection or PRL not preferred */

  AEECM_CALL_END_HDR_NO_SESSION=154,
    /* no HDR session  */

  AEECM_CALL_END_CM_COLLOC_ACQ_FAIL=155,
    /* only for internal CM use -Fail to acquire collocated HDR for
    ** origination*/

  AEECM_CALL_END_HDR_ORIG_DURING_GPS_FIX=156,
    /* Used if CM is ending a HDR call orig in favor of GPS fix */

  AEECM_CALL_END_HDR_CS_TIMEOUT=157,

  AEECM_CALL_END_BIG = 0x10000000
    /* Force 32-bit enums */

} AEECMEndStatus;


/* Enumeration of Ph status/errors returned through callback */
typedef enum
{
  AEECM_PH_ERROR_NONE = AEE_SUCCESS,

  AEECM_PH_ERROR_IN_USE_S,
    /* Phone is in-use (i.e. in a call) */

  AEECM_PH_ERROR_ONLINE_S,
    /* Phone is offline */

  AEECM_PH_ERROR_OPRT_MODE_S,
    /* Invalid command or operating mode switch relatively to the current
    ** operating mode - e.g. OFFLINE -> ONLINE */

  AEECM_PH_ERROR_MODE_PREF_S,
    /* Invalid mode preference selection relatively to what we
    ** currently have in the PRL - e.g. ANALOG_ONLY when there are no
    ** analog entries in the PRL */

  AEECM_PH_ERROR_SYS_PREF_S,
    /* Invalid system preference selection for this target
    ** - e.g. A_ONLY for a none 800 MHz target, etc. - obsolete in SD20*/

  AEECM_PH_ERROR_SYS_SEL_PREF_S,
    /* Invalid system selection preference for this target/PRL
    ** - e.g. band-class 1 for a PCS only target */

  AEECM_PH_ERROR_HYBR_PREF_S,
    /* FEATURE_HDR - Invalid command or hybrid preference selection */

  AEECM_PH_ERROR_SUBSCRIPTION_AVAILABLE_S,
    /* Invalid command relative to the current value of subscription
    ** available - e.g. transitioning from subscription available to
    ** subscription available is invalid or issuing a command when
    ** the subscription information is not available. GSM/WCDMA Only. */

  AEECM_PH_ERROR_SYS_NOT_IN_UZ,
      /* Indicates that the MS is not in user zone, when
      ** "Avoid system" on user zone was processed. CDMA Only
      */

  AEECM_PH_ERROR_OTHER,
    /* A problem other than the above was found */

  AEECM_PH_ERROR_CURRENT_NAM_IS_NOT_RUIM_NAM_S,
    /* Current NAM is not RUIM NAM. CDMA Only */

  AEECM_PH_ERROR_LINE_SWITCHING,
  AEECM_PH_ERROR_SELECTED_LINE,
  AEECM_PH_ERROR_ALS_NOT_SUPPORTED,
  AEECM_PH_ERROR_ALS_SIM_WRITE_FAILED,

  AEECM_PH_ERROR_BIG = 0x10000000
    /* Force 32-bit enums */

} AEECMPhError;

/* Call error provided along with the AEECM_EVENT_CALL_ERROR event.
** Bad parameter errors should be caught by ICM before sending a
** cmd to the lower layers.
*/
typedef enum
{
  AEECM_CALL_ERROR_NONE = AEE_SUCCESS,

  AEECM_CALL_ERROR_OFFLINE_S,
    /* Phone is offline */

  AEECM_CALL_ERROR_CDMA_LOCK_S,
    /* Phone is CDMA locked */

  AEECM_CALL_ERROR_FLASH_S,
    /* Client tried to send a flash while a call that not supporting
    ** flashes is in progress. CDMA only */

  AEECM_CALL_ERROR_ORIG_RES_S,
    /* Dialed number is not allowed under the current origination
    ** restriction */

  AEECM_CALL_ERROR_NO_FUNDS_S,
    /* Dialed number is not allowed because phone is out of funds. GW only */

  AEECM_CALL_ERROR_EMERGENCY_MODE_S,
    /* Client tried to originate a NONE EMERGENCY call while EMERGENCY mode
    ** preference is in effect. CDMA only */

  AEECM_CALL_ERROR_IN_USE_S,
    /* Can't perform the command in question while phone is in-use */

  AEECM_CALL_ERROR_SRV_TYPE_S,
    /* Client originated with service type that is currently not
    ** supported by the phone/PRL */

  AEECM_CALL_ERROR_CALL_TYPE_S,
    /* The command in question is invalid for the call(s) in progress.
    ** e.g. Client answered with inappropriate call type relative to
    ** to the type of the incoming call, etc. */

  AEECM_CALL_ERROR_CALL_STATE_S,
    /* The command in question is invalid in the current call state -
    ** e.g. answering a call that was already answered,
    ** ending a call that was already ended, etc. */

  AEECM_CALL_ERROR_SRV_STATE_S,
    /* The command in question is invalid in the current Phone serving state-
    ** e.g answering a call but serving state is no service */

  AEECM_CALL_ERROR_NO_SO_S,
    /* no valid service found for origination with AUTOMATIC
    ** or CDMA_AUTOMATIC srv type. CDMA only  */

  AEECM_CALL_ERROR_ALERT_ANS_BYPASS_S,
    /* Client is not allowed to answer the incoming call. CDMA only
    ** Valid for FEATURE_IS95B_ALERT_ANS_BYPASS */

  AEECM_CALL_ERROR_UIM_NOT_PRESENT_S,
    /* UIM not present. Valid for FEATURE_UIM_RUIM */

  AEECM_CALL_ERROR_BAD_PRIVACY_PREF_S,
    /* Invalid privacy preference. CDMA only */

  AEECM_CALL_ERROR_NO_CALL_ID_S,
    /* No call IDs can be allocated */

  AEECM_CALL_ERROR_CALL_ORIG_FAIL_S,
    /* Call origination failed in the lower layer */

  AEECM_CALL_ERROR_INVALID_MODE_S,
    /* Recieved the command in a mode different from the phone's
    ** operational mode
    */

  AEECM_CALL_ERROR_INVALID_SIM_STATE_S,
    /* Call origination failed because the SIM was marked
    ** by the network as invalid for the circuit and/or
    ** packet service domain.
    */

  AEECM_CALL_ERROR_NO_COLLOC_HDR,
    /* There is no clocated HDR */

  AEECM_CALL_ERROR_OTHER,
    /* A problem other than the above was found */

  AEECM_CALL_ERROR_CALL_CONTROL_REJECTED,
    /* Call control has rejected the command. */

  AEECM_CALL_ERROR_LOWER_LAYER,
    /* Call error from lower layer */

  AEECM_CALL_ERROR_BIG = 0x10000000
    /* Force 32-bit enums */

} AEECMCallError;

/*
** Define a type for the Localized Service Area identity.  The format
** is specified by 3GPP TS 24.008 and 23.003.
*/
typedef enum
{

  AEECM_LSA_ID_TYPE_NONE         = -1,
  AEECM_LSA_ID_TYPE_PLMN_SIG_NUM =  0,             /* The LSA is a PLMN      */
                                                 /* significant number     */
  AEECM_LSA_ID_TYPE_UNIVERSAL    =  1,             /* The LSA is a universal */
  AEECM_LSA_ID_BIG = 0x10000000
    /* Force 32-bit enums */

} AEECMLSAIdentityType;

/*
** Enumeration of service status.  Indicates whether service is
** available or not available.
*/
typedef enum
{
  AEECM_SRV_STATUS_NONE = -1,

  AEECM_SRV_STATUS_NO_SRV,
    /* No service                             */

  AEECM_SRV_STATUS_LIMITED,
    /* Limited service                        */

  AEECM_SRV_STATUS_SRV,
    /* Service available                      */

  AEECM_SRV_STATUS_LIMITED_REGIONAL,
    /* Limited regional service               */

  AEECM_SRV_STATUS_PWR_SAVE,
    /* MS is in power save or deep sleep      */

  AEECM_SRV_STATUS_BIG = 0x10000000
    /* Force 32-bit enums */

} AEECMSrvStatus;

/* Enumeration for BCMCS data service availability
*  Must match the values in sys_bcmcs_srv_status_e_type(sys.h)
*/
typedef enum {

  AEECM_BCMCS_SRV_STATUS_NONE = -1,
    /* FOR INTERNAL USE ONLY!
    */

  AEECM_BCMCS_SRV_STATUS_NO_SRV,
    /* No BCMCS service is available
    */

  AEECM_BCMCS_SRV_STATUS_SRV,
    /* BCMCS service is available
    */

  AEECM_BCMCS_SRV_STATUS_MAX,
    /* FOR INTERNAL USE OF CM ONLY!
    */

  AEECM_BCMCS_SRV_STATUS_BIG = 0x10000000
  
} AEECMBCMCSSrvStatus;

/* Enumeration for hsdpa, hsupa indication types
** High speed downlink packet access and high speed
** uplink packet access can be available within WCDMA
** service only
** NOTE: These enum values MUST match the ones in sys_hs_ind_e_type(sys.h)
*/
typedef enum
{

  AEECM_SYS_HS_IND_NONE = -1,
    /* FOR INTERNAL USE OF CM ONLY!
    */

  AEECM_SYS_HS_IND_HSDPA_HSUPA_UNSUPP_CELL,
    /* Cell does not support either HSDPA or HSUPA
    */

  AEECM_SYS_HS_IND_HSDPA_SUPP_CELL,
    /* Cell supports HSDPA
    */

  AEECM_SYS_HS_IND_HSUPA_SUPP_CELL,
    /* Cell supports HSUPA
    */

  AEECM_SYS_HS_IND_HSDPA_HSUPA_SUPP_CELL,
    /* Cell supports HSDPA and HSUPA
    */  

  AEECM_SYS_HS_IND_MAX
    /* FOR INTERNAL USE OF CM ONLY!
    */

} AEECMSysHsdpaHsupa;

/* Dual transfer mode allows simultaneous
** CS and ps calls during GSM only service
** Must match values in sys_dtm_support_e_type (sys.h)
*/
typedef enum
{

  AEECM_SYS_DTM_SUPPORT_NONE = -1,
    /* For bounds checking only
    */

  AEECM_SYS_DTM_SUPPORT_AVAIL,
    /* DTM support is available
    */

  AEECM_SYS_DTM_SUPPORT_NOT_AVAIL,
    /* DTM support is not available
    */

  AEECM_SYS_DTM_SUPPORT_MAX
    /* For bounds checking only
    */

} AEECMSysDtmSupport;

/*
** The following enumerated type defines type of service.
*/
typedef enum
{
  AEECM_SRV_DOMAIN_NONE = -1,

  AEECM_SRV_DOMAIN_NO_SRV,
    /* No service                             */

  AEECM_SRV_DOMAIN_CS_ONLY,
    /* Circuit Switched Only Capable          */

  AEECM_SRV_DOMAIN_PS_ONLY,
    /* Packet Switched Only Capable           */

  AEECM_SRV_DOMAIN_CS_PS,
  /* Circuit and Packet Switched Capable    */

  AEECM_SRV_DOMAIN_BIG = 0x10000000
    /* Force 32-bit enums */

} AEECMSrvDomain;

/*
** Enumeration of system modes.
*/
/* For compatibility with QXDM database, please do not change the following
** fix assigned values. If new values are added please also update the QXDM
** database
*/
typedef enum
{
  AEECM_SYS_MODE_NONE = -1,

  AEECM_SYS_MODE_NO_SRV=0,
    /* No service = NV_MODE_INACTIVE                      */

  AEECM_SYS_MODE_AMPS=1,
    /* Analog Mobile Phone System (AMPS) mode             */

  AEECM_SYS_MODE_CDMA=2,
    /* Code Division Multiple Access (CDMA) mode          */

  AEECM_SYS_MODE_GSM=3,
    /* Global System for Mobile communications (GSM) mode */

  AEECM_SYS_MODE_HDR=4,
    /* High Data Rate (HDR) mode                          */

  AEECM_SYS_MODE_WCDMA=5,
    /* Wideband CDMA (WCDMA) mode                         */

  AEECM_SYS_MODE_GPS=6,
    /* Global Positioning System (GPS) mode               */

  /* Value 7 reserved for the "GW" (GSM/WCDMA) mode in CM */

  AEECM_SYS_MODE_WLAN=8,
    /* WLAN mode                                          */

  AEECM_SYS_MODE_BIG = 0x10000000
    /* Force 32-bit enums */

} AEECMSysMode;


typedef enum {

  AEECM_CALL_MODE_INFO_NONE,
  
  AEECM_CALL_MODE_INFO_CDMA,
    /* Call info is CDMA */

  AEECM_CALL_MODE_INFO_GW_CS,
    /* Call info is GSM/WCDMA circuit switched */

  AEECM_CALL_MODE_INFO_GW_PS,
    /* Call info is GSM/WCDMA packet switched */

  AEECM_CALL_MODE_INFO_IP,
  /** Call info is WLAN*/

  AEECM_CALL_MODE_INFO_MAX = 0x10000000
    
} AEECMCallModeInfo;

/*
** Enumeration of PCS frequency blocks/Cellular systems.
*/
typedef enum
{
  AEECM_SYS_BLKSYS_NONE = -1,

  AEECM_SYS_BLKSYS_PCS_A,
    /* PCS block A                            */

  AEECM_SYS_BLKSYS_PCS_B,
    /* PCS block B                            */

  AEECM_SYS_BLKSYS_PCS_C,
    /* PCS block C                            */

  AEECM_SYS_BLKSYS_PCS_D,
    /* PCS block D                            */

  AEECM_SYS_BLKSYS_PCS_E,
    /* PCS block E                            */

  AEECM_SYS_BLKSYS_PCS_F,
    /* PCS block F                            */

  AEECM_SYS_BLKSYS_PCS_CELL_A,
    /* Cellular system A                      */

  AEECM_SYS_BLKSYS_PCS_CELL_B,
    /* Cellular system B                      */

  AEECM_SYS_BLKSYS_PCS_G,
    /* PCS E-Block                            */

  AEECM_SYS_BLKSYS_PCS_H,
    /* PCS F-Block                            */

  AEECM_SYS_BLKSYS_BIG = 0x10000000
    /* Force 32-bit enums */

} AEECMSysBlocks;


/*
** Define a enumerated type that indicates the type of system ID.
*/
typedef enum
{

  AEECM_SYS_ID_TYPE_UNDEFINED,
    /* System ID is undefined     */

  AEECM_SYS_ID_TYPE_UMTS,
    /* UMTS (WCDMA/GSM) system ID */

  AEECM_SYS_ID_TYPE_IS95,
    /* IS-95 system ID            */

  AEECM_SYS_ID_TYPE_IS856,
    /* IS-856 (HDR) system ID     */

  AEECM_SYS_ID_TYPE_BIG = 0x10000000
    /* Force 32-bit enums */

} AEECMSysIDType;

/*
** Enumeration of phone operating modes.
*/
/* For compatibility with QXDM database, please do not change the following
** fix assigned values. If new values are added please also update the QXDM
** database
*/
typedef enum
{
  AEECM_OPRT_MODE_NONE = -1,

  AEECM_OPRT_MODE_PWROFF = 0,
    /* Phone is powering off                            */

  AEECM_OPRT_MODE_FTM = 1,
    /* Phone is in factory test mode                    */

  AEECM_OPRT_MODE_OFFLINE = 2,
    /* Phone is offline                                 */

  AEECM_OPRT_MODE_OFFLINE_AMPS = 3,
    /* Phone is offline analog                          */

  AEECM_OPRT_MODE_OFFLINE_CDMA = 4,
    /* Phone is offline cdma                            */

  AEECM_OPRT_MODE_ONLINE = 5,
    /* Phone is online                                  */

  AEECM_OPRT_MODE_LPM = 6,
    /* Phone is in LPM - Low Power Mode                 */

  AEECM_OPRT_MODE_RESET = 7,
    /* Phone is resetting - i.e. power-cycling          */

  AEECM_OPRT_MODE_NET_TEST_GW = 8,
    /* Phone is conducting network test for GSM/WCDMA.  */
    /* This mode can NOT be set by the clients. It can  */
    /* only be set by the lower layers of the stack.    */

  AEECM_OPRT_MODE_BIG = 0x10000000
    /* Force 32-bit enums */

} AEECMOprtMode;

typedef enum
{
  AEECM_TEST_CONTROL_TYPE_NONE=-1,

  AEECM_TEST_CONTROL_TYPE_NO_TEST,
    /* Init value when no testing is going on */

  AEECM_TEST_CONTROL_TYPE_LB_MODE1,
    /* LOOPBACK MODE1 */

  AEECM_TEST_CONTROL_TYPE_LB_MODE2,
    /* LOOPBACK MODE2 */

  AEECM_TEST_CONTROL_TYPE_LB_MODE1_WITH_DUMMY_SIGNALLING,
    /* LOOPBACK MODE1 with dummy signalling */

  AEECM_TEST_CONTROL_TYPE_LB_MODE2_WITH_DUMMY_SIGNALLING,
    /* LOOPBACK MODE2 with dummy signalling */

  AEECM_TEST_CONTROL_TYPE_BIG = 0x10000000
    /* Force 32 bit enums */

} AEECMTestControlType;

/*
** The following enumerated type defines the radio access technologies.
*/
typedef enum
{
  AEECM_RAT_NONE = -1,

  AEECM_RAT_GSM_RADIO_ACCESS,
    /* GSM radio access technology            */

  AEECM_RAT_UMTS_RADIO_ACCESS,
  /* UMTS radio access technology           */

  AEECM_RAT_BIG = 0x10000000
    /* Force 32-bit enums */

} AEECMRadioAccessTech;

/*
** The following enumerated type defines the service capabilities
** of a network.
*/
typedef enum
{
  AEECM_SRV_CAPABILITY_NONE = -1,

  AEECM_SRV_CAPABILITY_NOT_AVAILABLE,
    /* Network can not provide service                                   */

  AEECM_SRV_CAPABILITY_CS_SERVICE_ONLY,
    /* Network is capable of providing Circuit Switched service          */

  AEECM_SRV_CAPABILITY_PS_SERVICE_ONLY,
    /* Network is capable of providing Packet Switched service           */

  AEECM_SRV_CAPABILITY_CS_PS_SERVICE,
  /* Network is capable of providing Circuit & Packet Switched service */

  AEECM_SRV_CAPABILITY_BIG = 0x10000000
    /* Force 32-bit enums */

} AEECMSrvCapability;

typedef enum
{
  AEECM_DETAILED_PLMN_LIST_CATEGORY_NONE = -1,
  AEECM_DETAILED_PLMN_LIST_CATEGORY_HPLMN,
  AEECM_DETAILED_PLMN_LIST_CATEGORY_PREFERRED,
  AEECM_DETAILED_PLMN_LIST_CATEGORY_USER_PREFERRED,
  AEECM_DETAILED_PLMN_LIST_CATEGORY_OPERATOR_PREFERRED,
  AEECM_DETAILED_PLMN_LIST_CATEGORY_OTHER,
  AEECM_DETAILED_PLMN_LIST_CATEGORY_BIG = 0x10000000
    /* Force 32-bit enums */

} AEECMDetailedPLMNCat;

/*
** The following enumerated type defines the PLMN's signal quality.
*/
typedef enum
{
  AEECM_SIGNAL_QUALITY_NONE = -1,
  AEECM_SIGNAL_QUALITY_HIGH,
  AEECM_SIGNAL_QUALITY_LOW,
  AEECM_SIGNAL_QUALITY_BIG = 0x10000000
    /* Force 32-bit enums */

} AEECMSignalQuality;

/*
** Define a type for the Network Name Coding Scheme.  The format is
** specified by 3GPP TS 24.008 and 03.38.
*/
typedef enum
{
  AEECM_NW_NAME_CODING_SCHEME_NONE                             = -1,

  AEECM_NW_NAME_CODING_SCHEME_CELL_BROADCAST_DEFAULT_ALPHABET  =  0,
    /* Cell Broadcast data coding, default alphabet, language unspecified */

  AEECM_NW_NAME_CODING_SCHEME_UCS2                             =  1,
    /* UCS2 coding scheme */

  AEECM_NW_NAME_CODING_SCHEME_RESERVED                         =  2,
    /* Reserved */

  AEECM_NW_NAME_CODING_SCHEME_BIG = 0x10000000
    /* Force 32-bit enums */

} AEECMNWCodingType;

/*
** The following enumerated type defines the state of the
** Subscriber Identfication Module (SIM) card.
*/
typedef enum
{
  AEECM_SIM_STATE_NONE = -1,

  AEECM_SIM_STATE_NOT_AVAILABLE,
    /* The SIM is not available               */

  AEECM_SIM_STATE_AVAILABLE,
    /* The SIM is available                   */

  AEECM_SIM_STATE_CS_INVALID,
    /* The SIM has been marked by the network */
    /* as invalid for CS services             */

  AEECM_SIM_STATE_PS_INVALID,
    /* The SIM has been marked by the network */
    /* as invalid for PS services             */

  AEECM_SIM_STATE_CS_PS_INVALID,
    /* The SIM has been marked by the network */
    /* as invalid for CS and PS services      */

  AEECM_SIM_STATE_BIG = 0x10000000
    /* To force enum to 32 bit for windows NT */

} AEECMSIMState;

/* Signal types */
typedef enum
{
  AEECM_SIGNAL_NONE,

  AEECM_SIGNAL_GW_TONE,
    /* GSM/WCDMA Tone Signal */

  AEECM_SIGNAL_CDMA_TONE,
    /* CDMA Tone signal */

  AEECM_SIGNAL_CDMA_ISDN,
    /* CDMA ISDN Alerting */

  AEECM_SIGNAL_CDMA_IS54B,
    /* CDMA IS-54B Alerting */

  AEECM_SIGNAL_BIG = 0x10000000
    /* Force 32-bit enums */

} AEECMSignalType;

/* GSM/WCDMA Signal Tones */
typedef enum
{

  AEECM_GW_DIAL_TONE_ON,
    /* dial tone */

  AEECM_GW_RING_BACK_TONE_ON,
    /* ring back tone */

  AEECM_GW_INTERCEPT_TONE_ON,
    /* intercept tone */

  AEECM_GW_NW_CONGESTION_TONE_ON,
    /* network congestion */

  AEECM_GW_BUSY_TONE_ON,
    /* busy tone */

  AEECM_GW_CONFIRM_TONE_ON,
    /* call confirmed tone */

  AEECM_GW_ANSWER_TONE_ON,
    /* answer tone */

  AEECM_GW_CALL_WAITING_TONE_ON,
    /* call waiting tone */

  AEECM_GW_OFF_HOOK_WARNING_TONE_ON,
    /* off hook warning tone */

  AEECM_GW_TONES_OFF = 0x3F,
    /* no tones */

  AEECM_GW_ALERTING_TONE_OFF = 0x4F,
    /* alerting tone off */

  AEECM_GW_TONE_BIG = 0x10000000
    /* Force 32-bit enums */

} AEECMGWSignalTone;

/* CDMA Signal Tones */
typedef enum
{

  AEECM_CDMA_TONE_DIAL,
    /* Dial tone on */

  AEECM_CDMA_TONE_RING,
    /* Ring back tone on */

  AEECM_CDMA_TONE_INTERCEPT,
    /* Intercept tone on */

  AEECM_CDMA_TONE_ABBR_INTERCEPT,
    /* Abbreviated intercept: alternating tones repeated for four seconds */

  AEECM_CDMA_TONE_REORDER,
    /* Network congestion (reorder) tone on */

  AEECM_CDMA_TONE_ABBR_REORDER,
    /* Abbreviated network congestion (reorder) */

  AEECM_CDMA_TONE_BUSY,
    /* Busy tone on */

  AEECM_CDMA_TONE_CONFIRM,
    /* Confirm tone on */

  AEECM_CDMA_TONE_ANSWER,
    /* Answer tone on */

  AEECM_CDMA_TONE_CALLWAIT,
    /* Call waiting tone on */

  AEECM_CDMA_TONE_PIP=0x0A,
    /* Pip tone on */

  AEECM_CDMA_TONE_OFF=0x3F,
    /* Tones off */

  AEECM_CDMA_TONE_BIG=0x10000000
    /* Force 32-bit enums */

} AEECMCDMASignalTone;

/* CDMA ISDN Alerting signals */
typedef enum
{
  AEECM_CDMA_ISDN_NORMAL,
  /* Normal Alerting */

  AEECM_CDMA_ISDN_INTERGROUP,
  /* Intergroup Alerting */

  AEECM_CDMA_ISDN_SPECIAL,
  /* Special/Priority Alerting */

  AEECM_CDMA_ISDN_RESERVED3,
  /* Reserved (ISDN Alerting pattern 3) */

  AEECM_CDMA_ISDN_PING,
  /* "Ping ring": single burst of 500 ms */

  AEECM_CDMA_ISDN_RESERVED5,
  /* Reserved (ISDN Alerting pattern 5) */

  AEECM_CDMA_ISDN_RESERVED6,
  /* Reserved (ISDN Alerting pattern 6) */

  AEECM_CDMA_ISDN_RESERVED7,
  /* Reserved (ISDN Alerting pattern 7) */

  AEECM_CDMA_ISDN_OFF=0x0F,
  /* Alerting off */

  AEECM_CDMA_ISDN_BIG=0x10000000
  /* Force 32-bit enums */
} AEECMCDMASignalISDN;

/* CDMA IS-54B Alerting Signals */
typedef enum
{
  AEECM_CDMA_IS54B_OFF,
  /* No Tone: off */

  AEECM_CDMA_IS54B_L,
  /* Long: (standard alert) */

  AEECM_CDMA_IS54B_SS,
  /* Short-short */

  AEECM_CDMA_IS54B_SSL,
  /* Short-short-long */

  AEECM_CDMA_IS54B_SS2,
  /* Short-short-2 */

  AEECM_CDMA_IS54B_SLS,
  /* Short-long-short */

  AEECM_CDMA_IS54B_SSSS,
  /* Short-short-short-short */

  AEECM_CDMA_IS54B_PBX_L,
  /* PBX Long */

  AEECM_CDMA_IS54B_PBX_SS,
  /* PBX Short-short */

  AEECM_CDMA_IS54B_PBX_SSL,
  /* PBX Short-short-long */

  AEECM_CDMA_IS54B_PBX_SLS,
  /* PBX Short-long-short */

  AEECM_CDMA_IS54B_PBX_SSSS,
  /* PBX Short-short-short-short */

  AEECM_CDMA_IS54B_PPPP,
  /* Pip-Pip-Pip-Pip */

  AEECM_CDMA_IS54B_BIG=0x10000000
  /* Force 32-bit enums */

} AEECMCDMASignalIS54B;

/* Signal Alert Pitch */
typedef enum
{
  AEECM_PITCH_NONE = -1,
    /* No Pitch information */

  AEECM_PITCH_MED,
    /* Medium pitch (standard alert) */

  AEECM_PITCH_HIGH,
    /* High pitch */

  AEECM_PITCH_LOW,
    /* Low Pitch */

  AEECM_PITCH_BIG=0x10000000
    /* Force 32-bit enums */

} AEECMSignalPitch;

/*
** Enumeration that defines the type of Wakeup Action required.
*/
typedef enum {

  AEECM_WAKEUP_NONE=-1,
    /* FOR INTERNAL USE OF CM ONLY! */

  AEECM_WAKEUP_MAIN   = AEECM_BM(0),
    /* Wakeup Main service. It could be CDMA, HDR, GSM, WCDMA or GW
    ** depending on the mode_pref and hybr_pref.
    */

  AEECM_WAKEUP_HDR    = AEECM_BM(1),
    /* Wakeup HDR service. It is valid only if hybr_pref is on */

  AEECM_WAKEUP_WLAN   = AEECM_BM(2),
    /* Wakeup WLAN Service */

  AEECM_WAKEUP_ALL    =  AEECM_WAKEUP_MAIN | \
                      AEECM_WAKEUP_HDR  | \
                      AEECM_WAKEUP_WLAN,
    /* Wakeup All services */

  AEECM_WAKEUP_BIG=0x10000000
    /* To force enum to 32 bit for windows NT */

} AEECMWakeupType;

/* AEECMWLANScanPref should be kept in sync with cm_wlan_scan_pref_e_type */
typedef enum {
  AEECM_WLAN_SCAN_PREF_ACTIVE = 0,
  AEECM_WLAN_SCAN_PREF_PASSIVE,
  AEECM_WLAN_SCAN_PREF_AUTO,
  AEECM_WLAN_SCAN_PREF_NO_CHANGE
} AEECMWLANScanPref;

#define AEECM_MAX_SSID_STR_SIZE 33

typedef uint64   AEECMWLAN_BSS_ID;
typedef AECHAR   AEECMWLAN_SSID[AEECM_MAX_SSID_STR_SIZE];
typedef uint16   AEECMWLANChannel;
typedef uint16   AEECMWLANProfileID;

/* AEECMWLAN2400Channel and AEECMWLAN5000Channel correspond to
 * net_chan_wlan_2400_e_type and net_chan_wlan_5000_e_type in CM,
 * respectively.  They define the values for the AEECMWLANChannel type.
 */
typedef enum
{
  AEECM_WLAN_2400_CHAN_1           = AEECM_BM(0),
  AEECM_WLAN_2400_CHAN_2           = AEECM_BM(1),
  AEECM_WLAN_2400_CHAN_3           = AEECM_BM(2),
  AEECM_WLAN_2400_CHAN_4           = AEECM_BM(3),
  AEECM_WLAN_2400_CHAN_5           = AEECM_BM(4),
  AEECM_WLAN_2400_CHAN_6           = AEECM_BM(5),
  AEECM_WLAN_2400_CHAN_7           = AEECM_BM(6),
  AEECM_WLAN_2400_CHAN_8           = AEECM_BM(7),
  AEECM_WLAN_2400_CHAN_9           = AEECM_BM(8),
  AEECM_WLAN_2400_CHAN_10          = AEECM_BM(9),
  AEECM_WLAN_2400_CHAN_11          = AEECM_BM(10),
  AEECM_WLAN_2400_CHAN_12          = AEECM_BM(11),
  AEECM_WLAN_2400_CHAN_13          = AEECM_BM(12),
  AEECM_WLAN_2400_CHAN_14          = AEECM_BM(13),

  AEECM_WLAN_2400_CHAN_ANY         = 0xFFFF
} AEECMWLAN2400Channel;

typedef enum
{
  AEECM_WLAN_5000_CHAN_36          = AEECM_BM(0),
  AEECM_WLAN_5000_CHAN_40          = AEECM_BM(1),
  AEECM_WLAN_5000_CHAN_44          = AEECM_BM(2),
  AEECM_WLAN_5000_CHAN_48          = AEECM_BM(3),
  AEECM_WLAN_5000_CHAN_52          = AEECM_BM(4),
  AEECM_WLAN_5000_CHAN_56          = AEECM_BM(5),
  AEECM_WLAN_5000_CHAN_60          = AEECM_BM(6),
  AEECM_WLAN_5000_CHAN_64          = AEECM_BM(7),
  AEECM_WLAN_5000_CHAN_149         = AEECM_BM(8),
  AEECM_WLAN_5000_CHAN_153         = AEECM_BM(9),
  AEECM_WLAN_5000_CHAN_157         = AEECM_BM(10),
  AEECM_WLAN_5000_CHAN_161         = AEECM_BM(11),

  AEECM_WLAN_5000_CHAN_ANY                = 0xFFFF
} AEECMWLAN5000Channel;

typedef enum
{
  AEECM_SYS_WLAN_SCAN_PREF_ACTIVE = 0,
    /* ACTIVE WLAN scan.
    */

  AEECM_SYS_WLAN_SCAN_PREF_PASSIVE,
    /* Passive WLAN Scan.
    */

  AEECM_SYS_WLAN_SCAN_PREF_AUTO
    /* CM to decide Active/Passive scan automatically.
    */
} AEECMWlanScanPrefStatus;

/*
** Enumeration that defines the WLAN scan preference
*/
typedef enum {

  AEECM_WLAN_SCAN_PREF_NONE =-1,
    /* FOR INTERNAL USE OF CM ONLY! */

  AEECMWlanSettingScanPrefActive   = AEECM_BM(AEECM_SYS_WLAN_SCAN_PREF_ACTIVE),
    /* ACTIVE WLAN scan.
    */

  AEECMWlanScanPrefPassive  = AEECM_BM(AEECM_SYS_WLAN_SCAN_PREF_PASSIVE),
    /* Passive WLAN Scan.
    */

  AEECMWlanScanPrefAuto     = AEECM_BM(AEECM_SYS_WLAN_SCAN_PREF_AUTO),
    /* CM to decide Active/Passive scan automatically.
    */

  AEECMWlanScanPrefNoChange,

  AEECM_WLAN_SCAN_PREF_MAX,

  AEECM_SERVICE_WAKEUP_BIG=0x10000000
    /* To force enum to 32 bit for windows NT */

} AEECMWlanScanPrefPref;

/* 
  Signal value depends on the signal type and the mapping is as follows.
  Signal Type:                   Signal Value
  ---------------------------------------------------
  AEECM_SIGNAL_GW_TONE:          AEECMGWSignalTone
  AEECM_SIGNAL_CDMA_TONE:        AEECMCDMASignalTone
  AEECM_SIGNAL_CDMA_ISDN:        AEECMCDMASignalISDN
  AEECM_SIGNAL_CDMA_IS54B:       AEECMCDMASignalIS54B
*/
typedef union
{
  AEECMGWSignalTone      gw_tone;
  AEECMCDMASignalTone    cdma_tone;
  AEECMCDMASignalISDN    cdma_isdn;
  AEECMCDMASignalIS54B   cdma_is54b;
} AEECMSignalValue;

/* Signal structure specify tones to be played
*/
typedef struct
{

  boolean               is_signal_info_avail;
    /* Indicate whether signal information is available. */

  AEECMSignalType       signal_type;
    /* Signal type - Tone Signal, ISDN Alerting or IS-54B Alerting */

  AEECMSignalValue      signal;
    /* Indicate which Tone signals, ISDN Alerting or IS_54B Alerting */

  AEECMSignalPitch      signal_pitch;
    /* Alert pitch - Medium, High or Low
     * (valid only for AEECM_SIGNAL_CDMA_IS54B) */

} AEECMSignal;

/*
** The following enumerated type defines the access technologies that are
** defined for the elementary file, EFPLMNwAcT (User controlled PLMN selector
** with Access Technology).  The #defines are to be used in a bit mask.
** Refer to TS 11.11.
*/
typedef uint16 AEECMAccessTech;

#define  AEECM_SYS_ACT_GSM_MASK         ( (1<<0) )
    /* GSM access technology                  */

#define  AEECM_SYS_ACT_GSM_COMPACT_MASK ( (1<<1) )
    /* GSM compact access technology          */

#define  AEECM_SYS_ACT_UMTS_MASK        ( (1<<2) )
    /* UMTS radio access technology           */


/*
** The following type defines the PLMN's the signal strength in
** units that allow direct comparison of the signal strength between
** GSM and WCDMA cells.
**
*/
typedef int32 AEECMSignalStrength;

/*
** Define a type for time zone.  The time zone is an offset from
** Universal time (i.e. difference between local time and Universal
** time) in increments of 15 minutes.  The format for this type is
** specified in 3G TS 24.008 and 23.040.
*/
typedef int AEECMTimeZone;


/*
** Define a structure for the Time Zone and Universal Time mobility
** management information.
*/
typedef struct
{
  JulianType                                time;
  AEECMTimeZone                             time_zone;
} AEECMTimeAndZone;

/*
** Define a type for the Localized Service Area identity.  The format
** is specified by 3GPP TS 24.008 and 23.003.
*/
typedef struct
{
  AEECMLSAIdentityType              type;
  uint8                             identity[3];
} AEECMLSAIdentity;

/*
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
typedef struct
{
  byte     identity[3];
} AEECMPLMNID;


/*
**        LAC      local area code.
**/
typedef word  AEECMLACID;

/*
**        PLMN + LAC.
**/
typedef struct
{
  AEECMPLMNID   plmn;
  AEECMLACID    lac;
}AEECMPLMNLACID;


/* Detailed PLMN Info */
typedef struct
{
  AEECMPLMNID                       plmn;
    /* The ID of the PLMN                                        */

  AEECMRadioAccessTech              rat;
    /* The radio access technology of the PLMN                   */

  AEECMSrvCapability                plmn_service_capability;
    /* Indicates the type of service domain the PLMN can provide */

  boolean                           plmn_forbidden;
    /* Indicates if the PLMN is forbidden                        */

  AEECMDetailedPLMNCat              list_category;
    /* Indicates the type of PLMN                                */

  AEECMSignalQuality                signal_quality;
    /* The signal quality of the PLMN                            */

  AEECMSignalStrength               signal_strength;
    /* The signal strength of the PLMN                           */

} AEECMDetailedPLMNInfo;


/*------------------------------------------------------------------------------
   Mobility Management Info Data Types
------------------------------------------------------------------------------*/
/*
** Define types for the network's short name and full name.
**/
typedef struct
{
  uint8                        length;
    /* Number of characters in the full name */

  AEECMNWCodingType            coding_scheme;
  /* The coding scheme used to encode the full name of the network */

  boolean                      add_country_initials;
    /* Flag that indicates whether MS should add the country initials to
       the full name of the network */

  uint8                        name[AEECM_MAX_NW_NAME_CHARS];
    /* The full name of the network          */
} AEECMNWName;

/*
** Define a structure that contains information regarding the user
** preferred/controlled network.
*/
typedef struct
{
  AEECMPLMNID                        plmn;
    /* PLMN ID */

  AEECMAccessTech                    access_tech;
    /* Access technologies that are defined for file EFPLMNwAcT */

} AEECMUserPLMNInfo;

/*
** Define a structure for the list of user preferred/controlled networks.
*/
typedef struct
{
  byte                            length;
    /* The number of PLMNs in the list */

  AEECMUserPLMNInfo               info[AEECM_USER_PPLMN_LIST_MAX_LENGTH];
    /* PLMN Info */

} AEECMUserPLMNList;

/*
** Define a structure for the detailed PLMN list.
*/
typedef struct
{
  uint32                           length;
    /* The number of PLMNs in the list */

  AEECMDetailedPLMNInfo            info[AEECM_PLMN_LIST_MAX_LENGTH];
    /* Detailed PLMN Info */

} AEECMDetailedPLMNList;

/*==================================================================*/
/* Type definitions used for WLAN                                   */
/*==================================================================*/
typedef struct
{
  AEECMWLANChannel                       chan;
   /* Specifies the Channel */

  AEECMSysBandClass                      band;
   /* Indicate the band */

  AEECMWLAN_BSS_TYPE                     bss_type;

  AEECMWLAN_BSS_ID                       bss_id;
    /* Specifies the BSS */

  AEECMWLAN_SSID                         ssid;
    /* Specifies the SSID */

  uint16                                 rssi;

  AEECMTechWLAN                          tech;
    /* Specify 802.11 a/b/g etc. here. */
} AEECMWLANBSSINFO;

typedef struct
{
  uint32                           length;
  /* The number of BSSs in the list. */

  AEECMWLANBSSINFO                 info[AEECM_MAX_BSS_LIST];
  /* Details of each BSS. */

} AEECMWLANBSSInfoList;


/* Structure to hold information about services in current cell */
typedef struct {

  AEECMSysHsdpaHsupa         hs_ind;
    /* High speed service indication (Over WCDMA) */

  AEECMSysDtmSupport         dtm_supp;
    /* Dual Transfer mode indication (Over GSM) */

} AEECMCellSrvInd;


/* WLAN transmission/receive rates */
typedef enum
{
  AEECM_WLAN_RATE_NONE      = -1,
  AEECM_WLAN_RATE_1_MBPS,
  AEECM_WLAN_RATE_2_MBPS,
  AEECM_WLAN_RATE_5_5_MBPS,
  AEECM_WLAN_RATE_11_MBPS,
  AEECM_WLAN_RATE_MAX
} AEECMWLANXmitRate;

typedef struct
{
  AEECMWLANXmitRate   current_xmit_rate;
  uint32              total_tx_bytes;
  uint32              total_rx_bytes;
} AEECMWLANStats;


/*
** Define a structure that identifies an IS-95 system.
**/
typedef struct
{
  uint16         sid;
       /* The system ID */

  uint16         nid;
       /* The network ID */

} AEECMIS95SysID;

/*
** Type for system ID.
*/
typedef union
{
  AEECMIS95SysID  is95;
    /* IS-95 system ID             */

  byte                    is856[16];
    /* IS-856 system ID            */

  AEECMPLMNID      plmn;
    /* UMTS (WCDMA/GSM) system ID  */

  AEECMPLMNLACID   plmn_lac;

} AEECMSysIDData;

typedef struct
{
  AEECMSysIDType  id_type;
    /* System identifier type */

  AEECMSysIDData  id;
    /* The system identifier */

} AEECMSysID;

//Define a enumerated type for the daylight savings adjustment.
typedef enum
{

  AEECM_DAY_SAV_ADJ_NO_ADJUSTMENT =  0,
  AEECM_DAY_SAV_ADJ_PLUS_ONE_HOUR =  1,
  AEECM_DAY_SAV_ADJ_PLUS_TWO_HOUR =  2
} AEECMDSTAdj;

/*
** Define a structure that contains the currently registered network's
** mobility management information (i.e. network's name, date, time and
** time zone).  All fields in this structure are optional.  If the
** "available" flag is true, then the corresponding data field contains
** valid information.  If the "available" flag is false, then the
** corresponding data field should be ignored.
*/
typedef struct
{
  /*
  ** Flags that indicate whether the particular mm information is
  ** available.
  **/
  boolean                           plmn_avail;
  boolean                           full_name_avail;
  boolean                           short_name_avail;
  boolean                           time_and_time_zone_avail;
  boolean                           time_zone_avail;
  boolean                           lsa_identity_avail;
  boolean                           daylight_saving_adj_avail;

  AEECMPLMNID                       plmn;
  AEECMNWName                       full_name;
  AEECMNWName                       short_name;
  AEECMTimeAndZone                  time_and_time_zone;
  AEECMTimeZone                     time_zone;
  AEECMLSAIdentity                  lsa_identity;
  AEECMDSTAdj                       daylight_saving_adj;
} AEECMMMInfo;

/*
** structure to hold user zone name information.
*/

typedef struct
{
  byte     uz_name_encoding;

  byte     uz_name_n_octets;

  byte     uz_name[AEECM_PUZL_MAX_UZ_NAME_LEN];
} AEECMUZName;


typedef struct
{
  uint32    ccm;
    /* Current Call Meter */

  uint32    acm;
    /* Accumulated Call Meter */

  uint32    acm_max;
    /* Accumulated Call Meter Max */

} AEECMAOCInfo;

/*===========================================================================

                              NEW ENUMERATIONS

===========================================================================*/

typedef enum
{
  AEECM_DTMF_ON_95,
  /* 95 ms recommended pulse width */

  AEECM_DTMF_ON_150,
  /* 150 ms recommended pulse width */

  AEECM_DTMF_ON_200,
  /* 200 ms recommended pulse width */

  AEECM_DTMF_ON_250,
  /* 250 ms recommended pulse width */

  AEECM_DTMF_ON_300,
  /* 300 ms recommended pulse width */

  AEECM_DTMF_ON_350,
  /* 350 ms recommended pulse width */

  AEECM_DTMF_ON_SMS,
  /* SMS Tx special pulse width */

  AEECM_DTMF_ON_BIG = 0x10000000
  /* Force 32-bit enums */

} AEECMDTMFOnLength;

typedef enum
{
  AEECM_DTMF_OFF_60,
  /* 60 ms recommended minimum interdigit interval */

  AEECM_DTMF_OFF_100,
  /* 100 ms recommended minimum interdigit interval */

  AEECM_DTMF_OFF_150,
  /* 150 ms recommended minimum interdigit interval */

  AEECM_DTMF_OFF_200,
  /* 200 ms recommended minimum interdigit interval */

  AEECM_DTMF_OFF_BIG = 0x10000000
  /* Force 32-bit enums */

} AEECMDTMFOffLength;

/*---------------------------------------------------------------------------
  AEECM Event Types
 ---------------------------------------------------------------------------*/

typedef enum AEECMEventEnum
{
  AEECM_EVENT_NONE = 0x0000,

  /* Call Events */
  AEECM_EVENT_CALL_ORIG = 0x0101, /* Phone originated a call */
  AEECM_EVENT_CALL_ANSWER,        /* Incoming call was answered */
  AEECM_EVENT_CALL_END,           /* In/Orig call was ended */
  AEECM_EVENT_CALL_INCOM,         /* Received an incoming call */
  AEECM_EVENT_CALL_CONNECT,       /* In/Orig call connected */
  AEECM_EVENT_CALL_ERROR,         /* Call Error */
  AEECM_EVENT_CALL_OPS_ERROR,     /* Call related sups error */
  AEECM_EVENT_CALL_OPS,           /* phone sent Flash/Flash with Info to BS */
  AEECM_EVENT_CALL_CALLER_ID,     /* Caller ID info was received from BS */
  AEECM_EVENT_CALL_DISPLAY,        /* display info was received from BS. */
  AEECM_EVENT_CALL_CALLED_PARTY,   /* called party info was received from BS. */
  AEECM_EVENT_CALL_CONNECTED_NUM,  /* connected number info was received */
  AEECM_EVENT_CALL_BARRED,         /* call barred notification */
  AEECM_EVENT_CALL_IS_WAITING,     /* call is in waiting notification */
  AEECM_EVENT_CALL_ON_HOLD,        /* call on hold notification */
  AEECM_EVENT_CALL_RETRIEVED,      /* call retrieved notification */
  AEECM_EVENT_CALL_ORIG_FWD_STATUS,/* call may be forwarded (to forwarding MS)*/
  AEECM_EVENT_CALL_FORWARDED,      /* call forwarded (to forwarding MS) */
  AEECM_EVENT_CALL_BEING_FORWARDED,/* call being forwarded (to calling MS) */
  AEECM_EVENT_CALL_INCOM_FWD_CALL, /* Incoming forwarded call notification */
  AEECM_EVENT_CALL_RESTRICTED,     /* call restricted notification */
  AEECM_EVENT_CALL_CUG_INFO_RECEIVED,   /* call forward CUG notification */
  AEECM_EVENT_CALL_CNAP_INFO_RECEIVED,  /* caller name info notification */
  AEECM_EVENT_CALL_EMERGENCY_FLASHED,   /* voice call converted to emergency */
  AEECM_EVENT_CALL_DEFLECTION,        /* Call deflection notification */
  AEECM_EVENT_CALL_TRANSFERRED,       /* Call transfer notification */
  AEECM_EVENT_CALL_SRV_OPT,           /* Service option changed */
  AEECM_EVENT_CALL_PRIVACY,           /* Privacy mode changed */
  AEECM_EVENT_CALL_ABRV_ALERT,        /* CMDA/AMPS abbreviated alert. */
  AEECM_EVENT_CALL_ABRV_REORDER,      /* AMPS abbreviated reorder. */
  AEECM_EVENT_CALL_ABRV_INTERCEPT,    /* AMPS abbreviated intercept. */
  AEECM_EVENT_CALL_SIGNAL,            /* signal info was received from BS */
  AEECM_EVENT_CALL_PROGRESS_INFO_IND, /* call origination progress indication */
  AEECM_EVENT_CALL_ENTER_DORMANT,     /* for AEECM_CALL_EVENT_RAB_REL_IND */
  AEECM_EVENT_CALL_EXIT_DORMANT,      /* for AEECM_CALL_EVENT_RAB_REESTAB_REQ */
  AEECM_EVENT_CALL_LINE_CTRL,         /* Line control event - 1x Only*/
  AEECM_EVENT_CALL_RECALL_IND,        /*CCBS Recall Indication */
  AEECM_EVENT_CALL_CCBS_ALLOWED, /*CCBS is Allowed*/
  AEECM_EVENT_CALL_CCBS_SET,         /*CCBS has been set up*/
  AEECM_EVENT_CALL_ALL_CALLS_ENDED,   /* all calls are ended */
  AEECM_CALL_EVENT_NSS_REL_REC,       /* NSS Release Record */
  AEECM_EVENT_CALL_MAX,

  /* Phone events */
  AEECM_EVENT_PH_OPRT_MODE = 0x0201, /* Operating mode was changed */
  AEECM_EVENT_PH_SYSTEM_PREF,        /* System preference changed */
  AEECM_EVENT_PH_ANSWER_VOICE,       /* Answer voice as data was changed. */
  AEECM_EVENT_PH_NAM_SEL,            /* NAM selection was changed */
  AEECM_EVENT_PH_CURR_NAM,           /* Current NAM was changed. */
  AEECM_EVENT_PH_IN_USE_STATE,       /* In use state was changed */
  AEECM_EVENT_PH_CDMA_LOCK_MODE,     /* CDMA lock mode was changed. */
  AEECM_EVENT_PH_MAINTREQ,           /* CDMA maintenance required command. */
  AEECM_EVENT_PH_STANDBY_SLEEP,      /* Entering powerdown sleep mode */
  AEECM_EVENT_PH_STANDBY_WAKE,       /* Exiting powerdown sleep mode */
  AEECM_EVENT_PH_INFO_AVAIL,         /* Phone information is now available */
  AEECM_EVENT_PH_SUBSCRIPTION,       /* Subscription info changed */
  AEECM_EVENT_PH_FUNDS_LOW,          /* Funds running low. */
  AEECM_EVENT_PH_NVRUIM_CONFIG,      /* RTRE configuration changed. */
  AEECM_EVENT_PH_ACM_RESET,          /* Accumulated Call Meter was reset */
  AEECM_EVENT_PH_ACMMAX_SET,         /* Accumulated Call Meter was set */
  AEECM_EVENT_PH_SELECTED_LINE,      /* Selected ALS*/
  AEECM_EVENT_PH_LINE_SWITCHING,
  AEECM_EVENT_PH_MAX,

  /* Serving system Events */
  AEECM_EVENT_SS_SRV_CHANGED = 0x0301, /* Serving system information changed */
  AEECM_EVENT_SS_RSSI,                 /* RSSI changed */
  AEECM_EVENT_SS_HDR_RSSI,             /* HDR RSSI changed */
  AEECM_EVENT_SS_WLAN_RSSI,            /* WLAN RSSI changed */
  AEECM_EVENT_SS_INFO,                 /* Serving system information changed */
  AEECM_EVENT_SS_MAX,

  /* Inband Events */
  AEECM_EVENT_INBAND_FWD_BURST_DTMF = 0x0401, /* received a CDMA-burst DTMF */
  AEECM_EVENT_INBAND_FWD_START_CONT_DTMF,/* received a start-cont DTMF order */
  AEECM_EVENT_INBAND_FWD_STOP_CONT_DTMF, /* received a stop-cont DTMF order */
  AEECM_EVENT_INBAND_MAX,

  /* Internal Events, not exported to Applications */
  AEECM_EVENT_INTERNAL_SETUP_IND = 0x0501,
  AEECM_EVENT_INTERNAL_MNG_CALLS_CONF,
  AEECM_EVENT_INTERNAL_END_REQ,
  AEECM_EVENT_INTERNAL_EXT_DISP,
  AEECM_EVENT_INTERNAL_PDP_MODIFY_IND,
  AEECM_EVENT_INTERNAL_PDP_MODIFY_CONF,
  AEECM_EVENT_INTERNAL_RECALL_RSP,
  AEECM_EVENT_INTERNAL_CALL_MAX,

  AEECM_EVENT_INTERNAL_TEST_CONTROL = 0x0601,
  AEECM_EVENT_INTERNAL_PREF_NW_SET,
  AEECM_EVENT_INTERNAL_DDTM_PREF,
  AEECM_EVENT_INTERNAL_WAKEUP,
  AEECM_EVENT_INTERNAL_PH_MAX,

  /* MBMS Events */
  AEECM_EVENT_MBMS_GENERIC_RPT = 0x0701,
  AEECM_EVENT_MBMS_GENERIC_CMD,

  AEECM_EVENT_BIG = 0x10000000

} AEECMEvent;


/* Number type */
/* Note: Different from DMSS, require translation */
typedef enum {
  AEECM_TON_UNKNOWN = 0,
  AEECM_TON_INTERNATIONAL,
  AEECM_TON_NATIONAL,
  AEECM_TON_NETWORK_SPECIFIC,
  AEECM_TON_DEDICATED_ACCESS,
  AEECM_TON_RFU = 0xFF,
  AEECM_TON_BIG = 0x10000000
    /* Force 32-bit enums */

} AEECMNumberType;

/* Number Plan */
typedef enum {
  AEECM_NPI_UNKNOWN    = 0x00,
  AEECM_NPI_ISDN       = 0x01,
  AEECM_NPI_DATA       = 0x03,
  AEECM_NPI_TELEX      = 0x04,
  AEECM_NPI_NATIONAL   = 0x08,
  AEECM_NPI_PRIVATE    = 0x09,
  AEECM_NPI_RES_CTS    = 0x0B,
  AEECM_NPI_RES_EXT    = 0x0F,
  AEECM_NPI_RFU        = 0xFF,
  AEECM_NPI_BIG        = 0x10000000
    /* Force 32-bit enums */

} AEECMNumberPlan;

/* ECT State */
typedef enum
{
  AEECM_ECT_ALERTING,
  AEECM_ECT_ACTIVE,
  AEECM_ECT_BIG = 0x10000000
    /* Force 32-bit enums */

} AEECMECTState;

/* Presentation indicator */
typedef enum {
  AEECM_PI_ALLOW = 0x00,         /* Presentation allowed */
  AEECM_PI_RES,           /* Presentation restricted */
  AEECM_PI_NOT_AVAIL,      /* Number not available */
  AEECM_PI_BIG = 0x10000000
    /* Force 32-bit enums */

} AEECMPI;

/* Screening indicator */
typedef enum {
  AEECM_SI_USER_NOT_SCREENED, /* User-provided, not screened */
  AEECM_SI_USER_PASS,         /* User-provided, verified and passed */
  AEECM_SI_USER_FAIL,         /* User-provided, verified and failed */
  AEECM_SI_NETWORK,            /* Network-provided */
  AEECM_SI_BIG = 0x10000000
    /* Force 32-bit enums */

} AEECMSI;

/* Service Options */
typedef enum {
  AEECM_SO_REJ=0x0000,
  AEECM_SO_VOICE_IS96A=0x0001,
  AEECM_SO_LOOPBACK=0x0002,
  AEECM_SO_VOICE_EVRC=0x0003,
  AEECM_SO_ASYNC_DATA_PRE707=0x0004,
  AEECM_SO_G3_FAX_PRE707=0x0005,
  AEECM_SO_SMS=0x0006,
  AEECM_SO_PPP_PKT_DATA_PRE707=0x0007,
  AEECM_SO_LOOPBACK_13K=0x0009,
  AEECM_SO_ASYNC_DATA_13K_PRE707=0x000C,
  AEECM_SO_G3_FAX_13K_PRE707=0x000D,
  AEECM_SO_RS2_SMS=0x000E,
  AEECM_SO_PPP_PKT_DATA_13K_PRE707=0x000F,
  AEECM_SO_VOICE_13K_IS733=0X0011,
  AEECM_SO_RS1_OTAPA=0x0012,
  AEECM_SO_RS2_OTAPA=0x0013,
  AEECM_SO_ANALOG_FAX_RS1=0x0014,
  AEECM_SO_ANALOG_FAX_RS2=0x0015,
  AEECM_SO_MDR_PKT_DATA_FRS1_RRS1=0x0016,
  AEECM_SO_MDR_PKT_DATA_FRS1_RRS2=0x0017,
  AEECM_SO_MDR_PKT_DATA_FRS2_RRS1=0x0018,
  AEECM_SO_MDR_PKT_DATA_FRS2_RRS2=0x0019,
  AEECM_SO_FULL_TDSO=0x0020,
  AEECM_SO_PPP_PKT_DATA_3G=0x0021,
  AEECM_SO_G3_FAX_13K_PROPTRY=0x8022,
  AEECM_SO_RS1_PD=0x0023,
  AEECM_SO_RS2_PD=0x0024,
  AEECM_SO_MARKOV_SO54=0x0036,
  AEECM_SO_LOOPBACK_SO55=0x0037,
  AEECM_SO_VOICE_SMV=0x0038,
  AEECM_SO_ASYNC_DATA_IS707=0x1004,
  AEECM_SO_G3_FAX_IS707=0x1005,
  AEECM_SO_PPP_PKT_DATA_IS707=0x1007,
  AEECM_SO_VOICE_13K=0x8000,
  AEECM_SO_VOICE_IS96=0x8001,
  AEECM_SO_MARKOV=0x8002,
  AEECM_SO_DATA=0x8003,
  AEECM_SO_TDSO=0x8008,
  AEECM_SO_MARKOV_13K=0x801C,
  AEECM_SO_WLL_OFF_HOOK=0x801D,
  AEECM_SO_RS1_MARKOV=0x801E,
  AEECM_SO_RS2_MARKOV=0x801F,
  AEECM_SO_PPP_PKT_DATA_13K_PROPTRY=0x8020,
  AEECM_SO_ASYNC_DATA_13K_PROPTRY=0x8021,
  AEECM_SO_MDR_DPT_FRS1_RRS1=0x8024,
  AEECM_SO_MDR_DPT_FRS2_RRS2=0x8025,
  AEECM_SO_HDR_PKT_DATA=0x80FF,
  AEECM_SO_NULL=0xFFFF,

  AEECM_SO_BIG = 0x10000000
    /* Force 32-bit enums */

} AEECMServiceOption;

/* Additional reason for ending/rejecting GSM/WCDMA/CDMA call */
typedef enum {

  AEECM_END_UNAVAILABLE = 0x0000,

  /* GSM End Reason */
  AEECM_GSMEND_UNASSIGNED_CAUSE = 0x0101,
  AEECM_GSMEND_NO_ROUTE_TO_DEST = 0x0103,
  AEECM_GSMEND_CHANNEL_UNACCEPTABLE = 0x0106,
  AEECM_GSMEND_OPERATOR_DETERMINED_BARRING = 0x0108,
  AEECM_GSMEND_NORMAL_CALL_CLEARING = 0x0110,
  AEECM_GSMEND_USER_BUSY,
  AEECM_GSMEND_NO_USER_RESPONDING,
  AEECM_GSMEND_USER_ALERTING_NO_ANSWER,
  AEECM_GSMEND_CALL_REJECTED = 0x0115,
  AEECM_GSMEND_NUMBER_CHANGED,
  AEECM_GSMEND_NON_SELECTED_USER_CLEARING = 0x011A,
  AEECM_GSMEND_DESTINATION_OUT_OF_ORDER,
  AEECM_GSMEND_INVALID_NUMBER_FORMAT,
  AEECM_GSMEND_FACILITY_REJECTED,
  AEECM_GSMEND_RESPONSE_TO_STATUS_ENQUIRY,
  AEECM_GSMEND_NORMAL_UNSPECIFIED,
  AEECM_GSMEND_NO_CIRCUIT_CHANNEL_AVAILABLE = 0x0122,
  AEECM_GSMEND_NETWORK_OUT_OF_ORDER = 0x0126,
  AEECM_GSMEND_TEMPORARY_FAILURE = 0x0129,
  AEECM_GSMEND_SWITCHING_EQUIPMENT_CONGESTION,
  AEECM_GSMEND_ACCESS_INFORMATION_DISCARDED,
  AEECM_GSMEND_REQUESTED_CIRCUIT_CHANNEL_NOT_AVAILABLE,
  AEECM_GSMEND_RESOURCES_UNAVAILABLE_UNSPECIFIED = 0x012F,
  AEECM_GSMEND_QUALITY_OF_SERVICE_UNAVAILABLE = 0x0131,
  AEECM_GSMEND_REQUESTED_FACILITY_NOT_SUBSCRIBED,
  AEECM_GSMEND_INCOMING_CALL_BARRED_WITHIN_CUG = 0x0137,
  AEECM_GSMEND_BEARER_CAPABILITY_NOT_AUTHORISED = 0x0139,
  AEECM_GSMEND_BEARER_CAPABILITY_NOT_PRESENTLY_AVAILABLE,
  AEECM_GSMEND_SERVICE_OR_OPTION_NOT_AVAILABLE = 0x013F,
  AEECM_GSMEND_BEARER_SERVICE_NOT_IMPLEMENTED = 0x0141,
  AEECM_GSMEND_ACM_GEQ_ACMMAX = 0x0144,
  AEECM_GSMEND_REQUESTED_FACILITY_NOT_IMPLEMENTED,
  AEECM_GSMEND_ONLY_RESTRICTED_DIGITAL_INFO_BC_AVAILABLE,
  AEECM_GSMEND_SERVICE_OR_OPTION_NOT_IMPLEMENTED = 0x014F,
  AEECM_GSMEND_INVALID_TRANSACTION_ID_VALUE = 0x0151,
  AEECM_GSMEND_USER_NOT_MEMBER_OF_CUG = 0x0157,
  AEECM_GSMEND_INCOMPATIBLE_DESTINATION,
  AEECM_GSMEND_INVALID_TRANSIT_NETWORK_SELECTION = 0x015B,
  AEECM_GSMEND_SEMANTICALLY_INCORRECT_MESSAGE = 0x015F,
  AEECM_GSMEND_INVALID_MANDATORY_INFORMATION,
  AEECM_GSMEND_MESSAGE_TYPE_NON_EXISTENT,
  AEECM_GSMEND_MESSAGE_TYPE_NOT_COMPATIBLE_WITH_PROT_STATE,
  AEECM_GSMEND_IE_NON_EXISTENT_OR_NOT_IMPLEMENTED,
  AEECM_GSMEND_CONDITIONAL_IE_ERROR,
  AEECM_GSMEND_MESSAGE_NOT_COMPATIBLE_WITH_PROTOCOL_STATE,
  AEECM_GSMEND_RECOVERY_ON_TIMER_EXPIRY,
  AEECM_GSMEND_PROTOCOL_ERROR_UNSPECIFIED = 0x016F,
  AEECM_GSMEND_INTERWORKING_UNSPECIFIED = 0x017F,

  /* GSM Reject Reason */
  AEECM_GSMREJ_IMSI_UNKNOWN_IN_HLR = 0x0202,
  AEECM_GSMREJ_ILLEGAL_MS = 0x0203,
  AEECM_GSMREJ_IMSI_UNKNOWN_IN_VLR = 0x0204,
  AEECM_GSMREJ_IMEI_NOT_ACCEPTED = 0x0205,
  AEECM_GSMREJ_ILLEGAL_ME = 0x0206,
  AEECM_GSMREJ_GPRS_SERVICES_NOT_ALLOWED = 0x0207,
  AEECM_GSMREJ_GPRS_SERVICES_AND_NON_GPRS_SERVICES_NOT_ALLOWED = 0x0208,
  AEECM_GSMREJ_MS_IDENTITY_CANNOT_BE_DERIVED_BY_THE_NETWORK = 0x0209,
  AEECM_GSMREJ_IMPLICITLY_DETACHED = 0x020A,
  AEECM_GSMREJ_PLMN_NOT_ALLOWED = 0x020B,
  AEECM_GSMREJ_LA_NOT_ALLOWED = 0x020C,
  AEECM_GSMREJ_NATIONAL_ROAMING_NOT_ALLOWED = 0x020D,
  AEECM_GSMREJ_NETWORK_FAILURE = 0x0211,
  AEECM_GSMREJ_CONGESTTION = 0x0216,
  AEECM_GSMREJ_SERVICE_OPTION_NOT_SUPPORTED = 0x0220,
  AEECM_GSMREJ_REQ_SERV_OPT_NOT_SUBSCRIBED = 0x0221,
  AEECM_GSMREJ_SERVICE_OPT__OUT_OF_ORDER = 0x0222,
  AEECM_GSMREJ_CALL_CANNOT_BE_IDENTIFIED = 0x0226,
  AEECM_GSMREJ_NO_PDP_CONTEXT_ACTIVATED = 0x0228,
  AEECM_GSMREJ_RETRY_UPON_ENTRY_INTO_A_NEW_CELL_MIN_VALUE = 0x0230,
  AEECM_GSMREJ_RETRY_UPON_ENTRY_INTO_A_NEW_CELL_MAX_VALUE = 0x023F,
  AEECM_GSMREJ_SEMANTICALLY_INCORRECT_MSG = 0x025F,
  AEECM_GSMREJ_INVALID_MANDATORY_INFO = 0x0260,
  AEECM_GSMREJ_MESSAGE_TYPE_NON_EXISTANT = 0x0261,
  AEECM_GSMREJ_MESSAGE_TYPE_NOT_COMP_PRT_ST = 0x0262,
  AEECM_GSMREJ_IE_NON_EXISTANT = 0x0263,

  /* CDMA End Reason */
  AEECM_CDMA_REL_NORMAL = 0x0300,
  AEECM_CDMA_REL_PWDN,
  AEECM_CDMA_REL_SO,
  AEECM_CDMA_REL_SRV_INACT,

  AEECM_CDMA_BIG = 0x10000000
    /* Force 32-bit enums */

} AEECMEndCause;

/* CLIR Type */
typedef enum {
  AEECM_CLIR_DEFAULT,
  AEECM_CLIR_INVOCATION = 11,
  AEECM_CLIR_SUPPRESSION = 12,
  AEECM_CLIR_SUPPRESSION_REJECTED = 46,

  AEECM_CLIR_BIG = 0x10000000
    /* Force 32-bit enums */

} AEECMCLIR;

typedef enum
{
  AEECM_CLI_UNAVAILABLE,
  AEECM_CLI_REJECT_BY_USER,
  AEECM_CLI_INTERACTION_WITH_OTHER_SRVC,
  AEECM_CLI_COIN_PAY_PHONE,
  AEECM_CLI_BIG = 0x10000000

} AEECMNoCLICause;

/* (Extended) Display Type */
typedef enum {
  AEECM_DISP_NORMAL = 0x00,
  AEECM_DISP_BLANK = 0x80,
  AEECM_DISP_SKIP = 0x81,
  AEECM_DISP_CONTINUATION = 0x82,
  AEECM_DISP_CALLED_ADDR = 0x83,
  AEECM_DISP_CAUSE = 0x84,
  AEECM_DISP_PROGRESS_IND = 0x85,
  AEECM_DISP_NOTIF_IND = 0x86,
  AEECM_DISP_PROMPT = 0x87,
  AEECM_DISP_ACCUM_DIGITS = 0x88,
  AEECM_DISP_STATUS = 0x89,
  AEECM_DISP_INBAND = 0x8A,
  AEECM_DISP_CALLING_ADDR = 0x8B,
  AEECM_DISP_REASON = 0x8C,
  AEECM_DISP_CALLING_NAME = 0x8D,
  AEECM_DISP_CALLED_NAME = 0x8E,
  AEECM_DISP_ORIGINAL_NAME = 0x8F,
  AEECM_DISP_REDIRECTING_NAME = 0x90,
  AEECM_DISP_CONNECTED_NAME = 0x91,
  AEECM_DISP_ORIG_RESTRICT = 0x92,
  AEECM_DISP_DATE_AND_TIME = 0x93,
  AEECM_DISP_CALL_APPEARNC = 0x94,
  AEECM_DISP_FEATURE_ADDR = 0x95,
  AEECM_DISP_REDIRECTION_NAM = 0x96,
  AEECM_DISP_REDIRECTION_NUM = 0x97,
  AEECM_DISP_REDIRECTING_NUM = 0x98,
  AEECM_DISP_ORIG_CALLED_NUM = 0x99,
  AEECM_DISP_CONNECTED_NUM = 0x9A,
  AEECM_DISP_TEXT = 0x9E,
  AEECM_DISP_BIG = 0x10000000
    /* Force 32-bit enums */

} AEECMDispType;

typedef enum {

  AEECM_RECALL_RESP_CCBS_NONE = -1,
  AEECM_RECALL_RESP_CCBS_ACCEPT,
  AEECM_RECALL_RESP_CCBS_REJECT,
  AEECM_RECALL_RESP_CCBS_HOLD_ACCEPT,
  AEECM_RECALL_RESP_CCBS_REL_ACCEPT,
  AEECM_RECALL_RESP_CCBS_BIG  = 0x10000000

} AEECMRecallRspType;

/* Used by SetSubscriptionStatusEx to indicate desired mode. */
typedef enum {
  AEECM_SUB_NO_CHANGE = 0,     /* Whatever it is, leave it alone. */
  AEECM_SUB_AVAILABLE = 1,     /* We want it to be available. */
  AEECM_SUB_NOT_AVAILABLE = 2  /* We want it to be not available. */
} AEECMSUB;


/*===========================================================================

                    NEW TYPE DECLARATIONS

===========================================================================*/

typedef uint8 AEECMCallID;


/* Sub Address */
typedef struct
{
  boolean   extension_bit;
    /* The extension bit */

  uint8     odd_even_ind;
    /* The indicator of even/odd bits - EVEN/ODD */

  AECHAR    addr[AEECM_MAX_SUBADDRESS_DIGITS_LENGTH];
    /* NULL terminated Array of subaddress digits */

} AEECMSubAddress;

/* Type for CM number.
** Able to store a dialed number or a caller ID number.
**
*/
typedef struct
{

  AECHAR                   buf[AEECM_MAX_DIAL_STRING];
    /* NULL Terminated Number buffer */

  AEECMNumberType          number_type;
    /* Number type */

  AEECMNumberPlan          number_plan;
    /* Numbering Plan */

  AEECMPI                  pi;
    /* Presentation Indicator */

  AEECMSI                  si;
    /* Screening Indicator */

  AEECMSubAddress          subaddr;
    /* Subaddress */

} AEECMNumber;

/* Explicit Call Tranfer info */
typedef struct
{
  boolean                   present;
    /* TRUE if ECT info present */

  AEECMECTState             ect_state;
    /* Explicit call transfer state */

} AEECMECTInfo;

/* Closed user group index */
typedef struct
{
   boolean                         present;
   byte                            msb;
   byte                            lsb;
} AEECMCugIndex;

/* Forward CUG info */
/* NOTE: If this structure grows, assign a new call option type
   to work with IWebOpts */
typedef struct
{
   boolean                         present;
   AEECMCugIndex                   cm_cug_index;
   boolean                         cm_suppress_pref_cug;
   boolean                         cm_suppress_outgoing_access;
} AEECMFwdCugInfo;

/*---------------------------------------------------------------------------
 * AEECM Call Progress Information
 * From mn_cm_exp.h
 ---------------------------------------------------------------------------*/
typedef enum {
  AEECM_PROGIND_USER_SUSPENDED = 0,
  AEECM_PROGIND_USER_RESUMED,
  AEECM_PROGIND_BEARER_CHANGED,
  AEECM_PROGIND_REMOTE_ALERT,
  AEECM_PROGIND_BEARER_RELEASED,
  AEECM_PROGIND_IP_REMOTE_ALERT,
  AEECM_PROGIND_IP_INFO_NONE,
  AEECM_PROGIND_BIG = 0x10000000
} AEECMCallProgress;

/*---------------------------------------------------------------------------
 * AEECM Data Call Information
 ---------------------------------------------------------------------------*/
typedef struct {
  uint32                  rx_total_bytes;
  /* Total number of bytes received in a data call */

  uint32                  rx_max_rate;
  /* Maximum receive rate possible (Unit of 100bps)
   * ie rx_max_rate of 96 means 9600bps or 9.6 Kbps */

  uint32                  tx_total_bytes;
  /* Total number of bytes transmitted in a data call */

  uint32                  tx_max_rate;
  /* Maximum transmit rate possible (Unit of 100bps)
   * ie tx_max_rate of 96 means 9600bps or 9.6 Kbps */
} AEECMDataCallStats;


typedef enum{
  AEECM_ALS_LINE_NONE =-1,
  AEECM_ALS_LINE_01, /* Line #1 */
  AEECM_ALS_LINE_02, /* Line #2 */
  AEECM_ALS_LINE_BIG = 0x10000000
} AEECMAlsLine;


typedef enum {
  AEECM_ALS_LINE_SWITCHING_NONE = -1,
  AEECM_ALS_LINE_SWITCHING_DISABLED, /* User is not allowed to switch between lines */
  AEECM_ALS_LINE_SWITCHING_ENABLED,  /* User is allowed to switch between lines */
  AEECM_ALS_LINE_SWITCHING_BIG = 0x10000000
} AEECMAlsLineSwitching;

/*---------------------------------------------------------------------------
 * AEE CM Call Information
 * Information about a call (includes technology dependent fields)
 ---------------------------------------------------------------------------*/
typedef struct {

  AEECMCallState          call_state;
  /* Call state - idle, orig, incom, conv */

  AEECMCallType           call_type;
  /* Call type - VOICE, DATA, OTASP, etc */

  AEECMSysMode            sys_mode;
  /* The system's mode. */

  AEECMCallModeInfo       call_mode_info;
  /* The call Mode*/

  AEECMCallDirection      direction;
  /* Direction of the call */

  uint32                  start_time;
  /* Start time of the call (milliseconds since device was powered on) */

  uint32                  duration;
  /* Duration of call (ms), meaningful only after call has ended */

  AECHAR                  alpha[AEECM_MAX_ALPHA_TAG_LENGTH];
  /* Last alpha numeric information received */

  AECHAR                  other_party_no[AEECM_MAX_DIAL_STRING];
  /* Other Party number; NULL Terminated */

  AECHAR                  other_party_subaddr[AEECM_MAX_SUBADDRESS_DIGITS_LENGTH];
  /* Other Party subaddress */

  AEECMNumber             redirect_party_number;
  /* Redirecting party number */

  AEECMFwdCugInfo         forward_cug_info;
  /* Closed user Group information */

  AEECMCLIR               cm_clir;
  /* CLIR information - CLIR suppression / invocation etc. */

  AEECMNoCLICause         cause_of_no_cli;
  /* Cause of no Caller line identification */

  AEECMServiceOption      srv_opt;
  /* Service option as defined in CAI.H file */

  AEECMPrivacyPref        privacy_pref;
  /* User privacy preference */

  boolean                 is_privacy;
  /* Indicate whether privacy mode is on */

  AEECMPI                 pi;
  /* Presentation Indicator */

  boolean                 alert_ans_bypass;
  /* Indicate whether the incoming call need to be answered by the clients */

  AECHAR                  forwarded_from[AEECM_MAX_DIAL_STRING];
  /* The number forwarded from, if applicable; NULL Terminated */

  AECHAR                  forwarded_to[AEECM_MAX_NUM_FWDING][AEECM_MAX_DIAL_STRING];
  /* List of numbers forwarded to for a call originating from this MS; NULL Terminated */

  AEECMCallProgress       call_progress;
  /* call origination progress information */

  AEECMDataCallStats      data_stats;
  /* Data Call Statistics */

  AEECMAlsLine             m_alsLine;   /* Current Line*/


  boolean                  bTTYcall;  /* Is a TTY call */

  boolean                  bAlphaRecvd; /* Is cnap alpha string received by OEMCM_HandleCallAlphaUpdate*/
  
  boolean                 is_last_cdma_info_rec;    /* this field indicates whether this is the last  ** CDMA info record or not */
} AEECMCallInfo;

/*---------------------------------------------------------------------------
 * AEE CM WLAN Preferences
 *--------------------------------------------------------------------------*/
typedef enum {
  AEECM_WLAN_ENC_NONE           = 0,
  AEECM_WLAN_ENC_WEP_64BIT,
  AEECM_WLAN_ENC_WEP_128BIT,
  AEECM_WLAN_ENC_TKIP,
  AEECM_WLAN_ENC_AES
} AEECMWLANEncType;

typedef enum {
  AEECM_WLAN_AUTH_NONE           = 0,
  AEECM_WLAN_AUTH_WEP,
  AEECM_WLAN_AUTH_8021X_PASSWORD,
  AEECM_WLAN_AUTH_8021X_TLS_RADIUS,
  AEECM_WLAN_AUTH_8021X_MD5,
  AEECM_WLAN_AUTH_8021X_TLS_RADIUS_WKEY,
  AEECM_WLAN_AUTH_8021X_TLS_CERT,
  AEECM_WLAN_AUTH_PAP,
  AEECM_WLAN_AUTH_CHAP,
  AEECM_WLAN_AUTH_MSCHAPV2,
  AEECM_WLAN_AUTH_PEAP,
  AEECM_WLAN_AUTH_TTLS
} AEECMWLANAuthType;                                             

/* WEP Keys are binary.  WEP-64 uses 40-bit keys, which
 * amounts to 5 bytes (40 bits / (8 bits/byte) = 5)
 */
#define AEECM_64BIT_WEP_KEY_SIZE 5
typedef struct {
  uint8      default_key;
  uint8      key1[AEECM_64BIT_WEP_KEY_SIZE];
  uint8      key2[AEECM_64BIT_WEP_KEY_SIZE];
  uint8      key3[AEECM_64BIT_WEP_KEY_SIZE];
  uint8      key4[AEECM_64BIT_WEP_KEY_SIZE];
} AEECMWLANWEP64AuthInfo;

/* WEP Keys are binary.  WEP-128 uses 104-bit keys, which
 * amounts to 13 bytes (104 bits / (8 bits/byte) = 13)
 */
#define AEECM_128BIT_WEP_KEY_SIZE 13
typedef struct {
  uint8      default_key;
  uint8      key1[AEECM_128BIT_WEP_KEY_SIZE];
  uint8      key2[AEECM_128BIT_WEP_KEY_SIZE];
  uint8      key3[AEECM_128BIT_WEP_KEY_SIZE];
  uint8      key4[AEECM_128BIT_WEP_KEY_SIZE];
} AEECMWLANWEP128AuthInfo;

#define AEECM_8021X_USER_ID_STR_SIZE 72
#define AEECM_8021X_PASSWD_STR_SIZE 63
typedef struct {
  uint8      pass_len;
  char       pass[AEECM_8021X_PASSWD_STR_SIZE];
} AEECMWLAN8021XPASSAuthInfo;

typedef struct {
  uint8      id_len;
  char       id[AEECM_8021X_USER_ID_STR_SIZE];
} AEECMWLANTLSAuthInfo;

typedef struct {
  uint8      id_len;
  char       id[AEECM_8021X_USER_ID_STR_SIZE];
  uint8      cert_id;
} AEECMWLANTLSCertAuthInfo;

typedef struct {
  uint8      id_len;
  char       id[AEECM_8021X_USER_ID_STR_SIZE];
  uint8      pass_len;
  char       pass[AEECM_8021X_PASSWD_STR_SIZE];
} AEECMWLANMD5AuthInfo;

typedef union {
  AEECMWLANWEP64AuthInfo      wep64_info;
  AEECMWLANWEP128AuthInfo     wep128_info;
  AEECMWLAN8021XPASSAuthInfo  passwd_info;
  AEECMWLANTLSCertAuthInfo    tls_cert_info;
  AEECMWLANMD5AuthInfo        md5_info;
} AEECMWLANAuthInfo;

typedef struct {

  AEECMNwSelModePref                    network_sel_mode_pref;
    /* WLAN selection is automatic or manual */

  AEECMWLANScanPref                      scan_mode;
  /* Active/Passive scan mode */

  AEECMWLAN_BSS_ID                         bss_id;
    /* If WLAN network selection is set to manual, this specifies BSS */
    /* if BSS Id is unknown, then set bss_id to 0 */

  AEECMWLAN_SSID                           ssid;
    /* If WLAN network selection is set to manual, this specifies SSID */
    /* if BSS Id is unknown, then set ssid length to 0 */

  AEECMWLANChannel                       chan;
   /* If WLAN network selection is set to manual, this specifies Channel */
   /* If WLAN channel is unkown, then set channel to 0*/

  AEECMWLANTechPref                      tech_pref;
   /* Specify 802.11 a/b/g etc. here. */

  AEECMWLAN_BSS_TYPE                     bss_type;
    
  AEECMWLAN_BSS_TYPE_PREF                bss_type_pref;

  AEECMWLANEncType                       encrypt;

  AEECMWLANAuthType                      auth;

  uint32                                 mob_ip_profile_id;

  AEECMWLANAuthInfo                      auth_info;

  AEECMBandPref                          band;
    /* Note that, when a band pref is used in this structure,
     * only one bit should be set.
     */

} AEECMWLANPref;


/* structure to store ccbs indexes*/
typedef struct {

    uint8              ccbs_index;
    /* ccbs index value stored  */

     AECHAR            forwarded_from[AEECM_MAX_DIAL_STRING];
  /* The number forwarded from, if applicable called party number for the corresp call */
  /* NULL Terminated */

} AEECMCCBSIndexStore;


/* Type for ccbs information returned to clients
*/
typedef struct {

    int                  ccbs_store_len;
      /* Total entries present in ccbs store
      */

    AEECMCCBSIndexStore   ccbs_indx_store[AEECM_MAX_CCBS_REQ];
      /* Carries ccbs index and called number
      */
} AEECMCCBSStoreInfo;


/*---------------------------------------------------------------------------
 * AEE CM Phone Information
 *--------------------------------------------------------------------------*/
typedef struct {
  boolean                          is_in_use;
  /* Indicate whether phone is currently in use */

  AEECMOprtMode                    oprt_mode;
  /* Indicate current operating mode */

  AEECMTestControlType             test_control_type;
  /* Indicate current test control type */

  AEECMCDMALockMode                cdma_lock_mode;
  /* Indicate whether phone is CDMA locked until power cycle */

  AEECMAnsVoicePref                answer_voice;
  /* Indicate whether phone is CDMA locked until power cycle */

  AEECMModePref                    mode_pref;
  /* Indicate current mode preference */

  AEECMPrefTerm                    pref_term;
  /* Indicate the current preference term - one-call, power-cycle, etc.*/

  AEECMAcqOrderPref                acq_order_pref;
  /* The acquisition order preferred by the client. GSM/WCDMA only. */

  AEECMServDomPref                 srv_domain_pref;
  /* The service domain preferred by the client. */

  AEECMBandPref                    band_pref;
   /* Indicate current band preference */

  AEECMRoamPref                    roam_pref;
  /* Indicate current roam preference */

  AEECMNAM                         nam_sel;
  /* Indicate the NAM selection. CDMA only */

  AEECMNAM                         curr_nam;
  /* Indicate the current NAM -
  ** needed when NAM selection is set to AUTO-NAM. CDMA only */

  AEECMHybrPref                    hybr_pref;
  /* Indicate hybrid preference */

  AEECMNwSelModePref               network_sel_mode_pref;
  /* The network selection mode selected by the client. GSM/WCDMA only */

  AEECMPLMNID                      plmn;
  /* The identifier of the PLMN to be selected manually. GSM/WCDMA only */

  AEECMWLANBSSINFO                 wlan_bss_info;
    /* The identifier of the BSS to be selected manually. WLAN only */

  AEECMUserPLMNList                user_pref_networks;
  /* The user preferred network list. GSM/WCDMA only */

  AEECMDetailedPLMNList            available_networks;
  /* Available networks. GSM/WCDMA only */

  AEECMWLANBSSInfoList             wlan_bss_list;
    /* Available WLAN BSS List */

  boolean                          is_cdma_subscription_available;
  /* Indicate whether the cdma subscription/provisioning information
  ** is available for consumption.                              */

  boolean                          is_gw_subscription_available;
  /* Indicate whether the GSM/WCDMA subscription/provisioning information
   ** is available for consumption.                              */

  boolean                          is_wlan_subscription_available;
    /* Indicate whether the WLAN subscription/provisioning information
    ** is available for consumption.                              */

  AEECMRTREConfig                  rtre_config;
  /* Current RTRE configuration - RUIM, NV, RUIM fallback NV. CDMA only */

  AEECMRTREControl                 rtre_control;
  /* Current RTRE control - RUIM or NV. CDMA only */

  AEECMAOCInfo                     aoc_info;
  /* Advice of charge info */

  AEECMDDTMPref                    ddtm_pref;
  /* Dedicated Data Transmission Mode */

  AEECMAlsLine             m_alsLine;   /* Current Line*/

  AEECMAlsLineSwitching    m_alsLineSwitch; /* is Switching allowed*/

  AEECMCCBSStoreInfo       m_ccbs;
  
  AEECMWLANPref            m_wlan_pref;

} AEECMPhInfo;



/*---------------------------------------------------------------------------
 * AEE CM Serving System Information
 *--------------------------------------------------------------------------*/
typedef struct {
  AEECMSrvStatus            srv_status;
  /* The service status (SERVICE/NO SERVICE) of the system. */

  AEECMSrvDomain            srv_domain;
  /* The service domain registered on the system. */

  AEECMSrvCapability        srv_capability;
  /* The system's service capability.*/

  AEECMSysMode              sys_mode;
  /* The system's mode. */

  AEECMRoamStatus           roam_status;
  /* Indicate current roaming status */

  AEECMSysID                sys_id;
  /* The ID of the selected system. */

  uint16                    rssi;
    /* RSSI in positive dBm i.e. 75 means -75dBm. The range for
    ** AMPS:      -110 to -89 dBm
    ** 800 CDMA:  -105 to -90 dBm
    ** 1900 CDMA: -108 to -93 dBm
    ** GSM/WCDMA: -105 to -60 dBm
    */

  int16                      rssi2;
    /* The -ve RSSI value, coming from RRC layer */

  int16                      ecio;

  boolean                    ecio_for_display; 
    /* TRUE if UE is in Data call */

  boolean                   hdr_hybrid;
    /* system is hybrid or not. FEATURE_HDR_HYBRID */

  AEECMSrvStatus            hdr_srv_status;
     /* Indicate current service state - FEATURE_HDR_HYBRID */

  AEECMRoamStatus           hdr_roam_status;
     /* Indicate current hybrid hdr roaming status - FEATURE_HDR_HYBRID */

  uint16                    hdr_rssi;
    /* HDR RSSI in positive dBm i.e. 75 means -75dBm - FEATURE_HDR_HYBRID
    ** The range is -105 to -90 dBm */

  uint8                     hdr_sinr;
  /* SINR range: from 0 to 8, Level 8 represents highest SINR*/

  uint16                    wlan_rssi;
    /* WLAN RSSI in positive dBm i.e. 75 means -75dBm - FEATURE_HDR_HYBRID
    ** The range is -105 to -90 dBm */  

  AEECMSIMState             sim_state;
    /* Current state of the SIM */

  boolean                   plmn_forbidden;
  /* Indicates whether the PLMN is forbidden */

  AEECMMMInfo               mm_information;
  /* Mobility management information, i.e. network name, date,
  ** time and time zone of registered network */

  AEECMSysBandClass         band_class;
  /* cellular or PCS */

  AEECMSysBlocks            block_or_system;
   /* PCS frequency block: A to F Cellular System A or B*/

  boolean                   is_registered;
  /* is mobile registered on the current system */

  int32                     uz_id;
  /* User zone Identification of the selected user zone */

  AEECMUZName               uz_name;
  /* User zone name of the selected user zone. */

  AEECMWLANStats            wlan_stats;

  AEECMSrvStatus            wlan_srv_status;

  AEECMWLANBSSINFO          wlan_bss_info;

  AEECMCellSrvInd           cell_srv_ind;
  /* Structure to hold information about services in current cell. */

  AEECMBCMCSSrvStatus       bcmcs_hdr_srv_status;
  /* Specify whether BCMCS service is available on HDR. */
    
  AEECMBCMCSSrvStatus       bcmcs_main_srv_status;
  /* Specify whether BCMCS service is available on MAIN. */
  
  AEECMActiveProtType       hdr_active_prot; 
  /* The HDR active protocol revision information */
} AEECMSSInfo;

/*---------------------------------------------------------------------------
 * Info passed with each Call Event
 ---------------------------------------------------------------------------*/
typedef struct {

  AEECMCallID             call_id;
  /* Call Id of the call */

  AEECMNumber             number;
  /* Event specific number info */

  AECHAR                  alpha[AEECM_MAX_ALPHA_TAG_LENGTH];
  /* Event specific Alpha numeric tag */

  AEECMDispType           disp_type;
  /* display type for display info record */

  AEECMSignal             signal;
  /* Alert signal information to be played for the incoming call */

  AEECMEndStatus          end_status;
  /* Call end status */

  boolean                 missed;
  /* TRUE, if call was not connected. False, otherwise */

  AEECMEndCause           end_additional_info;
  /* Additional Info about end/rejection of call
     (both WCDMA/GSM and CDMA) */

  AEECMECTInfo            ect_info;
  /* State of Explicit Call Transfer */

  AEECMCallError          call_error;
  /* Call error associated with CALL_ERROR event */

  AEESuppSvcSSErrorType   callops_error;
  /* Supplementary services error */

  AEECMCallInfo           call_info;
  /* Call Information */

} AEECMCallEventData;


/*---------------------------------------------------------------------------
 * Info passed with each Phone Event
 ---------------------------------------------------------------------------*/
typedef AEECMPhInfo AEECMPhEventData; /* Same as Phone Information */

/* AEECMEventData changed fields mask.
 * The changed field mask is sent along with Event Data on an SS event.
 * It denotes the fields in AEECMSSInfo structure that has changed for
 * this event.
 */

#define AEECM_SS_SRV_STATUS_MASK          ( (uint32) (1<<0)  )
  /* Serving Status - No, limited, full */

#define AEECM_SS_SRV_DOMAIN_MASK          ( (uint32) (1<<1)  )
  /* Service Domain */

#define AEECM_SS_SRV_CAPABILITY_MASK      ( (uint32) (1<<2)  )
  /* System/s service capability */

#define AEECM_SS_SYS_MODE_MASK            ( (uint32) (1<<3) )
  /* System mode */

#define AEECM_SS_ROAM_STATUS_MASK         ( (uint32) (1<<4) )
  /* Roaming status */

#define AEECM_SS_SYS_ID_MASK              ( (uint32) (1<<5) )
  /* System ID */

#define AEECM_SS_SIM_STATE_MASK           ( (uint32) (1<<6) )

#define AEECM_SS_PLMN_FORBIDDEN_MASK      ( (uint32) (1<<7) )
  /* Forbidden PLMN/System */

#define AEECM_SS_MOBILITY_MGMT_MASK       ( (uint32) (1<<8) )
  /* Mobility Management */

#define AEECM_SS_SRV_IND_MASK             ( (uint32) (1<<9) )

#define AEECM_SS_UZ_MASK                  ( (uint32) (1<<10) )

#define AEECM_SS_HDR_SRV_STATUS_MASK      ( (uint32) (1<<11) )

#define AEECM_SS_HDR_ROAM_STATUS_MASK     ( (uint32) (1<<12) )

  /* Service indicators: This mask indicates a change of service occurred.
  ** It is set even if the service is the same as the previous service. */

/*---------------------------------------------------------------------------
 * Info passed with each Serving System Event
 ---------------------------------------------------------------------------*/
typedef struct {

  uint32                  changed_fields;
  /* Bit mask indicating which of the serving system fields
  ** were changed since last notification. */

  AEECMSSInfo             ss_info;
  /* Serving System information */
} AEECMSSEventData;

/*---------------------------------------------------------------------------
 * Info passed with each Inband Event
 ---------------------------------------------------------------------------*/
typedef struct {

  AEECMCallID   call_id;
  /* call ID */

  uint8         on_length;
  /* DTMF pulse width */

  uint8         off_length;
  /* DTMF inter-digit interval */

  AECHAR        digits[AEECM_INBAND_MAX_DIGITS_LENGTH];
  /* DTMF digit buffer */

} AEECMInbandEventData;


#define AEECM_MBMS_NUM_MAX_SRVS_PER_REQ 2

/* Maximum number of MBMS services
** that can simultaneously exist.
*/
#define AEECM_MBMS_MAX_SERVICES 16

/* TMGI structure in int format */
typedef struct
{ 

  /* TRUE  -  TMGI is present
  ** FALSE -  TMGI is not present
  */
  boolean present;

  /* TMGI in uint64 format */
  uint64  tmgi;

}AEECMMBMSTmgi;

/*
** An enum that defines all of the address types supported - used to
** discriminate the union below
** (same as ip_addr_type from ps_ip_addr.h)
*/
typedef enum
{
  AEECM_IP_ANY_ADDR     = 0,
  AEECM_IPV4_ADDR       = 4,
  AEECM_IPV6_ADDR       = 6,
  AEECM_IP_ADDR_INVALID = 255

} AEECMIpAddrType;

#define AEECM_IP_ADDR_V6_LENGTH 2

/* IP address
*/
typedef struct AEECMIpAddress
{

  AEECMIpAddrType type;

  union {

    uint32 v4;
    uint64 v6[AEECM_IP_ADDR_V6_LENGTH];
  } addr;

} AEECMIpAddress;

/* MBMS report type */
typedef enum 
{
  /* FOR INTERNAL USE ONLY!
  */
  AEECM_MBMS_RPT_NONE = -1,

  /* MBMS Service Activate Cnf 
  */
  AEECM_MBMS_RPT_ACTIVATE_CNF ,

  /* MBMS Service Deactivate Cnf  
  */
  AEECM_MBMS_RPT_DEACTIVATE_CNF ,

  /* MBMS Service Ind (change in status)
  */
  AEECM_MBMS_RPT_SERVICE_IND ,

  /* MBMS Capability RPTication - from RRC, NAS 
  */
  AEECM_MBMS_RPT_CAPABILITY_IND ,

  /* MBMS Context Act Ind - from NAS after it received TMGI for a service 
  */
  AEECM_MBMS_RPT_CONTEXT_ACTIVATED_IND ,

  /* MBMS Context Deact Ind (MBMS context deactivation, PDP context 
  ** deactivation, etc)  - from NAS 
  */
  AEECM_MBMS_RPT_CONTEXT_DEACTIVATED_IND ,

  /* MBMS Abort cnf
  */
  AEECM_MBMS_RPT_ABORT_CNF,

  /* MBMS Service Suspend Cnf 
  */
  AEECM_MBMS_RPT_SUSPEND_CNF ,

  /* MBMS Service Resume Cnf 
  */
  AEECM_MBMS_RPT_RESUME_CNF ,

  /* MBMS Available Services List Cnf 
  */
  AEECM_MBMS_RPT_AVAIL_SERVICES_LIST_CNF ,

   /* MBMS Available Services List Cnf 
  */
  AEECM_MBMS_RPT_LL_READY_IND,

  /* MBMS Rpt Set Attribute
  */
  AEECM_MBMS_RPT_SET_ATTR ,

  /* FOR INTERNAL USE ONLY!
  */
  AEECM_MBMS_RPT_MAX

} AEECMMBMSRptType;

typedef enum
{

  AEECM_MBMS_SRV_STATUS_NONE = -1,
    /* For internal use only */

  AEECM_MBMS_SRV_STATUS_UNAVAILABLE_NETWORK_FAILURE,
    /* MBMS service in unavailable due to network failure */

  AEECM_MBMS_SRV_STATUS_UNAVAILABLE_SETUP_FAILURE,
    /* MBMS service in unavailable due to setup failure on UE side */

  AEECM_MBMS_SRV_STATUS_UNAVAILABLE_FAILURE_OTHER,
    /* MBMS service in unavailable due to some other failure */

  AEECM_MBMS_SRV_STATUS_AVAILABLE,
    /* MBMS service in available */

  AEECM_MBMS_SRV_STATUS_MONITORED,
    /* MBMS service monitored */

  AEECM_MBMS_SRV_STATUS_OTHER,
    /* Reserved for future use */
    
  AEECM_MBMS_SRV_STATUS_RESERVED1,
    /* Reserved for future use */
    
  AEECM_MBMS_SRV_STATUS_MAX
    /* For internal use only */

} AEECMMBMSSrvStatus;

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
  uint32 log_chan_id;

}AEECMMBMSLogChanId;

/* MBMS activate cnf
*/
typedef struct 
{
  /* TMGI of service */
  AEECMMBMSTmgi tmgi;

  /* Multicast IP*/
  AEECMIpAddress mip;

  /* Port number */
  uint32 port;

  /* Status of req - TRUE indicates success, FALSE indicates failure */
  boolean cnf_status;

  /* Service status */
  AEECMMBMSSrvStatus srv_status;

  /* Logical channel id that was added */
  AEECMMBMSLogChanId log_chan_id;

  /* Generic parameter */
  uint32 mbms_parm;        

}AEECMMBMSRptActivateCnfEntry;

/* Struct to hold parameters for MBMS activate conf report
*/
typedef struct 
{
  /* Number of services in the array */
  uint8 num_services;

  /* Services list */
  AEECMMBMSRptActivateCnfEntry services[AEECM_MBMS_NUM_MAX_SRVS_PER_REQ];

}AEECMMBMSRptActivateCnf;

typedef struct 
{
  /* TMGI of service */
  AEECMMBMSTmgi tmgi;

  /* Multicast IP*/
  AEECMIpAddress mip;

  /* Port number */
  uint32 port;

  /* Status of req - TRUE indicates success, FALSE indicates failure */
  boolean cnf_status;

  /* Service status */
  AEECMMBMSSrvStatus srv_status;

  /* Logical channel id that was added */
  AEECMMBMSLogChanId log_chan_id;

  /* Generic parameter */
  uint32 mbms_parm;

}AEECMMBMSRptDeactivateCnfEntry;

/* Struct to hold parameters for MBMS deactivate cnf
*/
typedef struct 
{
  /* Number of services in the array */
  uint8 num_services;

  /* Services list */
  AEECMMBMSRptDeactivateCnfEntry services[AEECM_MBMS_NUM_MAX_SRVS_PER_REQ];

}AEECMMBMSRptDeactivateCnf;

typedef struct 
{
  /* TMGI of service */
  AEECMMBMSTmgi tmgi;

  /* Multicast IP*/
  AEECMIpAddress mip;

  /* Port number */
  uint32 port;

  /* Logical channel id that was added/removed */
  AEECMMBMSLogChanId log_chan_id;

  /* Service status */
  AEECMMBMSSrvStatus srv_status;

  /* Generic parameter */
  uint32 mbms_parm;

}AEECMMBMSRptServiceIndEntry;

typedef struct 
{
  /* Number of services in the array */
  uint8 num_services;

  /* Services list */
  AEECMMBMSRptServiceIndEntry services[AEECM_MBMS_NUM_MAX_SRVS_PER_REQ];

}AEECMMBMSRptServiceInd;

/* Struct to hold parameters for capability indication
*/
typedef struct 
{
  /* Indication that core network is MBMS capable */
  boolean network_capability;

  /* Indication that cell is MBMS capable */
  boolean cell_capability;

  /* Generic parameter */
  uint32 mbms_parm;

}AEECMMBMSRptCapabilityInd;

/* Enum for MBMS context activation deactivation failure reason
*/
typedef enum
{
  AEECM_MBMS_CONTEXT_FAILURE_REASON_NONE = -1,
    /* For internal use only */

  AEECM_MBMS_CONTEXT_FAILURE_REASON_UNKNOWN,
    /* Unknown reason*/

  AEECM_MBMS_CONTEXT_FAILURE_NO_NETWORK_RESPONSE,
    /* No network response*/

  AEECM_MBMS_CONTEXT_FAILURE_INVALID_PDP_CONTEXT_STATE,
    /* Invalid PDP context state*/

  AEECM_MBMS_CONTEXT_FAILURE_PDP_CONTEXT_ERROR,
    /* PDP context error*/

  AEECM_MBMS_CONTEXT_FAILURE_APN_ERROR,
    /* APN error*/

  AEECM_MBMS_CONTEXT_FAILURE_INSUFFICIENT_RESOURCES,
    /* Insufficient resources*/

  AEECM_MBMS_CONTEXT_FAILURE_AUTHENTICATION_ERROR,
    /*  Authentication error*/

  AEECM_MBMS_CONTEXT_FAILURE_ACT_DEACT_REJECTED,
    /* Activate/Deactivate rejected */

  AEECM_MBMS_CONTEXT_FAILURE_DUPLICATE_MBMS,
    /* Duplicate MBMS*/

  AEECM_MBMS_CONTEXT_FAILURE_CONTEXT_SYNC_ERROR,
    /* Context sync error*/

  AEECM_MBMS_CONTEXT_FAILURE_REASON_OTHER,
    /* Other */

  AEECM_MBMS_CONTEXT_FAILURE_REASON_MAX
    /* For internal use only */
}AEECMMBMSContextFailureReason;

/* Struct to hold parameters for Context Activated Indication report
*/
typedef struct 
{
  /* TMGI reported by NAS for MIP specified below */
  AEECMMBMSTmgi tmgi;

  /* Multicast IP*/
  AEECMIpAddress mip;

  /* Port number */
  uint32 port;

  /* Context activation status. TRUE is successful, FALSE otherwise */
  boolean act_status;

  /* Failure reason of context activation req  if act_status is FALSE */
  AEECMMBMSContextFailureReason context_failure_reason;

}AEECMMBMSRptContextActivatedInd;

/* Struct to hold parameters for context deactivation
*/
typedef struct 
{
  /* TMGI of service */
  AEECMMBMSTmgi tmgi;

  /* Multicast IP*/
  AEECMIpAddress mip;

  /* Port number */
  uint32 port;

  /* Context de-activation status. TRUE is successful, FALSE otherwise */
  boolean deact_status;

  /* Failure reason of context deactivation req. Valid if deact_status is
  ** is FALSE
  */
  AEECMMBMSContextFailureReason context_failure_reason;

}AEECMMBMSRptContextDeactivatedInd;

/* MBMS Abort cnf
*/
typedef struct 
{
  /* TMGI of service */
  AEECMMBMSTmgi tmgi;

  /* Multicast IP*/
  AEECMIpAddress mip;

  /* Port number */
  uint32 port;

  /* Status of req - TRUE indicates success, FALSE indicates failure */
  boolean cnf_status;

}AEECMMBMSRptAbortCnfEntry;

/* Struct to hold parameters for MBMS Abort conf
*/
typedef struct 
{
  /* Number of services in the array */
  uint8 num_services;
 
  /* Services list */
  AEECMMBMSRptAbortCnfEntry services[AEECM_MBMS_NUM_MAX_SRVS_PER_REQ];

}AEECMMBMSRptAbortCnf;

/* MBMS suspend cnf
*/
typedef struct 
{
  /* TMGI of service */
  AEECMMBMSTmgi tmgi;

  /* Status of req - TRUE indicates success, FALSE indicates failure */
  boolean cnf_status;

  /* Generic parameter */
  uint32 mbms_parm;

}AEECMMBMSRptSuspendCnfEntry;

/* Struct to hold parameters for MBMS suspend cnf
*/
typedef struct 
{
  /* Number of services in the array */
  uint8 num_services;

  /* Services list */
  AEECMMBMSRptSuspendCnfEntry services[AEECM_MBMS_NUM_MAX_SRVS_PER_REQ];

}AEECMMBMSRptSuspendCnf;

/* MBMS resume cnf */
typedef struct 
{
  /* TMGI of service */
  AEECMMBMSTmgi tmgi;

  /* Status of req - TRUE indicates success, FALSE indicates failure */
  boolean cnf_status;

  /* Service status after resumption */
  AEECMMBMSSrvStatus srv_status;

  /* Generic parameter */
  uint32 mbms_parm;

}AEECMMBMSRptResumeCnfEntry;

/* Struct to hold parameters for MBMS resume cnf
*/
typedef struct 
{
  /* Number of services in the array */
  uint8 num_services;

  /* Services list */
  AEECMMBMSRptResumeCnfEntry services[AEECM_MBMS_NUM_MAX_SRVS_PER_REQ];

}AEECMMBMSRptResumeCnf;

/*
Struct to hold parameters for Available services list cnf
*/
typedef struct 
{
  /* Status of req - TRUE indicates success, FALSE indicates failure */
  boolean cnf_status;

  /* List of service ids that are available in the cell currently */
  uint32 service_id[AEECM_MBMS_MAX_SERVICES];

  /* Generic parameter */
  uint32 mbms_parm;

}AEECMMBMSRptAvailSrvListCnf;

typedef struct 
{
  /* Generic parameters */
  uint32 mbms_parm1;
  uint32 mbms_parm2;
  uint32 mbms_parm3;
  uint32 mbms_parm4;
  uint32 mbms_parm5;
  uint32 mbms_parm6;

}AEECMMBMSRptSetAttr;

/* Type to indicate the Lower Layer (RRC) is in a READY (IDLE) state to 
** service MBMS requests
*/
typedef struct 
{
  /* Generic parameters */
  uint32 mbms_parm1;

} AEECMMBMSRptLLReadyInd;

typedef struct AEECMMBMSGenericRptData
{
  /* MBMS report type */
  AEECMMBMSRptType   rpt_type;
  union{
    AEECMMBMSRptActivateCnf   activate_cnf;
    AEECMMBMSRptDeactivateCnf deactivate_cnf;
    AEECMMBMSRptServiceInd service_ind;
    AEECMMBMSRptCapabilityInd capability_ind;
    AEECMMBMSRptContextActivatedInd context_activated_ind;
    AEECMMBMSRptContextDeactivatedInd context_deactivated_ind;
    AEECMMBMSRptAbortCnf abort_cnf;
    AEECMMBMSRptSuspendCnf suspend_cnf;
    AEECMMBMSRptResumeCnf resume_cnf;
    AEECMMBMSRptAvailSrvListCnf avail_srv_list_cnf;
    AEECMMBMSRptLLReadyInd ll_ready_ind;
    AEECMMBMSRptSetAttr set_attr;
  }rpt;
} AEECMMBMSGenericRpt;

typedef enum 
{
  /* FOR INTERNAL USE ONLY!
  */
  AEECM_MBMS_CMD_NONE = -1,

  /* MBMS Service Activate command  
  */
  AEECM_MBMS_CMD_ACTIVATE_REQ,

  /* MBMS Service Deactivate command 
  */
  AEECM_MBMS_CMD_DEACTIVATE_REQ,

  /* MBMS Session Complete Ind 
  */
  AEECM_MBMS_CMD_SESSION_COMPLETE_IND,

  /* MBMS Abort Req  (to abort MBMS contex activation procedure in NAS)
  */
  AEECM_MBMS_CMD_ABORT_REQ,

  /* MBMS Service Suspend command  
  */
  AEECM_MBMS_CMD_SUSPEND_REQ,

  /* MBMS Service Resume command  
  */
  AEECM_MBMS_CMD_RESUME_REQ,

  /* MBMS Available Services List Request 
  */
  AEECM_MBMS_CMD_AVAIL_SERVICES_LIST_REQ,

  /* MBMS Capability enquiry
  */
  AEECM_MBMS_CMD_GET_CAPABILITY,

  /* MBMS Set Attribute 
  */
  AEECM_MBMS_CMD_SET_ATTR,

  /* FOR INTERNAL USE ONLY!
  */
  AEECM_MBMS_CMD_MAX

} AEECMMBMSCmdType;

/* MBMS service priority */
typedef struct
{
  /* TRUE  -  Priority is present
  ** FALSE -  Priority is not present
  */
  boolean present;

  /* Priority of MBMS service request */
  uint32   priority;

}AEECMMBMSSrvPriority;

/* MBMS mode of service */
typedef enum
{
  AEECM_MBMS_SRV_MODE_NONE = -1,
    /*For internal use only */

  AEECM_MBMS_SRV_MODE_UNKNOWN,
    /* Service mode unknown */

  AEECM_MBMS_SRV_MODE_BROADCAST,
    /* Service mode broadcast */

  AEECM_MBMS_SRV_MODE_MULTICAST,
    /* Service mode multicast */

  AEECM_MBMS_SRV_MODE_MAX
    /*For internal use only */

}AEECMMBMSSrvMode;

/* MBMS type of service */
typedef enum
{
  AEECM_MBMS_SRV_TYPE_NONE = -1,
    /* For internal use only */

  AEECM_MBMS_SRV_TYPE_UNKNOWN = 0,
    /* Service type unknown */

  AEECM_MBMS_SRV_TYPE_STREAMING,
    /* Service type streaming */

  AEECM_MBMS_SRV_TYPE_DOWNLOAD,
    /* Service type download */

  AEECM_MBMS_SRV_TYPE_OTHER,
    /* Service type other*/

  AEECM_MBMS_SRV_TYPE_MAX
    /* For internal use only */

}AEECMMBMSSrvType;

/*
** Struct to hold parameters for MBMS activate request for each service
*/
typedef struct 
{
  /* TMGI of service */
  AEECMMBMSTmgi                     tmgi;

  /* Multicast IP*/
  AEECMIpAddress                    mip;

  /* Port number */
  uint32                            port;

  /* Service Priority */  
  AEECMMBMSSrvPriority              priority; 

  /* Service Mode */
  AEECMMBMSSrvMode                  srv_mode;

  /* Service Type */
  AEECMMBMSSrvType                  srv_type;

  /* Generic parameter */
  uint32                            mbms_parm;

}AEECMMBMSCmdActivateReqEntryParams;

typedef struct
{
  /* Number of services in the array */
  uint8  num_services;

  /* Services that need to be activated */
  AEECMMBMSCmdActivateReqEntryParams services[AEECM_MBMS_NUM_MAX_SRVS_PER_REQ];

}AEECMMBMSCmdActivateReqInfo;

/* Struct to hold MBMS deactivate req parameters
*/
typedef struct 
{
  /* TMGI of service */
  AEECMMBMSTmgi  tmgi;

  /* Generic parameter */
  uint32         mbms_parm;        

}AEECMMBMSCmdDeactivateReqEntryParams;

/* Struct to hold MBMS deactivate req parameters for "num_services" MBMS 
** services
*/
typedef struct 
{
  /* Number of services in the array. Setting this to 0xFF will deactivate 
  ** all active MBMS services 
  */
  uint8                                    num_services;

  /* Services that need to be de-activated */
  AEECMMBMSCmdDeactivateReqEntryParams services[AEECM_MBMS_NUM_MAX_SRVS_PER_REQ];

}AEECMMBMSCmdDeactivateReqInfo;

/* MBMS session id */
typedef struct
{
  /* TRUE  -  Session id is present
  ** FALSE -  Session id is not present
  */
  boolean present;

  /* Session id_type */
  uint32  session_id;

}AEECMMBMSSessionId;

/* Struct to hold parameters for MBMS session complete ind
*/
typedef struct
{
  /* TMGI of service */
  AEECMMBMSTmgi        tmgi;

  /* Completed Session id */
  AEECMMBMSSessionId  session_id;

  /* Generic parameter */
  uint32              mbms_parm;
      
}AEECMMBMSCmdSessionCompleteIndInfo;

/* MBMS type of service */
typedef enum
{
  AEECM_MBMS_ABORT_NONE = -1,
    /* For internal use only */

  AEECM_MBMS_ABORT_ALL,
    /* Abort all */

  AEECM_MBMS_ABORT_CONTEXT_ACT,
    /* Abort context activation */

  AEECM_MBMS_ABORT_CONTEXT_DEACT,
    /* Abort context deactivation */

  AEECM_MBMS_ABORT_TRAFFIC_ACT,
    /* Abort traffic activation*/

  AEECM_MBMS_ABORT_TRAFFIC_DEACT,
    /* Abort traffic deactivation */

  AEECM_MBMS_ABORT_MAX
    /* For internal use only */
}AEECMMBMSAbortType;

/* Struct to hold parameters for MBMS abort req
*/
typedef struct
{
  /* TMGI of service */
  AEECMMBMSTmgi         tmgi;

  /* Multicast IP */
  AEECMIpAddress        mip;

  /* Port number */
  uint32                port;

  /* Abort type */
  AEECMMBMSAbortType    abort_type; 

  /* Generic parameter */
  uint32                mbms_parm;
      
}AEECMMBMSCmdAbortReqEntryParams;

/* To hold Abort req parameters for "num_services" MBMS services.
*/
typedef struct 
{
  /* Number of services in the array. Setting this to 0xFF will abort 
  ** all active MBMS services 
  */
  uint8                                num_services;

  /* Services list */
  AEECMMBMSCmdAbortReqEntryParams      services[AEECM_MBMS_NUM_MAX_SRVS_PER_REQ];
      
}AEECMMBMSCmdAbortReqInfo;

/* MBMS suspend request
*/
typedef struct 
{
  /* TMGI of service */
  AEECMMBMSTmgi      tmgi;

  /* Generic parameter */
  uint32             mbms_parm;
      
}AEECMMBMSCmdSuspendReqEntryParams;

/* Struct to hold parameters for MBMS suspend req cmd.
*/
typedef struct 
{
  /* Number of services in the array. Setting this to 0xFF will suspend 
  ** all active MBMS services 
  */
  uint8                                num_services;

  /* Services list */
  AEECMMBMSCmdSuspendReqEntryParams    services[AEECM_MBMS_NUM_MAX_SRVS_PER_REQ];
      
}AEECMMBMSCmdSuspendReqInfo;

/* MBMS resume request
*/
typedef struct 
{
  /* TMGI of service */
  AEECMMBMSTmgi        tmgi;

  /* Generic parameter */
  uint32                           mbms_parm;

}AEECMMBMSCmdResumeReqEntryParams;

/* Struct to hold parameters for MBMS resume req cmd.
*/
typedef struct 
{
  /* Number of services in the array. Setting this to 0xFF will resume 
  ** all suspended MBMS services 
  */
  uint8                                num_services;

  /* Services list */
  AEECMMBMSCmdResumeReqEntryParams     services[AEECM_MBMS_NUM_MAX_SRVS_PER_REQ];
      
}AEECMMBMSCmdResumeReqInfo;

/* Struct to hold parameters for MBMS available services list request command
*/
typedef struct 
{
  /* Placeholder for future use */
  uint32 mbms_parm;

}AEECMMBMSCmdAvailSrvListReqInfo;

/* Struct for clients to query MBMS capability
*/
typedef struct 
{
  /* Placeholder for future use */
  uint32 mbms_parm;

} AEECMMBMSCmdCapabilityReqInfo;

/* Generic MBMS command
*/
typedef struct 
{
  /* Generic parameters */
  uint32 mbms_parm1;
  uint32 mbms_parm2;
  uint32 mbms_parm3;
  uint32 mbms_parm4;
  uint32 mbms_parm5;
  uint32 mbms_parm6;

}AEECMMBMSCmdSetAttrInfo;

typedef struct {
   /* MBMS command being sent by the client */
   AEECMMBMSCmdType   cmd_type;
   union {
     AEECMMBMSCmdActivateReqInfo             activate_req;
     AEECMMBMSCmdDeactivateReqInfo           deactivate_req;
     AEECMMBMSCmdSessionCompleteIndInfo      session_complete_ind;
     AEECMMBMSCmdAbortReqInfo                abort_req;
     AEECMMBMSCmdSuspendReqInfo              suspend_req;
     AEECMMBMSCmdResumeReqInfo               resume_req;
     AEECMMBMSCmdAvailSrvListReqInfo         avail_srv_list_req;
     AEECMMBMSCmdCapabilityReqInfo           capability_req;
     AEECMMBMSCmdSetAttrInfo                 set_attr;
   }cmd;
} AEECMMBMSCmd;

typedef struct {

  AEECMMBMSGenericRpt mbms_generic_rpt;
    /* MBMS generic report structure */

  AEECMMBMSCmd        mbms_generic_cmd;
    /* MBMS cmd info gets copied to clients when mbms cmd is received */

  int requesting_client_id;
    /* Requesting client */

}AEECMMBMSEventData;


/*---------------------------------------------------------------------------
 * Info passed with each Event
 ---------------------------------------------------------------------------*/
typedef union {

  AEECMCallEventData    call;
  /* call specific data */

  AEECMPhEventData      ph;
  /* Phone specific Data */

  AEECMSSEventData      ss;
  /* Serving System specific data */

  AEECMInbandEventData  inb;
  /* Inband specific data */

  AEECMMBMSEventData    mbms;

} AEECMEventData;

/* Notification data provided along with every event. */
typedef struct {
  AEECMEvent        event;
  AEECMEventData    event_data;
} AEECMNotifyInfo;

/* See data structure documentation */
typedef WebOpt CallOpt;

/* Supplementary service type for ICM_SendFlashSupps */
typedef enum {
   AEECM_SENDFLASH_CALL_FORWARD,
   AEECM_SENDFLASH_EMERGENCY_CALL,
   AEECM_SENDFLASH_ACT_ANSWER_HOLD,
   AEECM_SENDFLASH_DEACT_ANSWER_HOLD,
   AEECM_SENDFLASH_SIMPLE_FLASH
} AEECMSendFlashSuppsType;

/*===========================================================================

                       PRL Related

===========================================================================*/
/* This enumerated type lists the types of acquisition records
*/
typedef enum
{
  AEECM_PRL_ACQ_REC_RESERVED,                 /* Reserved */
  AEECM_PRL_ACQ_REC_CELLULAR_ANALOG,          /* Cellular analog */
  AEECM_PRL_ACQ_REC_CELLULAR_CDMA_STD,
  AEECM_PRL_ACQ_REC_CELLULAR_CDMA_CUST,
  AEECM_PRL_ACQ_REC_CELLULAR_GENERIC,         /* Cellular generic */
  AEECM_PRL_ACQ_REC_PCS_CDMA_BLOCKS,          /* PCS CDMA using blocks */
  AEECM_PRL_ACQ_REC_PCS_CDMA_CHANNELS,        /* PCS CDMA using channels */

  /* The following three ACQ REC types apply for IS683B PRL or above */
  AEECM_PRL_ACQ_REC_JTACS_CDMA_STD,           /* JTACS CDMA standard channels */
  AEECM_PRL_ACQ_REC_JTACS_CDMA_CUST,          /* JTACS CDMA custom channels */
  AEECM_PRL_ACQ_REC_CDMA_2GHZ_CHANS,          /* 2G BAND CDMA using channels */

  /* The following three ACQ REC types apply for IS683C PRL or above */
  AEECM_PRL_ACQ_REC_CDMA_GENERIC,             /* CDMA Generic */
  AEECM_PRL_ACQ_REC_HDR_GENERIC,              /* HDR Generic */

  /* The following three ACQ REC types apply for IS683D PRL or above */
  AEECM_PRL_ACQ_REC_GSM_GENERIC,              /* GSM Generic*/
  AEECM_PRL_ACQ_REC_WCDMA_GENERIC,            /* WCDMA Generic*/
  AEECM_PRL_ACQ_REC_GW_PREFERRED,             /* GSM/WCDMA perferred */

  AEECM_PRL_ACQ_REC_NONE
} AEECMPrlAcqRecordType;

/* This enumerated type defines the values that the a_b field can contain
*/
typedef enum
{
  AEECM_PRL_AB_SYSTEM_A,            /* System A */
  AEECM_PRL_AB_SYSTEM_B,            /* System B */
  AEECM_PRL_AB_RESERVED,            /* Reserved */
  AEECM_PRL_AB_SYSTEM_A_OR_B        /* System A or system B */
} AEECMPrlABType;

/* This enumerated type defines the values that the pri_sec field can contain
*/
typedef enum
{
  AEECM_PRL_PRISEC_RESERVED=0,           /* Reserved */
  AEECM_PRL_PRISEC_PRIMARY=1,            /* Primary Channel Only */
  AEECM_PRL_PRISEC_SECONDARY=2,          /* Secondary Channel Only */
  AEECM_PRL_PRISEC_PRI_OR_SEC=3          /* Primary or Secondary Channel */
} AEECMPrlPriSecType;

/* The cellular analog system acquisition record
*/
typedef struct
{
  AEECMPrlABType a_b;                      /* System A/B selection type */
} AEECMPrlCellularAnalogType;

/* The cellular CDMA system acquisition record
*/
typedef struct
{
  AEECMPrlABType a_b;                      /* System A/B selection type */
  AEECMPrlPriSecType pri_sec;             /* Primary/secondary CDMA channel */
} AEECMPrlCellularCdmaType;              /* selection type */

typedef struct
{
  int num_chans;                        /* Number of channels */
  word chan[32];                        /* Channels */
} AEECMPrlCellularCdmaCustomType;

/* The cellular generic system acquisition record
*/
typedef struct
{
  AEECMPrlABType a_b;                      /* System A/B selection type */
} AEECMPrlCellularGenericType;

/* The PCS CDMA system acquisition (using blocks) record
*/
typedef struct
{
  int num_blocks;                       /* Number of blocks */
  word block[7];                        /* Blocks */
} AEECMPrlPcsCdmaBlockType;

/* The PCS CDMA system acquisition (using channels) record
*/
typedef struct
{
  int num_chans;                        /* Number of channels */
  word chan[32];                        /* Channels */
} AEECMPrlCdmaChanType;

/* The JTACS CDMA system acquisition record (standard channel)
*/
typedef struct
{
  AEECMPrlABType a_b;                      /* System A/B selection type */
  AEECMPrlPriSecType pri_sec;             /* Primary/secondary CDMA channel */
} AEECMPrlJtacsCdmaStdType;              /* selection type */

/* The JTACS CDMA system acquisition record (customer channels)
*/
typedef struct
{
  int num_chans;                        /* Number of channels */
  word chan[32];                        /* Channels */
} AEECMPrlJtacsCdmaCustType;

/* The 2G BAND CDMA system acquisition record (using channels)
*/
typedef struct
{
  int num_chans;                        /* Number of channels */
  word chan[32];                        /* Channels */
} AEECMPrlCdma2ghzChansType;

/* The CDMA generic acquisition record
*/
typedef struct
{
  int   num_recs;                         /* Number of records */
  word  rec[128];                         /* Packed records - 5bit band-class
                                          ** plus 11bit Channel number */
} AEECMPrlCdmaGenericType;

/* The HDR generic acquisition record
*/
typedef struct
{
  int   num_recs;                         /* Number of records */
  word  rec[128];                         /* Packed records - 5bit band-class
                                          ** plus 11bit Channel number */
} AEECMPrlHdrGenericType;

/* The GSM generic acquisition record
*/
typedef struct
{
  int num_bands;                          /* Number of bands */
  byte band[32];                          /* Bands - 5bit band-class*/

} AEECMPrlGsmGenericType;

/* This enumerated type defines the values the UARFCN_INCL field in the ACQ
** Record
*/
typedef enum
{
  AEECM_PRL_UARFCN_NOT_INCL=0,              /* UARFCN Not included in ACQ Record */
  AEECM_PRL_UARFCN_INCL=1,                  /* UARFCN Included in ACQ Record */
  AEECM_PRL_UARFCN_INCL_RESERVED=2
} AEECMPrlUarfcnInclType;

/* The WCDMA generic acquisition record
*/
typedef struct
{
  AEECMPrlUarfcnInclType uarfcn_incl;     /* UARFCN include */
  byte band;                              /* Bands -  5bit band-class */
  word uarfcn;                            /* Chans - 14bit UARFCN */
} AEECMPrlWcdmaGenericRecType;

typedef struct
{
  int num_bands;                          /* Number of bands */
  AEECMPrlWcdmaGenericRecType rec[32];   /* WCDMA generic record */
} AEECMPrlWcdmaGenericType;

/* This enumerated type defines the values the GSM/WCDMA preferred system
** indicator in the ACQ Record
*/
typedef enum
{
  AEECM_PRL_GW_PREF_SYS_IND_GSM=0,          /* GSM system preferred */
  AEECM_PRL_GW_PREF_SYS_IND_WCDMA=1,        /* WCDMA system preferred */
  AEECM_PRL_GW_PREF_SYS_IND_RESERVED=2
} AEECMPrlGwPrefSysIndType;

/* The GSM/WCDMA perferred acquisition record
*/
typedef struct
{
  AEECMPrlGwPrefSysIndType pref_sys_ind;   /* Preferred system indicator */
  int num_gsm_bands;                         /* Number of GSM bands */
  int num_wcdma_bands;                       /* Number of WCDMA bands */
  byte gsm_band[32];                         /* GSM bands - 5bit band-class*/
  AEECMPrlWcdmaGenericRecType wcdma_rec[32]; /* WCDMA generic record */
}AEECMPrlGwPrefType;

/* Enumeration of sd network selection mode.
**
*/
typedef enum {

    AEECM_SD_NET_SELECT_MODE_LIMITED,   /* PRL
                                  **    plmn select mode: Any
                                  **
                                  ** CMREG
                                  **   Sys network selection type: limited
                                  */

    AEECM_SD_NET_SELECT_MODE_AUTO,      /* PRL
                                  **    plmn select mode: allowed
                                  **
                                  ** CMREG
                                  **    Sys network selection type: Auto
                                  */

    AEECM_SD_NET_SELECT_MODE_HPLMN,     /* PRL
                                  **    plmn select mode: home plmn
                                  **
                                  ** CMREG
                                  **    Sys network selection type: manual
                                  **    manual plmn select type: hplmn
                                  */

    AEECM_SD_NET_SELECT_MODE_PRL_SYS,   /* PRL
                                  **    with specified plmn id
                                  **
                                  ** CMREG
                                  **    Sys net selection type: manual plmn
                                  **                    network select mode
                                  **    manual plmn select type: user mode
                                  */

    AEECM_SD_NET_SELECT_MODE_MANUAL_SYS,/* MANUAL SYS
                                  **   with specified plmn id
                                  **
                                  ** CMREG
                                  **    Sys network selection type is manual.
                                  **    is reselection flag: true
                                  */

    AEECM_SD_NET_SELECT_MODE_PRIV_SYS,  /* Private system provided by user for
                                  ** automatic selection
                                  */

    AEECM_SD_NET_SELECT_MODE_MANUAL_BSS_ID,
                                  /* Manual sys, in which only BSS Id is
                                  ** provided.
                                  */

    AEECM_SD_NET_SELECT_MODE_MAX

} AEECMSdNetSelectMode;

/* PLMN based system specific fields.
*/
typedef struct
{
  AEECMPLMNID               plmn_id;             /* PLMN ID use */

  word                      lac;                 /* LAC ID array*/

  /* Combined plmn_lac_incl field, plmn select mode (plmn lac not incl), and
  ** network selection mode (plmn included)
  */
  AEECMSdNetSelectMode net_select_mode;     /* network selection mode */

  byte                      num_lac;             /* Number of lac */

  word                      lac_a[16];           /* lac array */
} AEECMPrlPlmnSysType;

/* This structure defines the acquisition record type
*/
typedef struct
{
  AEECMPrlAcqRecordType acq_type;
  boolean          is_valid;
  union
  {
    AEECMPrlCellularAnalogType       cellular_analog;
    AEECMPrlCellularCdmaType         cellular_cdma;
    AEECMPrlCellularCdmaCustomType  cellular_cdma_cust;
    AEECMPrlCellularGenericType      cellular_generic;
    AEECMPrlPcsCdmaBlockType        pcs_cdma_block;
    AEECMPrlCdmaChanType         pcs_cdma_chan;

    /* The following three acq record types apply for IS683B PRL or above.*/
    AEECMPrlJtacsCdmaStdType        jtacs_cdma_std;
    AEECMPrlJtacsCdmaCustType       jtacs_cdma_cust;
    AEECMPrlCdma2ghzChansType       cdma_2ghz_chans;

    /* The following two acq record types apply for IS683C PRL or above. */
    AEECMPrlCdmaGenericType          cdma_generic;
    AEECMPrlHdrGenericType           hdr_generic;

    /* The following two acq record types apply for IS683D PRL or above. */
    AEECMPrlGsmGenericType           gsm_generic;
    AEECMPrlWcdmaGenericType         wcdma_generic;
    AEECMPrlGwPrefType               gw_pref;

  } type;
  AEECMPrlPlmnSysType plmn;          /* PLMN information from system record
                                    ** only valide for gsm/wcdma related acq
                                    ** record
                                    */

} AEECMPrlAcqRecord;


/*===========================================================================

                       ICM Interface

===========================================================================*/

typedef struct ICM ICM;
typedef struct ICMNotifier ICMNotifier;
typedef struct ICMModel ICMModel;
typedef struct ICallOpts ICallOpts;

AEEINTERFACE(ICMNotifier)
{
  INHERIT_INotifier(ICMNotifier);
};

AEEINTERFACE(ICM)
{
  INHERIT_IQueryInterface(ICM);

  /* Call Functions */
  int (*OriginateVoiceCall) (ICM *po, AECHAR const *number,
                             AEECMCallID *call_id);
  int (*OriginateCall)      (ICM *po, AEECMCallType call_type,
                             AECHAR const *number, AECHAR const *alpha,
                             ICallOpts * options, AEECMCallID  *call_id);
#ifdef FEATURE_GSTK_FDN_CC_SUPPORT							 
  int (*OriginateCall_SkipFDNChk)(ICM *po, AEECMCallType call_type,
                             AECHAR const *number, AECHAR const *alpha,
                             ICallOpts * options, AEECMCallID  *call_id);
#endif							 
  int (*AnswerCall)         (ICM *po, AEECMCallID call_id);
  int (*EndCall)            (ICM *po, AEECMCallID call_id);
  int (*EndAllCalls)        (ICM *po);
  uint16 (*GetActiveCallIDs)(ICM *po, AEECMCallType call_type,
                             AEECMCallState call_state,
                             AEECMCallID *call_ids, uint32 call_ids_size);
  int (*GetCallInfo)        (ICM *po, AEECMCallID call_id,
                             AEECMCallInfo *call_info, uint32 size);
  int (*SetPrivacyPref)     (ICM *po, AEECMPrivacyPref privacy_pref);
  AEECMIsCallOps (*CallOpsStringCheck) (ICM *po, AECHAR *dialed_string,
                             uint8 *call_num);
  int (*CallOpsRequest)     (ICM *po, AECHAR *dialed_string, AEECMCallID call_id);

  /* Phone Functions */
  int (*SetSystemPreference) (ICM *po, AEECMModePref mode_pref,
                             AEECMPrefTerm pref_term, uint32 pref_duration,
                             AEECMAcqOrderPref acq_order_pref,
                             AEECMBandPref band_pref, AEECMRoamPref roam_pref,
                             AEECMHybrPref hybr_pref,
                             AEECMServDomPref srv_domain_pref,
                             AEECMNwSelModePref nw_sel_mode_pref,
                             AEECMPLMNID * plmn,
                             AEECMWLANPref * wlan_ptr,
                             AEECMPhError *status,
                             AEECallback *cb);
  int (*SetNAMSel)          (ICM *po, AEECMNAM nam_sel);
  int (*SetOperatingMode)   (ICM *po, AEECMOprtMode oprt_mode);
  int (*GetPhoneInfo)       (ICM *po, AEECMPhInfo *info, uint32 size);
  int (*SetSubscriptionStatus) (ICM *po, AEECMSysMode mode,
                             boolean status);
  int (*SetRSSIDeltaThreshold) (ICM *po, uint8 delta);
  int (*GetNetworks)        (ICM *po, AEECMNetworkType list_type,
                             AEECMPhError *status,
                             void * list,
                             uint32 list_size,
                             AEECallback *cb);
  int (*SetPrefNetworks)    (ICM *po,
                             AEECMUserPLMNList  *pref_networks_ptr,
                             AEECMPhError *status,
                             AEECallback *cb);
  int (*WakeupFromStandby)  (ICM *po);
  int (*SetRTREConfig)   (ICM *po, AEECMRTREConfig rtre_config);
  int (*SetAnswerVoicePref) (ICM *po, AEECMAnsVoicePref answer_voice,
                                 uint32  duration);
  int (*AvoidServingSystem) (ICM *po, AEECMAvoidSSPref avoid_type,
                                 uint32 avoid_time);
  int (*ResetACM)           (ICM *po);
  int (*SetACMMax)          (ICM *po, uint32 new_max);
  int (*SetDDTMPref)        (ICM *po, AEECMDDTMPref ddtm_pref);

  /* Serving System Function */
  int (*GetSSInfo)          (ICM *po, AEECMSSInfo *ss_info, uint32 size);

  /* Inband functions */
  int (*BurstDTMF)          (ICM *po, AEECMCallID call_id,
                             AEECMDTMFOnLength on_length,
                             AEECMDTMFOffLength off_length,
                             AECHAR *digits);
  int (*StartContDTMF)      (ICM *po, AEECMCallID call_id, AECHAR digit);
  int (*StopContDTMF)       (ICM *po, AEECMCallID call_id);

  /* Helper functions */
  boolean (*IsValidModeBandPref) (ICM *po, AEECMModePref mode_pref,
                                     AEECMBandPref band_pref);
  AEECMSysMode (*GetSysMode)     (ICM *po);
  boolean (*IsMyCall)            (ICM *po, AEECMCallID call_id);
  int (*SelectLine)              (ICM *po, AEECMAlsLine selectLine,AEECMPhError *status,AEECallback *cb);
  int (*SetLineSwitching)        (ICM *po, AEECMAlsLineSwitching lineSwitching, AEECMPhError *status,AEECallback *cb);
  int (*GetPLMNFromIMSI)         (ICM *po, AEECMPLMNID* pPLMN, byte* imsi);
  boolean (*SysMatchPLMN)        (ICM *po, AEECMPLMNID PLMNa, AEECMPLMNID PLMNb);
  int (*RecallRsp)               (ICM *po, AEECMRecallRspType recallRsp, AEECMCallID callID);
  int (*ResetOrigThrottleTbl)    (ICM *po);
  int (*SetSubscriptionStatusEx) (ICM *po, AEECMSUB cdma, AEECMSUB gw, AEECMSUB wlan);
  int (*SendFlashSupps)          (ICM *po, AEECMSendFlashSuppsType suppsType, AEECMCallID callId);
  /* PRL Related */
  int (*PrlExtractAcqRecord)     (ICM *po, AEECMPrlAcqRecord *pAcqRec, int index);
  int (*PrlUpdateAcqRecord)      (ICM *po, const AEECMPrlAcqRecord *pAcqRec, int index);
  int (*PrlCommit)               (ICM *po);
  int (*SendMBMSCommand)         (ICM *po, AEECMMBMSCmd *mbms_cmd);
  
};

AEEINTERFACE(ICallOpts)
{
  INHERIT_IQueryInterface(ICallOpts);

  int (*AddOpt)(ICallOpts *po, CallOpt *pOpt);
  int (*RemoveOpt)(ICallOpts *po, int32 nOptId);
  int (*GetOpt)(ICallOpts *po, int32 nOptId, CallOpt *pOpt);
};

#define ICMNotifier_AddRef(p)                                                 \
                AEEGETPVTBL(p,ICMNotifier)->AddRef(p)
#define ICMNotifier_Release(p)                                                \
                AEEGETPVTBL(p,ICMNotifier)->Release(p)
#define ICMNotifier_SetMask(p, mask)                                          \
                AEEGETPVTBL(p,ICMNotifier)->SetMask(p, mask)

#define ICM_AddRef(p)                                                         \
                AEEGETPVTBL(p,ICM)->AddRef(p)
#define ICM_Release(p)                                                        \
                AEEGETPVTBL(p,ICM)->Release(p)
#define ICM_QueryInterface(p, clsid, pp)                                      \
                AEEGETPVTBL(p,ICM)->QueryInterface(p, clsid, pp)

#define ICM_OriginateVoiceCall(p, num, call_id)                               \
                AEEGETPVTBL(p,ICM)->OriginateVoiceCall(p, num, call_id)
#define ICM_OriginateCall(p, type, num, alpha, opt, cid)                      \
                AEEGETPVTBL(p,ICM)->OriginateCall(p, type, num, alpha, opt, cid)
#define ICM_OriginateCall_SkipFDNChk(p, type, num, alpha, opt, cid)           \
                AEEGETPVTBL(p,ICM)->OriginateCall_SkipFDNChk(p, type, num, alpha, opt, cid)
#define ICM_AnswerCall(p, cid)                                                \
                AEEGETPVTBL(p,ICM)->AnswerCall(p, cid)
#define ICM_EndCall(p, cid)                                                   \
                AEEGETPVTBL(p,ICM)->EndCall(p, cid)
#define ICM_EndAllCalls(p)                                                    \
                AEEGETPVTBL(p,ICM)->EndAllCalls(p)
#define ICM_GetActiveCallIDs(p, type, state, pcid, size)                      \
                AEEGETPVTBL(p,ICM)->GetActiveCallIDs(p, type, state, pcid, size)
#define ICM_GetCallInfo(p, cid, pcinfo, size)                                 \
                AEEGETPVTBL(p,ICM)->GetCallInfo(p, cid, pcinfo, size)
#define ICM_SetPrivacyPref(p, pref)                                           \
                AEEGETPVTBL(p,ICM)->SetPrivacyPref(p, pref)
#define ICM_CallOpsStringCheck(p, str, pn)                                    \
                AEEGETPVTBL(p,ICM)->CallOpsStringCheck(p, str, pn)
#define ICM_CallOpsRequest(p, str, cid)                                       \
                AEEGETPVTBL(p,ICM)->CallOpsRequest(p, str, cid)


#define ICM_SetSystemPreference(p, mpref, term, dur, acqord, band, roam, hybr,\
                                dom, selmode, plmn, status, cb)               \
                AEEGETPVTBL(p,ICM)->SetSystemPreference(p, mpref,             \
                            term, dur, acqord, band, roam, hybr,              \
                            dom, selmode, plmn, NULL, status, cb)

#define ICM_SetSystemPreference_WLAN(p, mpref, term, dur, acqord, band, roam, hybr,\
                                dom, selmode, plmn, wlan, status, cb)               \
                AEEGETPVTBL(p,ICM)->SetSystemPreference(p, mpref,             \
                            term, dur, acqord, band, roam, hybr,              \
                            dom, selmode, plmn, wlan, status, cb)

#define ICM_SetNAMSel(p, nam)                                                 \
                AEEGETPVTBL(p,ICM)->SetNAMSel(p, nam)
#define ICM_SetOperatingMode(p, oprt_mode)                                    \
                AEEGETPVTBL(p,ICM)->SetOperatingMode(p, oprt_mode)
#define ICM_GetPhoneInfo(p, info, size)                                       \
                AEEGETPVTBL(p,ICM)->GetPhoneInfo(p, info, size)
#define ICM_SetSubscriptionStatus(p, mode, status)                            \
                AEEGETPVTBL(p,ICM)->SetSubscriptionStatus(p, mode, status)
#define ICM_SetRSSIDeltaThreshold(p, delta)                                   \
                AEEGETPVTBL(p,ICM)->SetRSSIDeltaThreshold(p, delta)
#define ICM_GetNetworks(p, t, status, list, size, cb)                         \
                AEEGETPVTBL(p,ICM)->GetNetworks(p, t, status, list, size, cb)
#define ICM_SetPrefNetworks(p, pref, status, cb)                              \
                AEEGETPVTBL(p,ICM)->SetPrefNetworks(p, pref, status, cb)
#define ICM_WakeupFromStandby(p)                                              \
                AEEGETPVTBL(p,ICM)->WakeupFromStandby(p)
#define ICM_SetRTREConfig(p, rtre)                                            \
                AEEGETPVTBL(p,ICM)->SetRTREConfig(p, rtre)
#define ICM_SetAnswerVoicePref(p, av, dur)                                    \
                AEEGETPVTBL(p,ICM)->SetAnswerVoicePref(p, av, dur)
#define ICM_AvoidServingSystem(p, at, type)                                   \
                AEEGETPVTBL(p,ICM)->AvoidServingSystem(p, at, type)
#define ICM_ResetACM(p)                                                       \
                AEEGETPVTBL(p,ICM)->ResetACM(p)
#define ICM_SetACMMax(p, max)                                                 \
                AEEGETPVTBL(p,ICM)->SetACMMax(p, max)
#define ICM_SetDDTMPref(p, pref)                                              \
                AEEGETPVTBL(p,ICM)->SetDDTMPref(p, pref)


#define ICM_GetSSInfo(p, info, size)                                          \
                AEEGETPVTBL(p,ICM)->GetSSInfo(p, info, size)


#define ICM_BurstDTMF(p, cid, on, off, digits)                                \
                AEEGETPVTBL(p,ICM)->BurstDTMF(p, cid, on, off, digits)
#define ICM_StartContDTMF(p, cid, digits)                                     \
                AEEGETPVTBL(p,ICM)->StartContDTMF(p, cid, digits)
#define ICM_StopContDTMF(p, cid)                                              \
                AEEGETPVTBL(p,ICM)->StopContDTMF(p, cid)

#define ICM_IsValidModeBandPref(p, mode, band)                                \
                AEEGETPVTBL(p,ICM)->IsValidModeBandPref(p, mode, band)
#define ICM_GetSysMode(p)                                                     \
                AEEGETPVTBL(p, ICM)->GetSysMode(p)
#define ICM_IsMyCall(p, cid)                                                  \
                AEEGETPVTBL(p, ICM)->IsMyCall(p, cid)
#define ICM_SelectLine(p,line, status, pcb)                                   \
                AEEGETPVTBL(p, ICM)->SelectLine(p, line, status, pcb)
#define ICM_SetLineSwitching(p, line, status, pcb)                            \
                AEEGETPVTBL(p, ICM)->SetLineSwitching(p,line, status, pcb)

#define ICM_GetPLMNFromIMSI(p, pPLMN, pImsi)                            \
                AEEGETPVTBL(p, ICM)->GetPLMNFromIMSI(p,pPLMN,pImsi)

#define ICM_SysMatchPLMN(p, PLMNa, PLMNb)                            \
                AEEGETPVTBL(p, ICM)->SysMatchPLMN(p,PLMNa,PLMNb)

#define ICM_RecallRsp(p, recall, callID)                            \
                AEEGETPVTBL(p, ICM)->RecallRsp(p, recall, callID)
#define ICM_ResetOrigThrottleTbl(p)                            \
                AEEGETPVTBL(p, ICM)->ResetOrigThrottleTbl(p)
                
#define ICallOpts_AddRef(p)                                                   \
                AEEGETPVTBL(p,ICallOpts)->AddRef(p)
#define ICallOpts_Release(p)                                                  \
                AEEGETPVTBL(p,ICallOpts)->Release(p)
#define ICallOpts_QueryInterface(p, clsid, ppo)                               \
                AEEGETPVTBL(p,ICallOpts)->QueryInterface(p, clsid, ppo)
#define ICallOpts_AddOpt(p, o)                                                \
                AEEGETPVTBL((p),ICallOpts)->AddOpt(p, o)
#define ICallOpts_RemoveOpt(p, i)                                             \
                AEEGETPVTBL(p,ICallOpts)->RemoveOpt(p, i)
#define ICallOpts_GetOpt(p, i, o)                                             \
                AEEGETPVTBL(p,ICallOpts)->GetOpt(p, i, o)
#define ICM_SetSubscriptionStatusEx(p, cdma_status, gw_status, wlan_status)  \
                AEEGETPVTBL(p,ICM)->SetSubscriptionStatusEx(p, cdma_status,          \
                                                    gw_status, wlan_status)

#define ICM_SendFlashSupps(p, suppsType, callId)                                     \
                AEEGETPVTBL(p, ICM)->SendFlashSupps(p, suppsType, callId)

/* PRL RELATED */
#define ICM_PrlExtractAcqRecord(p, pAcqRec, i)                               \
                AEEGETPVTBL(p, ICM)->PrlExtractAcqRecord(p, pAcqRec, i)

#define ICM_PrlUpdateAcqRecord(p, pAcqRec, i)                                \
                AEEGETPVTBL(p, ICM)->PrlUpdateAcqRecord(p, pAcqRec, i)

#define ICM_PrlCommit(p)                                                     \
                AEEGETPVTBL(p, ICM)->PrlCommit(p)

#define ICM_SendMBMSCommand(p, pcmd)                                         \
                AEEGETPVTBL(p,ICM)->SendMBMSCommand(p, pcmd)


/*===========================================================================

                       INTERFACES DOCUMENTATION

===========================================================================*/


/*---------------------------------------------------------------------------
                          CALL FUNCTIONS
---------------------------------------------------------------------------*/



/*=============================================================================
FUNCTION: ICM_OriginateVoiceCall

DESCRIPTION:
  Originate a Voice call with default options.

PROTOTYPE:
  int ICM_OriginateVoiceCall(ICM *po,
                        AECHAR const *number,
                        AEECMCallID  *call_id)

PARAMETERS:
  po                [in]: ICM interface pointer.
  number            [in]: Number to be dialed
  call_id          [out]: Call ID assigned to the call

RETURN VALUE:
  Returns AEE return values.

  AEE_SUCCESS:  If procedure was started correctly.
  EBADPARM:     If number is NULL and the resulting operation is not a flash.
  EFAILED:      Otherwise.

COMMENTS:
  When user dials a number the first check should be whether it is a call
  independent supplementary service string (ISUPPSVC_StringCheck).
  If that check fails, ICM_CallOpsStringCheck() should be called.

  If ICM_CallOpsStringCheck() returns AEECM_IS_CALLOPS or
  AEECM_IS_CALLOPS_NEED_CALLID, ICM_CallOpsRequest() should be called.
  Otherwise the dialed string should be used to Originate[Voice]Call().

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

/*=============================================================================
FUNCTION: ICM_OriginateCall

DESCRIPTION:
  Originate a call as specified by the call_type.

PROTOTYPE:
  int ICM_OriginateCall(ICM *po,
                        AEECMCallType call_type,
                        AECHAR const *number,
                        AECHAR const *alpha,
                        ICallOpts * options,
                        AEECMCallID  *call_id)

PARAMETERS:
  po                [in]: ICM interface pointer.
  call_type         [in]: Type of call to be originated
  number            [in]: Number to be dialed
  alpha             [in]: Text to be displayed along with origination
  options           [in]: Call specific options, if required
  call_id          [out]: Call ID assigned to the call

RETURN VALUE:
  Returns AEE return values.

  AEE_SUCCESS:  If procedure was started correctly.
  EBADPARM:     If number is NULL or call_type is not defined.
  EFAILED:      Otherwise.

COMMENTS:
  Input parameters alpha and options are optional and can be NULL.
  When options paramater is NULL, default ICM options are used to make the call.

  When user dials a number the first check should be whether it is a call
  independent supplementary service string (ISUPPSVC_StringCheck).
  If that check fails, ICM_CallOpsStringCheck() should be called.

  If ICM_CallOpsStringCheck() returns AEECM_IS_CALLOPS or
  AEECM_IS_CALLOPS_NEED_CALLID, ICM_CallOpsRequest() should be called.
  Otherwise the dialed string should be used to Originate[Voice]Call().

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

/*=============================================================================
FUNCTION: ICM_OriginateCall_SkipFDNChk

DESCRIPTION:
  Originate a call as specified by the call_type but skip FDN checking.

PROTOTYPE:
  int ICM_OriginateCall_SkipFDNChk(ICM *po,
                        AEECMCallType call_type,
                        AECHAR const *number,
                        AECHAR const *alpha,
                        ICallOpts * options,
                        AEECMCallID  *call_id)

PARAMETERS:
  po                [in]: ICM interface pointer.
  call_type         [in]: Type of call to be originated
  number            [in]: Number to be dialed
  alpha             [in]: Text to be displayed along with origination
  options           [in]: Call specific options, if required
  call_id          [out]: Call ID assigned to the call

RETURN VALUE:
  Returns AEE return values.

  AEE_SUCCESS:  If procedure was started correctly.
  EBADPARM:     If number is NULL or call_type is not defined.
  EFAILED:      Otherwise.

COMMENTS:
  Input parameters alpha and options are optional and can be NULL.
  When options paramater is NULL, default ICM options are used to make the call.

  When user dials a number the first check should be whether it is a call
  independent supplementary service string (ISUPPSVC_StringCheck).
  If that check fails, ICM_CallOpsStringCheck() should be called.

  If ICM_CallOpsStringCheck() returns AEECM_IS_CALLOPS or
  AEECM_IS_CALLOPS_NEED_CALLID, ICM_CallOpsRequest() should be called.
  Otherwise the dialed string should be used to Originate[Voice]Call().

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

/*=============================================================================
FUNCTION: ICM_AnswerCall

DESCRIPTION:
  Answer an incoming call.

PROTOTYPE:
  int ICM_AnswerCall(ICM *po,
                        AEECMCallID call_id)

PARAMETERS:
  po                [in]: ICM interface pointer.
  call_id           [in]: Call ID of the call to be answered

RETURN VALUE:
  Returns AEE return values.

  AEE_SUCCESS:  If procedure was started correctly.
  EFAILED:      Otherwise.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

/*=============================================================================
FUNCTION: ICM_EndCall

DESCRIPTION:
  End a call specified by call id. In case of an incoming call, reject the call
  if applicable.

PROTOTYPE:
  int ICM_EndCall(ICM *po,
                  AEECMCallID call_id)

PARAMETERS:
  po                [in]: ICM interface pointer.
  call_id           [in]: Call ID of the call to be ended

RETURN VALUE:
  Returns AEE return values.

  AEE_SUCCESS:  If procedure was started correctly.
  EFAILED:      Otherwise.

COMMENTS:
  End reason is assumed to be always normal.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

/*=============================================================================
FUNCTION: ICM_EndAllCalls

DESCRIPTION:
  End all calls.

PROTOTYPE:
  int ICM_EndAllCalls(ICM *po)

PARAMETERS:
  po                [in]: ICM interface pointer.

RETURN VALUE:
  Returns AEE return values.

  AEE_SUCCESS:  If procedure was started correctly.
  EFAILED:      Otherwise.

COMMENTS:
  End reason is assumed to be normal.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

/*=============================================================================
FUNCTION: ICM_GetActiveCallIDs

DESCRIPTION:
  Obtain all call IDs or the calls matching a call_type.
  AEECM_CALL_TYPE_NONE matches all call types.
  AEECM_CALL_STATE_NONE matches all call states.
  call_type (call state) is a bit mask and hence can be ORed to search for
  multiple call types (call states).
  Return value gives the number of call ids filled in.

  The function can be called without specifying the call_ids array to obtain
  just the number of calls.

PROTOTYPE:
  uint16 ICM_GetActiveCallIDs(ICM *po,
                    AEECMCallType call_type,
                    AEECMCallState call_state,
                    AEECMCallID *call_ids,
                    uint32 call_ids_size)

PARAMETERS:
  po                [in]: ICM interface pointer.
  call_type         [in]: Call type
  call_state        [in]: Call state
  call_ids         [out]: Optional array of Call IDs to be filled in
  call_ids_size     [in]: Optional size of call ID array in bytes
                          (Required if call_ids is non-NULL)

RETURN VALUE:
  Number of calls or the number of call ids stored in the
  call ID array, if specified.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

/*=============================================================================
FUNCTION: ICM_GetCallInfo

DESCRIPTION:
  Get information about the call specified by call id.

PROTOTYPE:
  int ICM_GetCallInfo(ICM *po,
                      AEECMCallID call_id,
                      AEECMCallInfo *call_info,
                      uint32 size)

PARAMETERS:
  po                [in]: ICM interface pointer.
  call_id           [in]: Call ID of the call to be queried
  call_info        [out]: Return call information
  size              [in]: Size of Call info structure (bytes).

RETURN VALUE:
  Returns AEE return values.

  AEE_SUCCESS:  If procedure was started correctly.
  EFAILED:      Otherwise.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

/*=============================================================================
FUNCTION: ICM_SetPrivacyPref

DESCRIPTION:
  Change privacy preference for all calls while a call is in progress.

PROTOTYPE:
  int ICM_SetPrivacyPref(ICM *po,
                         AEECMPrivacyPref privacy_pref)

PARAMETERS:
  po                [in]: ICM interface pointer.
  privacy_pref      [in]: New privacy preference

RETURN VALUE:
  Returns AEE return values.

  AEE_SUCCESS:  If procedure was started correctly.
  EUNSUPPORTED: If mode is not CDMA.
  EFAILED:      Otherwise.

COMMENTS:
  This command can be used only when a call is going on and applies to all
  calls. Valid only in CDMA mode.
  Out-of-call privacy preference change is not supported through lower layers
  at this time.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

/*=============================================================================
FUNCTION: ICM_CallOpsStringCheck

DESCRIPTION:
  Check if the dialed string is a call related operation which does
  not result in call origination.

  In some cases ICM_CallOpsRequest() need a call_id associated with the
  number that user dialed. For eg: the call_id assocated with X if
  user types 1 X <send>. call_num will contain the value X in this case
  after ICM_CallOpsStringCheck() is called.

  If display used by Application is different from the call_id provided
  by ICM, X should be mapped to the appropriate call_id before calling
  ICM_CallOpsRequest(). Otherwise, the call_num returned can be directly
  used in ICM_CallOpsRequest().

PROTOTYPE:
  AEECMIsCallOps ICM_CallOpsStringCheck(ICM *po,
                             AECHAR * dialed_string,
                             uint8 *call_num)

PARAMETERS:
  po                [in]: ICM interface pointer.
  dialed_string     [in]: Dialed String (NULL terminated)
  call_num       [out]:   The number used to refer to a call.
                          (X in 1 X <send>)

RETURN VALUE:
  AEECM_IS_CALLOPS:             If dialed string is a call related operation.
  AEECM_IS_CALLOPS_NEED_CALLID: If dialed string is a call related operation
                           and ICM_CallOpsRequest() need a call id.
                           In this case call_num is filled in.
  AEECM_NOT_CALLOPS:       If dialed string is not a call related operation.

COMMENTS:
  When user dials a number the first check should be whether it is a call
  independent supplementary service string (ISUPPSVC_StringCheck).
  If that check fails, ICM_CallOpsStringCheck() should be called.

  If ICM_CallOpsStringCheck() returns AEECM_IS_CALLOPS or
  AEECM_IS_CALLOPS_NEED_CALLID,
  ICM_CallOpsRequest() should be called. Otherwise the dialed string should be
  used to Originate[Voice]Call().

SIDE EFFECTS:

SEE ALSO:
  ICM_CallOpsRequest(), ISUPPSVC_StringCheck(), ISUPPSVC_StartSS(),
  ICM_OriginateVoiceCall()

=============================================================================*/

/*=============================================================================
FUNCTION: ICM_CallOpsRequest

DESCRIPTION:
  Send a call related supplementary service request

PROTOTYPE:
  int ICM_CallOpsRequest(ICM *po,
                         AECHAR * dialed_string,
                         AEECMCallID call_id)

PARAMETERS:
  po                [in]: ICM interface pointer.
  dialed_string     [in]: Dialed String (NULL terminated)
  call_id           [in]: Optional call_id

RETURN VALUE:
  Returns AEE return values.

  AEE_SUCCESS:  If procedure was started correctly.
  EBADPARM:     If call_id is not valid in an operation that
                requires call_id.
  EFAILED:      Otherwise.

COMMENTS:
  When user dials a number the first check should be whether it is a call
  independent supplementary service string (ISUPPSVC_StringCheck).
  If that check fails, ICM_CallOpsStringCheck() should be called.

  If ICM_CallOpsStringCheck() returns AEECM_IS_CALLOPS or
  AEECM_IS_CALLOPS_NEED_CALLID, ICM_CallOpsRequest() should be called.
  Otherwise the dialed string should be
  used to Originate[Voice]Call().

SIDE EFFECTS:

SEE ALSO:
  ICM_CallOpsStringCheck()

=============================================================================*/


/*---------------------------------------------------------------------------
                          PHONE FUNCTIONS
---------------------------------------------------------------------------*/


/*=============================================================================
FUNCTION: ICM_SetAnswerVoicePref

DESCRIPTION:
  Change the answer voice setting.

PROTOTYPE:
  int ICM_SetAnswerVoicePref(ICM *po,
                            AEECMAnsVoicePref answer_voice,
                            uint32  duration)

PARAMETERS:
  po                [in]: ICM interface pointer.
  answer_voice      [in]: Indicate whether the phone should answer incoming
                          voice calls as voice, fax, or modem
  duration          [in]: Indicate duration in seconds for settings valid once.

RETURN VALUE:
  Returns AEE return values.

  AEE_SUCCESS:  If procedure was started correctly.
  EUNSUPPORTED: If mode is not CDMA
  EFAILED:      Otherwise.

COMMENTS:
  Valid only for CDMA mode

SIDE EFFECTS:
  AEECM_EVENT_CMD_RESULT will be sent to the App with asynchronous result from
  the lower layers. The event will also have the user_data provided along with
  this function.

SEE ALSO:

=============================================================================*/

/*=============================================================================
FUNCTION: ICM_AvoidServingSystem

DESCRIPTION:
  Avoid the serving system specified for a specified time.

PROTOTYPE:
  int ICM_AvoidServingSystem(ICM *po,
                             AEECMAvoidSSPref avoid_type,
                             uint32 avoid_time)

PARAMETERS:
  po                [in]: ICM interface pointer.
  avoid_type        [in]: Type of system to avoid
  avoid_time        [in]: Avoid time in seconds

RETURN VALUE:
  Returns AEE return values.

  AEE_SUCCESS:  If procedure was started correctly.
  EUNSUPPORTED: If mode is not CDMA
  EFAILED:      Otherwise.

COMMENTS:
  Valid only for CDMA mode

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

/*=============================================================================
FUNCTION: ICM_SetSystemPreference_WLAN

DESCRIPTION:  
  Change the phone's system selection preference (mode, band, and 
  roam preference). 
  Status pointer should be valid till status_cb is called. If asynchrounous
  result in not required, both status and status_cb can be NULL.

PROTOTYPE:
  int ICM_SetSystemPreference_WLAN(ICM *po,
                        AEECMModePref mode_pref,
                        AEECMPrefTerm pref_term,
                        uint32 pref_duration,
                        AEECMAcqOrderPref acq_order_pref,
                        AEECMBandPref band_pref,
                        AEECMRoamPref roam_pref,
                        AEECMHybrPref hybr_pref,
                        AEECMServDomPref srv_domain_pref,
                        AEECMNwSelModePref nw_sel_mode_pref,
                        AEECMPLMNID * plmn_ptr,
                        AEECMWLANPref * wlan_ptr,
                        AEECMPhError * status,
                        AEECallback *status_cb)

PARAMETERS:
  po                [in]: ICM interface pointer.
  mode_pref         [in]: Preferred mode 
  pref_term         [in]: Preference span
  pref_duration     [in]: Indicates duration in seconds for pref term
  acq_order_pref    [in]: GW acquisition order preference
  band_pref         [in]: Band preference
  roam_pref         [in]: Roam preference
  hybr_pref         [in]: Hybrid Preference
  srv_domain_pref   [in]: Service domain preference
  nw_sel_mode_pref  [in]: Network should be selected automatically or manually
  plmn_ptr          [in]: If network selection is set to manual, 
                          this specifies PLMN ID
  wlan_ptr          [in]: Information about the WLAN configuration.
  status            [in]: Phone error/success status
  status_cb         [in]: Status Callback

RETURN VALUE:
  Returns AEE return values.

  AEE_SUCCESS:  If procedure was started correctly.
  EFAILED:      Otherwise.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

/*=============================================================================
FUNCTION: ICM_SetSystemPreference

DESCRIPTION:
  Change the phone's system selection preference (mode, band, and
  roam preference).
  Status pointer should be valid till status_cb is called. If asynchrounous
  result in not required, both status and status_cb can be NULL.

PROTOTYPE:
  int ICM_SetSystemPreference(ICM *po,
                        AEECMModePref mode_pref,
                        AEECMPrefTerm pref_term,
                        uint32 pref_duration,
                        AEECMAcqOrderPref acq_order_pref,
                        AEECMBandPref band_pref,
                        AEECMRoamPref roam_pref,
                        AEECMHybrPref hybr_pref,
                        AEECMServDomPref srv_domain_pref,
                        AEECMNwSelModePref nw_sel_mode_pref,
                        AEECMPLMNID * plmn_ptr,
                        AEECMPhError * status,
                        AEECallback *status_cb)

PARAMETERS:
  po                [in]: ICM interface pointer.
  mode_pref         [in]: Preferred mode
  pref_term         [in]: Preference span
  pref_duration     [in]: Indicates duration in seconds for pref term
  acq_order_pref    [in]: GW acquisition order preference
  band_pref         [in]: Band preference
  roam_pref         [in]: Roam preference
  hybr_pref         [in]: Hybrid Preference
  srv_domain_pref   [in]: Service domain preference
  nw_sel_mode_pref  [in]: Network should be selected automatically or manually
  plmn_ptr          [in]: If network selection is set to manual,
                          this specifies PLMN ID
  status            [in]: Phone error/success status
  status_cb         [in]: Status Callback

RETURN VALUE:
  Returns AEE return values.

  AEE_SUCCESS:  If procedure was started correctly.
  EFAILED:      Otherwise.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/


/*=============================================================================
FUNCTION: ICM_SetNAMSel

DESCRIPTION:
  Change the phone's NAM selection.

PROTOTYPE:
  int ICM_SetNAMSel(ICM *po,
                       AEECMNAM nam_sel)

PARAMETERS:
  po                [in]: ICM interface pointer.
  nam_sel           [in]: Change NAM selection to this NAM

RETURN VALUE:
  Returns AEE return values.

  AEE_SUCCESS:  If procedure was started correctly.
  EUNSUPPORTED: If CDMA is not supported
  EFAILED:      Otherwise.

COMMENTS:
  Valid only for CDMA mode

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

/*=============================================================================
FUNCTION: ICM_SetOperatingMode

DESCRIPTION:
  Change the phone's operating mode.

PROTOTYPE:
  int ICM_SetOperatingMode(ICM *po,
                           AEECMOprtMode oprt_mode)

PARAMETERS:
  po                [in]: ICM interface pointer.
  oprt_mode         [in]: Switch to this operating mode

RETURN VALUE:
  Returns AEE return values.

  AEE_SUCCESS:  If procedure was started correctly.
  EFAILED:      Otherwise.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

/*=============================================================================
FUNCTION: ICM_GetPhoneInfo

DESCRIPTION:
  Get a snapshot of the current phone information

PROTOTYPE:
  int ICM_GetPhoneInfo(ICM *po,
                       AEECMPhInfo *info,
                       uint32 size)

PARAMETERS:
  po                [in]: ICM interface pointer.
  info             [out]: Information about the Phone
  size              [in]: Size of Phone info structure (bytes)

RETURN VALUE:
  Returns AEE return values.

  AEE_SUCCESS:  If procedure was started correctly.
  EFAILED:      Otherwise.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

/*=============================================================================
FUNCTION: ICM_SetSubscriptionStatus

DESCRIPTION:
  Informs the Call Manager that the subscription information is available
  or not available.

PROTOTYPE:
  int ICM_SetSubscriptionStatus(ICM *po,
                       AEECMSysMode     mode,
                       boolean   available)

PARAMETERS:
  po                [in]: ICM interface pointer.
  available         [in]: TRUE if Available and FALSE if Not Available
  mode              [in]: AEECM_SYS_MODE_CDMA or AEECM_SYS_MODE_GSM or
                          AEECM_SYS_MODE_WCDMA or AEECM_SYS_MODE_WLAN

RETURN VALUE:
  Returns AEE return values.

  AEE_SUCCESS:  If procedure was started correctly.
  EFAILED:      Otherwise.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

/*=============================================================================
FUNCTION: ICM_SendMBMSCommand

DESCRIPTION:
  Sends a command to the MBMS Service Manager.
  Clients should fill in the appropriate MBMS request type and also
  provide the corresponding parameters in the mbms_generic_req struct.

  Aysynchronous command responses will be sent to the application
  via notify events.  Apps that wish to receive MBMS related events
  must register with ICM notifier to receive events with the mask
  NMASK_CM_MBMS.  Eg:
  ISHELL_RegisterNotify(pIShell, AEECLSID_SOMEAPP, AEECLSID_CM_NOTIFIER, NMASK_CM_MBMS)

  Event specific data will accompany the event in the dwParam->data within a structure of
  type AEECMNotifyInfo.event_data.mbms.  See AEECMMBMSEventData to see the specific event
  types and data that can be received related to MBMS.           
  
PROTOTYPE:
  int OEMCM_SendMBMSCommand(ICM *po, AEECMMBMSCmd *pMbmsCmd)

PARAMETERS:
  po                [in]: ICM interface pointer.
  pMbmsCmd          [in]: MBMS command structure, used to specify the specific MBMS
                          command to be sent along with its parameters.  
                          See AEECMMBMSCmdType for available commands.
  
RETURN VALUE:
  AEE_SUCCESS:  Command was sent.
  EFAILED:      Error while sending command.

=============================================================================*/

/*=============================================================================
FUNCTION: ICM_SetRSSIDeltaThreshold

DESCRIPTION:
  Change RSSI-reporting delta threshold. The default delta threshold is 5 dBm.

PROTOTYPE:
  int ICM_SetRSSIDeltaThreshold(ICM *po,
                             uint8 delta)

PARAMETERS:
  po                [in]: ICM interface pointer.
  delta             [in]: Delta threshold in dBm for RSSI reporting

RETURN VALUE:
  Returns AEE return values.

  AEE_SUCCESS:  If procedure was started correctly.
  EFAILED:      Otherwise.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

/*=============================================================================
FUNCTION: ICM_GetNetworks

DESCRIPTION:
  Retrieve a list of preferred or available networks depending on the type
  specified. Only one pending GetNetworks() operation is allowed at a time.

  The reference to application memory should be valid till
  AEECallback is issued. This function cannot be invoked synchronously.

PROTOTYPE:
  int ICM_GetNetworks(ICM *po,
                      AEECMNetworkType list_type,
                      AEECMPhError *status,
                      void * list,
                      uint32 list_size,
                      AEECallback *cb)

PARAMETERS:
  po                [in]: ICM interface pointer.
  list_type         [in]: Indicates whether to retrieve the preferred or
                          available networks
  status            [in]: Contains status of the command on callback.
  list              [in]; Address of network list (AEECMUserPLMNList or
                          AEECMDetailedPLMNList).
  size              [in]: Size of network list structure (bytes).
                          sizeof(AEECMUserPLMNList) or
                          sizeof(AEECMDetailedPLMNList)
  cb                [in]: Callback issued when ICM_GetNetworks is complete

RETURN VALUE:
  Returns AEE return values.

  AEE_SUCCESS:  If procedure was started correctly.
  EUNSUPPORTED: If mode is not GSM/WCDMA
  EFAILED:      Otherwise.

COMMENTS:
  This function is only valid for GSM/WCDMA modes.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

/*=============================================================================
FUNCTION: ICM_SetPrefNetworks

DESCRIPTION:
  Save the preferred networks to the SIM/USIM.

  Status pointer should be valid till status_cb is called. If asynchrounous
  result in not required, both status and status_cb can be NULL.


PROTOTYPE:
  int ICM_SetPrefNetworks(ICM *po,
                          AEECMUserPLMNList *pref_networks_ptr,
                          AEECMPhError *status,
                          AEECallback *status_cb)

PARAMETERS:
  po                [in]: ICM interface pointer.
  pref_networks_ptr [in]: Preferred networks to be saved
  status            [in]: Phone error/success status
  status_cb         [in]: Status Callback

RETURN VALUE:
  Returns AEE return values.

  AEE_SUCCESS:  If procedure was started correctly.
  EUNSUPPORTED: If mode is not GSM/WCDMA
  EFAILED:      Otherwise.

COMMENTS:
  This function is only valid for GSM/WCDMA modes.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

/*=============================================================================
FUNCTION: ICM_WakeupFromStandby

DESCRIPTION:
  Informs the lower layers to wake up from deep sleep (standby mode).

PROTOTYPE:
  int ICM_WakeupFromStandby(ICM *po)

PARAMETERS:
  po                [in]: ICM interface pointer.

RETURN VALUE:
  Returns AEE return values.

  AEE_SUCCESS:  If procedure was started correctly.
  EUNSUPPORTED: If mode is not CDMA
  EFAILED:      Otherwise.

COMMENTS:
  This function is only valid for CDMA mode

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

/*=============================================================================
FUNCTION: ICM_SetRTREConfig

DESCRIPTION:
  Changes the RTRE configuration to the client specified value.

PROTOTYPE:
  int ICM_SetRTREConfig(ICM *po,
                           AEECMRTREConfig rtre_config)

PARAMETERS:
  po                [in]: ICM interface pointer.
  rtre_config       [in]: The desired RTRE configuration

RETURN VALUE:
  Returns AEE return values.

  AEE_SUCCESS:  If procedure was started correctly.
  EUNSUPPORTED: If mode is not CDMA
  EFAILED:      Otherwise.

COMMENTS:
  This function is only valid for CDMA mode

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
/*=============================================================================
FUNCTION: ICM_SetAnswerVoicePref

DESCRIPTION:
  Change the answer voice setting.

PROTOTYPE:
  int ICM_SetAnswerVoicePref(ICM *po,
                            AEECMAnsVoicePref answer_voice,
                            uint32  duration)

PARAMETERS:
  po                [in]: ICM interface pointer.
  answer_voice      [in]: Indicate whether the phone should answer incoming
                          voice calls as voice, fax, or modem
  duration          [in]: Indicate duration in seconds for settings valid once.

RETURN VALUE:
  Returns AEE return values.

  AEE_SUCCESS:  If procedure was started correctly.
  EUNSUPPORTED: If mode is not CDMA
  EFAILED:      Otherwise.

COMMENTS:
  Valid only for CDMA mode

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

/*=============================================================================
FUNCTION: ICM_ResetACM

DESCRIPTION:
  Reset the Accumulate Call Meter to 0 in a system supporting Advice of Charge.

PROTOTYPE:
  int ICM_ResetACM(ICM *po)

PARAMETERS:
  po                [in]: ICM interface pointer.

RETURN VALUE:
  Returns AEE return values.

  AEE_SUCCESS:  If procedure was started correctly.
  EUNSUPPORTED: If mode is not GSM/WCDMA
  EFAILED:      Otherwise.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

/*=============================================================================
FUNCTION: ICM_SetACMMax

DESCRIPTION:
  Set the Maximum value for Accumulate Call Meter.

PROTOTYPE:
  int ICM_SetACMMax(ICM *po, uint32 new_max)

PARAMETERS:
  po                [in]: ICM interface pointer.
  new_max           [in]: New value for ACMMAX

RETURN VALUE:
  Returns AEE return values.

  AEE_SUCCESS:  If procedure was started correctly.
  EUNSUPPORTED: If mode is not GSM/WCDMA
  EFAILED:      Otherwise.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

/*=============================================================================
FUNCTION: ICM_SetDDTMPref

DESCRIPTION:
  Change the phone's data dedicated transmission mode (DDTM)
  preference to a specified selection, such as ON or OFF etc.

PROTOTYPE:
  int ICM_SetDDTMPref(ICM *po, AEECMDDTMPref ddtm_pref)

PARAMETERS:
  po                [in]: ICM interface pointer.
  ddtm_pref         [in]: Dedicated data transmission mode

RETURN VALUE:
  Returns AEE return values.

  AEE_SUCCESS:  If procedure was started correctly.
  EUNSUPPORTED: If DDTM is not supported
  EFAILED:      Otherwise.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

/*---------------------------------------------------------------------------
                       SERVING SYSTEM FUNCTIONS
---------------------------------------------------------------------------*/

/*=============================================================================
FUNCTION: ICM_GetSSInfo

DESCRIPTION:
  Get the current serving system information and registration status.

PROTOTYPE:
  int ICM_GetSSInfo(ICM *po,
                    AEECMSSInfo *ss_info,
                    uint32 size)

PARAMETERS:
  po                [in]: ICM interface pointer.
  ss_info          [out]: Return Serving System Information
  size              [in]: Size of Serving System Info structure (bytes)

RETURN VALUE:
  Returns AEE return values.

  AEE_SUCCESS:  If procedure was started correctly.
  EFAILED:      Otherwise.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/


/*---------------------------------------------------------------------------
                           INBAND FUNCTIONS
---------------------------------------------------------------------------*/



/*=============================================================================
FUNCTION: ICM_BurstDTMF

DESCRIPTION:
  Send a Burst DTMF

PROTOTYPE:
  int ICM_BurstDTMF(ICM *po,
                    AEECMCallID call_id,
                    AEECMDTMFOnLength on_length,
                    AEECMDTMFOffLength off_length,
                    AECHAR *digits)

PARAMETERS:
  po                [in]: ICM interface pointer.
  call_id           [in]: Call to send DTMFs
  on_length         [in]: DTMF pulse width
  off_length        [in]: DTMF inter-digit interval
  digits            [in]: Null terminated string of DTMF digits

RETURN VALUE:
  Returns AEE return values.

  AEE_SUCCESS:  If procedure was started correctly.
  EUNSUPPORTED: If mode is not CDMA
  EFAILED:      Otherwise.

COMMENTS:
  Valid only for CDMA mode.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

/*=============================================================================
FUNCTION: ICM_StartContDTMF

DESCRIPTION:
  Start a continous DTMF tone

PROTOTYPE:
  int ICM_StartContDTMF(ICM *po,
                        AEECMCallID call_id,
                        AECHAR digit)

PARAMETERS:
  po                [in]: ICM interface pointer.
  call_id           [in]: Call ID
  digit             [in]: DTMF digit

RETURN VALUE:
  Returns AEE return values.

  AEE_SUCCESS:  If procedure was started correctly.
  EFAILED:      Otherwise.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

/*=============================================================================
FUNCTION: ICM_StopContDTMF

DESCRIPTION:
  Stop continous DTMF tone

PROTOTYPE:
  int ICM_StopContDTMF(ICM *po,
                       AEECMCallID call_id)

PARAMETERS:
  po                [in]: ICM interface pointer.
  call_id           [in]: Call ID

RETURN VALUE:
  Returns AEE return values.

  AEE_SUCCESS:  If procedure was started correctly.
  EFAILED:      Otherwise.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

/*=============================================================================
FUNCTION: ICM_SendFlashSupps

DESCRIPTION:
   Send a flash with info for the specified supplementary service.

PROTOTYPE:
   int ICM_SendFlashSupps(ICM *pMe,
                          AEECMSendFlashSuppsType suppsType,
                          AEECMCallID callId)

PARAMETERS:
   pMe                [in]: ICM interface pointer.
   suppsType          [in]: Supplementary service type
   callId             [in]: Call id to operate on

RETURN VALUES:
   SUCCESS
   EFAILED

=============================================================================*/

/*---------------------------------------------------------------------------
                           PRL FUNCTIONS
---------------------------------------------------------------------------*/
/*=============================================================================
FUNCTION: ICM_PrlExtractAcqRecord

DESCRIPTION:
   Extract a specified acquisition record from the currently loaded PRL.

PROTOTYPE:
   int ICM_PrlExtractAcqRecord(ICM *pMe,
                               AEECMPrlAcqRecord *pAcqRec,
                               int index)

PARAMETERS:
   pMe                [in]: ICM interface pointer.
   pAcqRec            [out]: Pointer to acq record structure in which to 
                            fill the record info.
   index              [in]: Index of the acquisition record to extract
   

RETURN VALUES:
   SUCCESS
   EBADPARM   
   EFAILED

=============================================================================*/

/*=============================================================================
FUNCTION: ICM_PrlUpdateAcqRecord

DESCRIPTION:
   Update the PRL acquisition table entry with the specified acqusition record.

PROTOTYPE:
   int ICM_PrlUpdateAcqRecord(ICM *pMe, 
                              const AEECMPrlAcqRecord *pAcqRec,
                              int index)

PARAMETERS:
   pMe                [in]: ICM interface pointer.
   pAcqRec            [in]: Pointer to AEECM acquisition record which will be
                            used to update the acquisition table.
   index              [in]: Index of table entry to update.

RETURN VALUES:
   SUCCESS
   EBADPARM
   EFAILED

=============================================================================*/

/*=============================================================================
FUNCTION: ICM_PrlCommit

DESCRIPTION:
   Commit PRL in NV

PROTOTYPE:
   int ICM_PrlCommit(ICM *pMe)

PARAMETERS:
   pMe                [in]: ICM interface pointer.

RETURN VALUES:
   SUCCESS
   EBADPARM
   EFAILED

=============================================================================*/

/*---------------------------------------------------------------------------
                           HELPER FUNCTIONS
---------------------------------------------------------------------------*/

/*=============================================================================
FUNCTION: ICM_IsValidModeBandPref

DESCRIPTION:
  If both mode_pref and band_pref is specified, checks whether the combination
  is supported by the current target.

  If only mode_pref is specified (band_pref is AEECM_BAND_PREF_NONE),
  checks whether the given mode preference is valid with the current band
  preference.

  If only band_pref is specified (mode_pref is AEECM_MODE_PREF_NONE),
  checks whether the given band preference is valid with the current mode
  preference.

  If both are NONE, returns FALSE;

PROTOTYPE:
  boolean ICM_IsValidModeBandPref(ICM *po,
                                     AEECMModePref mode_pref,
                                     AEECMBandPref band_pref)

PARAMETERS:
  po                [in]: ICM interface pointer.
  mode_pref         [in]: mode preference
  band_pref         [in]: band preference

RETURN VALUE:
  TRUE:      If valid mode and band preference
  FALSE:     Otherwise

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

/*=============================================================================
FUNCTION: ICM_GetSysMode

DESCRIPTION:
  Gets the current system mode or the mode that is being acquired.

PROTOTYPE:
  AEECMSysMode ICM_GetSysMode(ICM *po)

PARAMETERS:
  po                [in]: ICM interface pointer.

RETURN VALUE:
  AEECMSysMode

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

/*=============================================================================
FUNCTION: ICM_IsMyCall

DESCRIPTION:
  Checks if the call was originated or answered by this application/module
  (using this ICM interface pointer).

PROTOTYPE:
  boolean ICM_IsMyCall(ICM *po, AEECMCallID call_id)

PARAMETERS:
  po                [in]: ICM interface pointer.

RETURN VALUE:
  AEECMSysMode

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/


/*============================================================================

                              ICALLOPTS

=============================================================================*/


/*---------------------------------------------------------------------------
  DATA STRUCTURE DOCUMENTATION
---------------------------------------------------------------------------*/

/*============================================================================
CallOpt

Description:
   Holds an id, value pair for ICallOpts

Definition:
   typedef struct CallOpt
   {
      int32        nId;
      void        *pVal;
   } CallOpt;

Members:
   int32  nId: option id, chosen from one of the AEECM_CALLOPT_* defines.

   void *pVal: option value, depends on nId

Comments:
   CallOpt structures are the basic communication building block for
   talking to ICallOpts.  The structures, when passed to AddOpt, are not
   themselves copied, the contents are copied out.

See Also:
   None

============================================================================*/

/*---------------------------------------------------------------------------
  INTERFACE DOCUMENTATION
---------------------------------------------------------------------------*/

/*=============================================================================
FUNCTION: ICallOpts_AddOpt

DESCRIPTION:
  Adds an ID-Value pair to ICallOpts. Only one value is allowed for each
  ID unlike IWebOpts which maintains a stack of values for each ID.

PROTOTYPE:
  int ICallOpts_AddOpt(ICallOpts *po, CallOpt *pOpt)

PARAMETERS:
  po                [in]: ICallOpts interface pointer.
  pOpt              [in]: Call option

RETURN VALUE:
  SUCCESS:   If addition was successful.
  ENOMEMORY: If enough memory couldn't be allocated
  EBADPARM:  If any of the input parameters is NULL.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:
  IWEBOPTS_AddOpt()

=============================================================================*/

/*=============================================================================
FUNCTION: ICallOpts_RemoveOpt

DESCRIPTION:
  Remove the ID-value pair matching the specified ID.

PROTOTYPE:
  boolean ICallOpts_RemoveOpt(ICallOpts *po, int nOptId)

PARAMETERS:
  po                [in]: ICallOpts interface pointer.
  nOptId            [in]: Option ID.

RETURN VALUE:
  SUCCESS: If remove was successful
  EFAILED: If ID was not found

COMMENTS:
  Corresponds to IWEBOPTS_RemoveOpt() with index 0.

SIDE EFFECTS:

SEE ALSO:
  IWEBOPTS_RemoveOpt()

=============================================================================*/

/*=============================================================================
FUNCTION: ICallOpts_GetOpt

DESCRIPTION:
  Find the ID-value pair matching the specified ID.

PROTOTYPE:
  int ICallOpts_GetOpt(ICalOpts *po, int32 nOptId, CallOpt *pOpt)

PARAMETERS:
  po                [in]: ICallOpts interface pointer.
  nOptId            [in]: Option ID to search for.
  pOpt          [in/out]: Reference to CallOpt where matching call option
                          is copied.

RETURN VALUE:
  SUCCESS: If a matching call option is found.
  EFAILED: Otherwise

COMMENTS:
  Only the CallOpt structure is copied and not the data pointed by pVal field.

SIDE EFFECTS:

SEE ALSO:
  IWEBOPTS_GetOpt()

=============================================================================*/


/*=============================================================================
FUNCTION: ICM_PLMNFromIMSI

DESCRIPTION:
  Extracts PLMN from IMSI
PROTOTYPE:
  int ICM_GetPLMNFromIMSI(ICM *pMe, AEECMPLMNID *pPLMN, byte *pIMSI)

PARAMETERS:
  pMe               [in]: ICM interface pointer.
  plmn             [out]: PLMN Info pointer
  imsi             [in]: IMSI pointer

RETURN VALUE:
  SUCCESS:      If procedure was started correctly.
  EFAILED:      Otherwise.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

/*=============================================================================
FUNCTION: ICM_SysMatchPLMN

DESCRIPTION:
  Matches two given PLMNs
PROTOTYPE:
  boolean ICM_SysMatchPLMN(ICM *pMe, AEECMPLMNID PLMNa, AEECMPLMNID PLMNb)

PARAMETERS:
  pMe               [in]: ICM interface pointer.
  plmna             [in]: PLMN Info pointer for first PLMN
  plmnb             [in]: PLMN Info pointer for second PLMN

RETURN VALUE:
  TRUE:  If procedure was started correctly.
  FALSE:  Otherwise.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

#endif /* AEECM_H */

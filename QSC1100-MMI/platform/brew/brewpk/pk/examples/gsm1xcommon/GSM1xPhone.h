#ifndef GSM1XPHONE_H
#define GSM1XPHONE_H
/*===============================================================
    GSM1x Note 
    ----------
    This file provides an extended IPhone BREW Interface.
    As a temporary solution, it is a modified copy of an OEM file.
    
    Hence, it should ONLY be included in files that belong to GSM1x.    
=================================================================*/

/*===========================================================================

                           A E E P h o n e . h


GENERAL DESCRIPTION
   Extended AEE Phone/TAPI Services (Base level definitions, typedefs, etc.
   for AEE Phone Services)

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001-2002 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "AEE.h"

/*===========================================================================
                     following stuff was taken from OEMCM.h
===========================================================================*/

// defines that comes from custsurf.h
#ifndef FEATURE_IS95B_EXT_DISP
    #define FEATURE_IS95B_EXT_DISP
#endif

#ifndef FEATURE_IS95B_EXT_DISP
    #define FEATURE_IS95B_ALERT_ANS_BYPASS
#endif

#ifndef FEATURE_IS95B_NDSS
    #define FEATURE_IS95B_NDSS
#endif

#ifndef FEATURE_IS2000
    #define FEATURE_IS2000
#endif

#ifdef FEATURE_MULTIMODE_ARCH
	#define MAXNO_FORWARD_TO_NUMBER          (1)
	#define CM_ORIG_QOS_LEN_MAX         31

typedef uint8                            cm_repeat_indicator_s_type;
typedef uint8                            cm_bearer_capability_s_type;
typedef uint8                            cm_low_layer_compatibility_s_type;
typedef uint8                            cm_high_layer_compatibility_s_type;
typedef uint8                            cm_called_party_bcd_no_s_type;
typedef uint8                            cm_called_party_subaddress_s_type;
typedef uint8                            cm_calling_party_bcd_no_s_type;
typedef uint8                            cm_calling_party_subaddress_s_type;
typedef uint8                            cm_forward_cug_info_s_type;
typedef uint8                            ie_cm_cc_cause_s_type;
typedef uint8                            ie_cm_mo_call_details_s_type;
typedef uint8                            ie_cm_ss_error_s_type;

typedef uint8                            active_calls_list_s_type;
typedef uint8                            cm_connected_no_s_type;
typedef uint8                            cm_connected_subaddress_s_type;
typedef uint8                            cm_cause_of_no_cli_s_type;
typedef uint8                            ect_call_state_s_type;
typedef uint8                            routing_number_option_s_type;
typedef uint8                            ie_ss_status_s_type;
typedef uint8                            ie_ss_code_s_type;
typedef uint8                            ie_ss_notification_s_type;

typedef uint8                            nsapi_T;
typedef uint8                            qos_T;
typedef uint8                            pdp_address_T;
typedef uint8                            protocol_cfg_options_T;
typedef uint8                            apn_T;
typedef uint8                            ti_T;
typedef uint8                            cause_enum_type_T;
typedef uint8                            pdp_cause_T;
typedef uint8                            rabm_rabid_T;
typedef uint8                            rabmcm_cause_T;
typedef uint8                            sm_status_T;
#endif

// the next paragraph was taken from cai.h
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*                        PRESENTATION INDICATORS                          */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* See IS-95A Table 6.7.4.4-1. Presentation Indicators                     */
#define CAI_ALLOW_PI     0x00
  /* Presentation allowed */
#define CAI_RES_PI       0x01
  /* Presentation restricted */
#define CAI_NOT_AVAIL_PI 0x02
  /* Number not available */

#ifdef FEATURE_IS2000

/* Retry delay type ( FEATURE_IS2000 )
*/
typedef enum cm_retry_type_e {

  CM_RETRY_TYPE_NONE = -1,
    /* No retry order */

  CM_RETRY_TYPE_CLEAR_ALL,
    /* Clear all retry order */

  CM_RETRY_TYPE_ORIGINATION,
    /* Origination retry order */

  CM_RETRY_TYPE_RESOURCE_REQ,
    /* Resource request retry order */

  CM_RETRY_TYPE_SUPP_CH_REQ,
    /* Supplemental channel request retry order */

  CM_RETRY_TYPE_RESERVED4,
    /* Reserved */

  CM_RETRY_TYPE_RESERVED5,
    /* Reserved */

  CM_RETRY_TYPE_RESERVED6,
    /* Reserved */

  CM_RETRY_TYPE_RESERVED7,
    /* Reserved */

  CM_RETRY_TYPE_MAX
    /* FOR INTERNAL USE OF CM ONLY! */

  #ifdef FEATURE_RPC
  , CM_RETRY_TYPE_BIG=0x10000000
    /* To force enum to 32 bit for windows NT */
  #endif /* FEATURE_RPC */

} cm_retry_type_e_type;

/* Retry order info */
typedef struct
{
  cm_retry_type_e_type retry_type;     /* Retry delay type */
  word                 srv_opt;        /* Service option   */
  boolean              infinite_delay;
    /* True if infinite delay, it should always be FALSE for origination */
  uint32               retry_delay_in_ms;
    /* Only meaningful if infinite_delay is FALSE */
} cm_retry_order_type;

#endif /* FEATURE_IS2000 */

/* Enumeration of call states. At all times a CM call must be in one
** of the states below.
*/
typedef enum cm_call_state_e {

  CM_CALL_STATE_NONE=-1,
    /* FOR INTERNAL USE OF CM ONLY! */

  CM_CALL_STATE_IDLE,
    /* Call is in idle state - i.e. no call */

  CM_CALL_STATE_ORIG,
    /* Call is in origination state */

  CM_CALL_STATE_INCOM,
    /* Call is in alerting state */

  CM_CALL_STATE_CONV,
    /* Call is in conversation state */

  CM_CALL_STATE_MAX
    /* FOR INTERNAL USE OF CM ONLY! */

  #ifdef FEATURE_RPC
  , CM_CALL_STATE_BIG=0x10000000
    /* To force enum to 32 bit for windows NT */
  #endif /* FEATURE_RPC */

} cm_call_state_e_type;


#define CM_MAX_SUBADDRESS_DIGITS   20
  /* Max length of subaddress digit string */

typedef struct cm_subaddress_s
{
  boolean   extension_bit;
    /* The extension bit */

  uint8     subadd_type;
    /* Type of subaddress - NSAP/USER */

  uint8     odd_even_ind;
    /* The indicator of even/odd bits - EVEN/ODD */

  uint8     num_char;
    /* Number of characters in subaddress array */

  uint8     chari[CM_MAX_SUBADDRESS_DIGITS];
    /* Array of subaddress digits */

} cm_subaddress_s_type;


/* Client ID type to differentiate CM client objects from one another.
*/
typedef int  cm_client_id_type;

/* Enumeration of CM call types.
**
** When originating or answering a call, clients need to specify
** one of these call types.
*/

#ifndef FEATURE_MULTIMODE_ARCH
typedef enum cm_call_type_e {

    /* Call types for originating and answering calls.
    */

    CM_CALL_TYPE_NONE=-1,
        /* FOR INTERNAL USE OF CM ONLY! */

    CM_CALL_TYPE_VOICE,
        /* Voice call type.
        **
        ** NOTE! Answering with this type is only successful if the
        ** incoming call is of type VOICE */

    CM_CALL_TYPE_DATA,
        /* Data call type - (for modem, fax, packet, etc. calls).
        **
        ** NOTE! Answering with this call is only successful if the
        ** incoming call is of type DATA or VOICE (with AMPS service) */

    CM_CALL_TYPE_SMS,
        /* SMS call type (SMS SO-6 and SO-14)
        **
        ** NOTE! Answering with this call type is only successful
        ** if incoming call is of type SMS */

    CM_CALL_TYPE_PD,
        /* Position Determination call type (SO-35 and 36)
        ** This call type used to exchange IS-801 messages.
        **/

    CM_CALL_TYPE_TEST,
        /* Test call type (for Markov, Loopback, etc.)
        **
        ** NOTE! Answering with this call type is only successful
        ** if incoming call is of type TEST */

    CM_CALL_TYPE_OTAPA,
        /* OTAPA call type (OTAPA SO-18 and SO-19)
        **
        ** NOTE! Answering with this call type is only successful
        ** if incoming call is of type OTAPA */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Call types that are automatically assigned by call manager when
    ** detecting an origination dial-string that matches one of the
    ** special numbers that are stored in the phone-book.
    **
    ** NOTE! This call types should not be used to originate
    ** or answer calls.
    */

    CM_CALL_TYPE_STD_OTASP,
        /* Standard OTASP call type.
        **
        ** This call type is automatically assigned by call manager when
        ** detecting an origination dial-string that matches one of the
        ** standard OTASP numbers that are stored in the phone-book.
        **
        ** NOTE! This call type should not be used to originate
        ** or answer calls */

    CM_CALL_TYPE_NON_STD_OTASP,
        /* None standard OTASP call type.
        **
        ** This call type is automatically assigned by call manager when
        ** detecting an origination dial-string that matches one of the none
        ** standard OTASP numbers that are stored in the phone-book.
        **
        ** NOTE! This call type should not be used to originate
        ** or answer calls */

    CM_CALL_TYPE_E911,
        /* Emergency call type.
        **
        ** This call type is automatically assigned by call manager when
        ** detecting an origination dial-string that matches one of the
        ** E911 numbers that are stored in the phone-book.
        **
        ** NOTE! this call type should not be used to originate
        ** or answer calls
        **
        ** NOTE! When call manager assigned this call type, the mode
        ** preference is forced to E911 before origination. If at the
        ** end of the call the phone is not in E911 callback mode,
        ** the mode is automatically restored to its original selection.
        ** Else the E911 mode stays in effect until changed by a client
        ** or power-cycle */

    CM_CALL_TYPE_MAX
        /* FOR INTERNAL USE OF CM ONLY! */

#ifdef FEATURE_RPC
#error code not present
#endif /* FEATURE_RPC */

} cm_call_type_e_type;

typedef enum {
  CM_ABAND,
  CM_BBAND,
  CM_ABLOCK,
  CM_BBLOCK,
  CM_CBLOCK,
  CM_DBLOCK,
  CM_EBLOCK,
  CM_FBLOCK,
  CM_PREFERRED,
  CM_OTASP_ACT_CODE_NONE

#ifdef FEATURE_RPC
#error code not present
#endif /* FEATURE_RPC */

} cm_activate_code_type;

#else    /*FEATURE_MULTIMODE_ARCH*/

/* Enumeration of CM call types.
** When originating or answering a call, clients need to specify
** one of these call types.
*/
/* For compatibility with QXDM database, please do not change the following
** fix assigned values. If new values are added please also update the QXDM
** database
*/
typedef enum cm_call_type_e {

  CM_CALL_TYPE_NONE=-1,
    /* FOR INTERNAL USE OF CM ONLY! */

  CM_CALL_TYPE_VOICE=0,
    /* Voice call type.
    **
    ** NOTE! Answering with this type is only successful if the
    ** incoming call is of type VOICE */

  CM_CALL_TYPE_CS_DATA=1,
    /* CS Data call type - (for modem, fax, etc. calls).
    **
    ** NOTE! Answering with this call is only successful if the
    ** incoming call is of type DATA or VOICE (with AMPS service) */

  CM_CALL_TYPE_PS_DATA=2,
    /* Packet Switched Data call type */

  CM_CALL_TYPE_SMS=3,
    /* SMS call type (SMS SO-6 and SO-14). CDMA only
    **
    ** NOTE! Answering with this call type is only successful
    ** if incoming call is of type SMS */

  CM_CALL_TYPE_PD=4,
    /* Position Determination call type (SO-35 and 36). CDMA only
    ** This call type used to exchange IS-801 messages.
    ** Valid for FEATURE_GPSONE_DBM */

  CM_CALL_TYPE_TEST=5,
    /* Test call type (for Markov, Loopback, etc.). CDMA only
    **
    ** NOTE! Answering with this call type is only successful
    ** if incoming call is of type TEST */

  CM_CALL_TYPE_OTAPA=6,
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

  CM_CALL_TYPE_STD_OTASP=7,
    /* Standard OTASP call type. CDMA only
    **
    ** This call type is automatically assigned by call manager when
    ** detecting an origination dial-string that matches one of the
    ** standard OTASP numbers that are stored in the phone-book.
    **
    ** NOTE! This call type should not be used to originate
    ** or answer calls */

  CM_CALL_TYPE_NON_STD_OTASP=8,
    /* None standard OTASP call type. CDMA only
    **
    ** This call type is automatically assigned by call manager when
    ** detecting an origination dial-string that matches one of the none
    ** standard OTASP numbers that are stored in the phone-book.
    **
    ** NOTE! This call type should not be used to originate
    ** or answer calls */

  CM_CALL_TYPE_EMERGENCY=9,
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

  CM_CALL_TYPE_MAX
    /* FOR INTERNAL USE OF CM ONLY! */

  #ifdef FEATURE_RPC
    , CM_CALL_TYPE_BIG=0x10000000 /* To force enum to 32 bit for windowsNT */
  #endif /* FEATURE_RPC */

} cm_call_type_e_type;

/* OTASP activation code. For CDMA only.
*/
typedef enum cm_activate_code_e {
  CM_ABAND,
  /* A band */

  CM_BBAND,
  /* B band */

  CM_ABLOCK,
  /* A block */

  CM_BBLOCK,
  /* B block */

  CM_CBLOCK,
  /* C block */

  CM_DBLOCK,
  /* D block */

  CM_EBLOCK,
  /* E block */

  CM_FBLOCK,
  /* F block */

  CM_PREFERRED,
  /* Preferred OTASP */

  CM_OTASP_ACT_CODE_NONE
  /* None */

  #ifdef FEATURE_RPC
  , CM_OTASP_BIG=0x10000000
    /* To force enum to 32 bit for windows NT */
  #endif /* FEATURE_RPC */

} cm_activate_code_e_type;

#endif    /*end FEATURE_MULTIMODE_ARCH*/

#define CM_MAX_DIALED_DIGITS      32   /* This should be same NV_MAX_DIAL_DIGITS
                                          This is hardcoded here to eliminate
                                          dependency on nv.h for remote
                                          processors. */

#define CM_MAX_CALLER_ID_CHARS    32
    /* Max length of Caller ID string */

#define CM_MAX_CALLED_INFO_CHARS  64   /*  CM_MAX_DATA_DIGITS*/
    /* Max length of Called info string */

#define CM_MAX_NUMBER_CHARS      CM_MAX_CALLED_INFO_CHARS
    /* Max length of CM number string  This is the Maximum
       of CM_MAX_DIALED_DIGITS, CM_MAX_CALLER_ID_CHARS
       CM_MAX_CALLED_INFO_CHARS and CM_MAX_CALLED_INFO_CHARS */

/* Enumeration of digit modes.
*/
typedef enum cm_digit_mode_e {

  CM_DIGIT_MODE_NONE=-1,
    /* FOR INTERNAL USE OF CM ONLY! */

  CM_DIGIT_MODE_4BIT_DTMF,
    /* 4-bit DTMF */

  CM_DIGIT_MODE_8BIT_ASCII,
    /* 8-bit ASCII */

  CM_DIGIT_MODE_MAX
    /* FOR INTERNAL USE OF CM ONLY! */

  #ifdef FEATURE_RPC
  , CM_DIGIT_MODE_BIG=0x10000000
    /* To force enum to 32 bit for windows NT */
  #endif /* FEATURE_RPC */

} cm_digit_mode_e_type;

/* Privacy mode during a call. CDMA only.
*/
typedef enum cm_privacy_pref_e {

  CM_PRIVACY_PREF_NONE = -1,
    /* FOR INTERNAL USE OF CM ONLY! */

  CM_PRIVACY_PREF_STANDARD,
    /* Standard privacy */

  CM_PRIVACY_PREF_ENHANCED,
    /* Enhanced privacy */

  CM_PRIVACY_PREF_MAX
    /* FOR INTERNAL USE OF CM ONLY! */

  #ifdef FEATURE_RPC
  , CM_PRIVACY_PREF_BIG=0x10000000
    /* To force enum to 32 bit for windows NT */
  #endif /* FEATURE_RPC */

} cm_privacy_pref_e_type;

/* Type for CM number.
** Able to store a dialed number or a caller ID number.
**
** NOTE! In the case where the digit_mode parameter is set to
** CM_DIGIT_MODE_4BIT_DTMF the number buffer may only contain
** 0-9, "#", or "*" characters.
**
** In the case where the digit_mode parameter is set to
** CM_DIGIT_MODE_8BIT_ASCII the number buffer may contain any
** 8-bit ASCII characters.
**
** Digital calls include CDMA and GSM/WCDMA but not AMPS.
*/
typedef struct cm_num_s {

  uint8                    buf[CM_MAX_NUMBER_CHARS];
    /* Number buffer */

  uint8                    len;
    /* Length of number stored in num_buf */

  cm_digit_mode_e_type     digit_mode;
    /* Origination digit mode */

  uint8                    number_type;
    /* Number type */

  uint8                    number_plan;
    /* Numbering Plan */

  uint8                    pi;
    /* Presentation Indicator */

  uint8                    si;
    /* Screening Indicator */

  cm_subaddress_s_type     subaddr;
    /* Subaddress */

} cm_num_s_type;

#define CM_MAX_ALPHA_LEN_FOR_DISPLAY_INFO   64  /* CM_MAX_DATA_DIGITS */

#define CM_MAX_ALPHA_TAG_CHARS    CM_MAX_ALPHA_LEN_FOR_DISPLAY_INFO
    /* Max length of CM alpha tag string. This is the maximum of
       CM_MAX_ALPHA_LEN_FOR_CALL_ORIG and
       CM_MAX_ALPHA_LEN_FOR_DISPLAY_INFO  */

/* Type for CM alpha.
** Able to store alpha tag text.
*/
typedef struct cm_alpha_s {

  uint8    buf[CM_MAX_ALPHA_TAG_CHARS];
    /* Alpha buffer */

  uint8    len;
    /* Length of alpha tag stored in alpha_buf */

} cm_alpha_s_type;

/* Type for CM signal types.
*/
typedef struct cm_signal_s {

  boolean     is_signal_info_avail;
    /* Indicate whether signal information is available.
    **
    ** NOTE! if this variable is FALSE, the values found in other
    ** fields of this structure as NO meaning */

  uint8       signal_type;
    /* Signal type - Tone Signal, ISDN Alerting or IS-54B Alerting */

  uint8       alert_pitch;
    /* Alert pitch - Medium, High or Low */

  uint8       signal;
    /* Indicate which Tone signals, ISDN Alerting or IS_54B Alerting */

} cm_signal_s_type;


#ifndef FEATURE_MULTIMODE_ARCH
/* Enumeration of call end statuses
** - i.e. the reason for ending a call.
*/
typedef enum cm_call_end_e {
    CM_CALL_END_NONE=-1,            /* FOR INTERNAL USE OF CM ONLY! */
    CM_CALL_END_OFFLINE,            /* phone is offline */
    CM_CALL_END_CDMA_LOCK,          /* phone is CDMA locked until power cycle */
    CM_CALL_END_NO_SRV,             /* phone has no service */
    CM_CALL_END_FADE,               /* Call has ended abnormally */
    CM_CALL_END_INTERCEPT,          /* received intercept from BS - orig only */
    CM_CALL_END_REORDER,            /* received reorder from BS - orig only */
    CM_CALL_END_REL_NORMAL,         /* received release from BS - no reason given */
    CM_CALL_END_REL_SO_REJ,         /* received release from BS - SO reject */
    CM_CALL_END_INCOM_CALL,         /* received incoming call from BS */
    CM_CALL_END_ALERT_STOP,         /* received alert stop from BS - incom only */
    CM_CALL_END_CLIENT_END,         /* client ended the call */
    CM_CALL_END_ACTIVATION,         /* received end activation - OTASP call only */
    CM_CALL_END_MC_ABORT,           /* MC aborts the origination/conversation*/
    /* Obsolete in SD 2.0 */
    CM_CALL_END_NDSS_FAIL,          /* NDSS failure */
    CM_CALL_END_MAX_ACCESS_PROBE,   /* Max access probes transmitted */
    /* JCDMA */
    CM_CALL_END_PSIST_NG,           /* Persistence test failure */
    /* RUIM */
    CM_CALL_END_RUIM_NOT_PRESENT,   /* RUIM is not present */
    /* New for SD 2.0 */
    CM_CALL_END_ACC_IN_PROG,        /* Access attempt already in progress */
    CM_CALL_END_ACC_FAIL,           /* Access failure for reason other than
                                    ** the above */
    /* IS-2000 */
    CM_CALL_END_RETRY_ORDER,        /* Received retry order - orig only */
    #ifdef FEATURE_HDR
#error code not present
    #endif /* FEATURE_HDR */
    CM_CALL_END_MAX                 /* FOR INTERNAL USE OF CM ONLY! */
#ifdef FEATURE_RPC
#error code not present
#endif /* FEATURE_RPC */
} cm_call_end_e_type;

#else /*FEATURE_MULTIMODE_ARCH*/

/* Enumeration of call end statuses
** - i.e. the reason for ending a call.
*/
/* For compatibility with QXDM database, please do not change the following
** fix assigned values. If new values are added please also update the QXDM
** database
*/
typedef enum cm_call_end_e {

  CM_CALL_END_NONE=-1,
    /* FOR INTERNAL USE OF CM ONLY! */

  /* Common
  */
  CM_CALL_END_OFFLINE=0,
    /* phone is offline */

  /* CDMA
  */
  CM_CALL_END_CDMA_LOCK=20,
    /* phone is CDMA locked until power cycle. CDMA only */

  CM_CALL_END_NO_SRV=21,
    /* phone has no service. CDMA only */

  CM_CALL_END_FADE=22,
    /* Call has ended abnormally. CDMA only */

  CM_CALL_END_INTERCEPT=23,
    /* received intercept from BS - orig only. CDMA only */

  CM_CALL_END_REORDER=24,
    /* received reorder from BS - orig only. CDMA only */

  CM_CALL_END_REL_NORMAL=25,
    /* received release from BS - no reason given. CDMA only */

  CM_CALL_END_REL_SO_REJ=26,
    /* received release from BS - SO reject. CDMA only */

  CM_CALL_END_INCOM_CALL=27,
    /* received incoming call from BS */

  CM_CALL_END_ALERT_STOP=28,
    /* received alert stop from BS - incom only. CDMA only */

  CM_CALL_END_CLIENT_END=29,
    /* client ended the call */

  CM_CALL_END_ACTIVATION=30,
    /* received end activation - OTASP call only. CDMA only */

  CM_CALL_END_MC_ABORT=31,
    /* MC aborts the origination/conversation. CDMA only */

  CM_CALL_END_MAX_ACCESS_PROBE=32,
    /* Max access probes transmitted. CDMA only */

  CM_CALL_END_PSIST_NG=33,
    /* Persistence test failure. FEATURE_JCDMA only. CDMA only */

  CM_CALL_END_UIM_NOT_PRESENT=34,
    /* RUIM is not present */

  CM_CALL_END_ACC_IN_PROG=35,
    /* Access attempt already in progress. */

  CM_CALL_END_ACC_FAIL=36,
    /* Access failure for reason other than the above */

  CM_CALL_END_RETRY_ORDER=37,
    /* Received retry order - orig only. IS-2000. CDMA only */

  CM_CALL_END_CCS_NOT_SUPPORTED_BY_BS=38,
    /* Concurrent servive is not supported by base station */

  CM_CALL_END_NO_RESPONSE_FROM_BS=39,
    /* No response recived from base station */

  CM_CALL_END_REJECTED_BY_BS=40,
    /* Call rejected by the base station. CDMA only */

  CM_CALL_END_INCOMPATIBLE=41,
    /* Concurrent services requested were not compatible. CDMA only */

  CM_CALL_END_ACCESS_BLOCK=42,
    /* Access is blocked by base station. CDMA only */

  CM_CALL_END_ALREADY_IN_TC=43,
    /* Corresponds to CM_CALL_ORIG_ERR_ALREADY_IN_TC */

  CM_CALL_END_EMERGENCY_FLASHED=44,
    /* Call is ended due to Emergency call is flashed over this call. CDMA only */

  CM_CALL_END_USER_CALL_ORIG_DURING_GPS=45,
    /* Used if CM is ending a GPS call in favor of a user call */

  /* GSM/WCDMA
  */
  CM_CALL_END_LL_CAUSE=100,
    /* rxd a reason from lower layer,look in cc_cause. WCDMA/GSM only*/

  CM_CALL_END_CONF_FAILED,
    /* call orig request failed. WCDMA/GSM only */

  CM_CALL_END_INCOM_REJ,
    /* client rejected the incoming call. WCDMA/GSM only */

  CM_CALL_END_SETUP_REJ,
    /* client rejected the setup_ind. WCDMA/GSM only */

  CM_CALL_END_NETWORK_END,
    /* network ended the call,look in cc_cause. WCDMA/GSM only*/

  CM_CALL_END_NO_FUNDS,
    /* GSM/WCDMA only */

  /* HDR
  */
  CM_CALL_END_CD_GEN_OR_BUSY=150,
    /* abort connection setup due to the reception of a ConnectionDeny msg
    ** with deny code = general or network busy */

  CM_CALL_END_CD_BILL_OR_AUTH=151,
    /* abort connection setup due to the reception of a ConnectionDeny msg
    ** with deny code = billing or authentication failure */

  CM_CALL_END_CHG_HDR=152,
    /* change HDR system due to redirection or PRL not preferred */

  CM_CALL_END_EXIT_HDR=153,
    /* exit HDR  due to redirection or PRL not preferred */

  CM_CALL_END_HDR_NO_SESSION=154,
    /* no HDR session  */

  CM_CALL_END_CM_COLLOC_ACQ_FAIL=155,
    /* only for internal CM use -Fail to acquire collocated HDR for
    ** origination*/

  CM_CALL_END_HDR_ORIG_DURING_GPS_FIX=156,
    /* Used if CM is ending a HDR call orig in favor of GPS fix */

  CM_CALL_END_HDR_CS_TIMEOUT=157,
    /* connection setup timeout*/

  CM_CALL_END_MAX
    /* FOR INTERNAL USE OF CM ONLY! */

  #ifdef FEATURE_RPC
  , CM_CALL_END_BIG=0x10000000
    /* To force enum to 32 bit for windows NT */
  #endif /* FEATURE_RPC */

} cm_call_end_e_type;

/* Extended Burst Type International message - FEATURE_T53
*/
typedef struct cm_ext_brst_intl_s {

  uint16   mcc;
   /* Mobile Country Code (fixed '0101010011')*/

  uint8    db_subtype;
   /* Data Burst subtype (fixed '000001') */

  uint8    chg_ind;
   /* Charge Indication */

  uint8    sub_unit;
   /* Unit call time (1/10 second) */

  uint8    unit;
   /* Unit call time (second) */

} cm_ext_brst_intl_s_type;


/* National Supplementary services - CLIR - FEATURE_T53
*/
typedef struct cm_nss_clir_rec_s {

  uint8   cause;
    /* Reason code */

} cm_nss_clir_rec_s_type;

/* National Supplementary services - Audio Control - FEATURE_T53
*/
typedef struct cm_nss_aud_ctrl_s {

  uint8   up_link;
    /* Audio path from the MS to the BS */

  uint8   down_link;
    /* Audio path from the BS to the MS */

} cm_nss_aud_ctrl_s_type;


#endif /*end FEATURE_MULTIMODE_ARCH*/

/* Type to hold call state information.
** When notified of call events, clients are given
** a reference to such a structure.
*/
#ifndef FEATURE_MULTIMODE_ARCH

typedef struct cm_call_info_s {

    cm_call_state_e_type   call_state;
        /* Call state - idle, orig, incom, conv */

    cm_client_id_type      call_client;
        /* Call client - originating/answering client */

    cm_call_type_e_type    call_type;
        /* Call type - VOICE, DATA, OTASP, etc */

    word                   srv_opt;
        /* Service option as defined in CAI.H file
        **
        ** NOTE! The service option takes on the CM_SO_ANALOG
        ** value (defined in this file) when the call is connected
        ** (or to be connected) over analog service */

    cm_num_s_type          num;
        /* Number buffer:
        ** CM_CALL_EVENT_ORIG      - contains dialed digits string.
        ** CM_CALL_EVENT_FLASH     - contains dialed digits string.
        ** CM_CALL_EVENT_INCOM     - contains caller ID string.
        ** CM_CALL_EVENT_CALLER_ID - contains caller ID string. */

    cm_alpha_s_type        alpha;
        /* Alpha tag to be displayed along with dialed address
        ** of an originated call.
        **
        ** NOTE! this is not a phone book name match, but rather
        ** an indication for the UI to display some text for an
        ** outgoing call
        **
        ** We probably need to change this one to a UI text component to
        ** support I18N
        **
        Alpha buffer:
        CM_CALL_EVENT_ORIG
        CM_CALL_EVENT_DISPLAY
        CM_CALL_EVENT_EXT_DISP        */

    cm_signal_s_type       signal;
        /* Alert signal information to be played for the incoming call */

    cm_privacy_pref_e_type privacy_pref;
      /* NOTE: This is the user preference. It is different from is_privacy
      ** which indicates the actual status of privacy during the call.
      */

    boolean                is_privacy;
        /* Indicate whether privacy mode is on */

    cm_call_end_e_type     end_status;
        /* Call end status (i.e. reason for ending a call in origination,
        incoming or conversation state) */

    boolean                is_otasp_commit;
        /* Indicate whether OTASP activation is successful */


    boolean                is_last_cdma_info_rec;
        /* this field indicates whether this is the last
           CDMA info record or not */

#ifdef FEATURE_IS95B_EXT_DISP
    byte                        ext_disp_type;
        /* display type for extended display info record */
#endif /* FEATURE_IS95B_EXT_DISP */


#ifdef FEATURE_IS95B_ALERT_ANS_BYPASS
    boolean                 alert_ans_bypass;
         /* Indicate whether the incoming call need to be answered by the clients */
#endif /* FEATURE_IS95B_ALERT_ANS_BYPASS */

#ifdef FEATURE_IS95B_NDSS
    boolean                     ndss_in_progress;
        /* MC is performing NDSS redirection */
#endif /* FEATURE_IS95B_NDSS */

#ifdef FEATURE_T53
    cm_ext_brst_intl_s_type       ext_brst_intl_msg;
    /* Contains charge rate information */

    cm_nss_clir_rec_s_type        nss_clir_rec;
    /* National Supplementary services - CLIR */

    cm_nss_aud_ctrl_s_type        nss_aud_ctrl_rec;
    /* National Supplementary services - Audio Control */
#endif /* FEATURE_T53 */

#ifdef FEATURE_IS2000
    byte end_reason;
      /* the reason for ending a call, it's different from end_status to keep
       * the old interface unchanged while adding more IS2000 specific reasons
       * */

    cm_retry_order_type retry_order;
      /* retry order info */
#endif /* FEATURE_IS2000 */

} cm_call_info_s_type;

#else    /*FEATURE_MULTIMODE_ARCH*/

/* Data type for call id */
typedef uint8 cm_call_id_type;

/* Enumeration of call mode info types
*/
typedef enum cm_call_mode_info_e {

  CM_CALL_MODE_INFO_NONE,
    /* FOR INTERNAL USE OF CM ONLY! */

  CM_CALL_MODE_INFO_CDMA,
    /* Call info is CDMA */

  CM_CALL_MODE_INFO_GW_CS,
    /* Call info is GSM/WCDMA circuit switched */

  CM_CALL_MODE_INFO_GW_PS,
    /* Call info is GSM/WCDMA packet switched */

  CM_CALL_MODE_INFO_MAX
    /* FOR INTERNAL USE OF CM ONLY! */

  #ifdef FEATURE_RPC
  , CM_CALL_MODE_INFO_BIG=0x10000000
    /* To force enum to 32 bit for windows NT */
  #endif /* FEATURE_RPC */

} cm_call_mode_info_e_type;

typedef enum cm_call_direction_e {
  CM_CALL_DIRECTION_NONE,
    /* Undefined Direction */

  CM_CALL_DIRECTION_MO,
    /* Mobile Originated Call */

  CM_CALL_DIRECTION_MT,
    /* Mobile Terminated Call */

  CM_CALL_DIRECTION_MAX
  /* FOR INTERNAL USE OF CM ONLY! */

  #ifdef FEATURE_RPC
  , CM_NUM_ERR_BIG=0x10000000
    /* To force enum to 32 bit for windows NT */
  #endif /* FEATURE_RPC */

} cm_call_direction_e_type;


/* Enumeration of call command supplementary service type
*/
typedef enum cm_call_sups_type_e {

  CM_CALL_SUPS_TYPE_NONE = -1,
    /* FOR INTERNAL USE OF CM ONLY! */

  CM_CALL_SUPS_TYPE_HELD_CALL_REL_OR_UDUB,
    /* 0 SEND - Release all held calls or
    **          set User Determined User Busy (UDUB)
    **          for a waiting call. */

  CM_CALL_SUPS_TYPE_ACTIVE_CALL_REL,
    /* 1 SEND - Release all active calls and
    **          accept other call (held or waiting) */

  CM_CALL_SUPS_TYPE_ACTIVE_CALL_X_REL,
    /* 1 X SEND - Release a specific call X */

  CM_CALL_SUPS_TYPE_HOLD_CALL,
    /* 2 SEND - Place all active calls on hold and
    **          accept another call (held or waiting) */

  CM_CALL_SUPS_TYPE_SELECT_CALL_X,
    /* 2 X SEND - Place all active calls on hold except X
    **            with which communication is still established. */

  CM_CALL_SUPS_TYPE_MULTIPARTY_CALL,
    /* 3 SEND - Add a held call to the conversation,
    **          Multiparty call */

  CM_CALL_SUPS_TYPE_CALL_DEFLECTION,
    /* 4 * DN SEND - Deflect an incoming call */

  CM_CALL_SUPS_TYPE_EXPLICIT_CALL_TRANSFER,
    /* 4 SEND - Explicit call transfer */

  CM_CALL_SUPS_TYPE_CALL_FORWARD,
    /* Call forward */

  CM_CALL_SUPS_TYPE_EMERGENCY_CALL,
    /* CDMA - indicates emergency call */

  CM_CALL_SUPS_TYPE_ACT_ANSWER_HOLD,
    /* CDMA - Activate answer hold */

  CM_CALL_SUPS_TYPE_DEACT_ANSWER_HOLD,
    /* CDMA - Deactivate answer hold */

  CM_CALL_SUPS_TYPE_SIMPLE_FLASH,
    /* CDMA - Send it as regular flash */

  CM_CALL_SUPS_TYPE_MAX
    /* FOR INTERNAL USE OF CM ONLY! */

  #ifdef FEATURE_RPC
  , CM_CALL_SUPS_TYPE_BIG=0x10000000
    /* To force enum to 32 bit for windows NT */
  #endif /* FEATURE_RPC */

} cm_call_sups_type_e_type;


/* Type for holding optional supplementary service parameters
*/
typedef struct cm_call_sups_params_s {

  cm_call_id_type   call_id;
    /* Optional call ID of the call */

  cm_num_s_type     sups_number;
    /* Optional User supplied number */

  uint8             cause;
    /* Optional cause value for GSM/WCDMA N/Nx SEND command */

} cm_call_sups_params_s_type;


/* Redirected call information */
typedef struct cm_route_dest_number_s
{
  boolean                           present;
  routing_number_option_s_type      type;
  cm_called_party_bcd_no_s_type     number[MAXNO_FORWARD_TO_NUMBER];
  cm_called_party_subaddress_s_type address;

} cm_route_dest_number_s_type;

/* Explicit Call Tranfer indicator */
typedef struct cm_ect_indicator_s
{
  boolean                         present;
  ect_call_state_s_type           callState; 
  cm_route_dest_number_s_type     routeDestNumber;

} cm_ect_indicator_s_type;

typedef struct cm_call_ss_info_s
{
  ie_ss_status_s_type        ss_status;
  ie_ss_code_s_type          ss_code;
  ie_ss_notification_s_type  ss_notification;
}cm_call_ss_info_s_type;

/* Circuit switched Call information returned with each event
*/
typedef struct cm_gw_cs_call_info_s {


  cm_call_direction_e_type            direction;
    /* Call direction: MO, MT, NONE */

  uint8                               transaction_id;
    /* Call transaction ID */

  boolean                             accept;
    /* Accept the call */

  boolean                             success;
    /* Confirmation message from the lower layer is successful */


  cm_repeat_indicator_s_type          bc_repeat_indicator;
    /* Bearer capability repeat indicator - included only if
    ** bearer capability 1 and bearer capability 2 are included */

  cm_bearer_capability_s_type         bearer_capability_1;
    /* Bearer Capability 1 */

  cm_bearer_capability_s_type         bearer_capability_2;
    /* Bearer Capability 2*/

  cm_repeat_indicator_s_type          llc_repeat_indicator;
    /* LLC repeat indicator - included only if BC repeat indicator
    ** and low layer compatibility 1 are included */

  cm_low_layer_compatibility_s_type   low_layer_compatibility_1;
    /* Low Layer Compatibility 1 - included in the Network -> UE
    ** direction if the calling user specifies a  low layer
    ** compatibility */

  cm_low_layer_compatibility_s_type   low_layer_compatibility_2;
    /* Low Layer Compatibility 2 - included only if LLC repeat indicator
    ** is included */

  cm_repeat_indicator_s_type          hlc_repeat_indicator;
    /* HLC repeat indicator - included only if BC repeat indicator
    ** and high layer compatibility 1 are included */

  cm_high_layer_compatibility_s_type  high_layer_compatibility_1;
    /* High Layer compatibility 1 - included in the Network -> UE
    ** direction if the calling user specifies a  high layer
    ** compatibility
    */

  cm_high_layer_compatibility_s_type  high_layer_compatibility_2;
    /* High Layer compatibility 2 - included only if HLC repeat
    ** indicator is included */

  cm_called_party_bcd_no_s_type       called_party_bcd_number;
    /* Called party BCD number */

  cm_called_party_subaddress_s_type   called_party_subaddress;
    /* Called party subaddress */

  cm_calling_party_bcd_no_s_type      calling_party_bcd_number;
    /* Calling party BCD number */

  cm_calling_party_subaddress_s_type  calling_party_subaddress;
    /* Calling party subaddress */

  cm_num_s_type                       redirect_party_number;
    /* Redirecting party number */

  cm_forward_cug_info_s_type          forward_cug_info;
    /* Closed user Group information */

  uint32                              rab_id;
    /* Radio Access Bearer ID */

  uint8                               cm_clir;
    /* CLIR information - CLIR suppression / invocation etc. */

  ie_cm_cc_cause_s_type               cc_cause;
    /* Call control cause information - eg: */

  ie_cm_mo_call_details_s_type        call_details;
    /*  Bearer capability information for mobile originated
    **  calls */

  ie_cm_ss_error_s_type               ss_error;
    /* Supplementary service error */

  boolean                             call_ss_success;
    /* Flag to indicate whether multiple call management
    ** is successful or not */

  active_calls_list_s_type            active_calls_list;
    /* Active call ID list from multiple calls management
    ** confirmation */

  cm_connected_no_s_type              connected_num;
    /* Connected number */

  cm_connected_subaddress_s_type      connected_subaddress;
    /* Connected subaddress */

  cm_call_sups_type_e_type            sups_type;
     /* Supplementary services type */

  cm_call_sups_params_s_type          sups_params;
    /* supplementary service specific data */

  uint8                               call_progress;
    /* call origination progress information */

  cm_cause_of_no_cli_s_type           cause_of_no_cli;
    /* cause for no CLI in mobile terminated call */

  cm_ect_indicator_s_type             ect_info;
    /* explicit call transfer information */

  cm_call_ss_info_s_type              ss_info;
    /* call related SS information */

} cm_gw_cs_call_info_s_type;


/* Packet switched Call information returned with each event
*/
typedef struct cm_gw_ps_call_info_s {

  nsapi_T                         nsapi;
    /* NSAPI information */

  qos_T                           qos;
    /* Quality of Service information */

  pdp_address_T                   pdp_addr;
    /* Packet data protocol address */

  protocol_cfg_options_T          config_options;
    /* Protocol Configuration Options */

  apn_T                           apn_name;
    /* Access point  name - a label or a full
    ** qualified domain name */

  ti_T                            tear_down_ind;
    /* Tear down indicator - indicates whether
    ** only the PDP context associated with this specific
    ** TI or all active PDP contexts sharing the same PDP
    ** address associated with this specific TI shall be deactivated. */

  cause_enum_type_T               pdp_cause_type;
    /* PDP cause type */

  pdp_cause_T                     cause;
    /* PDP cause */

  rabm_rabid_T                    rabid;
    /* RAB Id */

  rabmcm_cause_T                  rab_cause;
    /* RAB cause */

} cm_gw_ps_call_info_s_type;

/* Retry order info
*/
typedef struct cm_retry_order_s {

  cm_retry_type_e_type retry_type;
    /* Retry delay type */

  uint16               srv_opt;
    /* Service option   */

  boolean              infinite_delay;
    /* True if infinite delay, it should always be FALSE for origination */

  uint32               retry_delay_in_ms;
    /* Only meaningful if infinite_delay is FALSE */

} cm_retry_order_s_type;

/* CDMA call information returned with each event
** Applicable for FEATURE_CDMA_800 or FEATURE_CDMA_1900.
*/
typedef struct cm_cdma_call_info_s {

  uint16                          srv_opt;
    /* Service option as defined in CAI.H file
    **
    ** NOTE! The service option takes on the CM_SO_ANALOG
    ** value (defined in this file) when the call is connected
    ** (or to be connected) over analog service */

  cm_privacy_pref_e_type          privacy_pref;
    /* NOTE: This is the user preference. It is different from is_privacy
    ** which indicates the actual status of privacy during the call. */

  boolean                         is_privacy;
    /* Indicate whether privacy mode is on */

  boolean                         is_otasp_commit;
    /* Indicate whether OTASP activation is successful */

  boolean                         is_last_cdma_info_rec;
    /* this field indicates whether this is the last
    ** CDMA info record or not */

  cm_call_sups_type_e_type        sups_type;
      /* Supplementary services (flash) type */

  cm_call_id_type                 emergency_flash_call_id;
      /* The call used in sending emergency flash */

  boolean                         emergency_flash_over_incoming_call;
      /* whether a previous emergency request was flashed on this
      incoming call or not */

  boolean                         qos_parms_incl;
    /* Qos Parameters presense indicator. TRUE - Present; FALSE - Not */

  uint8                           qos_parms_len;
    /* Length of QOS parameter list */

  uint8                           qos_parms[CM_ORIG_QOS_LEN_MAX];
    /* QOS parameters list */

  uint8 end_reason;
    /* the reason for ending a call, it's different from end_status to keep
    ** the old interface unchanged while adding more IS2000 specific reasons */

  cm_retry_order_s_type           retry_order;
    /* retry order info */

  uint8                           ext_disp_type;
    /* display type for extended disaply info record - FEATURE_IS95B_EXT_DISP */

  boolean                         alert_ans_bypass;
    /* Indicate whether the incoming call need to be answered by the clients
    ** FEATURE_IS95B_ALERT_ANS_BYPASS */

  boolean                         ndss_in_progress;
    /* MC is perfoming NDSS redirection - FEATURE_IS95B_NDSS */

  cm_ext_brst_intl_s_type         ext_brst_intl_msg;
    /* Contains charge rate information - FEATURE_T53 */

  cm_nss_clir_rec_s_type          nss_clir_rec;
    /* National Supplementary services - CLIR - FEATURE_T53 */

  cm_nss_aud_ctrl_s_type          nss_aud_ctrl_rec;
    /* National Supplementary services - Audio Control - FEATURE_T53 */

} cm_cdma_call_info_s_type;


typedef union cm_call_mode_info_u {

  cm_gw_cs_call_info_s_type     gw_cs_call;
      /* CS GSM or WCDMA specific info */

  cm_gw_ps_call_info_s_type     gw_ps_call;
      /* PS GSM or WCDMA specific info */

  cm_cdma_call_info_s_type      cdma_call;
      /* CDMA specific info */

} cm_call_mode_info_u_type;


/* Call End parameters for GSM/WCDMA circuit switched call
** set by the clients.
*/
typedef struct cm_gw_cs_end_params_s {

  uint8                      reserved;
    /* empty for now */

} cm_gw_cs_end_params_s_type;


/* Call End parameters for GSM/WCDMA packet switched call
** set by the clients.
*/
typedef struct cm_gw_ps_end_params_s {

  nsapi_T                    nsapi;
    /* NSAPI */

  ti_T                       tear_down_ind;
    /* Tear down indicator - indicates whether
       only the PDP context associated with this specific
       TI or all active PDP contexts sharing the same PDP
       address associated with this specific TI shall be deactivated.
    */

  sm_status_T                cause;
    /* Cause for ending the call */

} cm_gw_ps_end_params_s_type;


/* CDMA call end parameter structure
** Applicable for FEATURE_CDMA_800 or FEATURE_CDMA_1900.
*/
typedef struct cm_cdma_end_params_s {

  boolean      end_reason_included;
    /* End reason included or not */

  uint8        end_reason;
    /* End reson */

} cm_cdma_end_params_s_type;

/* Union of all mode call end parameters
*/
typedef union cm_end_params_u {

  cm_gw_cs_end_params_s_type           gw_cs_end;
    /* GSM/WCDMA circuit switched call end parameters */

  cm_gw_ps_end_params_s_type           gw_ps_end;
    /* GSM/WCDMA packet switched call end parameters */

  cm_cdma_end_params_s_type            cdma_end;
    /* CDMA call end parameters */

} cm_end_params_u_type;


/* Call end parameters depending on the mode - CDMA, GSM, WCDMA etc.
*/
typedef struct cm_end_params_s {

  cm_call_id_type           call_id;
    /* Call ID to end */

  cm_call_mode_info_e_type  info_type;
    /* Mode type */

  cm_end_params_u_type      end_params;
    /* mode specific end parameters */

} cm_end_params_s_type;

typedef struct cm_call_mode_info_s {

  cm_call_mode_info_e_type  info_type;
    /* Which mode information is present */

  cm_call_mode_info_u_type  info;
    /* Mode information */

} cm_call_mode_info_s_type;

/* Type to hold call state information.
** When notified of call events, clients are given
** a reference to such a structure.
*/
typedef struct cm_mm_call_info_s {

  cm_client_id_type         call_client_id;
    /* Call client - originating/answering client */

  cm_call_id_type           call_id;
    /* call ID of the call */

  cm_call_state_e_type      call_state;
    /* Call state - idle, orig, incom, conv */

  cm_call_type_e_type       call_type;
    /* Call type - VOICE, DATA, OTASP, etc */

  cm_num_s_type             num;
    /* Number buffer:
    ** CM_CALL_EVENT_ORIG      - contains dialed digits string.
    ** CM_CALL_EVENT_FLASH     - contains dialed digits string.
    ** CM_CALL_EVENT_INCOM     - contains caller ID string.
    ** CM_CALL_EVENT_CALLER_ID - contains caller ID string. */

  cm_alpha_s_type           alpha;
    /* Alpha tag to be displayed along with dialed address
    ** of an originated call.
    **
    ** NOTE! this is not a phone book name match, but rather
    ** an indication for the UI to display some text for an
    ** outgoing call
    **
    ** We probably need to change this one to a UI text component to
    ** support I18N
    **
    Alpha buffer:
    CM_CALL_EVENT_ORIG
    CM_CALL_EVENT_DISPLAY
    CM_CALL_EVENT_EXT_DISP        */

  cm_signal_s_type          signal;
    /* Alert signal information to be played for the incoming call */

  cm_call_end_e_type        end_status;
    /* Call end status (i.e. reason for ending a call in origination,
    incoming or conversation state) */

  uint8                     num_call_ids_ended;
    /* number of calls ended */

  cm_end_params_s_type      end_params;
    /* call end information */

  cm_call_mode_info_s_type  mode_info;
    /* System specific info - CDMA, GSM/WCDMA-CS, GSM/WCDMA-PS */

} cm_mm_call_info_s_type;

#endif    /*end FEATURE_MULTIMODE_ARCH*/


/* Enumeration of service types.
**
** When originating a call, clients need to specify
** one of these service types.
*/
/* For compatibility with QXDM database, please do not change the following
** fix assigned values. If new values are added please also update the QXDM
** database
*/
typedef enum cm_srv_type_e {

  CM_SRV_TYPE_NONE = -1,
    /* FOR INTERNAL USE OF CM ONLY! */

  CM_SRV_TYPE_AMPS=0,
    /* Call is to be connected over analog service.
    **
    ** Note that the client does NOT need to specify service option when
    ** originating with this call type.
    **
    ** NOTE! When originating with this service type, the mode preference
    ** might be forced to ANALOG ONLY before origination and returned to
    ** its original setting at the end of the call */

  CM_SRV_TYPE_HDR=1,
    /* Call is to be connected over HDR service.
    ** Valid for FEATURE_HDR */

  CM_SRV_TYPE_CDMA_SPECIFIC=2,
    /* Call is to be connected over CDMA service with specific client
    ** designated CDMA service option.
    **
    ** Note that the client need to specify service option when
    ** originating with this call type.
    **
    ** NOTE! When originating with this service type, the mode preference
    ** might be forced to DIGITAL ONLY before origination and returned to
    ** its original setting at the end of the call */

  CM_SRV_TYPE_CDMA_AUTOMATIC=3,
    /* Call is to be connected over CDMA service where CM automatically
    ** originate with the best available service option for the specified
    ** call type.
    **
    ** Note that the client does NOT need to specify service option when
    ** originating with this call type */

  CM_SRV_TYPE_AUTOMATIC=4,
    /* Call is to be connected over available service - AMPS or CDMA,
    ** or GSM or WCDMA or HDR.
    **
    ** Note that the client does NOT need to specify service option when
    ** originating with this srv type except for the packet data call*/

  CM_SRV_TYPE_CDMA_HDR=5,
    /* Call is to be connected over available service - CDMA or HDR.
    **
    ** Note that the client need to specify service option when
    ** originating with this srv type for CDMA. */

  CM_SRV_TYPE_GSM_WCDMA=6,
    /* Call is to be connected over available service - GSM or WCDMA */

  CM_SRV_TYPE_MAX
    /* FOR INTERNAL USE OF CM ONLY! */

  #ifdef FEATURE_RPC
    , CM_SRV_TYPE_BIG=0x10000000
      /* To force enum to 32 bit for windows NT */
  #endif /* FEATURE_RPC */

} cm_srv_type_e_type;


/*===========================================================================
                     Dummy definition for CM Phone Info events
===========================================================================*/
typedef void cm_ph_info_s_type;

/*===========================================================================
                     following stuff was taken from AEEPhone.h
===========================================================================*/

/*
** SOME TYPE DEFINITIONS THAT HAPPEN TO HAVE CHANGED FOR MULTIMODE ARCH
*/
#ifdef FEATURE_MULTIMODE_ARCH
typedef cm_mm_call_info_s_type    CM_CALL_INFO_S_TYPE;
typedef cm_activate_code_e_type   CM_ACTIVATE_CODE_TYPE;
#else
typedef cm_call_info_s_type       CM_CALL_INFO_S_TYPE;
typedef cm_activate_code_type     CM_ACTIVATE_CODE_TYPE;
#endif

typedef struct _IPhone  IPhone;

// Notification Masks...

#define NMASK_PH_PHONE_INFO   0x0100      // AEENotify->pData = cm_ph_info_s_type *
#define NMASK_PH_CALL_INFO    0x0200      // AEENotify->pData = cm_call_info_s_type *
#define NMASK_PH_SS_INFO      0x0400      // AEENotify->pData = cm_ss_info_s_type *
#define NMASK_PH_RAW_SMS      0x0800      // AEENotify->pData = uasms_client_message_struct *

// GSM1x SMS Teleservice Id's
#define GSM1xSIG_TELESERVICE_MIN  4104
#define GSM1xSIG_TELESERVICE_SIG  4104  /* GSM1X signalling */
#define GSM1xSIG_TELESERVICE_SMS  4105  /* GSM1X SMS including page, and voicemail */
#define GSM1xSIG_TELESERVICE_MAX  4105   

// Interface

QINTERFACE(IPhone)
{
   DECLARE_IBASE(IPhone)
   DECLARE_INOTIFIER(IPhone)

   void* p1;
   const CM_CALL_INFO_S_TYPE *      (*GetCallInfo)(IPhone * po);
   void* p3;
   void* p4;
   void* p5;
   void* p6;
   int                              (*MakeCall)(IPhone *po,
                                                cm_call_type_e_type cType,
                                                cm_srv_type_e_type nServ,
                                                word nServOpt,
                                                const byte * pNum,
                                                byte nNumLen,
                                                cm_digit_mode_e_type nDigitMode,
                                                const byte * pAlpha,
                                                byte nAlphaLen,
                                                CM_ACTIVATE_CODE_TYPE nActivateCode);

   int                              (*AnswerCall)(IPhone * po,cm_call_type_e_type ct);
   void* p9;
   int                              (*EndCall)(IPhone * po);
   void*     p11;
   void*     p12;
   void*     p13;
   void*     p14;
   int                           (*BurstDTMF)(IPhone * po, uint8 nOn, uint8 nOff, const uint8 * pDigits, uint8 nLen);
   int                           (*StartDTMF)(IPhone * po, uint8 nDigit);
   int                           (*StopDTMF)(IPhone * po, uint16 nDuration);
   void*     p18;
   void*     p19;
   void*     p20;
   void*     p21;
};


#define IPHONE_AddRef(p)                                 GET_PVTBL(p,IPhone)->AddRef(p)
#define IPHONE_Release(p)                                GET_PVTBL(p,IPhone)->Release(p)
//#define IPHONE_GetPhoneInfo(p)                           GET_PVTBL(p,IPhone)->GetPhoneInfo(p)
#define IPHONE_GetCallInfo(p)                            GET_PVTBL(p,IPhone)->GetCallInfo(p)
//#define IPHONE_GetSystemInfo(p)                          GET_PVTBL(p,IPhone)->GetSystemInfo(p)
//#define IPHONE_GetMinWStr(p,nn,pd,n)                     GET_PVTBL(p,IPhone)->GetMinWStr(p,nn,pd,n)
//#define IPHONE_GetMinStr(p,nn,pd,n)                      GET_PVTBL(p,IPhone)->GetMinStr(p,nn,pd,n)
//#define IPHONE_SetRSSIThreshold(p,n)                     GET_PVTBL(p,IPhone)->SetRSSIThreshold(p,n)
#define IPHONE_MakeCall(p,ct,ns,no,pn,nn,nd,pa,na,nac)   GET_PVTBL(p,IPhone)->MakeCall(p,ct,ns,no,pn,nn,nd,pa,na,nac)
#define IPHONE_AnswerCall(p,ct)                          GET_PVTBL(p,IPhone)->AnswerCall(p,ct)
//#define IPHONE_AnswerVoice(p,na,dw)                      GET_PVTBL(p,IPhone)->AnswerVoice(p,na,dw)
#define IPHONE_EndCall(p)                                GET_PVTBL(p,IPhone)->EndCall(p)
//#define IPHONE_Flash(p,pn,n)                             GET_PVTBL(p,IPhone)->Flash(p,pn,n)
//#define IPHONE_SetPrefMode(p,n,nm,dw)                    GET_PVTBL(p,IPhone)->SetPrefMode(p,n,nm,dw)
//#define IPHONE_SetNAM(p,n)                               GET_PVTBL(p,IPhone)->SetNAM(p,n)
//#define IPHONE_SetOprtMode(p,n)                          GET_PVTBL(p,IPhone)->SetOprtMode(p,n)
#define IPHONE_BurstDTMF(p,nOn,nOff,pd,n)                GET_PVTBL(p,IPhone)->BurstDTMF(p,nOn,nOff,pd,n)
#define IPHONE_StartDTMF(p,n)                            GET_PVTBL(p,IPhone)->StartDTMF(p,n)
#define IPHONE_StopDTMF(p,n)                             GET_PVTBL(p,IPhone)->StopDTMF(p,n)
//#define IPHONE_OnCallStatus(p,pfn,pu,dwd,bs)             GET_PVTBL(p,IPhone)->OnCallStatus(p,pfn,pu,dwd,bs)
//#define IPHONE_OnCallEnd(p,pfn,pu,dwd,bs)                GET_PVTBL(p,IPhone)->OnCallStatus(p,pfn,pu,dwd,bs)
//#define IPHONE_IsVoiceCall(p)                            GET_PVTBL(p,IPhone)->IsVoiceCall(p)
//#define IPHONE_IsDataSupported(p)                        GET_PVTBL(p,IPhone)->IsDataSupported(p)


#endif    // GSM1XPHONE_H


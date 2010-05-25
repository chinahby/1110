/*======================================================
                    IMPORTANT NOTICE

                  OEM DISTRIBUTION ONLY

This file is covered by the licensing terms for CDMA 
ASIC licensees.  Under terms of those licensing agreements, 
this file MUST NOT be transmitted in whole or in part 
to any party who has not been licensed for use of the
QUALCOMM CDMA technologies. 

FILE:  OEMCM.h

SERVICES:  AEE OEM CM Interface header

GENERAL DESCRIPTION:

This file contains all of the function prototypes for
routines that are required by the AEE to function with
QC's Call Manager.


        Copyright © 1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#ifndef OEMCM_H
#define OEMCM_H

// defines that comes from custsurf.h
#define FEATURE_IS95B_EXT_DISP
#define FEATURE_IS95B_ALERT_ANS_BYPASS
#define FEATURE_IS95B_NDSS
#define FEATURE_IS2000

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
/*---------------------------------------------------------------------------
                             RETRY ORDER TYPES
---------------------------------------------------------------------------*/

typedef enum cm_oprt_mode_e_ {

    CM_OPRT_MODE_NONE=-1,   /* FOR INTERNAL USE OF CM ONLY! */

    CM_OPRT_MODE_PWROFF,    /* phone is powering off */
    CM_OPRT_MODE_OFFLINE,   /* phone is offline Digital*/
#ifdef FEATURE_ACP
    CM_OPRT_MODE_OFFLINE_A, /* phone is offline analog */
#endif
    CM_OPRT_MODE_ONLINE,    /* phone is online */
    CM_OPRT_MODE_LPM,       /* phone is in LPM - Low Power Mode */
    CM_OPRT_MODE_RESET,     /* phone is resetting - i.e. power-cycling */

    CM_OPRT_MODE_MAX        /* FOR INTERNAL USE OF CM ONLY! */

#ifdef FEATURE_RPC
#error code not present
#endif

} cm_oprt_mode_e_type;

typedef enum cm_srv_state_e {

    CM_SRV_STATE_NONE=-1,       /* FOR INTERNAL USE OF CM ONLY! */

    CM_SRV_STATE_NO_SRV,        /* phone has no service */
    CM_SRV_STATE_ANALOG,        /* phone has analog service */
    CM_SRV_STATE_DIGITAL,       /* phone has digital service */

    CM_SRV_STATE_MAX            /* FOR INTERNAL USE OF CM ONLY! */

#ifdef FEATURE_RPC
#error code not present
#endif

} cm_srv_state_e_type;

typedef enum cm_cdma_lock_mode_e {

    CM_CDMA_LOCK_MODE_NONE=-1,  /* FOR INTERNAL USE OF CM ONLY! */

    CM_CDMA_LOCK_MODE_OFF,      /* phone is NOT CDMA locked */
    CM_CDMA_LOCK_MODE_ON,       /* phone is CDMA locked until power-cycle */

    CM_CDMA_LOCK_MODE_MAX       /* FOR INTERNAL USE OF CM ONLY! */

#ifdef FEATURE_RPC
#error code not present
#endif

} cm_cdma_lock_mode_e_type;

typedef enum {
  DB_VOICE_AS_DATA_NEVER,         /* Never answer voice call as a data call */
  DB_VOICE_AS_FAX_ONCE,           /* Answer voice as fax once               */
  DB_VOICE_AS_FAX_ALWAYS,         /* Always answer voice as fax             */
  DB_VOICE_AS_MODEM_ONCE,         /* Answer voice as modem once             */
  DB_VOICE_AS_MODEM_ALWAYS        /* Always answer voice as modem           */
} db_voice_as_data_type;

typedef enum cm_answer_voice_e {

    CM_ANSWER_VOICE_NONE=-1,          /* FOR INTERNAL USE OF CM ONLY! */

#ifdef FEATURE_NEWCM

    CM_ANSWER_VOICE_AS_VOICE,         /* = DB_VOICE_AS_DATA_NEVER, */
        /* Answer voice as voice */

    CM_ANSWER_VOICE_AS_FAX_ONCE,      /* = DB_VOICE_AS_FAX_ONCE, */
        /* Answer voice as fax once (for the next 10 minutes) */

    CM_ANSWER_VOICE_AS_FAX_ALWAYS,    /* = DB_VOICE_AS_FAX_ALWAYS, */
         /* Answer voice as fax always */

    CM_ANSWER_VOICE_AS_MODEM_ONCE,    /* = DB_VOICE_AS_MODEM_ONCE, */
        /* Answer voice as modem once (for the next 10 minutes) */

    CM_ANSWER_VOICE_AS_MODEM_ALWAYS,  /* = DB_VOICE_AS_MODEM_ALWAYS, */
        /* Answer voice as modem always */

    CM_ANSWER_VOICE_MAX               /* FOR INTERNAL USE OF CM ONLY! */

#ifdef FEATURE_RPC
#error code not present
#endif

#else /* FEATURE_NEWCM */
    CM_ANSWER_VOICE_AS_VOICE = DB_VOICE_AS_DATA_NEVER,
        /* Answer voice as voice */

    CM_ANSWER_VOICE_AS_FAX_ONCE = DB_VOICE_AS_FAX_ONCE,
        /* Answer voice as fax once (for the next 10 minutes) */

    CM_ANSWER_VOICE_AS_FAX_ALWAYS = DB_VOICE_AS_FAX_ALWAYS,
         /* Answer voice as fax always */

    CM_ANSWER_VOICE_AS_MODEM_ONCE = DB_VOICE_AS_MODEM_ONCE,
        /* Answer voice as modem once (for the next 10 minutes) */

    CM_ANSWER_VOICE_AS_MODEM_ALWAYS = DB_VOICE_AS_MODEM_ALWAYS,
        /* Answer voice as modem always */

    CM_ANSWER_VOICE_MAX               /* FOR INTERNAL USE OF CM ONLY! */

#endif /* FEATURE_NEWCM */


} cm_answer_voice_e_type;

typedef enum {
  NV_MODE_DIGITAL_PREF,     /* CDMA then Analog */
  NV_MODE_DIGITAL_ONLY,     /* CDMA only */
  NV_MODE_ANALOG_PREF,      /* Analog then CDMA */
  NV_MODE_ANALOG_ONLY,      /* Analog only */
  NV_MODE_AUTOMATIC,        /* Determine mode automatically */
  NV_MODE_E911,             /* Emergency mode */
  NV_MODE_HOME_ONLY,        /* Restrict to home only */
  NV_MODE_PCS_CDMA_ONLY,
  NV_MODE_CELL_CDMA_ONLY,
  NV_MODE_ENUM_PAD = 0x7FFF /* Pad to 16 bits on ARM */
} nv_mode_enum_type; 

typedef enum cm_mode_pref_e {

    CM_MODE_PREF_PERSISTENT=-2,
        /* Return to persistent mode preference */

    CM_MODE_PREF_NONE=-1,     /* FOR INTERNAL USE OF CM ONLY! */


#ifdef FEATURE_NEWCM

    CM_MODE_PREF_ANALOG_ONLY, /* = NV_MODE_ANALOG_ONLY, */
        /* Service is limited to analog only */

    CM_MODE_PREF_DIGITAL_ONLY,/* = NV_MODE_DIGITAL_ONLY, */
        /* Service is limited to digital only */


    CM_MODE_PREF_AUTOMATIC,   /* = NV_MODE_AUTOMATIC, */
        /* Determine the mode automatically from the PRL order */

    CM_MODE_PREF_E911,        /* = NV_MODE_E911, */
        /* Emergency mode */

#if ( defined(FEATURE_CDMA_800) && defined(FEATURE_CDMA_1900) )
    CM_MODE_PREF_PCS_CDMA_ONLY, /* = NV_MODE_PCS_CDMA_ONLY, */
        /* PCS mode */
    
    CM_MODE_PREF_CELL_CDMA_ONLY, /*= NV_MODE_CELL_CDMA_ONLY, */
        /* Cellular mode */
#endif

    CM_MODE_PREF_MAX          /* FOR INTERNAL USE OF CM ONLY! */

#ifdef FEATURE_RPC
#error code not present
#endif

#else /* FEATURE_NEWCM */
    CM_MODE_PREF_ANALOG_ONLY  = NV_MODE_ANALOG_ONLY,
        /* Service is limited to analog only */

    CM_MODE_PREF_DIGITAL_ONLY = NV_MODE_DIGITAL_ONLY,
        /* Service is limited to digital only */


    CM_MODE_PREF_AUTOMATIC    = NV_MODE_AUTOMATIC,
        /* Determine the mode automatically from the PRL order */

    CM_MODE_PREF_E911         = NV_MODE_E911,
        /* Emergency mode */

#if ( defined(FEATURE_CDMA_800) && defined(FEATURE_CDMA_1900) )
    CM_MODE_PREF_PCS_CDMA_ONLY= NV_MODE_PCS_CDMA_ONLY,
        /* PCS mode */
    
    CM_MODE_PREF_CELL_CDMA_ONLY= NV_MODE_CELL_CDMA_ONLY,
        /* Cellular mode */
#endif

    CM_MODE_PREF_MAX          /* FOR INTERNAL USE OF CM ONLY! */
#endif /* FEATURE_NEWCM */

} cm_mode_pref_e_type;

typedef enum cm_mode_term_e {

  CM_MODE_TERM_NONE=-1,      /* FOR INTERNAL USE OF CM ONLY! */


  CM_MODE_TERM_PERMANENT,
      /* Permanent mode change */

  CM_MODE_TERM_PWR_CYCLE,
      /* Until power cycle */

  CM_MODE_TERM_1_CALL,
      /* Until end of next call or power-cycle */

  CM_MODE_TERM_1_CALL_OR_TIME,
      /* Until end of next call, specified timeout or power-cycle */

  CM_MODE_TERM_CM_1_CALL,
  CM_MODE_TERM_CM_1_CALL_PLUS,
      /* Mode terms that are assigned automatically by call manager
      ** when mode is forced on the fly during a call origination.
      **
      ** CM unforce the mode back to it original settings at end of
      ** call or some short time after end of call.
      **
      ** NOTE! These mode term should NOT be used by clients
      ** when changing the mode preference */


  CM_MODE_TERM_MAX           /* FOR INTERNAL USE OF CM ONLY! */

#ifdef FEATURE_RPC
#error code not present
#endif

} cm_mode_term_e_type;

typedef enum {
  NV_SP_A_ONLY=0,          /* A only */
  NV_SP_B_ONLY,            /* B only */
  NV_SP_HOME_ONLY,         /* Home only */
  NV_SP_STANDARD,          /* Home preferred */
  NV_SYS_PREF_ENUM_PAD = 0x7FFF /* Pad to 16 bits on ARM */
} nv_sys_pref_enum_type;

typedef enum cm_sys_pref_e {

    CM_SYS_PREF_NONE=-1,      /* FOR INTERNAL USE OF CM ONLY! */


#ifdef FEATURE_NEWCM

    CM_SYS_PREF_A_ONLY,       /* = NV_SP_A_ONLY, */
        /* Service is limited to A system only
        ** NOTE! SHOULD ONLY BE USED IN 800 MHz TARGETS */

    CM_SYS_PREF_B_ONLY,       /* = NV_SP_B_ONLY, */
        /* Service is limited to B system only
        ** NOTE! SHOULD ONLY BE USED IN 800 MHz TARGETS */

    CM_SYS_PREF_HOME_ONLY,    /* = NV_SP_HOME_ONLY, */
        /* Service is limited to home system only */

    CM_SYS_PREF_STANDARD,     /* = NV_SP_STANDARD, */
        /* Home service is preferred */


    CM_SYS_PREF_MAX           /* FOR INTERNAL USE OF CM ONLY! */

#ifdef FEATURE_RPC
#error code not present
#endif

#else /* FEATURE_NEWCM */
    CM_SYS_PREF_A_ONLY     = NV_SP_A_ONLY,
        /* Service is limited to A system only
        ** NOTE! SHOULD ONLY BE USED IN 800 MHz TARGETS */

    CM_SYS_PREF_B_ONLY     = NV_SP_B_ONLY,
        /* Service is limited to B system only
        ** NOTE! SHOULD ONLY BE USED IN 800 MHz TARGETS */

    CM_SYS_PREF_HOME_ONLY  = NV_SP_HOME_ONLY,
        /* Service is limited to home system only */

    CM_SYS_PREF_STANDARD   = NV_SP_STANDARD,
        /* Home service is preferred */


    CM_SYS_PREF_MAX           /* FOR INTERNAL USE OF CM ONLY! */

#endif /* FEATURE_NEWCM */

} cm_sys_pref_e_type;

typedef enum {
  DB_ROAM_OFF,
  DB_ROAM_ON,
  DB_ROAM_BLINK
} db_roam_type;

typedef enum cm_roam_status_e {

    CM_ROAM_STATUS_NONE=-1,   /* FOR INTERNAL USE OF CM ONLY! */

#ifdef FEATURE_NEWCM
    
    CM_ROAM_STATUS_OFF,       /* = DB_ROAM_OFF, */
    CM_ROAM_STATUS_ON,        /* = DB_ROAM_ON, */
    CM_ROAM_STATUS_BLINK,     /* = DB_ROAM_BLINK, */

    CM_ROAM_STATUS_MAX        /* FOR INTERNAL USE OF CM ONLY! */

#ifdef FEATURE_RPC
#error code not present
#endif

#else /* FEATURE_NEWCM */
    CM_ROAM_STATUS_OFF   = DB_ROAM_OFF,
    CM_ROAM_STATUS_ON    = DB_ROAM_ON,
    CM_ROAM_STATUS_BLINK = DB_ROAM_BLINK,

    CM_ROAM_STATUS_MAX        /* FOR INTERNAL USE OF CM ONLY! */
#endif /* FEATURE_NEWCM */

} cm_roam_status_e_type;

typedef enum cm_nam_e {

    CM_NAM_NONE=-1,         /* FOR INTERNAL USE OF CM ONLY! */

    CM_NAM_1,               /* NAM 1*/
    CM_NAM_2,               /* NAM 2 - for targets with more than 1 NAM */
    CM_NAM_3,               /* NAM 3 - for targets with more than 2 NAM's */
    CM_NAM_4,               /* NAM 4 - for targets with more than 3 NAM's */
    CM_NAM_AUTO,          /* Auto NAM - for targets with more than 1 NAM */

    CM_NAM_MAX              /* FOR INTERNAL USE OF CM ONLY! */

#ifdef FEATURE_RPC
#error code not present
#endif

} cm_nam_e_type;

/* Retry delay type */
typedef enum
{
  CM_RETRY_TYPE_NONE = -1,    /* No retry order                           */
  CM_RETRY_TYPE_CLEAR_ALL,    /* Clear all retry order                    */
  CM_RETRY_TYPE_ORIGINATION,  /* Origination retry order                  */
  CM_RETRY_TYPE_RESOURCE_REQ, /* Resource request retry order             */
  CM_RETRY_TYPE_SUPP_CH_REQ,  /* Supplemental channel request retry order */
  CM_RETRY_TYPE_RESERVED4,
  CM_RETRY_TYPE_RESERVED5,
  CM_RETRY_TYPE_RESERVED6,
  CM_RETRY_TYPE_RESERVED7,
  CM_RETRY_TYPE_MAX

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

typedef enum  {

    CM_CALL_EVENT_NONE=-1,    /* FOR INTERNAL USE OF CM ONLY! */

    CM_CALL_EVENT_ORIG,       /* phone originated a call */
    CM_CALL_EVENT_ANSWER,     /* Incoming call was answered */
    CM_CALL_EVENT_END,        /* Originated/incoming call was ended */
    CM_CALL_EVENT_FLASH,      /* phone sent Flash / Flash with Info to BS */

    CM_CALL_EVENT_INCOM,      /* phone received an incoming call */
    CM_CALL_EVENT_CONNECT,    /* Originated/incoming call was connected */
    CM_CALL_EVENT_SRV_OPT,    /* Service option changed while in a call */
    CM_CALL_EVENT_PRIVACY,    /* Privacy mode changed while in a call */
    CM_CALL_EVENT_PRIVACY_PREF, /* Privacy mode preference changed */
    CM_CALL_EVENT_CALLER_ID,  /* Caller ID info was received from BS */

    /* Events added for CM 2.0 */
    CM_CALL_EVENT_ABRV_ALERT,     /* CMDA/AMPS abbreviated alert */
    CM_CALL_EVENT_ABRV_REORDER,   /* AMPS abbreviated reorder */
    CM_CALL_EVENT_ABRV_INTERCEPT, /* AMPS abbreviated intercept */
    CM_CALL_EVENT_SIGNAL,         /* signal info was received from BS */
    CM_CALL_EVENT_DISPLAY,        /* display info was received from BS */
    CM_CALL_EVENT_CALLED_PARTY,   /* called party info was received from BS */
    CM_CALL_EVENT_CONNECTED_NUM,  /* connected number info was received from BS */
    CM_CALL_EVENT_INFO,           /* call information, this event only sent to the client
                                     that request this information through
                                     cm_call_cmd_get_call_info */
    /* IS-95B */
    CM_CALL_EVENT_EXT_DISP,       /* extended display was received from BS */
    CM_CALL_EVENT_NDSS_START,     /* Start NDSS redirection */
    CM_CALL_EVENT_NDSS_CONNECT,   /* call was re-connected due to NDSS */

    /* JCDMA */
    CM_CALL_EVENT_EXT_BRST_INTL,  /* Extended Burst Type - International */
    CM_CALL_EVENT_NSS_CLIR_REC,   /* National Supplementary services - CLIR */
    CM_CALL_EVENT_NSS_REL_REC,    /* National Supplementary services - Release */
    CM_CALL_EVENT_NSS_AUD_CTRL,   /* National Supplementary services - Audio Control */
    CM_CALL_EVENT_L2ACK_CALL_HOLD,/* Call Hold */

    CM_CALL_EVENT_MAX         /* FOR INTERNAL USE OF CM ONLY! */

#ifdef FEATURE_RPC
#error code not present
#endif /* FEATURE_RPC */

} cm_call_event_e_type;

typedef enum cm_ph_event_e {

    CM_PH_EVENT_NONE=-1,        /* FOR INTERNAL USE OF CM ONLY! */

    CM_PH_EVENT_OPRT_MODE,      /* Operating mode was changed */
    CM_PH_EVENT_MODE_PREF,      /* Mode preference was changed */
    CM_PH_EVENT_SYS_PREF,       /* System preference was changed */
    CM_PH_EVENT_ANSWER_VOICE,   /* Answer voice as data was changed */
    CM_PH_EVENT_NAM_SEL,        /* NAM selection was changed */
    CM_PH_EVENT_CURR_NAM,       /* Current NAM was changed */

    CM_PH_EVENT_IN_USE_STATE,   /* In use state was changed */
    CM_PH_EVENT_SRV_STATE,      /* Service state was changed */
    CM_PH_EVENT_CDMA_LOCK_MODE, /* CDMA lock mode was changed */
    CM_PH_EVENT_ROAM_STATUS,    /* Roaming status was changed */

    CM_PH_EVENT_MAINTREQ,       /* CDMA maintenance required command */
    CM_PH_EVENT_STANDBY_SLEEP,  /* Entering powerdown sleep mode */
    CM_PH_EVENT_STANDBY_WAKE,   /* Existing powerdown sleep mode */
    CM_PH_EVENT_RSSI,           /* RSSI change */
    CM_PH_EVENT_INFO,           /* Phone information, this event only sent to the client
                                   that request this information through
                                   cm_ph_cmd_get_ph_info */

    CM_PH_EVENT_INFO_AVAIL,     /* Phone information is now available */

    CM_PH_EVENT_MAX             /* FOR INTERNAL USE OF CM ONLY! */

#ifdef FEATURE_RPC
#error code not present
#endif

} cm_ph_event_e_type;

typedef struct cm_ph_info_s {

    boolean                     is_in_use;
         /* Indicate whether phone is currently in use */

    cm_oprt_mode_e_type         oprt_mode;
        /* Indicate current operating mode */

    cm_srv_state_e_type         srv_state;
        /* Indicate current service state */

    cm_cdma_lock_mode_e_type    cdma_lock_mode;
        /* Indicate whether phone is CDMA locked until power cycle */

    cm_answer_voice_e_type      answer_voice;
        /* Indicate whether phone should answer incoming
        ** voice calls as voice, fax or modem */

    cm_mode_pref_e_type         mode_pref;
        /* Indicate current mode preference */

    cm_mode_term_e_type         mode_term;
        /* Indicate the current mode preference term -
        ** one-call, power-cycle, etc.*/

    cm_sys_pref_e_type          sys_pref;
        /* Indicate current system preference */

    cm_roam_status_e_type       roam_status;
        /* Indicate current roaming status */

    cm_nam_e_type               nam_sel;
        /* Indicate the NAM selection */

    cm_nam_e_type               curr_nam;
        /* Indicate the current NAM -
        ** needed when NAM selection is set to AUTO-NAM */

    uint16                       rssi;
       /* RSSI in positive dBm i.e. 75 means -75dBm */

} cm_ph_info_s_type;

typedef enum cm_call_state_e {

    CM_CALL_STATE_NONE=-1,  /* FOR INTERNAL USE OF CM ONLY! */

    CM_CALL_STATE_IDLE,     /* Call is in idle state - i.e. no call */
    CM_CALL_STATE_ORIG,     /* Call is in origination state */
    CM_CALL_STATE_INCOM,    /* Call is in alerting state */
    CM_CALL_STATE_CONV,     /* Call is in conversation state */

    CM_CALL_STATE_MAX       /* FOR INTERNAL USE OF CM ONLY! */

#ifdef FEATURE_RPC
#error code not present
#endif /* FEATURE_RPC */

} cm_call_state_e_type;

/* Client ID type to differentiate CM client objects from one another.
*/
typedef int  cm_client_id_type;

/* Enumeration of CM call types.
**
** When originating or answering a call, clients need to specify
** one of these call types.
*/
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

#define CM_MAX_CALLED_INFO_CHARS  64   /*  CM_MAX_DATA_DIGITS*/
    /* Max length of Called info string */

#define CM_MAX_NUMBER_CHARS      CM_MAX_CALLED_INFO_CHARS
    /* Max length of CM number string  This is the Maximum
       of CM_MAX_DIALED_DIGITS, CM_MAX_CALLER_ID_CHARS
       CM_MAX_CALLED_INFO_CHARS and CM_MAX_CALLED_INFO_CHARS */

/* Enumeration of digit modes - see J-STD-008 2.7.1.3.2.4.
*/
typedef enum cm_digit_mode_e {

    CM_DIGIT_MODE_NONE=-1,      /* FOR INTERNAL USE OF CM ONLY! */

    CM_DIGIT_MODE_4BIT_DTMF,    /* 4-bit DTMF */
    CM_DIGIT_MODE_8BIT_ASCII,   /* 8-bit ASCII */

    CM_DIGIT_MODE_MAX           /* FOR INTERNAL USE OF CM ONLY! */

#ifdef FEATURE_RPC
#error code not present
#endif /* FEATURE_RPC */

} cm_digit_mode_e_type;

/* Privacy mode during a call
*/
typedef enum
{
  CM_PRIVACY_PREF_NONE = -1,
  CM_PRIVACY_PREF_STANDARD,
  CM_PRIVACY_PREF_ENHANCED,
  CM_PRIVACY_PREF_MAX
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
*/
typedef struct cm_num_s {


    byte                    buf[CM_MAX_NUMBER_CHARS];
        /* Number buffer */

    byte                    len;
        /* Length of number stored in num_buf */

    cm_digit_mode_e_type    digit_mode;
        /* Origination digit mode - see J-STD-008 2.7.1.3.2.4 */

    byte                    number_type;
        /* Number type - see J-STD-008 2.7.1.3.2.4-2
        ** NOTE! ONLY VALID FOR CDMA CALLS */

    byte                    number_plan;
        /* Numbering Plan - see J-STD-008 2.7.1.3.2.4-3
        ** NOTE! ONLY VALID FOR CDMA CALLS */

    byte                    pi;
        /* Presentation Indicator - see J-STD-008  2.7.4.4-1 */

    byte                    si;
        /* Screening Indicator - see J-STD-008 2.7.4.4-2 */

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

    byte    buf[CM_MAX_ALPHA_TAG_CHARS];
        /* Alpha buffer */

    byte    len;
        /* Length of alpha tag stored in alpha_buf */
} cm_alpha_s_type;

/* Type for CM signal types - see J-STD-008 3.7.5.5 and CAI.H file.
*/
typedef struct cm_signal_s {

    boolean    is_signal_info_avail;
        /* Indicate whether signal information is available.
        **
        ** NOTE! if this variable is FALSE, the values found in other
        ** fields of this structure as NO meaning */

    byte       signal_type;
        /* Signal type - Tone Signal, ISDN Alerting or IS-54B Alerting */

    byte       alert_pitch;
        /* Alert pitch - Medium, High or Low */

    byte       signal;
        /* Indicate which Tone signals, ISDN Alerting or IS_54B Alerting */

} cm_signal_s_type;

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

/* Type to hold call state information.
** When notified of call events, clients are given
** a reference to such a structure.
*/
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

#endif

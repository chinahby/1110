#ifdef FEATURE_BT_EXTPF_CTP
#ifndef _OI_TCS_CONSTS_H
#define _OI_TCS_CONSTS_H
/**
 * @file  
 *
 * This file includes types and constant definitions for use by TCS clients.
 */

/** \addtogroup Telephony Telephony APIs */
/**@{*/

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#include "oi_stddefs.h"


/** packet type */
typedef enum {
    OI_TCS_BEARER_PACKET_TYPE_HV1 = (BIT2 | BIT0),
    OI_TCS_BEARER_PACKET_TYPE_HV2,
    OI_TCS_BEARER_PACKET_TYPE_HV3,
    OI_TCS_BEARER_PACKET_TYPE_DV
} OI_TCS_BEARER_PACKET_TYPE;

/** user information layer 1 (octet 4) */
typedef enum {
    OI_TCS_BEARER_CODING_CVSD      = BIT5,
    OI_TCS_BEARER_CODING_PCM_ALAW  = BIT6,
    OI_TCS_BEARER_CODING_PCM_ULAW  = (BIT6 | BIT5)
} OI_TCS_BEARER_CODING;

// Octets 4-25 for ACL are formatted as L2CAP does so.

/** user information layer 2 (octet 26) */
typedef enum {
    OI_TCS_BEARER_UI_LAYER2_RFCOMM_OVER_L2CAP  = 0
} OI_TCS_BEARER_UI_LAYER2;

/** user information layer 3 (octet 26) */
typedef enum {
    OI_TCS_BEARER_UI_LAYER3_NONE  = 0,
    OI_TCS_BEARER_UI_LAYER3_PPP   = BIT4,
    OI_TCS_BEARER_UI_LAYER3_IP    = BIT5
} OI_TCS_BEARER_UI_LAYER3;


/** signal */
typedef enum {
    OI_TCS_SIGNAL_NONE,
    OI_TCS_SIGNAL_EXTERNAL_CALL = (BIT6),
    OI_TCS_SIGNAL_INTERNAL_CALL = (BIT6 | BIT0),
    OI_TCS_SIGNAL_CALL_BACK     = (BIT6 | BIT1)
} OI_TCS_SIGNAL;

/** progress indicator type */
typedef enum {
    OI_TCS_PROGRESS_NONE = 0,
    OI_TCS_PROGRESS_INFO_AVAILABLE = BIT3
} OI_TCS_PROGRESS;

/** call class */
typedef enum {
    OI_TCS_CALL_CLASS_EXTERNAL = 0,
    OI_TCS_CALL_CLASS_INTERCOM = 1,
    OI_TCS_CALL_CLASS_SERVICE  = 2,
    OI_TCS_CALL_CLASS_EMERGENCY = 3,
    OI_TCS_CALL_CLASS_NONE
} OI_TCS_CALL_CLASS;

/** audio control */
typedef enum {
    OI_TCS_AUDIO_CONTROL_VOLUME_INCREASE   = 0,
    OI_TCS_AUDIO_CONTROL_VOLUME_DECREASE   = 1,
    OI_TCS_AUDIO_CONTROL_MIC_GAIN_INCREASE = 2,
    OI_TCS_AUDIO_CONTROL_MIC_GAIN_DECREASE = 3,
    //0 X X X X X X reserved for Bluetooth standardization
    //1 X X X X X X company-specific
    OI_TCS_AUDIO_CONTROL_NONE
} OI_TCS_AUDIO_CONTROL;

/** call bearer link type */
typedef enum {
    OI_TCS_BEARER_LINK_TYPE_SCO,
    OI_TCS_BEARER_LINK_TYPE_ACL,
    OI_TCS_BEARER_LINK_TYPE_NONE
} OI_TCS_BEARER_LINK_TYPE;

/** The value to be used when no keypad facility should be sent in a message. */
#define OI_TCS_KEYPAD_FACILITY_NONE 0

/**  cause codes */
typedef enum {
    
    OI_TCS_CAUSE_NONE                            = 0,   /** Note, 0 is never a valid value, it is only used internally 
                                                            as a placeholder for unused cause in elements structure */
    OI_TCS_CAUSE_UNALLOCATED_NUMBER              = 1,
    OI_TCS_CAUSE_NO_ROUTE_TO_DESTINATION         = 3,
    OI_TCS_CAUSE_NORMAL_CALL_CLEARING            = 16,
    OI_TCS_CAUSE_USER_BUSY                       = 17,
    OI_TCS_CAUSE_NO_USER_RESPONDING              = 18,
    OI_TCS_CAUSE_NO_ANSWER_FROM_USER             = 19,
    OI_TCS_CAUSE_CALL_REJECTED                   = 21,
    OI_TCS_CAUSE_NUMBER_CHANGED                  = 22,
    OI_TCS_CAUSE_NONSELECTED_USER_CLEARING       = 26,
    OI_TCS_CAUSE_INVALID_NUMBER_FORMAT           = 28,
    OI_TCS_CAUSE_FACILITY_REJECTED               = 29,
    OI_TCS_CAUSE_NO_CIRCUIT_AVAILABLE            = 34,
    OI_TCS_CAUSE_TEMPORARY_FAILURE               = 41,
    OI_TCS_CAUSE_REQUESTED_CIRCUIT_NOT_AVAILABLE = 44,
    OI_TCS_CAUSE_BEARER_CAPABILITY_UNAVAILABLE      = 58,
    OI_TCS_CAUSE_BEARER_CAPABILITY_NOT_IMPLEMENTED  = 65,
    OI_TCS_CAUSE_REQUESTED_FACILITY_NOT_IMPLEMENTED = 69,
    OI_TCS_CAUSE_MANDATORY_INFORMATION_ELEM_MISSING = 96,
    OI_TCS_CAUSE_INVALID_INFORMATION_ELEM_CONTENTS  = 100,
    OI_TCS_CAUSE_RECOVERY_ON_TIMER_EXPIRY           = 102
} OI_TCS_CAUSE;

/**********************************************************************

 called party number / calling party number constants 

***********************************************************************/

/** number type (octet 3) */
typedef enum {
    OI_TCS_NUMBER_TYPE_UNKNOWN       = 0,
    OI_TCS_NUMBER_TYPE_INTERNATIONAL = BIT4,
    OI_TCS_NUMBER_TYPE_NATIONAL      = BIT5,
    OI_TCS_NUMBER_TYPE_NETWORK       = (BIT4 | BIT5),
    OI_TCS_NUMBER_TYPE_SUBSCRIBER    = BIT6,
    OI_TCS_NUMBER_TYPE_ABBREVIATED   = (BIT6 | BIT5),
    OI_TCS_NUMBER_TYPE_RESERVED_FOR_EXTENSION = (BIT6 | BIT5 | BIT4)
} OI_TCS_NUMBER_TYPE;

/** numbering plan constants */
typedef enum {
    OI_TCS_NUMBER_PLAN_UNKNOWN = 0,
    OI_TCS_NUMBER_PLAN_ISDN    = BIT0,
    OI_TCS_NUMBER_PLAN_X121    = (BIT0 | BIT1),
    OI_TCS_NUMBER_PLAN_RESERVED = BIT2,
    OI_TCS_NUMBER_PLAN_NATIONAL = BIT3,
    OI_TCS_NUMBER_PLAN_PRIVATE  = (BIT3 | BIT0)
} OI_TCS_NUMBER_PLAN;

/** presentation indicator (octet 4, calling party only) */
typedef enum {
    OI_TCS_PRESENTATION_ALLOWED     = 0,
    OI_TCS_PRESENTATION_RESTRICTED  = BIT5,
    OI_TCS_PRESENTATION_NUM_NOT_AVAILABLE = BIT6, /**< Number not available due to interworking */
    OI_TCS_PRESENTATION_RESERVED    = (BIT5 | BIT6)
} OI_TCS_PRESENTATION;

/** screening indicator (octet 4, calling party only) */
typedef enum {
    OI_TCS_SCREENING_USER_NOT_SCREENED  = 0, /**< user-provided, not screened */
    OI_TCS_SCREENING_USER_PASSED        = BIT0, /**< user-provided, verified and passed */
    OI_TCS_SCREENING_USER_FAILED        = BIT1, /**< user-provided, verified and failed */
    OI_TCS_SCREENING_NETWORK            = (BIT0 | BIT1) /**< network provided */
} OI_TCS_SCREENING;

/**@}*/

#endif /* _OI_TCS_CONSTS_H */
#endif /* FEATURE_BT_EXTPF_CTP */

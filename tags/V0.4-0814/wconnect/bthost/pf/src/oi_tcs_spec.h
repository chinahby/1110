#ifdef FEATURE_BT_EXTPF_CTP
#ifndef _TCS_SPEC_H
#define _TCS_SPEC_H
/**
 * @file  
 *
 * private definitions for TCS-BIN from the Bluetooth specifications
 */

/** \addtogroup Telephony Telephony APIs */
/**@{*/

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#include "oi_stddefs.h"
#include "oi_bt_spec.h"

/** The sco handle type. */
typedef OI_UINT8 SCO_HANDLE;

/** The byte order for TCS. */
#define TCS_BYTE_ORDER  OI_LITTLE_ENDIAN_BYTE_ORDER

/** The maximum number of devices in a Wireless User Group. */
#define TCS_MAX_WUG_SIZE 7

/* Call states (Section 2) */
//General States
#define TCS_STATE_NULL 0
#define TCS_STATE_ACTIVE 10
#define TCS_STATE_DISCONNECT_REQ 11
#define TCS_STATE_DISCONNECT_IND 12
#define TCS_STATE_RELEASE_REQ 19
#define TCS_STATE_RELEASE_RECEIVED (253)            // This state is not defined by the spec, it is only used internally
#define TCS_STATE_RELEASE_COLLISION_RECEIVED (254)  // This state is not defined by the spec, it is only used internally

//Outgoing Side States
#define TCS_STATE_CALL_INITIATED (1)
#define TCS_STATE_CONNECT_RECEIVED (255)    // This state is not defined by the spec, it is only used internally

#ifndef TCS_LEAN
#define TCS_STATE_OVERLAP_SENDING (2)
#define TCS_STATE_OUTGOING_CALL_PROCEEDING (3)
#define TCS_STATE_CALL_DELIVERED (4)
#endif

//Incoming Side States
#define TCS_STATE_CALL_PRESENT (6)
#define TCS_STATE_CONNECT_REQ (8)

#ifndef TCS_LEAN
#define TCS_STATE_CALL_RECEIVED (7)
#define TCS_STATE_INCOMING_CALL_PROCEEDING (9)
#define TCS_STATE_OVERLAP_RECEIVING (25)
#endif

/** Identifiers for the timers. */
enum {
    TCS_TNULL,
    TCS_T301,
    TCS_T302,
    TCS_T303,
    TCS_T304,
    TCS_T305,
    TCS_T308,
    TCS_T310,
    TCS_T313,
    TCS_T401,
    TCS_T402,
    TCS_T403,
    TCS_T404,
    TCS_T405,
    TCS_T406,
    TCS_TIMER_ENUM_MAX //The last value in the enum
};

//Timer values defined by the spec (Section 9)
#define TCS_TNULL_DURATION 0
#define TCS_T301_DURATION OI_MINUTES(3) //Minimum 3 minutes
#define TCS_T302_DURATION OI_SECONDS(15) //15 seconds
#define TCS_T303_DURATION OI_SECONDS(20) //20 seconds
#define TCS_T304_DURATION OI_SECONDS(30) //30 seconds
#define TCS_T305_DURATION OI_SECONDS(30) //30 seconds
#define TCS_T308_DURATION OI_SECONDS(4) //4 seconds
#define TCS_T310_DURATION OI_SECONDS(30) //30-120 seconds
#define TCS_T313_DURATION OI_SECONDS(4) //4 seconds
#define TCS_T401_DURATION OI_SECONDS(8) //8 seconds
#define TCS_T402_DURATION OI_SECONDS(8) //8 seconds
#define TCS_T403_DURATION OI_SECONDS(4) //4 seconds
#define TCS_T404_DURATION OI_SECONDS(2.5) //2.5 seconds
#define TCS_T405_DURATION OI_SECONDS(2) //2 seconds
#define TCS_T406_DURATION OI_SECONDS(20) //20 seconds

/* Message Coding (Section 7) */

/** Get the protocol descriminator from the first byte of a message. */
#define GetProtocolType(x) (((x) & (BIT7 | BIT6 | BIT5))) //Get top 3 bits

/** Get the message type from the first byte of a message. */
#define GetMessageType(x) ((x) & 0x1F) //Get bottom 3 bits

//Protoctol descriminator
#define TCS_PROTOCOL_CALL_CONTROL    0 // 0b000
#define TCS_PROTOCOL_GROUP           BIT5 // 0b001
#define TCS_PROTOCOL_CONNECTIONLESS  BIT6 // 0b010

/* Message types */
/* NOTE: these are only enums for convenience. The individual message
 * type codes ARE defined in the spec. */

typedef enum {
    // Call control
    TCS_MSG_ALERTING = TCS_PROTOCOL_CALL_CONTROL,
    TCS_MSG_CALL_PROCEEDING,
    TCS_MSG_CONNECT,
    TCS_MSG_CONNECT_ACK,
    TCS_MSG_PROGRESS,
    TCS_MSG_SETUP,
    TCS_MSG_SETUP_ACK,
    
    //Call clearing
    TCS_MSG_DISCONNECT,
    TCS_MSG_RELEASE,
    TCS_MSG_RELEASE_COMPLETE,

    //Miscellaneous
    TCS_MSG_INFORMATION,

    /* Call control - DTMF*/
    TCS_MSG_START_DTMF = (TCS_PROTOCOL_CALL_CONTROL | 16),
    TCS_MSG_START_DTMF_ACK,
    TCS_MSG_START_DTMF_REJECT,
    TCS_MSG_STOP_DTMF,
    TCS_MSG_STOP_DTMF_ACK,

    /* Group management messages */
    TCS_MSG_INFO_SUGGEST = TCS_PROTOCOL_GROUP,
    TCS_MSG_INFO_ACCEPT,
    TCS_MSG_LISTEN_REQUEST,
    TCS_MSG_LISTEN_ACCEPT,
    TCS_MSG_LISTEN_SUGGEST,
    TCS_MSG_LISTEN_REJECT,
    TCS_MSG_ACCESS_RIGHTS_REQUEST,
    TCS_MSG_ACCESS_RIGHTS_ACCEPT,
    TCS_MSG_ACCESS_RIGHTS_REJECT,

    //Connectionless messages
    TCS_MSG_CL_INFO = TCS_PROTOCOL_CONNECTIONLESS

} TCS_MSG;


/* Information element identifiers (Section 7.4) */
typedef enum {
    //Single octet elements
    TCS_ELEM_SENDING_COMPLETE = BIT7 | BIT5 | BIT0,

    //Double octet elements
    TCS_ELEM_CALL_CLASS = BIT7 | BIT6,
    TCS_ELEM_CAUSE,
    TCS_ELEM_PROGRESS_INDICATOR,
    TCS_ELEM_SIGNAL,
    TCS_ELEM_KEYPAD_FACILITY,
    TCS_ELEM_SCO_HANDLE,

    //Variable octet elements
    TCS_ELEM_CLOCK_OFFSET = 0, //max len 4
    TCS_ELEM_CONFIG_DATA,
    TCS_ELEM_BEARER_CAPABILITY, //max len 4 (SCO) or 26 (ACL)
    TCS_ELEM_DEST_CID, //max len 4
    TCS_ELEM_CALLING_PARTY_NUMBER,
    TCS_ELEM_CALLED_PARTY_NUMBER,
    TCS_ELEM_AUDIO_CONTROL,
    TCS_ELEM_COMPANY_SPECIFIC
} TCS_ELEM;


#define TCS_ELEM_TYPE_MASK (BIT7 | BIT6)
#define TCS_ELEM_TYPE_SINGLE (BIT7)
#define TCS_ELEM_TYPE_DOUBLE (BIT7 | BIT6)
#define TCS_ELEM_TYPE_VARIABLE 0

/* Clock offset */
typedef OI_UINT16 TCS_CLOCK_OFFSET;

/* Company */
/* Defined by the BT assigned numbers. */

/* Configuration data */
typedef struct {
    OI_UINT8 number1;
    OI_UINT8 number2;
    OI_BD_ADDR addr;
    OI_LINK_KEY linkKey;
} TCS_WUG_MEMBER_DATA;

typedef struct {
    TCS_WUG_MEMBER_DATA members[TCS_MAX_WUG_SIZE];
    OI_UINT8 numMembers;
} TCS_CONFIG_DATA;


#define OI_TCS_SIGNAL_OCTET1  (BIT7 | BIT6 | BIT1 | BIT0)
#define OI_TCS_NUMBER_TYPE_MASK (BIT6 | BIT5 | BIT4)
#define OI_TCS_NUMBER_PLAN_MASK (BIT3 | BIT2 | BIT1 | BIT0)
#define OI_TCS_SCREENING_MASK (BIT1 | BIT0)
#define OI_TCS_PRESENTATION_MASK (BIT6 | BIT5)
#define OI_TCS_BEARER_PACKET_TYPE_MASK (BIT4 | BIT3 | BIT2 | BIT1 | BIT0)
#define OI_TCS_BEARER_CODING_MASK (BIT7 | BIT6 | BIT5)

#endif /* _TCS_SPEC_H */

/**@}*/
#endif /* FEATURE_BT_EXTPF_CTP */

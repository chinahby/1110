#ifndef AEETAPI_H
#define AEETAPI_H
/*======================================================
FILE:  AEETAPI.h

SERVICES:  AEE TAPI Interface

GENERAL DESCRIPTION:
   Base level definitions, typedefs, etc. for AEE TAPI

        Copyright ?1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#include "AEE.h"

/* Make sure that CUST_H is defined and then include whatever file it
** specifies.
*/

typedef struct _ITAPI         ITAPI;

//********************************************************************************************************************************
//
// ITAPI Interface
//
//********************************************************************************************************************************

#define MAX_DIGITS            64
#define MOBILE_ID_LEN         15

// Notification Masks...

#define NMASK_TAPI_STATUS     0x0001
#define NMASK_TAPI_SMS_TEXT   0x0002  // SMS Text - ASCIIZ
#define NMASK_TAPI_SMS_TS     0x0004  // TS ID == Upper 16 bits
#define NMASK_TAPI_WAP_PUSH   0x0010  // WAP Push

// Basic Phone States...

#define PS_OFFLINE            0
#define PS_IDLE               1
#define PS_INCOMING           2
#define PS_ORIG               3
#define PS_CONVERSATION       4

typedef byte  AEEPhoneState;

typedef struct
{
   uint16      nChars;
   char        szText[1];
} AEESMSTextMsg;

//
// Bits for use with ITAPI_OnCallStatus
//

#define OCS_CANCEL            0x0001
#define OCS_UNIQUE_PFN        0x0002
#define OCS_ONE_SHOT          0x0004

#define OCS_INCOMING          0x0010
#define OCS_ORIG              0x0020
#define OCS_CONVERSATION      0x0040
#define OCS_IDLE              0x0080 /* same as OnCallEnd(), also accept 0 */
#define OCS_OFFLINE           0x0100

#define OCS_ALL               (OCS_IDLE|OCS_INCOMING|\
                               OCS_ORIG|OCS_CONVERSATION|OCS_OFFLINE)

#define OCE_CANCEL            OCS_CANCEL
#define OCE_UNIQUE_PFN        OCS_UNIQUE_PFN
#define OCE_ONE_SHOT          OCS_ONE_SHOT

//*****************************************************************
//
// SMS Message Information
//
//
//*****************************************************************

#define SMS_MAX_DATA    3*229
#define SMS_MAX_DIGITS  32

enum {
   SMS_TELESERVICE_CMT_91             = 4096,  /* embedded IS91 SMS */
   SMS_TELESERVICE_CPT_95             = 4097,  /* page */
   SMS_TELESERVICE_CMT_95             = 4098,  /* short message */
   SMS_TELESERVICE_VMN_95             = 4099,  /* voice mail notification */
   SMS_TELESERVICE_IS91_PAGE          = 0,
   SMS_TELESERVICE_IS91_VOICE_MAIL    = 1,
   SMS_TELESERVICE_IS91_SHORT_MESSAGE = 2,
   SMS_TELESERVICE_MWI                = 3, /*Voice mail notification through Message Waiting 
                                             Indication in CDMA mode or Analog mode*/

   SMS_TELESERVICE_WAP                = 4100,  /* WAP */
   SMS_TELESERVICE_WEMT               = 4101,  /* Enhanced Messaging/EMS */
   SMS_TELESERVICE_SCPT               = 4102,  /* Srv Category Programming */
   SMS_TELESERVICE_CATPT              = 4103,  /* Card App Toolkit Protocol */
   SMS_TELESERVICE_GSM1x_01           = 4104,  /* GSM1x signalling message */
   SMS_TELESERVICE_GSM1x_02           = 4105,  /* GSM1x short message */
   SMS_TELESERVICE_GSM1x_03           = 4106,  /* Reserved for now */
   SMS_TELESERVICE_GSM1x_04           = 4107,  /* Reserved for now */
   SMS_TELESERVICE_GSM1x_05           = 4108,  /* Reserved for now */
   SMS_TELESERVICE_GSM1x_06           = 4109,  /* Reserved for now */
   SMS_TELESERVICE_GSM1x_07           = 4110,  /* Reserved for now */
   SMS_TELESERVICE_GSM1x_08           = 4111,  /* Reserved for now */
   SMS_TELESERVICE_GSM1x_09           = 4112,  /* Reserved for now */
   SMS_TELESERVICE_GSM1x_10           = 4113,  /* Reserved for now */
   SMS_TELESERVICE_BROADCAST          = 0x0005,

#ifdef CUST_EDITION
#if defined(FEATURE_QMA)
   SMS_TELESERVICE_QMA_WPUSH          = 65002,  /* QMA Wap push message service ID */
#else//#elif defined (FEATURE_CARRIER_CHINA_TELCOM)
   SMS_TELESERVICE_WPUSH              = 65002,  /* China telecom Wap push message service ID  */
#endif
#endif
   SMS_TELESERVICE_UNKNOWN            = 0xFFFF
};
#ifdef WIN32
typedef unsigned AEETeleservice;
#else
typedef uint16 AEETeleservice;
#endif

enum {
   SMS_ENCODING_OCTET,
   SMS_ENCODING_IS91EP,
   SMS_ENCODING_ASCII,
   SMS_ENCODING_IA5,
   SMS_ENCODING_UNICODE,   
   SMS_ENCODING_SHIFT_JIS, 
   SMS_ENCODING_KOREAN,    
   SMS_ENCODING_LATIN_HEBREW,
   SMS_ENCODING_LATIN,     
   SMS_ENCODING_GSM_7_BIT_DEFAULT,
   SMS_ENCODING_UNKNOWN = 0xff
};
#ifdef WIN32
typedef unsigned AEESMSEncoding;
#else
typedef int8 AEESMSEncoding;
#endif

enum {
   SMS_IS91EP_VOICE_MAIL         = 0x82,
   SMS_IS91EP_SHORT_MESSAGE_FULL = 0x83,
   SMS_IS91EP_CLI_ORDER          = 0x84,
   SMS_IS91EP_SHORT_MESSAGE      = 0x85
};
#ifdef WIN32
typedef unsigned AEESMSIS91EP;
#else
typedef uint8 AEESMSIS91EP;
#endif

enum {
   SMS_PRIORITY_NORMAL,
   SMS_PRIORITY_INTERACTIVE,
   SMS_PRIORITY_URGENT,
   SMS_PRIORITY_EMERGENCY
};
#ifdef WIN32
typedef unsigned AEESMSPriority;
#else
typedef uint8 AEESMSPriority;
#endif

enum {
   SMS_PRIVACY_NORMAL,
   SMS_PRIVACY_RESTRICTED,
   SMS_PRIVACY_CONFIDENTIAL,
   SMS_PRIVACY_SECRET
};
#ifdef WIN32
typedef unsigned AEESMSPrivacy;
#else
typedef int8 AEESMSPrivacy;
#endif

typedef struct
{
   byte              year;        /* 0-99,0-95: 2000+year, 96-99: 1900+year */
   byte              month;       /* 1-12 */
   byte              day;         /* 1-31 */
   byte              hour;        /* 0-23 */
   byte              minute;      /* 0-59 */
   byte              second;      /* 0-59 */
} AEESMSTimestamp;

typedef struct
{
   uint16            teleservice;
   AEESMSTimestamp   ts;
   AEESMSPriority    priority;
   AEESMSPrivacy     privacy;
   int               nMessages;     // Number of messages...
   boolean           bAlert;
   uint32            dwLang;

   // Callback number information...

   byte              nDigits;
   char              szDigits[SMS_MAX_DIGITS + 1];

   // Data payload...

   AEESMSEncoding    encoding;
   AEESMSIS91EP      is91;
   byte              nLen;
   byte              data[SMS_MAX_DATA];
} AEESMSMsg;


typedef struct
{
   char           szMobileID[MOBILE_ID_LEN + 1];   // Mobile ID (digits)
   AEEPhoneState  state;                           // Phone state
   flg            bData:1;                         // Data Mode
   flg            bDigital:1;                      // Digital service
   flg            bRoaming:1;                      // Roaming
   flg            bCallEnded:1;                    // Call Ended Flag
   flg            bE911CallbackMode:1;             // 911 Mode
   flg            bRestricted:1;                   // Unused
   flg            bRegistered:1;                   // System registration accomplished, set to true if not applicable
   flg            bDormancy:1;                     // Network fully supports dormancy
   flg            bHDRMode:1;                      // Air link is in HDR mode.
   flg            bAirplaneMode:1;                 // Airplane mode : Where receiver and transmitter on the phone are turned off.
} TAPIStatus;


#define AEEMOSMS_ERR_NO_ERR               0
#define AEEMOSMS_ERR_BASE                 0x00400000
#define AEEMOSMS_ERR_OUTOF_RESOURCES      (AEEMOSMS_ERR_BASE+1)
#define AEEMOSMS_ERR_NETWORK_NOT_READY    (AEEMOSMS_ERR_BASE+2)
#define AEEMOSMS_ERR_TRANSPORT_TEMP       (AEEMOSMS_ERR_BASE+3)
#define AEEMOSMS_ERR_TRANSPORT_PERM       (AEEMOSMS_ERR_BASE+4)
#define AEEMOSMS_ERR_INTERNAL             (AEEMOSMS_ERR_BASE+5)

//Callback to notify status of MO-SMS
typedef void (*PFNSMSSTATUS)(void * pUser, int status);

QINTERFACE(ITAPI)
{
   DECLARE_IBASE(ITAPI)
   DECLARE_INOTIFIER(ITAPI)

   int               (*GetStatus)(ITAPI * po, TAPIStatus * ps);
   int               (*MakeVoiceCall)(ITAPI * po, const char * pszNumber, AEECLSID clsReturn);
   AEESMSTextMsg *   (*ExtractSMSText)(ITAPI * po, AEESMSMsg * pMsg);
   boolean           (*GetCallerID)(ITAPI * po, AECHAR * psz, int nSize);
   int               (*OnCallStatus)(ITAPI * po,PFNNOTIFY pfn,void * pUser,uint32 dwDelay,uint16 wFlags);
   boolean           (*IsVoiceCall)(ITAPI * po);
   boolean           (*IsDataSupported)(ITAPI * po);
   int               (*SendSMS)(ITAPI *po, const char * pszDst, const char * pszMsg, AEECLSID clsDst,PFNSMSSTATUS pfn, void *pUser);
};

#define ITAPI_AddRef(p)                            GET_PVTBL(p,ITAPI)->AddRef(p)
#define ITAPI_Release(p)                           GET_PVTBL(p,ITAPI)->Release(p)
#define ITAPI_GetStatus(p,pStatus)                 GET_PVTBL(p,ITAPI)->GetStatus(p,pStatus)
#define ITAPI_MakeVoiceCall(p, psz, cls)           GET_PVTBL(p,ITAPI)->MakeVoiceCall(p, psz, cls)
#define ITAPI_ExtractSMSText(p,pMsg)               GET_PVTBL(p,ITAPI)->ExtractSMSText(p,pMsg)
#define ITAPI_GetCallerID(p,psz,n)                 GET_PVTBL(p,ITAPI)->GetCallerID(p,psz,n)
#define ITAPI_OnCallStatus(p,pfn,pu,dwd,bs)        GET_PVTBL(p,ITAPI)->OnCallStatus(p,pfn,pu,dwd,bs)
#define ITAPI_OnCallEnd(p,pfn,pu,dwd,bs)           GET_PVTBL(p,ITAPI)->OnCallStatus(p,pfn,pu,dwd,bs)
#define ITAPI_IsVoiceCall(p)                       GET_PVTBL(p,ITAPI)->IsVoiceCall(p)
#define ITAPI_IsDataSupported(p)                   GET_PVTBL(p,ITAPI)->IsDataSupported(p)
#define ITAPI_SendSMS(p,pDst,pMsg,cls,pfn,puser)   GET_PVTBL(p,ITAPI)->SendSMS(p,pDst,pMsg,cls,pfn,puser)


/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
======================================================================= 

NMASK_TAPI_STATUS

Description:

This is a notification mask that can be used by apps to register to ne notified whenever there is a
change in the telephony status of the device.
Applications can use the AEECLSID_TAPI class to be notified whenver there is a change in the
telephony status of the device. To register for this notification, applications must use the mask NMASK_TAPI_STATUS.
Whenever there is a status-change, applications receive the EVT_NOTIFY event. The dwParam of this event is of type AEENotify. 
The pData member inside this AEENotify structure is of type TAPIStatus and contains detailed information about the
current telephony status of the device.

Definition:

#define NMASK_TAPI_STATUS     0x0001

Members:
 None

Comments:
 None

See Also:
 TAPIStatus


============================================================================================================================================== 
NMASK_TAPI_SMS_TEXT

Description:

This is a notification mask that allows applications to register for all Text Messages (messages with 
TeleServce ID: SMS_TELESERVICE_CMT_95).
When a text message arrives, the app is notified through the EVT_NOTIFY event. The dwParam of this event is of 
type AEENotify. The pData member in this AEENotify Structure will be of type AEESMSTextMsg and contains the actual text message.
Just like in any BREW Notification, the contents of dwParam are valid only when EVT_NOTIFY is being handled. 
If apps need to remember the data even after the app has finished handling this event, a copy must be made by the app.

Definition:

#define NMASK_TAPI_SMS_TEXT     0x0002

Members:
 None

Comments:
 None

See Also:
 None

============================================================================================================================================== 
NMASK_TAPI_SMS_TS

Description:

This is a notification mask that allows applications to register for SMS message of a specific TeleService ID. To construct the
actual 32-bit mask to be used in the MIF for the registration, the upper 16 bits of the mask must contain the
TeleService ID value and the lower 16 bits must contain the value 0x0004 (which corresponds to NMASK_TAPI_SMS_TS).
For ex: To register for a message with TS ID: 0x1002, the 32-mask should be: 0x10020004
Applications can register for multiple messages by creating the 32-mask for each message. For ex: to register for messages with 
TS IS: 1002 and 1003, the application must register two separate masks: 0x10020004 and 0x10030004.

When the SMS message of this TS ID is received by the system, the application is notified through the EVT_NOTIFY event. The
dwParam of this event is of type AEENotify. The pData member in the AEENotify Structure is of type AEESMSMsg. This structure
contains detailed information about the message.

Just like in any BREW Notification, the contents of dwParam are valid only when EVT_NOTIFY is being handled. 
If apps need to remember the data even after the app has finished handling this event, a copy must be made by the app.

Definition:

#define NMASK_TAPI_SMS_TS     0x0004

Members:
 None

Comments:
 None

See Also:
 None

============================================================================================================================================== 
NMASK_TAPI_WAP_PUSH

Description:

This is a notification mask that allows applications to register for WAP push messages. ITAPI delivers WAP push messages
which can be concatenation of uptp 3 SMS messages.

When the SMS message of this TS ID is received by the system, the application is notified through the EVT_NOTIFY event. The
dwParam of this event is of type AEENotify. The pData member in the AEENotify Structure is of type AEESMSTextMsg. This structure
contains the message data. 

Just like in any BREW Notification, the contents of dwParam are valid only when EVT_NOTIFY is being handled. 
If apps need to remember the data even after the app has finished handling this event, a copy must be made by the app.

Definition:

#define NMASK_TAPI_WAP_PUSH     0x0010

Members:
 None

Comments:
 None

See Also:
 None

============================================================================================================================================== 
TAPIStatus

Description:

TAPIStatus is used in ITAPI_GetStatus() to get the current TAPI status on the phone. This is also sent as dwParam
member of the EVT_NOTIFY event when applications have registered for TAPI Status notifications (NMASK_TAPI_STATUS).

Definition:
typedef struct
{
   char           szMobileID[MOBILE_ID_LEN + 1];   
   AEEPhoneState  state;                           
   flg            bData:1;                         
   flg            bDigital:1;                      
   flg            bRoaming:1;                      
   flg            bCallEnded:1;                    
   flg            bE911CallbackMode:1;             
   flg            bRestricted:1;
   flg            bRegistered:1;
   flg            bDormancy:1;
   flg            bHDRMode:1;
   flg            bAirplaneMode:1;
} TAPIStatus;

Members:
 
   szMobileID: ASCII string representing Mobile Identification Number (MIN). 
               Applications that need the MDN information should use 
               ISHELL_GetDeviceInfoEx with AEE_DEVICEITEM_MDN.

   state: Current phone State. This is an enum of type AEEPhoneState

   bData: Indicates if phone is in a data call

   bDigital: Indicates if phone is in digital service

   bRoaming; Indicates if phone is in a roaming state

   bCallEnded: This flag indicates if this notification was sent as part of a call-end. This flag 
   is useful when applications have registered to be notified with updated TAPIStatus information 
   on any changes. 

   bE911CallbackMode; Indicates if phone is in the E911 callback mode

   bRestricted: This member is not used

   bRegistered: System registration accomplished, set to TRUE if not applicable

   bDormancy:   Network fully supports dormancy, FALSE is not applicable
   
   bHDRMode: Air link is in HDR(or 1xEv) mode.
   
   bAirplaneMode : Where RF receiver and transmitter on the phone are turned off.


Comments:
None

See Also:
   PhoneState
   ITAPI_GetStatus()

=======================================================================
AEESMSTimestamp

Description:
It is used inside AEESMSMsg to specify the time-stamp of the SMS message.

Definition:
typedef struct
{
   byte              year;        
   byte              month;       
   byte              day;         
   byte              hour;        
   byte              minute;      
   byte              second;      
} AEESMSTimestamp;

Members:

   Year: Specifies the year this message was received.(0 thru 99, 0-95: 2000+year, 96-99: 1900+year)

   month: Specifies the month (1 thru 12) 

   day: Specifies the day (1 thru 31)

   hour: Specifies the Hour (0 thru 23)

   minute: Specifies the minute (0 thru 59)

   second:  Specifies the second (0 thru 59)
   

Comments:
None

See Also: 
NMASK_TAPI_SMS_TS, AEESMSMsg

=======================================================================
AEESMSPriority

Description:
   It is used inside AEESMSMsg to specify the Priority of the SMS message.

Definition:
enum {
   SMS_PRIORITY_NORMAL,
   SMS_PRIORITY_INTERACTIVE,
   SMS_PRIORITY_URGENT,
   SMS_PRIORITY_EMERGENCY
};

Members:
   SMS_PRIORITY_NORMAL:
   SMS_PRIORITY_INTERACTIVE:
   SMS_PRIORITY_URGENT:
   SMS_PRIORITY_EMERGENCY:

Comments:
None

See Also: 
NMASK_TAPI_SMS_TS, AEESMSMsg

=======================================================================
AEESMSPrivacy

Description:
It is used inside AEESMSMsg to specify the Privacy of the SMS message.

Definition:
enum {
   SMS_PRIVACY_NORMAL,
   SMS_PRIVACY_RESTRICTED,
   SMS_PRIVACY_CONFIDENTIAL,
   SMS_PRIVACY_SECRET
};

Members:
   SMS_PRIVACY_NORMAL

   SMS_PRIVACY_RESTRICTED

   SMS_PRIVACY_CONFIDENTIAL

   SMS_PRIVACY_SECRET

Comments:
None

See Also: 
NMASK_TAPI_SMS_TS, AEESMSMsg

=======================================================================
AEESMSEncoding

Description:

It is used inside AEESMSMsg to specify the type of Encoding used in the SMS message.


Definition:
enum {
   SMS_ENCODING_OCTET,
   SMS_ENCODING_IS91EP,
   SMS_ENCODING_ASCII,
   SMS_ENCODING_IA5,
   SMS_ENCODING_UNICODE,   
   SMS_ENCODING_SHIFT_JIS, 
   SMS_ENCODING_KOREAN,    
   SMS_ENCODING_LATIN_HEBREW,
   SMS_ENCODING_LATIN,     
   SMS_ENCODING_GSM_7_BIT_DEFAULT
};


Members:

   SMS_ENCODING_OCTET,

   SMS_ENCODING_IS91EP,

   SMS_ENCODING_ASCII,

   SMS_ENCODING_IA5,

   SMS_ENCODING_UNICODE,  
   
   SMS_ENCODING_SHIFT_JIS, 

   SMS_ENCODING_KOREAN,    

   SMS_ENCODING_LATIN_HEBREW,

   SMS_ENCODING_LATIN,     

   SMS_ENCODING_GSM_7_BIT_DEFAULT


Comments:
None

See Also: 
NMASK_TAPI_SMS_TS, AEESMSMsg

=======================================================================
AEESMSIS91EP

Description:
   It is used inside AEESMSMsg to specify the IS-91 EP data type
 
Definition:
enum {
   SMS_IS91EP_VOICE_MAIL         = 0x82,
   SMS_IS91EP_SHORT_MESSAGE_FULL = 0x83,
   SMS_IS91EP_CLI_ORDER          = 0x84,
   SMS_IS91EP_SHORT_MESSAGE      = 0x85
};


Members:
   SMS_IS91EP_VOICE_MAIL

   SMS_IS91EP_SHORT_MESSAGE_FULL

   SMS_IS91EP_CLI_ORDER

   SMS_IS91EP_SHORT_MESSAGE

Comments:
None

See Also: 
NMASK_TAPI_SMS_TS, AEESMSMsg

=======================================================================
AEESMSMsg

Description:

This structure is passed to the applications as part of the SMS Notification i.e. when applications register to be notified 
of a specific TS ID using the mask: NMASK_TAPI_SMS_TS, application receive the EVT_NOTIFY event 
when the msg comes into the system. The dwParam of this notification event is of type AEENotify. Inside the
AEENotify structure, the pData member is of type AEESMSMsg. This structure contains detailed information about the
SMS message received by the system.

Definition:
typedef struct 
{
   uint16            teleservice;
   AEESMSTimestamp   ts;
   AEESMSPriority    priority;
   AEESMSPrivacy     privacy;
   int               nMessages;     
   boolean           bAlert;
   uint32            dwLang;
   byte              nDigits;
   char              szDigits[SMS_MAX_DIGITS + 1];
   AEESMSEncoding    encoding;
   AEESMSIS91EP      is91;
   byte              nLen;
   byte              data[SMS_MAX_DATA];
} AEESMSMsg;


Members:

   teleservice: Specifies the Teleservice ID of the message.

   ts: Specifies the time stamp of the message.

   priority: Specifies the Priority of the message.

   privacy: Specifies the Privacy level of the message.

   nMessages: Specifies the number of messages present in this structure.

   bAlert: This is not used
   
   dwLang: Specifies the language setting for the message. This is the ISO (ISO 639-1 and ISO 3166 A2 ) defined Language ID

   nDigits: Callback Number Information. Specifies the number of digits in the callback number.

   szDigits:Callback Number Information. Specifies the callback number.

   encoding: Specifies the type of encoding used in the message. This is of type AEESMSEncoding

   is91: This specifies the IS-91 standard EP data type

   nLen: Specifies the length of the actual data in the message

   data: Specifies the actual data in the message.



Comments:
None

See Also: 
ITAPI_ExtractSMSText(), NMASK_TAPI_SMS_TS, NMASK_TAPI_SMS_TEXT

=======================================================================
PFNSMSSTATUS

Description:

This is the prototype of the notification function whose address must be passed
to ITAPI_SendSMS() if the application wants to know about the status of the 
MO-SMS just sent. When the SMS msg is sent using ITAPI_SendSMS(), this notifier function
is invoked by BREW to inform the application about the status of the msg delivery

Definition:
typedef void (*PFNSMSSTATUS)(void * pUser, int status);

Members:
pUser: User-specific data that will be passed back to the notification function when invoked
       by BREW
status: Status of the msg delivery. This will be set to AEEMOSMS_ERR_*
===pre>
      AEEMOSMS_ERR_NO_ERR            : SUCCESSfully sent. Indicates the message has been sent to network.
      AEEMOSMS_ERR_OUTOF_RESOURCES   : Device is temporarily out of resources.
      AEEMOSMS_ERR_NETWORK_NOT_READY : Network is not reachable or ready for device to communicate.
      AEEMOSMS_ERR_TRANSPORT_TEMP    : Network temporarily refused message. Application may retry.
      AEEMOSMS_ERR_TRANSPORT_PERM    : Network rejected the message.
      AEEMOSMS_ERR_INTERNAL          : An internal error occured.
===/pre>      

Comments:
 None

See Also:
 ITAPI_SendSMS

=============================================================================
AEESMSTextMsg

Description:
    This structure is given to the application as a dwParam parameter of the EVT_NOTIFY
event as part of the NMASK_TAPI_SMS_TEXT and NMASK_TAPI_WAP_PUSH notifications.

Definition:
    typedef struct {
    uint16 nChars;
    char szText[1];
    } AEESMSTextMsg;

Members:
    nChars  :  Number of relevant bytes in message data.
    szText[1]  :  Message Data.

Comments:
    None

See Also:
    ITAPI_ExtractSMSText()

=============================================================================

    PhoneState

Description:
    PhoneState is used in ITAPI_GetStatus() to get the current state of the device.
This is one of the members in the TAPIStatus data structure that is filled by the
ITAPI_GetStatus() function.

Definition:
    typedef enum {
    PS_OFFLINE,
    PS_IDLE,
    PS_INCOMING,
    PS_ORIG,
    PS_CONVERSATION
    } PhoneState;

Members:
    PS_OFFLINE :  Device is in offline state.
    PS_IDLE :  Device is in Idle state.
    PS_INCOMING :  There is an incoming call to the device.
    PS_ORIG :  Device is in the process of originating a call.
    PS_CONVERSATION :  Device is in the middle of a call.

Comments:
    None

See Also:
    TAPIStatus

=======================================================================
  INTERFACES   DOCUMENTATION
======================================================================= 

ITAPI Interface

ITAPI is a simple interface to the telephony layer in the device. It provides the
following services:

- Retrieving Telephony status

- Placing voice calls

- Extracting SMS text from SMS messages

- Obtaining caller ID on incoming/in-progress calls

- Registering for SMS Messages

- Sending SMS Messages

The ITAPI interface is obtained via the ISHELL_CreateInstance mechanism. 

NOTE: ITAPI interface is DEPRECATED and is provided for backward compatibility only.
      Applications must use ISMS and ITelephone family of interfaces for SMS and
      telephony respectively.

===H2>
Notifications Sent by this Class:
===/H2>

The AEECLSID_TAPI class allows applications to register for the following Notifications:
~
a. NMASK_TAPI_STATUS~
b. NMASK_TAPI_SMS_TEXT~
c. NMASK_TAPI_SMS_TS~
d. NMASK_TAPI_WAP_PUSH~
*

Receiving SMS Messages:

A BREW applet can receive:
1. BREW Directed SMS Messages targeted to that applet

   - Sent as //BREW:<ClassID>:<Message>. <ClassID> is used to determine the class ID of
     the destination applet using STRTOUL() and thus is subject to the success or failure
     of STRTOUL() in determination of destination class ID.
   - ITAPI_SendSMS() internally formats message to this type when destination BREW applet 
     class ID is specified
	 
2. All text messages
3. All messages of specific TeleServiceID
4. All text messages that have a specific prefix

Receiving applet directed message: 

   - No registration is required for receiving applet directed message
   - Received as EVT_APP_MESSAGE event
   - dwParam contains pointer to the message payload which can be accessed as
===pre>
		char * pszMsgTxt = (char *)dwParam;
===/pre		
===H2>
Receiving any text message: 
===/H2>

   - Applet must register with AEECLSID_TAPI class using NMASK_TAPI_SMS_TEXT mask
   - Received as EVT_NOTIFY event where dwParam contains pointer to AEENotify
   - Message can be accessed as 
~
===pre>
      AEENotify * pNotify = (AEENotify*)dwParam;
      AEESMSTextMsg * pTextMsg = (AEESMSTextMsg*)pNotify->pData;
===/pre>
*	  
===H2>
Receiving TeleService specific message:
===/H2>

   - Applet must register with AEECLSID_TAPI class using following as mask
		uint32 mask = (NMASK_TAPI_SMS_TS |(SMS_TELESERVICE_ID << 16))
   - Received as EVT_NOTIFY event where dwParam contains pointer to AEENotify 
   - Message can be accessed as 
 ~
===pre>
	AEENotify * pNotify = (AEENotify*)dwParam;
	AEESMSMsg *pMsg = (AEESMSMsg *)pNotify->pData;
===/pre>
*	
===H2>
Receiving text messages with specific prefix:
===/H2>

   - Message format is //<SMS_Prefix>:<Message_Text>
   - Applet must register as handler for <SMS_Prefix>
   - Received as EVT_APP_BROWSE_URL
   - dwParam contains pointer to //<SMS_Prefix>:<Message_Text>

===H2>
Registering for Device Status Change:
===/H2>

NMASK_TAPI_STATUS: Applications can use the AEECLSID_TAPI class to be notified whenever there
is a change in the telephony status of the device. To register for this notification,
applications must use the mask NMASK_TAPI_STATUS.

Whenever there is a status-change, applications receive the EVT_NOTIFY event. The
dwParam of this event is of type AEENotify.

The pData member inside this AEENotify structure is of type TAPIStatus and contains
detailed information about the current telephony status of the device.
===H2>
Receiving WAP push messages: 
===/H2>

   - Applet must register with AEECLSID_TAPI class using NMASK_TAPI_WAP_PUSH mask
   - Received as EVT_NOTIFY event where dwParam contains pointer to AEENotify
   - Message can be accessed as 
~
===pre>
      AEENotify * pNotify = (AEENotify*)dwParam;
      AEESMSTextMsg * pTextMsg = (AEESMSTextMsg*)pNotify->pData;
===/pre>
*	  

List of Header files to be included

The following header file is required for ITAPI 

AEETAPI.h

=============================================================================

ITAPI_AddRef()

Description:
    This function is inherited from IBASE_AddRef(). 

Version:
   Introduced BREW Client 1.0

See Also:
    ITAPI_Release()

=============================================================================

ITAPI_ExtractSMSText()

Description:
    This function extracts the formatted text from a raw SMS message. The format
of the input parameter is an AEESMSMsg. 
    The typical means to use this function:
     -    When applications register for a text message by using the notification
mask NMASK_TAPI_SMS_TS and by specifying the TeleService ID as SMS_TELESERVICE_CMT_95,
the application gets notified using the EVT_NOTIFY mechanism.
     -    The dwParam to this event is of type AEENotify. The pData inside the AEENotify
structure is of type AEESMSMsg.
     -    For text messages, this function ITAPI_ExtractSMSText() can be invoked
using this AEESMSMsg so as to extract the actual text portion of the message.
     -    A recommended way for applications to register for text messages is using
the notification mask: NMASK_TAPI_SMS_TEXT.
     -    When this mask is used, the pData inside the AEENotify structure received
during the notification is already of type AEESMSTextMsg and contains the actual
text of the message.

Prototype:
    AEESMSTextMsg * ITAPI_ExtractSMSText
    (
    ITAPI * pITAPI,
    const AEESMSMsg * pMsg
    )

Parameters:
    pITAPI  :  Pointer to the ITAPI Interface object.
    pMsg :  Pointer to the input AEESMSMsg.

Return Value:
    NULL, if this function fails.

    If successful, this function returns a pointer to AEESMSTextMsg containing the
   actual text. This buffer is valid until the next call to ITAPI_ExtractSMSText()
   or until the interface is released.

Comments:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    AEESMSMsg
    AEESMSTextMsg

=============================================================================

ITAPI_GetCallerID()

Description:
    This function retrieves the ID, in digits, of an incoming or outgoing voice
call.

Prototype:
    boolean ITAPI_GetCallerID(ITAPI * pITAPI, AECHAR * pDest, int nSize)

Parameters:
    pITAPI :  Pointer to the ITAPI Interface object.
    pDest :  Destination pointer.
    nSize :  Size in bytes of the destination buffer.

Return Value:
    TRUE, if call in progress and buffer filled.

    FALSE, if no call in progress or invalid buffer.

Comments:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    None

=============================================================================

ITAPI_GetStatus()

Description:
    This function obtains the current status of the telephony device, including
service and call status. Applications can also register to receive updated TAPIStatus
information on any changes through the ISHELL_RegisterNotify() function.

Prototype:
    int ITAPI_GetStatus(ITPAI * pITAPI, TAPIStatus * ps)

Parameters:
    pITAPI :  Pointer to the ITAPI Interface object.
    ps :  Pointer to the status information to be filled.

Return Value:
    SUCCESS, if valid status information.

    EBADPARM, if bad parameter.

Comments:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    TAPIStatus
    ISHELL_RegisterNotify()

=============================================================================

ITAPI_IsDataSupported()

Description:
    This method can be used to determine whether the data services are available.
Data services are available when both handset device and wireless network have enough
resources and are capable of handling service.

Prototype:
    boolean ITAPI_IsDataSupported(ITapi *pITAPI)

Parameters:
    pITAPI :  Pointer to the ITAPI object

Return Value:
    TRUE, if the handset device and netowrk can handle data service

    FALSE, if the handset device or network cannot handle data service.

Comments:
    None

Version:
   Introduced BREW Client 2.0

See Also:
    None

=============================================================================

ITAPI_IsVoiceCall()

Description:
    This method can be used to determine whether the current call in the system
is a voice call.

Prototype:
    boolean ITAPI_IsVoiceCall(ITapi *pITAPI)

Parameters:
    pITAPI :  Pointer to the ITAPI object

Return Value:
    TRUE, if the current call is a voice call

    FALSE, if the current call is NOT a voice call

Comments:
    None

Version:
   Introduced BREW Client 2.0

See Also:
    ITAPI_MakeVoiceCall()

=============================================================================

ITAPI_MakeVoiceCall()

Description:
    This method is called to place a voice call. The number dialed is specified
in the digits string. No call is placed if the input string is empty or NULL. Only
the following digits are allowed: 0-9, #, *. All other digits are ignored. If a
voice call is in progress EALREADY is returned. If a data call is in progress the 
data call is suspended and the voice call is placed.

    This function enforces the privacy policies established by the carrier. This
may include intermediate prompts to the user using dialogs.

    Typically, when this function is invoked, a dialog is displayed to the user
requesting whether it is OK to place a call. When the user clicks "YES", the call
is placed.

    The event flow to the application when this function is invoked:
    1.   A dialog is displayed to the user.
    2.   When that dialog is dismissed, the event EVT_DIALOG_END is sent to the
application. 
    3.   At this point, the application must re-draw the screen.
    4.   If user accepted to place the call, the event EVT_APP_SUSPEND is sent to
the application.
    5.   When the call finishes, the event EVT_APP_RESUME is sent to the application.
    6.   The application must re-draw the screen. 

Prototype:
    int ITAPI_MakeVoiceCall
    (
    ITAPI * pITAPI, 
    const char * pszNumber, 
    AEECLSID clsReturn
    )

Parameters:
    pITAPI :  Pointer to the ITAPI Interface object
    pszNumber :  Pointer to number to dial
    clsReturn :  Classid of the applet to be run when the call is completed

Return Value:
    SUCCESS, if the function is in progress.

    EBADPARM, if the number is invalid.

    EALREADY, if there is a voice call already in progress.

Comments:
   If clsReturn is 0, the current application will be resumed. When ITAPI_MakeVoiceCall is invoked, a privacy
   dialog will prompt for placing a voice call. After the privacy dialog is selected by the user, EVT_DIALOG_END 
   is sent to the application with dwParam indicating the response." 
 
The dwParam has 1 for a "Yes" response and 2 for "No" response from user

Version:
   Introduced BREW Client 1.0

See Also:
    AEECLSID

=============================================================================

ITAPI_OnCallEnd()

Description:
    This method is identical to ITAPI_OnCallStatus().

Prototype:
    int ITAPI_OnCallEnd
    (
    ITAPI * pITAPI,
    PFNNOTIFY pfn,
    void * pUser,
    uint32 dwDelay,
    uint16 wFlags
    )

Parameters:
    pITAPI :  Pointer to the ITAPI object
    pfn :  Notification function to be called when any call-status changes
    pUser :  User Data to be passed to the notification function when it is invoked.
    dwDelay :  The time period in milliseconds BREW waits after the call-state has
changed and before notifying the application
    wFlags :  The following flags are supported-
~
     OCS_CANCEL  -  Cancel a previously registered Callback Function
     OCS_UNIQUE_PFN -  When this flag is set, any previous registrations of the
same callback function with different data pointers are cancelled.
     OCS_ONE_SHOT -  Informs BREW that this CB function is to be registered for
just one notification. Once a single call-status change occurs, this notification
is invoked and the CB function is removed from the internal list. This CB will not
longer be invoked.
     OCS_INCOMING -  Register for notifications when there is an incoming call
     OCS_ORIG -  Register for notifications when call-originations happen
     OCS_CONVERSATION -  Register for notifications when call enters the conversation
state (i.e. Two way state)
     OCS_IDLE -  Register for notifications when the device enters into idle state
     OCS_OFFLINE -  Register for notifications when the device loses coverage
     OCS_ALL -  Register for all call-state transitions (incoming, orig, conversation,
idle)
*

Return Value:
    SUCCESS, if successfully registered

    ENOMEMORY, if failed

Comments:
    None

Version:
   Introduced BREW Client 2.0

See Also:
    See ITAPI_OnCallStatus()

=============================================================================

ITAPI_OnCallStatus()

Description:
    This method can be used to register a Callback function that will be invoked
by BREW when there is a change in the call-status. It supports flags that can be
used to specify what type of call-states the application cares about.

Prototype:
    int ITAPI_OnCallStatus
    (
    ITAPI * pITAPI,
    PFNNOTIFY pfn,
    void * pUser,
    uint32 dwDelay,
    uint16 wFlags
    )

Parameters:
    pITAPI :  Pointer to the ITAPI object
    pfn :  Notification function to be called when any call-status changes
    pUser :  User Data to be passed to the notification function when it is invoked.
    dwDelay :  The time period in milliseconds BREW waits after the call-state has
changed and before notifying the application
    wFlags :  The following flags are supported -
~
     OCS_CANCEL  -  Cancel a previously registered Callback Function
     OCS_UNIQUE_PFN -  When this flag is set, any previous registrations of the
same callback function with different data pointers are cancelled.
     OCS_ONE_SHOT -  Informs BREW that this CB function is to be registered for
just one notification. Once a single call-status change occurs, this notification
is invoked and the CB function is removed from the internal list. This CB will not
longer be invoked.
     OCS_INCOMING -  Register for notifications when there is an incoming call
     OCS_ORIG -  Register for notifications when call-originations happen
     OCS_CONVERSATION -  Register for notifications when call enters the conversation
state (i.e. Two way state)
     OCS_IDLE -  Register for notifications when the device enters into idle state
     OCS_OFFLINE -  Register for notifications when the device loses coverage
     OCS_ALL -  Register for all call-state transitions (incoming, orig, conversation,
idle)
*

Return Value:
    SUCCESS, if successfully registered

    ENOMEMORY, if failed

Comments:
    None

Version:
   Introduced BREW Client 2.0

See Also:
    None

=============================================================================

ITAPI_Release()

Description:
    This function is inherited from IBASE_Release(). 

Version:
   Introduced BREW Client 1.0

See Also:
    ITAPI_AddRef()

=============================================================================

ITAPI_SendSMS()

Description:
    This method is used to send SMS messages from the handset.This function can
be used to send text messages to either a specific BREW application on another handset
or a general text message to the handset. It sends messages to a destination mobile
number or an email ID. When this function is called, it is likely that it will result
in a SUSPEND/RESUME sequence to the BREW application. This is initiated by the handset
when it sends the SMS message out.~
    The sequential flow of control is as follows:
~
    1.   The application invokes ITAPI_SendSMS()
    2.   The handset begins the process of sending the SMS message out
    3.   The notification function specified by pfn is invoked with the status of
the message delivery
*

    Associated with this process, the application may receive EVT_APP_SUSPEND and
EVT_APP_RESUME events sometime after step 1.

Prototype:
    int ITAPI_SendSMS
    (
    ITAPI *pITapi, 
    const char * pszDst, 
    const char * pszMsg, 
    AEECLSID clsDst,
    PFNSMSSTATUS pfn, 
    void *pUser
    )

Parameters:
    pITapi :  Pointer to the ITAPI Interface object

    pszDst :  Number or email ID of the destination where message must be sent to.
              If this is set to NULL and if clsDst is non-zero, this function sends 
              the EVT_APP_MESSAGE event to the application (clsDst) on the local 
			  handset and the dwParam of that event shall contain pszMsg. 
			  In this case, the return value of the function is the same as the 
			  return value of ISHELL_SendEvent(). The notification function will not
			  be called since this is a local delivery of the message. 

     pszMsg : Text message to be sent to the destination mobile. 
	           ITAPI_SendSMS() interprets the text message to be in encoding specified 
              by device item AEE_DEVICEITEM_TAPI_SMS_ENCODING.

     clsID :  If non-zero, it specifies the class ID of the BREW application on the
	          destination mobile to which this message must be sent 

     pfn :    Notification function that is invoked to inform the status of 
	          the SMS message sent 

    pUser :  User data to be sent to the notification function 

    Examples -

    Following examples demonstrate the use of ITAPI_SendSMS() on a device where
    device item AEE_DEVICEITEM_TAPI_SMS_ENCODING value is AEE_ENC_ISOLATIN1.
===pre>
    ITAPI_SendSMS (pITapi, "8581112222", "Hello World", 0, MyMOSMSNotify, pMe );
    ITAPI_SendSMS ( pITapi, "foo@sample.com", "Hello World", 0, MyMOSMSNotify, pMe );
===/pre>
    Likewise, following examples demonstrate the use of ITAPI_SendSMS() on a device where
    device item AEE_DEVICEITEM_TAPI_SMS_ENCODING value is AEE_ENC_UNICODE.
===pre>
    ITAPI_SendSMS (pITapi, "8581112222", (char*)STRTOWSTR("Hello World", pBuffer, nBufSize), 0, MyMOSMSNotify, pMe );
    ITAPI_SendSMS ( pITapi, "foo@sample.com", (char*)STRTOWSTR("Hello World", pBuffer, nBufSize), 0, MyMOSMSNotify, pMe );
===/pre>

Return Value:
    SUCCESS, if message is accepted for delivery by ITAPI. This success doesn't indicate
    the successful submission of the message to SMSC for delivery.

    EITEMBUSY, if ITAPI determines that the device is busy and cannot send this SMS. 
    Generally, a SMS message cannot be sent if the notification function has not yet been 
    called for a previously sent SMS message.

    EBADPARM, if pszMsg is NULL or pszDst and clsDst are both set to NULL.

    EBADCLASS, if TAPI is not enabled on this handset.

    EFAILED, if there is a general failure in sending the SMS.

Comments:
    NOTE: This function may cause some devices to experience difficulties when attempting
to send long messages.

    NOTE: The status passed to the notification function will be success if message was 
          successfully submitted to SMSC for delivery. The status passed to the notification 
          function can be EITEMBUSY or EFAILED to indicate problem in submitting the message
          to SMSC for delivery.

Version:
   Introduced BREW Client 2.0

See Also:
    None

=======================================================================*/


#endif    // AEETAPI_H


#ifndef AEEGSMSMS_H
#define AEEGSMSMS_H
/*===========================================================================

                             A E E G S M S M S . H
                

GENERAL DESCRIPTION
   AEE GSM SMS Interfaces (Base level definitions, typedefs, etc. for AEEGSMSMS)

EXTERNALIZED FUNCTIONS
   
INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001,2002 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "AEE.h"

/*===========================================================================

                      CONSTANTS

===========================================================================*/
#define AEECLSID_IGSMSMS            0x01011ccd
#define AEECLSID_IGSMSMSNOTIFIER    0x01011e9b

#define NMASK_GSMSMS_MT_MESSAGE  0x0001 // Mobile terminated message notification
#define NMASK_GSMSMS_INIT        0x0002 // The interface is fully initialized

// The following are used with the msgParam list below
#define GSMSMS_SIM_MO    1
#define GSMSMS_SIM_MT    2
#define GSMSMS_SIM_FREE  4
#define GSMSMS_SIM_ALL   0x0F

#define AEE_GSMSMS_SUCCESS            0
#define AEE_GSMSMS_EFAILED          (-1)
#define AEE_GSMSMS_EBADPARAM        (-2)
#define AEE_GSMSMS_EBUSY            (-4)
#define AEE_GSMSMS_EENCODE          (-5)
#define AEE_GSMSMS_ENODEFAULTSCADDR (-6)
#define AEE_GSMSMS_ESTORE_FULL      (-7)
#define AEE_GSMSMS_ETIMEDOUT        (-8)
#define AEE_GSMSMS_ENOSERVICE       (-9)
#define AEE_GSMSMS_ESST             (-10)

#define GSMSMS_MAX_TL_MESSAGE_LEN            159 // GSM 3.40 9.2.2.1a
#define GSMSMS_MAX_SIM_MESSAGE_LEN           176
#define GSMSMS_MAX_ADDRESS_LEN               20  // GSM 3.40
#define GSMSMS_MAX_SC_ADDRESS_LEN            11  

// User Data Lengths 
#define GSMSMS_MAX_SMS_SUBMIT_UD_LEN         140 // GSM 3.40 9.2.2.2
#define GSMSMS_MAX_SMS_DELIVER_UD_LEN        140 // GSM 3.40 (not specified, other refs used)
#define GSMSMS_MAX_SMS_DELIVER_REPORT_UD_LEN 159 // GSM 3.40 9.2.2.1a
#define GSMSMS_MAX_SMS_SUBMIT_REPORT_UD_LEN  152 // GSM 3.40 9.2.2.2a
#define GSMSMS_MAX_SMS_STATUS_REPORT_UD_LEN  143 // GSM 3.40 9.2.2.3
#define GSMSMS_MAX_SMS_COMMAND_CD_LEN        156 // GSM 3.40 9.2.2.4

// The GSMSMSEncodingType is used to specify the UserData encoding
#define  GSMSMS_ENCODING_7BIT          0
#define  GSMSMS_ENCODING_8BIT          (1 << 2)
#define  GSMSMS_ENCODING_UCS2          (2 << 2)

// GSMSMSStorageType values
#define GSMSMS_STORE_NVRAM             0
#define GSMSMS_STORE_NVRAM_VM          3
#define GSMSMS_STORE_SIM               4

// Flag to indicate that any free slotcan be used
#define GSMSMS_INDEX_ANY               0xFFFF

// The GSMSMSDCSCodingGroup is part of the Data Coding Scheme information
#define GSMSMS_DCS_CG_GENERAL_DATA     0
#define GSMSMS_DCS_CG_MESSAGE_WAITING  1
#define GSMSMS_DCS_CG_RESERVED         2
#define GSMSMS_DCS_CG_NOT_PRESENT      3

// The GSMSMSMsgClassType is part of the Data Coding Scheme information
#define GSMSMS_MSG_CLASS_0             0 
#define GSMSMS_MSG_CLASS_1             1 
#define GSMSMS_MSG_CLASS_2             2 
#define GSMSMS_MSG_CLASS_3             3 
#define GSMSMS_MSG_CLASS_NONE          0xff

// GSMSMSMsgType values
#define GSMSMS_MSG_SMS_SUBMIT           1
#define GSMSMS_MSG_SMS_SUBMIT_REPORT    2
#define GSMSMS_MSG_SMS_DELIVER          3
#define GSMSMS_MSG_SMS_DELIVER_REPORT   4
#define GSMSMS_MSG_SMS_STATUS_REPORT    5
#define GSMSMS_MSG_SMS_COMMAND          6
#define GSMSMS_MSG_SMS_UNKNOWN          7

// The GSMSMSSIMEntryType is part of the SIM entry status
#define GSMSMS_SIM_ENTRY_FREE           0 
#define GSMSMS_SIM_ENTRY_MT             1 
#define GSMSMS_SIM_ENTRY_MO             2 

// GSMSMSVPFormat values
#define GSMSMS_VP_FORMAT_NOT_PRESENT    0
#define GSMSMS_VP_FORMAT_ENHANCED       1
#define GSMSMS_VP_FORMAT_RELATIVE       2
#define GSMSMS_VP_FORMAT_ABSOLUTE       3

// GSMSMSRawFormat values
#define GSMSMS_RAW_FORMAT_GSM           0
#define GSMSMS_RAW_FORMAT_SIM           1

// GSMSMSTonType values
#define GSMSMS_TON_UNKNOWN                   0  // not supported
#define GSMSMS_TON_INTERNATIONAL_NUMBER      1 
#define GSMSMS_TON_NATIONAL_NUMBER           2 
#define GSMSMS_TON_NETWORK_SPECIFIC_NUMBER   3  // not supported
#define GSMSMS_TON_SUBSCRIBER_NUMBER         4  // not supported
#define GSMSMS_TON_ALPHANUMERIC              5  // not supported
#define GSMSMS_TON_ABBREVIATED_NUMBER        6  // not supported
#define GSMSMS_TON_RESERVED                  7  // not supported

// GSMSMSNpiType values
#define GSMSMS_NPI_UNKNOWN                   0
#define GSMSMS_NPI_ISDN_TELEPHONE            1
#define GSMSMS_NPI_DATA                      3
#define GSMSMS_NPI_TELEX                     4
#define GSMSMS_NPI_SERVICE_CENTRE_SPECIFIC_1 5
#define GSMSMS_NPI_SERVICE_CENTRE_SPECIFIC_2 6
#define GSMSMS_NPI_NATIONAL                  8
#define GSMSMS_NPI_PRIVATE                   9
#define GSMSMS_NPI_ERMES                     10
#define GSMSMS_NPI_RESERVED                  15

// GSMSMSStatusType values match those in the SIM/RUIM card
#define GSMSMS_STATUS_NONE                    0x00
#define GSMSMS_STATUS_MT_READ                 0x01
#define GSMSMS_STATUS_MT_NOT_READ             0x03
#define GSMSMS_STATUS_MO_SENT                 0x05 // No status report requested 
#define GSMSMS_STATUS_MO_NOT_SENT             0x07
#define GSMSMS_STATUS_MO_SENT_ST_NOT_RECEIVED 0x0D
#define GSMSMS_STATUS_MO_SENT_ST_NOT_STORED   0x15
#define GSMSMS_STATUS_MO_SENT_ST_STORED       0x35

// GSMSMSMsgWaitingIndType values
#define GSMSMS_MSG_WAITING_VOICEMAIL          0
#define GSMSMS_MSG_WAITING_FAX                1
#define GSMSMS_MSG_WAITING_EMAIL              2
#define GSMSMS_MSG_WAITING_OTHER              3

/*===========================================================================

                      TYPES 

===========================================================================*/
// Simuate enums with the typedefs
typedef byte GSMSMSEncodingType;
typedef byte GSMSMSStorageType;
typedef byte GSMSMSDCSCodingGroup;
typedef byte GSMSMSMsgClassType;
typedef byte GSMSMSMsgType;
typedef byte GSMSMSSIMEntryType;
typedef byte GSMSMSVPFormat;
typedef byte GSMSMSRawFormat;
typedef byte GSMSMSTonType;
typedef byte GSMSMSNpiType;
typedef byte GSMSMSStatusType;
typedef byte GSMSMSMsgWaitingIndType;

typedef struct {
    byte  data[GSMSMS_MAX_SIM_MESSAGE_LEN]; // Maximum GSM message length
} GSMSMSRawSIMData;


typedef struct {
   GSMSMSMsgClassType msgClass;
   boolean            isCompressed;
   boolean            autoDelete;
} GSMSMSDCSGeneralData;

typedef struct {
   GSMSMSMsgWaitingIndType indicationType;
   boolean                 setIndication;
   boolean                 store;
} GSMSMSDCSMessageWaiting;

typedef struct _GSMSMSDCS {
   GSMSMSDCSCodingGroup codingGroup;
   GSMSMSEncodingType   msgCoding;
   union {
      GSMSMSDCSGeneralData    generalData; 
      GSMSMSDCSMessageWaiting messageWaiting;
   } cg;
} GSMSMSDCS;

typedef struct {
   uint16 year;     // year is in yyyy format. Only >= 1996 is valid.
   uint8 month;     // decimal format 
   uint8 day;       // decimal format 
   uint8 hour;      // decimal format 
   uint8 minute;    // decimal format 
   uint8 second;    // decimal format 
   int16 tzoffset;  // timezone offset in minutes
} GSMSMSTime;

typedef struct {
   uint8 weeks;
   uint8 days;
   uint8 hours;
   uint8 minutes;
   uint8 seconds;
} GSMSMSVPRelative;

typedef struct {
   boolean extension;
   boolean singleShot;
   uint8 enhancedFormat;
   uint8 hours;
   uint8 minutes;
   uint8 seconds;
} GSMSMSVPEnhanced;

typedef struct {
   GSMSMSTime time;
} GSMSMSVPAbsolute;

typedef struct {
   GSMSMSVPFormat format;
   union {
      GSMSMSVPRelative relative;
      GSMSMSVPEnhanced enhanced;
      GSMSMSVPAbsolute absolute;
   } vp;
} GSMSMSVP;

typedef struct {
   byte          addr[GSMSMS_MAX_ADDRESS_LEN+1];   // Includes extra character for null termination
   GSMSMSTonType ton;        // Type of Number (TON)
   GSMSMSNpiType npi;        // Number Plan Indicator (NPI)
} GSMSMSAddress;

typedef struct _GSMSMSSubmitType {
    GSMSMSAddress SCAddr; // Service Center Address
    boolean       RD;     // Reject Duplicates
    GSMSMSVP      VP;     // Validity Period
    boolean       SRR;    // Status Report Request
    boolean       UDHI;   // User Data Header Indicator
    boolean       RP;     // Reply Path
    byte          MR;     // Message Reference
    GSMSMSAddress DA;     // Destination Address
    byte          PID;    // Protocol ID
    GSMSMSDCS     DCS;    // Data Coding Scheme
    byte          UDL;    // User Data Length
    byte          UD[GSMSMS_MAX_SMS_SUBMIT_UD_LEN];// User Data
} GSMSMSSubmitType;

typedef struct _GSMSMSDeliverType {
    GSMSMSAddress SCAddr; // Service Center Address
    boolean       MMS;    // More Message to Send
    boolean       SRI;    // Status Report Indicator
    boolean       UDHI;   // User Data Header Indicator
    boolean       RP;     // Reply Path
    GSMSMSAddress OA;     // Origination Address
    byte          PID;    // Protocol ID
    GSMSMSDCS     DCS;    // Data Coding Scheme
    GSMSMSTime    SCTS;   // Service Center Time Stamp
    byte          UDL;    // User Data Length
    byte          UD[GSMSMS_MAX_SMS_DELIVER_UD_LEN];// User Data
} GSMSMSDeliverType;

typedef struct _GSMSMSStatusReportType {
    GSMSMSAddress SCAddr; // Service Center Address
    boolean       MMS;    // More Message to Send
    boolean       SRQ;    // Status Report Qualifier
    boolean       UDHI;   // User Data Header Indicator
    byte          MR;     // Message Reference
    GSMSMSAddress RA;     // Recipient Address
    GSMSMSTime    SCTS;   // Service Center Time Stamp
    GSMSMSTime    DT;     // Deliver Time
    byte          ST;     // Status
    boolean       PID_present;
    byte          PID;    // Protocol ID
    boolean       DCS_present;
    GSMSMSDCS     DCS;    // Data Coding Scheme
    boolean       UDL_present;
    byte          UDL;    // User Data Length
    byte          UD[GSMSMS_MAX_SMS_STATUS_REPORT_UD_LEN];// User Data  
} GSMSMSStatusReportType;

typedef struct {
    boolean    UDHI;         // User Data Header Indicator
    boolean    FCS_present;
    byte       FCS;          // Failure Cause
    GSMSMSTime SCTS;         // Service Center Time Stamp
    boolean    PID_present;
    byte       PID;          // Protocol ID
    boolean    DCS_present;
    GSMSMSDCS  DCS;          // Data Coding Scheme
    boolean    UDL_present;
    byte       UDL;          // User Data Length
    byte       UD[GSMSMS_MAX_SMS_SUBMIT_REPORT_UD_LEN];      // User Data
}GSMSMSSubmitReportType; 

typedef struct {
    boolean    UDHI;         // User Data Header Indicator
    boolean    FCS_present;
    byte       FCS;          // Failure Cause
    boolean    PID_present;
    byte       PID;          // Protocol ID
    boolean    DCS_present;
    GSMSMSDCS  DCS;          // Data Coding Scheme
    boolean    UDL_present;
    byte       UDL;          // User Data Length
    byte       UD[GSMSMS_MAX_SMS_DELIVER_REPORT_UD_LEN];      // User Data
}GSMSMSDeliverReportType; 

typedef struct {
    GSMSMSMsgType   msgType;
    union {
        GSMSMSSubmitType        SMSSubmit;
        GSMSMSDeliverType       SMSDeliver;
        GSMSMSSubmitReportType  SMSSubmitReport;
        GSMSMSDeliverReportType SMSDeliverReport;
        GSMSMSStatusReportType  SMSStatusReport;
    } msg;
} GSMSMSMsg;

typedef struct {
    GSMSMSStatusType status;
    byte rawMsg[GSMSMS_MAX_SIM_MESSAGE_LEN];
} GSMSMSSIMData;

typedef struct {
    byte data[GSMSMS_MAX_TL_MESSAGE_LEN]; 
} GSMSMSRawData;

typedef struct {
    GSMSMSRawFormat format;
    union {
       GSMSMSRawData gsm;
       GSMSMSSIMData sim;
    } msg;
} GSMSMSRawMsg;

typedef struct {
    int sendResult;
    GSMSMSRawMsg reportMsg;
} GSMSMSSendReport;

typedef struct {
    int sendResult;    // AEE_GSMSMS_SUCCESS if message was received
    boolean rpError;   // True if response was RP-Error
    uint8 rpCause;     // Table 8.4 GSM 24.011
} GSMSMSSMMAReport;

/*===========================================================================

                    CLASS DEFINITIONS

===========================================================================*/
typedef struct _IGSMSMSNotifier IGSMSMSNotifier;

AEEINTERFACE(IGSMSMSNotifier)
{
    INHERIT_INotifier(IGSMSMSNotifier);
};

typedef struct _IGSMSMS IGSMSMS;

AEEINTERFACE(IGSMSMS)
{
   INHERIT_IQueryInterface(IGSMSMS);

   // Check If Interface is fully initialized
   boolean (*IsInit)(IGSMSMS *po);

   // Send Interface
   int (*SendSMSSubmit)(IGSMSMS *po, const GSMSMSSubmitType * pMsg, AEECallback *pCb, GSMSMSSendReport *pReport);
   int (*SendSMSDeliverReport)(IGSMSMS *po, const GSMSMSDeliverReportType *deliverReport);
   int (*SendMoreMemoryAvailable)(IGSMSMS *po, AEECallback *pCb, GSMSMSSMMAReport *pReport);

   // SIM/NV Interface
   int (*StoreMessage)(IGSMSMS *po, const GSMSMSMsg * pMsg, GSMSMSStorageType storeTo, uint16 *pIndex);
   int (*MoveMessage)(IGSMSMS *po, GSMSMSStorageType moveFrom, uint16 fromIndex, GSMSMSStorageType moveTo, uint16 *toIndex);
   int (*StoreStatusReport)(IGSMSMS *po, uint16 index, const GSMSMSMsg * pMsg, GSMSMSStorageType storeTo);
   int (*DeleteAllMessages)(IGSMSMS * po, uint16 msgMask, GSMSMSStorageType deleteFrom);
   int (*DeleteMessage)(IGSMSMS * po, uint16 index, GSMSMSStorageType deleteFrom);
   int (*GetMessage)(IGSMSMS * po, uint16 index, GSMSMSRawMsg *pMsg, GSMSMSStorageType readFrom);
   int (*GetStatusReport)(IGSMSMS * po, uint16 index, GSMSMSRawMsg *pMsg, GSMSMSStorageType readFrom);
   int (*GetStoreSize)(IGSMSMS * po, GSMSMSStorageType readFrom, uint16 *pCount);
   int (*SetMessageStatus)(IGSMSMS * po, uint16 index, GSMSMSStatusType status, GSMSMSStorageType writeTo);
   int (*GetMessageStatus)(IGSMSMS * po, uint16 index, GSMSMSStatusType *status, GSMSMSStorageType readFrom);
   int (*GetSCAddress)(IGSMSMS * po, GSMSMSAddress *address);
   int (*SetSCAddress)(IGSMSMS * po, const GSMSMSAddress *address);
   int (*GetMemoryCapExceededFlag)(IGSMSMS * po, boolean *pExceeded);
   int (*SetMemoryCapExceededFlag)(IGSMSMS * po, boolean exceeded);
   int (*GetTPMR)(IGSMSMS * po, uint8 * pTPMR);
   int (*SetTPMR)(IGSMSMS * po, uint8 TPMR);

   // Helper Functions
   int (*CreateDefaultMessage)(IGSMSMS *po, GSMSMSMsgType type, GSMSMSMsg *pMsg);
   int (*DecodeMessage)(IGSMSMS * po, const GSMSMSRawMsg *pRawMsg, GSMSMSMsg *pMsg);
   int (*DecodeUserData)(IGSMSMS * po, const GSMSMSMsg *pMsg, AECHAR *wstr, uint16 wstrlen);
   int (*EncodeUserData)(IGSMSMS * po, const AECHAR *pwszText, byte *pDest, byte destlen, GSMSMSEncodingType encoding, byte *pEncodedLen);

   // This interface is to be used for testing an application only
   void (*InjectMessage)(IGSMSMS * po, const byte * pData, uint8 length);
};

// IGSMSMSNotifier IBASE and INotifier 'inherited' interface methods
#define IGSMSMSNotifier_AddRef(p)                     AEEGETPVTBL(p,IGSMSMSNotifier)->AddRef(p)
#define IGSMSMSNotifier_Release(p)                    AEEGETPVTBL(p,IGSMSMSNotifier)->Release(p)
#define IGSMSMSNotifier_SetMask(p, a)                 AEEGETPVTBL(p,IGSMSMSNotifier)->SetMask(p,a)

// IGSMSMS IBASE and IQueryInterface 'inherited' interface methods
#define IGSMSMS_AddRef(p)                             AEEGETPVTBL(p,IGSMSMS)->AddRef(p)
#define IGSMSMS_Release(p)                            AEEGETPVTBL(p,IGSMSMS)->Release(p)
#define IGSMSMS_QueryInterface(p,a,b)                 AEEGETPVTBL(p,IGSMSMS)->QueryInterface(p,a,b)

// SMS Specific Methods
#define IGSMSMS_IsInit(p)                             AEEGETPVTBL(p,IGSMSMS)->IsInit(p)
#define IGSMSMS_SendSMSSubmit(p,a,b,c)                AEEGETPVTBL(p,IGSMSMS)->SendSMSSubmit(p,a,b,c)
#define IGSMSMS_SendSMSDeliverReport(p,a)             AEEGETPVTBL(p,IGSMSMS)->SendSMSDeliverReport(p,a)
#define IGSMSMS_SendMoreMemoryAvailable(p,a,b)        AEEGETPVTBL(p,IGSMSMS)->SendMoreMemoryAvailable(p,a,b)

// SIM Specific Methods
#define IGSMSMS_StoreMessage(p,a,b,c)                 AEEGETPVTBL(p,IGSMSMS)->StoreMessage(p,a,b,c)
#define IGSMSMS_MoveMessage(p,a,b,c,d)                AEEGETPVTBL(p,IGSMSMS)->MoveMessage(p,a,b,c,d)
#define IGSMSMS_StoreStatusReport(p,a,b,c)            AEEGETPVTBL(p,IGSMSMS)->StoreStatusReport(p,a,b,c)
#define IGSMSMS_DeleteAllMessages(p,a,b)              AEEGETPVTBL(p,IGSMSMS)->DeleteAllMessages(p,a,b)
#define IGSMSMS_DeleteMessage(p,a,b)                  AEEGETPVTBL(p,IGSMSMS)->DeleteMessage(p,a,b)
#define IGSMSMS_GetMessage(p,a,b,c)                   AEEGETPVTBL(p,IGSMSMS)->GetMessage(p,a,b,c)
#define IGSMSMS_GetStatusReport(p,a,b,c)              AEEGETPVTBL(p,IGSMSMS)->GetStatusReport(p,a,b,c)
#define IGSMSMS_GetStoreSize(p,a,b)                   AEEGETPVTBL(p,IGSMSMS)->GetStoreSize(p,a,b)
#define IGSMSMS_SetMessageStatus(p,a,b,c)             AEEGETPVTBL(p,IGSMSMS)->SetMessageStatus(p,a,b,c)
#define IGSMSMS_GetMessageStatus(p,a,b,c)             AEEGETPVTBL(p,IGSMSMS)->GetMessageStatus(p,a,b,c)
#define IGSMSMS_GetSCAddress(p,a)                     AEEGETPVTBL(p,IGSMSMS)->GetSCAddress(p,a)
#define IGSMSMS_SetSCAddress(p,a)                     AEEGETPVTBL(p,IGSMSMS)->SetSCAddress(p,a)
#define IGSMSMS_GetMemoryCapExceededFlag(p,a)         AEEGETPVTBL(p,IGSMSMS)->GetMemoryCapExceededFlag(p,a)
#define IGSMSMS_SetMemoryCapExceededFlag(p,a)         AEEGETPVTBL(p,IGSMSMS)->SetMemoryCapExceededFlag(p,a)
#define IGSMSMS_GetTPMR(p,a)                          AEEGETPVTBL(p,IGSMSMS)->GetTPMR(p,a)
#define IGSMSMS_SetTPMR(p,a)                          AEEGETPVTBL(p,IGSMSMS)->SetTPMR(p,a)

// Helper Functions
#define IGSMSMS_CreateDefaultMessage(p,a,b)           AEEGETPVTBL(p,IGSMSMS)->CreateDefaultMessage(p,a,b)
#define IGSMSMS_DecodeMessage(p,a,b)                  AEEGETPVTBL(p,IGSMSMS)->DecodeMessage(p,a,b)
#define IGSMSMS_DecodeUserData(p,a,b,c)               AEEGETPVTBL(p,IGSMSMS)->DecodeUserData(p,a,b,c)
#define IGSMSMS_EncodeUserData(p,a,b,c,d,e)           AEEGETPVTBL(p,IGSMSMS)->EncodeUserData(p,a,b,c,d,e)

#define IGSMSMS_InjectMessage(p,a,b)                  AEEGETPVTBL(p,IGSMSMS)->InjectMessage(p,a,b)

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
======================================================================= 
GSMSMSRawMsg

When reading stored messages from the SIM or NVRAM, the messages are
passed as GSMSMSRawMsg structures. The format field is set to 
GSMSMS_RAW_FORMAT_SIM.

These can be decoded using IGSMSMS_DecodeMessage to generate a GSMSMSMsg
structure. The msgType field indicated the type of the message stored
in the GSMSMSMsg structure.

When receiving incoming messages, a GSMSMSRawMsg structure is passed to
the applet. The format field is set to GSMSMS_RAW_FORMAT_GSM.
   
These messages can also be decoded using IGSMSMS_DecodeMessage to 
generate a GSMSMSMsg structure. The msgType field indicated the type 
of the message stored in the GSMSMSMsg structure.


=====================================================================
  INTERFACES   DOCUMENTATION
=========================================================================== 

Interface Name: IGSMSMS

Description: 
   IGSMSMS is a simple interface to the GSM1x support layer in the device.
It provides the following services:

- Sending GSM1x SMS messages (SMS_SUBMIT)
- Extracting SMS text and TL data from GSM1x SMS messages

The IGSMSMS interface is obtained via the ISHELL_CreateInstance mechanism.

=========================================================================== 

===========================================================================
Function: IGSMSMS_IsInit()

Description:

   This function indicates to the user whether the IGSMSMS interface has 
   finished internal initialization.

   If the interface is not initialized, the applet can register for 
   NMASK_GSMSMS_INIT in IGSMSMSNotifier to be notified when the initialization
   is complete.

Prototype:
   boolean IGSMSMS_IsInit(IGSMSMS *po)

Parameters:
   po:      Pointer to the IGSMSMS object

Return Value:
   TRUE if initialized, otherwise FALSE
   
Comments:  
   None

Side Effects: 
   None

===========================================================================
Function: IGSMSMS_SendSMSSubmit()

Description:

   This function allows an application to send a GSM SMS_SUBMIT message. This
   can be used to send a SMS/EMS message and is also used for SIM toolkit.

   The user would first fill out a GSMSMSSubmitType structure either manually
   or by calling IGSMSMS_CreateDefaultMessage to fill in the structure and only
   modifying the parameters that are different from the default. For a simple 
   SMS_SUBMIT message, this would typically be the Destination Address (DA) 
   and the UserData (UD,UDL).
   
   When the SMS_SubmitReport message is received, the pCB function will be 
   called and the status can be retrieved from the GSMSMSSendReport structure.

   If a status report message was requested, the SMS_StatusReport message 
   will be delivered via the callback specified in the OnMTMessage 
   registration.
 
Prototype:
   int IGSMSMS_SendSMSSubmit(IGSMSMS *po, const GSMSMSSubmitType * pMsg, 
                              AEECallback *pCb, GSMSMSSendReport *pReport)

Parameters:
   po:      Pointer to the IGSMSMS object
   pMsg:    Pointer to the SMS Submit message to send
   pCb:     This callback will be invoked by AEE when the SMS_SUBMIT_REPORT is received.
   pReport: Pointer to structure to be filled in reporting the submit status

Return Value:
   AEE_GSMSMS_EBUSY if a transaction is pending
   AEE_GSMSMS_EBADPARAM if pMsg is NULL or pCb is NULL
   AEE_GSMSMS_EENCODE if the message could not be encoded
   AEE_GSMSMS_SUCCESS if successful
   
Comments:  
   None

Side Effects: 
   None

===========================================================================
Function: IGSMSMS_SendSMSDeliverReport()

Description:

   This function allows an application to send a GSM SMS_DELIVER_REPORT message. 
   This should be sent in response to receiving an SMS_DELIVER message.

   The user would first fill out a GSMSMSDeliverReportType structure either manually
   or by calling IGSMSMS_CreateDefaultMessage to fill in the structure and only
   modifying the parameters that are different from the default. For a simple 
   SMS_DELIVER_REPORT message, this would typically be the UserData (GSMSMSSubmitType->UD) 
   and the destination address (GSMSMSSubmitType->DA).
   
Prototype:
   int IGSMSMS_SendSMSDeliverReport(IGSMSMS *po, 
                                    const GSMSMSDeliverReportType *pDeliverReport)

Parameters:
   po:             Pointer to the IGSMSMS object
   pDeliverReport: Pointer to the SMS Deliver Report message to send

Return Value:
   If successful: AEE_GSMSMS_SUCCESS. 
   If the the paremeters were not valid: AEE_GSMSMS_EBADPARAM 
   If the message was not valid: AEE_GSMSMS_EFAILED 
   
Comments:  
   None

Side Effects: 
   None

===========================================================================
Function: IGSMSMS_SendMoreMemoryAvailable()

Description:

   This function allows an application to send a GSM RP Layer SMMA message. 
   This function is called in accordance with the GSM 23.040 spec.

   If the message times out, pReport->sendResult is set to 
   AEE_GSMSMS_ETIMEDOUT. All other pReport fields are invalid.

   If the message is successfully sent, pReport->sendResult is set to 
   AEE_GSMSMS_SUCCESS.

Prototype:
   int IGSMSMS_MoreMemoryAvailable(IGSMSMS *po, AEECallback *pCb, 
                                   GSMSMSSMMAReport *pReport)

Parameters:
   po:      Pointer to the IGSMSMS object
   pCb:     Pointer to AEECallback to call upon response message arrival
   pReport: Pointer to report structure to fill with response

Return Value:
   If successful: AEE_GSMSMS_SUCCESS. 
   If the message could not be sent: AEE_GSMSMS_EFAILED 
   
Comments:  
   None

Side Effects: 
   None

===========================================================================
Function: IGSMSMS_CreateDefaultMessage()

Description:

   This function initializes a Mobile Originated (MO) message structure to
   the default values that are most commonly used.

   The defaults for a GSMSMSSubmitType are:
      SCAddr - Filled in with Default SC Address from SIM
      RD     - Reject Duplicates set to True
      VP     - Validity Period
      SRR    - Status Report Request set to True;
      UDHI   - User Data Header Indicator set to False
      RP     - Reply Path Parameter is not set (RP=False)
      MR     - Message Reference is 0 and is filled in when sent
      DA     - Destination Address is set to "0" and must be 
               overwritten by the application
      PID    - Protocol ID is SME to SME by default
      DCS    - Data coding Scheme is set to GSM-7bit, class None
      UDL    - User Data Length is set to 0
      UD     - User Data is initialized to all zeros

   The defaults for a GSMSMSDeliverReportType are:
      SCAddr      - Filled in with Default SC Address from SIM 
      UDHI        - User Data Header Indicator set to False
      FCS_present - Failure Cause present is set to False (indicating no failure)
      FCS         - Failure Cause is set to 0 (reserved)
      PID_present - Protocol ID is not present (PID_present=False)
      PID         - Protocol ID is SME to SME by default
      DCS_present - Data Coding Scheme is not present (DCS_present=False)
      DCS         - Data coding Scheme is set to GSM-7bit, class None
      UDL_present - User Data Length is not present (DCS_present=False)
      UDL         - User Data Length is set to 0
      UD          - User Data is initialized to all zeros

Prototype:
   int IGSMSMS_CreateDefaultMessage(IGSMSMS *po, GSMSMSMsgType type, GSMSMSMsg *pGsmMsg);

Parameters:
   po:      Pointer to the IGSMSMS object
   type:    The type of message to create
   pGsmMsg: Pointer to the GSM SMS Msg structure to initialize.  

Return Value:
   If successful: AEE_GSMSMS_SUCCESS. 
   If the the paremeters were not valid: AEE_GSMSMS_EBADPARAM 
   If the message was not valid: AEE_GSMSMS_EFAILED
   
Comments:  
   None

Side Effects: 
   None

===========================================================================
Function: IGSMSMS_StoreMessage()

Description:

   This function stores a GSM SMS message on the specified store in  
   the specified index. 

   If *pIndex is set to GSMSMS_STORE_SIM before the call, a free slot will 
   be selected and the value of *pIndex will be updated.  Otherwise, the 
   value of *pIndex will be used.

   If the storeTo parameter is set to GSMSMS_STORE_NVRAM the message will 
   be stored in the first free slot in the NVRAM mailbox. 

   If the storeTo parameter is set to storeTo GSMSMS_STORE_SIM, the message 
   will be stored in the first free slot in the SIM.

   If the storeTo parameter is set to GSMSMS_STORE_NVRAM_VM, the currently
   stored voicemail message is replaced with the specified voicemail
   message. 

Prototype:
   int IGSMSMS_StoreMessage(IGSMSMS *po, const GSMSMSMsg * pMsg, 
                             GSMSMSStorageType storeTo, uint16 *pIndex )

Parameters:
   po:      Pointer to the IGSMSMS object
   pMsg:    Pointer to the GSM SMS Msg to store
   storeTo: Indicates whether to store to the SIM or to NVRAM database
   pIndex:  Pointer to variable containing the index to to store the message at.
            If the index is set to GSMSMS_INDEX_ANY an index will be selected
            and the value of *pIndex will be updated.

Return Value:
   If successful: AEE_GSMSMS_SUCCESS. 
   If the store was full: AEE_GSMSMS_ESTORE_FULL
   If the SIM Service Table has SMS disabled: AEE_GSMSMS_ESST
   If the message could not be stored: AEE_GSMSMS_EFAILED
   
Comments:  
   None

Side Effects: 
   None

=============================================================================
Function: IGSMSMS_MoveMessage()

Description:

   This function moves a GSM SMS message from one store to another. It will also
   allow the the user to move from one slot to another on the same store.

Prototype:
   int IGSMSMS_MoveMessage(IGSMSMS *po, GSMSMSStorageType moveFrom, 
                           uint16 fromIndex, GSMSMSStorageType moveTo, 
                           uint16 *pToIndex )

Parameters:
   po:        Pointer to the IGSMSMS object
   moveFrom:  Store to move the message from
   fromIndex: Index of the message to move
   moveTo:    Store to move the message to
   pToIndex:  Pointer to the index of the stored message, if set to 
              GSMSMS_INDEX_ANY an empty slot will be selected and returned

Return Value:
   If successful: AEE_GSMSMS_SUCCESS. 
   If the store was full: AEE_GSMSMS_ESTORE_FULL
   If the SIM Service Table has SMS disabled: AEE_GSMSMS_ESST
   If the message could not be stored: AEE_GSMSMS_EFAILED
   
Comments:  
   None

Side Effects: 
   None
===========================================================================
Function: IGSMSMS_StoreStatusReport()

Description:

   This function stores a GSM SMS_StatusReport message that cooresponds
   to a SMS_Submit message stored at the specified slot on the SIM or in 
   NVRAM.

   In GSM, the SMSR directory on the SIM contains the StatusReport. This
   entry contains an index into the SMS directory for the corresponding
   SMS Submit message. When the entry in the SMS directory is deleted,
   the corresponding entry in the SMSR is no longer valid.  Therefore, 
   the status report must be stored on the same store as the SMS_Submit 
   message. If the SMS_Submit message is ever moved to another store,
   the SMS_StatusReport must be moved at the same time or it will be lost.

   If the status report is stored in NVRAM only the portion of the message
   that would be saved to the SMSR is preserved.

Prototype:
   int IGSMSMS_StoreStatusReport(IGSMSMS *po, uint16 index, 
                                 const GSMSMSMsg * pMsg, 
                                 GSMSMSStorageType storeTo)

Parameters:
   po:      Pointer to the IGSMSMS object
   index:   Pointer to slot index of the corresponding SMS_Submit message 
   pMsg:    Pointer to the GSM SMS Msg to store
   storeTo: Indicates whether the SMS_Submit message is on the SIM or in 
            NVRAM database

Return Value:
   If successful: AEE_GSMSMS_SUCCESS. 
   If pMsg is NULL or storeTo is invalid: AEE_GSMSMS_EBADPARAM 
   If the SIM Service Table has SMS disabled: AEE_GSMSMS_ESST
   If the message could not be stored: AEE_GSMSMS_EFAILED
   
Comments:  
   None

Side Effects: 
   None

===========================================================================
Function: IGSMSMS_DeleteAllMessages()

Description:

   This function deletes all entries of a specified type from the store 
   (SIM, NVRAM or NVRAM Voivemail). It provides a way to delete only mobile 
   originated (MO), mobile terminated (MT), or all messages.

Prototype:
   int IGSMSMS_DeleteAllMessages(IGSMSMS * po, uint16 msgMask, 
                                   GSMSMSStorageType deleteFrom)

Parameters:
   po:         Pointer to the IGSMSMS object
   msgMask:    Indicates the type of messages to delete. Any combination of the following:
                  GSMSMS_SIM_MO 
                  GSMSMS_SIM_MT 
                  GSMSMS_SIM_ALL 
            
   deleteFrom: Indicates whether to delete from the SIM or from NVRAM database.

Return Value:
   If successful: AEE_GSMSMS_SUCCESS. 
   If deleteFrom is invalid: AEE_GSMSMS_EBADPARAM 
   If the SIM Service Table has SMS disabled: AEE_GSMSMS_ESST
   If the messages could not be deleted: AEE_GSMSMS_EFAILED
   
Comments:  
   None

Side Effects: 
   None

===========================================================================
Function: IGSMSMS_DeleteMessage()

Description:

   This function deletes an entry from a specified slot on the store 
   (SIM, NVRAM or NVRAM Voicemail).

Prototype:
   void IGSMSMS_DeleteMessage(IGSMSMS * po, uint16 index, 
                              GSMSMSStorageType deleteFrom)

Parameters:
   po:         Pointer to the IGSMSMS object
   index:      Slot index of message to delete
   deleteFrom: Indicates whether to delete from the SIM or from NVRAM database.

Return Value:
   If successful: AEE_GSMSMS_SUCCESS. 
   If deleteFrom is invalid: AEE_GSMSMS_EBADPARAM 
   If the SIM Service Table has SMS disabled: AEE_GSMSMS_ESST
   If the message could not be deleted: AEE_GSMSMS_EFAILED
   
Comments:  
   None

Side Effects: 
   None

===========================================================================
Function: IGSMSMS_GetStatusReport()

Description:

   This function retrieves an SMS StatusReport entry that corresponds to a
   SMS_Submit message in a specified slot on the store (SIM or NVRAM).

Prototype:
   int IGSMSMS_GetStatusReport(IGSMSMS * po, uint16 index, 
                               GSMSMSRawMsg *pMsg,
                               GSMSMSStorageType readFrom)

Parameters:
   po:       Pointer to the IGSMSMS object
   index:    Slot index of the SMS_Submit corresponds to the status report
   pMsg:     Pointer to the GSM SMS Msg to write the status report to
   readFrom: Indicates whether to read from the SIM or from NVRAM database.

Return Value:
   If successful: AEE_GSMSMS_SUCCESS 
   If readFrom is invalid: AEE_GSMSMS_EBADPARAM 
   If the SIM Service Table has SMS disabled: AEE_GSMSMS_ESST
   If the message could not be read: AEE_GSMSMS_EFAILED
   
Comments:  
   None

Side Effects: 
   None

===========================================================================
Function: IGSMSMS_GetMessage()

Description:

   This function retrieves an SMS entry from a specified slot on the store 
   (SIM or NVRAM).

   The message is retrieved in raw format and can be decoded using 
   IGSMSMS_DecodeMessage and IGSMSMS_DecodeUserData.

Prototype:
   int IGSMSMS_GetMessage(IGSMSMS * po, uint16 index, GSMSMSRawMsg *pMsg,
                          GSMSMSStorageType readFrom)

Parameters:
   po:       Pointer to the IGSMSMS object
   index:    Slot index of message to retrieve
   pMsg:     Pointer to struct to be filled in with the retrieved message
   readFrom: Indicates whether to read from the SIM or from NVRAM database

Return Value:
   If successful: AEE_GSMSMS_SUCCESS. 
   If pMsg is NULL, or readFrom is invalid: AEE_GSMSMS_EBADPARAM 
   If the SIM Service Table has SMS disabled: AEE_GSMSMS_ESST
   If the messages could not be read: AEE_GSMSMS_EFAILED
   
Comments:  
   None

Side Effects: 
   None

===========================================================================
Function: IGSMSMS_SetMessageStatus()

Description:

   This function updates the status for a message in the specified store.

   It can be used to:
        - mark a MT message as read or unread
        - mark a MO message as sent or not sent
        - mark a MO message as having a pending status report
        - mark a MO message as having a received status report
        - mark a MO message as having a stored status report

Prototype:
   int IGSMSMS_SetMesageStatus(IGSMSMS * po, uint16 index, 
                               GSMSMSStatusType status, 
                               GSMSMSStorageType writeTo)

Parameters:
   po:       Pointer to the IGSMSMS object
   index:    Slot index of message to mark
   status:   The status to write
   writeTo:  Indicates whether to use the SIM or NVRAM database

Return Value:
   If successful: AEE_GSMSMS_SUCCESS 
   If writeTo is invalid: AEE_GSMSMS_EBADPARAM 
   If the SIM Service Table has SMS disabled: AEE_GSMSMS_ESST
   If the entry could not be updated: AEE_GSMSMS_EFAILED
   
Comments:  
   None

Side Effects: 
   None

===========================================================================
Function: IGSMSMS_GetMessageStatus()

Description:

   This function reads the status of a message in the specified store.
   The status will indicate whether the specified slotis free or if the
   message stored there is mobile terminated or mobile originated.

   If the message is mobile terminated it will also indicate whether the
   message has been read or not.

   If the message is mobile originated, it will also indicate whether the
   message has been sent or not and if sent, whether there is a pending
   or received status report for the message.

Prototype:
   int IGSMSMS_GetMesageStatus(IGSMSMS * po, uint16 index, 
                               GSMSMSStatusType *pStatus, 
                               GSMSMSStorageType readFrom)

Parameters:
   po:       Pointer to the IGSMSMS object
   index:    The index in the store to read from
   pStatus:  Pointer to the status to read
   readFrom: Indicates whether to use the SIM or NVRAM database.

Return Value:
   If successful: AEE_GSMSMS_SUCCESS 
   If readFrom is invalid: AEE_GSMSMS_EBADPARAM 
   If the SIM Service Table has SMS disabled: AEE_GSMSMS_ESST
   If the entry could not be updated: AEE_GSMSMS_EFAILED
   
Comments:  
   None

Side Effects: 
   None

===========================================================================
Function: IGSMSMS_GetSCAddress()

Description:

   This function retrieves the default GSM Service Center address from the 
   SIM.

Prototype:
   int IGSMSMS_GetSCAddress(IGSMSMS * po, GSMSMSAddress *pAddress)

Parameters:
   po:       Pointer to the IGSMSMS object
   pAddress: Pointer to the structure to fill in with the default SC address

Return Value:
   If successful: AEE_GSMSMS_SUCCESS 
   If the SC address could not be retrieved: AEE_GSMSMS_EFAILED
   
Comments:  
   None

Side Effects: 
   None

===========================================================================
Function: IGSMSMS_SetSCAddress()

Description:

   This function updates the default SC address on the SIM.

Prototype:
   void IGSMSMS_SetSCAddress(IGSMSMS * po, const GSMSMSAddress *pAddress) 

Parameters:
   po:       Pointer to the IGSMSMS object
   pAddress: Pointer to the structure to containing the new SC address

Return Value:
   If successful: AEE_GSMSMS_SUCCESS 
   If the SC address could not be set: AEE_GSMSMS_EFAILED
   
Comments:  
   None

Side Effects: 
   None

===========================================================================
Function: IGSMSMS_GetMemoryCapExceededFlag()

Description:

   This function retrieves the default Memory Capacity Exceeded Flag on the 
   SIM. In accordance with the GSM TS 11.11 spec, pFlag is set to 0 if 
   the flag is set, and set to 1 if the flag is not set. All other values 
   are reserved.

Prototype:
   void IGSMSMS_GetMemoryCapExceededFlag(IGSMSMS * po, uint8 *pFlag) 

Parameters:
   po:    Pointer to the IGSMSMS object
   pFlag: Pointer to variable to set to the value of the flag

Return Value:
   If successful: AEE_GSMSMS_SUCCESS 
   If the flag could not be read: AEE_GSMSMS_EFAILED
   
Comments:  
   None

Side Effects: 
   None

===========================================================================
Function: IGSMSMS_SetMemoryCapExceededFlag()

Description:

   This function updates the Memory Capacity Exceeded Flag on the 
   SIM. In accordance with the GSM TS 11.11 spec, flag should be set to 0 
   if the MemCapExceded flag is set, and set to 1 if the it is not set. All 
   other values are reserved.

Prototype:
   void IGSMSMS_SetMemoryCapExceededFlag(IGSMSMS * po, uint8 flag) 

Parameters:
   po:    Pointer to the IGSMSMS object
   pFlag: New setting for the MemCapExceded flag

Return Value:
   If successful: AEE_GSMSMS_SUCCESS 
   If flag is invaliid: AEE_GSMSMS_EBADPARAM 
   If the flag could not be set: AEE_GSMSMS_EFAILED
   
Comments:  
   None

Side Effects: 
   None

===========================================================================
Function: IGSMSMS_GetTPMR()

Description:

   This function retrieves the last used TP-MR from EF-SMSS on the
   SIM. 

Prototype:
   int IGSMSMS_GetTPMR(IGSMSMS * po, uint8 *pTPMR) 

Parameters:
   po:    Pointer to the IGSMSMS object
   pTPMR: Pointer to variable to set to the value of the TP-MR

Return Value:
   If successful: AEE_GSMSMS_SUCCESS 
   If the flag could not be read: AEE_GSMSMS_EFAILED
   
Comments:  
   None

Side Effects: 
   None

===========================================================================
Function: IGSMSMS_SetTPMR()

Description:

   This function sets the last used TP-MR from EF-SMSS on the SIM. 

Prototype:
   int IGSMSMS_SetTPMR(IGSMSMS * po, uint8 TPMR) 

Parameters:
   po:   Pointer to the IGSMSMS object
   TPMR: New value for the TP-MR in EF-SMSS on the SIM

Return Value:
   If successful: AEE_GSMSMS_SUCCESS 
   If the flag could not be read: AEE_GSMSMS_EFAILED
   
Comments:  
   None

Side Effects: 
   None

===========================================================================
Function: IGSMSMS_DecodeMessage()

Description:

   This function is used decode a raw SMS message into an appropriate 
   structure representation.

   The UserData (UD) will not be decoded and can be subsequently:

        AECHAR wszText[GSMSMS_MAX_UD_CHAR];
        GSMSMSMsg DecodedMsg;

        // Decode Message
        IGSMSMS_DecodeMessage(pMe, pNotifyMsg->rawMsg, sizeof(pNotifyMsg->rawMsg), &DecodedMsg);

        // Decode UserData
        IGSMSMS_DecodeUserData(pMe, DecodedMsg, &wszText[0], sizeof(wszText));
         
Prototype:
   int IGSMSMS_DecodeMessage(IGSMSMS * po, const GSMSMSRawMsg *pRawMsg, const GSMSMSMsg *pMsg)

Parameters:
   po:      Pointer to the IGSMSMS object
   pRawMsg: Pointer to the raw GSM SMS message to decode
   pMsg:    Pointer to the GSMSMSMsg struct to fill with the decoded information

Return Value:
   If successful: AEE_GSMSMS_SUCCESS 
   If the store could not be decoded: AEE_GSMSMS_EFAILED
   If pRawMsg or pMsg are NULL: AEE_GSMSMS_EBADPARAM
   
Comments:  
   None

Side Effects: 
   None

===========================================================================
Function: IGSMSMS_DecodeUserData()

Description:

   This function is used decode the UserData into UNICODE for display.
   If the UserData contains a user data header it will be skipped. 

   ONLY THE RAW TEXT IS RETURNED by this method.

   If the message is a concatinated or EMS message, it must be
   decoded directly by an external library or by the applet.

   See the description of IGSMSMS_DecodeMessage for an example of how
   this is used.

   If wstrlen is less than the decoded data length, the decoded data will be
   truncated and NULL terminated.

Prototype:
   IGSMSMS_DecodeUserData(IGSMSMS * po, const GSMSMSMsg *pMsg, AECHAR *pwzStr, uint16 wstrlen)

Parameters:
   po:      Pointer to the IGSMSMS object
   pMsg:    Pointer to the decoded SMS message
   pwzStr:  Pointer to the unicode string to fill with the decoded data
   wstrlen: Size in bytes of the wstr unicode string

Return Value:
   If successful: AEE_GSMSMS_SUCCESS 
   If the data could not be decoded: AEE_GSMSMS_EFAILED
   If pMsg or pwzStr are NULL: AEE_GSMSMS_EBADPARAM
   
Comments:  
   None

Side Effects: 
   None

===========================================================================
Function: IGSMSMS_EncodeUserData()

Description:

   This function is used to decode the UserData into UNICODE for display.

   If a UserData Header is included then address of the offset user data
   should be passed to this function. The udlen parameter must also be
   reduced accordingly.

   If the message will not fit in the space provided it will be truncated.


Prototype:
   IGSMSMS_EncodeUserData(IGSMSMS * po, const AECHAR *pwszText, byte *pDest, 
                          byte destLen, GSMEncodingType encoding, byte *pEncodedLen);

Parameters:
   po:               Pointer to the IGSMSMS object
   pwszText[in]:     Pointer to the unicode string to encode
   pDest[out]:       Pointer to the part of the UD field to encode the data
   destLen:          Size in bytes of the UD field left for the encoded string 
   encoding:         Encoding to use for text
   pEncodedLen[out]: Length of the encoded text in bytes

Return Value:
  If successful: AEE_GSMSMS_SUCCESS 
  If pUD is NULL, pwszText is NULL or encoding is invalid: AEE_GSMSMS_EBADPARAM

   
Comments:  
   None

Side Effects: 
   None

=======================================================================
Function: IGSMSMS_GetStoreSize()

Description:

   This function returns the size of the message store in slots.

Prototype:
   int AEEGSMSMS_GetStoreSize(IGSMSMS * po, GSMSMSStorageType readFrom, 
                              uint16 *pCount)

Parameters:
   po:        Pointer to the IGSMSMS object
   readFrom:  Message store to query
   pCount:    Pointer to the variable to set to the number of slots in the store

Return Value:
  If successful: AEE_GSMSMS_SUCCESS 
  If the SIM Service Table has SMS disabled: AEE_GSMSMS_ESST
  Otherwise: AEE_GSMSMS_EBADPARAM
   
Comments:  
   None

Side Effects: 
   None

=======================================================================*/

#endif    // AEE_GSM1SMS_H


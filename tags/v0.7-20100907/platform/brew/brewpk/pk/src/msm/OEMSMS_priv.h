#if !defined(OEMSMS_priv_H)
#define OEMSMS_priv_H
/*======================================================
                    IMPORTANT NOTICE

                  OEM DISTRIBUTION ONLY

FILE:  OEMSMS_priv.h

SERVICES:  AEE OEM SMS Interface header

GENERAL DESCRIPTION:

This file contains all of the function prototypes for
routines that are required by the AEE to function with
SMS.

        Copyright © 1999-2008 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR    
=====================================================*/
#include "customer.h"

#if defined (FEATURE_WMS)
#include "wms.h"
#elif defined (FEATURE_UASMS)
#include "uasmsx.h"
#endif

#include "OEMSMS.h"
#include "OEMCriticalSection.h"
#include "AEETelephoneTypes.h"
#include "AEEComdef.h"

// The additional +3 and ~3 is added to ensure that data size is a multiple
// of 4
#if defined (FEATURE_UASMS)
#define SMSWORK_BUFF_SIZE      ((sizeof(uasms_client_bd_type) + 3) & ~3)
#define SMS_MAX_RAW_TPDU_SIZE 1
#define SMS_MAX_ALPHA_ID_SIZE 1
int Xlate_Encoding(uasms_user_data_encoding_e_type enc);
uasms_user_data_encoding_e_type Xlate_2_NativeEncoding( int enc );
#elif defined (FEATURE_WMS)
#define SMSWORK_BUFF_SIZE     ((sizeof(wms_client_ts_data_s_type) + 3) & ~3)
#define SMS_MAX_RAW_TPDU_SIZE WMS_MAX_LEN
#define SMS_MAX_ALPHA_ID_SIZE 256
#endif

static OEMCriticalSection gSMSCriticalSection;

typedef struct
{
   uint16                     wStatus;                          // Cache Status

   uint32                     dwTransactionId;                  // Transaction Id

   byte                       data[SMSWORK_BUFF_SIZE];          // Bearer Data

   byte *                     pText;                            // Pointer to text data
   int                        nText;                            // Text Data Length

   boolean                    bIsLoopbackMsg;                   // Message is Loopback
   uint32                     nFromAppletCls;                   // sender Class id for loopback msgs

   int                        ts;                               // Teleservice
   int                        nEncoding;                        // Encoding
   boolean                    bPriorityPresent;                 // Optional Param
   int                        nPriority;                        // Priority
   boolean                    bPrivacyPresent;                  // Optional Param
   int                        nPrivacy;                         // Privacy
   int                        nNumMsgs;                         // Message Count in voice mail message
   int                        nTimeStampZone;                   // Timestamp Zone
   int                        nAbsoluteValidityTimeStampZone;   // Timestamp Zone, used by Absolute validity
   int                        nDischargeTimeZone;               // Discharge Time Zone
   boolean                    bLangPresent;                     // Optional Param
   uint32                     dwLang;                           // Language
   uint32                     dwPID;                            // Protocol ID 
   uint32                     dwMsgClass;                       // Message Class
   uint32                     dwMsgWaitingType;                 // Message Waiting Type
   uint32                     dwMsgWaitingKind;                 // Message Waiting Kind
   uint32                     dwRawDCS;                         // Raw DCS
   uint32                     dwTimeStamp;                      // Time Stamp
   uint32                     dwAbsoluteValidityTimeStamp;      // Time Stamp, used by Absolute validity
   uint32                     dwSrvID;                          // Service ID for CDMA broadcast
   char                       szFrom[MAX_DEVICE_ADDRESS];       // From Address
   char                       szCallback[MAX_DEVICE_ADDRESS];   // Callback Number
   uint32                     dwMOFromMTToType;                 // MT - From Address Type, MO - To Address Type
   uint32                     dwMOFromMTToPlan;                 // MT - From Address Plan, MO - To Address Plan
   uint32                     dwCallbackType;                   // bIsSCAddress ? SC Address Type: Callback Number Type
   uint32                     dwCallbackPlan;                   // bIsSCAddress ? SC Address Plan: Callback Number Plan
   uint32                     dwTag;                            // Message Tag
   uint32                     dwMsgID;                          // Message ID
   uint32                     dwMsgDischargeTime;               // Message Discharge Time
   uint32                     dwReportStatus;                   // Report Status
   uint32                     dwRelativeDeferredValidity;       // CDMA relative deferred validity
   uint32                     dwAbsoluteDeferredValidity;       // CDMA absolute deferred validity
   uint32                     dwRelativeValidity;               // Relative Validity
   uint32                     dwGWCommand;                      // GSM/WCDMA command
   uint32                     dwMessageDisplayMode; 
   boolean                    bRequestUserAck;                  // User Ack is requested
   boolean                    bRequestDLAck;                    // Delivery Ack is requested
   boolean                    bIsMOSMS;                         // Is MO SMS
   boolean                    bIsSCAddress;                     // szCallback contains SC address
   boolean                    bRequestReadAck;                  // Read Ack is requested
   boolean                    bIsDLAck;                         // Message is DL Ack
   boolean                    bIsUserAck;                       // Message is User Ack
   boolean                    bIsReadAck;                       // Message is Read Ack
   boolean                    bIsReplyPathPresent;              // Message contains reply path
   boolean                    bIsStored;                        // Message is automatically stored
   boolean                    bIsCompressed;                    // TRUE - TP - DCS specifies that message is compressed
   boolean                    bHasMoreMessageToSend;            // TRUE - TP - MMS specifies more messages to send                         // TRUE - MO, FALSE - MT
   boolean                    bIsGWBroadcast;                   // Message is GSM Broadcast
   boolean                    bIsCancelRequest;                 // Message is cancellation request for a CDMA message 

   AEESMSStorageType          st;                               // Storage Type
   uint32                     dwIndex;                          // Storage Index
   int                        nRawTPDULength;                   // GSM / WCDMA Raw TPDU Length
   byte                       aRawTPDUData[SMS_MAX_RAW_TPDU_SIZE]; // GSM / WCDMA Raw TPDU Data
   int                        nAlphaIDLength;                   // GW Alpha ID Length
   byte                       aAlphaID[SMS_MAX_ALPHA_ID_SIZE];  // GW Aplha ID
} SMSCacheEntry;

/* Internal representation of SMS Message */
typedef struct _OEMMsg OEMMsg;

// Internal representation for SMS Broadcast Configuration
typedef struct _OEMBcCfg OEMBcCfg;

typedef enum
{
   OEMSMSBCConfig_GetBCConfigRequest = 1,
   OEMSMSBCConfig_GetBCPrefRequest,
   OEMSMSBCConfig_SetBCPrefRequest,
   OEMSMSBCConfig_GetNumberOfServiceIDsRequest,
   OEMSMSBCConfig_GetServiceOptsRequest,
   OEMSMSBCConfig_SetServiceOptsRequest,
   OEMSMSBCConfig_GetNumberOfServiceOptsRequest,
   OEMSMSBCConfig_DeleteServiceRequest,
   OEMSMSBCConfig_MaxRequest
} OEMSMSBCRequestType;

#if defined(__cplusplus)
extern "C" {
#endif

/* Implemented in OEMWSMS.c and OEMUASMS.c */
int OEMSMS_NewMessage( OEMMsgData * pMsgData, OEMMsgPayload * pMsgPayload, AEETSysMode sysMode, OEMMsg ** hMsg);

AEE_EXPORTS int         OEMSMS_Init( void );
AEE_EXPORTS void        OEMSMS_SendSMS( OEMMsg* hMsg, AEECallback *pcb, uint32 *pdwStatus );
AEE_EXPORTS void        OEMSMS_DeleteMessage( OEMMsg* hMsg );
AEE_EXPORTS int         OEMSMS_Cleanup( void );
AEE_EXPORTS int         OEMSMS_SetMsgAckStatus( uint32 cs, uint32 mt);
AEE_EXPORTS void        OEMSMS_GetUDHData(SMSCacheEntry * pce, int * pnBytes, byte * pBuffer);
AEE_EXPORTS void        OEMSMS_GetGWCBParms(SMSCacheEntry * pce, OEMMsgData * pOEMMsgData);

/* Implemented in OEMSMS.c */
AEE_EXPORTS SMSCacheEntry *OEMSMSCache_GetEntry( void );
AEE_EXPORTS void           OEMSMSCache_Release( SMSCacheEntry *pce );
AEE_EXPORTS void           OEMSMSCache_Ready( SMSCacheEntry *pce );
AEE_EXPORTS boolean        OEMSMS_IsInitialized(void);
AEE_EXPORTS void           OEMSMS_CopyPayloadFromCache(SMSCacheEntry *pce, OEMMsgPayload * pMsgPayload);
AEE_EXPORTS void           OEMSMS_CopyPayloadDataFromCache(SMSCacheEntry *pce, OEMMsgData * pOEMMsgData);

AEE_EXPORTS char           GetDigit(byte ch);
AEE_EXPORTS byte           GetDTMF (char ch); 
AEE_EXPORTS uint32         GetASCII2DTMF (char * pcBuf, byte * pBuf, uint32 dwLen);

/* Implemented in OEMSMSCust.c */
void                       OEMSMSCust_LoopbackMsg (SMSCacheEntry * pce);

// BCD to integer
#define BCD2INT(x)  (((((x) & 0xf0) >> 4) * 10) + ((x) & 0x0f))

// integer to BCD
#define INT2BCD(x)  (((((x) / 10) << 4) & 0xf0) | (((x) % 10) & 0x0f))

// YEAR of when the century start
#define YEAR_2000   2000
#define YEAR_1900   1900

// Following define need to come from OEMConfig file
#define OEM_CONFIG_ALLOW_INTERNATIONAL    TRUE
#define OEM_CONFIG_INTERNATIONAL_PREFIX   "011"

#define DIALER_PLUS_CHAR    ((char)'+')

#if defined(__cplusplus)
}
#endif

#endif /*OEMSMS_priv_H*/

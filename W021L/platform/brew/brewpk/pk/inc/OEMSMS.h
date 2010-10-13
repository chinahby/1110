#if !defined(OEMSMS_H)
#define OEMSMS_H
/*======================================================
                    IMPORTANT NOTICE

                  OEM DISTRIBUTION ONLY

FILE:  OEMSMS.h

SERVICES:  AEE OEM SMS Interface header

GENERAL DESCRIPTION:

This file contains all of the function prototypes for
routines that are required by the AEE to function with
SMS.

        Copyright © 2006-2008 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#include "AEE.h"
#include "AEESMS.h"

#define FARF_SMS              AEESMS_IsDebugMsgOn()

#ifdef FARF
#undef FARF
#endif // FARF

#define FARF(x, p)  if (1 == FARF_##x) DBGPRINTF p

// Maximum address denoted by number of chars including null termination.
#define MAX_DEVICE_ADDRESS             32
								
// Maximum Label Size for a Broadcast Service
#define OEMSMS_MAX_LABEL_SIZE          30

typedef struct _OEMSMSMgr OEMSMSMgr;

// This struct is used to pass message payload between AEE and OEM layer
// Filelds specific to mobile originated or mobile terminated messages are
// indicated by MO and MT respectively.
typedef struct
{
   int                                 nEncoding;                 // Encoding
   int                                 nBytes;                    // Number of bytes
   int                                 nPadding;                  // Padding bits (MO)
   byte                                data[1];                   // Data Buffer
} OEMMsgPayload;

// This struct is used to pass message information between AEE and OEM layer
// Filelds specific to mobile originated or mobile terminated messages are
// indicated by MO and MT respectively.
typedef struct
{
   int                        ts;                                 // Teleservice

   boolean                    bIsLoopbackMsg;                     // Message is Loopback
   uint32                     nFromAppletCls;                     // sender Class id for loopback msgs

   boolean                    bPriorityPresent;                   // Optional Param
   int                        nPriority;                          // Priority
   boolean                    bPrivacyPresent;                    // Optional Param
   int                        nPrivacy;                           // Privacy
   int                        nNumMsgs;                           // Message count in voice mail message (MT)
   int                        nTimeStampZone;                     // Timestamp Zone
   int                        nAbsoluteValidityTimeStampZone;     // Timestamp Zone, used for absolute Validity
   int                        nDischargeTimeZone;                 // Discharge Time Zone
   boolean                    bRequestDLAck;                      // Request Delivery Ack (MO)
   boolean                    bRequestUserAck;                    // Request User Ack
   boolean                    bRequestReadAck;                    // Request Read Ack
   boolean                    bIsDLAck;                           // Message is Delivery Ack
   boolean                    bIsUserAck;                         // Message is User Ack
   boolean                    bIsReadAck;                         // Message is Read Ack
   boolean                    bIsMOSMS;                           // TRUE - MO, FALSE - MT
   boolean                    bIsSCAddress;                       // TRUE - SC Address, FALSE -  Callback
   boolean                    bIsBREWDirectedSMS;                 // TRUE - Message is BREW directed
   boolean                    bIsGWBroadcast;                     // TRUE - Message is Broadcast
   boolean                    bIsReplyPathPresent;                // TRUE - Reply path present for GSM/WCDMA message
   boolean                    bIsStored;                          // TRUE - Message is automatically stored
   boolean                    bIsCompressed;                      // TRUE - TP - DCS specifies that message is compressed
   boolean                    bHasMoreMessageToSend;              // TRUE - TP - MMS specifies more messages to send
   boolean                    bIsCancelRequest;                   // Message is cancellation request for a CDMA message 
   AEESMSStorageType          st;                                 // Storage Type
   uint32                     dwIndex;                            // Storage Index
   boolean                    bLangPresent;                       // Optional Param
   uint32                     dwLang;                             // Language
   uint32                     dwPID;                              // Protocol ID
   uint32                     dwTimeStamp;                        // Time Stamp 
   uint32                     dwAbsoluteValidityTimeStamp;        // Time Stamp value for Absolute Validity
   uint32                     dwMsgClass;                         // Message Class
   uint32                     dwMsgWaitingType;                   // Message Waiting Type
   uint32                     dwMsgWaitingKind;                   // Message Waiting Kind
   uint32                     dwRawDCS;                           // Raw DCS
   char                       szMOFromMTTo[MAX_DEVICE_ADDRESS];   // MT - From Address, MO - To Address
   char                       szCallback[MAX_DEVICE_ADDRESS];     // bIsSCAddress ? SC Address : Callback Number
   uint32                     dwTag;                              // Message Tag
   uint32                     dwMsgID;                            // Message ID
   uint32                     dwMsgDischargeTime;                 // Message Discharge Time
   uint32                     dwReportStatus;                     // Report Status
   uint32                     dwMOFromMTToType;                   // MT - From Address Type, MO - To Address Type
   uint32                     dwMOFromMTToPlan;                   // MT - From Address Plan, MO - To Address Plan
   uint32                     dwCallbackType;                     // bIsSCAddress ? SC Address Type: Callback Number Type
   uint32                     dwCallbackPlan;                     // bIsSCAddress ? SC Address Plan: Callback Number Plan
   uint32                     dwSrvID;                            // CDMA/GSM CB Service ID
   uint32                     dwGWCBPageNumber;                   // GW CB Page Number    
   uint32                     dwGWCBTotalPages;                   // GW CB Total Pages
   uint32                     dwGWCBGeoScope;                     // GW CB Geographic scope
   uint32                     dwGWCBMessageCode;                  // GW CB Message Code
   uint32                     dwGWCBUpdateNumber;                 // GW CB Update Number
   uint32                     dwRelativeDeferredValidity;         // CDMA relative deferred validity
   uint32                     dwAbsoluteDeferredValidity;         // CDMA absolute deferred validity
   uint32                     dwMessageDisplayMode;               // Message Display Mode
   uint32                     dwRelativeValidity;                 // Relative Validity
   uint32                     dwGWCommand;                        // GSM/WCDMA command
   int                        nUDHLength;                         // User Data Header Bytes
   int                        nRawTPDULength;                     // GSM / WCDMA Raw TPDU Length
   int                        nAlphaIDLength;                     // GSM / WCDMA alpha ID Length
   byte                       data[1];                            // User Data Header followed by Raw TPDU followed by Alpha ID
} OEMMsgData;

// Structure to unpack ISMSBCSrvOpts
typedef struct
{
   AEESMSBCSrvID          sSrvID;                                 // Service ID
   boolean                bSelected;                              // Is service selected
   AEEMSGPriority         nPriority;                              // Service Priority Threshold
   char                   szLabel[OEMSMS_MAX_LABEL_SIZE];         // Service Label
   int                    nLabelEncoding;                         // Service Label Encoding
   AEESMSBCAlertType      nAlert;                                 // Service Alert Type
   byte                   nMaxMessages;                           // Limit on maximum messages which can be stored for the service
} OEMSMSBCSrvInfo;

typedef struct
{
   uint32 nEvent;
   byte aEventData[32];
} OEMSMSUpdateData;

#ifdef __cplusplus
extern "C" {
#endif

AEE_EXPORTS int OEMSMS_New( OEMSMSMgr **ppme );
AEE_EXPORTS uint32 OEMSMS_AddRef( OEMSMSMgr *pme );
AEE_EXPORTS uint32 OEMSMS_Release( OEMSMSMgr *pme );
AEE_EXPORTS int OEMSMS_GetNewMsgInfo( OEMSMSMgr *pme, uint32 * pdwTransactionId, int * pts, boolean * pIsLoopback );
AEE_EXPORTS int OEMSMS_GetMsgPayload( OEMSMSMgr *pme, uint32 dwTransactionId, OEMMsgPayload * pOEMMsgPayload );
AEE_EXPORTS int OEMSMS_GetMsgData( OEMSMSMgr *pme, uint32 dwTransactionId, OEMMsgData * pOEMMsgData);
AEE_EXPORTS void OEMSMS_OnMTMessage( OEMSMSMgr *pme, AEECallback *pcb );
AEE_EXPORTS void OEMSMS_SendMessage( OEMSMSMgr *pme, OEMMsgData * pMsgData, OEMMsgPayload * pMsgPayload, uint32 *pdwStatus, AEECallback *pcb );
AEE_EXPORTS int OEMSMS_SetClientStatus(OEMSMSMgr *pme, uint32 cs, uint32 mt);
AEE_EXPORTS void OEMSMS_FreeMsg(OEMSMSMgr *pme, uint32 dwTransactionId);
AEE_EXPORTS void OEMSMS_OnSMSUpdate(OEMSMSUpdateData * pEventData, AEECallback *pcb);
AEE_EXPORTS boolean OEMSMS_IgnorePrefixForBrewDirectedMsg(void);
AEE_EXPORTS boolean OEMSMS_OEMDefinedHandler(const char * pszText, char ** ppszHandler);

void OEMSMSBCConfig_GetBCConfig(AEESMSMode mode, AEECallback *pcb, AEESMSBCConfig * pConfig, uint32 * pnErr);
void OEMSMSBCConfig_GetBCPref(AEESMSMode mode, AEECallback *pcb, AEESMSBCPref * pPref, uint32 * pnErr);
void OEMSMSBCConfig_SetBCPref(AEESMSMode mode, AEESMSBCPref nPref, AEECallback *pcb, uint32 * pnErr);
void OEMSMSBCConfig_GetNumberOfServiceIDs(AEESMSMode mode, AEECallback *pcb, uint32 * pnCount, uint32 * pnErr);
void OEMSMSBCConfig_GetServiceOpts(AEESMSMode mode, AEESMSBCSrvID *pSrvID, AEECallback *pcb, OEMSMSBCSrvInfo * pSrvInfo, uint32 * pnErr);
void OEMSMSBCConfig_SetServiceOpts(AEESMSMode mode, OEMSMSBCSrvInfo * pSrvInfo, AEECallback *pcb, uint32 * pnErr);
void OEMSMSBCConfig_GetNumberOfServiceOpts(AEESMSMode mode, AEECallback *pcb, uint32 * pnCount, uint32 * pnErr);
void OEMSMSBCConfig_DeleteService(AEESMSMode mode, AEESMSBCSrvID *pSrvID, AEECallback *pcb, uint32 * pnErr);
void OEMSMSBCConfig_FillServiceIDs(AEESMSBCSrvID *psid, uint32 dwBytes);
void OEMSMSBCConfig_FillServiceInfo(OEMSMSBCSrvInfo *pSrvInfo, uint32 dwBytes);
void OEMSMSBCConfig_OnSMSBCConfigUpdate(OEMSMSUpdateData * pEventData, AEECallback *pcb);

// Following APIs are not exposed to AEE. These are used within OEM layer
int OEMSMS_SetDomainPref(int gwDomainPref);
int OEMSMS_GetDomainPref(int * pgwDomainPref);
#ifdef __cplusplus
}
#endif
/*========================================================================
   DATA STRUCTURE DOCUMENTATION
==========================================================================

OEMMsgPayload

Description:
   This structure is used to pass message payload between AEE and OEM layer.
   Filelds specific to mobile originated or mobile terminated messages are
   indicated by MO and MT respectively.

Definition:

   typedef struct
   {
      int                                 nEncoding;                 // Encoding
      int                                 nBytes;                    // Number of bytes
      int                                 nPadding;                  // Padding bits (MO)
      byte                                data[1];                   // Data Buffer
   } OEMMsgPayload;


Comments:
   None.

See Also:
   None.

==========================================================================

OEMMsgData

Description:
   This structure is used to pass message information between AEE and OEM layer.
   Filelds specific to mobile originated or mobile terminated messages are
   indicated by MO and MT respectively.

Definition:

typedef struct
{
   int                        ts;                                 // Teleservice

   boolean                    bIsLoopbackMsg;                     // Message is Loopback
   uint32                     nFromAppletCls;                     // sender Class id for loopback msgs

   boolean                    bPriorityPresent;                   // Optional Param
   int                        nPriority;                          // Priority
   boolean                    bPrivacyPresent;                    // Optional Param
   int                        nPrivacy;                           // Privacy
   int                        nNumMsgs;                           // Message count in voice mail message (MT)
   int                        nTimeStampZone;                     // Timestamp Zone
   int                        nAbsoluteValidityTimeStampZone;     // Timestamp Zone, used for absolute Validity
   int                        nDischargeTimeZone;                 // Discharge Time Zone
   boolean                    bRequestDLAck;                      // Request Delivery Ack (MO)
   boolean                    bRequestUserAck;                    // Request User Ack
   boolean                    bRequestReadAck;                    // Request Read Ack
   boolean                    bIsDLAck;                           // Message is Delivery Ack
   boolean                    bIsUserAck;                         // Message is User Ack
   boolean                    bIsReadAck;                         // Message is Read Ack
   boolean                    bIsMOSMS;                           // TRUE - MO, FALSE - MT
   boolean                    bIsSCAddress;                       // TRUE - SC Address, FALSE -  Callback
   boolean                    bIsBREWDirectedSMS;                 // TRUE - Message is BREW directed
   boolean                    bIsGWBroadcast;                     // TRUE - Message is Broadcast
   boolean                    bIsReplyPathPresent;                // TRUE - Reply path present for GSM/WCDMA message
   boolean                    bIsStored;                          // TRUE - Message is automatically stored
   boolean                    bIsCompressed;                      // TRUE - TP - DCS specifies that message is compressed
   boolean                    bHasMoreMessageToSend;              // TRUE - TP - MMS specifies more messages to send
   boolean                    bIsCancelRequest;                   // Message is cancellation request for a CDMA message 
   AEESMSStorageType          st;                                 // Storage Type
   uint32                     dwIndex;                            // Storage Index
   boolean                    bLangPresent;                       // Optional Param
   uint32                     dwLang;                             // Language
   uint32                     dwPID;                              // Protocol ID
   uint32                     dwTimeStamp;                        // Time Stamp 
   uint32                     dwAbsoluteValidityTimeStamp;        // Time Stamp value for Absolute Validity
   uint32                     dwMsgClass;                         // Message Class
   uint32                     dwMsgWaitingType;                   // Message Waiting Type
   uint32                     dwMsgWaitingKind;                   // Message Waiting Kind
   uint32                     dwRawDCS;                           // Raw DCS
   char                       szMOFromMTTo[MAX_DEVICE_ADDRESS];   // MT - From Address, MO - To Address
   char                       szCallback[MAX_DEVICE_ADDRESS];     // bIsSCAddress ? SC Address : Callback Number
   uint32                     dwTag;                              // Message Tag
   uint32                     dwMsgID;                            // Message ID
   uint32                     dwMsgDischargeTime;                 // Message Discharge Time
   uint32                     dwReportStatus;                     // Report Status
   uint32                     dwMOFromMTToType;                   // MT - From Address Type, MO - To Address Type
   uint32                     dwMOFromMTToPlan;                   // MT - From Address Plan, MO - To Address Plan
   uint32                     dwCallbackType;                     // bIsSCAddress ? SC Address Type: Callback Number Type
   uint32                     dwCallbackPlan;                     // bIsSCAddress ? SC Address Plan: Callback Number Plan
   uint32                     dwSrvID;                            // CDMA/GSM CB Service ID
   uint32                     dwGWCBPageNumber;                   // GW CB Page Number    
   uint32                     dwGWCBTotalPages;                   // GW CB Total Pages
   uint32                     dwGWCBGeoScope;                     // GW CB Geographic scope
   uint32                     dwGWCBMessageCode;                  // GW CB Message Code
   uint32                     dwGWCBUpdateNumber;                 // GW CB Update Number
   uint32                     dwRelativeDeferredValidity;         // CDMA relative deferred validity
   uint32                     dwAbsoluteDeferredValidity;         // CDMA absolute deferred validity
   uint32                     dwMessageDisplayMode;               // Message Display Mode
   uint32                     dwRelativeValidity;                 // Relative Validity
   uint32                     dwGWCommand;                        // GSM/WCDMA command
   int                        nUDHLength;                         // User Data Header Bytes
   int                        nRawTPDULength;                     // GSM / WCDMA Raw TPDU Length
   int                        nAlphaIDLength;                     // GSM / WCDMA alpha ID Length
   byte                       data[1];                            // User Data Header followed by Raw TPDU followed by Alpha ID
} OEMMsgData;

Comments:
   None.

See Also:
   None.

==========================================================================

OEMSMSBCSrvInfo

Description:
   This structure is used to unpack ISMSBCSrvOpts and represents information
associated with a service.

Definition:

   typedef struct
   {
      AEESMSBCSrvID          sSrvID;                                 // Service ID
      boolean                bSelected;                              // Is service selected
      AEEMSGPriority         nPriority;                              // Service Priority Threshold
      char                   szLabel[OEMSMS_MAX_LABEL_SIZE];         // Service Label
      int                    nLabelEncoding;                         // Service Label Encoding
      AEESMSBCAlertType      nAlert;                                 // Service Alert Type
      byte                   nMaxMessages;                           // Limit on maximum messages which can be stored for the service
   } OEMSMSBCSrvInfo;

Comments:
   None.

See Also:
   None.

==========================================================================

OEMSMSUpdateData

Description:
   This structure is used to pass model event and corresponding data by
OEM layer to AEE layer when calling OEMSMS_OnSMSUpdate() or 
OEMSMSBCConfig_OnSMSBCConfigUpdate.

Definition:

   typedef struct
   {
      uint32 nEvent;             // EVT_MDL_SMS_XXX or EVT_MDL_SMSBCCONFIG_XXX event
      byte aEventData[32];       // Event Specific Data
   } OEMSMSUpdateData;

Comments:
   1. EVT_MDL_SMS_CELL_CHANGE event doesn't have any event specific data.

   2. Following is how data is specified for EVT_MDL_SMS_DUPLICATE_CB_PAGE.
===pre>
      OEMSMSUpdateData * pUpdateData = NULL;
      uint32 * pData = NULL;
      
      pUpdateData->nEvent = EVT_MDL_SMS_DUPLICATE_CB_PAGE;
      pData = (uint32*)pUpdateData->aEventData;

      // dwSrvID
      *pData = info_ptr->cb_page_header.cb_srv_id;
      pData++;

      // dwGWCBPageNumber
      *pData = info_ptr->cb_page_header.page_number;
      pData++;

      // dwGWCBTotalPages
      *pData = info_ptr->cb_page_header.total_pages;
      pData++;

      // dwGWCBGeoScope
      *pData = info_ptr->cb_page_header.geo_scope;
      pData++;

      // dwGWCBMessageCode
      *pData  = info_ptr->cb_page_header.message_code;
      pData++;

      // dwGWCBUpdateNumber
      *pData = info_ptr->cb_page_header.update_number;
      pData++;

      // dwLang
      *pData = Xlate_GWCBLang(info_ptr->cb_page_header.cb_dcs.language);
      pData++;

      // nEncoding
      switch(info_ptr->cb_page_header.cb_dcs.alphabet)
      {
         case WMS_GW_ALPHABET_UCS2:
            *pData = Xlate_Encoding(WMS_ENCODING_UNICODE);
            break;

         case WMS_GW_ALPHABET_8_BIT:
            *pData = Xlate_Encoding(WMS_ENCODING_OCTET);
            break;

         default:
            *pData = Xlate_Encoding(WMS_ENCODING_GSM_7_BIT_DEFAULT);
      }
===/pre>
   3. Following is how data is specified for 
 ~
EVT_MDL_SMSBCCONFIG_CONFIG, EVT_MDL_SMSBCCONFIG_PREF,    ~
EVT_MDL_SMSBCCONFIG_ADDED,    ~
EVT_MDL_SMSBCCONFIG_SELECT_ALL_SRVS,    ~
EVT_MDL_SMSBCCONFIG_SET_PRIORITY_ALL_SRVS   ~
EVT_MDL_SMSBCCONFIG_DELETED    ~
 EVT_MDL_SMSBCCONFIG_DELETE_ALL_SRVS  
 *
===pre>
         uint32 * pData = NULL;
         OEMSMSUpdateData * pUpdateData = (OEMSMSUpdateData *)gpWms->pcbOnSMSBCConfigUpdate->pReserved;
         pUpdateData->nEvent = EVT_MDL_SMSBCCONFIG_XXX;
         pData = (uint32*)pUpdateData->aEventData;
         
         // Message Mode
         *pData = info_ptr->message_mode;
         pData++;

   4. Following is how data is specified for EVT_MDL_SMSBCCONFIG_UPDATED:

         uint32 * pData = NULL;
         AEESMSBCSrvID *pSrvID = NULL;
         OEMSMSUpdateData * pUpdateData = (OEMSMSUpdateData *)gpWms->pcbOnSMSBCConfigUpdate->pReserved;
         pUpdateData->nEvent = EVT_MDL_SMSBCCONFIG_UPDATED;
         pData = (uint32*)pUpdateData->aEventData;
            
         // Message Mode
         *pData = WMS_MESSAGE_MODE_CDMA;
         pData++;

         // Service ID
         pSrvID = (AEESMSBCSrvID *)pData;
         ConvertWMSBCSrvIDToAEESMSBCSrvID(&info_ptr->bc_srv_info.srv_id, WMS_MESSAGE_MODE_CDMA, pSrvID);

   5. Following is how data is specified for EVT_MDL_SMS_CS_CHANGE
         
         OEMSMSUpdateData * pUpdateData = NULL;
         uint32 * pData = NULL;
         pUpdateData = (OEMSMSUpdateData *)gpWms->pcbOnSMSUpdate->pReserved;
         pUpdateData->nEvent = EVT_MDL_SMS_CS_CHANGE;
         pData = (uint32*)pUpdateData->aEventData;

         // Send message type, old & new client status info
         *pData = mt;
         pData++;

         *pData = oldCS;
         pData++;

         *pData = cs;

===/pre>
See Also:
   OEMSMS_OnSMSUpdate(), OEMSMSBCConfig_OnSMSBCConfigUpdate()

==========================================================================
  INTERFACES DOCUMENTATION
==========================================================================
Interface Name: OEMSMS
==========================================================================

Function: OEMSMS_New()

Description:
   Returns pointer to global OEMSMSMgr object.

Prototype:
   int OEMSMS_New( OEMSMSMgr **ppme )

Parameters:
   ppme: pointer to place holder for pointer to the global OEMSMSMgr object

Return Value:
   SUCCESS on SUCCESS
   If there is an error, this function should return an error
    from AEEError.h

Comments:
   None.

Side Effects:
   None.

See Also:
   None

==========================================================================

Function: OEMSMS_AddRef()

Description:
   Increments reference count of the object.

Prototype:
   uint32 OEMSMS_AddRef( OEMSMSMgr *pme )

Parameters:
   pme: pointer to global OEMSMSMgr object

Return Value:
   Incremented reference count

Comments:
   None.

Side Effects:
   None.

See Also:
   OEMSMS_Release()

==========================================================================

Function: OEMSMS_Release()

Description:
   Decrements the reference count of the object. Also frees it up when the
reference count reaches 0.

Prototype:
   uint32 OEMSMS_Release( OEMSMSMgr *pme )

Parameters:
   pme: pointer to global OEMSMSMgr object

Return Value:
   Decremented reference count

Comments:
   None.

Side Effects:
   None.

See Also:
   OEMSMS_AddRef()

==========================================================================

Function: OEMSMS_GetNewMsgInfo()

Description:
   Scans SMS cache and provides transaction ID and teleservice ID of the new 
   messages.

Prototype:
   int OEMSMS_GetNewMsgInfo( OEMSMSMgr *pme, uint32 * pdwTransactionId, int * pts )

Parameters:
   pme: pointer to global OEMSMSMgr object
   pdwTransactionId: place holder for transaction ID
   pts: place holder for teleservice ID

Return Value:
   SUCCESS on SUCCESS
   If there is an error, this function should return an error
    from AEEError.h

Comments:
   On being notified of mobile terminated message, AEESMS calls this to 
   find transaction ID and teleservice ID of the new messages.

Side Effects:
   None.

See Also:
   None.

==========================================================================

Function: OEMSMS_GetMsgPayload()

Description:
   Fills pOEMMsgPayload with payload data corresponding to the specified 
   transaction ID.

Prototype:
   int OEMSMS_GetMsgPayload( OEMSMSMgr *pme, uint32 dwTransactionId, OEMMsgPayload * pOEMMsgPayload )

Parameters:
   pme: pointer to global OEMSMSMgr object
   dwTransactionId: transaction ID
   pOEMMsgPayload: pointer to payload data

Return Value:
   SUCCESS on SUCCESS
   If there is an error, this function should return an error
    from AEEError.h

Comments:
   None.

Side Effects:
   None.

See Also:
   OEMMsgPayload

==========================================================================

Function: OEMSMS_GetMsgData()

Description:
   Fills pOEMMsgData with message data corresponding to the specified 
   transaction ID.

Prototype:
   int OEMSMS_GetMsgData( OEMSMSMgr *pme, uint32 dwTransactionId, OEMMsgData * pOEMMsgData)

Parameters:
   pme: pointer to global OEMSMSMgr object
   dwTransactionId: transaction ID
   pOEMMsgData: pointer to message data

Return Value:
   SUCCESS on SUCCESS
   If there is an error, this function should return an error
    from AEEError.h

Comments:
   None.

Side Effects:
   None.

See Also:
   OEMMsgData

==========================================================================

Function: OEMSMS_OnMTMessage()

Description:
   Registers a callback to be invoked to notify AEE layer about arrival of 
   mobile terminated messages.

Prototype:
   void OEMSMS_OnMTMessage( OEMSMSMgr *pme, AEECallback *pcb )

Parameters:
   pme: pointer to global OEMSMSMgr object
   pcb: pointer to callback

Return Value:
   None.

Comments:
   None.

Side Effects:
   None.

See Also:
   None.

==========================================================================

Function: OEMSMS_SendMessage()

Description:
   Sends the message specified by pMsgData and pMsgPayload. Callback is invoked 
   at the end of the operation and message status is specified in pdwStatus.

Prototype:
   void OEMSMS_SendMessage( OEMSMSMgr *pme, OEMMsgData * pMsgData, OEMMsgPayload * pMsgPayload, uint32 *pdwStatus, AEECallback *pcb )

Parameters:
   pme: pointer to global OEMSMSMgr object
   pMsgData: pointer to message data
   pMsgPayload: pointer to payload data
   pdwStatus: place holder for message status
   pcb: pointer to callback

Return Value:
   None.

Comments:
   None.

Side Effects:
   None.

See Also:
   OEMMsgData, OEMMsgPayload

==========================================================================

Function: OEMSMS_SetClientStatus()

Description:
   Enables a client to control how mobile terminated message of specified type 
   should be acked.

Prototype:
   int OEMSMS_SetClientStatus(OEMSMSMgr *pme, uint32 cs, uint32 mt)

Parameters:
   pme: pointer to global OEMSMSMgr object
   cs: client status
   mt: message type

Return Value:
   SUCCESS - If successful
   Error code - Otherwise

Comments:
   None.

Side Effects:
   None.

See Also:
   None.

==========================================================================

Function: OEMSMS_FreeMsg()

Description:
   Frees message corresponding to the specified transaction ID from the cache 
   of incoming messages.

Prototype:
   void OEMSMS_FreeMsg(OEMSMSMgr *pme, uint32 dwTransactionId)

Parameters:
   pme: pointer to global OEMSMSMgr object
   dwTransactionId: transaction ID of the message to be freed

Return Value:
   None.

Comments:
   None.

Side Effects:
   None.

See Also:
   None.

==========================================================================

Function: OEMSMS_OnSMSUpdate()

Description:
   Called to notify of EVT_MDL_SMS_XXX events originating from lower layers.

Prototype:
   OEMSMS_OnSMSUpdate(OEMSMSUpdateData * pEventData, AEECallback *pcb)

Parameters:
   pEventData: place holder for event data
   pcb: place holder for callback

Return Value:
   None.

Comments:
   None.

Side Effects:
   None.

See Also:
   OEMSMSUpdateData

==========================================================================

Function: OEMSMSBCConfig_GetBCConfig()

Description:
   Gets broadcast configuration corresponding to the specified mode. Callback is 
invoked at the end of the operation and return value and broadcast configuration 
are provided in pnErr and pConfig respectively.

Prototype:
   void OEMSMSBCConfig_GetBCConfig(AEESMSMode mode, AEECallback *pcb, AEESMSBCConfig * pConfig, uint32 * pnErr)

Parameters:
   mode: mode
   pcb: pointer to callback
   pConfig: place holder for configuration
   pnErr: place holder for return value

Return Value:
   None.

Comments:
   None.

Side Effects:
   None.

See Also:
   None.

==========================================================================

Function: OEMSMSBCConfig_GetBCPref()

Description:
   Gets broadcast preference corresponding to the specified mode. Callback is 
invoked at the end of the operation and return value and broadcast preference 
are provided in pnErr and pPref respectively.

Prototype:
   void OEMSMSBCConfig_GetBCPref(AEESMSMode mode, AEECallback *pcb, AEESMSBCPref * pPref, uint32 * pnErr)

Parameters:
   mode: mode
   pcb: pointer to callback
   pPref: place holder for preference
   pnErr: place holder for return value

Return Value:
   None.

Comments:
   None.

Side Effects:
   None.

See Also:
   None.

==========================================================================

Function: OEMSMSBCConfig_SetBCPref()

Description:
   Sets broadcast preference corresponding to the specified mode. Callback is 
invoked at the end of the operation and return value is provided in pnErr.

Prototype:
   void OEMSMSBCConfig_SetBCPref(AEESMSMode mode, AEESMSBCPref nPref, AEECallback *pcb, uint32 * pnErr)

Parameters:
   mode: mode
   nPref: preference
   pcb: pointer to callback
   pnErr: place holder for return value

Return Value:
   None.

Comments:
   None.

Side Effects:
   None.

See Also:
   None.

==========================================================================

Function: OEMSMSBCConfig_GetNumberOfServiceIDs()

Description:
   Gets number of currently configured service identifiers corresponding to 
the specified mode. This is called to determine size of buffer to pass to 
OEMSMSBCConfig_FillServiceIDs().Callback is invoked at the end of the operation 
and return value and service ID count are provided in pnErr and pnCount respectively.

Prototype:
   void OEMSMSBCConfig_GetNumberOfServiceIDs(AEESMSMode mode, AEECallback *pcb, uint32 * pnCount, uint32 * pnErr)

Parameters:
   mode: mode
   pcb: pointer to callback
   pnCount: place holder for count of service IDs
   pnErr: place holder for return value

Return Value:
   None.

Comments:
   None.

Side Effects:
   None.

See Also:
   OEMSMSBCConfig_FillServiceIDs().

==========================================================================

Function: OEMSMSBCConfig_GetServiceOpts()

Description:
   Gets detailed information corresponding to the specified service ID and 
mode. Callback is invoked at the end of the operation and return 
value and service information are provided in pnErr and pSrvInfo respectively.

Prototype:
   void OEMSMSBCConfig_GetServiceOpts(AEESMSMode mode, AEESMSBCSrvID *pSrvID, AEECallback *pcb, OEMSMSBCSrvInfo * pSrvInfo, uint32 * pnErr)

Parameters:
   mode: mode
   pSrvID: pointer to service ID
   pcb: pointer to callback
   pSrvInfo: place holder for service information
   pnErr: place holder for return value

Return Value:
   None.

Comments:
   None.

Side Effects:
   None.

See Also:
   None.

==========================================================================

Function: OEMSMSBCConfig_SetServiceOpts()

Description:
   Updates information corresponding to pSrvInfo->sSrvID service ID of the
specified mode. If service is not already there in service configuration table,
its added there. Callback is invoked at the end of the operation and return 
value is provided in pnErr.

Prototype:
   void OEMSMSBCConfig_SetServiceOpts(AEESMSMode mode, OEMSMSBCSrvInfo * pSrvInfo, AEECallback *pcb, uint32 * pnErr);

Parameters:
   mode: mode
   pSrvInfo: pointer to service information
   pcb: pointer to callback
   pnErr: place holder for return value

Return Value:
   None.

Comments:
   None.

Side Effects:
   None.

See Also:
   None.

==========================================================================

Function: OEMSMSBCConfig_GetNumberOfServiceOpts()

Description:
   Gets number of currently configured services. This is called to determine
size of buffer to pass to OEMSMSBCConfig_FillServiceInfo(). Callback is invoked 
at the end of the operation and return value and services count are provided in 
pnErr and pnCount respectively.

Prototype:
   void OEMSMSBCConfig_GetNumberOfServiceOpts(AEESMSMode mode, AEECallback *pcb, uint32 * pnCount, uint32 * pnErr)

Parameters:
   mode: mode
   pcb: pointer to callback
   pnCount: place holder for count of service IDs
   pnErr: place holder for return value

Return Value:
   None.

Comments:
   None.

Side Effects:
   None.

See Also:
   OEMSMSBCConfig_FillServiceInfo().

==========================================================================

Function: OEMSMSBCConfig_DeleteService()

Description:
   Deletes the specified service from the configuration table. Callback is 
invoked at the end of the operation and return value is provided in pnErr.

Prototype:
   void OEMSMSBCConfig_DeleteService(AEESMSMode mode, AEESMSBCSrvID *pSrvID, AEECallback *pcb, uint32 * pnErr);

Parameters:
   mode: mode
   pSrvID: pointer to service ID
   pcb: pointer to callback
   pnErr: place holder for return value

Return Value:
   None.

Comments:
   None.

Side Effects:
   None.

See Also:
   None.

==========================================================================

Function: OEMSMSBCConfig_FillServiceIDs()

Description:
   Fills psid buffer with the configured service IDs.

Prototype:
   void OEMSMSBCConfig_FillServiceIDs(AEESMSBCSrvID *psid, uint32 dwBytes)

Parameters:
   psid: pointer to buffer to be filled with service IDs
   dwBytes: size of buffer specified by psid in number of bytes

Return Value:
   None.

Comments:
   None.

Side Effects:
   None.

See Also:
   OEMSMSBCConfig_GetNumberOfServiceIDs()

==========================================================================

Function: OEMSMSBCConfig_FillServiceInfo()

Description:
   Fills pSrvInfo buffer with the configured services information.

Prototype:
   void OEMSMSBCConfig_FillServiceInfo(OEMSMSBCSrvInfo *pSrvInfo, uint32 dwBytes)

Parameters:
   pSrvInfo: pointer to buffer to be filled with services information
   dwBytes: size of buffer specified by pSrvInfo in number of bytes

Return Value:
   None.

Comments:
   None.

Side Effects:
   None.

See Also:
   OEMSMSBCConfig_GetNumberOfServiceOpts(), OEMSMSBCSrvInfo

==========================================================================

Function: OEMSMSBCConfig_OnSMSBCConfigUpdate()

Description:
   Called to notify of EVT_MDL_SMSBCCONFIG_XXX events originating from lower layers.

Prototype:
   OEMSMSBCConfig_OnSMSBCConfigUpdate(OEMSMSUpdateData * pEventData, AEECallback *pcb)

Parameters:
   pEventData: place holder for event data
   pcb: place holder for callback

Return Value:
   None.

Comments:
   None.

Side Effects:
   None.

See Also:
   OEMSMSUpdateData

=========================================================================*/
#endif /*OEMSMS_H*/


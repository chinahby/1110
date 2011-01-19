/*===========================================================================
                              OEMUASMS.c

  OEM REFERENCE SMS SUPPORT ROUTINES
  
  This file contains a reference implementation of SMS support.
  This file interfaces with UASMS services of DMSS.
    
        Copyright © 1999-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================*/
#include "OEMFeatures.h"

#ifdef FEATURE_UASMS

#include <stdlib.h>

#include "clk.h"

#include "OEMHeap.h"
#include "OEMSMS_priv.h"
#include "OEMTAPI.h"
#include "OEMConfig.h"
#include "msm/OEMPhone.h"
#include "AEEComdef.h"
#include "AEEShell.h"
#include "AEEStdLib.h"
#include "AEE_OEMDispatch.h"
#include "AEE_OEM.h"
#include "AEESMS.h"
#include "AEE_OEMSMS.h"


//The auto-disconnect timeout period for the DTC (dedicated traffic channel)
//in seconds
#define SO6_TIMEOUT_LENGTH     5

// Following encoding defines must come from UASMS. Conditionally define these
// as old uasms.h might not defined these.

#if !defined(UASMS_ENCODING_UNICODE)
#define UASMS_ENCODING_UNICODE            4
#endif

#if !defined(UASMS_ENCODING_SHIFT_JIS)
#define UASMS_ENCODING_SHIFT_JIS          5
#endif

#if !defined(UASMS_ENCODING_KOREAN)
#define UASMS_ENCODING_KOREAN             6
#endif

#if !defined(UASMS_ENCODING_LATIN_HEBREW)
#define UASMS_ENCODING_LATIN_HEBREW       7
#endif

#if !defined(UASMS_ENCODING_LATIN)
#define UASMS_ENCODING_LATIN              8
#endif

#if !defined(UASMS_ENCODING_GSM_7_BIT_DEFAULT)
#define UASMS_ENCODING_GSM_7_BIT_DEFAULT  9
#endif

#if !defined(UASMS_NUMBER_DATA_EMAIL)
#define UASMS_NUMBER_DATA_EMAIL           10
#endif

struct _OEMMsg
{
   uasms_client_message_type  CMsg;
   AEECallback                *pcb;
   uint32                     *pdwStatus;
   OEMMsg                     *pPrev;
   OEMMsg                     *pNext;
};

struct _OEMBcCfg
{
   boolean              bBroadcastInUse;
   OEMSMSBCRequestType  eCurRequest;
   AEESMSMode           dwMode;
   uint32               nNumCommands;
   AEECallback         *pcb;
   uint32              *pnErr;
   AEESMSBCConfig      *pConfig;
   AEESMSBCPref        *pPref;
   uint32              *pnCount;
   AEESMSBCSrvID       *psid;        // To Store the Service IDs got from UASMS
   OEMSMSBCSrvInfo     *pSrvInfo;    // To Store the Service Info got from UASMS
   boolean              bSrvInfoAlloc;
};

typedef struct _OEMUasms 
{
   OEMMsg               *pCurMOSMS;
   OEMBcCfg             sBcCfg;
   boolean              bTransmissionPending;
   boolean              bDedicatedChannel;
   boolean              bMOOnAccessChannel;     // Send MO SMS on access channel
   boolean              bMOOnTrafficChannel;    // Send MO SMS on traffic channel
   uint32               dwTextStatus;           // Client Status for text messages
   uint32               dwVMStatus;             // Client Status for voice mail messages
   uint32               dwPageStatus;           // Client Status for page messages
   uint32               dwBREWDirectedTS;       // Teleservice ID for BREW directed messages
   uint32               nCount;                 // Number of service IDs
   AEESMSBCSrvID        *psid;                  // To Store the Service IDs got from UASMS
   OEMSMSBCSrvInfo      *pSrvInfo;              // To Store the Service Info got from UASMS
} OEMUasms;

OEMUasms * gpUasms = 0;

//Language Mapping between UASMS and BREW
typedef struct
{
   uasms_language_e_type            OEM_Lang;
   uint32                           AEE_Lang;
} LangMap;

static const LangMap      gsLangMap[]  = {{UASMS_LANGUAGE_UNSPECIFIED,     0},
                                         {UASMS_LANGUAGE_ENGLISH,         LNG_ENGLISH},
                                         {UASMS_LANGUAGE_FRENCH,          LNG_FRENCH},
                                         {UASMS_LANGUAGE_SPANISH,         LNG_SPANISH},
                                         {UASMS_LANGUAGE_JAPANESE,        LNG_JAPANESE},
                                         {UASMS_LANGUAGE_KOREAN,          LNG_KOREAN},
                                         {UASMS_LANGUAGE_CHINESE,         LNG_CHINESE},
                                         {UASMS_LANGUAGE_HEBREW,          LNG_HEBREW}};


// Teleservice Mapping Between UASMS and OEM layer SMS teleservice defines
typedef struct
{
   int                        AEETAPI_TsId;
   uasms_teleservice_e_type   UASMS_TsId;
} TsIdMap;

static const TsIdMap   gsTsIdMap[] = 
                  {{SMS_TELESERVICE_CPT_95,              UASMS_TELESERVICE_CPT_95 },
                  {SMS_TELESERVICE_CMT_95,               UASMS_TELESERVICE_CMT_95 },
                  {SMS_TELESERVICE_VMN_95,               UASMS_TELESERVICE_VMN_95 },
                  {SMS_TELESERVICE_IS91_PAGE,            UASMS_TELESERVICE_IS91_PAGE },
                  {SMS_TELESERVICE_IS91_VOICE_MAIL,      UASMS_TELESERVICE_IS91_VOICE_MAIL},
                  {SMS_TELESERVICE_IS91_SHORT_MESSAGE,   UASMS_TELESERVICE_IS91_SHORT_MESSAGE},
                  {SMS_TELESERVICE_MWI,                  UASMS_TELESERVICE_MWI}};

//Priority Mapping between DMSS and BREW
typedef struct
{
   int                     AEE_Priority;
   uasms_priority_e_type   OEM_Priority;
} PriorityMap;

static const PriorityMap   gsPriorityMap[] = 
                  {{SMS_PRIORITY_NORMAL,      UASMS_PRIORITY_NORMAL},
                  {SMS_PRIORITY_INTERACTIVE,  UASMS_PRIORITY_INTERACTIVE},
                  {SMS_PRIORITY_URGENT,       UASMS_PRIORITY_URGENT},
                  {SMS_PRIORITY_EMERGENCY,    UASMS_PRIORITY_EMERGENCY }};

//Privacy Mapping between DMSS and BREW
typedef struct
{
   int                     AEE_Privacy;
   uasms_privacy_e_type    OEM_Privacy;
} PrivacyMap;


static const PrivacyMap   gsPrivacyMap[] = 
                  {{SMS_PRIVACY_NORMAL,         UASMS_PRIVACY_NORMAL},
                  {SMS_PRIVACY_RESTRICTED,      UASMS_PRIVACY_RESTRICTED},
                  {SMS_PRIVACY_CONFIDENTIAL,    UASMS_PRIVACY_CONFIDENTIAL},
                  {SMS_PRIVACY_SECRET,          UASMS_PRIVACY_SECRET}};

//Encoding Mapping between DMSS and OEM layer encoding
typedef struct
{
   int                                AEE_Enc;
   uasms_user_data_encoding_e_type    OEM_Enc;
} EncMap;

static const EncMap   gsEncMap[] = 
                  {{AEESMS_ENC_OCTET,            UASMS_ENCODING_OCTET},
                  {AEESMS_ENC_IS91EP,            UASMS_ENCODING_IS91EP},
                  {AEESMS_ENC_ASCII,             UASMS_ENCODING_ASCII},
                  {AEESMS_ENC_LATIN,             UASMS_ENCODING_LATIN},
                  {AEESMS_ENC_IA5,               UASMS_ENCODING_IA5},
                  {AEESMS_ENC_UNICODE,           UASMS_ENCODING_UNICODE},
                  {AEESMS_ENC_SHIFT_JIS,         UASMS_ENCODING_SHIFT_JIS},
                  {AEESMS_ENC_KOREAN,            UASMS_ENCODING_KOREAN},  
                  {AEESMS_ENC_LATIN_HEBREW,      UASMS_ENCODING_LATIN_HEBREW},
                  {AEESMS_ENC_GSM_7_BIT_DEFAULT, UASMS_ENCODING_GSM_7_BIT_DEFAULT}};  

/*===========================================================================

  Prototypes

===========================================================================*/
#ifdef FEATURE_NOT_BREW_ONLY_SMS
extern uasms_status_e_type  uixuasms_config_listeners
( 
   uasms_message_listener_type    message_listener_f,
   uasms_status_listener_type     status_listener_f, 
   uasms_event_listener_type      event_listener_f
);
#else
void OEMUASMS_MessageListener
( 
   uasms_transaction_id_type tid, 
   const uasms_client_message_type * message_ptr
);

static uasms_status_e_type OEMSMS_GetMsgAckStatus(uint32 ts);
#endif // FEATURE_NOT_BREW_ONLY_SMS

uasms_status_e_type OEMUASMS_HandleIncomingMessage
(
   uasms_transaction_id_type tid,
   const uasms_client_message_type * pm
);

 void OEMUASMS_StatusListener
(
   const void * user_data, 
   uasms_error_class_e_type error_class,
   uasms_status_e_type status
);

void OEMUASMS_EventListener
(
   uasms_event_e_type event,
   uasms_event_info_type * info_ptr
);

boolean OEMUASMS_CopyMessage
( 
   uasms_transaction_id_type tid, 
   const uasms_client_message_type *pm, 
   SMSCacheEntry *pce 
);

static boolean OEMUASMS_GetNumMsgsInfoFromCDMABDMsg 
(
   uasms_client_bd_type * pcd,
   uasms_teleservice_e_type ts,
   int * pnNumMsgs
);

static boolean OEMUASMS_GetCallBackNumberFromCDMABDMsg
( 
   uasms_client_bd_type *pcd,
   uasms_teleservice_e_type ts,
   char * pBuffer
);

boolean OEMUASMS_GetAddressString
(
   byte * pSrc,
   int nDigits,
   uasms_digit_mode_e_type digitMode,
   uasms_number_type_e_type numberType,
   uasms_teleservice_e_type ts,
   char * pBuffer
);
  
static boolean OEMUASMS_ConstructClientBD 
(
   OEMMsgData * pMsgData,
   OEMMsgPayload * pMsgPayload,
   uasms_client_bd_type * pClientBD
); 

static boolean OEMUASMS_ConstructClientMessage 
(
   OEMMsgData * pMsgData,
   uasms_client_message_type * pCMsg,
   uasms_raw_bd_type * pRaw
);
 
static int OEMUASMS_GetTimeStampFromCDMABDMsg(uasms_client_bd_type *pcd);

static uasms_status_e_type OEMUASMS_SendSMSOnTrafficChannel(OEMMsg * hMsg);

static uint16 Xlate_TS(uasms_teleservice_e_type ts);
static uint32 Xlate_Lang(uasms_language_e_type lang);
static int Xlate_Priority(uasms_priority_e_type priority);
static int Xlate_Privacy(uasms_privacy_e_type privacy);

static int Xlate_uasms_status_e_type_2_MOSMS_ERR(uasms_status_e_type e );
static int Xlate_uasms_error_class_e_type_2_MOSMS_ERRTYPE(uasms_error_class_e_type e );

static uasms_priority_e_type Xlate_2_NativePriority(int priority);
static uasms_privacy_e_type Xlate_2_NativePrivacy(int privacy);
static uasms_language_e_type Xlate_2_NativeLang(int lang);
static uasms_teleservice_e_type Xlate_2_NativeTS(uint16 ts);

// Broadcast Function Prototypes
#ifdef FEATURE_BREW_BROADCAST_SMS
static int Xlate_uasms_status_2_AEEErrorCode(uasms_status_e_type st);
static int Xlate_uasms_cmd_status_2_AEEErrorCode(uasms_cmd_status_e_type st);

static void OEMUASMSBCConfig_CommandCallback(uasms_cmd_id_e_type cmd, uasms_cmd_status_e_type status);
static void OEMUASMSBCConfig_EventCallback(uasms_event_e_type event, uasms_event_info_type * info_ptr);
static boolean ConvertAEESMSBCSrvIDToUASMSBCSrvID(AEESMSBCSrvID *pSrvID, uasms_bc_service_id_type *service_id);
static boolean ConvertUASMSBCSrvIDToAEESMSBCSrvID(uasms_bc_service_id_type *service_id, AEESMSBCSrvID *pSrvID);
static boolean ConvertOEMSMSBCSrvInfoToUASMSBCSrvInfo(OEMSMSBCSrvInfo *pSrvInfo, uasms_bc_service_info_type *srv_info);
static boolean ConvertUASMSBCSrvInfoToOEMSMSBCSrvInfo(uasms_bc_service_info_type *srv_info, OEMSMSBCSrvInfo *pSrvInfo);
#endif // FEATURE_BREW_BROADCAST_SMS

/*===========================================================================

  Called as part of SMS initialization. Registers message, event and status
  listeners with SMS task

===========================================================================*/
int OEMSMS_Init( void )
{
   FARF(SMS, ("==> OEMSMS_Init"));

   if (gpUasms)
      return SUCCESS;

   if ((gpUasms = sys_malloc(sizeof(OEMUasms))) == NULL)
      return ENOMEMORY;

   if (OEM_GetConfig(CFGI_BREWDIR_SMS_TELESERVICE, (void*)&gpUasms->dwBREWDirectedTS, sizeof(gpUasms->dwBREWDirectedTS)) != SUCCESS)
      gpUasms->dwBREWDirectedTS = 0;

   if (OEM_GetConfig(CFGI_SMS_MO_ON_ACCESS_CHANNEL, (void*)&gpUasms->bMOOnAccessChannel, sizeof(gpUasms->bMOOnAccessChannel)) != SUCCESS)
      gpUasms->bMOOnAccessChannel = FALSE;   // Disable by default

   if (OEM_GetConfig(CFGI_SMS_MO_ON_TRAFFIC_CHANNEL, (void*)&gpUasms->bMOOnTrafficChannel, sizeof(gpUasms->bMOOnTrafficChannel)) != SUCCESS)
      gpUasms->bMOOnTrafficChannel = TRUE;   // Enable by default

#ifdef FEATURE_NOT_BREW_ONLY_SMS   
   uixuasms_config_listeners((uasms_message_listener_type)OEMUASMS_HandleIncomingMessage, 
                              OEMUASMS_StatusListener, 
                              OEMUASMS_EventListener);
   FARF(SMS, ("SMS Listeners Registered"));
   return SUCCESS;
#else
   if (uasms_config_listeners((uasms_message_listener_type)OEMUASMS_MessageListener, 
                              OEMUASMS_StatusListener, 
                              OEMUASMS_EventListener) == UASMS_OK_S)
   {    
      FARF(SMS, ("SMS Listeners Registered")); 
      //Turn on the auto-disconnect feature of the DTC call.
      (void) uasms_DTC_enable_auto_disconnect(SO6_TIMEOUT_LENGTH);
      return SUCCESS;
   }
   else
   {    
      FARF(SMS, ("Failed to register SMS Listeners"));
      sys_free( gpUasms );
      gpUasms = NULL;
      return EFAILED;
   }
#endif // FEATURE_NOT_BREW_ONLY_SMS
}

#ifndef FEATURE_NOT_BREW_ONLY_SMS

/*===========================================================================

  Called by SMS task to handle incoming SMS message.  Cache the message
  and notify the registered handler...

===========================================================================*/
void OEMUASMS_MessageListener
(
   uasms_transaction_id_type tid,
   const uasms_client_message_type * pm
)
{
   uasms_status_e_type eStatus;

   FARF(SMS, ("==> OEMUASMS_MessageListener"));

   if ((eStatus = OEMUASMS_HandleIncomingMessage(tid, pm)) != UASMS_OK_S)
   {
      uasms_ack_message(tid, UASMS_ERROR_TEMP, eStatus);
   }
   else
   {
      if (!pm->is_broadcast && pm->is_tl_ack_requested)
      {
         FARF(SMS, ("ACK requested and sent for Msg (Id = %d)", tid));
         uasms_ack_message(tid, UASMS_ERROR_NONE, UASMS_OK_S);
      } 
      else 
      {
         FARF(SMS, ("No ACK requested for MT SMS Msg (Id = %d)", tid));
      }
   }
}

/*===========================================================================

   Gets message ack status for specified teleservice ID

===========================================================================*/
static uasms_status_e_type OEMSMS_GetMsgAckStatus(uint32 ts)
{
   uasms_status_e_type eStatus = UASMS_INVALID_TELESERVICE_ID_S;

   if (!gpUasms)
      return(eStatus);

   switch (ts)
   {
      case SMS_TELESERVICE_CMT_95:
      case SMS_TELESERVICE_IS91_SHORT_MESSAGE:
         eStatus = (gpUasms->dwTextStatus == AEESMS_CS_OK ? UASMS_OK_S : UASMS_DESTINATION_RESOURCE_SHORTAGE_S);
         break;

      case SMS_TELESERVICE_MWI:
      case SMS_TELESERVICE_VMN_95:
      case SMS_TELESERVICE_IS91_VOICE_MAIL:
         eStatus = (gpUasms->dwVMStatus == AEESMS_CS_OK ? UASMS_OK_S : UASMS_DESTINATION_RESOURCE_SHORTAGE_S);
         break;

      case SMS_TELESERVICE_CPT_95:
      case SMS_TELESERVICE_IS91_PAGE:
         eStatus = (gpUasms->dwPageStatus == AEESMS_CS_OK ? UASMS_OK_S : UASMS_DESTINATION_RESOURCE_SHORTAGE_S);
         break;

      default:
         eStatus = UASMS_OK_S;
         break;
   }

   return(eStatus);
}
#endif // FEATURE_NOT_BREW_ONLY_SMS

/*===========================================================================

  Called by SMS task to handle incoming SMS message.  Cache the message
  and notify the registered handler...

===========================================================================*/
uasms_status_e_type OEMUASMS_HandleIncomingMessage
(
   uasms_transaction_id_type tid,
   const uasms_client_message_type * pm
)
{
   SMSCacheEntry * pce = NULL;

   FARF(SMS, ("==> OEMUASMS_HandleIncomingMessage"));

   if (OEMSMS_IsInitialized() == FALSE) 
   {
      FARF(SMS, ("SMS object not present"));      
      FARF(SMS, ("Rejected SMS Msg (Id = %d)", tid));
      return UASMS_DESTINATION_OUT_OF_SERVICE_S;
   }

   FARF(SMS, ("Received SMS Msg (Id = %d)", tid));
   FARF(SMS, ("Message Teleservice Id = %x", pm->teleservice));

   // Get a buffer to copy
   pce = OEMSMSCache_GetEntry();

   if (pce == NULL) 
   {
      // Ooops, we do not have any free Cache Entry's available!
      FARF(SMS, ("No Cache Entry available, SMS Msg (Id = %d) Discarded", tid));
      return UASMS_DESTINATION_BUSY_S;
   }

   // Decode the incoming message and populate the Cache Entry

   // If it's not a valid message, exit
   if (OEMUASMS_CopyMessage(tid,pm,pce) == FALSE) 
   {
      OEMSMSCache_Release(pce);     
      FARF(SMS, ("Error processing MT SMS Msg (Id = %d)", tid));
      return UASMS_INVALID_TELESERVICE_ID_S;
   }
   else
   {
#ifdef FEATURE_NOT_BREW_ONLY_SMS
      char * pszText = NULL;
      boolean bRet = FALSE;

      OEMSMSCache_Ready(pce);

      if ((pce->ts == SMS_TELESERVICE_CMT_95) ||
          (pce->ts == SMS_TELESERVICE_IS91_SHORT_MESSAGE))
      {
         if ((pszText = (char*)AEE_GetDecodedTextString(pce->pText, pce->nText, pce->nEncoding, NULL, NULL)) != NULL)
         {
            if (AEE_IsBREWDirectedSMS(pszText) == TRUE)
               bRet = TRUE;
            FREE(pszText);
         }
      }

      if (bRet)
      {      
         if (pm->is_tl_ack_requested)
         {
            FARF(SMS, ("ACK requested and sent for Msg (Id = %d)", tid));
            uasms_ack_message(tid, UASMS_ERROR_NONE, UASMS_OK_S);
         } 
         else 
         {
            FARF(SMS, ("No ACK requested for MT SMS Msg (Id = %d)", tid));
         }
         return UASMS_OK_S;
      }
      else
      {
         // Return error for non BREW directed messages so that they can be shared
         return UASMS_INVALID_TELESERVICE_ID_S;
      }
#else
      OEMSMSCache_Ready(pce);

      return OEMSMS_GetMsgAckStatus(pce->ts);
#endif // FEATURE_NOT_BREW_ONLY_SMS
   }
}

/*===========================================================================

  Called by OEMUASMS_MessageListener to copy incoming SMS message to SMS cache

===========================================================================*/
boolean OEMUASMS_CopyMessage
( 
   uasms_transaction_id_type tid, 
   const uasms_client_message_type *pm, 
   SMSCacheEntry *pce 
)
{
   uasms_client_bd_type * pcd;
   uasms_status_e_type    decodeStatus;
   uasms_teleservice_e_type ts;

   FARF(SMS, ("==> OEMUASMS_CopyMessage"));
   FARF(SMS, ("SMS Len: %d, Format: %d", pm->raw_bd.bd_len, pm->raw_bd.format));

   // Save the teleservice ID and then decode into the cache entry

   pcd = (uasms_client_bd_type *)(void*)(pce->data); 

   decodeStatus = uasmsx_decode_bearer_data (&pm->raw_bd, pcd);  

   // Only continue if we have bearer data (text) or it's a voice mail message
   // or an IS91 Page message. (We reject CPT-95, CMT-95 and IS91 Short Message
   // Types if the Message does not contain user data ).

   if (decodeStatus == UASMS_OK_S)
   {
      // If the teleservice of the message happens to be UASMS_TELESERVICE_CMT_91
      // we shall map the teleservice to the teleservice corresponding 
      // to the Message Type of the embedded Analog IS91-EP message.
      // See section '4.3.2 IS-91 Extended Protocol Enhanced Services' of IS-637-B
      // for information about CMT_91 teleservice ( note that the section does not 
      // directly mention CMT_91 teleservice, but talks about how a IS91-EP message
      // can be embedded inside a SMS Deliver Message. )
      // CMT_91 teleservice is used to embed a IS-91 EP SMS message inside a SMS 
      // Deliver Message in a CDMA network.

      if (pm->teleservice == UASMS_TELESERVICE_CMT_91) 
      {
         // A CMT_91 Teleservice message contains a embedded IS91 message
         // the is91ep_type in the user_data specifies the IS91 Message 
         // type ( See Section 4.5.2 of IS637-B ).

         if (pcd->user_data.is91ep_type == UASMS_IS91EP_CLI_ORDER) 
         {
            ts = UASMS_TELESERVICE_IS91_PAGE; 
         } 
         else if (pcd->user_data.is91ep_type == UASMS_IS91EP_VOICE_MAIL) 
         {
            ts = UASMS_TELESERVICE_IS91_VOICE_MAIL;
         } 
         else if (pcd->user_data.is91ep_type == UASMS_IS91EP_SHORT_MESSAGE ||
                  pcd->user_data.is91ep_type == UASMS_IS91EP_SHORT_MESSAGE_FULL) 
         {
            ts = UASMS_TELESERVICE_IS91_SHORT_MESSAGE;
         } 
         else 
         {
            // This should never happen, but better safe than sorry....
            FARF(SMS, ("CMT_91 Msg with unknown IS91-EP Message Type %d", pcd->user_data.is91ep_type));
            FARF(SMS, ("Rejecting CMT_91 Msg"));
            return(FALSE);
         }
      }
      else
      {
         ts = pm->teleservice;
      }
      
      pce->ts = pm->is_broadcast ? SMS_TELESERVICE_BROADCAST : Xlate_TS(ts);
      pce->dwSrvID = pm->service;
      pce->pText = (pcd->mask & UASMS_MASK_BD_USER_DATA ? (byte*)pcd->user_data.data : NULL);
      pce->nText = (pcd->mask & UASMS_MASK_BD_USER_DATA ? pcd->user_data.data_len : 0);
      pce->nEncoding = Xlate_Encoding(pcd->user_data.encoding);

      // Before rest of the processing, let's ensure that if message is BREW directed,
      // its sent with teleservice ID designated for BREW directed messages
      if (gpUasms->dwBREWDirectedTS && pce->pText && pce->nText)
      {
         char * pszText = NULL;

         if (((pszText = (char*)AEE_GetDecodedTextString(pce->pText, pce->nText, pce->nEncoding, NULL, NULL)) != NULL) &&
            (AEE_IsBREWDirectedSMS(pszText) == TRUE) &&
            (ts != gpUasms->dwBREWDirectedTS))
         { 
            // Reject messages which are BREW directed but aren't sent with 
            // teleservice ID designated for BREW directed messages
            FREE(pszText);
            return FALSE;  
         }

         if (pszText)         
            FREE(pszText);
      }

      // Priority
      if (pcd->mask & UASMS_MASK_BD_PRIORITY )
      {
         pce->nPriority = Xlate_Priority(pcd->priority);
         pce->bPriorityPresent = TRUE;
         FARF(SMS, ("nPriority = %d", pce->nPriority));
      }
      else
      {
         pce->bPriorityPresent = FALSE;
      }

      // Privacy
      if (pcd->mask & UASMS_MASK_BD_PRIVACY )
      {
         pce->nPrivacy = Xlate_Privacy(pcd->privacy);
         pce->bPrivacyPresent = TRUE;
         FARF(SMS, ("nPrivacy = %d",pce->nPrivacy));
      }
      else
      {
         pce->bPrivacyPresent = FALSE;
      }

      // Language
      if (pcd->mask & UASMS_MASK_BD_LANGUAGE )
      {
         pce->dwLang = Xlate_Lang(pcd->language);
         pce->bLangPresent = TRUE;
         FARF(SMS, ("dwLang = %d", pce->dwLang));
      }
      else
      {
         pce->bLangPresent = FALSE;
      }
      pce->dwTimeStamp = OEMUASMS_GetTimeStampFromCDMABDMsg(pcd);
      pce->bRequestDLAck = pcd->reply_option.delivery_ack_requested;
      pce->bRequestUserAck = pcd->reply_option.user_ack_requested;
      pce->dwMsgID = pcd->message_id.id_number;      
      pce->bIsDLAck = (boolean)(UASMS_BD_TYPE_DELIVERY_ACK == pcd->message_id.type);
      pce->bIsUserAck = (boolean)(UASMS_BD_TYPE_USER_ACK == pcd->message_id.type); 
      pce->dwMessageDisplayMode = ((pcd->mask & UASMS_MASK_BD_DISPLAY_MODE) ? (uint32)pcd->display_mode : AEESMS_DISPLAY_MODE_NONE);

      if (OEMUASMS_GetNumMsgsInfoFromCDMABDMsg (pcd, ts, &pce->nNumMsgs) == FALSE)
         return FALSE;

      if (OEMUASMS_GetCallBackNumberFromCDMABDMsg(pcd, ts, pce->szCallback) == FALSE)
         return FALSE;

      OEMUASMS_GetAddressString((byte*)pm->address.digits, pm->address.number_of_digits,
                                pm->address.digit_mode, pm->address.number_type,
                                ts, pce->szFrom);

      FARF(SMS, ("tid = %d, nText = %d, nEncoding = %d", pce->dwTransactionId, pce->nText, pce->nEncoding));
      FARF(SMS, ("Text = %s", pce->pText));
      FARF(SMS, ("TimeStamp = %d, nNumMsgs = %d", pce->dwTimeStamp, pce->nNumMsgs));
      FARF(SMS, ("Callback = %s", (char*)(*pce->szCallback ? pce->szCallback : "Unknown")));
      FARF(SMS, ("From = %s", (char*)(*pce->szFrom ? pce->szFrom : "Unknown")));
      return(TRUE);
   }
   FARF(SMS, ("Teleservice %d Msg has no UserData",pce->ts));
   return(FALSE);
}

/*===========================================================================

  Translates UASMS Teleservice ID to AEE TAPI Teleservice ID

===========================================================================*/
static uint16 Xlate_TS(uasms_teleservice_e_type ts)
{
   TsIdMap* pe;
   int i,nEntries = sizeof(gsTsIdMap) / sizeof(TsIdMap);

   for(i = 0, pe = (TsIdMap *)gsTsIdMap; i < nEntries; pe++,i++)
   {
      if(pe->UASMS_TsId == ts)
         return(pe->AEETAPI_TsId);
   }

   // Assume DMSS uses standard TS IDs so that we dont need a mapping table and 
   // we can return the same TS ID
   return(ts); 
}

/*===========================================================================

   Translation of Priority Codes from DMSS to AEE

===========================================================================*/
static int Xlate_Priority(uasms_priority_e_type priority)
{
   PriorityMap * pe;
   int i,nEntries = sizeof(gsPriorityMap) / sizeof(PriorityMap);

   for(i = 0, pe = (PriorityMap *)gsPriorityMap; i < nEntries; pe++,i++)
   {
      if(pe->OEM_Priority == priority)
         return(pe->AEE_Priority);

   }

   return(SMS_PRIORITY_NORMAL); // Default   

}

/*===========================================================================

  Translation of Privacy Codes from DMSS to AEE

===========================================================================*/
static int Xlate_Privacy(uasms_privacy_e_type privacy)
{
   PrivacyMap * pe;
   int i,nEntries = sizeof(gsPrivacyMap) / sizeof(PrivacyMap);

   for(i = 0, pe = (PrivacyMap *)gsPrivacyMap; i < nEntries; pe++,i++)
   {
      if(pe->OEM_Privacy == privacy)
         return(pe->AEE_Privacy);
   }

   return(SMS_PRIVACY_NORMAL); // Default   
}

/*===========================================================================

  Translation of Language Codes from DMSS to AEE

===========================================================================*/
static uint32 Xlate_Lang(uasms_language_e_type lang)
{
   LangMap * pe;
   int i,nEntries = sizeof(gsLangMap) / sizeof(LangMap);

   for(i = 0, pe = (LangMap *)gsLangMap; i < nEntries; pe++,i++)
   {
      if(pe->OEM_Lang == lang)
         return(pe->AEE_Lang);
   }

   // Assume DMSS uses our LanguageCodes so that we dont need a mapping table and 
   // we can return the same code
   return(lang); 
}

/*===========================================================================

  Translation of Encoding Codes from DMSS to AEE

===========================================================================*/
int Xlate_Encoding(uasms_user_data_encoding_e_type enc)
{
   EncMap * pe;
   int i,nEntries = sizeof(gsEncMap) / sizeof(EncMap);

   for(i = 0, pe = (EncMap *)gsEncMap; i < nEntries; pe++,i++)
   {
      if(pe->OEM_Enc == enc)
         return(pe->AEE_Enc);

   }

   // We don't understand this encoding. Anyway, pass it to client and hope
   // that client understands it
   return(enc); 
}

/*===========================================================================

  This function converts the Timestamp information present in the UASMS Bearer 
  Data structure, from Julian format to seconds format. If the UASMS Bearer Data 
  structure does not contain Timestamp information,this function sets the 
  dwTimeStamp field to 0.

===========================================================================*/
static int OEMUASMS_GetTimeStampFromCDMABDMsg(uasms_client_bd_type *pcd)
{
   FARF(SMS, ("==>  OEMUASMS_GetTimeStampFromCDMABDMsg"));

   // Time stamp in the UASMS Client Bearer Data message is in
   // the Julian format, we'll convert that to seconds

   if (pcd->mask & UASMS_MASK_BD_MC_TIME) 
   {
      uint8           tmp;
      clk_julian_type jt;

      MEMSET(&jt, 0, sizeof(jt));

      tmp = BCD2INT(pcd->mc_time.year);
   
      // If the year is above 95 it we assume it is in 20 th century(1900+ year
      // otherwise we assume it is in 21 century(2000+year)
      jt.year = (tmp <= 95) ? (YEAR_2000 + tmp) : (YEAR_1900 + tmp);
      jt.month = BCD2INT(pcd->mc_time.month);
      jt.day = BCD2INT(pcd->mc_time.day);
      jt.hour = BCD2INT(pcd->mc_time.hour);
      jt.minute = BCD2INT(pcd->mc_time.minute);
      jt.second = BCD2INT(pcd->mc_time.second);

      return clk_julian_to_secs(&jt);
   } 
   else
   {
      FARF(SMS, ("MT SMS msg has no Timestamp info"));
      return 0;
   }
}

/*===========================================================================

  Gets number of messages information from the UASMS Client Bearer Data.

===========================================================================*/
static boolean OEMUASMS_GetNumMsgsInfoFromCDMABDMsg 
(
   uasms_client_bd_type * pcd, 
   uasms_teleservice_e_type ts, 
   int * pnNumMsgs
)
{
   FARF(SMS, ("==>  OEMUASMS_GetNumMsgsInfoFromCDMABDMsg"));

   *pnNumMsgs = 0;

   // Check if Num Messages information is present in the Bearer Data
   // Structure.
   if (pcd->mask & UASMS_MASK_BD_NUM_OF_MSGS) 
   {
      *pnNumMsgs = pcd->num_messages;
   }

   return TRUE;
}

/*===========================================================================

  This function gets the Callback number from the UASMS Client Bearer Data
  in the specified buffer.              

===========================================================================*/
static boolean OEMUASMS_GetCallBackNumberFromCDMABDMsg
( 
   uasms_client_bd_type *pcd,
   uasms_teleservice_e_type ts,
   char * pBuffer
)
{
   FARF(SMS, ("==>  OEMUASMS_GetCallBackNumberFromCDMABDMsg"));

   // Check if the UASMS bd message has Callback number 
   if (pcd->mask & UASMS_MASK_BD_CALLBACK) 
   {
      // If the incoming message is  IS91 Page message, we shall
      // re-compute the number of digits in the callback number by
      // ignoring the trailing '0x0' digits. ( A real 0 is encoded in
      // the IS91 CLI message as '0xA' so we are not in danger of ignoring
      // legitimate 0 digits from the message. )

      if (ts == UASMS_TELESERVICE_IS91_PAGE) 
      {
         int j = 0;

         // In an IS91 Page message, the Callback number is padded with trailing
         // zeroes, we do not want to display these digits, so we try to strip
         // the trailing zeroes out by setting the number_of_digits information
         // appropriately. E.g. 55512120000 ( for 5551212 )

         // Count the number of trailing zeros in the callback number
         // array and recompute the length of the callback number ignoring
         // the trailing zeros.

         for (j = pcd->callback.number_of_digits - 1; j >= 0; j--) 
         {
            if (pcd->callback.digits[j] == 0x0) 
            {
               pcd->callback.number_of_digits--;
            } 
            else 
            {
               // break at the first occurance of a non zero digit.
               break;
            }
         }
      }
      
      return OEMUASMS_GetAddressString((byte*)pcd->callback.digits, 
                                       pcd->callback.number_of_digits,
                                       pcd->callback.digit_mode, 
                                       pcd->callback.number_type,
                                       ts, 
                                       pBuffer);
   }
   return TRUE;
}

/*===========================================================================

  This function gets the phone number as string copied to pBuffer              

===========================================================================*/
boolean OEMUASMS_GetAddressString
(
   byte * pSrc,
   int nDigits,
   uasms_digit_mode_e_type digitMode,
   uasms_number_type_e_type numberType,
   uasms_teleservice_e_type ts,
   char * pBuffer
)
{
   int i;
   byte * pDst, bval;
   boolean bCBNumIsAllZeros = FALSE;

   FARF(SMS, ("==>  OEMUASMS_GetAddressString"));

   pDst = (byte*)pBuffer;

   if (numberType == UASMS_NUMBER_INTERNATIONAL) 
   {
      *pDst = DIALER_PLUS_CHAR;
      pDst++;
   }

   if (digitMode == UASMS_DIGIT_MODE_8_BIT) 
   {
      // Each byte in the callback.digits array is a ascii representation
      // of a digit, so we can directly copy each byte into the cbDigits
      // array.
      MEMCPY(pDst, pSrc, (unsigned int)nDigits);
   }   
   else if (digitMode == UASMS_DIGIT_MODE_4_BIT) 
   {
      // If the digit mode is 4 Bit Mode, then each byte in the 
      // callback.digits array has a four bit numeric representation of
      // a digit, where values 0x0A represents '0', 0x0b - '#', '0x0c' - '*'
      // 0x01 - 0x09 - '1' - '9'.
      // We call GetDigit to convert each byte to corresponding 
      // digit.

      i = nDigits;

      while (i--) 
      {
         bval = (byte)*(pSrc++);
         *(pDst++) = (char)GetDigit((byte)(bval & 0x0f));
      }
   } 
   else 
   {
      // Unknown Digit mode
      FARF(SMS, ("Unknown Digit Mode in Address %d", digitMode));
      return FALSE;
   }

   // Reset pDst to point to the beginning of pBuffer string.
   pDst = (byte*)pBuffer;

   // If the Callback number is all '0' then report error
   for (i = 0; i < nDigits; i++) 
   {
      if (pDst[i] != '0') 
      {
         bCBNumIsAllZeros = FALSE;
         break;
      }
   }

   if (bCBNumIsAllZeros == TRUE) 
   {
      FARF(SMS, ("Callback number in SMS Msg has all 0 digits"));
      return FALSE;
   }
   return TRUE;
}

/*===========================================================================

  Constructs OEMMsg struct to be sent              

===========================================================================*/
int OEMSMS_NewMessage( OEMMsgData * pMsgData, OEMMsgPayload * pMsgPayload, AEETSysMode sysMode, OEMMsg ** hMsgParam )
{
   OEMMsg* hMsg = NULL;
   uasms_client_bd_type clientBD;
   uasms_raw_bd_type rawBD;

   FARF(SMS, ("==>  OEMSMS_NewMessage"));

   hMsg = sys_malloc( sizeof(OEMMsg) );

   if( !hMsg ) {

      return ENOMEMORY;
   }

   MEMSET((void*)&clientBD, 0, sizeof(clientBD));
   MEMSET(&rawBD, 0, sizeof(rawBD));

   // Construct Client BD Type
   OEMUASMS_ConstructClientBD( pMsgData, pMsgPayload, &clientBD );

   // Construct Raw BD Type
   uasmsx_encode_bearer_data(&clientBD, &rawBD);

   // Construct Client Message
   OEMUASMS_ConstructClientMessage( pMsgData, &hMsg->CMsg, &rawBD );

   *hMsgParam = hMsg;
   return AEE_SUCCESS;
}

/*===========================================================================

  Converts AEE encoding to DMSS encoding            

===========================================================================*/
uasms_user_data_encoding_e_type Xlate_2_NativeEncoding( int enc )
{
   EncMap * pe;
   int i,nEntries = sizeof(gsEncMap) / sizeof(EncMap);

   for(i = 0, pe = (EncMap *)gsEncMap; i < nEntries; pe++,i++)
   {
      if(pe->AEE_Enc == enc)
         return(pe->OEM_Enc);
   }
   // Assume that we are being passed an encoding understood by DMSS
   return(enc); 
}

/*===========================================================================

  Constructs the client bearer data from the message and the user data

===========================================================================*/
static boolean OEMUASMS_ConstructClientBD 
(
   OEMMsgData * pMsgData,
   OEMMsgPayload * pMsgPayload,
   uasms_client_bd_type * pClientBD
) 
{
   static uint16 id = 0;
   uasms_user_data_encoding_e_type ne;

   FARF(SMS, ("==>  OEMUASMS_ConstructClientBD"));

   // Reset Mask
   pClientBD->mask = 0;
   
   // Copy User Data
   ne = Xlate_2_NativeEncoding( pMsgPayload->nEncoding );

   switch (ne) 
   {
      case UASMS_ENCODING_ASCII:  // 7-bit ASCII
      case UASMS_ENCODING_IA5:
         pClientBD->user_data.encoding = ne;
         pClientBD->user_data.is91ep_type = (uasms_IS91EP_type_e_type)0;
         pClientBD->user_data.padding_bits = pMsgPayload->nPadding;
         pClientBD->user_data.data_len = (uint8)MIN(pMsgPayload->nBytes,UASMS_USER_DATA_MAX);
         pClientBD->user_data.number_of_digits = (pClientBD->user_data.data_len*8 - pClientBD->user_data.padding_bits)/7;
         // copy the data directly
         MEMCPY((void*)pClientBD->user_data.data, (void*)pMsgPayload->data, (uint32)pClientBD->user_data.data_len);
         break;

      case UASMS_ENCODING_UNICODE: // Unicode
         pClientBD->user_data.encoding = ne;
         pClientBD->user_data.is91ep_type = (uasms_IS91EP_type_e_type)0;
         pClientBD->user_data.data_len = (uint8)MIN(pMsgPayload->nBytes,UASMS_USER_DATA_MAX);
         if( pClientBD->user_data.data_len & 0x01 ) // Even it
         { 
            pClientBD->user_data.data_len--;
         }
         pClientBD->user_data.padding_bits = pMsgPayload->nPadding;
         pClientBD->user_data.number_of_digits = pClientBD->user_data.data_len/2;
         // copy the data directly
         MEMCPY((void*)pClientBD->user_data.data, (void*)pMsgPayload->data, (uint32)pClientBD->user_data.data_len);
         break;

      default:
         // return the data as is--no interpretation
         pClientBD->user_data.encoding = ne;
         pClientBD->user_data.is91ep_type = (uasms_IS91EP_type_e_type)0;
         pClientBD->user_data.data_len = (uint8)MIN(pMsgPayload->nBytes,UASMS_USER_DATA_MAX);
         pClientBD->user_data.padding_bits = pMsgPayload->nPadding;
         pClientBD->user_data.number_of_digits = pClientBD->user_data.data_len;
         // copy the data directly
         MEMCPY((void*)pClientBD->user_data.data, (void*)pMsgPayload->data, (uint32)pClientBD->user_data.data_len);
   }
   pClientBD->mask |= UASMS_MASK_BD_USER_DATA;

   // Message Identifier
   if (pMsgData->bIsUserAck)
   {
      pClientBD->message_id.type = UASMS_BD_TYPE_USER_ACK;  // User Ack
   }
   else
   {
      pClientBD->message_id.type = UASMS_BD_TYPE_SUBMIT;    // Mobile Originated
   }
   pClientBD->message_id.id_number = (pMsgData->dwMsgID ? pMsgData->dwMsgID : id++);               // Id
   pClientBD->mask |= UASMS_MASK_BD_MSG_ID;              // set mask

   // Reply option is an optional bearer data parameter
   // This subparameter should not be included in the message with
   // USER_ACK_REQ, DAK_REQ and READ_ACK_REQ all set to ‘0’.
   if ( pMsgData->bRequestDLAck || 
         pMsgData->bRequestUserAck ||
         pMsgData->bRequestReadAck )
   {
      pClientBD->reply_option.delivery_ack_requested = pMsgData->bRequestDLAck;
      pClientBD->reply_option.user_ack_requested = pMsgData->bRequestUserAck;
      //pClientBD->reply_option.read_ack_requested = pMsgData->bRequestReadAck;

      pClientBD->mask |= UASMS_MASK_BD_REPLY_OPTION; //BD reply option on
   }
   
   // callback number
   if (*pMsgData->szCallback)
   {
      pClientBD->callback.digit_mode = UASMS_DIGIT_MODE_4_BIT;
      pClientBD->callback.number_type = UASMS_NUMBER_NATIONAL;
      pClientBD->callback.number_plan = UASMS_NUMBER_PLAN_TELEPHONY;
      pClientBD->callback.number_of_digits = (byte)GetASCII2DTMF(pMsgData->szCallback, 
                                                                 (byte*)pClientBD->callback.digits, 
                                                                 STRLEN(pMsgData->szCallback));

      if (pClientBD->callback.number_of_digits)
         pClientBD->mask |= UASMS_MASK_BD_CALLBACK;
   }

// Priority
   if ( pMsgData->bPriorityPresent )
   {
      pClientBD->priority = Xlate_2_NativePriority(pMsgData->nPriority);
      pClientBD->mask |= UASMS_MASK_BD_PRIORITY;
   }

   // Privacy
   if ( pMsgData->bPrivacyPresent )
   {
      pClientBD->privacy = Xlate_2_NativePrivacy(pMsgData->nPrivacy);
      pClientBD->mask |= UASMS_MASK_BD_PRIVACY;
   }

   // Language
   if ( pMsgData->bLangPresent )
   {
      pClientBD->language = Xlate_2_NativeLang(pMsgData->dwLang);
      pClientBD->mask |= UASMS_MASK_BD_LANGUAGE;
   }
   
   if ((!pMsgData->bIsMOSMS) && (AEESMS_DISPLAY_MODE_NONE != pMsgData->dwMessageDisplayMode))
   {
      pClientBD->display_mode = (uasms_display_mode_e_type)pMsgData->dwMessageDisplayMode;
      pClientBD->mask |= UASMS_MASK_BD_DISPLAY_MODE;
   }

   return TRUE;
}

/*===========================================================================

  Translates to native priority

===========================================================================*/
static uasms_priority_e_type Xlate_2_NativePriority(int priority)
{
   int i;

   for (i = 0; i < sizeof(gsPriorityMap)/sizeof(PriorityMap); i++)
   {
      if (gsPriorityMap[i].AEE_Priority == priority)
         return gsPriorityMap[i].OEM_Priority;
   }
   return priority;
}

/*===========================================================================

  Translates to native privacy

===========================================================================*/
static uasms_privacy_e_type Xlate_2_NativePrivacy(int privacy)
{
   int i;

   for (i = 0; i < sizeof(gsPrivacyMap)/sizeof(PrivacyMap); i++)
   {
      if (gsPrivacyMap[i].AEE_Privacy == privacy)
         return gsPrivacyMap[i].OEM_Privacy;
   }
   return privacy;
}

/*===========================================================================

  Translates to native language

===========================================================================*/
static uasms_language_e_type Xlate_2_NativeLang(int lang)
{
   int i;

   for (i = 0; i < sizeof(gsLangMap)/sizeof(LangMap); i++)
   {
      if (gsLangMap[i].AEE_Lang == lang)
         return gsLangMap[i].OEM_Lang;
   }
   return lang;
}

/*===========================================================================

  Translates AEE TAPI Teleservice ID to UASMS Teleservice ID

===========================================================================*/
static uasms_teleservice_e_type Xlate_2_NativeTS(uint16 ts)
{
   TsIdMap* pe;
   int i,nEntries = sizeof(gsTsIdMap) / sizeof(TsIdMap);

   for(i = 0, pe = (TsIdMap *)gsTsIdMap; i < nEntries; pe++,i++)
   {
      if(pe->AEETAPI_TsId == ts)
         return(pe->UASMS_TsId);
   }

   // Assume DMSS uses standard TS IDs so that we dont need a mapping table and 
   // we can return the same TS ID
   return(ts); 
}

/*===========================================================================

  Constructs a client message type from the raw bearer data

===========================================================================*/
static boolean OEMUASMS_ConstructClientMessage
(
   OEMMsgData * pMsgData,
   uasms_client_message_type * pCMsg,
   uasms_raw_bd_type * pRaw
) 
{
   char * atStringPtr, * dotStringPtr;

   FARF(SMS, ("==>  OEMUASMS_ConstructClientMessage"));

   pCMsg->is_broadcast = FALSE; 
   pCMsg->is_tl_ack_requested = TRUE;
   pCMsg->service = UASMS_SRV_UNKOWN;

   if (pMsgData->bIsBREWDirectedSMS)
   {
      if (OEM_GetConfig(CFGI_BREWDIR_SMS_TELESERVICE, (void*)&pCMsg->teleservice, sizeof(pCMsg->teleservice)) != SUCCESS)
      {
         pCMsg->teleservice = UASMS_TELESERVICE_CMT_95;
      }
   }
   else
   {
      pCMsg->teleservice = Xlate_2_NativeTS(pMsgData->ts);
   }

   // Set the raw_bd field with what we created above
   MEMCPY(&pCMsg->raw_bd, pRaw, sizeof(uasms_raw_bd_type));   
   
   // Determine whether the To address is an email address or a phone number.
   // We just check for <some chars>@<some chars>.<some chars>. May not be a 
   // totally fool proof way of checking/validating the email address. 
   // But it works!!!
   atStringPtr = STRCHR(pMsgData->szMOFromMTTo,(char)'@');
   dotStringPtr = STRCHR(pMsgData->szMOFromMTTo,(char)'.');

   // Check to make sure that the user entered both "@" and "." symbols
   // in his send to address.
   if (atStringPtr && dotStringPtr) 
   {   
      // Send address is an email address. Code the digit mode, number mode and
      // the number type accordingly.
      pCMsg->address.digit_mode = UASMS_DIGIT_MODE_8_BIT;            // 8 bit ASCII codes
      pCMsg->address.number_mode = UASMS_NUMBER_MODE_DATA_NETWORK; 
      pCMsg->address.number_type = UASMS_NUMBER_DATA_EMAIL;          // EMAIL address
      MEMSET((byte*)pCMsg->address.digits, 0, sizeof(pCMsg->address.digits));
      STRNCPY((char*)pCMsg->address.digits, pMsgData->szMOFromMTTo, sizeof(pCMsg->address.digits)-1);
      pCMsg->address.number_of_digits = (byte)STRLEN((char *)pMsgData->szMOFromMTTo);
   } 
   else 
   {  
      if (DIALER_PLUS_CHAR == *pMsgData->szMOFromMTTo) 
      {
         if (OEM_CONFIG_ALLOW_INTERNATIONAL) 
         {
            char * pcBuf = (char*)(pMsgData->szMOFromMTTo + 1); // Remove '+' char
            pCMsg->address.digit_mode  = UASMS_DIGIT_MODE_8_BIT;
            pCMsg->address.number_mode = UASMS_NUMBER_MODE_NONE_DATA_NETWORK;
            pCMsg->address.number_type = UASMS_NUMBER_INTERNATIONAL;
            MEMSET((byte*)pCMsg->address.digits, 0, sizeof(pCMsg->address.digits));
            // Remove the plus character and copy
            STRNCPY((char*)pCMsg->address.digits, pcBuf, sizeof(pCMsg->address.digits) - 1);         
            pCMsg->address.number_of_digits = (byte)STRLEN((char *)pcBuf);
         } 
         else 
         {
            int nPrefix = STRLEN(OEM_CONFIG_INTERNATIONAL_PREFIX);
            pCMsg->address.digit_mode = UASMS_DIGIT_MODE_4_BIT;   // DTMF digits
            pCMsg->address.number_mode = UASMS_NUMBER_MODE_NONE_DATA_NETWORK;  
            pCMsg->address.number_type = UASMS_NUMBER_UNKNOWN;  
            pCMsg->address.number_plan = UASMS_NUMBER_PLAN_UNKNOWN; 

            if (nPrefix)
            {            
               GetASCII2DTMF((char *)OEM_CONFIG_INTERNATIONAL_PREFIX, (byte*)pCMsg->address.digits, nPrefix);
            }         
            
            GetASCII2DTMF((char *)pMsgData->szMOFromMTTo, (byte*)(pCMsg->address.digits + nPrefix), STRLEN((char *)pMsgData->szMOFromMTTo));
            pCMsg->address.number_of_digits = (byte)(nPrefix + STRLEN((char *)pMsgData->szMOFromMTTo));
         }
      }
      else
      {
         pCMsg->address.digit_mode = UASMS_DIGIT_MODE_4_BIT;   // DTMF digits
         pCMsg->address.number_mode = UASMS_NUMBER_MODE_NONE_DATA_NETWORK; 
         pCMsg->address.number_type = UASMS_NUMBER_UNKNOWN;  
         pCMsg->address.number_plan = UASMS_NUMBER_PLAN_UNKNOWN; 
   
         GetASCII2DTMF((char *)pMsgData->szMOFromMTTo, (byte*)pCMsg->address.digits, STRLEN((char *)pMsgData->szMOFromMTTo));
   
         pCMsg->address.number_of_digits = (byte)STRLEN((char *)pMsgData->szMOFromMTTo);
      }
   }
   return TRUE;
}

/*===========================================================================

  Called to send SMS

===========================================================================*/
void OEMSMS_SendSMS( OEMMsg* hMsg, AEECallback *pcb, uint32 *pdwStatus )
{
   int nErr = SUCCESS;
   uint32 dwStatus = 0;

   FARF(SMS, ("==>  OEMSMS_SendSMS"));

   if(!gpUasms) 
   {   
      FARF(SMS, ("gpUasms NULL"));
      nErr = EFAILED;
      AEESMS_SETERROR(dwStatus, AEESMS_ERRORTYPE_TEMP, AEESMS_ERROR_GENERAL_PROBLEM);
      goto Done;
   }

   // Install the message being processed
   hMsg->pcb = pcb;
   hMsg->pdwStatus = pdwStatus;
   
   if (gpUasms->pCurMOSMS) 
   {
      hMsg->pNext = gpUasms->pCurMOSMS;
      gpUasms->pCurMOSMS->pPrev = hMsg;
   }

   gpUasms->pCurMOSMS = hMsg;

   if (gpUasms->bTransmissionPending) 
   {
      // We are done here as traffic channel is being brought up for some
      // previous message
      nErr = SUCCESS;
      goto Done;
   }

   if (gpUasms->bMOOnAccessChannel)
   {
      nErr = uasms_send_message((const void *)hMsg, &hMsg->CMsg);
   }
   else if (gpUasms->bMOOnTrafficChannel)
   {
      nErr = OEMUASMS_SendSMSOnTrafficChannel(hMsg);
   }
   else
   {
      FARF(SMS, ("Configuration Problem - Channel not specified for MO SMS"));
      nErr = EFAILED;
      AEESMS_SETERROR(dwStatus, AEESMS_ERRORTYPE_TEMP, AEESMS_ERROR_GENERAL_PROBLEM);
      goto Done;
   }

   if( nErr != UASMS_OK_S )
   {
      AEESMS_SETERROR(dwStatus, AEESMS_ERRORTYPE_UNKNOWN, Xlate_uasms_status_e_type_2_MOSMS_ERR(nErr));
      nErr = EFAILED;
   }
   else 
   {
      nErr = SUCCESS;
   }

Done:
   if (nErr != SUCCESS)
   {
      *pdwStatus = dwStatus;
      AEE_SYS_RESUME(pcb);
   }
}

/*===========================================================================

  Checks if node belongs to list of pending MO SMS

===========================================================================*/
boolean OEMUASMS_IsValidNode(OEMMsg* pNode)
{
   OEMMsg * pTest = NULL;
   for (pTest = gpUasms->pCurMOSMS; pTest; pTest = pTest->pNext)
   {
      if (pNode == pTest)
         return TRUE;
   }
   return FALSE;
}

/*===========================================================================

  Status listener registered with UASMS for being notified about status
  changes

===========================================================================*/
void OEMUASMS_StatusListener
(
   const void * user_data, 
   uasms_error_class_e_type error_class,
   uasms_status_e_type status
)
{
   OEMMsg* hMsg = (OEMMsg*)user_data;

   FARF(SMS, ("OEMUASMS_StatusListener"));

   //Null pointer check 
   if( !gpUasms || !hMsg )
   {
      return;
   }

   //Check if it is a CB for our MOSMS 
   if (OEMUASMS_IsValidNode(hMsg)) 
   {    
      // We expect to get two acks. 
      // First ack is from Layer 1 and we are done if this is error 
      // Second ack says msg has been reveived by the SMS Gateway. This is the TL Ack. 
      // When the second ack is received we are done. If the status says 
      // UASMS_WAITING_FOR_TL_ACK_S, then we need to wait for the second ack.

      if (status == UASMS_WAITING_FOR_TL_ACK_S)
      {
         return;  // Just return and wait
      }
      else if ((status == UASMS_ACCESS_TOO_LARGE_S) && gpUasms->bMOOnTrafficChannel)
      {
         uasms_status_e_type nErr;
         
         if ((nErr = OEMUASMS_SendSMSOnTrafficChannel(hMsg)) != UASMS_OK_S)
         {
            AEESMS_SETERROR(*(hMsg->pdwStatus), AEESMS_ERRORTYPE_UNKNOWN, Xlate_uasms_status_e_type_2_MOSMS_ERR(nErr));
            AEE_SYS_RESUME(hMsg->pcb);
         }
      }
      else 
      {
         AEESMS_SETERROR(*(hMsg->pdwStatus), Xlate_uasms_error_class_e_type_2_MOSMS_ERRTYPE(error_class), Xlate_uasms_status_e_type_2_MOSMS_ERR(status));
         AEE_SYS_RESUME(hMsg->pcb);
      }
   }
}

/*===========================================================================

  Event listener registered with UASMS for being notified on changes in 
  status events

===========================================================================*/
void OEMUASMS_EventListener
(
   uasms_event_e_type event,
   uasms_event_info_type * info_ptr
)
{
   uasms_status_e_type  ret = UASMS_NETWORK_NOT_READY_S;

   FARF(SMS, ("OEMUASMS_EventListener"));

   if( !gpUasms ) 
   {
      return;
   }

#ifdef FEATURE_BREW_BROADCAST_SMS
   if ((event == UASMS_EVENT_BC_CONFIG) ||
       (event == UASMS_EVENT_BC_PREF) ||
       (event == UASMS_EVENT_BC_TABLE) ||
       (event == UASMS_EVENT_BC_SRV_IDS) ||
       (event == UASMS_EVENT_BC_SRV_INFO) ||
       (event == UASMS_EVENT_BC_SRV_ADDED) ||
       (event == UASMS_EVENT_BC_SRV_DELETED) ||
       (event == UASMS_EVENT_BC_SRV_UPDATED))
   {
      OEMUASMSBCConfig_EventCallback(event, info_ptr);
   }
#endif // FEATURE_BREW_BROADCAST_SMS

   //We are interested in the Events only if we had initiated the TC to be brought up for
   //sending a SMS.
   if( gpUasms->bTransmissionPending == TRUE ) 
   {
      OEMMsg * pNode;
      gpUasms->bTransmissionPending = FALSE;

      if (event == UASMS_EVENT_DTC_CONNECTED) 
      {
         FARF(SMS, ("Phone_EL_CONN"));

         gpUasms->bDedicatedChannel = TRUE;

         for (pNode = gpUasms->pCurMOSMS; pNode; pNode = pNode->pNext)
         {
            // When message is sent (or failed), the function OEMUASMS_StatusListener() is invoked
            ret = uasms_send_message(pNode, &pNode->CMsg);

            if (UASMS_OK_S != ret)
            {
               AEESMS_SETERROR(*(pNode->pdwStatus), AEESMS_ERRORTYPE_UNKNOWN, Xlate_uasms_status_e_type_2_MOSMS_ERR(ret));
               AEE_SYS_RESUME(pNode->pcb);
            }
         }
      }
      else
      {  
         // Failure
         for (pNode = gpUasms->pCurMOSMS; pNode; pNode = pNode->pNext)
         {
            AEESMS_SETERROR(*(pNode->pdwStatus), AEESMS_ERRORTYPE_UNKNOWN, Xlate_uasms_status_e_type_2_MOSMS_ERR(ret));
            AEE_SYS_RESUME(pNode->pcb);
         }
      }
   }
}

/*===========================================================================

  Translates UASMS error to AEE error

===========================================================================*/
static int Xlate_uasms_status_e_type_2_MOSMS_ERR( uasms_status_e_type e )
{
   switch (e)
   {
      case UASMS_OK_S:
         return AEESMS_ERROR_NONE;

      case UASMS_ADDRESS_TRANSLATION_FAILURE_S:
         return AEESMS_ERROR_ADDRESS_TRANSLATION_FAILURE;

      case UASMS_SMS_ORIGINATION_DENIED_S:
         return AEESMS_ERROR_SMS_ORIGINATION_DENIED;

      case UASMS_SMS_TERMINATION_DENIED_S:
         return AEESMS_ERROR_SMS_TERMINATION_DENIED;

      case UASMS_SMS_NOT_SUPPORTED_S:
         return AEESMS_ERROR_SMS_NOT_SUPPORTED;

      case UASMS_ADDRESS_VACANT_S:
         return AEESMS_ERROR_ADDRESS_VACANT;

      case UASMS_NETWORK_RESOURCE_SHORTAGE_S:
         return AEESMS_ERROR_NETWORK_RESOURCE_SHORTAGE;

      case UASMS_NETWORK_FAILURE_S:
         return AEESMS_ERROR_NETWORK_FAILURE;

      case UASMS_INVALID_TELESERVICE_ID_S:
         return AEESMS_ERROR_NETWORK_INVALID_TELESERVICE_ID;

      case UASMS_OTHER_NETWORK_PROBLEM_S:
         return AEESMS_ERROR_NETWORK_PROBLEM;

      case UASMS_DESTINATION_BUSY_S:
         return AEESMS_ERROR_DESTINATION_BUSY;
      
      case UASMS_DESTINATION_OUT_OF_SERVICE_S:
         return AEESMS_ERROR_DESTINATION_OUT_OF_SERVICE;

      case UASMS_NO_PAGE_RESPONSE_S:
         return AEESMS_ERROR_DESTINATION_NO_PAGE_RESPONSE;

      case UASMS_NO_ACK_S:
         return AEESMS_ERROR_NO_ACK;

      case UASMS_DESTINATION_RESOURCE_SHORTAGE_S:
         return AEESMS_ERROR_DESTINATION_RESOURCE_SHORTAGE;

      case UASMS_SMS_DELIVERY_POSTPONED_S:
         return AEESMS_ERROR_SMS_DELIVERY_POSTPONED;

      case UASMS_DESTINATION_NO_LONGER_AT_THIS_ADDRESS_S:
         return AEESMS_ERROR_DESTINATION_NO_LONGER_AT_THIS_ADDRESS;

      case UASMS_OTHER_TERMINAL_PROBLEM_S:
         return AEESMS_ERROR_DESTINATION_PROBLEM;

      case UASMS_RADIO_IF_RESOURCE_SHORTAGE_S:
         return AEESMS_ERROR_RADIO_IF_RESOURCE_SHORTAGE;

      case UASMS_RADIO_IF_INCOMPATIBLE_S:
         return AEESMS_ERROR_RADIO_IF_INCOMPATIBLE;

      case UASMS_OTHER_RADIO_IF_PROBLEM_S:
         return AEESMS_ERROR_RADIO_PROBLEM;

      case UASMS_UNEXPECTED_PARM_SIZE_S:
         return AEESMS_ERROR_UNEXPECTED_PARM_SIZE;

      case UASMS_SUPPL_SERVICE_NOT_SUPPORTED:
         return AEESMS_ERROR_SUPPL_SERVICE_NOT_SUPPORTED;

      case UASMS_RESERVED_101_S:
         return AEESMS_ERROR_RESERVED;

      case UASMS_MISSING_EXPECTED_PARM_S:
         return AEESMS_ERROR_MISSING_EXPECTED_PARM;

      case UASMS_MISSING_MANDATORY_PARM_S:
         return AEESMS_ERROR_MISSING_MANDATORY_PARM;

      case UASMS_UNRECOGNIZED_PARM_VALUE_S:
         return AEESMS_ERROR_UNRECOGNIZED_PARM_VALUE;

      case UASMS_UNEXPECTED_PARM_VALUE_S:
         return AEESMS_ERROR_UNEXPECTED_PARM_VALUE;

      case UASMS_USER_DATA_SIZE_ERROR_S:
         return AEESMS_ERROR_USER_DATA_SIZE_ERROR;

      case UASMS_OTHER_GENERAL_PROBLEMS_S:
         return AEESMS_ERROR_GENERAL_PROBLEM;

      case UASMS_OUT_OF_RESOURCES_S:
         return AEESMS_ERROR_NO_MEMORY;

      case UASMS_ACCESS_TOO_LARGE_S:
         return AEESMS_ERROR_ACCESS_TOO_LARGE;

      case UASMS_DTC_TOO_LARGE_S:
         return AEESMS_ERROR_DTC_TOO_LARGE;

      case UASMS_NETWORK_NOT_READY_S:
         return AEESMS_ERROR_NETWORK_NOT_READY;

      case UASMS_PHONE_NOT_READY_S:
         return AEESMS_ERROR_PHONE_NOT_READY;

      case UASMS_NOT_ALLOWED_IN_AMPS_S:
         return AEESMS_ERROR_NOT_ALLOWED_IN_AMPS;

      case UASMS_INVALID_TRANSACTION_ID_S:
         return AEESMS_ERROR_INVALID_TRANSACTION_ID;

      default:
         if ((e >= UASMS_OTHER_NETWORK_PROBLEM_MORE_FIRST_S) &&
             (e <= UASMS_OTHER_NETWORK_PROBLEM_MORE_LAST_S))
         {
            return AEESMS_ERROR_NETWORK_PROBLEM;
         }
         else if ((e >= UASMS_OTHER_TERMINAL_PROBLEM_MORE_FIRST_S) &&
                  (e <= UASMS_OTHER_TERMINAL_PROBLEM_MORE_LAST_S))
         {
            return AEESMS_ERROR_DESTINATION_PROBLEM;
         }
         else if ((e >= UASMS_SMS_DELIVERY_POSTPONED_MORE_FIRST_S) &&
                  (e <= UASMS_SMS_DELIVERY_POSTPONED_MORE_LAST_S))
         {
            return AEESMS_ERROR_SMS_DELIVERY_POSTPONED;
         }
         else if ((e >= UASMS_OTHER_RADIO_IF_PROBLEM_MORE_FIRST_S) &&
                  (e <= UASMS_OTHER_RADIO_IF_PROBLEM_MORE_LAST_S))
         {
            return AEESMS_ERROR_RADIO_PROBLEM;
         }
         else
         {
            return AEESMS_ERROR_GENERAL_PROBLEM;
         }
   }
}

/*===========================================================================

  Translates uasms_error_class_e_type to MOSMS error type
===========================================================================*/
static int Xlate_uasms_error_class_e_type_2_MOSMS_ERRTYPE( uasms_error_class_e_type e )
{
   switch (e)
   {
      case UASMS_ERROR_NONE:
         return AEESMS_ERRORTYPE_NONE;

      case UASMS_ERROR_TEMP:
         return AEESMS_ERRORTYPE_TEMP;

      case UASMS_ERROR_PERM:
         return AEESMS_ERRORTYPE_PERM;

      default:
         return AEESMS_ERRORTYPE_UNKNOWN;
   }
}

/*===========================================================================

  Deletes OEMMsg instance

===========================================================================*/
void OEMSMS_DeleteMessage( OEMMsg* hMsg )
{
   if (gpUasms && OEMUASMS_IsValidNode(hMsg)) 
   {
      if (hMsg->pPrev)
         hMsg->pPrev->pNext = hMsg->pNext;
      else
         gpUasms->pCurMOSMS = hMsg->pNext;
      
      if (hMsg->pNext)
         hMsg->pNext->pPrev = hMsg->pPrev;

      if (!gpUasms->pCurMOSMS)
      {
         if (gpUasms->bDedicatedChannel)
         {
            uasms_DTC_disconnect();
            gpUasms->bDedicatedChannel = FALSE;
         }
      }
   }

   if ( hMsg )
      sys_free( hMsg );
}

/*===========================================================================

   Deregisters Message, Event and Status listeners

===========================================================================*/
int OEMSMS_Cleanup( void )
{
   if( gpUasms ) 
   {
#ifdef FEATURE_NOT_BREW_ONLY_SMS
      uixuasms_config_listeners( NULL, NULL, NULL );
#else
      uasms_config_listeners( NULL, NULL, NULL );
#endif // FEATURE_NOT_BREW_ONLY_SMS

      sys_free( gpUasms );

      gpUasms = 0;
   }

   return SUCCESS;
}

/*===========================================================================

   Sets message ack status for specified message type

===========================================================================*/
int OEMSMS_SetMsgAckStatus( uint32 cs, uint32 mt)
{
   int nErr = SUCCESS;

   if (!gpUasms)
      return EFAILED;

   switch (mt)
   {
      case AEESMS_TYPE_TEXT:
         gpUasms->dwTextStatus = cs;
         break;

      case AEESMS_TYPE_VOICE_MAIL_NOTIFICATION:
         gpUasms->dwVMStatus = cs;
         break;

      case AEESMS_TYPE_PAGE:
         gpUasms->dwPageStatus = cs;
         break;

      default:
         nErr = EBADPARM;
         break;
   }
   return nErr;
}

/*===========================================================================

   Sends message on traffic channel

===========================================================================*/
static uasms_status_e_type OEMUASMS_SendSMSOnTrafficChannel(OEMMsg * hMsg)
{
   OEMPhone *pPhone = NULL;
   uasms_status_e_type nErr = UASMS_OUT_OF_RESOURCES_S;

   //Check if the Traffic Channel is Already up. If yes, proceed with sending the msg
   if(SUCCESS == OEMPhone_New(&pPhone))
   {
      if(OEMPhone_InConversation(pPhone)) 
      {
         FARF(SMS, ("SMS in call"));

         // if this succeeds, response falls in OEMUASMS_EventListener()
         if ((nErr = uasms_send_message((const void *)hMsg, &hMsg->CMsg)) != UASMS_OK_S)
         {
            FARF(SMS, ("Send SMS in call failed"));
         }
      }
      else 
      { 
         FARF(SMS, ("Raise Traffic Channel"));
         
         // Raise the SO6 traffic channel
         // Flag for sending over traffic channel
         gpUasms->bTransmissionPending = TRUE;
         
         // If this succeeds, response falls in OEMUASMS_EventListener()
         if ((nErr = uasms_DTC_connect_i(UASMS_SO_6)) != UASMS_OK_S)
         {
            FARF(SMS, ("DTC connect failed"));
            gpUasms->bTransmissionPending = FALSE;
         }
      }
      OEMPhone_Release(pPhone);
   }
   else
   {
      FARF(SMS, ("OEMPhone_New failed during MO SMS"));
   }
   return nErr;
}

/*===========================================================================

   Gets UDH data

===========================================================================*/
void OEMSMS_GetUDHData(SMSCacheEntry * pce, int * pnBytes, byte * pBuffer)
{
   if (*pnBytes)
   {
      MEMSET(pBuffer, 0, *pnBytes);
   }
   *pnBytes = 0;
}

/*===========================================================================

   Gets GW CB Parms

===========================================================================*/
void OEMSMS_GetGWCBParms(SMSCacheEntry * pce, OEMMsgData * pOEMMsgData)
{
   // No op for UASMS
}

/*===========================================================================
Called by ISMS to register callback to be invoked on a lower layer SMS event
===========================================================================*/
void OEMSMS_OnSMSUpdate(OEMSMSUpdateData * pEventData, AEECallback *pcb)
{
   // No op for UASMS
}

/*===========================================================================
Called by ISMS to register callback to be invoked on a lower layer SMS BCConfig
event
===========================================================================*/
void OEMSMSBCConfig_OnSMSBCConfigUpdate(OEMSMSUpdateData * pEventData, AEECallback *pcb)
{
   // No op for UASMS
}

#ifdef FEATURE_BREW_BROADCAST_SMS
/*===========================================================================
Function: OEMSMSBCConfig_CleanOpInfo
Description:
   Cleans up information about current broadcast operation from gpUasms
Return Value:
   None
===========================================================================*/
static void OEMSMSBCConfig_CleanOpInfo(void)
{
   if (gpUasms->sBcCfg.bSrvInfoAlloc)
      FREEIF(gpUasms->sBcCfg.pSrvInfo);

   MEMSET(&gpUasms->sBcCfg, 0, sizeof(gpUasms->sBcCfg));
}

/*===========================================================================

  Called to cancel pending BCConfig operation

===========================================================================*/
static void OEMSMSBCConfig_CancelOp( AEECallback *pcb )
{
   FARF(SMS, ("==>  OEMSMSBCConfig_CancelOp"));

   //Verify that it is me set as the cancel function.
   //Cross-task cancellations need this check.
   if(pcb->pfnCancel != OEMSMSBCConfig_CancelOp)
      return;
   
   OEMSMSBCConfig_CleanOpInfo();

   pcb->pCancelData = 0;
   pcb->pfnCancel   = 0;
}

/*===========================================================================
Function: OEMSMSBCConfig_GetBCConfig
Description:
   Returns carrier's broadcast configuration
Return Value:
   None
===========================================================================*/
void OEMSMSBCConfig_GetBCConfig(AEESMSMode mode, AEECallback *pcb, AEESMSBCConfig * pConfig, uint32 * pnErr)
{
   int nErr = EUNSUPPORTED;

   FARF(SMS, ("==>  OEMSMSBCConfig_GetBCConfig"));

   // Check for CDMA Mode
   if (mode == AEESMS_MODE_CDMA)
   {
      if (gpUasms->sBcCfg.bBroadcastInUse != TRUE)
      {
         uasms_status_e_type  st;

         // Submit the request to UASMS 
         st = uasms_bc_get_config(OEMUASMSBCConfig_CommandCallback);

         if (st == UASMS_OK_S)
         {
            CALLBACK_Cancel(pcb);
            pcb->pCancelData = NULL;
            pcb->pfnCancel   = OEMSMSBCConfig_CancelOp;

            // Fill up the Current BC Configuration
            gpUasms->sBcCfg.bBroadcastInUse = TRUE;                
            gpUasms->sBcCfg.eCurRequest     = OEMSMSBCConfig_GetBCConfigRequest;
            gpUasms->sBcCfg.nNumCommands    = 1;
            gpUasms->sBcCfg.dwMode         = mode;
            gpUasms->sBcCfg.pcb            = pcb;
            gpUasms->sBcCfg.pConfig        = pConfig;
            gpUasms->sBcCfg.pnErr          = pnErr;  
         
            // Return Success to client
            nErr = SUCCESS;
         }
         else
         {
            // Return Error Code to Client
            nErr = Xlate_uasms_status_2_AEEErrorCode(st);
         }
      }
      else
      {
         FARF(SMS, ("Request Rejected - Broadcast In Use"));
         nErr = EITEMBUSY;
      }
   }
   else
   {
      // Return Unsupported to Client 
      nErr = EUNSUPPORTED;
   }
        
   if (nErr != SUCCESS)
   {
      // Fill up the Data
      *pnErr = nErr;

      // Call the Kernel Callback
      AEE_SYS_RESUME(pcb);
   }
}

/*===========================================================================
Function: OEMSMSBCConfig_GetBCPref
Description:
   Returns user's broadcast preference
Return Value:
   None
===========================================================================*/
void OEMSMSBCConfig_GetBCPref(AEESMSMode mode, AEECallback *pcb, AEESMSBCPref * pPref, uint32 * pnErr)
{
   int nErr = EUNSUPPORTED;

   FARF(SMS, ("==>  OEMSMSBCConfig_GetBCPref"));

   // Check for CDMA Mode
   if (mode == AEESMS_MODE_CDMA)
   {
      if (gpUasms->sBcCfg.bBroadcastInUse != TRUE)
      {
         uasms_status_e_type  st;

         // Submit the request to UASMS
         st = uasms_bc_get_pref(OEMUASMSBCConfig_CommandCallback);

         if (st == UASMS_OK_S)
         {
            CALLBACK_Cancel(pcb);
            pcb->pCancelData = NULL;
            pcb->pfnCancel   = OEMSMSBCConfig_CancelOp;

            // Fill up the Current BC Configuration
            gpUasms->sBcCfg.bBroadcastInUse = TRUE;                
            gpUasms->sBcCfg.eCurRequest     = OEMSMSBCConfig_GetBCPrefRequest;
            gpUasms->sBcCfg.nNumCommands    = 1;
            gpUasms->sBcCfg.dwMode         = mode;
            gpUasms->sBcCfg.pcb            = pcb;
            gpUasms->sBcCfg.pPref          = pPref;
            gpUasms->sBcCfg.pnErr          = pnErr;  

            // Return Success to client
            nErr = SUCCESS;
         }
         else
         {
            // Return Error Code to Client
            nErr = Xlate_uasms_status_2_AEEErrorCode(st);
         }
      }
      else
      {
         FARF(SMS, ("Request Rejected - Broadcast In Use"));
         nErr = EITEMBUSY;
      }
   }
   else
   {
      // Return Unsupported to Client
      nErr = EUNSUPPORTED;
   }
   
   if (nErr != SUCCESS)
   {
      // Fill up the Data
      *pnErr = nErr;

      // Call the Kernel Callback
      AEE_SYS_RESUME(pcb);
   }
}

/*===========================================================================
Function: OEMSMSBCConfig_SetBCPref
Description:
   Sets user's broadcast preference
Return Value:
   None
===========================================================================*/
void OEMSMSBCConfig_SetBCPref(AEESMSMode mode, AEESMSBCPref nPref, AEECallback *pcb, uint32 * pnErr)
{
   int nErr = EUNSUPPORTED;

   FARF(SMS, ("==>  OEMSMSBCConfig_SetBCPref"));

   // Check for CDMA Mode
   if (mode == AEESMS_MODE_CDMA)
   {
      if (gpUasms->sBcCfg.bBroadcastInUse != TRUE)
      {
         uasms_status_e_type  st;

         // Submit the request to UASMS
         st = uasms_bc_set_pref(OEMUASMSBCConfig_CommandCallback, nPref);

         if (st == UASMS_OK_S)
         {
            CALLBACK_Cancel(pcb);
            pcb->pCancelData = NULL;
            pcb->pfnCancel   = OEMSMSBCConfig_CancelOp;

            // Fill up the Current BC Configuration
            gpUasms->sBcCfg.bBroadcastInUse = TRUE;                
            gpUasms->sBcCfg.eCurRequest     = OEMSMSBCConfig_SetBCPrefRequest;
            gpUasms->sBcCfg.nNumCommands    = 1;
            gpUasms->sBcCfg.dwMode         = mode;
            gpUasms->sBcCfg.pcb            = pcb;
            gpUasms->sBcCfg.pnErr          = pnErr;  
               
            // Return Success to client
            nErr = SUCCESS;
         }
         else
         {
            // Return Error Code to Client
            nErr = Xlate_uasms_status_2_AEEErrorCode(st);
         }
      }
      else
      {
         FARF(SMS, ("Request Rejected - Broadcast In Use"));
         nErr = EITEMBUSY;
      }
   }
   else
   {
      // Return Unsupported to Client
      nErr = EUNSUPPORTED;
   }

   if (nErr != SUCCESS)
   {
      // Fill up the Data
      *pnErr = nErr;

      // Call the Kernel Callback
      AEE_SYS_RESUME(pcb);
   }
}
/*===========================================================================
Function: OEMSMSBCConfig_GetNumberOfServiceIDs
Description:
   Returns service IDs currently configured in the table.
Return Value:
   None
===========================================================================*/
void OEMSMSBCConfig_GetNumberOfServiceIDs(AEESMSMode mode, AEECallback *pcb, uint32 * pnCount, uint32 * pnErr)
{
   int nErr = EUNSUPPORTED;

   FARF(SMS, ("==>  OEMSMSBCConfig_GetNumberOfServiceIDs"));

   // Check for CDMA Mode */
   if (mode == AEESMS_MODE_CDMA)
   {
      if (gpUasms->sBcCfg.bBroadcastInUse != TRUE)
      {
         uasms_status_e_type  st;

         // Submit the request to UASMS
         st = uasms_bc_get_all_service_ids(OEMUASMSBCConfig_CommandCallback);

         if (st == UASMS_OK_S)
         {
            CALLBACK_Cancel(pcb);
            pcb->pCancelData = NULL;
            pcb->pfnCancel   = OEMSMSBCConfig_CancelOp;

            // Fill up the Current BC Configuration
            gpUasms->sBcCfg.bBroadcastInUse = TRUE;                
            gpUasms->sBcCfg.eCurRequest     = OEMSMSBCConfig_GetNumberOfServiceIDsRequest;
            gpUasms->sBcCfg.nNumCommands    = 1;
            gpUasms->sBcCfg.dwMode         = mode;
            gpUasms->sBcCfg.pcb            = pcb;
            gpUasms->sBcCfg.pnCount        = pnCount;
            gpUasms->sBcCfg.pnErr          = pnErr;  
            
            // Return Success to client
            nErr = SUCCESS;
         }
         else
         {
            // Return Error Code to Client
            nErr = Xlate_uasms_status_2_AEEErrorCode(st);
         }
      }
      else
      {
         FARF(SMS, ("Request Rejected - Broadcast In Use"));
         nErr = EITEMBUSY;
      }
   }
   else
   {
      // Return Unsupported to Client
      nErr = EUNSUPPORTED;
   }

   if (nErr != SUCCESS)
   {
      // Fill up the Data
      *pnErr = nErr;

      // Call the Kernel Callback
      AEE_SYS_RESUME(pcb);
   }
}

/*===========================================================================
Function: OEMSMSBCConfig_GetServiceOpts
Description:
   Returns service options corresponding to the specified ID.
Return Value:
   None
===========================================================================*/
void OEMSMSBCConfig_GetServiceOpts(AEESMSMode mode, AEESMSBCSrvID *pSrvID, AEECallback *pcb, OEMSMSBCSrvInfo * pSrvInfo, uint32 * pnErr)
{
   int nErr = EUNSUPPORTED;

   FARF(SMS, ("==>  OEMSMSBCConfig_GetServiceOpts"));

   // Check for CDMA Mode
   if (mode == AEESMS_MODE_CDMA)
   {
      if (gpUasms->sBcCfg.bBroadcastInUse != TRUE)
      {
         uasms_status_e_type  st;
         uasms_bc_service_id_type  srv_id;

         MEMSET(&srv_id, 0, sizeof(uasms_bc_service_id_type));
   
         if (ConvertAEESMSBCSrvIDToUASMSBCSrvID(pSrvID, &srv_id))
         {
            // Submit the request to UASMS
            st = uasms_bc_get_service_info(OEMUASMSBCConfig_CommandCallback, &srv_id);

            if (st == UASMS_OK_S)
            {
               CALLBACK_Cancel(pcb);
               pcb->pCancelData = NULL;
               pcb->pfnCancel   = OEMSMSBCConfig_CancelOp;

               // Fill up the Current BC Configuration
               gpUasms->sBcCfg.bBroadcastInUse = TRUE;                
               gpUasms->sBcCfg.eCurRequest     = OEMSMSBCConfig_GetServiceOptsRequest;
               gpUasms->sBcCfg.nNumCommands    = 1;
               gpUasms->sBcCfg.dwMode         = mode;
               gpUasms->sBcCfg.psid           = pSrvID;
               gpUasms->sBcCfg.pcb            = pcb;
               gpUasms->sBcCfg.pSrvInfo       = pSrvInfo;
               gpUasms->sBcCfg.pnErr          = pnErr;  
                  
               // Return Success to client
               nErr = SUCCESS;
            }
            else
            {
               // Return Error Code to Client
               nErr = Xlate_uasms_status_2_AEEErrorCode(st);
            }
         }
         else
         {
            // Return Bad Parameter to Client
            nErr = EBADPARM;
         }
      }
      else
      {
         FARF(SMS, ("Request Rejected - Broadcast In Use"));
         nErr = EITEMBUSY;
      }
   }
   else
   {
      // Return Unsupported to Client
      nErr = EUNSUPPORTED;
   }
   
   if (nErr != SUCCESS)
   {
      // Fill up the Data
      *pnErr = nErr;

      // Call the Kernel Callback
      AEE_SYS_RESUME(pcb);
   }
}

/*===========================================================================
Function: OEMSMSBCConfig_SetServiceOpts
Description:
   Sets service options corresponding to the specified ID.
Return Value:
   None
===========================================================================*/
void OEMSMSBCConfig_SetServiceOpts(AEESMSMode mode, OEMSMSBCSrvInfo *pSrvInfo, AEECallback *pcb, uint32 * pnErr)
{
   int nErr = EUNSUPPORTED;

   FARF(SMS, ("==>  OEMSMSBCConfig_SetServiceOpts"));

   // Check for CDMA Mode
   if (mode == AEESMS_MODE_CDMA)
   {
      if (gpUasms->sBcCfg.bBroadcastInUse != TRUE)
      {
         uasms_status_e_type        st;
         uasms_bc_service_info_type srv_info;

         MEMSET(&srv_info, 0, sizeof(uasms_bc_service_info_type));

         // Convert the Structure
         if (ConvertOEMSMSBCSrvInfoToUASMSBCSrvInfo(pSrvInfo, &srv_info) == TRUE)
         {
            // Submit the 1st request to UASMS
            st = uasms_bc_select_service(OEMUASMSBCConfig_CommandCallback, &srv_info.srv_id, srv_info.selected, srv_info.priority);
            
            if (st == UASMS_OK_S)
            {
               // Submit the 2nd request to UASMS
               st = uasms_bc_change_label(OEMUASMSBCConfig_CommandCallback, &srv_info.srv_id, srv_info.label);

               if (st == UASMS_OK_S)
               {
                  if ((gpUasms->sBcCfg.pSrvInfo = MALLOC(sizeof(OEMSMSBCSrvInfo))) != NULL)
                  {
                     CALLBACK_Cancel(pcb);
                     pcb->pCancelData = NULL;
                     pcb->pfnCancel   = OEMSMSBCConfig_CancelOp;

                     gpUasms->sBcCfg.bSrvInfoAlloc = TRUE;

                     MEMCPY(gpUasms->sBcCfg.pSrvInfo, pSrvInfo, sizeof(OEMSMSBCSrvInfo));

                     // Fill up the Current BC Configuration
                     gpUasms->sBcCfg.bBroadcastInUse = TRUE;                
                     gpUasms->sBcCfg.eCurRequest     = OEMSMSBCConfig_SetServiceOptsRequest;
                     gpUasms->sBcCfg.nNumCommands    = 2;
                     gpUasms->sBcCfg.dwMode         = mode;
                     gpUasms->sBcCfg.pcb            = pcb;
                     gpUasms->sBcCfg.pnErr          = pnErr;               

                     // Return Success to client
                     nErr = SUCCESS;
                  }
                  else
                  {
                     nErr = ENOMEMORY;
                  }
               }
               else
               {
                  // Return Error Code to Client
                  nErr = Xlate_uasms_status_2_AEEErrorCode(st);
               }
            }
            else
            {
               // Return Error Code to Client
               nErr = Xlate_uasms_status_2_AEEErrorCode(st);
            }
         }
         else
         {
            // Return Bad Parameter to Client
            nErr = EBADPARM;
         }
      }
      else
      {
         FARF(SMS, ("Request Rejected - Broadcast In Use"));
         nErr = EITEMBUSY;
      }
   }      
   else
   {
      // Return Unsupported to Client
      nErr = EUNSUPPORTED;
   }

   if (nErr != SUCCESS)
   {
      // Fill up the Data
      *pnErr = nErr;

      // Call the Kernel Callback
      AEE_SYS_RESUME(pcb);
   }
}

/*===========================================================================
Function: OEMSMSBCConfig_GetNumberOfServiceOpts
Description:
   Returns all the service options corresponding to the specified mode
Return Value:
   None
===========================================================================*/
void OEMSMSBCConfig_GetNumberOfServiceOpts(AEESMSMode mode, AEECallback *pcb, uint32 *pnCount, uint32 * pnErr)
{
   int nErr = EUNSUPPORTED;

   FARF(SMS, ("==>  OEMSMSBCConfig_GetNumberOfServiceOpts"));

   // Check for CDMA Mode
   if (mode == AEESMS_MODE_CDMA)
   {
      if (gpUasms->sBcCfg.bBroadcastInUse != TRUE)
      {
         uasms_status_e_type  st;
      
         // Submit the request to UASMS
         st = uasms_bc_get_table(OEMUASMSBCConfig_CommandCallback);

         if (st == UASMS_OK_S)
         {
            CALLBACK_Cancel(pcb);
            pcb->pCancelData = NULL;
            pcb->pfnCancel   = OEMSMSBCConfig_CancelOp;

            // Fill up the Current BC Configuration
            gpUasms->sBcCfg.bBroadcastInUse = TRUE;                
            gpUasms->sBcCfg.eCurRequest     = OEMSMSBCConfig_GetNumberOfServiceOptsRequest;
            gpUasms->sBcCfg.nNumCommands    = 1;
            gpUasms->sBcCfg.dwMode         = mode;
            gpUasms->sBcCfg.pcb            = pcb;
            gpUasms->sBcCfg.pnCount        = pnCount;
            gpUasms->sBcCfg.pnErr          = pnErr;               

            // Return Success to client
            nErr = SUCCESS;
         }
         else
         {
            // Return Error Code to Client
            nErr = Xlate_uasms_status_2_AEEErrorCode(st);
         }
      }
      else
      {
         FARF(SMS, ("Request Rejected - Broadcast In Use"));
         nErr = EITEMBUSY;
      }
   }
   else
   {
      // Return Unsupported to Client
      nErr = EUNSUPPORTED;
   }
   
   if (nErr != SUCCESS)
   {
      // Fill up the Data
      *pnErr = nErr;

      // Call the Kernel Callback
      AEE_SYS_RESUME(pcb);
   }
}

/*===========================================================================
Function: OEMSMSBCConfig_DeleteService
Description:
   Removes a service from the device service table.
Return Value:
   None
===========================================================================*/
void OEMSMSBCConfig_DeleteService(AEESMSMode mode, AEESMSBCSrvID *pSrvID, AEECallback *pcb, uint32 * pnErr)
{
  int nErr = EUNSUPPORTED;

  FARF(SMS, ("==>  OEMSMSBCConfig_DeleteService"));

   // Check for CDMA Mode
   if (mode == AEESMS_MODE_CDMA)
   {
      if (gpUasms->sBcCfg.bBroadcastInUse != TRUE)
      {
         uasms_status_e_type  st;
         uasms_bc_service_id_type  srv_id;

         MEMSET(&srv_id, 0, sizeof(uasms_bc_service_id_type));

         if (ConvertAEESMSBCSrvIDToUASMSBCSrvID(pSrvID, &srv_id))
         {
            // Submit the request to UASMS
            st = uasms_bc_delete_service(OEMUASMSBCConfig_CommandCallback, &srv_id);

            if (st == UASMS_OK_S)
            {
               CALLBACK_Cancel(pcb);
               pcb->pCancelData = NULL;
               pcb->pfnCancel   = OEMSMSBCConfig_CancelOp;

               // Fill up the Current BC Configuration
               gpUasms->sBcCfg.bBroadcastInUse = TRUE;                
               gpUasms->sBcCfg.eCurRequest     = OEMSMSBCConfig_DeleteServiceRequest;
               gpUasms->sBcCfg.nNumCommands    = 1;
               gpUasms->sBcCfg.dwMode         = mode;
               gpUasms->sBcCfg.pcb            = pcb;
               gpUasms->sBcCfg.pnErr          = pnErr;

               // Return Success to client
               nErr = SUCCESS;
            }
            else
            {
               // Return Error Code to Client
               nErr = Xlate_uasms_status_2_AEEErrorCode(st);
            }
         }
         else
         {
            // Return Unsupported to Client
            nErr = EBADPARM;
         }
      }
      else
      {
         FARF(SMS, ("Request Rejected - Broadcast In Use"));
         nErr = EITEMBUSY;
      }
   }
   else
   {
      // Return Unsupported to Client
      nErr = EUNSUPPORTED;
   }

   if (nErr != SUCCESS)
   {
      // Fill up the Data
      *pnErr = nErr;

      // Call the Kernel Callback
      AEE_SYS_RESUME(pcb);
   }
}

/*===========================================================================
Function: OEMSMSBCConfig_FillServiceIDs
Description:
   Fills the Service IDs
Return Value:
   None
===========================================================================*/
void OEMSMSBCConfig_FillServiceIDs( AEESMSBCSrvID *psid, uint32 dwBytes )
{
   FARF(SMS, ("==>  OEMSMSBCConfig_FillServiceIDs"));

   if ((psid != NULL) && 
       (gpUasms->nCount) &&
       (gpUasms->psid != NULL) &&
       (dwBytes >= gpUasms->nCount*sizeof(AEESMSBCSrvID)))
   {
      MEMCPY(psid, gpUasms->psid, gpUasms->nCount*sizeof(AEESMSBCSrvID));
   }

   FREEIF(gpUasms->psid);
   gpUasms->psid = 0;
   gpUasms->nCount = 0;
}

/*===========================================================================
Function: OEMSMSBCConfig_FillServiceInfo
Description:
   Fills the Service Info
Return Value:
   None
===========================================================================*/
void OEMSMSBCConfig_FillServiceInfo( OEMSMSBCSrvInfo *pSrvInfo, uint32 dwBytes )
{
   FARF(SMS, ("==>  OEMSMSBCConfig_FillServiceIDs"));

   if ((pSrvInfo != NULL) &&
       (gpUasms->nCount) &&
       (gpUasms->pSrvInfo != NULL) &&
       (dwBytes >= gpUasms->nCount*sizeof(OEMSMSBCSrvInfo)))
   {
      MEMCPY(pSrvInfo, gpUasms->pSrvInfo, gpUasms->nCount*sizeof(OEMSMSBCSrvInfo));
   }

   FREEIF(gpUasms->pSrvInfo);
   gpUasms->pSrvInfo = 0;
   gpUasms->nCount = 0;
}

/*===========================================================================
Function: Xlate_uasms_status_2_AEEErrorCode
Description:
   Traslates the UASMS Status Codes to AEE Error Codes
Return Value:
   AEE Error Code
===========================================================================*/
static int Xlate_uasms_status_2_AEEErrorCode(uasms_status_e_type st)
{
   int nRet = EFAILED;

   FARF(SMS, ("==>  Xlate_uasms_status_2_AEEErrorCode"));

   switch(st)
   {
      case UASMS_OK_S:
         nRet = SUCCESS;
         break;

      // Add more codes here to be more specific

      default:
         nRet = EFAILED;
         break;
   }
   return (nRet);
}

/*===========================================================================
Function: Xlate_uasms_cmd_status_2_AEEErrorCode
Description:
   Translates the UASMS Command Status Codes to AEE Error Codes
Return Value:
   AEE Error Code
===========================================================================*/
static int Xlate_uasms_cmd_status_2_AEEErrorCode(uasms_cmd_status_e_type st)
{
   int nRet = EFAILED;

   FARF(SMS, ("==>  Xlate_uasms_cmd_status_2_AEEErrorCode"));

   switch(st)
   {
      case UASMS_CMD_ERR_NONE:
         nRet = SUCCESS;
         break;

      // Add more codes here to be more specific

      default:
         nRet = EFAILED;
         break;
   }
   return (nRet);
}

/*===========================================================================
Function: OEMUASMSBCConfig_CommandCallback
Description:
   UASMS Broadcast Command Callback function
Return Value:
   None
===========================================================================*/
static void OEMUASMSBCConfig_CommandCallback(uasms_cmd_id_e_type cmd, uasms_cmd_status_e_type status)
{
   int nErr = SUCCESS;

   FARF(SMS, ("==>  OEMUASMSBCConfig_CommandCallback"));

   if (status == UASMS_CMD_ERR_NONE)
   {
      // Ignore it; An event will be arriving shortly
      return;
   }
   
   // Special Handling for Add Service
   if ((cmd == UASMS_CMD_BC_SELECT_SRV) && 
       (status == UASMS_CMD_ERR_BC_BAD_SRV_ID))
   {
      uasms_status_e_type  st;
      uasms_bc_service_info_type srv_info;

      MEMSET(&srv_info, 0, sizeof(uasms_bc_service_info_type));

      // Convert the Structure
      if (ConvertOEMSMSBCSrvInfoToUASMSBCSrvInfo(gpUasms->sBcCfg.pSrvInfo, &srv_info) == TRUE)
      {
         // Service not Found ; Hence Add it
         st = uasms_bc_add_service(OEMUASMSBCConfig_CommandCallback, &srv_info);
              
         if (st == UASMS_OK_S)
         {
            gpUasms->sBcCfg.nNumCommands = 1;
         }
         else
         {
            nErr = Xlate_uasms_cmd_status_2_AEEErrorCode(status);
         }
      }
      else
      {
         nErr = EBADPARM;
      }
   }
   else
   {
      nErr = Xlate_uasms_cmd_status_2_AEEErrorCode(status);
   }

   if (nErr != SUCCESS)
   {
      // Fill up the Data
      if (gpUasms->sBcCfg.pnErr)
         *gpUasms->sBcCfg.pnErr = nErr;

      // Call the Kernel Callback
      if (gpUasms->sBcCfg.pcb)
         AEE_SYS_RESUME(gpUasms->sBcCfg.pcb);
   }
}

/*===========================================================================
Function: OEMUASMSBCConfig_EventCallback
Description:
   UASMS Broadcast Event Callback function
Return Value:
   None
===========================================================================*/
static void OEMUASMSBCConfig_EventCallback( uasms_event_e_type event, uasms_event_info_type * info_ptr)
{
   boolean bEventHandled = FALSE;
   int i = 0;

   FARF(SMS, ("==>  OEMUASMSBCConfig_EventCallback"));
      
   if ((event == UASMS_EVENT_BC_CONFIG) &&
       (gpUasms->sBcCfg.eCurRequest == OEMSMSBCConfig_GetBCConfigRequest))
   {
      *gpUasms->sBcCfg.pnErr = SUCCESS;
      *gpUasms->sBcCfg.pConfig = info_ptr->bc_config;
      bEventHandled = TRUE;
   }
   else if ((event == UASMS_EVENT_BC_PREF) &&
            (gpUasms->sBcCfg.eCurRequest == OEMSMSBCConfig_GetBCPrefRequest))
   {
      *gpUasms->sBcCfg.pnErr = SUCCESS;
      *gpUasms->sBcCfg.pPref = info_ptr->bc_pref;
      bEventHandled = TRUE;
   }
   else if((event == UASMS_EVENT_BC_PREF) &&
           (gpUasms->sBcCfg.eCurRequest == OEMSMSBCConfig_SetBCPrefRequest))
   {
      *gpUasms->sBcCfg.pnErr = SUCCESS;
      bEventHandled = TRUE;
   }
   else if ((event == UASMS_EVENT_BC_SRV_IDS) &&
            (gpUasms->sBcCfg.eCurRequest == OEMSMSBCConfig_GetNumberOfServiceIDsRequest))
   {
      if ((gpUasms->psid = MALLOC(sizeof(AEESMSBCSrvID)*info_ptr->bc_srv_ids.size)) != NULL)
      {
         gpUasms->nCount = info_ptr->bc_srv_ids.size;
         *gpUasms->sBcCfg.pnErr = SUCCESS;
         for (i = 0; i < gpUasms->nCount; i++)
         {
            (void)ConvertUASMSBCSrvIDToAEESMSBCSrvID(&info_ptr->bc_srv_ids.entries[i], &gpUasms->psid[i]);
         }
      }
      else
      {
         *gpUasms->sBcCfg.pnErr = ENOMEMORY;
      }
      bEventHandled = TRUE;
   }
   else if ((event == UASMS_EVENT_BC_SRV_INFO) &&
            (gpUasms->sBcCfg.eCurRequest == OEMSMSBCConfig_GetServiceOptsRequest))
   {
      *gpUasms->sBcCfg.pnErr = SUCCESS;
      (void)ConvertUASMSBCSrvInfoToOEMSMSBCSrvInfo(&info_ptr->bc_srv_info, gpUasms->sBcCfg.pSrvInfo);
      bEventHandled = TRUE;
   }
   else if (((event == UASMS_EVENT_BC_SRV_ADDED) || (event == UASMS_EVENT_BC_SRV_UPDATED)) &&
            (gpUasms->sBcCfg.eCurRequest == OEMSMSBCConfig_SetServiceOptsRequest))
   {
      *gpUasms->sBcCfg.pnErr = SUCCESS;
      bEventHandled = TRUE;
   }
   else if ((event == UASMS_EVENT_BC_TABLE) &&
            (gpUasms->sBcCfg.eCurRequest == OEMSMSBCConfig_GetNumberOfServiceOptsRequest))
   {
      if ((gpUasms->pSrvInfo = MALLOC(sizeof(OEMSMSBCSrvInfo)*info_ptr->bc_table.size)) != NULL)
      {
         gpUasms->nCount = info_ptr->bc_table.size;
         *gpUasms->sBcCfg.pnErr = SUCCESS;
         for (i = 0; i < gpUasms->nCount; i++)
         {
            (void)ConvertUASMSBCSrvInfoToOEMSMSBCSrvInfo(&info_ptr->bc_table.entries[i], &gpUasms->pSrvInfo[i]);
         }
      }
      else
      {
         *gpUasms->sBcCfg.pnErr = ENOMEMORY;
      }
      bEventHandled = TRUE;
   }
   else if ((event == UASMS_EVENT_BC_SRV_DELETED) &&
            (gpUasms->sBcCfg.eCurRequest == OEMSMSBCConfig_DeleteServiceRequest))
   {
      *gpUasms->sBcCfg.pnErr = SUCCESS;
      bEventHandled = TRUE;
   }
   else
   {
      // Not Handled Event
      bEventHandled = FALSE;
   }
   
   if (bEventHandled)
   {
      if (gpUasms->sBcCfg.nNumCommands == 1)
      {
         // Call the Kernel Callback
         AEE_SYS_RESUME(gpUasms->sBcCfg.pcb);
      }
      else
      {
         // Decrement Number of Commands 
         gpUasms->sBcCfg.nNumCommands--;
      }
   }
}

/*===========================================================================
Function: ConvertAEESMSBCSrvIDToUASMSBCSrvID
Description:
   Convert AEESMS Service ID to UASMS Service ID
Return Value:
  Indicates successful/ unsuccessful conversion
===========================================================================*/
static boolean ConvertAEESMSBCSrvIDToUASMSBCSrvID(AEESMSBCSrvID *pSrvID, uasms_bc_service_id_type *service_id)
{
   boolean bRetVal = TRUE;

   FARF(SMS, ("==>  ConvertAEESMSBCSrvIDToUASMSBCSrvID"));

   if ( pSrvID == NULL || service_id == NULL)
   {
      bRetVal = FALSE;
   }
   else
   {
      service_id->service  = (uasms_service_e_type)pSrvID->srvType;
      service_id->language = (uasms_language_e_type)pSrvID->dwLang;
   }
   return bRetVal;
}

/*===========================================================================
Function: ConvertUASMSBCSrvIDToAEESMSBCSrvID
Description:
   Convert UASMS Service ID to AEESMS Service ID
Return Value:
  Indicates successful/ unsuccessful conversion
===========================================================================*/
static boolean ConvertUASMSBCSrvIDToAEESMSBCSrvID(uasms_bc_service_id_type *service_id, AEESMSBCSrvID *pSrvID)
{
   boolean bRetVal = TRUE;

   FARF(SMS, ("==>  ConvertUASMSBCSrvIDToAEESMSBCSrvID"));
  
   if ( pSrvID == NULL || service_id == NULL)
   {
      bRetVal = FALSE;
   }
   else
   {
      pSrvID->srvType = (AEESMSBCType)service_id->service;
      pSrvID->dwLang  = (uint32)service_id->language;
   }
   return bRetVal;
}

/*===========================================================================
Function: ConvertOEMSMSBCSrvInfoToUASMSBCSrvInfo
Description:
   Convert OEMSMS Service Info to UASMS Service Info
Return Value:
  Indicates successful/ unsuccessful conversion
===========================================================================*/
static boolean ConvertOEMSMSBCSrvInfoToUASMSBCSrvInfo(OEMSMSBCSrvInfo *pSrvInfo, uasms_bc_service_info_type *srv_info)
{
   boolean bRetVal = TRUE;

   FARF(SMS, ("==>  ConvertOEMSMSBCSrvInfoToUASMSBCSrvInfo"));

   if ( pSrvInfo == NULL || srv_info == NULL)
   {
      bRetVal = FALSE;
   }
   else
   {
      // Service ID 
      if (ConvertAEESMSBCSrvIDToUASMSBCSrvID(&pSrvInfo->sSrvID, &srv_info->srv_id) == FALSE)
      {
         bRetVal = FALSE;
      }

      // Selected
      srv_info->selected = pSrvInfo->bSelected;

      // Priority
      srv_info->priority = pSrvInfo->nPriority;

      // Label 
      MEMCPY(srv_info->label, pSrvInfo->szLabel, UASMS_BC_SRV_LABEL_SIZE);
   }
   return bRetVal;
}

/*===========================================================================
Function: ConvertUASMSBCSrvInfoToOEMSMSBCSrvInfo
Description:
   Convert UASMS Service Info to OEMSMS Service Info
Return Value:
  Indicates successful/ unsuccessful conversion
===========================================================================*/
static boolean ConvertUASMSBCSrvInfoToOEMSMSBCSrvInfo( uasms_bc_service_info_type *srv_info, OEMSMSBCSrvInfo *pSrvInfo)
{
   boolean bRetVal = TRUE;

   FARF(SMS, ("==>  ConvertUASMSBCSrvInfoToOEMSMSBCSrvInfo"));

   if ( pSrvInfo == NULL || srv_info == NULL)
   {
      bRetVal = FALSE;
   }
   else
   {
      // Service ID
      if (ConvertUASMSBCSrvIDToAEESMSBCSrvID(&srv_info->srv_id, &pSrvInfo->sSrvID) == FALSE)
      {
         bRetVal = FALSE;
      }

      // Selected 
      pSrvInfo->bSelected = srv_info->selected;

      // Priority
      pSrvInfo->nPriority = srv_info->priority;

      // Label
      MEMSET(pSrvInfo->szLabel, 0, sizeof(pSrvInfo->szLabel));
      MEMCPY(pSrvInfo->szLabel, srv_info->label, UASMS_BC_SRV_LABEL_SIZE);

      // Default Parameters since UASMS does not have these params
      pSrvInfo->nLabelEncoding = AEESMS_ENC_OCTET;
      pSrvInfo->nAlert          = AEESMS_BC_ALERT_DEFAULT;
      pSrvInfo->nMaxMessages   = 10;
   }
   return bRetVal;
}

#else // FEATURE_BREW_BROADCAST_SMS
/*===========================================================================
   Stub
===========================================================================*/
void OEMSMSBCConfig_GetBCConfig(AEESMSMode mode, AEECallback *pcb, AEESMSBCConfig * pConfig, uint32 * pnErr)
{   
   FARF(SMS, ("==>  OEMSMSBCConfig_GetBCConfig - Not Supported"));
   *pnErr = EUNSUPPORTED;
   AEE_SYS_RESUME(pcb);
}

/*===========================================================================
   Stub
===========================================================================*/
void OEMSMSBCConfig_GetBCPref(AEESMSMode mode, AEECallback *pcb, AEESMSBCPref * pPref, uint32 * pnErr)
{   
   FARF(SMS, ("==>  OEMSMSBCConfig_GetBCPref - Not Supported"));
   *pnErr = EUNSUPPORTED;
   AEE_SYS_RESUME(pcb);
}

/*===========================================================================
   Stub
===========================================================================*/
void OEMSMSBCConfig_SetBCPref(AEESMSMode mode, AEESMSBCPref nPref, AEECallback *pcb, uint32 * pnErr)
{   
   FARF(SMS, ("==>  OEMSMSBCConfig_SetBCPref - Not Supported"));
   *pnErr = EUNSUPPORTED;
   AEE_SYS_RESUME(pcb);
}

/*===========================================================================
   Stub
===========================================================================*/
void OEMSMSBCConfig_GetNumberOfServiceIDs(AEESMSMode mode, AEECallback *pcb, uint32 * pnCount, uint32 * pnErr)
{   
   FARF(SMS, ("==>  OEMSMSBCConfig_GetNumberOfServiceIDs - Not Supported"));
   *pnErr = EUNSUPPORTED;
   AEE_SYS_RESUME(pcb);
}

/*===========================================================================
   Stub
===========================================================================*/
void OEMSMSBCConfig_GetServiceOpts(AEESMSMode mode, AEESMSBCSrvID *pSrvID, AEECallback *pcb, OEMSMSBCSrvInfo * pSrvInfo, uint32 * pnErr)
{   
   FARF(SMS, ("==>  OEMSMSBCConfig_GetServiceOpts - Not Supported"));
   *pnErr = EUNSUPPORTED;
   AEE_SYS_RESUME(pcb);
}

/*===========================================================================
   Stub
===========================================================================*/
void OEMSMSBCConfig_SetServiceOpts(AEESMSMode mode, OEMSMSBCSrvInfo * pSrvInfo, AEECallback *pcb, uint32 * pnErr)
{   
   FARF(SMS, ("==>  OEMSMSBCConfig_SetServiceOpts - Not Supported"));
   *pnErr = EUNSUPPORTED;
   AEE_SYS_RESUME(pcb);
}

/*===========================================================================
   Stub
===========================================================================*/
void OEMSMSBCConfig_GetNumberOfServiceOpts(AEESMSMode mode, AEECallback *pcb, uint32 * pnCount, uint32 * pnErr)
{   
   FARF(SMS, ("==>  OEMSMSBCConfig_GetNumberOfServiceOpts - Not Supported"));
   *pnErr = EUNSUPPORTED;
   AEE_SYS_RESUME(pcb);
}

/*===========================================================================
   Stub
===========================================================================*/
void OEMSMSBCConfig_DeleteService(AEESMSMode mode, AEESMSBCSrvID *pSrvID, AEECallback *pcb, uint32 * pnErr)
{   
   FARF(SMS, ("==>  OEMSMSBCConfig_DeleteService - Not Supported"));
   *pnErr = EUNSUPPORTED;
   AEE_SYS_RESUME(pcb);
}

/*===========================================================================
   Stub
===========================================================================*/
void OEMSMSBCConfig_FillServiceIDs(AEESMSBCSrvID *psid, uint32 dwBytes)
{   
   FARF(SMS, ("==>  OEMSMSBCConfig_FillServiceIDs - Not Supported"));
}

/*===========================================================================
   Stub
===========================================================================*/
void OEMSMSBCConfig_FillServiceInfo(OEMSMSBCSrvInfo *pSrvInfo, uint32 dwBytes)
{   
   FARF(SMS, ("==>  OEMSMSBCConfig_FillServiceInfo - Not Supported"));
}
#endif // FEATURE_BREW_BROADCAST_SMS

/*===========================================================================

   OEMSMS_GetDomainPref

===========================================================================*/
int OEMSMS_GetDomainPref(int * pgwDomainPref)
{
   FARF(SMS, ("OEMSMS_GetDomainPref"));
   return EUNSUPPORTED;
}

/*===========================================================================

   OEMSMS_SetDomainPref

===========================================================================*/
int OEMSMS_SetDomainPref(int gwDomainPref)
{
   FARF(SMS, ("OEMSMS_SetDomainPref"));
   return EUNSUPPORTED;
}

#endif /*FEATURE_UASMS*/
